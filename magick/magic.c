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

static void IntializeImageMagic(void);



/*
  Set magic string (sized to MaxTextExtent)
  based on image header data provided in magick,
  with length magick_length.
*/
Export unsigned int GetImageMagic(char* magic,
                                  const unsigned char *magick,
                                  const unsigned int magick_length)
{
  int
    i;

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
            for(member=magic_test_list[i]->member;member!=(MagicTestMember*)NULL;)
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
Export void ExitMagic(void)
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
static void IntializeImageMagic(void)
{
  char
    buffer[MaxTextExtent],
    *buff_p,
    file_name[MaxTextExtent],
    tag[MaxTextExtent],
    *tag_p;

  int
    line_number;

  FILE*
    file;

  MagicTestMember
    test_member;

  strcpy(file_name,DelegatePath);
  strcpy(file_name,DirectorySeparator);
  strcpy(file_name,"magic.mgk");

  file = fopen(file_name, "r");
  if(file != (FILE*) NULL)
    {
      line_number=0;
      while(!feof(file))
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
            {
              printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                     "unexpected end of line");
              continue;
            }

          /* parse sequence of match rules */
          while(1)
            {
              /* skip over white space */
              while(isspace((int)*buff_p))
                ++buff_p;
              if(*buff_p == '\0')
                {
                  printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                         "unexpected end of line");
                  continue;
                }

              /* intialize test_member */
              test_member.method=UndefinedMagicMethod;
              test_member.argument=(void*)NULL;
              test_member.truth_value=True;
              test_member.next=(MagicTestMember*)NULL;

              /* test truth value */
              if(*buff_p++ == '!')
                test_member.truth_value=False;

              /* skip over white space */
              while(isspace((int)*buff_p))
                ++buff_p;
              if(*buff_p == '\0')
                {
                  printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                         "unexpected end of line");
                  break;
                }

              if(LocaleCompare(buff_p,"string(") == 0)
                {
                  StringMethodArgument
                    string_argument;

                  unsigned char
                    string_buff[StringMethodArgumentExtent],
                    *str_p;

                  test_member.method=StringMagicMethod;
                  *string_buff='\0';
                  str_p=string_buff;

                  /* skip over "string(" */
                  buff_p += 7;

                  /* skip over white space */
                  while(isspace((int)*buff_p))
                    ++buff_p;
                  if(*buff_p == '\0')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                             "unexpected end of line");
                      break;
                    }

                  /* get offset */
                  string_argument.value_offset=strtol(buff_p, &buff_p, 10);

                  /* skip over white space */
                  while(isspace((int)*buff_p))
                    ++buff_p;
                  if(*buff_p == '\0')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                             "unexpected end of line");
                      break;
                    }

                  /* check for comma */
                  if(*buff_p != ',')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                             buff_p);
                      break;
                    }
                  ++buff_p;

                  /* skip over white space */
                  while(isspace((int)*buff_p))
                    ++buff_p;
                  if(*buff_p == '\0')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                             "unexpected end of line");
                      break;
                    }

                  /* check for double quotes */
                  if(*buff_p != '"')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                             buff_p);
                      break;
                    }
                  ++buff_p;

                  /* translate string */
                  while(1)
                    {
                      if(*buff_p == '"')
                        break;

                      if(*buff_p == '\\');
                      {
                        ++buff_p;
                        if(isdigit((int)*(buff_p)) &&
                           isdigit((int)*(buff_p+1)) &&
                           isdigit((int)*(buff_p+2)))
                          {
                            /* octal code */
                            char
                              lbuff[4];

                            lbuff[0]=*buff_p++;
                            lbuff[1]=*buff_p++;
                            lbuff[2]=*buff_p++;
                            lbuff[3]='\0';

                            *str_p++=(unsigned char)strtol(lbuff,
                                                           (char**)NULL,8);
                            buff_p += 3;
                          }
                        else
                          {
                            /* escaped character */
                            *str_p++=*buff_p++;
                          }
                        continue;
                      }

                      if(*buff_p == '\0')
                        {
                          printf("%s:%d: syntax: \"%s\"\n", file_name,
                                 line_number,
                                 "unexpected end of line");
                          break;
                        }
                    }

                  /* skip over white space */
                  while(isspace((int)*buff_p))
                    ++buff_p;
                  if(*buff_p == '\0')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name,
                             line_number, "unexpected end of line");
                      break;
                    }

                  /* test parens */
                  if(*buff_p != ')')
                    {
                      printf("%s:%d: syntax: \"%s\"\n", file_name,
                             line_number, buff_p);
                      break;
                    }
                  ++buff_p;

                  /* done with the line? */
                  if(*buff_p == '\0')
                    break;
                }
              else
                {
                  printf("%s:%d: syntax: \"%s\"\n", file_name, line_number,
                         buff_p);
                  continue;
                }

            }
        }
      fclose(file);
    }
}
