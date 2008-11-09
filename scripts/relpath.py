#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ts=4:sw=4:expandtab:

# Copyright 2008 Mark Mitchell
# All rights reserved.
# License:  same as Python itself

import sys
import os, os.path

__doc__ = """Given two paths, one being a "top" directory,
and the other a file in a directory subordinate to the top directory,
returns a relative path from subdir to topdir.

Since this is intended for generating prefixes for constructing
relative URLs, the path separator in the output string is always
a forward slash, no matter what platform is used.

Usage:
    relpath.py  topdir   subdir/filename

Examples:

  args:     foo  foo/bar/plugh/index.html
  returns:  ../../

  args:     /usr/local/src/GraphicsMagick /usr/local/src/GraphicsMagick/www/index.html
  returns:  ../
"""

def rel_to_top(topdir, subdir):
    if topdir == subdir:
        return ''

    head = subdir
    count = 0
    while True:
        head, tail = os.path.split(head)
        if head == topdir or tail == '':
            break
        count += 1

    up = '/'.join(['..'] * count)
    if up:
        up += '/'

    return up


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print __doc__
        sys.exit(1)

    topdir = os.path.abspath(sys.argv[1])
    subdir = os.path.abspath(sys.argv[2])
    prefix = os.path.commonprefix([topdir, subdir])
    if prefix != topdir:
        print >> sys.stderr, "'%s' is not a subordinate path of '%s'" % (subdir, topdir)
        sys.exit(1)

    print rel_to_top(topdir, subdir)
    sys.exit(0)
