/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                     ImageMagick Image Magic Methods                         %
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
#define MagicFilename  "magic.mgk"

/*
  Declare color map.
*/
static char
  *MagicMap =
    "<?xml version=\"1.0\"?>"
    "<magicmap>"
    "  <magic />"
    "</magicmap>";

/*
  Static declarations.
*/
static MagicInfo
  *magic_list = (MagicInfo *) NULL;

static SemaphoreInfo
  *magic_semaphore = (SemaphoreInfo *) NULL;

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
+   D e s t r o y M a g i c I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagicInfo deallocates memory associated with the magic list.
%
%  The format of the DestroyMagicInfo method is:
%
%      DestroyMagicInfo(void)
%
%
*/
MagickExport void DestroyMagicInfo(void)
{
  register MagicInfo
    *p;

  AcquireSemaphoreInfo(&magic_semaphore);
  for (p=magic_list; p != (MagicInfo *) NULL; )
  {
    if (p->filename != (char *) NULL)
      LiberateMemory((void **) &p->filename);
    if (p->name != (char *) NULL)
      LiberateMemory((void **) &p->name);
    if (p->target != (char *) NULL)
      LiberateMemory((void **) &p->target);
    if (p->magic != (unsigned char *) NULL)
      LiberateMemory((void **) &p->magic);
    magic_list=p;
    p=p->next;
    LiberateMemory((void **) &magic_list);
  }
  magic_list=(MagicInfo *) NULL;
  DestroySemaphoreInfo(magic_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagicInfo searches the magic list for the specified name and if
%  found returns attributes for that magic.
%
%  The format of the GetMagicInfo method is:
%
%      MagicInfo *GetMagicInfo(const unsigned char *magic,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o magic_info: Method GetMagicInfo searches the magic list for any image
%      format tag that matches the specified image signature and if found
%      returns attributes for that image format.
%
%    o magic: A binary string generally representing the first few characters
%      of the image file or blob.
%
%    o length: The length of the binary signature.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagicInfo *GetMagicInfo(const unsigned char *magic,
  const size_t length,ExceptionInfo *exception)
{
  register MagicInfo
    *p;

  AcquireSemaphoreInfo(&magic_semaphore);
  if (magic_list == (MagicInfo *) NULL)
    (void) ReadConfigurationFile(MagicFilename,exception);
  LiberateSemaphoreInfo(&magic_semaphore);
  if ((magic == (const unsigned char *) NULL) || (length == 0))
    return(magic_list);
  /*
    Search for requested magic.
  */
  for (p=magic_list; p != (MagicInfo *) NULL; p=p->next)
    if (memcmp(magic+p->offset,p->magic,p->length) == 0)
      break;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagicInfo lists the magic info to a file.
%
%  The format of the ListMagicInfo method is:
%
%      unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
{
  register MagicInfo
    *p;

  register size_t
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"ImageMagick understands these magic strings:\n");
  p=GetMagicInfo((unsigned char *) NULL,0,exception);
  if (p == (MagicInfo *) NULL)
    return(False);
  if (p->filename != (char *) NULL)
    (void) fprintf(file,"\nFilename: %.1024s\n\n",p->filename);
  (void) fprintf(file,"Name      Offset Target\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  for ( ; p != (MagicInfo *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=strlen(p->name); i <= 9; i++)
      (void) fprintf(file," ");
    (void) fprintf(file,"%6ld ",p->offset);
    if (p->target != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->target);
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
    Read the magic configuration file.
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
    xml=AllocateString(MagicMap);
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
    if (LocaleCompare(keyword,"<magic") == 0)
      {
        MagicInfo
          *magic_info;

        /*
          Allocate memory for the magic list.
        */
        magic_info=(MagicInfo *) AcquireMemory(sizeof(MagicInfo));
        if (magic_info == (MagicInfo *) NULL)
          MagickError(ResourceLimitError,"Unable to allocate magics",
            "Memory allocation failed");
        memset(magic_info,0,sizeof(MagicInfo));
        if (magic_list == (MagicInfo *) NULL)
          {
            magic_info->filename=AllocateString(filename);
            magic_list=magic_info;
            continue;
          }
        magic_list->next=magic_info;
        magic_info->previous=magic_list;
        magic_list=magic_list->next;
        continue;
      }
    if (magic_list == (MagicInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword) 
    {
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            magic_list->name=AllocateString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"offset") == 0)
          {
            magic_list->offset=atoi(token);
            break;
          }
        break;
      }
      case 'T':
      case 't':
      {
        if (LocaleCompare((char *) keyword,"target") == 0)
          {
            char
              *p;

            register unsigned char
              *q;

            magic_list->target=AllocateString(token);
            magic_list->magic=(unsigned char *) AllocateString(token);
            q=magic_list->magic;
            for (p=magic_list->target; *p != '\0'; )
            {
              if (*p == '\\')
                {
                  p++;
                  if (isdigit((int) *p))
                    {
                      *q++=(unsigned char) strtol(p,&p,8);
                      magic_list->length++;
                      continue;
                    }
                  switch (*p)
                  {
                    case 'b': *q='\b'; break;
                    case 'f': *q='\f'; break;
                    case 'n': *q='\n'; break;
                    case 'r': *q='\r'; break;
                    case 't': *q='\t'; break;
                    case 'v': *q='\v'; break;
                    case 'a': *q='a'; break;
                    case '?': *q='\?'; break;
                    default: *q=(*p); break;
                  }
                  p++;
                  q++;
                  magic_list->length++;
                  continue;
                }
              *q++=(*p++);
              magic_list->length++;
            }
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
  if (magic_list == (MagicInfo *) NULL)
    return(False);
  while (magic_list->previous != (MagicInfo *) NULL)
    magic_list=magic_list->previous;
  return(True);
}
