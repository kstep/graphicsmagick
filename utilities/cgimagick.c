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

/*  -------------------------------------------------------------------------
    Function: decode_hex

    Synopsis: Decodes a hexadecimal string.  Stops after outmax characters
    or when an invalid hex character is reached.  Sets the input pointer
    to the first unprocessed character.  Returns the result.
    -------------------------------------------------------------------------*/

int decode_hex (const char **input,size_t outmax)
{
    static char
        hex_to_bin [128] = {
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,    /*   0..9     */
           -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*   A..F     */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*            */
           -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,    /*   a..f     */
           -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };  /*            */
    int
        nextch;
    size_t
        index,
        result;

    assert(input != (const char **) NULL);
    assert((*input) != (const char *) NULL);

    index  = 0;
    result = 0;
    while (outmax == 0 || index < outmax)
      {
        nextch = (*input) [index] & 127;
        if (nextch && hex_to_bin [nextch] != -1)
          {
            result = result * 16 + hex_to_bin [nextch];
            index++;
          }
        else
            break;
      }
    (*input) += index;
    return (result);
}

/*  ---------------------------------------------------------------------[<]-
    Function: http_unescape

    Synopsis: Removes HTTP escaping from a string.  See http_escape() for
    details of the escaping algorithm.  If the result string is NULL,
    modifies the source string in place, else fills-in the result string.
    Returns the resulting string.  End-of-line sequences (%0A%0D) are
    stored as a single new-line character, i.e. carriage-returns (%0D) are
    not stored.
    ---------------------------------------------------------------------[>]-*/

char *http_unescape(char *string,char *result)
{
  char
    *target;                          /*  Where we store the result        */

  assert(string != (char *) NULL);
  if (!result)                        /*  If result string is null,        */
    result = string;                  /*    modify in place                */
  target = result;

  while (*string)
  {
    if (*string == '%'                /*  Unescape %xx sequence            */
          && string [1] && string [2])
      {
        string++;
        *target = decode_hex ((const char **) &string, 2);
        if (*target != '\r')
          target++;                   /*  We do not store CRs              */
      }
    else
      {
        if (*string == '+')           /*  Spaces are escaped as '+'        */
          *target++ = ' ';
        else
          *target++ = *string;        /*  Otherwise just copy              */

        string++;
      }
  }
  *target = '\0';                     /*  Terminate target string          */
  return (result);
}


/*  ---------------------------------------------------------------------[<]-
    Function: http_unescape_hex

    Synopsis: Removes HTTP hex escaping from a URL string, by expanding any
    sequences of characters %xx.
    ---------------------------------------------------------------------[>]-*/

char *http_unescape_hex(char *string,char *result)
{
  char
    *target;                          /*  Where we store the result        */

  assert(string != (char *) NULL);
  if (!result)                        /*  If result string is null,        */
    result = string;                  /*    modify in place                */
  target = result;

  while (*string)
  {
    if (*string == '%'                /*  Unescape %xx sequence            */
      &&   string [1] && string [2])
      {
        string++;
        *target = decode_hex ((const char **) &string, 2);
        target++;             
      }
    else
      {
        *target++ = *string;          /*  Otherwise just copy              */
        string++;
      }
  }
  *target = '\0';                     /*  Terminate target string          */
  return (result);
}

/* Defines for input methods. */
#define CGI_GET  0
#define CGI_POST 1
#define CGI_ANY  2

char *cgi_get_input(int iMethod)
{
  int iStdinLen = 0, iMethodWas = 0;
  char *strHead, *strRetBuf;

  if (iMethod == CGI_POST || iMethod == CGI_ANY)
    {
      if (getenv ("CONTENT_LENGTH"))
        {
          iStdinLen = atoi (getenv ("CONTENT_LENGTH"));
          iMethodWas = CGI_POST;
        }
    }
  if (iMethod == CGI_GET || (iMethod == CGI_ANY && !iStdinLen))
    {
      if (getenv ("QUERY_STRING"))
        {
          iStdinLen = Extent (getenv ("QUERY_STRING"));
          iMethodWas = CGI_GET;
        }
    }
  if (!iStdinLen)
      return (NULL);

  strHead = strRetBuf = (char *) AllocateMemory(sizeof (char) * iStdinLen + 1);
  if (strHead == (char *) NULL)
      return (NULL);

  memset (strRetBuf, 0, iStdinLen + 1);
  if (iMethodWas == CGI_POST)
    {
#ifdef WIN32
			setmode(fileno(stdin), O_BINARY);
#endif
      fread (strRetBuf, sizeof (char), iStdinLen, stdin);
    }
  else
    strncpy (strRetBuf, getenv ("QUERY_STRING"), (iStdinLen + 1));

  return (*strHead? strHead: NULL);
}

/*  ---------------------------------------------------------------------[<]-
    Function: env_get_string

    Synopsis: Translates the specified environment variable and returns a
    static string containing the value.  If the variable is not defined in
    the environment, returns the specified default value.  Note: if you
    want to use the value in a program you should use strdupl() to make a
    copy.  The environment variable name is always translated into upper
    case.  The default value may be NULL.

    Examples:
    config_file = strdupl (env_get_string ("config", "default.cfg"));
    ---------------------------------------------------------------------[>]-*/

char *env_get_string(const char *name,const char *default_value)
{
  char
    *variable_name,
    *variable_value;

  variable_name = AllocateString (name);
  LocaleUpper (variable_name);
  variable_value = getenv (variable_name);
  FreeMemory((void **) &variable_name);
  return (variable_value? variable_value: (char *) default_value);
}

/*  ---------------------------------------------------------------------[<]-
    Function: env_get_number

    Synopsis: Translates the specified environment variable and returns the
    long numeric value of the string.  If the variable is not defined in
    the environment, returns the specified default value.  The environment
    variable name is always translated into upper case.

    Examples:
    max_retries = env_get_number ("retries", 5);
    ---------------------------------------------------------------------[>]-*/

long env_get_number(const char *name,long default_value)
{
  char
    *variable_value;

  variable_value = env_get_string (name, NULL);
  return (variable_value? atol (variable_value): default_value);
}

#define IsCGIDelimiter(c)  (((c) == '&') || ((c) == '=') || ((c) == '\0'))

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
  /*
    Determine the number of arguments by scanning for delimiters
  */
  q=text;
  count=0;
  while (1)
  {
    int
      len;

    p=q;
    while (!IsCGIDelimiter(*q))
      q++;
    len=q-p;
    if (len > 0)
      count++;
    if (*q == '\0')
      break;
    q++;
  }
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
  vector[count+1]=(char *) NULL;
  q=text;
  i=1;
  while (i <= count)
  {
    int
      len;

    p=q;
    while (!IsCGIDelimiter(*q))
      q++;
    /*
       Skip an zero length tokens. This typically happens for the case
       of xxx=& on a CGI GET or POST were the name value pair has no
       value
    */
    len=q-p;
    if (len > 0)
      {
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
        http_unescape(vector[i], (char *) NULL);
        i++;
      }
    q++;
  }
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
    prefix[MaxTextExtent];

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
	if (getenv("GATEWAY_INTERFACE") || (argc>1))
    {
      FormatString(prefix,"HTTP/1.0 200 Ok\nContent-Type: %s\n\n","image/jpeg");
      if (!getenv("GATEWAY_INTERFACE"))
        status=CGIToArgv(argv[1],&argc,&argv);
      else
        {
          char
            *query;

          query=cgi_get_input(CGI_ANY);
          status=CGIToArgv(query,&argc,&argv);
          FreeMemory((void **) &query);
        }
      if (status == True)
        {
          for (argc_hw=1; argc_hw < argc; argc_hw++)
          {
            argv_hw = &argv[argc_hw];
            if (LocaleNCompare("-convert",argv[argc_hw],8) == 0)
              {
                for (i=argc_hw; i < argc; i++)
                {
                  if (LocaleNCompare("convert-",argv[i],8) == 0)
                    break;
                }
                convert_main(i-argc_hw,argv_hw,prefix,Extent(prefix));
                argc_hw = i+1;
                argv_hw = &argv[argc_hw];
              }
            else if (LocaleNCompare("-combine",argv[argc_hw],8) == 0)
              {
                argv_hw = &argv[argc_hw];
                for (i=argc_hw+1; i < argc; i++)
                {
                  if (LocaleNCompare("combine-",argv[i],8) == 0)
                    break;
                }
                combine_main(argc-argc_hw-1,argv_hw,prefix,Extent(prefix));
                argc_hw = i+1;
                argv_hw = &argv[argc_hw];
              }
          }
        }
    }
  FreeMemory((void **) &argv);
  Exit(0);
  return(False);
}
