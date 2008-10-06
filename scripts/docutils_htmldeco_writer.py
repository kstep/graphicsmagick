#!/usr/bin/env python
# vim:ts=4:sw=4:expandtab:tw=100:

#* Author: Mark Mitchell
#* Copyright 2005-2008  Mark Mitchell, All Rights Reserved
#* License: see __license__ below.

__doc__ = """
A specialized subclass of the docutils html4css1 writer.

It includes some additional HTML in the HTMLTranslator body_prefix and body_suffix
so as to put a standard banner and navigation menus on the page.
"""

__copyright__ = "2005, 2008, Mark Mitchell"

__license__ = """
Copyright 2005, 2008, Mark Mitchell

Permission is hereby granted, free of charge, to any person obtaining
a copy of this oftware and associated documentation files (the
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

from docutils.writers import html4css1
from docutils import nodes
# the html fragments for banner, navigation menu, and footer
import html_fragments


class Writer(html4css1.Writer):
    '''My derived Writer class'''

    super_init = html4css1.Writer.__init__
    def __init__(self):
        self.super_init()
        self.translator_class = MmHtmlTranslator


class MmHtmlTranslator(html4css1.HTMLTranslator):
    """My HTML translator which alters body_prefix to include
    some extra HTML after the body opening tag, and alters body_suffix
    to include a bit more before the body closing tag."""

    def __init__(self, document):
        html4css1.HTMLTranslator.__init__(self, document)

        self.body_prefix.append(html_fragments.banner)
        self.body_prefix.append(html_fragments.nav)
        #self.body_suffix = [html_fragments.footer, '</body>\n</html>\n']
        self.body_suffix = ['</body>\n</html>\n']

