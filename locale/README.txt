This directory contains the GraphicsMagick message subsystem which
provides text message strings corresponding to pre-processor symbols
which are used at the point where the message is emitted.  This is
indended to help support internationalizing GraphicsMagick.

The definitions are in an XML format.  An already working copy of
GraphicsMagick (producing compatible output) is necessary in order to
convert from C.mgk to locale_c.h.

For now the only locale supported is C.

To update messages use this procedure:

1. Edit C.mgk

2. 'make'

3. 'make install'

4. Now rebuild GraphicsMagick.
