#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ts=4:sw=4:expandtab:

# Copyright 2008 Mark Mitchell
# License: Same as GraphicsMagick.

__doc__ = """Runs format_c_api_doc.py on all files in the whatis list,
producing reStructuredText files.  Then runs rst2htmldeco.py on all the
reStructuredText files, producing HTML files.

Usage:
    format_c_api_docs.py whatisfile [GMTOPDIR] [OUTDIR]

    GMTOPDIR is used to construct filesystem paths to the 'magick'
    directory of the .c input files, i.e. GMTOPDIR/magick.

    If OUTDIR is not specified, it defaults to GMTOPDIR/www/api

    Example:
    ./format_c_api_docs.py  whatis.txt /usr/local/src/GraphicsMagick/ ../www/api

    would expect to find the .c files listed in whatis.txt in the
    directory /usr/local/src/GraphicsMagick/magick, and will write
    the HTML files into the directory ../www/api, which is relative
    to the current working directory.

    The value of --url-prefix passed to rst2htmldeco.py is hard-wired
    as '../../', meaning this script assumes the standard GM directory
    layout.

    The value of --link-stylesheet passed to rst2htmldeco.py is hard-wired
    as '../../www/docutils-api.css'.
"""

import sys
import os, os.path
import shutil

import format_c_api_doc
import rst2htmldeco


# assume standard GM directory layout
stylesheet_file = 'docutils-api.css'
url_prefix = '../../'
stylesheet_url = '%swww/%s' % (url_prefix, stylesheet_file)



def main(argv=None):
    if argv is None:
        argv = sys.argv

    if len(argv) < 2:
        print >> sys.stderr, __doc__
        return 1

    if argv[1] in ('-h', '--help'):
        print __doc__
        return 0

    whatis_file = argv[1]

    gm_topdir = None
    if len(argv) >= 3:
        gm_topdir = argv[2]

    outfile_dir = None
    if len(argv) >= 4:
        outfile_dir = argv[3]

    current_dir = os.path.dirname(os.path.abspath(__file__))

    if not gm_topdir:
        # NOTE: the following is predicated on the directory structure of the
        # GraphicsMagick source tree.  It assumes this script resides in
        # TOPDIR/scripts and the .c files referred to by the whatis file
        # reside in TOPDIR/magick
        gm_topdir = os.path.normpath(os.path.join(current_dir, '..'))

    srcfile_dir = os.path.join(gm_topdir, 'magick')

    if not outfile_dir:
        outfile_dir = os.path.join(gm_topdir, 'www', 'api')

    f = file(whatis_file, 'r')
    cnt = 0
    for line in f:
        cnt += 1
        if not line.strip():
            continue
        if line.startswith('#'):
            continue
        try:
            cfile, whatis = line.split(None, 1)
        except ValueError:
            print >> sys.stderr, "Line %u of %s: improper format" % (cnt, whatis_file)
            return 1

        srcfile_path = os.path.join(srcfile_dir, cfile)
        srcfile = os.path.basename(srcfile_path)
        base, ext = os.path.splitext(srcfile)
        rstfile = base + '.rst'
        rstfile_path = os.path.join(outfile_dir, rstfile)
        htmlfile = base + '.html'
        htmlfile_path = os.path.join(outfile_dir, htmlfile)

        args = ['-w', whatis_file, srcfile_path, rstfile_path]
        print 'format_c_api_doc.py', ' '.join(args)
        format_c_api_doc.main(args)

        # Now generate HTML from the .rst files
        args = ['--link-stylesheet=%s' % stylesheet_url,
                '--url-prefix=%s' % url_prefix,
                rstfile_path,
                htmlfile_path]
        print 'rst2htmldeco.py %s' % ' '.join(args)
        rst2htmldeco.main(args)
        print 'rm', rstfile_path
        os.unlink(rstfile_path)
    f.close()



if __name__ == '__main__':
    sys.exit(main(sys.argv))

