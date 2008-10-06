# vim:ts=4:sw=4:expandtab:tw=100:

#* Author: Mark Mitchell

__doc__ = """
HTML fragments which are used as the banner and navigation menu in
GraphicsMagick web site pages.
"""


url_mailinglist = "http://sourceforge.net/mail/?group_id=73485"
url_sourceforge = "http://sourceforge.net/projects/graphicsmagick/"

banner_logo = 'images/gm-107x76.png' # relative to top directory


# banner_template and nav_template contain chunk which has to be created at
# runtime: the path to the image directory, and the path to the top directory.
# So, rst2htmldeco.py imports html_fragments and changes url_prefix to the
# appropriate value.  When docutils_htmldeco_writer imports html_fragments, this
# url_prefix attribute should already be fixed up.

url_prefix = '' # trailing slash always needed

banner_template = """
<div class="banner">
<span>
<img src="%(url_prefix)s%(banner_logo)s" alt="GraphicMagick logo" width="107" height="76" />
<span class="title">GraphicsMagick</span>
<form action="http://www.google.com/search">
    Search&nbsp;site
	<input type=hidden name="domains" value="www.graphicsmagick.org" />
	<input type=hidden name="sitesearch" value="www.graphicsmagick.org" />
    <input type=text name="q" size="25" maxlength="255" />
    <input type=submit name="sa" value="Search" />
</form>
</span>
</div>
"""


nav_template = """
<div class="navmenu">
<ul>
<li><a href="%(url_prefix)sindex.html">Home</a></li>
<li><a href="%(url_prefix)sCopyright.html">License</a></li>
<li><a href="%(url_prefix)smission.html">Mission</a></li>
<li><a href="%(url_prefix)sdownload.html">Download</a></li>
<li><a href="%(url_prefix)sREADME.html">Installation</a></li>
<li><a href="%(url_prefix)sFAQ.html">FAQ</a></li>
<li><a href="%(url_prefix)sNEWS.html">News</a> </li>
<li><a href="%(url_prefix)sformats.html">Formats</a></li>
</ul>
</div>
<div class="navmenu">
<ul>
<li><a href="%(url_prefix)sprocess.html">Process</a></li>
<li><a href="%(url_prefix)scontribute.html">Contribute</a></li>
<li><a href="%(url_prefix)sCVS.html">CVS</a></li>
<li><a href="%(url_mailinglist)s" target="top_">Mailing Lists</a></li>
<li><a href="%(url_prefix)sChangelog.html">ChangeLog</a></li>
<li><a href="%(url_sourceforge)s" target="top_">Report Bugs</a></li>
<li><a href="%(url_prefix)sutilities.html">Utilities</a></li>
<li><a href="%(url_prefix)sprogramming.html">Programming</a></li>
<li><a href="%(url_prefix)slinks.html">Links</a></li>
</ul>
</div>
"""


footer_template = """
<hr class="divider">
<div class="footer">
    <p><a href="%(url_prefix)sCopyright.html">Copyright</a>GraphicsMagick Group 2002 - 2008</p>
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
