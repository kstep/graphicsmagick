#!/bin/csh
# cd ~/ImageMagick-5.4.1/magick
# ../scripts/api.pl
api.pl
foreach file (*.pod)
  pod2html -netscape $file >! $file:r.html
  pod2text  $file >! $file:r.txt
end
replace '<HTML>' \
'<\!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"<HTML>' *.html
replace '<HTML>' \
' "http://www.w3.org/TR/html4/loose.dtd"><HTML>' *.html
replace '<HEAD>' \
'<HEAD><meta http-equiv="Content-Type" content="text/html; charset=utf-8">' *.html
replace '<BODY>' '<body bgcolor="#FFFFFF" text="#000000" link="#0085c0" alink="#0085c0"  vlink="#800080">' *.html
replace '<DT>' '<dd><p></p><dt>' *.html
replace '<DL>' '<DL><DT><DD><DL>' *.html
replace '</DL>' '</DL></DL>' *.html
replace '</LI>' '' *.html
replace '>o ' '>' *.html
replace 'unsignedint' 'unsigned int' *.html
# mv *.html ../www/api
/bin/rm *.pod
/bin/rm pod2*
# cd ~/ImageMagick-5.4.5/www/api
# replace 'cristy\@mystic.es.dupont.com' 'magick-user\@imagemagick.org' *.html
replace '<LINK REV="made" HREF="mailto:bhcompile\@daffy.perf.redhat.com">' '<link rel="StyleSheet" href="../magick.css" type="text/css">' *.html
#/bin/cp -f *.html /home/cristy/ImageMagick/www/api
