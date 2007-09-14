
/*
% Copyright (C) 2003, 2006 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                  M   M   AAA   TTTTT  L       AAA   BBBB                    %
%                  MM MM  A   A    T    L      A   A  B   B                   %
%                  M M M  AAAAA    T    L      AAAAA  BBBB                    %
%                  M   M  A   A    T    L      A   A  B   B                   %
%                  M   M  A   A    T    LLLLL  A   A  BBBB                    %
%                                                                             %
%                                                                             %
%                        Read MATLAB Image Format.                            %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                2001 - 2007                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%Currently supported formats:
%  2D matrices:      X*Y    byte, word, dword, qword, single, double
%  3D matrices: only X*Y*3  byte, word, dword, qword, single, double
%  complex:          X*Y                              single, double
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/shear.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/constitute.h"
#include "magick/magick_endian.h"


/* Auto coloring method, sorry this creates some artefact inside data
MinReal+j*MaxComplex = red  MaxReal+j*MaxComplex = black
MinReal+j*0 = white          MaxReal+j*0 = black
MinReal+j*MinComplex = blue  MaxReal+j*MinComplex = black
*/

typedef struct
{
  char identific[124];
  unsigned short Version;
  char EndianIndicator[2];
  unsigned long DataType;
  unsigned long ObjectSize;
  unsigned long unknown1;
  unsigned long unknown2;

  unsigned short unknown5;
  unsigned char StructureFlag;
  unsigned char StructureClass;
  unsigned long unknown3;
  unsigned long unknown4;
  unsigned long DimFlag;

  unsigned long SizeX;
  unsigned long SizeY;
  unsigned short Flag1;
  unsigned short NameFlag;
}
MATHeader;

static const char *MonthsTab[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
static const char *DayOfWTab[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
static const char *OsDesc=
#ifdef __WIN32__
    "PCWIN";
#else
 #ifdef __APPLE__
    "MAC";
 #else
    "LNX86";
 #endif
#endif

typedef enum
  {
    miINT8 = 1,			/* 8 bit signed */
    miUINT8,			/* 8 bit unsigned */
    miINT16,			/* 16 bit signed */
    miUINT16,			/* 16 bit unsigned */
    miINT32,			/* 32 bit signed */
    miUINT32,			/* 32 bit unsigned */
    miSINGLE,			/* IEEE 754 single precision float */
    miRESERVE1,
    miDOUBLE,			/* IEEE 754 double precision float */
    miRESERVE2,
    miRESERVE3,
    miINT64,			/* 64 bit signed */
    miUINT64,			/* 64 bit unsigned */
    miMATRIX,		        /* MATLAB array */
    miCOMPRESSED,	        /* Compressed Data */
    miUTF8,		        /* Unicode UTF-8 Encoded Character Data */
    miUTF16,		        /* Unicode UTF-16 Encoded Character Data */
    miUTF32			/* Unicode UTF-32 Encoded Character Data */
  } mat5_data_type;

typedef enum
  {
    mxCELL_CLASS=1,		/* cell array */
    mxSTRUCT_CLASS,		/* structure */
    mxOBJECT_CLASS,		/* object */
    mxCHAR_CLASS,		/* character array */
    mxSPARSE_CLASS,		/* sparse array */
    mxDOUBLE_CLASS,		/* double precision array */
    mxSINGLE_CLASS,		/* single precision floating point */
    mxINT8_CLASS,		/* 8 bit signed integer */
    mxUINT8_CLASS,		/* 8 bit unsigned integer */
    mxINT16_CLASS,		/* 16 bit signed integer */
    mxUINT16_CLASS,		/* 16 bit unsigned integer */
    mxINT32_CLASS,		/* 32 bit signed integer */
    mxUINT32_CLASS,		/* 32 bit unsigned integer */
    mxINT64_CLASS,		/* 64 bit signed integer */
    mxUINT64_CLASS,		/* 64 bit unsigned integer */
    mxFUNCTION_CLASS            /* Function handle */
  } arrayclasstype;

#define FLAG_COMPLEX 0x8
#define FLAG_GLOBAL  0x4
#define FLAG_LOGICAL 0x2

static const QuantumType z2qtype[4] = {GrayQuantum, BlueQuantum, GreenQuantum, RedQuantum};


static void InsertComplexDoubleRow(double *p, int y, Image * image, double MinVal,
                                  double MaxVal)
{
  double f;
  int x;
  register PixelPacket *q;

  if (MinVal == 0)
    MinVal = -1;
  if (MaxVal == 0)
    MaxVal = 1;

  q = SetImagePixels(image, 0, y, image->columns, 1);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (long) image->columns; x++)
  {
    if (*p > 0)
    {
      f = (*p / MaxVal) * (MaxRGB - q->red);
      if (f + q->red > MaxRGB)
        q->red = MaxRGB;
      else
        q->red += (int) f;
      if ((int) f / 2.0 > q->green)
        q->green = q->blue = 0;
      else
        q->green = q->blue -= (int) (f / 2.0);
    }
    if (*p < 0)
    {
      f = (*p / MaxVal) * (MaxRGB - q->blue);
      if (f + q->blue > MaxRGB)
        q->blue = MaxRGB;
      else
        q->blue += (int) f;
      if ((int) f / 2.0 > q->green)
        q->green = q->red = 0;
      else
        q->green = q->red -= (int) (f / 2.0);
    }
    p++;
    q++;
  }
  if (!SyncImagePixels(image))
    return;
  /*          if (image->previous == (Image *) NULL)
     if (QuantumTick(y,image->rows))
     MagickMonitor(LoadImageText,image->rows-y-1,image->rows); */
  return;
}


static void InsertComplexFloatRow(float *p, int y, Image * image, double MinVal,
                                  double MaxVal)
{
  double f;
  int x;
  register PixelPacket *q;

  if (MinVal == 0)
    MinVal = -1;
  if (MaxVal == 0)
    MaxVal = 1;

  q = SetImagePixels(image, 0, y, image->columns, 1);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (long) image->columns; x++)
  {
    if (*p > 0)
    {
      f = (*p / MaxVal) * (MaxRGB - q->red);
      if (f + q->red > MaxRGB)
        q->red = MaxRGB;
      else
        q->red += (int) f;
      if ((int) f / 2.0 > q->green)
        q->green = q->blue = 0;
      else
        q->green = q->blue -= (int) (f / 2.0);
    }
    if (*p < 0)
    {
      f = (*p / MaxVal) * (MaxRGB - q->blue);
      if (f + q->blue > MaxRGB)
        q->blue = MaxRGB;
      else
        q->blue += (int) f;
      if ((int) f / 2.0 > q->green)
        q->green = q->red = 0;
      else
        q->green = q->red -= (int) (f / 2.0);
    }
    p++;
    q++;
  }
  if (!SyncImagePixels(image))
    return;
  /*          if (image->previous == (Image *) NULL)
     if (QuantumTick(y,image->rows))
     MagickMonitor(LoadImageText,image->rows-y-1,image->rows); */
  return;
}


/************** READERS ******************/

/* This function reads one block of floats*/
static void ReadBlobFloatsLSB(Image * image, size_t len, float *data)
{
  while (len >= 4)
  {
    *data++ = ReadBlobLSBFloat(image);
    len -= sizeof(float);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

static void ReadBlobFloatsMSB(Image * image, size_t len, float *data)
{
  while (len >= 4)
  {
    *data++ = ReadBlobMSBFloat(image);
    len -= sizeof(float);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

/* This function reads one block of doubles*/
static void ReadBlobDoublesLSB(Image * image, size_t len, double *data)
{
  while (len >= 8)
  {
    *data++ = ReadBlobLSBDouble(image);
    len -= sizeof(double);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

static void ReadBlobDoublesMSB(Image * image, size_t len, double *data)
{
  while (len >= 8)
  {
    *data++ = ReadBlobMSBDouble(image);
    len -= sizeof(double);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}


/* Calculate minimum and maximum from a given block of data */
static void CalcMinMax(Image *image, MATHeader *MATLAB_HDR, magick_uint32_t CellType, unsigned ldblk, void *BImgBuff, double *Min, double *Max)
{
ExtendedSignedIntegralType filepos;
int i, x;
void (*ReadBlobDoublesXXX)(Image * image, size_t len, double *data);
void (*ReadBlobFloatsXXX)(Image * image, size_t len, float *data);
double *dblrow;
float *fltrow;

  if (!strncmp(MATLAB_HDR->EndianIndicator, "IM", 2))
  {    
    ReadBlobDoublesXXX = ReadBlobDoublesLSB;
    ReadBlobFloatsXXX = ReadBlobFloatsLSB;   
  } 
  else		/* MI */
  {    
    ReadBlobDoublesXXX = ReadBlobDoublesMSB;
    ReadBlobFloatsXXX = ReadBlobFloatsMSB;   
  }

  filepos = TellBlob(image);	   /* Please note that file seeking occurs only in the case of doubles */
  for (i = 0; i < (long) MATLAB_HDR->SizeY; i++)
  {
    if (CellType==miDOUBLE)
    {
      ReadBlobDoublesXXX(image, ldblk, (double *)BImgBuff);
      dblrow = (double *)BImgBuff;
      if (i == 0)
      {
        *Min = *Max = *dblrow;
      }
      for (x = 0; x < (long) MATLAB_HDR->SizeX; x++)
      {
        if (*Min > *dblrow)
          *Min = *dblrow;
        if (*Max < *dblrow)
          *Max = *dblrow;
        dblrow++;
      }
    }
    if (CellType==miSINGLE)
    {
      ReadBlobFloatsXXX(image, ldblk, (float *)BImgBuff);
      fltrow = (float *)BImgBuff;
      if (i == 0)
      {
        *Min = *Max = *fltrow;
      }
    for (x = 0; x < (long) MATLAB_HDR->SizeX; x++)
      {
        if (*Min > *fltrow)
          *Min = *fltrow;
        if (*Max < *fltrow)
          *Max = *fltrow;
        fltrow++;
      }
    }
  }
  (void) SeekBlob(image, filepos, SEEK_SET);
}


static void FixSignedValues(PixelPacket *q, int y)
{
  while(y-->0)
  {
     /* Please note that negative values will overflow
        Q=8; MaxRGB=255: <0;127> + 127+1 = <128; 255> 
		       <-1;-128> + 127+1 = <0; 127> */
    q->red += MaxRGB/2 + 1;
    q->green += MaxRGB/ + 1;
    q->blue += MaxRGB/ + 1;
    q++;
  }
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A T L A B i m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMATImage reads an MAT X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMATImage method is:
%
%      Image *ReadMATImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMATImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMATImage(const ImageInfo * image_info, ExceptionInfo * exception)
{
  Image *image,
   *rotated_image;
  PixelPacket *q;
  unsigned int status;
  MATHeader MATLAB_HDR;
  unsigned long size;  
  magick_uint32_t CellType;
  ImportPixelAreaOptions import_options;
  int i;
  long ldblk;
  unsigned char *BImgBuff = NULL;
  double MinVal, MaxVal;
  magick_uint32_t Unknown6;
  unsigned z;
  int logging;
  int sample_size;

  unsigned long (*ReadBlobXXXLong)(Image *image);
  unsigned short (*ReadBlobXXXShort)(Image *image);
  void (*ReadBlobDoublesXXX)(Image * image, size_t len, double *data);
  void (*ReadBlobFloatsXXX)(Image * image, size_t len, float *data);


  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter"); 

  /*
     Open image file.
   */
  image = AllocateImage(image_info);

  status = OpenBlob(image_info, image, ReadBinaryBlobMode, exception);
  if (status == False)
    ThrowReaderException(FileOpenError, UnableToOpenFile, image);
  /*
     Read MATLAB image.
   */
  (void) ReadBlob(image, 124, &MATLAB_HDR.identific);
  MATLAB_HDR.Version = ReadBlobLSBShort(image);
  (void) ReadBlob(image, 2, &MATLAB_HDR.EndianIndicator);

  ImportPixelAreaOptionsInit(&import_options);

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),"  Endian %c%c",
        MATLAB_HDR.EndianIndicator[0],MATLAB_HDR.EndianIndicator[1]);
  if (!strncmp(MATLAB_HDR.EndianIndicator, "IM", 2))
  {
    ReadBlobXXXLong = ReadBlobLSBLong;
    ReadBlobXXXShort = ReadBlobLSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesLSB;
    ReadBlobFloatsXXX = ReadBlobFloatsLSB;
    import_options.endian = LSBEndian;
  } 
  else if (!strncmp(MATLAB_HDR.EndianIndicator, "MI", 2))
  {
    ReadBlobXXXLong = ReadBlobMSBLong;
    ReadBlobXXXShort = ReadBlobMSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesMSB;
    ReadBlobFloatsXXX = ReadBlobFloatsMSB;
    import_options.endian = MSBEndian;
  }
  else 
    goto MATLAB_KO;    /* unsupported endian */

  if (strncmp(MATLAB_HDR.identific, "MATLAB", 6))
MATLAB_KO: ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

//NEXT_IMAGE:
  MATLAB_HDR.DataType = ReadBlobXXXLong(image);
  MATLAB_HDR.ObjectSize = ReadBlobXXXLong(image);
  if (MATLAB_HDR.DataType != miMATRIX)
    goto MATLAB_KO;
 
  MATLAB_HDR.unknown1 = ReadBlobXXXLong(image);
  MATLAB_HDR.unknown2 = ReadBlobXXXLong(image);  

  MATLAB_HDR.unknown5 = ReadBlobXXXLong(image);
  MATLAB_HDR.StructureClass = MATLAB_HDR.unknown5 & 0xFF;
  MATLAB_HDR.StructureFlag = (MATLAB_HDR.unknown5>>8) & 0xFF;  

  MATLAB_HDR.unknown3 = ReadBlobXXXLong(image);
  MATLAB_HDR.unknown4 = ReadBlobXXXLong(image);
  MATLAB_HDR.DimFlag = ReadBlobXXXLong(image);
  MATLAB_HDR.SizeX = ReadBlobXXXLong(image);
  MATLAB_HDR.SizeY = ReadBlobXXXLong(image);  
   

  switch(MATLAB_HDR.DimFlag)
  {
    case  8: z=1; break;	       /* 2D matrix*/
    case 12: z=ReadBlobXXXLong(image); /* 3D matrix RGB*/
	     Unknown6 = ReadBlobXXXLong(image);
	     if(z!=3) ThrowReaderException(CoderError, MultidimensionalMatricesAreNotSupported,
                         image);
	     break;
    default: ThrowReaderException(CoderError, MultidimensionalMatricesAreNotSupported,
                         image);
  }  

  MATLAB_HDR.Flag1 = ReadBlobXXXShort(image);
  MATLAB_HDR.NameFlag = ReadBlobXXXShort(image);

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "MATLAB_HDR.StructureClass %d",MATLAB_HDR.StructureClass);
  if (MATLAB_HDR.StructureClass != mxCHAR_CLASS && 
      MATLAB_HDR.StructureClass != mxSINGLE_CLASS &&	     /* float + complex float */
      MATLAB_HDR.StructureClass != mxDOUBLE_CLASS &&	     /* double + complex double */
      MATLAB_HDR.StructureClass != mxINT8_CLASS &&
      MATLAB_HDR.StructureClass != mxUINT8_CLASS &&          /* uint8 + uint8 3D */
      MATLAB_HDR.StructureClass != mxINT16_CLASS &&
      MATLAB_HDR.StructureClass != mxUINT16_CLASS &&	     /* uint16 + uint16 3D */
      MATLAB_HDR.StructureClass != mxINT32_CLASS &&
      MATLAB_HDR.StructureClass != mxUINT32_CLASS &&	     /* uint32 + uint32 3D */
      MATLAB_HDR.StructureClass != mxINT64_CLASS &&
      MATLAB_HDR.StructureClass != mxUINT64_CLASS)	     /* uint64 + uint64 3D */
    ThrowReaderException(CoderError,UnsupportedCellTypeInTheMatrix,image);

  switch (MATLAB_HDR.NameFlag)
  {
    case 0:
      size = ReadBlobXXXLong(image);	/* Object name string size */
      size = 4 * (long) ((size + 3 + 1) / 4);
      (void) SeekBlob(image, size, SEEK_CUR);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
      (void) ReadBlob(image, 4, &size); /* Object name string */
      break;
    default:
      goto MATLAB_KO;
  }

  CellType = ReadBlobXXXLong(image);    /* Additional object type */
  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),
          "MATLAB_HDR.CellType: %d",CellType);
  
  (void) ReadBlob(image, 4, &size);     /* data size */

    /* Image is gray when no complex flag is set and 2D Matrix */
  image->is_grayscale = (MATLAB_HDR.DimFlag==8) && 
           ((MATLAB_HDR.StructureFlag & FLAG_COMPLEX) == 0);

  switch (CellType)
  {
    case miINT8:
    case miUINT8:
      sample_size = 8;
      image->depth = Min(QuantumDepth,8);         /* Byte type cell */
      import_options.sample_type = UnsignedQuantumSampleType;
      ldblk = (long) MATLAB_HDR.SizeX;      
      break;
    case miINT16:
    case miUINT16:
      sample_size = 16;
      image->depth = Min(QuantumDepth,16);        /* Word type cell */
      ldblk = (long) (2 * MATLAB_HDR.SizeX);
      import_options.sample_type = UnsignedQuantumSampleType;       
      break;
    case miINT32:
    case miUINT32:
      sample_size = 32;
      image->depth = Min(QuantumDepth,32);        /* Dword type cell */
      ldblk = (long) (4 * MATLAB_HDR.SizeX);      
      import_options.sample_type = UnsignedQuantumSampleType;      
      break;
    case miINT64:
    case miUINT64:
      sample_size = 64;
      image->depth = Min(QuantumDepth,32);        /* Qword type cell */
      ldblk = (long) (8 * MATLAB_HDR.SizeX);      
      import_options.sample_type = UnsignedQuantumSampleType;      
      break;   
   case miSINGLE:
      sample_size = 32;
      image->depth = Min(QuantumDepth,32);        /* double type cell */
      import_options.sample_type = FloatQuantumSampleType;
//      if (sizeof(float) != 4)
//        ThrowReaderException(CoderError, IncompatibleSizeOfFloat, image);
      if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
      {                         /* complex float type cell */
      }
      ldblk = (long) (4 * MATLAB_HDR.SizeX);
      break;
    case miDOUBLE:
      sample_size = 64; 
      image->depth = Min(QuantumDepth,32);        /* double type cell */
      import_options.sample_type = FloatQuantumSampleType;
      if (sizeof(double) != 8)
        ThrowReaderException(CoderError, IncompatibleSizeOfDouble, image);
      if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
      {                         /* complex double type cell */        
      }
      ldblk = (long) (8 * MATLAB_HDR.SizeX);
      break;
    default:
      ThrowReaderException(CoderError, UnsupportedCellTypeInTheMatrix, image)
  }

  image->columns = MATLAB_HDR.SizeX;
  image->rows = MATLAB_HDR.SizeY;
  image->colors = 1l >> 8;
  if (image->columns == 0 || image->rows == 0)
    goto MATLAB_KO;

  /* ----- Create gray palette ----- */

  if (CellType==miUINT8 && z!=3)
  {
    image->colors = 256;
    if (!AllocateImageColormap(image, image->colors))
    {
 NoMemory:ThrowReaderException(ResourceLimitError, MemoryAllocationFailed,
                           image)}

    for (i = 0; i < (long) image->colors; i++)
    {
      image->colormap[i].red = ScaleCharToQuantum(i);
      image->colormap[i].green = ScaleCharToQuantum(i);
      image->colormap[i].blue = ScaleCharToQuantum(i);
    }
  }

  /*
    If ping is true, then only set image size and colors without
    reading any image data.
  */
  if (image_info->ping)
    {
      unsigned long temp = image->columns;
      image->columns = image->rows;
      image->rows = temp;
      goto done_reading;
    }  

  /* ----- Load raster data ----- */
  BImgBuff = MagickAllocateMemory(unsigned char *,ldblk);    /* Ldblk was set in the check phase */
  if (BImgBuff == NULL)
    goto NoMemory;

  MinVal = 0;
  MaxVal = 0;
  if (CellType==miDOUBLE || CellType==miSINGLE)        /* Find Min and Max Values for floats */
  {
    CalcMinMax(image, &MATLAB_HDR, CellType, ldblk, BImgBuff, &import_options.double_minvalue, &import_options.double_maxvalue);
  }

  /* Main loop for reading all scanlines */
  if(z==1) z=0; /* read grey scanlines */
		/* else read color scanlines */
  do
  {
  for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
    {
      q=SetImagePixels(image,0,MATLAB_HDR.SizeY-i-1,image->columns,1);
      if (q == (PixelPacket *)NULL) goto ExitLoop;
      (void)ReadBlob(image, ldblk, (char *)BImgBuff);	  
      (void)ImportImagePixelArea(image,z2qtype[z],sample_size,BImgBuff,&import_options,0);
      if (z<=1 &&			 // fix only during a last pass z==0 || z==1
	      (CellType==miINT8 || CellType==miINT16 || CellType==miINT32 || CellType==miINT64))
	FixSignedValues(q,MATLAB_HDR.SizeX);
      if (!SyncImagePixels(image)) goto ExitLoop;
    }
  } while(z-- >= 2);
ExitLoop:


  /* Read complex part of numbers here */
  if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
  {        /* Find Min and Max Values for complex parts of floats */
    CellType = ReadBlobXXXLong(image);    /* Additional object type */
    i = ReadBlobXXXLong(image);           /* size of a complex part - toss away*/

    if (CellType==miDOUBLE || CellType==miSINGLE)
    {
      CalcMinMax(image, &MATLAB_HDR, CellType, ldblk, BImgBuff, &MinVal, &MaxVal);      
    }

    if (CellType==miDOUBLE)
      for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
      {
        ReadBlobDoublesXXX(image, ldblk, (double *)BImgBuff);
        InsertComplexDoubleRow((double *)BImgBuff, i, image, MinVal, MaxVal);
      }

    if (CellType==miSINGLE)
      for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
      {
        ReadBlobFloatsXXX(image, ldblk, (float *)BImgBuff);
        InsertComplexFloatRow((float *)BImgBuff, i, image, MinVal, MaxVal);
      }    
  }

    /* Image is gray when no complex flag is set and 2D Matrix AGAIN!!! */
  image->is_grayscale = (MATLAB_HDR.DimFlag==8) && 
           ((MATLAB_HDR.StructureFlag & FLAG_COMPLEX) == 0);


  /*  Rotate image. */
  rotated_image = RotateImage(image, 90.0, exception);
  if (rotated_image != (Image *) NULL)
  {
    /* Remove page offsets added by RotateImage */
    rotated_image->page.x=0;
    rotated_image->page.y=0;
    DestroyImage(image);
    image = rotated_image;
  }

done_reading:

  MagickFreeMemory(BImgBuff);
  CloseBlob(image);

  if (logging) (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return (image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A T L A B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteMATLABImage writes an Matlab matrix to a file.  
%
%  The format of the WriteMATLABImage method is:
%
%      unsigned int WriteMATLABImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteMATLABImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
*/
static unsigned int WriteMATLABImage(const ImageInfo *image_info,Image *image)
{
  long y;
  unsigned z;
  const PixelPacket *q;
  unsigned int status;
  int logging;
  unsigned long DataSize;
  char padding;
  char MATLAB_HDR[0x84];
  time_t current_time;
  const struct tm *t;
  unsigned char *pixels;
  int is_gray;

  current_time=time((time_t *) NULL);
  t=localtime(&current_time);

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter MAT");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  pixels=MagickAllocateMemory(unsigned char *,image->rows);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  is_gray = IsGrayImage(image,&image->exception);
  z = is_gray ? 0 : 3;

  /*
    Store MAT header.
  */
  DataSize = image->rows /*Y*/ * image->columns /*X*/;
  if(!is_gray) DataSize *= 3 /*Z*/;
  padding=((unsigned char)(DataSize-1) & 0x7) ^ 0x7;

  (void) memset(MATLAB_HDR,' ',Min(sizeof(MATLAB_HDR),124));
  FormatString(MATLAB_HDR,"MATLAB 5.0 MAT-file, Platform: %s, Created on: %s %s %2d %2d:%2d:%2d %d",
    OsDesc,
    DayOfWTab[t->tm_wday],
    MonthsTab[t->tm_mon],
    t->tm_mday,
    t->tm_hour,t->tm_min,t->tm_sec,
    t->tm_year+1900);
  MATLAB_HDR[0x7C]=0;
  MATLAB_HDR[0x7D]=1;
  MATLAB_HDR[0x7E]='I';
  MATLAB_HDR[0x7F]='M';
  MATLAB_HDR[0x80]=0xE;
  MATLAB_HDR[0x81]=0;  MATLAB_HDR[0x82]=0;   MATLAB_HDR[0x83]=0;
  (void) WriteBlob(image,sizeof(MATLAB_HDR),MATLAB_HDR);

  (void) WriteBlobLSBLong(image, DataSize + 56l + padding); /* 0x84 */
  (void) WriteBlobLSBLong(image, 0x6); /* 0x88 */
  (void) WriteBlobLSBLong(image, 0x8); /* 0x8C */
  (void) WriteBlobLSBLong(image, 0x6); /* 0x90 */  
  (void) WriteBlobLSBLong(image, 0);   
  (void) WriteBlobLSBLong(image, 0x5); /* 0x98 */
  (void) WriteBlobLSBLong(image, is_gray?0x8:0xC); /* 0x9C - DimFlag */
  (void) WriteBlobLSBLong(image, image->rows);    /* x: 0xA0 */  
  (void) WriteBlobLSBLong(image, image->columns); /* y: 0xA4 */  
  if(!is_gray)
  {
    (void) WriteBlobLSBLong(image, 3); /* z: 0xA8 */  
    (void) WriteBlobLSBLong(image, 0);
  }
  (void) WriteBlobLSBShort(image, 1);  /* 0xB0 */  
  (void) WriteBlobLSBShort(image, 1);  /* 0xB2 */
  (void) WriteBlobLSBLong(image, 'M'); /* 0xB4 */
  (void) WriteBlobLSBLong(image, 0x2); /* 0xB8 */  
  (void) WriteBlobLSBLong(image, DataSize); /* 0xBC */

  /*
    Store image data.
  */
  {
    magick_uint64_t
      progress_span;

    magick_int64_t
      progress_quantum;

    progress_span = image->columns;
    if(!is_gray) progress_span *= 3;
    progress_quantum = 0;

    do
    {
      for (y=0; y<(long)image->columns; y++)
      {
        progress_quantum++;
        q = AcquireImagePixels(image,y,0,1,image->rows,&image->exception);
        (void) ExportImagePixelArea(image,z2qtype[z],8,pixels,0,0);
        (void) WriteBlob(image,image->rows,pixels);
        if (QuantumTick(progress_quantum,progress_span))
          if (!MagickMonitor(SaveImageText,progress_quantum,progress_span,&image->exception))
            goto BreakAll;
      }    
    } while(z-- >= 2);
  }
BreakAll:

  while(padding-->0) (void) WriteBlobByte(image,0);

  status=True;

  CloseBlob(image);
  MagickFreeMemory(pixels);

  if (logging)
    (void)LogMagickEvent(CoderEvent,GetMagickModule(),"return MAT");
  
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMATImage adds attributes for the MAT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMATImage method is:
%
%      RegisterMATImage(void)
%
*/
ModuleExport void RegisterMATImage(void)
{
  MagickInfo * entry;

  entry = SetMagickInfo("MAT");
  entry->decoder = (DecoderHandler) ReadMATImage;
  entry->encoder = (EncoderHandler) WriteMATLABImage;
  entry->seekable_stream = True;
  entry->description = AcquireString("MATLAB image format");
  entry->module = AcquireString("MAT");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMATImage removes format registrations made by the
%  MAT module from the list of supported formats.
%
%  The format of the UnregisterMATImage method is:
%
%      UnregisterMATImage(void)
%
*/
ModuleExport void UnregisterMATImage(void)
{
  (void) UnregisterMagickInfo("MAT");
}
