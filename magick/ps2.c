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
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
^L
/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include "proxy.h"

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
%  The format of the WritePS2Image routine is:
%
%      status=WritePS2Image(image_info,image)
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
Export unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
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
      "/buffer 512 string def",
      "/pixel_stream currentfile /ASCII85Decode filter def",
      "",
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
      "      { /DataSource pixel_stream }",
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
      "      { /DataSource pixel_stream }",
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
      "      { /DataSource pixel_stream }",
      "      {",
      "        /DataSource pixel_stream",
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
      "      { /DataSource pixel_stream }",
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
    date[MaxTextExtent],
    density[MaxTextExtent],
    **labels;

  const char
    **q;

  CompressionType
    compression;

  double
    x_scale,
    y_scale;

  float
    dx_resolution,
    dy_resolution,
    x_resolution,
    y_resolution;

  int
    count,
    status,
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  SegmentInfo
    bounding_box;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    page,
    scene,
    text_size,
    width;

  unsigned long
    number_packets;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  compression=image_info->compression;
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
  page=1;
  scene=0;
  do
  {
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    if (image_info->page != (char *) NULL)
      (void) ParseImageGeometry(image_info->page,&x,&y,&width,&height);
    else
      if (image->page != (char *) NULL)
        (void) ParseImageGeometry(image->page,&x,&y,&width,&height);
      else
        if (Latin1Compare(image_info->magick,"PS2") == 0)
          (void) ParseImageGeometry(PSPageGeometry,&x,&y,&width,&height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%fx%f",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%fx%f",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (Latin1Compare(image_info->magick,"PS2") == 0)
          (void) fprintf(image->file,"%%!PS-Adobe-3.0\n");
        else
          (void) fprintf(image->file,"%%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) fprintf(image->file,"%%%%Creator: (ImageMagick)\n");
        (void) fprintf(image->file,"%%%%Title: (%.1024s)\n",image->filename);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strcpy(date,ctime(&timer));
        date[Extent(date)-1]='\0';
        (void) fprintf(image->file,"%%%%CreationDate: (%.1024s)\n",date);
        bounding_box.x1=x;
        bounding_box.y1=y;
        bounding_box.x2=x+width-1;
        bounding_box.y2=y+(height+text_size)-1;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) fprintf(image->file,"%%%%BoundingBox: (atend)\n");
        else
          (void) fprintf(image->file,"%%%%BoundingBox: %g %g %g %g\n",
            bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
        if (image->label != (char *) NULL)
          (void) fprintf(image->file,
            "%%%%DocumentNeededResources: font Helvetica\n");
        (void) fprintf(image->file,"%%%%LanguageLevel: 2\n");
        if (Latin1Compare(image_info->magick,"PS2") != 0)
          (void) fprintf(image->file,"%%%%Pages: 0\n");
        else
          {
            Image
             *next_image;

            unsigned int
              pages;

            /*
              Compute the number of pages.
            */
            (void) fprintf(image->file,"%%%%Orientation: Portrait\n");
            (void) fprintf(image->file,"%%%%PageOrder: Ascend\n");
            pages=1;
            if (image_info->adjoin)
              for (next_image=image->next; next_image != (Image *) NULL; )
              {
                next_image=next_image->next;
                pages++;
              }
            (void) fprintf(image->file,"%%%%Pages: %u\n",pages);
          }
        (void) fprintf(image->file,"%%%%EndComments\n");
        (void) fprintf(image->file,"\n%%%%BeginDefaults\n");
        (void) fprintf(image->file,"%%%%PageOrientation: Portrait\n");
        (void) fprintf(image->file,"%%%%EndDefaults\n\n");
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
        {
          (void) fprintf(image->file,*q,
            compression == ZipCompression ? "FlateDecode" :
            compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
          (void) fprintf(image->file,"\n");
        }
        for (i=MultilineCensus(image->label)-1; i >= 0; i--)
        {
          (void) fprintf(image->file,"  /label 512 string def\n");
          (void) fprintf(image->file,"  currentfile label readline pop\n");
          (void) fprintf(image->file,"  0 y %d add moveto label show pop\n",
            i*image_info->pointsize+12);
        }
        for (q=PostscriptEpilog; *q; q++)
          (void) fprintf(image->file,"%.255s\n",*q);
        if (Latin1Compare(image_info->magick,"PS2") == 0)
          (void) fprintf(image->file,"  showpage\n");
        (void) fprintf(image->file,"} bind def\n");
        (void) fprintf(image->file,"%%%%EndProlog\n");
      }
    (void) fprintf(image->file,"%%%%Page:  1 %u\n",page++);
    (void) fprintf(image->file,"%%%%PageBoundingBox: %d %d %d %d\n",x,y,
      x+(int) width-1,y+(int) (height+text_size)-1);
    if (x < bounding_box.x1)
      bounding_box.x1=x;
    if (y < bounding_box.y1)
      bounding_box.y1=y;
    if ((x+(int) width-1) > bounding_box.x2)
      bounding_box.x2=x+width-1;
    if ((y+(int) (height+text_size)-1) > bounding_box.y2)
      bounding_box.y2=y+(height+text_size)-1;
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"%%%%PageResources: font Helvetica\n");
    if (Latin1Compare(image_info->magick,"PS2") != 0)
      (void) fprintf(image->file,"userdict begin\n");
    (void) fprintf(image->file,"%%%%BeginData:\n");
    (void) fprintf(image->file,"DisplayImage\n");
    /*
      Output image data.
    */
    labels=StringToList(image->label);
    (void) fprintf(image->file,"%d %d\n%f %f\n%u\n",x,y,x_scale,y_scale,
      image_info->pointsize);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) fprintf(image->file,"%.1024s \n",labels[i]);
          FreeMemory(labels[i]);
        }
        FreeMemory(labels);
      }
    (void) fprintf(image->file,"%u %u\n%u\n%d\n%d\n",image->columns,image->rows,
      IsPseudoClass(image),(int) (image->colorspace == CMYKColorspace),
      (int) (compression == NoCompression));
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
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
          number_packets=(image->colorspace == CMYKColorspace ? 4 : 3)*
            image->columns*image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            WriterExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          /*
            Dump Packbit encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
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
                    *q++=DownScale(p->index);
                  }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          if (compression == ZipCompression)
            status=ZLIBEncodeImage(image->file,number_packets,
              image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image->file,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image->file,number_packets,pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
          FreeMemory((char *) pixels);
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                if (image->colorspace != CMYKColorspace)
                  {
                    Ascii85Encode(DownScale(p->red),image->file);
                    Ascii85Encode(DownScale(p->green),image->file);
                    Ascii85Encode(DownScale(p->blue),image->file);
                  }
                else
                  {
                    Ascii85Encode(DownScale(p->red),image->file);
                    Ascii85Encode(DownScale(p->green),image->file);
                    Ascii85Encode(DownScale(p->blue),image->file);
                    Ascii85Encode(DownScale(p->index),image->file);
                  }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          Ascii85Flush(image->file);
          break;
        }
      }
    else
      if (IsFaxImage(image) && (compression != LZWCompression))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          (void) fprintf(image->file,"0\n");
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              if (Latin1Compare(CCITTParam,"0") == 0)
                (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
              else
                (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          ProgressMonitor(SaveImageText,y,image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          (void) fprintf(image->file,"%u\n",image->colors);
          for (i=0; i < (int) image->colors; i++)
            (void) fprintf(image->file,"%02lx%02lx%02lx\n",
              DownScale(image->colormap[i].red),
              DownScale(image->colormap[i].green),
              DownScale(image->colormap[i].blue));
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
              number_packets=image->columns*image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                WriterExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"%%%%EndData\n");
    if (Latin1Compare(image_info->magick,"PS2") != 0)
      (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"%%%%PageTrailer\n");
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) fprintf(image->file,"%%%%Trailer\n");
  if (page > 1)
    (void) fprintf(image->file,"%%%%BoundingBox: %g %g %g %g\n",
      bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
  (void) fprintf(image->file,"%%%%EOF\n");
  CloseImage(image);
  return(True);
}
