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
%      unsigned int IsXPM(const unsigned char *magick,
%        const unsigned int length)
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
static unsigned int IsXPM(const unsigned char *magick,const unsigned int length)
{
  if (length < 9)
    return(False);
  if (strncmp((char *) magick,"/* XPM */",9) == 0)
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
    count,
    j,
    none,
    y;

  register char
    *p,
    *q;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *r;

  unsigned int
    length,
    status,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read XPM file.
  */
  length=MaxTextExtent;
  xpm_buffer=(char *) AllocateMemory(length);
  p=xpm_buffer;
  if (xpm_buffer != (char *) NULL)
    while (GetStringBlob(image,p) != (char *) NULL)
    {
      if (*p == '#')
        if ((p == xpm_buffer) || (*(p-1) == '\n'))
          continue;
      if ((*p == '}') && (*(p+1) == ';'))
        break;
      p+=Extent(p);
      if ((p-xpm_buffer+MaxTextExtent+1) < length)
        continue;
      length<<=1;
      xpm_buffer=(char *) ReallocateMemory((char *) xpm_buffer,length);
      if (xpm_buffer == (char *) NULL)
        break;
      p=xpm_buffer+Extent(xpm_buffer);
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
  FreeMemory((void *) &xpm_buffer);
  if (textlist == (char **) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read hints.
  */
  image->class=PseudoClass;
  count=sscanf(textlist[0],"%u %u %u %u",&image->columns,&image->rows,
    &image->colors,&width);
  if ((count != 4) || (width > 2) ||
      ((image->columns*image->rows*image->colors) == 0))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(CorruptImageWarning,"Not a XPM image file",image);
    }
  image->depth=8;
  /*
    Initialize image structure.
  */
  image->colormap=(PixelPacket *)
    AllocateMemory(image->colors*sizeof(PixelPacket));
  keys=(char **) AllocateMemory(image->colors*sizeof(char *));
  if ((image->colormap == (PixelPacket *) NULL) || (keys == (char **) NULL))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    }
  /*
    Read image colormap.
  */
  i=1;
  none=(-1);
  for (x=0; x < (int) image->colors; x++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    keys[x]=(char *) AllocateMemory(width+1);
    if (keys[x] == (char *) NULL)
      {
        for (i=0; textlist[i] != (char *) NULL; i++)
          FreeMemory((void *) &textlist[i]);
        FreeMemory((void *) &textlist);
        FreeMemory((void *) &keys);
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      }
    keys[x][width]='\0';
    (void) strncpy(keys[x],p,width);
    /*
      Parse color.
    */
    (void) strcpy(target,"gray");
    q=ParseColor(p+width);
    if (q != (char *) NULL)
      {
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
        (void) strcpy(target,q);
        q=ParseColor(target);
        if (q != (char *) NULL)
          *q='\0';
      }
    Strip(target);
    if (Latin1Compare(target,"none") == 0)
      {
        image->class=DirectClass;
        image->matte=True;
        none=x;
        (void) strcpy(target,"black");
      }
    (void) QueryColorDatabase(target,&image->colormap[x]);
  }
  if (x < (int) image->colors)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(CorruptImageWarning,"Corrupt XPM image file",image);
    }
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  j=0;
  key[width]='\0';
  for (y=0; y < (int) image->rows; y++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    r=SetImagePixels(image,0,y,image->columns,1);
    if (r == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
    {
      (void) strncpy(key,p,width);
      if (strcmp(key,keys[j]) != 0)
        for (j=0; j < (int) (image->colors-1); j++)
          if (strcmp(key,keys[j]) == 0)
            break;
      if (image->class == PseudoClass)
        indexes[x]=j;
      *r=image->colormap[j];
      r->opacity=j == none ? Transparent : Opaque;
      r++;
      p+=width;
    }
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Free resources.
  */
  for (x=0; x < (int) image->colors; x++)
    FreeMemory((void *) &keys[x]);
  FreeMemory((void *) &keys);
  for (i=0; textlist[i] != (char *) NULL; i++)
    FreeMemory((void *) &textlist[i]);
  FreeMemory((void *) &textlist);
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
Export void RegisterXPMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("XPM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->magick=IsXPM;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  RegisterMagickInfo(entry);
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
Export void UnregisterXPMImage(void)
{
  UnregisterMagickInfo("PM");
  UnregisterMagickInfo("XPM");
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

  double
    distance,
    distance_squared,
    min_distance;

  int
    j,
    k,
    y;

  long
    mean;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p;

  register const ColorlistInfo
    *q;

  unsigned int
    characters_per_pixel,
    colors,
    status,
    transparent;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  transparent=False;
  if (image->class == PseudoClass)
    colors=image->colors;
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
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              p->opacity=p->opacity == Transparent;
              if (p->opacity == Transparent)
                transparent=True;
              p++;
            }
            if (!SyncImagePixels(image))
              break;
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      colors=image->colors;
      if (transparent)
        {
          colors++;
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (int) image->columns; x++)
            {
              if (p->opacity)
                indexes[x]=image->colors;
              p++;
            }
            if (!SyncImagePixels(image))
              break;
          }
        }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (int) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) strcpy(buffer,"/* XPM */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"static char *magick[] = {\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"/* columns rows colors chars-per-pixel */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  FormatString(buffer,"\"%u %u %u %d\",\n",image->columns,
    image->rows,colors,characters_per_pixel);
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (i=0; i < (int) colors; i++)
  {
    PixelPacket
      *p;

    /*
      Define XPM color.
    */
    min_distance=0;
    p=image->colormap+i;
    FormatString(name,HexColorFormat,(unsigned int) p->red,
      (unsigned int) p->green,(unsigned int) p->blue);
    for (q=XPMColorlist; q->name != (char *) NULL; q++)
    {
      mean=(p->red+(int) q->red)/2;
      distance=p->red-(int) q->red;
      distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
      distance=p->green-(int) q->green;
      distance_squared+=4.0*(distance*distance);
      distance=p->blue-(int) q->blue;
      distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
      if ((q == XPMColorlist) || (distance_squared <= min_distance))
        {
          min_distance=distance_squared;
          if (min_distance == 0.0)
            (void) strcpy(name,q->name);
        }
    }
    if (transparent)
      if (i == (int) (colors-1))
        (void) strcpy(name,"None");
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
    (void) WriteBlob(image,strlen(buffer),buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) strcpy(buffer,"/* pixels */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) strcpy(buffer,"\"");
    (void) WriteBlob(image,strlen(buffer),buffer);
    for (x=0; x < (int) image->columns; x++)
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
      (void) WriteBlob(image,strlen(buffer),buffer);
      p++;
    }
    FormatString(buffer,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? "" : ","));
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SaveImageText,y,image->rows);
  }
  (void) strcpy(buffer,"};\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  CloseBlob(image);
  return(True);
}
