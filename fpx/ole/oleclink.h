//  ----------------------------------------------------------------------------
//  MODULE    : OLECustomLink
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, May 26, 1996 
//  DESCRIPTION : Custom Moniker Management
//  COMMENTS  : It's debatable if we have to have this in OLE (because it's
//          basically an OLE feature) or in FlashPix (because it's a special
//          implementation for FlashPix). We choose to put it there
//          considering that, at the Image Format level, whenever you open
//          a storage, you want to be sure that the moniker is resolved
//          automatically, whatever it takes. That way, we can be sure that
//          all monikers are handled the same way. With another solution,
//          we'll have a quite big duplication of code, or the necessity
//          of creating a new object layer called FPXStorages for example.
//  SCCSID      : @(#)oleclink.h  1.1 11:53:27 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef OLECustomLink_h
  #define OLECustomLink_h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include  "olecore.h"
#endif

  class OLEFile;
  class OLEStorage;
  class OLEPropertySet;
  class OLEPropertySection;

//  Constants
//  ---------

  // Copied from FlashPixUtilities.h to avoid a link from the OLE folder to the FlashPix folder
#ifndef FlashPixUtils_h
  #define ID_FlashPixSuffix(prefix)  {prefix,0xC154,0x11CE,{0x85,0x53,0x00,0xAA,0x00,0xA1,0xF9,0x5B}}
  #define ID_CustomMoniker  ID_FlashPixSuffix(0x56616200)
#endif

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class  OLECustomLink;
  struct FPXCustomLink;

//  Classes Definitions
//  -------------------

  // Custom Link Object
  class OLECustomLink : public OLECore
  {
    public:
      OLECustomLink (OLEStorage*  theInitialStorage);
      virtual ~OLECustomLink ();
           
      IStorage* GetLinkedIStorage ();
           
    private:
      Boolean   GetCustomLinkPropertySet (OLEPropertySet* customLinkSet, FPXCustomLink* customLinkDescriptor);
      Boolean   ResolveLink (OLEStorage* initialStorage, const FPXCustomLink& customLinkDescriptor);
      OLEStorage* ParseStoragePath (OLEStorage* initialStorage, const FPXCustomLink& customLinkDescriptor);
      HRESULT   BindMonikerToStorage(IStream* lpStream, IStorage** pStg);
        
      OLEStorage*     linkedStorage;        // This is the storage linked by the moniker
      OLEFile*      linkedFile;         // This is the file containing the linked storage (if any)
  };

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

/* To be added one day to the public C interface in FPXBaselineIO.h

extern "C" {

 FPXStatus FPX_GetCustomLink (
              FPXImageHandle*             theFPX,
              FPXStr*                     theStorageName,
              FPXCustomLink*              theCustomLink);

 FPXStatus FPX_SetCustomLink (
              FPXImageHandle*             theFPX,
              FPXStr*                     theStorageName,
              FPXCustomLink*              theCustomLink);
              
} // extern "C"
*/

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // OLECustomLink_h
//  ----------------------------------------------------------------------------
