AC_PREREQ([2.57])
AC_INIT(spice-xpi, [2.9pre], [], spice-xpi)

AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR(.)

AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
LT_LANG([Windows Resource])
AM_PROG_CC_C_O

AC_MSG_CHECKING([operating system])
case $host in
*-linux*)
        AC_MSG_RESULT([Linux])
        backend="linux"
        AC_DEFINE([XP_UNIX], 1, [Building Linux plugin])
        ;;
*-mingw*)
        AC_MSG_RESULT([Windows])
        backend="windows"
        AC_CHECK_TOOL([WINDRES], [windres], [no])
        AC_DEFINE([XP_WIN], 1, [Building Windows plugin])
        ;;
esac

AM_CONDITIONAL([OS_LINUX], [test "x$backend" = xlinux])
AM_CONDITIONAL([OS_WINDOWS], [test "x$backend" = xwindows])

dnl =========================================================================
dnl Check deps

AC_CONFIG_SUBDIRS([spice-protocol])
SPICE_PROTOCOL_CFLAGS='-I ${top_srcdir}/spice-protocol'
AC_SUBST(SPICE_PROTOCOL_CFLAGS)

SPICE_XPI_REQUIRES="glib-2.0 gio-2.0 gthread-2.0"
AS_IF([test "x$backend" = "xwindows"], [SPICE_XPI_REQUIRES="$SPICE_XPI_REQUIRES gio-windows-2.0"])
PKG_CHECK_MODULES([GLIB],[$SPICE_XPI_REQUIRES])

AC_ARG_ENABLE([xpi],
  [AS_HELP_STRING([--enable-xpi],
                  [Enable compilation of an xpi package])],
  [], [enable_xpi=no])
if test x"$enable_xpi" != xno; then
PKG_CHECK_MODULES(XUL, libxul-embedding >= 10)
AC_SUBST(XUL_CFLAGS)
AC_SUBST(XUL_LIBS)

XUL_INCLUDEDIR=`pkg-config --variable=includedir libxul`
XUL_IDLDIR=`pkg-config --variable=idldir libxul`
AC_SUBST(XUL_INCLUDEDIR)
AC_SUBST(XUL_IDLDIR)
fi
AM_CONDITIONAL([BUILD_XPI], [test x"$enable_xpi" != xno])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ARG_ENABLE([generator],
  [AS_HELP_STRING([--enable-generator],
                  [Enable compilation of a test page generator])],
  [], [enable_generator=no])
AM_CONDITIONAL([BUILD_GENERATOR], [test x$enable_generator != xno])

AC_OUTPUT([
Makefile
data/Makefile
generator/Makefile
SpiceXPI/Makefile
SpiceXPI/src/Makefile
SpiceXPI/src/plugin/Makefile
])

dnl ==========================================================================
AC_MSG_NOTICE([

        Spice-XPI $VERSION
        ==============

        prefix:                    ${prefix}
        compiler:                  ${CC}
        XUL includes:		   ${XUL_INCLUDEDIR}
        XUL IDL files:	           ${XUL_IDLDIR}
        Build test page generator: ${enable_generator}
        Build XPI package:         ${enable_xpi}

        Now type 'make' to build $PACKAGE
])
