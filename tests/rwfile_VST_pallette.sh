#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile -filespec 'out_pallette_%d' ${SRCDIR}/input_pallette.miff VST
