//  ------------------------------------------------------------------------------------------------
//  MODULE    : FileUtils
//  LANGAGE   : C++
//  CREATEUR  : Laurent ALBERT
//  DATE    : Friday, January 27, 1995
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)fileutil.h  1.1 11:50:00 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef FileUtils_h
  #define FileUtils_h
  #ifndef ApplCommun_h
    #include  "common_a.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  <StandardFile.h>

//  Constantes
//  ----------

//  Declarations des classes
//  ------------------------

/*
**  Macros to get information out of GetVolParmsInfoBuffer
*/

#define isNetworkVolume(volParms) ((volParms).vMServerAdr != 0)
#define hasLimitFCBs(volParms)    (((volParms).vMAttrib & (1L << bLimitFCBs)) != 0)
#define hasLocalWList(volParms)   (((volParms).vMAttrib & (1L << bLocalWList)) != 0)
#define hasNoMiniFndr(volParms)   (((volParms).vMAttrib & (1L << bNoMiniFndr)) != 0)
#define hasNoVNEdit(volParms)   (((volParms).vMAttrib & (1L << bNoVNEdit)) != 0)
#define hasNoLclSync(volParms)    (((volParms).vMAttrib & (1L << bNoLclSync)) != 0)
#define hasTrshOffLine(volParms)  (((volParms).vMAttrib & (1L << bTrshOffLine)) != 0)
#define hasNoSwitchTo(volParms)   (((volParms).vMAttrib & (1L << bNoSwitchTo)) != 0)
#define hasNoDeskItems(volParms)  (((volParms).vMAttrib & (1L << bNoDeskItems)) != 0)
#define hasNoBootBlks(volParms)   (((volParms).vMAttrib & (1L << bNoBootBlks)) != 0)
#define hasAccessCntl(volParms)   (((volParms).vMAttrib & (1L << bAccessCntl)) != 0)
#define hasNoSysDir(volParms)   (((volParms).vMAttrib & (1L << bNoSysDir)) != 0)
#define hasExtFSVol(volParms)   (((volParms).vMAttrib & (1L << bHasExtFSVol)) != 0)
#define hasOpenDeny(volParms)   (((volParms).vMAttrib & (1L << bHasOpenDeny)) != 0)
#define hasCopyFile(volParms)   (((volParms).vMAttrib & (1L << bHasCopyFile)) != 0)
#define hasMoveRename(volParms)   (((volParms).vMAttrib & (1L << bHasMoveRename)) != 0)
#define hasDesktopMgr(volParms)   (((volParms).vMAttrib & (1L << bHasDesktopMgr)) != 0)
#define hasShortName(volParms)    (((volParms).vMAttrib & (1L << bHasShortName)) != 0)
#define hasFolderLock(volParms)   (((volParms).vMAttrib & (1L << bHasFolderLock)) != 0)
#define hasPersonalAccessPrivileges(volParms) \
    (((volParms).vMAttrib & (1L << bHasPersonalAccessPrivileges)) != 0)
#define hasUserGroupList(volParms)  (((volParms).vMAttrib & (1L << bHasUserGroupList)) != 0)
#define hasCatSearch(volParms)    (((volParms).vMAttrib & (1L << bHasCatSearch)) != 0)
#define hasFileIDs(volParms)    (((volParms).vMAttrib & (1L << bHasFileIDs)) != 0)
#define hasBTreeMgr(volParms)   (((volParms).vMAttrib & (1L << bHasBTreeMgr)) != 0)
#define hasBlankAccessPrivileges(volParms) \
    (((volParms).vMAttrib & (1L << bHasBlankAccessPrivileges)) != 0)

/*
**  Deny mode permissions for use with the HOpenAware, HOpenRFAware,
**  FSpOpenAware, and FSpOpenRFAware functions.
*/

enum
{
  dmNone      = 0x0000,
  dmNoneDenyRd  = 0x0010,
  dmNoneDenyWr  = 0x0020,
  dmNoneDenyRdWr  = 0x0030,
  dmRd      = 0x0001, /* Single writer, multiple readers; the readers */
  dmRdDenyRd    = 0x0011,
  dmRdDenyWr    = 0x0021, /* Browsing - equivalent to fsRdPerm */
  dmRdDenyRdWr  = 0x0031,
  dmWr      = 0x0002,
  dmWrDenyRd    = 0x0012,
  dmWrDenyWr    = 0x0022,
  dmWrDenyRdWr  = 0x0032,
  dmRdWr      = 0x0003, /* Shared access - equivalent to fsRdWrShPerm */
  dmRdWrDenyRd  = 0x0013,
  dmRdWrDenyWr  = 0x0023, /* Single writer, multiple readers; the writer */
  dmRdWrDenyRdWr  = 0x0033  /* Exclusive access - equivalent to fsRdWrPerm */
};
  

//  Definitions des classes
//  -----------------------

/*****************************************************************************/

pascal  OSErr FileCopy(short srcVRefNum,
             long srcDirID,
             ConstStr255Param srcName,
             short dstVRefNum,
             long dstDirID,
             StringPtr dstPathname,
             StringPtr copyName,
             Ptr copyBufferPtr,
             long copyBufferSize,
             Boolean preflight);
/*  ¦ Duplicate a file and optionally rename it.
  The FileCopy function duplicates a file and optionally renames it.
  Since the PBHCopyFile routine is only available on some
  AFP server volumes under specific conditions, this routine
  either uses PBHCopyFile, or does all of the work PBHCopyFile
  does.  The srcVRefNum, srcDirID and srcName are used to
  determine the location of the file to copy.  The dstVRefNum
  dstDirID and dstPathname are used to determine the location of
  the destination directory.  If copyName <> NIL, then it points
  to the name of the new file.  If copyBufferPtr <> NIL, it
  points to a buffer of copyBufferSize that is used to copy
  the file's data.  The larger the supplied buffer, the
  faster the copy.  If copyBufferPtr = NIL, then this routine
  allocates a buffer in the application heap. If you pass a
  copy buffer to this routine, make its size a multiple of 512
  ($200) bytes for optimum performance.
  
  srcVRefNum    input:  Source volume specification.
  srcDirID    input:  Source directory ID.
  srcName     input:  Source file name.
  dstVRefNum    input:  Destination volume specification.
  dstDirID    input:  Destination directory ID.
  dstPathname   input:  Pointer to destination directory name, or
              nil when dstDirID specifies a directory.
  copyName    input:  Points to the new file name if the file is
              to be renamed or nil if the file isn't to
              be renamed.
  copyBufferPtr input:  Points to a buffer of copyBufferSize that
              is used the i/o buffer for the copy or
              nil if you want FileCopy to allocate its
              own buffer in the application heap.
  copyBufferSize  input:  The size of the buffer pointed to
              by copyBufferPtr.
  preflight   input:  If true, FileCopy makes sure there are enough
              allocation blocks on the destination volume to
              hold both the data and resource forks before
              starting the copy.

  __________
  
  Also see: FSpFileCopy, DirectoryCopy, FSpDirectoryCopy
*/

/*****************************************************************************/

pascal  OSErr FSpFileCopy(const FSSpec *srcSpec,
              const FSSpec *dstSpec,
              StringPtr copyName,
              Ptr copyBufferPtr,
              long copyBufferSize,
              Boolean preflight);
/*  ¦ Duplicate a file and optionally rename it.
  The FSpFileCopy function duplicates a file and optionally renames it.
  Since the PBHCopyFile routine is only available on some
  AFP server volumes under specific conditions, this routine
  either uses PBHCopyFile, or does all of the work PBHCopyFile
  does.  The srcSpec is used to
  determine the location of the file to copy.  The dstSpec is
  used to determine the location of the
  destination directory.  If copyName <> NIL, then it points
  to the name of the new file.  If copyBufferPtr <> NIL, it
  points to a buffer of copyBufferSize that is used to copy
  the file's data.  The larger the supplied buffer, the
  faster the copy.  If copyBufferPtr = NIL, then this routine
  allocates a buffer in the application heap. If you pass a
  copy buffer to this routine, make its size a multiple of 512
  ($200) bytes for optimum performance.
  
  srcSpec     input:  An FSSpec record specifying the source file.
  dstSpec     input:  An FSSpec record specifying the destination
              directory.
  copyName    input:  Points to the new file name if the file is
              to be renamed or nil if the file isn't to
              be renamed.
  copyBufferPtr input:  Points to a buffer of copyBufferSize that
              is used the i/o buffer for the copy or
              nil if you want FileCopy to allocate its
              own buffer in the application heap.
  copyBufferSize  input:  The size of the buffer pointed to
              by copyBufferPtr.
  preflight   input:  If true, FSpFileCopy makes sure there are
              enough allocation blocks on the destination
              volume to hold both the data and resource forks
              before starting the copy.

  __________
  
  Also see: FileCopy, DirectoryCopy, FSpDirectoryCopy
*/

/*****************************************************************************/

pascal  OSErr FSpRenameCompat(FSSpec *spec,
                ConstStr255Param newName);
/*  ¦ Rename a file or directory.
  The FSpRenameCompat function renames a file or directory.

  spec    input:  An FSSpec record specifying the file.
  newName   input:  The new name of the file or directory.
*/

/*****************************************************************************/

pascal  OSErr HMoveRename(short vRefNum,
              long srcDirID,
              ConstStr255Param srcName,
              long dstDirID,
              StringPtr dstpathName,
              StringPtr copyName);
/*  ¦ Move a file or directory on a file server and optionally to rename it.
  The HMoveRename function moves a file or directory and optionally
  renames it. The source and destination locations must be on the same
  shared volume.
  
  vRefNum   input:  Volume specification.
  srcDirID  input:  Source directory ID.
  srcName   input:  The source object name.
  dstDirID  input:  Destination directory ID.
  dstName   input:  Pointer to destination directory name, or
            nil when dstDirID specifies a directory.
  copyName  input:  Points to the new name if the object is to be
            renamed or nil if the object isn't to be renamed.

  __________
  
  Also see: FSpMoveRename, HMoveRenameCompat, FSpMoveRenameCompat
*/

/*****************************************************************************/

pascal  OSErr GetFilenameFromPathname(ConstStr255Param pathname,
                    Str255 filename);
/*  ¦ Get the object name from the end of a full or partial pathname.
  The GetFilenameFromPathname function gets the file (or directory) name
  from the end of a full or partial pathname. Returns notAFileErr if the
  pathname is nil, the pathname is empty, or the pathname cannot refer to
  a filename (with a noErr result, the pathname could still refer to a
  directory).
  
  pathname  input:  A full or partial pathname.
  filename  output: The file (or directory) name.

  __________
  
  See also: GetObjectLocation.
*/

/*****************************************************************************/

pascal  OSErr GetObjectLocation(short vRefNum,
                  long dirID,
                  StringPtr pathname,
                  short *realVRefNum,
                  long *realParID,
                  Str255 realName,
                  Boolean *isDirectory);
/*  ¦ Get a file system object's location.
  The GetObjectLocation function gets a file system object's location -
  that is, its real volume reference number, real parent directory ID,
  and name. While we're at it, determine if the object is a file or directory.
  If GetObjectLocation returns fnfErr, then the location information
  returned is valid, but it describes an object that doesn't exist.
  You can use the location information for another operation, such as
  creating a file or directory.
  
  vRefNum   input:  Volume specification.
  dirID   input:  Directory ID.
  pathname  input:  Pointer to object name, or nil when dirID specifies
            a directory that's the object.
  realVRefNum output: The real volume reference number.
  realParID output: The parent directory ID of the specified object.
  realName  output: The name of the specified object (the case of the
            object name may not be the same as the object's
            catalog entry on disk - since the Macintosh file
            system is not case sensitive, it shouldn't matter).
  isDirectory output: True if object is a directory; false if object
            is a file.

  __________
  
  See also: FSMakeFSSpecCompat
*/
/*****************************************************************************/

pascal  OSErr HMoveRenameCompat(short vRefNum,
                  long srcDirID,
                  ConstStr255Param srcName,
                  long dstDirID,
                  StringPtr dstpathName,
                  StringPtr copyName);
/*  ¦ Move a file or directory and optionally rename it.
  The HMoveRenameCompat function moves a file or directory and optionally
  renames it.  The source and destination locations must be on the same
  volume. This routine works even if the volume doesn't support MoveRename.
  
  vRefNum   input:  Volume specification.
  srcDirID  input:  Source directory ID.
  srcName   input:  The source object name.
  dstDirID  input:  Destination directory ID.
  dstName   input:  Pointer to destination directory name, or
            nil when dstDirID specifies a directory.
  copyName  input:  Points to the new name if the object is to be
            renamed or nil if the object isn't to be renamed.

  __________
  
  See also: FSpMoveRenameCompat
*/

/*****************************************************************************/

pascal  OSErr FSpMoveRenameCompat(const FSSpec *srcSpec,
                  const FSSpec *dstSpec,
                  StringPtr copyName);
/*  ¦ Move a file or directory and optionally rename it.
  The FSpMoveRenameCompat function moves a file or directory and optionally
  renames it.  The source and destination locations must be on the same
  volume. This routine works even if the volume doesn't support MoveRename.
  
  srcSpec   input:  An FSSpec record specifying the source object.
  dstSpec   input:  An FSSpec record specifying the destination
            directory.
  copyName  input:  Points to the new name if the object is to be
            renamed or nil if the object isn't to be renamed.

  __________
  
  See also: HMoveRenameCompat
*/

/*****************************************************************************/

pascal  OSErr HGetDirAccess(short vRefNum,
                long dirID,
                StringPtr name,
                long *ownerID,
                long *groupID,
                long *accessRights);
/*  ¦ Get a directory's access control information on a shared volume.
  The HGetDirAccess function retrieves the directory access control
  information for a directory on a shared volume.
  
  vRefNum     input:  Volume specification.
  dirID     input:  Directory ID.
  name      input:  Pointer to directory name, or nil if dirID
              specifies the directory.
  ownerID     output: The directory's owner ID.
  groupID     output: The directory's group ID or
              0 if no group affiliation.
  accessRights  output: The directory's access rights.

  __________
  
  Also see: HGetLogInInfo, FSpGetDirAccess, HSetDirAccess,
        FSpSetDirAccess, HMapName, HMapID
*/

/*****************************************************************************/

pascal  OSErr FSpGetDirAccess(const FSSpec *spec,
                long *ownerID,
                long *groupID,
                long *accessRights);
/*  ¦ Get a directory's access control information on a shared volume.
  The FSpGetDirAccess function retrieves the directory access control
  information for a directory on a shared volume.
  
  spec      input:  An FSSpec record specifying the directory.
  ownerID     output: The directory's owner ID.
  groupID     output: The directory's group ID or
              0 if no group affiliation.
  accessRights  output: The directory's access rights.

  __________
  
  Also see: HGetLogInInfo, HGetDirAccess, HSetDirAccess,
        FSpSetDirAccess, HMapName, HMapID
*/

/*****************************************************************************/

pascal  OSErr HSetDirAccess(short vRefNum,
                long dirID,
                StringPtr name,
                long ownerID,
                long groupID,
                long accessRights);
/*  ¦ Set a directory's access control information on a shared volume.
  The HSetDirAccess function changes the directory access control
  information for a directory on a shared volume. You must own a directory
  to change its access control information.
  
  vRefNum     input:  Volume specification.
  dirID     input:  Directory ID.
  name      input:  Pointer to directory name, or nil if dirID
              specifies the directory.
  ownerID     input:  The directory's owner ID.
  groupID     input:  The directory's group ID or
              0 if no group affiliation.
  accessRights  input:  The directory's access rights.

  __________
  
  Also see: HGetLogInInfo, HGetDirAccess, FSpGetDirAccess,
        FSpSetDirAccess, HMapName, HMapID
*/

/*****************************************************************************/

pascal  OSErr FSpSetDirAccess(const FSSpec *spec,
                long ownerID,
                long groupID,
                long accessRights);
/*  ¦ Set a directory's access control information on a shared volume.
  The FSpSetDirAccess function changes the directory access control
  information for a directory on a shared volume. You must own a directory
  to change its access control information.
  
  spec      input:  An FSSpec record specifying the directory.
  ownerID     input:  The directory's owner ID.
  groupID     input:  The directory's group ID or
              0 if no group affiliation.
  accessRights  input:  The directory's access rights.

  __________
  
  Also see: HGetLogInInfo, HGetDirAccess, FSpGetDirAccess, HSetDirAccess,
        HMapName, HMapID
*/

/*****************************************************************************/

//  Fonctions 'inline'
//  ------------------

//  Fonctions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // FileUtils_h
//  ------------------------------------------------------------------------------------------------
