# Copyright (C) 2004-2010 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick coder modules
#
#

# Where coder modules get installed
codersdir = $(MagickCoderModulesPath)

if HasDPS
MAGICK_DPS_MODULES = coders/dps.la
MAGICK_DPS_SRCS = coders/dps.c
endif

if HasFPX
MAGICK_FPX_MODULES = coders/fpx.la
MAGICK_FPX_SRCS = coders/fpx.c
endif

if HasWINGDI32
MAGICK_GDI32_MODULES = coders/clipboard.la coders/emf.la
MAGICK_GDI32_SRCS = coders/clipboard.c coders/emf.c
endif

if HasJBIG
MAGICK_JBIG_MODULES = coders/jbig.la
MAGICK_JBIG_SRCS = coders/jbig.c
endif

if HasJPEG
MAGICK_JPEG_MODULES = coders/jnx.la coders/jpeg.la
MAGICK_JPEG_SRCS = coders/jnx.c coders/jpeg.c
endif

if HasJP2
MAGICK_JP2_MODULES = coders/jp2.la
MAGICK_JP2_SRCS = coders/jp2.c
endif

if HasPNG
MAGICK_PNG_MODULES = coders/png.la
MAGICK_PNG_SRCS = coders/png.c
endif

if HasTIFF
MAGICK_TIFF_MODULES = coders/ept.la coders/tiff.la
MAGICK_TIFF_SRCS = coders/ept.c coders/tiff.c
endif

if HasX11
MAGICK_X11_CODER_MODULES = coders/x.la coders/xwd.la
MAGICK_X11_CODER_SRCS = coders/x.c coders/xwd.c
endif

if HasWEBP
MAGICK_WEBP_MODULES = coders/webp.la
MAGICK_WEBP_SRCS = coders/webp.c
endif

MAGICK_CODER_CPPFLAGS = \
	$(MODULECOMMONCPPFLAGS) \
	$(MODULE_EXTRA_CPPFLAGS)

MAGICK_CODER_SRCS = \
	$(MAGICK_PNG_SRCS) \
	coders/art.c \
	coders/avs.c \
	coders/bmp.c \
	coders/cals.c \
	coders/caption.c \
	coders/cineon.c \
	coders/cmyk.c \
	coders/cut.c \
	coders/dcm.c \
	coders/dcraw.c \
	coders/dib.c \
	coders/dpx.c \
	coders/fax.c \
	coders/fits.c \
	coders/gif.c \
	coders/gradient.c \
	coders/gray.c \
	coders/histogram.c \
	coders/hrz.c \
	coders/html.c \
	coders/icon.c \
	coders/identity.c \
	coders/info.c \
	coders/label.c \
	coders/locale.c \
	coders/logo.c \
	coders/mac.c \
	coders/map.c \
	coders/mat.c \
	coders/matte.c \
	coders/meta.c \
	coders/miff.c \
	coders/mono.c \
	coders/mpc.c \
	coders/mpeg.c \
	coders/mpr.c \
	coders/msl.c \
	coders/mtv.c \
	coders/mvg.c \
	coders/null.c \
	coders/otb.c \
	coders/palm.c \
	coders/pcd.c \
	coders/pcl.c \
	coders/pcx.c \
	coders/pdb.c \
	coders/pdf.c \
	coders/pict.c \
	coders/pix.c \
	coders/plasma.c \
	coders/pnm.c \
	coders/preview.c \
	coders/ps.c \
	coders/ps2.c \
	coders/ps3.c \
	coders/psd.c \
	coders/pwp.c \
	coders/rgb.c \
	coders/rla.c \
	coders/rle.c \
	coders/sct.c \
	coders/sfw.c \
	coders/sgi.c \
	coders/stegano.c \
	coders/sun.c \
	coders/svg.c \
	coders/tga.c \
	coders/tile.c \
	coders/tim.c \
	coders/topol.c \
	coders/ttf.c \
	coders/txt.c \
	coders/uil.c \
	coders/url.c \
	coders/uyvy.c \
	coders/vicar.c \
	coders/vid.c \
	coders/viff.c \
	coders/wbmp.c \
	coders/wmf.c \
	coders/wpg.c \
	coders/xbm.c \
	coders/xc.c \
	coders/xcf.c \
	coders/xpm.c \
	coders/yuv.c \
	$(MAGICK_DPS_SRCS) \
	$(MAGICK_GDI32_SRCS) \
	$(MAGICK_FPX_SRCS) \
	$(MAGICK_JBIG_SRCS) \
	$(MAGICK_JPEG_SRCS) \
	$(MAGICK_JP2_SRCS) \
	$(MAGICK_TIFF_SRCS) \
	$(MAGICK_X11_CODER_SRCS) \
	$(MAGICK_WEBP_SRCS)

if WITH_MODULES
coders_LTLIBRARIES = \
	coders/art.la \
	coders/avs.la \
	coders/bmp.la \
	coders/cals.la \
	coders/caption.la \
	coders/cineon.la \
	coders/cmyk.la \
	coders/cut.la \
	coders/dcm.la \
	coders/dcraw.la \
	coders/dib.la \
	coders/dpx.la \
	coders/fax.la \
	coders/fits.la \
	coders/gif.la \
	coders/gradient.la \
	coders/gray.la \
	coders/histogram.la \
	coders/hrz.la \
	coders/html.la \
	coders/icon.la \
	coders/identity.la \
	coders/info.la \
	coders/label.la \
	coders/locale.la \
	coders/logo.la \
	coders/mac.la \
	coders/map.la \
	coders/mat.la \
	coders/matte.la \
	coders/meta.la \
	coders/miff.la \
	coders/mono.la \
	coders/mpc.la \
	coders/mpeg.la \
	coders/mpr.la \
	coders/msl.la \
	coders/mtv.la \
	coders/mvg.la \
	coders/null.la \
	coders/otb.la \
	coders/palm.la \
	coders/pcd.la \
	coders/pcl.la \
	coders/pcx.la \
	coders/pdb.la \
	coders/pdf.la \
	coders/pict.la \
	coders/pix.la \
	coders/plasma.la \
	coders/pnm.la \
	coders/preview.la \
	coders/ps.la \
	coders/ps2.la \
	coders/ps3.la \
	coders/psd.la \
	coders/pwp.la \
	coders/rgb.la \
	coders/rla.la \
	coders/rle.la \
	coders/sct.la \
	coders/sfw.la \
	coders/sgi.la \
	coders/stegano.la \
	coders/sun.la \
	coders/svg.la \
	coders/tga.la \
	coders/tile.la \
	coders/tim.la \
	coders/topol.la \
	coders/ttf.la \
	coders/txt.la \
	coders/uil.la \
	coders/url.la \
	coders/uyvy.la \
	coders/vicar.la \
	coders/vid.la \
	coders/viff.la \
	coders/wbmp.la \
	coders/wmf.la \
	coders/wpg.la \
	coders/xbm.la \
	coders/xc.la \
	coders/xcf.la \
	coders/xpm.la \
	coders/yuv.la \
	$(MAGICK_DPS_MODULES) $(MAGICK_FPX_MODULES) $(MAGICK_GDI32_MODULES) \
	$(MAGICK_JBIG_MODULES) $(MAGICK_JPEG_MODULES) $(MAGICK_JP2_MODULES) \
	$(MAGICK_PNG_MODULES) $(MAGICK_TIFF_MODULES) $(MAGICK_X11_CODER_MODULES) \
	$(MAGICK_WEBP_MODULES)
else
coders_LTLIBRARIES =
endif # WITH_MODULES

# ART coder module
coders_art_la_SOURCES      = coders/art.c
coders_art_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_art_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_art_la_LIBADD       = $(LIBMAGICK)

# AVS coder module
coders_avs_la_SOURCES      = coders/avs.c
coders_avs_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_avs_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_avs_la_LIBADD       = $(LIBMAGICK)

# BMP coder module
coders_bmp_la_SOURCES      = coders/bmp.c
coders_bmp_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_bmp_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_bmp_la_LIBADD       = $(LIBMAGICK)

# CALS coder module
coders_cals_la_SOURCES     = coders/cals.c
coders_cals_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_cals_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_cals_la_LIBADD      = $(LIBMAGICK)

# CAPTION coder module
coders_caption_la_SOURCES  = coders/caption.c
coders_caption_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_caption_la_LDFLAGS  = $(MODULECOMMONFLAGS)
coders_caption_la_LIBADD   = $(LIBMAGICK)

# CINEON coder module
coders_cineon_la_SOURCES  = coders/cineon.c
coders_cineon_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_cineon_la_LDFLAGS  = $(MODULECOMMONFLAGS)
coders_cineon_la_LIBADD   = $(LIBMAGICK)

# CLIPBOARD coder module
coders_clipboard_la_SOURCES= coders/clipboard.c
coders_clipboard_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_clipboard_la_LDFLAGS= $(MODULECOMMONFLAGS)
coders_clipboard_la_LIBADD = $(LIBMAGICK) $(LIB_GDI32)

# CMYK coder module
coders_cmyk_la_SOURCES     = coders/cmyk.c
coders_cmyk_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_cmyk_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_cmyk_la_LIBADD      = $(LIBMAGICK)

# CUT coder module
coders_cut_la_SOURCES      = coders/cut.c
coders_cut_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_cut_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_cut_la_LIBADD       = $(LIBMAGICK)

# DCM coder module
coders_dcm_la_SOURCES      = coders/dcm.c
coders_dcm_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_dcm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_dcm_la_LIBADD       = $(LIBMAGICK)

# DCRAW coder module
coders_dcraw_la_SOURCES    = coders/dcraw.c
coders_dcraw_la_CPPFLAGS   = $(MAGICK_CODER_CPPFLAGS)
coders_dcraw_la_LDFLAGS    = $(MODULECOMMONFLAGS)
coders_dcraw_la_LIBADD     = $(LIBMAGICK)

# DIB coder module
coders_dib_la_SOURCES      = coders/dib.c
coders_dib_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_dib_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_dib_la_LIBADD       = $(LIBMAGICK)

# DPS coder module
coders_dps_la_SOURCES      = coders/dps.c
coders_dps_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_dps_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_dps_la_LIBADD       = $(LIBMAGICK) $(LIB_DPS) $(LIB_XEXT) $(LIB_X11)

# DPX coder module
coders_dpx_la_SOURCES      = coders/dpx.c
coders_dpx_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_dpx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_dpx_la_LIBADD       = $(LIBMAGICK) 

# EMF coder module
coders_emf_la_SOURCES      = coders/emf.c
coders_emf_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_emf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_emf_la_LIBADD       = $(LIBMAGICK) $(LIB_GDI32)

# EPT coder module
coders_ept_la_SOURCES      = coders/ept.c
coders_epd_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_ept_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_ept_la_LIBADD       = $(LIBMAGICK) $(LIB_GS) $(LIB_MATH)

# FAX coder module
coders_fax_la_SOURCES      = coders/fax.c
coders_fax_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_fax_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_fax_la_LIBADD       = $(LIBMAGICK)

# FITS coder module
coders_fits_la_SOURCES     = coders/fits.c
coders_fits_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_fits_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_fits_la_LIBADD      = $(LIBMAGICK) $(LIB_MATH)

# FPX coder module
coders_fpx_la_SOURCES      = coders/fpx.c
coders_fpx_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_fpx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_fpx_la_LIBADD       = $(LIBMAGICK) $(LIB_FPX)

# GIF coder module
coders_gif_la_SOURCES      = coders/gif.c
coders_gif_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_gif_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_gif_la_LIBADD       = $(LIBMAGICK)

# GRAY coder module
coders_gray_la_SOURCES     = coders/gray.c
coders_gray_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_gray_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_gray_la_LIBADD      = $(LIBMAGICK)

# GRADIENT coder module
coders_gradient_la_SOURCES = coders/gradient.c
coders_gradient_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_gradient_la_LDFLAGS = $(MODULECOMMONFLAGS)
coders_gradient_la_LIBADD  = $(LIBMAGICK)

# HISTOGRAM coder module
coders_histogram_la_SOURCES = coders/histogram.c
coders_histogram_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_histogram_la_LDFLAGS = $(MODULECOMMONFLAGS)
coders_histogram_la_LIBADD  = $(LIBMAGICK)

# HRZ coder module
coders_hrz_la_SOURCES       = coders/hrz.c
coders_hrz_la_CPPFLAGS      = $(MAGICK_CODER_CPPFLAGS)
coders_hrz_la_LDFLAGS       = $(MODULECOMMONFLAGS)
coders_hrz_la_LIBADD        = $(LIBMAGICK)

# HTML coder module
coders_html_la_SOURCES     = coders/html.c
coders_html_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_html_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_html_la_LIBADD      = $(LIBMAGICK)

# ICON coder module
coders_icon_la_SOURCES     = coders/icon.c
coders_icon_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_icon_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_icon_la_LIBADD      = $(LIBMAGICK)

# Identity coder module
coders_identity_la_SOURCES  = coders/identity.c
coders_identity_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_identity_la_LDFLAGS  = $(MODULECOMMONFLAGS)
coders_identity_la_LIBADD   = $(LIBMAGICK)

# Info coder module
coders_info_la_SOURCES     = coders/info.c
coders_info_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_info_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_info_la_LIBADD      = $(LIBMAGICK)

# JBIG coder module
coders_jbig_la_SOURCES     = coders/jbig.c
coders_jbig_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_jbig_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_jbig_la_LIBADD      = $(LIBMAGICK) $(LIB_JBIG)

# JNX coder module
coders_jnx_la_SOURCES      = coders/jnx.c
coders_jnx_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_jnx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_jnx_la_LIBADD       = $(LIBMAGICK) $(LIB_JPEG)

# JPEG coder module
coders_jpeg_la_SOURCES     = coders/jpeg.c
coders_jpeg_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_jpeg_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_jpeg_la_LIBADD      = $(LIBMAGICK) $(LIB_JPEG)

# JPEG 2000 coder module
coders_jp2_la_SOURCES      = coders/jp2.c
coders_jp2_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_jp2_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_jp2_la_LIBADD       = $(LIBMAGICK) $(LIB_JP2) $(LIB_JPEG) $(LIB_MATH)

# LABEL coder module
coders_label_la_SOURCES    = coders/label.c
coders_label_la_CPPFLAGS   = $(MAGICK_CODER_CPPFLAGS)
coders_label_la_LDFLAGS    = $(MODULECOMMONFLAGS) $(LIB_MATH)
coders_label_la_LIBADD     = $(LIBMAGICK)

# LOCALE coder module
coders_locale_la_SOURCES   = coders/locale.c
coders_locale_la_CPPFLAGS  = $(MAGICK_CODER_CPPFLAGS)
coders_locale_la_LDFLAGS   = $(MODULECOMMONFLAGS)
coders_locale_la_LIBADD    = $(LIBMAGICK)

# LOGO coder module
coders_logo_la_SOURCES     = coders/logo.c
coders_logo_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_logo_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_logo_la_LIBADD      = $(LIBMAGICK)

# MAC coder module
coders_mac_la_SOURCES      = coders/mac.c
coders_mac_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mac_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mac_la_LIBADD       = $(LIBMAGICK)

# MAP coder module
coders_map_la_SOURCES      = coders/map.c
coders_map_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_map_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_map_la_LIBADD       = $(LIBMAGICK)

# MAT coder module
coders_mat_la_SOURCES      = coders/mat.c
coders_mat_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mat_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mat_la_LIBADD       = $(LIBMAGICK) $(LIB_ZLIB) $(LIB_MATH)

# MATTE coder module
coders_matte_la_SOURCES    = coders/matte.c
coders_matte_la_CPPFLAGS   = $(MAGICK_CODER_CPPFLAGS)
coders_matte_la_LDFLAGS    = $(MODULECOMMONFLAGS)
coders_matte_la_LIBADD     = $(LIBMAGICK)

# META coder module
coders_meta_la_SOURCES     = coders/meta.c
coders_meta_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_meta_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_meta_la_LIBADD      = $(LIBMAGICK)

# MIFF coder module
coders_miff_la_SOURCES     = coders/miff.c
coders_miff_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_miff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_miff_la_LIBADD      = $(LIBMAGICK) $(LIB_ZLIB) $(LIB_BZLIB)

# MONO coder module
coders_mono_la_SOURCES     = coders/mono.c
coders_mono_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_mono_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_mono_la_LIBADD      = $(LIBMAGICK)

# MPC coder module
coders_mpc_la_SOURCES      = coders/mpc.c
coders_mpc_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mpc_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mpc_la_LIBADD       = $(LIBMAGICK)

# MPEG coder module
coders_mpeg_la_SOURCES     = coders/mpeg.c
coders_mpeg_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_mpeg_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_mpeg_la_LIBADD      = $(LIBMAGICK)

# MPR coder module
coders_mpr_la_SOURCES      = coders/mpr.c
coders_mpr_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mpr_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mpr_la_LIBADD       = $(LIBMAGICK)

# MSL coder module
coders_msl_la_SOURCES      = coders/msl.c
coders_msl_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_msl_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_msl_la_LIBADD       = $(LIBMAGICK) $(LIB_XML) $(LIB_XML_DEPS)

# MTV coder module
coders_mtv_la_SOURCES      = coders/mtv.c
coders_mtv_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mtv_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mtv_la_LIBADD       = $(LIBMAGICK)

# MVG coder module
coders_mvg_la_SOURCES      = coders/mvg.c
coders_mvg_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_mvg_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_mvg_la_LIBADD       = $(LIBMAGICK)

# NULL coder module
coders_null_la_SOURCES     = coders/null.c
coders_null_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_null_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_null_la_LIBADD      = $(LIBMAGICK)

# OTB coder module
coders_otb_la_SOURCES      = coders/otb.c
coders_otb_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_otb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_otb_la_LIBADD       = $(LIBMAGICK)

# PALM coder module
coders_palm_la_SOURCES     = coders/palm.c
coders_palm_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_palm_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_palm_la_LIBADD      = $(LIBMAGICK)

# PCD coder module
coders_pcd_la_SOURCES      = coders/pcd.c
coders_pcd_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pcd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pcd_la_LIBADD       = $(LIBMAGICK)

# PCL coder module
coders_pcl_la_SOURCES      = coders/pcl.c
coders_pcl_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pcl_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pcl_la_LIBADD       = $(LIBMAGICK)

# PCX coder module
coders_pcx_la_SOURCES      = coders/pcx.c
coders_pcx_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pcx_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pcx_la_LIBADD       = $(LIBMAGICK)

# PDB coder module
coders_pdb_la_SOURCES      = coders/pdb.c
coders_pdb_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pdb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pdb_la_LIBADD       = $(LIBMAGICK)

# PDF coder module
coders_pdf_la_SOURCES      = coders/pdf.c
coders_pdf_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pdf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pdf_la_LIBADD       = $(LIBMAGICK) $(LIB_ZLIB) $(LIB_GS)

# PICT coder module
coders_pict_la_SOURCES     = coders/pict.c
coders_pict_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_pict_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_pict_la_LIBADD      = $(LIBMAGICK)

# PIX coder module
coders_pix_la_SOURCES      = coders/pix.c
coders_pix_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pix_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pix_la_LIBADD       = $(LIBMAGICK)

# PNG coder module
coders_png_la_SOURCES      = coders/png.c
coders_png_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_png_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_png_la_LIBADD       = $(LIBMAGICK) $(LIB_PNG) $(LIB_JPEG) $(LIB_ZLIB) $(LIB_MATH)

# PLASMA coder module
coders_plasma_la_SOURCES   = coders/plasma.c
coders_plasma_la_CPPFLAGS  = $(MAGICK_CODER_CPPFLAGS)
coders_plasma_la_LDFLAGS   = $(MODULECOMMONFLAGS)
coders_plasma_la_LIBADD    = $(LIBMAGICK)

# PNM coder module
coders_pnm_la_SOURCES      = coders/pnm.c
coders_pnm_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pnm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pnm_la_LIBADD       = $(LIBMAGICK)

# PREVIEW coder module
coders_preview_la_SOURCES  = coders/preview.c
coders_preview_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_preview_la_LDFLAGS  = $(MODULECOMMONFLAGS)
coders_preview_la_LIBADD   = $(LIBMAGICK)

# PS coder module
coders_ps_la_SOURCES       = coders/ps.c
coders_ps_la_CPPFLAGS      = $(MAGICK_CODER_CPPFLAGS)
coders_ps_la_LDFLAGS       = $(MODULECOMMONFLAGS)
coders_ps_la_LIBADD        = $(LIBMAGICK) $(LIB_GS) $(LIB_MATH)

# PS2 coder module
coders_ps2_la_SOURCES      = coders/ps2.c
coders_ps2_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_ps2_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_ps2_la_LIBADD       = $(LIBMAGICK) $(LIB_TIFF) $(LIB_JBIG) $(LIB_JPEG) $(LIB_LZMA) $(LIB_ZLIB) $(LIB_MATH)

# PS3 coder module
coders_ps3_la_SOURCES      = coders/ps3.c
coders_ps3_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_ps3_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_ps3_la_LIBADD       = $(LIBMAGICK) $(LIB_TIFF) $(LIB_JBIG) $(LIB_JPEG) $(LIB_LZMA) $(LIB_ZLIB) $(LIB_MATH)

# PSD coder module
coders_psd_la_SOURCES      = coders/psd.c
coders_psd_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_psd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_psd_la_LIBADD       = $(LIBMAGICK)

# PWP coder module
coders_pwp_la_SOURCES      = coders/pwp.c
coders_pwp_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_pwp_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_pwp_la_LIBADD       = $(LIBMAGICK)

# RGB coder module
coders_rgb_la_SOURCES      = coders/rgb.c
coders_rgb_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_rgb_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_rgb_la_LIBADD       = $(LIBMAGICK)

# RLA coder module
coders_rla_la_SOURCES      = coders/rla.c
coders_rla_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_rla_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_rla_la_LIBADD       = $(LIBMAGICK)

# RLE coder module
coders_rle_la_SOURCES      = coders/rle.c
coders_rle_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_rle_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_rle_la_LIBADD       = $(LIBMAGICK)

# SCT coder module
coders_sct_la_SOURCES      = coders/sct.c
coders_sct_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_sct_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_sct_la_LIBADD       = $(LIBMAGICK)

# SFW coder module
coders_sfw_la_SOURCES      = coders/sfw.c
coders_sfw_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_sfw_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_sfw_la_LIBADD       = $(LIBMAGICK)

# SGI coder module
coders_sgi_la_SOURCES      = coders/sgi.c
coders_sgi_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_sgi_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_sgi_la_LIBADD       = $(LIBMAGICK)

# STEGANO coder module
coders_stegano_la_SOURCES  = coders/stegano.c
coders_stegano_la_CPPFLAGS = $(MAGICK_CODER_CPPFLAGS)
coders_stegano_la_LDFLAGS  = $(MODULECOMMONFLAGS)
coders_stegano_la_LIBADD   = $(LIBMAGICK)

# SUN coder module
coders_sun_la_SOURCES      = coders/sun.c
coders_sun_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_sun_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_sun_la_LIBADD       = $(LIBMAGICK)

# SVG coder module
coders_svg_la_SOURCES      = coders/svg.c
coders_svg_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_svg_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_svg_la_LIBADD       = $(LIBMAGICK) $(LIB_XML) $(LIB_XML_DEPS)

# TGA coder module
coders_tga_la_SOURCES      = coders/tga.c
coders_tga_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_tga_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_tga_la_LIBADD       = $(LIBMAGICK)

# TIFF coder module
coders_tiff_la_SOURCES     = coders/tiff.c
coders_tiff_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_tiff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_tiff_la_LIBADD      = $(LIBMAGICK) $(LIB_TIFF) $(LIB_JBIG) $(LIB_JPEG) $(LIB_LZMA) $(LIB_ZLIB) $(LIB_MATH)

# TILE coder module
coders_tile_la_SOURCES     = coders/tile.c
coders_tile_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_tile_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_tile_la_LIBADD      = $(LIBMAGICK)

# TIM coder module
coders_tim_la_SOURCES      = coders/tim.c
coders_tim_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_tim_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_tim_la_LIBADD       = $(LIBMAGICK)

# TOPOL coder module
coders_topol_la_SOURCES    = coders/topol.c
coders_topol_la_CPPFLAGS   = $(MAGICK_CODER_CPPFLAGS)
coders_topol_la_LDFLAGS    = $(MODULECOMMONFLAGS)
coders_topol_la_LIBADD     = $(LIBMAGICK)

# TTF coder module
coders_ttf_la_SOURCES      = coders/ttf.c
coders_ttf_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_ttf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_ttf_la_LIBADD       = $(LIBMAGICK)

# TXT coder module
coders_txt_la_SOURCES      = coders/txt.c
coders_txt_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_txt_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_txt_la_LIBADD       = $(LIBMAGICK) $(LIB_MATH)

# UIL coder module
coders_uil_la_SOURCES      = coders/uil.c
coders_uil_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_uil_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_uil_la_LIBADD       = $(LIBMAGICK)

# URL coder module
coders_url_la_SOURCES      = coders/url.c
coders_url_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_url_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_url_la_LIBADD       = $(LIBMAGICK) $(LIB_XML) $(LIB_XML_DEPS)

# UYVY coder module
coders_uyvy_la_SOURCES     = coders/uyvy.c
coders_uyvy_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_uyvy_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_uyvy_la_LIBADD      = $(LIBMAGICK)

# VICAR coder module
coders_vicar_la_SOURCES    = coders/vicar.c
coders_vicar_la_CPPFLAGS   = $(MAGICK_CODER_CPPFLAGS)
coders_vicar_la_LDFLAGS    = $(MODULECOMMONFLAGS)
coders_vicar_la_LIBADD     = $(LIBMAGICK)

# VID coder module
coders_vid_la_SOURCES      = coders/vid.c
coders_vid_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_vid_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_vid_la_LIBADD       = $(LIBMAGICK)

# VIFF coder module
coders_viff_la_SOURCES     = coders/viff.c
coders_viff_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_viff_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_viff_la_LIBADD      = $(LIBMAGICK)

# WBMP coder module
coders_wbmp_la_SOURCES     = coders/wbmp.c
coders_wbmp_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_wbmp_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_wbmp_la_LIBADD      = $(LIBMAGICK)

# WMF coder module
coders_wmf_la_SOURCES      = coders/wmf.c
coders_wmf_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_wmf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_wmf_la_LIBADD       = $(LIBMAGICK) $(LIB_WMF) $(LIB_WMF_DEPS) $(LIB_MATH)

# WPG coder module
coders_wpg_la_SOURCES      = coders/wpg.c
coders_wpg_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_wpg_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_wpg_la_LIBADD       = $(LIBMAGICK)

# X coder module
coders_x_la_SOURCES        = coders/x.c
coders_x_la_CPPFLAGS       = $(MAGICK_CODER_CPPFLAGS)
coders_x_la_LDFLAGS        = $(MODULECOMMONFLAGS)
coders_x_la_LIBADD         = $(LIBMAGICK) $(LIB_X11)

# XBM coder module
coders_xbm_la_SOURCES      = coders/xbm.c
coders_xbm_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_xbm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_xbm_la_LIBADD       = $(LIBMAGICK)

# XC coder module
coders_xc_la_SOURCES       = coders/xc.c
coders_xc_la_CPPFLAGS      = $(MAGICK_CODER_CPPFLAGS)
coders_xc_la_LDFLAGS       = $(MODULECOMMONFLAGS)
coders_xc_la_LIBADD        = $(LIBMAGICK)

# XCF coder module
coders_xcf_la_SOURCES      = coders/xcf.c
coders_xcf_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_xcf_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_xcf_la_LIBADD       = $(LIBMAGICK)

# XPM coder module
coders_xpm_la_SOURCES      = coders/xpm.c
coders_xpm_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_xpm_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_xpm_la_LIBADD       = $(LIBMAGICK)

# XWD coder module
coders_xwd_la_SOURCES      = coders/xwd.c
coders_xwd_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_xwd_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_xwd_la_LIBADD       = $(LIBMAGICK) $(LIB_X11)

# YUV coder module
coders_yuv_la_SOURCES      = coders/yuv.c
coders_yuv_la_CPPFLAGS     = $(MAGICK_CODER_CPPFLAGS)
coders_yuv_la_LDFLAGS      = $(MODULECOMMONFLAGS)
coders_yuv_la_LIBADD       = $(LIBMAGICK)

# WEBP coder module
coders_webp_la_SOURCES     = coders/webp.c
coders_webp_la_CPPFLAGS    = $(MAGICK_CODER_CPPFLAGS)
coders_webp_la_LDFLAGS     = $(MODULECOMMONFLAGS)
coders_webp_la_LIBADD      = $(LIBMAGICK) $(LIB_WEBP)
