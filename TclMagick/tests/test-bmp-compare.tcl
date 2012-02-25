###########################################################
# MagickWand compare test
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

##########################################
# Global options
#
set imgdir [file join .. images]
set base [file join $imgdir tclmagick-small.bmp]
set basecopy [file join $imgdir tclmagick-small-copy.bmp]
set bigdiff [file join $imgdir tclmagick-big-diff.bmp]
set onepixeldiff [file join $imgdir tclmagick-small-diff.bmp]
set epsilon 3.0e-5

proc runTest {} {

    foreach cmd {comparePass compareFail compareOnePixelDiff} {
        if {[$cmd]} {
            set result PASS
        } else {
            set result FAIL
        }
        puts "$result $cmd"
    }
}

proc compareImages {img1 img2} {
    puts "Comparing $img1 and $img2"

    set cmpCmd {
        set wand1 [magick create wand]
        set wand2 [magick create wand]

        $wand1 ReadImage $img1
        $wand2 ReadImage $img2

        set result [$wand1 CompareImages $wand2 meansquarederror]

        puts "Images compared at $result using meansquarederror"

        magick delete $wand1
        magick delete $wand2
    }
    
    if {[catch {eval $cmpCmd} err]} {
        # get stack trace
        set localinfo $::errorInfo
        
        # free any left over wands
        foreach item [magick names] {
            magick delete $item
        }

        set m "Failed to compare images. error: $err"
        puts $m

        error $m $localinfo
    }
    
    return $result
}

proc comparePass {} {
    puts "Checking for equal images to pass comparison"

    set result [compareImages $::base $::basecopy]

    # result should be 0.0 from an equal comparison
    return [string eq $result 0.0]
}

proc compareFail {} {
    puts "Checking for vastly different images to fail comparison\
            at epsilon $::epsilon"

    set result [compareImages $::base $::bigdiff]

    if {[fequal $result 0]} {
        return 0
    } else {
        return 1
    }
}

proc compareOnePixelDiff {} {

    puts "Checking epsilon $::epsilon difference in image comparison with\
            only one pixel difference"

    set result [compareImages $::base $::onepixeldiff]

    if {[fequal $result 0]} {
        return 1
    } else {
        return 0
    }
}

# from Tclers' Wiki
proc fequal {x y} {
    global epsilon

    if {[expr abs($y) < $epsilon]} {
        return [expr abs($x) < $epsilon]
    }
    return [expr (abs($x - $y) / double($y)) < $epsilon]
}

runTest
#console show