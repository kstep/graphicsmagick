#  Makefile for building ImageMagick coder modules
#
# Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated
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
AUTOMAKE_OPTIONS = 1.5 foreign

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/magick

MAGICK_CODER_SRCS = \
  art.c avi.c avs.c bmp.c caption.c cmyk.c cut.c dcm.c dib.c dps.c dpx.c \
  ept.c fax.c fits.c fpx.c gif.c gradient.c gray.c hdf.c \
  histogram.c html.c icon.c jbig.c jp2.c jpeg.c label.c logo.c \
  map.c mat.c matte.c meta.c miff.c mono.c mpc.c mpeg.c mpr.c msl.c \
  mtv.c mvg.c null.c otb.c palm.c pcd.c pcl.c pcx.c pdb.c pdf.c pict.c \
  pix.c plasma.c png.c pnm.c preview.c ps.c ps2.c ps3.c psd.c \
  pwp.c rgb.c rla.c rle.c sct.c sfw.c sgi.c stegano.c sun.c svg.c \
  tga.c tiff.c tile.c tim.c ttf.c txt.c uil.c url.c uyvy.c vicar.c \
  vid.c viff.c wbmp.c wmf.c wpg.c x.c xbm.c xc.c xcf.c xpm.c xwd.c \
  yuv.c

MAJOR_VERSION=@MAJOR_VERSION@
MINOR_VERSION=@MINOR_VERSION@
MICRO_VERSION=@MICRO_VERSION@
if WITH_MODULES
noinst_LTLIBRARIES =
else
noinst_LTLIBRARIES = libCoders.la
libCoders_la_SOURCES = $(MAGICK_CODER_SRCS)
endif # WITH_MODULES

# Additional files to distribute
EXTRA_DIST = modules.mgk delegates.mgk.in type.mgk.in

# Headers which are not installed but which are distributed
noinst_HEADERS = 

# Location of libMagick.la
LIBMAGICK = ../magick/libMagick.la

if WITH_MODULES
# Where modules are installed
pkgdir = $(libdir)/ImageMagick/modules/coders

# Architecture-dependent data files installed with modules
pkg_DATA = modules.mgk

# Architecture independent data files installed in the package's data directory
pkgdata_DATA = 

# Modules which are to be built
pkg_LTLIBRARIES = \
  art.la avi.la avs.la bmp.la caption.la cmyk.la cut.la dcm.la dib.la dps.la \
	dpx.la ept.la fax.la fits.la fpx.la gif.la gradient.la gray.la hdf.la \
  histogram.la html.la icon.la jbig.la jp2.la jpeg.la label.la logo.la \
  map.la mat.la matte.la meta.la miff.la mono.la mpc.la mpeg.la mpr.la msl.la \
  mtv.la mvg.la null.la otb.la palm.la pcd.la pcl.la pcx.la pdb.la pdf.la \
	pict.la pix.la plasma.la png.la pnm.la preview.la ps.la ps2.la ps3.la psd.la \
  pwp.la rgb.la rla.la rle.la sct.la sfw.la sgi.la stegano.la sun.la svg.la \
  tga.la tiff.la tile.la tim.la ttf.la txt.la uil.la url.la uyvy.la vicar.la \
  vid.la viff.la wbmp.la wmf.la wpg.la x.la xbm.la xc.la xcf.la xpm.la xwd.la \
  yuv.la
MODULECOMMONFLAGS = -no-undefined -module -avoid-version
else
pkg_LTLIBRARIES =
MODULECOMMONFLAGS     =
endif # WITH_MODULES

# ART module
art_la_SOURCES      = art.c
art_la_LDFLAGS      = $(MODULECOMMONFLAGS)
art_la_LIBADD       = $(LIBMAGICK)

# AVI module
avi_la_SOURCES      = avi.c
avi_la_LDFLAGS      = $(MODULECOMMONFLAGS)
avi_la_LIBADD       = $(LIBMAGICK)

# AVS module
avs_la_SOURCES      = avs.c
avs_la_LDFLAGS      = $(MODULECOMMONFLAGS)
avs_la_LIBADD       = $(LIBMAGICK)

# BMP module
bmp_la_SOURCES      = bmp.c
bmp_la_LDFLAGS      = $(MODULECOMMONFLAGS)
bmp_la_LIBADD       = $(LIBMAGICK)

# CAPTION module
caption_la_SOURCES     = caption.c
caption_la_LDFLAGS     = $(MODULECOMMONFLAGS)
caption_la_LIBADD      = $(LIBMAGICK)

# CMYK module
cmyk_la_SOURCES     = cmyk.c
cmyk_la_LDFLAGS     = $(MODULECOMMONFLAGS)
cmyk_la_LIBADD      = $(LIBMAGICK)

# CUT module
cut_la_SOURCES     = cut.c
cut_la_LDFLAGS     = $(MODULECOMMONFLAGS)
cut_la_LIBADD      = $(LIBMAGICK)

# DCM module
dcm_la_SOURCES      = dcm.c
dcm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
dcm_la_LIBADD       = $(LIBMAGICK)

# DIB module
dib_la_SOURCES      = dib.c
dib_la_LDFLAGS      = $(MODULECOMMONFLAGS)
dib_la_LIBADD       = $(LIBMAGICK)

# DPS module
dps_la_SOURCES      = dps.c
dps_la_LDFLAGS      = $(MODULECOMMONFLAGS)
dps_la_LIBADD       = $(LIBMAGICK) @LIB_DPS@ @LIB_XEXT@ @LIB_XT@ @LIB_X11@

# DPX module
dpx_la_SOURCES      = dpx.c
dpx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
dpx_la_LIBADD       = $(LIBMAGICK) 

# EPT module
ept_la_SOURCES      = ept.c
ept_la_LDFLAGS      = $(MODULECOMMONFLAGS)
ept_la_LIBADD       = $(LIBMAGICK) @LIB_GS@ $(LIB_MATH)

# FAX module
fax_la_SOURCES      = fax.c
fax_la_LDFLAGS      = $(MODULECOMMONFLAGS)
fax_la_LIBADD       = $(LIBMAGICK)

# FITS module
fits_la_SOURCES     = fits.c
fits_la_LDFLAGS     = $(MODULECOMMONFLAGS)
fits_la_LIBADD      = $(LIBMAGICK)

# FPX module
fpx_la_SOURCES      = fpx.c
fpx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
fpx_la_LIBADD       = $(LIBMAGICK) @LIB_FPX@

# GIF module
gif_la_SOURCES      = gif.c
gif_la_LDFLAGS      = $(MODULECOMMONFLAGS)
gif_la_LIBADD       = $(LIBMAGICK)

# GRAY module
gray_la_SOURCES     = gray.c
gray_la_LDFLAGS     = $(MODULECOMMONFLAGS)
gray_la_LIBADD      = $(LIBMAGICK)

# GRADIENT module
gradient_la_SOURCES = gradient.c
gradient_la_LDFLAGS = $(MODULECOMMONFLAGS)
gradient_la_LIBADD  = $(LIBMAGICK)

# HDF module
hdf_la_SOURCES      = hdf.c
hdf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
hdf_la_LIBADD       = $(LIBMAGICK) @LIB_HDF@ @LIB_JPEG@ @LIB_ZLIB@

# HISTOGRAM module
histogram_la_SOURCES = histogram.c
histogram_la_LDFLAGS = $(MODULECOMMONFLAGS)
histogram_la_LIBADD  = $(LIBMAGICK)

# HTML module
html_la_SOURCES     = html.c
html_la_LDFLAGS     = $(MODULECOMMONFLAGS)
html_la_LIBADD      = $(LIBMAGICK)

# ICON module
icon_la_SOURCES     = icon.c
icon_la_LDFLAGS     = $(MODULECOMMONFLAGS)
icon_la_LIBADD      = $(LIBMAGICK)

# JBIG module
jbig_la_SOURCES     = jbig.c
jbig_la_LDFLAGS     = $(MODULECOMMONFLAGS)
jbig_la_LIBADD      = $(LIBMAGICK) @LIB_JBIG@

# JPEG module
jpeg_la_SOURCES     = jpeg.c
jpeg_la_LDFLAGS     = $(MODULECOMMONFLAGS)
jpeg_la_LIBADD      = $(LIBMAGICK) @LIB_JPEG@

# JPEG 2000 module
jp2_la_SOURCES     = jp2.c
jp2_la_LDFLAGS     = $(MODULECOMMONFLAGS)
jp2_la_LIBADD      = $(LIBMAGICK) @LIB_JP2@  @LIB_JPEG@ $(LIB_MATH)

# LABEL module
label_la_SOURCES     = label.c
label_la_LDFLAGS     = $(MODULECOMMONFLAGS)
label_la_LIBADD      = $(LIBMAGICK)

# LOGO module
logo_la_SOURCES     = logo.c
logo_la_LDFLAGS     = $(MODULECOMMONFLAGS)
logo_la_LIBADD      = $(LIBMAGICK)

# MAP module
map_la_SOURCES      = map.c
map_la_LDFLAGS      = $(MODULECOMMONFLAGS)
map_la_LIBADD       = $(LIBMAGICK)

# MAT module
mat_la_SOURCES    = mat.c
mat_la_LDFLAGS    = $(MODULECOMMONFLAGS)
mat_la_LIBADD     = $(LIBMAGICK)

# MATTE module
matte_la_SOURCES    = matte.c
matte_la_LDFLAGS    = $(MODULECOMMONFLAGS)
matte_la_LIBADD     = $(LIBMAGICK)

# META module
meta_la_SOURCES     = meta.c
meta_la_LDFLAGS     = $(MODULECOMMONFLAGS)
meta_la_LIBADD      = $(LIBMAGICK)

# MIFF module
miff_la_SOURCES     = miff.c
miff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
miff_la_LIBADD      = $(LIBMAGICK) @LIB_ZLIB@  @LIB_BZLIB@ @LIB_JPEG@

# MONO module
mono_la_SOURCES     = mono.c
mono_la_LDFLAGS     = $(MODULECOMMONFLAGS)
mono_la_LIBADD      = $(LIBMAGICK)

# MPC module
mpc_la_SOURCES    = mpc.c
mpc_la_LDFLAGS    = $(MODULECOMMONFLAGS)
mpc_la_LIBADD     = $(LIBMAGICK)

# MPEG module
mpeg_la_SOURCES    = mpeg.c
mpeg_la_LDFLAGS    = $(MODULECOMMONFLAGS)
mpeg_la_LIBADD     = $(LIBMAGICK) @LIB_MPEG2@

# MPR module
mpr_la_SOURCES      = mpr.c
mpr_la_LDFLAGS      = $(MODULECOMMONFLAGS)
mpr_la_LIBADD       = $(LIBMAGICK) @LIB_XML@ @LIB_ZLIB@ @LIB_X11@ $(LIB_MATH)

# MSL module
msl_la_SOURCES      = msl.c
msl_la_LDFLAGS      = $(MODULECOMMONFLAGS)
msl_la_LIBADD       = $(LIBMAGICK) @LIB_XML@ @LIB_ZLIB@ @LIB_X11@ $(LIB_MATH)

# MTV module
mtv_la_SOURCES      = mtv.c
mtv_la_LDFLAGS      = $(MODULECOMMONFLAGS)
mtv_la_LIBADD       = $(LIBMAGICK)

# MVG module
mvg_la_SOURCES      = mvg.c
mvg_la_LDFLAGS      = $(MODULECOMMONFLAGS)
mvg_la_LIBADD       = $(LIBMAGICK)

# NULL module
null_la_SOURCES     = null.c
null_la_LDFLAGS     = $(MODULECOMMONFLAGS)
null_la_LIBADD      = $(LIBMAGICK)

# OTB module
otb_la_SOURCES      = otb.c
otb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
otb_la_LIBADD       = $(LIBMAGICK)

# PALM module
palm_la_SOURCES     = palm.c
palm_la_LDFLAGS     = $(MODULECOMMONFLAGS)
palm_la_LIBADD      = $(LIBMAGICK)

# PCD module
pcd_la_SOURCES      = pcd.c
pcd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pcd_la_LIBADD       = $(LIBMAGICK)

# PCL module
pcl_la_SOURCES      = pcl.c
pcl_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pcl_la_LIBADD       = $(LIBMAGICK)

# PCX module
pcx_la_SOURCES      = pcx.c
pcx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pcx_la_LIBADD       = $(LIBMAGICK)

# PDB module
pdb_la_SOURCES      = pdb.c
pdb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pdb_la_LIBADD       = $(LIBMAGICK)

# PDF module
pdf_la_SOURCES      = pdf.c
pdf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pdf_la_LIBADD       = $(LIBMAGICK) @LIB_TIFF@ @LIB_JPEG@ @LIB_ZLIB@ @LIB_GS@ $(LIB_MATH)

# PICT module
pict_la_SOURCES     = pict.c
pict_la_LDFLAGS     = $(MODULECOMMONFLAGS)
pict_la_LIBADD      = $(LIBMAGICK)

# PIX module
pix_la_SOURCES      = pix.c
pix_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pix_la_LIBADD       = $(LIBMAGICK)

# PNG module
png_la_SOURCES      = png.c
png_la_LDFLAGS      = $(MODULECOMMONFLAGS)
png_la_LIBADD       = $(LIBMAGICK) @LIB_PNG@ @LIB_ZLIB@ $(LIB_MATH)

# PLASMA module
plasma_la_SOURCES   = plasma.c
plasma_la_LDFLAGS   = $(MODULECOMMONFLAGS)
plasma_la_LIBADD    = $(LIBMAGICK)

# PNM module
pnm_la_SOURCES      = pnm.c
pnm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pnm_la_LIBADD       = $(LIBMAGICK)

# PREVIEW module
preview_la_SOURCES  = preview.c
preview_la_LDFLAGS  = $(MODULECOMMONFLAGS)
preview_la_LIBADD   = $(LIBMAGICK)

# PS module
ps_la_SOURCES       = ps.c
ps_la_LDFLAGS       = $(MODULECOMMONFLAGS)
ps_la_LIBADD        = $(LIBMAGICK) @LIB_GS@ $(LIB_MATH)

# PS2 module
ps2_la_SOURCES      = ps2.c
ps2_la_LDFLAGS      = $(MODULECOMMONFLAGS)
ps2_la_LIBADD       = $(LIBMAGICK) @LIB_TIFF@ @LIB_JPEG@ @LIB_ZLIB@ $(LIB_MATH)

# PS3 module
ps3_la_SOURCES      = ps3.c
ps3_la_LDFLAGS      = $(MODULECOMMONFLAGS)
ps3_la_LIBADD       = $(LIBMAGICK) @LIB_TIFF@ @LIB_JPEG@ @LIB_ZLIB@ $(LIB_MATH)

# PSD module
psd_la_SOURCES      = psd.c
psd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
psd_la_LIBADD       = $(LIBMAGICK)

# PWP module
pwp_la_SOURCES      = pwp.c
pwp_la_LDFLAGS      = $(MODULECOMMONFLAGS)
pwp_la_LIBADD       = $(LIBMAGICK)

# RGB module
rgb_la_SOURCES      = rgb.c
rgb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
rgb_la_LIBADD       = $(LIBMAGICK)

# RLA module
rla_la_SOURCES      = rla.c
rla_la_LDFLAGS      = $(MODULECOMMONFLAGS)
rla_la_LIBADD       = $(LIBMAGICK)

# RLE module
rle_la_SOURCES      = rle.c
rle_la_LDFLAGS      = $(MODULECOMMONFLAGS)
rle_la_LIBADD       = $(LIBMAGICK)

# SCT module
sct_la_SOURCES      = sct.c
sct_la_LDFLAGS      = $(MODULECOMMONFLAGS)
sct_la_LIBADD       = $(LIBMAGICK)

# SFW module
sfw_la_SOURCES      = sfw.c
sfw_la_LDFLAGS      = $(MODULECOMMONFLAGS)
sfw_la_LIBADD       = $(LIBMAGICK)

# SGI module
sgi_la_SOURCES      = sgi.c
sgi_la_LDFLAGS      = $(MODULECOMMONFLAGS)
sgi_la_LIBADD       = $(LIBMAGICK)

# STEGANO module
stegano_la_SOURCES  = stegano.c
stegano_la_LDFLAGS  = $(MODULECOMMONFLAGS)
stegano_la_LIBADD   = $(LIBMAGICK)

# SUN module
sun_la_SOURCES      = sun.c
sun_la_LDFLAGS      = $(MODULECOMMONFLAGS)
sun_la_LIBADD       = $(LIBMAGICK)

# SVG module
svg_la_SOURCES      = svg.c
svg_la_LDFLAGS      = $(MODULECOMMONFLAGS)
svg_la_LIBADD       = $(LIBMAGICK) @LIB_XML@ @LIB_ZLIB@ @LIB_X11@ $(LIB_MATH)

# TGA module
tga_la_SOURCES      = tga.c
tga_la_LDFLAGS      = $(MODULECOMMONFLAGS)
tga_la_LIBADD       = $(LIBMAGICK)

# TIFF module
tiff_la_SOURCES     = tiff.c
tiff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
tiff_la_LIBADD      = $(LIBMAGICK) @LIB_TIFF@ @LIB_JPEG@ @LIB_ZLIB@

# TILE module
tile_la_SOURCES     = tile.c
tile_la_LDFLAGS     = $(MODULECOMMONFLAGS)
tile_la_LIBADD      = $(LIBMAGICK)

# TIM module
tim_la_SOURCES      = tim.c
tim_la_LDFLAGS      = $(MODULECOMMONFLAGS)
tim_la_LIBADD       = $(LIBMAGICK)

# TTF module
ttf_la_SOURCES      = ttf.c
ttf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
ttf_la_LIBADD       = $(LIBMAGICK)

# TXT module
txt_la_SOURCES      = txt.c
txt_la_LDFLAGS      = $(MODULECOMMONFLAGS)
txt_la_LIBADD       = $(LIBMAGICK) $(LIB_MATH)

# UIL module
uil_la_SOURCES      = uil.c
uil_la_LDFLAGS      = $(MODULECOMMONFLAGS)
uil_la_LIBADD       = $(LIBMAGICK)

# URL module
url_la_SOURCES      = url.c
url_la_LDFLAGS      = $(MODULECOMMONFLAGS)
url_la_LIBADD       = $(LIBMAGICK) @LIB_XML@ @LIB_ZLIB@ @LIB_X11@ $(LIB_MATH)

# UYVY module
uyvy_la_SOURCES     = uyvy.c
uyvy_la_LDFLAGS     = $(MODULECOMMONFLAGS)
uyvy_la_LIBADD      = $(LIBMAGICK)

# VICAR module
vicar_la_SOURCES    = vicar.c
vicar_la_LDFLAGS    = $(MODULECOMMONFLAGS)
vicar_la_LIBADD     = $(LIBMAGICK)

# VID module
vid_la_SOURCES      = vid.c
vid_la_LDFLAGS      = $(MODULECOMMONFLAGS)
vid_la_LIBADD       = $(LIBMAGICK)

# VIFF module
viff_la_SOURCES     = viff.c
viff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
viff_la_LIBADD      = $(LIBMAGICK)

# WBMP module
wbmp_la_SOURCES     = wbmp.c
wbmp_la_LDFLAGS     = $(MODULECOMMONFLAGS)
wbmp_la_LIBADD      = $(LIBMAGICK)

# WMF module
wmf_la_SOURCES      = wmf.c
wmf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
wmf_la_LIBADD       = $(LIBMAGICK) @LIB_WMF@ @LIB_WMF_DEPS@ $(LIB_MATH)

# WPG module
wpg_la_SOURCES     = wpg.c
wpg_la_LDFLAGS     = $(MODULECOMMONFLAGS)
wpg_la_LIBADD      = $(LIBMAGICK)

# X module
x_la_SOURCES        = x.c
x_la_LDFLAGS        = $(MODULECOMMONFLAGS)
x_la_LIBADD         = $(LIBMAGICK) @LIB_X11@

# XBM module
xbm_la_SOURCES      = xbm.c
xbm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
xbm_la_LIBADD       = $(LIBMAGICK)

# XC module
xc_la_SOURCES       = xc.c
xc_la_LDFLAGS       = $(MODULECOMMONFLAGS)
xc_la_LIBADD        = $(LIBMAGICK)

# XCF module
xcf_la_SOURCES      = xcf.c
xcf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
xcf_la_LIBADD       = $(LIBMAGICK)

# XPM module
xpm_la_SOURCES      = xpm.c
xpm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
xpm_la_LIBADD       = $(LIBMAGICK)

# XWD module
xwd_la_SOURCES      = xwd.c
xwd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
xwd_la_LIBADD       = $(LIBMAGICK) @LIB_X11@

# YUV module
yuv_la_SOURCES      = yuv.c
yuv_la_LDFLAGS      = $(MODULECOMMONFLAGS)
yuv_la_LIBADD       = $(LIBMAGICK)

DEFS = @DEFS@ -I../

# Install delegates files
DATADELEGATES = \
	delegates.mgk \
	type-ghostscript.mgk \
	type-windows.mgk \
	type-solaris.mgk \
	type.mgk

install-data-delegates: 
	$(mkinstalldirs) $(DESTDIR)$(pkglibdir)
	for file in $(DATADELEGATES) ; do  \
	  $(INSTALL_DATA) $$file $(DESTDIR)$(pkglibdir) ; \
	done

# Uninstall delegates files
uninstall-data-delegates:
	for file in $(DATADELEGATES) ; do \
	  rm -f $(DESTDIR)$(pkglibdir)/$$file ; \
	done


install-exec-local: install-data-delegates

uninstall-local: uninstall-data-delegates
