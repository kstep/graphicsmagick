/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   SSSSS  22222                              %
%                            P   P  SS        22                              %
%                            PPPP    SSS    222                               %
%                            P         SS  22                                 %
%                            P      SSSSS  22222                              %
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
  WritePS2Image(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPS2Image adds attributes for the PS2 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPS2Image method is:
%
%      RegisterPS2Image(void)
%
*/
ModuleExport void RegisterPS2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPS2");
  entry->encoder=WritePS2Image;
  entry->adjoin=False;
  entry->description=AllocateString("Adobe Level II Encapsulated PostScript");
  entry->module=AllocateString("PS2");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PS2");
  entry->encoder=WritePS2Image;
  entry->description=AllocateString("Adobe Level II PostScript");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S 2 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPS2Image removes format registrations made by the
%  PS2 module from the list of supported formats.
%
%  The format of the UnregisterPS2Image method is:
%
%      UnregisterPS2Image(void)
%
*/
ModuleExport void UnregisterPS2Image(void)
{
  (void) UnregisterMagickInfo("EPS2");
  (void) UnregisterMagickInfo("PS2");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS2Image translates an image to encapsulated Postscript
%  Level II for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS2Image method is:
%
%      unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS2Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
{
  static const char
    *PostscriptProlog[]=
    {
      "%%%%BeginProlog",
      "%%",
      "%% Display a color image.  The image is displayed in color on",
      "%% Postscript viewers or printers that support color, otherwise",
      "%% it is displayed as grayscale.",
      "%%",
      "/DirectClassImage",
      "{",
      "  %%",
      "  %% Display a DirectClass image.",
      "  %%",
      "  colorspace 0 eq",
      "  {",
      "    /DeviceRGB setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 1 0 1 0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  }",
      "  {",
      "    /DeviceCMYK setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 1 0 1 0 1 0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  } ifelse",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %%",
      "  %% Display a PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   colors: number of colors in the colormap.",
      "  %%",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  colors 0 eq",
      "  {",
      "    %%",
      "    %% Image is grayscale.",
      "    %%",
      "    /DeviceGray setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 1",
      "      /Decode [0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      {",
      "        /DataSource pixel_stream /%.1024s filter",
      "        <<",
      "           /K "CCITTParam,
      "           /Columns columns",
      "           /Rows rows",
      "        >> /CCITTFaxDecode filter",
      "      } ifelse",
      "    >> image",
      "  }",
      "  {",
      "    %%",
      "    %% Parameters:",
      "    %%   colormap: red, green, blue color packets.",
      "    %%",
      "    /colormap colors 3 mul string def",
      "    currentfile colormap readhexstring pop pop",
      "    [ /Indexed /DeviceRGB colors 1 sub colormap ] setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 255]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream /%.1024s filter }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %%",
      "  %% Display a DirectClass or PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   x & y translation.",
      "  %%   x & y scale.",
      "  %%   label pointsize.",
      "  %%   image label.",
      "  %%   image columns & rows.",
      "  %%   class: 0-DirectClass or 1-PseudoClass.",
      "  %%   colorspace: 0-RGB or 1-CMYK.",
      "  %%   compression: 0-RunlengthEncodedCompression or 1-NoCompression.",
      "  %%   hex color packets.",
      "  %%",
      "  gsave",
      "  /buffer 512 string def",
      "  /pixel_stream currentfile def",
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
      "  /Helvetica findfont pointsize scalefont setfont",
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
      "  token pop /colorspace exch def pop",
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

  CompressionType
    compression;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ImageAttribute
    *attribute;

  int
    count,
    status;

  long
    j,
    x,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i;

  SegmentInfo
    bounds;

  size_t
    length;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    page,
    scene,
    text_size;

  unsigned long
    height,
    number_pixels,
    width;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
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
  page=1;
  scene=0;
  do
  {
    if (compression == FaxCompression)
      if ((image->storage_class == DirectClass) ||
          !IsMonochromeImage(image,&image->exception))
        SetImageType(image,BilevelType);
    if ((compression != NoCompression) &&
        (compression != RunlengthEncodedCompression))
      image->storage_class=DirectClass;
    /*
      Scale image to size of Postscript page.
    */
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
        if (LocaleCompare(image_info->magick,"PS2") == 0)
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
    width=(unsigned int) ceil(x_scale-0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) ceil(y_scale-0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (LocaleCompare(image_info->magick,"PS2") == 0)
          (void) strcpy(buffer,"%!PS-Adobe-3.0\n");
        else
          (void) strcpy(buffer,"%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) WriteBlobString(image,buffer);
        (void) WriteBlobString(image,"%%Creator: (ImageMagick)\n");
        FormatString(buffer,"%%Title: (%.1024s)\n",image->filename);
        (void) WriteBlobString(image,buffer);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strncpy(date,ctime(&timer),MaxTextExtent-1);
        date[strlen(date)-1]='\0';
        FormatString(buffer,"%%%%CreationDate: (%.1024s)\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=x;
        bounds.y1=y;
        bounds.x2=x+width;
        bounds.y2=y+(height+text_size);
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) strcpy(buffer,"%%BoundingBox: (atend)\n");
        else
          FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",
            floor(bounds.x1+0.5),floor(bounds.y1+0.5),ceil(bounds.x2-0.5),
            ceil(bounds.y2-0.5));
        (void) WriteBlobString(image,buffer);
        attribute=GetImageAttribute(image,"label");
        if (attribute != (ImageAttribute *) NULL)
          (void) WriteBlobString(image,
            "%%%%DocumentNeededResources: font Helvetica\n");
        (void) WriteBlobString(image,"%%LanguageLevel: 2\n");
        if (LocaleCompare(image_info->magick,"PS2") != 0)
          (void) WriteBlobString(image,"%%%%Pages: 0\n");
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (!image_info->adjoin)
              (void) strcpy(buffer,"%%Pages: 0\n");
            else
              FormatString(buffer,"%%%%Pages: %u\n",GetNumberScenes(image));
            (void) WriteBlobString(image,buffer);
          }
        (void) WriteBlobString(image,"%%EndComments\n");
        (void) WriteBlobString(image,"\n%%BeginDefaults\n");
        (void) WriteBlobString(image,"%%PageOrientation: Portrait\n");
        (void) WriteBlobString(image,"%%EndDefaults\n\n");
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
        {
          switch (compression)
          {
            case NoCompression: FormatString(buffer,*q,"ASCII85Decode"); break;
            case JPEGCompression: FormatString(buffer,*q,"DCTDecode"); break;
            case LZWCompression: FormatString(buffer,*q,"LZWDecode"); break;
            case FaxCompression: FormatString(buffer,*q,"ASCII85Decode"); break;
            default: FormatString(buffer,*q,"RunLengthDecode"); break;
          }
          (void) WriteBlobString(image,buffer);
          (void) WriteBlobByte(image,'\n');
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
        if (LocaleCompare(image_info->magick,"PS2") == 0)
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
      (void) WriteBlobString(image,"%%PageResources: font Times-Roman\n");
    if (LocaleCompare(image_info->magick,"PS2") != 0)
      (void) WriteBlobString(image,"userdict begin\n");
    (void) WriteBlobString(image,"%%BeginData:\n");
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
    FormatString(buffer,"%lu %lu\n%u\n%d\n%d\n",image->columns,image->rows,
      (int) (image->storage_class == PseudoClass),
      (int) (image->colorspace == CMYKColorspace),
      (int) (compression == NoCompression));
    (void) WriteBlobString(image,buffer);
    number_pixels=image->columns*image->rows;
    if (compression == FaxCompression)
      {
        (void) WriteBlobByte(image,'0');
        (void) WriteBlobByte(image,'\n');
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
            register unsigned char
              *q;

            /*
              Allocate pixel array.
            */
            length=(image->colorspace == CMYKColorspace ? 4 : 3)*number_pixels;
            pixels=(unsigned char *) AcquireMemory(length);
            if (pixels == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitWarning,
                "Memory allocation failed",image);
            /*
              Dump Packbit encoded pixels.
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
                  }
                else
                  if (image->colorspace != CMYKColorspace)
                    {
                      *q++=DownScale(p->red);
                      *q++=DownScale(p->green);
                      *q++=DownScale(p->blue);
                    }
                  else
                    {
                      *q++=DownScale(p->red);
                      *q++=DownScale(p->green);
                      *q++=DownScale(p->blue);
                      *q++=DownScale(p->opacity);
                    }
                p++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(SaveImageText,y,image->rows);
            }
            if (compression == LZWCompression)
              status=LZWEncodeImage(image,length,pixels);
            else
              status=PackbitsEncodeImage(image,length,pixels);
            if (!status)
              {
                CloseBlob(image);
                return(False);
              }
            LiberateMemory((void **) &pixels);
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
                  }
                else
                  if (image->colorspace != CMYKColorspace)
                    {
                      Ascii85Encode(image,DownScale(p->red));
                      Ascii85Encode(image,DownScale(p->green));
                      Ascii85Encode(image,DownScale(p->blue));
                    }
                  else
                    {
                      Ascii85Encode(image,DownScale(p->red));
                      Ascii85Encode(image,DownScale(p->green));
                      Ascii85Encode(image,DownScale(p->blue));
                      Ascii85Encode(image,DownScale(p->opacity));
                    }
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
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              register unsigned char
                *q;

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
                {
                  *q++=indexes[x];
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    MagickMonitor(SaveImageText,y,image->rows);
              }
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
    (void) WriteBlobByte(image,'\n');
    (void) WriteBlobString(image,"%%EndData\n");
    if (LocaleCompare(image_info->magick,"PS2") != 0)
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
  if (page > 1)
    {
      FormatString(buffer,"%%%%BoundingBox: %g %g %g %g\n",floor(bounds.x1+0.5),
        floor(bounds.y1+0.5),ceil(bounds.x2-0.5),ceil(bounds.y2-0.5));
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  CloseBlob(image);
  return(True);
}
