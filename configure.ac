# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1998, 1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# GraphicsMagick Configure Script
#
 
AC_PREREQ(2.57)
AC_INIT(magick/magick.h)

# Source file containing package/library versioning information.
. ${srcdir}/version.sh

echo "configuring ${PACKAGE_NAME} ${PACKAGE_VERSION}${PACKAGE_VERSION_ADDENDUM}"
AC_CANONICAL_SYSTEM
AC_SUBST(LIBRARY_CURRENT)dnl
AC_SUBST(LIBRARY_REVISION)dnl
AC_SUBST(LIBRARY_AGE)dnl
AC_SUBST(PACKAGE_NAME)dnl
AC_SUBST(PACKAGE_VERSION)dnl
AC_SUBST(PACKAGE_VERSION_ADDENDUM)dnl
AC_SUBST(PACKAGE_RELEASE_DATE)dnl

# Definition used to define MagickLibVersion in version.h
# As soon as any of these values exceeds 9, a more sophisticated formatting
# scheme must be invented for this string.
MAGICK_LIB_VERSION="0x0${LIBRARY_CURRENT}0${LIBRARY_AGE}0${LIBRARY_REVISION}"
AC_SUBST(MAGICK_LIB_VERSION)

# Definition used to define MagickLibVersionText in version.h
MAGICK_LIB_VERSION_TEXT="${PACKAGE_VERSION}"
AC_SUBST(MAGICK_LIB_VERSION_TEXT)

# Definition used to define MagickLibVersionNumber in version.h
MAGICK_LIB_VERSION_NUMBER="${LIBRARY_CURRENT},${LIBRARY_AGE},${LIBRARY_REVISION}"
AC_SUBST(MAGICK_LIB_VERSION_NUMBER)

# Ensure that make can run correctly
AM_SANITY_CHECK

# Generate configure header.
AC_CONFIG_HEADERS([magick/magick_config.h magick/magick_config_api.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME,"${PACKAGE_VERSION}${PACKAGE_VERSION_ADDENDUM}", ' ')

#
# Save initial user-tunable values
#
CC_INITIAL=$CC
CFLAGS_INITIAL=$CFLAGS
CPPFLAGS_INITIAL=$CPPFLAGS
LDFLAGS_INITIAL=$LDFLAGS
LIBS_INITIAL=$LIBS
PERLMAINCC=$CC

#
# Evaluate shell variable equivalents to Makefile directory variables
#
if test "x$prefix" = xNONE
then
  prefix=$ac_default_prefix
fi
# Let make expand exec_prefix.
if test "x$exec_prefix" = xNONE
then
  exec_prefix='${prefix}'
fi

#
eval "eval PREFIX_DIR=${prefix}"
AC_SUBST(PREFIX_DIR)
eval "eval EXEC_PREFIX_DIR=${exec_prefix}"
AC_SUBST(EXEC_PREFIX_DIR)
eval "eval BIN_DIR=$bindir"
AC_SUBST(BIN_DIR)
eval "eval SBIN_DIR=$sbindir"
AC_SUBST(SBIN_DIR)
eval "eval LIBEXEC_DIR=$libexecdir"
AC_SUBST(LIBEXEC_DIR)
eval "eval DATA_DIR=$datadir"
AC_SUBST(DATA_DIR)
eval "eval SYSCONF_DIR=$sysconfdir"
AC_SUBST(SYSCONF_DIR)
eval "eval SHAREDSTATE_DIR=$sharedstatedir"
AC_SUBST(SHAREDSTATE_DIR)
eval "eval LOCALSTATE_DIR=$localstatedir"
AC_SUBST(LOCALSTATE_DIR)
eval "eval LIB_DIR=$libdir"
AC_SUBST(LIB_DIR)
eval "eval INCLUDE_DIR=$includedir"
AC_SUBST(INCLUDE_DIR)
eval "eval OLDINCLUDE_DIR=$oldincludedir"
AC_SUBST(OLDINCLUDE_DIR)
eval "eval INFO_DIR=$infodir"
AC_SUBST(INFO_DIR)
eval "eval MAN_DIR=$mandir"
AC_SUBST(MAN_DIR)

# Get full paths to source and build directories
srcdirfull=`cd $srcdir && pwd`
builddir=`pwd`

WinPathScript="${srcdirfull}/winpath.sh"
AC_SUBST(WinPathScript)

# Check for programs
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AM_PROG_LD
AC_SUBST(LD)
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S

#
# Tests for Windows
#
AC_EXEEXT
AC_OBJEXT

LIB_GDI32=''
native_win32_build='no'
cygwin_build='no'
case "${host_os}" in
  cygwin* )
    cygwin_build='yes'
    LIB_GDI32='-lgdi32'
    ;;
  mingw* )
    native_win32_build='yes'
    LIB_GDI32='-lgdi32'
    ;;
esac
if test "${LIB_GDI32}x" != 'x'
then
  AC_DEFINE(HasWINGDI32,1,Define to use the Windows GDI32 library)
fi
AC_SUBST(LIB_GDI32)
AM_CONDITIONAL(HasWINGDI32, test "${LIB_GDI32}x" != 'x' )
AM_CONDITIONAL(WIN32_NATIVE_BUILD, test "${native_win32_build}" = 'yes' )
AM_CONDITIONAL(CYGWIN_BUILD, test "${cygwin_build}" = 'yes' )

#
# Compiler flags tweaks
#
if test "${GCC}" != "yes"
then
  case "${host}" in
    *-*-hpux* )
      # aCC: HP ANSI C++ B3910B A.03.34
      CFLAGS="${CFLAGS} -Wp,-H30000"
      CXX="${CXX} -AA"
      ;;
    *-dec-osf5.* )
      # Compaq alphaev68-dec-osf5.1 compiler
      CXXFLAGS="${CXXFLAGS} -std strict_ansi -noimplicit_include"
  esac
else
  CFLAGS="${CFLAGS} -Wall"
fi

#
# Configure libtool & libltdl
#

# Configure libltdl
#AC_CONFIG_SUBDIRS(ltdl)
AC_LIBLTDL_CONVENIENCE(ltdl)
AC_LIB_LTDL
# Substitute INCLTDL and LIBLTDL in the Makefiles
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

# Configure libtool
AC_DISABLE_SHARED
AC_ENABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_DLOPEN
AC_LIBTOOL_SETUP
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Check to see if building shared libraries
libtool_build_shared_libs='no'
if ./libtool --config | grep 'build_libtool_libs=yes' > /dev/null
then
  libtool_build_shared_libs='yes'
fi

# Check to see if building static libraries
libtool_build_static_libs='no'
if ./libtool --config | grep 'build_old_libs=yes' > /dev/null
then
  libtool_build_static_libs='yes'
fi

#
# Enable support for building loadable modules
#
AC_ARG_WITH(modules,
	      [  --with-modules          enable support for dynamically loadable modules],
	      [with_modules=$withval],
	      [with_modules='no'])

# Only allow building loadable modules if we are building shared libraries
if test "$with_modules" != 'no' ; then
  if test "$libtool_build_shared_libs" = 'no' ; then
    AC_MSG_WARN([Modules may only be built if building shared libraries is enabled.])
    with_modules='no'
  fi
fi
if test "$with_modules" != 'no' ; then
  AC_DEFINE(BuildMagickModules,1,Define if coders and filters are to be built as modules.)
fi
AM_CONDITIONAL(WITH_MODULES, test "$with_modules" != 'no')

# Enable building/use of libltdl if we are building shared libraries regardless
# of whether modules are built or not.
with_ltdl='no'
if test "$libtool_build_shared_libs" != 'no' ; then
  with_ltdl='yes'
fi

AM_CONDITIONAL(WITH_LTDL, test "$with_ltdl" != 'no')
if test "$with_ltdl" != 'no' ; then
  AC_DEFINE(HasLTDL,1,Define if using libltdl to support dynamically loadable modules)

  # Set DLLDFLAGS
  if test X"$enable_shared" = Xyes; then
    DLLDFLAGS=-export-dynamic
    AC_SUBST(DLLDFLAGS)
  fi
fi

# Enable build using delegate libraries built in subdirectories rather than installed
# delegate libraries (bzlib fpx jasper jbig jpeg lcms png tiff ttf wmf xml zlib)
AC_ARG_ENABLE(delegate-build,
	      [  --enable-delegate-build enable using delegate libs in subdirectories
                          (default disabled)],
	      [with_delegate_build=$enableval],
	      [with_delegate_build='no'])

# Build a version of GraphicsMagick which operates uninstalled.
# Used to build distributions located via MAGICK_HOME / executable path
AC_ARG_ENABLE(installed,
	      [  --disable-installed      disable building an installed GraphicsMagick
                          (default enabled)],
	      [with_installed=$enableval],
	      [with_installed='yes'])
if test "$with_installed" = 'yes'
then
  AC_DEFINE(UseInstalledMagick,1,[GraphicsMagick is formally installed under prefix])
fi

# Enable verbose output from libtool (enable libtool's default)
AC_ARG_ENABLE(libtool-verbose,
	      [  --enable-libtool-verbose enable verbose libtool output (default disabled)],
	      [with_libtool_verbose=$enableval],
	      [with_libtool_verbose='no'])
if test "$with_libtool_verbose" = 'no'
then
  LIBTOOL="$LIBTOOL --silent"
fi

# Add configure option --enable-maintainer-mode which enables dependency
# checking and generation useful to package maintainers.  This is made an
# option to avoid confusing end users.
AM_MAINTAINER_MODE


# Enable ccmalloc memory debugging support
AC_ARG_ENABLE(ccmalloc,
	      [  --enable-ccmalloc       enable 'ccmalloc' memory debug support (default disabled)],
	      [with_ccmalloc=$enableval],
	      [with_ccmalloc='no'])

# Enable prof-based profiling support
AC_ARG_ENABLE(prof,
	      [  --enable-prof           enable 'prof' profiling support (default disabled)],
	      [with_prof=$enableval],
	      [with_prof='no'])

# Enable gprof-based profiling support
AC_ARG_ENABLE(gprof,
	      [  --enable-gprof          enable 'gprof' profiling support (default disabled)],
	      [with_gprof=$enableval],
	      [with_gprof='no'])

# Enable gcov-based profiling support
AC_ARG_ENABLE(gcov,
	      [  --enable-gcov           enable 'gcov' profiling support (default disabled)],
	      [with_gcov=$enableval],
	      [with_gcov='no'])

with_profiling='no'
if test "$with_prof" = 'yes' || test "$with_gprof" = 'yes' || test "$with_gcov" = 'yes'
then
  with_profiling='yes'

  if test "$libtool_build_shared_libs" = 'yes'
  then
    echo "Warning: Can not profile code using shared libraries"
  fi
fi

# Disable support for largefiles
AC_ARG_ENABLE(largefile,
	      [  --disable-largefile     disable support for large (64 bit) file offsets],
	      [with_largefile=$enableval],
	      [with_largefile='yes'])

# Enable LZW compression.
AC_ARG_ENABLE(lzw,
	      [  --enable-lzw            enable LZW support (default disabled)],
	      [with_lzw=$enableval],
	      [with_lzw='no'])
test "$with_lzw" = 'yes' && AC_DEFINE(HasLZW,1,Include LZW compression)
AM_CONDITIONAL(HasLZW, test "$have_lzw" != 'no')

# Number of bits in a Quantum
AC_ARG_WITH(quantum-depth,
	      [  --with-quantum-depth    number of bits in a pixel quantum (default 8)],
	      [with_quantum_depth=$withval],
	      [with_quantum_depth=8])

case "${with_quantum_depth}" in
  8 ) ;;
  16 ) ;;
  32 ) ;;
  * ) AC_ERROR("Pixel quantum depth must have value of 8, 16, or 32") ;;
esac
QuantumDepth="$with_quantum_depth"
AC_DEFINE_UNQUOTED(QuantumDepth,$QuantumDepth,[Number of bits in a pixel Quantum (8/16/32)])
AC_SUBST(QuantumDepth)dnl

# Set pixel cache threshold
AC_ARG_WITH(cache,
	      [  --with-cache            set pixel cache threshhold in MB (default available memory)],
	      [with_cache=$withval],
	      [with_cache=''])
if test "$with_cache" != ''
then
  AC_DEFINE_UNQUOTED(PixelCacheThreshold,$with_cache,[Pixel cache threshold in MB (defaults to available memory)])
fi

# Enable support for threads
AC_ARG_WITH(threads,
	      [  --without-threads       disable threads support],
	      [with_threads=$withval],
	      [with_threads='yes'])

# Disable/Enable support for full delegate paths
if test "$native_win32_build" = 'yes'
then
  AC_ARG_WITH(frozenpaths,
	      [  --with-frozenpaths      enable frozen delegate paths],
	      [with_frozenpaths=$withval],
	      [with_frozenpaths='no'])
else
  AC_ARG_WITH(frozenpaths,
	      [  --without-frozenpaths   disable frozen delegate paths],
	      [with_frozenpaths=$withval],
	      [with_frozenpaths='yes'])
fi

# Enable build/install of Magick++
AC_ARG_WITH(magick-plus-plus,
	      [  --without-magick-plus-plus disable build/install of Magick++],
	      [with_magick_plus_plus=$withval],
	      [with_magick_plus_plus='yes'])

# Disable build/install of PerlMagick.
AC_ARG_WITH(perl,
	      [  --without-perl          disable build/install of PerlMagick
       or
  --with-perl=PERL        use specified Perl binary to configure PerlMagick],
	      [with_perl=$withval],
	      [with_perl='yes'])

# Options to pass when configuring PerlMagick
AC_ARG_WITH(perl-options,
[  --with-perl-options=[OPTIONS]  options to pass on command-line when
                          generating PerlMagick's Makefile from Makefile.PL],
[PERL_MAKE_OPTIONS=$withval])
AC_SUBST(PERL_MAKE_OPTIONS)


# Disable BZLIB (bzip2 library)
AC_ARG_WITH(bzlib,
	      [  --without-bzlib         disable BZLIB support],
	      [with_bzlib=$withval],
	      [with_bzlib='yes'])

# Disable Display Postscript.
AC_ARG_WITH(dps,
	      [  --without-dps           disable Display Postscript support],
	      [with_dps=$withval],
	      [with_dps='yes'])

# Disable FlashPIX.
AC_ARG_WITH(fpx,
	      [  --without-fpx           disable FlashPIX support],
	      [with_fpx=$withval],
	      [with_fpx='yes'])

# Enable Ghostscript library support.
AC_ARG_WITH(gslib,
	      [  --with-gslib            enable Ghostscript library support],
	      [with_gslib=$withval],
	      [with_gslib='no'])

# # Enable HDF.
# AC_ARG_WITH(hdf,
# 	      [  --with-hdf              enable HDF support],
# 	      [with_hdf=$withval],
# 	      [with_hdf='no'])

# Disable JBIG.
AC_ARG_WITH(jbig,
	      [  --without-jbig          disable JBIG support],
	      [with_jbig=$withval],
	      [with_jbig='yes'])

# Disable JPEG.
AC_ARG_WITH(jpeg,
	      [  --without-jpeg          disable JPEG support],
	      [with_jpeg=$withval],
	      [with_jpeg='yes'])

# Disable JPEG Version 2.
AC_ARG_WITH(jp2,
	      [  --without-jp2           disable JPEG v2 support],
	      [with_jp2=$withval],
	      [with_jp2='yes'])

# Disable LCMS.
AC_ARG_WITH(lcms,
	      [  --without-lcms          disable LCMS support],
	      [with_lcms=$withval],
	      [with_lcms='yes'])

# # Disable MPEG.
# AC_ARG_WITH(mpeg2,
# 	      [  --without-mpeg2         disable MPEG support],
# 	      [with_mpeg2=$withval],
# 	      [with_mpeg2='yes'])

# Disable PNG.
AC_ARG_WITH(png,
	      [  --without-png           disable PNG support],
	      [with_png=$withval],
	      [with_png='yes'])

# Disable TIFF.
AC_ARG_WITH(tiff,
	      [  --without-tiff          disable TIFF support],
	      [with_tiff=$withval],
	      [with_tiff='yes'])

# Disable TTF.
AC_ARG_WITH(ttf,
	      [  --without-ttf           disable TrueType support],
	      [with_ttf=$withval],
	      [with_ttf='yes'])

# Disable WMF.
AC_ARG_WITH(wmf,
	      [  --without-wmf           disable WMF support],
	      [with_wmf=$withval],
	      [with_wmf='yes'])

# Set default font search path
AC_ARG_WITH(fontpath,
              [  --with-fontpath=DIR     prepend to default font search path],
              [with_fontpath=$withval],
              [with_fontpath=''])
if test "$with_fontpath" != "yes" && test -z "$with_fontpath"
then
  with_fontpath=''
else
  AC_DEFINE_UNQUOTED(MAGICK_FONT_PATH,"$with_fontpath",Define to prepend to default font search path.)
fi

# Set Ghostscript font directory
AC_ARG_WITH(gs-font-dir,
	      [  --with-gs-font-dir=DIR  directory containing Ghostscript fonts],
	      [with_gs_font_dir=$withval],
	      [with_gs_font_dir='default'])

# Set Windows font directory
AC_ARG_WITH(windows-font-dir,
	      [  --with-windows-font-dir=DIR
                          directory containing MS-Windows fonts],
	      [with_windows_font_dir=$withval],
	      [with_windows_font_dir=''])

# Disable XML.
AC_ARG_WITH(xml,
	      [  --without-xml           disable XML support],
	      [with_xml=$withval],
	      [with_xml='yes'])

AC_ARG_WITH(zlib,
	      [  --without-zlib          disable ZLIB support],
	      [with_zlib=$withval],
	      [with_zlib='yes'])

#
# Specify path to shared libstdc++ if not in normal location
#
AC_ARG_WITH(libstdc,
  [  --with-libstdc=DIR      use libstdc++ in DIR (for GNU C++)],
  [if test "$withval" != no -a "$withval" != yes; then
    if test -d "$withval"; then
      LIBSTDCLDFLAGS="-L$withval"
    fi
  fi])
AC_SUBST(LIBSTDCLDFLAGS)

# Does gcc required -traditional?
AC_PROG_GCC_TRADITIONAL

########
#
# Set defines required to build DLLs and modules using MinGW
#
########
# These options are set for multi-thread DLL module build
#  libMagick:           _DLL _MAGICKMOD_ _MAGICKLIB_
#  module:              _DLL 
#  executable/Magick++: _DLL _MAGICKMOD_
MODULE_EXTRA_CPPFLAGS=''
LIBRARY_EXTRA_CPPFLAGS=''
if test "${native_win32_build}" = 'yes'
then
  if test "${libtool_build_shared_libs}" = 'yes'
  then
    CPPFLAGS="$CPPFLAGS -D_DLL"
    LIBRARY_EXTRA_CPPFLAGS="$LIBRARY_EXTRA_CPPFLAGS -D_MAGICKLIB_"

    if test "$with_modules" = 'yes'
    then
      LIBRARY_EXTRA_CPPFLAGS="$LIBRARY_EXTRA_CPPFLAGS -D_MAGICKMOD_"
    else
      MODULE_EXTRA_CPPFLAGS="$MODULE_EXTRA_CPPFLAGS -D_MAGICKLIB_"
    fi

  else
    CPPFLAGS="$CPPFLAGS -D_LIB"

  fi
  if test "$with_threads" = 'yes'
  then
     CPPFLAGS="$CPPFLAGS -D_MT"
  fi
fi
AC_SUBST(MODULE_EXTRA_CPPFLAGS)
AC_SUBST(LIBRARY_EXTRA_CPPFLAGS)

########
#
# Check for large file support
#
########
LIB_LF=''
if test "$with_largefile" = 'yes'
then
#  AC_SYS_LARGEFILE
  AC_MSG_CHECKING(whether large file support needs explicit enabling)
  result='no'
  getconf='getconf LFS_CFLAGS'
  if ($getconf) >/dev/null 2>&1
  then
    CPPFLAGS="$CPPFLAGS "`$getconf`
    result='yes'
  fi
  getconf='getconf LFS_LDFLAGS'
  if ($getconf) >/dev/null 2>&1
  then
    LDFLAGS="$LDFLAGS "`$getconf`
    result='yes'
  fi
  getconf='getconf LFS_LIBS'
  if ($getconf) >/dev/null 2>&1
  then
    LIB_LF=`$getconf`
    LIBS="$LIB_LF $LIBS"
    result='yes'
  fi
  AC_MSG_RESULT($result)
fi
AC_SUBST(LIB_LF)

# Check standard headers
AC_HEADER_STDC
AC_HEADER_DIRENT

# Check additional headers
AC_CHECK_HEADERS(sys/times.h sys/types.h)

########
#
# Checks for typedefs, structures, and compiler characteristics.
#
########

# If the C compiler does not fully support the ANSI C qualifier const,
# define const to be empty.
AC_C_CONST

# If the C compiler supports the keyword inline, do nothing. Otherwise
# define inline to __inline__ or __inline if it accepts one of those,
# otherwise define inline to be empty.
AC_C_INLINE

# If words are stored with the most significant byte first (like
# Motorola and SPARC CPUs), define `WORDS_BIGENDIAN'.
AC_C_BIGENDIAN

# If the C compiler supports a working long double type with more range
# or precision than the double type, define HAVE_LONG_DOUBLE.
AC_C_LONG_DOUBLE

# Obtain size of an 'short' and define as SIZEOF_SHORT
AC_CHECK_SIZEOF(short)

# Obtain size of an 'unsigned short' and define as SIZEOF_UNSIGNED_SHORT
AC_CHECK_SIZEOF(unsigned short)

# Obtain size of an 'int' and define as SIZEOF_INT
AC_CHECK_SIZEOF(int)

# Obtain size of an 'unsigned int' and define as SIZEOF_UNSIGNED_INT
AC_CHECK_SIZEOF(unsigned int)

# Obtain size of a 'long' and define as SIZEOF_LONG
AC_CHECK_SIZEOF(long)

# Obtain size of a 'unsigned long' and define as SIZEOF_UNSIGNED_LONG
AC_CHECK_SIZEOF(unsigned long)

# Obtain size of a 'long long' and define as SIZEOF_LONG_LONG.  If
# 'long long' is not supported then the value defined is zero.
AC_CHECK_SIZEOF(long long)

# Obtain size of a 'unsigned long long' and define as
# SIZEOF_UNSIGNED_LONG_LONG.  If 'unsigned long long' is not
# supported then the value defined is zero.
AC_CHECK_SIZEOF(unsigned long long)

# If the system does not provide a 'mode_t' type, use 'int' instead
AC_CHECK_TYPE(mode_t, int)

# If the system does not provide a 'off_t' type, use 'long' instead
AC_CHECK_TYPE(off_t, long)

# If the system does not provide a 'pid_t' type, use 'int' instead
AC_CHECK_TYPE(pid_t, int)

# If the system does not provide a 'size_t', use 'unsigned int' instead
AC_CHECK_TYPE(size_t, unsigned)

# If `signal.h' declares signal as returning a pointer to a function
# returning void, define RETSIGTYPE to be void; otherwise, define it
# to be int.
AC_TYPE_SIGNAL

# Test for C compiler __func__ support
if test "$ac_cv_have___func__" != 'yes' ; then
AC_CACHE_CHECK(for C compiler __func__ support, ac_cv_have___func__,
[AC_TRY_COMPILE(
,
changequote(<<, >>)dnl
<<
const char *func=__func__;
return 0;
>>,
changequote([, ])dnl
ac_cv_have___func__='yes',
ac_cv_have___func__='no')])

if test "$ac_cv_have___func__" = 'yes' ; then
 AC_MSG_RESULT(yes)
 AC_DEFINE(HAS___func__,1,Define if C compiler supports __func__)
else
 AC_MSG_RESULT(no)
fi
fi

########
#
# Check for functions
#
########
AC_FUNC_MMAP
AC_CHECK_FUNCS([ftime getexecname getc_unlocked getpagesize mkstemp poll \
  pread pwrite putc_unlocked raise select tempnam times sigemptyset sigaction \
  strerror sysconf vsprintf vsnprintf])

########
#
# Try to find a command which reports usable physical memory
#
########
MAGICK_PHYSICAL_MEMORY_COMMAND=''
case "${host}" in
  *-*-freebsd* | *-apple-darwin*)
    AC_PATH_PROG(SysCtlDelegate,sysctl,)
    if test "${SysCtlDelegate}X" != 'X'
    then
       # "sysctl -n hw.physmem" became available in FreeBSD 2.0
       # Apple's Darwin is based on FreeBSD and supports sysctl
       MAGICK_PHYSICAL_MEMORY_COMMAND="${SysCtlDelegate} -n hw.physmem"
    fi
    ;;
esac
if test "${MAGICK_PHYSICAL_MEMORY_COMMAND}X" != 'X'
then
  AC_DEFINE_UNQUOTED(MAGICK_PHYSICAL_MEMORY_COMMAND,
    "${MAGICK_PHYSICAL_MEMORY_COMMAND}",
    [Command which returns total physical memory in bytes])
fi

########
#
# C++ Support Tests (For Magick++)
#
########
have_magick_plus_plus='no'
if test "$with_magick_plus_plus" = 'yes'
then
 OLIBS="$LIBS"
 LIBS=''
 AC_LANG_PUSH(C++)
# Full set of headers used ...
# algorithm cctype cerrno cmath cstdio cstdlib cstring ctime exception
# functional iomanip iosfwd iostream iterator list string strstream utility
 AC_LANG_CPLUSPLUS
 AC_PROG_CXX
 AC_CXX_BOOL
 AC_CXX_CONST_CAST
 AC_CXX_DEFAULT_TEMPLATE_PARAMETERS
 AC_CXX_EXCEPTIONS
 AC_CXX_NAMESPACES
 AC_CXX_EXPLICIT
 AC_CXX_HAVE_STD
 AC_CXX_HAVE_STL
 AC_CXX_MUTABLE
 AC_CXX_NEW_FOR_SCOPING
 AC_CXX_STATIC_CAST
 AC_CXX_TEMPLATES
 AC_LANG_POP

 AC_MSG_CHECKING(whether C++ compiler is sufficient for Magick++)
 if test \
  $ac_cv_cxx_bool = 'yes' -a \
  $ac_cv_cxx_const_cast = 'yes' -a \
  $ac_cv_cxx_default_template_parameters = 'yes' -a \
  $ac_cv_cxx_exceptions = 'yes' -a \
  $ac_cv_cxx_explicit = 'yes' -a \
  $ac_cv_cxx_have_std = 'yes' -a \
  $ac_cv_cxx_have_stl = 'yes' -a \
  $ac_cv_cxx_mutable = 'yes' -a \
  $ac_cv_cxx_namespaces = 'yes' -a \
  $ac_cv_cxx_new_for_scoping = 'yes' -a \
  $ac_cv_cxx_static_cast = 'yes' -a \
  $ac_cv_cxx_templates = 'yes'
 then
  have_magick_plus_plus='yes'
 else
  have_magick_plus_plus='no (failed tests)'
 fi
 AC_MSG_RESULT($have_magick_plus_plus)
 LIBS="$OLIBS"
fi
AM_CONDITIONAL(WITH_MAGICK_PLUS_PLUS, test "$have_magick_plus_plus" = 'yes')

# Only check for delegate libraries in subdirectories if requested.
if test "$with_delegate_build" != 'no'
then
  # Check for delegate sub-directories and add -I & -L options as required.
  # This presumes that delegates are installed as detailed in the GraphicsMagick
  # README. If delegates are installed in a standard location where the
  # compiler will automatically find them then these options should not be
  # required.

  #
  # Most delegates have includes in the same directory as the library, but not all ...
  #
  # Includes
  for dir in bzlib fpx jasper jbig/libjbig jpeg lcms lcms/src png tiff/libtiff ttf/include wmf/include xml zlib
  do
    if test -d "$builddir/$dir"
    then
      CPPFLAGS="$CPPFLAGS -I$builddir/$dir"
    else
      if test -d "$srcdirfull/$dir"
      then
        CPPFLAGS="$CPPFLAGS -I$srcdirfull/$dir"
      fi
    fi
  done

  # Libraries
  for dir in bzlib fpx/obj jasper jbig/libjbig jpeg lcms lcms/src png tiff/libtiff ttf/objs wmf/src xml zlib
  do
    if test -d "$builddir/$dir"
    then
      LDFLAGS="$LDFLAGS -L$builddir/$dir"
    else
      if test -d "$srcdirfull/$dir"
      then
        LDFLAGS="$LDFLAGS -L$srcdirfull/$dir"
      fi
    fi
    if test -d "$builddir/$dir/.libs"
    then
      LDFLAGS="$LDFLAGS -L$builddir/$dir/.libs"
    else
      if test -d "$srcdirfull/$dir/.libs"
      then
        LDFLAGS="$LDFLAGS -L$srcdirfull/$dir/.libs"
      fi
    fi
  done
fi

# Assume that delegate headers and libraries may reside under same
# directory as GraphicsMagick installation prefix.
#LDFLAGS="$LDFLAGS -L$LIB_DIR"
#CPPFLAGS="$CPPFLAGS -I$INCLUDE_DIR"

#
# Find the X11 RGB database
#
AC_CACHE_CHECK(for X11 configure files,im_cv_x_configure,
[# Look for the header file in a standard set of common directories.
# Check X11 before X11Rn because it is often a symlink to the current release.
  for ac_dir in               \
    /lib/usr/lib/X11	  \
    /usr/X11/lib          \
    /usr/X11R4/lib        \
    /usr/X11R5/lib        \
    /usr/X11R6/lib        \
    /usr/X386/lib         \
    /usr/XFree86/lib/X11  \
    /usr/athena/lib       \
    /usr/lib              \
    /usr/lib/X11          \
    /usr/lib/X11R4        \
    /usr/lib/X11R5        \
    /usr/lib/X11R6        \
    /usr/local/X11/lib    \
    /usr/local/X11R4/lib  \
    /usr/local/X11R5/lib  \
    /usr/local/X11R6/lib  \
    /usr/local/lib        \
    /usr/local/lib/X11    \
    /usr/local/lib/X11R4  \
    /usr/local/lib/X11R5  \
    /usr/local/lib/X11R6  \
    /usr/local/x11r5/lib  \
    /usr/lpp/Xamples/lib  \
    /usr/openwin/lib      \
    /usr/openwin/share/lib \
    /usr/unsupported/lib  \
    /usr/x386/lib         \
    ; \
  do
    if test -f "$ac_dir/X11/rgb.txt"
    then
        im_cv_x_configure="$ac_dir/X11/"
	break
    elif test -f "$ac_dir/rgb.txt"
    then 
        im_cv_x_configure="$ac_dir/"
        break
    fi

  done])
X11ConfigurePath="$im_cv_x_configure"
if test "$native_win32_build" = 'yes'
then
  X11ConfigurePath=`$WinPathScript "$X11ConfigurePath=" 1`
fi
AC_DEFINE_UNQUOTED(X11ConfigurePath,"X11ConfigurePath",Location of X11 configure files)

#
# Add support for ccmalloc memory debugging library if requested
#
have_ccmalloc='no'
LIB_CCMALLOC=''
if test "$with_ccmalloc" = 'yes'
then
  AC_PATH_PROG(CCMALLOCDelegate,ccmalloc,)
  if test -n "$CCMALLOCDelegate"
  then
    eval `grep PREFIX= $CCMALLOCDelegate | sed -e 's/PREFIX/CCMALLOC_PREFIX/'`
    OLIBS="$LIBS"
    # Assume that gcc is used with ccmalloc.
    LIBS="$LIBS $CCMALLOC_PREFIX/lib/ccmalloc-gcc.o"
    AC_CHECK_LIB(ccmalloc,ccmalloc_malloc,LIB_CCMALLOC="$CCMALLOC_PREFIX/lib/ccmalloc-gcc.o -lccmalloc -ldl",,-ldl)
    if test -n "$LIB_CCMALLOC"
    then
      LIBS="$OLIBS"
      LIBS="$LIBS $LIB_CCMALLOC"
      have_ccmalloc='yes'
     else
      LIBS="$OLIBS"
    fi
  fi
fi

#
# Find math library
#
LIB_MATH=''
AC_CHECK_LIB(m,sqrt,LIB_MATH="-lm",,)
LIBS="$LIB_MATH $LIBS"
AC_SUBST(LIB_MATH)

#
# Find Posix threads library
#
LIB_THREAD=''
if test "$with_threads" != 'no'
then
AC_MSG_CHECKING([for POSIX thread support])
OLIBS=$LIBS
LIBS="$LIBS -lpthread"
AC_TRY_LINK([#include <pthread.h>],[(void)pthread_create(0,0,0,0);],[
  AC_MSG_RESULT([yes])
  AC_DEFINE(HasPTHREADS, 1, [Define if you have Posix thread methods.])
  LIB_THREAD="-lpthread"
  DEF_THREAD="-D_REENTRANT"
  LIBS="$LIBS $LIB_THREAD"
  CPPFLAGS="$CPPFLAGS $DEF_THREAD"],[
  AC_MSG_RESULT([no])
  LIBS=$OLIBS])
fi
AC_SUBST(LIB_THREAD)

#
# Check for ZLIB
#
have_zlib='no'
dnl PNG requires zlib so enable zlib check if PNG is requested
if test "$with_zlib" != 'no' || test "$with_png" != 'no'
then
  LIB_ZLIB=''
  AC_MSG_CHECKING(for ZLIB support )
  AC_MSG_RESULT()
  failed=0;
  passed=0;
  AC_CHECK_HEADER(zconf.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
  AC_CHECK_HEADER(zlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
  AC_CHECK_LIB(z,compress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,uncompress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,deflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,inflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,gzseek,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,gztell,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_MSG_CHECKING(if ZLIB package is complete)
  if test $passed -gt 0
  then
    if test $failed -gt 0
    then
      AC_MSG_RESULT(no -- some components failed test)
      have_zlib='no (failed tests)'
    else
      LIB_ZLIB='-lz'
      LIBS="$LIB_ZLIB $LIBS"
      AC_DEFINE(HasZLIB,1,Define if you have zlib compression library)
      AC_MSG_RESULT(yes)
      have_zlib='yes'
    fi
  else
    AC_MSG_RESULT(no)
  fi
fi
AM_CONDITIONAL(HasZLIB, test "$have_zlib" = 'yes')
AC_SUBST(LIB_ZLIB)

#
# Check for BZLIB
#
have_bzlib='no'
if test "$with_bzlib" != 'no'
then
  LIB_BZLIB=''
  AC_MSG_CHECKING(for BZLIB support )
  AC_MSG_RESULT()
  failed=0;
  passed=0;
  AC_CHECK_HEADER(bzlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
  AC_CHECK_LIB(bz2,BZ2_bzCompress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(bz2,BZ2_bzDecompress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_MSG_CHECKING(if BZLIB package is complete)
  if test $passed -gt 0
  then
    if test $failed -gt 0
    then
      AC_MSG_RESULT(no -- some components failed test)
      have_bzlib='no (failed tests)'
    else
      LIB_BZLIB='-lbz2'
      LIBS="$LIB_BZLIB $LIBS"
      AC_DEFINE(HasBZLIB,1,Define if you have the bzip2 library)
      AC_MSG_RESULT(yes)
      have_bzlib='yes'
    fi
  else
    AC_MSG_RESULT(no)
  fi
fi
AM_CONDITIONAL(HasBZLIB, test "$have_bzlib" = 'yes')
AC_SUBST(LIB_BZLIB)

#
# Find the X11 include and library directories.
#
LIB_X11=''
LIB_XEXT=''
AC_PATH_XTRA
if test "$no_x" != 'yes'
then
  LDFLAGS="$LDFLAGS $X_LIBS"
  LIB_X11="$X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
  LIBS="$LIB_X11 $LIBS"
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"

  AC_DEFINE(HasX11,1,Define if you have X11 library)dnl

  #
  # Check for X11 shared memory extension
  #
  # shmctl is required to support the shared memory extension
  LIB_IPC=''
  AC_CHECK_FUNC([shmctl],[have_shmctl='yes'],[])
  if test "$have_shmctl" != 'yes'
  then
     AC_SEARCH_LIBS([shmctl],[cygipc],[have_shmctl='yes'; LIB_IPC='-lcygipc'],[])
  fi

  if test "$have_shmctl" = 'yes'
  then
    AC_CHECK_LIB([Xext],[XShmAttach],[LIB_XEXT='-lXext' ; AC_DEFINE(HasSharedMemory,1,X11 server supports shared memory extension)],[],[])
  fi

  #
  # Check for X11 shape extension
  #
  AC_CHECK_LIB([Xext],[XShapeCombineMask],[LIB_XEXT='-lXext' ; AC_DEFINE(HasShape,1,X11 server supports shape extension)],[],[])

  LIBS="$LIB_XEXT $LIBS"
fi
if test "$no_x" != 'yes'
then
  have_x='yes'
else
  have_x='no'
fi
AM_CONDITIONAL(HasX11, test "$have_x" = 'yes')
AC_SUBST(LIB_X11)
AC_SUBST(LIB_XEXT)

#
# If profiling, then check for -ldl and dlopen (required for Solaris & gcc)
#
LIB_DL=''
if test "$with_profiling" = 'yes'
then
  AC_CHECK_LIB(dl,dlopen,LIB_DL='-ldl',,)
  LIBS="$LIB_DL $LIBS"
fi
AC_SUBST(LIB_DL)

#
# Check for Display Postscript
#
have_dps='no'
LIB_DPS=''
if test "$with_dps" != 'no' && test "$with_x" != 'no'
then
    AC_MSG_CHECKING([for Display Postscript support ])
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    O_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I${ac_x_includes}/X11"
    AC_CHECK_HEADER(DPS/dpsXclient.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    # DPS issues:
    # XFree86-4.x needs -lXt to provide XtMalloc for -ldps.
    # Cygwin doesn't deliver -lXt as a DLL, which prevents a DLL build.
    # Adobe DPS (as delivered on Solaris) doesn't require -lXt.
    # GraphicsMagick itself doesn't use -lXt.
    have_libdps='no'
    LIBDPS_XT=''
    AC_CHECK_LIB(dps,DPSInitialize,have_libdps='yes',have_libdps='no',)
    if test "$have_libdps" != 'yes'
    then
      # Unset cache variable so we can try again.
      unset ac_cv_lib_dps_DPSInitialize
      AC_CHECK_LIB(dps,DPSInitialize,have_libdps='yes',have_libdps='no',-lXt)
      if test "$have_libdps" = 'yes'
      then
        LIBDPS_XT='-lXt'
      fi
    fi
    if test "$have_libdps" = 'yes'
    then
      passed=`expr $passed + 1`
    else
      failed=`expr $failed + 1`
    fi
    AC_CHECK_LIB(dpstk,XDPSPixelsPerPoint,passed=`expr $passed + 1`,failed=`expr $failed + 1`,-ldps $LIBDPS_XT)
    AC_MSG_CHECKING(if DPS package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT([no -- some components failed test])
        have_dps='no (failed tests)'
	CPPFLAGS="$O_CPPFLAGS"
    else
	LIB_DPS="-ldpstk -ldps ${LIBDPS_XT}"
	LIBS="$LIB_DPS $LIBS"
	AC_DEFINE(HasDPS,1,Define if you have Display Postscript)
	AC_MSG_RESULT(yes)
	have_dps='yes'
    fi
    else
    AC_MSG_RESULT(no)
    CPPFLAGS=$O_CPPFLAGS
    fi
fi
AM_CONDITIONAL(HasDPS, test "$have_dps" = 'yes')
AC_SUBST(LIB_DPS)

#
# Check for FlashPIX
#
have_fpx='no'
LIB_FPX=''
if test "$with_fpx" != 'no'
then
    AC_MSG_CHECKING(for FlashPIX components )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADER(fpxlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(fpx,FPX_OpenImageByFilename,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_LANG_POP
    AC_MSG_CHECKING(if FlashPIX package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_fpx='no (failed tests)'
    else
	LIB_FPX='-lfpx'
	# LIBS="$LIB_FPX $LIBS"
	AC_DEFINE(HasFPX,1,Define if you have FlashPIX library)
	AC_MSG_RESULT(yes)
        have_fpx='yes'
        PERLMAINCC="$CXX"
    fi
    else
        AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasFPX, test "$have_fpx" = 'yes')
AC_SUBST(LIB_FPX)

#
# Check for LCMS
#
have_lcms='no'
LIB_LCMS=''
if test "$with_lcms" != 'no'
then
    AC_MSG_CHECKING(for LCMS support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    have_lcms_header='no'
    AC_CHECK_HEADER(lcms.h,have_lcms_header='yes',,)
    if test "$have_lcms_header" = 'yes'
    then
      passed=`expr $passed + 1`
        AC_DEFINE(HAVE_LCMS_H,1,Define if you have the <lcms.h> header file.)
    else
      AC_CHECK_HEADER(lcms/lcms.h,have_lcms_header='yes',,)
      if test "$have_lcms_header" = 'yes'
      then
        passed=`expr $passed + 1`
        AC_DEFINE(HAVE_LCMS_LCMS_H,1,Define if you have the <lcms/lcms.h> header file.)
      else
        failed=`expr $failed + 1`
      fi
    fi
    AC_CHECK_LIB(lcms,cmsOpenProfileFromMem,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if LCMS package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_lcms='no (failed tests)'
    else
	LIB_LCMS='-llcms'
	LIBS="$LIB_LCMS $LIBS"
	AC_DEFINE(HasLCMS,1,Define if you have LCMS library)
	AC_MSG_RESULT(yes)
	have_lcms='yes'
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasLCMS, test "$have_lcms" = 'yes')
AC_SUBST(LIB_LCMS)

#
# Check for PNG
#
have_png='no'
LIB_PNG=''
if test "$with_png" != 'no'
then
    AC_MSG_CHECKING(for PNG support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(png.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(png,png_get_io_ptr,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if PNG package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_png='no (failed tests)'
    else
	LIB_PNG='-lpng'
	LIBS="$LIB_PNG $LIBS"
	AC_DEFINE(HasPNG,1,Define if you have PNG library)
	AC_MSG_RESULT(yes)
	have_png='yes'
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasPNG, test "$have_png" = 'yes')
AC_SUBST(LIB_PNG)

#
# Check for JPEG
#
have_jpeg='no'
LIB_JPEG=''
if test "$with_jpeg" != 'no'
then
    AC_MSG_CHECKING(for JPEG support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(jconfig.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jerror.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jmorecfg.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jpeglib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(jpeg,jpeg_read_header,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)

# Test for compatible JPEG library
if test "$ac_cv_jpeg_version_ok" != 'yes' ; then
AC_CACHE_CHECK(for JPEG library is version 6b or later, ac_cv_jpeg_version_ok,
[AC_TRY_COMPILE(
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
,
changequote(<<, >>)dnl
<<
#if JPEG_LIB_VERSION < 62
#error IJG JPEG library must be version 6b or newer!
#endif
return 0;
>>,
changequote([, ])dnl
ac_cv_jpeg_version_ok='yes',
ac_cv_jpeg_version_ok='no')])
if test "$ac_cv_jpeg_version_ok" = 'yes' ; then
 AC_MSG_RESULT(yes)
 passed=`expr $passed + 1`
else
 AC_MSG_RESULT(no)
 failed=`expr $failed + 1`
fi
fi
    AC_MSG_CHECKING(if JPEG package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
        have_jpeg='no (failed tests)'
    else
	LIB_JPEG='-ljpeg'
	LIBS="$LIB_JPEG $LIBS"
	AC_DEFINE(HasJPEG,1,Define if you have JPEG library)
	AC_MSG_RESULT(yes)
        have_jpeg='yes'
    fi
    else
        AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasJPEG, test "$have_jpeg" = 'yes')
AC_SUBST(LIB_JPEG)

#
# Check for JPEG Version 2
#
have_jp2='no'
LIB_JP2=''
if test "$with_jp2" != 'no'
then
    AC_MSG_CHECKING(for JPEG version 2 support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(jasper/jasper.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(jasper,jas_stream_fopen,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if JPEG version 2 support package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_jp2='no (failed tests)'
    else
	LIB_JP2='-ljasper'
	LIBS="$LIB_JP2 $LIBS"
	AC_DEFINE(HasJP2,1,Define if you have JPEG version 2 "Jasper" library)
	AC_MSG_RESULT(yes)
	have_jp2='yes'
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasJP2, test "$have_jp2" = 'yes')
AC_SUBST(LIB_JP2)

#
# Check for Ghostscript library
#
# Test for iapi.h & test for gsapi_new_instance in -lgs
have_gslib='no'
LIB_GS=''
if test "$with_gslib" != 'no'
then
    AC_MSG_CHECKING(for Ghostscript library support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(ps/iapi.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_HEADER(ps/errors.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(gs,gsapi_new_instance,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if Ghostscript library package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_gslib='no (failed tests)'
    else
	LIB_GS='-lgs'
	LIBS="$LIB_GS $LIBS"
	AC_DEFINE(HasGS,1,Define if you have Ghostscript library)
	AC_MSG_RESULT(yes)
	have_gslib='yes'
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasGS, test "$have_gslib" = 'yes')
AC_SUBST(LIB_GS)

# #
# # Check for MPEG2 library
# #
# have_mpeg2='no'
# LIB_MPEG2=''
# if test "$with_mpeg2" != 'no'
# then
#     AC_MSG_CHECKING(for MPEG version 2 support )
#     AC_MSG_RESULT()
#     failed=0;
#     passed=0;
#     AC_CHECK_HEADER(mpeg2dec/mpeg2.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
#     AC_CHECK_LIB(mpeg2,mpeg2_decode_data,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
#     AC_MSG_CHECKING(if MPEG version 2 support package is complete)
#     if test $passed -gt 0
#     then
#     if test $failed -gt 0
#     then
# 	AC_MSG_RESULT(no -- some components failed test)
# 	have_mpeg2='no (failed tests)'
#     else
# 	LIB_MPEG2='-lmpeg2'
# 	LIBS="$LIB_MPEG2 $LIBS"
# 	AC_DEFINE(HasMPEG2,1,Define if you have MPEG2 library)
# 	AC_MSG_RESULT(yes)
# 	have_mpeg2='yes'
#     fi
#     else
#     AC_MSG_RESULT(no)
#     fi
# fi
# AM_CONDITIONAL(HasMPEG2, test "$have_mpeg2" = 'yes')
# AC_SUBST(LIB_MPEG2)

#
# Check for TTF
#
have_ttf='no'
LIB_TTF=''
if test "$with_ttf" != 'no'
then
    AC_MSG_CHECKING(for FreeType 2.0 )
    AC_MSG_RESULT()
    failed=0;
    passed=0;

    OLD_LDFLAGS="$LDFLAGS"
    OLD_CPPFLAGS="$CPPFLAGS"
    if test "$with_delegate_build" != 'no' -a -d "$builddir/ttf/include"
    then
       :
    else
       freetype_config=''
       AC_CHECK_PROGS(freetype_config,freetype-config,)dnl
       if test -n "$freetype_config"
       then
          freetype_prefix=`freetype-config --prefix`
          freetype_exec_prefix=`freetype-config --exec-prefix`
          LDFLAGS="$LDFLAGS -L${freetype_exec_prefix}/lib"
          CPPFLAGS="-I${freetype_prefix}/include/freetype2 $CPPFLAGS"
       fi
    fi

dnl First see if there is a library
    if test "$LIB_TTF" = ''
    then
      AC_CHECK_LIB(freetype,FT_Init_FreeType,LIB_TTF='-lfreetype',,)
      if test "$LIB_TTF" != ''
      then
        passed=`expr $passed + 1`
      else
        failed=`expr $failed + 1`
        LDFLAGS="$OLD_LDFLAGS"
      fi
    fi

dnl Now test for the headers
    AC_CHECK_HEADERS(freetype/freetype.h)
    if test "$ac_cv_header_freetype_freetype_h" = 'yes'
    then
	passed=`expr $passed + 1`
    else
	failed=`expr $failed + 1`
        CPPFLAGS="$OLD_CPPFLAGS"
    fi

    AC_MSG_CHECKING(if FreeType package is complete)
    if test $passed -gt 0
    then
	if test $failed -gt 0
	then
            LIB_TTF=''
	    AC_MSG_RESULT(no -- some components failed test)
	    have_ttf='no (failed tests)'
	else
	    LIBS="$LIB_TTF $LIBS"
	    AC_DEFINE(HasTTF,1,Define if you have FreeType (TrueType font) library)
	    AC_MSG_RESULT(yes)
	    have_ttf='yes'
	fi
    else
	AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasTTF, test "$have_ttf" = 'yes')
AC_SUBST(LIB_TTF)

#
# Check for TIFF
#
have_tiff='no'
LIB_TIFF=''
if test "$with_tiff" != 'no'
then
    AC_MSG_CHECKING(for TIFF support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(tiff.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(tiffio.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(tiff,TIFFOpen,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFClientOpen,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFIsByteSwapped,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFReadRGBATile,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFReadRGBAStrip,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if TIFF package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_tiff='no (failed tests)'
    else
	LIB_TIFF='-ltiff'
	LIBS="$LIB_TIFF $LIBS"
	AC_DEFINE(HasTIFF,1,Define if you have TIFF library)
	AC_MSG_RESULT(yes)
	have_tiff='yes'
	AC_CHECK_HEADERS(tiffconf.h)
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasTIFF, test "$have_tiff" = 'yes')
AC_SUBST(LIB_TIFF)

# #
# # Check for HDF
# #
# have_hdf='no'
# LIB_HDF=''
# if test "$with_hdf" != 'no'
# then
#     AC_MSG_CHECKING(for HDF components )
#     AC_MSG_RESULT()
#     failed=0;
#     passed=0;
#     AC_CHECK_HEADERS(hdf.h hdf/hdf.h)
#     if test "$ac_cv_header_hdf_h" = 'yes' -o "$ac_cv_header_hdf_hdf_h" = 'yes'
#     then
#         passed=`expr $passed + 1`
#     else
#         failed=`expr $failed + 1`
#     fi
#     AC_CHECK_LIB(df,DFANputlabel,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
#     AC_MSG_CHECKING(if HDF package is complete)
#     if test $passed -gt 0
#     then
#     if test $failed -gt 0
#     then
#         AC_MSG_RESULT(no -- some components failed test)
#         have_hdf='no (failed tests)'
#     else
#         LIB_HDF='-ldf'
#         LIBS="$LIB_HDF $LIBS"
#         AC_DEFINE(HasHDF,1,Define if you have HDF library)
#         AC_MSG_RESULT(yes)
#         have_hdf='yes'
#     fi
#     else
#     AC_MSG_RESULT(no)
#     fi
# fi
# AM_CONDITIONAL(HasHDF, test "$have_hdf" = 'yes')
# AC_SUBST(LIB_HDF)

#
# Check for JBIG
#
have_jbig='no'
LIB_JBIG=''
if test "$with_jbig" != 'no'
then
    AC_MSG_CHECKING(for JBIG support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(jbig.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(jbig,jbg_dec_init,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if JBIG package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
        have_jbig='no (failed tests)'
    else
	LIB_JBIG='-ljbig'
	LIBS="$LIB_JBIG $LIBS"
	AC_DEFINE(HasJBIG,1,Define if you have JBIG library)
	AC_MSG_RESULT(yes)
        have_jbig='yes'
    fi
    else
        AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasJBIG, test "$have_jbig" = 'yes')
AC_SUBST(LIB_JBIG)

#
# Check for XML
#
have_xml='no'
LIB_XML=''
if test "$with_xml" != 'no'
then
    OLD_LDFLAGS=$LDFLAGS
    OLD_CPPFLAGS=$CPPFLAGS
    AC_MSG_CHECKING(for XML support )
    AC_MSG_RESULT()
    xml2_config=''
    AC_CHECK_PROGS(xml2_config,xml2-config,)dnl
    if test -n "$xml2_config"
    then
        # Debian installs libxml headers under /usr/include/libxml2/libxml with
        # the shared library installed under /usr/lib, whereas the package
        # installs itself under $prefix/libxml and $prefix/lib.
        xml2_prefix=`xml2-config --prefix`
        if test -d "${xml2_prefix}/include/libxml2"
        then
          CPPFLAGS="$CPPFLAGS -I${xml2_prefix}/include/libxml2"
        fi
        LDFLAGS="$LDFLAGS -L${xml2_prefix}/lib"
     fi
    failed=0;
    passed=0;
    AC_CHECK_HEADER(libxml/parser.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(xml2,xmlParseExternalEntity,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if XML package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
        have_xml='no (failed tests)'
        LDFLAGS="$OLD_LDFLAGS"
        CPPFLAGS="$OLD_CPPFLAGS"
    else
	LIB_XML='-lxml2'
	LIBS="$LIB_XML $LIBS"
	AC_DEFINE(HasXML,1,Define if you have XML library)
	AC_MSG_RESULT(yes)
        have_xml='yes'
    fi
    else
        AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasXML, test "$have_xml" = 'yes')
AC_SUBST(LIB_XML)

#
# Check for WMF
#

have_wmf='no'
LIB_WMF=''
LIB_WMF_DEPS=''
OLIBS="$LIBS"
if test "$with_wmf" != 'no'
then
  AC_MSG_CHECKING(for WMF support )
  AC_MSG_RESULT()

  have_libwmf='no'
  have_libwmflite='no'
  have_libwmf_ipa_h='no'

  AC_CHECK_HEADER(libwmf/ipa.h,have_libwmf_ipa_h='yes',)
  if test "$have_libwmf_ipa_h" = 'yes'
  then

    AC_CHECK_LIB(wmflite,wmf_lite_create,have_libwmflite='yes',,)
    if test "$have_libwmflite" = 'yes'
    then
      AC_DEFINE(HasWMFlite,1,Define if you have wmflite library)

      LIB_WMF='-lwmflite'
      LIBS="$LIB_WMF $LIBS"
      have_wmf='yes'
    else

      LIB_WMF_DEPS=''
      WMF_CONFIG_LIBS=`libwmf-config --libs`
      for lib in xml2 expat freetype jpeg png z
      do
        testlib="-l${lib}"
        echo "$WMF_CONFIG_LIBS" | grep -- "$testlib" > /dev/null && LIB_WMF_DEPS="$LIB_WMF_DEPS $testlib"
      done

      AC_CHECK_LIB(wmf,wmf_api_create,have_libwmf='yes',,$LIB_WMF_DEPS)
      if test "$have_libwmf" = 'yes'
      then
        AC_DEFINE(HasWMF,1,Define if you have wmf library)
        LIB_WMF='-lwmf'
        LIBS="$LIB_WMF $LIBS"
        have_wmf='yes'
      else
	AC_MSG_RESULT(no -- some components failed test)
        have_wmf='no (failed tests)'
        have_wmflite='no (failed tests)'
        LIBS="$OLIBS"
        LIB_WMF=''
      fi
    fi
  fi
fi
AC_MSG_CHECKING(if WMF package is complete)
if test "$have_wmf" = 'yes'
then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(HasWMF, test "$have_wmf" = 'yes')
AC_SUBST(LIB_WMF)
AC_SUBST(LIB_WMF_DEPS)

#
# Substitute compiler name to build/link PerlMagick
#
AC_SUBST(PERLMAINCC)

#
# Configure install Paths
#

# Subdirectory under lib to place GraphicsMagick lib files
MagickLibSubdir="${PACKAGE_NAME}-${PACKAGE_VERSION}"
AC_DEFINE_UNQUOTED(MagickLibSubdir,"$MagickLibSubdir",Subdirectory of lib where GraphicsMagick architecture dependent files are installed)

# Path to GraphicsMagick bin directory
MagickBinPath="${BIN_DIR}"
MagickBinPathDefine="${MagickBinPath}/"
if test "$native_win32_build" = 'yes'
then
  MagickBinPathDefine=`$WinPathScript "$MagickBinPathDefine" 1`
fi
AC_DEFINE_UNQUOTED(MagickBinPath,"$MagickBinPathDefine",Directory where executables are installed.)
AC_SUBST(MagickBinPath)

# Path to GraphicsMagick lib
MagickLibPath="${LIB_DIR}/${MagickLibSubdir}"
MagickLibPathDefine="${MagickLibPath}/"
if test "$native_win32_build" = 'yes'
then
  MagickLibPathDefine=`$WinPathScript "$MagickLibPathDefine" 1`
fi
AC_DEFINE_UNQUOTED(MagickLibPath,"$MagickLibPathDefine",Directory where architecture-dependent files live.)
AC_SUBST(MagickLibPath)

#
# Subdirectory under lib to place GraphicsMagick coder module files
MagickCoderModulesSubdir="${MagickLibSubdir}/modules-Q${QuantumDepth}/coders"
AC_DEFINE_UNQUOTED(MagickCoderModulesSubdir,"$MagickCoderModulesSubdir",Subdirectory of lib where GraphicsMagick coder modules are installed)
MagickCoderModulesPath="${LIB_DIR}/${MagickCoderModulesSubdir}"
MagickCoderModulesPathDefine="${MagickCoderModulesPath}/"
if test "$native_win32_build" = 'yes'
then
  MagickCoderModulesPathDefine=`$WinPathScript "$MagickCoderModulesPathDefine" 1`
fi
AC_DEFINE_UNQUOTED(MagickCoderModulesPath,"$MagickCoderModulesPathDefine",Location of coder modules)
AC_SUBST(MagickCoderModulesPath)

#
# Subdirectory under lib to place GraphicsMagick filter module files
MagickFilterModulesSubdir="${MagickLibSubdir}/modules-Q${QuantumDepth}/filters"
AC_DEFINE_UNQUOTED(MagickFilterModulesSubdir,"$MagickFilterModulesSubdir",Subdirectory of lib where GraphicsMagick filter modules are installed)
MagickFilterModulesPath="${LIB_DIR}/${MagickFilterModulesSubdir}"
MagickFilterModulesPathDefine="${MagickFilterModulesPath}/"
if test "$native_win32_build" = 'yes'
then
  MagickFilterModulesPathDefine=`$WinPathScript "$MagickFilterModulesPathDefine" 1`
fi
AC_DEFINE_UNQUOTED(MagickFilterModulesPath,"$MagickFilterModulesPathDefine",Location of filter modules)
AC_SUBST(MagickFilterModulesPath)

#
# Path to GraphicsMagick share files
MagickShareSubdir="${PACKAGE_NAME}-${PACKAGE_VERSION}"
MagickSharePath="${DATA_DIR}/${MagickShareSubdir}"
MagickSharePathDefine="${MagickSharePath}/"
if test "$native_win32_build" = 'yes'
then
  MagickSharePathDefine=`$WinPathScript "$MagickSharePathDefine" 1`
fi
AC_DEFINE_UNQUOTED(MagickSharePath,"$MagickSharePathDefine",Directory where architecture-independent files live.)
AC_SUBST(MagickSharePath)

#
# program_transform_name is formed for use in a Makefile, so create a
# modified version for use in a shell script.
configure_transform_name=`echo ${program_transform_name} | sed 's,\\$\\$,$,'`

# Default delegate definitions
AutotraceDecodeDelegateDefault='autotrace'
BZIPDelegateDefault='bzip2'
BrowseDelegateDefault='netscape'
CGMDecodeDelegateDefault='ralcgm'
CatDelegateDefault='cat'
### ConvertDelegateDefault=`echo convert | sed ${configure_transform_name}`
DOTDecodeDelegateDefault='dot'
DVIDecodeDelegateDefault='dvips'
EchoDelegateDefault='echo'
EditorDelegateDefault='xterm'
FIGDecodeDelegateDefault='fig2dev'
GMDelegateDefault=`echo gm | sed ${configure_transform_name}`
GnuplotDecodeDelegateDefault='gnuplot'
HPGLDecodeDelegateDefault='hp2xx'
HTMLDecodeDelegateDefault='html2ps'
ILBMDecodeDelegateDefault='ilbmtoppm'
ILBMEncodeDelegateDefault='ppmtoilbm'
LPDelegateDefault='lp'
LPRDelegateDefault='lpr'
LZWDecodeDelegateDefault='uncompress'
LZWEncodeDelegateDefault='compress'
LaunchDelegateDefault='gimp'
MANDelegateDefault='groff'
MPEGDecodeDelegateDefault='mpeg2decode'
MPEGEncodeDelegateDefault='mpeg2encode'
MVDelegateDefault='mv'
### MogrifyDelegateDefault=`echo mogrify | sed ${configure_transform_name}`
PGPDecodeDelegateDefault='pgpv'
POVDelegateDefault='povray'
if test "$native_win32_build" = 'yes' ; then
  PSDelegateDefault='gswin32c'
else
  PSDelegateDefault='gs'
fi
RADDecodeDelegateDefault='ra_ppm'
RLEEncodeDelegateDefault='rawtorle'
RMDelegateDefault='rm'
SCANDecodeDelegateDefault='scanimage'
### ShowImageDelegateDefault=`echo display | sed ${configure_transform_name}`
TXTDelegateDefault='enscript'
WMFDecodeDelegateDefault='wmf2eps'
WWWDecodeDelegateDefault='wget'
ZipDelegateDefault='gzip'

# Search for delegates
AC_PATH_PROG(AutotraceDecodeDelegate, "$AutotraceDecodeDelegateDefault", "$AutotraceDecodeDelegateDefault")
AC_PATH_PROG(BZIPDelegate, "$BZIPDelegateDefault", "$BZIPDelegateDefault")
AC_PATH_PROG(BrowseDelegate, "$BrowseDelegateDefault", "$BrowseDelegateDefault")
AC_PATH_PROG(CGMDecodeDelegate, "$CGMDecodeDelegateDefault", "$CGMDecodeDelegateDefault")
AC_PATH_PROG(CatDelegate, "$CatDelegateDefault", "$CatDelegateDefault")
AC_PATH_PROG(DOTDecodeDelegate, "$DOTDecodeDelegateDefault", "$DOTDecodeDelegateDefault")
AC_PATH_PROG(DVIDecodeDelegate, "$DVIDecodeDelegateDefault", "$DVIDecodeDelegateDefault")
AC_PATH_PROG(EchoDelegate, "$EchoDelegateDefault", "$EchoDelegateDefault")
AC_PATH_PROG(EditorDelegate, "$EditorDelegateDefault", "$EditorDelegateDefault")
AC_PATH_PROG(FIGDecodeDelegate, "$FIGDecodeDelegateDefault", "$FIGDecodeDelegateDefault")
AC_PATH_PROG(GMDelegate, "$GMDelegateDefault", "$GMDelegateDefault")
AC_PATH_PROG(GnuplotDecodeDelegate, "$GnuplotDecodeDelegateDefault", "$GnuplotDecodeDelegateDefault")
AC_PATH_PROG(HPGLDecodeDelegate, "$HPGLDecodeDelegateDefault", "$HPGLDecodeDelegateDefault")
AC_PATH_PROG(HTMLDecodeDelegate, "$HTMLDecodeDelegateDefault", "$HTMLDecodeDelegateDefault")
AC_PATH_PROG(ILBMDecodeDelegate, "$ILBMDecodeDelegateDefault", "$ILBMDecodeDelegateDefault")
AC_PATH_PROG(ILBMEncodeDelegate, "$ILBMEncodeDelegateDefault", "$ILBMEncodeDelegateDefault")
AC_PATH_PROG(LPDelegate, "$LPDelegateDefault", no)
AC_PATH_PROG(LPRDelegate, "$LPRDelegateDefault", no)
AC_PATH_PROG(LZWDecodeDelegate, "$LZWDecodeDelegateDefault", "$LZWDecodeDelegateDefault")
AC_PATH_PROG(LZWEncodeDelegate, "$LZWEncodeDelegateDefault", "$LZWEncodeDelegateDefault")
AC_PATH_PROG(LaunchDelegate, "$LaunchDelegateDefault", "$LaunchDelegateDefault")
AC_PATH_PROG(MANDelegate, "$MANDelegateDefault", "$MANDelegateDefault")
AC_PATH_PROG(MPEGDecodeDelegate, "$MPEGDecodeDelegateDefault", "$MPEGDecodeDelegateDefault")
AC_PATH_PROG(MPEGEncodeDelegate, "$MPEGEncodeDelegateDefault", "$MPEGEncodeDelegateDefault")
AC_PATH_PROG(MVDelegate, "$MVDelegateDefault", "$MVDelegateDefault")
AC_PATH_PROG(PGPDecodeDelegate, "$PGPDecodeDelegateDefault", "$PGPDecodeDelegateDefault")
AC_PATH_PROG(POVDelegate, "$POVDelegateDefault", "$POVDelegateDefault")
AC_PATH_PROG(PSDelegate, "$PSDelegateDefault", "$PSDelegateDefault")
AC_PATH_PROG(RADDecodeDelegate, "$RADDecodeDelegateDefault", "$RADDecodeDelegateDefault")
AC_PATH_PROG(RLEEncodeDelegate, "$RLEEncodeDelegateDefault", "$RLEEncodeDelegateDefault")
AC_PATH_PROG(RMDelegate, "$RMDelegateDefault", "$RMDelegateDefault")
AC_PATH_PROG(SCANDecodeDelegate, "$SCANDecodeDelegateDefault", "$SCANDecodeDelegateDefault")
AC_PATH_PROG(TXTDelegate, "$TXTDelegateDefault", "$TXTDelegateDefault")
AC_PATH_PROG(WMFDecodeDelegate, "$WMFDecodeDelegateDefault", "$WMFDecodeDelegateDefault")
AC_PATH_PROG(WWWDecodeDelegate, "$WWWDecodeDelegateDefault", "$WWWDecodeDelegateDefault")
AC_PATH_PROG(ZipDelegate, "$ZipDelegateDefault", "$ZipDelegateDefault")

# Prefer lpr to lp; lp needs options tacked on.
if test "$LPRDelegate" != no
then
  PrintDelegate="$LPRDelegate"
else
  PrintDelegate="$LPDelegate -c -s"
fi
AC_SUBST(PrintDelegate)

# Installed GraphicsMagick utiltity paths
GMDelegate="${BIN_DIR}/${GMDelegateDefault}"
### ConvertDelegate="${BIN_DIR}/${ConvertDelegateDefault}"
### ShowImageDelegate="${BIN_DIR}/${ShowImageDelegateDefault}"
### MogrifyDelegate="${BIN_DIR}/${MogrifyDelegateDefault}"

# Set delegate booleans
have_fig2dev='no' ; if test "$FIGDecodeDelegate" != "$FIGDecodeDelegateDefault" ; then have_fig2dev='yes' ; fi
have_gs='no'      ; if test "$PSDelegate" != "$PSDelegateDefault"; then have_gs='yes' ; fi
have_hp2xx='no'   ; if test "$HPGLDecodeDelegate" !=  "$HPGLDecodeDelegateDefault" ; then have_hp2xx='yes' ; fi
have_ilbmtoppm='no' ; if test "$ILBMDecodeDelegate" != "$ILBMDecodeDelegateDefault" ; then have_ilbmtoppm='yes' ; fi
have_ppmtoilbm='no' ; if test "$ILBMEncodeDelegate" != "$ILBMEncodeDelegateDefault" ; then have_ppmtoilbm='yes' ; fi
have_mpeg2decode='no' ; if test "$MPEGDecodeDelegate" != "$MPEGDecodeDelegateDefault" ; then have_mpeg2decode='yes' ; fi
have_mpeg2encode='no' ; if test "$MPEGEncodeDelegate" != "$MPEGEncodeDelegateDefault" ; then have_mpeg2encode='yes' ; fi
have_ra_ppm='no'  ; if test "$RADDecodeDelegate" != "$RADDecodeDelegateDefault" ; then have_ra_ppm='yes' ; fi
have_ralcgm='no'  ; if test "$CGMDecodeDelegate" != "$CGMDecodeDelegateDefault" ; then have_ralcgm='yes' ; fi

# Test for optional txt2html utility and define automake conditional HasTXT2HTML if found.
AC_PATH_PROG(TXT2HTML, txt2html, txt2html)
have_txt2html='no' ; if test "$TXT2HTML" != 'txt2html' ; then have_txt2html='yes' ; fi
AM_CONDITIONAL(HasTXT2HTML, test "$have_txt2html" = 'yes')

#
# Test for font directories
#
type_include_files=''

# Windows
windows_font_dir=''
if test "$with_windows_font_dir" != "yes" && test -n "$with_windows_font_dir"
then
   windows_font_dir="${with_windows_font_dir}/"
fi
if test -n "$windows_font_dir"
then
  type_include_files="$type_include_files "'<include file="type-windows.mgk" />'
fi
AC_SUBST(windows_font_dir)

# Adobe Postscript fonts on various systems
case $host_os in
  solaris*) type_include_files="$type_include_files "'<include file="type-solaris.mgk" />';;
esac

# Ghostscript
AC_MSG_CHECKING(for Ghostscript fonts directory)
ghostscript_font_dir=''
if test "${with_gs_font_dir}" != 'default'
then
  ghostscript_font_dir="${with_gs_font_dir}/"

else
  if test "${native_win32_build}" = 'yes'
  then
    ghostscript_font_dir="c:\\gs\\fonts\\"
    if test "${PSDelegate}" != 'gswin32c'
    then
      ghostscript_font_dir=`echo "${PSDelegate}" | sed -e 's:/gs/.*:/gs:;s:^/::;s/./&:/;s:/:\\\\:g'`"\\fonts\\"
    fi

  else
    # Red Hat Linux puts Ghostscript fonts in /usr/share/fonts/default/Type1
    for font_dir in '/usr/share/fonts/default/Type1/' "${prefix}/share/ghostscript/fonts/"
    do
      if test -f "${font_dir}a010013l.pfb"
      then
        ghostscript_font_dir="${font_dir}"
        break 1
      fi
    done

    if test "${ghostscript_font_dir}x" = 'x'
    then
      if test "$PSDelegate" != 'gs'
      then
        ghostscript_font_dir=`echo "$PSDelegate" | sed -e 's:/bin/gs:/share/ghostscript/fonts:'`"/"
      fi
    fi

  fi
fi
if test "${ghostscript_font_dir}x" != 'x'
then
  type_include_files="${type_include_files} "'<include file="type-ghostscript.mgk"/>'
  AC_MSG_RESULT($ghostscript_font_dir)
else
  AC_MSG_RESULT(not found!);
fi
AC_SUBST(ghostscript_font_dir)
if test "${native_win32_build}" = 'yes'
then
  PSDelegate=`$WinPathScript "$PSDelegate" 1`
fi

AC_SUBST(type_include_files)

#
# Handle case where user doesn't want frozen paths
#
if test "$with_frozenpaths" != 'yes'
then
  # Re-set delegate definitions to default (no paths)
  AutotraceDecodeDelegate="$AutotraceDecodeDelegateDefault"
  BZIPDelegate="$BZIPDelegateDefault"
  BrowseDelegate="$BrowseDelegateDefault"
  CGMDecodeDelegate="$CGMDecodeDelegateDefault"
  CatDelegate="$CatDelegateDefault"
  ConvertDelegate="$ConvertDelegateDefault"
  DOTDecodeDelegate="$DOTDecodeDelegateDefault"
  DVIDecodeDelegate="$DVIDecodeDelegateDefault"
  EchoDelegate="$EchoDelegateDefault"
  EditorDelegate="$EditorDelegateDefault"
  FIGDecodeDelegate="$FIGDecodeDelegateDefault"
  GnuplotDecodeDelegate="$GnuplotDecodeDelegateDefault"
  HPGLDecodeDelegate="$HPGLDecodeDelegateDefault"
  HTMLDecodeDelegate="$HTMLDecodeDelegatDefault"
  ILBMDecodeDelegate="$ILBMDecodeDelegateDefault"
  ILBMEncodeDelegate="$ILBMEncodeDelegateDefault"
  LPDelegate="$LPDelegateDefault"
  LZWDecodeDelegate="$LZWDecodeDelegateDefault"
  LZWEncodeDelegate="$LZWEncodeDelegateDefault"
  LaunchDelegate="$LaunchDelegateDefault"
  MANDelegate="$MANDelegateDefault"
  MPEGDecodeDelegate="$MPEGDecodeDelegateDefault"
  MPEGEncodeDelegate="$MPEGEncodeDelegateDefault"
  MVDelegate="$MVDelegateDefault"
  MogrifyDelegate="$MogrifyDelegateDefault"
  PGPDecodeDelegate="$PGPDecodeDelegateDefault"
  POVDelegate="$POVDelegateDefault"
  PSDelegate="$PSDelegateDefault"
  RADDecodeDelegate="$RADDecodeDelegateDefault"
  RLEEncodeDelegate="$RLEEncodeDelegateDefault"
  RMDelegate="$RMDelegateDefault"
  SCANDecodeDelegate="$SCANDecodeDelegateDefault"
  ShowImageDelegate="$ShowImageDelegateDefault"
  TXTDelegate="$TXTDelegateDefault"
  WMFDecodeDelegate="$WMFDecodeDelegateDefault"
  WWWDecodeDelegate="$WWWDecodeDelegateDefault"
  ZipDelegate="$ZipDelegateDefault"
fi

# Delegate substitutions
AC_SUBST(AutotraceDecodeDelegate)
AC_SUBST(BZIPDelegate)
AC_SUBST(BrowseDelegate)
AC_SUBST(CGMDecodeDelegate)
AC_SUBST(CatDelegate)
AC_SUBST(ConvertDelegate)
AC_SUBST(DOTDecodeDelegate)
AC_SUBST(DVIDecodeDelegate)
AC_SUBST(EchoDelegate)
AC_SUBST(EditorDelegate)
AC_SUBST(FIGDecodeDelegate)
AC_SUBST(GnuplotDecodeDelegate)
AC_SUBST(HPGLDecodeDelegate)
AC_SUBST(HTMLDecodeDelegate)
AC_SUBST(ILBMDecodeDelegate)
AC_SUBST(ILBMEncodeDelegate)
AC_SUBST(LPDelegate)
AC_SUBST(LZWDecodeDelegate)
AC_SUBST(LZWEncodeDelegate)
AC_SUBST(LaunchDelegate)
AC_SUBST(MANDelegate)
AC_SUBST(MPEGDecodeDelegate)
AC_SUBST(MPEGEncodeDelegate)
AC_SUBST(MVDelegate)
AC_SUBST(MogrifyDelegate)
AC_SUBST(PGPDecodeDelegate)
AC_SUBST(POVDelegate)
AC_SUBST(PSDelegate)
AC_SUBST(RADDecodeDelegate)
AC_SUBST(RLEEncodeDelegate)
AC_SUBST(RMDelegate)
AC_SUBST(SCANDecodeDelegate)
AC_SUBST(ShowImageDelegate)
AC_SUBST(TXTDelegate)
AC_SUBST(WMFDecodeDelegate)
AC_SUBST(WWWDecodeDelegate)
AC_SUBST(ZipDelegate)

#
# Ghostscript related configuration.
#
GSColorDevice=ppmraw
GSMonoDevice=pbmraw
GSPDFDevice=pdfwrite
GSPSDevice=pswrite
GSEPSDevice=epswrite
GSVersion='unknown'
if test $have_gs = 'yes'
then

  AC_MSG_CHECKING(for Ghostscript version)
  if GSVersion=`$PSDelegate --version`
  then
    :
  else
    GSVersion=`$PSDelegate --help | sed -e '1q' | awk '{ print $3 }'`
  fi
  AC_MSG_RESULT($GSVersion)

  # GSColorDevice
  AC_MSG_CHECKING(for gs color device)
  if $PSDelegate -q -dBATCH -sDEVICE=pnmraw -sOutputFile=/dev/null < /dev/null 2> /dev/null
  then
    GSColorDevice=pnmraw
  else
    if $PSDelegate -q -dBATCH -sDEVICE=ppmraw -sOutputFile=/dev/null < /dev/null 2> /dev/null
    then
      GSColorDevice=ppmraw
    else
      GSColorDevice=ppmraw
    fi
  fi
  AC_MSG_RESULT($GSColorDevice)

  # GSMonoDevice
  AC_MSG_CHECKING(for gs mono device)
  if $PSDelegate -q -dBATCH -sDEVICE=pbmraw -sOutputFile=/dev/null < /dev/null 2> /dev/null
  then
    GSMonoDevice=pbmraw
  else
    GSMonoDevice=ppmraw
  fi
  AC_MSG_RESULT($GSMonoDevice)

  # GSPDFDevice
  AC_MSG_CHECKING(for gs PDF writing device)
  if $PSDelegate -q -dBATCH -sDEVICE=pdfwrite -sOutputFile=/dev/null < /dev/null 2> /dev/null
  then
    GSPDFDevice=pdfwrite
  else
    GSPDFDevice=nodevice
  fi
  AC_MSG_RESULT($GSPDFDevice)

  # GSPSDevice
  AC_MSG_CHECKING(for gs PS writing device)
  if $PSDelegate -q -dBATCH -sDEVICE=pswrite -sOutputFile=/dev/null < /dev/null 2> /dev/null
  then
    GSPSDevice=pswrite
  else
    GSPSDevice=nodevice
  fi
  AC_MSG_RESULT($GSPSDevice)

  # GSEPSDevice
  AC_MSG_CHECKING(for gs EPS writing device)
  if $PSDelegate -q -dBATCH -sDEVICE=epswrite -sOutputFile=/dev/null < /dev/null 2> /dev/null
  then
    GSEPSDevice=epswrite
  else
    GSEPSDevice=nodevice
  fi
  AC_MSG_RESULT($GSEPSDevice)

fi

AC_SUBST(GSMonoDevice)
AC_SUBST(GSPDFDevice)
AC_SUBST(GSPSDevice)
AC_SUBST(GSColorDevice)
AC_SUBST(GSEPSDevice)
AC_SUBST(GSVersion)

#
# PerlMagick-related configuration
#

# Look for PERL if PerlMagick requested
# If name/path of desired PERL interpreter is specified, look for that one first
have_perl='no'
if test "$with_perl" != 'no'
then
    if test "$with_perl" != 'yes'
    then
	AC_CACHE_CHECK(for perl,ac_cv_path_PERL,ac_cv_path_PERL="$with_perl");
	PERL=$ac_cv_path_PERL
	AC_SUBST(PERL)dnl
	have_perl="$ac_cv_path_PERL"
    else
	AC_PATH_PROGS(PERL,perl perl5,)dnl
	if test "$ac_cv_path_PERL"
        then
	  have_perl="$ac_cv_path_PERL"
        fi
    fi
fi

with_perl_static='no'
with_perl_dynamic='no'
if test "$have_perl" != 'no'
then
  if test "$with_perl" != 'no' && test "$libtool_build_shared_libs" = 'no'
  then
    with_perl_static='yes'
  fi
  if test "$with_perl" != 'no' && test "$libtool_build_shared_libs" = 'yes'
  then
    with_perl_dynamic='yes'
  fi
fi
AM_CONDITIONAL(WITH_PERL, test "$have_perl" != 'no')
AM_CONDITIONAL(WITH_PERL_STATIC, test $with_perl_static = 'yes')
AM_CONDITIONAL(WITH_PERL_DYNAMIC, test $with_perl_dynamic = 'yes')

# Determine path to pick up GraphicsMagick library from for use with building PerlMagick
MAGICKLIBDIR="${LIB_DIR}"
MAGICKLIB="-L${MAGICKLIBDIR} -lGraphicsMagick"
if test $with_perl_static = 'yes'
then
  # Find out where libtool hides its uninstalled libraries (as libtool_objdir)
  eval `./libtool --config|grep '^objdir='|sed -e 's/^objdir/libtool_objdir/'`

  # Find out what extension is applied to static libraries (as libtool_libext)
  #eval `./libtool --config|grep '^libext='|sed -e 's/^libext/libtool_libext/'`

  # Find out full form of library name (as libtool_libname_spec)
  #eval `./libtool --config|grep '^libname_spec='|sed -e 's/^libname_spec/libtool_libname_spec/'`
  #eval 'name=GraphicsMagick '`eval 'echo library_name="${libtool_libname_spec}.${libtool_libext}"'`

  # Explicit path to static library (Perl rejects it!)
  #MAGICKLIB="${builddir}/magick/${libtool_objdir}/${library_name}"

  # Linker search path to library, followed by -lGraphicsMagick
  MAGICKLIBDIR="${builddir}/magick/${libtool_objdir}"
  MAGICKLIB="-L${MAGICKLIBDIR} -lGraphicsMagick"
fi
AC_SUBST(MAGICKLIB)
AC_SUBST(MAGICKLIBDIR)

# Create a simple string containing format names for all delegate libraries
DELEGATES=''
if test "$have_bzlib"  = 'yes' ; then DELEGATES="$DELEGATES bzlib"  ; fi
if test "$have_ralcgm" = 'yes' ; then DELEGATES="$DELEGATES cgm"  ; fi
if test "$have_fpx"    = 'yes' ; then DELEGATES="$DELEGATES fpx"  ; fi
if test "$have_hp2xx"  = 'yes' ; then DELEGATES="$DELEGATES hpgl"  ; fi
# if test "$have_hdf"    = 'yes' ; then DELEGATES="$DELEGATES hdf"  ; fi
if test "$have_jbig"   = 'yes' ; then DELEGATES="$DELEGATES jbig" ; fi
if test "$have_png$have_jpeg" = 'yesyes' ; then DELEGATES="$DELEGATES jng"; fi
if test "$have_jp2"   = 'yes' ; then DELEGATES="$DELEGATES jp2" ; fi
if test "$have_jpeg"   = 'yes' ; then DELEGATES="$DELEGATES jpeg" ; fi
if test "$have_lcms"    = 'yes' ; then DELEGATES="$DELEGATES lcms"  ; fi
# if test "$have_mpeg2"   = 'yes' ; then DELEGATES="$DELEGATES mpeg2" ; fi
if test "$have_mpeg2decode" = 'yes' && test "$have_mpeg2encode"  = 'yes' ; then DELEGATES="$DELEGATES mpeg" ; fi
if test "$have_png"    = 'yes' ; then DELEGATES="$DELEGATES png"  ; fi

have_ps='no'
if test "$have_dps"    = 'yes' || test "$have_gs" = 'yes' ; then have_ps='yes'  ; fi
if test "$have_ps"     = 'yes' ; then DELEGATES="$DELEGATES ps"  ; fi

if test "$have_ra_ppm" = 'yes' ; then DELEGATES="$DELEGATES rad" ; fi
if test "$have_tiff"   = 'yes' ; then DELEGATES="$DELEGATES tiff" ; fi
if test "$have_ttf"    = 'yes' ; then DELEGATES="$DELEGATES ttf"  ; fi
if test "$have_wmf"    = 'yes' ; then DELEGATES="$DELEGATES wmf"  ; fi
if test "$have_x"      = 'yes' ; then DELEGATES="$DELEGATES x"    ; fi
if test "$have_fig2dev" = 'yes' && test "$have_ps" = 'yes'  ; then DELEGATES="$DELEGATES xfig"    ; fi
if test "$have_zlib"   = 'yes' ; then DELEGATES="$DELEGATES zlib" ; fi

AC_SUBST(DELEGATES)

#
# Handle special compiler flags
#

# Add '-p' if prof source profiling support enabled
if test "$with_prof" = 'yes'
then
  CFLAGS="-p $CFLAGS"
  CXXFLAGS="-p $CXXFLAGS"
  LDFLAGS="-p $LDFLAGS"
fi

# Add '-pg' if gprof source profiling support enabled
if test "$with_gprof" = 'yes'
then
  CFLAGS="-pg $CFLAGS"
  CXXFLAGS="-pg $CXXFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Add '-ftest-coverage -fprofile-arcs' if gcov source profiling support enabled
# This is a gcc-specific feature
if test "$with_gcov" = 'yes'
then
  CFLAGS="-ftest-coverage -fprofile-arcs  $CFLAGS"
  CXXFLAGS="-ftest-coverage -fprofile-arcs  $CXXFLAGS"
  LDFLAGS="-ftest-coverage -fprofile-arcs $LDFLAGS"
fi

#
# Build library dependency list for libMagick
#
if test "$with_modules" != 'no'
then
  MAGICK_DEP_LIBS="$LIB_LCMS $LIB_TTF $LIB_GS $LIB_XEXT $LIB_IPC $LIB_X11 $LIB_BZLIB $LIB_ZLIB $LIB_GDI32 $LIB_THREAD $LIB_LF $LIB_MATH $LIB_CCMALLOC $LIBS_INITIAL"
else
  MAGICK_DEP_LIBS="$LIB_JBIG $LIB_LCMS $LIB_TIFF $LIB_TTF $LIB_JP2 $LIB_JPEG $LIB_GS $LIB_PNG $LIB_FPX $LIB_WMF $LIB_DPS $LIB_XEXT $LIB_IPC $LIB_X11 $LIB_BZLIB $LIB_XML $LIB_ZLIB $LIB_GDI32 $LIB_THREAD $LIB_LF $LIB_MATH $LIB_CCMALLOC $LIBS_INITIAL"
fi
AC_SUBST(MAGICK_DEP_LIBS)

#
# Remove extraneous spaces from output variables (asthetic)
#
X_CFLAGS=`echo $X_CFLAGS | sed -e 's/  */ /g'`
X_PRE_LIBS=`echo $X_PRE_LIBS | sed -e 's/  */ /g'`
X_LIBS=`echo $X_LIBS | sed -e 's/  */ /g'`
X_EXTRA_LIBS=`echo $X_EXTRA_LIBS | sed -e 's/  */ /g'`

CC=`echo $CC | sed -e 's/  */ /g'`
CFLAGS=`echo $CFLAGS | sed -e 's/  */ /g'`
CPPFLAGS=`echo $CPPFLAGS | sed -e 's/  */ /g'`
CXXFLAGS=`echo $CXXFLAGS | sed -e 's/  */ /g'`
LDFLAGS=`echo $LDFLAGS | sed -e 's/  */ /g'`
TESTED_LIBS=`echo $LIBS | sed -e 's/  */ /g'`
MAGICK_DEP_LIBS=`echo $MAGICK_DEP_LIBS | sed -e 's/  */ /g'`
#LIBS=`echo $LIBS | sed -e 's/  */ /g'`
LIBS=""

#AC_SUBST(CPPFLAGS)
AC_SUBST(X_CFLAGS)
#AC_SUBST(LDFLAGS)
#AC_SUBST(X_PRE_LIBS)
#AC_SUBST(X_LIBS)
#AC_SUBST(X_EXTRA_LIBS)

AC_OUTPUT( \
        Makefile \
        magick/Makefile \
        coders/delegates.mgk \
        coders/Makefile \
        utilities/Makefile \
        tests/Makefile \
        coders/type-ghostscript.mgk \
        coders/type-solaris.mgk \
        coders/type-windows.mgk \
        coders/type.mgk \
        filters/Makefile \
        ltdl/Makefile \
        magick/GraphicsMagick-config \
        magick/GraphicsMagick.pc \
        magick/version.h \
        Magick++/Makefile \
        Magick++/bin/GraphicsMagick++-config \
        Magick++/bin/Makefile \
        Magick++/demo/Makefile \
        Magick++/lib/GraphicsMagick++.pc \
        Magick++/lib/Makefile \
        Magick++/lib/Magick++/Makefile \
        Magick++/tests/Makefile \
        PerlMagick/Magick.pm \
        PerlMagick/Makefile.PL )
rm -f magick-version

echo ""

echo "GraphicsMagick is configured as follows. Please verify that this"
echo "configuration matches your expectations."
echo ""
echo "Host system type : $host"
echo ""
echo "Option            Configure option           	Configured value"
echo "-----------------------------------------------------------------"
echo "Shared libraries  --enable-shared=$enable_shared    	$libtool_build_shared_libs"
echo "Static libraries  --enable-static=$enable_static    	$libtool_build_static_libs"
echo "GNU ld            --with-gnu-ld=$with_gnu_ld        	$lt_cv_prog_gnu_ld"
echo "LZW support       --enable-lzw=$with_lzw            	$with_lzw"
echo "Quantum depth     --with-quantum-depth=$with_quantum_depth 	$with_quantum_depth"
echo ""
echo "Delegate Configuration:"
echo "BZLIB             --with-bzlib=$with_bzlib          	$have_bzlib"
echo "DPS               --with-dps=$with_dps              	$have_dps"
echo "FlashPIX          --with-fpx=$with_fpx              	$have_fpx"
echo "FreeType 2.0      --with-ttf=$with_ttf          	$have_ttf"
echo "Ghostscript       None                   	$PSDelegate ($GSVersion)"
result_ghostscript_font_dir='none'
if test "${ghostscript_font_dir}x" != 'x'
then
  result_ghostscript_font_dir="$ghostscript_font_dir"
fi
echo "Ghostscript fonts --with-gs-font-dir=$with_gs_font_dir    $result_ghostscript_font_dir"
echo "Ghostscript lib   --with-gslib=$with_gslib       	$have_gslib"
# echo "HDF               --with-hdf=$with_hdf          	$have_hdf"
echo "JBIG              --with-jbig=$with_jbig        	$have_jbig"
echo "JPEG v1           --with-jpeg=$with_jpeg        	$have_jpeg"
echo "JPEG-2000         --with-jp2=$with_jp2          	$have_jp2"
echo "LCMS              --with-lcms=$with_lcms        	$have_lcms"
# echo "MPEG v2           --with-mpeg2=$with_mpeg2      	$have_mpeg2"
echo "Magick++          --with-magick-plus-plus=$with_magick_plus_plus 	$have_magick_plus_plus"
echo "PERL              --with-perl=$with_perl            	$have_perl"
echo "PNG               --with-png=$with_png          	$have_png"
echo "TIFF              --with-tiff=$with_tiff        	$have_tiff"
result_windows_font_dir='none'
if test "${windows_font_dir}x" != 'x'
then
  result_windows_font_dir="${windows_font_dir}"
fi
echo "Windows fonts     --with-windows-font-dir=$with_windows_font_dir	$result_windows_font_dir"
echo "WMF               --with-wmf=$with_wmf          	$have_wmf"
echo "X11               --with-x=$with_x             	$have_x"
echo "XML               --with-xml=$with_xml          	$have_xml"
echo "ZLIB              --with-zlib=$with_zlib        	$have_zlib"
echo ""
echo "X11 Configuration:"
if test "$have_x" != 'no'
then
  echo "  X_CFLAGS     = $X_CFLAGS"
  echo "  X_PRE_LIBS   = $X_PRE_LIBS"
  echo "  X_LIBS       = $X_LIBS"
  echo "  X_EXTRA_LIBS = $X_EXTRA_LIBS"
else
  echo ""
  echo "  Not using X11."
fi
echo ""
echo "Options used to compile and link:"
echo "  CC       = $CC"
echo "  CFLAGS   = $CFLAGS"
echo "  CPPFLAGS = $CPPFLAGS"
echo "  CXX      = $CXX"
echo "  CXXFLAGS = $CXXFLAGS"
echo "  LDFLAGS  = $LDFLAGS"
#echo "  LIBS     = $TESTED_LIBS"
echo "  LIBS     = $MAGICK_DEP_LIBS"
echo ""

