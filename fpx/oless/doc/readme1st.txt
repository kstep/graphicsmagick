The source code in this directory is an implemtation of OLE structured storage.
It has been written to be cross platform and take into account big-endian and
little-endian machines. This code was originally supplied by Microsoft, but was
not drawn from the actual Windows NT source code base. It was written entirely
from scratch. The downside of this is that it is not written in order to be a
high performance commercial grade implementation. The view was that you would
simply use the OLE support supplied by the operating system for those systems
that support it and that this code would server only as a reference.

The DLL produced by this reference is not used by the Windows build of the
reference implementation. In fact there is no way yet to build the reference
implementation on Windows using this library. This remains an excercise for the
open source implementation going forward.