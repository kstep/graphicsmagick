#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ts=4:sw=4:expandtab:

# Copyright 2008 Mark Mitchell
# License: Same as GraphicsMagick.

import sys
import re

try:
    infilename = sys.argv[1]
    infile = file(infilename, 'r')
except IndexError:
    infile = sys.stdin

for line in infile:
    line = re.sub(r'<td>(\d+\.\d+</td>)', r'<td class="decimal">\1', line)
    sys.stdout.write(line)
