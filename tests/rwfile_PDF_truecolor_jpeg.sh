#!/bin/sh
# Copyright (C) 2004-2010 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_truecolor_jpeg_%d' -compress jpeg ${SRCDIR}/input_truecolor.miff PDF
