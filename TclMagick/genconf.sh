#!/bin/sh
#
# Execute this script to update the build environment if the version of
# Automake or Autoconf is changed.
#

libtoolize --verbose --copy --force
aclocal -I unix/m4
#autoheader
autoconf
automake --verbose --add-missing --copy --force-missing

