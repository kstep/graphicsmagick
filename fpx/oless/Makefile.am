
# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.4 foreign

#INCLUDES = -I$(top_srcdir)/basics -I$(top_srcdir)/fpx -I$(top_srcdir)/ole \
#	-I$(top_srcdir)/ri_image -I$(top_srcdir)/jpeg -I$(top_srcdir)/oless \
#	-I$(top_srcdir)/oless/h
INCLUDES =  \
	-I$(top_srcdir)/oless -I$(top_srcdir)/oless/h

# A convenience library
noinst_LTLIBRARIES = libOless.la

# Sources to compile into library
libOless_la_SOURCES = \
  ascii.cxx cdocfile.cxx chinst.cxx dffuncs.cxx dfiter.cxx dfstream.cxx \
  difat.cxx dir.cxx dirp.cxx docfile.cxx entry.cxx expdf.cxx expiter.cxx \
  expst.cxx fat.cxx funcs.cxx header.cxx iter.cxx mem.cxx msf.cxx \
  msfiter.cxx mstream.cxx page.cxx refilb.cxx rexpdf.cxx sstream.cxx \
  storage.cxx time.cxx vect.cxx wchar.c

# Additional files to distribute
EXTRA_DIST = dfhead.cxx exphead.cxx msfhead.cxx

# Headers which are not installed but which are distributed
noinst_HEADERS = \
  ascii.hxx byteordr.hxx dbg.hxx dfbasis.hxx expdf.hxx expiter.hxx \
  expst.hxx iter.hxx logfile.hxx mread.hxx time.hxx

# Non-Automake subdirectories to distribute
DISTDIRS = doc h props test
dist-hook:
	( \
	  builddir=`pwd` ; \
	  cd $(srcdir) && \
	  ( \
	    for dir in $(DISTDIRS) ; do \
	      find $$dir -depth -print | egrep -v '(~$$)|(/CVS)|(/\.#)|(/\.deps)' | cpio -pdum $$builddir/$(distdir) 2> /dev/null ; \
	    done \
	  ) \
	)
