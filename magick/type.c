/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        TTTTT  Y   Y  PPPP   EEEEE                           %
%                          T     Y Y   P   P  E                               %
%                          T      Y    PPPP   EEE                             %
%                          T      Y    P      E                               %
%                          T      Y    P      EEEEE                           %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Type Methods                          %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
*/

/*
  Include declarations.
*/
#include "studio.h"

/*
  Define declarations.
*/

/*
  Declare type map.
*/
static char
  *TypeMap =
    "<?xml version=\"1.0\"?>"
    "<typemap>"
    "  <type stealth=\"True\" />"
    "</typemap>";

/*
  Static declarations.
*/
static SemaphoreInfo
  *type_semaphore = (SemaphoreInfo *) NULL;

static TypeInfo
  *type_list = (TypeInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadConfigurationFile(const char *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y T y p e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyTypeInfo deallocates memory associated with the font list.
%
%  The format of the DestroyTypeInfo method is:
%
%      DestroyTypeInfo(void)
%
%
*/
MagickExport void DestroyTypeInfo(void)
{
  register TypeInfo
    *p;

  TypeInfo
    *type_info;

  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (TypeInfo *) NULL; )
  {
    type_info=p;
    p=p->next;
    if (type_info->filename != (char *) NULL)
      LiberateMemory((void **) &type_info->filename);
    if (type_info->name != (char *) NULL)
      LiberateMemory((void **) &type_info->name);
    if (type_info->description != (char *) NULL)
      LiberateMemory((void **) &type_info->description);
    if (type_info->family != (char *) NULL)
      LiberateMemory((void **) &type_info->family);
    if (type_info->encoding != (char *) NULL)
      LiberateMemory((void **) &type_info->encoding);
    if (type_info->foundry != (char *) NULL)
      LiberateMemory((void **) &type_info->foundry);
    if (type_info->format != (char *) NULL)
      LiberateMemory((void **) &type_info->format);
    if (type_info->metrics != (char *) NULL)
      LiberateMemory((void **) &type_info->metrics);
    if (type_info->glyphs != (char *) NULL)
      LiberateMemory((void **) &type_info->glyphs);
    LiberateMemory((void **) &type_info);
  }
  type_list=(TypeInfo *) NULL;
  DestroySemaphoreInfo(&type_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T y p e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetTypeInfo searches the type list for the specified name and if
%  found returns attributes for that type.
%
%  The format of the GetTypeInfo method is:
%
%      const TypeInfo *GetTypeInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o type_info: Method GetTypeInfo searches the type list for the specified
%      name and if found returns attributes for that type.
%
%    o name: The type name.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const TypeInfo *GetTypeInfo(const char *name,
  ExceptionInfo *exception)
{
  register TypeInfo
    *p;

  AcquireSemaphoreInfo(&type_semaphore);
  if (type_list == (TypeInfo *) NULL)
    (void) ReadConfigurationFile(TypeFilename,exception);
  LiberateSemaphoreInfo(&type_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return(type_list);
  /*
    Search for requested type.
  */
  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
    if ((p->name != (char *) NULL) && (LocaleCompare(p->name,name) == 0))
      break;
  if (p == (TypeInfo *) NULL)
    ThrowException(exception,OptionWarning,"Unrecognized type",name);
  else
    if (p != type_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (TypeInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (TypeInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(TypeInfo *) NULL;
        p->next=type_list;
        type_list->previous=p;
        type_list=p;
      }
  LiberateSemaphoreInfo(&type_semaphore);
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T y p e I n f o B y F a m i l y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetTypeInfoByFamily searches the type list for the specified family
%  and if found returns attributes for that type.
%
%  Type substitution and scoring algorithm contributed by Bob Friesenhahn.
%
%  The format of the GetTypeInfoByFamily method is:
%
%      const TypeInfo *GetTypeInfoByFamily(const char *family,
%        const StyleType style,const StretchType stretch,
%        const unsigned long weight,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o type_info: Method GetTypeInfo searches the type list for the specified
%      name and if found returns attributes for that type.
%
%    o family: The type family.
%
%    o style: The type style.
%
%    o stretch: The type stretch.
%
%    o weight: The type weight.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const TypeInfo *GetTypeInfoByFamily(const char *family,
  const StyleType style,const StretchType stretch,const unsigned long weight,
  ExceptionInfo *exception)
{
  typedef struct _Fontmap
  {
    char
      *name,
      *substitute;
  } Fontmap;

  const TypeInfo
    *type_info;

  long
    range;

  register const TypeInfo
    *p;

  register long
    i;

  static Fontmap
    fontmap[] =
    {
      { "fixed", "courier" }, { "modern","courier" },
      { "monotype corsiva", "courier" }, { "news gothic", "helvetica" },
      { "system", "courier" }, { "terminal", "courier" },
      { "wingdings", "symbol" }, { (char *) NULL, (char *) NULL }
    };

  unsigned long
    max_score,
    score;

  /*
    Check for an exact type match.
  */
  (void) GetTypeInfo("*",exception);
  if (type_list == (TypeInfo *) NULL)
    return((TypeInfo *) NULL);
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
  {
    if (p->family == (char *) NULL)
      continue;
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(p->family,"arial") != 0) &&
            (LocaleCompare(p->family,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(p->family,family) != 0)
        continue;
    if ((style != AnyStyle) && (p->style != style))
      continue;
    if ((stretch != AnyStretch) && (p->stretch != stretch))
      continue;
    if ((weight != 0) && (p->weight != weight))
      continue;
    return(p);
  }
  /*
    Check for types in the same family.
  */
  max_score=0;
  type_info=(TypeInfo *) NULL;
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
  {
    if (p->family == (char *) NULL)
      continue;
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(p->family,"arial") != 0) &&
            (LocaleCompare(p->family,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(p->family,family) != 0)
        continue;
    score=0;
    if ((style == AnyStyle) || (p->style == style))
      score+=32;
    else
      if (((style == ItalicStyle) || (style == ObliqueStyle)) &&
          ((p->style == ItalicStyle) || (p->style == ObliqueStyle)))
        score+=25;
    if (weight == 0)
      score+=16;
    else
      score+=(16*(800-((long) Max(Min(weight,900),p->weight)-
        (long) Min(Min(weight,900),p->weight))))/800;
    if (stretch == AnyStretch)
      score+=8;
    else
      {
        range=(long) UltraExpandedStretch-(long) NormalStretch;
        score+=(8*(range-((long) Max(stretch,p->stretch)-
          (long) Min(stretch,p->stretch))))/range;
      }
    if (score > max_score)
      {
        max_score=score;
        type_info=p;
      }
  }
  if (type_info != (TypeInfo *) NULL)
    return(type_info);
  /*
    Check for table-based substitution match.
  */
  for (i=0; fontmap[i].name != (char *) NULL; i++)
  {
    if (family == (const char *) NULL)
      {
        if ((LocaleCompare(fontmap[i].name,"arial") != 0) &&
            (LocaleCompare(fontmap[i].name,"helvetica") != 0))
          continue;
      }
    else
      if (LocaleCompare(fontmap[i].name,family) != 0)
        continue;
    type_info=GetTypeInfoByFamily(fontmap[i].substitute,style,stretch,weight,
      exception);
    break;
  }
  if (type_info != (TypeInfo *) NULL)
    {
      ThrowException(exception,TypeError,"Font substitution required",
        type_info->family);
      return(type_info);
    }
  if (family != (const char *) NULL)
    type_info=GetTypeInfoByFamily((const char *) NULL,style,stretch,weight,
      exception);
  return(type_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t T y p e I n f o                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListTypeInfo lists the fonts to a file.
%
%  The format of the ListTypeInfo method is:
%
%      unsigned int ListTypeInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListTypeInfo(FILE *file,ExceptionInfo *exception)
{
  register long
    i;

  register const TypeInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) GetTypeInfo("*",exception);
  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (const TypeInfo *) NULL; p=p->next)
  {
    if ((p->previous == (TypeInfo *) NULL) ||
        (LocaleCompare(p->filename,p->previous->filename) != 0))
      {
        if (p->previous != (TypeInfo *) NULL)
          (void) fprintf(file,"\n");
        (void) fprintf(file,"Filename: %.1024s\n\n",p->filename);
        (void) fprintf(file,"Name                             Description\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 32; i++)
      (void) fprintf(file," ");
    if (p->description != (char *) NULL)
      (void) fprintf(file," %.1024s",p->description);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&type_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r a t i o n F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *basename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigurationFile(const char *basename,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    keyword[MaxTextExtent],
    *path,
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the type configuration file.
  */
  FormatString(filename,"%.1024s",basename);
  path=GetMagickConfigurePath(filename,exception);
  if (path != (char *) NULL)
    {
      FormatString(filename,"%.1024s",path);
      LiberateMemory((void **) &path);
    }
  xml=(char *) FileToBlob(filename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(TypeMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              (void) ReadConfigurationFile(token,exception);
              while (type_list->next != (TypeInfo *) NULL)
                type_list=type_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<type") == 0)
      {
        TypeInfo
          *type_info;

        /*
          Allocate memory for the type list.
        */
        type_info=(TypeInfo *) AcquireMemory(sizeof(TypeInfo));
        if (type_info == (TypeInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"Unable to allocate fonts",
            "Memory allocation failed");
        (void) memset(type_info,0,sizeof(TypeInfo));
        type_info->filename=AcquireString(filename);
        type_info->signature=MagickSignature;
        if (type_list == (TypeInfo *) NULL)
          {
            type_list=type_info;
            continue;
          }
        type_list->next=type_info;
        type_info->previous=type_list;
        type_list=type_list->next;
        continue;
      }
    if (type_list == (TypeInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'E':
      case 'e':
      {
        if (LocaleCompare((char *) keyword,"encoding") == 0)
          {
            type_list->encoding=AcquireString(token);
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"family") == 0)
          {
            type_list->family=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            type_list->format=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"foundry") == 0)
          {
            type_list->foundry=AcquireString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"fullname") == 0)
          {
            type_list->description=AcquireString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"glyphs") == 0)
          {
            type_list->glyphs=AcquireString(token);
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"metrics") == 0)
          {
            type_list->metrics=AcquireString(token);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            type_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            type_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        if (LocaleCompare((char *) keyword,"stretch") == 0)
          {
            if (LocaleCompare(token,"condensed") == 0)
              type_list->stretch=CondensedStretch;
            if (LocaleCompare(token,"expanded") == 0)
              type_list->stretch=ExpandedStretch;
            if (LocaleCompare(token,"extra-condensed") == 0)
              type_list->stretch=ExtraCondensedStretch;
            if (LocaleCompare(token,"extra-expanded") == 0)
              type_list->stretch=ExtraExpandedStretch;
            if (LocaleCompare(token,"normal") == 0)
              type_list->stretch=NormalStretch;
            if (LocaleCompare(token,"semi-condensed") == 0)
              type_list->stretch=SemiCondensedStretch;
            if (LocaleCompare(token,"semi-expanded") == 0)
              type_list->stretch=SemiExpandedStretch;
            if (LocaleCompare(token,"ultra-condensed") == 0)
              type_list->stretch=UltraCondensedStretch;
            if (LocaleCompare(token,"ultra-expanded") == 0)
              type_list->stretch=UltraExpandedStretch;
            break;
          }
        if (LocaleCompare((char *) keyword,"style") == 0)
          {
            if (LocaleCompare(token,"italic") == 0)
              type_list->style=ItalicStyle;
            if (LocaleCompare(token,"normal") == 0)
              type_list->style=NormalStyle;
            if (LocaleCompare(token,"oblique") == 0)
              type_list->style=ObliqueStyle;
            break;
          }
        break;
      }
      case 'W':
      case 'w':
      {
        if (LocaleCompare((char *) keyword,"weight") == 0)
          {
            type_list->weight=atol(token);
            if (LocaleCompare(token,"bold") == 0)
              type_list->weight=700;
            if (LocaleCompare(token,"normal") == 0)
              type_list->weight=400;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  LiberateMemory((void **) &xml);
  if (type_list == (TypeInfo *) NULL)
    return(False);
  while (type_list->previous != (TypeInfo *) NULL)
    type_list=type_list->previous;
  return(True);
}
