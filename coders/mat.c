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
%  2D matrices:       X*Y   byte, word, double, complex
%  3D matrices: only X*Y*3  byte, word
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/magick.h"
#include "magick/shear.h"
#include "magick/transform.h"
#include "magick/utility.h"

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
  unsigned long unknown0;
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


/* Update one byte row inside image. */
static void InsertByteRow(unsigned char *p, int y, Image * image, int channel)
{
  int x;
  register PixelPacket *q;
  IndexPacket index;
  register IndexPacket *indexes;

                       /* Convert PseudoColor scanline. */  
  q = SetImagePixels(image, 0, y, image->columns, 1);
  if (q == (PixelPacket *) NULL) return;

  switch(channel)
    {
    case 0:          
      indexes = GetIndexes(image);

      for (x = 0; x < (long) image->columns; x++)
        {
        index = (IndexPacket) (*p);
        VerifyColormapIndex(image, index);
        indexes[x] = index;
        *q++ = image->colormap[index];
        p++;
        }
      break;
    case 1:
       for (x = 0; x < (long) image->columns; x++)        
         { 
         q->blue = ScaleCharToQuantum(*p);
         p++;
         q++;
         }
       break;
    case 2:
       for (x = 0; x < (long) image->columns; x++)        
         {
         q->green = ScaleCharToQuantum(*p);
         p++;
         q++;
         }
       break;
    case 3:
       for (x = 0; x < (long) image->columns; x++)        
	 {         
         q->red = ScaleCharToQuantum(*p);
         q->opacity = OpaqueOpacity;
         p++;
         q++;
         }
       break;
    }
  if (!SyncImagePixels(image))
    return;
        /*           if (image->previous == (Image *) NULL)
           if (QuantumTick(y,image->rows))
           ProgressMonitor(LoadImageText,image->rows-y-1,image->rows); */
      
  return;
}


/* Update one word (uint16) row inside image. */
static void InsertWordRow(magick_uint16_t *p, int y, Image * image, int channel)
{
  int x;
  register PixelPacket *q;

  q = SetImagePixels(image, 0, y, image->columns, 1);
  if (q == (PixelPacket *) NULL) return;

  switch(channel)
    {
    case 0: 
      for (x = 0; x < (long) image->columns; x++)
        {
        q->red =
          q->green =
          q->blue = ScaleShortToQuantum(*p);
        q->opacity = OpaqueOpacity;
        p++;
        q++;
        }
       break;
    case 1:
       for (x = 0; x < (long) image->columns; x++)        
         { 
         q->blue = ScaleShortToQuantum(*p);
         p++;
         q++;
         }
       break;
    case 2:
       for (x = 0; x < (long) image->columns; x++)        
         {
         q->green = ScaleShortToQuantum(*p);
         p++;
         q++;
         }
       break;
    case 3:
       for (x = 0; x < (long) image->columns; x++)        
	 {
         q->red = ScaleShortToQuantum(*p);
         p++;
         q++;
         }
       break;
    }   
         
  if (!SyncImagePixels(image))
    return;
        /*          if (image->previous == (Image *) NULL)
           if (QuantumTick(y,image->rows))
           MagickMonitor(LoadImageText,image->rows-y-1,image->rows); */
  return; 
}


/* Update one double organised row inside image. */
static void InsertFloatRow(double *p, int y, Image * image, double MinVal, double MaxVal)
{
  double f;
  int x;
  register PixelPacket *q;

  if (MinVal >= MaxVal)
    MaxVal = MinVal + 1;

  q = SetImagePixels(image, 0, y, image->columns, 1);
  if (q == (PixelPacket *) NULL)
    return;
  for (x = 0; x < (long) image->columns; x++)
  {
    f = (double) MaxRGB *(*p - MinVal) / (MaxVal - MinVal);
    q->red = 
      q->green = 
      q->blue = RoundSignedToQuantum(f);
    q->opacity = OpaqueOpacity;
  
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

static void InsertComplexFloatRow(double *p, int y, Image * image, double MinVal,
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

/* This function reads one block of unsigned shortS*/
static void ReadBlobWordLSB(Image * image, size_t len, unsigned short *data)
{
  while (len >= 2)
  {
    *data++ = ReadBlobLSBShort(image);
    len -= 2;
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

static void ReadBlobWordMSB(Image * image, size_t len, unsigned short *data)
{
  while (len >= 2)
  {
    *data++ = ReadBlobMSBShort(image);
    len -= 2;
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

static void ReadBlobDoublesLSB(Image * image, size_t len, double *data)
{
  while (len >= 8)
  {
    *data++ = ReadBlobLSBdouble(image);
    len -= sizeof(double);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
}

static void ReadBlobDoublesMSB(Image * image, size_t len, double *data)
{
  while (len >= 8)
  {
    *data++ = ReadBlobMSBdouble(image);
    len -= sizeof(double);
  }
  if (len > 0)
    (void) SeekBlob(image, len, SEEK_CUR);
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
  unsigned int status;
  MATHeader MATLAB_HDR;
  unsigned long size;
  ExtendedSignedIntegralType filepos;
  unsigned long CellType;
  int i,
    x;
  long ldblk;
  unsigned char *BImgBuff = NULL;
  double MinVal, MaxVal,
   *dblrow;
  unsigned long z, Unknown5;
  int logging;

  unsigned long (*ReadBlobXXXLong)(Image *image);
  unsigned short (*ReadBlobXXXShort)(Image *image);
  void (*ReadBlobDoublesXXX)(Image * image, size_t len, double *data);


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

  if (logging) (void)LogMagickEvent(CoderEvent,GetMagickModule(),"  Endian %c%c",
        MATLAB_HDR.EndianIndicator[0],MATLAB_HDR.EndianIndicator[1]);
  if (!strncmp(MATLAB_HDR.EndianIndicator, "IM", 2))
  {
    ReadBlobXXXLong = ReadBlobLSBLong;
    ReadBlobXXXShort = ReadBlobLSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesLSB;
  } 
  else if (!strncmp(MATLAB_HDR.EndianIndicator, "MI", 2))
  {
    ReadBlobXXXLong = ReadBlobMSBLong;
    ReadBlobXXXShort = ReadBlobMSBShort;
    ReadBlobDoublesXXX = ReadBlobDoublesMSB;
  }
  else 
    goto MATLAB_KO;    /* unsupported endian */

  if (strncmp(MATLAB_HDR.identific, "MATLAB", 6))
  MATLAB_KO:ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  MATLAB_HDR.unknown0 = ReadBlobXXXLong(image);
  MATLAB_HDR.ObjectSize = ReadBlobXXXLong(image);
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
    
  if (MATLAB_HDR.unknown0 != 0x0E)
    goto MATLAB_KO;

  switch(MATLAB_HDR.DimFlag)
  {
    case  8: z=1; break;	       /* 2D matrix*/
    case 12: z=ReadBlobXXXLong(image); /* 3D matrix RGB*/
	     Unknown5 = ReadBlobXXXLong(image);
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
      MATLAB_HDR.StructureClass != mxDOUBLE_CLASS &&	     /* double / complex double */      
      MATLAB_HDR.StructureClass != mxUINT8_CLASS &&           /* uint8 3D */
      MATLAB_HDR.StructureClass != mxUINT16_CLASS)	     /* uint16 3D */
    goto MATLAB_KO;

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
  /* fprintf(stdout,"Cell type:%d\n",CellType); */
  (void) ReadBlob(image, 4, &size);     /* data size */

    /* Image is gray when no complex flag is set and 2D Matrix */
  image->is_grayscale = (MATLAB_HDR.DimFlag==8) && 
           ((MATLAB_HDR.StructureFlag & FLAG_COMPLEX) == 0);

  switch (CellType)
  {
    case miUINT8:
      image->depth = Min(QuantumDepth,8);         /* Byte type cell */
      ldblk = (long) MATLAB_HDR.SizeX;      
      if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
        goto MATLAB_KO;
      break;
    case miUINT16:
      image->depth = Min(QuantumDepth,16);        /* Word type cell */
      ldblk = (long) (2 * MATLAB_HDR.SizeX);      
      if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
        goto MATLAB_KO;
      break;   
    case miDOUBLE:
      image->depth = Min(QuantumDepth,32);        /* double type cell */
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

  if (CellType == miUINT8  && z!=3)
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
  if (CellType == miDOUBLE)        /* Find Min and Max Values for floats */
  {
    filepos = TellBlob(image);
    for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
    {      
      ReadBlobDoublesXXX(image, ldblk, (double *) BImgBuff);      
      dblrow = (double *) BImgBuff;
      if (i == 0)
      {
        MinVal = MaxVal = *dblrow;
      }
      for (x = 0; x < (long) MATLAB_HDR.SizeX; x++)
      {
        if (MinVal > *dblrow)
          MinVal = *dblrow;
        if (MaxVal < *dblrow)
          MaxVal = *dblrow;
        dblrow++;
      }
    }
    (void) SeekBlob(image, filepos, SEEK_SET);
  }

  /* Main loop for reading all scanlines */
  if(z==1)
  {  
    for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
    {
      switch (CellType)
      {
        case miUINT8:	/* Byte order */
          (void) ReadBlob(image, ldblk, (char *) BImgBuff);
          InsertByteRow(BImgBuff, i, image,0);
          break;
        case miUINT16:	/* Word order */
          if (MATLAB_HDR.EndianIndicator[0]=='I')
            ReadBlobWordLSB(image, ldblk, (unsigned short *) BImgBuff);
          else
            ReadBlobWordMSB(image, ldblk, (unsigned short *) BImgBuff);
          InsertWordRow((magick_uint16_t *)BImgBuff, i, image,0);
          break;
        case miDOUBLE:
          ReadBlobDoublesXXX(image, ldblk, (double *) BImgBuff);          
          InsertFloatRow((double *) BImgBuff, i, image, MinVal, MaxVal);
          break;      
      }
    }
  }
  else
   while(z>=1)
   {
   for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
    {
      switch (CellType)
      {
        case miUINT8:	/* Byte order */
          (void) ReadBlob(image, ldblk, (char *) BImgBuff);
          InsertByteRow(BImgBuff, i, image, z);
          break;
        case miUINT16: 	/* Word order */          
          if (MATLAB_HDR.EndianIndicator[0]=='I')
            ReadBlobWordLSB(image, ldblk, (unsigned short *) BImgBuff);
          else
            ReadBlobWordMSB(image, ldblk, (unsigned short *) BImgBuff);
          InsertWordRow((magick_uint16_t *)BImgBuff, i, image, z);
          break;
        case miDOUBLE:
          goto MATLAB_KO;
          /* ReadBlobDoublesXXX(image, ldblk, (double *) BImgBuff);
          InsertFloatRow((double *) BImgBuff, i, image, MinVal, MaxVal);
          break; */
      }
    }

   z--;
   } 

  /* Read complex part of numbers here */
  if (MATLAB_HDR.StructureFlag & FLAG_COMPLEX)
  {
    if (CellType == miDOUBLE)        /* Find Min and Max Values for complex parts of floats */
    {
      filepos = TellBlob(image);
      for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
      {        
        ReadBlobDoublesXXX(image, ldblk, (double *) BImgBuff);        

        dblrow = (double *) BImgBuff;
        if (i == 0)
        {
          MinVal = MaxVal = *dblrow;
        }
        for (x = 0; x < (long) MATLAB_HDR.SizeX; x++)
        {
          if (MinVal > *dblrow)
            MinVal = *dblrow;
          if (MaxVal < *dblrow)
            MaxVal = *dblrow;
          dblrow++;
        }
      }
      (void) SeekBlob(image, filepos, SEEK_SET);

      for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
      {
        ReadBlobDoublesXXX(image, ldblk, (double *) BImgBuff);        
        InsertComplexFloatRow((double *) BImgBuff, i, image, MinVal, MaxVal);
      }
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
    image = FlopImage(rotated_image, exception);
    if (image == NULL)
      image = rotated_image;    /* Obtain something if flop operation fails */
    else
      DestroyImage(rotated_image);
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
  register long x;
  register const PixelPacket *q;
  unsigned int status;
  int logging;
  unsigned long DataSize;
  char padding;
  char MATLAB_HDR[0x84];
  time_t current_time;
  const struct tm *t;

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

  /*
    Store MAT header.
  */
  DataSize = image->rows /*Y*/ * image->columns /*X*/ * 3 /*Z*/;
  padding=((unsigned char)(DataSize-1) & 0x7) ^ 0x7;

  (void) memset(MATLAB_HDR,' ',124);
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
  (void) WriteBlobLSBLong(image, 0xC); /* 0x9C */      
  (void) WriteBlobLSBLong(image, image->rows);    /* x: 0xA0 */  
  (void) WriteBlobLSBLong(image, image->columns); /* y: 0xA4 */  
  (void) WriteBlobLSBLong(image, 3);              /* z: 0xA8 */  
  (void) WriteBlobLSBLong(image, 0);
  (void) WriteBlobLSBShort(image, 1);  /* 0xB0 */  
  (void) WriteBlobLSBShort(image, 1);  /* 0xB2 */
  (void) WriteBlobLSBLong(image, 'M'); /* 0xB4 */
  (void) WriteBlobLSBLong(image, 0x2); /* 0xB8 */  
  (void) WriteBlobLSBLong(image, DataSize); /* 0xBC */

  /*
    Store image data.
  */
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);    
    for (x=0; x < (long) image->rows; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(q->red));
      q++;
    }
  }
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);
    for (x=0; x < (long) image->rows; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(q->green));
      q++;
    }
  }
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);
    for (x=0; x < (long) image->rows; x++)
    {
      (void) WriteBlobByte(image,ScaleQuantumToChar(q->blue));
      q++;
    }
  }

  while(padding-->0) (void) WriteBlobByte(image,0);

  status=True;

  CloseBlob(image);
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
