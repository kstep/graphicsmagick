//  ----------------------------------------------------------------------------
//  MODULE    : FPXBaselineIO
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Friday, April 5, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//    SCCSID          : @(#)fpxlibio.cpp  1.13 12:35:57 22 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
#ifdef _WINDOWS
  #pragma code_seg ("FPXBaselineIO")
#else
  #pragma segment FPXBaselineIO
#endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  Includes
//  --------


#ifndef Memoire_h
//include "b_memory.h"
#endif
#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEStorages_h
  #include "olestorg.h"
#endif
#ifndef OLEStreams_h
  #include "olestrm.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif
#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif

#ifndef Compresseur_h
  #include "fpxcompress.h"
#endif
#ifndef CompressorJPEG_h
  #include "cp_jpeg.h"
#endif
#ifndef FileFlashPixIO_h
  #include "f_fpxio.h"
#endif
#ifndef FPXImageView_h
  #include "fpximgvw.h"
#endif
#ifndef FlashPixFormat_h
  #include "fpxformt.h"
#endif
#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif
#ifndef PageIVUE_h
  #include "ri_page.h"
#endif
#ifndef FlashPixJPEG_h
  #include "fpxjpeg.h"
#endif
#if defined(__unix) // for CoFileTimeNow
  #include "time.hxx"
#endif

//  Constants
//  ---------

// Default resolution is 300 dpi
#define DEFAULT_RESOLUTION (300.0/25.4)

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Utilities Functions
//  ----------------------------------------------------------------------------



void FPXUpdateTime (FPXfiletime* theFPXTime)
{
  // This has to be rewritten for UNIX...
  CoFileTimeNow((FILETIME *)(theFPXTime));
}

void InitFPXStr (FPXStr* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXShortArray (FPXShortArray* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXLongArray (FPXLongArray* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXRealArray (FPXRealArray* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXWideStr (FPXWideStr* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXWideStrArray (FPXWideStrArray* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

void InitFPXStrArray (FPXStrArray* theFPXArray)
{
  theFPXArray->ptr = NULL; theFPXArray->length = 0; 
}

FPXStatus FPX_Strcpy (FPXStr* theFPXStr, const char* string)
{
  long i = strlen(string);
  if (FPX_AllocFPXStr(theFPXStr,i+1)!=FPX_OK) {
    theFPXStr = NULL;
    return FPX_MEMORY_ALLOCATION_FAILED;
  }
  unsigned char *pt = theFPXStr->ptr;
  while (i--)
    *pt++ = *string++;
  *pt = 0;
  return FPX_OK;
}

FPXStatus FPX_WideStrcpy (FPXWideStr* theFPXStr, const char* string)
{
  long i = strlen(string);
  if (FPX_AllocFPXWideStr(theFPXStr,i+1)!=FPX_OK) {
    theFPXStr = NULL;
    return FPX_MEMORY_ALLOCATION_FAILED;
  }
  unsigned short *pt = theFPXStr->ptr;
  while (i--)
    *pt++ = *string++;
  *pt = 0;
  return FPX_OK;
}

// FPX_WideStrcmp returns:
//  -1 if the first non-match in fpxStr1 is < fpxWStr2 or if fpxWStr1 is shorter
//   0 if strings match
//  +1 if the first non-match in fpxStr1 is > fpxWStr2 or if fpxWStr1 is longer
long FPX_WideStrcmp (FPXWideStr* fpxWStr1,  FPXWideStr* fpxWStr2)
{
  long      cnt1, cnt2, limit, index;
  unsigned short  *ptr1, *ptr2;
  
  cnt1 = fpxWStr1->length;
  cnt2 = fpxWStr2->length;
  
  ptr1 = fpxWStr1->ptr;
  ptr2 = fpxWStr2->ptr;

  if( cnt1 <= cnt2)
    limit = cnt1;
  else
    limit = cnt2;

  while (limit-- > 0) {
    if      (*ptr1 < *ptr2)   return -1;      
    else if (*ptr1 > *ptr2)   return  1;
    ptr1++; 
    ptr2++;
  }

  // If we got here, the first N characters are a match.
  if      (cnt1 < cnt2) return( -1);
  else if (cnt1 > cnt2) return(  1);
  else          return(  0);
}

long FPX_LPWSTRlen (LPWSTR wideStr)
{
  long cnt;
  for (cnt = 0; *wideStr++; cnt++)
    ;
  return cnt;
}

FPXStatus FPX_DeleteFPXStr (FPXStr* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXStr (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXShortArray (FPXShortArray* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXShortArray (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXLongArray (FPXLongArray* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXLongArray (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXRealArray (FPXRealArray* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXRealArray (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXWideStr (FPXWideStr* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXWideStr (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXWideStrArray (FPXWideStrArray* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXWideStrArray (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_DeleteFPXStrArray (FPXStrArray* theFPXArray)
{
  if (theFPXArray->ptr!=NULL) {
    delete [] theFPXArray->ptr; 
  }
  InitFPXStrArray (theFPXArray);
  return FPX_OK;
}

FPXStatus FPX_AllocFPXStr (FPXStr* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new unsigned char [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXShortArray (FPXShortArray* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new unsigned short [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXLongArray (FPXLongArray* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }

  theFPXArray->ptr    = new unsigned long [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXRealArray (FPXRealArray* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new float [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXWideStr (FPXWideStr* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new unsigned short [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = sizeof(unsigned short) * nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXWideStrArray (FPXWideStrArray* theFPXArray, long nbElem)
{

  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new FPXWideStr [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}

FPXStatus FPX_AllocFPXStrArray (FPXStrArray* theFPXArray, long nbElem)
{
  if (theFPXArray == NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->ptr    = new FPXStr [nbElem];
  if (theFPXArray->ptr==NULL) {
    theFPXArray->length = 0;
    return FPX_OBJECT_CREATION_FAILED;
  }
  theFPXArray->length = nbElem; 
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
//  External Functions 
//  This is the interface of the Baseline FlashPix IO Toolkit
//  ----------------------------------------------------------------------------

// System management tools : setting options and obtaining global information

FPXStatus FPX_GetErrorString (FPXStatus errorCode, 
                              char *errorString,
                              unsigned short maxStrLen) {

// errStrTable holds an array of strings.  The first element corresponds to FPX_OK (0), 
// and so on so the integer enum values can be used to index this array


// Note:  This array implementation is only temporary.  If a resource file is
// used, the function AfxGetInstanceHandle() must be called, and this causes a
// compile error because it uses the non-LivePicture memory management functions (eg malloc)
// The solution is to recompile the Afx code using LivePicture memory management.  (but how?)

char *errorStrTable[ERRSTR_TBLSIZE] = {
"FPX_OK",
"FPX_INVALID_FORMAT_ERROR",
"FPX_FILE_WRITE_ERROR",
"FPX_FILE_READ_ERROR",
"FPX_FILE_NOT_FOUND",
"FPX_COLOR_CONVERSION_ERROR",
"FPX_SEVER_INIT_ERROR",
"FPX_LOW_MEMORY_ERROR",
"FPX_IMAGE_TOO_BIG_ERROR",
"FPX_INVALID_COMPRESSION_ERROR",
"FPX_INVALID_RESOLUTION",
"FPX_INVALID_FPX_HANDLE",
"FPX_TOO_MANY_LINES",
"FPX_BAD_COORDINATES",
"FPX_FILE_SYSTEM_FULL",
"FPX_MISSING_TABLE",
"FPX_RETURN_PARAMETER_TOO_LARGE",
"FPX_NOT_A_VIEW",
"FPX_VIEW_IS_TRANFORMLESS",
"FPX_ERROR",
"FPX_UNIMPLEMENTED_FUNCTION",
"FPX_INVALID_IMAGE_DESC",
"FPX_INVALID_JPEG_TABLE",
"FPX_ILLEGAL_JPEG_ID",
"FPX_MEMORY_ALLOCATION_FAILED",
"FPX_NO_MEMORY_MANAGEMENT",
"FPX_OBJECT_CREATION_FAILED",
"FPX_FREE_NULL_PTR",
};

  if ((short)errorCode>=ERRSTR_TBLSIZE) {
    return FPX_ERROR;
  }
  char *tmpStr = errorStrTable[errorCode];
  if (strlen(tmpStr) > maxStrLen) {
    strncpy(errorString,tmpStr,maxStrLen);
  }
  else {
    strcpy(errorString,tmpStr);
  }

  return FPX_OK;

//-------------------------------------------------------------------------------------------------
// Resource based version.  Again, it won't compile because of the call to AfxGetInstanceHandle()
/*
WCHAR theError[256];
char *tmpStr;

  if (LoadString(AfxGetInstanceHandle(),  (UINT) errorCode+1, theError, (unsigned short)maxStrLen));

  WideCharToMultiByte(CP_ACP, 0, (WCHAR *)theError, -1, tmpStr, 256, NULL, NULL);
  strcpy(errorString,tmpStr);
  free(tmpStr);
*/
//-----------------------------------------------------------------------------------------------------

}

FPXStatus FPX_Delete(void *FPXObj) {

  if (FPXObj==NULL) {
    return FPX_FREE_NULL_PTR;
  }
  delete (unsigned char*)FPXObj;

  return FPX_OK;
}


FPXStatus FPX_InitSystem () 

{
  FPXStatus status = FPX_OK;        // PTCH_402 ensure OK status if no error found
  HRESULT   res;

  // Create an instance of the Toolkit context handle
  if (GtheSystemToolkit) {
    status = FPX_ClearSystem();
    if (status != FPX_OK) {
      return status;
    }
  }
  GtheSystemToolkit = new PSystemToolkit;
  if (GtheSystemToolkit==NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }

  res = OLEInit();
  int retcode = FAILED(res);
  
  switch (retcode) {
    case S_FALSE:
        GtheSystemToolkit->manageOLE = FALSE;
        break;
    case S_OK:
        GtheSystemToolkit->manageOLE = TRUE;
        break;
    default:
        FPX_ClearSystem();
        status = FPX_MEMORY_ALLOCATION_FAILED;
        break;
  }

  return status;
}

// CAUTION : memoryLimit = 0 is a legal value. In this case, no memory management will be
// done by the toolkit. Everything should be done by the host using FPX_PurgeToolkitMemory().
FPXStatus FPX_SetToolkitMemoryLimit ( unsigned long * memoryLimit)

{
  FPXStatus status = FPX_OK;
  HRESULT res;

  // if the GtheSystemToolkit exist yet delete it
  if (GtheSystemToolkit)
  {
    GtheSystemToolkit->PurgeSystem();
    delete GtheSystemToolkit;
    GtheSystemToolkit = NULL;
  }

  if (*memoryLimit < 0 ) {  
    // the requested memory is not valid
    *memoryLimit = 0;
    status = FPX_NO_MEMORY_MANAGEMENT;  
  } else if (*memoryLimit > 0 ){
    if (*memoryLimit < TOOLKIT_MIN_MEMORY) {
      // the requested memory is too small
      *memoryLimit = TOOLKIT_MIN_MEMORY;
      status = FPX_OK;  // We should may be send a warning to the user...
    }
  }
    
  GtheSystemToolkit = new PSystemToolkit;
  if (GtheSystemToolkit==NULL) {
    return FPX_OBJECT_CREATION_FAILED;
  }
  
  if (status == FPX_OK) 
  { 
    GtheSystemToolkit->manageOLE = TRUE;
  }
  
  return status;
}

long FPX_PurgeToolkitMemory (unsigned long memoryToBePurged) 

{
  return GtheSystemToolkit->PurgeSystemWithAmount(memoryToBePurged);
}

FPXStatus FPX_ClearSystem () 

{
  GtheSystemToolkit->PurgeSystem();

  if (GtheSystemToolkit->manageOLE == TRUE)
    OLEUninit();

  delete GtheSystemToolkit;
  GtheSystemToolkit = NULL;

  return FPX_OK;
}

FPXStatus FPX_SetProgressFunction ( FPXProgressFunction theProgressive)

{
  GtheSystemToolkit->SetProgFunc(theProgressive);

  return FPX_OK;
}

FPXStatus FPX_GetToolkitMemoryLimit ( unsigned long * memoryLimit)

{
  return FPX_NO_MEMORY_MANAGEMENT;
}

FPXStatus FPX_GetToolkitMemoryAvailable ( unsigned long * availableMemory)

{
  return FPX_NO_MEMORY_MANAGEMENT;
}

FPXStatus FPX_GetToolkitMemoryUsed ( unsigned long * usedMemory)

{
  return FPX_NO_MEMORY_MANAGEMENT;
}

FPXStatus FPX_LockFPXImage ( FPXImageHandle*  theFPX)

{ 
  GtheSystemToolkit->LockImage(theFPX->GetImage());

  return FPX_OK;
}

FPXStatus FPX_SetUnit ( FPXResolutionUnit  newUnit)

{
  GtheSystemToolkit->SetUnit((Typ_Unit)(newUnit));

  return FPX_OK;
}

FPXStatus FPX_GetUnit ( FPXResolutionUnit * newUnit)

{
  GtheSystemToolkit->GetUnit((Typ_Unit*)(newUnit));

  return FPX_OK;
}

FPXStatus FPX_GetToolkitVersion ( char* versionName, long* versionNumber)

{
  strcpy(versionName,TOOLKIT_NAME);
  *versionNumber = GtheSystemToolkit->GetToolkitVersion();

  return FPX_OK;
}

FPXStatus FPX_SetResampleMethod ( FPXResampleMethod method)

{
  GtheSystemToolkit->SetAntialias ((Typ_Antialias)(method));

  return FPX_OK;
}

FPXStatus FPX_SetComposeMethod (FPXComposeMethod method)

{
  GtheSystemToolkit->PreserveBackground (method == FPX_PROTECT_BACKGROUND);

  return FPX_OK;
}

FPXStatus FPX_SetViewBackgroundColor ( FPXColorspace colorspace, FPXBackground color)

{
  // Convert the color space into a Baseline space
  FPXBaselineColorSpace baseSpace;
  baseSpace = AnalyseFPXColorSpace (colorspace);

  // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
  unsigned long backColor = ((unsigned char)(color.color1_value) << 24) | 
                  ((unsigned char)(color.color2_value) << 16) | 
                  ((unsigned char)(color.color3_value) <<  8) | 
                   (unsigned char)(color.color4_value);
  backColor = backColor >> (8 * (4 - colorspace.numberOfComponents));

  // Update the Global background color
  // On Mac it is used the constructor Pixel (32int backColor)
  // (since SetBackgoundColor has Pixel type as a first parameter)
  // which map an integer on a Mac color; this does not work on Windows
  // since there is a different memory architecture
  // So, we do the conversion here---**IM--04/22/97
#ifdef _WINDOWS //**IM_18229
  Pixel newBackColor;
  newBackColor.alpha = (unsigned char)((backColor >> 24) & 0XFF);
  newBackColor.rouge = (unsigned char)((backColor >> 16) & 0XFF);
  newBackColor.vert  = (unsigned char)((backColor >> 8) & 0XFF);
  newBackColor.bleu  = (unsigned char)(backColor & 0XFF);
  GtheSystemToolkit->SetBackgroundColor (newBackColor,baseSpace);

#else //Mac code 
  GtheSystemToolkit->SetBackgroundColor (backColor,baseSpace);
#endif
  return FPX_OK;
}


//  Make a FPXJPEGTableGroup from a bunch of Q and H tables.            
FPXStatus FPX_CreateJPEGTableGroup (
              FPXJPEGTableGroup*    theGroup,
              short         numOfQuanTable,
              unsigned char *   quanTableChanID,
              FPXJPEGQuantTable*    quanTable,
              short         numOfHuffTable,
              unsigned char *   huffDCTableChanID,
              unsigned char *   huffACTableChanID,
              FPXJPEGHUFFTable*     huffTable )
{
  FPXStatus     status = FPX_OK;  
  ENCODER     encoder; 
  long      headerSize, i; 
  unsigned char header[FPX_MAX_TABLE_STREAM_SIZE]; 
  ptr_Compresseur jpegCompressor; 
  
  JPEGHuffTable   huffmanTables[8]; 
    JPEGQuantTable  quantTables[4];
  
  // Get the jpeg compressor
  jpegCompressor = (*tousLesCodecs)[TLC_JPEG];
  if ( !jpegCompressor ) 
  {
    status = FPX_OBJECT_CREATION_FAILED; 
    return status; 
  } 
  
  // Check the validity of number of Q-tables
    if (numOfQuanTable > 4 || numOfQuanTable < 1)
    return FPX_INVALID_JPEG_TABLE;
    
  // Check the validity of number of huffman-tables
    if (((numOfHuffTable % 2)!=0) || (numOfHuffTable > 8) || (numOfHuffTable < 2))
    return FPX_INVALID_JPEG_TABLE;
  
  // Get the encoder
  if ( !jpegCompressor->CompresseurPresent() ) {
    status = FPX_INVALID_JPEG_TABLE; 
    return status;  
  }
  else
    encoder = ((PCompressorJPEG *)jpegCompressor)->GetEncoder(); 
    
  for ( i = 0; i< numOfQuanTable; i++ ) {
    quantTables[i].quantizer = quanTable[i].quantizer; 
    quantTables[i].ident   = quanTable[i].ident; 
  }
  
  // Set the Q-tables
  if ( eJPEG_SetQuantTables(encoder, numOfQuanTable, (JPEGQuantTable **)&quantTables, quanTableChanID) )
  {
    status = FPX_INVALID_JPEG_TABLE; 
    return status; 
  }   
  
  for ( i = 0; i< numOfHuffTable; i++ ) {
    huffmanTables[i].bits   = huffTable[i].bits; 
    huffmanTables[i].vals   = huffTable[i].vals; 
    huffmanTables[i].hclass = huffTable[i].hclass; 
    huffmanTables[i].ident  = huffTable[i].ident; 
  }

  // Set the huffman tables
  if ( eJPEG_SetHuffTables(encoder, numOfHuffTable, (JPEGHuffTable **)&huffmanTables, 
          huffDCTableChanID, huffACTableChanID) )
  {
    status = FPX_INVALID_JPEG_TABLE; 
    return status; 
  }   
  
  // Create the jpeg header 
  if ( eJPEG_CreateHeader(
            encoder,      
            FPX_MAX_TABLE_STREAM_SIZE,  // the size of the default header in bytes
            header,           // the buffer itself
            &headerSize         // header length
            ))
  {
    status = FPX_INVALID_JPEG_TABLE; 
    return status; 
  }   
          
  theGroup->theStreamSize = (unsigned short) headerSize; 
  memcpy(theGroup->theStream, header, headerSize);        

  return status;
}

// Open and Close FlashPix files tools :

FPXStatus FPX_CloseImage ( FPXImageHandle* theFPX)

{
  FPXStatus status = FPX_OK;
  if (theFPX == NULL) {
    status = FPX_INVALID_FPX_HANDLE;
  } else {
    status = FPX_FlushModifiedTiles(theFPX);
    delete theFPX;
  }
  return status;
}
        
// Basic access tools :

FPXStatus FPX_WriteImageLine ( FPXImageHandle* theFPX, FPXImageDesc* theLine)

{
  FPXStatus status = FPX_OK;
  
  if (!theFPX) {
    status = FPX_INVALID_FPX_HANDLE;
  } else {
    long width, height;
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    image->GetTrueDimensions(&width,&height);

    FPXBufferDesc line(theLine,width,1);
    if ( line.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if (line.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      line.UpdateBuffer();
      image->SetUsedColorSpace(line.GetBaselineColorSpace());
      status = image->WriteLine (line.Get32BitsBuffer(), ActiveChannel_All); 

      if(status == FPX_OK)
        theFPX->SetImageEditedFlag();
    }
  }
  return status;
}

FPXStatus FPX_WriteImageRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              FPXImageDesc*       theData)

{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else 
  {
    // check to see if users descriptor matched internal memory layout
    // 4 channel RGBA
    if(!IsASupportedDescriptor(*theData,X1-X0+1))
    {
      theFPX->setInternalBuffer(X1-X0+1, Y1-Y0+1);
    }
    
    FPXBufferDesc rect(theData, X1-X0+1,Y1-Y0+1, theFPX->getInternalBuffer());
    
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if (rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      rect.UpdateBuffer();
      PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
      image->SetUsedColorSpace(rect.GetBaselineColorSpace());
      status = image->WriteRectangle (X0, Y0, X1, Y1, rect.Get32BitsBuffer(), 
                ActiveChannel_All);
      if (status == FPX_OK)
        theFPX->SetImageEditedFlag();
    }
  }
  return status;
}


FPXStatus FPX_WriteBackgroundRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              FPXColorspace       theColorspace,
              FPXBackground       theColor)
{
  FPXStatus status = FPX_OK;
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());

    // Convert the color space into a Baseline space
    FPXBaselineColorSpace baseSpace;
    baseSpace = AnalyseFPXColorSpace (theColorspace);

    // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
    // CHG_WIN_COLOR - use different byte ordering for Windows than on Mac
#ifdef _WINDOWS
      unsigned long backColor = ((unsigned char)(theColor.color1_value) <<  0) | 
                      ((unsigned char)(theColor.color2_value) <<  8) | 
                      ((unsigned char)(theColor.color3_value) << 16) | 
                      ((unsigned char)(theColor.color4_value) << 24);
    backColor = backColor << (8 * (4 - theColorspace.numberOfComponents));
#else
      unsigned long backColor = ((unsigned char)(theColor.color1_value) << 24) | 
                      ((unsigned char)(theColor.color2_value) << 16) | 
                      ((unsigned char)(theColor.color3_value) <<  8) | 
                       (unsigned char)(theColor.color4_value);
    backColor = backColor >> (8 * (4 - theColorspace.numberOfComponents));
#endif    
    // Set the description buffer and the used color for the file
    FPXBufferDesc rect(backColor,X1-X0+1,Y1-Y0+1,baseSpace);
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if (rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      // Write the rectangle    
      image->SetUsedColorSpace(baseSpace);
      status = image->WriteRectangle (X0, Y0, X1, Y1, rect.Get32BitsBuffer(), ActiveChannel_All);
      
      if(status == FPX_OK)
        theFPX->SetImageEditedFlag();
    }
  }
  return status;
}

FPXStatus FPX_WriteImageResolution (
              FPXImageHandle*     theFPX,
              unsigned long       theResolution,
              FPXImageDesc*       theData)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else 
  {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    // In the FlashPix API, the resolution ordering is reverse of the natural order...
    theResolution = (theResolution < 0 ? 0 : image->GetNbResolutions() - 1 - theResolution);
    long width, height;
    image->GetResolutionSize((short) theResolution,&width,&height);
    FPXBufferDesc rect(theData,width,height);
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if (rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      rect.UpdateBuffer();
      image->SetUsedColorSpace(rect.GetBaselineColorSpace());
      status = image->WriteRectangle (0, 0, width-1, height-1, rect.Get32BitsBuffer(), ActiveChannel_All, (short) theResolution);

      if(status == FPX_OK)  
        theFPX->SetImageEditedFlag();
    }
  }
  return status;
}

FPXStatus FPX_FlushModifiedTiles (
              FPXImageHandle*     theFPX)
{
  FPXStatus status = FPX_INVALID_FPX_HANDLE;
  
  if (theFPX) {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    if (image) 
      status = image->FlushModifiedTiles();
    else
      status = FPX_INVALID_IMAGE_DESC;
  }
    
  return status;
}

FPXStatus FPX_ReadImageTransformRectangle (
              FPXImageHandle*     theFPX,
              float               X0,
              float               Y0,
              float               X1,
              float               Y1,
              long                rectWidth,
              long                rectHeight,
              FPXImageDesc*       theRectangle)

{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else 
  {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    FPXBufferDesc rect(theRectangle, rectWidth, rectHeight);
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if (rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE)
      status = FPX_INVALID_IMAGE_DESC;
    else {
      // Set the color space required by theRectangle
      image->SetUsedColorSpace(rect.GetBaselineColorSpace());

      // We will apply the viewing parameters
      image->SetApplyParameter(TRUE);

      // Create an intermediate page as a rendering context
      float resolution = (float)(rectWidth) / (X1 - X0);
      TransfoPerspective identity;
      PageImage page (theFPX, rectWidth, rectHeight, resolution, X0, Y0, identity);

      // Render the page      
      status = page.ReadRectangle (0, 0, rectWidth, rectHeight, rect.Get32BitsBuffer(), rectWidth, FALSE);

      if(status == FPX_OK)
        rect.UpdateDescriptor();
    }
  }
  return status;
}

FPXStatus FPX_ReadImageRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              unsigned long       theResolution,
              FPXImageDesc*       theImage)

{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else 
  {
    PFileFlashPixIO* image = (PFileFlashPixIO*)(theFPX->GetImage());
    
    // In the FlashPix API, the resolution ordering is reverse of the natural order...
    theResolution = (theResolution < 0 ? 0 : image->GetNbResolutions() - 1 - theResolution);
    FPXBufferDesc rect(theImage, X1-X0+1, Y1-Y0+1);
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_MEMORY_ALLOCATION_FAILED; 
      
    if ( rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE )
      status = FPX_INVALID_IMAGE_DESC;
    else {
      // Set the color space required by theRectangle
      image->SetUsedColorSpace(rect.GetBaselineColorSpace());

      // We will NOT apply the viewing parameters
      image->SetApplyParameter(FALSE);

      status = image->ReadRectangle (X0, Y0, X1, Y1, rect.Get32BitsBuffer(), theResolution);
      
      if(status == FPX_OK)
        rect.UpdateDescriptor();
    }
  }
  return status;
}

FPXStatus FPX_ReadImageTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXImageDesc*       theTile)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else 
  {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    // In the FlashPix API, the resolution ordering is reverse of the natural order...
    theResolution = (theResolution < 0 ? 0 : image->GetNbResolutions() - 1 - theResolution);
    long tileWidth = image->GetTileWidth();
    FPXBufferDesc rect(theTile, tileWidth, tileWidth);
    if ( rect.Get32BitsBuffer() == NULL ) 
      return FPX_OBJECT_CREATION_FAILED; 
          
    if (rect.GetBaselineColorSpace() == NON_AUTHORIZED_SPACE) {
      status = FPX_INVALID_IMAGE_DESC;
    } else {
      long nbTilesW, nbTilesH;
      image->GetResolutionTileSize ((short)theResolution, &nbTilesW, &nbTilesH);
      long X0 = (whichTile % nbTilesW) * tileWidth;
      long Y0 = (whichTile / nbTilesW) * tileWidth;

      // Set the color space required by theRectangle
      image->SetUsedColorSpace(rect.GetBaselineColorSpace());

      // We will NOT apply the viewing parameters
      image->SetApplyParameter(FALSE);

      status = image->ReadRectangle (X0, Y0, X0+tileWidth-1, Y0+tileWidth-1, rect.Get32BitsBuffer(), theResolution);

      if(status == FPX_OK)
        rect.UpdateDescriptor();
    }
  }
  return status;
}

FPXStatus FPX_ReadImageCompressedTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXTileDesc*        theTile)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {
  
    // Get the pointer to the I/O object
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    // In the FlashPix API, the resolution ordering is reverse of the natural order...
    theResolution = (theResolution < 0 ? 0 : image->GetNbResolutions() - 1 - theResolution);
    
      
    // Call the read raw function
    status = image->ReadRawTile (theResolution, whichTile, 
                  &(theTile->compInfo.compressOption), 
                  &(theTile->compInfo.compressQuality),
                  &(theTile->compInfo.compressSubtype),
                  &(theTile->dataLength),
                  &(theTile->data));
  }
  return status;
}

FPXStatus FPX_WriteImageCompressedTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXTileDesc*        theTile)

{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {

    // Get the pointer to the I/O object
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    
    // In the FlashPix API, the resolution ordering is reverse of the natural order...
    theResolution = (theResolution < 0 ? 0 : image->GetNbResolutions() - 1 - theResolution);
    
      
    // Call the write raw function
    status = image->WriteRawTile(theResolution, whichTile, 
                  theTile->compInfo.compressOption, 
                  theTile->compInfo.compressQuality,
                  theTile->compInfo.compressSubtype,
                  theTile->dataLength,
                  theTile->data);

    if(status == FPX_OK)     // If no error when writing, set the "edited" flag
      theFPX->SetImageEditedFlag();
  }
  return status;
}


FPXStatus FPX_SetDecimationQuality ( FPXImageHandle*  theFPX, unsigned short decimationQuality)
{
  Typ_Convolution newConvolution = Convolution_Standard;
  if (decimationQuality >= 4)
    newConvolution = Convolution_Gauss;
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  image->SetConvolution (newConvolution);
  return FPX_OK;
}

FPXStatus FPX_SetResolutionInfo (
              FPXImageHandle*  theFPX,
              FPXResolution*   theResolutionInfo)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  image->SetResolutionInfo(theResolutionInfo); 
  
  return status;  
}

FPXStatus FPX_GetResolutionInfo (
              FPXImageHandle*  theFPX,
              FPXResolution*   theResolutionInfo)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  image->GetResolutionInfo(theResolutionInfo); 
  
  return status;  
}

// Not implemented in Baseline
FPXStatus FPX_SetAlphaType (
              FPXImageHandle*      theFPX,
              FPXPreComputedAlpha  theAlphaType)
{
  FPXImageHandle* handle = theFPX;
  FPXPreComputedAlpha  a = theAlphaType;
  return FPX_UNIMPLEMENTED_FUNCTION;
}

// Not implemented in Baseline
FPXStatus FPX_GetAlphaType (
              FPXImageHandle*      theFPX,
              FPXPreComputedAlpha* theAlphaType)
{
  FPXImageHandle* handle = theFPX;
  FPXPreComputedAlpha*  a = theAlphaType;
  return FPX_UNIMPLEMENTED_FUNCTION;
}

FPXStatus FPX_SetJPEGTableGroup (
              FPXImageHandle*     theFPX,
              FPXJPEGTableGroup*  theGroup,
              unsigned char       theTableGroupID)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
  
  if (filePtr) {
    OLEProperty*  aProp;
    OLEBlob     jpegTable; 
    
    // Convert the index into property ID for jpeg table
    unsigned long   PID_jpegTableIndex = PID_JPEGTables(theTableGroupID); 
    
    // Copy the stream in jpeg table into blob
    jpegTable.WriteVT_VECTOR(theGroup->theStream, (unsigned long)theGroup->theStreamSize); 
    
    // Save the jpeg table into image content property set
    if (filePtr->SetImageContentProperty (PID_jpegTableIndex, TYP_JPEGTables, &aProp)) {
      *aProp = jpegTable.GetBlob();
      
      // Save the maximum jpeg table index
      if ( !filePtr->GetImageContentProperty (PID_MaxJPEGTables, &aProp) ) 
        if (filePtr->SetImageContentProperty (PID_MaxJPEGTables, TYP_MaxJPEGTables, &aProp)) 
          *aProp = (long)theTableGroupID;
        else
          return  FPX_INVALID_JPEG_TABLE;
      else {
        long tempMaxIndex = (long)(*aProp); 
        tempMaxIndex = (tempMaxIndex < theTableGroupID) ? theTableGroupID : tempMaxIndex; 
        *aProp = (long)tempMaxIndex;
      }
      filePtr->Commit();
      
      // Set the compression table index
      image->SetCompressTableGroup(theTableGroupID); 
    }
    else
      status = FPX_INVALID_JPEG_TABLE;            
  }
  
  return status;
}

FPXStatus FPX_GetJPEGTableGroup (
              FPXImageHandle*     theFPX,
              FPXJPEGTableGroup*  theGroup,
              unsigned char       theTableGroupID)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
  
  if (filePtr) {
    OLEProperty*  aProp;
    OLEBlob     jpegTable; 
    
    unsigned long   PID_jpegTableIndex = PID_JPEGTables(theTableGroupID); 
    
    // Get the jpeg table from image content property set
    if (filePtr->GetImageContentProperty (PID_jpegTableIndex, &aProp)) {
      jpegTable = (BLOB*)(*aProp);
    }
    else {
      status = FPX_INVALID_JPEG_TABLE;
      return status;
    } 
      
    // Read the stream in jpeg table from blob
    unsigned char *str;   
    long strlen; 
      
    strlen = jpegTable.ReadVT_VECTOR(&str);   
    
    // Make sure the stream length can not exceed maximum stream table length
    if ( strlen > FPX_MAX_TABLE_STREAM_SIZE )
      status = FPX_INVALID_JPEG_TABLE;
    else {
      theGroup->theStreamSize = (unsigned short) strlen; 
      memcpy(theGroup->theStream, str, strlen); 
      delete str; 
    }
    
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_SelectJPEGTableGroup (
              FPXImageHandle*  theFPX,
              unsigned char    theTableGroupID)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
  
  if (filePtr) {
    OLEProperty*  aProp;
    
    // Convert the index into property ID for jpeg table
    unsigned long   PID_jpegTableIndex = PID_JPEGTables(theTableGroupID); 
    
    // Find the jpeg table specified by theTableGroupID in the image content property set
    if (filePtr->GetImageContentProperty (PID_jpegTableIndex, &aProp)) {

      // Set the compression table index
      image->SetCompressTableGroup(theTableGroupID); 
    }
    else
      // No such table in the image content property set
      status = FPX_INVALID_JPEG_TABLE;
            
  }
  
  return FPX_UNIMPLEMENTED_FUNCTION; 
  //return status;
}

FPXStatus FPX_SetJPEGCompression (
              FPXImageHandle*  theFPX,
              unsigned short   theQualityFactor)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  // Convert the Kodak's quality factor to JPEG's
  unsigned char qualityFactor; 
  qualityFactor = (unsigned char) (2.55 * (100 - theQualityFactor)); 
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  image->SetQualityFactor(qualityFactor);       // Set quality factor
  
  return status;  
}

// Create a fpx file
FPXStatus CreateImageByFilename (
              FicNom&             fileName,
        unsigned long         width,
              unsigned long         height,
              unsigned long         tileWidth,      // not implemented in Baseline (always 64)
              unsigned long         tileHeight,     // not implemented in Baseline (always 64)
              FPXColorspace         colorspace,
              FPXBackground         backgroundColor,
              FPXCompressionOption  compressOption, 
              FPXImageHandle**      theFPX)
{
  FPXStatus status = FPX_OK;
  long w = tileWidth = tileHeight;
  
  // CHG_FILE_ERR2 - clear the errors list, so we only get new error reports
  GtheSystemToolkit->DeleteErrorsList();
  
  // Convert the color space into a Baseline space
  FPXBaselineColorSpace baseSpace;
  baseSpace = AnalyseFPXColorSpace (colorspace);

  // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
  // CHG_WIN_COLOR - use different byte ordering for Windows than on Mac
#ifdef _WINDOWS
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) <<  0) | 
                  ((unsigned char)(backgroundColor.color2_value) <<  8) | 
                  ((unsigned char)(backgroundColor.color3_value) << 16) | 
                  ((unsigned char)(backgroundColor.color4_value) << 24);
  backColor = backColor << (8 * (4 - colorspace.numberOfComponents));
#else
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) << 24) | 
                  ((unsigned char)(backgroundColor.color2_value) << 16) | 
                  ((unsigned char)(backgroundColor.color3_value) <<  8) | 
                   (unsigned char)(backgroundColor.color4_value);
  backColor = backColor >> (8 * (4 - colorspace.numberOfComponents));
#endif  
  // The last parameter to "TRUE" indicates that we ALWAYS create a FPX Image View. If the user never call
  // Viewing parameter function, the Toolkit will automatically creates transformless images
  *theFPX = new PFlashPixImageView(fileName, width, height,(float) DEFAULT_RESOLUTION, baseSpace, backColor, compressOption, TRUE, colorspace.isUncalibrated);
  
  if (*theFPX == NULL)
    status = FPX_SEVER_INIT_ERROR;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL)
      status = FPX_FILE_NOT_FOUND;
    else
      status = image->OpenImage();
  }

  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  }
  
  return status;
}

// Create function using the proper file identifier for each platform
#ifdef macintosh
FPXStatus FPX_CreateImageByFilename (
              const FSSpec&        fileSpecs,
              unsigned long        width,
              unsigned long        height,
              unsigned long        tileWidth,
              unsigned long        tileHeight,
              FPXColorspace        colorspace,
              FPXBackground        backgroundColor,
              FPXCompressionOption compressOption,
              FPXImageHandle**     theFPX)
{
  FicNom  file(fileSpecs);
  return CreateImageByFilename (
              file,
              width,
              height,
              tileWidth,
              tileHeight,
              colorspace,
              backgroundColor,
              compressOption,
              theFPX);
}
#else
FPXStatus FPX_CreateImageByFilename (
              char*                fileName,
              unsigned long        width,
              unsigned long        height,
              unsigned long        tileWidth,
              unsigned long        tileHeight,
              FPXColorspace        colorspace,
              FPXBackground        backgroundColor,
              FPXCompressionOption compressOption,
              FPXImageHandle**     theFPX)
{
  FicNom  file(fileName);
  return CreateImageByFilename (
              file,
              width,
              height,
              tileWidth,
              tileHeight,
              colorspace,
              backgroundColor,
              compressOption,
              theFPX);
}
#endif

FPXStatus FPX_CreateImageByStorage (
              IStorage*            owningStorage,
              char*                storageName,
              unsigned long        width,
              unsigned long        height,
              unsigned long        tileWidth,     // not implemented in Baseline (always 64)
              unsigned long        tileHeight,      // not implemented in Baseline (always 64)
              FPXColorspace        colorspace,
              FPXBackground        backgroundColor,
              FPXCompressionOption compressOption,
              FPXImageHandle**     theFPX)
{
// BEGIN: djk 052297 AI17850
  // Check params
  if (!(theFPX && owningStorage))
    return FPX_ERROR;
  
  // Satisfy failure conditions
  *theFPX = NULL;
// END: djk 052297 AI17850
  
  FPXStatus status = FPX_OK;
  long a = tileWidth = tileHeight;
  
  // Convert the color space into a Baseline space
  FPXBaselineColorSpace baseSpace;
  baseSpace = AnalyseFPXColorSpace (colorspace);

  // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
#ifdef _WINDOWS
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) <<  0) | 
                  ((unsigned char)(backgroundColor.color2_value) <<  8) | 
                  ((unsigned char)(backgroundColor.color3_value) << 16) | 
                  ((unsigned char)(backgroundColor.color4_value) << 24);
  backColor = backColor << (8 * (4 - colorspace.numberOfComponents));
#else
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) << 24) | 
                  ((unsigned char)(backgroundColor.color2_value) << 16) | 
                  ((unsigned char)(backgroundColor.color3_value) <<  8) | 
                   (unsigned char)(backgroundColor.color4_value);
  backColor = backColor >> (8 * (4 - colorspace.numberOfComponents));
#endif
// BEGIN: djk 052297 AI17850: handle sub-storages correctly
  const CLSID clsidFpxRoot = ID_ImageView;
  OLEStorage* theowningStorage = NULL;
  OLEStorage* theSubStorage = NULL;

  if (storageName)
  {
    // Create an object wrap around the storage
    theowningStorage = new OLEStorage(
      (OLEStorage *)(NULL),
      owningStorage);
    if (!theowningStorage)
    {
      // Couldn't create storage wrapper
      return FPX_MEMORY_ALLOCATION_FAILED;
    }

    // Create the sub-storage as an FPX Root
    Boolean bRes;
    bRes = theowningStorage->CreateStorage(
      (CLSID) clsidFpxRoot,
      (const char*) storageName,
      &theSubStorage);

    // We're done with the parent storage now; release it
    status = theowningStorage->getFPXStatus();
    delete theowningStorage; theowningStorage = NULL;

    // See if we actually got the sub-storage
    if (!bRes)
      return status;

    // Create the ImageView
    *theFPX = new PFlashPixImageView(
      theSubStorage,
      (const char*) NULL,
      width,
      height,
      (float) DEFAULT_RESOLUTION,
      baseSpace,
      backColor,
      compressOption,
      TRUE,
      colorspace.isUncalibrated);
  }
  else
  {
    // The specified storage is to become an FPX Root.
    // Create an object wrap around the storage and set the CLSID for FPX
    theowningStorage = new OLEStorage(
      (CLSID) clsidFpxRoot,
      (OLEStorage *)(NULL),
      owningStorage);
    if (!theowningStorage)
    {
      // Couldn't create storage wrapper
      return FPX_MEMORY_ALLOCATION_FAILED;
    }

    // Create the ImageView at the specified storage
    *theFPX = new PFlashPixImageView(
      theowningStorage,
      (const char*) NULL,
      width,
      height,
      (float) DEFAULT_RESOLUTION,
      baseSpace,
      backColor,
      compressOption,
      TRUE,
      colorspace.isUncalibrated);
  }
  
  // See if we actually got the ImageView
  if (*theFPX == NULL)
  {
    // Couldn't create ImageView; clean up
    delete theSubStorage; theSubStorage = NULL;
    delete theowningStorage; theowningStorage = NULL;
    status = FPX_MEMORY_ALLOCATION_FAILED;
  }
// END: djk 052297 AI17850: handle sub-storages correctly
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL)
      status = FPX_FILE_NOT_FOUND;
    else
      status = image->OpenImage();
  }

  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  }
  
  return status;
}

// Open a fpx file
// CHG_VIS_OUT - Added a new argument to support specifying a visible output
//  index during open. This allows support for some non-core files that have
//  multiple visible output images.
FPXStatus OpenImageByFilename (
              FicNom&        fileName,
              char*            storagePathInFile, // for the moment, only the root storage is considered
              unsigned long    visibleOutputIndex,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  FPXStatus status = FPX_OK;
  
  // CHG_FILE_ERR2 - clear the errors list, so we only get new error reports
  GtheSystemToolkit->DeleteErrorsList();
  
  // 
  // Open the FPX thing: the constructor automatically detects if it's a FPX Image Object or a 
  // FPX Image View
  *theFPX = new PFlashPixImageView (fileName, storagePathInFile, 
            mode_Modification,
            visibleOutputIndex,
            NULL, &status);
  if (*theFPX == NULL)
    status = FPX_SEVER_INIT_ERROR;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL) 
      status = (*theFPX)->FileStatus ();
    else
      status = image->OpenImage();
  }
  
  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  } else {
    // If no error, get the FPX Image Object pointer and read the values
    long w, h, tw, th;
    FPXBaselineColorSpace baseSpace;
    Typ_Compression c; 
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    image->GetInfo(&w,&h,&tw,&th,&c);
    baseSpace = image->GetBaselineSpace();
    *width  = w;
    *height = h;
    *tileWidth  = tw;
    *tileHeight = th;
    CreateFPXColorSpace (baseSpace, colorspace);
    colorspace->isUncalibrated = image->GetUncalibratedFlag();
  }
  
  return status;
}

#ifdef macintosh
FPXStatus FPX_OpenImageByFilename (
              const FSSpec&    fileSpecs,
              char*            storagePathInFile,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  return OpenImageByFilename (
              fileSpecs,
              storagePathInFile,
              0,          // CHG_VIS_OUT - Core images use visible output index of 0
              width,
              height,
              tileWidth,
              tileHeight,
              colorspace,
              theFPX);
}

// CHG_VIS_OUT - Added a file open call to support specifying a visible output
//  index during open. This allows support for some non-core files that have
//  multiple visible output images.
FPXStatus FPX_OpenIndexedImageByFilename (
              const FSSpec&    fileSpecs,
              char*            storagePathInFile,
              unsigned long    visibleOutputIndex,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  FicNom  file(fileSpecs);
  return OpenImageByFilename (
        file,
                storagePathInFile,
                visibleOutputIndex,
                width,
                height,
                tileWidth,
                tileHeight,
                colorspace,
                theFPX);
}

#else
FPXStatus FPX_OpenImageByFilename (
              char*            fileName,
              char*            storagePathInFile,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  FicNom  file(fileName);
  return OpenImageByFilename (
        file,
                storagePathInFile,
                0,          // CHG_VIS_OUT - Core images use visible output index of 0
                width,
                height,
                tileWidth,
                tileHeight,
                colorspace,
                theFPX);
}

// CHG_VIS_OUT - Added a file open call to support specifying a visible output
//  index during open. This allows support for some non-core files that have
//  multiple visible output images.
FPXStatus FPX_OpenIndexedImageByFilename (
              char*            fileName,
              char*            storagePathInFile,
              unsigned long    visibleOutputIndex,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  FicNom  file(fileName);
  return OpenImageByFilename (
        file,
                storagePathInFile,
                visibleOutputIndex,
                width,
                height,
                tileWidth,
                tileHeight,
                colorspace,
                theFPX);
}
#endif

FPXStatus FPX_OpenImageByStorage (
              IStorage*        storagePointer,
              char*            storagePathInFile,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX)
{
  FPXStatus status = FPX_OK;
  
// BEGIN: djk 052197 AI17850
  if (!(storagePointer && width && height && tileWidth && tileHeight && colorspace && theFPX))
    return FPX_ERROR;

  // Satisfy failure conditions
  *theFPX = NULL;
// END: djk 052197 AI17850

// BEGIN: djk 052297 AI17850: handle sub-storages correctly
  OLEStorage* theowningStorage = NULL;
  OLEStorage* theSubStorage = NULL;
  
  // Create an object wrap around the IStorage
  theowningStorage = new OLEStorage
    ((OLEStorage *)(NULL),
    storagePointer);
  if (!theowningStorage)
    return FPX_MEMORY_ALLOCATION_FAILED;

  if (storagePathInFile)
  {
    // Open the sub-storage
    Boolean bRes;
    bRes = theowningStorage->OpenStorage(
      (const char*) storagePathInFile,
      &theSubStorage,
      OLE_READWRITE_MODE);

    // We're done with the parent storage now
    status = theowningStorage->getFPXStatus();
    delete theowningStorage; theowningStorage = NULL;

    // See if we really got the sub-storage
    if (!theSubStorage)
      return status;

    // Open the FPX thing: the constructor automatically detects if it's a FPX Image Object or a 
    // FPX Image View
    *theFPX = new PFlashPixImageView(
      theSubStorage,
      (const char*) NULL,
      mode_Modification);
  }
  else
  {
    // Open the FPX thing: the constructor automatically detects if it's a FPX Image Object or a 
    // FPX Image View
    *theFPX = new PFlashPixImageView(
      theowningStorage,
      (const char*) NULL,
      mode_Modification);
  }

  // See if we actually got the ImageView
  if (*theFPX == NULL)
  {
    // Failed to create ImageView; clean up
    delete theSubStorage; theSubStorage = NULL;
    delete theowningStorage; theowningStorage = NULL;
    status = FPX_SEVER_INIT_ERROR;
  }
// END: djk 052297 AI17850: handle sub-storages correctly
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL)
      status = FPX_FILE_NOT_FOUND;
    else
      status = image->OpenImage();
  }
  
  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  } else {
    // If no error, get the FPX Image Object pointer and read the values
    long w, h, tw, th;
    FPXBaselineColorSpace baseSpace;
    Typ_Compression c; 
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    image->GetInfo(&w,&h,&tw,&th,&c);
    baseSpace = image->GetBaselineSpace();
    *width  = w;
    *height = h;
    *tileWidth  = tw;
    *tileHeight = th;
    CreateFPXColorSpace (baseSpace, colorspace);
    colorspace->isUncalibrated = image->GetUncalibratedFlag();
  }
  
  return status;
}

/***************************************************************************
   IMAGES WITH VIEWS
 ***************************************************************************/

FPXStatus CreateImageWithViewByFilename (
              FicNom&              fileName,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,       // not implemented in Baseline (always 64)
              unsigned long          tileHeight,      // not implemented in Baseline (always 64)
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX)
{
  FPXStatus status = FPX_OK;
  long a = tileWidth = tileHeight; 
  
  // CHG_FILE_ERR2 - clear the errors list, so we only get new error reports
  GtheSystemToolkit->DeleteErrorsList();
  
  // Convert the color space into a Baseline space
  FPXBaselineColorSpace baseSpace;
  baseSpace = AnalyseFPXColorSpace (colorspace);

  // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
  // CHG_WIN_COLOR - use different byte ordering for Windows than on Mac
#ifdef _WINDOWS
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) <<  0) | 
                  ((unsigned char)(backgroundColor.color2_value) <<  8) | 
                  ((unsigned char)(backgroundColor.color3_value) << 16) | 
                  ((unsigned char)(backgroundColor.color4_value) << 24);
  backColor = backColor << (8 * (4 - colorspace.numberOfComponents));
#else
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) << 24) | 
                  ((unsigned char)(backgroundColor.color2_value) << 16) | 
                  ((unsigned char)(backgroundColor.color3_value) <<  8) | 
                   (unsigned char)(backgroundColor.color4_value);
  backColor = backColor >> (8 * (4 - colorspace.numberOfComponents));
#endif
  *theFPX = new PFlashPixImageView  (fileName, width, height,(float) DEFAULT_RESOLUTION, baseSpace, backColor, compressOption, TRUE, colorspace.isUncalibrated);
  
  if (*theFPX == NULL)
    status = FPX_SEVER_INIT_ERROR;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL)
      status = FPX_FILE_NOT_FOUND;
    else {
      status = image->OpenImage();

      (*theFPX)->SetImageAffineMatrix (affineMatrix);
      (*theFPX)->SetImageContrastAdjustment (contrastValue);
      (*theFPX)->SetImageColorTwistMatrix (colorTwist); 
      (*theFPX)->SetImageFilteringValue (filteringValue);
      (*theFPX)->SetImageROI (regionOfInterest);
      (*theFPX)->SetImageResultAspectRatio (resultAspectRatio);
    }
  }

  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  }
  
  return status;
}

// Create function using the proper file identifier for each platform
#ifdef macintosh
FPXStatus FPX_CreateImageWithViewByFilename (
              const FSSpec&          fileSpecs,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,
              unsigned long          tileHeight,
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX)
{
  FicNom  file(fileSpecs);
  return CreateImageWithViewByFilename (
              file,
              width,
              height,
              tileWidth,
              tileHeight,
              colorspace,
              backgroundColor,
              compressOption,
              affineMatrix,
              contrastValue,
              colorTwist,
              filteringValue,
              regionOfInterest,
              resultAspectRatio,
              theFPX);
}
#else
FPXStatus FPX_CreateImageWithViewByFilename (
              char*                  fileName,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,
              unsigned long          tileHeight,
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX)
{
  FicNom  file(fileName);
  return CreateImageWithViewByFilename (
              file,
              width,
              height,
              tileWidth,
              tileHeight,
              colorspace,
              backgroundColor,
              compressOption,
              affineMatrix,
              contrastValue,
              colorTwist,
              filteringValue,
              regionOfInterest,
              resultAspectRatio,
              theFPX);
}
#endif

FPXStatus FPX_CreateImageWithViewByStorage (
              IStorage*              owningStorage,
              char*                  storageName,
              unsigned long          width,
              unsigned long          height,
              unsigned long          tileWidth,
              unsigned long          tileHeight,
              FPXColorspace          colorspace,
              FPXBackground          backgroundColor,
              FPXCompressionOption   compressOption,
              FPXAffineMatrix*       affineMatrix,
              FPXContrastAdjustment* contrastValue,
              FPXColorTwistMatrix*   colorTwist,
              FPXFilteringValue*     filteringValue,
              FPXROI*                regionOfInterest,
              FPXResultAspectRatio*  resultAspectRatio,
              FPXImageHandle**       theFPX)
{
  FPXStatus status = FPX_OK;
  long a = tileWidth = tileHeight; 
  
  // Convert the color space into a Baseline space
  FPXBaselineColorSpace baseSpace;
  baseSpace = AnalyseFPXColorSpace (colorspace);

  // Convert the background color into a Baseline (32 bits per pixel, 8 bits per channel) value
  // CHG_WIN_COLOR - Use different byte ordering for Windows
#ifdef _WINDOWS
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) <<  0) | 
                  ((unsigned char)(backgroundColor.color2_value) <<  8) | 
                  ((unsigned char)(backgroundColor.color3_value) << 16) | 
                  ((unsigned char)(backgroundColor.color4_value) << 24);
  backColor = backColor << (8 * (4 - colorspace.numberOfComponents));
#else
  unsigned long backColor = ((unsigned char)(backgroundColor.color1_value) << 24) | 
                  ((unsigned char)(backgroundColor.color2_value) << 16) | 
                  ((unsigned char)(backgroundColor.color3_value) <<  8) | 
                   (unsigned char)(backgroundColor.color4_value);
  backColor = backColor >> (8 * (4 - colorspace.numberOfComponents));
#endif

// BEGIN: djk 052297 AI17850: handle sub-storages correctly
  const CLSID clsidFpxRoot = ID_ImageView;
  OLEStorage* theowningStorage = NULL;
  OLEStorage* theSubStorage = NULL;

  if (storageName)
  {
    // Create an object wrap around the storage
    theowningStorage = new OLEStorage(
      (OLEStorage *)(NULL),
      owningStorage);
    if (!theowningStorage)
    {
      // Couldn't create storage wrapper
      return FPX_MEMORY_ALLOCATION_FAILED;
    }

    // Create the sub-storage as an FPX Root
    Boolean bRes;
    bRes = theowningStorage->CreateStorage(
      (CLSID) clsidFpxRoot,
      (const char*) storageName,
      &theSubStorage);

    // We're done with the parent storage now; release it
    status = theowningStorage->getFPXStatus();
    delete theowningStorage; theowningStorage = NULL;

    // See if we actually got the sub-storage
    if (!bRes)
      return status;

    // Create the ImageView
    *theFPX = new PFlashPixImageView(
      theSubStorage,
      (const char*) NULL,
      width,
      height,
      (float) DEFAULT_RESOLUTION,
      baseSpace,
      backColor,
      compressOption,
      TRUE,
      colorspace.isUncalibrated);
  }
  else
  {
    // The specified storage is to become an FPX Root.
    // Create an object wrap around the storage and set the CLSID for FPX
    theowningStorage = new OLEStorage(
      (CLSID) clsidFpxRoot,
      (OLEStorage *)(NULL),
      owningStorage);
    if (!theowningStorage)
    {
      // Couldn't create storage wrapper
      return FPX_MEMORY_ALLOCATION_FAILED;
    }

    // Create the ImageView at the specified storage
    *theFPX = new PFlashPixImageView(
      theowningStorage,
      (const char*) NULL,
      width,
      height,
      (float) DEFAULT_RESOLUTION,
      baseSpace,
      backColor,
      compressOption,
      TRUE,
      colorspace.isUncalibrated);
  }
  
  // See if we actually got the ImageView
  if (*theFPX == NULL)
  {
    // Couldn't create ImageView; clean up
    delete theSubStorage; theSubStorage = NULL;
    delete theowningStorage; theowningStorage = NULL;
    status = FPX_MEMORY_ALLOCATION_FAILED;
  }
// END: djk 052297 AI17850: handle sub-storages correctly
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)((*theFPX)->GetImage());
    if (image == NULL)
      status = FPX_FILE_NOT_FOUND;
    else {
      status = image->OpenImage();

      (*theFPX)->SetImageAffineMatrix (affineMatrix);
      (*theFPX)->SetImageContrastAdjustment (contrastValue);
      (*theFPX)->SetImageColorTwistMatrix (colorTwist); 
      (*theFPX)->SetImageFilteringValue (filteringValue);
      (*theFPX)->SetImageROI (regionOfInterest);
      (*theFPX)->SetImageResultAspectRatio (resultAspectRatio);
    }
  }

  if ((status != FPX_OK) && (*theFPX)) {
    delete *theFPX;
    *theFPX = NULL;
  }
  
  return status;
}

/********************************************************************************
   ICC PROFILE
 *******************************************************************************/

FPXStatus FPX_SetICCProfile(
              FPXImageHandle *theFPX,
              FPXStr *        theProfile,
              unsigned short  profileIndex)
{
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
  
  if ( !filePtr->SetICCProfile(profileIndex, FPXStrToLPSTR(*theProfile)) ) 
    return FPX_ERROR; 
    
  return status;
}

FPXStatus FPX_GetICCProfile(
              FPXImageHandle *theFPX,
              FPXStr *        theProfile,
              unsigned short  profileIndex)
{
  char *profileStream;
  
  FPXStatus status = FPX_OK;
  
  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
  PFlashPixFile* filePtr = (PFlashPixFile*)(image->GetCurrentFile());
  
  if ( filePtr->GetICCProfile(profileIndex, &profileStream) ) 
    status=FPX_Strcpy(theProfile, profileStream);    
  else 
    return FPX_ERROR; 
    
  return status;
}

/***************************************************************************
    HIERARCHY GENERATION AND FLAT IMAGES HANDLING routines
 ***************************************************************************/

FPXStatus FPX_DoNotComputeHierarchy (
              FPXImageHandle*  theFPX)
{
  FPXStatus status = FPX_OK;
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    image->SetAutomaticDecimation(FALSE);
  }
  return status;
}

FPXStatus FPX_GenerateHierarchy (
              FPXImageHandle*  theFPX)
{
  FPXStatus status = FPX_OK;
  if (!theFPX)
    status = FPX_INVALID_FPX_HANDLE;
  else {
    PFileFlashPixIO* image =  (PFileFlashPixIO*)(theFPX->GetImage());
    image->RegenerateHierarchy();
    status = FPX_FlushModifiedTiles(theFPX);
  }
  return status;
}

/********************************************************************************
   PATH
 *******************************************************************************/
 
/*
 *  CAUTION: these functions are not implemented in Baseline
 */

FPXStatus FPX_SetPath(
              FPXImageHandle *theFPX,
              unsigned short  thePathIndex,
              FPXPath*        thePath)
{
  FPXImageHandle *handle = theFPX; 
  short     p = thePathIndex;
  FPXPath*  path= thePath; 
  return FPX_UNIMPLEMENTED_FUNCTION;
}

FPXStatus FPX_GetPath(
              FPXImageHandle *theFPX,
              unsigned short  thePathIndex,
              FPXPath**       thePath)
{
  FPXImageHandle *handle = theFPX; 
  short  p = thePathIndex;
  FPXPath**   path= thePath; 
  return FPX_UNIMPLEMENTED_FUNCTION;
}

/********************************************************************************
   IMAGE WITH VIEW
 *******************************************************************************/

FPXStatus FPX_SetImageROI (
              FPXImageHandle*  theFPX,
              FPXROI*          theROI)
{
  
  FPXStatus status;
  if( ( status = theFPX->SetImageROI(theROI)) == FPX_OK)
    status = theFPX->SaveImageROI();
  return status;  
  
  //return theFPX->SetImageROI(theROI);
}
        
FPXStatus FPX_GetImageROI (
              FPXImageHandle*  theFPX,
              FPXROI*          theROI)
{
  return theFPX->GetImageROI(theROI);
}
        
FPXStatus FPX_SetImageFilteringValue (
              FPXImageHandle*    theFPX,
              FPXFilteringValue* theFiltering)
{
  FPXStatus status;
  if( ( status = theFPX->SetImageFilteringValue(theFiltering)) == FPX_OK)
    status = theFPX->SaveImageFilteringValue();
  return status;  
  
}
        
FPXStatus FPX_GetImageFilteringValue (
              FPXImageHandle*     theFPX,
              FPXFilteringValue*  theFiltering)
{
  return theFPX->GetImageFilteringValue(theFiltering);
}
        
FPXStatus FPX_SetImageAffineMatrix (
              FPXImageHandle*   theFPX,
              FPXAffineMatrix*  theAffineMatrix)
{
  FPXStatus status;
  if( ( status = theFPX->SetImageAffineMatrix(theAffineMatrix)) == FPX_OK)
    status = theFPX->SaveImageAffineMatrix();
  return status;  
}
        
FPXStatus FPX_GetImageAffineMatrix (
              FPXImageHandle*   theFPX,
              FPXAffineMatrix*  theAffineMatrix)
{
  return theFPX->GetImageAffineMatrix(theAffineMatrix);
}
        
FPXStatus FPX_SetImageResultAspectRatio (
              FPXImageHandle*       theFPX,
              FPXResultAspectRatio* theAspectRatio)
{
/**
  FPXStatus status;
  if( ( status = theFPX->SetImageResultAspectRatio(theAspectRatio)) == FPX_OK)
    status = theFPX->SaveImageResultAspectRatio();
  return status;  
**/
  return theFPX->SetImageResultAspectRatio(theAspectRatio);
}

FPXStatus FPX_GetImageResultAspectRatio (
              FPXImageHandle*        theFPX,
              FPXResultAspectRatio*  theAspectRatio)
{
  return theFPX->GetImageResultAspectRatio(theAspectRatio);
}
        
FPXStatus FPX_SetImageColorTwistMatrix (
              FPXImageHandle*       theFPX,
              FPXColorTwistMatrix*  theColorTwistMatrix)
{
  FPXStatus status;
  if( ( status = theFPX->SetImageColorTwistMatrix(theColorTwistMatrix)) == FPX_OK)
    status = theFPX->SaveImageColorTwistMatrix();
  return status;
}
        
FPXStatus FPX_GetImageColorTwistMatrix (
              FPXImageHandle*       theFPX,
              FPXColorTwistMatrix*  theColorTwistMatrix)
{
  return theFPX->GetImageColorTwistMatrix(theColorTwistMatrix);
}
        
FPXStatus FPX_SetImageContrastAdjustment (
              FPXImageHandle*        theFPX,
              FPXContrastAdjustment* theContrastAdjustment)
{
  FPXStatus status;
  if( ( status = theFPX->SetImageContrastAdjustment(theContrastAdjustment)) == FPX_OK)
    status = theFPX->SaveImageContrastAdjustment();
  return status;
}
        
FPXStatus FPX_GetImageContrastAdjustment (
              FPXImageHandle*         theFPX,
              FPXContrastAdjustment*  theContrastAdjustment)
{
  return theFPX->GetImageContrastAdjustment(theContrastAdjustment);
}

FPXStatus FPX_SetSummaryInformation(
              FPXImageHandle *        theFPX,
              FPXSummaryInformation * theSummary)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr = theFPX->GetCurrentFile();

  if (filePtr) {
    OLEProperty* aProp;
    
    if (theSummary->title_valid)
      if (filePtr->SetSummaryInfoProperty (PID_TITLE, TYP_TITLE, &aProp))
        *aProp = (FPXStr)(theSummary->title);

    if (theSummary->subject_valid)
      if (filePtr->SetSummaryInfoProperty (PID_SUBJECT, TYP_SUBJECT, &aProp))
        *aProp = (FPXStr)(theSummary->subject);

    if (theSummary->author_valid)
      if (filePtr->SetSummaryInfoProperty (PID_AUTHOR, TYP_AUTHOR, &aProp))
        *aProp = (FPXStr)theSummary->author;

    if (theSummary->keywords_valid)
      if (filePtr->SetSummaryInfoProperty (PID_KEYWORDS, TYP_KEYWORDS, &aProp))
        *aProp = (FPXStr)theSummary->keywords; 

    if (theSummary->comments_valid)
      if (filePtr->SetSummaryInfoProperty (PID_COMMENTS, TYP_COMMENTS, &aProp))
        *aProp = (FPXStr)theSummary->comments;

    if (theSummary->OLEtemplate_valid)
      if (filePtr->SetSummaryInfoProperty (PID_TEMPLATE, TYP_TEMPLATE, &aProp))
        *aProp = (FPXStr)(theSummary->OLEtemplate);

    if (theSummary->last_author_valid)
      if (filePtr->SetSummaryInfoProperty (PID_LASTAUTHOR, TYP_LASTAUTHOR, &aProp))
        *aProp = (FPXStr)(theSummary->last_author);

    if (theSummary->rev_number_valid)
      if (filePtr->SetSummaryInfoProperty (PID_REV_NUMBER, TYP_REV_NUMBER, &aProp))
        *aProp = (FPXStr)(theSummary->rev_number);

    if (theSummary->edit_time_valid)
      if (filePtr->SetSummaryInfoProperty (PID_EDITTIME, TYP_EDITTIME, &aProp))
        *aProp = theSummary->edit_time;

    if (theSummary->last_printed_valid)
      if (filePtr->SetSummaryInfoProperty (PID_LASTPRINTED, TYP_LASTPRINTED, &aProp))
        *aProp = theSummary->last_printed;

    if (theSummary->create_dtm_valid)
      if (filePtr->SetSummaryInfoProperty (PID_CREATE_DTM, TYP_CREATE_DTM, &aProp))
        *aProp = theSummary->create_dtm;

    if (theSummary->last_save_dtm_valid)
      if (filePtr->SetSummaryInfoProperty (PID_LASTSAVE_DTM, TYP_LASTSAVE_DTM, &aProp))
        *aProp = theSummary->last_save_dtm;

    if (theSummary->page_count_valid)
      if (filePtr->SetSummaryInfoProperty (PID_PAGECOUNT, TYP_PAGECOUNT, &aProp))
        *aProp = (long)(theSummary->page_count);

    if (theSummary->word_count_valid)
      if (filePtr->SetSummaryInfoProperty (PID_WORDCOUNT, TYP_WORDCOUNT, &aProp))
        *aProp = (long)(theSummary->word_count);

    if (theSummary->char_count_valid)
      if (filePtr->SetSummaryInfoProperty (PID_CHARCOUNT, TYP_CHARCOUNT, &aProp))
        *aProp = (long)(theSummary->char_count);

    if (theSummary->thumbnail_valid)
      if (filePtr->SetSummaryInfoProperty (PID_THUMBNAIL, TYP_THUMBNAIL, &aProp))
        *aProp = (FPXThumbnail *)&(theSummary->thumbnail);

    if (theSummary->appname_valid)
      if (filePtr->SetSummaryInfoProperty (PID_APPNAME, TYP_APPNAME, &aProp))
        *aProp = (FPXStr)(theSummary->appname);

    if (theSummary->security_valid)
      if (filePtr->SetSummaryInfoProperty (PID_SECURITY, TYP_SECURITY, &aProp))
        *aProp = (long)(theSummary->security);

    filePtr->Commit();
  } else
    status = FPX_INVALID_FPX_HANDLE;
  
  return status;
}

FPXStatus FPX_GetSummaryInformation(
              FPXImageHandle *        theFPX,
              FPXSummaryInformation * theSummary)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr = theFPX->GetCurrentFile();
  
  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetSummaryInfoProperty (PID_TITLE, &aProp)) {
      theSummary->title = (FPXStr)(*aProp);
      theSummary->title_valid = TRUE;
    } else
      theSummary->title_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_SUBJECT, &aProp)) {
      theSummary->subject = (FPXStr)(*aProp);
      theSummary->subject_valid = TRUE;
    } else
      theSummary->subject_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_AUTHOR, &aProp)) {
      theSummary->author = (FPXStr)(*aProp);
      theSummary->author_valid = TRUE;
    } else
      theSummary->author_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_KEYWORDS, &aProp)) {
      theSummary->keywords = (FPXStr)(*aProp);
      theSummary->keywords_valid = TRUE;
    } else
      theSummary->keywords_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_COMMENTS, &aProp)) {
      theSummary->comments = (FPXStr)(*aProp);
      theSummary->comments_valid = TRUE;
    } else
      theSummary->comments_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_TEMPLATE, &aProp)) {
      theSummary->OLEtemplate = (FPXStr)(*aProp);
      theSummary->OLEtemplate_valid = TRUE;
    } else
      theSummary->OLEtemplate_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_LASTAUTHOR, &aProp)) {
      theSummary->last_author = (FPXStr)(*aProp);
      theSummary->last_author_valid = TRUE;
    } else
      theSummary->last_author_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_REV_NUMBER, &aProp)) {
      theSummary->rev_number = (FPXStr)(*aProp);
      theSummary->rev_number_valid = TRUE;
    } else
      theSummary->rev_number_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_EDITTIME, &aProp)) {
      theSummary->edit_time = (FPXfiletime)(*aProp);
      theSummary->edit_time_valid = TRUE;
    } else
      theSummary->edit_time_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_LASTPRINTED, &aProp)) {
      theSummary->last_printed = (FPXfiletime)(*aProp);
      theSummary->last_printed_valid = TRUE;
    } else
      theSummary->last_printed_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_CREATE_DTM, &aProp)) {
      theSummary->create_dtm = (FPXfiletime)(*aProp);
      theSummary->create_dtm_valid = TRUE;
    } else
      theSummary->create_dtm_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_LASTSAVE_DTM, &aProp)) {
      theSummary->last_save_dtm = (FPXfiletime)(*aProp);
      theSummary->last_save_dtm_valid = TRUE;
    } else
      theSummary->last_save_dtm_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_PAGECOUNT, &aProp)) {
      theSummary->page_count = (long)(*aProp);
      theSummary->page_count_valid = TRUE;
    } else
      theSummary->page_count_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_WORDCOUNT, &aProp)) {
      theSummary->word_count = (long)(*aProp);
      theSummary->word_count_valid = TRUE;
    } else
      theSummary->word_count_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_CHARCOUNT, &aProp)) {
      theSummary->char_count = (long)(*aProp);
      theSummary->char_count_valid = TRUE;
    } else
      theSummary->char_count_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_THUMBNAIL, &aProp)) {
      theSummary->thumbnail = *((FPXThumbnail *)(*aProp));
      theSummary->thumbnail_valid = TRUE;
    } else
      theSummary->thumbnail_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_APPNAME, &aProp)) {
      theSummary->appname = (FPXStr)(*aProp);
      theSummary->appname_valid = TRUE;
    } else
      theSummary->appname_valid = FALSE;

    if (filePtr->GetSummaryInfoProperty (PID_SECURITY, &aProp)) {
      theSummary->security = (long)(*aProp);
      theSummary->security_valid = TRUE;
    } else
      theSummary->security_valid = FALSE;

  }
  
  return status;
}

// CHG_GBLINFO - added call to return a struct containing data from the Global
//  Info property set
FPXStatus FPX_GetGlobalInformation(
              FPXImageHandle *        theFPX,
              FPXGlobalInformation *  theGlobalInfo)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr = theFPX->GetCurrentFile();
  
  if (filePtr) {
    OLEProperty* aProp;
    
    if (filePtr->GetGlobalInfoProperty (PID_VisibleOutputs, &aProp)) {
      theGlobalInfo->visible_outputs = (FPXLongArray)(*aProp);
      theGlobalInfo->visible_outputs_valid = TRUE;
    } else
      theGlobalInfo->visible_outputs_valid = FALSE;
    
    if (filePtr->GetGlobalInfoProperty (PID_MaxImageIndex, &aProp)) {
      theGlobalInfo->max_image_index = (unsigned long)(*aProp);
      theGlobalInfo->max_image_index_valid = TRUE;
    } else
      theGlobalInfo->max_image_index_valid = FALSE;
    
    if (filePtr->GetGlobalInfoProperty (PID_MaxTransfoIndex, &aProp)) {
      theGlobalInfo->max_transform_index = (unsigned long)(*aProp);
      theGlobalInfo->max_transform_index_valid = TRUE;
    } else
      theGlobalInfo->max_transform_index_valid = FALSE;
    
    if (filePtr->GetGlobalInfoProperty (PID_MaxOperatorIndex, &aProp)) {
      theGlobalInfo->max_operation_index = (unsigned long)(*aProp);
      theGlobalInfo->max_operation_index_valid = TRUE;
    } else
      theGlobalInfo->max_operation_index_valid = FALSE;
  } 
  return status;
}


/****************************************************************************/
/* EXTENSION LIST MANAGEMENT
/****************************************************************************/

FPXStatus FPX_GetExtensionDescription (
              FPXImageHandle*             theFPX,
              LPWSTR                      extensionName,
              FPXExtensionDescription*    theDescription)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr =  (PFileFlashPixView *)(theFPX->GetCurrentFile());
  
  if (filePtr) {
    OLEProperty* aProp;
    short extensionNumber; 
    
    // Get the extension number matching the extension name, return false if fails
    if ( filePtr->GetExtensionNumber(extensionName, &extensionNumber) ) {
    
      if (filePtr->GetExtensionProperty (PID_ExtensionClassID(extensionNumber), &aProp)) 
        theDescription->extensionClassID = *(CLSID*)(*aProp);
      
      if (filePtr->GetExtensionProperty (PID_ExtensionPersistence(extensionNumber), &aProp)) 
        theDescription->extensionPersistence = (FPXExtensionPersistence)(WORD)(*aProp);
      
      if (filePtr->GetExtensionProperty (PID_ExtensionCreationDate(extensionNumber), &aProp)) {
        theDescription->extensionCreationDate = (FILETIME)(*aProp);
        theDescription->extensionCreationDateIsValid = TRUE;
      } else
        theDescription->extensionCreationDateIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_ExtensionModificationDate(extensionNumber), &aProp)) {
        theDescription->extensionModificationDate = (FILETIME)(*aProp);
        theDescription->extensionModificationDateIsValid = TRUE;
      } else
        theDescription->extensionModificationDateIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_CreateApplication(extensionNumber), &aProp)) {
        theDescription->creatingApplication = (FPXWideStr)(*aProp);
        theDescription->creatingApplicationIsValid = TRUE;
      } else
        theDescription->creatingApplicationIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_ExtensionDescription(extensionNumber), &aProp)) {
        theDescription->extensionDescription = (FPXWideStr)(*aProp);
        theDescription->extensionDescriptionIsValid = TRUE;
      } else
        theDescription->extensionDescriptionIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_StorageStreamPathName(extensionNumber), &aProp)) {
        theDescription->streamPathName = (FPXWideStrArray)(*aProp);
        theDescription->streamPathNameIsValid = TRUE;
      } else
        theDescription->streamPathNameIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_FPXStreamPathName(extensionNumber), &aProp)) {
        theDescription->fpxStreamPathName = (FPXWideStrArray)(*aProp);
        theDescription->fpxStreamPathNameIsValid = TRUE;
      } else
        theDescription->fpxStreamPathNameIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_FPXStreamFieldOffset(extensionNumber), &aProp)) {
        theDescription->fpxStreamOffset = (FPXLongArray)(*aProp);
        theDescription->fpxStreamOffsetIsValid = TRUE;
      } else
        theDescription->fpxStreamOffsetIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_PropertySetPathName(extensionNumber), &aProp)) {
        theDescription->propertySetPathName = (FPXWideStrArray)(*aProp);
        theDescription->propertySetPathNameIsValid = TRUE;
      } else
        theDescription->propertySetPathNameIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_PropertySetFormatID(extensionNumber), &aProp)) {
        theDescription->propertySetFormatID = (FPXClsIDArray)(*aProp);
        theDescription->propertySetFormatIDIsValid = TRUE;
      } else
        theDescription->propertySetFormatIDIsValid = FALSE;
        
      if (filePtr->GetExtensionProperty (PID_PropertySetIDCodes(extensionNumber), &aProp)) {
        theDescription->propertySetIDCodes = (FPXWideStrArray)(*aProp);
        theDescription->propertySetIDCodesIsValid = TRUE;
      } else
        theDescription->propertySetIDCodesIsValid = FALSE;
        
    }
    else
      return FPX_EXTENSION_FAILED;    
  }
  else
    return FPX_INVALID_FPX_HANDLE; 

  
  return status;
}

FPXStatus FPX_SetExtensionDescription (
              FPXImageHandle*             theFPX,
              LPWSTR                        extensionName,
              FPXExtensionDescription*    theDescription)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr =  (PFileFlashPixView *)(theFPX->GetCurrentFile());
  
  if (filePtr) {
    OLEProperty* aProp;
    short extensionNumber; 
    
    // Get the extension number matching the extension name, return false if fails
    if ( filePtr->GetExtensionNumber(extensionName, &extensionNumber) ) {
    
      if (filePtr->SetExtensionProperty (PID_ExtensionClassID(extensionNumber), TYP_ExtensionClassID, &aProp)) 
        *aProp = &theDescription->extensionClassID;
      
      if (filePtr->SetExtensionProperty (PID_ExtensionPersistence(extensionNumber), TYP_ExtensionPersistence, &aProp)) 
        *aProp = (WORD)theDescription->extensionPersistence;
      
      if ( theDescription->extensionCreationDateIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionCreationDate(extensionNumber), TYP_ExtensionCreationDate, &aProp)) 
          *aProp = (FPXfiletime)theDescription->extensionCreationDate;
      
      if ( theDescription->extensionModificationDateIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionModificationDate(extensionNumber), TYP_ExtensionModificationDate, &aProp)) 
          *aProp = (FPXfiletime)theDescription->extensionModificationDate;
      
      if ( theDescription->creatingApplicationIsValid )
        if (filePtr->SetExtensionProperty (PID_CreateApplication(extensionNumber), TYP_CreateApplication, &aProp)) 
          *aProp = (FPXWideStr)theDescription->creatingApplication;
      
      if ( theDescription->extensionDescriptionIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionDescription(extensionNumber), TYP_ExtensionDescription, &aProp)) 
          *aProp = (FPXWideStr)theDescription->extensionDescription;
      
      if ( theDescription->streamPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_StorageStreamPathName(extensionNumber),TYP_StorageStreamPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->streamPathName;
      
      if ( theDescription->fpxStreamPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_FPXStreamPathName(extensionNumber), TYP_FPXStreamPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->fpxStreamPathName;
      
      if ( theDescription->fpxStreamOffsetIsValid )
        if (filePtr->SetExtensionProperty (PID_FPXStreamFieldOffset(extensionNumber), TYP_FPXStreamFieldOffset, &aProp)) 
          *aProp = (FPXLongArray)theDescription->fpxStreamOffset;
      
      if ( theDescription->propertySetPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetPathName(extensionNumber), TYP_PropertySetPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->propertySetPathName;
      
      if ( theDescription->propertySetFormatIDIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetFormatID(extensionNumber), TYP_PropertySetFormatID, &aProp)) 
          *aProp = (FPXClsIDArray)theDescription->propertySetFormatID;
      
      if ( theDescription->propertySetIDCodesIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetIDCodes(extensionNumber), TYP_PropertySetIDCodes, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->propertySetIDCodes;
      
    }
    else if ( filePtr->SetExtensionNumber(&extensionNumber) ) {
    
      if (filePtr->SetExtensionProperty (PID_ExtensionClassID(extensionNumber), TYP_ExtensionClassID, &aProp)) 
        *aProp = &theDescription->extensionClassID;
      
      if (filePtr->SetExtensionProperty (PID_ExtensionPersistence(extensionNumber), TYP_ExtensionPersistence, &aProp)) 
        *aProp = (WORD)theDescription->extensionPersistence;
      
      if ( theDescription->extensionCreationDateIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionCreationDate(extensionNumber), TYP_ExtensionCreationDate, &aProp)) 
          *aProp = (FPXfiletime)theDescription->extensionCreationDate;
      
      if ( theDescription->extensionModificationDateIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionModificationDate(extensionNumber), TYP_ExtensionModificationDate, &aProp)) 
          *aProp = (FPXfiletime)theDescription->extensionModificationDate;
      
      if ( theDescription->creatingApplicationIsValid )
        if (filePtr->SetExtensionProperty (PID_CreateApplication(extensionNumber), TYP_CreateApplication, &aProp)) 
          *aProp = (FPXWideStr)theDescription->creatingApplication;
      
      if ( theDescription->extensionDescriptionIsValid )
        if (filePtr->SetExtensionProperty (PID_ExtensionDescription(extensionNumber), TYP_ExtensionDescription, &aProp)) 
          *aProp = (FPXWideStr)theDescription->extensionDescription;
      
      if ( theDescription->streamPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_StorageStreamPathName(extensionNumber),TYP_StorageStreamPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->streamPathName;
      
      if ( theDescription->fpxStreamPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_FPXStreamPathName(extensionNumber), TYP_FPXStreamPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->fpxStreamPathName;
      
      if ( theDescription->fpxStreamOffsetIsValid )
        if (filePtr->SetExtensionProperty (PID_FPXStreamFieldOffset(extensionNumber), TYP_FPXStreamFieldOffset, &aProp)) 
          *aProp = (FPXLongArray)theDescription->fpxStreamOffset;
      
      if ( theDescription->propertySetPathNameIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetPathName(extensionNumber), TYP_PropertySetPathName, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->propertySetPathName;
      
      if ( theDescription->propertySetFormatIDIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetFormatID(extensionNumber), TYP_PropertySetFormatID, &aProp)) 
          *aProp = (FPXClsIDArray)theDescription->propertySetFormatID;
      
      if ( theDescription->propertySetIDCodesIsValid )
        if (filePtr->SetExtensionProperty (PID_PropertySetIDCodes(extensionNumber), TYP_PropertySetIDCodes, &aProp)) 
          *aProp = (FPXWideStrArray)theDescription->propertySetIDCodes;
      
    }
    else  
      return FPX_EXTENSION_FAILED;    
  }
  else
    return FPX_INVALID_FPX_HANDLE;
    
  return status;
}

FPXStatus FPX_GetStreamPointer (
              FPXImageHandle*             theFPX,
              char*                       streamName,
              IStream**                   oleStream)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr =  (PFileFlashPixView *)(theFPX->GetCurrentFile());
  
  if (filePtr) {
    OLEStream * currentStream;
    long    streamLength; 
    
    OLEStorage * rootStorage = filePtr->GetRootStorage(); 
    if ( rootStorage ) {
      if ( rootStorage->OpenStream(streamName, &currentStream, OLE_READWRITE_MODE) ) {      
        currentStream->GetEndOfFile(&streamLength); 
        currentStream->CopyTo(*oleStream, streamLength);
      }
      else
        status = FPX_EXTENSION_FAILED;      
    } 
    else
      status = FPX_EXTENSION_FAILED;                
  }
  else
    status = FPX_INVALID_FPX_HANDLE;      
  
  return status;
}

FPXStatus FPX_GetStoragePointer (
              FPXImageHandle*             theFPX,
              char*                       storageName,
              IStorage**                  oleStorage)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr =  (PFileFlashPixView *)(theFPX->GetCurrentFile());
  
  if (filePtr) {
    OLEStorage * currentStorage;
    
    OLEStorage * rootStorage = filePtr->GetRootStorage(); 
    if ( rootStorage ) {
      if ( rootStorage->OpenStorage(storageName, &currentStorage, OLE_READWRITE_MODE) ) {     
        currentStorage->CopyTo(*oleStorage);
      }
      else
        status = FPX_EXTENSION_FAILED;      
    } 
    else
      status = FPX_EXTENSION_FAILED;                
  }
  else
    status = FPX_INVALID_FPX_HANDLE;      
  
  return status;
}

FPXStatus FPX_GetPropertySetPointer (
              FPXImageHandle*             theFPX,
              char*                       propertySetName,
              IStream**                   olePropertySet)
{
  FPXStatus status = FPX_OK;

  if (!theFPX)  {
    status = FPX_INVALID_FPX_HANDLE;
    return status;
  }
  
  PFileFlashPixView* filePtr =  (PFileFlashPixView *)(theFPX->GetCurrentFile());
  
  if (filePtr) {
    OLEPropertySet * currentPropertySet;
    long    propertySetLength; 
    
    OLEStorage * rootStorage = filePtr->GetRootStorage(); 
    if ( rootStorage ) {
      if ( rootStorage->OpenPropertySet(propertySetName, &currentPropertySet, OLE_READWRITE_MODE) ) {     
        currentPropertySet->GetEndOfFile(&propertySetLength); 
        currentPropertySet->CopyTo(*olePropertySet, propertySetLength);
      }
      else
        status = FPX_EXTENSION_FAILED;      
    } 
    else
      status = FPX_EXTENSION_FAILED;                
  }
  else
    status = FPX_INVALID_FPX_HANDLE;      
  
  return status;
}

//  - EOF ----------------------------------------------------------------------
