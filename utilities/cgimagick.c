/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                             C G I M A G I C K                               %
%                                                                             %
%                                                                             %
%        A shim layer to give command line utilities a cgi interface.         %
%                                                                             %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                           William T. Radcliffe                              %
%                                June 2000                                    %
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
%  This is a simple shim that provides a CGI layer for calling into any of the
%  normal command line utilities.
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/defines.h"
static void Usage(const char *client_name)
{
}
/*
  Include the convert mainline as a subroutine
*/
#define CONVERT_MAIN
#include "convert.c"
/*
  Include the combine mainline as a subroutine
*/
#define COMBINE_MAIN
#include "combine.c"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C G I T o A r g v                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CGIToArgv converts a text string passed as part of a CGI request
%  into command line arguments.
%
%  The format of the CGIToArgv method is:
%
%      char **CGIToArgv(const char *text,int *argc)
%
%  A description of each parameter follows:
%
%    o argv:  Method CGIToArgv returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%    o argc:  This integer pointer returns the number of arguments in the
%      list.
%
%
*/

typedef struct _html_code
{
  short
    len;
  const char
    *code,
    val;
} html_code;

static html_code html_codes[] = {
  4,"&lt;",'<',
  4,"&gt;",'>',
  5,"&amp;",'&',
  6,"&quot;",'"'
};

/*
  This routine converts HTML escape sequences back to the
  original ASCII representation and returns the number of
  characters dropped.
*/
int convertHTMLcodes(char *s, int len)
{
  if (len <=0 || s==(char*)NULL || *s=='\0')
    return 0;

  if (s[1] == '#')
    {
      int val, o;

      if (sscanf(s,"&#%d;",&val) == 1)
      {
        o = 3;
        while (s[o] != ';')
        {
          o++;
          if (o > 5)
            break;
        }
        if (o < 6)
          strcpy(s+1, s+1+o);
        *s = val;
        return o;
      }
    }
  else
    {
      int
        i,
        codes = sizeof(html_codes) / sizeof(html_code);

      for (i=0; i < codes; i++)
      {
        if (html_codes[i].len <= len)
          if (strnicmp(s, html_codes[i].code, html_codes[i].len) == 0)
            {
              strcpy(s+1, s+html_codes[i].len);
              *s = html_codes[i].val;
              return html_codes[i].len-1;
            }
      }
    }
  return 0;
}

#define IsCGIDelimiter(c)  ((c) == '&')

unsigned int CGIToArgv(const char *text,int *argc,char ***argv)
{
  char
    **vector;

  const char
    *p,
    *q;

  register int
    i;

  int
    count;

  assert(argc != (int *) NULL);
  assert(argv != (char ***) NULL);
  if (text == (char *) NULL)
    return(False);
  if (argc == (int *) NULL)
    return(False);
  if (argv == (char ***) NULL)
    return(False);
  count=0;
  /*
    Determine the number of arguments by scanning for delimiters
  */
  for (p=text; *p != '\0'; p++)
  {
    if (IsCGIDelimiter(*p))
      count++;
  }
  count++;
  vector=(char **) AllocateMemory((count+2)*sizeof(char *));
  if (vector == (char **) NULL)
    {
      MagickError(ResourceLimitError,"Unable to convert string to argv",
        "Memory allocation failed");
      return(False);
    }
  /*
    Convert string to an ASCII list.
  */
  vector[0]=AllocateString("magick");
  q=text;
  for (i=1; i < count+1; i++)
  {
    p=q;
    while (!IsCGIDelimiter(*q))
      q++;
    vector[i]=(char *) AllocateMemory(q-p+1);
    if (vector[i] == (char *) NULL)
      {
        MagickError(ResourceLimitError,"Unable to convert string to argv",
          "Memory allocation failed");
        return(False);
      }
    (void) strncpy(vector[i],p,q-p);
    vector[i][q-p]='\0';
    /*
      Convert any special HTML codes in place back to ASCII
    */
    convertHTMLcodes(vector[i], q-p);
    q++;
  }
  vector[i]=(char *) NULL;
  *argc=count+1;
  *argv=vector;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
  char
    **argv_hw,
    *command,
    *mime_type,
    prefix[MaxTextExtent],
    *option;

  int
    argc_hw,
    i;

  unsigned int
    status;

  /*
    Initialize command line arguments.
  */
  ReadCommandlLine(argc,&argv);
  MagickIncarnate(*argv);
	if (getenv("GATEWAY_INTERFACE"))
    {
      FormatString(prefix,"HTTP/1.0 200 Ok\r\nContent-Type: %s\r\n","image/jpeg");
      status=CGIToArgv(getenv("QUERY_STRING"),&argc,&argv);
      if (status == True)
        {
          for (argc_hw=1; argc_hw < argc; argc_hw++)
          {
            if (LocaleCompare("convert",argv[argc_hw]) == 0)
              {
                argv_hw = &argv[argc_hw];
                for (i=argc_hw; i < argc; i++)
                {
                  option=argv[i];
                  if ((Extent(option) < 2) || ((*option != '-') && (*option != '+')))
                    {
                      convert_main(argc-argc_hw-1,argv_hw,prefix,Extent(prefix));
                      argc_hw = i+1;
                      argv_hw = &argv[argc_hw];
                    }
                }
              }
            else if (LocaleCompare("combine",argv[i]) == 0)
              {
                for (i=argc_hw; i < argc; i++)
                {
                  option=argv[i];
                  if ((Extent(option) < 2) || ((*option != '-') && (*option != '+')))
                    {
                      combine_main(argc-argc_hw-1,argv_hw,prefix,Extent(prefix));
                      argc_hw = i+1;
                      argv_hw = &argv[argc_hw];
                    }
                }
              }
          }
        }
    }
  FreeMemory((void **) &argv);
  Exit(0);
  return(False);
}
