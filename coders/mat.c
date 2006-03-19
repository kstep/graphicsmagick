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
%                                2001 - 2006                                  %
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
  char identific[125];
  char idx[3];
  unsigned long unknown0;
  unsigned long ObjectSize;
  unsigned long unknown1;
  unsigned long unknown2;

  unsigned long StructureFlag;
  unsigned long unknown3;
  unsigned long unknown4;
  unsigned long DimFlag;

  unsigned long SizeX;
  unsigned long SizeY;
  unsigned short Flag1;
  unsigned short NameFlag;
}
MATHeader;

char *MonthsTab[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
char *DayOfWTab[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
#ifdef __WIN32__
 char *OsDesc="PCWIN";
#else
 char *OsDesc="LNX86";
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
    miMATRIX			/* MATLAB array */
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


/* Update one word row inside image. */
static void InsertWordRow(unsigned char *p, int y, Image * image, int channel)
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
          q->blue = ScaleShortToQuantum(*(unsigned short *) p);    
        q->opacity = OpaqueOpacity;
        p += 2;
        q++;
        }
       break;
    case 1:
       for (x = 0; x < (long) image->columns; x++)        
         { 
         q->blue = ScaleShortToQuantum(*(unsigned short *) p);
         p += 2;
         q++;
         }
       break;
    case 2:
       for (x = 0; x < (long) image->columns; x++)        
         {
         q->green = ScaleShortToQuantum(*(unsigned short *) p);
         p += 2;
         q++;
         }
       break;
    case 3:
       for (x = 0; x < (long) image->columns; x++)        
	 {
         q->red = ScaleShortToQuantum(*(unsigned short *) p);
         p += 2;
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

static double ReadBlobLSBdouble(Image * image)
{
  typedef union
  {
    double d;
    char chars[8];
  }
  dbl;
  static unsigned long lsb_first = 1;
  dbl buffer;
  char c;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image, 8, (unsigned char *) &buffer) == 0)
    return (0.0);
  if (*(char *) &lsb_first == 1)
    return (buffer.d);

  c = buffer.chars[0];
  buffer.chars[0] = buffer.chars[7];
  buffer.chars[7] = c;
  c = buffer.chars[1];
  buffer.chars[1] = buffer.chars[6];
  buffer.chars[6] = c;
  c = buffer.chars[2];
  buffer.chars[2] = buffer.chars[5];
  buffer.chars[5] = c;
  c = buffer.chars[3];
  buffer.chars[3] = buffer.chars[4];
  buffer.chars[4] = c;
  return (buffer.d);
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
  (void) ReadBlob(image, 125, &MATLAB_HDR.identific);
  (void) ReadBlob(image, 3, &MATLAB_HDR.idx);
  MATLAB_HDR.unknown0 = ReadBlobLSBLong(image);
  MATLAB_HDR.ObjectSize = ReadBlobLSBLong(image);
  MATLAB_HDR.unknown1 = ReadBlobLSBLong(image);
  MATLAB_HDR.unknown2 = ReadBlobLSBLong(image);
  MATLAB_HDR.StructureFlag = ReadBlobLSBLong(image);
  MATLAB_HDR.unknown3 = ReadBlobLSBLong(image);
  MATLAB_HDR.unknown4 = ReadBlobLSBLong(image);
  MATLAB_HDR.DimFlag = ReadBlobLSBLong(image);
  MATLAB_HDR.SizeX = ReadBlobLSBLong(image);
  MATLAB_HDR.SizeY = ReadBlobLSBLong(image);  

  if (strncmp(MATLAB_HDR.identific, "MATLAB", 6))
  MATLAB_KO:ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if (strncmp(MATLAB_HDR.idx, "\1IM", 3))
    goto MATLAB_KO;
  if (MATLAB_HDR.unknown0 != 0x0E)
    goto MATLAB_KO;

  switch(MATLAB_HDR.DimFlag)
  {
    case  8: z=1; break;	       /* 2D matrix*/
    case 12: z=ReadBlobLSBLong(image); /* 3D matrix RGB*/
	     Unknown5= ReadBlobLSBLong(image);
	     if(z!=3) ThrowReaderException(CoderError, MultidimensionalMatricesAreNotSupported,
                         image);
	     break;
    default: ThrowReaderException(CoderError, MultidimensionalMatricesAreNotSupported,
                         image);
  }  

  MATLAB_HDR.Flag1 = ReadBlobLSBShort(image);
  MATLAB_HDR.NameFlag = ReadBlobLSBShort(image);

  /* printf("MATLAB_HDR.StructureFlag %ld\n",MATLAB_HDR.StructureFlag); */
  if (MATLAB_HDR.StructureFlag != mxCHAR_CLASS && 
      MATLAB_HDR.StructureFlag != mxDOUBLE_CLASS && 
      MATLAB_HDR.StructureFlag != (0x800|mxDOUBLE_CLASS) &&  /* complex double */
      MATLAB_HDR.StructureFlag != mxUINT8_CLASS &&           /* uint8 3D */
      MATLAB_HDR.StructureFlag != mxUINT16_CLASS)	     /* uint16 3D */
    goto MATLAB_KO;

  switch (MATLAB_HDR.NameFlag)
  {
    case 0:
      (void) ReadBlob(image, 4, &size); /* Object name string */
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

  CellType = ReadBlobLSBLong(image);    /* Additional object type */
  /* fprintf(stdout,"Cell type:%ld\n",CellType); */
  (void) ReadBlob(image, 4, &size);     /* data size */

  switch (CellType)
  {
    case miUINT8:
      image->depth = Min(QuantumDepth,8);         /* Byte type cell */
      ldblk = (long) MATLAB_HDR.SizeX;
      if (MATLAB_HDR.StructureFlag == 0x806)
        goto MATLAB_KO;
      break;
    case miUINT16:
      image->depth = Min(QuantumDepth,16);        /* Word type cell */
      ldblk = (long) (2 * MATLAB_HDR.SizeX);
      if (MATLAB_HDR.StructureFlag == 0x806)
        goto MATLAB_KO;
      break;
    case miDOUBLE:
      image->depth = Min(QuantumDepth,32);        /* double type cell */
      if (sizeof(double) != 8)
        ThrowReaderException(CoderError, IncompatibleSizeOfDouble, image);
      if (MATLAB_HDR.StructureFlag == 0x806)
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
      ReadBlobDoublesLSB(image, ldblk, (double *) BImgBuff);
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
          ReadBlobWordLSB(image, ldblk, (unsigned short *) BImgBuff);
          InsertWordRow(BImgBuff, i, image,0);
          break;
        case miDOUBLE:
          ReadBlobDoublesLSB(image, ldblk, (double *) BImgBuff);
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
          ReadBlobWordLSB(image, ldblk, (unsigned short *) BImgBuff);
          InsertWordRow(BImgBuff, i, image, z);
          break;
        case miDOUBLE:
          goto MATLAB_KO;
          /* ReadBlobDoublesLSB(image, ldblk, (double *) BImgBuff);
          InsertFloatRow((double *) BImgBuff, i, image, MinVal, MaxVal);
          break; */
      }
    }

   z--;
   } 

  /* Read complex part of numbers here */
  if (MATLAB_HDR.StructureFlag == 0x806)
  {
    if (CellType == miDOUBLE)        /* Find Min and Max Values for complex parts of floats */
    {
      filepos = TellBlob(image);
      for (i = 0; i < (long) MATLAB_HDR.SizeY; i++)
      {
        ReadBlobDoublesLSB(image, ldblk, (double *) BImgBuff);
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
        ReadBlobDoublesLSB(image, ldblk, (double *) BImgBuff);
        InsertComplexFloatRow((double *) BImgBuff, i, image, MinVal, MaxVal);
      }
    }
  }

  if (BImgBuff != NULL)
  {
    MagickFreeMemory(BImgBuff);
    BImgBuff = NULL;
  }
  CloseBlob(image);

  /*  Rotate image. */
  rotated_image = RotateImage(image, 90.0, exception);
  if (rotated_image != (Image *) NULL)
  {
    DestroyImage(image);
    image = FlopImage(rotated_image, exception);
    if (image == NULL)
      image = rotated_image;    /* Obtain something if flop operation fails */
    else
      DestroyImage(rotated_image);
  }

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

  memset(MATLAB_HDR,' ',124);
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
  WriteBlob(image,sizeof(MATLAB_HDR),MATLAB_HDR);

  WriteBlobLSBLong(image, DataSize + 56l + padding); /* 0x84 */
  WriteBlobLSBLong(image, 0x6); /* 0x88 */
  WriteBlobLSBLong(image, 0x8); /* 0x8C */
  WriteBlobLSBLong(image, 0x6); /* 0x90 */  
  WriteBlobLSBLong(image, 0);   
  WriteBlobLSBLong(image, 0x5); /* 0x98 */  
  WriteBlobLSBLong(image, 0xC); /* 0x9C */      
  WriteBlobLSBLong(image, image->rows);    /* x: 0xA0 */  
  WriteBlobLSBLong(image, image->columns); /* y: 0xA4 */  
  WriteBlobLSBLong(image, 3);              /* z: 0xA8 */  
  WriteBlobLSBLong(image, 0);
  WriteBlobLSBShort(image, 1);  /* 0xB0 */  
  WriteBlobLSBShort(image, 1);  /* 0xB2 */
  WriteBlobLSBLong(image, 'M'); /* 0xB4 */
  WriteBlobLSBLong(image, 0x2); /* 0xB8 */  
  WriteBlobLSBLong(image, DataSize); /* 0xBC */

  /*
    Store image data.
  */
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);    
    for (x=0; x < (long) image->rows; x++)
    {
      WriteBlobByte(image,ScaleQuantumToChar(q->red));
      q++;
    }
  }
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);
    for (x=0; x < (long) image->rows; x++)
    {
      WriteBlobByte(image,ScaleQuantumToChar(q->green));
      q++;
    }
  }
  for (y=0; y<(long)image->columns; y++)
  {
    q=AcquireImagePixels(image,y-1,0,1,image->rows-1,&image->exception);
    for (x=0; x < (long) image->rows; x++)
    {
      WriteBlobByte(image,ScaleQuantumToChar(q->blue));
      q++;
    }
  }

  while(padding-->0) WriteBlobByte(image,0);

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
