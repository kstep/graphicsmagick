#!/bin/sh
# Copyright (C) 2010 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#  Test 'convert' for reading input from a pipe.
#

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

${GM} convert ${CONVERT_FLAGS} - 'null:' < ${SUNRISE_MIFF}
${GM} convert ${CONVERT_FLAGS} MIFF:- 'null:' < ${SUNRISE_MIFF}