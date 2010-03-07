#!/bin/sh
# Copyright (C) 2004-2010 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_bilevel_bzip_%d' -compress bzip ${SRCDIR}/input_bilevel.miff MIFF
