File:  READ_ME2.TXT

Porting Notes specific to the FlashPix image format
---------------------------------------------------

To support the FlashPix format's use of Microsoft OLE Structured 
Storage, a Structured Storage library needs to include this listed 
suite of functions or their equivalents.  The FlashPix reference 
implementation on this SDK requires that the functions provide 
the same calling attributes and return values (as well as 
functionality) as those public functions presented by the 
Microsoft supplied OLE DLL's on Windows and Macintosh platforms.  

1.  IStorage Interface object:
    -CreateStream()
    -OpenStream()
    -CreateStorage()
    -OpenStorage()
    -Commit()
    -EnumElements()
    -DestroyElement()
    -RenameElement()
    -SetElementTimes()
    -SetClass()
    -SetStateBits()

2. IStream Interface object:
    -Read()
    -Write()
    -Seek()
    -SetSize()
    -CopyTo()
    -Commit()
    -Stat()
    -Clone()

3. IEnumSTATSTG Interface object:
    -Next()
    -Skip()
   
4. Direct functions:
    -StgCreateDocfileOnILockBytes()
    -StgIsStorageILockBytes()
    -StgOpenStorageOnILockBytes()

5. Underlying ILockBytes interface that supports the:
   IStorage, IStream, IEnumSTATSTG and Stg*ILockBytes() 
   interfaces above.

In order for the Structured Storage Reference Implementation (e.g.
source code described in this document) to read and write compatible
FlashPix files, it may be necessary for the developer to change
the Structured Storage code as indicated below.
1.  Enable wide character (16 bit) handling functions (mentioned 
    in Microsoft's MSREADME.TXT
2.  IStorage->EnumElements(): the types of strings returned by 
    EnumElements in the reference code must match those expected
    by the calling program.
3.  Structure packing for byte alignment (mentioned in Porting Notes).
4.  Memory handling/Heap requirement functions, as needed for the 
    implementation.
5.  Platform specific implementation changes (e.g. big-endian, etc.,
    as noted above).

In order to support the complete FlashPix image specification,
additional functions need to be added to the Microsoft Structured 
Storage reference code.  These functions are included in Microsoft
supplied implementations of OLE SS on the Macintosh and Windows
based platforms.

1.  GUID/UUID generation code (platform specific).  Information 
    that describes the specification for a GUID/UUID is contained in
    "DEC/HP, Network Computing Architecture, Remote Procedure Call
    Run Time Extensions Specification", Version OSF TX1.0.11,
    July 23, 1992, Appendix A "Universal Unique Indentifiers",
    http://www.osf.org/dce.  Note that the generation of a UUID 
    requires the address of the ethernet card in the workstation.
    The code required to read that address is platform and operating
    system specific.  (This reference is also contained in the FlashPix
    specification.)
2.  FILETIME handling functions (platform specific).
3.  Property Set manipulation code (possibly platform independent).
    Property Set manipulation is fairly complex.  The OLE functions
    for NT 4.0 (and for this version of reference Structured Storage
    code) include property set manipulation functions (iPropertySet
    interface) that significantly reduce the work required if the
    application were to be run on an NT system.  If a developer
    is porting FlashPix code to other platforms, adaptation of the
    existing FlashPix property set code, or enhancement of the code
    to support the new IPropertySet interface will have to be done
    to provide support for this functionality on other platforms.
    (FlashPix was initially written before the IPropertySet
    interface was available.

Additional functionality that could be added to the Microsoft reference
code to support performance improvments, or support license requirements
would be as follows.
1.  Support for non-ANSI file handling functions, such as native
    open/read/write file functions.
2.  File locking/permission mechanisms.  The license for use of
    the Reference Structured Storage source code requires that the 
    document file be opened in "exclusive" (non-shared) mode.  
    This is because the reference code has no provisions for 
    correctly updating the compound document file structures when 
    more than one application is accessing the same file at the 
    same time.  Failure to open the file in exclusive mode could 
    lead to corruption of the contents of the file.  

Architecture Overview of the basic Structured Storage code
----------------------------------------------------------

(Property Set code is not described here. Property sets are
data contained in Structured Storage streams.)
    
    .....................................................
    .PLATFORM INDEPENDENT LAYER                         .
    .                                                   .
    .Core part taken from     +---------+               .
    .NT                       | COM API |               .
    .                         |         |               .
    .Caching is in this       +----+----+               .
    .layer                         |                    .
    .                              |                    .
    .                       +------+-------+            .
    .                       | Internal API |            .
    .                       |              |            .
    .                       +------+-------+            .
    .                              |                    .
    .         +-----------------+--+-----------+----------------+
    .         |                 |              |        .       |
    .  +------+-----+     +-----+-----+  +-----+-----+  . +-----+-----+
    .  | MSF Class  |     | FAT Class |  | Dir Class |  . | C Runtime |
    .  | (low level |     |           |  |           |  . |  Library  |
    .  | stream     |     +-----+-----+  +-----+-----+  . +-----+-----+
    .  | handler)   |           |              |        .       |
    .  +-----+------+           |              |        .       |
    .        |                  |              |        .       |
    .        +------------------+--+-----------+----------------+
    .............................. | ....................
                                   |
    .............................. | ....................
    .PLATFORM DEPENDENT LAYER      |                    .
    .                         +----+-------+            .
    .                         | ILockBytes |            .
    .                         |            |            .
    .                         +----+-------             .
    .............................. | ....................
                                   |
                              +----+------+
                              | Operating |
                              |   System  |
                              +-----------+

HIERARCHY OF COMPONENTS
    
    "Exposed..."            
    classes              
    (external 
    IStream, 
    IStorage       "Public..."    "Direct..."
    interface)     classes        classes
    +------+       +------+       +------+
    |      |------>|      |------>|      |---+
    |      |       |      |       |      |   |
    +------+       +------+       +------+   |
                                             |
            +--------------------------------+
            |
            |
    Handle  V           CMStream        ILockBytes
    +-----+----+        +------+        +------+     +------+
    | SID |    |------->|      |------->|      |---->| DISK |
    +-----+----+        |      |        |      |     | FILE |
                        +------+        +------+     +------+
                        responsible 
                        for manipulating 
                        the file format.

EXPLANATIONS

OLE Structured Storage FATs can grow. After using up the 109
locations for FAT pointers in the header, a double indirect
method is used.

The large amount of layering that is in the reference code
was originally present to support the transacted mode.
    
There are multiple "exposed" objects pointing to "public"
objects. The "public" object supports marshalling. The
"public" classes map to the "direct" classes in a 1:1
relationship. The "public" classes (doc file interface)
contain more code.

The "direct" object has a handle.

There are many objects in CMStream that map into structures
within the file.

The classes:
    CFat
    CDifat
    CDirectory
    CFat-->Minifat
all map to structures within the file. Each class manipulates
its associated part of the file.

Inside some of these classes is a vector:
    CFat Vector
    CDir Vector
    
A vector is just a large array of pointers
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    | | |   |   |   |   |   |   |   |   |   |   |   |   |   |...
    +-|-+---+---+---+---+---+---+---+---+---+---+---+---+---+
      V
     CFatSect
     CDirSect

The vector structure is designed to work correctly, even if
the system is out of memory.

CMSFPageTable supports a "virtual memory" handler. It manages
blocks of memory to hand off to vectors. It has a minimum
number of pages of memory (512+) to guarantee being able to
save data in low memory situations. The maximum number of
pages is set to prevent it from being a memory hog.

When a CFat, etc., needs memory, it checks a page in the
vector for a memory pointer. If one is present, it locks the
page (makes it busy), and uses it. The vector itself may grow
based on need. If there are no available pages, go to the
page table handler and get some.

SPECIAL PORTING CONCERNS

String compares:
    Only a couple of places require byte swapping, or special 
    conversion.
    Example:  wsnicmp
              compare: 'a' to 'A' --> 'A' to 'A'
    Correct comparison REQUIRES character conversion to UPPER
    CASE. If not, the presence of '_' in a name is not
    handled correctly in the comparison.

Range locking:
    Locking is in LockByte.  It provides access control on 
    the original file.

File open rights:
    Files are usually opened as read or read/write, deny none.
    Deny none was chosen to support cross process usage. In the
    reference implementation, they should be opened as
    read or read/write, share exclusive. For files that want to
    write, there should be only 1 writer at a time.  

    Multiple writes don't work in direct mode. If they are
    not prohibited, they can corrupt the file. The reason for
    this is that the page table is not always flushed.

FILES MAKING UP THE CLASS DESCRIBED ABOVE

    Class /Object   Files
    -------------   -----
    Exposed         ASCII.CXX
                    ASCII.HXX
                    EXPDF.CXX
                    EXPDF.HXX
                    EXPHEAD.CXX
                    EXPITER.CXX
                    EXPITER.HXX
                    EXPST.CXX
                    EXPST.HXX
                    REXPDF.CXX
                    STORAGE.CXX

    Dir & Fat       DIFAT.CXX
                    DIR.CXX
                    DIRP.CXX
                    FAT.CXX
                    
    ILockBytes      REFILB.CXX
                    

    MSF             HEADER.CXX
                    MREAD.HXX
                    MSF.CXX
                    MSFHEAD.CXX
                    MSFITER.CXX
                    MSTREAM.CXX
                    PAGE.CXX
                    SSTREAM.CXX
                    VECT.CXX

    Public          CDOCFILE.CXX
                    CHINST.CXX
                    DFFUNCS.CXX
                    DFHEAD.CXX
                    DFITER.CXX
                    DFSTREAM.CXX
                    DOCFILE.CXX
                    ENTRY.CXX
                    FUNCS.CXX
                    ITER.CXX
                    ITER.HXX
                    
    
    Various         MEM.CXX
                    TIME.CXX
                    TIME.HXX

    Wide Character  WCHAR.C

Specification Clarifications
----------------------------

Microsoft has issued a clarification about an item contained in
the Structured Storage specification that is included as Appendix
A of the FlashPix specification.

In the section of the specification titled "Directory Sectors",
According to the specification, each level in the directory
hierarchy of a Structured Storage file is represented as a "red-black"
tree.  The "coloration" scheme is designed to provide a balanced
tree.  One of the requirements of such a tree is that the root node
of a Structured Storage file "must always be black".

Investigations into the characteristics of existing Structured
Storage files showed that many of them had red nodes in the root
directory.  In response to this, Microsoft issued this clarification:

  "Since the root directory does not have siblings, it's color is
  irrelevant and may therefore be either red or black."

Known problems
--------------

Some problems have been discovered in the Structured Storage
reference code in this version of the SDK.

1.  The Structured Storage reference code will crash with a memory
    access violation if a portion of a stream that is greater than
    65535 blocks from the previous location accessed.  For the default
    512 byte block size, that equates to an access of over 32MB
    forward of a previous access.  

    The problem was caused by comparison of certain variables of type
    USHORT with variables of type ULONG. 

    One developer who adapted the Structured Storage code for use in
    a product made the following changes, which seemed to correct the
    problem. Not all may be necessary:

    1. VECT.CXX: CPagedVector::Init 
                Change USHORT i, to ULONG i

    2. FAT.CXX: CFat::InitConvert 
                Change:  for (USHORT i = 0; i < csectFat; i++) to
                         for (ULONG  i = 0; i < csectFat; i++)

    3. STREAM.CXX: CDirectStream::ReadAt
                Change USHORT offset; to
                       ULONG  offset;

    4. SSTREAM.CXX: CDirectStream::SetSize(ULONG cbNewSize)
                Change USHORT cbCopy; to 
                      ULONG cbCopy;
              
                Change cbCopy = (USHORT)(cbNewSize > _ulSize ? 
                                  _ulSize : cbNewSize); to
                       cbCopy = (ULONG)(cbNewSize > _ulSize ? 
                                  _ulSize : cbNewSize);

    5. MSTREAM.CXX: GetBuffer
                Change GetBuffer(USHORT cbMin, USHORT cbMax, 
                                 BYTE **ppb, USHORT *pcbActual) to
                       GetBuffer(ULONG cbMin,  ULONG cbMax, 
                                 BYTE **ppb, ULONG *pcbActual)

                Change (in GetBuffer) USHORT cbSize; to 
                                      ULONG cbSize;

    6. MSTREAM.CXX: GetSafeBuffer
                Change GetSafeBuffer(USHORT cbMin, USHORT cbMax, 
                                     BYTE **ppb, USHORT *pcbActual) to 
                       GetSafeBuffer(ULONG cbMin,  ULONG cbMax, 
                                    BYTE **ppb, ULONG *pcbActual)

    7. MSTRESM.CXX: CMStream::MWrite
                Change USHORT offset; to 
                       ULONG offset;

    8. PUBLICDF.CXX: CPubDocFile::CopyLStreamToLStream
                Change USHORT cbBuffer; to
                       ULONG cbBuffer;

    9. DFMSP.HXX: 
                Change GetBuffer and GetSafeBuffer function declarations to
                      accept ULONG instead of USHORT.

    10. MSF.HXX: 
                Change typedef SHORT OFFSET; to 
                       typedef ULONG OFFSET;

                Change typedef USHORT FSOFFSET; to 
                       typedef ULONG FSOFFSET;

2.  On Win32 systems, the Property set tester (PROPTEST) crashes with a 
    memory access violation if the "/w" option is specified without 
    a filename.

3.  On UNIX systems (e.g. solaris), PROPTEST has not been modified to
    portably parse option flags and directory specifications.
    The "/" directory separator character in a UNIX path name is 
    interpreted as an option flag (as usually is done on Windows platforms).

4.  PROPTEST lists in its syntax summary, an "-m" option which is 
    not supported, and ignored when the test is run.

5.  PROPTEST's syntax summary should be updated to indicate 
    more clearly that a directory should be specified when needed.

6.  Running REFTEST with the "r" option results in a "memory leaks
    detected" error message, and a dump when built and run on 32 bit 
    Windows with VC++ V4.2.  No errors result when the UNIX version is
    built and run.

7.  The GNU makefile for the STGDRT test does not create an "obj" directory
    like the other makefiles.  There is also no "byte order" macro in the
    compiler command lines. 

8. The GNU makefile for the REFTEST test does not include the "byte order"
    macro in the compile command lines, nor is CFLAGS specified in
    the link line.  It also does not include a means to make a dependency file.

9.  DEPEND.GCC file for STGDRT is empty.

The "obj" directory for STGDRT, and a filled-in DEPEND.GCC for STGDRT 
have been provided in the GNU makefiles for UNIX that are located in the 
UNIX\SS.TAR file.
