/* ***** BEGIN LICENSE BLOCK *****
 *   Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 *   The contents of this file are subject to the Mozilla Public License Version
 *   1.1 (the "License"); you may not use this file except in compliance with
 *   the License. You may obtain a copy of the License at
 *   http://www.mozilla.org/MPL/
 *
 *   Software distributed under the License is distributed on an "AS IS" basis,
 *   WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *   for the specific language governing rights and limitations under the
 *   License.
 *
 *   Copyright 2009-2013, Red Hat Inc.
 *   Based on mozilla.org's scriptable plugin example
 *
 *   The Original Code is mozilla.org code.
 *
 *   The Initial Developer of the Original Code is
 *   Netscape Communications Corporation.
 *   Portions created by the Initial Developer are Copyright (C) 1998
 *   the Initial Developer. All Rights Reserved.
 *
 *   Contributor(s):
 *   Uri Lublin
 *   Martin Stransky
 *   Peter Hatina
 *   Christophe Fergeau
 *
 *   Alternatively, the contents of this file may be used under the terms of
 *   either the GNU General Public License Version 2 or later (the "GPL"), or
 *   the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 *   in which case the provisions of the GPL or the LGPL are applicable instead
 *   of those above. If you wish to allow use of your version of this file only
 *   under the terms of either the GPL or the LGPL, and not to allow others to
 *   use your version of this file under the terms of the MPL, indicate your
 *   decision by deleting the provisions above and replace them with the notice
 *   and other provisions required by the GPL or the LGPL. If you do not delete
 *   the provisions above, a recipient may use your version of this file under
 *   the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <glib.h>

extern "C" {
#  include <stdint.h>
#  include <unistd.h>
#  include <fcntl.h>
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <sys/wait.h>
}

#include "rederrorcodes.h"
#include "controller.h"
#include "plugin.h"

SpiceController::SpiceController(nsPluginInstance *aPlugin):
    m_plugin(aPlugin),
    m_client_socket(-1)
{
    // create temporary directory in /tmp
    char tmp_dir[] = "/tmp/spicec-XXXXXX";
    m_tmp_dir = mkdtemp(tmp_dir);
}

SpiceController::~SpiceController()
{
    g_debug(G_STRFUNC);
    Disconnect();

    // delete the temporary directory used for a client socket
    rmdir(m_tmp_dir.c_str());
}

void SpiceController::SetFilename(const std::string &name)
{
    m_name = name;
}

void SpiceController::SetProxy(const std::string &proxy)
{
    m_proxy = proxy;
}

int SpiceController::Connect()
{
    // check, if we have a filename for socket to create
    if (m_name.empty())
        return -1;

    if (m_client_socket == -1)
    {
        if ((m_client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            g_critical("controller socket: %s", g_strerror(errno));
            return -1;
        }
    }

    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    if (m_name.length() + 1 > sizeof(remote.sun_path))
        return -1;
    strcpy(remote.sun_path, m_name.c_str());

    int rc = connect(m_client_socket, (struct sockaddr *) &remote, strlen(remote.sun_path) + sizeof(remote.sun_family));
    if (rc == -1)
    {
        g_critical("controller connect: %s", g_strerror(errno));
    }
    else
    {
        g_debug("controller connected");
    }

    return rc;
}

int SpiceController::Connect(const int nRetries)
{
    int rc = -1;
    int sleep_time = 0;

    // try to connect for specified count
    for (int i = 0; rc != 0 && i < nRetries; ++i)
    {
        rc = Connect();
        sleep(sleep_time);
        ++sleep_time;
    }

    return rc;
}

void SpiceController::Disconnect()
{
    // close the socket
    close(m_client_socket);
    m_client_socket = -1;

    // delete the temporary file, which is used for the socket
    unlink(m_name.c_str());
    m_name.clear();
}

uint32_t SpiceController::Write(const void *lpBuffer, uint32_t nBytesToWrite)
{
    ssize_t len = send(m_client_socket, lpBuffer, nBytesToWrite, 0);

    if (len != (ssize_t)nBytesToWrite)
    {
        g_warning("incomplete send, bytes to write = %u, bytes written = %zd: %s",
                  nBytesToWrite, len, g_strerror(errno));
    }

    return len;
}

bool SpiceController::StartClient()
{
    std::string socket_file(m_tmp_dir);
    socket_file += "/spice-xpi";

    /* use a pipe for the children to wait until it gets tracked */
    int pipe_fds[2] = { -1, -1 };
    if (pipe(pipe_fds) < 0) {
        perror("spice-xpi system error");
        return false;
    }

    m_pid_controller = fork();
    if (m_pid_controller == 0)
    {
        setpgrp();

        close(pipe_fds[1]);
        pipe_fds[1] = -1;

        char c;
        if (read(pipe_fds[0], &c, 1) != 0)
            g_critical("Error while reading on pipe: %s", g_strerror(errno));

        close(pipe_fds[0]);
        pipe_fds[0] = -1;

        gchar **env = g_get_environ();
        env = g_environ_setenv(env, "SPICE_XPI_SOCKET", socket_file.c_str(), TRUE);
        if (!m_proxy.empty())
            env = g_environ_setenv(env, "SPICE_PROXY", m_proxy.c_str(), TRUE);

        execle("/usr/libexec/spice-xpi-client",
               "/usr/libexec/spice-xpi-client", NULL,
               env);
        g_message("failed to run spice-xpi-client, running spicec instead");

        // TODO: temporary fallback for backward compatibility
        execle("/usr/bin/spicec",
               "/usr/bin/spicec", "--controller", NULL,
               env);

        g_critical("ERROR failed to run spicec fallback");
        g_strfreev(env);
        exit(EXIT_FAILURE);
    }
    else
    {
        g_debug("child pid: %"G_GUINT64_FORMAT, (guint64)m_pid_controller);

        close(pipe_fds[0]);
        pipe_fds[0] = -1;

        pthread_t controller_thread_id;
        pthread_create(&controller_thread_id, NULL, ControllerWaitHelper,
            reinterpret_cast<void*>(this));

        close(pipe_fds[1]);
        pipe_fds[1] = -1;

        this->SetFilename(socket_file);

        return true;
    }

    g_return_val_if_reached(false);
}

void SpiceController::StopClient()
{
    if (m_pid_controller > 0)
        kill(-m_pid_controller, SIGTERM);
}

void *SpiceController::ControllerWaitHelper(void *opaque)
{
    SpiceController *fake_this = reinterpret_cast<SpiceController *>(opaque);
    if (!fake_this)
        return NULL;

    int exit_code;
    waitpid(fake_this->m_pid_controller, &exit_code, 0);
    g_debug("child finished, pid: %"G_GUINT64_FORMAT, (guint64)exit_code);

    fake_this->m_plugin->OnSpiceClientExit(exit_code);
    fake_this->m_pid_controller = -1;

    return NULL;
}

int SpiceController::TranslateRC(int nRC)
{
    switch (nRC)
    {
    case SPICEC_ERROR_CODE_SUCCESS:
        return 0;

    case SPICEC_ERROR_CODE_GETHOSTBYNAME_FAILED:
        return RDP_ERROR_CODE_HOST_NOT_FOUND;

    case SPICEC_ERROR_CODE_CONNECT_FAILED:
        return RDP_ERROR_CODE_WINSOCK_CONNECT_FAILED;

    case SPICEC_ERROR_CODE_ERROR:
    case SPICEC_ERROR_CODE_SOCKET_FAILED:
        return RDP_ERROR_CODE_INTERNAL_ERROR;

    case SPICEC_ERROR_CODE_RECV_FAILED:
        return RDP_ERROR_RECV_WINSOCK_FAILED;

    case SPICEC_ERROR_CODE_SEND_FAILED:
        return RDP_ERROR_SEND_WINSOCK_FAILED;

    case SPICEC_ERROR_CODE_NOT_ENOUGH_MEMORY:
        return RDP_ERROR_CODE_OUT_OF_MEMORY;

    case SPICEC_ERROR_CODE_AGENT_TIMEOUT:
        return RDP_ERROR_CODE_TIMEOUT;

    case SPICEC_ERROR_CODE_AGENT_ERROR:
        return RDP_ERROR_CODE_INTERNAL_ERROR;

    default:
        return RDP_ERROR_CODE_INTERNAL_ERROR;
    }
}

