
AUTOMAKE_OPTIONS = 1.6 dist-zip dist-bzip2 foreign

# Directories to include in distribution
DIST_SUBDIRS = basics fpx jpeg ole oless ri_image

# Directories to build
SUBDIRS = oless jpeg basics ole ri_image fpx

# Where to look for include files
INCLUDES = -I$(top_srcdir)

# Headers which are installed
include_HEADERS = fpxlib.h

# Shared libraries built in this directory
lib_LTLIBRARIES = libfpx.la

# Library ld flags
libfpx_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

# Sources
libfpx_la_SOURCES =

# Convenience libraries
libfpx_la_LIBADD = \
  basics/libBasics.la fpx/libfpx.la jpeg/libJpeg.la ole/libole.la \
  ri_image/libRiImage.la oless/libOless.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

# Additional files to distribute
EXTRA_DIST = \
	README \
	README.gcc \
	_mac.sea \
	commk.gcc \
	depend.gcc \
	fpx.h \
	makefile.gcc \
	doc/fpx_api.pdf \
	doc/readme.txt \
	doc/ri_arch.pdf \
	_win/fpxlib.aps \
	_win/fpxlib.dsp \
	_win/fpxlib.dsw \
	_win/fpxlib.opt \
	_win/fpxlib.rc \
	_win/jpeglib.aps \
	_win/jpeglib.dsp \
	_win/jpeglib.rc \
	_win/resource.h \
	macdeps/aliases.h \
	macdeps/appleeve.h \
	macdeps/appletal.h \
	macdeps/coguid.h \
	macdeps/colorcon.h \
	macdeps/colormat.h \
	macdeps/componen.h \
	macdeps/controle.h \
	macdeps/desk.h \
	macdeps/dialogs.h \
	macdeps/dialogue.h \
	macdeps/errors.h \
	macdeps/evenemen.h \
	macdeps/events.h \
	macdeps/files.h \
	macdeps/finder.h \
	macdeps/fonts.h \
	macdeps/gestalte.h \
	macdeps/gxprinti.h \
	macdeps/lists.h \
	macdeps/lowmem.h \
	macdeps/lpfilety.h \
	macdeps/menus.h \
	macdeps/nif_api.h \
	macdeps/osevents.h \
	macdeps/osutils.h \
	macdeps/packages.h \
	macdeps/pmemory.h \
	macdeps/pnoffscr.h \
	macdeps/preferen.h \
	macdeps/prefs.h \
	macdeps/printing.h \
	macdeps/pwrdef.h \
	macdeps/qdoffscr.h \
	macdeps/quickdra.h \
	macdeps/quicktim.h \
	macdeps/resource.h \
	macdeps/sane.h \
	macdeps/segload.h \
	macdeps/standard.h \
	macdeps/strings.h \
	macdeps/sysequ.h \
	macdeps/textutil.h \
	macdeps/unistd.h \
	macdeps/wininclu.h \
	macdeps/winolein.h \
	macdeps/xbitmap.h \
	macdeps/xnoffscr.h \
	macdeps/xtrackab.h \
	macdeps/xvt_env.h \
	macdeps/xvt.h \
	macdeps/xvt_styp.h \
	macdeps/xvt_type.h

# Headers which are not installed but which are distributed
noinst_HEADERS = flashpix.h

