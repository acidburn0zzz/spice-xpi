NULL            =
EXTENSION_ID    = SpiceXPI@redhat.com
FIREFOX_APPID   = {ec8030f7-c20a-464f-9b0e-13a3a9e97384}
extensiondir    = $(libdir)/mozilla
SDK_INCLUDE_DIR = `pkg-config --variable=idldir libxul`

plugindir=$(extensiondir)/plugins

plugin_LTLIBRARIES = libnsISpicec.la

libnsISpicec_la_LDFLAGS = -avoid-version -module -no-undefined
libnsISpicec_la_CPPFLAGS =			\
	-I$(top_srcdir)/common			\
	-I$(srcdir)/npapi			\
	$(GLIB_CFLAGS)				\
	$(SPICE_PROTOCOL_CFLAGS)		\
	-DG_LOG_DOMAIN=\"SpiceXPI\"		\
	$(NULL)

libnsISpicec_la_LIBADD =			\
	$(GLIB_LIBS)				\
	$(NULL)

libnsISpicec_la_SOURCES =			\
	$(top_srcdir)/common/common.h		\
	$(top_srcdir)/common/rederrorcodes.h	\
	glib-compat.c				\
	glib-compat.h				\
	controller.cpp				\
	controller.h				\
	npapi/npapi.h				\
	npapi/npfunctions.h			\
	npapi/npruntime.h			\
	npapi/nptypes.h				\
	np_entry.cpp				\
	npn_gate.cpp				\
	npp_gate.cpp				\
	npplat.h				\
	nsISpicec.idl				\
	nsScriptablePeer.cpp			\
	nsScriptablePeer.h			\
	nsScriptablePeerBase.cpp		\
	nsScriptablePeerBase.h			\
	plugin.cpp				\
	plugin.h				\
	pluginbase.cpp				\
	pluginbase.h				\
	$(NULL)

if OS_LINUX
libnsISpicec_la_SOURCES +=			\
	controller-unix.cpp			\
	controller-unix.h			\
	$(NULL)
endif

if OS_WINDOWS
.rc.lo:
	$(LIBTOOL) --tag=RC $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile $(WINDRES) $(RCFLAGS) -i $< -o $@

libnsISpicec_la_SOURCES +=			\
	resource.rc				\
	$(NULL)

.rc.$(OBJEXT):
	$(AM_V_GEN)$(WINDRES) $(RCFLAGS) -i $< -o $@
endif

if BUILD_XPI
libnsISpicec_la_SOURCES +=			\
	nsISpicec.h				\
	$(NULL)

nsISpicec.h: nsISpicec.idl
	$(AM_V_GEN)$(PYTHON) `pkg-config --variable=sdkdir libxul`/sdk/bin/header.py --cachedir . -I $(SDK_INCLUDE_DIR) $< -o $@

nsISpicec.xpt: nsISpicec.idl
	$(AM_V_GEN)$(PYTHON) `pkg-config --variable=sdkdir libxul`/sdk/bin/typelib.py --cachedir . -I $(SDK_INCLUDE_DIR) $< -o $@

BUILT_SOURCES =					\
	nsISpicec.h				\
	nsISpicec.xpt				\
	$(NULL)

distclean-local:
	rm -f $(BUILT_SOURCES)

CLEANFILES =					\
	xpidllex.py				\
	xpidllex.pyc				\
	xpidlyacc.py				\
	xpidlyacc.pyc				\
	xpidl_debug
endif
