/*
 *  FPXLib.h
 *
 *  FlashPix Library API
 */

/****************************************************************************/
#ifndef FPXLibAPI_h
#define FPXLibAPI_h
/****************************************************************************/

	typedef struct _XSTREAM
	{
    long streampointer;
	} XSTREAM;
	typedef XSTREAM IStream;

	typedef struct _XGUID
	{
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];
	} XGUID;
	//typedef XGUID CLSID;
  #define CLSID XGUID

	#ifndef FARSTRUCT
		#define FARSTRUCT
	#endif
	#ifndef __RPC_FAR
		#define __RPC_FAR
	#endif
	#ifndef BYTE
		#define	BYTE unsigned char	
	#endif
	#ifndef ULONG
		#define	ULONG unsigned long
	#endif

  typedef struct  tagCLIPDATA {
		ULONG cbSize;
		long ulClipFmt;
		BYTE __RPC_FAR *pClipData;
	}	CLIPDATA;

#ifndef WCHAR
	typedef unsigned short WCHAR; 
	typedef WCHAR * LPWSTR; 
#endif

/***************************************************************************
   TOOLKIT HANDLE
 ***************************************************************************/

typedef struct _FPXImageHandle {
  long handle;
} FPXImageHandle;


/****************************************************************************
  BASIC TYPEDEFS
 ****************************************************************************/

typedef unsigned char   FPXbool;

typedef struct {
              unsigned long     length;        /* number of chars */
              unsigned char  	  * ptr;
} FPXStr;

typedef struct {
              unsigned long     length;        /* number of shorts */
              unsigned short    * ptr;
} FPXShortArray;

typedef struct {
              unsigned long     length;        /* number of longs */
              unsigned long     * ptr;
} FPXLongArray;

typedef struct {
              unsigned long     length;        /* number of reals */
              float        		  * ptr;
} FPXRealArray;

typedef struct FPXWideStr {
              unsigned long     length;        /* length of string off ptr in bytes */
              unsigned short    * ptr;
} FPXWideStr;

typedef struct {
              unsigned long     length;        /* number of strings */
              FPXStr         	  * ptr;
} FPXStrArray;

typedef struct {
              unsigned long     length;        /* number of wide strings */
              FPXWideStr     	  * ptr;
} FPXWideStrArray;

typedef struct {
              unsigned long     length;        /* number of strings */
              CLSID         	  * ptr;
} FPXClsIDArray;

#ifndef _FILETIME_
#define _FILETIME_
typedef struct FARSTRUCT tagFILETIME
{
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
} FILETIME;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef FILETIME FPXfiletime;                  	   /* cf. OLE FILETIME in compobj.h */

#ifndef DATE 
typedef  double DATE; 
#endif

/****************************************************************************
  TOOLKIT ERROR CODES
 ****************************************************************************/

typedef enum {
  FPX_OK = 0,
  FPX_INVALID_FORMAT_ERROR = 1,
  FPX_FILE_WRITE_ERROR = 2,
  FPX_FILE_READ_ERROR = 3,
  FPX_FILE_NOT_FOUND = 4,
  FPX_COLOR_CONVERSION_ERROR = 5,
  FPX_SEVER_INIT_ERROR = 6,
  FPX_LOW_MEMORY_ERROR = 7,
  FPX_IMAGE_TOO_BIG_ERROR = 8,
  FPX_INVALID_COMPRESSION_ERROR = 9,
  FPX_INVALID_RESOLUTION = 10,
  FPX_INVALID_FPX_HANDLE = 11,
  FPX_TOO_MANY_LINES = 12,
  FPX_BAD_COORDINATES = 13,
  FPX_FILE_SYSTEM_FULL = 14,
  FPX_MISSING_TABLE = 15,
  FPX_RETURN_PARAMETER_TOO_LARGE = 16,
  FPX_NOT_A_VIEW = 17,
  FPX_VIEW_IS_TRANFORMLESS = 18,
  FPX_ERROR = 19,
  FPX_UNIMPLEMENTED_FUNCTION = 20,
  FPX_INVALID_IMAGE_DESC = 21,
  FPX_INVALID_JPEG_TABLE = 22,
  FPX_ILLEGAL_JPEG_ID = 23,
  FPX_MEMORY_ALLOCATION_FAILED = 24,
  FPX_NO_MEMORY_MANAGEMENT = 25,
  FPX_OBJECT_CREATION_FAILED = 26,
  FPX_EXTENSION_FAILED = 27,
  FPX_FREE_NULL_PTR = 28,
  FPX_INVALID_TILE = 29,
  FPX_FILE_IN_USE = 30,
  FPX_FILE_CREATE_ERROR = 31,
  FPX_FILE_NOT_OPEN_ERROR = 32,
  FPX_USER_ABORT = 33,
  FPX_OLE_FILE_ERROR = 34,

  // Numbers above 1000 are reserved for warnings. When returning a warning, the
  // function did something (default behavior) and it's safe for the application
  // to move on. It's up to the application to display a warning message to the user.
  FPX_W_COORDINATES_OUT_OF_RANGE = 1000
} FPXStatus;

#define ERRSTR_TBLSIZE	28

FPXStatus FPX_GetErrorString (FPXStatus errorCode, 
                              char *errorString,
                              unsigned short maxStrLen);

/*
 * Some C functions to ease the usage of these structures.
 */
 void FPXUpdateTime (FPXfiletime* theFPXTime);
 void InitFPXStr            (FPXStr*          theFPXArray);
 void InitFPXShortArray     (FPXShortArray*   theFPXArray);
 void InitFPXLongArray      (FPXLongArray*    theFPXArray); 
 void InitFPXRealArray      (FPXRealArray*    theFPXArray);
 void InitFPXWideStr        (FPXWideStr*      theFPXArray);
 void InitFPXWideStrArray   (FPXWideStrArray* theFPXArray);
 void InitFPXStrArray       (FPXStrArray*     theFPXArray);

 FPXStatus FPX_Strcpy     (FPXStr*     theFPXStr, const char* string);
 FPXStatus FPX_WideStrcpy (FPXWideStr* theFPXStr, const char* string);
 long      FPX_WideStrcmp (FPXWideStr* fpxWStr1,  FPXWideStr* fpxWStr2);
 
 long      FPX_LPWSTRlen (LPWSTR wideStr);

 FPXStatus FPX_DeleteFPXStr          (FPXStr*          theFPXArray);
 FPXStatus FPX_DeleteFPXShortArray   (FPXShortArray*   theFPXArray);
 FPXStatus FPX_DeleteFPXLongArray    (FPXLongArray*    theFPXArray);
 FPXStatus FPX_DeleteFPXRealArray    (FPXRealArray*    theFPXArray); 
 FPXStatus FPX_DeleteFPXWideStr      (FPXWideStr*      theFPXArray);
 FPXStatus FPX_DeleteFPXWideStrArray (FPXWideStrArray* theFPXArray);
 FPXStatus FPX_DeleteFPXStrArray     (FPXStrArray*     theFPXArray);

 FPXStatus FPX_AllocFPXStr           (FPXStr*          theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXShortArray    (FPXShortArray*   theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXLongArray     (FPXLongArray*    theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXRealArray     (FPXRealArray*    theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXWideStr       (FPXWideStr*      theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXWideStrArray  (FPXWideStrArray* theFPXArray, long nbElem);
 FPXStatus FPX_AllocFPXStrArray      (FPXStrArray*     theFPXArray, long nbElem);

/****************************************************************************
   TOOLKIT COMPRESSION CONTROL
 ***************************************************************************/

/* Desired compression style                                                         */
typedef enum {
              NONE                = 0,    /* no compression used.                    */
              SINGLE_COLOR        = 1,    /* use single color compression            */
              JPEG_UNSPECIFIED    = 2,    /* let the toolkit pick the tables.        */
              JPEG_BY_QUALITY     = 3,    /* we will specify the quality.            */
              JPEG_BY_TABLE_GROUP = 4     /* we will specify the table group to use  */
} FPXCompressionOption;


/*  A JPEG quantizer table
    XXX Is it bigendian or little endian? JPEG byte order or host machine?
    Probably should be in JPEG format.
    Rem: unsigned short quantizer is large enough for non-baseline JPEG.
    (16bit lossless)
*/

typedef struct {
    unsigned char *bits;
    unsigned char *vals;
    unsigned char hclass; /* 0: DC, 1: AC */
    unsigned char ident;  /* 0,1,2,or 3 for extended JPEG systems */
                          /* 0, or 1 for baseline JPEG */
} FPXJPEGHUFFTable;

typedef struct {
    unsigned char *quantizer; 	/* Table elements specified in zigzag order */
    unsigned char ident;  		/* 0,1,2,3 */
} FPXJPEGQuantTable;


/*  A JPEG abbreviated stream for table info.
    XXX we will fill in the details later. It's basically a bunch of 
    overhead, bytes counts, Q tables, and H tables.
 */

#define FPX_MAX_TABLE_STREAM_SIZE   1400

typedef struct {
			  unsigned short  theStreamSize;
              unsigned char   theStream[FPX_MAX_TABLE_STREAM_SIZE];
} FPXJPEGTableGroup;


/*  Make a FPXJPEGTableGroup from a bunch of Q and H tables.            */
/*  Not all tables need be supplied. A NULL indicates such an omission. */

FPXStatus FPX_CreateJPEGTableGroup (
              FPXJPEGTableGroup*  	theGroup,
              short 				numOfQuanTable,
              unsigned char *		quanTableChanID,
              FPXJPEGQuantTable*  	quanTable,
              short 				numOfHuffTable,
              unsigned char *		huffDCTableChanID,
              unsigned char *		huffACTableChanID,
              FPXJPEGHUFFTable*   	huffTable ); 

/***************************************************************************
   TOOLKIT RESOLUTION CONTROL
 ***************************************************************************/

/*  Number of levels in the pyramid and the compression used for each.
 *  This allows user control of the compression/quality on a per resolution
 *  basis during FPX file creation.
 */
#define FPXMAXRESOLUTIONS 29    /* 8x8 pixel tiles, 2^32 x 2^32 image */

/*  Control the compression on a per resolution basis. */
typedef struct {
              FPXCompressionOption compressOption;
              unsigned char        compressQuality;
              unsigned char        compressTableGroup;
} FPXPerResolutionControl;

typedef struct {
              short                   numberOfResolutions;
              FPXPerResolutionControl compressionControl[FPXMAXRESOLUTIONS];
} FPXResolution;



/***************************************************************************
   TOOLKIT IMAGE COMPONENT COLORS/ORGANIZATIONS
 ***************************************************************************/

/*  Some FPX defined colors for components.
 *
 *  Whenever an alpha channel is seen, the other channels are assumed to
 *  be premultiplied by it.  When an opacity of 0 (transparent) is seen
 *  in the alpha channel, the toolkit will NOT automatically insert chroma
 *  channels that are equal to the known background color, when either 
 *      reading or writing FPX images.  This work is left to the application and 
 *  is not considered a function of the toolkit.
 */
typedef enum { PHOTO_YCC_Y,
               PHOTO_YCC_C1,
               PHOTO_YCC_C2,
               NIFRGB_R,
               NIFRGB_G,
               NIFRGB_B,
               ALPHA,
               MONOCHROME
} FPXComponentColor;

/*  Some FPX defined/allowed data types for components.                                 */
typedef enum { DATA_TYPE_UNSIGNED_BYTE,
               DATA_TYPE_SIGNED_BYTE,
               DATA_TYPE_UNSIGNED_SHORT,
               DATA_TYPE_SIGNED_SHORT,
               DATA_TYPE_FLOAT,
               DATA_TYPE_DOUBLE
} FPXDataType;

/*  Define the color/datatype of a component.                                            */
typedef struct {
              FPXComponentColor myColor;      /* Y, C1, C2, R, B, G, or ALPHA            */
              FPXDataType       myDataType;   /* unsigned byte, signed short, etc        */
} FPXComponentColorType;

#define FPX_MAX_COMPONENTS 4

/*  Specify a color space by a collection of color/datatype values.
 *  NOTE: only certain combinations are allowed by the FPX spec.
 */
typedef struct FPXColorspace {
              FPXbool               isUncalibrated;
              short         		numberOfComponents;
              FPXComponentColorType theComponents[FPX_MAX_COMPONENTS];
} FPXColorspace;

/***************************************************************************
   TOOLKIT GLOBAL CONTROL
 ***************************************************************************/

/*  FPX_Delete() is needed in the special case that the calling program
 *  has to delete objects created by the toolkit.  In this case 
 *  a crash will occur unless the TOOLKIT calls delete.
 */
FPXStatus FPX_Delete(void *FPXObj);


/* Create an instance of the tool kit. The Tool kit can be used only if this
 * function is called prior any other call. 
 */
FPXStatus FPX_InitSystem ();

/* Delete the instance of the tool kit. The Tool kit cannot be used
 * after a call to this function has been made. 
 */
FPXStatus FPX_ClearSystem ();

/* Get tool kit name (here "Reference") and version number.
 */
FPXStatus FPX_GetToolkitVersion (
         char* versionName, 
         long* versionNumber);

/* Global options
 * --------------
 */

typedef enum {
              FPX_INCHES = 0,
              FPX_METERS,
              FPX_CENTIMETERS,
              FPX_MILLIMETERS
} FPXResolutionUnit;

typedef enum {
              FPX_NEAREST_NEIGHBOR = 0,
              FPX_LINEAR_INTERPOLATION
} FPXResampleMethod;

typedef enum {
              FPX_OVERWRITE_BACKGROUND = 0,
              FPX_PROTECT_BACKGROUND
} FPXComposeMethod;

/*  Background colors are used for for background compression.
 *  Values are in order of components of the color space specified
 *  for the FPX image.
 */

typedef struct {
              signed long color1_value;
              signed long color2_value;
              signed long color3_value;
              signed long color4_value;
} FPXBackground;

/* Set the unit used in resolution independent transactions 
 * in the viewing tools.
 */

FPXStatus FPX_SetUnit (
        FPXResolutionUnit  newUnit);
 
FPXStatus FPX_GetUnit (
         FPXResolutionUnit* newUnit);
         
/* Set the method used to compute an intermediate resolution.
 * (formerly FPX_SetAntialias())
 */

 FPXStatus FPX_SetResampleMethod (
         FPXResampleMethod method);

         
/* Set the method used when composing images in the viewing tools.
 * (formerly FPX_NoJaggies())
 */
 FPXStatus FPX_SetComposeMethod (
         FPXComposeMethod method);
         

/* Set the default color used when composing images in the viewing tools.
 * (formerly FPX_SetBackgroundColor())
 */

 FPXStatus FPX_SetViewBackgroundColor (
         FPXColorspace colorspace,
         FPXBackground color);
	

/***************************************************************************
   TOOLKIT MEMORY CONTROL
 ***************************************************************************/

/*  This function sets the amount of memory to be used by the FPX toolkit.
 *  This value does NOT include the amount that could also be used
 *  by the subordinate OLE layer. There is currently no way to limit
 *  OLE memory usage.
 *
 *  0 means use unlimited available.
 */
 
 FPXStatus FPX_SetToolkitMemoryLimit ( 
              unsigned long *    memoryLimit);
         
 FPXStatus FPX_GetToolkitMemoryLimit (
		 unsigned long * 	memoryLimit);
		 
 FPXStatus FPX_GetToolkitMemoryAvailable (
		 unsigned long * 	availableMemory);
		 
 FPXStatus FPX_GetToolkitMemoryUsed (
		 unsigned long * 	usedMemory);

// Memory management functions
// ---------------------------

/* Purge the Toolkit memory (basically, the cached tiles).
 * Return the amount of memory really purged
 */
 
 long FPX_PurgeToolkitMemory (
		 unsigned long      memoryToBePurged);

/* Lock a FPX image tiles to avoid having them purged 
 * during a FPX_PurgeToolkitMemory()
 */
 
 FPXStatus FPX_LockFPXImage (
		 FPXImageHandle*	theFPX);


/***************************************************************************
 *  FPX FILE CREATION
 ***************************************************************************/

/*  FPX_CreateImageByFilename() - create a new, empty, FPX image.
 *
 *  Input image must already be in output color space.
 *  I.e. if you want a YCC FPX, feed us YCC!
 */
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
              FPXImageHandle**     theFPX);
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
              FPXImageHandle**     theFPX);
#endif

/***************************************************************************
    HIERARCHY GENERATION AND FLAT IMAGES HANDLING routines
 ***************************************************************************/

/* No automatic decimation will be done by the Toolkit if this function is
 * called  and as long as the symetric function FPX_GenerateHierarchy()
 * is not called.
 * If a file is created, written and closed with this option set, the resulting
 * file will be a flat image (no hierarchy).
 */
 
 FPXStatus FPX_DoNotComputeHierarchy (
              FPXImageHandle*  theFPX);
              
/* When this function is called, the hierarchy is recomputed from bottom up, using
 * what is available from the high res up to the lowest res. This function unset
 * the "no compute" flag, thus allowing automatic decimation to occur on a file
 * when editing.
 */
 
 FPXStatus FPX_GenerateHierarchy (
              FPXImageHandle*  theFPX);

/***************************************************************************
    TOOLKIT SET/GET routines
 ***************************************************************************/

/*  FPX_GetResolutionInfo() - return the resolution pyramid info from the image.
 *
 *  After creating a new, empty image, use this to
 *  get info about the pyramid for later user (such as
 *  controlling compression on a per level basis).
 *  Error possible for NULL handle.
 */
 FPXStatus FPX_GetResolutionInfo (
              FPXImageHandle*  theFPX,
              FPXResolution*   theResolutionInfo);


/*  For a given image, set the pyramid info.
 *  Should only modify the compression option and quality/tablegroup for the
 *  pyramid layers.
 *
 *  Or set the number of levels to 1 (flatten it for non-hierarchical FPX).
 *  error return(for stuff that was diddled that shouldn't have been or bad
 *  values.)
 */
 FPXStatus FPX_SetResolutionInfo (
              FPXImageHandle*  theFPX,
              FPXResolution*   theResolutionInfo);

/***************************************************************************
    TOOLKIT PROGRESS CALLBACK
 ***************************************************************************/

/*  Typedef for the progress function used by FPX function calls which may
 *  take a while.  The two arguments passed to the progress function are
 *  indications of how much work total is to be done by the toolkit function
 *  call and how much of it is done at this  point.  The progress function
 *  can abort the toolkit function by returning a non-zero value.
 */
#ifndef tag_FPXProgressFunction
#define tag_FPXProgressFunction
typedef short (* FPXProgressFunction) (long totalToDo, long amountDoneSoFar);
#endif

/* Set the progressive function for the Toolkit. The function will be called
 * automatically whenever it's necesary.
 */
 
 FPXStatus FPX_SetProgressFunction (
		 FPXProgressFunction	theProgressive);

/***************************************************************************
    TOOLKIT FILE CLOSING
 ***************************************************************************/

/*  Close out a FlashPix image.
 *  Finish creation of the image and writing to file.
 *  May take a long time and so includes a progress callback.
 *  May error for lots of reasons.
 */

 FPXStatus FPX_CloseImage (
              FPXImageHandle*     theFPX);

/***************************************************************************
    TOOLKIT COLOR COMPONENTS
 ***************************************************************************/

/*  A struct to hold the component descriptor.
 *  Holds the color/datatype,
 *  subsampling factors,
 *  column and line strides,
 *  and pointer to the data.
 */
typedef struct {
              FPXComponentColorType myColorType;           /* the color and datatype      */
                                                           /* of this component.          */
              unsigned long         horzSubSampFactor; 	   /* horizontal subsampling  	  */
              unsigned long         vertSubSampFactor; 	   /* vertical subsampling    	  */
              long                  columnStride;          /* items to next column on 	  */
                                                           /* this row.                   */
              long                  lineStride;            /* items to next line in       */
                                                           /* this column.                */
              unsigned char*        theData;               /* maybe void * XXX?           */
} FPXImageComponentDesc;


/*  A struct to hold the image descriptor.
 *  Holds the number of components (channels) and
 *  their descriptors. NOTE that the components implicitly
 *  describe the colorspace.
 */
typedef struct FPXImageDesc {
              unsigned long         numberOfComponents;
              FPXImageComponentDesc components[FPX_MAX_COMPONENTS];
} FPXImageDesc;

/* In Baseline, channels are premultiplied by the alpha channel.
 *  However, using non premultiplied images is no big deal: just a
 *  bit to set in the color subfield. 
 *  These functions allow the handling of FPX with or without 
 *  premultiplication.
 *  CAUTION: 
 *     - if some tiles are already written, FPX_SetAlphaType 
 *     returns an error.
 *     - these functions are not implemented in Baseline
 */
typedef enum {
	PREMULTIPLIED_CHANNELS,
	INDEPENDENT_CHANNELS
} FPXPreComputedAlpha;

 FPXStatus FPX_SetAlphaType (
              FPXImageHandle*      theFPX,
              FPXPreComputedAlpha  theAlphaType);

 FPXStatus FPX_GetAlphaType (
              FPXImageHandle*      theFPX,
              FPXPreComputedAlpha* theAlphaType);

/*  provide a table group and assign an ID number to it.
 *  Provides user control over compression quality.
 *  ERROR return for NULL table, illegal/already used ID.
 */
 FPXStatus FPX_SetJPEGTableGroup (
              FPXImageHandle*     theFPX,
              FPXJPEGTableGroup*  theGroup,
              unsigned char       theTableGroupID);


/*  Get a given table group from a FPX image.
 *
 *  ERROR return for ID not valid.
 */
 FPXStatus FPX_GetJPEGTableGroup (
              FPXImageHandle*     theFPX,
              FPXJPEGTableGroup*  theGroup,
              unsigned char       theTableGroupID);


/*  specify the quant_ID's to be used for compression
 *
 *  A table is specified for the entire image, all levels.
 *  Error return if no such table.
 */
 FPXStatus FPX_SelectJPEGTableGroup (
              FPXImageHandle*  theFPX,
              unsigned char    theTableGroupID);


/*  Alternative JPEG table control:
 *  builds tables according to value of compressionFactor (a la JFIF)
 *  not allowed with other sets of huff or quant tables.
 */
 FPXStatus FPX_SetJPEGCompression (
              FPXImageHandle*  theFPX,
              unsigned short   theQualityFactor);    /* 0->100                              */

/*  Resolution decimation quality control:
 *
 *  Tell the toolkit how large a kernel to use.
 *  (Probably need to provide another call with the actual coefficients of
 *   a separable NxN kernel.)
 *
 *  Errors on bad handle, bad kernel size.
 */
 FPXStatus FPX_SetDecimationQuality (
              FPXImageHandle*  theFPX,
              unsigned short   decimationQuality);        /* perhaps one dimension        */
                                                          /* of decimation kernel.        */

/*  The following two calls are for writing rectangular regions
 *
 *  into the image. But this may be too general.
 *  Perhaps we should only allow rectangles that multiples of the
 *  tile size, or just one rectangle, or XXX ?
 *  If we do allow general rectangles, we must verify that there is
 *  no overlap!  XXX
 */

/*  Write a rectangle of data into the image.
 *
 *  Specify upper Left and Lower right pixel coords. of
 *  full sized image. Subsampled components must be handled accordingly.
 */
 FPXStatus FPX_WriteImageRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              FPXImageDesc*       theData);

/*  Write a rectangle of background color.  */
 FPXStatus FPX_WriteBackgroundRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              FPXColorspace       theColorspace,
              FPXBackground       theColor);

/*  Write a line of data into the image.
 *  May NOT be mixed with writing of rectangles.
 *
 *  Opening of image effectively starts the "iterator" for line
 *  writing at the top of the image.
 *  LINE of input data is in an FPXImageDesc with is presumed to
 *  be loaded with the line of data as the first (only?) line in the
 *  data.
 */
 FPXStatus FPX_WriteImageLine (
              FPXImageHandle*     theFPX,
              FPXImageDesc*       theLine);

/*  Write to specific resolution.
 *
 *  The caller will have decimated the image data external to the toolkit.
 *  Resolution 0 is the lowest res. (smallest)
 *  Resolution FPXResoltion.numberOfResolutions-1 is the highest
 *  (largest).
 *  Expected use is for the user to provide all data for the entire
 *  resultion pyramid, one call at a time.
 */
 FPXStatus FPX_WriteImageResolution (
              FPXImageHandle*     theFPX,
              unsigned long       theResolution,
              FPXImageDesc*       theData);


/*	Flush modified tiles to the file.
 *
 *	After pixel content has been modified by an FPX_WriteXXXX() routine, the
 *	changes may be cached in memory. This call ensures that the modified tiles
 *	are written to the file. Failure to call this may result in stale pixel data
 *	when lower resolutions are read.
 */ 
FPXStatus FPX_FlushModifiedTiles (
              FPXImageHandle*     theFPX);

/***************************************************************************
    FPX FILE READING
 ***************************************************************************/

/*  A struct for the various amounts of info that describe the
 *  compression of a tile.
 */
typedef struct {
              FPXCompressionOption 	compressOption;
              unsigned char       	compressQuality;
              long                  compressSubtype;
} FPXTileCompressionInfo;

/*  A compressed tile. Includes compression info as well as the
 *  JPEG data.
 */
typedef struct {
              FPXTileCompressionInfo compInfo;
              unsigned long			 dataLength;
              void*			 		 data;
} FPXTileDesc;

#ifdef macintosh
FPXStatus FPX_OpenImageByFilename (
              const FSSpec&    fileSpecs,
              char*            storagePathInFile,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX);
#else
 FPXStatus FPX_OpenImageByFilename (
              char*            fileName,
              char*            storagePathInFile,
              unsigned long*   width,
              unsigned long*   height,
              unsigned long*   tileWidth,
              unsigned long*   tileHeight,
              FPXColorspace*   colorspace,
              FPXImageHandle** theFPX);
#endif

/*  Read a rectangle of pixels from the transformed image.
 *
 *  The specified rectangle is read from the specified resolution
 *  into the components provided. Color conversion and pixel shuffling
 *  may occur in the process as well as cropping and rotation.
 *  XXX ColorKnobs! ColorTwist! Sharpeness!
 *  
 */
 FPXStatus FPX_ReadImageTransformRectangle (
              FPXImageHandle*     theFPX,
              float               X0,
              float               Y0,
              float               X1,
              float               Y1,
              long                rectWidth,
              long                rectHeight,
              FPXImageDesc*       theRectangle);

/*  Read a rectangle of pixels from a given resolution.
 *
 *  The specified rectangle is read from the specified resolution
 *  into the components provided.
 *  CAUTION : this is the symetric function to FPX_WriteImageRectangle() so
 *  no viewing transform is applied to the data!
 */
 FPXStatus FPX_ReadImageRectangle (
              FPXImageHandle*     theFPX,
              unsigned long       X0,
              unsigned long       Y0,
              unsigned long       X1,
              unsigned long       Y1,
              unsigned long       theResolution,
              FPXImageDesc*       theImage);

/*  Read a decompressed tile of pixels from a Resolution.
 *  Read the specified tile and decompress it.
 *  CAUTION : viewing parameters (and particularly geometric ones) cannot
 *  be applied to a single tile of a particular resolution.
 */
 FPXStatus FPX_ReadImageTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXImageDesc*       theTile);

/*  Read a compressed tile of pixels from a Resolution.
 *
 *  if tile was not compressed, it will still return successfully.
 */
 FPXStatus FPX_ReadImageCompressedTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXTileDesc*        theTile);

 FPXStatus FPX_WriteImageCompressedTile (
              FPXImageHandle*     theFPX,
              unsigned long       whichTile,
              unsigned long       theResolution,
              FPXTileDesc*        theTile);

/***************************************************************************
   IMAGES WITH VIEWS
 ***************************************************************************/
 
typedef struct {
              float left;        /* left edge                              */
              float top;         /* top edge                               */
              float width;       /* width                                  */
              float height;      /* height                                 */
} FPXROI;

typedef float FPXFilteringValue;

typedef struct {
              /* first row:                                                */
              float a11;
              float a12;
              float a13;
              float a14;
              /* second row:                                               */
              float a21;
              float a22;
              float a23;
              float a24;
              /* third row:                                                */
              float a31;
              float a32;
              float a33;
              float a34;
              /* fourth row:                                               */
              float a41;
              float a42;
              float a43;
              float a44;
        
} FPXAffineMatrix;

typedef float FPXResultAspectRatio;

typedef struct FPXColorTwistMatrix {
              /* first row                                                 */
              float byy;
              float byc1;
              float byc2;
              float dummy1_zero;      /* nominally zero.                   */
              /* second row                                                */
              float bc1y;
              float bc1c1;
              float bc1c2;
              float dummy2_zero;      /* nominally zero.                   */
              /* third row                                                 */
              float bc2y;
              float bc2c1;
              float bc2c2;
              float dummy3_zero;      /* nominally zero.                   */
              /* fourth row                                                */
              float dummy4_zero;      /* nominally zero.                   */
              float dummy5_zero;      /* nominally zero.                   */
              float dummy6_zero;      /* nominally zero.                   */
              float dummy7_one;       /* nominally one.                    */
        
} FPXColorTwistMatrix;

typedef float FPXContrastAdjustment;

/*  There is a desire to create images associated with views.
 *  These are Image Views with embedded Images.
 *  The View created may have no Transform (optionnal pointers set to NULL).
 *  The PIW (for instance) may wish to create such beasts.
 */

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
              FPXImageHandle**       theFPX);

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
              FPXImageHandle**       theFPX);

#endif

/********************************************************************************
   ICC PROFILE
 *******************************************************************************/
 
/*  The ICC profile is a data structure defined in the ICC spec.
 *
 *  Please visit ftp:sgigate.sgi.com/pub/icc for a copy of the spec which
 *  defines this structure.  The format will not be addressed here.
 *
 *  FPX will allow you to obtain the structure if it is stored in the
 *  file.
 *
 */

 FPXStatus FPX_SetICCProfile(
              FPXImageHandle *theFPX,
              FPXStr *        theProfile,
              unsigned short  profileIndex);

 FPXStatus FPX_GetICCProfile(
              FPXImageHandle *theFPX,
              FPXStr *        theProfile,
              unsigned short  profileIndex);

/********************************************************************************
   PATH
 *******************************************************************************/
 
/*  The Paths are vector informations stored along with a FPX Image.
 *  CAUTION: 
 *     - these functions are not implemented in Baseline
 */

typedef enum {  LINKED_POINT = 0,   // Smooth Bezier point
                FREE_POINT   = 1,   // Angular Bezier point
                LINE_LEFT    = 2,   // Line left, Bezier right
                LINE_RIGHT   = 3,   // Bezier left, line right
                LINE_POINT   = 4    // Angular polygon point
} FPXPointType;

typedef struct {
		float x;  // Horizontal coordinate
		float y;  // Vertical   coordinate
} FPXCoordinate;

typedef struct {
		FPXPointType 	type;
		FPXCoordinate 	directionLeft;
		FPXCoordinate 	anchor;
		FPXCoordinate 	directionRight;
} FPXBezierPoint;

typedef enum {  CLOSED_PATH = 0,   // Closed path
                OPEN_PATH   = 1    // Open path
} FPXPathType;

typedef struct {
		FPXPathType     type;
		unsigned long   numberOfPoints;
		FPXBezierPoint* points;
} FPXPath;

 FPXStatus FPX_SetPath(
              FPXImageHandle *theFPX,
              unsigned short  thePathIndex,
              FPXPath*        thePath);

 FPXStatus FPX_GetPath(
              FPXImageHandle *theFPX,
              unsigned short  thePathIndex,
              FPXPath**       thePath);

/********************************************************************************
   IMAGE WITH VIEW
 *******************************************************************************/

/* the following are only valid for an Image With a View:                         */

/* get/set the Image View transforms:
        ROI                                                                       */

FPXStatus FPX_SetImageROI (
              FPXImageHandle*  theFPX,
              FPXROI*          theROI);

FPXStatus FPX_GetImageROI (
              FPXImageHandle*  theFPX,
              FPXROI*          theROI);
        
/* Filtering.                                                                      */

 FPXStatus FPX_SetImageFilteringValue (
              FPXImageHandle*    theFPX,
              FPXFilteringValue* theFiltering);
        
 FPXStatus FPX_GetImageFilteringValue (
              FPXImageHandle*     theFPX,
              FPXFilteringValue*  theFiltering);
        

/* Spatial Orientation (matrix)                                                     */

 FPXStatus FPX_SetImageAffineMatrix (
              FPXImageHandle*   theFPX,
              FPXAffineMatrix*  theAffineMatrix);
        
 FPXStatus FPX_GetImageAffineMatrix (
              FPXImageHandle*   theFPX,
              FPXAffineMatrix*  theAffineMatrix);
        
/* Result Aspect Ratio                                                               */

 FPXStatus FPX_SetImageResultAspectRatio (
              FPXImageHandle*       theFPX,
              FPXResultAspectRatio* theAspectRatio);

 FPXStatus FPX_GetImageResultAspectRatio (
              FPXImageHandle*        theFPX,
              FPXResultAspectRatio*  theAspectRatio);
        
/*
        ColorTwist Matrix                                                            */

 FPXStatus FPX_SetImageColorTwistMatrix (
              FPXImageHandle*       theFPX,
              FPXColorTwistMatrix*  theColorTwistMatrix);
        
 FPXStatus FPX_GetImageColorTwistMatrix (
              FPXImageHandle*       theFPX,
              FPXColorTwistMatrix*  theColorTwistMatrix);
        
/*
 *      Contrast Adjustment
 */
 FPXStatus FPX_SetImageContrastAdjustment (
              FPXImageHandle*        theFPX,
              FPXContrastAdjustment* theContrastAdjustment);
        
 FPXStatus FPX_GetImageContrastAdjustment (
              FPXImageHandle*         theFPX,
              FPXContrastAdjustment*  theContrastAdjustment);
        

typedef CLIPDATA  FPXThumbnail;
   
   typedef struct {
        FPXbool         title_valid;
        FPXStr          title;

        FPXbool         subject_valid;
        FPXStr          subject;

        FPXbool         author_valid;
        FPXStr          author;

        FPXbool         keywords_valid;
        FPXStr          keywords;

        FPXbool         comments_valid;
        FPXStr          comments;

        FPXbool         OLEtemplate_valid;
        FPXStr          OLEtemplate;
                                         
        FPXbool         last_author_valid;
        FPXStr          last_author;
                                                                          
        FPXbool         rev_number_valid;
        FPXStr          rev_number;

        FPXbool         edit_time_valid;
        FPXfiletime     edit_time;
 
        FPXbool         last_printed_valid;
        FPXfiletime     last_printed;
 
        FPXbool         create_dtm_valid;
        FPXfiletime     create_dtm;             /* We set, you only get! */
 
        FPXbool         last_save_dtm_valid;
        FPXfiletime     last_save_dtm;
 
        FPXbool         page_count_valid;
        unsigned long   page_count;
 
        FPXbool         word_count_valid;
        unsigned long   word_count;
 
        FPXbool         char_count_valid;
        unsigned long   char_count;
 
        FPXbool         thumbnail_valid;
        FPXThumbnail    thumbnail;
 
        FPXbool         appname_valid;
        FPXStr          appname;
 
        FPXbool         security_valid;
        unsigned long   security;
}FPXSummaryInformation;

FPXStatus FPX_SetSummaryInformation(
              FPXImageHandle *        theFPX,
              FPXSummaryInformation * theSummary);

FPXStatus FPX_GetSummaryInformation(
              FPXImageHandle *        theFPX,
              FPXSummaryInformation * theSummary);

/***************************************************************************
   IMAGE INFO PROPERTY SET  A.K.A 'NON-IMAGE DATA'
 ***************************************************************************/

/* Set/Get the Non-Image data ("Image Info" Property Set). This is a
 * very large collection of values of various sorts divided into several
 * groups of values. The interface will be through structs passed into the
 * set/get routines.  Passing a valid flag==TRUE will cause the associated 
 * field to be adjusted when a set operation is performed.  After a get 
 * operation finding the valid flag==TRUE means that the file produced a 
 * value for the associated field.
 */

typedef enum {
              FPX_UNIDENTIFIED_SOURCE = 0,
              FPX_FILM_SCANNER,
              FPX_REFLECTION_PRINT_SCANNER,
              FPX_DIGITAL_CAMERA,
              FPX_STILL_FROM_VIDEO,
              FPX_COMPUTER_GRAPHICS
} FPXSource;

typedef enum {
              FPX_UNIDENTIFIED_SCENE = 0,
              FPX_ORIGINAL_SCENE,
              FPX_SECOND_GENERATION_SCENE,
              FPX_DIGITAL_SCENE_GENERATION
} FPXScene;

/***************************************************************************/

typedef struct {
              FPXbool       file_source_valid;
              FPXSource     file_source;

              FPXbool       scene_type_valid;
              FPXScene      scene_type;

              FPXbool       creation_path_valid;
              FPXLongArray  creation_path;

              FPXbool       name_man_release_valid;
              FPXWideStr    name_man_release;

              FPXbool       user_defined_id_valid;
              FPXWideStr    user_defined_id;

              FPXbool       original_sharpness_approximation_valid;
              float         original_sharpness_approximation;
} FPXFileSourceGroup;

FPXStatus FPX_SetSourceGroup ( 
			  FPXImageHandle*     theFPX,
			  FPXFileSourceGroup* theSourceGroup);

FPXStatus FPX_GetSourceGroup ( 
			  FPXImageHandle*     theFPX,
		      FPXFileSourceGroup* theSourceGroup);

/***************************************************************************/

typedef struct {
              FPXbool       copyright_valid;
              FPXWideStr    copyright;

              FPXbool       legal_broker_for_orig_image_valid;
              FPXWideStr    legal_broker_for_orig_image;

              FPXbool       legal_broker_for_digital_image_valid;
              FPXWideStr    legal_broker_for_digital_image;

              FPXbool       authorship_valid;
              FPXWideStr    authorship;

              FPXbool       intellectual_prop_notes_valid;
              FPXWideStr    intellectual_prop_notes;
} FPXIntellectualPropertyGroup;

 FPXStatus FPX_SetIntellectualPropGroup ( 
			  FPXImageHandle*     theFPX,
              FPXIntellectualPropertyGroup*  thePropGroup);

 FPXStatus FPX_GetIntellectualPropGroup ( 
			  FPXImageHandle*     theFPX,
              FPXIntellectualPropertyGroup*  thePropGroup);

/***************************************************************************/

typedef enum {
              FPX_UNIDENTIFIED_TARGET = 0,
              FPX_COLOR_CHART,
              FPX_GREY_CARD,
              FPX_GREYSCALE,
              FPX_RESOLUTION_CHART,
              FPX_INCH_SCALE,
              FPX_CENTIMETER_SCALE,
              FPX_MILLIMETER_SCALE,
              FPX_MICROMETER_SCALE
} FPXTestTargetInImage;

typedef struct {
              FPXbool         test_target_in_the_image_valid;
              unsigned long   test_target_in_the_image;

              FPXbool         group_caption_valid;
              FPXWideStr      group_caption;

              FPXbool         caption_text_valid;
              FPXWideStr      caption_text;

              FPXbool         people_in_the_image_valid;
              FPXWideStrArray people_in_the_image;

              FPXbool         things_in_image_valid;
              FPXWideStrArray things_in_image;

              FPXbool         date_of_original_image_valid;
              FPXfiletime     date_of_original_image;     	

              FPXbool         events_in_the_image_valid;
              FPXWideStrArray events_in_the_image;

              FPXbool         places_in_the_valid;
              FPXWideStrArray places_in_the;

              FPXbool         content_description_notes_valid;
              FPXWideStr      content_description_notes;
} FPXContentDescriptionGroup;

FPXStatus FPX_SetContentDescriptionGroup (
			  FPXImageHandle*     theFPX,
              FPXContentDescriptionGroup*    theContentGroup);

FPXStatus FPX_GetContentDescriptionGroup (
			  FPXImageHandle*     theFPX,
              FPXContentDescriptionGroup*    theContentGroup);

/***************************************************************************/

typedef struct {
              FPXbool     camera_manufacturer_name_valid;
              FPXWideStr  camera_manufacturer_name;

              FPXbool     camera_model_name_valid;
              FPXWideStr  camera_model_name;

              FPXbool     camera_serial_number_valid;
              FPXWideStr  camera_serial_number;
} FPXCameraInformationGroup;

FPXStatus FPX_SetCameraInformationGroup (
			  FPXImageHandle*               theFPX,
              FPXCameraInformationGroup*    theCameraGroup);

FPXStatus FPX_GetCameraInformationGroup (
			  FPXImageHandle*               theFPX,
              FPXCameraInformationGroup*    theCameraGroup);

/***************************************************************************/

typedef enum {
              FPX_UNIDENTIFIED_EXPOSURE_PROGRAM = 0,
              FPX_MANUAL,
              FPX_PROGRAM_NORMAL,
              FPX_APERTURE_PRIORITY,
              FPX_SHUTTER_PRIORITY,
              FPX_PROGRAM_CREATIVE,
              FPX_PROGRAM_ACTION,
              FPX_PROTRAIT_MODE,
              FPX_LANDSCAPE_MODE
} FPXExposureProgram;

typedef enum {
              FPX_UNIDENTIFED_METERING_MODE = 0,
              FPX_AVERAGE,
              FPX_CENTER_WEIGHTED_AVERAGE,
              FPX_SPOT,
              FPX_MULTI_SPOT
} FPXMeteringMode;

typedef enum {
              FPX_UNIDENTIFIED_SCENE_ILLUMINANT = 0,
              FPX_DAYLIGHT,
              FPX_FLUORESCENT_LIGHT,
              FPX_TUNGSTEN_LAMP,
              FPX_FLASH,
              FPX_STANDARD_ILLUMINANT_A,
              FPX_STANDARD_ILLUMINANT_B,
              FPX_STANDARD_ILLUMINANT_C,
              FPX_D55_ILLUMINANT,
              FPX_D65_ILLUMINANT,
              FPX_D75_ILLUMINANT
} FPXSceneIlluminant;

typedef enum {
              FPX_FLASH_USE_UNKNOWN = 0,
              FPX_NO_FLASH_USED,
              FPX_FLASH_USED
} FPXFlash;

typedef enum {
              FPX_NOT_A_CAMERA_FEATURE_FLASH_RETURN = 0,
              FPX_SUBJECT_OUTSIDE_FLASH_RANGE,
              FPX_SUBJECT_INSIDE_FLASH_RANGE
} FPXFlashReturn;

typedef enum {
              FPX_NOT_A_CAMERA_FEATURE_BACKLIGHT = 0,
              FPX_FRONT_LIT,
              FPX_BACK_LIT1,
              FPX_BACK_LIT2
} FPXBackLight;

typedef enum {
              FPX_UNIDENTIFIED_SPECIAL_EFFECTS_OPTICAL_FILTER = 0,
              FPX_NONE,
              FPX_COLORED,
              FPX_DIFFUSION,
              FPX_MULTI_IMAGE,
              FPX_POLARIZING,
              FPX_SPLIT_FIELD,
              FPX_STAR
} FPXSpecialEffectsOpticalFilter;

typedef struct {
			unsigned long length; // number of filters
			FPXSpecialEffectsOpticalFilter *ptr;
} FPXOpticalFilterArray;

typedef struct {
              FPXbool            capture_date_valid;
              FPXfiletime        capture_date;            

              FPXbool            exposure_time_valid;
              float              exposure_time;

              FPXbool            f_number_valid;
              float              f_number;

              FPXbool            exposure_program_valid;
              FPXExposureProgram exposure_program;

              FPXbool            brightness_value_valid;
              FPXRealArray       brightness_value;

              FPXbool            exposure_bias_value_valid;
              float              exposure_bias_value;

              FPXbool            subject_distance_valid;
              FPXRealArray       subject_distance;

              FPXbool            metering_mode_valid;
              FPXMeteringMode    metering_mode;

              FPXbool            scene_illuminant_valid;
              FPXSceneIlluminant scene_illuminant;

              FPXbool            focal_length_valid;
              float              focal_length;

              FPXbool            maximum_aperature_value_valid;
              float              maximum_aperature_value;

              FPXbool            flash_valid;
              FPXFlash           flash;

              FPXbool            flash_energy_valid;
              float              flash_energy;

              FPXbool            flash_return_valid;
              FPXFlashReturn     flash_return;

              FPXbool            back_light_valid;
              FPXBackLight       back_light;

              FPXbool            subject_location_valid;
              FPXRealArray       subject_location;

              FPXbool            exposure_index_valid;
              float              exposure_index;

              FPXbool           special_effects_optical_filter_valid;
              FPXLongArray  	special_effects_optical_filter;

              FPXbool            per_picture_notes_valid;
              FPXWideStr         per_picture_notes;
} FPXPerPictureCameraSettingsGroup;

FPXStatus FPX_SetPerPictureGroup (
			  FPXImageHandle*                      theFPX,
              FPXPerPictureCameraSettingsGroup*    thePerPictureGroup);

FPXStatus FPX_GetPerPictureGroup (
			  FPXImageHandle*                      theFPX,
              FPXPerPictureCameraSettingsGroup*    thePerPictureGroup);

/***************************************************************************/

typedef enum {
              FPX_UNDEFINED = 0,
              FPX_MONOCHROME_AREA_SENSOR,
              FPX_ONE_CHIP_COLOR_AREA_SENSOR,
              FPX_TWO_CHIP_COLOR_AREA_SENSOR,
              FPX_THREE_CHIP_COLOR_AREA_SENSOR,
              FPX_COLOR_SEQUENCIAL_AREA_SENSOR,
              FPX_MONOCHROME_LINEAR_SENSOR,
              FPX_TRILINEAR_SENSOR,
              FPX_COLOR_SEQUENCIAL_LINEAR_SENSOR
} FPXSensingMethod;

typedef enum {
              FPX_RED = 0,
              FPX_GREEN,
              FPX_BLUE,
              FPX_CYAN,
              FPX_MAGENTA,
              FPX_YELLOW,
              FPX_WHITE
} FPXColorFilterPatternValues;

typedef struct {
              unsigned long    number_of_columns;
              unsigned long    number_of_rows;
              FPXWideStrArray      column_headings;
              FPXRealArray     data;
} FPXSpacialFrequencyResponseBlock;

typedef struct {
              unsigned short    cfa_repeat_rows;
              unsigned short    cfa_repeat_cols;
              FPXStr            cfa_array;
} FPXCFA_PatternBlock;

typedef struct {
              unsigned short   number_of_columns;
              unsigned short   number_of_rows;
              FPXWideStrArray      column_headings;
              FPXRealArray     data;
} FPXOECF_Block;

typedef struct {
              FPXbool           sensing_method_valid;
              FPXSensingMethod  sensing_method;

              FPXbool           focal_plane_x_resolution_valid;
              float             focal_plane_x_resolution;

              FPXbool           focal_plane_y_resolution_valid;
              float             focal_plane_y_resolution;

              FPXbool           focal_plane_resolution_unit_valid;
              FPXResolutionUnit focal_plane_resolution_unit;

              FPXbool           spacial_frequency_valid;
              FPXSpacialFrequencyResponseBlock spacial_frequency;

              FPXbool           cfa_pattern_valid;
              FPXCFA_PatternBlock cfa_pattern;

              FPXbool           spectral_sensitivity_valid;
              FPXWideStr        spectral_sensitivity;

              FPXbool           iso_speed_ratings_valid;
              FPXShortArray     iso_speed_ratings;

              FPXbool            oecf_valid;
              FPXOECF_Block 	 oecf;
} FPXDigitalCameraCharacterizationGroup;

FPXStatus FPX_SetDigitalCameraGroup (
			  FPXImageHandle*                        theFPX,
              FPXDigitalCameraCharacterizationGroup* theDigitalCameraGroup);

FPXStatus FPX_GetDigitalCameraGroup (
			  FPXImageHandle*                        theFPX,
              FPXDigitalCameraCharacterizationGroup* theDigitalCameraGroup);

/***************************************************************************/

typedef enum {
              FPX_UNIDENTIFIED = 0,
              FPX_NEGATIVE_BW,
              FPX_NEGATIVE_COLOR,
              FPX_REVERSAL_BW,
              FPX_REVERSAL_COLOR,
              FPX_CHROMAGENIC,
              FPX_INTERNEGATIVE_BW,
              FPX_INTERNEGATIVE_COLOR
} FPXFilmCategory;


typedef struct {
              FPXbool           film_brand_valid;
              FPXWideStr        film_brand;

              FPXbool           film_category_valid;
              FPXFilmCategory   film_category;

              FPXbool           film_size_valid;
              float				film_size_x;
              float 			film_size_y;
              FPXResolutionUnit film_size_unit;

              FPXbool           film_roll_number_valid;
              unsigned short    film_roll_number;

              FPXbool           film_frame_number_valid;
              unsigned short    film_frame_number;
} FPXFilmDescriptionGroup;

FPXStatus FPX_SetFilmDescriptionGroup (
			  FPXImageHandle*          theFPX,
              FPXFilmDescriptionGroup* theFilmGroup);

FPXStatus FPX_GetFilmDescriptionGroup (
			  FPXImageHandle*          theFPX,
              FPXFilmDescriptionGroup* theFilmGroup);

/***************************************************************************/

typedef enum {
              FPX_UNIDENTIFIED_ORIGINAL_MEDIUM = 0,
              FPX_CONTINUOUS_TONE_IMAGE,
              FPX_HALFTONE_IMAGE,
              FPX_LINE_ART
} FPXOriginalMedium;

typedef enum {
              FPX_UNIDENTIFIED_TYPE_OF_REFLECTION_ORIGINAL = 0,
              FPX_BW_PRINT,
              FPX_COLOR_PRINT,
              FPX_BW_DOCUMENT,
              FPX_COLOR_DOCUMENT
} FPXTypeOfReflectionOriginal;

typedef struct {
              float             original_size_x;
              float             original_size_y;
              FPXResolutionUnit original_size_unit;
} FPXScannedImageSizeBlock;

typedef struct {
              FPXbool                     original_scanned_image_size_valid;
              FPXScannedImageSizeBlock    original_scanned_image_size;

              FPXbool                     original_document_size_valid;
              FPXScannedImageSizeBlock    original_document_size;

              FPXbool                     original_medium_valid;
              FPXOriginalMedium           original_medium;

              FPXbool                     type_of_reflection_original_valid;
              FPXTypeOfReflectionOriginal type_of_reflection_original;
} FPXOriginalDocumentScanDescriptionGroup;

FPXStatus FPX_SetOriginalDocDescriptionGroup (
			  FPXImageHandle*                          theFPX,
              FPXOriginalDocumentScanDescriptionGroup* theDocGroup);

FPXStatus FPX_GetOriginalDocDescriptionGroup (
			  FPXImageHandle*                          theFPX,
              FPXOriginalDocumentScanDescriptionGroup* theDocGroup);

/***************************************************************************/


typedef struct {
              FPXbool       scanner_manufacturer_name_valid;
              FPXWideStr    scanner_manufacturer_name;

              FPXbool       scanner_model_name_valid;
              FPXWideStr    scanner_model_name;

              FPXbool       scanner_serial_number_valid;
              FPXWideStr    scanner_serial_number;

              FPXbool       scan_software_valid;
              FPXWideStr    scan_software;

              FPXbool       scan_software_revision_date_valid;
              DATE        	scan_software_revision_date;    

              FPXbool       service_bureau_org_name_valid;
              FPXWideStr    service_bureau_org_name;

              FPXbool       scan_operator_id_valid;
              FPXWideStr    scan_operator_id;

              FPXbool       scan_date_valid;
              FPXfiletime   scan_date;                       

              FPXbool       last_modified_date_valid;
              FPXfiletime   last_modified_date;              

              FPXbool       scanner_pixel_size_valid;
              float         scanner_pixel_size;
} FPXScanDevicePropertyGroup;

FPXStatus FPX_SetScanDevicePropertyGroup (
			  FPXImageHandle*             theFPX,
              FPXScanDevicePropertyGroup* theScanGroup);

FPXStatus FPX_GetScanDevicePropertyGroup (
			  FPXImageHandle*             theFPX,
              FPXScanDevicePropertyGroup* theScanGroup);

/****************************************************************************/
/* EXTENSION LIST MANAGEMENT                                                */
/****************************************************************************/

#define FPX_MAX_EXTENSION_ITEMS		5

typedef enum {
              FPX_EXTENSION_PERSISTENT = 0,
              FPX_EXTENSION_VOLATILE,
              FPX_EXTENSION_POTENTIALLY_VOLATILE
} FPXExtensionPersistence;

typedef struct {
              FPXWideStr              	extensionName;
              CLSID                   	extensionClassID;
              FPXExtensionPersistence 	extensionPersistence;

              FPXbool				  	extensionCreationDateIsValid;
              FPXfiletime             	extensionCreationDate;
              
              FPXbool				  	extensionModificationDateIsValid;
              FPXfiletime             	extensionModificationDate;
              
              FPXbool				  	creatingApplicationIsValid;
              FPXWideStr              	creatingApplication;
              
              FPXbool				  	extensionDescriptionIsValid;
              FPXWideStr              	extensionDescription;
              
              FPXbool				  	streamPathNameIsValid;
              FPXWideStrArray         	streamPathName;
              
              FPXbool				  	fpxStreamPathNameIsValid;
              FPXWideStrArray         	fpxStreamPathName;
              
              FPXbool				  	fpxStreamOffsetIsValid;
              FPXLongArray         		fpxStreamOffset;
              
              FPXbool				  	propertySetPathNameIsValid;
              FPXWideStrArray         	propertySetPathName;
              
              FPXbool				  	propertySetFormatIDIsValid;
              FPXClsIDArray             propertySetFormatID;
              
              FPXbool				  	propertySetIDCodesIsValid;
              FPXWideStrArray           propertySetIDCodes;
              
} FPXExtensionDescription;

FPXStatus FPX_GetExtensionDescription (
              FPXImageHandle*             	theFPX,
              LPWSTR                      	extensionName,
              FPXExtensionDescription*    	theDescription);
FPXStatus FPX_SetExtensionDescription (
              FPXImageHandle*             	theFPX,
              LPWSTR                       	extensionName,
              FPXExtensionDescription*    	theDescription);
FPXStatus FPX_GetStreamPointer (
              FPXImageHandle*             theFPX,
              char*                       streamName,
              IStream**                   oleStream);
FPXStatus FPX_GetPropertySetPointer (
              FPXImageHandle*             theFPX,
              char*                       propertySetName,
              IStream**                   olePropertySet);

//	Viewing Toolkit Class Definitions
//  ---------------------------------

typedef struct PageImage {
  long pagehandle;
} FPXPage;
typedef struct ViewWorld {
  long worldhandle;
} FPXWorld;
typedef struct ViewImage {
  long imagehandle;
} FPXImageInWorld;
typedef struct ViewWindow {
  long windowshandle;
} FPXWindow;

// Formatted output tools :
// Perhaps misnamed. Actually should be called simple render or
// simple print. provide quick means of rendering a single image.
// allows for simple rotation. Auto scales to maximize rotated 
// image size in page without crop.

FPXStatus FPX_SetPageSetup (
              FPXImageHandle* theFPX, 
              FPXPage**       thePage, 
              long            width, 
              long            height,
              float           rotation,
              FPXColorspace   backgroundColorspace,
              FPXBackground   backgroundColor);
             
                        
FPXStatus FPX_ClosePage (
              FPXPage*      thePage);
             
                        
FPXStatus FPX_ReadPage (
              FPXPage*      thePage, 
              FPXImageDesc* renderingBuffer);

                        
FPXStatus FPX_ReadPageLine (
              FPXPage*      thePage, 
              long          lineNumber, 
              FPXImageDesc* renderingBuffer);
                           
// World tools :
                        
FPXStatus FPX_CreateWorld (
              FPXWorld**    theWorld, 
              float         width, 
              float         height,
              FPXColorspace backgroundColorspace,
              FPXBackground backgroundColor);
            
FPXStatus FPX_DeleteWorld (
              FPXWorld*     theWorld);
        
// the following several functions are for managing multiple images
// in a world.
// Should we wish to support multiple FPX images in a world,
// we will need these functions.

// List of images management :

FPXStatus FPX_AddImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage,
              FPXImageHandle*   theFPX);
                
// Adds an image to the world. returns handle to the 
// FPXImageInWorld.
// The image is placed as specified by the affine 
// matrix in it's view.
                                
FPXStatus FPX_DeleteImage (
              FPXWorld*        theWorld, 
              FPXImageInWorld* theImage);
                
// removes an image from the world.
// does not remove associated window created at 
// FPX_AddImage time.
                
FPXStatus FPX_GetFirstImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get first image from world image list.
                                
FPXStatus FPX_GetLastImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get last image from world image list.
                                
FPXStatus FPX_GetCurrentImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get current image from world image list.
                
FPXStatus FPX_NextImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);
                
// get the next image in the list. becomes current.
                                
FPXStatus FPX_PreviousImage (
              FPXWorld*         theWorld, 
              FPXImageInWorld** theImage);

// get the previous image in the list. becomes current.


// misc.

// Image position tools :
FPXStatus FPX_SetImageSize (
              FPXImageInWorld* theImage, 
              float            width, 
              float            height);

// Set width and height of image in world coord.s
// I.e. scale in x and y.

FPXStatus FPX_SetImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22);
                    
// Set full 2D affine in one swell foop.
        
FPXStatus FPX_ApplyImageTransform (
              FPXImageInWorld* theImage, 
              float x0,  float y0, 
              float m11, float m12, float m21, float m22);
                    
// Compose the specified affine with the existing affine xfrm 
// of the image.
        
FPXStatus FPX_TranslateImage (
              FPXImageInWorld* theImage, 
              float dx, float dy);
           
// Move the image in world coord.s. will form new rendering xform.
// Composed with existing xform.
        
FPXStatus FPX_RotateImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float theta);
                                 
// Rotate an image ccw about a given point.
// Implemented as xlate of said point to origin, rotate, 
// xlate back.
        
FPXStatus FPX_ScaleImage (
              FPXImageInWorld* theImage, 
              float x0, float y0,
              float sx, float sy);
                                
// Scale in x and y about a specific point.
// Implemented as xlation of said point to origin, scale, 
// xlate back.

FPXStatus FPX_ShearHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh);
                                     
// Compose a horizontal shear on to the xform.
// Shear is wrt a given point. Implemented by translation of
// said point to the origin, shear, translation back.
        
FPXStatus FPX_ShearVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float sh);
           
// Compose a vertical shear on to the xform.
// Shear is wrt a given point. Implemented by translation of
// said point to the origin, shear, translation back.
        
FPXStatus FPX_FlipHorizontal (
              FPXImageInWorld* theImage, 
              float x0, float y0);
           
// Flip (mirror) image about a horzontal line.
// Said line runs through x0,y0.
// Implemented by xlate of point to origin, scale x by -1, 
// xlate back.

FPXStatus FPX_FlipVertical (
              FPXImageInWorld* theImage, 
              float x0, float y0);
           
// Flip (mirror) image about a vertical line.
// Said line runs through x0,y0.
// Implemented by xlate of point to origin, scale x by -1, 
// xlate back.
        
FPXStatus FPX_GetOutlineParallelogram (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1,
              float* x2, float* y2, 
              float* x3, float* y3);
                                                                                        
// Return corners of an image as rendered.
// The bounding box including shear and rotation and crop (ROI).
        
FPXStatus FPX_GetOutlineRectangle (
              FPXImageInWorld* theImage, 
              float* x0, float* y0, 
              float* x1, float* y1);

// Get the corners of a 0 rotation rectangle which bounds the 
// image. Conventional bounding box.
        
FPXStatus FPX_GetOrigin (
              FPXImageInWorld* theImage, 
              float* x0, float* y0);
           
// return coord.s of origin of the image (upper left)
// == translation values from xfrm. I.e. location of the 
// upper left.
        
FPXStatus FPX_SetImageCrop (
              FPXImageInWorld* theImage, 
              float x0, float y0, 
              float x1, float y1);
                                  
// Set Region of Interest (ROI) on image.
// Coords. are in world coords.
        
FPXStatus FPX_ResetImageCrop (
              FPXImageInWorld* theImage);

// Set ROI to full image.

// Viewing options: this concern the usage of the alpha channel stored along
// with the image in the composition:

FPXStatus FPX_UseAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          useAlphaChannel);
// The alpha channel is taken into account if (useAlphaChannel == true)

FPXStatus FPX_InvertAlphaChannel (
              FPXImageInWorld* theImage,
              FPXbool          inverseAlpha);
// The opacity is inversed prior composition if (inverseAlpha == true)
		
// View Window  tools :
// An FPX window is really a viewport on the world.
// It establishes resolution and hence pixels.
                        
FPXStatus FPX_CreateWindow (
              FPXWorld*    theWorld, 
              FPXWindow**  theWindow, 
              float x0,    float y0, 
              float width, float height, 
              float resolution);
                                  
FPXStatus FPX_DeleteWindow (
              FPXWindow* theWindow);

FPXStatus FPX_ReadWindowSample (
              FPXWindow*    theWindow, 
              long x0,      long y0,
              FPXImageDesc* windowBufferInfo);
                                      
// Render one lump of output.
// This is a 4x4 square about the specified point (in pixels) 
// in the window.
                        
FPXStatus FPX_RefreshWindow (
              FPXWindow*    theWindow, 
              FPXImageDesc* windowBufferInfo);
           
// Render the entire window at once.
// Includes a progress Function, as rendering may take a while, 
// especially at high res.
        
FPXStatus FPX_TranslateWindow (
              FPXWindow* theWindow, 
              float  dx, float  dy);
           
// Move a window in world coords.
// Changes xform associated with window.
        
FPXStatus FPX_ResizeWindow (
              FPXWindow*   theWindow, 
              float width, float height);
           
// Crop or enlarge a window, without changing resolution.
// I.e. no zooming, just a bigger window with more pixels in it.
        
FPXStatus FPX_ZoomWindow (
              FPXWindow* theWindow, 
              float      zoomRatio);
           
// Change window dimensions and resolution inversely so as 
// to zoom/dezoom.
        
FPXStatus FPX_GetWindowDefinition (
              FPXWindow*    theWindow, 
              float* x0,    float* y0, 
              float* width, float* height, 
              float* resolution);
                                         
// Get vital statistics about a window: location, size, 
// resolution.
        
FPXStatus FPX_WindowToWorld (
              FPXWindow* theWindow, 
              long i,    long j, 
              float* x,  float* y);
                                   
// Xform point from window (pixels) to world coords.
        
FPXStatus FPX_WorldToWindow (
              FPXWindow* theWindow, 
              float x, float y, 
              long* i, long* j);
                                   
// Xform point from world coords. to window coord.s (pixels.)


//      Contrast adjustment:
//              Setting, getting, processing.
//              Note that this is really a property of a View of an image.
//              We will associate it with the image in the world.
/*
        Contrast Adjustment
*/

FPXStatus FPX_SetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment);
        
FPXStatus FPX_GetImageInWorldContrastAdjustment (
              FPXImageInWorld*       theImage,
              FPXContrastAdjustment* theContrastAdjustment);
        
        
//      Color twist:
//              Setting, getting, processing.
/*
        ColorTwist Matrix */

FPXStatus FPX_SetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix);
        
FPXStatus FPX_GetImageInWorldColorTwistMatrix (
              FPXImageInWorld*     theImage,
              FPXColorTwistMatrix* theColorTwistMatrix);
        
                
//      Sharpening:
//              Setting, getting, processing.
//              This is defined to occur after Image Decompression, but
//              before color transformation/twisting/constrast adjustment.

/*        Filtering. */

FPXStatus FPX_SetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering);
        
FPXStatus FPX_GetImageInWorldFilteringValue (
              FPXImageInWorld*   theImage,
              FPXFilteringValue* theFiltering);

/* Get current Image in World Affine Matrix */

FPXStatus FPX_GetImageInWorldAffineMatrix (
			FPXImageInWorld*   	theImage,
			FPXAffineMatrix* 	mat);
			
/* Set current Image in World Affine Matrix */

FPXStatus FPX_SetImageInWorldAffineMatrix (
			FPXImageInWorld*   	theImage,
			FPXAffineMatrix* 	mat);
                
/****************************************************************************/
#endif // FPXLibAPI_h
/****************************************************************************/
