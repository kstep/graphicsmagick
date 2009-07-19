#!/bin/sh
# Copyright (C) 2009 GraphicsMagick Group
#
# Verify that processing the Hald CLUT identity image with the Hald
# CLUT identity image produces an unaltered image.
#

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

OUTFILE=HaldClutIdentity_out.miff
levels='2 7 8 10 12'
for level in $levels
do
  echo "Testing Hald CLUT identity level ${level} ..."
  identity_image=identity:${level}
  rm -f ${OUTFILE}
  ${GM} convert ${CONVERT_FLAGS} ${identity_image} -hald-clut ${identity_image} -label Hald-Clut ${OUTFILE}
  ${GM} compare -maximum-error 4.0e-12 -metric MAE ${identity_image} ${OUTFILE}
  echo
done
