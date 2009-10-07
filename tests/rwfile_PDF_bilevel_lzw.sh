#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_bilevel_lzw_%d' -compress lzw ${SRCDIR}/input_bilevel.miff PDF
