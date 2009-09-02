#!/bin/sh
# Copyright (C) 2009 GraphicsMagick Group
#
# Perform a simple verification that we can usefully round-trip by
# applying a profile and then reverting the changes by applying the
# original profile.

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

ORIGINAL_PROFILE=sunrise.icc
IMAGE_OUTPUT=ICCTransform_out.miff

rm -f ${ORIGINAL_PROFILE}
rm -f ${IMAGE_OUTPUT}
${GM} convert ${SUNRISE_MIFF} ${ORIGINAL_PROFILE}
${GM} convert ${SUNRISE_MIFF} -profile ${BETARGB_PROFILE} -profile ${ORIGINAL_PROFILE} ${IMAGE_OUTPUT}
${GM} compare -maximum-error 0.004 -metric MAE ${SUNRISE_MIFF} ${IMAGE_OUTPUT}
rm -f ${ORIGINAL_PROFILE}
rm -f ${IMAGE_OUTPUT}
