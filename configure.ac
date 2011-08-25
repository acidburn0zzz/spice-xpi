AC_PREREQ([2.57])
AC_INIT(spice-xpi, [2.6], [], spice-xpi)

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

# Before making a release, the SPICE_LT_VERSION string should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A
SPICE_LT_VERSION=0:0:0
AC_SUBST(SPICE_LT_VERSION)

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

AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    os_win32=yes
    ;;
  *)
    os_win32=no
    ;;
esac
AC_MSG_RESULT([$os_win32])

case $host in
  *-*-linux*)
    os_linux=yes
    ;;
esac

dnl =========================================================================
dnl Check OS target

AC_MSG_CHECKING([for some Win32 platform])
case "$host" in
  *-*-mingw*|*-*-cygwin*)
    platform_win32=yes
    ;;
  *)
    platform_win32=no
    ;;
esac
AC_MSG_RESULT([$platform_win32])
if test "$platform_win32" = yes; then
  red_target=windows
else
  red_target=x11
fi
AC_SUBST(red_target)

AM_CONDITIONAL(OS_WIN32, test "$os_win32" = "yes")
AM_CONDITIONAL(OS_UNIX, test "$os_win32" != "yes")
AM_CONDITIONAL(OS_LINUX, test "$os_linux" = "yes")

dnl =========================================================================
dnl Check deps

AC_CHECK_LIBM
AC_SUBST(LIBM)

AC_CHECK_LIB(rt, clock_gettime,
   AC_DEFINE(HAVE_CLOCK_GETTIME, 1, [Defined if we have clock_gettime()])
   LIBRT=-lrt
   )
AC_SUBST(LIBRT)

SPICE_NONPKGCONFIG_LIBS+=" $LIBM $LIBRT"

SPICE_REQUIRES=""

PKG_CHECK_MODULES(LOG4CPP, log4cpp)
AC_SUBST(LOG4CPP_CFLAGS)
AC_SUBST(LOG4CPP_LIBS)
SPICE_REQUIRES+=" log4cpp"

# Add parameter for (partial) static linkage of spice client.
# this is used to achive single binary package for all (?) distros.
AC_ARG_ENABLE(static-linkage, 
	[  --enable-static-linkage will generate spice client binary with static linkage to external libraries ],
	[SPICEC_STATIC_LINKAGE_BSTATIC=["-Wl,-Bstatic"];
	SPICEC_STATIC_LINKAGE_BDYNAMIC=["-Wl,-Bdynamic"]])

# The explicit nspr dep is needed because libxul-embedding
# in RHEL5 is missing the Requires
PKG_CHECK_MODULES(XUL, libxul-embedding >= 1.9 nspr >= 4.7.1)
AC_SUBST(XUL_CFLAGS)
AC_SUBST(XUL_LIBS)
SPICE_REQUIRES+=" libxul-embedding >= 1.9 nspr >= 4.7.1"

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

dnl ===========================================================================
dnl check compiler flags

AC_DEFUN([SPICE_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether $CC supports $1])

  spice_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1"

  AC_COMPILE_IFELSE([ ], [spice_cc_flag=yes], [spice_cc_flag=no])
  CFLAGS="$spice_save_CFLAGS"

  if test "x$spice_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$spice_cc_flag])
])


dnl Use lots of warning flags with with gcc and compatible compilers

dnl Note: if you change the following variable, the cache is automatically
dnl skipped and all flags rechecked.  So there's no need to do anything
dnl else.  If for any reason you need to force a recheck, just change
dnl MAYBE_WARN in an ignorable way (like adding whitespace)

dnl MAYBE_WARN="-Wall -Wno-sign-compare -Werror -Wno-deprecated-declarations"

MAYBE_WARN="-Wall -Wno-sign-compare -Werror -Wno-deprecated-declarations"


# invalidate cached value if MAYBE_WARN has changed
if test "x$spice_cv_warn_maybe" != "x$MAYBE_WARN"; then
	unset spice_cv_warn_cflags
fi
AC_CACHE_CHECK([for supported warning flags], spice_cv_warn_cflags, [
	echo
	WARN_CFLAGS=""

	# Some warning options are not supported by all versions of
	# gcc, so test all desired options against the current
	# compiler.
	#
	# Note that there are some order dependencies
	# here. Specifically, an option that disables a warning will
	# have no net effect if a later option then enables that
	# warnings, (perhaps implicitly). So we put some grouped
	# options (-Wall and -Wextra) up front and the -Wno options
	# last.

	for W in $MAYBE_WARN; do
		SPICE_CC_TRY_FLAG([$W], [WARN_CFLAGS="$WARN_CFLAGS $W"])
	done

	spice_cv_warn_cflags=$WARN_CFLAGS
	spice_cv_warn_maybe=$MAYBE_WARN

	AC_MSG_CHECKING([which warning flags were supported])])
WARN_CFLAGS="$spice_cv_warn_cflags"
SPICE_CFLAGS="$SPICE_CFLAGS $WARN_CFLAGS"

# We only wish to enable attribute(warn_unused_result) if we can prevent
# gcc from generating thousands of warnings about the misapplication of the
# attribute to void functions and variables.
AC_MSG_CHECKING([how to enable unused result warnings])
warn_unused_result=""
if echo $WARN_CFLAGS | grep -e '-Wno-attributes' >/dev/null; then
    AC_TRY_COMPILE([__attribute__((__warn_unused_result__))
	int f (int i) { return i; }], [],
	[warn_unused_result="__attribute__((__warn_unused_result__))"])
fi
AC_DEFINE_UNQUOTED([WARN_UNUSED_RESULT], [$warn_unused_result],
	  [Define to the value your compiler uses to support the warn-unused-result attribute])
AC_MSG_RESULT([$warn_unused_result])

AC_SUBST(WARN_CFLAGS)
AC_SUBST(CFLAGS_CFLAGS)

dnl =========================================================================
dnl -fvisibility stuff

have_gcc4=no
AC_MSG_CHECKING(for -fvisibility)
AC_COMPILE_IFELSE([
#if defined(__GNUC__) && (__GNUC__ >= 4)
#else
error Need GCC 4.0 for visibility
#endif
int main () { return 0; }
], have_gcc4=yes)

if test "x$have_gcc4" = "xyes"; then
   VISIBILITY_HIDDEN_CFLAGS="-fvisibility=hidden"
fi
AC_MSG_RESULT($have_gcc4)
AC_SUBST(VISIBILITY_HIDDEN_CFLAGS)


AC_SUBST(SPICE_REQUIRES)
AC_SUBST(SPICE_NONPKGCONFIG_CFLAGS)
AC_SUBST(SPICE_NONPKGCONFIG_LIBS)

AC_SUBST([SPICEC_STATIC_LINKAGE_BSTATIC])
AC_SUBST([SPICEC_STATIC_LINKAGE_BDYNAMIC])

AC_OUTPUT([
Makefile
SpiceXPI/Makefile
SpiceXPI/src/Makefile
SpiceXPI/src/plugin/Makefile
])

dnl ==========================================================================
echo "

        Spice $VERSION
        ==============

        prefix:                   ${prefix}
        compiler:                 ${CC}
        xpidl:			  ${XPIDL}
        XUL includes:		  ${XUL_INCLUDEDIR}
        XUL IDL files:	          ${XUL_IDLDIR}

        Have XRANDR 1.2:          ${have_xrandr12}

        Red target:               ${red_target}

        Now type 'make' to build $PACKAGE
"
