.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

===================
GraphicsMagick Bugs
===================

The following bugs are known to exist in GraphicsMagick. Please report
any additional bugs to the GraphicsMagick `bug tracker
<http://sourceforge.net/tracker/?group_id=73485>`_ at SourceForge.

 * BMP reader is not working for some obscure low-color packed files.

 * DPX reader/writer does not properly handle subformats where row
   samples spill over the storage word boundary. This means that
   single-channel/grayscale 10-bit DPX files will only work for widths
   evenly divisible by three. (SF 1533184)

 * PSD reader is not working for some files. Can fix by extracting the
   already rendered image from the PSD file rather than the layers.

 * Scitex reader is not working for some files.

 * SVG writer works only if you are particularly luckly.

 * SVG reader mishandles basic units and many other syntax elements (SF
   1231547, 1298606).

 * BlobToImage ignores the value of the 'magick' parameter. I don't know
   how to fix this due to how things work. (SF 1839932).

 * Text annotation options do not work perfectly (SF 1539050, 1539052, 1539059)
