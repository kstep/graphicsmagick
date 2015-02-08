.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=============================
Magick::PixelPacket Structure
=============================

The PixelPacket structure is used to represent DirectClass pixels in
GraphicsMagick. GraphicsMagick may be compiled to support 32, 64, or
even 128 bit pixels. The size of PixelPacket is controlled by the
value of the QuantumDepth define. The default build depth is 32 bit
pixels, which provides minumum ("web standard") accuracy, least memory
consumption, and best performance.  If deeper images need to be
supported or more mathematical accuracy is desired, then
GraphicsMagick may be compiled with QuantumDepth=16 or
QuantumDepth=32.

The following table shows the relationship between QuantumDepth, the
type of Quantum, and the overall PixelPacket size:

.. table:: Effect Of QuantumDepth Values

   ============  ===============  ================
   QuantumDepth  Quantum Type     PixelPacket Size
   ============  ===============  ================
   8             unsigned char    32 bits
   16            unsigned short   64 bits
   32            unsigned int     128 bits
   ============  ===============  ================

The members of the PixelPacket structure, and their interpretation,
are shown in the following table:

.. table:: PixelPacket Structure Members

   =======  =======  =============  =====================  ===================
   Member    Type                        Interpretation
   -------  -------  ---------------------------------------------------------
   \                 RGBColorspace  RGBColorspace + matte  CMYKColorspace
   =======  =======  =============  =====================  ===================
   red      Quantum  Red            Red                    Cyan
   green    Quantum  Green          Green                  Magenta
   blue     Quantum  Blue           Blue                   Yellow
   opacity  Quantum  Ignored        Opacity                Black
   =======  =======  =============  =====================  ===================

Note that opacity is stored inverted from most other software
(i.e. maximum value is completely transparent and minum value is
totally opaque).

Note that for CMYKColorspace + matte (CMYKA), the opacity is stored in
the assocated IndexPacket.

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| Bob Friesenhahn 1999 - 2015
