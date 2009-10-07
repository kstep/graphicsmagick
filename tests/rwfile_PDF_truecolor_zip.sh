#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_truecolor_zip_%d' -compress zip ${SRCDIR}/input_truecolor.miff PDF
