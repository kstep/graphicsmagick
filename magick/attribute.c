/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%        AAA   TTTTT  TTTTT  RRRR   IIIII  BBBB   U   U  TTTTT  EEEEE         %
%       A   A    T      T    R   R    I    B   B  U   U    T    E             %
%       AAAAA    T      T    RRRR     I    BBBB   U   U    T    EEE           %
%       A   A    T      T    R R      I    B   B  U   U    T    E             %
%       A   A    T      T    R  R   IIIII  BBBB    UUU     T    EEEEE         %
%                                                                             %
%                                                                             %
%              Methods to Get/Set/Destroy Image Text Attributes               %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              February 2000                                  %
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
%  The Attributes methods gets, sets, or destroys attributes associated
%  with a particular image (e.g. comments, copyright, author, etc).
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e A t t r i b u t e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImageAttributes deallocates memory associated with the image
%  attribute list.
%
%  The format of the DestroyImageAttributes method is:
%
%      DestroyImageAttributes(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImageAttributes(Image *image)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (p=image->attributes; p != (ImageAttribute *) NULL; )
  {
    attribute=p;
    p=p->next;
    if (attribute->key != (char *) NULL)
      LiberateMemory((void **) &attribute->key);
    if (attribute->value != (char *) NULL)
      LiberateMemory((void **) &attribute->value);
    LiberateMemory((void **) &attribute);
  }
  image->attributes=(ImageAttribute *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageAttribute searches the list of image attributes and returns
%  a pointer to attribute if it exists otherwise NULL.
%
%  The format of the GetImageAttribute method is:
%
%      ImageAttribute *GetImageAttribute(const Image *image,const char *key)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageAttribute returns the attribute if it
%      exists otherwise NULL.
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
%
*/

static int GenerateIPTCAttribute(Image *image,const char *key)
{
  char
    *attribute;

  int
    count,
    dataset,
    record;

  register int
    i;

  unsigned int
    length;

  if (image->iptc_profile.length == 0)
    return(False);
  count=sscanf(key,"IPTC:%d:%d",&dataset,&record);
  if (count != 2)
    return(False);
  for (i=0; i < (int) image->iptc_profile.length; i++)
  {
    if (image->iptc_profile.info[i] != 0x1c)
      continue;
    if (image->iptc_profile.info[i+1] != dataset)
      continue;
    if (image->iptc_profile.info[i+2] != record)
      continue;
    length=image->iptc_profile.info[i+3] << 8;
    length|=image->iptc_profile.info[i+4];
    attribute=(char *) AcquireMemory(length+MaxTextExtent);
    if (attribute == (char *) NULL)
      continue;
    (void) strncpy(attribute,(char *) image->iptc_profile.info+i+5,length);
    attribute[length]='\0';
    SetImageAttribute(image,key,(const char *) attribute);
    LiberateMemory((void **) &attribute);
    break;
  }
  return(i < (int) image->iptc_profile.length);
}

static unsigned char ReadByte(char **p,unsigned int *length)
{
  unsigned char
    c;

  if (*length < 1)
    return(0xff);
  c=(*(*p)++);
  (*length)--;
  return(c);
}

static long ReadMSBLong(char **p,unsigned int *length)
{
  int
    c;

  long
    value;

  register int
    i;

  unsigned char
    buffer[4];

  if (*length < 4)
    return(-1);
  for (i=0; i < 4; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=c;
  }
  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value);
}

static int ReadMSBShort(char **p,unsigned int *length)
{
  int
    c,
    value;

  register int
    i;

  unsigned char
    buffer[2];

  if (*length < 2)
    return(-1);
  for (i=0; i < 2; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=c;
  }
  value=buffer[0] << 8;
  value|=buffer[1];
  return(value);
}

static char *GenerateClippingPath(char *blob,unsigned int length,
  unsigned int columns,unsigned int rows)
{
  char
    *path,
    *message;

  int
    count,
    selector;
  
  long
    x,
    y;

  PointInfo
    first[3],
    last[3],
    point[3];

  register int
    i;

  unsigned int
    status;

  path=AllocateString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AllocateString((char *) NULL);
  while (length > 0)
  {
    selector=ReadMSBShort(&blob,&length);
    if (selector != 6)
      {
        /*
          Path fill record.
        */
        blob+=24;
        length-=24;
        continue;
      }
    blob+=24;
    length-=24;
    FormatString(message,"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
    ConcatenateString(&path,message);
    FormatString(message,"<svg width=\"%d\" height=\"%d\">\n",columns,rows);
    ConcatenateString(&path,message);
    FormatString(message,"<g>\n");
    ConcatenateString(&path,message);
    FormatString(message,"<path style=\"fill:#ffffff;stroke:none\" d=\"\n");
    ConcatenateString(&path,message);
    while (length > 0)
    {
      selector=ReadMSBShort(&blob,&length);
      if ((selector != 0) && (selector != 3))
        break;
      count=ReadMSBShort(&blob,&length);
      blob+=22;
      length-=22;
      status=True;
      while (count > 0)
      {
        selector=ReadMSBShort(&blob,&length);
        if ((selector == 1) || (selector == 2) || (selector == 4) ||
            (selector == 5))
          {
            for (i=0; i < 3; i++)
            {
              y=ReadMSBLong(&blob,&length);
              x=ReadMSBLong(&blob,&length);
              point[i].y=((double) y*(double) rows)/16777216.0;
              point[i].x=((double) x*(double) columns)/16777216.0;
            }
            if (status)
              {
                FormatString(message,"M %.1f,%.1f\n",point[1].x,point[1].y);
                for (i=0; i < 3; i++)
                {
                  first[i]=point[i];
                  last[i]=point[i];
                }
              }
            else
              {
                FormatString(message,"C %.1f,%.1f %.1f,%.1f %.1f,%.1f\n",
                  last[2].x,last[2].y,point[0].x,point[0].y,point[1].x,
                  point[1].y);
                for (i=0; i < 3; i++)
                  last[i]=point[i];
              }
          ConcatenateString(&path,message);
          status=False;
          count--;
        }
      }
      FormatString(message,"C %.1f,%.1f %.1f,%.1f %.1f,%.1f Z\n",last[2].x,
        last[2].y,first[0].x,first[0].y,first[1].x,first[1].y);
      ConcatenateString(&path,message);
    }
    FormatString(message,"\"/>\n");
    ConcatenateString(&path,message);
    FormatString(message,"</g>\n");
    ConcatenateString(&path,message);
    FormatString(message,"</svg>\n");
    ConcatenateString(&path,message);
    break;
  }
  LiberateMemory((void **) &message);
  return(path);
}

static int Generate8BIMAttribute(Image *image,const char *key)
{
  char
    *attribute,
    *string;

  int
    i,
    id,
    start,
    stop;

  long
    count;

  unsigned char
    c,
    *info;

  unsigned int
    length,
    status;

  if (image->iptc_profile.length == 0)
    return(False);
  count=sscanf(key,"8BIM:%d,%d",&start,&stop);
  if (count != 2)
    return(False);
  status=False;
  length=image->iptc_profile.length;
  info=image->iptc_profile.info;
  while (length > 0)
  {
    if (ReadByte((char **) &info,&length) != '8')
      continue;
    if (ReadByte((char **) &info,&length) != 'B')
      continue;
    if (ReadByte((char **) &info,&length) != 'I')
      continue;
    if (ReadByte((char **) &info,&length) != 'M')
      continue;
    id=ReadMSBShort((char **) &info,&length);
    if (id < start)
      continue;
    if (id > stop)
      continue;
    count=ReadByte((char **) &info,&length);
    string=(char *) NULL;
    if ((count > 0) && (count <= (int) length))
      {
        string=(char *) AcquireMemory(count+MaxTextExtent);
        if (string != (char *) NULL)
          {
            for (i=0; i < count; i++)
              string[i]=(char) ReadByte((char **) &info,&length);
            string[count]=0;
            LiberateMemory((void **) &string);
          }
      }
    if (!(count & 0x01))
      c=ReadByte((char **) &info,&length);
    count=ReadMSBLong((char **) &info,&length);
    attribute=(char *) AcquireMemory(count+MaxTextExtent);
    if (attribute != (char *) NULL)
      {
        (void) memcpy(attribute,(char *) info,count);
        attribute[count]='\0';
        info+=count;
        length-=count;
        if ((id <= 1999) || (id >= 2999))
          SetImageAttribute(image,key,(const char *) attribute);
        else
          {
            char
              *path;

            path=GenerateClippingPath(attribute,count,image->columns,
              image->rows);
            SetImageAttribute(image,key,(const char *) path);
            LiberateMemory((void **) &path);
          }
        LiberateMemory((void **) &attribute);
        status=True;
      }
  }
  return(status);
}

MagickExport ImageAttribute *GetImageAttribute(const Image *image,
  const char *key)
{
  register ImageAttribute
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (key == (char *) NULL)
    return(image->attributes);
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
    if (LocaleCompare(key,p->key) == 0)
      return(p);
  if (LocaleNCompare("IPTC:",key,5) == 0)
    {
      if (GenerateIPTCAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  if (LocaleNCompare("8BIM:",key,5) == 0)
    {
      if (Generate8BIMAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o A t t r i b u t e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageInfoAttribute returns a "fake" attribute based on data in the
%  image info or image structures.
%
%  The format of the GetImageInfoAttribute method is:
%
%      ImageAttribute *GetImageAttribute(const Image *image,const char *key)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageInfoAttribute returns the attribute if it
%      exists otherwise NULL.
%
%    o image_info: The imageInfo.
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
*/
ImageAttribute *GetImageInfoAttribute(const ImageInfo *image_info,
  Image *image,const char *key)
{
  char
    attribute[MaxTextExtent],
    filename[MaxTextExtent];

  attribute[0]='\0';
  switch(*(key))
  {
    case 'b':
    {
      if (LocaleNCompare("base",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,BasePath,filename);
          (void) strcpy(attribute,filename);
          break;
        }
      break;
    }
    case 'd':
    {
      if (LocaleNCompare("depth",key,2) == 0)
        {
          FormatString(attribute,"%u",image->depth);
          break;
        }
      if (LocaleNCompare("directory",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,HeadPath,filename);
          (void) strcpy(attribute,filename);
          break;
        }
      break;
    }
    case 'e':
    {
      if (LocaleNCompare("extension",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,ExtensionPath,filename);
          (void) strcpy(attribute,filename);
          break;
        }
      break;
    }
    case 'g':
    {
      if (LocaleNCompare("group",key,2) == 0)
        {
          FormatString(attribute,"0x%lx",image_info->group);
          break;
        }
      break;
    }
    case 'h':
    {
      if (LocaleNCompare("height",key,2) == 0)
        {
          FormatString(attribute,"%u",
            image->magick_rows ? image->magick_rows : 256);
          break;
        }
      break;
    }
    case 'i':
    {
      if (LocaleNCompare("input",key,2) == 0)
        {
          (void) strcpy(attribute,image->filename);
          break;
        }
      break;
    }
    case 'm':
    {
      if (LocaleNCompare("magick",key,2) == 0)
        {
          (void) strcpy(attribute,image->magick);
          break;
        }
      break;
    }
    case 'n':
    {
      if (LocaleNCompare("name",key,2) == 0)
        {
          (void) strcpy(attribute,filename);
          break;
        }
     break;
    }
    case 's':
    {
      if (LocaleNCompare("size",key,2) == 0)
        {
          if (SizeBlob(image) >= (1 << 24))
            FormatString(attribute,"%lumb",
              (unsigned long) (SizeBlob(image)/1024/1024));
          else
            if (SizeBlob(image) >= (1 << 16))
              FormatString(attribute,"%lukb",
                (unsigned long) (SizeBlob(image)/1024));
            else
              FormatString(attribute,"%lu",
                (unsigned long) SizeBlob(image));
          break;
        }
      if (LocaleNCompare("scene",key,2) == 0)
        {
          FormatString(attribute,"%u",image->scene);
          if (image_info->subrange != 0)
            FormatString(attribute,"%u",image_info->subimage);
          break;
        }
      if (LocaleNCompare("scenes",key,6) == 0)
        {
          FormatString(attribute,"%u",GetNumberScenes(image));
          break;
        }
       break;
    }
    case 'o':
    {
      if (LocaleNCompare("output",key,2) == 0)
        {
          (void) strcpy(attribute,image_info->filename);
          break;
        }
     break;
    }
    case 'p':
    {
      if (LocaleNCompare("page",key,2) == 0)
        {
          register const Image
            *p;

          unsigned int
            page;

          p=image;
          for (page=1; p->previous != (Image *) NULL; page++)
            p=p->previous;
          FormatString(attribute,"%u",page);
          break;
        }
      break;
    }
    case 'u':
    {
      if (LocaleNCompare("unique",key,2) == 0)
        {
          (void) strcpy(attribute,image_info->unique);
          break;
        }
      break;
    }
    case 'w':
    {
      if (LocaleNCompare("width",key,2) == 0)
        {
          FormatString(attribute,"%u",
            image->magick_columns ? image->magick_columns : 256);
          break;
        }
      break;
    }
    case 'x':
    {
      if (LocaleNCompare("xresolution",key,2) == 0)
        {
          FormatString(attribute,"%u",(unsigned int) image->x_resolution);
          break;
        }
      break;
    }
    case 'y':
    {
      if (LocaleNCompare("yresolution",key,2) == 0)
        {
          FormatString(attribute,"%u",(unsigned int) image->y_resolution);
          break;
        }
      break;
    }
    case 'z':
    {
      if (LocaleNCompare("zero",key,2) == 0)
        {
          (void) strcpy(attribute,image_info->zero);
          break;
        }
      break;
    }
  }
  if (Extent(image->magick_filename) > 0)
    {
      SetImageAttribute(image,key,(const char *) attribute);
      return(GetImageAttribute(image,key));
    }
  return((ImageAttribute *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageAttribute searches the list of image attributes and replaces
%  the attribute value.  If it is not found in the list, the attribute name
%  and value is added to the list.   If the attribute exists in the list,
%  the value is concatenated to the attribute.  SetImageAttribute returns True
%  if the attribute is successfully concatenated or added to the list,
%  otherwise False.  If the value is NULL, the matching key is deleted from
%  the list.
%
%  The format of the SetImageAttribute method is:
%
%      unsigned int SetImageAttribute(Image *image,const char *key,
%        const char *value)
%
%  A description of each parameter follows:
%
%    o status:  Method SetImageAttribute returns True if the attribute is
%      successfully replaced or added to the list, otherwise False.
%
%    o image: The image.
%
%    o key,value:  These character strings are the name and value of an image
%      attribute to replace or add to the list.
%
%
*/
MagickExport unsigned int SetImageAttribute(Image *image,const char *key,
  const char *value)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  /*
    Initialize new attribute.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((key == (const char *) NULL) || (*key == '\0'))
    return(False);
  if (value == (const char *) NULL)
    {
      /*
        Delete attribute from the image attributes list.
      */
      for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
        if (LocaleCompare(key,p->key) == 0)
          break;
      if (p == (ImageAttribute *) NULL)
        return(False);
      if (p->key != (char *) NULL)
        LiberateMemory((void **) &p->key);
      if (p->value != (char *) NULL)
        LiberateMemory((void **) &p->value);
      if (p->previous != (ImageAttribute *) NULL)
        p->previous->next=p->next;
      else
        {
          image->attributes=p->next;
          if (p->next != (ImageAttribute *) NULL)
            p->next->previous=(ImageAttribute *) NULL;
        }
      if (p->next != (ImageAttribute *) NULL)
        p->next->previous=p->previous;
      attribute=p;
      LiberateMemory((void **) &attribute);
      return(True);
    }
  if (*value == '\0')
    return(False);
  attribute=(ImageAttribute *) AcquireMemory(sizeof(ImageAttribute));
  if (attribute == (ImageAttribute *) NULL)
    return(False);
  attribute->key=AllocateString(key);
  if ((LocaleNCompare(key,"IPTC",4) == 0) ||
      (LocaleNCompare(key,"8BIM",4) == 0))
    attribute->value=AllocateString(value);
  else
    attribute->value=TranslateText((ImageInfo *) NULL,image,value);
  attribute->compression=False;
  attribute->previous=(ImageAttribute *) NULL;
  attribute->next=(ImageAttribute *) NULL;
  if (image->attributes == (ImageAttribute *) NULL)
    {
      image->attributes=attribute;
      return(True);
    }
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
  {
    if (LocaleCompare(attribute->key,p->key) == 0)
      {
        ConcatenateString(&p->value,attribute->value);
        LiberateMemory((void **) &attribute->value);
        LiberateMemory((void **) &attribute->key);
        return(True);
      }
    if (p->next == (ImageAttribute *) NULL)
      break;
  }
  /*
    Place new attribute at the end of the attribute list.
  */
  attribute->previous=p;
  p->next=attribute;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t o r e I m a g e A t t r i b u t e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StoreImageAttribute is used to store an image attribute from a
%  text string with the syntax: NAME=VALUE.
%
%  The format of the StoreImageAttribute method is:
%
%      StoreImageAttribute(Image *image,char *text)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o text: The text string that is parsed and used to determine the name
%      and value of the new attribute.
%
%
*/
void StoreImageAttribute(Image *image,char *text)
{
  char
    breaker,
    *key,
    quote,
    *token,
    *value;

  int
    next,
    state,
    status;

  TokenInfo
    token_info;

  next=0;
  state=0;
  token=AllocateString((char *) NULL);
  for ( ; ; )
  {
    status=Tokenizer(&token_info,0,token,MaxTextExtent,text,(char *) "",
      (char *) "=",(char *) "\'",0,&breaker,&next,&quote);
    if (status != False)
      break;
    switch (state)
    {
      case 0:
      {
        key=AllocateString(token);
        break;
      }
      case 1:
      {
        value=AllocateString(token);
        break;
      }
    }
    state++;
  }
  if ((state > 1) && (key != (char *) NULL) && (value != (char *) NULL))
    SetImageAttribute(image,key,value);
  if (token != (char *) NULL)
    LiberateMemory((void **) &token);
  if (key != (char *) NULL)
    LiberateMemory((void **) &key);
  if (value != (char *) NULL)
    LiberateMemory((void **) &value);
  return;
}
