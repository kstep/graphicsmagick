#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_gray_bzip_%d' -compress bzip ${SRCDIR}/input_gray.miff MIFF
