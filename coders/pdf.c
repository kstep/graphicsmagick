/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   DDDD   FFFFF                              %
%                            P   P  D   D  F                                  %
%                            PPPP   D   D  FFF                                %
%                            P      D   D  F                                  %
%                            P      DDDD   F                                  %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePDFImage(const ImageInfo *,Image *),
  ZLIBEncodeImage(Image *,const size_t,const unsigned long,unsigned char *);

#if defined(HasGS)
#include "ps/iapi.h"
#include "ps/errors.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e P o s t I n t e r p r e t e r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExecutePostscriptInterpreter executes the postscript interpreter
%  with the specified command.
%
%  The format of the ExecutePostscriptInterpreter method is:
%
%      unsigned int ExecutePostscriptInterpreter(const unsigned int verbose,
%        const char *command)
%
%  A description of each parameter follows:
%
%    o status:  Method ExecutePostscriptInterpreter returns True is the command
%      is successfully executed, otherwise False.
%
%    o verbose: A value other than zero displays the command prior to
%      executing it.
%
%    o command: The address of a character string containing the command to
%      execute.
%
%
*/
static unsigned int ExecutePostscriptInterpreter(const unsigned int verbose,
  const char *command)
{
  char
    **argv;

  gs_main_instance
    *interpreter;

  int
    argc,
    code,
    status;

  register int
    i;

  if (verbose)
    (void) fputs(command,stdout);
  status=gsapi_new_instance(&interpreter,(void *) NULL);
  if (status < 0)
    return(False);
  argv=StringToArgv(command,&argc);
  status=gsapi_init_with_args(interpreter,argc-1,argv+1);
  if (status == 0)
    status=gsapi_run_string(interpreter,"systemdict /start get exec\n",0,&code);
  gsapi_exit(interpreter);
  gsapi_delete_instance(interpreter);
  for (i=0; i < argc; i++)
    LiberateMemory((void **) &argv[i]);
  LiberateMemory((void **) &argv);
  if ((status == 0) || (status == e_Quit))
    return(False);
  return(True);
}
#else
static unsigned int ExecutePostscriptInterpreter(const unsigned int verbose,
  const char *command)
{
  return(SystemCommand(verbose,command));
}
#endif

#if defined(HasTIFF)
#if defined(HAVE_TIFFCONF_H)
#include "tiffconf.h"
#endif
#include "tiffio.h"
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
%      unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
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
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  char
    filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  long
    count,
    j;

  register long
    i;

  TIFF
    *tiff;

  uint16
    fillorder;

  unsigned char
    *buffer;

  unsigned int
    *byte_count,
    status,
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);
  if ((huffman_image->storage_class == DirectClass) ||
       !IsMonochromeImage(huffman_image,&image->exception))
    SetImageType(huffman_image,BilevelType);
  TemporaryFilename(filename);
  FormatString(huffman_image->filename,"tiff:%s",filename);
  clone_info=CloneImageInfo(image_info);
  clone_info->compression=Group4Compression;
  status=WriteImage(clone_info,huffman_image);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == False)
    return(False);
  tiff=TIFFOpen(filename,ReadBinaryType);
  if (tiff == (TIFF *) NULL)
    {
      (void) remove(filename);
      ThrowBinaryException(FileOpenWarning,"Unable to open file",
        image_info->filename)
    }
  /*
    Allocate raw strip buffer.
  */
  (void) TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  strip_size=byte_count[0];
  for (i=1; i < (long) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  buffer=(unsigned char *) AcquireMemory(strip_size);
  if (buffer == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      (void) remove(filename);
      ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL)
    }
  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fillorder);
  for (i=0; i < (long) TIFFNumberOfStrips(tiff); i++)
  {
    Ascii85Initialize(image);
    count=TIFFReadRawStrip(tiff,(uint32) i,buffer,byte_count[i]);
    if (fillorder == FILLORDER_LSB2MSB)
      TIFFReverseBits(buffer,count);
    for (j=0; j < count; j++)
      Ascii85Encode(image,(unsigned int) buffer[j]);
    Ascii85Flush(image);
  }
  LiberateMemory((void **) &buffer);
  TIFFClose(tiff);
  (void) remove(filename);
  return(True);
}
#else
static unsigned int Huffman2DEncodeImage(const ImageInfo *image_info,
  Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateWarning,"TIFF library is not available",
    image->filename);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P D F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPDF returns True if the image format type, identified by the
%  magick string, is PDF.
%
%  The format of the IsPDF method is:
%
%      unsigned int IsPDF(const unsigned char *magick,const size_t offset)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPDF returns True if the image format type is PDF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o offset: Specifies the offset of the magick string.
%
%
*/
static unsigned int IsPDF(const unsigned char *magick,const size_t offset)
{
  if (offset < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"%PDF-",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDFImage reads a Portable Document Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPDFImage method is:
%
%      Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MediaBox  "/MediaBox"

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  ImageInfo
    *clone_info;

  int
    count,
    status;

  RectangleInfo
    box,
    page;

  register char
    *p;

  register int
    c;

  SegmentInfo
    bounds;

  unsigned int
    portrait;

  unsigned long
    height,
    width;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image_info->monochrome)
    {
      delegate_info=GetDelegateInfo("gs-mono",(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        return((Image *) NULL);
    }
  else
    {
      delegate_info=GetDelegateInfo("gs-color",(char *) NULL,exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        return((Image *) NULL);
    }
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to write file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
     (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  page.width=612;
  page.height=792;
  page.x=0;
  page.y=0;
  (void) ParseGeometry(PSPageGeometry,&page.x,&page.y,&page.width,&page.height);
  portrait=True;
  /*
    Determine page geometry from the PDF media box.
  */
  box.width=0;
  box.height=0;
  for (p=command; ; )
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=(char) c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Continue unless this is a MediaBox statement.
    */
    if (LocaleNCompare(command,"/Rotate 90",10) == 0)
      portrait=False;
    if (LocaleNCompare(MediaBox,command,strlen(MediaBox)) != 0)
      continue;
    count=sscanf(command,"/MediaBox [%lf %lf %lf %lf",&bounds.x1,&bounds.y1,
      &bounds.x2,&bounds.y2);
    if (count != 4)
      count=sscanf(command,"/MediaBox[%lf %lf %lf %lf",&bounds.x1,&bounds.y1,
        &bounds.x2,&bounds.y2);
    if (count != 4)
      continue;
    if ((bounds.x1 > bounds.x2) || (bounds.y1 > bounds.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    width=(unsigned int) (bounds.x2-bounds.x1);
    if ((float) ((int) bounds.x2) != bounds.x2)
      width++;
    height=(unsigned int) (bounds.y2-bounds.y1);
    if ((float) ((int) bounds.y2) != bounds.y2)
      height++;
    if ((width <= box.width) && (height <= box.height))
      continue;
    page.width=width;
    page.height=height;
    box=page;
  }
  if (image_info->page != (char *) NULL)
    (void) ParseGeometry(image_info->page,&page.x,&page.y,
      &page.width,&page.height);
  FormatString(geometry,"%ux%u",
    (unsigned int) ceil(page.width*image->x_resolution/dx_resolution-0.5),
    (unsigned int) ceil(page.height*image->y_resolution/dy_resolution-0.5));
  if (ferror(file))
    {
      (void) fclose(file);
      ThrowReaderException(FileOpenWarning,
        "An error has occurred writing to file",image)
    }
  (void) fclose(file);
  CloseBlob(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strncpy(filename,image_info->filename,MaxTextExtent-1);
  clone_info=CloneImageInfo(image_info);
  TemporaryFilename(clone_info->filename);
  FormatString(command,delegate_info->commands,clone_info->antialias ? 4 : 1,
    clone_info->antialias ? 4 : 1,geometry,density,options,clone_info->filename,
    postscript_filename);
  MagickMonitor(RenderPostscriptText,0,8);
  status=ExecutePostscriptInterpreter(clone_info->verbose,command);
  MagickMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      (void) remove(postscript_filename);
      ThrowReaderException(FileOpenWarning,"Portable Document delegate failed",
        image)
    }
  DestroyImage(image);
  DetachBlob(clone_info->blob);
  image=ReadImage(clone_info,exception);
  (void) remove(postscript_filename);
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  if (image == (Image *) NULL)
    ThrowReaderException(CorruptImageWarning,
      "Portable Document delegate failed",image);
  do
  {
    (void) strcpy(image->magick,"PDF");
    (void) strncpy(image->filename,filename,MaxTextExtent-1);
    if (!portrait)
      {
        Image
          *rotate_image;

        /*
          Rotate image.
        */
        rotate_image=RotateImage(image,90,exception);
        if (rotate_image != (Image *) NULL)
          {
            DestroyImage(image);
            image=rotate_image;
          }
      }
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P D F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPDFImage adds attributes for the PDF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPDFImage method is:
%
%      RegisterPDFImage(void)
%
*/
ModuleExport void RegisterPDFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPDF");
  entry->decoder=ReadPDFImage;
  entry->encoder=WritePDFImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Encapsulated Portable Document Format");
  entry->module=AllocateString("PDF");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PDF");
  entry->decoder=ReadPDFImage;
  entry->encoder=WritePDFImage;
  entry->magick=IsPDF;
  entry->description=AllocateString("Portable Document Format");
  entry->module=AllocateString("PDF");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P D F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPDFImage removes format registrations made by the
%  PDF module from the list of supported formats.
%
%  The format of the UnregisterPDFImage method is:
%
%      UnregisterPDFImage(void)
%
*/
ModuleExport void UnregisterPDFImage(void)
{
  (void) UnregisterMagickInfo("EPDF");
  (void) UnregisterMagickInfo("PDF");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDFImage writes an image in the Portable Document image
%  format.
%
%  The format of the WritePDFImage method is:
%
%      unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static char *EscapeParenthesis(const char *text)
{
  register char
    *p;

  register long
    i;

  static char
    buffer[MaxTextExtent];

  unsigned long
    escapes;

  escapes=0;
  p=buffer;
  for (i=0; i < (long) Min(strlen(text),(MaxTextExtent-escapes-1)); i++)
  {
    if ((text[i] == '(') || (text[i] == ')'))
      {
        *p++='\\';
        escapes++;
      }
    *p++=text[i];
  }
  *p='\0';
  return(buffer);
}

static unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
{
#define CFormat  "/Filter [ /%.1024s ]\n"
#define ObjectsPerImage  12

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    geometry[MaxTextExtent],
    **labels;

  CompressionType
    compression;

  const ImageAttribute
    *attribute;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  long
    count,
    x,
    y;

  Image
    *clone_image,
    encode_image,
    *tile_image;

  off_t
    offset;

  RectangleInfo
    media_info;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register unsigned char
    *q;

  register long
    i;

  size_t
    length;

  struct tm
    *time_meridian;

  time_t
    seconds;

  unsigned char
    *pixels;

  unsigned int
    scene,
    status,
    text_size;

  unsigned long
    info_id,
    number_pixels,
    object,
    pages_id,
    root_id,
    height,
    width,
    *xref;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdout) || image->pipet)
    {
      /*
        Write standard output or pipe to temporary file.
      */
      encode_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
    }
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
#if !defined(HasJPEG)
    case JPEGCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "JPEG compression is not available",image->filename);
      break;
    }
#endif
#if !defined(HasLZW)
    case LZWCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "LZW compression is not available",image->filename);
      break;
    }
#endif
#if !defined(HasZLIB)
    case ZipCompression:
    {
      compression=RunlengthEncodedCompression;
      ThrowException(&image->exception,MissingDelegateWarning,
        "ZLIB compression is not available",image->filename);
      break;
    }
#endif
    default:
      break;
  }
  /*
    Allocate X ref memory.
  */
  xref=(unsigned long *) AcquireMemory(2048*sizeof(unsigned long));
  if (xref == (unsigned long *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write Info object.
  */
  object=0;
  (void) WriteBlobString(image,"%PDF-1.2 \n");
  xref[object++]=TellBlob(image);
  info_id=object;
  FormatString(buffer,"%u 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  seconds=time((time_t *) NULL);
  time_meridian=localtime(&seconds);
  FormatString(date,"D:%04d%02d%02d%02d%02d%02d",time_meridian->tm_year+1900,
    time_meridian->tm_mon+1,time_meridian->tm_mday,time_meridian->tm_hour,
    time_meridian->tm_min,time_meridian->tm_sec);
  FormatString(buffer,"/CreationDate (%.1024s)\n",date);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Producer (%.1024s)\n",EscapeParenthesis(MagickVersion));
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  /*
    Write Catalog object.
  */
  xref[object++]=TellBlob(image);
  root_id=object;
  FormatString(buffer,"%u 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  (void) WriteBlobString(image,"/Type /Catalog\n");
  FormatString(buffer,"/Pages %u 0 R\n",object+1);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  /*
    Write Pages object.
  */
  xref[object++]=TellBlob(image);
  pages_id=object;
  FormatString(buffer,"%u 0 obj\n",object);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"<<\n");
  (void) WriteBlobString(image,"/Type /Pages\n");
  FormatString(buffer,"/Kids [ %u 0 R ",object+1);
  (void) WriteBlobString(image,buffer);
  count=pages_id+ObjectsPerImage+1;
  if (image_info->adjoin)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; kid_image->next != (Image *) NULL; count+=ObjectsPerImage)
      {
        FormatString(buffer,"%d 0 R ",count);
        (void) WriteBlobString(image,buffer);
        kid_image=kid_image->next;
      }
      ReacquireMemory((void **) &xref,(count+2048)*sizeof(unsigned long));
      if (xref == (unsigned long *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
          image);
    }
  (void) WriteBlobString(image,"]\n");
  FormatString(buffer,"/Count %u\n",(count-pages_id)/ObjectsPerImage);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"endobj\n");
  scene=0;
  do
  {
    if (compression == FaxCompression)
      if ((image->storage_class == DirectClass) ||
          !IsMonochromeImage(image,&image->exception))
      SetImageType(image,BilevelType);
    if (compression == JPEGCompression)
      image->storage_class=DirectClass;
    /*
      Scale image to size of Portable Document page.
    */
    text_size=0;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      text_size=(unsigned int)
        (MultilineCensus(attribute->value)*image_info->pointsize+12);
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    FormatString(geometry,"%lux%lu",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      (void) strncpy(geometry,image_info->page,MaxTextExtent-1);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatString(geometry,"%ux%u%+d%+d",image->page.width,
          image->page.height,image->page.x,image->page.y);
      else
        if (LocaleCompare(image_info->magick,"PDF") == 0)
          (void) strcpy(geometry,PSPageGeometry);
    (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
    (void) GetGeometry(geometry,&media_info.x,&media_info.y,
      &media_info.width,&media_info.height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    /*
      Write Page object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) WriteBlobString(image,"/Type /Page\n");
    FormatString(buffer,"/Parent %u 0 R\n",pages_id);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"/Resources <<\n");
    FormatString(buffer,"/Font << /F%u %u 0 R >>\n",image->scene,
      object+4);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/XObject << /Im%u %u 0 R >>\n",image->scene,
      object+5);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/ProcSet %u 0 R >>\n",object+3);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/MediaBox [%d %d %d %d]\n",0,0,
      media_info.width,media_info.height);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Contents %u 0 R\n",object+1);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Thumb %u 0 R\n",object+8);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Contents object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    FormatString(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    (void) WriteBlobString(image,"q\n");
    labels=(char **) NULL;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      labels=StringToList(attribute->value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) WriteBlobString(image,"BT\n");
          FormatString(buffer,"/F%u %f Tf\n",image->scene,
            image_info->pointsize);
          (void) WriteBlobString(image,buffer);
          FormatString(buffer,"%d %f Td\n",x,y+height+
            i*image_info->pointsize+12);
          (void) WriteBlobString(image,buffer);
          FormatString(buffer,"(%.1024s) Tj\n",labels[i]);
          (void) WriteBlobString(image,buffer);
          (void) WriteBlobString(image,"ET\n");
          LiberateMemory((void **) &labels[i]);
        }
        LiberateMemory((void **) &labels);
      }
    FormatString(buffer,"%g 0 0 %g %d %d cm\n",x_scale,y_scale,x,y);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Im%u Do\n",image->scene);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"Q\n");
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"endstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%lu\n",(unsigned long) offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Procset object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    if (image->storage_class == DirectClass)
      (void) strcpy(buffer,"[ /PDF /Text /ImageC");
    else
      if (compression == FaxCompression)
        (void) strcpy(buffer,"[ /PDF /Text /ImageB");
      else
        (void) strcpy(buffer,"[ /PDF /Text /ImageI");
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image," ]\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Font object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) WriteBlobString(image,"/Type /Font\n");
    (void) WriteBlobString(image,"/Subtype /Type1\n");
    FormatString(buffer,"/Name /F%u\n",image->scene);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"/BaseFont /Helvetica\n");
    (void) WriteBlobString(image,"/Encoding /MacRomanEncoding\n");
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write XObject object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    (void) WriteBlobString(image,"/Type /XObject\n");
    (void) WriteBlobString(image,"/Subtype /Image\n");
    FormatString(buffer,"/Name /Im%u\n",image->scene);
    (void) WriteBlobString(image,buffer);
    switch (compression)
    {
      case NoCompression: FormatString(buffer,CFormat,"ASCII85Decode"); break;
      case JPEGCompression: FormatString(buffer,CFormat,"DCTDecode"); break;
      case LZWCompression: FormatString(buffer,CFormat,"LZWDecode"); break;
      case ZipCompression: FormatString(buffer,CFormat,"FlateDecode"); break;
      case FaxCompression:
      {
        (void) strcpy(buffer,"/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,
          "/DecodeParms [ << >> << /K %.1024s /Columns %d /Rows %d >> ]\n",
          CCITTParam,image->columns,image->rows);
        break;
      }
      default: FormatString(buffer,CFormat,"RunLengthDecode"); break;
    }
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Width %lu\n",image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Height %lu\n",image->rows);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/ColorSpace %u 0 R\n",object+2);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/BitsPerComponent %d\n",
      compression == FaxCompression ? 1 : 8);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    number_pixels=image->columns*image->rows;
    if (compression == FaxCompression)
      {
        if (LocaleCompare(CCITTParam,"0") == 0)
          (void) HuffmanEncodeImage(image_info,image);
        else
          (void) Huffman2DEncodeImage(image_info,image);
      }
    else
      if (image->storage_class == DirectClass)
        switch (compression)
        {
          case JPEGCompression:
          {
            char
              filename[MaxTextExtent];

            FILE
              *file;

            Image
              *jpeg_image;

            int
              c;

            /*
              Write image to temporary file in JPEG format.
            */
            TemporaryFilename(filename);
            jpeg_image=CloneImage(image,0,0,True,&image->exception);
            if (jpeg_image == (Image *) NULL)
              ThrowWriterException(DelegateWarning,
                "Unable to clone image",image);
            (void) FormatString(jpeg_image->filename,"jpeg:%.1024s",filename);
            status=WriteImage(image_info,jpeg_image);
            DestroyImage(jpeg_image);
            if (status == False)
              ThrowWriterException(DelegateWarning,"Unable to write image",
                image);
            file=fopen(filename,ReadBinaryType);
            if (file == (FILE *) NULL)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
            for (c=fgetc(file); c != EOF; c=fgetc(file))
              (void) WriteBlobByte(image,c);
            (void) fclose(file);
            (void) remove(filename);
            break;
          }
          case RunlengthEncodedCompression:
          default:
          {
            /*
              Allocate pixel array.
            */
            length=(image->colorspace == CMYKColorspace ? 4 : 3)*number_pixels;
            pixels=(unsigned char *) AcquireMemory(length);
            if (pixels == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitWarning,
                "Memory allocation failed",image);
            /*
              Dump runoffset encoded pixels.
            */
            q=pixels;
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                if (image->matte && (p->opacity == TransparentOpacity))
                  {
                    *q++=DownScale(MaxRGB);
                    *q++=DownScale(MaxRGB);
                    *q++=DownScale(MaxRGB);
                    p++;
                    continue;
                  }
                *q++=DownScale(p->red);
                *q++=DownScale(p->green);
                *q++=DownScale(p->blue);
                if (image->colorspace == CMYKColorspace)
                  *q++=DownScale(p->opacity);
                p++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
            else
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels);
              else
                status=PackbitsEncodeImage(image,length,pixels);
            LiberateMemory((void **) &pixels);
            if (!status)
              {
                CloseBlob(image);
                return(False);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                if (image->matte && (p->opacity == TransparentOpacity))
                  {
                    Ascii85Encode(image,DownScale(MaxRGB));
                    Ascii85Encode(image,DownScale(MaxRGB));
                    Ascii85Encode(image,DownScale(MaxRGB));
                    continue;
                  }
                Ascii85Encode(image,DownScale(p->red));
                Ascii85Encode(image,DownScale(p->green));
                Ascii85Encode(image,DownScale(p->blue));
                if (image->colorspace == CMYKColorspace)
                  Ascii85Encode(image,DownScale(p->opacity));
                p++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
            Ascii85Flush(image);
            break;
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              length=number_pixels;
              pixels=(unsigned char *) AcquireMemory(length);
              if (pixels == (unsigned char *) NULL)
                ThrowWriterException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                  *q++=indexes[x];
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,length,pixels);
                else
                  status=PackbitsEncodeImage(image,length,pixels);
              LiberateMemory((void **) &pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize(image);
              for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                  Ascii85Encode(image,indexes[x]);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"\nendstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%lu\n",(unsigned long) offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Colorspace object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    if (image->colorspace == CMYKColorspace)
      (void) strcpy(buffer,"/DeviceCMYK\n");
    else
      if (image->storage_class == DirectClass)
        (void) strcpy(buffer,"/DeviceRGB\n");
      else
        if (compression == FaxCompression)
          (void) strcpy(buffer,"/DeviceGray\n");
        else
          FormatString(buffer,"[ /Indexed /DeviceRGB %u %u 0 R ]\n",
            image->colors-1,object+3);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Thumb object.
    */
    width=image->columns;
    height=image->rows;
    x=0;
    y=0;
    (void) ParseImageGeometry("106x106+0+0>",&x,&y,&width,&height);
    clone_image=CloneImage(image,0,0,True,&image->exception);
    if (clone_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Unable to scale image",
        image);
    if (clone_image->storage_class == PseudoClass)
      clone_image->filter=PointFilter;
    tile_image=ZoomImage(clone_image,width,height,&image->exception);
    DestroyImage(clone_image);
    if (tile_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Unable to scale image",
        image);
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"<<\n");
    switch (compression)
    {
      case NoCompression: FormatString(buffer,CFormat,"ASCII85Decode"); break;
      case JPEGCompression: FormatString(buffer,CFormat,"DCTDecode"); break;
      case LZWCompression: FormatString(buffer,CFormat,"LZWDecode"); break;
      case ZipCompression: FormatString(buffer,CFormat,"FlateDecode"); break;
      case FaxCompression:
      {
        (void) strcpy(buffer,"/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,
          "/DecodeParms [ << >> << /K %.1024s /Columns %d /Rows %d >> ]\n",
          CCITTParam,image->columns,image->rows);
        break;
      }
      default: FormatString(buffer,CFormat,"RunLengthDecode"); break;
    }
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Width %lu\n",tile_image->columns);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Height %lu\n",tile_image->rows);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/ColorSpace %u 0 R\n",object-1);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/BitsPerComponent %d\n",
      compression == FaxCompression ? 1 : 8);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,">>\n");
    (void) WriteBlobString(image,"stream\n");
    offset=TellBlob(image);
    number_pixels=tile_image->columns*tile_image->rows;
    if (compression == FaxCompression)
      {
        *tile_image->blob=(*image->blob);
        tile_image->file=image->file;
        if (LocaleCompare(CCITTParam,"0") == 0)
          (void) HuffmanEncodeImage(image_info,tile_image);
        else
          (void) Huffman2DEncodeImage(image_info,tile_image);
        *image->blob=(*tile_image->blob);
        image->file=tile_image->file;
      }
    else
      if (image->storage_class == DirectClass)
        switch (compression)
        {
          case JPEGCompression:
          {
            char
              filename[MaxTextExtent];

            FILE
              *file;

            Image
              *jpeg_image;

            int
              c;

            /*
              Write image to temporary file in JPEG format.
            */
            TemporaryFilename(filename);
            jpeg_image=CloneImage(tile_image,0,0,True,&image->exception);
            if (jpeg_image == (Image *) NULL)
              ThrowWriterException(DelegateWarning,"Unable to clone image",
                image);
            (void) FormatString(jpeg_image->filename,"jpeg:%.1024s",filename);
            status=WriteImage(image_info,jpeg_image);
            DestroyImage(jpeg_image);
            if (status == False)
              ThrowWriterException(DelegateWarning,"Unable to write image",
                image);
            file=fopen(filename,ReadBinaryType);
            if (file == (FILE *) NULL)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
            for (c=fgetc(file); c != EOF; c=fgetc(file))
              (void) WriteBlobByte(image,c);
            (void) fclose(file);
            (void) remove(filename);
            break;
          }
          case RunlengthEncodedCompression:
          default:
          {
            /*
              Allocate pixel array.
            */
            length=(image->colorspace == CMYKColorspace ? 4 : 3)*number_pixels;
            pixels=(unsigned char *) AcquireMemory(length);
            if (pixels == (unsigned char *) NULL)
              {
                DestroyImage(tile_image);
                ThrowWriterException(ResourceLimitWarning,
                  "Memory allocation failed",image)
              }
            /*
              Dump runoffset encoded pixels.
            */
            q=pixels;
            for (y=0; y < (long) tile_image->rows; y++)
            {
              p=AcquireImagePixels(tile_image,0,y,tile_image->columns,1,
                &tile_image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) tile_image->columns; x++)
              {
                if (tile_image->matte && (p->opacity == TransparentOpacity))
                  {
                    *q++=DownScale(MaxRGB);
                    *q++=DownScale(MaxRGB);
                    *q++=DownScale(MaxRGB);
                    continue;
                  }
                *q++=DownScale(p->red);
                *q++=DownScale(p->green);
                *q++=DownScale(p->blue);
                if (image->colorspace == CMYKColorspace)
                  *q++=DownScale(p->opacity);
                p++;
              }
            }
            if (compression == ZipCompression)
              status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
            else
              if (compression == LZWCompression)
                status=LZWEncodeImage(image,length,pixels);
              else
                status=PackbitsEncodeImage(image,length,pixels);
            LiberateMemory((void **) &pixels);
            if (!status)
              {
                CloseBlob(image);
                return(False);
              }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            Ascii85Initialize(image);
            for (y=0; y < (long) tile_image->rows; y++)
            {
              p=AcquireImagePixels(tile_image,0,y,tile_image->columns,1,
                &tile_image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              for (x=0; x < (long) tile_image->columns; x++)
              {
                if (tile_image->matte && (p->opacity == TransparentOpacity))
                  {
                    Ascii85Encode(image,DownScale(MaxRGB));
                    Ascii85Encode(image,DownScale(MaxRGB));
                    Ascii85Encode(image,DownScale(MaxRGB));
                    continue;
                  }
                Ascii85Encode(image,DownScale(p->red));
                Ascii85Encode(image,DownScale(p->green));
                Ascii85Encode(image,DownScale(p->blue));
                if (image->colorspace == CMYKColorspace)
                  Ascii85Encode(image,DownScale(p->opacity));
                p++;
              }
            }
            Ascii85Flush(image);
            break;
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              length=number_pixels;
              pixels=(unsigned char *) AcquireMemory(length);
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  ThrowWriterException(ResourceLimitWarning,
                    "Memory allocation failed",image)
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (long) tile_image->rows; y++)
              {
                p=AcquireImagePixels(tile_image,0,y,tile_image->columns,1,
                  &tile_image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                for (x=0; x < (long) tile_image->columns; x++)
                  *q++=indexes[x];
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,length,image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,length,pixels);
                else
                  status=PackbitsEncodeImage(image,length,pixels);
              LiberateMemory((void **) &pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize(image);
              for (y=0; y < (long) tile_image->rows; y++)
              {
                p=AcquireImagePixels(tile_image,0,y,tile_image->columns,1,
                  &tile_image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                for (x=0; x < (long) tile_image->columns; x++)
                  Ascii85Encode(image,indexes[x]);
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    DestroyImage(tile_image);
    offset=TellBlob(image)-offset;
    (void) WriteBlobString(image,"\nendstream\n");
    (void) WriteBlobString(image,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    FormatString(buffer,"%u 0 obj\n",object);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%lu\n",(unsigned long) offset);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"endobj\n");
    if ((image->storage_class == DirectClass) ||
        (compression == FaxCompression))
      {
        xref[object++]=0;
        xref[object++]=0;
      }
    else
      {
        /*
          Write Colormap object.
        */
        xref[object++]=TellBlob(image);
        FormatString(buffer,"%u 0 obj\n",object);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"<<\n");
        if (compression == NoCompression)
          (void) WriteBlobString(image,"/Filter [ /ASCII85Decode ]\n");
        FormatString(buffer,"/Length %u 0 R\n",object+1);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,">>\n");
        (void) WriteBlobString(image,"stream\n");
        offset=TellBlob(image);
        if (compression == NoCompression)
          Ascii85Initialize(image);
        for (i=0; i < (long) image->colors; i++)
        {
          if (compression == NoCompression)
            {
              Ascii85Encode(image,DownScale(image->colormap[i].red));
              Ascii85Encode(image,DownScale(image->colormap[i].green));
              Ascii85Encode(image,DownScale(image->colormap[i].blue));
              continue;
            }
          (void) WriteBlobByte(image,DownScale(image->colormap[i].red));
          (void) WriteBlobByte(image,DownScale(image->colormap[i].green));
          (void) WriteBlobByte(image,DownScale(image->colormap[i].blue));
        }
        if (compression == NoCompression)
          Ascii85Flush(image);
        offset=TellBlob(image)-offset;
        (void) WriteBlobString(image,"\nendstream\n");
        (void) WriteBlobString(image,"endobj\n");
        /*
          Write Length object.
        */
        xref[object++]=TellBlob(image);
        FormatString(buffer,"%u 0 obj\n",object);
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,"%lu\n",(unsigned long) offset);
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"endobj\n");
      }
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  /*
    Write Xref object.
  */
  offset=TellBlob(image)-xref[0]+10;
  (void) WriteBlobString(image,"xref\n");
  FormatString(buffer,"0 %u\n",object+1);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"0000000000 65535 f \n");
  for (i=0; i < (long) object; i++)
  {
    FormatString(buffer,"%010lu 00000 n \n",xref[i]);
    (void) WriteBlobString(image,buffer);
  }
  (void) WriteBlobString(image,"trailer\n");
  (void) WriteBlobString(image,"<<\n");
  FormatString(buffer,"/Size %u\n",object+1);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Info %u 0 R\n",info_id);
  (void) WriteBlobString(image,buffer);
  FormatString(buffer,"/Root %u 0 R\n",root_id);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,">>\n");
  (void) WriteBlobString(image,"startxref\n");
  FormatString(buffer,"%lu\n",(unsigned long) offset);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"%%EOF\n");
  LiberateMemory((void **) &xref);
  CloseBlob(image);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) fputc(c,encode_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseBlob(&encode_image);
    }
  return(True);
}

#if defined(HasZLIB)
#include "zlib.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z L I B E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZLIBEncodeImage compresses an image via ZLIB-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary ZLIB bytes are encoded as ASCII base-85.
%
%  The format of the ZLIBEncodeImage method is:
%
%      unsigned int ZLIBEncodeImage(Image *image,const size_t length,
%        const unsigned long quality,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method ZLIBEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  ZLIB encoded pixels
%      are written to this file.
%
%    o length:  A value that specifies the number of pixels to compress.
%
%    o quality: the compression level (0-100).
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  int
    status;

  register long
    i;

  unsigned char
    *compressed_pixels;

  unsigned long
    compressed_packets;

  z_stream
    stream;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  compressed_packets=(unsigned long) (1.001*length+12);
  compressed_pixels=(unsigned char *) AcquireMemory(compressed_packets);
  if (compressed_pixels == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      (char *) NULL);
  stream.next_in=pixels;
  stream.avail_in=(unsigned int) length;
  stream.next_out=compressed_pixels;
  stream.avail_out=(unsigned int) compressed_packets;
  stream.zalloc=(alloc_func) NULL;
  stream.zfree=(free_func) NULL;
  stream.opaque=(voidpf) NULL;
  status=deflateInit(&stream,(int) Min(quality/10,9));
  if (status == Z_OK)
    {
      status=deflate(&stream,Z_FINISH);
      if (status == Z_STREAM_END)
        status=deflateEnd(&stream);
      else
        (void) deflateEnd(&stream);
      compressed_packets=stream.total_out;
    }
  if (status)
    ThrowBinaryException(DelegateWarning,"Unable to Zip compress image",
      (char *) NULL)
  else
    for (i=0; i < (long) compressed_packets; i++)
      (void) WriteBlobByte(image,compressed_pixels[i]);
  LiberateMemory((void **) &compressed_pixels);
  return(!status);
}
#else
static unsigned int ZLIBEncodeImage(Image *image,const size_t length,
  const unsigned long quality,unsigned char *pixels)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateWarning,"ZLIB library is not available",
    image->filename);
  return(False);
}
#endif
