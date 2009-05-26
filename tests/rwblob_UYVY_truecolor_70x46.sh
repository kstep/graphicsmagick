#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwblob -size 70x46 ${SRCDIR}/input_truecolor_70x46.miff UYVY
