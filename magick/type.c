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
  Define declarations.
*/
#define TypeFilename  "type.mgk"

/*
  Declare type map.
*/
static char
  *TypeMap =
    "<?xml version=\"1.0\"?>"
    "<typemap>"
    "  <type />"
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

  AcquireSemaphoreInfo(&type_semaphore);
  for (p=type_list; p != (TypeInfo *) NULL; )
  {
    if (p->filename != (char *) NULL)
      LiberateMemory((void **) &p->filename);
    if (p->name != (char *) NULL)
      LiberateMemory((void **) &p->name);
    if (p->family != (char *) NULL)
      LiberateMemory((void **) &p->family);
    if (p->alias != (char *) NULL)
      LiberateMemory((void **) &p->alias);
    if (p->description != (char *) NULL)
      LiberateMemory((void **) &p->description);
    if (p->format != (char *) NULL)
      LiberateMemory((void **) &p->format);
    if (p->weight != (char *) NULL)
      LiberateMemory((void **) &p->weight);
    if (p->metrics != (char *) NULL)
      LiberateMemory((void **) &p->metrics);
    if (p->glyphs != (char *) NULL)
      LiberateMemory((void **) &p->glyphs);
    if (p->version != (char *) NULL)
      LiberateMemory((void **) &p->version);
    type_list=p;
    p=p->next;
    LiberateMemory((void **) &type_list);
  }
  type_list=(TypeInfo *) NULL;
  DestroySemaphoreInfo(type_semaphore);
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
%      TypeInfo *GetTypeInfo(const char *name,ExceptionInfo *exception)
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
MagickExport TypeInfo *GetTypeInfo(const char *name,ExceptionInfo *exception)
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
  for (p=type_list; p != (TypeInfo *) NULL; p=p->next)
    if ((p->name != (char *) NULL) && (LocaleCompare(p->name,name) == 0))
      break;
  return(p);
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
  register int
    i;

  register TypeInfo
    *p;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"ImageMagick supports these built-in fonts.\n");
  p=GetTypeInfo("*",exception);
  if (p == (TypeInfo *) NULL)
    return(False);
  if (p->filename != (char *) NULL)
    (void) fprintf(file,"\nFilename: %.1024s\n\n",p->filename);
  (void) fprintf(file,"Name                         Description\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  for ( ; p != (TypeInfo *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=strlen(p->name); i <= 28; i++)
      (void) fprintf(file," ");
    if (p->description != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->description);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
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
  path=GetMagickConfigurePath(filename);
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
    FormatString(keyword,"%.1024s",token);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
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
          MagickError(ResourceLimitError,"Unable to allocate fonts",
            "Memory allocation failed");
        memset(type_info,0,sizeof(TypeInfo));
        if (type_list == (TypeInfo *) NULL)
          {
            type_info->filename=AllocateString(filename);
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
      case 'A':
      case 'a':
      {
        if (LocaleCompare((char *) keyword,"alias") == 0)
          {
            type_list->alias=AllocateString(token);
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            type_list->format=AllocateString(token);
            break;
          }
        if (LocaleCompare((char *) keyword,"fullname") == 0)
          {
            type_list->description=AllocateString(token);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"glyphs") == 0)
          {
            type_list->glyphs=AllocateString(token);
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"metrics") == 0)
          {
            type_list->metrics=AllocateString(token);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            type_list->name=AllocateString(token);
            break;
          }
        break;
      }
      case 'V':
      case 'v':
      {
        if (LocaleCompare((char *) keyword,"version") == 0)
          {
            type_list->version=AllocateString(token);
            break;
          }
        break;
      }
      case 'W':
      case 'w':
      {
        if (LocaleCompare((char *) keyword,"weight") == 0)
          {
            type_list->weight=AllocateString(token);
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
