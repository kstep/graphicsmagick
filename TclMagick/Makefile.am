
EXTRA_DIST=LICENSE tests images doc pkgIndex.tcl
SUBDIRS = generic

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkglibdir)
	$(INSTALL_DATA) pkgIndex.tcl $(DESTDIR)$(pkglibdir);
