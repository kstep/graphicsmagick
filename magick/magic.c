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



/*
  Set magic string (sized to MaxTextExtent)
  based on image header data provided in magick,
  with length magick_length.
*/
Export unsigned int GetImageMagic(unsigned char* magic,
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

                if(memcmp((void*)magick+arg->value_offset,(void*)arg->value,
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
