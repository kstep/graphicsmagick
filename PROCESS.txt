.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

==================================
GraphicsMagick Development Process
==================================

In order to ensure an orderly development process, and assure the highest
quality releases, a development process has been established for
GraphicsMagick.

Phases of Development
---------------------

Four major development phases have been identified. These are the feature
development phase, snapshot phase, beta phase, and release phase. The
descriptions of these phases are as follows:

Feature Development Phase
~~~~~~~~~~~~~~~~~~~~~~~~~

  The feature development phase is a time of rapid development. Work is
  normally done on the CVS trunk. Work which may temporarily destabilize
  the ability to build or execute the software is done on a separate
  development branch, which is merged into the CVS trunk once the work
  has been completed. During the feature development phase, CVS is the
  only way to access the work in progress. Before updates are committed
  to CVS, they should be proven to compile on at least one architecture.

  Entry Criteria: Changes from any release branch are merged into CVS trunk.

  Exit Criteria: Interfaces are stable, and code compiles and runs on multiple architectures.

Snapshot Phase
~~~~~~~~~~~~~~

  The snapshot phase is entered when it is decided that the work is
  stable enough for non-developers to test. API and user interfaces
  should be stable before the snapshot phase begins in order to avoid
  confusion. Snapshot development is done on the CVS trunk. Snapshot
  packages are identified by the date of the snapshot, and no CVS
  branching or tagging is performed to support the snapshot.

  Entry Criteria: Interfaces are stable, and code compiles and runs on
  multiple architectures.

  Exit Criteria: Code is considered ready for release. The test suite
  must show 100% completion for Q:8 and Q:16 quantum depths on at least
  three operating environments.

Beta Phase
~~~~~~~~~~

  The Beta phase is entered when the work is feature complete, the
  package passes all tests, and it is considered time for versioned
  releases. As the first step of entering the beta phase, a release
  branch is created off of the trunk to support all development targeting
  the release. The purpose of the beta phase is to wring out any
  remaining bugs prior to release. When a beta package is prepared, a
  release tag is applied to the associated release branch in CVS.

  Entry Criteria: Code is considered ready for release. The test suite
  must show 100% completion for Q:8 and Q:16 quantum depths on at least
  three operating environments.

  Exit Criteria: The test suite must show 100% completion for Q:8, Q:16,
  and Q:32 quantum depths on at least three operating environments. The
  current beta package is determined to be sufficiently flawless for a
  final release.

Release Phase
~~~~~~~~~~~~~

  The release phase is entered when the most recent Beta is considered to
  be of acceptable quality for a release or bug-fixes have been prepared
  based on a previous release. At this time, a formal release tag is
  applied to the release branch, and release packages are created. Once a
  release tag has been applied to a release branch, that release branch
  is considered to be in release state and may only be used to prepare
  additional releases intended to correct bugs found in preceding
  releases. The initial release on a release branch has a two-digit (i.e.
  X.X) release designation. Bug-fix releases have a three-digit (i.e.
  X.X.X) release designation. A release designation may only be used
  once. If a problem is discovered with the release package, a new
  release number is assigned to support the corrected release package.

  Entry Criteria: The test suite must show 100% completion for Q:8, Q:16,
  and Q:32 quantum depths on at least three operating environments. The
  current beta package is determined to be sufficiently flawless for a
  final release.

  Exit Criteria: None

Test Configurations
-------------------

Verification testing is vital to ensure the correct operation of the
package. GraphicsMagick is compiled, and its test suite is executed, in
the following environments:

============  =======  ========  =========================  ====  ====================================
OS            Version  CPU       Tripplet                   Bits  Compiler
============  =======  ========  =========================  ====  ====================================
AIX           4.3.3    PowerPC   powerpc-ibm-aix4.3.3.0     32    C for AIX version 6.0.0
AIX           5.1      PowerPC   powerpc-ibm-aix5.1.0.0     32    IBM XL C/C++ Enterprise Edition V8.0
AIX           5.2      PowerPC   powerpc-ibm-aix5.2.0.0     32    IBM XL C/C++ Enterprise Edition V8.0
AIX           5.3      POWER5+   powerpc-ibm-aix5.3.0.0     32    IBM XL C/C++ Enterprise Edition V8.0
FreeBSD       5.4      i686      i386-unknown-freebsd5.4    32    GCC 3.4.2
HP-UX         10.20    PA-RISC   hppa2.0-hp-hpux10.20       32    HP92453-01 A.10.32.30
HP-UX         11.00    PA-RISC   hppa2.0w-hp-hpux11.00      32    HP92453-01 B.11.X.35098-35101.GP
HP-UX         11.11    PA-RISC   hppa2.0w-hp-hpux11.11      32    HP92453-01 B.11.X.35098-35101.GP
HP-UX         11.23    Itanium2  ia64-hp-hpux11.23          32    HP C/aC++ B3910B A.06.13
HP-UX         11.23    PA-RISC   hppa2.0w-hp-hpux11.23      32    HP92453-01 B.11.X.35098-35101.GP
IRIX          6.5      MIPS      mips-sgi-irix6.5           32    MIPSpro Compilers: Version 7.4.4m
OS X          Leopard  G5        powerpc-apple-darwin9.1.0  32    GCC 4.0.1
OS X          Leopard  G5        powerpc-apple-darwin9.1.0  64    GCC 4.0.1
RHEL          2.1      i686      i686-pc-linux-gnu          32    GCC 2.96
RHEL          3        AMD64     x86_64-unknown-linux-gnu   64    GCC 3.2.3
RHEL          3        i686      i686-pc-linux-gnu          32    GCC 3.2.3
RHEL          4        AMD64     x86_64-unknown-linux-gnu   64    GCC 3.4.6
RHEL          4        i686      i686-pc-linux-gnu          32    GCC 3.4.6
RHEL          5        AMD64     x86_64-unknown-linux-gnu   64    GCC 4.1.1
RHEL          5        i686      i686-pc-linux-gnu          32    GCC 4.1.1
Redhat Linux  7.1      i686      i686-pc-linux-gnu          32    GCC 2.96
Redhat Linux  9        i686      i686-pc-linux-gnu          32    GCC 3.2.2
Solaris       10       AMD64     i386-pc-solaris2.10        32    GCC 4.2.1
Solaris       10       AMD64     i386-pc-solaris2.10        64    GCC 4.2.1
Solaris       10       SPARC     sparc-sun-solaris2.10      32    Sun C 5.8 Patch 121015-04
Solaris       10       i386      i386-pc-solaris2.10        32    Sun C 5.8 Patch 121016-05
Solaris       2.6      SPARC     sparc-sun-solaris2.6       32    Sun C 5.3 Patch 111679-14
Solaris       7        SPARC     sparc-sun-solaris2.7       32    Vendor (Sun)
Solaris       8        SPARC     sparc-sun-solaris2.8       32    Sun C 5.8 Patch 121015-04
Solaris       9        SPARC     sparc-sun-solaris2.9       32    GCC 4.2.1
Solaris       9        SPARC     sparc-sun-solaris2.9       32    Sun C 5.8 Patch 121015-04
Solaris       9        SPARC     sparc-sun-solaris2.9       64    GCC 4.2.1
Tru64 UNIX    4.0D     Alpha     alphaev5-dec-osf4.0d       64    Compaq C V6.5-207 (dtk)
Tru64 UNIX    5.1      Alpha     alphaev67-dec-osf5.1       64    Compaq C V6.5-303 (dtk)
Windows XP    SP2      i686      N/A                        32    Visual C++ 6.0
Windows XP    SP2      i686      N/A                        32    Visual Studio 2003
Windows XP    SP2      i686      i686-pc-cygwin             32    Cygwin 1.5.24-2 / GCC 3.4.4
Windows XP    SP2      i686      i686-pc-mingw32            32    MinGW / GCC 3.2.3
============  =======  ========  =========================  ====  ====================================

The output of any tests which do not support self-validation (e.g. the
Magick++ demos) is visually inspected for correctness.

--------------------------------------------------------------------------

| Copyright (C) 2002 - 2008 GraphicsMagick Group

This program is covered by multiple licenses, which are described in
Copyright.txt. You should have received a copy of Copyright.txt with this
package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.


