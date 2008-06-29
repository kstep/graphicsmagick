# Hand-written package index file.

package ifneeded TclMagick 0.45 \
    [list load [file join $dir libTclMagick[info sharedlibextension]]]

package ifneeded TkMagick 0.45 "
    package require Tk
    package require TclMagick
    load [file join $dir libTkMagick[info sharedlibextension]]"
