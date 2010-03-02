#!/bin/sh
# Copyright (C) 2004-2010 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_pallette_lzw_%d' -compress lzw ${SRCDIR}/input_pallette.miff PDF
