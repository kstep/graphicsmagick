#
# Package name and versioning information for GraphicsMagick
#
# This file is sourced by a Bourne shell (/bin/sh) script so it must
# observe Bourne shell syntax.
#
# Package base name
PACKAGE_NAME='GraphicsMagick'
#
# Package version.  This is is the numeric version suffix applied to
# PACKAGE_NAME (e.g. "1.0").
PACKAGE_VERSION='1.1'

#
# Formal Package release date
#PACKAGE_RELEASE_DATE="05/05/03"
PACKAGE_RELEASE_DATE="unreleased"

#
# Package version addendum.  This is an arbitrary suffix (if any)
# appended to the package version. (e.g. "beta1")
# Extract the most recent update date from the ChangeLog
CHANGE_DATE=`awk '/^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]/ { print substr($1,1,4) substr($1,6,2) substr($1,9,2); exit; }' ${srcdir}/ChangeLog`
PACKAGE_VERSION_ADDENDUM="-${CHANGE_DATE}"

# To hardcode the version addendum, uncomment the following line
#PACKAGE_VERSION_ADDENDUM=''

#
# Libtool library revision control info
# See the libtool documentation under the heading "Libtool's versioning
# system" in order to understand the meaning of these fields
#
# Here are a set of rules to help you update your library version
# information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#  2. Update the version information only immediately before a public
#     release of your software. More frequent updates are unnecessary, and
#     only guarantee that the current interface number gets larger faster.
#  3. If the library source code has changed at all since the last update,
#     then increment revision (`c:r:a' becomes `c:r+1:a').
#  4. If any interfaces have been added, removed, or changed since the last
#     update, increment current, and set revision to 0.
#  5. If any interfaces have been added since the last public release, then
#     increment age.
#  6. If any interfaces have been removed since the last public release,
#     then set age to 0.
LIBRARY_CURRENT=0
LIBRARY_REVISION=2
LIBRARY_AGE=0

