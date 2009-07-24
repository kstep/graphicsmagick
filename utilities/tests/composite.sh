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

OUTFILE=TileComposite_out.miff
rm -f ${OUTFILE}
${GM} composite ${COMPOSITE_FLAGS} ${SMILE_MIFF} -geometry +35+65 ${MODEL_MIFF} -label Composite ${OUTFILE}
