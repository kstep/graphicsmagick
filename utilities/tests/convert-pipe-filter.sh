#!/bin/sh
# Copyright (C) 2010 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Test 'convert' for filter mode
#

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

echo "Expect MIFF output"
${GM} convert ${CONVERT_FLAGS} - -  < ${SUNRISE_MIFF} | ${GM} identify -format '%m %wx%h %r %q-bit' -
echo
echo "Expect MIFF output"
${GM} convert ${CONVERT_FLAGS} MIFF:- -  < ${SUNRISE_MIFF} | ${GM} identify -format '%m %wx%h %r %q-bit' -
echo
echo "Expect PNM output"
${GM} convert ${CONVERT_FLAGS} MIFF:- PNM:-  < ${SUNRISE_MIFF} | ${GM} identify -format '%m %wx%h %r %q-bit' -
echo
echo "Expect PNM output"
${GM} convert ${CONVERT_FLAGS} - PNM:-  < ${SUNRISE_MIFF} | ${GM} identify -format '%m %wx%h %r %q-bit' -
