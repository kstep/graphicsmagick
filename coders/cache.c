/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      CCCC   AAA    CCCC  H   H  EEEEE                       %
%                     C      A   A  C      H   H  E                           %
%                     C      AAAAA  C      HHHHH  EEE                         %
%                     C      A   A  C      H   H  E                           %
%                      CCCC  A   A   CCCC  H   H  EEEEE                       %
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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
  WriteCACHEImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C A C H E                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsCACHE returns True if the image format type, identified by the
%  magick string, is an Magick Persistent Cache image.
%
%  The format of the IsCACHE method is:
%
%      unsigned int IsCACHE(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsCACHE returns True if the image format type is Cache.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsCACHE(const unsigned char *magick,
  const unsigned int length)
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
%  Method ReadCACHEImage reads an Magick Persistent Cache image file and returns
%  it.  It allocates the memory necessary for the new Image structure and 
%  returns a pointer to the new image.
%
%  The format of the ReadCACHEImage method is:
%
%      Image *ReadCACHEImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  Decompression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o image: Method ReadCACHEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCACHEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
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

  size_t
    offset;

  unsigned int
    status;

  void
    *allocation;

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
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    length=MaxTextExtent;
    values=(char *) AllocateMemory(length);
    if (values == (char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
        image);
    GetCacheInfo(&image->cache);
    cache_info=(CacheInfo *) image->cache;
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
          comment=(char *) AllocateMemory(length);
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
                comment=(char *) ReallocateMemory(comment,length);
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
          FreeMemory((void **) &comment);
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
                  values=(char *) ReallocateMemory(values,length);
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
            if (LocaleCompare(keyword,"Background-color") == 0)
              (void) QueryColorDatabase(values,&image->background_color);
            if (LocaleCompare(keyword,"Blue-primary") == 0)
              (void) sscanf(values,"%lf,%lf",
                &image->chromaticity.blue_primary.x,
                &image->chromaticity.blue_primary.y);
            if (LocaleCompare(keyword,"Border-color") == 0)
              (void) QueryColorDatabase(values,&image->border_color);
            if (LocaleCompare(keyword,"Cache") == 0)
              {
                (void) strcpy(cache_info->filename,values);
                cache_info->file=
                  open(cache_info->filename,O_RDWR | O_BINARY,0777);
                if (cache_info->file == -1)
                  ThrowReaderException(CacheWarning,
                    "failed to open persistent cache",image);
              }
            if (LocaleCompare(keyword,"Class") == 0)
              {
                if (LocaleCompare(values,"PseudoClass") == 0)
                  image->class=PseudoClass;
                else
                  if (LocaleCompare(values,"DirectClass") == 0)
                    image->class=DirectClass;
                  else
                    image->class=UndefinedClass;
              }
            if (LocaleCompare(keyword,"Colors") == 0)
              image->colors=(unsigned int) atoi(values);
            if (LocaleCompare(keyword,"Color-profile") == 0)
              image->color_profile.length=(unsigned int) atoi(values);
            if (LocaleCompare(keyword,"Colorspace") == 0)
              {
                if (LocaleCompare(values,"CMYK") == 0)
                  image->colorspace=CMYKColorspace;
                else
                  if (LocaleCompare(values,"RGB") == 0)
                    image->colorspace=RGBColorspace;
              }
            if (LocaleCompare(keyword,"Compression") == 0)
              {
                if (LocaleCompare(values,"Zip") == 0)
                  image->compression=ZipCompression;
                else
                  if (LocaleCompare(values,"BZip") == 0)
                    image->compression=BZipCompression;
                  else
                    if (LocaleCompare(values,"RunlengthEncoded") == 0)
                      image->compression=RunlengthEncodedCompression;
                    else
                      image->compression=UndefinedCompression;
              }
            if (LocaleCompare(keyword,"Columns") == 0)
              image->columns=(unsigned int) atoi(values);
            if (LocaleCompare(keyword,"Delay") == 0)
              {
                if (image_info->delay == (char *) NULL)
                  image->delay=atoi(values);
              }
            if (LocaleCompare(keyword,"Depth") == 0)
              image->depth=atoi(values) <= 8 ? 8 : 16;
            if (LocaleCompare(keyword,"Dispose") == 0)
              {
                if (image_info->dispose == (char *) NULL)
                  image->dispose=atoi(values);
              }
            if (LocaleCompare(keyword,"Gamma") == 0)
              image->gamma=atof(values);
            if (LocaleCompare(keyword,"Green-primary") == 0)
              (void) sscanf(values,"%lf,%lf",
                &image->chromaticity.green_primary.x,
                &image->chromaticity.green_primary.y);
            if (LocaleCompare(keyword,"Id") == 0)
              (void) strcpy(id,values);
            if (LocaleCompare(keyword,"Iterations") == 0)
              {
                if (image_info->iterations == (char *) NULL)
                  image->iterations=atoi(values);
              }
            if (LocaleCompare(keyword,"Matte") == 0)
              image->matte=(LocaleCompare(values,"True") == 0) ||
                (LocaleCompare(values,"true") == 0);
            if (LocaleCompare(keyword,"Matte-color") == 0)
              (void) QueryColorDatabase(values,&image->matte_color);
            if (LocaleCompare(keyword,"Montage") == 0)
              (void) CloneString(&image->montage,values);
            if (LocaleCompare(keyword,"Page") == 0)
              ParseImageGeometry(PostscriptGeometry(values),
                &image->page.x,&image->page.y,
                &image->page.width,&image->page.height);
            if (LocaleCompare(keyword,"Red-primary") == 0)
              (void) sscanf(values,"%lf,%lf",&image->chromaticity.red_primary.x,
                &image->chromaticity.red_primary.y);
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
              }
            if (LocaleCompare(keyword,"Resolution") == 0)
              (void) sscanf(values,"%lfx%lf",&image->x_resolution,
                &image->y_resolution);
            if (LocaleCompare(keyword,"Rows") == 0)
              image->rows=(unsigned int) atoi(values);
            if (LocaleCompare(keyword,"Scene") == 0)
              image->scene=(unsigned int) atoi(values);
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
              }
            if (LocaleCompare(keyword,"White-point") == 0)
              (void) sscanf(values,"%lf,%lf",&image->chromaticity.white_point.x,
                &image->chromaticity.white_point.y);
            if (*values == '{')
              (void) SetImageAttribute(image,keyword,values+1);
          }
        else
          c=ReadByte(image);
      while (isspace(c))
        c=ReadByte(image);
    }
    FreeMemory((void **) &values);
    (void) ReadByte(image);
    /*
      Verify that required image information is defined.
    */
    if ((strcmp(id,"MagickCache") != 0) || (image->class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0) || (*cache_info->filename == '\0'))
      ThrowReaderException(CorruptImageWarning,
        "Incorrect image header in file",image);
    if (image_info->ping)
      {
        CloseBlob(image);
        return(image);
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        image->directory=(char *) AllocateMemory(MaxTextExtent);
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
              image->directory=(char *) ReallocateMemory((char *)
                image->directory,(Extent(image->directory)+MaxTextExtent+1));
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
          AllocateMemory(image->color_profile.length);
        if (image->color_profile.info == (unsigned char *) NULL)
          ThrowReaderException(CorruptImageWarning,
            "Unable to read color profile",image);
        (void) ReadBlob(image,image->color_profile.length,
          image->color_profile.info);
      }
    if (image->class == PseudoClass)
      {
        /*
          Create image colormap.
        */
        image->colormap=(PixelPacket *)
          AllocateMemory(Max(image->colors,256)*sizeof(PixelPacket));
        if (image->colormap == (PixelPacket *) NULL)
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
            colormap=(unsigned char *)
              AllocateMemory(packet_size*image->colors);
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
            FreeMemory((void **) &colormap);
          }
      }
    /*
      Initialize cache.
    */
    cache_info->class=image->class;
    cache_info->type=DiskCache;
    cache_info->rows=image->rows;
    cache_info->columns=image->columns;
    cache_info->persist=True;
    allocation=MapBlob(cache_info->file,IOMode,&offset);
    if (allocation != (void *) NULL)
      {
        /*
          Create memory-mapped pixel cache.
        */
        cache_info->type=MemoryMappedCache;
        cache_info->pixels=(PixelPacket *) allocation;
        if (cache_info->class == PseudoClass)
          cache_info->indexes=(IndexPacket *)
            (cache_info->pixels+cache_info->columns*cache_info->rows);
      }
    /*
      Initialize cache nexus.
    */
    cache_info->nexus=(NexusInfo *)
      AllocateMemory((cache_info->rows+1)*sizeof(NexusInfo));
    if (cache_info->nexus == (NexusInfo *) NULL)
      MagickError(ResourceLimitError,"Memory allocation failed",
        "unable to allocate cache nexus");
    for (i=0; i <= cache_info->rows; i++)
    {
      cache_info->nexus[i].available=True;
      cache_info->nexus[i].columns=0;
      cache_info->nexus[i].rows=0;
      cache_info->nexus[i].x=0;
      cache_info->nexus[i].y=0;
      cache_info->nexus[i].length=0;
      cache_info->nexus[i].line=(void *) NULL;
      cache_info->nexus[i].pixels=(PixelPacket *) NULL;
      cache_info->nexus[i].indexes=(IndexPacket *) NULL;
    }
    cache_info->nexus[0].available=False;
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    do
    {
      c=ReadByte(image);
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
        ProgressMonitor(LoadImagesText,TellBlob(image),image->filesize);
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
%   R e g i s t e r C A C H E I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCACHEImage adds attributes for the Cache image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCACHEImage method is:
%
%      RegisterCACHEImage(void)
%
*/
ModuleExport void RegisterCACHEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CACHE");
  entry->decoder=ReadCACHEImage;
  entry->encoder=WriteCACHEImage;
  entry->magick=IsCACHE;
  entry->description=AllocateString("Magick Persistent Cache image format");
  entry->module=AllocateString("CACHE");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C A C H E I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCACHEImage removes format registrations made by the
%  CACHE module from the list of supported formats.
%
%  The format of the UnregisterCACHEImage method is:
%
%      UnregisterCACHEImage(void)
%
*/
ModuleExport void UnregisterCACHEImage(void)
{
  UnregisterMagickInfo("CACHE");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C A C H E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCACHEImage writes an Magick Persistent Cache image to a file.
%
%  The format of the WriteCACHEImage method is:
%
%      unsigned int WriteCACHEImage(const ImageInfo *image_info,Image *image)
%
%  Compression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o status: Method WriteCACHEImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
static unsigned int WriteCACHEImage(const ImageInfo *image_info,Image *image)
{
  CacheInfo
    *cache_info;

  char
    buffer[MaxTextExtent],
    color[MaxTextExtent];

  CompressionType
    compression;

  ImageAttribute
    *attribute;

  register int
    i;

  register unsigned char
    *q;

  unsigned int
    packet_size,
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) IsPseudoClass(image);
  (void) strcpy((char *) image_info->magick,"Cache");
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  scene=0;
  do
  {
    /*
      Write Cache header.
    */
    cache_info=(CacheInfo *) image->cache;
    if (cache_info->type == MemoryCache)
      ThrowWriterException(CacheWarning,"Cache must be out-of-core",image);
    cache_info->persist=True;
    if (((image_info->colorspace != UndefinedColorspace) ||
         (image->colorspace != CMYKColorspace)) &&
         (image_info->colorspace != CMYKColorspace))
      TransformRGBImage(image,RGBColorspace);
    else
      if (image->colorspace != CMYKColorspace)
        RGBTransformImage(image,CMYKColorspace);
    (void) strcpy(buffer,"Id=MagickCache\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    FormatString(buffer,"cache=%.1024s\n",cache_info->filename);
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (image->class == PseudoClass)
      FormatString(buffer,"Class=PseudoClass  Colors=%u  Matte=%s\n",
        image->colors,image->matte ? "True" : "False");
    else
      if (image->colorspace != CMYKColorspace)
        FormatString(buffer,"Class=DirectClass  Matte=%s\n",
          image->matte ? "True" : "False");
      else
        (void) strcpy(buffer,"Class=DirectClass  Colorspace=CMYK\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    *buffer='\0';
    if (compression == RunlengthEncodedCompression)
      FormatString(buffer,"Compression=RunlengthEncoded\n");
    else
      if (compression == BZipCompression)
        FormatString(buffer,"Compression=BZip\n");
      else
        if (compression == ZipCompression)
          FormatString(buffer,"Compression=Zip\n");
    if (*buffer != '\0')
      (void) WriteBlob(image,strlen(buffer),buffer);
    FormatString(buffer,"Columns=%u  Rows=%u  Depth=%u\n",image->columns,
      image->rows,image->depth);
    (void) WriteBlob(image,strlen(buffer),buffer);
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
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if ((image->page.width != 0) && (image->page.height != 0))
      {
        FormatString(buffer,"Page=%ux%u%+d%+d\n",image->page.width,
          image->page.height,image->page.x,image->page.y);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    (void) QueryColorName(&image->background_color,color);
    FormatString(buffer,"Background-color=%.1024s  ",color);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) QueryColorName(&image->border_color,color);
    FormatString(buffer,"Border-color=%.1024s  ",color);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) QueryColorName(&image->matte_color,color);
    FormatString(buffer,"Matte-color=%.1024s\n",color);
    (void) WriteBlob(image,strlen(buffer),buffer);
    if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
      {
        FormatString(buffer,"Scene=%u  Iterations=%u  Delay=%u  Dispose=%u\n",
          image->scene,image->iterations,image->delay,image->dispose);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    else
      {
        if (image->scene != 0)
          {
            FormatString(buffer,"Scene=%u\n",image->scene);
            (void) WriteBlob(image,strlen(buffer),buffer);
          }
        if (image->iterations != 1)
          {
            FormatString(buffer,"Iterations=%u\n",image->iterations);
            (void) WriteBlob(image,strlen(buffer),buffer);
          }
        if (image->delay != 0)
          {
            FormatString(buffer,"Delay=%u\n",image->delay);
            (void) WriteBlob(image,strlen(buffer),buffer);
          }
        if (image->dispose != 0)
          {
            FormatString(buffer,"Dispose=%u\n",image->dispose);
            (void) WriteBlob(image,strlen(buffer),buffer);
          }
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        if (image->rendering_intent == SaturationIntent)
          (void) strcpy(buffer,"Rendering-intent=saturation\n");
        else
          if (image->rendering_intent == PerceptualIntent)
            (void) strcpy(buffer,"Rendering-intent=perceptual\n");
          else
            if (image->rendering_intent == AbsoluteIntent)
              (void) strcpy(buffer,"Rendering-intent=absolute\n");
            else
              (void) strcpy(buffer,"Rendering-intent=relative\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if (image->gamma != 0.0)
      {
        FormatString(buffer,"Gamma=%g\n",image->gamma);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        FormatString(buffer,
          "Red-primary=%g,%g  Green-primary=%g,%g  Blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) WriteBlob(image,strlen(buffer),buffer);
        FormatString(buffer,"White-point=%g,%g\n",
          image->chromaticity.white_point.x,image->chromaticity.white_point.y);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if (image->color_profile.length > 0)
      {
        FormatString(buffer,"Color-profile=%u\n",image->color_profile.length);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if (image->montage != (char *) NULL)
      {
        FormatString(buffer,"Montage=%.1024s\n",image->montage);
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    SignatureImage(image);
    attribute=GetImageAttribute(image,(char *) NULL);
    while (attribute != (ImageAttribute *) NULL)
    {
      if (attribute->value != NULL)
        {
          FormatString(buffer,"%.1024s=",attribute->key);
          (void) WriteBlob(image,strlen(buffer),buffer);
          for (i=0; i < strlen(attribute->value); i++)
            if (isspace((int) attribute->value[i]))
              break;
          if (i < strlen(attribute->value))
            (void) WriteByte(image,'{');
          (void) WriteBlob(image,strlen(attribute->value),attribute->value);
          if (i < strlen(attribute->value))
            (void) WriteByte(image,'}');
          (void) WriteByte(image,'\n');
        }
      attribute=attribute->next;
    }
    (void) strcpy(buffer,"\f\n:\032");
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) WriteBlob(image,strlen(image->directory),image->directory);
        (void) WriteByte(image,'\0');
      }
    if (image->color_profile.length > 0)
      (void) WriteBlob(image,(int) image->color_profile.length,
        (char *) image->color_profile.info);
    if (image->class == PseudoClass)
      {
        unsigned char
          *colormap;

        /*
          Allocate colormap.
        */
        packet_size=image->colors > 256 ? 6 : 3;
        colormap=(unsigned char *) AllocateMemory(packet_size*image->colors);
        if (colormap == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Write colormap to file.
        */
        q=colormap;
        if (image->colors <= 256)
          for (i=0; i < (int) image->colors; i++)
          {
            *q++=image->colormap[i].red;
            *q++=image->colormap[i].green;
            *q++=image->colormap[i].blue;
          }
        else
          for (i=0; i < (int) image->colors; i++)
          {
            *q++=image->colormap[i].red >> 8;
            *q++=image->colormap[i].red & 0xff;
            *q++=image->colormap[i].green >> 8;
            *q++=image->colormap[i].green  & 0xff;
            *q++=image->colormap[i].blue >> 8;
            *q++=image->colormap[i].blue  & 0xff;
          }
        (void) WriteBlob(image,packet_size*image->colors,colormap);
        FreeMemory((void **) &colormap);
      }
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(status);
}
