###########################################################
# DrawingWand test
# - The purpose is to test the correctness of TclMagick,
#   not to produce meaningful output
#
###########################################################


############################################
# Load debugging version
# or require TclMagick package from library
#

if { $tcl_platform(platform) == "unix" } {
    set auto_path [linsert $auto_path 0 [file join .. unix]]
    package require TclMagick
} else {
    set dll [file join .. win debug tclMagick.dll]
    if {[file exists $dll]} {
	load $dll
    }
    package require TclMagick
}
puts [info script]


##########################################
# Global options
#
set IMG "xc:white"
set TMP [file join .. tmp]
set FONT "Ariel"        ;# undefine FONT if none available

##########################################
# Check which tests should be performed
#
set TestFunctions {
    DrawTest            img     1
    PathTest            img     1
}

############################################
# Command debugging
#
catch { 
    wm withdraw . 
    console show 
    console eval {wm protocol . WM_DELETE_WINDOW exit}
}
proc debug {args} {
    foreach cmdMask $args {
        foreach cmd [info commands $cmdMask] {
            trace add execution $cmd leave debugLeave
        }
    }
}
proc debugLeave {cmdstr code result op} {
    puts [format "    %s ==>{%s}" [string range $cmdstr 0 50] [string range $result 0 50]]
    update
}

##########################################
# Single image tests
#
proc DrawTest {img} {
    set wand [$img clone imgX]
    set draw [magick create draw draw0]
    debug $draw $wand
    
    [magick create pixel pix0] SetColor "lightblue"
    [magick create pixel pix1] SetColor "blue"
    [magick create pixel pix2] SetColor "red"
    [magick create pixel pix3] SetColor "yellow"
    [magick create pixel pix4] SetColor "brown"
    [magick create pixel pix5] SetColor "lightgreen"
    [magick create pixel pix6] SetColor "green"
    [magick create pixel pix7] SetColor "lightgray"
    [magick create pixel pix8] SetColor "black"

    $draw PushGraphicContext
        $draw SetStrokeWidth 1.0
        $draw SetStrokeColor pix1
        $draw SetFillColor pix1
        $draw SetFontSize 24
        $draw Annotation 5 30 "Created by TclMagick:"
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetStrokeWidth 1.0
        $draw SetStrokeColor pix8
        $draw SetFillColor pix2
        $draw Arc 50 50 100 100 45 -45
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetStrokeWidth 1.0
        $draw SetStrokeColor pix1
        $draw SetFillColor pix3
        $draw Bezier 110 110 140 10 170 120 150 150 110 110
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetStrokeWidth 1.0
        $draw SetStrokeColor pix4
        $draw SetFillColor pix7
        $draw Circle 100 200 50 200
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetStrokeColor pix8
        $draw SetFillColor pix6
        $draw SetFillOpacity 0.7
        $draw Ellipse 250 100 50 25 0 360
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetStrokeWidth 1.0
        $draw SetStrokeColor pix2
        $draw SetFillColor pix2
        $draw Point 200 200
    $draw PopGraphicContext
    $draw PushGraphicContext
        $draw SetFillColor pix0
        $draw Color 0 0 replace
    $draw PopGraphicContext
    
    $wand DrawImage $draw
    $wand WriteImage "$::TMP/y-Draw-%0d.bmp"
    
    magick delete pix0 pix1 pix2 pix3 pix4 pix5 pix6 pix7 pix8 
    magick delete $draw $wand
}

##########################################
# 
#
proc DLR_logo {draw color x y scale} {

    $draw PushGraphicContext
    
    $draw SetStrokeColor $color
    $draw SetFillColor $color
    $draw SetStrokeWidth 0.0

    $draw Scale $scale
    $draw path start move [expr {$x+72.5}] [expr {$y+0.0}]
    $draw path {
        # "Diagonal part of the DLR bird" 
        -relative on
        line -45 +45 vertical +31 line +45 -45 vertical -31
        line -5 +12
        line -35 +35 vertical +17 line +35 -35 vertical -17
        close
        finish
    }
    $draw path start move [expr {$x+1.0}] [expr {$y+49.0}]
    $draw path {
        # "Horizontal part of the DLR bird"
        -relative on
        horizontal +76 line +22 -22 horizontal -76 line -22 +22
        line +12 -5
        horizontal +62 line +12 -12 horizontal -62 line -12 +12
        close
        finish
    }
    if {[info exists ::FONT]} {
        # Use specified FONT
        #
        $draw SetFontFamily $::FONT
        $draw SetFontSize 20
        $draw Annotation [expr {$x+55.0}] [expr {$y+82.0}] "DLR"
    } else {
        # Draw the letters manually
        #
        $draw path start move [expr {$x+55.0}] [expr {$y+82.0}]
        $draw path {
            # "Letter D"
            -relative on
            vertical -18 horizontal +6
            quadratic +7 +0 +7 +9
            quadratic +0 +9 -7 +9
            horizontal -6
            line +2.5 -2.5
            vertical -13 horizontal +3
            quadratic +5 +0 +5 +6.5
            quadratic +0 +6.5 -5 +6.5
            horizontal -3
            line -2.5 +2.5
            close

            # "Letter L"
            move +17 +0
            vertical -18 horizontal +2.5
            vertical +15 horizontal +7
            vertical +3 horizontal -10
            close

            # "Letter R"
            move +12.5 +0
            vertical -18 horizontal +5
            quadratic +5 +0 +5 +5
            quadratic +0 +5 -4 +5
            line +5 +8 horizontal -2.5 line -5 -8
            horizontal -1 vertical +8 horizontal -2.5
            line +2.5 -10.5
            vertical -5 horizontal +2
            quadratic +3 +0 +3 +2.5
            quadratic +0 +2.5 -3 +2.5
            horizontal -2
            line -2.5 +10.5
            close
	    finish
        }
    }
    $draw PopGraphicContext    
}
proc PathTest {img} {
    set wand [$img clone imgX]
    set draw [magick create draw draw0]
    debug $draw $wand
    
    [magick create pixel pix0] SetColor "gray"
    [magick create pixel pix1] SetColor "black"

    DLR_logo $draw pix0 0 2 4
    DLR_logo $draw pix1 1 1.5 4

    $wand DrawImage $draw
    $wand WriteImage "$::TMP/y-Path-%0d.bmp"
    
    magick delete $draw $wand
}

##########################################
# Main test loop
#

debug magick

if { ! [file isdirectory $TMP]} {
    file mkdir $TMP
}

set img [magick create wand img0]
debug $img

$img ReadImage $IMG
$img ResizeImage 400 340

magick names

set ERRORS 0
foreach {func var flag} $TestFunctions {
    if {$flag} {
        puts [format "%s:" $func $var]
        set err [catch {$func [set $var]} result]
        if {$err} {
            incr ERRORS
            puts stderr [format "error: %s" $result]
        }
    } else {
        puts [format "--- skip: %s ---" $func]
    }
}

puts "##### DRAW TEST READY #####"
if {!$ERRORS} {
    after 3000 exit
}

