###########################################################
# PixelWand test
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
# Show pixel colors
#
proc ShowPixel {pix {all 1}} {
    puts "----------"
    if {$all} {
        $pix color
        $pix get -quant red green blue opacity
        $pix get red green blue opacity
        $pix get -quant cyan magenta yellow black
        $pix get cyan magenta yellow black
        $pix GetQuantumColor
    } else {
        $pix GetColorAsString
    }
    puts "----------"
}

##########################################
# Single image tests
#
proc TestPixel {} {
    set pix [magick create pixel pix0]
    debug $pix

    $pix SetColor rgb(10%)
    ShowPixel $pix
    
    $pix SetColor rgb(20%,30%)
    ShowPixel $pix
    
    $pix SetColor rgb(30%,40%,50%)
    ShowPixel $pix
    
    $pix SetColor rgb(80%,,20%)
    ShowPixel $pix
    
    $pix SetColor rgb(30,40,50)
    ShowPixel $pix
    
    $pix color #99B2FF
    ShowPixel $pix
    
    $pix set red 0.1 green 0.2 blue 0.3 opacity 0.4
    ShowPixel $pix

    $pix set -quant red 10000 green 20000 blue 30000 opacity 40000
    ShowPixel $pix

    $pix set cyan 0.1 magenta 0.2 yellow 0.3 black 0.4
    ShowPixel $pix

    $pix set -quant cyan 10000 magenta 20000 yellow 30000 black 40000
    ShowPixel $pix

    $pix SetRed 0.9
    $pix SetGreen 0.8
    $pix SetBlue 0.7
    $pix SetOpacity 0.6
    ShowPixel $pix
    
    $pix SetRedQuantum 12345
    $pix SetGreenQuantum 23456
    $pix SetBlueQuantum 34567
    $pix SetOpacityQuantum 45678
    ShowPixel $pix

    $pix SetCyan 0.9
    $pix SetMagenta 0.8
    $pix SetYellow 0.7
    $pix SetBlack 0.6
    ShowPixel $pix
    
    $pix SetCyanQuantum 12345
    $pix SetMagentaQuantum 23456
    $pix SetYellowQuantum 34567
    $pix SetBlackQuantum 45678
    ShowPixel $pix

    $pix SetQuantumColor {100 150 200 250}
    ShowPixel $pix
    
    magick delete $pix
}


##########################################
# Main test loop
#

debug magick

TestPixel
