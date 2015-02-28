.. -*- mode: rst -*-
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

Four major development phases have been identified. These are the
feature development phase, snapshot phase, beta phase, and release
phase.  The phases used depend on the nature of preceding development.
The Beta phase is usually skipped. The descriptions of these phases
are as follows:

Feature Development Phase
~~~~~~~~~~~~~~~~~~~~~~~~~

  The feature development phase is a time of rapid development and
  innovation. Work is normally done on Mercurial 'tip'.  During the
  feature development phase, Mercurial is the only way to access the
  work in progress. Before updates are committed to Mercurial, they
  should be proven to compile on at least one architecture and pass
  the test suite ('make distcheck').

  Entry Criteria: There is a plan for feature development.

  Exit Criteria: Test suite passes on at least one machine.

Snapshot Phase
~~~~~~~~~~~~~~

  The snapshot phase is entered when it is decided that the work is
  stable enough for non-developers to test. API and user interfaces
  should be stable before the snapshot phase begins in order to avoid
  confusion. Snapshot development is done on Mercurial 'tip'. Snapshot
  packages are identified by the date of the snapshot, and no Mercurial
  branching or tagging is performed to support the snapshot.

  Entry Criteria: Interfaces are stable, and code compiles and runs on
  multiple architectures.

  Exit Criteria: Code is release quality. The test suite must show
  100% completion for Q:8 and Q:16 quantum depths on at least three
  operating environments.

Beta Phase
~~~~~~~~~~

  The Beta phase (usually only occuring at the beginning of a major
  release cycle) is entered when the work is feature complete, the
  package passes all tests, and it is considered time for versioned
  releases. As the first step of entering the beta phase, a release
  branch is created off of the trunk to support change sets for the
  release. The purpose of the beta phase is to wring out any remaining
  bugs prior to release. When a beta package is prepared, a release
  tag is applied to the associated release branch in Mercurial.

  Entry Criteria: Code is release quality.. The test suite must show
  100% completion for Q:8 and Q:16 quantum depths on at least three
  operating environments.

  Exit Criteria: The test suite must show 100% completion for Q:8, Q:16,
  and Q:32 quantum depths on at least three operating environments. The
  current beta package is determined to be sufficiently flawless for a
  final release.

Release Phase
~~~~~~~~~~~~~

  The release phase is entered when the most recent Beta (or snapshot)
  is considered to be of acceptable quality for a release or bug-fixes
  have been prepared based on a previous release. At this time, a
  formal release tag is applied to the release branch, and release
  packages are created. Once a release tag has been applied to a
  release branch, that release branch is considered to be in release
  state and may only be used to prepare additional releases intended
  to correct bugs found in preceding releases. The initial release on
  a release branch has a two-digit (i.e.  X.X) release
  designation. Bug-fix releases have a three-digit (i.e.  X.X.X)
  release designation. A release designation may only be used once. If
  a problem is discovered with the release package, a new release
  number is assigned to support the corrected release package.

  Entry Criteria: The test suite must show 100% completion for Q:8, Q:16,
  and Q:32 quantum depths on at least three operating environments. The
  current beta package is determined to be sufficiently flawless for a
  final release.

  Exit Criteria: None

--------------------------------------------------------------------------

.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2015
