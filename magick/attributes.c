/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     AAA   TTTTT  TTTTT  RRRR   IIIII  BBBB   U   U  TTTTT  EEEEE  SSSSS     %
%    A   A    T      T    R   R    I    B   B  U   U    T    E      SS        %
%    AAAAA    T      T    RRRR     I    BBBB   U   U    T    EEE     SSS      %
%    A   A    T      T    R R      I    B   B  U   U    T    E         SS     %
%    A   A    T      T    R  R   IIIII  BBBB    UUU     T    EEEEE  SSSSS     %
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
%  The Attributes methods gets, sets, or destroys attributes associated
%  with a particular image (e.g. comments, copyright, author, etc).
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

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
%    o image: The address of a structure of type Image.
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
%   G e t I m a g e A t t r i b u t e s                                       %
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
%    o image: The address of a structure of type Image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
%
*/

static void GenerateIPTCAttribute(Image *image,const char *key)
{
  char
    *attribute;

  int
    count,
    dataset,
    record;

  register int
    i;

  unsigned short
    length;

  if (image->iptc_profile.length == 0)
    return;
  count=sscanf(key,"IPTC:%d:%d",&dataset,&record);
  if (count != 2)
    return;
  for (i=0; i < image->iptc_profile.length; i++)
  {
    if (image->iptc_profile.info[i] != 0x1c)
      continue;
    if (image->iptc_profile.info[i+1] != dataset)
      continue;
    if (image->iptc_profile.info[i+2] != record)
      continue;
    length=image->iptc_profile.info[i+3] << 8;
    length|=image->iptc_profile.info[i+4];
    attribute=(char *) AcquireMemory(length+1);
    if (attribute == (char *) NULL)
      continue;
    (void) strncpy(attribute,(char *) image->iptc_profile.info+i+5,length);
    attribute[length]='\0';
    SetImageAttribute(image,key,(const char *) attribute);
    LiberateMemory((void **) &attribute);
  }
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
      GenerateIPTCAttribute((Image *) image, key);
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
%    o image_info: The address of a structure of type ImageInfo.
%
%    o image: The address of a structure of type Image.
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
    directory[MaxTextExtent],
    *extension,
    *filename;

  attribute[0]='\0';
  if (Extent(image->magick_filename) <= 0)
    {
      directory[0]='\0';
      extension=directory;
      filename=extension;
    }
  else
    {
      (void) strcpy(directory,image->magick_filename);
      extension=directory+Extent(directory);
      filename=extension;
      while ((filename > directory) && !IsBasenameSeparator(*(filename-1)))
      {
        if (*filename == '.')
          if (*extension == '\0')
            extension=filename+1;
        filename--;
      }
    }
  switch(*(key))
  {
    case 'b':
    {
      if (LocaleNCompare("base",key,2) == 0)
        {
          *(extension-1)='\0';
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
          *filename='\0';
          (void) strcpy(attribute,directory);
          break;
        }
      break;
    }
    case 'e':
    {
      if (LocaleNCompare("extension",key,2) == 0)
        {
          (void) strcpy(attribute,extension);
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
          if (image->filesize >= (1 << 24))
            FormatString(attribute,"%ldmb",image->filesize/1024/1024);
          else
            if (image->filesize >= (1 << 14))
              FormatString(attribute,"%ldkb",image->filesize/1024);
            else
              FormatString(attribute,"%ldb",image->filesize);
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
%   S e t I m a g e A t t r i b u t e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageAttribute searches the list of image attributes and replaces
%  the attribute value.  If it is not found in the list, the attribute name
%  and value is added to the list. SetImageAttribute returns True if the
%  attribute is successfully replaced or added to the list, otherwise False.
%  If the value is NULL, the matching key is deleted from the list.
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
%    o image: The address of a structure of type Image.
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
  if (LocaleCompare(key,"Comment") == 0)
    while (SetImageAttribute(image,"Comment",(char *) NULL) != False);
  if (LocaleCompare(key,"Label") == 0)
    while (SetImageAttribute(image,"Label",(char *) NULL) != False);
  if (LocaleCompare(key,"Signature") == 0)
    while (SetImageAttribute(image,"Signature",(char *) NULL) != False);
  attribute=(ImageAttribute *) AcquireMemory(sizeof(ImageAttribute));
  if (attribute == (ImageAttribute *) NULL)
    return(False);
  attribute->key=AllocateString(key);
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
      break;
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
%    o image: The address of a structure of type Image.
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
    quoted,
    *token,
    *value;

  int
    length,
    next,
    state,
    status;

  TokenInfo
    token_info;

  length=MaxTextExtent;
  next=0;
  state=0;
  token=(char *) AcquireMemory(length);
  if (token == (char *) NULL)
    MagickError(ResourceLimitError,"Unable to parse attribute",
      "Memory allocation failed");
  for ( ; ; )
  {
    status=Tokenizer(&token_info,0,token,length,text,"","=","\'",0,&breaker,
      &next,&quoted);
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
