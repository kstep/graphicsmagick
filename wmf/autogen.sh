#!/bin/sh
# Run this to generate all the initial makefiles, etc.

libtoolize --help > /dev/null              || exit 1
autoconf   --help > /dev/null 2> /dev/null || exit 1
automake   --help > /dev/null              || exit 1

# This file adapted from Gnumeric's autogen.sh

case $CC in
xlc )
	am_opt=--include-deps;;
esac

echo "Running: libtoolize"
libtoolize --force --copy || {
	echo "**Error**: libtoolize failed.";
	exit 1;
}

echo "Running: aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS || {
	echo "**Error**: aclocal failed. This may mean that you have not"
	echo "installed all of the packages you need, or you may need to"
	echo "set ACLOCAL_FLAGS to include \"-I \$prefix/share/aclocal\""
	echo "for the prefix where you installed the packages whose"
	echo "macros were not found"
	exit 1
}

echo "Running: autoheader"
autoheader || {
	echo "**Error**: autoheader failed.";
	exit 1;
}

echo "Running: automake $am_opt"
automake $am_opt || {
	echo "**Error**: automake failed.";
	exit 1;
}

echo "Running: autoconf"
autoconf || {
	echo "**Error**: autoconf failed.";
	exit 1;
}

# GnuWin32 fontmap defaults
# '--prefix=c:/progra~1/libwmf'
# '--with-sysfontmap=c:/progra~1/libwmf/share/libwmf/fonts/fontmap'
# '--with-xtrafontmap=./fontmap'

echo Running: configure $@
configure $@ || exit 1

echo Now type \`make\' to compile libwmf 
echo
