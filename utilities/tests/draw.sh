#!/bin/sh
# Copyright (C) 2004 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Test for 'gm' utility.
#

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

OUTFILE=TileDraw_out.miff
DRAWFILE=draw_circle_out.txt
rm -f ${OUTFILE}
echo 'circle 60,90 60,120' > ${DRAWFILE}
${GM} convert ${CONVERT_FLAGS} ${MODEL_MIFF} -fill none -stroke gold \
         -draw @${DRAWFILE} -label Draw ${OUTFILE}
rm -f ${DRAWFILE}