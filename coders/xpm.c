/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  PPPP   M   M                              %
%                             X X   P   P  MM MM                              %
%                              X    PPPP   M M M                              %
%                             X X   P      M   M                              %
%                            X   X  P      M   M                              %
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
  WritePICONImage(const ImageInfo *,Image *),
  WriteXPMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s X P M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsXPM returns True if the image format type, identified by the
%  magick string, is XPM.
%
%  The format of the IsXPM method is:
%
%      unsigned int IsXPM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsXPM returns True if the image format type is XPM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsXPM(const unsigned char *magick,const size_t length)
{
  if (length < 9)
    return(False);
  if (LocaleNCompare((char *) magick,"/* XPM */",9) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X P M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXPMImage reads an X11 pixmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXPMImage method is:
%
%      Image *ReadXPMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXPMImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static char *ParseColor(char *data)
{
#define NumberTargets  6

  static const char
    *targets[NumberTargets] = { "c ", "g ", "g4 ", "m ", "b ", "s " };

  register char
     *p,
     *r;

  register const char
     *q;

  register int
    i;

  for (i=0; i < NumberTargets; i++)
  {
    r=data;
    for (q=targets[i]; *r != '\0'; r++)
    {
      if (*r != *q)
        continue;
      if (!isspace((int) (*(r-1))))
        continue;
      p=r;
      for ( ; ; )
      {
        if (*q == '\0')
          return(r);
        if (*p++ != *q++)
          break;
      }
      q=targets[i];
    }
  }
  return((char *) NULL);
}

static Image *ReadXPMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    key[MaxTextExtent],
    **keys,
    target[MaxTextExtent],
    **textlist,
    *xpm_buffer;

  Image
    *image;

  int
    count;

  long
    j,
    none,
    y;

  register char
    *p,
    *q;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *r;

  register long
    i;

  unsigned int
    length,
    status,
    width;

  unsigned long
    colors;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read XPM file.
  */
  length=MaxTextExtent;
  xpm_buffer=(char *) AcquireMemory(length);
  p=xpm_buffer;
  if (xpm_buffer != (char *) NULL)
    while (ReadBlobString(image,p) != (char *) NULL)
    {
      if (*p == '#')
        if ((p == xpm_buffer) || (*(p-1) == '\n'))
          continue;
      if ((*p == '}') && (*(p+1) == ';'))
        break;
      p+=strlen(p);
      if ((p-xpm_buffer+MaxTextExtent+1) < (int) length)
        continue;
      length<<=1;
      ReacquireMemory((void **) &xpm_buffer,length);
      if (xpm_buffer == (char *) NULL)
        break;
      p=xpm_buffer+strlen(xpm_buffer);
    }
  if (xpm_buffer == (char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Remove comments.
  */
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if ((*p == '"') || (*p == '\''))
      {
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          for (p++; *p != '\0'; p++)
            if ((*p == '\'') && (*(p-1) != '\\'))
              break;
        if (*p == '\0')
          break;
        continue;
      }
    if ((*p != '/') || (*(p+1) != '*'))
      continue;
    for (q=p+2; *q != '\0'; q++)
      if ((*q == '*') && (*(q+1) == '/'))
        break;
    (void) strcpy(p,q+2);
  }
  /*
    Remove unquoted characters.
  */
  i=0;
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if (*p != '"')
      continue;
    for (q=p+1; *q != '\0'; q++)
      if (*q == '"')
        break;
    (void) strncpy(xpm_buffer+i,p+1,q-p-1);
    i+=q-p-1;
    xpm_buffer[i++]='\n';
    p=q+1;
  }
  xpm_buffer[i]='\0';
  textlist=StringToList(xpm_buffer);
  LiberateMemory((void **) &xpm_buffer);
  if (textlist == (char **) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read hints.
  */
  count=sscanf(textlist[0],"%lu %lu %lu %u",&image->columns,&image->rows,
    &colors,&width);
  image->colors=colors;
  if ((count != 4) || (width > 2) || (image->columns == 0) ||
      (image->rows == 0) || (image->colors == 0))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        LiberateMemory((void **) &textlist[i]);
      LiberateMemory((void **) &textlist);
      ThrowReaderException(CorruptImageWarning,"Not a XPM image file",image)
    }
  image->depth=8;
  /*
    Initialize image structure.
  */
  keys=(char **) AcquireMemory(image->colors*sizeof(char *));
  if (!AllocateImageColormap(image,image->colors) || (keys == (char **) NULL))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        LiberateMemory((void **) &textlist[i]);
      LiberateMemory((void **) &textlist);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image)
    }
  /*
    Read image colormap.
  */
  i=1;
  none=(-1);
  for (j=0; j < (long) image->colors; j++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    keys[j]=(char *) AcquireMemory(width+1);
    if (keys[j] == (char *) NULL)
      {
        for (i=0; textlist[i] != (char *) NULL; i++)
          LiberateMemory((void **) &textlist[i]);
        LiberateMemory((void **) &textlist);
        LiberateMemory((void **) &keys);
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image)
      }
    keys[j][width]='\0';
    (void) strncpy(keys[j],p,width);
    /*
      Parse color.
    */
    (void) strcpy(target,"gray");
    q=ParseColor(p+width);
    if (q != (char *) NULL)
      {
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
        (void) strncpy(target,q,MaxTextExtent-1);
        q=ParseColor(target);
        if (q != (char *) NULL)
          *q='\0';
      }
    Strip(target);
    if (LocaleCompare(target,"none") == 0)
      {
        image->storage_class=DirectClass;
        image->matte=True;
        none=j;
        (void) strcpy(target,"black");
      }
    (void) QueryColorDatabase(target,&image->colormap[j]);
  }
  if (j < (long) image->colors)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        LiberateMemory((void **) &textlist[i]);
      LiberateMemory((void **) &textlist);
      ThrowReaderException(CorruptImageWarning,"Corrupt XPM image file",image)
    }
  /*
    Read image pixels.
  */
  j=0;
  key[width]='\0';
  for (y=0; y < (long) image->rows; y++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    r=SetImagePixels(image,0,y,image->columns,1);
    if (r == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      (void) strncpy(key,p,width);
      if (strcmp(key,keys[j]) != 0)
        for (j=0; j < (long) Max(image->colors-1,1); j++)
          if (strcmp(key,keys[j]) == 0)
            break;
      if (image->storage_class == PseudoClass)
        indexes[x]=(IndexPacket) j;
      *r=image->colormap[j];
      r->opacity=j == (long) none ? TransparentOpacity : OpaqueOpacity;
      r++;
      p+=width;
    }
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Free resources.
  */
  for (i=0; i < (long) image->colors; i++)
    LiberateMemory((void **) &keys[i]);
  LiberateMemory((void **) &keys);
  for (i=0; textlist[i] != (char *) NULL; i++)
    LiberateMemory((void **) &textlist[i]);
  LiberateMemory((void **) &textlist);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X P M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXPMImage adds attributes for the XPM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXPMImage method is:
%
%      RegisterXPMImage(void)
%
*/
ModuleExport void RegisterXPMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PICON");
  entry->decoder=ReadXPMImage;
  entry->encoder=WritePICONImage;
  entry->adjoin=False;
  entry->description=AllocateString("Personal Icon");
  entry->module=AllocateString("PICON");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("PM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  entry->module=AllocateString("XPM");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("XPM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->magick=IsXPM;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  entry->module=AllocateString("XPM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X P M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXPMImage removes format registrations made by the
%  XPM module from the list of supported formats.
%
%  The format of the UnregisterXPMImage method is:
%
%      UnregisterXPMImage(void)
%
*/
ModuleExport void UnregisterXPMImage(void)
{
  (void) UnregisterMagickInfo("PM");
  (void) UnregisterMagickInfo("XPM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P I C O N I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WritePICONImage writes an image to a file in the Personal Icon
%  format.
%
%  The format of the WritePICONImage method is:
%
%      unsigned int WritePICONImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePICONImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePICONImage(const ImageInfo *image_info,Image *image)
{
#define ColormapExtent  155
#define GraymapExtent  95
#define PiconGeometry  "48x48>"

  static unsigned char
    Colormap[]=
    {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x06, 0x00, 0x05, 0x00, 0xf4, 0x05,
      0x00, 0x00, 0x00, 0x00, 0x2f, 0x4f, 0x4f, 0x70, 0x80, 0x90, 0x7e, 0x7e,
      0x7e, 0xdc, 0xdc, 0xdc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x80, 0x00, 0x00,
      0xff, 0x1e, 0x90, 0xff, 0x87, 0xce, 0xeb, 0xe6, 0xe6, 0xfa, 0x00, 0xff,
      0xff, 0x80, 0x00, 0x80, 0xb2, 0x22, 0x22, 0x2e, 0x8b, 0x57, 0x32, 0xcd,
      0x32, 0x00, 0xff, 0x00, 0x98, 0xfb, 0x98, 0xff, 0x00, 0xff, 0xff, 0x00,
      0x00, 0xff, 0x63, 0x47, 0xff, 0xa5, 0x00, 0xff, 0xd7, 0x00, 0xff, 0xff,
      0x00, 0xee, 0x82, 0xee, 0xa0, 0x52, 0x2d, 0xcd, 0x85, 0x3f, 0xd2, 0xb4,
      0x8c, 0xf5, 0xde, 0xb3, 0xff, 0xfa, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x21, 0xf9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
      0x00, 0x00, 0x06, 0x00, 0x05, 0x00, 0x00, 0x05, 0x18, 0x20, 0x10, 0x08,
      0x03, 0x51, 0x18, 0x07, 0x92, 0x28, 0x0b, 0xd3, 0x38, 0x0f, 0x14, 0x49,
      0x13, 0x55, 0x59, 0x17, 0x96, 0x69, 0x1b, 0xd7, 0x85, 0x00, 0x3b,
    },
    Graymap[]=
    {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x04, 0x00, 0x04, 0x00, 0xf3, 0x0f,
      0x00, 0x00, 0x00, 0x00, 0x12, 0x12, 0x12, 0x21, 0x21, 0x21, 0x33, 0x33,
      0x33, 0x45, 0x45, 0x45, 0x54, 0x54, 0x54, 0x66, 0x66, 0x66, 0x78, 0x78,
      0x78, 0x87, 0x87, 0x87, 0x99, 0x99, 0x99, 0xab, 0xab, 0xab, 0xba, 0xba,
      0xba, 0xcc, 0xcc, 0xcc, 0xde, 0xde, 0xde, 0xed, 0xed, 0xed, 0xff, 0xff,
      0xff, 0x21, 0xf9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
      0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x04, 0x0c, 0x10, 0x04, 0x31,
      0x48, 0x31, 0x07, 0x25, 0xb5, 0x58, 0x73, 0x4f, 0x04, 0x00, 0x3b,
    };

  Image
    *picon,
    *map;

  long
    x,
    y;

  unsigned int
    status;

  unsigned long
    height,
    width;

  width=image->columns;
  height=image->rows;
  (void) ParseImageGeometry(PiconGeometry,&x,&y,&width,&height);
  picon=ZoomImage(image,width,height,&image->exception);
  if (IsGrayImage(image))
    map=BlobToImage(image_info,Graymap,GraymapExtent,&image->exception);
  else
    map=BlobToImage(image_info,Colormap,ColormapExtent,&image->exception);
  if ((picon == (Image *) NULL) || (map == (Image *) NULL))
    return(False);
  status=MapImage(picon,map,image_info->dither);
  status|=WriteXPMImage(image_info,picon);
  DestroyImage(map);
  DestroyImage(picon);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X P M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXPMImage writes an image to a file in the X pixmap format.
%
%  The format of the WriteXPMImage method is:
%
%      unsigned int WriteXPMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXPMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteXPMImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    buffer[MaxTextExtent],
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  int
    j;

  long
    k,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    characters_per_pixel,
    status,
    transparent;

  unsigned long
    colors;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  transparent=False;
  if (image->storage_class == PseudoClass)
    {
      CompressColormap(image);
      if (image->matte)
         transparent=True;
    }
  else
    {
      QuantizeInfo
        quantize_info;

      /*
        Convert DirectClass to PseudoClass image.
      */
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
            {
              if (q->opacity == TransparentOpacity)
                transparent=True;
              else
                q->opacity=OpaqueOpacity;
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
    }
  colors=image->colors;
  if (transparent)
    {
      colors++;
      ReacquireMemory((void **) &image->colormap,colors*sizeof(PixelPacket));
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          if (q->opacity == TransparentOpacity)
            indexes[x]=image->colors;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (long) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) WriteBlobString(image,"/* XPM */\n");
  (void) WriteBlobString(image,"static char *magick[] = {\n");
  (void) WriteBlobString(image,"/* columns rows colors chars-per-pixel */\n");
  FormatString(buffer,"\"%lu %lu %lu %d\",\n",image->columns,
    image->rows,colors,characters_per_pixel);
  (void) WriteBlobString(image,buffer);
  for (i=0; i < (long) colors; i++)
  {
    /*
      Define XPM color.
    */
    image->colormap[i].opacity=OpaqueOpacity;
    (void) QueryColorname(image,image->colormap+i,X11Compliance,name);
    if (transparent)
      {
        if (i == (long) (colors-1))
          {
            if (LocaleCompare(image_info->magick,"PICON") == 0)
              (void) strcpy(name,"grey75");
            else
              (void) strcpy(name,"None");
          }
      }
    /*
      Write XPM color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    FormatString(buffer,"\"%.1024s c %.1024s\",\n",symbol,name);
    (void) WriteBlobString(image,buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) WriteBlobString(image,"/* pixels */\n");
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) WriteBlobString(image,"\"");
    for (x=0; x < (long) image->columns; x++)
    {
      k=indexes[x] % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) indexes[x]-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      FormatString(buffer,"%.1024s",symbol);
      (void) WriteBlobString(image,buffer);
    }
    FormatString(buffer,"\"%.1024s\n",
      (y == (long) (image->rows-1) ? "" : ","));
    (void) WriteBlobString(image,buffer);
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  (void) WriteBlobString(image,"};\n");
  CloseBlob(image);
  return(True);
}
