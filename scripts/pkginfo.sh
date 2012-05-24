#!/bin/sh
#
# Print the package version based on content of $srcdir/version.sh.
#
# This script is expected to be run under the context of a configure
# script (via AC_INIT) to dynamically determine the package version.
me=$0
info=$1
srcdir=`dirname $0`/..
value=
nl='
'
. version.sh

case "${info}" in
  package_bugreport )
    value="${PACKAGE_BUGREPORT}"
    ;;
  package_name )
    value="${PACKAGE_NAME}"
    ;;
  package_version )
    value="${PACKAGE_VERSION}${PACKAGE_VERSION_ADDENDUM}"
    ;;
  * )
    echo "$me: Must supply argument {package_bugreport, package_name, package_version}"
    exit 1
    ;;
esac

# Omit the trailing newline, so that m4_esyscmd can use the result directly.
echo "${value}" | tr -d "$nl"
