AC_PREREQ(2.2)
AC_INIT(fpx/buffdesc.h)

# Set version
MAJOR_VERSION=1
MINOR_VERSION=2
MICRO_VERSION=0
PATCH_VERSION=4
echo "configuring Digital Imaging Group FlashPIX $MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION.$PATCH_VERSION"
AC_CANONICAL_SYSTEM
AC_SUBST(MAJOR_VERSION)dnl
AC_SUBST(MINOR_VERSION)dnl
AC_SUBST(MICRO_VERSION)dnl
AC_SUBST(PATCH_VERSION)dnl

# Ensure that make can run correctly
AM_SANITY_CHECK

AM_INIT_AUTOMAKE(libfpx,$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION.$PATCH_VERSION, ' ')

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

CPPFLAGS="-D_UNIX $CPPFLAGS"

# Check for programs
AC_PROG_CC
AC_PROG_CPP
AM_PROG_LD
AC_SUBST(LD)
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S

#
# Tests for Windows
#
AC_CYGWIN
AC_EXEEXT
AC_OBJEXT

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN

AC_CHECK_HEADERS(wchar.h)

#
# Configure libtool and ltdl
#
AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(yes)
AC_LIBLTDL_CONVENIENCE
# Substitute INCLTDL and LIBLTDL in the Makefiles
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
# Configure libtool
AC_PROG_LIBTOOL

AC_LANG_CPLUSPLUS
AC_PROG_CXX

AC_OUTPUT(Makefile basics/Makefile fpx/Makefile jpeg/Makefile \
  ole/Makefile ri_image/Makefile oless/Makefile)
