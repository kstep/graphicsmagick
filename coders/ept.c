/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            EEEEE  PPPP   TTTTT                              %
%                            E      P   P    T                                %
%                            EEE    PPPP     T                                %
%                            E      P        T                                %
%                            EEEEE  P        T                                %
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

/*
  Forward declarations.
*/
static unsigned int
  WriteEPTImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s E P T                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsEPT returns True if the image format type, identified by the
%  magick string, is EPT.
%
%  The format of the IsEPT method is:
%
%      unsigned int IsEPT(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsEPT returns True if the image format type is EPT.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsEPT(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\305\320\323\306",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d E P T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadEPTImage reads a binary Adobe Postscript image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadEPTImage method is:
%
%      Image *ReadEPTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadEPTImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadEPTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BoundingBox  "%%BoundingBox:"
#define DocumentMedia  "%%DocumentMedia:"
#define PageBoundingBox  "%%PageBoundingBox:"
#define PostscriptLevel  "%!PS-"

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent],
    translate_geometry[MaxTextExtent];

  DelegateInfo
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
    c,
    status;

  RectangleInfo
    box,
    page;

  register char
    *p;

  register long
    i;

  SegmentInfo
    bounds;

  size_t
    count;

  unsigned int
    height,
    width;

  unsigned long
    filesize;

  if (image_info->monochrome)
    {
      delegate_info=GetDelegateInfo("gs-mono",(char *) NULL,exception);
      if (delegate_info == (DelegateInfo *) NULL)
        return((Image *) NULL);
    }
  else
    {
      delegate_info=GetDelegateInfo("gs-color",(char *) NULL,exception);
      if (delegate_info == (DelegateInfo *) NULL)
        return((Image *) NULL);
    }
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to write file",image);
  FormatString(translate_geometry,"%f %f translate\n              ",0.0,0.0);
  (void) fputs(translate_geometry,file);
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
  (void) ParseImageGeometry(PSPageGeometry,&page.x,&page.y,&page.width,
    &page.height);
  /*
    Determine page geometry from the Postscript bounding box.
  */
  (void) ReadBlobLSBLong(image);
  count=ReadBlobLSBLong(image);
  filesize=ReadBlobLSBLong(image);
  for (i=0; i < (long) (count-12); i++)
    (void) ReadBlobByte(image);
  /*
    Copy Postscript to temporary file.
  */
  box.width=0;
  box.height=0;
  p=command;
  for (i=0; i < (long) filesize; i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Parse a bounding box statement.
    */
    count=0;
    if (LocaleNCompare(BoundingBox,command,strlen(BoundingBox)) == 0)
      count=sscanf(command,"%%%%BoundingBox: %lf %lf %lf %lf",&bounds.x1,
        &bounds.y1,&bounds.x2,&bounds.y2);
    if (LocaleNCompare(DocumentMedia,command,strlen(DocumentMedia)) == 0)
      count=sscanf(command,"%%%%DocumentMedia: %*s %lf %lf",&bounds.x2,
        &bounds.y2)+2;
    if (LocaleNCompare(PageBoundingBox,command,strlen(PageBoundingBox)) == 0)
      count=sscanf(command,"%%%%PageBoundingBox: %lf %lf %lf %lf",
        &bounds.x1,&bounds.y1,&bounds.x2,&bounds.y2);
    if (count != 4)
      continue;
    if ((bounds.x1 > bounds.x2) ||
        (bounds.y1 > bounds.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    FormatString(translate_geometry,"%f %f translate\n",-bounds.x1,
      -bounds.y1);
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
    (void) ParseImageGeometry(image_info->page,&page.x,&page.y,
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
  (void) rewind(file);
  (void) fputs(translate_geometry,file);
  (void) fclose(file);
  CloseBlob(image);
  filesize=SizeBlob(image);
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strcpy(filename,image_info->filename);
  TemporaryFilename((char *) image_info->filename);
  FormatString(command,delegate_info->commands,image_info->antialias ? 4 : 1,
    image_info->antialias ? 4 : 1,geometry,density,options,image_info->filename,
    postscript_filename);
  MagickMonitor(RenderPostscriptText,0,8);
  status=ExecutePostscriptInterpreter(image_info->verbose,command);
  if (!IsAccessible(image_info->filename))
    {
      /*
        Ghostscript requires a showpage operator.
      */
      file=fopen(postscript_filename,AppendBinaryType);
      if (file == (FILE *) NULL)
        ThrowReaderException(FileOpenWarning,"Unable to write file",image);
      (void) fputs("showpage\n",file);
      (void) fclose(file);
      status=ExecutePostscriptInterpreter(image_info->verbose,command);
    }
  (void) remove(postscript_filename);
  MagickMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      /*
        Ghostscript has failed-- try the Display Postscript Extension.
      */
      (void) FormatString((char *) image_info->filename,"dps:%.1024s",filename);
      image=ReadImage((ImageInfo *) image_info,exception);
      if (image != (Image *) NULL)
        return(image);
      ThrowReaderException(CorruptImageWarning,"Postscript delegate failed",
        image)
    }
  clone_info=CloneImageInfo(image_info);
  RewindBlob(clone_info->blob);
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    ThrowReaderException(CorruptImageWarning,"Postscript delegate failed",
      image);
  (void) strcpy((char *) image_info->filename,filename);
  do
  {
    (void) strcpy(image->magick,"PS");
    (void) strcpy(image->filename,image_info->filename);
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
%   R e g i s t e r E P T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterEPTImage adds attributes for the EPT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterEPTImage method is:
%
%      RegisterEPTImage(void)
%
*/
ModuleExport void RegisterEPTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPT");
  entry->decoder=ReadEPTImage;
  entry->encoder=WriteEPTImage;
  entry->magick=IsEPT;
  entry->adjoin=False;
  entry->description=
    AllocateString("Adobe Encapsulated PostScript with TIFF preview");
  entry->module=AllocateString("EPT");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r E P T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterEPTImage removes format registrations made by the
%  EPT module from the list of supported formats.
%
%  The format of the UnregisterEPTImage method is:
%
%      UnregisterEPTImage(void)
%
*/
ModuleExport void UnregisterEPTImage(void)
{
  (void) UnregisterMagickInfo("EPT");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e E P T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteEPTImage writes an image in the Adobe Encapsulated Postscript
%  format with a TIFF preview.
%
%  The format of the WriteEPTImage method is:
%
%      unsigned int WriteEPTImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteEPTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteEPTImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent],
    ps_filename[MaxTextExtent],
    tiff_filename[MaxTextExtent];

  FILE
    *ps_file,
    *tiff_file;

  unsigned int
    status;

  (void) strcpy(filename,image->filename);
  (void) strcpy(ps_filename,image->magick_filename);
  if (LocaleCompare(image_info->magick,"EPS") != 0)
    {
      /*
        Write image as Encapsulated Postscript to a temporary file.
      */
      TemporaryFilename(ps_filename);
      FormatString(image->filename,"eps:%.1024s",ps_filename);
      (void) WriteImage(image_info,image);
    }
  /*
    Write image as TIFF to a temporary file.
  */
  TemporaryFilename(tiff_filename);
  FormatString(image->filename,"tiff:%.1024s",tiff_filename);
  (void) strcpy(image->filename,tiff_filename);
  (void) WriteImage(image_info,image);
  /*
    Write EPT image.
  */
  (void) strcpy(image->filename,filename);
  status=OpenBlob(image_info,image,WriteBinaryType);
  ps_file=fopen(ps_filename,ReadBinaryType);
  status&=ps_file != (FILE *) NULL;
  tiff_file=fopen(tiff_filename,ReadBinaryType);
  status&=tiff_file != (FILE *) NULL;
  if (status != False)
    {
      int
        c;

      struct stat
        attributes;

      /*
        Write EPT image.
      */
      WriteBlobLSBLong(image,0xc6d3d0c5ul);
      WriteBlobLSBLong(image,30);
      attributes.st_size=0;
      (void) fstat(fileno(ps_file),&attributes);
      WriteBlobLSBLong(image,attributes.st_size);
      WriteBlobLSBLong(image,0);
      WriteBlobLSBLong(image,0);
      WriteBlobLSBLong(image,attributes.st_size+30);
      (void) fstat(fileno(tiff_file),&attributes);
      WriteBlobLSBLong(image,attributes.st_size);
      (void) WriteBlobLSBShort(image,0xffff);
      for (c=fgetc(ps_file); c != EOF; c=fgetc(ps_file))
        (void) WriteBlobByte(image,c);
      for (c=fgetc(tiff_file); c != EOF; c=fgetc(tiff_file))
        (void) WriteBlobByte(image,c);
      CloseBlob(image);
    }
  (void) fclose(ps_file);
  (void) fclose(tiff_file);
  if (LocaleCompare(image_info->magick,"EPS") != 0)
    (void) remove(ps_filename);
  (void) remove(tiff_filename);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  return(True);
}
