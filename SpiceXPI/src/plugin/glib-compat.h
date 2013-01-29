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

#ifndef GLIB_COMPAT_H_
# define GLIB_COMPAT_H_

#include <glib.h>

G_BEGIN_DECLS

#if !GLIB_CHECK_VERSION (2,32,0)
const gchar *      g_environ_getenv          (gchar       **envp,
                                              const gchar  *variable);
gchar **           g_environ_setenv          (gchar       **envp,
                                              const gchar  *variable,
                                              const gchar  *value,
                                              gboolean      overwrite);
#endif

#if !GLIB_CHECK_VERSION (2,28,0)
gchar **           g_get_environ             (void);
#endif

G_END_DECLS

#endif /* GLIB_COMPAT_H_ */
