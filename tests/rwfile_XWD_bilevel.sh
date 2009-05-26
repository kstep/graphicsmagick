#!/bin/sh
# Copyright (C) 2004-2009-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile -filespec 'out_bilevel_%d' ${SRCDIR}/input_bilevel.miff XWD
