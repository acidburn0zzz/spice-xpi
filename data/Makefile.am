if BUILD_GENERATOR
TEST_PAGE         = test.html
IDL               = $(top_srcdir)/SpiceXPI/src/plugin/nsISpicec.idl
GENERATOR         = $(top_builddir)/generator/spice-xpi-generator

pkgdata_DATA = $(TEST_PAGE)

$(TEST_PAGE): $(IDL)
	$(AM_V_GEN)$(GENERATOR) -i $< -o $@
endif
