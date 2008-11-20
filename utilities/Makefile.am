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

if !HasTTF
UTILITIES_TTF_XFAIL_TESTS = \
	utilities/tests/msl_composite.sh
endif

if !HasXML
UTILITIES_XML_XFAIL_TESTS = \
	utilities/tests/msl_composite.sh
endif

UTILITIES_XFAIL_TESTS = \
	$(UTILITIES_TTF_XFAIL_TESTS) \
	$(UTILITIES_XML_XFAIL_TESTS)

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
	utilities/tests/montage.sh \
	utilities/tests/msl_composite.sh \
        utilities/tests/preview_addnoise.sh \
        utilities/tests/preview_blur.sh \
        utilities/tests/preview_brightness.sh \
        utilities/tests/preview_charcoal.sh \
        utilities/tests/preview_despeckle.sh \
        utilities/tests/preview_dull.sh \
        utilities/tests/preview_edgedetect.sh \
        utilities/tests/preview_gamma.sh \
        utilities/tests/preview_grayscale.sh \
        utilities/tests/preview_hue.sh \
        utilities/tests/preview_implode.sh \
        utilities/tests/preview_oilpaint.sh \
        utilities/tests/preview_quantize.sh \
        utilities/tests/preview_raise.sh \
        utilities/tests/preview_reducenoise.sh \
        utilities/tests/preview_roll.sh \
        utilities/tests/preview_rotate.sh \
        utilities/tests/preview_saturation.sh \
        utilities/tests/preview_segment.sh \
        utilities/tests/preview_shade.sh \
        utilities/tests/preview_sharpen.sh \
        utilities/tests/preview_shear.sh \
        utilities/tests/preview_solarize.sh \
        utilities/tests/preview_spiff.sh \
        utilities/tests/preview_spread.sh \
        utilities/tests/preview_swirl.sh \
        utilities/tests/preview_threshold.sh \
        utilities/tests/preview_wave.sh

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
	utilities/tests/*_out.pnm \
	utilities/tests/*_out.txt \
	utilities/tests/demo*.miff \
	utilities/tests/gm.core \
	utilities/tests/core

# Install/Uninstall ImageMagick compatibility links
if MAGICK_COMPAT
MAGICKPROGRAMS=animate composite conjure convert display identify import mogrify montage
UTILITIES_INSTALL_EXEC_LOCAL_TARGETS=install-exec-local-utilities
install-exec-local-utilities:
	$(mkdir_p) $(DESTDIR)$(bindir)
	cd $(DESTDIR)$(bindir) ; \
	gm=`echo "gm" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	for name in $(MAGICKPROGRAMS) ; \
	do \
	  target=`echo "$$name" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  rm -f $$target ; \
	  $(LN_S) $$gm $$target ; \
	done

UTILITIES_UNINSTALL_LOCAL_TARGETS=uninstall-local-utilities
uninstall-local-utilities:
	cd $(DESTDIR)$(bindir) ; \
	for name in $(MAGICKPROGRAMS) ; \
	do \
	  target=`echo "$$name" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  rm -f $$target ; \
	done
endif # MAGICK_COMPAT
