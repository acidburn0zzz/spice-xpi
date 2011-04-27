SUBDIRS=src

XPI=SpiceXPI.xpi

EXTRA_DIST = logger.ini
sharedir=/usr/share/spice
share_DATA = \
    logger.ini  \
    $(NULL)

logger.ini :
	echo do nothing

DISTDIR=dist
ZIP=zip

# FIXME: we are actually using gcc 4
if X86_64
PLATFORM=Linux_x86_64-gcc3
else
PLATFORM=Linux_x86-gcc3
endif

all-local: SpiceXPI.xpi

CLEANFILES = SpiceXPI.xpi

SpiceXPI.xpi: $(top_builddir)/SpiceXPI/src/install.rdf $(top_builddir)/SpiceXPI/src/plugin/nsISpicec.xpt $(top_builddir)/SpiceXPI/src/plugin/.libs/libnsISpicec.so
	rm -rf dist
	@[ -d $(DISTDIR)/platform ] || mkdir -p $(DISTDIR)/platform
	cp $(top_srcdir)/SpiceXPI/src/install.rdf $(DISTDIR)
#	cp $(XPT) src/*.js $(DISTDIR)/components
	@[ -d $(DISTDIR)/platform/$(PLATFORM) ] || mkdir -p $(DISTDIR)/platform/$(PLATFORM)
	@[ -d $(DISTDIR)/platform/$(PLATFORM)/plugins ] || mkdir -p $(DISTDIR)/platform/$(PLATFORM)/plugins
	cp $(top_srcdir)/SpiceXPI/src/plugin/*.xpt $(DISTDIR)/platform/$(PLATFORM)/plugins
	cp $(top_builddir)/SpiceXPI/src/plugin/.libs/libnsISpicec.so* $(DISTDIR)/platform/$(PLATFORM)/plugins/nsISpicec.so
	cp $(top_srcdir)/SpiceXPI/logger.ini $(DISTDIR)/platform/$(PLATFORM)/logger.ini
	(cd $(DISTDIR); $(ZIP) -r ../$(XPI) .)

distclean-local:
	rm -rf $(DISTDIR)
