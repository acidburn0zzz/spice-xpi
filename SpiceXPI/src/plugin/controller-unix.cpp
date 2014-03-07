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
#include "config.h"

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
#include "controller-unix.h"
#include "plugin.h"

SpiceControllerUnix::SpiceControllerUnix(nsPluginInstance *aPlugin):
    SpiceController(aPlugin),
    m_client_socket(-1)
{
    // create temporary directory in /tmp
    char tmp_dir[] = "/tmp/spicec-XXXXXX";
    m_tmp_dir = mkdtemp(tmp_dir);
}

SpiceControllerUnix::~SpiceControllerUnix()
{
    g_debug("%s", G_STRFUNC);
    Disconnect();

    // delete the temporary directory used for a client socket
    rmdir(m_tmp_dir.c_str());
}

int SpiceControllerUnix::Connect()
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
        if (errno == EISCONN)
            rc = 1;
        g_critical("controller connect: %s", g_strerror(errno));
    }
    else
    {
        g_debug("controller connected");
    }

    return rc;
}

bool SpiceControllerUnix::CheckPipe()
{
}

GStrv SpiceControllerUnix::GetClientPath()
{
    const char *client_argv[] = { "/usr/libexec/spice-xpi-client", NULL };

    return g_strdupv((GStrv)client_argv);
}

GStrv SpiceControllerUnix::GetFallbackClientPath()
{
    const char *fallback_argv[] = { "/usr/bin/spicec", "--controller", NULL };

    return g_strdupv((GStrv)fallback_argv);
}

void SpiceControllerUnix::SetupControllerPipe(GStrv &env)
{
    std::string socket_file(this->m_tmp_dir);
    socket_file += "/spice-xpi";

    this->SetFilename(socket_file);

    env = g_environ_setenv(env, "SPICE_XPI_SOCKET", socket_file.c_str(), TRUE);
}

void SpiceControllerUnix::StopClient()
{
    if (m_pid_controller > 0)
        kill(-m_pid_controller, SIGTERM);
}

uint32_t SpiceControllerUnix::Write(const void *lpBuffer, uint32_t nBytesToWrite)
{
    ssize_t len = send(m_client_socket, lpBuffer, nBytesToWrite, 0);

    if (len != (ssize_t)nBytesToWrite)
    {
        g_warning("incomplete send, bytes to write = %u, bytes written = %zd: %s",
                  nBytesToWrite, len, g_strerror(errno));
    }

    return len;
}

void SpiceControllerUnix::Disconnect()
{
    // close the socket
    close(m_client_socket);
    m_client_socket = -1;

    // delete the temporary file, which is used for the socket
    unlink(m_name.c_str());
    m_name.clear();
}
