/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             GGGG  IIIII  FFFFF                              %
%                            G        I    F                                  %
%                            G  GG    I    FFF                                %
%                            G   G    I    F                                  %
%                             GGG   IIIII  F                                  %
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
%   R e a d G I F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGIFImage reads a Compuserve Graphics image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadGIFImage routine is:
%
%      image=ReadGIFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGIFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      an error occurs.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadGIFImage(const ImageInfo *image_info)
{
#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

  char
    geometry[MaxTextExtent];

  Image
    *image;

  int
    status;

  RectangleInfo
    page_info;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  short int
    transparency_index;

  unsigned char
    background,
    c,
    flag,
    *global_colormap,
    header[MaxTextExtent],
    magick[12];

  unsigned int
    delay,
    dispose,
    global_colors,
    image_count,
    iterations;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a GIF file.
  */
  status=ReadData((char *) magick,1,6,image->file);
  if ((status == False) || ((strncmp((char *) magick,"GIF87",5) != 0) &&
      (strncmp((char *) magick,"GIF89",5) != 0)))
    ReaderExit(CorruptImageWarning,"Not a GIF image file",image);
  global_colors=0;
  global_colormap=(unsigned char *) NULL;
  page_info.width=LSBFirstReadShort(image->file);
  page_info.height=LSBFirstReadShort(image->file);
  (void) ReadData((char *) &flag,1,1,image->file);
  (void) ReadData((char *) &background,1,1,image->file);
  (void) ReadData((char *) &c,1,1,image->file);  /* reserved */
  if (BitSet(flag,0x80))
    {
      /*
        Read global colormap.
      */
      global_colors=1 << ((flag & 0x07)+1);
      global_colormap=(unsigned char *)
        AllocateMemory(3*global_colors*sizeof(unsigned char));
      if (global_colormap == (unsigned char *) NULL)
        ReaderExit(ResourceLimitWarning,"Unable to read image colormap file",
          image);
      (void) ReadData((char *) global_colormap,3,global_colors,image->file);
    }
  delay=0;
  dispose=0;
  iterations=1;
  transparency_index=(-1);
  image_count=0;
  for ( ; ; )
  {
    status=ReadData((char *) &c,1,1,image->file);
    if (status == False)
      break;
    if (c == ';')
      break;  /* terminator */
    if (c == '!')
      {
        /*
          GIF Extension block.
        */
        status=ReadData((char *) &c,1,1,image->file);
        if (status == False)
          ReaderExit(CorruptImageWarning,"Unable to read extension block",
            image);
        switch (c)
        {
          case 0xf9:
          {
            /*
              Read Graphics Control extension.
            */
            while (ReadDataBlock((char *) header,image->file) > 0);
            dispose=header[0] >> 2;
            delay=(header[2] << 8) | header[1];
            if ((header[0] & 0x01) == 1)
              transparency_index=header[3];
            break;
          }
          case 0xfe:
          {
            int
              length;

            /*
              Read Comment extension.
            */
            for ( ; ; )
            {
              length=ReadDataBlock((char *) header,image->file);
              if (length <= 0)
                break;
              if (image->comments != (char *) NULL)
                {
                  image->comments=(char *) ReallocateMemory((char *)
                    image->comments,(Extent(image->comments)+length+1)*
                    sizeof(char));
                }
              else
                {
                  image->comments=(char *)
                    AllocateMemory((length+1)*sizeof(char));
                  if (image->comments != (char *) NULL)
                    *image->comments='\0';
                }
              if (image->comments == (char *) NULL)
                ReaderExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              header[length]='\0';
              (void) strcat(image->comments,(char *) header);
            }
            break;
          }
          case 0xff:
          {
            /*
              Read Netscape Loop extension.
            */
            int
               found_netscape_loop=False;

            if (ReadDataBlock((char *) header,image->file) > 0)
             {
               found_netscape_loop=!strncmp((char *) header,"NETSCAPE2.0",11);
               if(image_info->verbose)
                 printf("Found GIF application extension block %s\n",header);
             }
            while (ReadDataBlock((char *) header,image->file) > 0)
              /* look for terminator */ ;
            if(found_netscape_loop)
              {
            iterations=(header[2] << 8) | header[1];
               if(image_info->verbose)
                 printf("with iterations= %d\n",iterations);
              }
            break;
          }
          default:
          {
            while (ReadDataBlock((char *) header,image->file) > 0);
            break;
          }
        }
      }
    if (c != ',')
      continue;
    if (image_count != 0)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image=image->next;
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
    image_count++;
    /*
      Read image attributes.
    */
    image->class=PseudoClass;
    page_info.x=LSBFirstReadShort(image->file);
    page_info.y=LSBFirstReadShort(image->file);
    image->columns=LSBFirstReadShort(image->file);
    image->rows=LSBFirstReadShort(image->file);
    (void) ReadData((char *) &flag,1,1,image->file);
    image->interlace=BitSet(flag,0x40) ? PlaneInterlace : NoInterlace;
    image->colors=!BitSet(flag,0x80) ? global_colors : 1 << ((flag & 0x07)+1);
    FormatString(geometry,"%ux%u%+d%+d",page_info.width,page_info.height,
      page_info.x,page_info.y);
    if (image_info->page == (char *) NULL)
      image->page=PostscriptGeometry(geometry);
    if (image_info->delay == (char *) NULL)
      image->delay=delay;
    if (image_info->dispose == (char *) NULL)
      image->dispose=dispose;
    if (image_info->iterations == (char *) NULL)
      image->iterations=iterations;
    delay=0;
    dispose=0;
    iterations=1;
    if (image_info->ping)
      {
        if (transparency_index >= 0)
          image->class=DirectClass;
        CloseImage(image);
        return(image);
      }
    if ((image->columns == 0) || (image->rows == 0))
      ReaderExit(CorruptImageWarning,"image size is 0",image);
    /*
      Inititialize colormap.
    */
    if (image->pixels != (RunlengthPacket *) NULL)
      FreeMemory((char *) image->pixels);
    image->pixels=(RunlengthPacket *) NULL;
    image->colormap=(ColorPacket *)
      AllocateMemory(image->colors*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (!BitSet(flag,0x80))
      {
        /*
          Use global colormap.
        */
        p=global_colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=UpScale(*p++);
          image->colormap[i].green=UpScale(*p++);
          image->colormap[i].blue=UpScale(*p++);
        }
        image->background_color=
          image->colormap[Min(background,image->colors-1)];
      }
    else
      {
        unsigned char
          *colormap;

        /*
          Read local colormap.
        */
        colormap=(unsigned char *)
          AllocateMemory(3*image->colors*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) colormap,3,image->colors,image->file);
        p=colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=UpScale(*p++);
          image->colormap[i].green=UpScale(*p++);
          image->colormap[i].blue=UpScale(*p++);
        }
        FreeMemory((char *) colormap);
      }
    /*
      Decode image.
    */
    status=GIFDecodeImage(image);
    if (transparency_index >= 0)
      {
        /*
          Create matte channel.
        */
        q=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          if (q->index != (unsigned short) transparency_index)
            q->index=Opaque;
          else
            q->index=Transparent;
          q++;
        }
        transparency_index=(-1);
        image->class=DirectClass;
        image->matte=True;
      }
    if (status == False)
      {
        MagickWarning(CorruptImageWarning,"Corrupt GIF image",image->filename);
        break;
      }
    if (image_info->subrange != 0)
      {
        if (image->scene < image_info->subimage)
          {
            Image
              subimage;

            /*
              Destroy image.
            */
            subimage=(*image);
            image->file=(FILE *) NULL;
            DestroyImage(image);
            image=AllocateImage(image_info);
            if (image == (Image *) NULL)
              return((Image *) NULL);
            image->file=subimage.file;
            image->scene=subimage.scene+1;
            image_count=0;
          }
        else
          if (image->scene >= (image_info->subimage+image_info->subrange-1))
            break;
      }
  }
  if (global_colormap != (unsigned char *) NULL)
    FreeMemory((char *) global_colormap);
  if (image->pixels == (RunlengthPacket *) NULL)
    ReaderExit(CorruptImageWarning,"Corrupt GIF image or subimage not found",
      image);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G I F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGIFImage writes an image to a file in the Compuserve Graphics
%  image format.
%
%  The format of the WriteGIFImage routine is:
%
%      status=WriteGIFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGIFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
Export unsigned int WriteGIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *next_image;

  RectangleInfo
    page_info;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    bits_per_pixel,
    c,
    *colormap,
    *global_colormap,
    *matte;

  unsigned int
    colors,
    height,
    interlace,
    scene,
    status,
    transparency,
    width;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine image bounding box.
  */
  page_info.width=image->columns;
  page_info.height=image->rows;
  page_info.x=0;
  page_info.y=0;
  next_image=image;
  for (next_image=image; next_image != (Image *) NULL; )
  {
    (void) XParseGeometry(next_image->page,&page_info.x,&page_info.y,
      &width,&height);
    if ((next_image->columns+page_info.x) > page_info.width)
      page_info.width=next_image->columns+page_info.x;
    if ((next_image->rows+page_info.y) > page_info.height)
      page_info.height=next_image->rows+page_info.y;
    next_image=next_image->next;
  }
  /*
    Allocate colormap.
  */
  matte=(unsigned char *)
    AllocateMemory(page_info.width*page_info.height*sizeof(unsigned char));
  global_colormap=(unsigned char *) AllocateMemory(768*sizeof(unsigned char));
  colormap=(unsigned char *) AllocateMemory(768*sizeof(unsigned char));
  if ((matte == (unsigned char *) NULL) ||
      (global_colormap == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < 768; i++)
    colormap[i]=0;
  /*
    Write GIF header.
  */
  if ((image->comments == (char *) NULL) && !image_info->adjoin &&
      !image->matte)
    (void) fwrite("GIF87a",1,6,image->file);
  else
    if (Latin1Compare(image_info->magick,"GIF87") == 0)
      (void) fwrite("GIF87a",1,6,image->file);
    else
      (void) fwrite("GIF89a",1,6,image->file);
  if (image_info->page != (char *) NULL)
    (void) XParseGeometry(image_info->page,&page_info.x,&page_info.y,
      &page_info.width,&page_info.height);
  LSBFirstWriteShort(page_info.width,image->file);
  LSBFirstWriteShort(page_info.height,image->file);
  /*
    Write images to file.
  */
  interlace=image_info->interlace;
  if (image_info->adjoin && (image->next != (Image *) NULL))
    interlace=NoInterlace;
  scene=0;
  do
  {
    TransformRGBImage(image,RGBColorspace);
    transparency=False;
    if (IsPseudoClass(image))
      colors=image->colors;
    else
      {
        QuantizeInfo
          quantize_info;

        if (image->matte)
          {
            /*
              Track all the transparent pixels.
            */
            if (!UncondenseImage(image))
              return(False);
            p=image->pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              matte[i]=p->index == Transparent;
              if (p->index == Transparent)
                transparency=True;
              p++;
            }
          }
        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=transparency ? 255 : 256;
        quantize_info.dither=image_info->dither;
        (void) QuantizeImage(&quantize_info,image);
        SyncImage(image);
        colors=image->colors;
        if (transparency)
          {
            /*
              Set the transparent pixel index.
            */
            if (!UncondenseImage(image))
              return(False);
            p=image->pixels;
            image->class=DirectClass;
            image->matte=True;
            for (i=0; i < (int) image->packets; i++)
            {
              if (matte[i])
                p->index=image->colors;
              p++;
            }
            colors++;
          }
      }
    for (bits_per_pixel=1; bits_per_pixel < 8; bits_per_pixel++)
      if ((1 << bits_per_pixel) >= (int) colors)
        break;
    q=colormap;
    for (i=0; i < (int) image->colors; i++)
    {
      *q++=DownScale(image->colormap[i].red);
      *q++=DownScale(image->colormap[i].green);
      *q++=DownScale(image->colormap[i].blue);
    }
    if (transparency)
      {
        *q++=DownScale(image->background_color.red);
        *q++=DownScale(image->background_color.green);
        *q++=DownScale(image->background_color.blue);
        i++;
      }
    for ( ; i < (int) (1 << bits_per_pixel); i++)
    {
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
    }
    if ((image->previous == (Image *) NULL) || !image_info->adjoin)
      {
        /*
          Write global colormap.
        */
        c=0x80;
        c|=(8-1) << 4;  /* color resolution */
        c|=(bits_per_pixel-1);   /* size of global colormap */
        (void) fputc((char) c,image->file);
        for (j=0; j < (int) (image->colors-1); j++)
          if (ColorMatch(image->background_color,image->colormap[j],0))
            break;
        (void) fputc(j,image->file);  /* background color */
        (void) fputc(0x0,image->file);  /* reserved */
        (void) fwrite(colormap,1,3*(1 << bits_per_pixel),image->file);
        for (j=0; j < 768; j++)
          global_colormap[j]=colormap[j];
      }
    if (Latin1Compare(image_info->magick,"GIF87") != 0)
      {
        /*
          Write Graphics Control extension.
        */
        (void) fputc(0x21,image->file);
        (void) fputc(0xf9,image->file);
        (void) fputc(0x04,image->file);
        c=image->dispose << 2;
        if (transparency)
          c|=0x01;
        (void) fputc(c,image->file);
        LSBFirstWriteShort(image->delay,image->file);
        (void) fputc((char) image->colors,image->file);
        (void) fputc(0x00,image->file);
        if (image->comments != (char *) NULL)
          {
            register char
              *p;

            register unsigned int
              count;

            /*
              Write Comment extension.
            */
            (void) fputc(0x21,image->file);
            (void) fputc(0xfe,image->file);
            p=image->comments;
            while (Extent(p) > 0)
            {
              count=Min(Extent(p),255);
              (void) fputc(count,image->file);
              for (i=0; i < (int) count; i++)
                (void) fputc(*p++,image->file);
            }
            (void) fputc(0x0,image->file);
          }
        if ((image->previous == (Image *) NULL) &&
            (image->next != (Image *) NULL) && (image->iterations != 1))
          {
            /*
              Write Netscape Loop extension.
            */
            (void) fputc(0x21,image->file);
            (void) fputc(0xff,image->file);
            (void) fputc(0x0b,image->file);
            (void) fwrite("NETSCAPE2.0",1,11,image->file);
            (void) fputc(0x03,image->file);
            (void) fputc(0x01,image->file);
            LSBFirstWriteShort(image->iterations,image->file);
            (void) fputc(0x00,image->file);
          }
      }
    (void) fputc(',',image->file);  /* image separator */
    /*
      Write the image header.
    */
    if (image->page != (char *) NULL)
      (void) XParseGeometry(image->page,&page_info.x,&page_info.y,
        &page_info.width,&page_info.height);
    LSBFirstWriteShort(page_info.x,image->file);
    LSBFirstWriteShort(page_info.y,image->file);
    LSBFirstWriteShort(image->columns,image->file);
    LSBFirstWriteShort(image->rows,image->file);
    c=0x00;
    if (interlace != NoInterlace)
      c|=0x40;  /* pixel data is interlaced */
    for (j=0; j < (int) (3*colors); j++)
      if (colormap[j] != global_colormap[j])
        break;
    if (j == (int) (3*colors))
      (void) fputc((char) c,image->file);
    else
      {
        c|=0x80;
        c|=(bits_per_pixel-1);   /* size of local colormap */
        (void) fputc((char) c,image->file);
        (void) fwrite(colormap,1,3*(1 << bits_per_pixel),image->file);
      }
    /*
      Write the image data.
    */
    c=Max(bits_per_pixel,2);
    (void) fputc((char) c,image->file);
    if (interlace == NoInterlace)
      status=GIFEncodeImage(image,Max(bits_per_pixel,2)+1);
    else
      {
        Image
          *interlaced_image;

        int
          pass,
          y;

        register RunlengthPacket
          *q;

        static const int
          interlace_rate[4] = { 8, 8, 4, 2 },
          interlace_start[4] = { 0, 4, 2, 1 };

        /*
          Interlace image.
        */
        if (!UncondenseImage(image))
          return(False);
        image->orphan=True;
        interlaced_image=CloneImage(image,image->columns,image->rows,False);
        image->orphan=False;
        if (interlaced_image == (Image *) NULL)
          WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
        p=image->pixels;
        q=interlaced_image->pixels;
        for (pass=0; pass < 4; pass++)
        {
          y=interlace_start[pass];
          while (y < (int) image->rows)
          {
            p=image->pixels+(y*image->columns);
            for (x=0; x < (int) image->columns; x++)
            {
              *q=(*p);
              p++;
              q++;
            }
            y+=interlace_rate[pass];
          }
        }
        interlaced_image->file=image->file;
        status=GIFEncodeImage(interlaced_image,Max(bits_per_pixel,2)+1);
        interlaced_image->file=(FILE *) NULL;
        DestroyImage(interlaced_image);
      }
    if (status == False)
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) fputc(0x0,image->file);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  (void) fputc(';',image->file); /* terminator */
  FreeMemory((char *) global_colormap);
  FreeMemory((char *) colormap);
  FreeMemory((char *) matte);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}
