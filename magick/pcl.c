/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP    CCCC  L                                  %
%                            P   P  C      L                                  %
%                            PPPP   C      L                                  %
%                            P      C      L                                  %
%                            P       CCCC  LLLLL                              %
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
%   R e a d P C L I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCLImage reads a Page Control Language image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPCLImage routine is:
%
%      image=ReadPCLImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCLImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadPCLImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"Cannot read PCL images",
    image_info->filename);
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             % %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCLImage writes an image in the Page Control Language encoded
%  image format.
%
%  The format of the WritePCLImage routine is:
%
%      status=WritePCLImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
%
*/
Export unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
{
  char
    geometry[MaxTextExtent];

  int
    sans_offset,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  RectangleInfo
    media_info;

  unsigned int
    density,
    height,
    page_size,
    text_size,
    width;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  /*
    Initialize the printer.
  */
  (void) fprintf(image->file,"\033E");  /* portrait orientation */
  (void) fprintf(image->file,"\033&l0O");  /* portrait orientation */
  (void) fprintf(image->file,"\033&l0E");  /* top margin 0 */
  /*
    Center image on PCL page.
  */
  text_size=0;
  if (image->label != (char *) NULL)
    text_size=MultilineCensus(image->label)*image_info->pointsize+12;
  width=image->columns;
  height=image->rows;
  x=0;
  y=text_size;
  if (image_info->page != (char *) NULL)
    (void) strcpy(geometry,image_info->page);
  else
    if (image->page != (char *) NULL)
      (void) strcpy(geometry,image->page);
    else
      (void) strcpy(geometry,PSPageGeometry);
  (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,
    &media_info.width,&media_info.height);
  page_size=2;
  if ((media_info.width == 540) && (media_info.height == 720))
    page_size=1;  /* executive */
  if ((media_info.width == 612) && (media_info.height == 792))
    page_size=2;  /* letter */
  if ((media_info.width == 612) && (media_info.height == 1008))
    page_size=3;  /* legal */
  if ((media_info.width == 1224) && (media_info.height == 792))
    page_size=6;  /* ledger */
  if ((media_info.width == 595) && (media_info.height == 842))
    page_size=26;  /* A4 */
  if ((media_info.width == 842) && (media_info.height == 1191))
    page_size=27;  /* A3 */
  if ((media_info.width == 729) && (media_info.height == 1032))
    page_size=45;  /* B5 */
  if ((media_info.width == 516) && (media_info.height == 729))
    page_size=46;  /* B4 */
  (void) fprintf(image->file,"\033&l%uA",page_size);  /* papersize */
  density=72;
  if (image_info->density != (char *) NULL)
    (void) XParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &density,&density);
  else
    (void) XParseGeometry("75x75",&sans_offset,&sans_offset,
      &density,&density);
  (void) fprintf(image->file,"\033*p%dx%dY",x,y);
  if (image->label != (char *) NULL)
    {
      /*
        Print label.
      */
      (void) fprintf(image->file,"\033&k2G");
      (void) fprintf(image->file,"\033(s1p%uv5t3b",image_info->pointsize);
      (void) fprintf(image->file,"\n%.1024s\n",image->label);
      (void) fprintf(image->file,"\033(s0B");
    }
  (void) fprintf(image->file,"\033*t%uR",density);  /* graphic resolution */
  width=(density*width)/75;
  height=(density*height)/75;
  if (!IsGrayImage(image))
    {
      /*
        Write PCL color image.
      */
      (void) fprintf(image->file,"\033*r%us%uT",image->columns,image->rows);
      (void) fprintf(image->file,"\033*t%uh%uV",width,height);
      (void) fprintf(image->file,"\033*v6W");
      (void) fputc('\000',image->file);  /* color model */
      (void) fputc('\003',image->file);  /* direct pixel encoding */
      (void) fputc('\000',image->file);  /* bits per index */
      (void) fputc('\010',image->file);  /* bits red*/
      (void) fputc('\010',image->file);  /* bits green*/
      (void) fputc('\010',image->file);  /* bits blue */
      (void) fprintf(image->file,"\033*r2A");  /* start graphics */
      (void) fprintf(image->file,"\033*b0M");  /* no compression */
      (void) fprintf(image->file,"\033*b%uW",3*image->columns);
      x=0;
      y=0;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          (void) fprintf(image->file,"%c%c%c",(int) DownScale(p->red),
            (int) DownScale(p->green),(int) DownScale(p->blue));
        x++;
        if (x == (int) image->columns)
          {
            (void) fprintf(image->file,"\033*b%uW",3*image->columns);
            if (QuantumTick(y,image->rows))
              ProgressMonitor(SaveImageText,y,image->rows);
            x=0;
            y++;
          }
        p++;
      }
      (void) fprintf(image->file,"\033*rC");  /* end graphics */
    }
  else
    {
      Image
        *monochrome_image;

      register unsigned char
        bit,
        byte,
        polarity;

      /*
        Write PCL monochrome image.
      */
      monochrome_image=image;
      if ((width != image->columns) || (height != image->rows))
        {
          /*
            Scale image.
          */
          image->orphan=True;
          monochrome_image=ZoomImage(image,width,height);
          image->orphan=False;
          if (monochrome_image == (Image *) NULL)
            WriterExit(ResourceLimitWarning,"Unable to scale image",image);
        }
      if (!IsMonochromeImage(monochrome_image))
        {
          QuantizeInfo
            quantize_info;

          GetQuantizeInfo(&quantize_info);
          quantize_info.number_colors=2;
          quantize_info.dither=image_info->dither;
          quantize_info.colorspace=GRAYColorspace;
          (void) QuantizeImage(&quantize_info,monochrome_image);
          SyncImage(monochrome_image);
        }
      p=monochrome_image->pixels;
      polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
      if (monochrome_image->colors == 2)
        polarity=Intensity(monochrome_image->colormap[0]) >
          Intensity(monochrome_image->colormap[1]);
      bit=0;
      byte=0;
      x=0;
      y=0;
      (void) fprintf(image->file,"\033*r%us%uT",monochrome_image->columns,
        monochrome_image->rows);
      (void) fprintf(image->file,"\033*r1A");  /* start graphics */
      (void) fprintf(image->file,"\033*b0M");  /* no compression */
      (void) fprintf(image->file,"\033*b%uW",(image->columns+7)/8);
      for (i=0; i < (int) monochrome_image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte<<=1;
          if (p->index == polarity)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              (void) fputc(byte,image->file);
              bit=0;
              byte=0;
            }
          x++;
          if (x == (int) monochrome_image->columns)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                (void) fputc(byte  << (8-bit),image->file);
              if (QuantumTick(y,monochrome_image->rows))
                ProgressMonitor(SaveImageText,y,monochrome_image->rows);
              bit=0;
              byte=0;
              x=0;
              y++;
              if (y < (int) monochrome_image->rows)
                (void) fprintf(image->file,"\033*b%uW",
                  (monochrome_image->columns+7)/8);
           }
        }
        p++;
      }
      (void) fprintf(image->file,"\033*rB");  /* end graphics */
      if (image != monochrome_image)
        DestroyImage(monochrome_image);
    }
  (void) fprintf(image->file,"\033&l0H");
  (void) fprintf(image->file,"\033E");  /* portrait orientation */
  CloseImage(image);
  return(True);
}
