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

${GM} montage ${MONTAGE_FLAGS} -geometry '130x194+10+5>' -gravity 'Center' \
  -bordercolor 'green' -borderwidth 1x1 -tile '7x4' -compose 'over' \
  -background '#ffffff' -font ${GENERIC_TTF} -pointsize 18 -fill '#600' \
  -stroke 'none' null: null: null: null: null: null: null: 'tmp:[A-Z]*_out.miff' \
  -compress rle montage_out.miff
${GM} convert ${CONVERT_FLAGS} logo: -resize 40% logo_out.miff
${GM} composite ${COMPOSITE_FLAGS} 'tmp:logo_out.miff' -gravity north 'tmp:montage_out.miff' -depth 8 demo-%d.miff
