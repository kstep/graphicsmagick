
SUBDIRS = generic
pkgdata_DATA=LICENSE

install-data-local:
	$(INSTALL_DATA) pkgIndex.tcl $(DESTDIR)$(pkglibdir);
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) doc/*Magick.html $(DESTDIR)$(pkgdatadir);