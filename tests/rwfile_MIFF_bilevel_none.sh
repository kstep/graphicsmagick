#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_bilevel_none_%d' -compress none ${SRCDIR}/input_bilevel.miff MIFF
