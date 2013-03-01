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

#include <aclapi.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <glib.h>
#include <gio/gwin32outputstream.h>

#include "rederrorcodes.h"
#include "controller-win.h"
#include "plugin.h"

SpiceControllerWin::SpiceControllerWin(nsPluginInstance *aPlugin):
    SpiceController(aPlugin)
{
    g_random_set_seed(time(NULL));
}

SpiceControllerWin::~SpiceControllerWin()
{
}

int SpiceControllerWin::Connect()
{
    HANDLE hClientPipe;

    hClientPipe = CreateFile(m_name.c_str(),
                             GENERIC_READ |  GENERIC_WRITE,
                             0, NULL,
                             OPEN_EXISTING,
                             SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                             NULL);
    g_return_val_if_fail(hClientPipe != INVALID_HANDLE_VALUE, -1);

    g_warning("Connection OK");
    m_pipe = g_win32_output_stream_new(hClientPipe, TRUE);
    return 0;
}

static int get_sid(HANDLE handle, PSID* ppsid, PSECURITY_DESCRIPTOR* ppsec_desc)
{
    DWORD ret = GetSecurityInfo(handle, SE_KERNEL_OBJECT, OWNER_SECURITY_INFORMATION,
                                ppsid, NULL, NULL, NULL, ppsec_desc);
    if (ret != ERROR_SUCCESS) {
        return ret;
    }
    if (!IsValidSid(*ppsid)) {
        return -1;
    }
    return 0;
}

//checks whether the handle owner is the current user.
static bool is_same_user(HANDLE handle)
{
    PSECURITY_DESCRIPTOR psec_desc_handle = NULL;
    PSECURITY_DESCRIPTOR psec_desc_user = NULL;
    PSID psid_handle;
    PSID psid_user;
    bool ret;

    ret = !get_sid(handle, &psid_handle, &psec_desc_handle) &&
          !get_sid(GetCurrentProcess(), &psid_user, &psec_desc_user) &&
          EqualSid(psid_handle, psid_user);
    LocalFree(psec_desc_handle);
    LocalFree(psec_desc_user);
    return ret;
}

bool SpiceControllerWin::CheckPipe()
{
    void *hClientPipe;

    g_return_val_if_fail(G_IS_WIN32_OUTPUT_STREAM(m_pipe), false);

    hClientPipe = g_win32_output_stream_get_handle(G_WIN32_OUTPUT_STREAM(m_pipe));
    // Verify the named-pipe-server owner is the current user.
    if (hClientPipe != INVALID_HANDLE_VALUE) {
        if (!is_same_user(hClientPipe)) {
            g_critical("Closing pipe to spicec -- it is not safe");
            g_object_unref(G_OBJECT(m_pipe));
            m_pipe = NULL;
            return false;
        }
    }

    return true;
}

#define SPICE_CLIENT_REGISTRY_KEY TEXT("Software\\spice-space.org\\spicex")
#define SPICE_XPI_DLL TEXT("npSpiceConsole.dll")
#define RED_CLIENT_FILE_NAME TEXT("spicec.exe")
#define CMDLINE_LENGTH 32768

GStrv SpiceControllerWin::GetClientPath()
{
    LONG lret;
    HKEY hkey;
    DWORD dwType = REG_SZ;
    TCHAR lpCommandLine[CMDLINE_LENGTH] = {0};
    DWORD dwSize = sizeof(lpCommandLine);
    GError *err = NULL;
    gchar **args = NULL;

    lret = RegOpenKeyEx(HKEY_CURRENT_USER, SPICE_CLIENT_REGISTRY_KEY,
            0, KEY_READ, &hkey);
    g_return_val_if_fail(lret == ERROR_SUCCESS, NULL);

    lret = RegQueryValueEx(hkey, "client", NULL, &dwType,
            (LPBYTE)lpCommandLine, &dwSize);
    RegCloseKey(hkey);

    /* The registry key contains the command to run as a string, the GSpawn
     * API expects an array of strings. The awkward part is that the GSpawn
     * API will then rebuild a commandline string from this array :-/ */
    g_shell_parse_argv(lpCommandLine, NULL, &args, &err);
    if (err != NULL)
        g_warning("Failed to parse '%s': %s", lpCommandLine, err->message);

    return args;
}

GStrv SpiceControllerWin::GetFallbackClientPath()
{
    HMODULE hModule;
    gchar *module_path;
    GStrv fallback_argv;

    // we assume the Spice client binary is located in the same dir as the
    // Firefox plugin
    hModule = GetModuleHandle(SPICE_XPI_DLL);
    g_return_val_if_fail(hModule != NULL, NULL);

    module_path = g_win32_get_package_installation_directory_of_module(hModule);
    g_return_val_if_fail(module_path != NULL, NULL);
    fallback_argv = g_new0(char *, 3);
    fallback_argv[0] = g_build_filename(module_path, RED_CLIENT_FILE_NAME, NULL);
    fallback_argv[1] = g_strdup("--controller");
    g_free(module_path);

    return fallback_argv;
}

#define RED_CLIENT_PIPE_NAME TEXT("\\\\.\\pipe\\SpiceController-%lu")
void SpiceControllerWin::SetupControllerPipe(GStrv &env)
{
    char *pipe_name;
    pipe_name = g_strdup_printf(RED_CLIENT_PIPE_NAME, (unsigned long)g_random_int());
    this->SetFilename(pipe_name);
    env = g_environ_setenv(env, "SPICE_XPI_NAMEDPIPE", pipe_name, TRUE);
    g_free(pipe_name);
}

void SpiceControllerWin::StopClient()
{
    if (m_pid_controller != NULL) {
        //WaitForPid will take care of closing the handle
        TerminateProcess(m_pid_controller, 0);
        m_pid_controller = NULL;
    }
}


uint32_t SpiceControllerWin::Write(const void *lpBuffer, uint32_t nBytesToWrite)
{
    GError *error = NULL;
    gsize bytes_written;

    g_return_val_if_fail(G_IS_OUTPUT_STREAM(m_pipe), 0);

    g_output_stream_write_all(m_pipe, lpBuffer, nBytesToWrite,
                              &bytes_written, NULL, &error);
    if (error != NULL) {
        g_warning("Error writing to controller pipe: %s", error->message);
        g_clear_error(&error);
        return -1;
    }
    if (bytes_written != nBytesToWrite) {
        g_warning("Partial write (%"G_GSIZE_MODIFIER"u instead of %u",
                  bytes_written, (unsigned int)nBytesToWrite);
    }

    return bytes_written;
}
