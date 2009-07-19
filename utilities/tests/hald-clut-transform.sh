#!/bin/sh
# Copyright (C) 2009 GraphicsMagick Group
#
# Perform a simple verification that the Hald CLUT transform can be
# used to duplicate some other transform.

# We test duplication of -negate, which is a linear transform,
# allowing perfect value interpolation and detection of any bias,
# arithmetic inaccuracies, or "push".

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

XFORM='-negate'
IDENTITY_CLUT='identity:8'
CLUT_OUTPUT=HaldClutTransform_out.miff
REFERENCE_OUTPUT=HaldClutTransformRef_out.miff
XFORM_CLUT=HaldClutTransformCLUT_out.miff

eval ${GM} convert ${IDENTITY_CLUT} ${XFORM} ${XFORM_CLUT}
eval ${GM} convert ${MODEL_MIFF} ${XFORM} ${REFERENCE_OUTPUT}
${GM} convert ${MODEL_MIFF} -hald-clut ${XFORM_CLUT} ${CLUT_OUTPUT}
${GM} compare -maximum-error 4.0e-12 -metric MAE ${REFERENCE_OUTPUT} ${CLUT_OUTPUT}
