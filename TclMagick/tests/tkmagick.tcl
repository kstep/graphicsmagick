# tkmagick.tcl -- Test the TkMagick functions.

# $Id$

package require Tk

set dirname [file dirname [info script]]

# load [file join $dirname .. generic .libs libTclMagick[info sharedlibextension]]
# load [file join $dirname .. generic .libs libTkMagick[info sharedlibextension]]
package require TkMagick

# magickimage --
#
#	Takes a file name, and returns the name of an image that can
#	be used with Tk.

proc magickimage {filename} {
    set magimg [magick create wand]
    set tkimg [image create photo]
    $magimg ReadImage $filename
    magicktophoto $magimg $tkimg
    magick delete $magimg
    return $tkimg
}

proc saveimage {} {
    global poolphoto
    set magimg [magick create wand]
    phototomagick $poolphoto $magimg
    $magimg WriteImage tmp.jpg
    magick delete $magimg
}

set poolphoto [magickimage [file join $dirname .. images pool.jpg]]
label .im
.im configure -image $poolphoto
pack .im
bind . <q> exit
bind . <s> saveimage