# Hand-written package index file.

package ifneeded TclMagick 0.42 \
    [list load [file join $dir .. libTclMagick[info sharedlibextension]]]

package ifneeded TkMagick 0.42 \
    [list load [file join $dir .. libTkMagick[info sharedlibextension]]]