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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

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
%      unsigned int IsMPC(const unsigned char *magick,const unsigned int length)
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
static unsigned int IsMPC(const unsigned char *magick,const unsigned int length)
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
    id[MaxTextExtent],
    keyword[MaxTextExtent],
    *values;

  CacheInfo
    *cache_info;

  Image
    *image;

  int
    c,
    length;

  register int
    i;

  register unsigned char
    *p;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  c=ReadByte(image);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  *id='\0';
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  length=MaxTextExtent;
  values=AllocateString((char *) NULL);
  GetCacheInfo(&image->cache);
  cache_info=(CacheInfo *) image->cache;
  (void) strcpy(cache_info->meta_filename,image_info->filename);
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
        comment=AllocateString((char *) NULL);
        p=comment;
        for ( ; comment != (char *) NULL; p++)
        {
          c=ReadByte(image);
          if ((c == EOF) || (c == '}'))
            break;
          if ((p-comment+1) >= (int) length)
            {
              *p='\0';
              length<<=1;
              ReacquireMemory((void **) &comment,length);
              if (comment == (char *) NULL)
                break;
              p=comment+Extent(comment);
            }
          *p=c;
        }
        if (comment == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,
            "Memory allocation failed",image);
        *p='\0';
        (void) SetImageAttribute(image,"Comment",comment);
        LiberateMemory((void **) &comment);
        c=ReadByte(image);
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
            c=ReadByte(image);
          } while (isalnum(c) || (c == '-'));
          *p='\0';
          while (isspace(c) || (c == '='))
            c=ReadByte(image);
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
                p=values+Extent(values);
              }
            if (values == (char *) NULL)
              ThrowReaderException(ResourceLimitWarning,
                "Memory allocation failed",image);
            *p++=c;
            c=ReadByte(image);
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
              if (LocaleCompare(keyword,"Background-color") == 0)
                {
                  (void) QueryColorDatabase(values,&image->background_color);
                  break;
                }
              if (LocaleCompare(keyword,"Blue-primary") == 0)
                {
                  (void) sscanf(values,"%lf,%lf",
                    &image->chromaticity.blue_primary.x,
                    &image->chromaticity.blue_primary.y);
                  break;
                }
              if (LocaleCompare(keyword,"Border-color") == 0)
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
              if (LocaleCompare(keyword,"Cache") == 0)
                {
                  (void) strcpy(cache_info->cache_filename,values);
                  break;
                }
              if (LocaleCompare(keyword,"Class") == 0)
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
              if (LocaleCompare(keyword,"Colors") == 0)
                {
                  image->colors=(unsigned int) atoi(values);
                  break;
                }
              if (LocaleCompare(keyword,"Colorspace") == 0)
                {
                  if (LocaleCompare(values,"CMYK") == 0)
                    image->colorspace=CMYKColorspace;
                  else
                    if (LocaleCompare(values,"RGB") == 0)
                      image->colorspace=RGBColorspace;
                  break;
                }
              if (LocaleCompare(keyword,"Compression") == 0)
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
                  if (LocaleCompare("RunlengthEncoded",values) == 0)
                    compression=RunlengthEncodedCompression;
                  if (LocaleCompare("Zip",values) == 0)
                    compression=ZipCompression;
                  image->compression=compression;
                  break;
                }
              if (LocaleCompare(keyword,"Columns") == 0)
                {
                  image->columns=(unsigned int) atoi(values);
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 'd':
            case 'D':
            {
              if (LocaleCompare(keyword,"Delay") == 0)
                {
                  if (image_info->delay == (char *) NULL)
                    image->delay=atoi(values);
                  break;
                }
              if (LocaleCompare(keyword,"Depth") == 0)
                {
                  image->depth=atoi(values) <= 8 ? 8 : 16;
                  break;
                }
              if (LocaleCompare(keyword,"Dispose") == 0)
                {
                  if (image_info->dispose == (char *) NULL)
                    image->dispose=atoi(values);
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 'g':
            case 'G':
            {
              if (LocaleCompare(keyword,"Gamma") == 0)
                {
                  image->gamma=atof(values);
                  break;
                }
              if (LocaleCompare(keyword,"Green-primary") == 0)
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
              if (LocaleCompare(keyword,"Id") == 0)
                {
                  (void) strcpy(id,values);
                  break;
                }
              if (LocaleCompare(keyword,"Iterations") == 0)
                {
                  if (image_info->iterations == (char *) NULL)
                    image->iterations=atoi(values);
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 'm':
            case 'M':
            {
              if (LocaleCompare(keyword,"Matte") == 0)
                {
                  image->matte=(LocaleCompare(values,"True") == 0) ||
                    (LocaleCompare(values,"true") == 0);
                  break;
                }
              if (LocaleCompare(keyword,"Matte-color") == 0)
                {
                  (void) QueryColorDatabase(values,&image->matte_color);
                  break;
                }
              if (LocaleCompare(keyword,"Montage") == 0)
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
              if (LocaleCompare(keyword,"Page") == 0)
                {
                  ParseImageGeometry(PostscriptGeometry(values),
                    &image->page.x,&image->page.y,
                    &image->page.width,&image->page.height);
                  break;
                }
              if (LocaleNCompare(keyword,"Profile-",8) == 0)
                {
                  if (LocaleCompare(keyword,"Profile-icc") == 0)
                    {
                      image->color_profile.length=atoi(values);
                      break;
                    }
                  if (LocaleCompare(keyword,"Profile-iptc") == 0)
                    {
                      image->iptc_profile.length=atoi(values);
                      break;
                    }
                  i=image->generic_profiles;
                  if (image->generic_profile == (ProfileInfo *) NULL)
                    image->generic_profile=(ProfileInfo *)
                      AcquireMemory(sizeof(ProfileInfo));
                  else
                    ReacquireMemory((void **) &image->generic_profile,
                      (i+1)*sizeof(ProfileInfo));
                  if (image->generic_profile == (ProfileInfo *) NULL)
                    ThrowReaderException(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  image->generic_profile[i].name=AllocateString(keyword+8);
                  image->generic_profile[i].length=atoi(values);
                  image->generic_profile[i].info=(unsigned char *) NULL;
                  image->generic_profiles++;
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 'r':
            case 'R':
            {
              if (LocaleCompare(keyword,"Red-primary") == 0)
                {
                  (void) sscanf(values,"%lf,%lf",
                    &image->chromaticity.red_primary.x,
                    &image->chromaticity.red_primary.y);
                  break;
                }
              if (LocaleCompare(keyword,"Rendering-intent") == 0)
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
              if (LocaleCompare(keyword,"Resolution") == 0)
                {
                  (void) sscanf(values,"%lfx%lf",&image->x_resolution,
                    &image->y_resolution);
                  break;
                }
              if (LocaleCompare(keyword,"Rows") == 0)
                {
                  image->rows=(unsigned int) atoi(values);
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 's':
            case 'S':
            {
              if (LocaleCompare(keyword,"Scene") == 0)
                {
                  image->scene=(unsigned int) atoi(values);
                  break;
                }
              (void) SetImageAttribute(image,keyword,
                *values == '{' ? values+1 : values);
              break;
            }
            case 'u':
            case 'U':
            {
              if (LocaleCompare(keyword,"Units") == 0)
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
              if (LocaleCompare(keyword,"White-point") == 0)
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
        c=ReadByte(image);
    while (isspace(c))
      c=ReadByte(image);
  }
  LiberateMemory((void **) &values);
  (void) ReadByte(image);
  /*
    Verify that required image information is defined.
  */
  if ((LocaleCompare(id,"MagickCache") != 0) ||
      (image->storage_class == UndefinedClass) ||
      (image->compression == UndefinedCompression) || (image->columns == 0) ||
      (image->rows == 0) || (*cache_info->cache_filename == '\0'))
    ThrowReaderException(CorruptImageWarning,"Incorrect image header in file",
      image);
  if (image->depth != QuantumDepth)
    ThrowReaderException(CacheWarning,"Inconsistent peristent cache depth",
      image);
  if (!Isaccessible(cache_info->cache_filename))
    ThrowReaderException(CorruptImageWarning,"Unable to open peristent cache",
      image);
  if (image->montage != (char *) NULL)
    {
      register char
        *p;

      /*
        Image directory.
      */
      image->directory=AllocateString((char *) NULL);
      if (image->directory == (char *) NULL)
        ThrowReaderException(CorruptImageWarning,"Unable to read image data",
          image);
      p=image->directory;
      do
      {
        *p='\0';
        if (((Extent(image->directory)+1) % MaxTextExtent) == 0)
          {
            /*
              Allocate more memory for the image directory.
            */
            ReacquireMemory((void **) &image->directory,
              (Extent(image->directory)+MaxTextExtent+1));
            if (image->directory == (char *) NULL)
              ThrowReaderException(CorruptImageWarning,
                "Unable to read image data",image);
            p=image->directory+Extent(image->directory);
          }
        c=ReadByte(image);
        *p++=c;
      } while (c != '\0');
    }
  if (image->color_profile.length > 0)
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
  if (image->iptc_profile.length > 0)
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
      for (i=0; i < image->generic_profiles; i++)
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
          image->colormap[i].red=UpScale(i);
          image->colormap[i].green=UpScale(i);
          image->colormap[i].blue=UpScale(i);
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
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=UpScale(*p++);
              image->colormap[i].green=UpScale(*p++);
              image->colormap[i].blue=UpScale(*p++);
            }
          else
            for (i=0; i < (int) image->colors; i++)
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
  /*
    Initialize cache.
  */
  cache_info->storage_class=image->storage_class;
  cache_info->type=DiskCache;
  cache_info->persist=True;
  status=OpenCache(cache_info,image->storage_class,image->columns,image->rows);
  if (status == False)
    ThrowReaderException(CacheWarning,"Unable to open peristent cache",image);
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
  entry->adjoin=False;
  entry->description=AllocateString("Magick Persistent Cache image format");
  entry->module=AllocateString("MPC");
  RegisterMagickInfo(entry);
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
  UnregisterMagickInfo("MPC");
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
  CacheInfo
    *cache_info;

  unsigned int
    status;

  /*
    Open persistent cache.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  cache_info=(CacheInfo *) image->cache;
  cache_info->persist=True;
  (void) strcpy(cache_info->meta_filename,image->filename);
  if (cache_info->type == MemoryCache)
    {
      int
        file;

      char
        filename[MaxTextExtent];

      register int
        y;

      register PixelPacket
        *p;

      /*
        Write persistent cache to disk.
      */
      (void) strcpy(cache_info->cache_filename,image->filename);
      AppendImageFormat("cache",cache_info->cache_filename);
      file=open(cache_info->cache_filename,O_WRONLY | O_CREAT | O_BINARY,0777);
      if (file == -1)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      for (y=0; y < (int) image->rows; y++)
      {
        p=GetImagePixels(image,0,y,image->columns,1);
        if (p == (PixelPacket *) NULL)
          break;
        (void) write(file,(char *) p,image->columns*sizeof(PixelPacket));
      }
      if (image->storage_class == PseudoClass)
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          (void) write(file,(char *) GetIndexes(image),image->columns*
            sizeof(IndexPacket));
        }
      (void) close(file);
    }
  CloseBlob(image);
  return(status);
}
