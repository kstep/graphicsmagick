# Hand-written package index file.

package ifneeded TclMagick $provides \
    [list load [file join $dir .. libTclMagick[info sharedlibextension]]]

package ifneeded TkMagick $provides \
    [list load [file join $dir .. libTkMagick[info sharedlibextension]]]