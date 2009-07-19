#!/bin/sh
# Copyright (C) 2009 GraphicsMagick Group

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

rm -f mask_out.miff TileMask_out.miff
${GM} convert ${CONVERT_FLAGS} ${MODEL_MIFF} -threshold "30%" mask_out.miff
${GM} convert ${CONVERT_FLAGS} -mask mask_out.miff ${MODEL_MIFF} -negate -label Mask TileMask_out.miff
