AC_PREREQ([2.57])
AC_INIT(spice-xpi, [2.5.2pre], [], spice-xpi)

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
AM_PROG_CC_C_O

# Check for the CPU we are using
#
AC_MSG_CHECKING(for x86 or x86-64 platform)
case $host_cpu in
  i386|i486|i586|i686|i786|k6|k7)
       variant=32
        ;;
  x86_64)
       variant=64
       ;;
  *)
       AC_MSG_RESULT(no)
       echo Only x86 and x86-64 are supported
       exit 1
esac
AC_MSG_RESULT($variant bit)
AM_CONDITIONAL([X86_64], [test "$variant" = 64])

dnl =========================================================================
dnl Check deps

PKG_CHECK_MODULES(LOG4CPP, log4cpp)
AC_SUBST(LOG4CPP_CFLAGS)
AC_SUBST(LOG4CPP_LIBS)

# The explicit nspr dep is needed because libxul-embedding
# in RHEL5 is missing the Requires
PKG_CHECK_MODULES(XUL, libxul-embedding >= 1.9 nspr >= 4.7.1)
AC_SUBST(XUL_CFLAGS)
AC_SUBST(XUL_LIBS)

# Find xpidl
for i in `pkg-config --variable=libdir libxul`/xpidl ; do
    if test -x $i ; then
       XPIDL=$i
    fi
done
if test x"XPIDL" = x ; then
   AC_MSG_ERROR([Can't find xpidl based on libxulrunner .pc file])
fi
AC_SUBST(XPIDL)

XUL_INCLUDEDIR=`pkg-config --variable=includedir libxul`
XUL_IDLDIR=`pkg-config --variable=idldir libxul`
AC_SUBST(XUL_INCLUDEDIR)
AC_SUBST(XUL_IDLDIR)

dnl libxul 1.9.1 changed header file names
PKG_CHECK_MODULES(XUL191,
        libxul >= 1.9.1,
        have_xul191=yes,
        have_xul191=no)

AM_CONDITIONAL([HAVE_XUL191], [test "x$have_xul191" = "xyes"])
if test "x$have_xul191" = "xyes" ; then
  AC_DEFINE([HAVE_XUL191], [], [Define if we have libxul >= 1.9.1])
fi

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_OUTPUT([
Makefile
data/Makefile
SpiceXPI/Makefile
SpiceXPI/src/Makefile
SpiceXPI/src/plugin/Makefile
])

dnl ==========================================================================
AC_MSG_NOTICE([

        Spice-XPI $VERSION
        ==============

        prefix:                   ${prefix}
        compiler:                 ${CC}
        xpidl:			  ${XPIDL}
        XUL includes:		  ${XUL_INCLUDEDIR}
        XUL IDL files:	          ${XUL_IDLDIR}

        Red target:               ${red_target}

        Now type 'make' to build $PACKAGE
])
