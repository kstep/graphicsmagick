#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ts=4:sw=4:expandtab:

# Copyright 2008 Mark Mitchell
# License: Same as GraphicsMagick.

__doc__ = """Runs rst2htmldeco.py on all files in the rst_pagelist.txt list.

Usage:
    make_www.py rst_pagelist.txt [GMTOPDIR] [OUTDIR]

    GMTOPDIR is prefixed to the path of the rst input file specified in
    rst_pagelist.txt.  If not specified, it defaults to the parent
    directory of the directory in which this make_www.py script lives.

    OUTDIR is prefixed to the path of the html output file specified in
    rst_pagelist.txt.  If not specified it defaults to GMTOPDIR.

    The value of --link-stylesheet passed to rst2htmldeco.py is obtained
    from rst_pagelist.txt column 3.

    The value of --url-prefix passed to rst2htmldeco.py is obtained
    from rst_pagelist.txt column 4.
"""

import sys
import os, os.path

import rst2htmldeco

def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 2:
        print >> sys.stderr, __doc__
        return 1

    if argv[1] in ('-h', '--help'):
        print >> sys.stdout, __doc__
        return 0

    rstlist_file = argv[1]

    gm_topdir = None
    if len(argv) >= 3:
        gm_topdir = argv[2]

    outfile_dir = None
    if len(argv) >= 4:
        outfile_dir = argv[3]

    if not gm_topdir:
        # NOTE: the following is predicated on the directory structure of the
        # GraphicsMagick source tree.  It assumes this script resides in
        # TOPDIR/scripts and the .c files referred to by the whatis file
        # reside in TOPDIR/magick
        current_dir = os.path.dirname(os.path.abspath(__file__))
        gm_topdir = os.path.normpath(os.path.join(current_dir, '..'))

    if not outfile_dir:
        outfile_dir = gm_topdir

    f = file(rstlist_file, 'r')
    cnt = 0
    for line in f:
        cnt += 1
        line = line.strip()
        if not line:
            continue
        if line.startswith('#'):
            continue
        try:
            rstfile, outfile, stylesheet_url, url_prefix = line.split(None, 3)
        except ValueError:
            print >> sys.stderr, "Line %u of %s: improper format" % (cnt, rstlist_file)
            return 1

        rstfile_path = os.path.join(gm_topdir, rstfile.strip())
        outfile_path = os.path.join(outfile_dir, outfile.strip())
        args = ['--link-stylesheet=%s' % stylesheet_url,
                '--url-prefix=%s' % url_prefix,
                rstfile_path,
                outfile_path]
        print 'rst2htmldeco.py %s' % ' '.join(args)
        rst2htmldeco.main(args)

    f.close()


if __name__ == '__main__':
    sys.exit(main())

