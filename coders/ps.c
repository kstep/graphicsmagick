/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                               PPPP   SSSSS                                  %
%                               P   P  SS                                     %
%                               PPPP    SSS                                   %
%                               P         SS                                  %
%                               P      SSSSS                                  %
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
  WritePSImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P S                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPS returns True if the image format type, identified by the
%  magick string, is PS.
%
%  The format of the IsPS method is:
%
%      unsigned int IsPS(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPS returns True if the image format type is PS.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPS(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"%!",2) == 0)
    return(True);
  if (memcmp(magick,"\004%!",3) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P S I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPSImage reads a Adobe Postscript image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPSImage method is:
%
%      Image *ReadPSImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPSImage(const ImageInfo *image_info,ExceptionInfo *exception)
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

  unsigned long
    height,
    filesize,
    width;

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
  filesize=0;
  if (LocaleCompare(image_info->magick,"EPT") == 0)
    {
      /*
        Dos binary file header.
      */
      (void) ReadBlobLSBLong(image);
      count=ReadBlobLSBLong(image);
      filesize=ReadBlobLSBLong(image);
      for (i=0; i < (long) (count-12); i++)
        (void) ReadBlobByte(image);
    }
  box.width=0;
  box.height=0;
  p=command;
  for (i=0; (LocaleCompare(image_info->magick,"EPT") != 0) ||
    (i < (long) filesize); i++)
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
    if ((bounds.x1 > bounds.x2) || (bounds.y1 > bounds.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    FormatString(translate_geometry,"%f %f translate\n",-bounds.x1,-bounds.y1);
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
    (void) ParseImageGeometry(image_info->page,&page.x,&page.y,&page.width,
      &page.height);
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
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",image_info->subimage+1,
      image_info->subimage+image_info->subrange);
  (void) strncpy(filename,image_info->filename,MaxTextExtent-1);
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
      image=ReadImage(image_info,exception);
      if (image != (Image *) NULL)
        return(image);
      ThrowReaderException(CorruptImageWarning,"Postscript delegate failed",
        image)
    }
  clone_info=CloneImageInfo(image_info);
  DetachBlob(clone_info->blob);
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    ThrowReaderException(CorruptImageWarning,"Postscript delegate failed",
      image);
  do
  {
    (void) strcpy(image->magick,"PS");
    (void) strncpy(image->filename,filename,MaxTextExtent-1);
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
%   R e g i s t e r P S I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPSImage adds attributes for the PS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPSImage method is:
%
%      RegisterPSImage(void)
%
*/
ModuleExport void RegisterPSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPI");
  entry->decoder=ReadPSImage;
  entry->encoder=WritePSImage;
  entry->magick=IsPS;
  entry->adjoin=False;
  entry->description=
    AllocateString("Adobe Encapsulated PostScript Interchange format");
  entry->module=AllocateString("PS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("EPS");
  entry->decoder=ReadPSImage;
  entry->encoder=WritePSImage;
  entry->magick=IsPS;
  entry->adjoin=False;
  entry->description=AllocateString("Adobe Encapsulated PostScript");
  entry->module=AllocateString("PS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("EPSF");
  entry->decoder=ReadPSImage;
  entry->encoder=WritePSImage;
  entry->magick=IsPS;
  entry->adjoin=False;
  entry->description=AllocateString("Adobe Encapsulated PostScript");
  entry->module=AllocateString("PS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("EPSI");
  entry->decoder=ReadPSImage;
  entry->encoder=WritePSImage;
  entry->magick=IsPS;
  entry->adjoin=False;
  entry->description=
    AllocateString("Adobe Encapsulated PostScript Interchange format");
  entry->module=AllocateString("PS");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS");
  entry->decoder=ReadPSImage;
  entry->encoder=WritePSImage;
  entry->magick=IsPS;
  entry->description=AllocateString("Adobe PostScript");
  entry->module=AllocateString("PS");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPSImage removes format registrations made by the
%  PS module from the list of supported formats.
%
%  The format of the UnregisterPSImage method is:
%
%      UnregisterPSImage(void)
%
*/
ModuleExport void UnregisterPSImage(void)
{
  (void) UnregisterMagickInfo("EPI");
  (void) UnregisterMagickInfo("EPS");
  (void) UnregisterMagickInfo("EPSF");
  (void) UnregisterMagickInfo("EPSI");
  (void) UnregisterMagickInfo("PS");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePSImage translates an image to encapsulated Postscript
%  Level I for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePSImage method is:
%
%      unsigned int WritePSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePSImage return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static unsigned int WritePSImage(const ImageInfo *image_info,Image *image)
{
#define WriteRunlengthPacket(image,pixel,length,p) \
{ \
  if (image->matte && (p->opacity == TransparentOpacity)) \
    FormatString(buffer,"ffffff%02x",(unsigned int) Min(length,0xff)); \
  else \
    FormatString(buffer,"%02lx%02lx%02lx%02lx",DownScale(pixel.red), \
      DownScale(pixel.green),DownScale(pixel.blue), \
      (unsigned long) Min(length,0xff)); \
  (void) WriteBlobString(image,buffer); \
}

  static const char 
    *PostscriptProlog[]=
    {
      "%%BeginProlog",
      "%",
      "% Display a color image.  The image is displayed in color on",
      "% Postscript viewers or printers that support color, otherwise",
      "% it is displayed as grayscale.",
      "%",
      "/DirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet.",
      "  %",
      "  % Parameters:",
      "  %   red.",
      "  %   green.",
      "  %   blue.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  compression 0 eq",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/DirectClassImage",
      "{",
      "  %",
      "  % Display a DirectClass image.",
      "  %",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { DirectClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayDirectClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/GrayDirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters:",
      "  %   red",
      "  %   green",
      "  %   blue",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 eq",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/GrayPseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters:",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 eq",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet.",
      "  %",
      "  % Parameters:",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  compression 0 eq",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %",
      "  % Display a PseudoClass image.",
      "  %",
      "  % Parameters:",
      "  %   class: 0-PseudoClass or 1-Grayscale.",
      "  %",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  class 0 gt",
      "  {",
      "    currentfile buffer readline pop",
      "    token pop /depth exch def pop",
      "    /grays columns 8 add depth sub depth mul 8 idiv string def",
      "    columns rows depth",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { currentfile grays readhexstring pop } image",
      "  }",
      "  {",
      "    %",
      "    % Parameters:",
      "    %   colors: number of colors in the colormap.",
      "    %   colormap: red, green, blue color packets.",
      "    %",
      "    currentfile buffer readline pop",
      "    token pop /colors exch def pop",
      "    /colors colors 3 mul def",
      "    /colormap colors string def",
      "    currentfile colormap readhexstring pop pop",
      "    systemdict /colorimage known",
      "    {",
      "      columns rows 8",
      "      [",
      "        columns 0 0",
      "        rows neg 0 rows",
      "      ]",
      "      { PseudoClassPacket } false 3 colorimage",
      "    }",
      "    {",
      "      %",
      "      % No colorimage operator;  convert to grayscale.",
      "      %",
      "      columns rows 8",
      "      [",
      "        columns 0 0",
      "        rows neg 0 rows",
      "      ]",
      "      { GrayPseudoClassPacket } image",
      "    } ifelse",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %",
      "  % Display a DirectClass or PseudoClass image.",
      "  %",
      "  % Parameters:",
      "  %   x & y translation.",
      "  %   x & y scale.",
      "  %   label pointsize.",
      "  %   image label.",
      "  %   image columns & rows.",
      "  %   class: 0-DirectClass or 1-PseudoClass.",
      "  %   compression: 0-none or 1-RunlengthEncoded.",
      "  %   hex color packets.",
      "  %",
      "  gsave",
      "  /buffer 512 string def",
      "  /byte 1 string def",
      "  /color_packet 3 string def",
      "  /pixels 768 string def",
      "",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      "  /Times-Roman findfont pointsize scalefont setfont",
      (char *) NULL
    },
    *PostscriptEpilog[]=
    {
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  grestore",
      (char *) NULL
    };

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    geometry[MaxTextExtent],
    **labels;

  const char
    **q;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ImageAttribute
    *attribute;

  IndexPacket
    index;

  int
    length;

  long
    x,
    y;

  long
    j;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register PixelPacket
    *p;

  register long
    i;

  SegmentInfo
    bounds;

  time_t
    timer;

  unsigned int
    bit,
    byte,
    count,
    page,
    polarity,
    scene,
    status,
    text_size;

  unsigned long
    height,
    width;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  page=1;
  scene=0;
  do
  {
    /*
      Scale image to size of Postscript page.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    text_size=0;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (ImageAttribute *) NULL)
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
        if (LocaleCompare(image_info->magick,"PS") == 0)
          (void) strcpy(geometry,PSPageGeometry);
    (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
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
    width=(unsigned int) x_scale;
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) y_scale;
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (LocaleCompare(image_info->magick,"PS") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"%%Creator: (ImageMagick)\n");
        FormatString(buffer,"%%%%Title: (%.1024s)\n",image->filename);
        (void) WriteBlobString(image,buffer);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strncpy(date,ctime(&timer),MaxTextExtent-1);
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: (%.1024s)\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=x;
        bounds.y1=y;
        bounds.x2=x+x_scale;
        bounds.y2=y+(height+text_size);
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) strcpy(buffer,"%%%%BoundingBox: (atend)\n");
        else
          FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
            floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
            ceil(bounds.y2-0.5));
        (void) WriteBlobString(image,buffer);
        attribute=GetImageAttribute(image,"label");
        if (attribute != (ImageAttribute *) NULL)
          (void) WriteBlobString(image,
            "%%%%DocumentNeededResources: font Times-Roman\n");
        (void) WriteBlobString(image,"%%DocumentData: Clean7Bit\n");
        (void) WriteBlobString(image,"%%LanguageLevel: 1\n");
        if (LocaleCompare(image_info->magick,"PS") != 0)
          (void) WriteBlobString(image,"%%Pages: 1\n");
        else
          {
            /*
              Compute the number of pages.
            */
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            FormatString(buffer,"%%%%Pages: %u\n",
              image_info->adjoin ? GetNumberScenes(image) : 1);
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");
        (void) WriteBlobString(image,"\n%%BeginDefaults\n");
        (void) WriteBlobString(image,"%%PageOrientation: Portrait\n");
        (void) WriteBlobString(image,"%%EndDefaults\n\n");
        if ((LocaleCompare(image_info->magick,"EPI") == 0) ||
            (LocaleCompare(image_info->magick,"EPT") == 0) ||
            (LocaleCompare(image_info->magick,"EPSI") == 0))
          {
            Image
              *preview_image;

            long
              y;

            register long
              x;

            /*
              Create preview image.
            */
            preview_image=CloneImage(image,0,0,True,&image->exception);
            if (preview_image == (Image *) NULL)
              ThrowWriterException(ResourceLimitWarning,
                "Memory allocation failed",image);
            /*
              Dump image as bitmap.
            */
            if (!IsMonochromeImage(preview_image))
              {
                QuantizeInfo
                  quantize_info;
  
                GetQuantizeInfo(&quantize_info);
                quantize_info.number_colors=2;
                quantize_info.dither=image_info->dither;
                quantize_info.colorspace=GRAYColorspace;
                (void) QuantizeImage(&quantize_info,preview_image);
              }
            polarity=Intensity(preview_image->colormap[0]) < (0.5*MaxRGB);
            if (preview_image->colors == 2)
              polarity=Intensity(preview_image->colormap[0]) >
                Intensity(preview_image->colormap[1]);
            FormatString(buffer,"%%%%BeginPreview: %u %u %u %u\n%%  ",
              preview_image->columns,preview_image->rows,(unsigned int) 1,
              (((preview_image->columns+7) >> 3)*preview_image->rows+35)/36);
            (void) WriteBlobString(image,buffer);
            count=0;
            for (y=0; y < (long) image->rows; y++)
            {
              p=GetImagePixels(preview_image,0,y,preview_image->columns,1);
              if (p == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(preview_image);
              bit=0;
              byte=0;
              for (x=0; x < (long) preview_image->columns; x++)
              {
                byte<<=1;
                if (indexes[x] == polarity)
                  byte|=0x01;
                bit++;
                if (bit == 8)
                  {
                    FormatString(buffer,"%02x",byte & 0xff);
                    (void) WriteBlobString(image,buffer);
                    count++;
                    if (count == 36)
                      {
                        (void) WriteBlobString(image,"\n%  ");
                        count=0;
                      };
                    bit=0;
                    byte=0;
                  }
              }
              if (bit != 0)
                {
                  byte<<=(8-bit);
                  FormatString(buffer,"%02x",byte & 0xff);
                  (void) WriteBlobString(image,buffer);
                  count++;
                  if (count == 36)
                    {
                      (void) WriteBlobString(image,"\n%  ");
                      count=0;
                    };
                };
            }
            (void) WriteBlobString(image,"\n%%EndPreview\n");
            DestroyImage(preview_image);
          }
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
        {
          FormatString(buffer,"%.255s\n",*q);
          (void) WriteBlobString(image,buffer);
        }
        attribute=GetImageAttribute(image,"label");
        if (attribute != (ImageAttribute *) NULL)
          for (j=MultilineCensus(attribute->value)-1; j >= 0; j--)
          {
            (void) WriteBlobString(image,"  /label 512 string def\n");
            (void) WriteBlobString(image,"  currentfile label readline pop\n");
            FormatString(buffer,"  0 y %f add moveto label show pop\n",
              j*image_info->pointsize+12);
            (void) WriteBlobString(image,buffer);
          }
        for (q=PostscriptEpilog; *q; q++)
        {
          FormatString(buffer,"%.255s\n",*q);
          (void) WriteBlobString(image,buffer);
        }
        if (LocaleCompare(image_info->magick,"PS") == 0)
          (void) WriteBlobString(image,"  showpage\n");
        (void) WriteBlobString(image,"} bind def\n");
        (void) WriteBlobString(image,"%%EndProlog\n");
      }
    FormatString(buffer,"%%%%Page:  1 %u\n",page++);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%%%%PageBoundingBox: %ld %ld %ld %ld\n",x,y,
      x+(long) width,y+(long) (height+text_size));
    (void) WriteBlobString(image,buffer);
    if (x < bounds.x1)
      bounds.x1=x;
    if (y < bounds.y1)
      bounds.y1=y;
    if ((x+(long) width-1) > bounds.x2)
      bounds.x2=x+width-1;
    if ((y+(long) (height+text_size)-1) > bounds.y2)
      bounds.y2=y+(height+text_size)-1;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (ImageAttribute *) NULL)
      (void) WriteBlobString(image,"%%%%PageResources: font Times-Roman\n");
    if (LocaleCompare(image_info->magick,"PS") != 0)
      (void) WriteBlobString(image,"userdict begin\n");
    (void) WriteBlobString(image,"DisplayImage\n");
    /*
      Output image data.
    */
    FormatString(buffer,"%d %d\n%g %g\n%f\n",x,y,x_scale,y_scale,
      image_info->pointsize);
    (void) WriteBlobString(image,buffer);
    labels=(char **) NULL;
    attribute=GetImageAttribute(image,"label");
    if (attribute != (ImageAttribute *) NULL)
      labels=StringToList(attribute->value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          FormatString(buffer,"%.1024s \n",labels[i]);
          (void) WriteBlobString(image,buffer);
          LiberateMemory((void **) &labels[i]);
        }
        LiberateMemory((void **) &labels);
      }
    (void) memset(&pixel,0,sizeof(PixelPacket));
    pixel.opacity=TransparentOpacity;
    i=0;
    index=0;
    x=0;
    if (image->storage_class == DirectClass)
      {
        /*
          Dump DirectClass image.
        */
        FormatString(buffer,"%lu %lu\n%d\n%d\n",image->columns,image->rows,
          (int) (image->storage_class == PseudoClass),
          (int) (image_info->compression == RunlengthEncodedCompression));
        (void) WriteBlobString(image,buffer);
        switch (image_info->compression)
        {
          case RunlengthEncodedCompression:
          {
            /*
              Dump runlength-encoded DirectColor packets.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              p=GetImagePixels(image,0,y,image->columns,1);
              if (p == (PixelPacket *) NULL)
                break;
              pixel=(*p);
              length=255;
              for (x=0; x < (long) image->columns; x++)
              {
                if ((p->red == pixel.red) && (p->green == pixel.green) &&
                    (p->blue == pixel.blue) && (p->opacity == pixel.opacity) &&
                    (length < 255) && (x < (long) (image->columns-1)))
                  length++;
                else
                  {
                    if (x > 0)
                      {
                        WriteRunlengthPacket(image,pixel,length,p);
                        i++;
                        if (i == 9)
                          {
                            (void) WriteBlobByte(image,'\n');
                            i=0;
                          }
                      }
                    length=0;
                  }
                pixel=(*p);
                p++;
              }
              WriteRunlengthPacket(image,pixel,length,p);
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
            break;
          }
          case NoCompression:
          default:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            i=0;
            for (y=0; y < (long) image->rows; y++)
            {
              p=GetImagePixels(image,0,y,image->columns,1);
              if (p == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
              {
                if (image->matte && (p->opacity == TransparentOpacity))
                  (void) strcpy(buffer,"ffffff");
                else
                  FormatString(buffer,"%02lx%02lx%02lx",
                    DownScale(p->red),DownScale(p->green),DownScale(p->blue));
                (void) WriteBlobString(image,buffer);
                i++;
                if (i == 12)
                  {
                    i=0;
                    (void) WriteBlobByte(image,'\n');
                  }
                p++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
            break;
          }
        }
        (void) WriteBlobByte(image,'\n');
      }
    else
      if (IsGrayImage(image))
        {
          FormatString(buffer,"%u %u\n1\n1\n1\n%d\n",
            image->columns,image->rows,IsMonochromeImage(image) ? 1 : 8);
          (void) WriteBlobString(image,buffer);
          if (!IsMonochromeImage(image))
            {
              /*
                Dump image as grayscale.
              */
              i++;
              for (y=0; y < (long) image->rows; y++)
              {
                p=GetImagePixels(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (long) image->columns; x++)
                {
                  FormatString(buffer,"%02lx",DownScale(Intensity(*p)));
                  (void) WriteBlobString(image,buffer);
                  i++;
                  if (i == 36)
                    {
                      (void) WriteBlobByte(image,'\n');
                      i=0;
                    }
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
            }
          else
            {
              int
                y;

              /*
                Dump image as bitmap.
              */
              polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
              if (image->colors == 2)
                polarity=
                  Intensity(image->colormap[1]) > Intensity(image->colormap[0]);
              count=0;
              for (y=0; y < (long) image->rows; y++)
              {
                p=GetImagePixels(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                bit=0;
                byte=0;
                for (x=0; x < (long) image->columns; x++)
                {
                  byte<<=1;
                  if (indexes[x] == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      FormatString(buffer,"%02x",byte & 0xff);
                      (void) WriteBlobString(image,buffer);
                      count++;
                      if (count == 36)
                        {
                          (void) WriteBlobByte(image,'\n');
                          count=0;
                        };
                      bit=0;
                      byte=0;
                    }
                  p++;
                }
                if (bit != 0)
                  {
                    byte<<=(8-bit);
                    FormatString(buffer,"%02x",byte & 0xff);
                    (void) WriteBlobString(image,buffer);
                    count++;
                    if (count == 36)
                      {
                        (void) WriteBlobByte(image,'\n');
                        count=0;
                      };
                  };
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
            }
          if (count != 0)
            (void) WriteBlobByte(image,'\n');
        }
      else
        {
          /*
            Dump PseudoClass image.
          */
          FormatString(buffer,"%lu %lu\n%d\n%d\n0\n",image->columns,image->rows,
            (int) (image->storage_class == PseudoClass),
            (int) (image_info->compression == RunlengthEncodedCompression));
          (void) WriteBlobString(image,buffer);
          /*
            Dump number of colors and colormap.
          */
          FormatString(buffer,"%u\n",image->colors);
          (void) WriteBlobString(image,buffer);
          for (i=0; i < (long) image->colors; i++)
          {
            FormatString(buffer,"%02lx%02lx%02lx\n",
              DownScale(image->colormap[i].red),
              DownScale(image->colormap[i].green),
              DownScale(image->colormap[i].blue));
            (void) WriteBlobString(image,buffer);
          }
          switch (image_info->compression)
          {
            case RunlengthEncodedCompression:
            {
              /*
                Dump runlength-encoded PseudoColor packets.
              */
              i=0;
              for (y=0; y < (long) image->rows; y++)
              {
                p=GetImagePixels(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                index=(*indexes);
                length=255;
                for (x=0; x < (long) image->columns; x++)
                {
                  if ((index == indexes[x]) && (length < 255) &&
                      (x < ((long) image->columns-1)))
                    length++;
                  else
                    {
                      if (x > 0)
                        {
                          FormatString(buffer,"%02x%02x",(unsigned int)
                            index,(unsigned int) Min(length,0xff));
                          (void) WriteBlobString(image,buffer);
                          i++;
                          if (i == 18)
                            {
                              (void) WriteBlobByte(image,'\n');
                              i=0;
                            }
                        }
                      length=0;
                    }
                  index=indexes[x];
                  pixel=(*p);
                  p++;
                }
                FormatString(buffer,"%02x%02x",(unsigned int) index,
                  (unsigned int) Min(length,0xff));
                (void) WriteBlobString(image,buffer);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
              break;
            }
            case NoCompression:
            default:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              i=0;
              for (y=0; y < (long) image->rows; y++)
              {
                p=GetImagePixels(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                {
                  FormatString(buffer,"%02x",indexes[x]);
                  (void) WriteBlobString(image,buffer);
                  i++;
                  if (i == 36)
                    {
                      (void) WriteBlobByte(image,'\n');
                      i=0;
                    }
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
              break;
            }
          }
          (void) WriteBlobByte(image,'\n');
        }
    if (LocaleCompare(image_info->magick,"PS") != 0)
      (void) WriteBlobString(image,"end\n");
    (void) WriteBlobString(image,"%%PageTrailer\n");
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) WriteBlobString(image,"%%Trailer\n");
  if (page > 2)
    {
      FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",floor(bounds.x1+0.5),
        floor(bounds.y1+0.5),ceil(bounds.x2-0.5),ceil(bounds.y2-0.5));
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  CloseBlob(image);
  return(True);
}
