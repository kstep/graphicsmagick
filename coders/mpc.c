/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            M   M  PPPP    CCCC                              %
%                            MM MM  P   P  C                                  %
%                            M M M  PPPP   C                                  %
%                            M   M  P      C                                  %
%                            M   M  P       CCCC                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
  WriteMPCImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M P C                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMPC returns True if the image format type, identified by the
%  magick string, is an Magick Persistent Cache image.
%
%  The format of the IsMPC method is:
%
%      unsigned int IsMPC(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMPC returns True if the image format type is MPC.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMPC(const unsigned char *magick,const size_t length)
{
  if (length < 14)
    return(False);
  if (LocaleNCompare((char *) magick,"id=MagickCache",14) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C A C H E I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMPCImage reads an Magick Persistent Cache image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadMPCImage method is:
%
%      Image *ReadMPCImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  Decompression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o image: Method ReadMPCImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMPCImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    id[MaxTextExtent],
    keyword[MaxTextExtent],
    *values;

  Image
    *image;

  int
    c;

  off_t
    length,
    offset;

  register long
    i;

  register unsigned char
    *p;

  unsigned int
    quantum_depth,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  c=ReadBlobByte(image);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  *id='\0';
  offset=0;
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    length=MaxTextExtent;
    values=GetString((char *) NULL);
    quantum_depth=QuantumDepth;
    image->depth=8;
    image->compression=NoCompression;
    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          char
            *comment;

          /*
            Read comment-- any text between { }.
          */
          length=MaxTextExtent;
          comment=GetString((char *) NULL);
          p=comment;
          for ( ; comment != (char *) NULL; p++)
          {
            c=ReadBlobByte(image);
            if ((c == EOF) || (c == '}'))
              break;
            if ((p-comment+1) >= (int) length)
              {
                *p='\0';
                length<<=1;
                ReacquireMemory((void **) &comment,length);
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            *p=c;
          }
          if (comment == (char *) NULL)
            ThrowReaderException(ResourceLimitWarning,
              "Memory allocation failed",image);
          *p='\0';
          (void) SetImageAttribute(image,"comment",comment);
          LiberateMemory((void **) &comment);
          c=ReadBlobByte(image);
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
                *p++=c;
              c=ReadBlobByte(image);
            } while (isalnum(c) || (c == '-'));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=ReadBlobByte(image);
            p=values;
            while ((c != '}') && (c != EOF))
            {
              if ((p-values+1) >= (int) length)
                {
                  *p='\0';
                  length<<=1;
                  ReacquireMemory((void **) &values,length);
                  if (values == (char *) NULL)
                    break;
                  p=values+strlen(values);
                }
              if (values == (char *) NULL)
                ThrowReaderException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              *p++=c;
              c=ReadBlobByte(image);
              if (*values != '{')
                if (isspace(c))
                  break;
            }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            switch (*keyword)
            {
              case 'b':
              case 'B':
              {
                if (LocaleCompare(keyword,"background-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->background_color);
                    break;
                  }
                if (LocaleCompare(keyword,"blue-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.blue_primary.x,
                      &image->chromaticity.blue_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"border-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->border_color);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'c':
              case 'C':
              {
                if (LocaleCompare(keyword,"class") == 0)
                  {
                    if (LocaleCompare(values,"PseudoClass") == 0)
                      image->storage_class=PseudoClass;
                    else
                      if (LocaleCompare(values,"DirectClass") == 0)
                        image->storage_class=DirectClass;
                      else
                        image->storage_class=UndefinedClass;
                    break;
                  }
                if (LocaleCompare(keyword,"colors") == 0)
                  {
                    image->colors=atol(values);
                    break;
                  }
                if (LocaleCompare(keyword,"colorspace") == 0)
                  {
                    if (LocaleCompare(values,"CMYK") == 0)
                      image->colorspace=CMYKColorspace;
                    else
                      if (LocaleCompare(values,"RGB") == 0)
                        image->colorspace=RGBColorspace;
                    break;
                  }
                if (LocaleCompare(keyword,"compression") == 0)
                  {
                    CompressionType
                      compression;

                    compression=UndefinedCompression;
                    if (LocaleCompare("None",values) == 0)
                      compression=NoCompression;
                    if (LocaleCompare("BZip",values) == 0)
                      compression=BZipCompression;
                    if (LocaleCompare("Fax",values) == 0)
                      compression=FaxCompression;
                    if (LocaleCompare("Group4",values) == 0)
                      compression=Group4Compression;
                    if (LocaleCompare("JPEG",values) == 0)
                      compression=JPEGCompression;
                    if (LocaleCompare("Lossless",values) == 0)
                      compression=LosslessJPEGCompression;
                    if (LocaleCompare("LZW",values) == 0)
                      compression=LZWCompression;
                    if (LocaleCompare("RLE",values) == 0)
                      compression=RunlengthEncodedCompression;
                    if (LocaleCompare("Zip",values) == 0)
                      compression=ZipCompression;
                    image->compression=compression;
                    break;
                  }
                if (LocaleCompare(keyword,"columns") == 0)
                  {
                    image->columns= atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'd':
              case 'D':
              {
                if (LocaleCompare(keyword,"delay") == 0)
                  {
                    image->delay=atol(values);
                    break;
                  }
                if (LocaleCompare(keyword,"depth") == 0)
                  {
                    image->depth=atol(values) <= 8 ? 8 : 16;
                    break;
                  }
                if (LocaleCompare(keyword,"dispose") == 0)
                  {
                    image->dispose=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'e':
              case 'E':
              {
                if (LocaleCompare(keyword,"error") == 0)
                  {
                    image->mean_error_per_pixel=atof(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'g':
              case 'G':
              {
                if (LocaleCompare(keyword,"gamma") == 0)
                  {
                    image->gamma=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"green-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.green_primary.x,
                      &image->chromaticity.green_primary.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'i':
              case 'I':
              {
                if (LocaleCompare(keyword,"id") == 0)
                  {
                    (void) strncpy(id,values,MaxTextExtent-1);
                    break;
                  }
                if (LocaleCompare(keyword,"iterations") == 0)
                  {
                    image->iterations=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'm':
              case 'M':
              {
                if (LocaleCompare(keyword,"matte") == 0)
                  {
                    image->matte=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                  }
                if (LocaleCompare(keyword,"matte-color") == 0)
                  {
                    (void) QueryColorDatabase(values,&image->matte_color);
                    break;
                  }
                if (LocaleCompare(keyword,"maximum-error") == 0)
                  {
                    image->normalized_maximum_error=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"mean-error") == 0)
                  {
                    image->normalized_mean_error=atof(values);
                    break;
                  }
                if (LocaleCompare(keyword,"montage") == 0)
                  {
                    (void) CloneString(&image->montage,values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'p':
              case 'P':
              {
                if (LocaleCompare(keyword,"page") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(values);
                    (void) GetGeometry(geometry,&image->page.x,&image->page.y,
                      &image->page.width,&image->page.height);
                    LiberateMemory((void **) &geometry);
                    break;
                  }
                if (LocaleNCompare(keyword,"profile-",8) == 0)
                  {
                    if (LocaleCompare(keyword,"profile-icc") == 0)
                      {
                        image->color_profile.length=atol(values);
                        break;
                      }
                    if (LocaleCompare(keyword,"profile-iptc") == 0)
                      {
                        image->iptc_profile.length=atol(values);
                        break;
                      }
                    i=(long) image->generic_profiles;
                    if (image->generic_profile == (ProfileInfo *) NULL)
                      image->generic_profile=(ProfileInfo *)
                        AcquireMemory(sizeof(ProfileInfo));
                    else
                      ReacquireMemory((void **) &image->generic_profile,
                        (i+1)*sizeof(ProfileInfo));
                    if (image->generic_profile == (ProfileInfo *) NULL)
                      ThrowReaderException(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    image->generic_profile[i].name=GetString(keyword+8);
                    image->generic_profile[i].length=atol(values);
                    image->generic_profile[i].info=(unsigned char *) NULL;
                    image->generic_profiles++;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'o':
              case 'O':
              {
                if (LocaleCompare(keyword,"opaque") == 0)
                  {
                    image->matte=(LocaleCompare(values,"True") == 0) ||
                      (LocaleCompare(values,"true") == 0);
                    break;
                   }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'q':
              case 'Q':
              {
                if (LocaleCompare(keyword,"quantum-depth") == 0)
                  {
                    quantum_depth=atol(values) <= 8 ? 8 : 16;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'r':
              case 'R':
              {
                if (LocaleCompare(keyword,"red-primary") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.red_primary.x,
                      &image->chromaticity.red_primary.y);
                    break;
                  }
                if (LocaleCompare(keyword,"rendering-intent") == 0)
                  {
                    if (LocaleCompare(values,"Saturation") == 0)
                      image->rendering_intent=SaturationIntent;
                    else
                      if (LocaleCompare(values,"perceptual") == 0)
                        image->rendering_intent=PerceptualIntent;
                      else
                        if (LocaleCompare(values,"absolute") == 0)
                          image->rendering_intent=AbsoluteIntent;
                        else
                          if (LocaleCompare(values,"relative") == 0)
                            image->rendering_intent=RelativeIntent;
                          else
                            image->rendering_intent=UndefinedIntent;
                    break;
                  }
                if (LocaleCompare(keyword,"resolution") == 0)
                  {
                    (void) sscanf(values,"%lfx%lf",&image->x_resolution,
                      &image->y_resolution);
                    break;
                  }
                if (LocaleCompare(keyword,"rows") == 0)
                  {
                    image->rows=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 's':
              case 'S':
              {
                if (LocaleCompare(keyword,"scene") == 0)
                  {
                    image->scene=atol(values);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'u':
              case 'U':
              {
                if (LocaleCompare(keyword,"units") == 0)
                  {
                    if (LocaleCompare(values,"undefined") == 0)
                      image->units=UndefinedResolution;
                    else
                      if (LocaleCompare(values,"pixels-per-inch") == 0)
                        image->units=PixelsPerInchResolution;
                      else
                        if (LocaleCompare(values,"pixels-per-centimeter") == 0)
                          image->units=PixelsPerCentimeterResolution;
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              case 'w':
              case 'W':
              {
                if (LocaleCompare(keyword,"white-point") == 0)
                  {
                    (void) sscanf(values,"%lf,%lf",
                      &image->chromaticity.white_point.x,
                      &image->chromaticity.white_point.y);
                    break;
                  }
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
              default:
              {
                (void) SetImageAttribute(image,keyword,
                  *values == '{' ? values+1 : values);
                break;
              }
            }
          }
        else
          c=ReadBlobByte(image);
      while (isspace(c))
        c=ReadBlobByte(image);
    }
    LiberateMemory((void **) &values);
    (void) ReadBlobByte(image);
    /*
      Verify that required image information is defined.
    */
    if ((LocaleCompare(id,"MagickCache") != 0) ||
        (image->storage_class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      ThrowReaderException(CorruptImageWarning,"Incorrect image header in file",
        image);
    if (quantum_depth != QuantumDepth)
      ThrowReaderException(CacheWarning,"Inconsistent peristent cache depth",
        image);
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        image->directory=GetString((char *) NULL);
        if (image->directory == (char *) NULL)
          ThrowReaderException(CorruptImageWarning,"Unable to read image data",
            image);
        p=image->directory;
        do
        {
          *p='\0';
          if (((strlen(image->directory)+1) % MaxTextExtent) == 0)
            {
              /*
                Allocate more memory for the image directory.
              */
              ReacquireMemory((void **) &image->directory,
                (strlen(image->directory)+MaxTextExtent+1));
              if (image->directory == (char *) NULL)
                ThrowReaderException(CorruptImageWarning,
                  "Unable to read image data",image);
              p=image->directory+strlen(image->directory);
            }
          c=ReadBlobByte(image);
          *p++=c;
        } while (c != '\0');
      }
    if (image->color_profile.length != 0)
      {
        /*
          Color profile.
        */
        image->color_profile.info=(unsigned char *)
          AcquireMemory(image->color_profile.length);
        if (image->color_profile.info == (unsigned char *) NULL)
          ThrowReaderException(CorruptImageWarning,
            "Unable to read color profile",image);
        (void) ReadBlob(image,image->color_profile.length,
          image->color_profile.info);
      }
    if (image->iptc_profile.length != 0)
      {
        /*
          IPTC profile.
        */
        image->iptc_profile.info=(unsigned char *)
          AcquireMemory(image->iptc_profile.length);
        if (image->iptc_profile.info == (unsigned char *) NULL)
          ThrowReaderException(CorruptImageWarning,
            "Unable to read IPTC profile",image);
        (void) ReadBlob(image,image->iptc_profile.length,
          image->iptc_profile.info);
      }
    if (image->generic_profiles != 0)
      {
        /*
          Generic profile.
        */
        for (i=0; i < (long) image->generic_profiles; i++)
        {
          if (image->generic_profile[i].length == 0)
            continue;
          image->generic_profile[i].info=(unsigned char *)
            AcquireMemory(image->generic_profile[i].length);
          if (image->generic_profile[i].info == (unsigned char *) NULL)
            ThrowReaderException(CorruptImageWarning,
              "Unable to read generic profile",image);
          (void) ReadBlob(image,image->generic_profile[i].length,
            image->generic_profile[i].info);
        }
      }
    if (image->storage_class == PseudoClass)
      {
        /*
          Create image colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        if (image->colors == 0)
          for (i=0; i < 256; i++)
          {
            image->colormap[i].red=Upscale(i);
            image->colormap[i].green=Upscale(i);
            image->colormap[i].blue=Upscale(i);
            image->colors++;
          }
        else
          {
            unsigned char
              *colormap;

            unsigned int
              packet_size;

            /*
              Read image colormap from file.
            */
            packet_size=image->colors > 256 ? 6 : 3;
            colormap=(unsigned char *) AcquireMemory(packet_size*image->colors);
            if (colormap == (unsigned char *) NULL)
              ThrowReaderException(ResourceLimitWarning,
                "Memory allocation failed",image);
            (void) ReadBlob(image,packet_size*image->colors,colormap);
            p=colormap;
            if (image->colors <= 256)
              for (i=0; i < (long) image->colors; i++)
              {
                image->colormap[i].red=Upscale(*p++);
                image->colormap[i].green=Upscale(*p++);
                image->colormap[i].blue=Upscale(*p++);
              }
            else
              for (i=0; i < (long) image->colors; i++)
              {
                image->colormap[i].red=(*p++ << 8);
                image->colormap[i].red|=(*p++);
                image->colormap[i].green=(*p++ << 8);
                image->colormap[i].green|=(*p++);
                image->colormap[i].blue=(*p++ << 8);
                image->colormap[i].blue|=(*p++);
              }
            LiberateMemory((void **) &colormap);
          }
      }
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (image->fifo == (int (*)(const Image *,const void *,const size_t)) NULL)
      {
        /*
          Attach persistent pixel cache.
        */
        (void) strncpy(filename,image->filename,MaxTextExtent-1);
        AppendImageFormat("cache",filename);
        status=PersistCache(image,filename,True,&offset,exception);
        if (status == False)
          ThrowReaderException(CacheWarning,"Unable to perist pixel cache",
            image);
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    do
    {
      c=ReadBlobByte(image);
    } while (!isgraph(c) && (c != EOF));
    if (c != EOF)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=image->next;
        MagickMonitor(LoadImagesText,TellBlob(image),GetBlobSize(image));
      }
  } while (c != EOF);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M P C I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMPCImage adds attributes for the Cache image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMPCImage method is:
%
%      RegisterMPCImage(void)
%
*/
ModuleExport void RegisterMPCImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MPC");
  entry->decoder=ReadMPCImage;
  entry->encoder=WriteMPCImage;
  entry->magick=IsMPC;
  entry->description=AcquireString("Magick Persistent Cache image format");
  entry->module=AcquireString("MPC");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M P C I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMPCImage removes format registrations made by the
%  MPC module from the list of supported formats.
%
%  The format of the UnregisterMPCImage method is:
%
%      UnregisterMPCImage(void)
%
*/
ModuleExport void UnregisterMPCImage(void)
{
  (void) UnregisterMagickInfo("MPC");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M P C I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMPCImage writes an Magick Persistent Cache image to a file.
%
%  The format of the WriteMPCImage method is:
%
%      unsigned int WriteMPCImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteMPCImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
static unsigned int WriteMPCImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent],
    filename[MaxTextExtent];

  const ImageAttribute
    *attribute;

  off_t
    offset;

  register long
    i;

  unsigned int
    status;

  unsigned long
    scene;

  /*
    Open persistent cache.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  offset=0;
  do
  {
    /*
      Write persistent cache meta-information.
    */
    (void) WriteBlobString(image,"id=MagickCache\n");
    FormatString(buffer,"quantum-depth=%d\n",QuantumDepth);
    (void) WriteBlobString(image,buffer);
    if (image->storage_class == PseudoClass)
      FormatString(buffer,"class=PseudoClass  colors=%lu  matte=%s\n",
        image->colors,image->matte ? "True" : "False");
    else
      if (image->colorspace == CMYKColorspace)
        FormatString(buffer,"class=DirectClass  colorspace=CMYK  matte=%s\n",
          image->matte ? "True" : "False");
      else
        FormatString(buffer,"class=DirectClass  matte=%s\n",
          image->matte ? "True" : "False");
    (void) WriteBlobString(image,buffer);
    switch (image->compression)
    {
      default:
      case NoCompression:
      {
        (void) WriteBlobString(image,"compression=None\n");
        break;
      }
      case BZipCompression:
      {
        (void) WriteBlobString(image,"compression=BZip\n");
        break;
      }
      case FaxCompression:
      {
        (void) WriteBlobString(image,"compression=Fax\n");
        break;
      }
      case Group4Compression:
      {
        (void) WriteBlobString(image,"compression=Group4\n");
        break;
      }
      case JPEGCompression:
      {
        (void) WriteBlobString(image,"compression=JPEG\n");
        break;
      }
      case LosslessJPEGCompression:
      {
        (void) WriteBlobString(image,"compression=Lossless\n");
        break;
      }
      case LZWCompression:
      {
        (void) WriteBlobString(image,"compression=LZW\n");
        break;
      }
      case RunlengthEncodedCompression:
      {
        (void) WriteBlobString(image,"compression=RLE\n");
        break;
      }
      case ZipCompression:
      {
        (void) WriteBlobString(image,"compression=Zip\n");
        break;
      }
    }
    FormatString(buffer,"columns=%lu  rows=%lu  depth=%lu\n",image->columns,
      image->rows,image->depth);
    (void) WriteBlobString(image,buffer);
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
        FormatString(buffer,"Resolution=%gx%g  units=%.1024s\n",
          image->x_resolution,image->y_resolution,units);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->page.width != 0) && (image->page.height != 0))
      {
        FormatString(buffer,"page=%lux%lu%+ld%+ld\n",image->page.width,
          image->page.height,image->page.x,image->page.y);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
      {
        FormatString(buffer,
          "scene=%lu  iterations=%lu  delay=%lu  Dispose=%lu\n",
          image->scene,image->iterations,image->delay,image->dispose);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        if (image->scene != 0)
          {
            FormatString(buffer,"scene=%lu\n",image->scene);
            (void) WriteBlobString(image,buffer);
          }
        if (image->iterations != 1)
          {
            FormatString(buffer,"iterations=%lu\n",image->iterations);
            (void) WriteBlobString(image,buffer);
          }
        if (image->delay != 0)
          {
            FormatString(buffer,"delay=%lu\n",image->delay);
            (void) WriteBlobString(image,buffer);
          }
        if (image->dispose != 0)
          {
            FormatString(buffer,"dispose=%lu\n",image->dispose);
            (void) WriteBlobString(image,buffer);
          }
      }
    if (image->mean_error_per_pixel != 0.0)
      {
        FormatString(buffer,"error=%g  mean-error=%g  maximum-error=%g\n",
          image->mean_error_per_pixel,image->normalized_mean_error,
          image->normalized_maximum_error);
        (void) WriteBlobString(image,buffer);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        if (image->rendering_intent == SaturationIntent)
          (void) WriteBlobString(image,"rendering-intent=saturation\n");
        else
          if (image->rendering_intent == PerceptualIntent)
            (void) WriteBlobString(image,"rendering-intent=perceptual\n");
          else
            if (image->rendering_intent == AbsoluteIntent)
              (void) WriteBlobString(image,"rendering-intent=absolute\n");
            else
              (void) WriteBlobString(image,"rendering-intent=relative\n");
      }
    if (image->gamma != 0.0)
      {
        FormatString(buffer,"gamma=%g\n",image->gamma);
        (void) WriteBlobString(image,buffer);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        FormatString(buffer,
          "red-primary=%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) WriteBlobString(image,buffer);
        FormatString(buffer,"white-point=%g,%g\n",
          image->chromaticity.white_point.x,image->chromaticity.white_point.y);
        (void) WriteBlobString(image,buffer);
      }
    if (image->color_profile.length != 0)
      {
        FormatString(buffer,"profile-icc=%lu\n",(unsigned long)
          image->color_profile.length);
        (void) WriteBlobString(image,buffer);
      }
    if (image->iptc_profile.length != 0)
      {
        FormatString(buffer,"profile-iptc=%lu\n",(unsigned long)
          image->iptc_profile.length);
        (void) WriteBlobString(image,buffer);
      }
    if (image->generic_profiles != 0)
      {
        /*
          Generic profile.
        */
        for (i=0; i < (long) image->generic_profiles; i++)
        {
          FormatString(buffer,"profile-%s=%lu\n",
            image->generic_profile[i].name == (char *) NULL ? "generic" :
            image->generic_profile[i].name,(unsigned long)
            image->generic_profile[i].length);
          (void) WriteBlobString(image,buffer);
        }
      }
    if (image->montage != (char *) NULL)
      {
        FormatString(buffer,"montage=%.1024s\n",image->montage);
        (void) WriteBlobString(image,buffer);
      }
    (void) SignatureImage(image);
    attribute=GetImageAttribute(image,(char *) NULL);
    for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
    {
      if (attribute->value != NULL)
        {
          long
            j;

          FormatString(buffer,"%.1024s=",attribute->key);
          (void) WriteBlobString(image,buffer);
          for (j=0; j < (long) strlen(attribute->value); j++)
            if (isspace((int) attribute->value[j]))
              break;
          if (j < (long) strlen(attribute->value))
            (void) WriteBlobByte(image,'{');
          (void) WriteBlobString(image,attribute->value);
          if (j < (long) strlen(attribute->value))
            (void) WriteBlobByte(image,'}');
          (void) WriteBlobByte(image,'\n');
        }
    }
    (void) WriteBlobString(image,"\f\n:\032");
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) WriteBlobString(image,image->directory);
        (void) WriteBlobByte(image,'\0');
      }
    if (image->color_profile.length != 0)
      (void) WriteBlob(image,image->color_profile.length,
        image->color_profile.info);
    if (image->iptc_profile.length != 0)
      (void) WriteBlob(image,image->iptc_profile.length,
        image->iptc_profile.info);
    if (image->generic_profiles != 0)
      {
        /*
          Generic profile.
        */
        for (i=0; i < (long) image->generic_profiles; i++)
        {
          if (image->generic_profile[i].length == 0)
            continue;
          (void) WriteBlob(image,image->generic_profile[i].length,
            image->generic_profile[i].info);
        }
      }
    if (image->storage_class == PseudoClass)
      {
        register unsigned char
          *q;

        unsigned char
          *colormap;

        unsigned int
          packet_size;

        /*
          Allocate colormap.
        */
        packet_size=image->colors > 256 ? 6 : 3;
        colormap=(unsigned char *) AcquireMemory(packet_size*image->colors);
        if (colormap == (unsigned char *) NULL)
          return(False);
        /*
          Write colormap to file.
        */
        q=colormap;
        if (image->colors <= 256)
          for (i=0; i < (long) image->colors; i++)
          {
            *q++=image->colormap[i].red;
            *q++=image->colormap[i].green;
            *q++=image->colormap[i].blue;
          }
        else
          for (i=0; i < (long) image->colors; i++)
          {
            *q++=image->colormap[i].red >> 8;
            *q++=image->colormap[i].red & 0xff;
            *q++=image->colormap[i].green >> 8;
            *q++=image->colormap[i].green & 0xff;
            *q++=image->colormap[i].blue >> 8;
            *q++=image->colormap[i].blue & 0xff;
          }
        (void) WriteBlob(image,packet_size*image->colors,colormap);
        LiberateMemory((void **) &colormap);
      }
    /*
      Initialize persistent pixel cache.
    */
    (void) strncpy(filename,image->filename,MaxTextExtent-1);
    AppendImageFormat("cache",filename);
    status=PersistCache(image,filename,False,&offset,&image->exception);
    if (status == False)
      ThrowWriterException(CacheWarning,"Unable to perist pixel cache",image);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetImageListSize(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(status);
}
