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

static MagicTest
  **magic_test_list = (MagicTest**) NULL;

static void InitializeMagic(void);



/*
  Set magic string (sized to MaxTextExtent)
  based on image header data provided in magick,
  with length magick_length.
*/
Export unsigned int GetMagic(char* magic,
                             const unsigned char *magick,
                             const unsigned int magick_length)
{
  int
    i;

  if(magic_test_list == (MagicTest**) NULL)
    InitializeMagic();

  if(magic_test_list == (MagicTest**) NULL)
    MagickError(FileOpenError,"Failed to initialize ImageMagick",NULL);

  /* Traverse magic tests */
  for (i=0;magic_test_list[i]!=(MagicTest*)NULL;++i)
    {
      switch(magic_test_list[i]->member->method)
        {
        case StringMagicMethod:
          {
            MagicTestMember
              *member;

            /* Traverse test members */
            for(member=magic_test_list[i]->member;
                member!=(MagicTestMember*)NULL;)
              {
                StringMethodArgument
                  *arg;

                arg=(StringMethodArgument*)member->argument;
                if(arg->value_offset+arg->value_length > magick_length)
                  break;

                if(memcmp((char*)magick+arg->value_offset,(char*)arg->value,
                          arg->value_length)==0)
                  {
                    if(member->truth_value == True)
                      {
                        if (member->next==(MagicTestMember*)NULL)
                          {
                            strcpy(magic, magic_test_list[i]->tag);
                            return True;
                          }
                      }
                    else
                      {
                        /* Short-circuit search */
                        break;
                      }
                  }
                member=member->next;
              }

            break;
          }
        default:
          {
          }
        }
    }

  return False;
}

/* Free any memory allocated by this source module */
Export void QuitMagic(void)
{
  int
    i;

  /* Traverse magic tests */
  for (i=0;magic_test_list[i]!=(MagicTest*)NULL;++i)
    {
      MagicTestMember
        *member;

      /* Traverse test members */
      for(member=magic_test_list[i]->member;member!=(MagicTestMember*)NULL;)
        {
          MagicTestMember*
            entry;

          entry=member;
          member=member->next;
          FreeMemory((void**)&member->argument);
          FreeMemory((void**)&member);
        }
    }
  FreeMemory((void**)&magic_test_list[i]);
}

/* Initialize magic_test_list */
static void InitializeMagic(void)
{
  char
    buffer[MaxTextExtent],
    *buff_p,
    file_name[MaxTextExtent],
    tag[MaxTextExtent],
    *tag_p;

  int
    line_number,
    list_index;

  FILE*
    file;

  MagicTestMember
    *test_member;

  strcpy(file_name,DelegatePath);
  strcat(file_name,DirectorySeparator);
  strcat(file_name,"magic.mgk");

  file = fopen(file_name, "r");
  if(file != (FILE*) NULL)
    {
      /* allocate and init format list */
      list_index=0;
      magic_test_list=
        (MagicTest**)AllocateMemory((MagicTestListExtent)*sizeof(MagicTest*));
      if(magic_test_list == (MagicTest**)NULL)
        {
          MagickError(ResourceLimitError,"Unable to allocate image",
                      "Memory allocation failed");
        }
      magic_test_list[list_index]=(MagicTest*)NULL;

      line_number=0;
      while((list_index<MagicTestListExtent-1) && !feof(file))
        {
          ++line_number;
          fgets(buffer,MaxTextExtent,file);
          buffer[MaxTextExtent-1]='\0';
          Strip(buffer);
          if(*buffer=='\0' || *buffer=='#')
            continue;

          /* tag */
          buff_p=buffer;
          tag_p=tag;
          while(isalnum((int)*buff_p))
            *tag_p++=*buff_p++;
          *tag_p='\0';
          if(*buff_p == '\0')
            goto eol_error;
          magic_test_list[list_index]=
            (MagicTest*)AllocateMemory(sizeof(MagicTest));
          if(magic_test_list[list_index]==(MagicTest*)NULL)
            MagickError(ResourceLimitError,"Unable to allocate image",
                        "Memory allocation failed");
          magic_test_list[list_index]->tag=AllocateString(tag);
          magic_test_list[list_index]->member=(MagicTestMember*)NULL;
            
          /* parse sequence of match rules */
          while(1)
            {
              /* skip over white space */
              while(isspace((int)*(buff_p)))
                ++buff_p;
              if(*buff_p == '\0')
                goto eol_error;

              /* intialize test_member */
              test_member=
                (MagicTestMember*)AllocateMemory(sizeof(MagicTestMember));
              if(test_member==(MagicTestMember*)NULL)
                MagickError(ResourceLimitError,"Unable to allocate image",
                            "Memory allocation failed");
              test_member->method=UndefinedMagicMethod;
              test_member->argument=(void*)NULL;
              test_member->truth_value=True;
              test_member->next=(MagicTestMember*)NULL;

              /* test truth value */
              if(*buff_p == '!')
                {
                  ++buff_p;
                  test_member->truth_value=False;
                }

              /* skip over white space */
              while(isspace((int)*(buff_p)))
                ++buff_p;
              if(*buff_p == '\0')
                goto eol_error;

              if(LocaleNCompare(buff_p,"string(",7) == 0)
                {
                  StringMethodArgument
                    *string_argument;

                  unsigned char
                    *str_p;

                  string_argument=
                    (StringMethodArgument*)AllocateMemory(sizeof(StringMethodArgument));
                  if(string_argument==(StringMethodArgument*)NULL)
                    MagickError(ResourceLimitError,"Unable to allocate image",
                                "Memory allocation failed");
                  test_member->argument=(void*)string_argument;

                  test_member->method=StringMagicMethod;
                  string_argument->value_length=0;
                  string_argument->value_offset=0;
                  *string_argument->value='\0';

                  /* skip over "string(" */
                  buff_p += 7;

                  /* skip over white space */
                  while(isspace((int)*(buff_p)))
                    ++buff_p;
                  if(*buff_p == '\0')
                    goto eol_error;

                  /* get offset */
                  string_argument->value_offset=strtol(buff_p, &buff_p, 10);

                  /* skip over white space */
                  while(isspace((int)*(buff_p)))
                    ++buff_p;
                  if(*buff_p == '\0')
                    goto eol_error;

                  /* check for comma */
                  if(*buff_p != ',')
                    goto syntax_error;
                  ++buff_p;

                  /* skip over white space */
                  while(isspace((int)*(buff_p)))
                    ++buff_p;
                  if(*buff_p == '\0')
                    goto eol_error;

                  /* check for double quotes */
                  if(*buff_p != '"')
                    goto syntax_error;
                  ++buff_p;

                  /* translate string */
                  str_p=string_argument->value;
                  while(1)
                    {
                      /* unexpected end of line */
                      if(*buff_p == '\0')
                        goto eol_error;

                      /* end of string, done */
                      if(*buff_p == '"')
                        {
                          *str_p='\0';
                          ++buff_p;
                          break;
                        }

                      if(*buff_p == '\\')
                        {
                          ++buff_p;
                          if(isdigit((int)*(buff_p)))
                            {
                              *str_p=(unsigned char)strtol(buff_p,
                                                           &buff_p,8);
                              ++str_p;
                              ++string_argument->value_length;
                              continue;
                            }

                          switch(*buff_p)
                            {
                            case 'b' :
                              *str_p='\b';
                              break;
                            case 'f' :
                              *str_p='\f';
                              break;
                            case 'n' :
                              *str_p='\n';
                              break;
                            case 'r' :
                              *str_p='\r';
                              break;
                            case 't' :
                              *str_p='\t';
                              break;
                            case 'v' :
                              *str_p='\v';
                              break;
                            case 'a' :
                              *str_p='a';
                              break;
                            case '?' :
                              *str_p='\?';
                              break;
                            default :
                              {
                                *str_p=*buff_p;
                              }
                            }
                          ++str_p;
                          ++buff_p;
                          ++string_argument->value_length;
                          continue;
                        }

                      /* just copy character */
                      *str_p=*buff_p;
                      ++str_p;
                      ++buff_p;
                      ++string_argument->value_length;
                    }

                  /* skip over white space */
                  while(isspace((int)*(buff_p)))
                    ++buff_p;
                  if(*buff_p == '\0')
                    goto eol_error;

                  /* test parens */
                  if(*buff_p != ')')
                    goto syntax_error;
                  ++buff_p;

                  /* add test to list */
                  if(magic_test_list[list_index]->member==(MagicTestMember*)NULL)
                    {
                      /* first in list */
                      magic_test_list[list_index]->member=test_member;
                    }
                  else
                    {
                      MagicTestMember
                        *member;

                      for(member=magic_test_list[list_index]->member;
                          member->next!=(MagicTestMember*)NULL;
                          member=member->next);
                      member->next=test_member;
                    }

                  /* skip over white space */
                  while(isspace((int)*(buff_p)))
                    ++buff_p;

                  if(*buff_p == ';')
                    {
                      ++buff_p;
                      continue;
                    }

                  /* done with the line? */
                  if(*buff_p == '\0')
                    break;
                }
              else
                goto syntax_error;
            }
          ++list_index;
          continue;
          
          syntax_error :
            {
              char
                buff[MaxTextExtent];
              
              sprintf(buff,"%s:%d: syntax: \"%s\"\n", file_name,
                      line_number, buff_p);
              MagickError(OptionError,buff,NULL);
            }
          
          eol_error :
            {
              char
                buff[MaxTextExtent];

              sprintf(buff,"%s:%d: syntax: \"%s\"\n", file_name,
                      line_number, "unexpected end of line");
              MagickError(OptionError,buff,NULL);
            }
        }
      fclose(file);
    }
  else
    {
      MagickError(FileOpenError,"File open error",file_name);
    }
}
