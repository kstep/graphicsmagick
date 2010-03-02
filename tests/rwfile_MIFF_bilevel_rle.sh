#!/bin/sh
# Copyright (C) 2004-2010 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_bilevel_rle_%d' -compress rle ${SRCDIR}/input_bilevel.miff MIFF
