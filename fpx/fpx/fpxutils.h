//  ----------------------------------------------------------------------------
//  MODULE    : FlashPixUtils.h
//  LANGUAGE  : C++
//  AUTHORS   : Nicolas ROMANTZOFF, Philippe BOSSUT
//  DATE    : Friday, July 7th 1995
//  DESCRIPTION : Names, IDs and Types of things as they are defined in the FPX spec:
//                FPX Image Specification, Final Draft, Eastman Kodak company, 
//                December 7 1995
//  COMMENT   : EVERYTHING you could find in the spec should be found here
//          and only here as a define. 
//  SCCSID      : @(#)fpxutils.h  1.2 04:09:42 29 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ----------------------------------------------------------------------------
  #ifndef FlashPixUtils_h
  #define FlashPixUtils_h
//  ----------------------------------------------------------------------------

//  Includes
//  --------
  
#ifndef OLECommun_h
  #include "olecomm.h"    // For the definition of the BLOB structure
#endif
#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"   // For the definition of the data type conversions
#endif

//  Constants
//  ---------

  // Names of the streams and storages. Streams start with '\005'
  #define StreamPrefix        '\005'
  #define FMT_SummaryInfoName     "%cSummaryInformation"
  #define FMT_ImageInfoName     "%cImage Info"
  #define FMT_ImageContentName    "%cImage Contents"
  #define FMT_ResolutionName      "Resolution %04d"
  #define FMT_SubImageHdrName     "Subimage %04d Header"
  #define FMT_SubImageDataName    "Subimage %04d Data"
  #define FMT_SupportDataName       "Support Data %04d"
  #define FMT_ICCProfiles       "ICC Profile %04d"
  #define FMT_CustomLink        "%cCustom Link"
  
  // FPX Image View
  #define FMT_GlobalInfo        "%cGlobal Info"
  #define FMT_ImageDescription    "%cData Object %06d"
  #define FMT_ImageStore        "Data Object Store %06d"
  #define FMT_Transform       "%cTransform %06d"
  #define FMT_Operation       "%cOperation %06d"
  #define FMT_Extension       "%cExtension List"

  // CLSID of the streams and storages
  // ---------------------------------
  
  #define ID_FlashPixSuffix(prefix)  {prefix,0xC154,0x11CE,{0x85,0x53,0x00,0xAA,0x00,0xA1,0xF9,0x5B}}
  #define ID_SummaryInfo  {0xF29F85E0,0x4FF9,0x1068,{0xAB,0x91,0x08,0x00,0x2B,0x27,0xB3,0xD9}}  

  // FPX Image Object
  #define ID_Baseline     ID_FlashPixSuffix(0x56616000)
  #define ID_ImageInfo    ID_FlashPixSuffix(0x56616500)
  #define ID_ImageContent ID_FlashPixSuffix(0x56616400)
  #define ID_Resolution   ID_FlashPixSuffix(0x56616100)
  #define ID_SubImageHdr  ID_FlashPixSuffix(0x00010000)
  #define ID_SubImageData ID_FlashPixSuffix(0x00010100)
  #define ID_SupportData  ID_FlashPixSuffix(0x00010200)
  #define ID_ICCProfiles  ID_FlashPixSuffix(0x56616600)
  
  // FPX Image View
  #define ID_ImageView      ID_FlashPixSuffix(0x56616700)
  #define ID_TransfImage      ID_FlashPixSuffix(0x56616800)
  #define ID_CachedView     ID_FlashPixSuffix(0x00000200)
  #define ID_CachedTransf     ID_FlashPixSuffix(0x00000100)
  #define ID_GlobalInfo     ID_FlashPixSuffix(0x56616F00)
  #define ID_ImageDescription   ID_FlashPixSuffix(0x56616080)
  #define ID_Transform      ID_FlashPixSuffix(0x56616A00)
  #define ID_Operation      ID_FlashPixSuffix(0x56616E00)
  #define ID_Extension      ID_FlashPixSuffix(0x56616010)
  
  // Custom Monikers
  #define ID_CustomMoniker  ID_FlashPixSuffix(0x56616200)
  #define ID_LocalFileMoniker ID_FlashPixSuffix(0x00030000)
  #define ID_URLMoniker   ID_FlashPixSuffix(0x00030100)

  // Other IDs
  #define ID_URLAction    ID_FlashPixSuffix(0x00020000)

  // ID Codes and Types of all FPX Properties:
  // -----------------------------------------
  // Comment: on the disk, it's always in the right side (little endian)...
  
  // Image Contents Property Set: Primary Description Group
  #define PID_NbResolutions       (DWORD)(0x01000000)
  #define PID_HiResWidth          (DWORD)(0x01000002)
  #define PID_HiResHeight         (DWORD)(0x01000003)
  #define PID_DefaultDisplHeight      (DWORD)(0x01000004)
  #define PID_DefaultDisplWidth     (DWORD)(0x01000005)
  #define PID_DefaultDisplUnit      (DWORD)(0x01000006)
  
  #define TYP_NbResolutions       VT_UI4
  #define TYP_HiResWidth          VT_UI4
  #define TYP_HiResHeight         VT_UI4
  #define TYP_DefaultDisplHeight      VT_R4
  #define TYP_DefaultDisplWidth     VT_R4
  #define TYP_DefaultDisplUnit      VT_UI4
  
  // Image Contents Property Set: Resolution Description Group
  #define PID_ResolutionRoot(id,resNum)     (DWORD)(0x02000000 | id | (resNum<<16))
  
  #define PID_SubImageWidth(resNum)       PID_ResolutionRoot(0x00,resNum)
  #define PID_SubImageHeight(resNum)        PID_ResolutionRoot(0x01,resNum)
  #define PID_SubImageColor(resNum)       PID_ResolutionRoot(0x02,resNum)
  #define PID_SubImageNumFormat(resNum)     PID_ResolutionRoot(0x03,resNum)
  #define PID_DecimationMethod(resNum)      PID_ResolutionRoot(0x04,resNum)
  #define PID_DecimationPrefilterWidth(resNum)  PID_ResolutionRoot(0x05,resNum)
  #define PID_SubImageAttributes(resNum)      PID_ResolutionRoot(0x06,resNum)
  #define PID_SubImageICCProfile(resNum)      PID_ResolutionRoot(0x07,resNum)
  #define PID_SupportDataContents(resNum)     PID_ResolutionRoot(0x08,resNum)
  
  #define TYP_SubImageWidth           VT_UI4
  #define TYP_SubImageHeight            VT_UI4
  #define TYP_SubImageColor           VT_BLOB
  #define TYP_SubImageNumFormat         VT_UI4|VT_VECTOR
  #define TYP_DecimationMethod          VT_I4
  #define TYP_DecimationPrefilterWidth      VT_R4
  #define TYP_SubImageAttributes          VT_TBD|VT_VECTOR
  #define TYP_SubImageICCProfile          VT_UI2|VT_VECTOR
  #define TYP_SupportDataContents         VT_TBD|VT_VECTOR

  #define DecimationStandard            0x7FFFFFFF
  
  // Image Contents Property Set: Compression Information Group
  #define PID_CompressionRoot(id,compressIndex)     (0x03000000 | id | (compressIndex<<16))
  
  #define PID_JPEGTables(compressIndex)   PID_CompressionRoot(0x01,compressIndex)
  #define PID_MaxJPEGTables         PID_CompressionRoot(0x02, 0) 
  
  #define TYP_JPEGTables            VT_BLOB
  #define TYP_MaxJPEGTables         VT_UI4
  
  // Image Contents Property Set: Path Specification Group
  #define PID_NumberOfPaths   0x04000000
  #define PID_PathNames     0x04000001
  #define PID_PathDefinition    0x04000002
  
  #define TYP_NumberOfPaths   VT_UI4
  #define TYP_PathNames     VT_LPWSTR|VT_VECTOR
  #define TYP_PathDefinition    VT_BLOB|VT_VECTOR
  
  // Custom Monikers Property Set
  #define PID_OLEMoniker      0x00010000
  #define PID_AbsoluteFilePath  0x00020001
  #define PID_RelativeFilePath  0x00020002
  #define PID_AbsoluteStoragePath 0x00020003
  #define PID_PhysicalVolumeName  0x00020004
  #define PID_VirtualVolumeName 0x00020005
  #define PID_LastMountPoint    0x00020006
//  #define PID_Connection      0x00020103
//  #define PID_HostAddress     0x00020104
  
  #define TYP_OLEMoniker      VT_BLOB
  #define TYP_AbsoluteFilePath  VT_LPWSTR
  #define TYP_RelativeFilePath  VT_LPWSTR
  #define TYP_AbsoluteStoragePath VT_LPWSTR
  #define TYP_PhysicalVolumeName  VT_LPWSTR
  #define TYP_VirtualVolumeName VT_LPWSTR
  #define TYP_LastMountPoint    VT_LPWSTR
//  #define TYP_Connection      VT_LPWSTR
//  #define TYP_HostAddress     VT_LPWSTR
  

  // Image Info Property Set (a.k.a. Non-Image data): File Source Group
  #define PID_FileSource      0x21000000
  #define PID_SceneType     0x21000001
  #define PID_CreationPath    0x21000002
  #define PID_SoftwareName    0x21000003
  #define PID_UserDefinedID   0x21000004
  #define PID_Sharpness     0x21000005

  #define TYP_FileSource      VT_UI4
  #define TYP_SceneType     VT_UI4
  #define TYP_CreationPath    VT_UI4|VT_VECTOR
  #define TYP_SoftwareName    VT_LPWSTR
  #define TYP_UserDefinedID   VT_LPWSTR
  #define TYP_Sharpness     VT_R4

  // Image Info Property Set (a.k.a. Non-Image data): Intellectual Property Group
  #define PID_Copyright     0x22000000
  #define PID_OriginalBroker    0x22000001
  #define PID_DigitalBroker   0x22000002
  #define PID_Authorship      0x22000003
  #define PID_PropertyNotes   0x22000004

  #define TYP_Copyright     VT_LPWSTR
  #define TYP_OriginalBroker    VT_LPWSTR
  #define TYP_DigitalBroker   VT_LPWSTR
  #define TYP_Authorship      VT_LPWSTR
  #define TYP_PropertyNotes   VT_LPWSTR

  // Image Info Property Set (a.k.a. Non-Image data): Content Description Group
  #define PID_TestTargetType    0x23000000
  #define PID_GroupCaption    0x23000002
  #define PID_CaptionText     0x23000003
  #define PID_PeopleInTheImage  0x23000004
  #define PID_ThingsInTheImage  0x23000007
  #define PID_DateOfTheOriginal 0x2300000A
  #define PID_EventsInTheImage  0x2300000B
  #define PID_PlacesInTheImage  0x2300000C
  #define PID_DescriptionNotes  0x2300000F

  #define TYP_TestTargetType    VT_UI4
  #define TYP_GroupCaption    VT_LPWSTR
  #define TYP_CaptionText     VT_LPWSTR
  #define TYP_PeopleInTheImage  VT_LPWSTR|VT_VECTOR
  #define TYP_ThingsInTheImage  VT_LPWSTR|VT_VECTOR
  #define TYP_DateOfTheOriginal VT_FILETIME
  #define TYP_EventsInTheImage  VT_LPWSTR|VT_VECTOR
  #define TYP_PlacesInTheImage  VT_LPWSTR|VT_VECTOR
  #define TYP_DescriptionNotes  VT_LPWSTR

  // Image Info Property Set (a.k.a. Non-Image data): Camera Information Group
  #define PID_CameraManufacturer  0x24000000
  #define PID_CameraModel     0x24000001
  #define PID_CameraSerialNumber  0x24000002

  #define TYP_CameraManufacturer  VT_LPWSTR
  #define TYP_CameraModel     VT_LPWSTR
  #define TYP_CameraSerialNumber  VT_LPWSTR

  // Image Info Property Set (a.k.a. Non-Image data): Per Picture Camera Settings Group
  #define PID_CaptureDate     0x25000000
  #define PID_ExposureTime    0x25000001
  #define PID_F_Number      0x25000002
  #define PID_ExposureProgram   0x25000003
  #define PID_BrightnessValue   0x25000004
  #define PID_ExposureBiasValue 0x25000005
  #define PID_SubjectDistance   0x25000006
  #define PID_MeteringMode    0x25000007
  #define PID_SceneIlluminant   0x25000008
  #define PID_FocalLength     0x25000009
  #define PID_ApertureValue   0x2500000A
  #define PID_Flash       0x2500000B
  #define PID_FlashEnergy     0x2500000C
  #define PID_FlashReturn     0x2500000D
  #define PID_BackLight     0x2500000E
  #define PID_SubjectLocation   0x2500000F
  #define PID_ExposureIndex   0x25000010
  #define PID_OpticalFilter   0x25000011
  #define PID_PerPictureNotes   0x25000012

  #define TYP_CaptureDate     VT_FILETIME
  #define TYP_ExposureTime    VT_R4
  #define TYP_F_Number      VT_R4
  #define TYP_ExposureProgram   VT_UI4
  #define TYP_BrightnessValue   VT_R4|VT_VECTOR
  #define TYP_ExposureBiasValue VT_R4
  #define TYP_SubjectDistance   VT_R4|VT_VECTOR
  #define TYP_MeteringMode    VT_UI4
  #define TYP_SceneIlluminant   VT_UI4
  #define TYP_FocalLength     VT_R4
  #define TYP_ApertureValue   VT_R4
  #define TYP_Flash       VT_UI4
  #define TYP_FlashEnergy     VT_R4
  #define TYP_FlashReturn     VT_UI4
  #define TYP_BackLight     VT_UI4
  #define TYP_SubjectLocation   VT_R4|VT_VECTOR
  #define TYP_ExposureIndex   VT_R4
  #define TYP_OpticalFilter   VT_UI4|VT_VECTOR
  #define TYP_PerPictureNotes   VT_LPWSTR

  // Image Info Property Set (a.k.a. Non-Image data): Digital Camera Characterization Group
  #define PID_SensingMethod       0x26000000
  #define PID_FocalXResolution      0x26000001
  #define PID_FocalYResolution      0x26000002
  #define PID_FocalResolutionUnit     0x26000003
  #define PID_SpatialFrequencyResponse  0x26000004
  #define PID_CFAPattern          0x26000005
  #define PID_SpectralSensitivity     0x26000006
  #define PID_ISOSpeedRatings       0x26000007
  #define PID_OECF            0x26000008

  #define TYP_SensingMethod       VT_UI4
  #define TYP_FocalXResolution      VT_R4
  #define TYP_FocalYResolution      VT_R4
  #define TYP_FocalResolutionUnit     VT_UI4
  #define TYP_SpatialFrequencyResponse  VT_VARIANT|VT_VECTOR
  #define TYP_CFAPattern          VT_VARIANT|VT_VECTOR
  #define TYP_SpectralSensitivity     VT_LPWSTR
  #define TYP_ISOSpeedRatings       VT_UI2|VT_VECTOR
  #define TYP_OECF            VT_VARIANT|VT_VECTOR

  // Image Info Property Set (a.k.a. Non-Image data): Film Description Group
  #define PID_FilmBrand     0x27000000
  #define PID_FilmCategory    0x27000001
  #define PID_FilmSize      0x27000002
  #define PID_FilmRollNumber    0x27000003
  #define PID_FilmFrameNumber   0x27000004

  #define TYP_FilmBrand     VT_LPWSTR
  #define TYP_FilmCategory    VT_UI4
  #define TYP_FilmSize      VT_VARIANT|VT_VECTOR
  #define TYP_FilmRollNumber    VT_UI4
  #define TYP_FilmFrameNumber   VT_UI4

  // Image Info Property Set (a.k.a. Non-Image data): Original Document Scan Description Group
  #define PID_ScannedImageSize  0x29000000
  #define PID_DocumentSize    0x29000001
  #define PID_OriginalMedium    0x29000002
  #define PID_TypeOfOriginal    0x29000003

  #define TYP_ScannedImageSize  VT_VARIANT|VT_VECTOR
  #define TYP_DocumentSize    VT_VARIANT|VT_VECTOR
  #define TYP_OriginalMedium    VT_UI4
  #define TYP_TypeOfOriginal    VT_UI4

  // Image Info Property Set (a.k.a. Non-Image data): Scan Device Property Group
  #define PID_ScannerManufacturer 0x28000000
  #define PID_ScannerModel    0x28000001
  #define PID_ScannerSerialNumber 0x28000002
  #define PID_ScanSoftware    0x28000003
  #define PID_ScanSoftRevDate   0x28000004
  #define PID_ServiceBureau   0x28000005
  #define PID_ScanOperatorID    0x28000006
  #define PID_ScanDate      0x28000008
  #define PID_LastModifiedDate  0x28000009
  #define PID_ScannerPixelSize  0x2800000A

  #define TYP_ScannerManufacturer VT_LPWSTR
  #define TYP_ScannerModel    VT_LPWSTR
  #define TYP_ScannerSerialNumber VT_LPWSTR
  #define TYP_ScanSoftware    VT_LPWSTR
  #define TYP_ScanSoftRevDate   VT_DATE
  #define TYP_ServiceBureau   VT_LPWSTR
  #define TYP_ScanOperatorID    VT_LPWSTR
  #define TYP_ScanDate      VT_FILETIME
  #define TYP_LastModifiedDate  VT_FILETIME
  #define TYP_ScannerPixelSize  VT_R4
  
  // FPX Image View:
  // ---------------
  
  // Source and result description property set
  
  #define PID_ImageID       0x00010000
//  #define PID_Reserved      0x00010001
  #define PID_LockedPropertyList  0x00010002
  #define PID_Title         0x00010003
  #define PID_LastModifier    0x00010004
  #define PID_RevisionNumber    0x00010005
  #define PID_CreationTime    0x00010006
  #define PID_ModificationTime  0x00010007
  #define PID_CreatingApplication 0x00010008
  #define PID_Status        0x00010100
  #define PID_Creator       0x00010101
  #define PID_Users       0x00010102
  #define PID_CachedImageHeight 0x10000000
  #define PID_CachedImageWidth  0x10000001

  #define TYP_ImageID       VT_CLSID
//  #define TYP_Reserved      VT_XX
  #define TYP_LockedPropertyList  VT_UI4|VT_VECTOR
  #define TYP_Title         VT_LPWSTR
  #define TYP_LastModifier    VT_LPWSTR
  #define TYP_RevisionNumber    VT_UI4
  #define TYP_CreationTime    VT_FILETIME
  #define TYP_ModificationTime  VT_FILETIME
  #define TYP_CreatingApplication VT_LPWSTR
  #define TYP_Status        VT_UI4
  #define TYP_Creator       VT_UI4
  #define TYP_Users       VT_UI4|VT_VECTOR
  #define TYP_CachedImageHeight VT_UI4
  #define TYP_CachedImageWidth  VT_UI4

  // Transform Property Set

  #define PID_TransformNodeID   0x00010000
  #define PID_OperationClassID  0x00010001
//  #define PID_LockedPropertyList  0x00010002
//  #define PID_Title           0x00010003
//  #define PID_LastModifier    0x00010004
//  #define PID_RevisionNumber    0x00010005
//  #define PID_CreationTime    0x00010006
//  #define PID_ModificationTime  0x00010007
//  #define PID_CreatingApplication 0x00010008
  #define PID_InputObjectList   0x00010100
  #define PID_OutputObjectList  0x00010101
  #define PID_OperationNumber   0x00010102
  #define PID_ResultAspectRatio 0x10000000
  #define PID_RectOfInterest    0x10000001
  #define PID_Filtering     0x10000002
  #define PID_SpatialOrientation  0x10000003
  #define PID_ColortwistMatrix  0x10000004
  #define PID_ContrastAdjust    0x10000005

  #define TYP_TransformNodeID   VT_CLSID
  #define TYP_OperationClassID  VT_CLSID
//  #define TYP_LockedPropertyList  VT_UI4|VT_VECTOR
//  #define TYP_Title           VT_LPWSTR
//  #define TYP_LastModifier    VT_LPWTSR
//  #define TYP_RevisionNumber    VT_UI4
//  #define TYP_CreationTime    VT_FILETIME
//  #define TYP_ModificationTime  VT_FILETIME
//  #define TYP_CreatingApplication VT_LPWSTR
  #define TYP_InputObjectList   VT_UI4|VT_VECTOR
  #define TYP_OutputObjectList  VT_UI4|VT_VECTOR
  #define TYP_OperationNumber   VT_UI4
  #define TYP_ResultAspectRatio VT_R4
  #define TYP_RectOfInterest    VT_R4|VT_VECTOR
  #define TYP_Filtering     VT_R4
  #define TYP_SpatialOrientation  VT_R4|VT_VECTOR
  #define TYP_ColortwistMatrix  VT_R4|VT_VECTOR
  #define TYP_ContrastAdjust    VT_R4

  // Operation Property Set

  #define PID_OperationID     0x00010000
  #define TYP_OperationID     VT_CLSID

  // Summary Info Property Set
  #define FPX_CODEPAGE    0x04B0
#ifndef PID_DICTIONARY
  #define PID_DICTIONARY    0x00000000
#endif
#ifndef PID_CODEPAGE
  #define PID_CODEPAGE    0x00000001 /* VT_I2 */ /* required */
#endif
  #define PID_TITLE     0x00000002 /* VT_LPSTR */
  #define PID_SUBJECT     0x00000003 /* VT_LPSTR */
  #define PID_AUTHOR      0x00000004 /* VT_LPSTR */
  #define PID_KEYWORDS    0x00000005 /* VT_LPSTR */
  #define PID_COMMENTS    0x00000006 /* VT_LPSTR */
  #define PID_TEMPLATE    0x00000007 /* VT_LPSTR */
  #define PID_LASTAUTHOR    0x00000008 /* VT_LPSTR */
  #define PID_REV_NUMBER    0x00000009 /* VT_LPSTR */
  #define PID_EDITTIME    0x0000000a /* VT_FILETIME */
  #define PID_LASTPRINTED   0x0000000b /* VT_FILETIME */
  #define PID_CREATE_DTM    0x0000000c /* VT_FILETIME */
  #define PID_LASTSAVE_DTM  0x0000000d /* VT_FILETIME */
  #define PID_PAGECOUNT   0x0000000e /* VT_I4 */
  #define PID_WORDCOUNT   0x0000000f /* VT_I4 */
  #define PID_CHARCOUNT   0x00000010 /* VT_I4 */
  #define PID_THUMBNAIL   0x00000011 /* VT_CF */
  #define PID_APPNAME     0x00000012 /* VT_LPSTR */
#ifndef PID_SECURITY
  #define PID_SECURITY    0x00000013 /* VT_I4 */
#endif

  #define TYP_CODEPAGE    VT_I2 
  #define TYP_TITLE     VT_LPSTR 
  #define TYP_SUBJECT     VT_LPSTR 
  #define TYP_AUTHOR      VT_LPSTR 
  #define TYP_KEYWORDS    VT_LPSTR 
  #define TYP_COMMENTS    VT_LPSTR 
  #define TYP_TEMPLATE    VT_LPSTR 
  #define TYP_LASTAUTHOR    VT_LPSTR 
  #define TYP_REV_NUMBER    VT_LPSTR 
  #define TYP_EDITTIME    VT_FILETIME 
  #define TYP_LASTPRINTED   VT_FILETIME
  #define TYP_CREATE_DTM    VT_FILETIME 
  #define TYP_LASTSAVE_DTM  VT_FILETIME
  #define TYP_PAGECOUNT   VT_I4 
  #define TYP_WORDCOUNT   VT_I4 
  #define TYP_CHARCOUNT   VT_I4 
  #define TYP_THUMBNAIL   VT_CF 
  #define TYP_APPNAME     VT_LPSTR 
  #define TYP_SECURITY    VT_I4 

  #define MaxThumbNailSize  96      // Maximum wdith or height of thumbnail image
  #define DefaultPaletteSize  256     // Default size of Palette
  
// Define window DIB structures  
#if defined(macintosh) || defined(__unix) 

  // Bitmap information header
  typedef struct tagBITMAPINFOHEADER
  {
    long      biSize; 
    long      biWidth; 
    long      biHeight; 
    unsigned short  biPlanes; 
    unsigned short  biBitCount; 
    unsigned long biCompression; 
    unsigned long biSizeImage; 
    unsigned long biXPelsPerMeter; 
    unsigned long biYPelsPerMeter; 
    unsigned long biClrUsed; 
    unsigned long biClrImportant; 
  } BITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER; 
  
  // constants for the biCompression field 
  #define BI_RGB        0L
  #define BI_RLE8       1L
  #define BI_RLE4       2L
  #define BI_BITFIELDS  3L

  // RGBQUAD structure for palette
  typedef struct tagRGBQUAD
  {
    unsigned char rgbRed; 
    unsigned char rgbGreen; 
    unsigned char rgbBlue;  
    unsigned char rgbReserved;  
  } RGBQUAD; 
  
#endif
  
  // Global Info Property Set
  
//  #define PID_LockedPropertyList  0x00010002
//  #define PID_Title           0x00010003
//  #define PID_LastModifier    0x00010004
  #define PID_VisibleOutputs    0x00010100
  #define PID_MaxImageIndex   0x00010101
  #define PID_MaxTransfoIndex   0x00010102
  #define PID_MaxOperatorIndex  0x00010103

//  #define TYP_LockedPropertyList  VT_UI4|VT_VECTOR
//  #define TYP_Title       VT_LPWSTR
//  #define TYP_LastModifier    VT_LPWTSR
  #define TYP_VisibleOutputs    VT_UI4|VT_VECTOR
  #define TYP_MaxImageIndex   VT_UI4
  #define TYP_MaxTransfoIndex   VT_UI4
  #define TYP_MaxOperatorIndex  VT_UI4

  // Extension List Property Set
  #define PID_UsedExtensionNumber           0x10000000
  #define PID_ExtensionName(extNumber)        ( extNumber << 16 | 0x00000001 )
  #define PID_ExtensionClassID(extNumber)       ( extNumber << 16 | 0x00000002 )
  #define PID_ExtensionPersistence(extNumber)     ( extNumber << 16 | 0x00000003 )
  #define PID_ExtensionCreationDate(extNumber)    ( extNumber << 16 | 0x00000004 )
  #define PID_ExtensionModificationDate(extNumber)  ( extNumber << 16 | 0x00000005 )
  #define PID_CreateApplication(extNumber)      ( extNumber << 16 | 0x00000006 )
  #define PID_ExtensionDescription(extNumber)     ( extNumber << 16 | 0x00000007 )
  #define PID_StorageStreamPathName(extNumber)    ( extNumber << 16 | 0x00001000 )
  #define PID_FPXStreamPathName(extNumber)      ( extNumber << 16 | 0x00002000 )
  #define PID_FPXStreamFieldOffset(extNumber)     ( extNumber << 16 | 0x00002001 )
  #define PID_PropertySetPathName(extNumber)      ( extNumber << 16 | 0x00003000 )
  #define PID_PropertySetFormatID(extNumber)      ( extNumber << 16 | 0x00003001 )
  #define PID_PropertySetIDCodes(extNumber)     ( extNumber << 16 | 0x00003002 )
  
  #define TYP_UsedExtensionNumber           VT_UI2|VT_VECTOR
  #define TYP_ExtensionName             VT_LPWSTR
  #define TYP_ExtensionClassID            VT_CLSID
  #define TYP_ExtensionPersistence          VT_UI2
  #define TYP_ExtensionCreationDate         VT_FILETIME
  #define TYP_ExtensionModificationDate       VT_FILETIME
  #define TYP_CreateApplication           VT_LPWSTR
  #define TYP_ExtensionDescription          VT_LPWSTR
  #define TYP_StorageStreamPathName         VT_LPWSTR|VT_VECTOR
  #define TYP_FPXStreamPathName           VT_LPWSTR|VT_VECTOR
  #define TYP_FPXStreamFieldOffset          VT_UI4|VT_VECTOR
  #define TYP_PropertySetPathName           VT_LPWSTR|VT_VECTOR
  #define TYP_PropertySetFormatID           VT_CLSID|VT_VECTOR
  #define TYP_PropertySetIDCodes            VT_LPWSTR|VT_VECTOR 
  
  // Legal Values of subfields:
  // --------------------------
  
  typedef enum {
    Compress_No       = 0x00,
    Compress_SingleColor  = 0x01,
    Compress_JPEG     = 0x02
//    Compress_PNG      = 0xFF  // Could be cool for the web...
  } TypCompression;

  typedef enum {
    ColorSpace_Colorless  = 0x0000,
    ColorSpace_Monochrome = 0x0001,
    ColorSpace_PhotoYCC   = 0x0002,
    ColorSpace_NifRGB   = 0x0003,
    ColorSpace_HSV      = 0x0004,
    ColorSpace_CIELab   = 0x0005,
    ColorSpace_CMYK     = 0x0006,
    ColorSpace_CMY      = 0x0007,
    ColorSpace_RGB      = 0x0008,
    ColorSpace_YRB      = 0x0009
  } TypColorSpace;

  typedef enum {
    Color_Monochrome  = 0x0000,
    Color_Red     = 0x0000,
    Color_Green     = 0x0001,
    Color_Blue      = 0x0002,
    Color_PhotoYCC_Y  = 0x0000,
    Color_PhotoYCC_C1 = 0x0001,
    Color_PhotoYCC_C2 = 0x0002,
    Color_Cyan      = 0x0000,
    Color_Magenta   = 0x0001,
    Color_Yellow    = 0x0002,
    Color_Black     = 0x0003,
    Color_Unspecified = 0x0000,
    Color_CIE_L     = 0x0000,
    Color_CIE_A     = 0x0001,
    Color_CIE_B     = 0x0002,
    Color_Hue     = 0x0000,
    Color_Saturation  = 0x0001,
    Color_Value     = 0x0002,
    Color_Unspec_Y    = 0x0000,
    Color_Unspec_R    = 0x0001,
    Color_Unspec_B    = 0x0002,
    Color_Opacity   = 0x7FFE,
    Color_Depth     = 0x7FFF
  } TypColor;

  // Image Contents Property Set: Extract compression information from compression subtype 
  #define GET_InterleaveType(compressionSubtype)    (compressionSubtype & 0x000000FF)
  #define GET_ChromaSubSample(compressionSubtype)   (compressionSubtype & 0x0000FFFF) >> 8
  #define GET_InternalColorConv(compressionSubtype) (compressionSubtype & 0x00FFFFFF) >> 16
  #define GET_JPEGTablesIndex(compressionSubtype)   (compressionSubtype & 0xFFFFFFFF) >> 24
  #define MAKE_CompressionSubType(Interleave, ChromaSubSample, InternalColor, JPEGTablesIndex )  (JPEGTablesIndex << 24 | InternalColor << 16 | ChromaSubSample << 8 | Interleave )
  #define SET_JPEGTablesIndex(compressionSubtype, JPEGTablesIndex) ((compressionSubtype & 0x00FFFFFF) | JPEGTablesIndex << 24)
  
  // Markers of a JPEG abbreviated format stream
  #define JPEGStartOfImageMarker  0xFFD8
  #define JPEGEndOfImageMarker  0xFFD9

  #define JPEGStartOfQuantizationTable  0xFFDB
  #define JPEGStartOfHuffmanTable     0xFFC4

  #define JPEGStartOfFrameMarker  0xFFC0
  #define JPEGStartOfScanMarker 0xFFDA

  #define PremultipliedBit  0x00008000
  #define UncalibratedBit   0x80000000

  #define MakeColorSpec(colorSpace,color) (long)(((long)(colorSpace)<<16) | (long)(color))
  #define IsPremultiplied(color)    ((color & PremultipliedBit) != 0)
  #define SetPremultiplied(color)   color = color |  PremultipliedBit
  #define UnsetPremultiplied(color) color = color & ~PremultipliedBit
  #define IsUncalibrated(color)     ((color & UncalibratedBit) != 0)
  #define SetUncalibrated(color)    color = color |  UncalibratedBit
  #define UnsetUncalibrated(color)  color = color & ~UncalibratedBit
  #define ExtractColorSpace(color)  color = color >> 16
  #define ExtractColorType(color)   color = color & OxFFFF

  typedef enum {
    Existence_NotCached = 0x0000,
    Existence_Cached  = 0x0001
  } TypExistence;
  
  typedef enum {
    Permission_Purgeable  = 0x0000,
    Permission_NotPurgeable = 0x0001
  } TypPermission;
  
//  Functions 'extern'
//  ------------------


  //  BUFFERS MUST BE LONG ENOUGH TO SUPPORT ANY OLE-NAME ... (32 chars)
  void  GetSummaryInfoName  (char * buffer);
  void  GetImageContentName (char * buffer);
  void  GetImageInfoName    (char * buffer);
  void  GetResolutionName   (char * buffer, unsigned long resNum);
  void  GetSubImageHdrName  (char * buffer, unsigned long imgNum);
  void  GetSubImageDataName (char * buffer, unsigned long imgNum);
  void  GetSupportDataName  (char * buffer, unsigned long imgNum);
  void  GetICCProfileName (char * buffer, unsigned short imgNum);
  void  GetCustomLinkName   (char * buffer);
  void  GetGlobalInfoName (char * buffer);
  void  GetImageDescName  (char * buffer, unsigned long index);
  void  GetImageStoreName (char * buffer, unsigned long index);
  void  GetTransformName  (char * buffer, unsigned long index);
  void  GetOperationName  (char * buffer, unsigned long index);
  void  GetExtensionName  (char * buffer);

#ifdef macintosh
  struct GUID;
#endif
  
//    GetGuid();
 
//  Variables 'extern'
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // FlashPixUtils_h
//  ----------------------------------------------------------------------------
