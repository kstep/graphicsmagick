
EXTRA_DIST=LICENSE tests images doc pkgIndex.tcl
SUBDIRS = generic

install-data-local:
	$(mkinstalldirs) $(DESTDIR)@libdir@/TclMagick@VERSION@
	$(INSTALL_DATA) pkgIndex.tcl $(DESTDIR)@libdir@/TclMagick@VERSION@
