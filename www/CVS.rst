=======================================
GraphicsMagick CVS
=======================================

.. meta::
   :description: GraphicsMagick is a robust collection of tools and libraries to read,
                 write, and manipulate an image in any of the more popular
                 image formats including GIF, JPEG, PNG, PDF, and Photo CD.
                 With GraphicsMagick you can create GIFs dynamically making it
                 suitable for Web applications.  You can also resize, rotate,
                 sharpen, color reduce, or add special effects to an image and
                 save your completed work in the same or differing image format.

   :keywords: GraphicsMagick, Image Magick, Image Magic, PerlMagick, Perl Magick,
              Perl Magic, CineMagick, PixelMagick, Pixel Magic, WebMagick,
              Web Magic, visualization, image processing, software development,
              simulation, image, software, AniMagick, Animagic,  Magick++


.. _CVS program : http://www.cvshome.org/
.. _TortoiseCVS : http://www.tortoisecvs.org/
.. _CVS Home Page : http://www.cvshome.org/
.. _cvs.graphicsmagick.org : http://cvs.graphicsmagick.org/
.. _CVSWeb: http://cvs.graphicsmagick.org/cgi-bin/cvsweb.cgi/

The GraphicsMagick source is available via anonymous CVS. This is a convenient
way for developers from around the country or world to download the
GraphicsMagick source, fix bugs, or add new features.

What is CVS
============
CVS is the Concurrent Version System and is a very popular version control tool
for software projects. It is designed to allow multiple authors to be
able to simultaneously operate on the same source tree. This source tree is
centrally maintained, but each developer has a local mirror of this repository
that they make their changes to.

CVSWeb Access
=============
A `CVSWeb`_ interface is available which may be used to interactively view the
latest versions of files, or the changes to files, using your web browser.

CVS Command Summary
====================
Here is a summary of CVS commands. See the `CVS Home Page`_ for detailed
information and documentation about CVS. The command ``cvs –help-commands`` will
print out all of the available commands. The command ``cvs <command> –help`` will
cause a help message to be printed for the specified <command>.


========  ========================================================================
Action    Result
========  ========================================================================
add       Add a new file or directory to the repository.
checkout  Make a working directory of source files for editing.
commit    Apply changes to the source repository (write access).
diff      Show differences between local files and the source repository.
history   Show reports on cvs commands against the source repository.
log       Display CVS log information.
rdiff     Prepare a collection of differences reflecting changes between release.
status    Show current status of files in the repository and local copies.
update    Bring your working directory up to date with the repository.
========  ========================================================================

Using Anonymous CVS
====================
A live source tree for GraphicsMagick is available from
``cvs.graphicsmagick.org``. This source tree may be retrieved over the network using
the `CVS program`_. Microsoft Windows users may be interested in `TortoiseCVS`_,
which transparently extends the Windows file manager to support CVS. There
are currently two CVS protocols available for use: 'EXT' and 'PSERVER'. The
'EXT' protocol is used on top of encrypted SSH so it is very secure. The
'PSERVER' protocol uses simple TCP and plain-text passwords so it is not
very secure. GraphicsMagick CVS is in the process of transitioning to using
SSH access only (due to security concerns), so using the EXT protocol is
recommended. Some corporate firewalls do not allow outbound PSERVER connections
(to TCP port 2401) while they do allow SSH connections.

The protocol to use is indicated by the prefix of the CVSROOT specification. The
CVSROOT specification may be specified via the CVSROOT environment variable or
as an argument to the cvs -d option. The necessary CVSROOT specifications are
shown in the following table.

========  ===========================================================
Protocol  CVSROOT Specification
========  ===========================================================
EXT       :ext:anoncvs@cvs.graphicsmagick.org:/GraphicsMagick
PSERVER   :pserver:anonymous@cvs.graphicsmagick.org:/GraphicsMagick
========  ===========================================================


To specify the cvs root directly on the command line::

  cvs -d :pserver:anonymous@cvs.graphicsmagick.org:/GraphicsMagick command args...

Or to set as an environment variable::

  export CVSROOT=:ext:anoncvs@cvs.graphicsmagick.org:/GraphicsMagick
  setenv CVSROOT :ext:anoncvs@cvs.graphicsmagick.org:/GraphicsMagick

When using the PSERVER protocol, a CVS login is needed prior to checking out the
sources::

  cvs login

This will ask you for a password. The password is anonymous. Enter it and press
Return. The EXT protocol does not currently require a password when used for
anonymous access.

The CVS program supports a configuration file (``$HOME/.cvsrc``). Many command
options used in the remaining instructions may be set as defaults (so they don't
need to be repeated) by creating a .cvsrc file in your home directory with the
following contents::

  cvs -z3
  update -P -d
  checkout -P
  diff -u

To get the tree and place it in a sub-directory of your current working
directory, issue the command::

  cvs co -P GraphicsMagick

Or to save bandwidth get the compressed version, type::

  cvs -z3 co -P GraphicsMagick

Note that adding ``-r GraphicsMagick-1_2`` requests the sources from the
GraphicsMagick 1.2 release branch.

To update the sources within a checked out directory execute::

  cvs update -P -d

and only the files which have changed will be updated.

The available modules relating to GraphicsMagick are:

*  ``GraphicsMagick`` (core sources)
*  ``GraphicsMagick-NT`` (everything needed for Microsoft Windows)
*  ``GraphicsMagick-World`` (everything related to GraphicsMagick)

To check out the GraphicsMagick source for Microsoft Windows, try this command
line::

  cvs -z3 co -P -r GraphicsMagick-1_2 GraphicsMagick-NT

If you require a specific release of GraphicsMagick (e.g. 1.2.2), you may check
it out like::

 cvs -z3 co-r GraphicsMagick-1_2_2 -P GraphicsMagick

where the ``GraphicsMagick-1_2_2`` is the release tag. You can use::

  cvs status -v README.txt

from within an existing checked out directory to see what branch and release
tags are available.

You can use::

  cvs -z3 update -r GraphicsMagick-1_2_2

from within an existing checked out directory to move it up (or down) to that
branch or release tag.

Checking out from release tag, or updating to a release tag, causes the "sticky"
flag to be set on checked out files so that further update requests will only
cause an update if the tag is a branch tag, and is a newer version on the
branch. To clear the sticky flags and update to the most current version on the
trunk, use::

  cvs -z3 update -A

Once a release has been tagged, checking out or updating using a release tag is
the only reliable way to obtain release files from CVS since the default is to
check out the most current sources on the requested branch, which may not be
stable or released.

Using compression levels over ``-z3`` may slow the CVS transfer since they require
the CVS server to work much harder, yet don't compress the data much more.

CVS Software
============
The best place to look for the latest version of CVS is at the `CVS Home Page`_.
There is also a lot of information and documentation about CVS available on
numerous other web sites.


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2008
