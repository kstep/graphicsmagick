/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M  IIIII  FFFFF  FFFFF                           %
%                        MM MM    I    F      F                               %
%                        M M M    I    FFF    FFF                             %
%                        M   M    I    F      F                               %
%                        M   M  IIIII  F      F                               %
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
%   R e a d M I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMIFFImage reads a MIFF image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMIFFImage routine is:
%
%      image=ReadMIFFImage(filename)
%
%  A description of each parameter follows:
%
%    o image: Method ReadMIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadMIFFImage(const ImageInfo *image_info)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  register int
    c,
    i;

  register unsigned char
    *p;

  unsigned int
    length,
    packet_size,
    status;

  unsigned long
    count,
    max_packets;

  XColor
    color;

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
  image->depth=8;
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  c=fgetc(image->file);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    image->compression=NoCompression;
    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          /*
            Read comment-- any text between { }.
          */
          if (image->comments != (char *) NULL)
            {
              length=Extent(image->comments);
              p=image->comments+length;
            }
          else
            {
              length=MaxTextExtent;
              image->comments=(char *) AllocateMemory(length*sizeof(char));
              p=image->comments;
            }
          for ( ; image->comments != (char *) NULL; p++)
          {
            c=fgetc(image->file);
            if ((c == EOF) || (c == '}'))
              break;
            if ((p-image->comments+1) >= (int) length)
              {
                *p='\0';
                length<<=1;
                image->comments=(char *) ReallocateMemory((char *)
                  image->comments,length*sizeof(char));
                if (image->comments == (char *) NULL)
                  break;
                p=image->comments+Extent(image->comments);
              }
            *p=(unsigned char) c;
          }
          if (image->comments == (char *) NULL)
            ReaderExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          *p='\0';
          c=fgetc(image->file);
        }
      else
        if (isalnum(c))
          {
            /*
              Determine a keyword and its value.
            */
            p=keyword;
            do
            {
              if ((p-keyword) < (MaxTextExtent-1))
                *p++=(char) c;
              c=fgetc(image->file);
            } while (isalnum(c) || (c == '-'));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=fgetc(image->file);
            p=value;
            if (c != '"')
              while (!isspace(c) && (c != EOF))
              {
                if ((p-value) < (MaxTextExtent-1))
                  *p++=(char) c;
                c=fgetc(image->file);
              }
            else
              {
                c=fgetc(image->file);
                while ((c != '"') && (c != EOF))
                {
                  if ((p-value) < (MaxTextExtent-1))
                    *p++=(char) c;
                  c=fgetc(image->file);
                }
              }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            if (Latin1Compare(keyword,"background-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->background_color.red=XDownScale(color.red);
                image->background_color.green=XDownScale(color.green);
                image->background_color.blue=XDownScale(color.blue);
                image->background_color.index=0;
              }
            if (Latin1Compare(keyword,"blue-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.blue_primary.x,
                &image->chromaticity.blue_primary.y);
            if (Latin1Compare(keyword,"border-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->border_color.red=XDownScale(color.red);
                image->border_color.green=XDownScale(color.green);
                image->border_color.blue=XDownScale(color.blue);
                image->border_color.index=0;
              }
            if (Latin1Compare(keyword,"class") == 0)
              {
                if (Latin1Compare(value,"PseudoClass") == 0)
                  image->class=PseudoClass;
                else
                  if (Latin1Compare(value,"DirectClass") == 0)
                    image->class=DirectClass;
                  else
                    image->class=UndefinedClass;
              }
            if (Latin1Compare(keyword,"colors") == 0)
              image->colors=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"color-profile") == 0)
              image->color_profile.length=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"colorspace") == 0)
              {
                if (Latin1Compare(value,"CMYK") == 0)
                  image->colorspace=CMYKColorspace;
                else
                  if (Latin1Compare(value,"RGB") == 0)
                    image->colorspace=RGBColorspace;
              }
            if (Latin1Compare(keyword,"compression") == 0)
              {
                if (Latin1Compare(value,"Zip") == 0)
                  image->compression=ZipCompression;
                else
                  if (Latin1Compare(value,"BZip") == 0)
                    image->compression=BZipCompression;
                  else
                    if (Latin1Compare(value,"RunlengthEncoded") == 0)
                      image->compression=RunlengthEncodedCompression;
                    else
                      image->compression=UndefinedCompression;
              }
            if (Latin1Compare(keyword,"columns") == 0)
              image->columns=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"delay") == 0)
              {
                if (image_info->delay == (char *) NULL)
                  image->delay=atoi(value);
              }
            if (Latin1Compare(keyword,"depth") == 0)
              image->depth=atoi(value) <= 8 ? 8 : 16;
            if (Latin1Compare(keyword,"dispose") == 0)
              {
              if (image_info->dispose == (char *) NULL)
                image->dispose=atoi(value);
              }
            if (Latin1Compare(keyword,"gamma") == 0)
              image->gamma=atof(value);
            if (Latin1Compare(keyword,"green-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.green_primary.x,
                &image->chromaticity.green_primary.y);
            if (Latin1Compare(keyword,"id") == 0)
              {
                if (Latin1Compare(value,"ImageMagick") == 0)
                  image->id=ImageMagickId;
                else
                  image->id=UndefinedId;
              }
            if (Latin1Compare(keyword,"iterations") == 0)
              {
                if (image_info->iterations == (char *) NULL)
                  image->iterations=atoi(value);
              }
            if (Latin1Compare(keyword,"label") == 0)
              (void) CloneString(&image->label,value);
            if ((Latin1Compare(keyword,"matte") == 0) ||
                (Latin1Compare(keyword,"alpha") == 0))
              {
                if ((Latin1Compare(value,"True") == 0) ||
                    (Latin1Compare(value,"true") == 0))
                  image->matte=True;
                else
                  image->matte=False;
              }
            if (Latin1Compare(keyword,"matte-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->matte_color.red=XDownScale(color.red);
                image->matte_color.green=XDownScale(color.green);
                image->matte_color.blue=XDownScale(color.blue);
                image->matte_color.index=0;
              }
            if (Latin1Compare(keyword,"montage") == 0)
              (void) CloneString(&image->montage,value);
            if (Latin1Compare(keyword,"page") == 0)
              {
                if (image_info->page == (char *) NULL)
                  image->page=PostscriptGeometry(value);
              }
            if (Latin1Compare(keyword,"packets") == 0)
              image->packets=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"red-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.red_primary.x,
                &image->chromaticity.red_primary.y);
            if (Latin1Compare(keyword,"rendering-intent") == 0)
              {
                if (Latin1Compare(value,"saturation") == 0)
                  image->rendering_intent=SaturationIntent;
                else
                  if (Latin1Compare(value,"perceptual") == 0)
                    image->rendering_intent=PerceptualIntent;
                  else
                    if (Latin1Compare(value,"absolute") == 0)
                      image->rendering_intent=AbsoluteIntent;
                    else
                      if (Latin1Compare(value,"relative") == 0)
                        image->rendering_intent=RelativeIntent;
                      else
                        image->rendering_intent=UndefinedIntent;
              }
            if (Latin1Compare(keyword,"resolution") == 0)
              (void) sscanf(value,"%fx%f",&image->x_resolution,
                &image->y_resolution);
            if (Latin1Compare(keyword,"rows") == 0)
              image->rows=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"scene") == 0)
              image->scene=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"signature") == 0)
              (void) CloneString(&image->signature,value);
            if (Latin1Compare(keyword,"units") == 0)
              {
                if (Latin1Compare(value,"undefined") == 0)
                  image->units=UndefinedResolution;
                else
                  if (Latin1Compare(value,"pixels-per-inch") == 0)
                    image->units=PixelsPerInchResolution;
                  else
                    if (Latin1Compare(value,"pixels-per-centimeter") == 0)
                      image->units=PixelsPerCentimeterResolution;
              }
            if (Latin1Compare(keyword,"white-point") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.white_point.x,
                &image->chromaticity.white_point.y);
          }
        else
          c=fgetc(image->file);
      while (isspace(c))
        c=fgetc(image->file);
    }
    (void) fgetc(image->file);
    /*
      Verify that required image information is defined.
    */
    if ((image->id == UndefinedId) || (image->class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      ReaderExit(CorruptImageWarning,"Incorrect image header in file",image);
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        image->directory=(char *) AllocateMemory(MaxTextExtent*sizeof(char));
        if (image->directory == (char *) NULL)
          ReaderExit(CorruptImageWarning,"Unable to read image data",image);
        p=image->directory;
        do
        {
          *p='\0';
          if (((Extent(image->directory)+1) % MaxTextExtent) == 0)
            {
              /*
                Allocate more memory for the image directory.
              */
              image->directory=(char *) ReallocateMemory((char *)
                image->directory,(Extent(image->directory)+MaxTextExtent+1)*
                sizeof(char));
              if (image->directory == (char *) NULL)
                ReaderExit(CorruptImageWarning,"Unable to read image data",
                  image);
              p=image->directory+Extent(image->directory);
            }
          c=fgetc(image->file);
          *p++=(unsigned char) c;
        } while (c != '\0');
      }
    if (image->color_profile.length > 0)
      {
        /*
          Color profile.
        */
        image->color_profile.info=(unsigned char *)
          AllocateMemory(image->color_profile.length*sizeof(unsigned char));
        if (image->color_profile.info == (unsigned char *) NULL)
          ReaderExit(CorruptImageWarning,"Unable to read color profile",
            image);
        (void) fread(image->color_profile.info,1,image->color_profile.length,
          image->file);
      }
    if (image->class == PseudoClass)
      {
        unsigned int
          colors;

        unsigned short
          value;

        /*
          PseudoClass image cannot have matte data.
        */
        if (image->matte)
          ReaderExit(CorruptImageWarning,
            "Matte images must be DirectClass",image);
        /*
          Create image colormap.
        */
        colors=image->colors;
        if (colors == 0)
          colors=256;
        image->colormap=(ColorPacket *)
          AllocateMemory(colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
        if (image->colors == 0)
          for (i=0; i < (int) colors; i++)
          {
            image->colormap[i].red=(Quantum) UpScale(i);
            image->colormap[i].green=(Quantum) UpScale(i);
            image->colormap[i].blue=(Quantum) UpScale(i);
            image->colors++;
          }
        else
          {
            unsigned char
              *colormap;

            /*
              Read image colormap from file.
            */
            packet_size=3*(image->depth >> 3);
            colormap=(unsigned char *)
              AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
            if (colormap == (unsigned char *) NULL)
              ReaderExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            (void) ReadData((char *) colormap,1,packet_size*image->colors,
              image->file);
            p=colormap;
            for (i=0; i < (int) image->colors; i++)
            {
              ReadQuantum(image->colormap[i].red,p);
              ReadQuantum(image->colormap[i].green,p);
              ReadQuantum(image->colormap[i].blue,p);
            }
            FreeMemory((char *) colormap);
          }
      }
    /*
      Determine packed packet size.
    */
    if (image->class == PseudoClass)
      {
        image->packet_size=1;
        if (image->colors > 256)
          image->packet_size++;
      }
    else
      {
        image->packet_size=3*(image->depth >> 3);
        if (image->matte || (image->colorspace == CMYKColorspace))
          image->packet_size++;
      }
    if (image->compression == RunlengthEncodedCompression)
      image->packet_size++;
    packet_size=image->packet_size;
    if (image->compression == ZipCompression)
      packet_size=1;
    /*
      Allocate image pixels.
    */
    if (image->compression == NoCompression)
      image->packets=image->columns*image->rows;
    max_packets=image->packets;
    if (image->packets == 0)
      max_packets=image->columns*image->rows;
    image->packed_pixels=(unsigned char *) AllocateMemory((unsigned int)
      max_packets*packet_size*sizeof(unsigned char));
    if (image->packed_pixels == (unsigned char *) NULL)
      ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Read image pixels from file.
    */
    if ((image->compression != RunlengthEncodedCompression) ||
        (image->packets != 0))
      (void) ReadData((char *) image->packed_pixels,1,(unsigned int)
        max_packets*packet_size,image->file);
    else
      {
        /*
          Number of runlength packets is unspecified.
        */
        count=0;
        p=image->packed_pixels;
        do
        {
          (void) ReadData((char *) p,1,packet_size,image->file);
          image->packets++;
          p+=(packet_size-1);
          count+=(*p+1);
          p++;
        }
        while (count < (image->columns*image->rows));
      }
    if (image->compression == BZipCompression)
      {
        unsigned char
          *compressed_pixels;

        /*
          Uncompress image pixels with BZip decoding.
        */
        compressed_pixels=image->packed_pixels;
        max_packets=image->columns*image->rows*image->packet_size;
        image->packed_pixels=(unsigned char *)
          AllocateMemory((max_packets+8)*sizeof(unsigned char));
        if (image->packed_pixels == (unsigned char *) NULL)
          ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasBZLIB)
        {
          unsigned int
            compressed_packets;

          compressed_packets=max_packets;
          status=bzBuffToBuffDecompress((char *) image->packed_pixels,
            &compressed_packets,(char *) compressed_pixels,image->packets,
            image_info->verbose,False);
          max_packets=compressed_packets;
        }
#endif
        image->packets=(unsigned int) (max_packets/image->packet_size);
        FreeMemory((char *) compressed_pixels);
        if (status)
          ReaderExit(DelegateWarning,"Unable to uncompress image",image);
      }
    if (image->compression == ZipCompression)
      {
        unsigned char
          *compressed_pixels;

        /*
          Uncompress image pixels with Zip decoding.
        */
        compressed_pixels=image->packed_pixels;
        max_packets=image->columns*image->rows*image->packet_size;
        image->packed_pixels=(unsigned char *)
          AllocateMemory((max_packets+8)*sizeof(unsigned char));
        if (image->packed_pixels == (unsigned char *) NULL)
          ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasZLIB)
        status=uncompress(image->packed_pixels,&max_packets,compressed_pixels,
          image->packets);
#endif
        image->packets=(unsigned int) (max_packets/image->packet_size);
        FreeMemory((char *) compressed_pixels);
        if (status)
          ReaderExit(DelegateWarning,"Unable to uncompress image",image);
      }
    /*
      Unpack the packed image pixels into runlength-encoded pixel packets.
    */
    status=RunlengthDecodeImage(image);
    if (status == False)
      {
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    do
    {
      c=fgetc(image->file);
    } while (!isgraph(c) && (c != EOF));
    if (c != EOF)
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
  } while (c != EOF);
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
%   W r i t e M I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMIFFImage writes an image to a file.
%
%  The format of the WriteMIFFImage routine is:
%
%      status=WriteMIFFImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteMIFFImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
Export unsigned int WriteMIFFImage(const ImageInfo *image_info,Image *image)
{
  char
    color[MaxTextExtent];

  CompressionType
    compression;

  register int
    i;

  unsigned int
    scene;

  unsigned long
    packets;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  CondenseImage(image);
  if (image->class == DirectClass)
    {
      if (image->packets >= ((3*image->columns*image->rows) >> 2))
        image->compression=NoCompression;
    }
  else
    if (image->packets >= ((image->columns*image->rows) >> 1))
      image->compression=NoCompression;
  compression=image_info->compression;
#if defined(HasZLIB)
  if (compression == UndefinedCompression)
    compression=ZipCompression;
#endif
#if defined(HasBZLIB)
  if (compression == UndefinedCompression)
    compression=BZipCompression;
#endif
  if (compression == UndefinedCompression)
    compression=RunlengthEncodedCompression;
  (void) strcpy((char *) image_info->magick,"MIFF");
  scene=0;
  do
  {
    /*
      Pack image pixels.
    */
    if (((image_info->colorspace != UndefinedColorspace) ||
         (image->colorspace != CMYKColorspace)) &&
         (image_info->colorspace != CMYKColorspace))
      TransformRGBImage(image,RGBColorspace);
    else
      if (image->colorspace != CMYKColorspace)
        RGBTransformImage(image,CMYKColorspace);
    image->compression=compression;
    packets=RunlengthEncodeImage(image);
    if ((image->compression != NoCompression) &&
        (image->compression != RunlengthEncodedCompression))
      {
        int
          status;

        unsigned char
          *compressed_pixels;

        unsigned long
          compressed_packets;

        /*
          Compress image pixels with Zip encoding.
        */
        compressed_packets=(long unsigned int)
          (1.001*(packets*image->packet_size)+12);
        compressed_pixels=(unsigned char *)
          AllocateMemory(compressed_packets*sizeof(unsigned char));
        if (compressed_pixels == (unsigned char *) NULL)
          WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasBZLIB)
        if (compression == BZipCompression)
          {
            unsigned int
              max_packets;

            /*
              BZip compress the image pixels.
            */
            max_packets=compressed_packets;
            status=bzBuffToBuffCompress((char *) compressed_pixels,&max_packets,
              (char *) image->packed_pixels,packets*image->packet_size,
              Min(image_info->quality/10,9),image_info->verbose,
              (image_info->quality % 10)*(image_info->quality % 10)+5);
            compressed_packets=max_packets;
          }
#endif
#if defined(HasZLIB)
        if (status)
          {
            z_stream
              stream;

            /*
              BZlib compress the image pixels.
            */
            stream.next_in=image->packed_pixels;
            stream.avail_in=packets*image->packet_size;
            stream.next_out=compressed_pixels;
            stream.avail_out=compressed_packets;
            stream.zalloc=(alloc_func) NULL;
            stream.zfree=(free_func) NULL;
            stream.opaque=(voidpf) NULL;
            status=deflateInit(&stream,Min(image_info->quality/10,9));
            if (status == Z_OK)
              {
                status=deflate(&stream,Z_FINISH);
                if (status == Z_STREAM_END)
                  status=deflateEnd(&stream);
                else
                  (void) deflateEnd(&stream);
                compressed_packets=stream.total_out;
              }
            if (status == Z_OK)
              compression=ZipCompression;
          }
#endif
        if (status)
          {
            FreeMemory((char *) compressed_pixels);
            WriterExit(DelegateWarning,"Unable to compress image",image);
          }
        else
          {
            FreeMemory((char *) image->packed_pixels);
            image->packed_pixels=compressed_pixels;
            image->packet_size=1;
            packets=compressed_packets;
          }
      }
    /*
      Write header to file.
    */
    (void) fprintf(image->file,"id=ImageMagick\n");
    if (image->class == PseudoClass)
      (void) fprintf(image->file,"class=PseudoClass  colors=%u\n",
        image->colors);
    else
      if (image->matte)
        (void) fprintf(image->file,"class=DirectClass  matte=True\n");
      else
        if (image->colorspace == CMYKColorspace)
          (void) fprintf(image->file,"class=DirectClass  colorspace=CMYK\n");
        else
          (void) fprintf(image->file,"class=DirectClass\n");
    if (image->compression == RunlengthEncodedCompression)
      (void) fprintf(image->file,"compression=RunlengthEncoded  packets=%lu\n",
        packets);
    else
      if (image->compression == BZipCompression)
        (void) fprintf(image->file,"compression=BZip  packets=%lu\n",packets);
      else
        if (image->compression != NoCompression)
          (void) fprintf(image->file,"compression=Zip  packets=%lu\n",packets);
    (void) fprintf(image->file,"columns=%u  rows=%u  depth=%u\n",image->columns,
      image->rows,image->depth);
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        char
          units[MaxTextExtent];

        /*
          Set image resolution.
        */
        (void) strcpy(units,"undefined");
        if (image->units == PixelsPerInchResolution)
          (void) strcpy(units,"pixels-per-inch");
        if (image->units == PixelsPerCentimeterResolution)
          (void) strcpy(units,"pixels-per-centimeter");
        (void) fprintf(image->file,"resolution=%gx%g  units=%.1024s\n",
          image->x_resolution,image->y_resolution,units);
      }
    SignatureImage(image);
    if (image->signature != (char *) NULL)
      (void) fprintf(image->file,"signature=%.1024s\n",image->signature);
    if (image->page != (char *) NULL)
      (void) fprintf(image->file,"page=%.1024s\n",image->page);
    (void) QueryColorName(&image->background_color,color);
    (void) fprintf(image->file,"background-color=%.1024s  ",color);
    (void) QueryColorName(&image->border_color,color);
    (void) fprintf(image->file,"border-color=%.1024s  ",color);
    (void) QueryColorName(&image->matte_color,color);
    (void) fprintf(image->file,"matte-color=%.1024s\n",color);
    if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
      (void) fprintf(image->file,
        "scene=%u  iterations=%u  delay=%u  dispose=%u\n",
        image->scene,image->iterations,image->delay,image->dispose);
    else
      {
        if (image->scene != 0)
          (void) fprintf(image->file,"scene=%u\n",image->scene);
        if (image->iterations != 1)
          (void) fprintf(image->file,"iterations=%u\n",image->iterations);
        if (image->delay != 0)
          (void) fprintf(image->file,"delay=%u\n",image->delay);
        if (image->dispose != 0)
          (void) fprintf(image->file,"dispose=%u\n",image->dispose);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        if (image->rendering_intent == SaturationIntent)
          (void) fprintf(image->file,"rendering-intent=saturation\n");
        else
          if (image->rendering_intent == PerceptualIntent)
            (void) fprintf(image->file,"rendering-intent=perceptual\n");
          else
            if (image->rendering_intent == AbsoluteIntent)
              (void) fprintf(image->file,"rendering-intent=absolute\n");
            else
              (void) fprintf(image->file,"rendering-intent=relative\n");
      }
    if (image->gamma != 0.0)
      (void) fprintf(image->file,"gamma=%g\n",image->gamma);
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        (void) fprintf(image->file,
          "red-primary=%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) fprintf(image->file,"white-point=%g,%g\n",
          image->chromaticity.white_point.x,image->chromaticity.white_point.y);
      }
    if (image->color_profile.length > 0)
      (void) fprintf(image->file,"color-profile=%u\n",
        image->color_profile.length);
    if (image->montage != (char *) NULL)
      (void) fprintf(image->file,"montage=%.1024s\n",image->montage);
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"label=\"%.1024s\"\n",image->label);
    if (image->comments != (char *) NULL)
      (void) fprintf(image->file,"{%s}\n",image->comments);
    (void) fprintf(image->file,"\f\n:\032");
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) fprintf(image->file,"%s",image->directory);
        (void) fputc('\0',image->file);
      }
    if (image->color_profile.length > 0)
      (void) fwrite((char *) image->color_profile.info,1,(int)
        image->color_profile.length,image->file);
    if (image->class == PseudoClass)
      {
        register unsigned char
          *q;

        unsigned char
          *colormap;

        unsigned int
          packet_size;

        unsigned short
          value;

        /*
          Allocate colormap.
        */
        packet_size=3*(image->depth >> 3);
        colormap=(unsigned char *)
          AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
        q=colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          WriteQuantum(image->colormap[i].red,q);
          WriteQuantum(image->colormap[i].green,q);
          WriteQuantum(image->colormap[i].blue,q);
        }
        /*
          Write colormap to file.
        */
        (void) fwrite((char *) colormap,1,(int) image->colors*packet_size,
          image->file);
        FreeMemory((char *) colormap);
      }
    /*
      Write image pixels to file.
    */
    (void) fwrite((char *) image->packed_pixels,(int) image->packet_size,
      (int) packets,image->file);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}
