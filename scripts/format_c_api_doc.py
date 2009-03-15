#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: ts=4:sw=4:expandtab:

# Copyright 2008 Mark Mitchell
# License: see __license__ below.


__doc__ = """
Reads a GraphicsMagick source file and parses the specially formatted
comment blocks which precede each function and writes the information
obtained from the comment block into a reStructuredText file.

Usage:
    format_c_api_docs.py [options] SRCFILE OUTFILE

    SRCFILE is the path to a Graphicsmagick API .c file.
    For example: ./magick/animate.c

    OUTFILE is the path where the reStructuredText file is written.

    Options:

    -h --help        -- Print this help message
    -w --whatis-file -- The path to a file containing "whatis" information for
                        the source files.  The format of this file is:
                        * one line per source file
                        * source filename (without directory paths) and whatis text
                          are separated by whitespace
                        * blank lines are ignored
                        * lines starting with '#' are ignored
    -i --include-rst -- Comma-separated list of file paths to be objects of reST
                        ..include:: directives inserted in OUTFILE.
                        The default is the single file 'api_hyperlinks.rst'

Example of whatis file format:

animate.c          Interactively animate an image sequence
annotate.c         Annotate an image with text
"""

__copyright__ = "2008, Mark Mitchell"

__license__ = """
Copyright 2008, Mark Mitchell

Permission is hereby granted, free of charge, to any person obtaining
a copy of this Software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

The Software is provided "as is", without warranty of any kind,
express or implied, including but not limited to the warranties of
merchantability, fitness for a particular purpose and noninfringement.
In no event shall the authors or copyright holders be liable for any
claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with Software
or the use or other dealings in the Software.
"""

import sys
import getopt
import os, os.path
import re
import textwrap


# Key words to replace with HTML links
keywords = {
    'AffineMatrix'       : '`AffineMatrix`_',
    'BlobInfo'           : '`BlobInfo`_',
    'Cache'              : '`Cache`_',
    'ChannelType'        : '`ChannelType`_',
    'ChromaticityInfo'   : '`ChromaticityInfo`_',
    'ClassType'          : '`ClassType`_',
    'ClipPathUnits'      : '`ClipPathUnits`_',
    'ColorPacket'        : '`ColorPacket`_',
    'ColorspaceType'     : '`ColorspaceType`_',
    'ComplianceType'     : '`ComplianceType`_',
    'CompositeOperator'  : '`CompositeOperator`_',
    'CompressionType'    : '`CompressionType`_',
    'DecorationType'     : '`DecorationType`_',
    'DrawContext'        : '`DrawContext`_',
    'DrawInfo'           : '`DrawInfo`_',
    'ErrorHandler'       : '`ErrorHandler`_',
    'ExceptionInfo'      : '`ExceptionInfo`_',
    'ExceptionType'      : '`ExceptionType`_',
    'FillRule'           : '`FillRule`_',
    'FilterTypes'        : '`FilterTypes`_',
    'FrameInfo'          : '`FrameInfo`_',
    'GravityType'        : '`GravityType`_',
    'Image'              : '`Image`_',
    'ImageInfo'          : '`ImageInfo`_',
    'ImageType'          : '`ImageType`_',
    'InterlaceType'      : '`InterlaceType`_',
    'LayerType'          : '`LayerType`_',
    'MagickInfo'         : '`MagickInfo`_',
    'MonitorHandler'     : '`MonitorHandler`_',
    'MontageInfo'        : '`MontageInfo`_',
    'NoiseType'          : '`NoiseType`_',
    'PaintMethod'        : '`PaintMethod`_',
    'PixelPacket'        : '`PixelPacket`_',
    'PointInfo'          : '`PointInfo`_',
    'ProfileInfo'        : '`ProfileInfo`_',
    'QuantizeInfo'       : '`QuantizeInfo`_',
    'Quantum'            : '`Quantum`_',
    'QuantumType'        : '`QuantumType`_',
    'RectangleInfo'      : '`RectangleInfo`_',
    'RegistryType'       : '`RegistryType`_',
    'RenderingIntent'    : '`RenderingIntent`_',
    'ResolutionType'     : '`ResolutionType`_',
    'ResourceType'       : '`ResourceType`_',
    'SegmentInfo'        : '`SegmentInfo`_',
    'SignatureInfo'      : '`SignatureInfo`_',
    'StorageType'        : '`StorageType`_',
    'StreamHandler'      : '`StreamHandler`_',
    'StretchType'        : '`StretchType`_',
    'StyleType'          : '`StyleType`_',
    'TypeMetric'         : '`TypeMetric`_',
    'ViewInfo'           : '`ViewInfo`_',
    'VirtualPixelMethod' : '`VirtualPixelMethod`_',
    'MagickXResourceInfo'      : '`MagickXResourceInfo`_',
    }


state_init = 0
state_found_fcncomment = 1
state_found_fcntitle = 2
state_found_fcndoc = 3
state_more_prototype = 4
state_found_prototype = 5
state_found_private = 6
state_parmdescr = 7

def warn(msg):
    print >> sys.stderr, msg

def debugtrace(msg):
    print >> sys.stdout, msg

def nodebugtrace(msg):
    pass

dtrace = nodebugtrace
#dtrace = debugtrace

# extract and save function title.  example:
# +   X M a g i c k C o m m a n d                                               %
# %   X A n i m a t e B a c k g r o u n d I m a g e                             %
# Lines starting with '+' are private APIs which should not appear in 
# in the output.
re_func_title = re.compile(r'^[+|%]\s+((\w )+)\s*%')


def proto_pretty(line):
    """fixes up inconsistent spaces in C function prototypes"""
    line = re.sub(r',',   ' , ', line)
    line = re.sub(r'\(',  ' ( ', line)
    line = re.sub(r'\)',  ' ) ', line)
    line = re.sub(r'\*',  ' * ', line)
    line = re.sub(r'\s+', ' ',   line)

    line = re.sub(r'\(\s+\*', '(*', line)
    line = re.sub(r' ,',      ',',  line)
    line = re.sub(r' \(',     '(',  line)
    line = re.sub(r'\) ',     ')',  line)
    line = re.sub(r' \* ',    ' *', line)

    line = re.sub('^\s*',     '',   line)
    return line


class Paragraph:
    "Paragraphs consist of one or more lines of text."
    def __init__(self):
        self.lines = []

    def __str__(self):
        #return '\n'.join(self.lines)
        return '\n'.join([line.strip() for line in self.lines])


class Prototype:
    def __init__(self):
        self.lines = []

    def __str__(self):
        proto = ' '.join(self.lines)
        proto = proto_pretty(proto)
        # escape all the '*' chars
        proto = re.sub(r'\*', '\\*', proto)
        # escape all the '_' chars
        proto = re.sub(r'_', '\\_', proto)
        # now replace keywords with hyperlinks
        for k,v in keywords.iteritems():
            proto = re.sub(r'^%s ' % k, '%s ' % v, proto)
            proto = re.sub(r' %s ' % k, ' %s ' % v, proto)

        # make some attempt to wrap the text nicely
        openparen_index = proto.find('(')
        if openparen_index > 0:
            fcn = proto[:openparen_index+1]
            indent_len = len(fcn) + 3
            toomuch = (2 * fcn.count('\\')) + (3 * fcn.count('`_'))
            if toomuch > 0: # account for the space following the opening paren
                toomuch -= 1
            indent_len -= toomuch
            params = proto[openparen_index+1:].split(',')
            params = [p.strip() for p in params]
            max_param_len = 0
            for x in params:
                if len(x) > max_param_len:
                    max_param_len = len(x)
            wrap_width = max(96, max_param_len + indent_len)

            proto_lines = []
            line = fcn + ' '
            while params:
                x = params.pop(0)
                if len(line) + len(x) > wrap_width:
                    proto_lines.append(line)
                    line = ' ' * indent_len

                line += x
                if params:
                    line += ', '

            proto_lines.append(line)
            proto = '\n  '.join(proto_lines)                    
        return ".. parsed-literal::\n\n    %s" % proto


class ListItem:
    """List items are used for parameter descriptions, and consist of the
    parameter name and one or more lines of description text."""
    def __init__(self, name):
        self.name = name
        self.lines = []

    def __str__(self):
        s = []
        s.append('%s:' % self.name)
        for line in self.lines:
            s.append('    %s' % line.strip())
        return '\n'.join(s)


class Function:
    def __init__(self, name):
        self.name = name
        self.prototype = None
        # Description is a list, the items of which are either Paragraph or
        # ListItem or Prototype instances.
        self.description = []

    def __str__(self):
        lines = []
        lines.append('')
        lines.append('')
        lines.append(self.name)
        lines.append('=' * len(self.name))
        lines.append('')
        lines.append('Synopsis')
        lines.append('--------')
        lines.append(str(self.prototype))
        lines.append('')
        lines.append('Description')
        lines.append('-----------')
        for item in self.description:
            lines.append(str(item))
            lines.append('')

        return '\n'.join(lines)


def parse(srcfilepath):
    list_item = None
    proto = None
    para = None
    func = None
    functions = []
    state = state_init
    linecnt = 0
    ftitle = None
    f = file(srcfilepath, 'r')
    for line in f:
        linecnt += 1
        if not (line.startswith('%') or line.startswith('+') or re.search(r'\*/', line)):
            continue

        line = line.strip()

        if state == state_init:
            # Find first line of function title/comment block
            if line.startswith('%%%%%%%%'):
                dtrace('Line %d: start of function comment block ############' % linecnt)
                state = state_found_fcncomment
                continue

        elif state == state_found_fcncomment:
            # Search for the function name, with spaces between each letter
            if line.startswith('%%%%%%%%'):
                warn('Line %d: WARNING: no function name found, found start of function comment block instead.' % linecnt)
                state = state_init
                continue

            m = re_func_title.search(line)
            if m:
                if line.startswith('+'):
                    dtrace('Line %d: private API' % linecnt)
                    # private API, skip it
                    state = state_found_private
                else:
                    # public API, process it
                    ftitle = re.sub(' ', '', m.group(1))
                    dtrace('Line %d: public API %s' % (linecnt, ftitle))
                    func = Function(ftitle)
                    functions.append(func)
                    state = state_found_fcntitle
            continue

        elif state == state_found_private:
            # skip to end of function title block
            if line.startswith('%%%%%%%%'):
                dtrace('Line %d: end of private function comment block' % linecnt)
                state = state_init
                continue

        elif state == state_found_fcntitle:
            # skip to first line following end of function title block.
            # lines of the function title block start with and end with '%'.
            if not re.match(r'%.+%', line):
                dtrace('Line %d: end of public function comment block %s' % (linecnt, ftitle))
                state = state_found_fcndoc
                # fall through
        elif state == state_found_fcndoc:
            # extract function prototype
            if line.startswith('% '):
                line = re.sub(r'^%\s{0,2}', '', line, 1)
                # if empty args (), it's not the prototype, but the one-line summary
                if re.search(r'%s\(\)' % ftitle, line):
                    if para is None:
                        dtrace('Line %d: found_fcndoc start paragraph ()' % linecnt)
                        para = Paragraph()
                        func.description.append(para)
                    para.lines.append(line)
                # is this only line of prototype?
                elif re.search(r'%s\([^)]+\)$' % ftitle, line):
                    if para:
                        dtrace('Line %d: found_fcndoc end paragraph by proto ()' % linecnt)
                        para = None
                    dtrace('Line %d: one-line prototype' % linecnt)
                    proto = Prototype()
                    proto.lines.append(line)
                    func.description.append(proto)
                    func.prototype = proto
                    proto = None
                    state = state_found_prototype
                # is this first line of multiline prototype?
                elif re.search(r'%s\([^)]*$' % ftitle, line):
                    if para:
                        dtrace('Line %d: found_fcndoc end paragraph by proto (' % linecnt)
                        para = None
                    dtrace('Line %d: first line of multi-line prototype' % linecnt)
                    proto = Prototype()
                    proto.lines.append(line)
                    func.description.append(proto)
                    func.prototype = proto
                    state = state_more_prototype
                else:
                    if para is None:
                        dtrace('Line %d: found_fcndoc start paragraph' % linecnt)
                        para = Paragraph()
                        func.description.append(para)
                    para.lines.append(line)
            else:
                if line.startswith('%%%%%%%%'):
                    warn('Line %d: WARNING: no prototype found for %s, found start of function comment block instead.' % (linecnt, ftitle))
                    state = state_found_fcncomment
                    continue

                if line.strip() == '%':
                    # empty line terminates paragraph
                    if para:
                        dtrace('Line %d: found_fcndoc end paragraph by blank line' % linecnt)
                        para = None
                    if proto:
                        dtrace('Line %d: found_fcndoc end proto by blank line' % linecnt)
                        proto = None

            continue

        elif state == state_more_prototype:
            if re.match(r'%.+%', line):
                # really this should raise a warning of "incomplete prototype"
                continue
            line = re.sub(r'^%\s{0,2}', '', line, 1)
            if re.search(r'^\s*$', line):
                dtrace('Line %d: end of more prototype' % linecnt)
                state = state_found_prototype
            else:
                func.prototype.lines.append(line)
            continue

        elif state == state_found_prototype:
            dtrace('Line %d: found prototype of function %s' % (linecnt, ftitle))
            func.prototype.lines.append(';')

            #print 'Function %s' % func.name
            #print 'Synopsis'
            #print ' '.join(func.prototype)
            #print

            # Process parm description.
            # Description consists of two kinds of texts: paragraphs, and lists.
            # Lists consist of list items.  List items are one or more lines.
            # List items are separated by blank lines.  The first line of a list
            # item starts with 'o '.
            # Paragraphs consist of one or more lines which don't start with 'o '.
            # Paragraphs are separated from each other and from adjacent list items
            # by blank lines.
            # In theory, a line which starts with 'o ' which is not preceded by a
            # blank line is illegal syntax.
            para = None
            state = state_parmdescr
            # fall through

        elif state == state_parmdescr:
            if line.endswith('*/'):
                # end of function comment block
                dtrace('Line %d: end of parmdescr ************' % linecnt)
                if list_item:
                    func.description.append(list_item)
                    list_item = None
                if para:
                    func.description.append(para)
                    dtrace('Line %d: parmdescr end paragraph ()' % linecnt)
                    para = None
                func = None
                state = state_init
                continue

            line = re.sub(r'^%\s{0,2}', '', line, 1)
            if line:
                # look for list item, which starts with 'o'
                m = re.search(r'^\s+o\s+([^:]+:|o|[0-9]\.)\s(.*)', line)
                if m:
                    # first line of list item
                    if list_item: # if blank lines separate list items, this should never evaluate true
                        dtrace('Line %d: surprising end of list item' % linecnt)
                        func.description.append(list_item)
                        list_item = None
                    dtrace('Line %d: start list item' % linecnt)
                    list_item = ListItem(m.group(1).strip().rstrip(':'))
                    list_item.lines.append(m.group(2))
                else:
                    # either a line of paragraph or subsequent line of list item
                    if list_item:
                        # subsequent line of list item
                        list_item.lines.append(line)
                    else:
                        # line of paragraph
                        if list_item: # if blank lines after list items, this should never evaluate true
                            dtrace('Line %d: end of list item, end of list' % linecnt)
                            func.description.append(list_item)
                            list_item = None
                        if para is None:
                            dtrace('Line %d: parmdescr start paragraph' % linecnt)
                            para = Paragraph()
                        para.lines.append(line)

            else:
                # empty line, two cases:
                # 1. terminate multi-line list item
                # 2. terminate multi-line paragraph
                if list_item:
                    dtrace('Line %d: parmdescr end of list item by blank line' % linecnt)
                    func.description.append(list_item)
                    list_item = None
                elif para:
                    # terminate any paragraph
                    dtrace('Line %d: parmdescr end of paragraph by blank line' % linecnt)
                    func.description.append(para)
                    para = None

            continue

    f.close()
    return functions



def process_srcfile(srcfilepath, basename, whatis, outfile, include_rst):
    """outfile is a file object open for writing"""
    functions = parse(srcfilepath)
    print >> outfile, "=" * len(basename)
    print >> outfile, basename
    print >> outfile, "=" * len(basename)
    if whatis:
        print >> outfile, "-" * len(whatis)
        print >> outfile, whatis
        print >> outfile, "-" * len(whatis)
    print >> outfile
    print >> outfile, '.. contents:: :depth: 1'
    print >> outfile
    for x in include_rst:
        print >> outfile, '.. include:: %s' % x
    print >> outfile

    # print all functions found in this source file
    for func in functions:
        print >> outfile, func

        #para = para.strip() # trim leading and trailing whitespace
        #para = re.sub(r'\s+', ' ', para) # canonicalize inner whitespace
        #para = re.sub(r"""([a-zA-Z0-9][.!?][)'"]*) """, '\1  ', para) # Fix sentence ends



def find_val(key, keyval_file):
    val = None
    f = file(keyval_file, 'r')
    cnt = 0
    for line in f:
        cnt += 1
        if not line.strip():
            continue
        if line.startswith('#'):
            continue
        try:
            k, v = line.split(None, 1)
        except ValueError:
            print >> sys.stderr, "Line %u of %s: improper format" % (cnt, keyval_file)
            return None

        if k == key:
            val = v
            break

    f.close()
    return val.strip()


def main(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    # parse command line options
    try:
        opts, posn_args = getopt.getopt(argv, 'hw:i:',
                                       ['help',
                                        'whatis-file=',
                                        'include-rst=',
                                        ])
    except getopt.GetoptError, msg:
         print msg
         print __doc__
         return 1

    # process options
    whatis_file = None
    include_rst = ['api_hyperlinks.rst']

    for opt, val in opts:
        if opt in ("-h", "--help"):
            print __doc__
            return 0

        if opt in ("-w", "--whatis-file"):
            whatis_file = val

        if opt in ("-i", "--include-rst"):
            include_rst = [x for x in val.split(',') if x]

    if len(posn_args) != 2:
        print >> sys.stderr, 'Missing arguments'
        print >> sys.stderr, __doc__
        return 1

    srcfile_path = posn_args[0]
    outfile_path = posn_args[1]

    srcfile = os.path.basename(srcfile_path)
    base, ext = os.path.splitext(srcfile)
    if whatis_file:
        whatis = find_val(srcfile, whatis_file)
    else:
        whatis = None
    fout = file(outfile_path, 'w')
    process_srcfile(srcfile_path, base, whatis, fout, include_rst)
    fout.close()
    return 0


if __name__ == '__main__':
    sys.exit(main())

