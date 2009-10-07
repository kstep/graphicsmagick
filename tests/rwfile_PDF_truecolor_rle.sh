#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_truecolor_rle_%d' -compress rle ${SRCDIR}/input_truecolor.miff PDF
