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
#include "magic.h"

/*
  Global declarations.
*/
static MagicInfo
  **magic_list = (MagicInfo **) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagicInfo deallocates memory associated with the MagicInfo
%  list.
%
%  The format of the DestroyMagicInfo method is:
%
%      void DestroyMagicInfo(void)
%
*/
Export void DestroyMagicInfo(void)
{
  MagicInfoMember
    *entry,
    *member;

  register int
    i;

  for (i=0; magic_list[i] != (MagicInfo *) NULL; i++)
  {
    for (member=magic_list[i]->member; member != (MagicInfoMember *) NULL; )
    {
      entry=member;
      FreeMemory((void **) &entry->argument);
      FreeMemory((void **) &entry);
      member=member->next;
    }
  }
  FreeMemory((void **) &magic_list[i]);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M a g i c C o n f i g u r a t i o n F i l e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMagicConfigurationFile reads the magic configuration file.  It
%  contains one or tag and magic combinations to help identify which image
%  format a particular image file or blob may be.  For example,
%
%    JPEG    string(0,"\377\330\377")
%    PNG     string(0,"\211PNG\r\n\032\n")
%    TIFF    string(0,"\115\115\000\052")
%
%  The format of the ReadMagicConfigurationFile method is:
%
%      unsigned int ReadMagicConfigurationFile(const char *filename)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadMagicConfigurationFile returns True if the
%      configuration file is parsed properly, otherwise False is returned.
%
%    o filename:  A pointer to a character string.
%
*/
static int ReadMagicConfigurationFile(const char *filename)
{
  char
    buffer[MaxTextExtent],
    *p,
    tag[MaxTextExtent];

  FILE
    *file;

  int
    j,
    line_number;

  MagicInfoMember
    *test_member;

  register unsigned char
    *q;

  register int
    i;

  /*
    Allocate and initialize the format list.
  */
  file=fopen(filename,"r");
  if (file == (FILE *) NULL)
    return(False);
  i=0;
  magic_list=(MagicInfo **)
    AllocateMemory((MagicInfoListExtent)*sizeof(MagicInfo *));
  if (magic_list == (MagicInfo **) NULL)
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  line_number=0;
  magic_list[i]=(MagicInfo *) NULL;
  while ((i < MagicInfoListExtent-1) && !feof(file))
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
    magic_list[i]=(MagicInfo *) AllocateMemory(sizeof(MagicInfo));
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
      test_member=(MagicInfoMember *) AllocateMemory(sizeof(MagicInfoMember));
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
            AllocateMemory(sizeof(StringMethodArgument));
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
            
      FormatString(message,"%s:%d: syntax: \"%s\"\n",filename,line_number,p);
      MagickWarning(OptionWarning,message,(char *) NULL);
      continue;
    }
    eol_error:
    {
      char
        message[MaxTextExtent];

      FormatString(message,"%s:%d: syntax: \"%s\"\n",message,line_number,
        "unexpected end of line");
      MagickWarning(OptionWarning,message,(char *) NULL);
      continue;
    }
  }
  (void) fclose(file);
  if (magic_list == (MagicInfo **) NULL)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e M a g i c                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageMagic identifies the type of a image blob or file using,
%  among other tests, a test for whether the image begins with a certain
%  magic number.  If a match is found, the tag is returned that represents
%  the image format (e.g. JPEG, TIFF, GIF, etc).
%
%  The format of the SetImageMagic method is:
%
%      unsigned int SetImageMagic(const unsigned char *magick,
%        const unsigned int length,char *magic)
%
%  A description of each parameter follows:
%
%    o status:  Method SetImageMagic returns True if the magic number matches
%      a string in the magic list otherwise False.
%
%    o magick: A binary string generally representing the first few characters
%      of the image file or blob.
%
%    o length: The length of the binary string.
%
%    o magic: A pointer to a character string.  If a match is found, the image
%      format is returned as this string.
%
%
*/

static unsigned int InitializeMagic(void)
{
  char
    path[MaxTextExtent];

  if (getenv("DELEGATE_PATH") != (char *) NULL)
    {
      (void) strcpy(path,getenv("DELEGATE_PATH"));
      (void) strcat(path,DirectorySeparator);
      (void) strcat(path,"magic.mgk");
      if (ReadMagicConfigurationFile(path) == True)
        return(True);
    }
  (void) strcpy(path,DelegatePath);
  (void) strcat(path,DirectorySeparator);
  (void) strcat(path,"magic.mgk");
  return(ReadMagicConfigurationFile(path));
}

Export unsigned int SetImageMagic(const unsigned char *magick,
  const unsigned int length,char *magic)
{
  MagicInfoMember
    *member;

  register int
    i;

  register StringMethodArgument
    *p;

  *magic='\0';
  if (magic_list == (MagicInfo **) NULL)
    if (InitializeMagic() == False)
      return(False);
  if (magic_list == (MagicInfo **) NULL)
    MagickError(FileOpenError,"Unable to read file","magic.mgk");
  for (i=0; magic_list[i] != (MagicInfo *) NULL; i++)
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
                {
                  (void) strcpy(magic,magic_list[i]->tag);
                  return(True);
                }
             }
          member=member->next;
        }
        break;
      }
      default:
        break;
    }
  return(False);
}
