# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Makefile for building GraphicsMagick utilities
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.8 foreign

MAGICK_LIB	= $(top_builddir)/magick/libGraphicsMagick.la # libtool library name
AM_CPPFLAGS	=  -I$(top_builddir) -I$(top_srcdir)

STDMANPAGES = gm.1 miff.4 quantize.5
X11MANPAGES = 
ALLMANPAGES = $(STDMANPAGES) $(X11MANPAGES)


MANPAGES = $(ALLMANPAGES)

bin_PROGRAMS = gm

# Libtool libraries, dependencies, and linker flags used by most executables
if WITH_LTDL
COMMONLDADD	= $(MAGICK_LIB) $(top_builddir)/ltdl/libltdlc.la
COMMONLDFLAGS	= @LDFLAGS@ @DLLDFLAGS@
else
COMMONLDADD	= $(MAGICK_LIB)
COMMONLDFLAGS	= @LDFLAGS@
endif # WITH_LTDL

gm_LDADD           = $(COMMONLDADD)
gm_LDFLAGS         = $(COMMONLDFLAGS)
gm_SOURCES         = gm.c

# Manual pages to install
man_MANS = $(MANPAGES)

# Additional files to distribute
EXTRA_DIST = $(ALLMANPAGES)

clean-local:
	rm -f *_out.miff demo.miff gm.core core

RUNENV = $(SHELL) $(top_builddir)/rungm.sh

GM=@$(RUNENV) ./gm

MODEL_MIFF=$(top_srcdir)/Magick++/demo/model.miff
SMILE_MIFF=$(top_srcdir)/Magick++/demo/smile.miff
GENERIC_TTF=$(top_srcdir)/PerlMagick/demo/Generic.ttf

check:
	rm -f demo.miff *_out.miff
	${GM} convert $(MODEL_MIFF) +noise Laplacian              -label AddNoise AddNoise_out.miff
	${GM} convert $(MODEL_MIFF) -fill gold \
	        -pointsize 14 -font $(GENERIC_TTF) \
	        -draw 'gravity North text 0,20 "Magick"' -label Annotate Annotate_out.miff
	${GM} convert $(MODEL_MIFF) -blur 0.0x1.0                 -label Blur Blur_out.miff
	${GM} convert $(MODEL_MIFF) -bordercolor gold -border 6x6 -label Border Border_out.miff
	${GM} convert $(MODEL_MIFF)                               -label Channel Channel_out.miff
	${GM} convert $(MODEL_MIFF) -charcoal 0x1                 -label Charcoal Charcoal_out.miff
	${GM} composite $(SMILE_MIFF) -geometry +35+65 $(MODEL_MIFF) -label Composite Comp_out.miff
	${GM} convert $(MODEL_MIFF) -contrast                     -label Contrast Contrast_out.miff
	${GM} convert $(MODEL_MIFF)                               -label Convolve Convolve_out.miff
	${GM} convert $(MODEL_MIFF) -despeckle                    -label Despeckle Despeckle_out.miff
	${GM} convert $(MODEL_MIFF) -crop '80x80+25+50'           -label Crop Crop_out.miff
	${GM} convert $(MODEL_MIFF) -fill none -stroke gold \
	        -draw 'circle 60,90 60,120'              -label Draw Draw_out.miff
	${GM} convert $(MODEL_MIFF) -edge 0x1                     -label Edge Edge_out.miff
	${GM} convert $(MODEL_MIFF) -emboss 0x1                   -label Emboss Emboss_out.miff
	${GM} convert $(MODEL_MIFF) -equalize                     -label Equalize Equalize_out.miff
	${GM} convert $(MODEL_MIFF) -implode -1                   -label Explode Explode_out.miff
	${GM} convert $(MODEL_MIFF) -flip                         -label Flip Flip_out.miff
	${GM} convert $(MODEL_MIFF) -flop                         -label Flop Flop_out.miff
	${GM} convert $(MODEL_MIFF) -frame 15x15+3+3              -label Frame Frame_out.miff
	${GM} convert $(MODEL_MIFF) -gamma 1.6                    -label Gamma Gamma_out.miff
	${GM} convert $(MODEL_MIFF) -gaussian 0.0x1.5             -label GaussianBlur GB_out.miff
	${GM} convert -size 130x194 gradient:'#20a0ff-#ffff00' -label Gradient Gradient_out.miff
	${GM} convert $(MODEL_MIFF) -colorspace gray              -label Grayscale Grayscale_out.miff
	${GM} convert $(MODEL_MIFF) -implode 0.5                  -label Implode Implode_out.miff
	${GM} convert $(MODEL_MIFF) -level 20%                    -label Level Level_out.miff
	${GM} convert $(MODEL_MIFF) -median 3                     -label MedianFilter Median_out.miff
	${GM} convert $(MODEL_MIFF) -modulate 110/110/110         -label Modulate Mod_out.miff
	${GM} convert $(MODEL_MIFF) -colorspace gray -colors 2 \
	       +dither                                   -label Monochrome Mono_out.miff
	${GM} convert $(MODEL_MIFF) -negate                       -label Negate Negate_out.miff
	${GM} convert $(MODEL_MIFF) -normalize                    -label Normalize Normal_out.miff
	${GM} convert $(MODEL_MIFF) -paint 0x1                    -label Oilpaint Oil_out.miff
	${GM} convert -size 130x194 plasma:fractal             -label Plasma Plasma_out.miff
	${GM} convert $(MODEL_MIFF)                               -label Quantize Quantize_out.miff
	${GM} convert $(MODEL_MIFF) -raise 10x10                  -label Raise Raise_out.miff
	${GM} convert $(MODEL_MIFF) -noise 0x1                    -label ReduceNoise RedN_out.miff
	${GM} convert $(MODEL_MIFF) -resize 50%                   -label Resize Resize_out.miff
	${GM} convert $(MODEL_MIFF) -roll +20+10                  -label Roll Roll_out.miff
	${GM} convert $(MODEL_MIFF) -fill black -rotate 45 \
	        -transparent black                       -label Rotate Rotate_out.miff
	${GM} convert $(MODEL_MIFF) -scale 60%                    -label Scale Scale_out.miff
	${GM} convert $(MODEL_MIFF) -segment 1x1.5                -label Segment Segment_out.miff
	${GM} convert $(MODEL_MIFF) -shade 30x30                  -label Shade Shade_out.miff
	${GM} convert $(MODEL_MIFF) -sharpen 0.0x1.0              -label Sharpen Sharpen_out.miff
	${GM} convert $(MODEL_MIFF) -shave 10x10                  -label Shave Shave_out.miff
	${GM} convert $(MODEL_MIFF) -fill black -shear 45x45 \
	        -transparent black                       -label Shear Shear_out.miff
	${GM} convert $(MODEL_MIFF) -spread 3                     -label Spread Spread_out.miff
	${GM} convert $(MODEL_MIFF) -solarize 50%                 -label Solarize Solar_out.miff
	${GM} convert $(MODEL_MIFF) -background '#000000FF' \
	        -swirl 90                                -label Swirl Swirl_out.miff
	${GM} convert $(MODEL_MIFF) -unsharp 0.0x1.0              -label UnsharpMask Unsharp_out.miff
	${GM} convert $(MODEL_MIFF) -background '#000000FF' \
	        -wave 25x150                             -label Wave Wave_out.miff
	${GM} convert $(MODEL_MIFF) -equalize -ordered-dither intensity 2x2 -label Ordered2x2 Ordered2_out.miff
	${GM} convert $(MODEL_MIFF) -equalize -ordered-dither intensity 3x3 -label Ordered3x3 Ordered3_out.miff
	${GM} convert $(MODEL_MIFF) -equalize -ordered-dither intensity 4x4 -label Ordered4x4 Ordered4_out.miff
	${GM} convert $(MODEL_MIFF) -equalize -random-threshold intensity '10%' -label 'Random10%%' Random10_out.miff
	${GM} montage -geometry '130x194+10+5>' -gravity 'Center' \
	  -bordercolor 'green' -borderwidth 1x1 -tile '5x1000' -compose 'over' \
	  -background '#ffffff' -font $(GENERIC_TTF) -pointsize 18 -fill '#600' \
	  -stroke 'none' null: null: null: null: null: 'tmp:[A-Z]*_out.miff' \
	  -compress rle montage_out.miff
	${GM} convert logo: -resize 40% logo_out.miff
	${GM} composite 'tmp:logo_out.miff' -gravity north 'tmp:montage_out.miff' demo.miff
