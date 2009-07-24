#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./constitute -storagetype char ${SRCDIR}/input_truecolor.miff bgro
