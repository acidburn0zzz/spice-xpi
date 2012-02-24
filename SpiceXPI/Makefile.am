SUBDIRS=src

EXTRA_DIST = logger.ini
spicedatadir = $(datadir)/spice
spicedata_DATA = logger.ini

DISTDIR=dist

all-local: SpiceXPI.xpi

CLEANFILES = SpiceXPI.xpi

SpiceXPI.xpi: $(srcdir)/src/install.rdf src/plugin/nsISpicec.xpt src/plugin/.libs/libnsISpicec.so
	$(AM_V_GEN)rm -rf $(DISTDIR)
	@[ -d $(DISTDIR)/plugins ] || mkdir -p $(DISTDIR)/plugins
	@cp $(srcdir)/src/install.rdf $(DISTDIR)
	@cp src/plugin/*.xpt $(DISTDIR)/plugins
	@cp src/plugin/.libs/libnsISpicec.so $(DISTDIR)/plugins/nsISpicec.so
	@cp $(srcdir)/logger.ini $(DISTDIR)/plugins/logger.ini
	@(cd $(DISTDIR); zip -q -r ../$@ .)

distclean-local:
	rm -rf $(DISTDIR)
