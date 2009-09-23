/*
% Copyright (C) 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                          CCCC   AAA   L     SSSSS                           %
%                         C      A   A  L     SS                              %
%                         C      AAAAA  L       SSS                           %
%                         C      A   A  L        SS                           %
%                          CCCC  A   A  LLLLL SSSSS                           %
%                                                                             %
%                                                                             %
%                 Read/Write CALS (CALS type 1) format                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                               John Sergeant                                 %
%                                  May 2009                                   %
%                                                                             %
%         NB Huffman2DEncodeImage 'borrowed' wholesale from pdf.c             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

#if defined(HasTIFF)
#  if defined(HAVE_TIFFCONF_H)
#    include "tiffconf.h"
#  endif /* defined(HAVE_TIFFCONF_H) */
#  include "tiffio.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n 2 D E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Huffman2DEncodeImage compresses an image via two-dimensional
%  Huffman-coding.
%
%  The format of the Huffman2DEncodeImage method is:
%
%      MagickPassFail Huffman2DEncodeImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method Huffman2DEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
*/
static MagickPassFail Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  long
    count;

  register long
    i;

  TIFF
    *tiff;

  uint16
    fillorder;

  unsigned char
    *buffer;

  unsigned int
    status;

  uint32
    *byte_count;

  unsigned long
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if(!AcquireTemporaryFileName(filename))
    ThrowBinaryException(FileOpenError,UnableToCreateTemporaryFile,
      filename);

  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);

  (void) SetImageType(huffman_image,BilevelType);
  FormatString(huffman_image->filename,"tiff:%s",filename);

  clone_info=CloneImageInfo((ImageInfo *) NULL);
  clone_info->compression=Group4Compression;
  clone_info->type=BilevelType;
  (void) AddDefinitions(clone_info,"tiff:strip-per-page=TRUE",
                        &image->exception);

  status=WriteImage(clone_info,huffman_image);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == MagickFalse)
    return(MagickFalse);

  tiff=TIFFOpen(filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(FileOpenError,UnableToOpenFile,
        image_info->filename)
    }

  /*
    Allocate raw strip buffer.
  */
  byte_count=0;
  (void) TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  strip_size=byte_count[0];
  for (i=1; i < (long) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  /* strip_size=TIFFStripSize(tiff); */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Allocating %lu bytes of memory for TIFF strip",
                        (unsigned long) strip_size);
  buffer=MagickAllocateMemory(unsigned char *,strip_size);
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(filename);
      ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
        (char *) NULL)
    }

  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Output 2D Huffman pixels.");
  (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fillorder);
  for (i=0; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    count=TIFFReadRawStrip(tiff,(uint32) i,buffer,(long) byte_count[i]);
    if (fillorder == FILLORDER_LSB2MSB)
      TIFFReverseBits(buffer,count);

    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Writing %lu strip bytes to blob ...",
                          (unsigned long) count);
    (void) WriteBlob(image,count,buffer);
  }

  MagickFreeMemory(buffer);
  TIFFClose(tiff);
  (void) LiberateTemporaryFile(filename);
  return(True);
}
#endif /* if defined(HasTIFF) */


/* 
   Write an unsigned long to file with Intel/LSB ordering
*/
static void CALS_WriteIntelULong(FILE *file,unsigned long ul)
{
  fputc((unsigned char)ul,file);
  fputc((unsigned char)(ul >> 8),file);
  fputc((unsigned char)(ul >> 16),file);
  fputc((unsigned char)(ul >> 24),file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C A L S                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsCALS returns True if the image format type, identified by the
%  magick string, is CAL.
%
%  The format of the IsCALS method is:
%
%      unsigned int IsCALS(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsCALS returns True if the image format type is CAL.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsCALS(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(False);
  if (LocaleNCompare((char *) (magick),"version: MIL-STD-1840",21) == 0)
    return(True);
  if (LocaleNCompare((char *) (magick),"srcdocid:",9) == 0)
    return(True);
  if (LocaleNCompare((char *) (magick),"rorient:",8) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C A L S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCALSImage reads a CALS type 1 image file and returns it. It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCALSImage method is:
%
%      Image *ReadCALSImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCALSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCALSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  char
    record[129];  

  unsigned long
    status,
    width,
    height,
    rtype,
    orient,
    density;

  char
    filename[MaxTextExtent];

  FILE
    *file;

  unsigned long
    byte_count_pos,
    strip_off_pos,
    flen;

  int
    c;

  ImageInfo
    *clone_info;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Scan CAL header, record by record, looking for mandatory fields
  */ 
  rtype = 1;
  width = height = 0;
  orient = 1;
  density = 200;
  record[128]=0;
  for (y = 0; y < 16; y++)
    {
      ReadBlob(image,128,(char *) record);
      if (LocaleNCompare(record,"rtype:",6) == 0)
        { /* rtype */
          sscanf(record+6,"%ld",&rtype);
        }
      else
      if (LocaleNCompare(record,"rorient:",8) == 0)
        { /* rorient */
          unsigned long
            pel_path_rot,
            line_rot;

          pel_path_rot = line_rot = 0;
          sscanf(record+8,"%ld,%ld",&pel_path_rot,&line_rot);
          switch (pel_path_rot)
            {
              case 90:
                orient = 5;
                break;
              case 180:
                orient = 3;
                break;
              case 270:
                orient = 7;
                break;
              default:
                orient = 1;
            }
          if (line_rot == 90) orient++;
        }
      else
      if (LocaleNCompare(record,"rpelcnt:",8) == 0)
        { /* replcnt */
          sscanf(record+8,"%ld,%ld",&width,&height);
        }
     else
     if (LocaleNCompare(record,"rdensty:",8) == 0)
        { /* rdensty */
          sscanf(record+8,"%ld",&density);
          if (!density) density = 200;
        }
    }
  if ((!width) || (!height) || (rtype != 1))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  /* Create TIFF wrapper to handle file data using TIFF library */
  file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
  if (file == (FILE *) NULL)
    ThrowReaderTemporaryFileException(filename);

  /* Intel TIFF with IFD at offset 8 - IFD has 14 records */
  fwrite("\111\111\052\000\010\000\000\000\016\000",1,10,file);
  /* New sub image - normal type */
  fwrite("\376\000\003\000\001\000\000\000\000\000\000\000",1,12,file);
  /* Image width */
  fwrite("\000\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,width);
  /* Image height */
  fwrite("\001\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,height);
  /* 1 bit per sample */
  fwrite("\002\001\003\000\001\000\000\000\001\000\000\000",1,12,file);
  /* CCITT Group 4 compression */
  fwrite("\003\001\003\000\001\000\000\000\004\000\000\000",1,12,file);
  /* Photometric interpretation MAX BLACK */
  fwrite("\006\001\003\000\001\000\000\000\000\000\000\000",1,12,file);
  /* Strip offset */
  fwrite("\021\001\003\000\001\000\000\000",1,8,file);
  strip_off_pos = 10 + (12 * 14) + 4 + 8;
  CALS_WriteIntelULong(file,strip_off_pos);
  /* Orientation */
  fwrite("\022\001\003\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,orient);
  /* 1 sample per pixel */
  fwrite("\025\001\003\000\001\000\000\000\001\000\000\000",1,12,file);
  /* Rows per strip (same as width) */
  fwrite("\026\001\004\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,width);
  /* Strip byte count */
  fwrite("\027\001\004\000\001\000\000\000\000\000\000\000",1,12,file);
  byte_count_pos = ftell(file)-4;
  /* X resolution */
  fwrite("\032\001\005\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,strip_off_pos-8);
  /* Y resolution */
  fwrite("\033\001\005\000\001\000\000\000",1,8,file);
  CALS_WriteIntelULong(file,strip_off_pos-8);
  /* Resolution unit is inch */
  fwrite("\050\001\003\000\001\000\000\000\002\000\000\000",1,12,file);
  /* Offset to next IFD ie end of images */
  fwrite("\000\000\000\000",1,4,file);
  /* Write X/Y resolution as rational data */
  CALS_WriteIntelULong(file,density);
  CALS_WriteIntelULong(file,1);

  /* Copy image stream data */
  flen = 0;
  c=ReadBlobByte(image);
  while (c != EOF)
    {
      flen++;
      (void) fputc(c,file);
      c=ReadBlobByte(image);
    }

  /* Return to correct location and output strip byte count */
  fseek(file,byte_count_pos,SEEK_SET);
  CALS_WriteIntelULong(file,flen);

  (void) fclose(file);
  DestroyImage(image);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  FormatString(clone_info->filename,"tiff:%.1024s",filename);
  image=ReadImage(clone_info,exception);
  (void) LiberateTemporaryFile(filename);
  DestroyImageInfo(clone_info);
  if (image != (Image *) NULL)
    {
      (void) strlcpy(image->filename,image_info->filename,
		     sizeof(image->filename));
      (void) strlcpy(image->magick_filename,image_info->filename,
		     sizeof(image->magick_filename));
      (void) strlcpy(image->magick,"CALS",sizeof(image->magick));
    }
  return(image);
}
#if defined(HasTIFF)

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C A L S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCALSImage writes an image in the CALS type I image format
%
%  The format of the WriteCALSImage method is:
%
%      unsigned int WriteCALSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteCALSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
%
*/
static void WriteCALSRecord(Image *image,const char *data)
{
  int
    i;

  const char
    *p;

  char
    pad[128];

  i = 0;
  if (data)
    {
      /* Limit output to 128 chars */
      p=data;
      while ((i < 128) && (p[i])) i++;
      WriteBlob(image,i,data);
    }
  if (i < 128)
    {
      /* Pad record to 128 characters using trailing spaces */
      i=128-i;
      memset(pad,' ',i);
      WriteBlob(image,i,pad);
    }
}

static unsigned int WriteCALSImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  int
    i;

  long
    sans;

  unsigned long
    density;

  int
    orx,
    ory;

  /*
    Validate input image
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Open output image file.
  */
  if (OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception) == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Create standard header
  */
  WriteCALSRecord(image,"srcdocid: NONE");
  WriteCALSRecord(image,"dstdocid: NONE");
  WriteCALSRecord(image,"txtfilid: NONE");
  WriteCALSRecord(image,"figid: NONE");
  WriteCALSRecord(image,"srcgph: NONE");
  WriteCALSRecord(image,"docls: NONE");
  WriteCALSRecord(image,"rtype: 1");
  /* orientation based on input or default of upright */
  switch (image->orientation)
  {
    case TopRightOrientation:
      orx=180;
      ory=270;
      break;
    case BottomRightOrientation:
      orx=180;
      ory=90;
      break;
    case BottomLeftOrientation:
      orx=0;
      ory=90;
      break;
    case LeftTopOrientation:
      orx=270;
      ory=0;
      break;
    case RightTopOrientation:
      orx=270;
      ory=180;
      break;
    case RightBottomOrientation:
      orx=90;
      ory=180;
      break;
    case LeftBottomOrientation:
      orx=90;
      ory=0;
      break;
    default:
      orx=0; 
      ory=270;
  }
  FormatString(buffer,"rorient: %03d,%03d",orx,ory);
  WriteCALSRecord(image,buffer);
  /* pixel counts based on columns/rows of input */
  FormatString(buffer,"rpelcnt: %06ld,%06ld",image->columns,image->rows);
  WriteCALSRecord(image,buffer);  
  /* density based on input density or default of 200 */
  density=200;
  if (image_info->density != (char *) NULL)
    (void) GetGeometry(image_info->density,&sans,&sans,&density,&density);
  FormatString(buffer,"rdensty: %04ld",density);
  WriteCALSRecord(image,buffer);
  WriteCALSRecord(image,"notes: NONE");

  /*
    Pad header to make 16 records / 2048 bytes
  */
  memset(buffer,' ',128);
  for (i = 0; i < 5; i++)
    WriteBlob(image,128,buffer);

  /*
    Encode data to Group 4 - routine 'borrowed' from pdf.c
  */
  if (!Huffman2DEncodeImage(image_info,image) )
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Huffman2DEncodeImage reports failure!");

  /*
    Close output file and return image
  */
  CloseBlob(image);
  return(True);
}
#endif /* if defined(HasTIFF) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C A L S I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCALSImage adds attributes for the CALS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCALSImage method is:
%
%      RegisterCALSImage(void)
%
*/
ModuleExport void RegisterCALSImage(void)
{
  MagickInfo
    *entry;

  const char
    *description = "Continuous Acquisition and Life-cycle Support Type 1 image",
    *module      = "CALS",
    *note        = "Specified in MIL-R-28002 and MIL-PRF-28002";

  entry=SetMagickInfo("CAL");
  entry->decoder=(DecoderHandler) ReadCALSImage;
#if defined(HasTIFF)
  entry->encoder=(EncoderHandler) WriteCALSImage;
#endif /* if defined(HasTIFF) */
  entry->magick=(MagickHandler) IsCALS;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=description;
  entry->note=note;
  entry->module=module;
  entry->stealth=MagickTrue;
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("CALS");
  entry->decoder=(DecoderHandler) ReadCALSImage;
#if defined(HasTIFF)
  entry->encoder=(EncoderHandler) WriteCALSImage;
#endif /* if defined(HasTIFF) */
  entry->magick=(MagickHandler) IsCALS;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=description;
  entry->note=note;
  entry->module=module;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C A L S I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCALSImage removes format registrations made by the
%  CAL module from the list of supported formats.
%
%  The format of the UnregisterCALSImage method is:
%
%      UnregisterCALSImage(void)
%
*/
ModuleExport void UnregisterCALSImage(void)
{
  (void) UnregisterMagickInfo("CAL");
  (void) UnregisterMagickInfo("CALS");
}
