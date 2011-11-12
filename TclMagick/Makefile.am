AUTOMAKE_OPTIONS = 1.11 dist-zip dist-bzip2 dist-lzma foreign
ACLOCAL_AMFLAGS = -I unix/m4

EXTRA_DIST=LICENSE tests images doc pkgIndex.tcl
SUBDIRS = generic

install-data-local:
	$(mkinstalldirs) $(DESTDIR)@libdir@/TclMagick@VERSION@
	$(INSTALL_DATA) $(top_srcdir)/pkgIndex.tcl $(DESTDIR)@libdir@/TclMagick@VERSION@
