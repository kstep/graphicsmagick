/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                    Methods to Recognize Image formats                       %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                               March 2000                                    %
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
#include "magic.h"

/*
  Define declaration.
*/
#define MagicInfoListExtent  256
#define StringMethodArgumentExtent  64

/*
  Typedef declarations.
*/
typedef struct _StringMethodArgument
{
  unsigned char
    value[StringMethodArgumentExtent];

  unsigned int
    length,
    offset;
} StringMethodArgument;

/*
  Global declarations.
*/
static MagicInfo
  **magic_list = (MagicInfo **) NULL;

static SemaphoreInfo
  *magic_semaphore = (SemaphoreInfo *) NULL;
/*
  Forward declarations.
*/
static int
  ReadConfigurationFile(const char *filename);

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
%      void DestroyMagicInfo(void)
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static void DestroyMagicInfo(void)
{
  MagicInfoMember
    *entry,
    *member;

  register int
    i;

  if (magic_list == (MagicInfo **) NULL)
    return;
  AcquireSemaphore(&magic_semaphore);
  for (i=0; magic_list[i] != (MagicInfo *) NULL; i++)
  {
    LiberateMemory((void **) &magic_list[i]->tag);
    for (member=magic_list[i]->member; member != (MagicInfoMember *) NULL; )
    {
      entry=member;
      member=member->next;
      LiberateMemory((void **) &entry->argument);
      LiberateMemory((void **) &entry);
    }
    LiberateMemory((void **) &magic_list[i]);
  }
  LiberateMemory((void **) &magic_list);
  magic_list=(MagicInfo **) NULL;
  LiberateSemaphore(&magic_semaphore);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

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
%  Method GetMagicInfo searches the magic list for any image format tag that
%  matches the specified image signature and if found returns attributes for
%  that image format.
%
%  The format of the GetMagicInfo method is:
%
%      MagicInfo *GetMagicInfo(const unsigned char *magick,
%        const unsigned int length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o magic_info: Method GetMagicInfo searches the magic list for any image
%      format tag that matches the specified image signature and if found
%      returns attributes for that image format.
%
%    o image: The address of a structure of type Image.
%
%    o magick: A binary string generally representing the first few characters
%      of the image file or blob.
%
%    o length: The length of the binary signature.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport MagicInfo *GetMagicInfo(const unsigned char *magick,
  const unsigned int length,ExceptionInfo *exception)
{
  MagicInfoMember
    *member;

  register int
    i;

  register StringMethodArgument
    *p;

  assert(magick != (const unsigned char *) NULL);
  AcquireSemaphore(&magic_semaphore);
  if (magic_list == (MagicInfo **) NULL)
    {
      unsigned int
        status;

      /*
        Read magiclist.
      */
      status=ReadConfigurationFile("magic.mgk");
      if (status == False)
        ThrowException(exception,FileOpenWarning,
          "Unable to read font configuration file","magic.mgk");
      atexit(DestroyMagicInfo);
    }
  LiberateSemaphore(&magic_semaphore);
  if (magic_list == (MagicInfo **) NULL)
    return((MagicInfo *) NULL);
  /*
    Search for requested signature.
  */
  for (i=0; magic_list[i] != (MagicInfo *) NULL; i++)
  {
    switch (magic_list[i]->member->method)
    {
      case StringMagicMethod:
      {
        for (member=magic_list[i]->member; member != (MagicInfoMember *) NULL; )
        {
          p=(StringMethodArgument *) member->argument;
          if ((p->offset+p->length) > length)
            break;
          if (memcmp(magick+p->offset,p->value,p->length) == 0)
            {
              if (member->status != True)
                break;
              if (member->next == (MagicInfoMember *) NULL)
                return(magic_list[i]);
             }
          member=member->next;
        }
        break;
      }
      default:
        break;
    }
  }
  return((MagicInfo *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M a g i c C o n f i g u r e F i l e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the magic configuration file.  This file
%  contains one or tag and magic combinations to help identify which image
%  format a particular image file or blob may be.  For example,
%
%    JPEG    string(0,"\377\330\377")
%    PNG     string(0,"\211PNG\r\n\032\n")
%    TIFF    string(0,"\115\115\000\052")
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *filename)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadConfigurationFile returns True if the
%      configuration file is parsed properly, otherwise False is returned.
%
%    o filename:  This character string is the filename of the magic
%      configuration file.
%
*/
static int ReadConfigurationFile(const char *filename)
{
  char
    buffer[MaxTextExtent],
    *p,
    *path,
    tag[MaxTextExtent];

  FILE
    *file;

  int
    j,
    line_number;

  MagicInfoMember
    *test_member;

  register int
    i;

  register unsigned char
    *q;

  assert(filename != (const char *) NULL);
  if (magic_list == (MagicInfo **) NULL)
    {
      /*
        Allocate the magic format list.
      */
      magic_list=(MagicInfo **)
        AcquireMemory(MagicInfoListExtent*sizeof(MagicInfo *));
      if (magic_list == (MagicInfo **) NULL)
        MagickError(ResourceLimitError,"Unable to allocate image",
          "Memory allocation failed");
      magic_list[0]=(MagicInfo *) NULL;
    }
  /*
    Allocate and initialize the format list.
  */
  path=GetMagickConfigurePath(filename);
  if (path == (char *) NULL)
    return(False);
  file=fopen(path,"r");
  LiberateMemory((void **) &path);
  if (file == (FILE *) NULL)
    return(False);
  for (i=0; magic_list[i] != (MagicInfo *) NULL; i++);
  for (line_number=0; (i < MagicInfoListExtent-1) && !feof(file); )
  {
    line_number++;
    (void) fgets(buffer,MaxTextExtent,file);
    buffer[MaxTextExtent-1]='\0';
    Strip(buffer);
    if ((*buffer == '\0') || (*buffer == '#'))
      continue;
    p=buffer;
    for (j=0; isalnum((int) *p); j++)
      tag[j]=(*p++);
    tag[j]='\0';
    if (*p == '\0')
      goto eol_error;
    magic_list[i]=(MagicInfo *) AcquireMemory(sizeof(MagicInfo));
    if (magic_list[i] == (MagicInfo *) NULL)
      MagickError(ResourceLimitError,"Unable to allocate image",
        "Memory allocation failed");
    magic_list[i]->tag=AllocateString(tag);
    magic_list[i]->member=(MagicInfoMember *) NULL;
    /*
      Parse sequence of match rules.
    */
    for ( ; ; )
    {
      while (isspace((int) *p))
        p++;
      if (*p == '\0')
        goto eol_error;
      test_member=(MagicInfoMember *) AcquireMemory(sizeof(MagicInfoMember));
      if (test_member == (MagicInfoMember *) NULL)
        MagickError(ResourceLimitError,"Unable to allocate image",
          "Memory allocation failed");
      test_member->method=UndefinedMagicMethod;
      test_member->argument=(void *) NULL;
      test_member->status=True;
      test_member->next=(MagicInfoMember *) NULL;
      if (*p == '!')
        {
          test_member->status=False;
          p++;
        }
      while (isspace((int) *p))
        p++;
      if (*p == '\0')
        goto eol_error;
      if (LocaleNCompare(p,"string(",7) == 0)
        {
          StringMethodArgument
            *argument;

          argument=(StringMethodArgument *)
            AcquireMemory(sizeof(StringMethodArgument));
          if (argument == (StringMethodArgument*)NULL)
            MagickError(ResourceLimitError,"Unable to allocate image",
              "Memory allocation failed");
          test_member->argument=(void *) argument;
          test_member->method=StringMagicMethod;
          argument->length=0;
          argument->offset=0;
          *argument->value='\0';
          p+=7;
          while (isspace((int) *p))
            p++;
          if (*p == '\0')
            goto eol_error;
          argument->offset=strtol(p,&p,10);
          while (isspace((int) *p))
            p++;
          if (*p == '\0')
            goto eol_error;
          if (*p != ',')
            goto syntax_error;
          p++;
          while (isspace((int) *p))
            p++;
          if (*p == '\0')
            goto eol_error;
          if (*p != '"')
            goto syntax_error;
          p++;
          q=argument->value;
          for ( ; ; )
          {
            if (*p == '\0')
              goto eol_error;
            if (*p == '"')
              {
                *q='\0';
                p++;
                break;
              }
            if (*p == '\\')
              {
                p++;
                if (isdigit((int) *p))
                  {
                    *q=(unsigned char) strtol(p,&p,8);
                    q++;
                    argument->length++;
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
                q++;
                p++;
                argument->length++;
                continue;
              }
              *q=(*p);
              q++;
              p++;
              argument->length++;
            }
            while (isspace((int) *p))
              p++;
            if (*p == '\0')
              goto eol_error;
            if (*p != ')')
              goto syntax_error;
            p++;
            if (magic_list[i]->member == (MagicInfoMember *) NULL)
              magic_list[i]->member=test_member;
            else
              {
                MagicInfoMember
                  *member;

                member=magic_list[i]->member;
                while (member->next != (MagicInfoMember *) NULL)
                  member=member->next;
                member->next=test_member;
              }
            while (isspace((int) *p))
              p++;
            if (*p == ';')
              {
                p++;
                continue;
              }
            if (*p == '\0')
              break;
          }
        else
          goto syntax_error;
    }
    i++;
    magic_list[i]=(MagicInfo *) NULL;
    continue;
    syntax_error:
    {
      char
        message[MaxTextExtent];

      FormatString(message,"%.1024s:%d: syntax: \"%.1024s\"\n",filename,
        line_number,p);
      MagickWarning(OptionWarning,message,(char *) NULL);
      continue;
    }
    eol_error:
    {
      char
        message[MaxTextExtent];

      FormatString(message,"%.1024s:%d: syntax: \"%.1024s\"\n",message,
        line_number,"unexpected end of line");
      MagickWarning(OptionWarning,message,(char *) NULL);
      continue;
    }
  }
  (void) fclose(file);
  if (magic_list == (MagicInfo **) NULL)
    return(False);
  return(True);
}
