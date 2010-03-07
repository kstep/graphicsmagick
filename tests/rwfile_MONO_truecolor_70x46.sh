#!/bin/sh
# Copyright (C) 2004-2010 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile -filespec 'out_truecolor_70x46_%d' -size 70x46 ${SRCDIR}/input_truecolor_70x46.miff MONO
