NULL            =
EXTENSION_ID    = SpiceXPI@redhat.com
FIREFOX_APPID   = {ec8030f7-c20a-464f-9b0e-13a3a9e97384}
extensiondir    = $(libdir)/mozilla
SDK_INCLUDE_DIR = `pkg-config --variable=idldir libxul`

plugindir=$(extensiondir)/plugins

plugin_LTLIBRARIES = libnsISpicec.la

libnsISpicec_la_LDFLAGS = -avoid-version -module
libnsISpicec_la_CPPFLAGS =			\
	-I$(XUL_INCLUDEDIR)			\
	-I$(XUL_INCLUDEDIR)/dom			\
	-I$(XUL_INCLUDEDIR)/necko		\
	-I$(XUL_INCLUDEDIR)/plugin		\
	-I$(XUL_INCLUDEDIR)/string		\
	-I$(top_srcdir)/common			\
	$(GLIB_CFLAGS)				\
	$(SPICE_PROTOCOL_CFLAGS)		\
	$(XUL_CFLAGS)				\
	-DCAIRO_CANVAS_ACCESS_TEST		\
	-DCAIRO_CANVAS_CACHE			\
	-DCAIRO_CANVAS_NO_CHUNKS		\
	-DG_LOG_DOMAIN=\"SpiceXPI\"		\
	-DMOZILLA_INTERNAL_API			\
	-DXP_UNIX				\
	$(NULL)

libnsISpicec_la_LIBADD =			\
	$(GLIB_LIBS)				\
	$(XUL_LIBS)				\
	$(NULL)

libnsISpicec_la_SOURCES =			\
	$(top_srcdir)/common/common.h		\
	$(top_srcdir)/common/rederrorcodes.h	\
	controller.cpp				\
	controller.h				\
	np_entry.cpp				\
	npn_gate.cpp				\
	npp_gate.cpp				\
	npplat.h				\
	nsISpicec.h				\
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

if HAVE_XPIDL
nsISpicec.h: nsISpicec.idl
	$(AM_V_GEN)$(XPIDL) -m header -I $(SDK_INCLUDE_DIR) $<

nsISpicec.xpt: nsISpicec.idl
	$(AM_V_GEN)$(XPIDL) -m typelib -I $(SDK_INCLUDE_DIR) $<
else
nsISpicec.h: nsISpicec.idl
	$(AM_V_GEN)$(PYTHON) `pkg-config --variable=sdkdir libxul`/sdk/bin/header.py --cachedir . -I $(SDK_INCLUDE_DIR) $< -o $@

nsISpicec.xpt: nsISpicec.idl
	$(AM_V_GEN)$(PYTHON) `pkg-config --variable=sdkdir libxul`/sdk/bin/typelib.py --cachedir . -I $(SDK_INCLUDE_DIR) $< -o $@
endif

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
