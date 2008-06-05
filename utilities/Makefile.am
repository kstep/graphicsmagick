# Copyright (C) 2004 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# Makefile for building GraphicsMagick utilities
#
#

UTILITIES_PGMS = \
	utilities/gm

utilities_gm_LDADD	= $(LIBMAGICK)
utilities_gm_LDFLAGS	= $(LDFLAGS)
utilities_gm_SOURCES	= utilities/gm.c

# Tests to run
UTILITIES_TESTS = \
	utilities/tests/addnoise.sh \
	utilities/tests/affine.sh \
	utilities/tests/annotate.sh \
	utilities/tests/black-threshold.sh \
	utilities/tests/blur.sh \
	utilities/tests/border.sh \
	utilities/tests/channel.sh \
	utilities/tests/charcoal.sh \
	utilities/tests/composite.sh \
	utilities/tests/contrast.sh \
	utilities/tests/convolve.sh \
	utilities/tests/crop.sh \
	utilities/tests/despeckle.sh \
	utilities/tests/draw.sh \
	utilities/tests/edge.sh \
	utilities/tests/emboss.sh \
	utilities/tests/equalize.sh \
	utilities/tests/explode.sh \
	utilities/tests/flip.sh \
	utilities/tests/flop.sh \
	utilities/tests/frame.sh \
	utilities/tests/gamma.sh \
	utilities/tests/gaussianblur.sh \
	utilities/tests/gradient.sh \
	utilities/tests/grayscale.sh \
	utilities/tests/implode.sh \
	utilities/tests/level.sh \
	utilities/tests/medianfilter.sh \
	utilities/tests/modulate.sh \
	utilities/tests/monochrome.sh \
	utilities/tests/negate.sh \
	utilities/tests/normalize.sh \
	utilities/tests/oilpaint.sh \
	utilities/tests/ordered-dither2x2.sh \
	utilities/tests/ordered-dither3x3.sh \
	utilities/tests/ordered-dither4x4.sh \
	utilities/tests/plasma.sh \
	utilities/tests/quantize.sh \
	utilities/tests/raise.sh \
	utilities/tests/random-threshold.sh \
	utilities/tests/reducenoise.sh \
	utilities/tests/resize.sh \
	utilities/tests/roll.sh \
	utilities/tests/rotate.sh \
	utilities/tests/scale.sh \
	utilities/tests/segment.sh \
	utilities/tests/shade.sh \
	utilities/tests/sharpen.sh \
	utilities/tests/shave.sh \
	utilities/tests/shear.sh \
	utilities/tests/solarize.sh \
	utilities/tests/spread.sh \
	utilities/tests/swirl.sh \
	utilities/tests/threshold.sh \
	utilities/tests/unsharpmask.sh \
	utilities/tests/wave.sh \
	utilities/tests/white-threshold.sh \
	utilities/tests/montage.sh

UTILITIES_MANS = \
	utilities/gm.1 \
	utilities/miff.4 \
	utilities/quantize.5

UTILITIES_EXTRA_DIST = \
	$(UTILITIES_MANS) \
	$(UTILITIES_TESTS) \
	utilities/tests/common.sh

UTILITIES_CLEANFILES = \
	utilities/tests/*_out.miff \
	utilities/tests/*_out.txt \
	utilities/tests/demo*.miff \
	utilities/tests/gm.core \
	utilities/tests/core
