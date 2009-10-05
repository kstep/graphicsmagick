#!/bin/sh
# Copyright (C) 2009 GraphicsMagick Group

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

${GM} -help
