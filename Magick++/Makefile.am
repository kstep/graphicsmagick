#
# Top Makefile for Magick++
#
# Copyright Bob Friesenhahn, 1999, 2000
#
AUTOMAKE_OPTIONS = 1.4 dist-zip

# Additional files to distribute
EXTRA_DIST =

SUBDIRS = bin lib tests demo

if HasTXT2HTML
all-local: ../www/Magick++/NEWS.html ../www/Magick++/ChangeLog.html
else
endif

# Build HTML version of news
../www/Magick++/NEWS.html: NEWS
	txt2html -t 'Magick++ News' < $(srcdir)/NEWS  > $(srcdir)/../www/Magick++/NEWS.html
	echo "Please do a 'cvs commit www/Magick++/NEWS.html' to submit updated HTML file"

# Build HTML version of ChangeLog
../www/Magick++/ChangeLog.html: ChangeLog
	txt2html -t 'Magick++ ChangeLog' < $(srcdir)/ChangeLog > $(srcdir)/../www/Magick++/ChangeLog.html
	echo "Please do a 'cvs commit www/Magick++/ChangeLog.html' to submit updated HTML file"
