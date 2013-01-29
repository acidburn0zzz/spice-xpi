/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1998  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#include <unistd.h>
#include <string.h>

#include "glib-compat.h"

#if !GLIB_CHECK_VERSION (2,32,0)
static gint
g_environ_find (gchar       **envp,
                const gchar  *variable)
{
  gint len, i;

  if (envp == NULL)
    return -1;

  len = strlen (variable);

  for (i = 0; envp[i]; i++)
    {
      if (strncmp (envp[i], variable, len) == 0 &&
          envp[i][len] == '=')
        return i;
    }

  return -1;
}

/**
 * g_environ_getenv:
 * @envp: (allow-none) (array zero-terminated=1) (transfer none): an environment
 *     list (eg, as returned from g_get_environ()), or %NULL
 *     for an empty environment list
 * @variable: the environment variable to get, in the GLib file name
 *     encoding
 *
 * Returns the value of the environment variable @variable in the
 * provided list @envp.
 *
 * The name and value are in the GLib file name encoding.
 * On UNIX, this means the actual bytes which might or might not
 * be in some consistent character set and encoding. On Windows,
 * it is in UTF-8. On Windows, in case the environment variable's
 * value contains references to other environment variables, they
 * are expanded.
 *
 * Return value: the value of the environment variable, or %NULL if
 *     the environment variable is not set in @envp. The returned
 *     string is owned by @envp, and will be freed if @variable is
 *     set or unset again.
 *
 * Since: 2.32
 */
const gchar *
g_environ_getenv (gchar       **envp,
                  const gchar  *variable)
{
  gint index;

  g_return_val_if_fail (variable != NULL, NULL);

  index = g_environ_find (envp, variable);
  if (index != -1)
    return envp[index] + strlen (variable) + 1;
  else
    return NULL;
}

/**
 * g_environ_setenv:
 * @envp: (allow-none) (array zero-terminated=1) (transfer full): an environment
 *     list that can be freed using g_strfreev() (e.g., as returned from g_get_environ()), or %NULL
 *     for an empty environment list
 * @variable: the environment variable to set, must not contain '='
 * @value: the value for to set the variable to
 * @overwrite: whether to change the variable if it already exists
 *
 * Sets the environment variable @variable in the provided list
 * @envp to @value.
 *
 * Both the variable's name and value should be in the GLib
 * file name encoding. On UNIX, this means that they can be
 * arbitrary byte strings. On Windows, they should be in UTF-8.
 *
 * Return value: (array zero-terminated=1) (transfer full): the
 *     updated environment list. Free it using g_strfreev().
 *
 * Since: 2.32
 */
gchar **
g_environ_setenv (gchar       **envp,
                  const gchar  *variable,
                  const gchar  *value,
                  gboolean      overwrite)
{
  gint index;

  g_return_val_if_fail (variable != NULL, NULL);
  g_return_val_if_fail (strchr (variable, '=') == NULL, NULL);

  index = g_environ_find (envp, variable);
  if (index != -1)
    {
      if (overwrite)
        {
          g_free (envp[index]);
          envp[index] = g_strdup_printf ("%s=%s", variable, value);
        }
    }
  else
    {
      gint length;

      length = envp ? g_strv_length (envp) : 0;
      envp = g_renew (gchar *, envp, length + 2);
      envp[length] = g_strdup_printf ("%s=%s", variable, value);
      envp[length + 1] = NULL;
    }

  return envp;
}
#endif /* !GLIB_CHECK_VERSION (2,32,0) */


#if !GLIB_CHECK_VERSION (2,28,0)
#ifndef G_OS_WIN32
/* According to the Single Unix Specification, environ is not
 * in any system header, although unistd.h often declares it.
 */
extern char **environ;

gchar **
g_get_environ (void)
{
  return g_strdupv (environ);
}

#else /* G_OS_WIN32 */

gchar **
g_get_environ (void)
{
  gunichar2 *strings;
  gchar **result;
  gint i, n;

  strings = GetEnvironmentStringsW ();
  for (n = 0, i = 0; strings[n]; i++)
    n += wcslen (strings + n) + 1;

  result = g_new (char *, i + 1);
  for (n = 0, i = 0; strings[n]; i++)
    {
      result[i] = g_utf16_to_utf8 (strings + n, -1, NULL, NULL, NULL);
      n += wcslen (strings + n) + 1;
    }
  FreeEnvironmentStringsW (strings);
  result[i] = NULL;

  return result;
}
#endif /* G_OS_WIN32 */
#endif /* !GLIB_CHECK_VERSION (2,28,0) */
