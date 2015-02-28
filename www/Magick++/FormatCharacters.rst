.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=========================
Special Format Characters
=========================

The Magick::Image methods `annotate <Image.html#annotate>`_, `draw
<Image.html#draw>`_, `label <Image.html#label>`_, and the template
function `montageImages <STL.html#montageImages>`_ support special
format characters contained in the argument text. These format
characters work similar to C's printf. Whenever a format character
appears in the text, it is replaced with the equivalent attribute
text. The available format characters are shown in the following
table:

.. table:: Format Characters

   ================  ===========================
   Format Character  Description
   ================  ===========================
   %b                file size
   %d                directory
   %e                filename extension
   %f                filename
   %h                height
   %m                magick (e.g GIF)
   %p                page number
   %s                scene number
   %t                top of filename
   %w                width
   %x                x resolution
   %y                y resolution
   \n                newline
   \r                carriage return
   ================  ===========================

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015

