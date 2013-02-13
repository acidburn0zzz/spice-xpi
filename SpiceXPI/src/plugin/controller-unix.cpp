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

void SpiceController::SetupControllerPipe(GStrv &env)
{
    std::string socket_file(this->m_tmp_dir);
    socket_file += "/spice-xpi";

    this->SetFilename(socket_file);

    env = g_environ_setenv(env, "SPICE_XPI_SOCKET", socket_file.c_str(), TRUE);
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

void SpiceController::ChildExited(GPid pid, gint status, gpointer user_data)
{
    SpiceController *fake_this = (SpiceController *)user_data;

    g_message("Client with pid %p exited", pid);

    g_main_loop_quit(fake_this->m_child_watch_mainloop);
    /* FIXME: we are not in the main thread!! */
    fake_this->m_plugin->OnSpiceClientExit(status);
}

void SpiceController::WaitForPid(GPid pid)
{
    GMainContext *context;
    GSource *source;

    context = g_main_context_new();

    m_child_watch_mainloop = g_main_loop_new(context, FALSE);
    source = g_child_watch_source_new(pid);
    g_source_set_callback(source, (GSourceFunc)ChildExited, this, NULL);
    g_source_attach(source, context);

    g_main_loop_run(m_child_watch_mainloop);

    g_main_loop_unref(m_child_watch_mainloop);
    g_main_context_unref(context);

    g_spawn_close_pid(pid);
    if (pid == m_pid_controller)
        m_pid_controller = 0;
}


gpointer SpiceController::ClientThread(gpointer data)
{
    char *spice_xpi_argv[] = { "/usr/libexec/spice-xpi-client", NULL };
    SpiceController *fake_this = (SpiceController *)data;
    gchar **env = g_get_environ();
    GPid pid;
    gboolean spawned;
    GError *error = NULL;


    fake_this->SetupControllerPipe(env);

    if (!fake_this->m_proxy.empty())
        env = g_environ_setenv(env, "SPICE_PROXY", fake_this->m_proxy.c_str(), TRUE);

    spawned = g_spawn_async(NULL,
                            spice_xpi_argv, env,
                            G_SPAWN_DO_NOT_REAP_CHILD,
                            NULL, NULL, /* child_func, child_arg */
                            &pid, &error);
    if (error != NULL) {
        g_warning("failed to start spice-xpi-client: %s", error->message);
        g_clear_error(&error);
    }
    if (!spawned) {
        // TODO: temporary fallback for backward compatibility
        char *spicec_argv[] = { "/usr/bin/spicec", "--controller", NULL };
        g_message("failed to run spice-xpi-client, running spicec instead");
        spawned = g_spawn_async(NULL, spicec_argv, env,
                                G_SPAWN_DO_NOT_REAP_CHILD,
                                NULL, NULL, /* child_func, child_arg */
                                &pid, &error);
    }
    if (error != NULL) {
        g_warning("failed to start spice-xpi-client: %s", error->message);
        g_clear_error(&error);
    }
    g_strfreev(env);
    if (!spawned) {
        g_critical("ERROR failed to run spicec fallback");
        return NULL;
    }

#ifdef XP_UNIX
    fake_this->m_pid_controller = pid;
#endif
    fake_this->WaitForPid(pid);

    return NULL;
}

bool SpiceController::StartClient()
{
    GThread *thread;

    thread = g_thread_new("spice-xpi client thread", ClientThread, this);

    return (thread != NULL);
}

void SpiceController::StopClient()
{
    if (m_pid_controller > 0)
        kill(-m_pid_controller, SIGTERM);
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

