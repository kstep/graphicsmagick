//  ----------------------------------------------------------------------------
//  MODULE    : FPXImageView
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Friday, March 29, 1996 
//  DESCRIPTION : 
//  COMMENTS  : FPX Image View can be implemented in a hundred of different
//          ways. Providing all the primitives for every little tiny fileds
//          specified in the format is just unworkable (for the user of the
//          class). We then implemented a version of the FPX Image View which
//          is "Core Reader/Writer" oriented. Basically, this means that only
//          the fields available through the FPX Reference Implementation are
//          implemented as parameters and all the rest is implemented as
//          automatic variables by the PFlashPixImageView class.
//          For instance, there are a bunch of functions to tweak the viewing
//          parameters like position, contrast, filtering and the rest, but
//          there is no function to read or write the operation property set.
//          This one is handled completly automatically.
//      SCCSID          : @(#)fpximgvw.h  1.4 12:46:42 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef FPXImageView_h
  #define FPXImageView_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include "ri_sys.h"
#endif
#ifndef AdvancedIVUE_h
  #include  "viewimg.h"
#endif

#ifndef FileFlashPixView_h
  #include  "f_fpxvw.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

  /*
   * Global Info Property Set
   */
   
  typedef struct {
              FPXbool       lockedPropertiesIsValid;
              FPXLongArray          lockedProperties;
              
              FPXbool       transformedImageTitleIsValid;
              FPXWideStr      transformedImageTitle;
              
              FPXbool       lastModifierIsValid;
              FPXWideStr      lastModifier;

              FPXLongArray          visibleOutputs;
              long                  maxImageIndex;
              long                  maxTransformIndex;
              long                  maxOperatorIndex;
  } FPXGlobalInfo;

  /*
   * Source and Result Description Property Set
   */
   
  typedef enum {
              FPX_DATA_NOT_CACHED = 0,
              FPX_DATA_CACHED
  } FPXImageExistence;

  typedef enum {
              FPX_DATA_PURGEABLE = 0,
              FPX_DATA_NOT_PURGEABLE
  } FPXImagePermission;

  typedef struct {
              FPXImageExistence     existenceData;
              FPXImagePermission    permission;
  } FPXImageStatus;

  typedef struct {
              CLSID                 dataObjectID;

              FPXbool       lockedPropertiesIsValid;
              FPXLongArray          lockedProperties;
              
              FPXbool       dataObjectTitleIsValid;
              FPXWideStr      dataObjectTitle;
              
              FPXbool       lastModifierIsValid;
              FPXWideStr      lastModifier;
              
              FPXbool       revisionNumberIsValid;
              long          revisionNumber;
              
              FPXbool       creationDateIsValid;
              FPXfiletime           creationDate;
              
              FPXbool       modificationDateIsValid;
              FPXfiletime           modificationDate;
              
              FPXbool       creatingApplicationIsValid;
              FPXWideStr            creatingApplication;
              
              FPXImageStatus        status;
              long                  creator;
              FPXLongArray          users;
              
              FPXbool       heightIsValid;
              long                  height;

              FPXbool       widthIsValid;
              long                  width;
  } FPXImageDescription;

  /*
   * Transform Property Set
   */

  typedef struct {
              CLSID                 transformNodeID;
              CLSID                 operationClassID;

              FPXbool       lockedPropertiesIsValid;
              FPXLongArray          lockedProperties;
              
              FPXbool       transformTitleIsValid;
              FPXWideStr      transformTitle;
              
              FPXbool       lastModifierIsValid;
              FPXWideStr      lastModifier;
              
              FPXbool       revisionNumberIsValid;
              long          revisionNumber;
              
              FPXbool       creationDateIsValid;
              FPXfiletime           creationDate;
              
              FPXbool       modificationDateIsValid;
              FPXfiletime           modificationDate;
              
              FPXbool       creatingApplicationIsValid;
              FPXWideStr            creatingApplication;
              
              FPXLongArray          inputDataObjects;
              FPXLongArray          outputDataObjects;
              long                  operationNumber;

              // The actual transform parameters are written or read using independent
              // methods (see SetImageXx() and GetImageXx()) of the PFlashPixImageView class
              // Note that all of these properties are optionnal.
  } FPXTransform;

  /*
   * Operation Property Set
   */
   
  typedef struct {
              CLSID                 operationID;
  } FPXOperation;

//  Classes Declarations
//  --------------------

  class PFlashPixImageView;

  class PSearchHookObject;

//  Classes Definitions
//  -------------------

  class PFlashPixImageView : public ViewImage {

  public:
                 // CHG_VIS_OUT - added visibleOutputIndex argument to constructors that
                 // open a file.
                 // Open the file in Read Mode only 
                 PFlashPixImageView (FicNom& refName, const char* theStorageName = NULL, 
                            long visibleOutputIndex = 0,
                            FPXStatus * status = NULL);
                 PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName,
                            long visibleOutputIndex = 0);
                 // Open the file in Create Mode
                 PFlashPixImageView (FicNom& refName, long width, long height, float resolution,
                          FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                          FPXCompressionOption FPXCompressOption,
                          Boolean createFPXImageView = FALSE,
                          Boolean baseUncalibrated   = FALSE);
                 PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName, long width, long height, float resolution,
                          FPXBaselineColorSpace baseSpace, unsigned long backColor, 
                          FPXCompressionOption FPXCompressOption,
                          Boolean createFPXImageView = FALSE,
                          Boolean baseUncalibrated   = FALSE);
                 // Open the file in Read/Write mode (but not Create)
                 PFlashPixImageView (FicNom& refName, const char* theStorageName, 
                            mode_Ouverture openMode, long visibleOutputIndex = 0,
                            PSearchHookObject* hook = 0, FPXStatus * status = NULL);
                 PFlashPixImageView (OLEStorage* theOwningStorage, const char* theStorageName, 
                            mode_Ouverture openMode, long visibleOutputIndex = 0,
                            FPXStatus * status = NULL);
                 // Create a FPX Image View from an already opened image
                 PFlashPixImageView (PRIImage* theImage);
        virtual     ~PFlashPixImageView ();

        FPXStatus     OpenFile();
        FPXStatus     CloseFile();

        // Set and Get Transform properties       
        FPXStatus     SetImageROI (FPXROI* theROI);
        FPXStatus     GetImageROI (FPXROI* theROI);
        FPXStatus     SetImageFilteringValue (FPXFilteringValue* theFiltering);
        FPXStatus     GetImageFilteringValue (FPXFilteringValue* theFiltering);
        FPXStatus   SetImageAffineMatrix (FPXAffineMatrix*  theAffineMatrix);
        FPXStatus   GetImageAffineMatrix (FPXAffineMatrix*  theAffineMatrix);
        FPXStatus     SetImageResultAspectRatio (FPXResultAspectRatio* theAspectRatio);
        FPXStatus     GetImageResultAspectRatio (FPXResultAspectRatio* theAspectRatio);
        FPXStatus     SetImageColorTwistMatrix (FPXColorTwistMatrix* theColorTwistMatrix);
        FPXStatus     GetImageColorTwistMatrix (FPXColorTwistMatrix* theColorTwistMatrix);
        FPXStatus     SetImageContrastAdjustment (FPXContrastAdjustment* theContrastAdjustment);
        FPXStatus     GetImageContrastAdjustment (FPXContrastAdjustment* theContrastAdjustment);

        void      SetImageEditedFlag();
        PFileFlashPixView*  GetCurrentFile()  { return filePtr; }
        
        FPXStatus     SaveImageFilteringValue ();
        FPXStatus     SaveImageColorTwistMatrix ();
        FPXStatus     SaveImageContrastAdjustment ();
        FPXStatus   SaveImageAffineMatrix ();
        FPXStatus     SaveImageROI ();
        FPXStatus     SaveImageResultAspectRatio ();
        unsigned char*  getInternalBuffer(){return internalBuffer;};
        void        setInternalBuffer(long theWidth, long theHeight);

  protected:
    Boolean   readOnlyFile;
    unsigned char*    internalBuffer;
    long        internalBufSize;
  private:
        // Implement default Baseline behavior for the other property sets
        FPXStatus     SaveSummaryInfoPropertySet();
        FPXStatus   MakeNewThumbnail(OLEProperty *aProp);

        FPXStatus     SaveGlobalInfoPropertySet();
        FPXStatus     SaveSourcePropertySet();
        FPXStatus     SaveResultPropertySet();
        FPXStatus     SaveTransformPropertySet();
        FPXStatus     SaveOperationPropertySet();
        FPXStatus     SaveExtensionListPropertySet();

        void      InitGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo);
        void      InitSourcePropertySet(FPXImageDescription* theImageDesc);
        void      InitResultPropertySet(FPXImageDescription* theImageDesc);
        void      InitTransformPropertySet(FPXTransform* theTransform);
        void      InitOperationPropertySet(FPXOperation* theOperation);
        void      InitExtensionListPropertySet(FPXExtensionDescription* theExtension);

        // Set and Get property sets functions: one day, these functions will be made public and
        // provided to the user through some calls in the Reference Implementation API...
        FPXStatus   SetGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo);
        FPXStatus   SetSourcePropertySet(FPXImageDescription* theSourceDescription);
        FPXStatus   SetResultPropertySet(FPXImageDescription* theResultDescription);
        FPXStatus   SetTransformPropertySet(FPXTransform* theTransform);
        FPXStatus   SetOperationPropertySet(FPXOperation* theOperation);
        FPXStatus   SetExtensionListPropertySet(FPXExtensionDescription* theExtension, char* extensionName);

        FPXStatus   GetGlobalInfoPropertySet(FPXGlobalInfo* theGlobalInfo);
        FPXStatus   GetSourcePropertySet(FPXImageDescription* theSourceDescription);
        FPXStatus   GetResultPropertySet(FPXImageDescription* theResultDescription);
        FPXStatus   GetTransformPropertySet(FPXTransform* theTransform);
        FPXStatus   GetOperationPropertySet(FPXOperation* theOperation);
        FPXStatus   GetExtensionListPropertySet(FPXExtensionDescription* theExtension, char* extensionName);
        
        // Load and Save Transform properties
        void      InitTransformProperties ();     
        
        FPXStatus     LoadImageROI ();
        
        FPXStatus     LoadImageFilteringValue ();
        
        FPXStatus   LoadImageAffineMatrix ();
        
        FPXStatus     LoadImageResultAspectRatio ();
        
        FPXStatus     LoadImageColorTwistMatrix ();
        
        FPXStatus     LoadImageContrastAdjustment ();

        PFileFlashPixView*  filePtr;      // The image file descriptor
        OLEStorage*   sourceStorage;    // Source FPX Image storage
        OLEStorage*   resultStorage;    // Result FPX Image storage

        // Geometric parameters
        Boolean         hasRegionOfInterest;
        Boolean           hasAffineMatrix;
        Boolean         hasAspectRatio;
        FPXROI          regionOfInterest;
        FPXAffineMatrix       affineMatrix;
        FPXResultAspectRatio  aspectRatio;

        // Display parameters
        Boolean         hasFiltering;
        Boolean         hasColorTwistMatrix;
        Boolean         hasContrastAdjustment;
        FPXFilteringValue     filtering;
        FPXColorTwistMatrix   colorTwistMatrix;
        FPXContrastAdjustment contrastAdjustment;

        Boolean     transformsHaveBeenEdited; // True if a SetXx() function is called on a transform  
        Boolean     imageHasBeenEdited;     // True if WriteRectangle() or other pixel editing function has been called
  };

//  'inline' Functions
//  ------------------

inline void  PFlashPixImageView::SetImageEditedFlag() { imageHasBeenEdited = TRUE; }

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // FPXImageView_h
//  ----------------------------------------------------------------------------
