Welcome to GraphicsMagick.

GraphicsMagick provides a comprehensive collection of utilities,
programming interfaces, and GUIs, to support image processing and 2D
vector rendering.

GraphicsMagick is originally based on ImageMagick from ImageMagick
Studio (which was originally written at Dupont). The goal of Open
ImageMagick is to provide the highest quality product possible while
encouraging open and active participation from all interested
developers. The GraphicsMagick usage license is designed to allow it
to be used for any application, including proprietary or GPLed
applications.

AVAILABILITY

  GraphicsMagick is currently under development.  It may be retrieved
  via CVS using the procedure

  cvs -d ":pserver:anonymous@cvs.simplesystems.org:/GraphicsMagick" login
  [ enter "anonymous" ]

  cvs -d ":pserver:anonymous@cvs.simplesystems.org:/GraphicsMagick" \
   co ImageMagick

DOCUMENTATION

  Open the file ImageMagick.html in a web browser, or refer to
  the manual pages for the display(1), animate(1), montage(1),
  import(1), mogrify(1), identify(1), composite(1), and convert(1)
  utilities. The ImageMagick(1) manual page provides details for utility
  options and will also aid with understanding library interfaces. Also
  read the ImageMagick frequently asked questions in the file
  www/Magick.html.

INSTALLATION

  GraphicsMagick may be compiled from source code for virtually any
  modern Unix system (including Linux and MacOS X), Microsoft Windows,
  MacOS9, and VMS. Installation instructions may be found in the
  following files:

  Unix:
    INSTALL-unix.txt

  Microsoft Windows
    INSTALL-windows.txt

  Macintosh (for MacOS X follow the Unix procedure)
    INSTALL-mac.txt

  VMS
    INSTALL-vms.txt


MAGICK DELEGATES

  To further enhance the capabilities of GraphicsMagick, you may want to
  get these programs or libraries:

    o GraphicsMagick requires the BZLIB library from

        http://sourceware.cygnus.com/bzip2/index.html

      to read and write BZip compressed MIFF images.

    o GraphicsMagick requires ralcgm from

        http://www.agocg.ac.uk/train/cgm/ralcgm.htm

      to read the Computer Graphics Metafile image format (may not
      compile under linux).  You also need Ghostscript (see below).

    o GraphicsMagick requires fig2dev from

        ftp://ftp.x.org/contrib/applications/drawing_tools/transfig

      to read the Fig image format.

    o GraphicsMagick requires the FreeType software, version 2.0 or above,
      available as

        http://www.freetype.org/

      to annotate with TrueType and Postscript Type 1 fonts.

    o GraphicsMagick requires Ghostscript software available from

        http://www.cs.wisc.edu/~ghost/

      to read the Postscript or the Portable Document format. It is used
      to annotate an image when the FreeType library is not used, or an
      X server is not available. See the FreeType library above for
      another means to annotate an image. Note, Ghostscript must support
      the ppmraw device (type gs -h to verify). If Ghostscript is
      unavailable, the Display Postscript extension is used to rasterize
      a Postscript document (assuming you define HasDPS). The DPS
      extension is less robust than Ghostscript in that it will only
      rasterize one page of a multi-page document.

      Ghostscript (released 7.0 and later) may optionally install a
      library (libgs). If this library is installed, GraphicsMagick will
      attempt to use it unless the option --without-gslib is passed to
      configure. Note that Ghostscript provides its own modified version
      of libjpeg and that symbols from this libjpeg may be confused with
      symbols with the stand-alone libjpeg. If conflicts cause JPEG to
      fail, it may be necessary to use Ghostscript's copy of libjpeg for
      GraphicsMagick, and all delegate libraries which depend on libjpeg.

    o GraphicsMagick requires hp2xx available from

         http://www.gnu.org/software/hp2xx/hp2xx.html

      to read the HP-GL image format. Note that HPGL is a plotter file
      format. HP printers usually accept PCL format rather than HPGL
      format.

    o GraphicsMagick requires the LCMS library available from

         http://www.littlecms.com/

      to perform color management.

    o GraphicsMagick requires gnuplot available via anonymous FTP as

         ftp://ftp.dartmouth.edu/pub/gnuplot/gnuplot-3.7.tar.gz

      to read GNUPLOT plot files (with extension gplt).

    o GraphicsMagick requires html2ps available from

         http://www.tdb.uu.se/~jan/html2ps.html

      to read the HTML image format.

    o GraphicsMagick requires the JBIG-Kit software available via
      HTTP from

         http://www.cl.cam.ac.uk/~mgk25/jbigkit/

      or via anonymous FTP as

         ftp://ftp.informatik.uni-erlangen.de/pub/doc/ISO/JBIG/

      to read the JBIG image format.

    o GraphicsMagick requires the Independent JPEG Group's software
      available via anonymous FTP as

         ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz

      to read the JPEG v1 image format.

      Apply this JPEG patch to Independent JPEG Group's source
      distribution if you want to read lossless jpeg-encoded DICOM
      (medical) images:

         ftp://ftp.imagemagick.org/pub/GraphicsMagick/delegates/ljpeg-6b.tar.gz

      Use of lossless JPEG is not encouraged. Unless you have a requirement
      to read lossless jpeg-encoded DICOM images, please disregard the patch.

    o GraphicsMagick requires the JasPer Project's Jasper library available
      via http from

         http://www.ece.uvic.ca/~mdadams/jasper/

      to read and write the JPEG v2 format.

    o GraphicsMagick requires the MPEG utilities from the MPEG Software
      Simulation Group, which are available via anonymous FTP as

         ftp://ftp.mpeg.org/pub/mpeg/mssg/mpeg2vidcodec_v12.tar.gz

      to read or write the MPEG image format.

    o GraphicsMagick requires the PNG library, version 1.0 or above, from

         http://www.libpng.org/pub/png/pngcode.html

      to read the PNG image format.

    o GraphicsMagick requires ra_ppm from Greg Ward's Radiance software
      available from

         http://radsite.lbl.gov/radiance/HOME.html

      to read the Radiance image format.

    o GraphicsMagick requires rawtorle from the Utah Raster Toolkit
      available via anonymous FTP as

         ftp://ftp.cs.utah.edu/pub/dept/OLD/pub/urt-3.1b.tar.Z

      to write the RLE image format (may not compile under linux).

    o GraphicsMagick requires scanimage from

         http://www.mostang.com/sane/
								 
      to import an image from a scanner device.

    o GraphicsMagick requires Sam Leffler's TIFF software available via
      anonymous FTP at

         ftp://ftp.remotesensing.org/pub/libtiff/

      or via HTTP at

         http://www.libtiff.org/

      to read the TIFF image format. It in turn optionally requires the
      JPEG and ZLIB libraries. Support for JPEG, ZIP, and LZW compression
      must be explicitly enabled by editing libtiff Makefiles. The TIFF
      library no longer includes support for LZW compression due to patent
      issues. If you need to use LZW compression, support is available via
      a seperate LZW compression kit (a patch) at the sites listed above.

    o GraphicsMagick requires libwmf 0.2.5 (or 0.2.2) available from

         http://sourceforge.net/projects/wvware/

      to render files in the Windows Meta File (WMF) metafile format
      (16-bit WMF files only, not 32-bit "EMF"). This is the format
      commonly used for Windows clipart (available on CD at your local
      computer or technical book store). WMF support requires the
      FreeType 2 library in order to render TrueType and Postscript fonts.

      While GraphicsMagick uses the libwmflite (parser) component of the
      libwmf package which does not depend on any special libraries, the
      libwmf package as a whole depends on FreeType 2 and either the
      xmlsoft libxml, or expat libraries. Since GraphicsMagick already uses
      libxml (for reading SVG and to retrieve files via HTTP or FTP), it
      is recommended that the options '--without-expat --with-xml' be
      supplied to libwmf's configure script.

      GraphicsMagick's WMF renderer provides some of the finest WMF
      rendering available due its use of antialiased drawing algorithms.
      You may select a background color or texture image to render on.
      For example, "-background '#ffffffff'" renders on a transparent
      background while "-texture plasma:fractal" renders on a fractal
      image.

      The WMF renderer is not capable of using Windows fonts with libwmf
      0.2.2. Windows fonts are fully supported when using libwmf 0.2.5
      or later.

    o GraphicsMagick requires the FlashPIX library version 1.2.0 from
      the Digital Imaging Group in order to support the FlashPIX
      format. The FlashPIX library may be obtained from GraphicsMagick
      anonymous CVS by checking out the 'fpx' module, or retrieving the
      file libfpx-1.2.0.4.tar.gz from the ftp directory

         ftp://ftp.imagemagick.org/pub/GraphicsMagick/delegates/.

    o GraphicsMagick requires an X server for display and animate to work
      properly. Unix systems usually provide an X server as part of
      their standard installation.

      A free X server for Microsoft Windows is available from

         http://sources.redhat.com/win32-x11/

      The Cygwin port of XFree86 may also be used. It is available from

         http://www.cygwin.com/xfree/

      There is a nearly free X server available for Windows and
      Macintosh at

         http://www.microimages.com/freestuf/mix/

    o GraphicsMagick requires libxml available from

         http://xmlsoft.org/

      to read the SVG image format and to retrieve files from over a
      network via FTP and HTTP.

    o GraphicsMagick requires the ZLIB library from

         http://www.gzip.org/zlib/

      to read or write the PNG or Zip compressed MIFF images.

    o GraphicsMagick requires a background texture for the TILE
      format and for the -texture option of montage(1).  You can use
      your own or get samples from

         http://the-tech.mit.edu/KPT/

