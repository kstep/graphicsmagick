/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     FFFFF   OOO   N   N  TTTTT  SSSSS                       %
%                     F      O   O  NN  N    T    SS                          %
%                     FFF    O   O  N N N    T     SSS                        %
%                     F      O   O  N  NN    T       SS                       %
%                     F       OOO   N   N    T    SSSSS                       %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Font Methods                          %
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
#include "defines.h"

/* 
  Define declarations.
*/ 
#define FontFilename  "fonts.mgk"

/*
  Static declarations.
*/
static FontInfo
  *font_list = (FontInfo *) NULL;

static SemaphoreInfo
  *font_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadConfigurationFile(const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y F o n t I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyFontInfo deallocates memory associated with the fonts list.
%
%  The format of the DestroyFontInfo method is:
%
%      DestroyFontInfo(void)
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport void DestroyFontInfo(void)
{
  register FontInfo
    *p;

  AcquireSemaphore(&font_semaphore);
  for (p=font_list; p != (FontInfo *) NULL; )
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
    font_list=p;
    p=p->next;
    LiberateMemory((void **) &font_list);
  }
  font_list=(FontInfo *) NULL;
  LiberateSemaphore(&font_semaphore);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t F o n t I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetFontInfo searches the font list for the specified name and if
%  found returns attributes for that font.
%
%  The format of the GetFontInfo method is:
%
%      FontInfo *GetFontInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o font_info: Method GetFontInfo searches the font list for the specified
%      name and if found returns attributes for that font.
%
%    o name: The font name.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport FontInfo *GetFontInfo(const char *name,ExceptionInfo *exception)
{
  register FontInfo
    *p;

  AcquireSemaphore(&font_semaphore);
  if (font_list == (FontInfo *) NULL)
    {
      unsigned int
        status;

      /*
        Read fonts.
      */
      status=ReadConfigurationFile(FontFilename);
      if (status == False)
        ThrowException(exception,FileOpenWarning,
          "Unable to read font configuration file",FontFilename);
      atexit(DestroyFontInfo);
    }
  LiberateSemaphore(&font_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return(font_list);
  /*
    Search for requested font.
  */
  for (p=font_list; p != (FontInfo *) NULL; p=p->next)
    if ((p->name != (char *) NULL) && (LocaleCompare(p->name,name) == 0))
      break;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t F o n t I n f o                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListFontInfo lists the text fonts to a file.
%
%  The format of the ListFontInfo method is:
%
%      unsigned int ListFontInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListFontInfo(FILE *file,ExceptionInfo *exception)
{
  register FontInfo
    *p;

  register int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"ImageMagick supports these built-in fonts.\n");
  p=GetFontInfo("*",exception);
  if (p == (FontInfo *) NULL)
    return(False);
  if (p->filename != (char *) NULL)
    (void) fprintf(file,"\nFilename: %.1024s\n\n",p->filename);
  (void) fprintf(file,"Name                         Description\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  for ( ; p != (FontInfo *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=Extent(p->name); i <= 28; i++)
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
%  Method ReadConfigurationFile reads the font configuration file which maps
%  font names with Type1 or TrueType glyph files on disk.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *basename)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one font
%      is defined otherwise False.
%
%    o basename:  The font configuration filename.
%
%
*/
static unsigned int ReadConfigurationFile(const char *basename)
{
  char
    filename[MaxTextExtent],
    keyword[MaxTextExtent],
    *path,
    value[MaxTextExtent];

  FILE
    *file;

  int
    c;

  register char
    *p;

  /*
    Read the font configuration file.
  */
  path=GetMagickConfigurePath(basename);
  if (path == (char *) NULL)
    return(False);
  FormatString(filename,"%.1024s",path);
  LiberateMemory((void **) &path);
  file=fopen(filename,"r");
  if (file == (FILE *) NULL)
    return(False);
  for (c=fgetc(file); c != EOF; c=fgetc(file))
  {
    /*
      Parse keyword.
    */
    while (isspace(c))
      c=fgetc(file);
    p=keyword;
    do
    {
      if ((p-keyword) < (MaxTextExtent-1))
        *p++=c;
      c=fgetc(file);
    } while ((c == '<') || isalnum(c) || (c == '!'));
    *p='\0';
    if (*keyword == '<')
      {
        if (LocaleCompare(keyword,"<!") == 0)
          for (c=fgetc(file); (c != '>') && (c != EOF); c=fgetc(file));
        if (LocaleCompare(keyword,"<font") == 0)
          {
            FontInfo
              *font_info;

            /*
              Allocate memory for the font list.
            */
            font_info=(FontInfo *) AcquireMemory(sizeof(FontInfo));
            if (font_info == (FontInfo *) NULL)
              MagickError(ResourceLimitError,"Unable to allocate fonts",
                "Memory allocation failed");
            memset(font_info,0,sizeof(FontInfo));
            if (font_list == (FontInfo *) NULL)
              {
                font_info->filename=AllocateString(filename);
                font_list=font_info;
              }
            else
              {
                font_list->next=font_info;
                font_info->previous=font_list;
                font_list=font_list->next;
              }
          }
        continue;
      }
    while (isspace(c))
      c=fgetc(file);
    if (c != '=')
      continue;
    for (c=fgetc(file); isspace(c); c=fgetc(file));
    if ((c != '"') && (c != '\''))
      continue;
    /*
      Parse value.
    */
    p=value;
    if (c == '"')
      {
        for (c=fgetc(file); (c != '"') && (c != EOF); c=fgetc(file))
          if ((p-value) < (MaxTextExtent-1))
            *p++=c;
      }
    else
      for (c=fgetc(file); (c != '\'') && (c != EOF); c=fgetc(file))
        if ((p-value) < (MaxTextExtent-1))
          *p++=c;
    *p='\0';
    if (font_list == (FontInfo *) NULL)
      continue;
    switch (*keyword) 
    {
      case 'A':
      case 'a':
      {
        if (LocaleCompare((char *) keyword,"alias") == 0)
          {
            font_list->alias=AllocateString(value);
            break;
          }
        break;
      }
      case 'F':
      case 'f':
      {
        if (LocaleCompare((char *) keyword,"format") == 0)
          {
            font_list->format=AllocateString(value);
            break;
          }
        if (LocaleCompare((char *) keyword,"fullname") == 0)
          {
            font_list->description=AllocateString(value);
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"glyphs") == 0)
          {
            font_list->glyphs=AllocateString(value);
            break;
          }
        break;
      }
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"metrics") == 0)
          {
            font_list->metrics=AllocateString(value);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            font_list->name=AllocateString(value);
            break;
          }
        break;
      }
      case 'V':
      case 'v':
      {
        if (LocaleCompare((char *) keyword,"version") == 0)
          {
            font_list->version=AllocateString(value);
            break;
          }
        break;
      }
      case 'W':
      case 'w':
      {
        if (LocaleCompare((char *) keyword,"weight") == 0)
          {
            font_list->weight=AllocateString(value);
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  (void) fclose(file);
  if (font_list == (FontInfo *) NULL)
    return(False);
  while (font_list->previous != (FontInfo *) NULL)
    font_list=font_list->previous;
  return(True);
}
