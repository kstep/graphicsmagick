File: READ_ME1.TXT

Summary:
--------

The Structured Storage directory contains the 1/97 version of the
Reference Structured Storage code as supplied by Microsoft. It
provides a demonstration imlementation of Structured Storage that
could, if necessary, be ported to other platforms, or used in embedded
systems.  

This version of the code provides built-in support for big-endian
(e.g. SUN) and little-endian (e.g. PC) machines.  It is 32 bit only
code.  It does not compile with a 16 bit compiler.

The code consists of two main pieces:
- The Structured Storage "driver" code
- Test programs that exercise the driver.

The files are supplied in C++ source form. There are several
makefiles.  One makefile builds the "driver" code, and the 
other makefiles build the 3 different test programs.

THESE FILES ARE SUPPLIED AS-IS.  NO SUPPORT WILL BE PROVIDED.

Contents of the directory:
--------------------------

The makefiles described here expect this directory structure
relative to the parent directory they are contained in.

    Directory or filename   Usage
    ---------------------   -----
    CHANGES                 Files that are different in the UNIX
                            version. These files are the makefiles
                            set up to work with GNU C.
        PROPS\              Property set code.
        TEST\               Changed files to build test code.
            PROPTEST\       ...Property set tests
            REFTEST\        ...Subset of tests in STGDRT
            STGDRT\         ...Structured Storage exerciser
        MAKEFILE
    H\                      Header files: *.h, *.hxx.
    OBJ\                    Object files and refstg.dll that 
                            result from makefile.
    PROPS\                  Property set code; object code is added
                            to Structured Storage code to make
                            complete "driver".
        OBJ\                Object files 
        DEPEND.MSC          File dependencies for Windows systems
        MAKEFILE            32 bit makefile; includes MAKEFILE.GCC or
                            MAKEFILE.MSC
        MAKEFILE.GCC        GNU makefile for UNIX systems
        MAKEFILE.MSC        Microsoft makefile for Windows systems
                            
    TEST\                   Test program source files
        PROPTEST\           Property set tests
            OBJ\            Object files 
            DEPEND.GCC      File dependencies for UNIX systems
            MAKEFILE        32 bit makefile; includes MAKEFILE.GCC or
                            MAKEFILE.MSC
            MAKEFILE.GCC    GNU makefile for UNIX systems
            MAKEFILE.MSC    Microsoft makefile for Windows systems
        REFTEST\            Small subset of tests in STGDRT
            OBJ\            Object files 
            MAKEFILE        32 bit makefile; includes MAKEFILE.GCC or
                            MAKEFILE.MSC
            MAKEFILE.GCC    GNU makefile for UNIX systems
            MAKEFILE.MSC    Microsoft makefile for Windows systems
        STGDRT\             Reasonably complete exerciser of the
            OBJ\            Object files 
            DEPEND.GCC      File dependencies for UNIX systems
            DEPEND.MSC      File dependencies for Windows systems
            MAKEFILE        32 bit makefile; includes MAKEFILE.GCC or
                            MAKEFILE.MSC
            MAKEFILE.GCC    GNU makefile for UNIX systems
            MAKEFILE.MSC    Microsoft makefile for Windows systems
    UNIX\
        SS.TAR              TAR file containing versions of files
                            converted for UNIX.  Binary transmission
                            using FTP can be used to move these files
                            from PC to UNIX system.  Using 
                              tar -xvf SS.TAR
                            will expand the files into a directory 
                            called "sstorage\code".
    <source files>          Source files for Structured Storage
    DEPEND.GCC              File dependencies for UNIX systems
    DEPEND.MSC              File dependencies for Windows systems
    MAKEFILE                32 bit makefile; includes MAKEFILE.GCC or
                            MAKEFILE.MSC
    MAKEFILE.GCC            GNU makefile for UNIX systems
    MAKEFILE.MSC            Microsoft makefile for Windows systems
    REFPROP.DEF             Definition file for property set code,
                            for Windows systems
    REFSTG.DEF              Definition file for Structured Storage 
                            set code, for Windows systems.

Header and Library files used in Windows version
------------------------------------------------

As an aid to sort out any porting difficulties, these are the C 
compiler header and library files used to build the Windows version
of the Structured Storage code.

    Header files
    ------------
        SYS
            STAT.H
            TYPES.H
        ASSERT.H
        CRTDBG.H
        ERRNO.H
        IO.H
        LIMITS.H
        MALLOC.H
        MEMORY.H
        STDARG.H
        STDIO.H
        STDLIB.H
        STRING.H
        TIME.H

    Library Files
    -------------
        KERNEL32.LIB
        MSVCRTD.LIB

Usage Comments:
---------------

The GNU makefiles have been tested on Sun Solaris v2.5.  They were
built with GNU C++ (gcc) version 2.7.2.

The code in this directory is supplied so that developers who are
writing code for support of Structured Storage on non-MAC and non-PC
platforms may have an example code base to support their efforts.

The code in this directory is not an official Microsoft product.
Microsoft does not supply support for it.

It is strongly recommended that developers who are writing application
programs for the Windows and MAC platforms make use of the standard
OLE libraries already available for those platforms. For applications
intended for other platforms, there are third party OLE libraries
available for a variety of platforms. Use of standard or third party
libraries will minimize the development and testing time for the
application. Microsoft or FlashPix Technical Support can provide names
of companies that provide third-party OLE libraries.

The process of porting the reference structured storage code to 
other platforms will incur expenses such as the long-term cost of 
maintenance on that platform.  This needs to be balanced against 
the potentially faster time to market by use of standard/third party 
OLE libraries.

IMPORTANT NOTE: On the 32 bit Windows platform, the functions on this
directory are contained in a DLL called REFSTG.DLL. On the UNIX
platform, the functions are contained in a library file and are
intended to be directly linked with a user's program.

When making a .DLL to contain the Structured Storage functions,
the developer MUST NOT name the DLL as STORAGE.DLL so as to not
conflict with the Structured Storage functions contained in OLE. If
the developer wishes to use these functions in a COM object on a
Windows system concurrently with the standard Structured Storage
functions contained in OLE, it is recommended that the Interface ID
that is used to access these functions be changed from the standard
Structured Storage ID to the user's own ID, so as not to interfere
with use of OLE's functions.

Test Programs:
-------------

There are three test programs that exercise the structured storage
code.

PROPTEST

  Tests Property Set read/write code.
  Proptest is a set of tests for the property sets Interfaces.
  One interesting feature is that with the /d parameter, you
  can dump any storage file with properties to look at it. 


  Syntax:
    proptest [options]

    Options:
        -g<file>        Specifies a file to be munGed (property sets 
                        are read, deleted, & re-written)
        -d<file>        Specifies a file to be Dumped (property sets 
                        are dumped to stdout)
        -iw             Creates a file for the interoperabity test
        -ir             Verifies the file created for the 
                        interoperability test.
        -w<directory>   Enables the Word 6 test.  <directory> must be 
                        a fully qualified name of an existing directory 
                        (e.g. C:\TESTDIR). Don't use relative path 
                        names (e.g. .\mydir).


   Examples:
        proptest -dword6.doc
        proptest -gword6.doc

REFTEST

  Exercises the Structured Storage code; it is a small subset of 
  STGDRT. It contains a feature for interoperability testing.
  With 'reftest r' it reads in the file and verifies the data written
  with the 'c' option.  This test will be useful, for example, when 
  one uses reftest to create a file on one platform and use another 
  platform's reftest to verify the file.

  Syntax:
    reftest [options]

    Options:
        c               Creates an interoperability test file.
        r               Verify the contents of the test file.

   Examples:
        reftest r

STGDRT

  Structured Storage interface runtime test.
  
  This test calls some of the interfaces of storage and verifies 
  some of the functionalities. It is non-exhaustive.
  
  
  Syntax:
    stgdrt [options]

    Options:
        -h              Display syntax information
        -d              Suppress direct tests
        -i              Enable illegitimate tests
        -v              Display test output
        -#[+|-]<number> Turn test <number> on (+) or off (-)
                        No number present changes state of all tests.
        -n[+|-]<prefix> Turn test <prefix> on or off
                        Prefix can be any prefix of:
                          Create
                          Open
                          AddRef
                          DModify
                          Stat
                          Stream
                          Enum
                          StgCopyTo
                          MoveCopy
                          ILockBytes
                          StgMisc
                          IllStg
                          IllStm
                          IllEnum
        -N<file>        Specify file to use for tests


   Examples:
        stgdrt -i

Makefile comments
-----------------

- MAKEFILE is compatible with the current version of the 32
  bit C++ compiler (VC++ 4.2b).
- The makefiles assume that the standard VC++ symbols have been
  configured:
  - The environment symbol INCLUDE will have to be set to the 
    location of the compiler's include files.
  - The environment symbol LIB will have to be set to the 
    location of the compiler's library files.
  - The operating system PATH string will need to include the path
    to the C compiler.  

Code comments
-------------

- Programming interface
  The Reference implentation implements most of the 
  Structured Storage programming interface as descrived
  in "Inside OLE" and the "OLE Programmers' Reference".
  The interface is primarily through a COM (Component Object
  Module) interface.
  
- Non-ANSI extensions  
  The reference implementation uses a non-ANSI extension
  (zero-sized array in structs). Most compilers support this
  extension. It should be no problem to change these arrays to
  size 1, although this was not tested by Microsoft.

- File Compliance
  The reference implementation compiles with Visual C++ 4.2 and the
  makefile. It generates correct compound files.

  The binary format of files created with this code has been
  validated against the specification.

  DFView.EXE, from the Visual C++ 4.x product, or the Win 32 SDK,
  can be used to examine the files.

Implementation Notes
--------------------

The Structured Storage functions supplied in this directory
provide the normal StgOpenStorage, StgCreateDocfile, and
the ...OnILockBytes functions, such as StgCreateDocfileOnILockbytes.

The implementation here has a complete ILockbytes implementation
that should correctly support read-only files.

For use in special situations, the file-based ILockBytes
(file: refilb.cxx) could be replaced with an ILockBytes that supported
a different type of support.  By replacing ILockbytes with a
device handler, for instance, one could make the native file system
on a hard disk be Structured Storage.

Porting Notes
-------------

The code in this directory has been designed by Microsoft
for easy porting to other environments.  The Microsoft
MSREADME.TXT file has several comments that address this issue.

In general, there should be no "collision" problems between
UNIX functions and with functions or types in the Structured 
Storage code.  The SS code was supplied with both Windows and
UNIX makefiles.  (Uses GNU compiler on UNIX.)

Even though this Structured Storage code provides a COM type of
interface, the code in this module is self-contained, and full
COM support (such as the registry) is not required on the target
platform

- Conversion of source files
  All the files in the reference code use <CR><LF> characters as
  end of line terminators.  Use of the files on UNIX requires 
  conversion of the line terminator to only <LF>.  Use of the files 
  on MAC requires conversion of the terminator to only <CR>.

  UNIX systems are case sensitive for filenames, whereas Windows
  and MAC systems are not. The distribution of the Structured Storage 
  files on CD will have upper case filenames.  The makefiles refer 
  to lower case filenames.

  As a convenience, a TAR file has been provided in the UNIX 
  directory.  This file can be uploaded to a UNIX system and 
  expanded.  The source files in the TAR file have had the line
  terminators converted to <LF>, and the filenames have been
  converted to lower case.

- Structure packing  
  The structures in this code are designed to be packed
  on 2 byte boundaries.  The default settings for Microsoft
  compilers will support this packing, due to the way that 
  the on-disk structures are defined.  The GNU compiler on
  Sun SPARC also handles this correctly.  Compiler flags
  for any other compiler may have to be adjusted accordingly.

- Little-endian vs. Big-endian machines.
  The on-disk format of the data structures within a Structured 
  Storage are designed for operation on an Intel processor based
  system.  An Intel processor uses little-endian notation for
  storing 2 byte or larger values in memory, and thus into the
  disk file.  When using the GNU makefiles in a SPARC UNIX 
  environment, big-endian usage is enabled.  This can be 
  modified by changing the BYTE_ORDER macro in the COMMK.GCC
  file.
  
  (For review, a little-endian system stores data in a
  "least-significant byte first" order.  This means that a 
  developer would see a hexadecimal value of 01234567 stored
  in successive 1 byte memory locations as 67 45 23 01.
  On a big-endian system, that same number would be seen as
  01 23 45 67.)
  
- Shrinking the size
  A dedicated developer could probably drastically shrink the
  size of this implementation for use in embedded systems.  The
  code is basically an implementation of the DOS FAT file system
  and uses a disk file to store the data, rather than directly
  accessing a hard disk device.  If one looks at the size of 
  DOS 5, for example, it is only about 119K bytes of 16 bit code.
  That amount of code contains the command processor, many device
  handlers, program loader, support for all the DOS calls, etc.
  If one were to extract just the file handling essentials from
  that, it would probably amount to 15-20K.
  
  To that end, the following are some actions that could 
  taken to reduce the size:
  - Design a new, simple calling interface.  One could use
    the C runtime libraries "open", "read", "write", etc.
    for examples.
  - Remove all support for the COM interface (e.g. IUnknown,
    etc.)
  - Minimize the functions and options supported.

- Test programs
  As part of a port to a different environment, the developer
  should create portable test programs that will write data 
  using the ported code, and read data using the standard Win32
  OLE libraries, and vice versa, to ensure that the files written
  by the ported code are correct.

