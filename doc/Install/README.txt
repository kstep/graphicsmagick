  "I swear by my life and my love of it that I will never live for the
   sake of another man, nor ask another man to live for mine"

                    John Galt in "Atlas Shrugged", by Ayn Rand


AUTHOR

  The author is magick@mystic.es.dupont.com.  This software is NOT
  shareware.  However, I am interested in who might be using it.
  Please consider sending me a picture postcard of the area where
  you live.  Send postcards to

    ImageMagick Studio
    P.O. Box 40
    Landenberg, PA  19350
    USA

  I'm also interested in receiving currency or stamps from around the
  world for my collection.


AVAILABILITY

  ImageMagick is available as

    ftp://ftp.imagemagick.org/pub/ImageMagick/ImageMagick-5.4.8.tar.gz

  ImageMagick client executables are available for some platforms. See

    ftp://ftp.imagemagick.org/pub/ImageMagick/binaries
    ftp://ftp.imagemagick.org/pub/ImageMagick/mac
    ftp://ftp.imagemagick.org/pub/ImageMagick/win2k
    ftp://ftp.imagemagick.org/pub/ImageMagick/vms
    ftp://ftp.imagemagick.org/pub/ImageMagick/linux

  I want ImageMagick to be of high quality, so if you encounter a
  problem I will investigate.  However, be sure you are using the
  most recent version from

    ftp://ftp.imagemagick.org/pub/ImageMagick

  before submitting any bug reports or suggestions.  Report
  any problems via the web-based reporting facility at

    http://studio.imagemagick.org/mailman/listinfo/magick-bugs.

WWW

  The official ImageMagick WWW page is

    http://www.imagemagick.org/

  To use display as your external image viewer, edit the global
  mail-cap file or your personal mail-cap file .mailrc (located at your
  home directory) and put this entry:

    image/*; display %s


MAILING LIST

  There is a mailing list for discussions and bug reports about
  ImageMagick.  To subscribe send the message

    subscribe magick-user

  to majordomo@imagemagick.org.  You will receive a welcome message
  which tells you how to post messages to the list,
  magick-user@imagemagick.org.


UNIX/Cygwin/MinGW COMPILATION

  Note:

    Platform specific notes regarding specific operating systems may
    be found in the PLATFORMS.txt file.  This file (README.txt) provides
    generic instructions which work in most common cases.  Additional
    notes regarding Cygwin & MinGW are provided later in this file.

  Type:

    gzip -dc ImageMagick-5.4.8.tar.gz | tar xvf -
    cd ImageMagick-5.4.8

  If you do not have gzip(1), the source for the gzip package is
  available as a shell archive at

    ftp://ftp.gnu.org/gnu/gzip/gzip-1.2.4a.shar

  or as a tar archive at

    ftp://ftp.gnu.org/gnu/gzip/gzip-1.2.4a.tar

  Use 'configure' to automatically configure, build, and install
  ImageMagick. The configure script may be executed from the ImageMagick
  source directory (e.g ./configure) or from a seperate build directory by
  specifying the full path to configure (e.g.
  /src/ImageMagick-5.4.8/configure). The advantage of using a seperate
  build directory is that multiple ImageMagick builds may share the same
  ImageMagick source directory while allowing each build to use a unique
  set of options.

  If you are willing to accept configure's default options, and build from
  within the source directory, type:

      ./configure

  and watch the configure script output to verify that it finds everything
  that you think it should. If it does not, then adjust your environment
  so that it does.

  By default, `make install' will install the package's files
  in `/usr/local/bin', `/usr/local/man', etc. You can specify an
  installation prefix other than `/usr/local' by giving `configure'
  the option `--prefix=PATH'.  This is valuable in case you don't have
  privileges to install under the default paths or if you want to install
  in the system directories instead.

  If you are not happy with configure's choice of compiler, compilation
  flags, or libraries, you can give `configure' initial values for
  variables by specifying them on the configure command line, e.g.:

      ./configure CC=c89 CFLAGS=-O2 LIBS=-lposix

  Options which should be common to packages installed under the same
  directory heirarchy may be supplied via a 'config.site' file located
  under the installation prefix via the path ${prefix}/share/config.site
  where ${prefix} is the installation prefix.  This file is used for all
  packages installed under that prefix.  This is an example config.site
  file:

  # Configuration values for all packages installed under this prefix
  CC=gcc
  CXX=c++
  CPPFLAGS='-I/usr/local/include'
  LDFLAGS='-L/usr/local/lib -R/usr/local/lib'

  When the 'config.site' file is being used to supply configuration
  options, configure will issue a message similar to:

    configure: loading site script /usr/local/share/config.site

  The configure variables you should be aware of are:

      CC          Name of C compiler (e.g. 'cc -Xa') to use
      CXX         Name of C++ compiler to use (e.g. 'CC')
      CFLAGS      Compiler flags (e.g. '-g -O2') to compile C code
      CXXFLAGS    Compiler flags (e.g. '-g -O2') to compile C++ code
      CPPFLAGS    Include paths (-I/somedir) to look for header files
      LDFLAGS     Library paths (-L/somedir) to look for libraries
                  Systems that support the notion of a library run-path
                  may require an additional argument in order to find
                  shared libraries at run time. For example, the Solaris
                  linker requires an argument of the form '-R/somedir',
                  some Linux systems will work with '-rpath /somedir',
                  while some other Linux systems who's gcc does not pass
                  -rpath to the linker require an argument of the form
                  '-Wl,-rpath,/somedir'.
      LIBS        Extra libraries (-lsomelib) required to link

  Any variable (e.g. CPPFLAGS or LDFLAGS) which requires a directory
  path must specify an absolute path rather than a relative path.

  Configure can usually find the X include and library files
  automatically, but if it doesn't, you can use the `configure' options
  `--x-includes=DIR' and `--x-libraries=DIR' to specify their locations.

  The configure script provides a number of ImageMagick specific
  options.  When disabling an option --disable-something is equivalent to
  specifying --enable-something=no and --without-something is equivalent
  to --with-something=no.  The configure options are as
  follows (execute 'configure --help' to see all options).

  Optional Features:
   --enable-ccmalloc       enable 'ccmalloc' memory debug support (default disabled)
   --enable-prof           enable 'prof' profiling support (default disabled)
   --enable-gprof          enable 'gprof' profiling support (default disabled)
   --enable-gcov           enable 'gcov' profiling support (default disabled)
   --disable-largefile     disable support for large (64 bit) file offsets
   --enable-lzw            enable LZW support (default disabled)
   --disable-16bit-pixel   disable 16 bit/quantum pixels (default enabled)

  Optional Packages:
   --with-modules          enable support for dynamically loadable modules
   --with-cache            set pixel cache threshhold (defaults to available memory)
   --without-threads       disable threads support
   --without-frozenpaths   disable frozen delegate paths
   --without-magick-plus-plus disable build/install of Magick++
   --without-perl          disable build/install of PerlMagick
        or
   --with-perl=PERL        use specified Perl binary to configure PerlMagick
   --with-perl-options=OPTIONS  options to pass on command-line when
                           generating PerlMagick's Makefile from Makefile.PL
   --without-bzlib         disable BZLIB support
   --without-dps           disable Display Postscript support
   --without-fpx           disable FlashPIX support
   --without-gslib         disable Ghostscript library support
   --with-hdf              enable HDF support
   --without-jbig          disable JBIG support
   --without-jpeg          disable JPEG support
   --without-jp2           disable JPEG v2 support
   --without-lcms          disable LCMS support
   --without-mpeg2         disable MPEG support
   --without-png           disable PNG support
   --without-tiff          disable TIFF support
   --without-ttf           disable TrueType support
   --without-wmf           disable WMF support
   --with-fontpath         prepend to default font search path
   --with-gs-font-dir      directory containing Ghostscript fonts
   --with-windows-font-dir directory containing MS-Windows fonts
   --without-xml           disable XML support
   --without-zlib          disable ZLIB support
   --with-x                use the X Window System
   --with-share-path=DIR   Alternate path to share directory
                           (default share/ImageMagick)
   --with-libstdc=DIR      use libstdc++ in DIR (for GNU C++)

  ImageMagick options represent either features to be enabled, disabled,
  or packages to be included in the build.  When a feature is enabled (via
  --enable-something), it enables code already present in ImageMagick.
  When a package is enabled (via --with-something), the configure script
  will search for it, and if is is properly installed and ready to use
  (headers and built libraries are found by compiler) it will be included
  in the build.  The configure script is delivered with all features
  disabled and all packages enabled. In general, the only reason to
  disable a package is if a package exists but it is unsuitable for the
  build (perhaps an old version or not compiled with
  the right compilation flags).

  Several configure options require special note:

    o --enable-shared: the shared libraries are built. Shared
      libraries are preferred because they allow programs to share
      common code, making the individual programs much smaller. In
      addition shared libraries are required in order for PerlMagick to
      be dynamically loaded by an installed PERL (otherwise an additional
      PERL (PerlMagick) must be installed. This option is not the default
      because all libraries used by ImageMagick must also be dynamic
      libraries if ImageMagick itself is to be dynamically loaded (such
      as for PerlMagick).

      ImageMagick built with delegates (see MAGICK PLUG-INS below) can
      pose additional challenges. If ImageMagick is built using static
      libraries (the default without --enable-shared) then delegate
      libraries may be built as either static libraries or shared
      libraries. However, if ImageMagick is built using shared libraries,
      then all delegate libraries must also be built as shared libraries.
      Static libraries usually have the extension .a, while shared
      libraries typically have extensions like .so, .sa, or .dll. Code in
      shared libraries normally must compiled using a special compiler
      option to produce Position Independent Code (PIC). The only time
      this is not necessary is if the platform compiles code as PIC by
      default.

      PIC compilation flags differ from vendor to vendor (gcc's is
      -fPIC). However, you must compile all shared library source with
      the same flag (for gcc use -fPIC rather than -fpic). While static
      libraries are normally created using an archive tool like 'ar',
      shared libraries are built using special linker or compiler
      options (e.g. -shared for gcc).

      Building shared libraries often requires subtantial hand-editing of
      Makefiles and is only recommended for those who know what they are
      doing.

      If --enable-shared is not specified, a new PERL interpreter
      (PerlMagick) is built which is statically linked against the
      PerlMagick extension. This new interpreter is installed into the
      same directory as the ImageMagick utilities. If --enable-shared
      is specified, the PerlMagick extension is built as a dynamically
      loadable object which is loaded into your current PERL interpreter
      at run-time. Use of dynamically-loaded extensions is preferable over
      statically linked extensions so --enable-shared should be specified
      if possible (note that all libraries used with ImageMagick must
      be shared libraries!).

    o --disable-static: static archive libraries (with extension .a)
      are not built.  If you are building shared libraries, there
      is little value to building static libraries. Reasons to build
      static libraries include: 1) they can be easier to debug; 2) the
      clients do not have external dependencies (i.e. libMagick.so); 3)
      building PIC versions of the delegate libraries may take additional
      expertise and effort; 4) you are unable to build shared libraries.

    o --with-modules: image format coders are built as loadable modules
      which are installed in [prefix]/lib/ImageMagick/modules/coders.
      Support for modules allows the formats supported by ImageMagick
      to be extended by simply adding additional modules to the module
      search path.  The modules option is only available in conjunction
      with --enable-shared.  If --enable-shared is not also specified,
      then support for modules is disabled.

    o --enable-lzw: Unisys claims a patent on the algorithm supporting
      LZW compression (e.g. used by GIF and TIFF). To avoid possibly
      infringing on this patent, support for LZW is disabled by default.
      With LZW support, GIF files written by ImageMagick will be much
      larger than expected. Note that the TIFF library must be patched in
      order to support LZW compression, and that this support must be
      explicitly enabled in the libtiff Makefiles.

    o --disable-16bit-pixel: By default ImageMagick represents images
      internally using a sixteen-bit pixel quantum (the size of the
      red, green, blue, and alpha pixel components). The definition
      QuantumDepth=8 is applied when ImageMagick is built, allowing
      RGBA values to range from 0 to 255 rather than 0 to 65535. Use
      of sixteen-bit pixel quantums typically causes ImageMagick to run
      about 30% slower (and take twice as much memory) than when it is
      built to support eight-bit pixel quantums. Those who value
      performance over accuracy may specify --disable-16bit-pixel.
      
    o --without-magick-plus-plus: Disable building Magick++, the C++
      application programming interface to ImageMagick. A suitable C++
      compiler is required in order to build Magick++. Specify the CXX
      configure variable to select the C++ compiler to use (default
      "g++"), and CXXFLAGS to select the desired compiler opimization
      and debug flags (default "-g -O2"). Antique C++ compilers will
      normally be rejected by configure tests so specifying this option
      should only be necessary if Magick++ fails to compile.

    o --without-frozenpaths: Normally full paths to external programs
      (based on searching the PATH set when configure is run) will be
      saved to the delegates.mgk file. Specify this option to disable
      saving full paths to programs.

    o --without-threads: By default, the ImageMagick library is compiled
      with multi-thread support.  If this is undesireable, then specify
      --without-threads.

    o --with-cache: Specify a different image pixel cache threshold
      using the --with-cache option. When ImageMagick will have more image
      pixel data in memory than the cache threshold setting, additional
      images are cached on disk. Since memory is much faster than disk,
      it is usually better to use memory rather than disk for the pixel
      cache. The default cache threshold is 2047MB.  Small memory machines
      may want to decrease the threshold.

    o --disable-largefile: By default, ImageMagick is compiled with
      support for large (> 2GB on a 32-bit CPU) files if the operating
      system supports large files.  All applications which use the
      ImageMagick library must then also include support for large
      files. By disabling support for large files via --disable-largefile,
      dependent applications do not require special compilation options
      for large files in order to use the library.

    o --without-perl: By default, PerlMagick is conveniently compiled
      and installed as part of ImageMagick's normal "configure", "make",
      "make install" process.. When --without-perl is specified, you must
      first install ImageMagick, change to the PerlMagick subdirectory,
      build, and finally install PerlMagick. Note, PerlMagick is
      configured even if --without-perl is specified. If the argument
      --with-perl=/path/to/perl is supplied, then /path/to/perl will be
      taken as the PERL interpreter to use. This is important in case the
      'perl' executable in your PATH is not PERL5, or is not the PERL
      you want to use.

    o --with-perl-options: The PerlMagick module is normally installed
      using the Perl interpreter's installation PREFIX, rather than
      ImageMagick's. If ImageMagick's installation prefix is not
      the same as PERL's PREFIX, then you may find that PerlMagick's
      'make install' step tries to install into a directory tree that
      you don't have write permissions to. This is common when PERL is
      delivered with the operating system or on Internet Service Provider
      (ISP) web servers. If you want PerlMagick to install elsewhere,
      then provide a PREFIX option to PERL's configuration step via
      "--with-perl-options=PREFIX=/some/place". Other options
      accepted by MakeMaker are 'LIB', 'LIBPERL_A', 'LINKTYPE', and
      'OPTIMIZE'. See the ExtUtils::MakeMaker(3) manual page for more
      information on configuring PERL extensions.

    o --without-x: By default, ImageMagick will use X11 libraries if
      they are available. When --without-x is specified, use of X11 is
      disabled. The display, animate, and import programs are not built
      or installed. The remaining programs have reduced functionality
      such as no access to X11 fonts (consider using Postscript or
      TrueType fonts instead).

    o --with-gs-font-dir: Specify the directory containing the
      Ghostscript Postscript Type 1 font files (e.g. "n022003l.pfb") so
      that they can be rendered using the FreeType library. If the font
      files are installed using the default Ghostscript installation paths
      (${prefix}/share/ghostscript/fonts), they should be discovered
      automatically by configure and specifying this option is not
      necessary. Specify this option if the Ghostscript fonts fail to
      be located automatically, or the location needs to be overridden.

    o --with-windows-font-dir: If configured under a Unix emulation
      environment like Cygwin configure should automatically locate the
      MS-Windows system font directory. If configure is not running
      under MS-Windows yet MS-Windows-compatible fonts are available
      use --with-windows-font-dir=/path to specify the directory where
      the fonts are installed.

  Building under Cygwin

    ImageMagick may be built under the Windows '95-XP Cygwin
    Unix-emulation environment available for free from

      http://www.cygwin.com/

    X11R6 for Cygwin is available from

      http://xfree86.cygwin.com/

    It is strongly recommended that the
    X11R6 package be installed since this enables ImageMagick's X11
    support (animate, display, and import will work) and it includes the
    Freetype v2 DLL required to support TrueType and Postscript Type 1
    fonts. Make sure that /usr/X11R6/bin is in your PATH prior to running
    configure. If you are using Cygwin version 1.3.9 or later, you may
    specify the configure option '--enable-shared' to build Cygwin DLLs.
    Specifying '--enable-shared' is required if you want to build
    PerlMagick under Cygwin because Cygwin does not provide the libperl.a
    static library required to create a static PerlMagick.

  Building under MinGW & MSYS

    ImageMagick may be built using the free MinGW ("Minimalistic GNU for
    Windows") package version 1.1, available from

      http://www.mingw.org

    which consists of a GNU-based (e.g. gcc) compilation toolset plus
    headers and libraries required to build programs which are entirely
    based on standard Microsoft Windows DLLs. MSYS provides a Unix-like
    console shell window with sufficient functionality to run the
    ImageMagick configure script and execute make.

    Unlike the Cygwin build which creates programs based on a
    Unix-emulation DLL, and which uses Unix-style paths to access
    Windows files, the MinGW build creates native Windows console
    applications similar to the Visual C++ build.

    Please note that since the MinGW build is very new, some aspects of
    the installation may vary from Windows user's expectations, and that
    only a static build (no DLLs or modules) is currently supported.

    Once MinGW & MSYS have been installed, start the MSYS console (via
    the MSYS icon on the Windows desktop) and follow the Unix configure
    and build instructions. Note that the default installation prefix is
    "/usr/local" which installs the package into a MSYS directory. To
    install outside of the MSYS directory tree, you may specify an
    installation prefix like "/c/ImageMagick" which causes the package
    to be installed under the Windows directory "C:\ImageMagick". The
    installation directory structure will look very much like the Unix
    installation layout (e.g. "C:\ImageMagick\bin",
    "C:\ImageMagick\lib", "C:\ImageMagick\share", etc.). Any additional
    delegate libraries (e.g. libpng) will need to be built under MinGW
    in order to be used.

  Dealing with configuration failures:

    While configure is designed to ease installation of ImageMagick, it
    often discovers problems that would otherwise be encountered later
    when compiling ImageMagick. The configure script tests for headers
    and libraries by executing the compiler (CC) with the specified
    compilation flags (CFLAGS), pre-processor flags (CPPFLAGS), and linker
    flags (LDFLAGS). Any errors are logged to the file 'config.log'. If
    configure fails to discover a header or library please review this
    log file to determine why, however, please be aware that *errors
    in the config.log are normal* because configure works by trying
    something and seeing if it fails. An error in config.log is only a
    problem if the test should have passed on your system. After taking
    corrective action, be sure to remove the 'config.cache' file before
    running configure so that configure will re-inspect the environment
    rather than using cached values.

    Common causes of configure failures are: 1) a delegate header is
    not in the header include path (CPPFLAGS -I option); 2) a delegate
    library is not in the linker search/run path (LDFLAGS -L/-R option);
    3) a delegate library is missing a function (old version?); 4)
    compilation environment is faulty.

    If all reasonable corrective actions have been tried and the
    problem appears to be due to a flaw in the configure script,
    please send a bug report to the configure script maintainer
    (currently bfriesen@simple.dallas.tx.us). All bug reports should
    contain the operating system type (as reported by 'uname -a')
    and the compiler/compiler-version. A copy of the configure script
    output and/or the config.log file may be valuable in order to find
    the problem. If you send a config.log, please also send a script of
    the configure output and a description of what you expected to see
    (and why) so the failure you are observing can be identified and
    resolved.

  Makefile Build Targets

    Once ImageMagick is configured, these standard build targets are
    available from the generated Makefiles:
  
       'make'
          Build the package
  
       'make install'
          Install the package
  
       'make check'
          Run tests using the installed ImageMagick ('make install' must
          be done first!).
  
       'make clean'
          Remove everything in the build directory created by 'make'
  
       'make distclean'
          Remove everything in the build directory created by 'configure'
          and 'make'. This is useful if you want to start over from scratch.
  
       'make uninstall'
     	Remove all files from the system which are (or would be)
     	installed by 'make install' using the current configuration.
          Note that this target is imperfect for PerlMagick since Perl
          no longer supports an 'uninstall' target.

  Build & Install:

    Now that ImageMagick is configured, type

       make

    to build the package and

       make install

    to install it.

  Verifying The Build:

    To confirm your installation of the ImageMagick distribution was
    successful, type:
  
        ./utilities/display

    The ImageMagick logo is displayed.
  
    If the image colors are not correct use this command:
  
        ./utilities/display -visual default
  
    For a more serious test, you may run the ImageMagick test suite by
    typing
  
        make check

    Note that due to differences between the developer's environment and
    your own it is possible that some tests may be indicated as failed
    even though the results are ok. Differences between the developer's
    environment environment and your own may include the compiler, the
    CPU type, and the library versions used. The ImageMagick developers
    use the current release of all dependent libraries.

  Documentation:

    Be sure to read the manual pages for the display(1), animate(1),
    montage(1), import(1), mogrify(1), identify(1), composite(1), and
    convert(1) utilities. The ImageMagick(1) manual page provides details
    for utility options and will also aid with understanding library
    interfaces. Also read the ImageMagick frequently asked questions in
    the file www/Magick.html.

  X11 Resources:

    Place display(1) X application defaults in
    /usr/lib/X11/app-defaults/Display or in a directory specified by the
    XUSERFILESEARCHPATH environment variable. Use the appropriate name
    for other clients (e.g. Animate, Montage, etc). To execute display(1)
    from as a menu item of any window manager (olwm, mwm, twm, etc), use

       display logo:Untitled

MAGICK DELEGATES

  To further enhance the capabilities of ImageMagick, you may want to
  get these programs or libraries:

    o ImageMagick requires the BZLIB library from

        http://sourceware.cygnus.com/bzip2/index.html

      to read and write BZip compressed MIFF images.

    o ImageMagick requires ralcgm from

        http://www.agocg.ac.uk/train/cgm/ralcgm.htm

      to read the Computer Graphics Metafile image format (may not
      compile under linux).  You also need Ghostscript (see below).

    o ImageMagick requires fig2dev from

        ftp://ftp.x.org/contrib/applications/drawing_tools/transfig

      to read the Fig image format.

    o ImageMagick requires the FreeType software, version 2.0 or above,
      available as

        http://www.freetype.org/

      to annotate with TrueType and Postscript Type 1 fonts.

    o ImageMagick requires Ghostscript software available from

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
      library (libgs). If this library is installed, ImageMagick will
      attempt to use it unless the option --without-gslib is passed to
      configure. Note that Ghostscript provides its own modified version
      of libjpeg and that symbols from this libjpeg may be confused with
      symbols with the stand-alone libjpeg. If conflicts cause JPEG to
      fail, it may be necessary to use Ghostscript's copy of libjpeg for
      ImageMagick, and all delegate libraries which depend on libjpeg.

    o ImageMagick requires the NCSA HDF5 library available via anonymous
      FTP as

         ftp://ftp.ncsa.uiuc.edu/HDF/HDF/HDF_Current

      to read and write the HDF image format.  By default HDF support is
      disabled because HDF5 is not yet complete.

    o ImageMagick requires hp2xx available from

         http://www.gnu.org/software/hp2xx/hp2xx.html

      to read the HP-GL image format. Note that HPGL is a plotter file
      format. HP printers usually accept PCL format rather than HPGL
      format.

    o ImageMagick requires the LCMS library available from

         http://www.littlecms.com/

      to perform color management.

    o ImageMagick requires gnuplot available via anonymous FTP as

         ftp://ftp.dartmouth.edu/pub/gnuplot/gnuplot-3.7.tar.gz

      to read GNUPLOT plot files (with extension gplt).

    o ImageMagick requires html2ps available from

         http://www.tdb.uu.se/~jan/html2ps.html

      to read the HTML image format.

    o ImageMagick requires the JBIG-Kit software available via
      HTTP from

         http://www.cl.cam.ac.uk/~mgk25/jbigkit/

      or via anonymous FTP as

         ftp://ftp.informatik.uni-erlangen.de/pub/doc/ISO/JBIG/

      to read the JBIG image format.

    o ImageMagick requires the Independent JPEG Group's software
      available via anonymous FTP as

         ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz

      to read the JPEG image format.

      Apply this JPEG patch to Independent JPEG Group's source
      distribution if you want to read lossless jpeg-encoded DICOM
      (medical) images:

         ftp://ftp.imagemagick.org/pub/ImageMagick/delegates/ljpeg-6b.tar.gz

      Use of lossless JPEG is not encouraged. Unless you have a requirement
      to read lossless jpeg-encoded DICOM images, please disregard the patch.

    o ImageMagick requires the MPEG utilities from the MPEG Software
      Simulation Group, which are available via anonymous FTP as

         ftp://ftp.mpeg.org/pub/mpeg/mssg/mpeg2vidcodec_v12.tar.gz

      to read or write the MPEG image format.

    o ImageMagick requires the PNG library, version 1.0 or above, from

         http://www.libpng.org/pub/png/pngcode.html

      to read the PNG image format.

    o ImageMagick requires ra_ppm from Greg Ward's Radiance software
      available from

         http://radsite.lbl.gov/radiance/HOME.html

      to read the Radiance image format.

    o ImageMagick requires rawtorle from the Utah Raster Toolkit
      available via anonymous FTP as

         ftp://ftp.cs.utah.edu/pub/dept/OLD/pub/urt-3.1b.tar.Z

      to write the RLE image format (may not compile under linux).

    o ImageMagick requires scanimage from

         http://www.mostang.com/sane/
								 
      to import an image from a scanner device.

    o ImageMagick requires Sam Leffler's TIFF software available via
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

    o ImageMagick requires libwmf 0.2.5 (or 0.2.2) available from

         http://sourceforge.net/projects/wvware/

      to render files in the Windows Meta File (WMF) metafile format
      (16-bit WMF files only, not 32-bit "EMF"). This is the format
      commonly used for Windows clipart (available on CD at your local
      computer or technical book store). WMF support requires the
      FreeType 2 library in order to render TrueType and Postscript fonts.

      While ImageMagick uses the libwmflite (parser) component of the
      libwmf package which does not depend on any special libraries, the
      libwmf package as a whole depends on FreeType 2 and either the
      xmlsoft libxml, or expat libraries. Since ImageMagick already uses
      libxml (for reading SVG and to retrieve files via HTTP or FTP), it
      is recommended that the options '--without-expat --with-xml' be
      supplied to libwmf's configure script.

      ImageMagick's WMF renderer provides some of the finest WMF
      rendering available due its use of antialiased drawing algorithms.
      You may select a background color or texture image to render on.
      For example, "-background '#ffffffff'" renders on a transparent
      background while "-texture plasma:fractal" renders on a fractal
      image.

      A set of Windows 3.1 compatible TrueType fonts (Arial, Times New
      Roman, and Courier New) may be downloaded without charge from
      Microsoft's web site via the URL

         http://www.microsoft.com/typography/fontpack/default.htm

      The WMF renderer is not capable of using Windows fonts with libwmf
      0.2.2. Windows fonts are fully supported when using libwmf 0.2.5
      or later.

    o ImageMagick requires the FlashPIX library version 1.2.0 from
      the Digital Imaging Group in order to support the FlashPIX
      format. The FlashPIX library may be obtained from ImageMagick
      anonymous CVS by checking out the 'fpx' module, or retrieving the
      file libfpx-1.2.0.4.tar.gz from the ftp directory

         ftp://ftp.imagemagick.org/pub/ImageMagick/delegates/.

    o ImageMagick requires an X server for display and animate to work
      properly. Unix systems usually provide an X server as part of
      their standard installation.

      A free X server for Microsoft Windows is available from

         http://sources.redhat.com/win32-x11/

      The Cygwin port of XFree86 may also be used. It is available from

         http://www.cygwin.com/xfree/

      There is a nearly free X server available for Windows and
      Macintosh at

         http://www.microimages.com/freestuf/mix/

    o ImageMagick requires libxml available from

         http://xmlsoft.org/

      to read the SVG image format and to retrieve files from over a
      network via FTP and HTTP.

    o ImageMagick requires the ZLIB library from

         http://www.gzip.org/zlib/

      to read or write the PNG or Zip compressed MIFF images.

    o ImageMagick requires a background texture for the TILE
      format and for the -texture option of montage(1).  You can use
      your own or get samples from

         http://the-tech.mit.edu/KPT/


VMS COMPILATION

  You might want to check the values of certain program definitions
  before compiling.  Verify the definitions in delegates.mgk to suit
  your local requirements.  Next, type.

  Type

      unzip ImageMagick-5.4.8.zip set default [.imagemagick] @make set
      display/create/node=node_name::

  where node_name is the DECNET X server to contact.

  Finally type:

      display

  Alternatively, get a zipped distribution (with JPEG, PNG, TIFF, TTF)
  from

      ftp://ftp.imagemagick.org/pub/ImageMagick/vms/ImageMagick-5.4.8.zip

  Thanks to pmoreau@cenaath.cena.dgac.fr for supplying invaluable help
  as well as the VMS versions of the JPEG, PNG, TTF, and TIFF
  libraries.


Windows Win2K/95 VISUAL C++ 6.0 COMPILATION

  The Visual C++ distribution targeted at Windows Win2K or Windows 95
  does not provide any stock workspace (DSW) or project files (DSP)
  except for those included with third party libraries. Instead, there
  is a "configure" program that must be built and run that creates an
  environment that meets your particular needs.

  The issue with the Visual C++ working environment is that there are
  a fairly large number of mutually exclusive options that must all be
  used in a coherent manner, or problems result.

  The Visual C++ system provides three different types of "runtimes"
  that must match across all application, library, and DLL code that is
  built. The "configure" program creates a set of build files that are
  consistent for a specific runtime selection.

  The three options for runtime support are:

    1) Dynamic Multi-threaded DLL runtimes.
    2) Static Single-threaded runtimes
    3) Static Multi-threaded runtimes
    4) Static Multi-threaded DLL runtimes

  In addition to these runtimes, the VisualMagick build environment allows
  you to select whether to include the X11 libraries in the build or use
  the X11 "stubs". The assumption is that most Win32 user could care
  less about X11 support, so we stub out all the X11 functionality so
  that everything compiles. However, the X11 utility programs animate,
  display, and import will not work.

  This leads to five different possible build options, which should
  cover almost any particular situation. The default binary distribution
  is built using #1 from above with the X11 libraries included. This
  results in an X11 compatible build using all DLL's for everything and
  multi-threaded support (the only option for DLL's).

  To do a build for your requirements, simply go to the configure sub-
  directory under VisualMagick and open the configure.dsw workspace.
  Set the build configuration to "Release" under the

      "Build..., Set Active Configuration..."  menu.

  Build and execute the configure program and follow the instructions.
  You should probably not change any of the defaults unless you have a
  specific reason to do so.

  After creating your build environment you can proceed to open the DSW
  file that was generated and build everything from there.

  In the final DSW file you will find a project call "All". In order to
  build everything in the distribution, select this project and make it
  the "active" project. Set the build configuration to the desired one
  (Debug, or Release) and do a "clean" followed by a "build". You should
  do the build in a specific way:

    1) Make the "All" project the active project (Bold)
       Right click on the All project and select "Set As Active Project"
    2) Select "Build..., Clean"
    3) Select "Build..., Build"
    4) Go get some coffee unless you have a very fast machine!.

  The "Clean" step is needed in order to make sure that all of the target
  support libraries are updated with any patches needed to get them to
  compile properly under Visual C++.

  All of the required files that are needed to run any of the command
  line tools will be found in the "bin" subdirectory of the VisualMagick
  subdirectory. This includes EXE, and DLL files. You should be able
  to test the build directly from this directory without having to move
  anything to any of the global SYSTEM or SYSTEM32 areas in the operating
  system installation.

  NOTE #1:

  The Visual C++ distribution of ImageMagick comes with the Magick++
  C++ wrapper by default. This add-on layer has a large number of demo
  and test files that can be found in ImageMagick\Magick++\demo, and
  ImageMagick\Magick++\tests. There are also a variety of tests that
  use the straight C API as well in ImageMagick\tests.

  All of these programs are NOT configured to be built in the default
  workspace created by the configure program. You can cause all of these
  demos and test programs to be built by checking the box in configure
  that says:

    "Include all demo and test programs"

  In addition, there is another related checkbox (checked by default)
  that causes all generate project files to be created standalone so
  that they can be copied to other areas of you system.

  This is the checkbox:

   "Generate all utility projects with full paths rather then relative
    paths"

  WOW - that a mouthfull - eh?

  The problem is that Visual C++ uses a concept of "dependencies" that
  tell it what other things need to be build when a particular project
  is being build. This mechanism is also used to cause things to link
  properly. In my normal development environment, I want to be able to
  make changes and debug the system as a whole, so I like and NEED to
  use dependencies. However, most end users don't want to work this way.

  Instead they really just want to build the package and then get down
  to business working on their application. The solution is to make all
  the utility projects (UTIL_xxxx_yy_exe.dsp) use full absolute paths
  to all the things they need. This way the projects stand on their own
  and can actually be copied and used as templates to get a particular
  custom application compiling with little effort.

  With this feature enabled, you should be able to nab a copy of...
    
    VisualMagick\utilities\UTIL_convert_xxx_exe.dsp  (for C)

     -or-

    VisualMagick\Magick++\demo\UTIL_demo_xxx_exe.dsp (for C++)

  ... and pop it into notepad, modify it (carefully) to your needs and
  be on your way to happy compiling and linking.

  You can feel free to pick any of the standard utilities, tests, or
  demo programs as the basis for a new program by copying the project
  and the source and hacking away.

  The choice of what to use as a starting point is very easy...

  For straight C API command line applications use something from:

    ImageMagick\tests or ImageMagick\utilities (source code)
    ImageMagick\VisualMagick\tests or ImageMagick\Visualmagick\utilities
      (project - DSP)

  For C++ and Magick++ command line applications use something from:

    ImageMagick\Magick++\tests or ImageMagick\Magick++\demo
      (source code)
    ImageMagick\VisualMagick\Magick++\tests or
      ImageMagick\VisualMagick\Magick++\demo (project - DSP)

  For C++ and Magick++ and MFC windows applications use:
   
    ImageMagick\contrib\win32\MFC\NtMagick (source code)
    ImageMagick\VisualMagick\contrib\win32\MFC\NtMagick
      (project - DSP)

  NOTE #2:

  The ImageMagick distribution is very modular. The default
  configuration is there to get you rolling, but you need to make some
  serious choices when you wish to change things around.

  The binary distribution and default options are all targeted at
  having all the components in one place - the "bin" directory of the
  VisualMagick build tree AND the C:\ImageMagick folder.

  The C:\ImageMagick folder should contain the following files:

      1) magic.mgk
      2) delegates.mgk (derived from nt.mgk)
      3) modules.mgk
      4) rgb.txt

  The "bin" folder should contains all EXE's and DLL's as well as the
  very important "modules.mgk" file.

  With this default setup, you can use any of the command line tools
  and run scripts as normal. You can actually get by quite nicely this
  way by doing something like "pushd e:\xxx\yyy\bin" in any scripts you
  write to execute "out of" this directory.

  ALSO, By default the core of ImageMagick on Win32 always looks in the
  place were the exe program is run from in order to find all of the
  files as well as the DLL's it needs.

  Of course - all of this is configurable and there are certain
  environment variables and compiler switches that allow you to do
  whatever you want. Some of these are Windows things and the rest are
  looked at by the core ImageMagick code.

  ENVIRONMENT VARIABLES:

  You can use the "System" control panel to allow you to add and delete
  what is in any of the environment variables. You can even have user
  specific environment variables if you wish.

  PATH

   This sets the default list of places were Windows looks for EXE's and
   DLL's. Windows CMD shell seems to look in the "current" directory first
   - no matter what, so that is why you really don't have to muck. If
   you wish to run any of utilities from another location then you must
   add the path to your "bin" directory in. For instance, you might add:

      D:\CVS\ImageMagick\VisualMagick\bin

   to do this for the default build environment like I do.  One slight
   problem with this is that fact that Windows has another program called
   "convert". I usually rename this to something else since it is only
   used to convert disks from FAT to NTFS.

  MAGICK_HOME

   If all you do is modify the PATH variable, the first problem you
   will run into is that ImageMagick may not be able to find any of
   its "modules. Modules are all the IM_MOD*.DLL files you see in the
   distribution. There is one of these for each and every file format
   that ImageMagick supports. This environment variable tells the system
   were to look for these DLL's. The compiled in "default" is "execution
   path" - which says - look in the same place that the application is
   running "in". If you are running from somewhere other then "bin" -
   this will no longer work and you must use this variable. If you elect
   to leave the modules in the same place as the EXE's (a good idea)
   then you can simply set this to the same place
   as you did the PATH variable. In my case:

      D:\\ImageMagick\coders

   This is also the place were ImageMagick expects to find the
   "colors.mgk", "delegates.mgk", "magic.mgk", "modules.mgk", and
   "type.mgk" files.

   One cool thing about the modules build of ImageMagick is that you can
   now leave out file formats and lighten you load. If all you ever need
   is GIF and JPEG, then simply drop all the other DLL's into the local
   trash can and get on with your life.

   WARNING: Always keep the "xc" format, since IM seems to need and
   "assume" that this one exists and gets real "unhappy" if it does
   not. We are still tracking down some of these issues.

  ALSO. You can elect to changes these things the good old "hard-coded"
  way. Two #defines are applicable.

  defines.h has 

      #define MagickConfigurePath  "c:\\ImageMagick\\"

  NOTE #3:

  The two utilities "display" and "animate" will only be usable when
  the real X11 libraries are included. The other build environments
  uses the X11 stubs to supply non-functional stubs for the X-Window
  functionality. There is no support for a "static" build of the X11
  support because the build procedures for the X11 libraries do not seem
  to support this.

  To view any image in a Microsoft window, type

      convert image.ext win:

  Make sure gswin32 (Ghostscript) is in your execution path (see
  Autoexec.bat), otherwise, you will be unable to convert or view a
  Postscript document.

  Make sure iexplore (Internet Explorer) is in your execution path (see
  Autoexec.bat), otherwise, you will be unable to browse the ImageMagick
  documentation.

  The Win2K executables will work under Windows 95/98.


MACINTOSH COMPILATION

  The Macintosh distribution contains MetroWerks Codewarrior Professional
  projects for compilation.  For those who do not have access to
  CodeWarrior, the binaries for the command line utilities are enclosed.
  I had to comment the inline intrinsic functions in math.h in order
  to compile.  If you have a better solution, let me know.

  Display(1), animate(1), and import(1) currently do not work on the
  Macintosh.


Magick++

  Magick++ provides a straightforward C++ language API to ImageMagick.
  Magick++ is built by default under the Unix, Windows, and Mac build
  environments. To disable building Magick++ under Unix, specify
  --without-magick-plus-plus as an argument to the configure script.

  Magick++ is currently supported using the following C++ compilers:

    egcs 1.1.2 (or later)
    gcc 2.95.2 (or later)
    Visual C++ 6.0 (Windows)
    IRIX C++ 7.3.1.1m
    HP-UX HP-UX aCC A.03.30
    Sun Workshop 5.0 C++ (tests/demos require work-around to build)
    Sun Forte 6.0 C++
    CodeWarrior Professional Release 5 (Macintosh)

  and may compile under other modern C++ compilers as well.

COPYRIGHT

  Copyright (C) 2002 ImageMagick Studio, a non-profit organization
  dedicated to making software imaging solutions freely available.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files ("ImageMagick"), to deal in ImageMagick without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of ImageMagick,
  and to permit persons to whom the ImageMagick is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of ImageMagick.

  The software is provided "as is", without warranty of any kind,
  express or implied, including but not limited to the warranties of
  merchantability, fitness for a particular purpose and noninfringement.
  In no event shall ImageMagick Studio be liable for any claim, damages or
  other liability, whether in an action of contract, tort or otherwise,
  arising from, out of or in connection with ImageMagick or the use or
  other dealings in ImageMagick.

  Except as contained in this notice, the name of the ImageMagick Studio
  shall not be used in advertising or otherwise to promote the sale, use
  or other dealings in ImageMagick without prior written authorization
  from the ImageMagick Studio.

