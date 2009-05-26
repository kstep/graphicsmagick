#!/bin/sh
# Copyright (C) 2004-2009-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile -filespec 'out_gray_%d' ${SRCDIR}/input_gray.miff GIF
