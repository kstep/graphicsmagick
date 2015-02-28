# vim:ts=4:sw=4:expandtab:tw=100:

#* Author: Mark Mitchell
#* License: Same as GraphicsMagick.

__doc__ = """
HTML fragments which are used as the banner and navigation menu in
GraphicsMagick web site pages.
"""


url_mailinglist = "https://sourceforge.net/p/graphicsmagick/mailman/?source=navbar"
url_sourceforge = "https://sourceforge.net/projects/graphicsmagick/"

banner_logo = 'images/gm-107x76.png' # relative to top directory


# banner_template and nav_template contain chunk which has to be created at
# runtime: the path to the image directory, and the path to the top directory.
# So, rst2htmldeco.py imports html_fragments and changes url_prefix to the
# appropriate value.  When docutils_htmldeco_writer imports html_fragments, this
# url_prefix attribute should already be fixed up.

url_prefix = '' # trailing slash always needed

banner_template = """
<div class="banner">
<img src="%(url_prefix)s%(banner_logo)s" alt="GraphicMagick logo" width="107" height="76" />
<span class="title">GraphicsMagick</span>
<form action="http://www.google.com/search">
	<input type="hidden" name="domains" value="www.graphicsmagick.org" />
	<input type="hidden" name="sitesearch" value="www.graphicsmagick.org" />
    <span class="nowrap"><input type="text" name="q" size="25" maxlength="255" />&nbsp;<input type="submit" name="sa" value="Search" /></span>
</form>
</div>
"""


nav_template = """
<div class="navmenu">
<ul>
<li><a href="%(url_prefix)sindex.html">Home</a></li>
<li><a href="%(url_prefix)sproject.html">Project</a></li>
<li><a href="%(url_prefix)sdownload.html">Download</a></li>
<li><a href="%(url_prefix)sREADME.html">Install</a></li>
<li><a href="%(url_prefix)sHg.html">Source</a></li>
<li><a href="%(url_prefix)sNEWS.html">News</a> </li>
<li><a href="%(url_prefix)sutilities.html">Utilities</a></li>
<li><a href="%(url_prefix)sprogramming.html">Programming</a></li>
<li><a href="%(url_prefix)sreference.html">Reference</a></li>
</ul>
</div>
"""

footer_template = """
<hr class="divider">
<div class="footer">
    <p><a href="%(url_prefix)sCopyright.html">Copyright</a>GraphicsMagick Group 2002 - 2015</p>
</div>
"""

def make_footer():
    return footer_template % { 'url_prefix' : url_prefix,
                             }

def make_banner():
    return banner_template % { 'url_prefix' : url_prefix,
                               'banner_logo' : banner_logo,
                             }

def make_nav():
    return nav_template % { 'url_mailinglist' : url_mailinglist,
                            'url_sourceforge' : url_sourceforge,
                            'url_prefix' : url_prefix,
                          }

# These get fixed up by rst2htmldeco.py
nav = None
banner = None
footer = None

# test
if __name__ == '__main__':
    print make_banner()
    print make_nav()
