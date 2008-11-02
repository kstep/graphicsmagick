#!/bin/sh
# Copyright (C) 2008 GraphicsMagick Group
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

#Author: Max Hohenegger
#Last change: 2008-10-01
#Module: conjure, MSL
#Description: Test whether compositing images over a background image using
#             gravity delivers the same result using conjure (MSL) and convert

set -e # Exit on any error
. ${srcdir}/utilities/tests/common.sh

COMPOSITE_CMD="${GM} composite"
CONJURE_CMD="${GM} conjure -debug exception"
CONVERT_CMD="${GM} convert"
COMPARE_CMD="${GM} compare"
DISPLAY_CMD="${GM} display"

FORMAT="_out.pnm"

BLANK="comp_blank$FORMAT"
CENTER="comp_Center$FORMAT"
NORTH="comp_North$FORMAT"
NORTHEAST="comp_NorthEast$FORMAT"
EAST="comp_East$FORMAT"
SOUTHEAST="comp_SouthEast$FORMAT"
SOUTH="comp_South$FORMAT"
SOUTHWEST="comp_SouthWest$FORMAT"
WEST="comp_West$FORMAT"
NORTHWEST="comp_NorthWest$FORMAT"

RESULT="comp_result_out$FORMAT"
CONJURE_RESULT="comp_conjure_result_out$FORMAT"
COMPARED="comp_compared$FORMAT"


draw_text ()
{
    TEMPFILE=draw_text_tmp.msl
    TEXT=$1
    TARGET=$2

    cat <<EOF >$TEMPFILE
<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="120" height="100" xmlns="http://www.w3.org/2000/svg">
    <style type="text/css"><![CDATA[ text {font-size:20px; text-anchor:middle;} ]]></style>
    <text x="50" y="50">$TEXT</text>
</svg>
EOF

    $CONVERT_CMD $TEMPFILE $TARGET || exit 1
    rm -f $TEMPFILE
}

draw_blank ()
{
    TEMPFILE=draw_blank_tmp.msl
    TARGET=$1

    cat <<EOF >$TEMPFILE
<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="500" height="500"
 xmlns="http://www.w3.org/2000/svg">
</svg>
EOF

    $CONVERT_CMD $TEMPFILE $TARGET || exit 2
    rm -f $TEMPFILE
}

########################## PREPARE TEST OBJECTS ###########################
draw_blank $BLANK
draw_text "Center" $CENTER
draw_text "North" $NORTH
draw_text "NorthEast" $NORTHEAST
draw_text "East" $EAST
draw_text "SouthEast" $SOUTHEAST
draw_text "South" $SOUTH
draw_text "SouthWest" $SOUTHWEST
draw_text "West" $WEST
draw_text "NorthWest" $NORTHWEST

########################## CREATE TEST BASE ###########################
cp $BLANK $RESULT
$COMPOSITE_CMD $CENTER    $RESULT -gravity Center    $RESULT || exit 3
$COMPOSITE_CMD $NORTH     $RESULT -gravity North     $RESULT || exit 3
$COMPOSITE_CMD $NORTHEAST $RESULT -gravity NorthEast $RESULT || exit 3
$COMPOSITE_CMD $EAST      $RESULT -gravity East      $RESULT || exit 3
$COMPOSITE_CMD $SOUTHEAST $RESULT -gravity SouthEast $RESULT || exit 3
$COMPOSITE_CMD $SOUTH     $RESULT -gravity South     $RESULT || exit 3
$COMPOSITE_CMD $SOUTHWEST $RESULT -gravity SouthWest $RESULT || exit 3
$COMPOSITE_CMD $WEST      $RESULT -gravity West      $RESULT || exit 3
$COMPOSITE_CMD $NORTHWEST $RESULT -gravity NorthWest $RESULT || exit 3

########################## CREATE TEST OBJECT ###########################
TEMPFILE=composite_tmp.msl
cat <<EOF >$TEMPFILE
<?xml version="1.0" encoding="utf-8" ?>
<group>
    <image id="1"><read filename="$CENTER"/></image>
    <image id="2"><read filename="$NORTH"/></image>
    <image id="3"><read filename="$NORTHEAST"/></image>
    <image id="4"><read filename="$EAST"/></image>
    <image id="5"><read filename="$SOUTHEAST"/></image>
    <image id="6"><read filename="$SOUTH"/></image>
    <image id="7"><read filename="$SOUTHWEST"/></image>
    <image id="8"><read filename="$WEST"/></image>
    <image id="9"><read filename="$NORTHWEST"/></image>
    <image>
        <read filename="$BLANK"/>
        <composite image="1" gravity="Center"/>
        <composite image="2" gravity="North"/>
        <composite image="3" gravity="NorthEast"/>
        <composite image="4" gravity="East"/>
        <composite image="5" gravity="SouthEast"/>
        <composite image="6" gravity="South"/>
        <composite image="7" gravity="SouthWest"/>
        <composite image="8" gravity="West"/>
        <composite image="9" gravity="NorthWest"/>
    </image>
    <write filename="$CONJURE_RESULT"/>
</group>
EOF
$CONJURE_CMD $TEMPFILE

################### COMPARE TEST OBJECT TO TESTBASE #########################

#gm display $RESULT
#gm display $CONJURE_RESULT

cmp $RESULT $CONJURE_RESULT
status=$?
if test $status -eq 0
then
  rm -f $TEMPFILE $BLANK $CENTER $NORTH $NORTHEAST $EAST $SOUTHEAST $SOUTH \
    $SOUTHWEST $WEST $NORTHWEST $RESULT $CONJURE_RESULT
fi
#diff --report-identical-files $RESULT $CONJURE_RESULT
#$COMPARE_CMD $RESULT $CONJURE_RESULT $COMPARED || exit 5
#$DISPLAY_CMD $COMPARED || exit 6

exit $status
