SUBDIRS = src

if BUILD_XPI
spicedatadir = $(datadir)/spice

DISTDIR = dist

all-local: SpiceXPI.xpi

CLEANFILES = SpiceXPI.xpi

SpiceXPI.xpi: $(srcdir)/src/install.rdf src/plugin/nsISpicec.xpt src/plugin/.libs/npSpiceConsole.so
	$(AM_V_GEN)rm -rf $(DISTDIR)
	@[ -d $(DISTDIR)/plugins ] || mkdir -p $(DISTDIR)/plugins
	@cp $(srcdir)/src/install.rdf $(DISTDIR)
	@cp src/plugin/*.xpt $(DISTDIR)/plugins
	@cp src/plugin/.libs/npSpiceConsole.so $(DISTDIR)/plugins/npSpiceConsole.so
	@(cd $(DISTDIR); zip -q -r ../$@ .)

distclean-local:
	rm -rf $(DISTDIR)
endif
