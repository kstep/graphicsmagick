#  Makefile for building ImageMagick library
#
# Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated
# to making software imaging solutions freely available.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files ("ImageMagick"),
# to deal in ImageMagick without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense
# and/or sell copies of ImageMagick, and to permit persons to whom the
# ImageMagick is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of ImageMagick.
#
# The software is provided "as is", without warranty of any kind, express or
# implied, including but not limited to the warranties of merchantability,
# fitness for a particular purpose and noninfringement.  In no event shall
# ImageMagick Studio be liable for any claim, damages or other liability,
# whether in an action of contract, tort or otherwise, arising from, out of
# or in connection with ImageMagick or the use or other dealings in
# ImageMagick.
#
# Except as contained in this notice, the name of the ImageMagick Studio
# shall not be used in advertising or otherwise to promote the sale, use or
# other dealings in ImageMagick without prior written authorization from the
# ImageMagick Studio.
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

# Shared libraries built in this directory
lib_LTLIBRARIES = libMagick.la

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@

INCLUDES = -I$(top_srcdir)

ALLMANPAGES = Magick-config.1

libMagick_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)$(MICRO_VERSION):0

# Library base sources
MAGICK_BASE_SRCS=PreRvIcccm.c animate.c annotate.c attributes.c blob.c \
  cache_view.c colors.c compress.c constitute.c decorate.c delegates.c \
  display.c draw.c effects.c enhance.c error.c gems.c image.c magick.c \
  memory.c modules.c monitor.c montage.c quantize.c segment.c shear.c \
  signature.c stash.c stream.c timer.c transform.c utility.c widget.c \
  xwindows.c zoom.c

# Library coder sources
MAGICK_CODER_SRCS=avs.c bim.c bmp.c cache.c cmyk.c dcm.c dps.c ept.c fax.c \
  fits.c fpx.c gif.c gradation.c gray.c hdf.c histogram.c html.c icc.c icon.c \
  iptc.c jbig.c jpeg.c label.c logo.c map.c matte.c miff.c mono.c mtv.c \
  pcd.c pcl.c pcx.c pdb.c pdf.c pict.c pix.c png.c plasma.c pnm.c preview.c \
  ps.c ps2.c ps3.c psd.c pwp.c rgb.c rla.c rle.c sct.c sfw.c sgi.c stegano.c \
  sun.c svg.c tga.c tiff.c tile.c tim.c ttf.c txt.c uil.c uyvy.c vicar.c \
  vid.c viff.c wbmp.c x.c xbm.c xc.c xpm.c xwd.c yuv.c

# Library sources
if WITH_MODULES
libMagick_la_SOURCES = $(MAGICK_BASE_SRCS)
libMagick_la_LIBADD = @LIB_TIFF@ @LIB_JPEG@ @LIB_XEXT@ @LIB_XT@ \
  @LIB_X11@ @LIB_BZLIB@ @LIB_ZLIB@ @LIB_SFIO@ @LIB_THREAD@ @LIB_LF@ \
  @LIB_MATH@ $(top_builddir)/ltdl/libltdlc.la
else
libMagick_la_SOURCES = $(MAGICK_BASE_SRCS) $(MAGICK_CODER_SRCS)
libMagick_la_LIBADD = @LIB_JBIG@ @LIB_HDF@ @LIB_TIFF@ @LIB_TTF@ @LIB_JPEG@ \
  @LIB_PNG@ @LIB_FPX@ @LIB_DPS@ @LIB_XEXT@ @LIB_XT@ \
  @LIB_X11@ @LIB_BZLIB@ @LIB_ZLIB@ @LIB_SFIO@ @LIB_THREAD@ @LIB_LF@ @LIB_MATH@
endif # WITH_MODULES

# Binary scripts
bin_SCRIPTS = Magick-config

# Headers which are installed
include_HEADERS = PreRvIcccm.h api.h attributes.h blob.h cache_view.h \
  classify.h compress.h config.h delegates.h error.h gems.h image.h magick.h \
  memory.h monitor.h quantize.h stash.h timer.h utility.h version.h widget.h \
  xwindows.h

# Headers which are not installed but which are distributed
noinst_HEADERS = animate.h stash.h defines.h display.h mac.h mng.h modules.h\
  nt.h vms.h

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES) Imakefile Magick-config.in Make.com mac.c \
  modules.mgk nt.c vms.c $(MAGICK_BASE_SRCS)

# Architecture-independent data files
pkgdata_DATA = modules.mgk

#
# Libtool Modules
#

# Where modules are installed
pkgdir            = $(libdir)/ImageMagick/modules/coders

# Modules which are to be built
if WITH_MODULES
pkg_LTLIBRARIES   = avs.la bmp.la cmyk.la dcm.la dps.la ept.la fax.la fpx.la \
  fits.la gif.la gray.la gradation.la hdf.la histogram.la html.la icc.la \
  icon.la iptc.la jbig.la jpeg.la label.la logo.la map.la matte.la miff.la \
  mono.la mtv.la pcd.la pcl.la pcx.la pdb.la pdf.la pict.la png.la pix.la \
  plasma.la pnm.la preview.la ps.la ps2.la ps3.la psd.la pwp.la rgb.la \
  rla.la rle.la sct.la sfw.la sgi.la stegano.la sun.la svg.la tga.la \
  tiff.la tile.la tim.la ttf.la txt.la uil.la uyvy.la vicar.la vid.la \
  viff.la wbmp.la x.la xbm.la xc.la xpm.la xwd.la yuv.la bim.la
else
pkg_LTLIBRARIES   =
endif # WITH_MODULES

# AVS module
avs_la_SOURCES      = avs.c
avs_la_LDFLAGS      = -no-undefined -module -avoid-version
avs_la_LIBADD       = libMagick.la

# BIM module
bim_la_SOURCES     = bim.c
bim_la_LDFLAGS     = -no-undefined -module -avoid-version
bim_la_LIBADD      = libMagick.la

# BMP module
bmp_la_SOURCES      = bmp.c
bmp_la_LDFLAGS      = -no-undefined -module -avoid-version
bmp_la_LIBADD       = libMagick.la

# CMYK module
cmyk_la_SOURCES     = cmyk.c
cmyk_la_LDFLAGS     = -no-undefined -module -avoid-version
cmyk_la_LIBADD      = libMagick.la

# DCM module
dcm_la_SOURCES      = dcm.c
dcm_la_LDFLAGS      = -no-undefined -module -avoid-version
dcm_la_LIBADD       = libMagick.la

# DPS module
dps_la_SOURCES      = dps.c
dps_la_LDFLAGS      = -no-undefined -module -avoid-version
dps_la_LIBADD       = libMagick.la @LIB_DPS@ @LIB_XEXT@ @LIB_XT@ @LIB_X11@

# EPT module
ept_la_SOURCES      = ept.c
ept_la_LDFLAGS      = -no-undefined -module -avoid-version
ept_la_LIBADD       = libMagick.la

# FAX module
fax_la_SOURCES      = fax.c
fax_la_LDFLAGS      = -no-undefined -module -avoid-version
fax_la_LIBADD       = libMagick.la

# FITS module
fits_la_SOURCES     = fits.c
fits_la_LDFLAGS     = -no-undefined -module -avoid-version
fits_la_LIBADD      = libMagick.la

# FPX module
fpx_la_SOURCES      = fpx.c
fpx_la_LDFLAGS      = -no-undefined -module -avoid-version
fpx_la_LIBADD       = libMagick.la @LIB_FPX@

# GIF module
gif_la_SOURCES      = gif.c
gif_la_LDFLAGS      = -no-undefined -module -avoid-version
gif_la_LIBADD       = libMagick.la

# GRAY module
gray_la_SOURCES     = gray.c
gray_la_LDFLAGS     = -no-undefined -module -avoid-version
gray_la_LIBADD      = libMagick.la

# GRADATION module
gradation_la_SOURCES = gradation.c
gradation_la_LDFLAGS = -no-undefined -module -avoid-version
gradation_la_LIBADD  = libMagick.la

# HDF module
hdf_la_SOURCES      = hdf.c
hdf_la_LDFLAGS      = -no-undefined -module -avoid-version
hdf_la_LIBADD       = libMagick.la @LIB_HDF@ @LIB_JPEG@ @LIB_ZLIB@

# HISTOGRAM module
histogram_la_SOURCES = histogram.c
histogram_la_LDFLAGS = -no-undefined -module -avoid-version
histogram_la_LIBADD  = libMagick.la

# HTML module
html_la_SOURCES      = html.c
html_la_LDFLAGS      = -no-undefined -module -avoid-version
html_la_LIBADD       = libMagick.la

# ICC module
icc_la_SOURCES      = icc.c
icc_la_LDFLAGS      = -no-undefined -module -avoid-version
icc_la_LIBADD       = libMagick.la

# ICON module
icon_la_SOURCES     = icon.c
icon_la_LDFLAGS     = -no-undefined -module -avoid-version
icon_la_LIBADD      = libMagick.la

# IPTC module
iptc_la_SOURCES     = iptc.c
iptc_la_LDFLAGS     = -no-undefined -module -avoid-version
iptc_la_LIBADD      = libMagick.la

# JBIG module
jbig_la_SOURCES     = jbig.c
jbig_la_LDFLAGS     = -no-undefined -module -avoid-version
jbig_la_LIBADD      = libMagick.la @LIB_JBIG@

# JPEG module
jpeg_la_SOURCES     = jpeg.c
jpeg_la_LDFLAGS     = -no-undefined -module -avoid-version
jpeg_la_LIBADD      = libMagick.la @LIB_JPEG@

# LABEL module
label_la_SOURCES     = label.c
label_la_LDFLAGS     = -no-undefined -module -avoid-version
label_la_LIBADD      = libMagick.la @LIB_TTF@ @LIB_X11@

# LOGO module
logo_la_SOURCES     = logo.c
logo_la_LDFLAGS     = -no-undefined -module -avoid-version
logo_la_LIBADD      = libMagick.la

# MAP module
map_la_SOURCES      = map.c
map_la_LDFLAGS      = -no-undefined -module -avoid-version
map_la_LIBADD       = libMagick.la

# MATTE module
matte_la_SOURCES    = matte.c
matte_la_LDFLAGS    = -no-undefined -module -avoid-version
matte_la_LIBADD     = libMagick.la

# MIFF module
miff_la_SOURCES     = miff.c
miff_la_LDFLAGS     = -no-undefined -module -avoid-version
miff_la_LIBADD      = libMagick.la

# MONO module
mono_la_SOURCES     = mono.c
mono_la_LDFLAGS     = -no-undefined -module -avoid-version
mono_la_LIBADD      = libMagick.la

# MTV module
mtv_la_SOURCES      = mtv.c
mtv_la_LDFLAGS      = -no-undefined -module -avoid-version
mtv_la_LIBADD       = libMagick.la

# PCD module
pcd_la_SOURCES      = pcd.c
pcd_la_LDFLAGS      = -no-undefined -module -avoid-version
pcd_la_LIBADD       = libMagick.la

# PCL module
pcl_la_SOURCES      = pcl.c
pcl_la_LDFLAGS      = -no-undefined -module -avoid-version
pcl_la_LIBADD       = libMagick.la

# PCX module
pcx_la_SOURCES      = pcx.c
pcx_la_LDFLAGS      = -no-undefined -module -avoid-version
pcx_la_LIBADD       = libMagick.la

# PDB module
pdb_la_SOURCES      = pdb.c
pdb_la_LDFLAGS      = -no-undefined -module -avoid-version
pdb_la_LIBADD       = libMagick.la

# PDF module
pdf_la_SOURCES      = pdf.c
pdf_la_LDFLAGS      = -no-undefined -module -avoid-version
pdf_la_LIBADD       = libMagick.la

# PICT module
pict_la_SOURCES     = pict.c
pict_la_LDFLAGS     = -no-undefined -module -avoid-version
pict_la_LIBADD      = libMagick.la

# PIX module
pix_la_SOURCES      = pix.c
pix_la_LDFLAGS      = -no-undefined -module -avoid-version
pix_la_LIBADD       = libMagick.la

# PNG module
png_la_SOURCES      = png.c
png_la_LDFLAGS      = -no-undefined -module -avoid-version
png_la_LIBADD       = libMagick.la @LIB_PNG@ @LIB_ZLIB@

# PLASMA module
plasma_la_SOURCES   = plasma.c
plasma_la_LDFLAGS   = -no-undefined -module -avoid-version
plasma_la_LIBADD    = libMagick.la

# PNM module
pnm_la_SOURCES      = pnm.c
pnm_la_LDFLAGS      = -no-undefined -module -avoid-version
pnm_la_LIBADD       = libMagick.la

# PREVIEW module
preview_la_SOURCES  = preview.c
preview_la_LDFLAGS  = -no-undefined -module -avoid-version
preview_la_LIBADD   = libMagick.la

# PS module
ps_la_SOURCES       = ps.c
ps_la_LDFLAGS       = -no-undefined -module -avoid-version
ps_la_LIBADD        = libMagick.la

# PS2 module
ps2_la_SOURCES      = ps2.c
ps2_la_LDFLAGS      = -no-undefined -module -avoid-version
ps2_la_LIBADD       = libMagick.la

# PS3 module
ps3_la_SOURCES      = ps3.c
ps3_la_LDFLAGS      = -no-undefined -module -avoid-version
ps3_la_LIBADD       = libMagick.la

# PSD module
psd_la_SOURCES      = psd.c
psd_la_LDFLAGS      = -no-undefined -module -avoid-version
psd_la_LIBADD       = libMagick.la

# PWP module
pwp_la_SOURCES      = pwp.c
pwp_la_LDFLAGS      = -no-undefined -module -avoid-version
pwp_la_LIBADD       = libMagick.la

# RGB module
rgb_la_SOURCES      = rgb.c
rgb_la_LDFLAGS      = -no-undefined -module -avoid-version
rgb_la_LIBADD       = libMagick.la

# RLA module
rla_la_SOURCES      = rla.c
rla_la_LDFLAGS      = -no-undefined -module -avoid-version
rla_la_LIBADD       = libMagick.la

# RLE module
rle_la_SOURCES      = rle.c
rle_la_LDFLAGS      = -no-undefined -module -avoid-version
rle_la_LIBADD       = libMagick.la

# SCT module
sct_la_SOURCES      = sct.c
sct_la_LDFLAGS      = -no-undefined -module -avoid-version
sct_la_LIBADD       = libMagick.la

# SFW module
sfw_la_SOURCES      = sfw.c
sfw_la_LDFLAGS      = -no-undefined -module -avoid-version
sfw_la_LIBADD       = libMagick.la

# SGI module
sgi_la_SOURCES      = sgi.c
sgi_la_LDFLAGS      = -no-undefined -module -avoid-version
sgi_la_LIBADD       = libMagick.la

# STEGANO module
stegano_la_SOURCES  = stegano.c
stegano_la_LDFLAGS  = -no-undefined -module -avoid-version
stegano_la_LIBADD   = libMagick.la

# SUN module
sun_la_SOURCES      = sun.c
sun_la_LDFLAGS      = -no-undefined -module -avoid-version
sun_la_LIBADD       = libMagick.la

# SVG module
svg_la_SOURCES      = svg.c
svg_la_LDFLAGS      = -no-undefined -module -avoid-version
svg_la_LIBADD       = libMagick.la

# TGA module
tga_la_SOURCES      = tga.c
tga_la_LDFLAGS      = -no-undefined -module -avoid-version
tga_la_LIBADD       = libMagick.la

# TIFF module
tiff_la_SOURCES     = tiff.c
tiff_la_LDFLAGS     = -no-undefined -module -avoid-version
tiff_la_LIBADD      = libMagick.la @LIB_TIFF@ @LIB_ZLIB@

# TILE module
tile_la_SOURCES     = tile.c
tile_la_LDFLAGS     = -no-undefined -module -avoid-version
tile_la_LIBADD      = libMagick.la

# TIM module
tim_la_SOURCES      = tim.c
tim_la_LDFLAGS      = -no-undefined -module -avoid-version
tim_la_LIBADD       = libMagick.la

# TTF module
ttf_la_SOURCES      = ttf.c
ttf_la_LDFLAGS      = -no-undefined -module -avoid-version
ttf_la_LIBADD       = libMagick.la @LIB_TTF@

# TXT module
txt_la_SOURCES      = txt.c
txt_la_LDFLAGS      = -no-undefined -module -avoid-version
txt_la_LIBADD       = libMagick.la

# UIL module
uil_la_SOURCES      = uil.c
uil_la_LDFLAGS      = -no-undefined -module -avoid-version
uil_la_LIBADD       = libMagick.la

# UYVY module
uyvy_la_SOURCES     = uyvy.c
uyvy_la_LDFLAGS     = -no-undefined -module -avoid-version
uyvy_la_LIBADD      = libMagick.la

# VICAR module
vicar_la_SOURCES    = vicar.c
vicar_la_LDFLAGS    = -no-undefined -module -avoid-version
vicar_la_LIBADD     = libMagick.la

# VID module
vid_la_SOURCES      = vid.c
vid_la_LDFLAGS      = -no-undefined -module -avoid-version
vid_la_LIBADD       = libMagick.la

# VIFF module
viff_la_SOURCES     = viff.c
viff_la_LDFLAGS     = -no-undefined -module -avoid-version
viff_la_LIBADD      = libMagick.la

# WBMP module
wbmp_la_SOURCES     = wbmp.c
wbmp_la_LDFLAGS     = -no-undefined -module -avoid-version
wbmp_la_LIBADD      = libMagick.la

# X module
x_la_SOURCES        = x.c
x_la_LDFLAGS        = -no-undefined -module -avoid-version
x_la_LIBADD         = libMagick.la @LIB_X11@

# XBM module
xbm_la_SOURCES      = xbm.c
xbm_la_LDFLAGS      = -no-undefined -module -avoid-version
xbm_la_LIBADD       = libMagick.la

# XC module
xc_la_SOURCES       = xc.c
xc_la_LDFLAGS       = -no-undefined -module -avoid-version
xc_la_LIBADD        = libMagick.la

# XPM module
xpm_la_SOURCES      = xpm.c
xpm_la_LDFLAGS      = -no-undefined -module -avoid-version
xpm_la_LIBADD       = libMagick.la

# XWD module
xwd_la_SOURCES      = xwd.c
xwd_la_LDFLAGS      = -no-undefined -module -avoid-version
xwd_la_LIBADD       = libMagick.la @LIB_X11@

# YUV module
yuv_la_SOURCES      = yuv.c
yuv_la_LDFLAGS      = -no-undefined -module -avoid-version
yuv_la_LIBADD       = libMagick.la

# Ensure that configuration header is not distributed
dist-hook:
	-rm $(distdir)/config.h

DEFS = @DEFS@ -I../
