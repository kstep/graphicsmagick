#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwblob ${SRCDIR}/input_gray.miff PNG
