/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             U   U  TTTTT  IIIII  L      IIIII  TTTTT  Y   Y                 %
%             U   U    T      I    L        I      T     Y Y                  %
%             U   U    T      I    L        I      T      Y                   %
%             U   U    T      I    L        I      T      Y                   %
%              UUU     T    IIIII  LLLLL  IIIII    T      Y                   %
%                                                                             %
%                                                                             %
%                       ImageMagick Utility Methods                           %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1993                                   %
%                                                                             %
%                                                                             %
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "Colorlist.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  A p p e n d I m a g e F o r m a t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AppendImageFormat appends the image format type to the filename.
%  If an extension to the file already exists, it is first removed.
%
%  The format of the AppendImageFormat routine is:
%
%       AppendImageFormat(format,filename)
%
%  A description of each parameter follows.
%
%   o  format:  Specifies a pointer to an array of characters.  This is the
%      format of the image.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
Export void AppendImageFormat(const char *format,char *filename)
{
  char
    staging[MaxTextExtent];

  register char
    *p;

  assert(format != (char *) NULL);
  assert(filename != (char *) NULL);
  if ((*format == '\0') || (*filename == '\0'))
    return;
  if (Latin1Compare(filename,"-") == 0)
    {
      FormatString(staging,"%.1024s:%.1024s",format,filename);
      (void) strcpy(filename,staging);
      return;
    }
  p=filename+Extent(filename)-1;
  while ((p > (filename+1)) && !IsBasenameSeparator(*p))
  {
    if (*(p-1) == '.')
      {
        (void) strcpy(p,format);
        return;
      }
    p--;
  }
  (void) strcat(filename,".");
  (void) strcat(filename,format);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a s e F i l e n a m e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BaseFilename removes the path name component and any extensions.
%
%  The format of the BaseFilename function is:
%
%      BaseFilename(name)
%
%  A description of each parameter follows:
%
%    o name: Specifies a pointer to an character array that contains the
%      name.
%
%
*/
Export char *BaseFilename(const char *name)
{
  register char
    *p;

  static char
    basename[MaxTextExtent];

  /*
    Get basename of client.
  */
  assert(name != (char *) NULL);
  (void) strcpy(basename,name);
  p=basename+(Extent(basename)-1);
  while (p > basename)
  {
    if (IsBasenameSeparator(*p))
      {
        (void) strcpy(basename,p+1);
        break;
      }
    p--;
  }
  /*
    Delete any extension.
  */
  p=basename+(Extent(basename)-1);
  while (p > basename)
  {
    if (*p == '.')
      {
        *p='\0';
        break;
      }
    p--;
  }
  return(basename);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e S t r i n g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneString allocates memory for the destination string and copies
%  the source string to that memory location.
%
%  The format of the CloneString routine is:
%
%      CloneString(destination,source)
%
%  A description of each parameter follows:
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
%
*/
void CloneString(char **destination,char *source)
{
  if (*destination != (char *) NULL)
    FreeMemory(*destination);
  *destination=(char *) AllocateMemory((Extent(source)+1)*sizeof(char));
  if (*destination == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to allocate string",
        "Memory allocation failed");
      return;
    }
  (void) strcpy(*destination,source);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n v e r t T e x t T o U n i c o d e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConvertTextToUnicode converts an ASCII text string to Unicode and
%  returns the Unicode translation and the character count.  Normal characters
%  are just copied.  However, escaped hex codes are interpreted before
%  converting to Unicode (e.g. \0x30a3 is 12451 in the Unicode character set).
%
%  The format of the ConvertTextToUnicode function is:
%
%      unicode=ConvertTextToUnicode(text,count)
%
%  A description of each parameter follows:
%
%    o unicode:  Method ConvertTextToUnicode returns a pointer to an
%      unsigned short array representing the Unicode translation of the
%      ASCII string.
%
%    o text: Specifies a pointer to an character array that contains the
%      text to convert.
%
%    o count: The number of characters that were translated from ASCII to
%      Unicode is returned in this unsigned integer pointer.
%
%
*/

static int InterpretUnicode(char *code,int n)
{
  int
    total,
    value;

  register char
    c;

  register int
    i;

  if (!code)
    return(-1);
  if (n >= (int) (2*sizeof(int)))
    return(-1);
  total=0;
  value=0;
  for (i=0; i < n; i++)
  {
    c=code[i];
    if (c == '\0')
      return(-1);
    if ((c >= '0') && (c <= '9'))
      value=(unsigned char) c-48;
    else
      if ((c >= 'A') && (c <= 'F'))
        value=(unsigned char) c-65+10;
      else
        if ((c >= 'a') && (c <= 'f'))
          value=(unsigned char) c-97+10;
        else
          return(-1);
    total*=16;
    total+=value;
  }
  return(total);
}

unsigned short *ConvertTextToUnicode(char *text,unsigned int *count)
{
  int
    value;

  register char
    *p;

  register unsigned short
    *q;

  unsigned short
    *unicode;

  *count=0;
  if ((text == (char *) NULL) || (*text == '\0'))
    return((unsigned short*) NULL);
  unicode=(unsigned short *)
    AllocateMemory(strlen(text)*sizeof(unsigned short *));
  if (unicode == (unsigned short *) NULL)
    return((unsigned short*) NULL);
  p=text;
  q=unicode;
  while (*p != '\0')
  {
    *q=(*p);
    if (strncmp(p,"\\0x",3) == 0)
      {
        p+=3;
        value=InterpretUnicode(p,4);
        if (value < 0)
          {
            FreeMemory((char *) unicode);
            return((unsigned short *) NULL);
          }
        *q=(unsigned short) value;
        p+=3;
      }
    p++;
    q++;
  }
  *count=q-unicode;
  return(unicode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilename expands '~' in a filename.
%
%  The format of the ExpandFilename function is:
%
%      ExpandFilename(filename)
%
%  A description of each parameter follows:
%
%    o filename: Specifies a pointer to an character array that contains the
%      filename.
%
%
*/
Export void ExpandFilename(char *filename)
{
  char
    expanded_filename[MaxTextExtent];

  register char
    *p;

  if (filename == (char *) NULL)
    return;
  if (*filename != '~')
    return;
  (void) strcpy(expanded_filename,filename);
  if (*(filename+1) == '/')
    {
      /*
        Substitute ~ with $HOME.
      */
      p=(char *) getenv("HOME");
      if (p == (char *) NULL)
        p=".";
      (void) strcpy(expanded_filename,p);
      (void) strcat(expanded_filename,filename+1);
    }
  else
    {
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
      char
        username[MaxTextExtent];

      struct passwd
        *entry;

      /*
        Substitute ~ with home directory from password file.
      */
      (void) strcpy(username,filename+1);
      p=strchr(username,'/');
      if (p != (char *) NULL)
        *p='\0';
      entry=getpwnam(username);
      if (entry == (struct passwd *) NULL)
        return;
      (void) strcpy(expanded_filename,entry->pw_dir);
      if (p != (char *) NULL)
        {
          (void) strcat(expanded_filename,"/");
          (void) strcat(expanded_filename,p+1);
        }
#endif
    }
  (void) strcpy(filename,expanded_filename);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilenames checks each argument of the command line vector and
%  expands it if they have a wildcard character.  For example, *.jpg might
%  expand to:  bird.jpg rose.jpg tiki.jpg.
%
%  The format of the ExpandFilenames function is:
%
%      status=ExpandFilenames(argc,argv)
%
%  A description of each parameter follows:
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%
*/
Export unsigned int ExpandFilenames(int *argc,char ***argv)
{
  char
    **filelist,
    home_directory[MaxTextExtent],
    *option,
    **vector,
    working_directory[MaxTextExtent];

  int
    count,
    expanded,
    number_files;

  register char
    *p,
    *q;

  register int
    i,
    j;

  /*
    Allocate argument vector.
  */
  assert(argc != (int *) NULL);
  assert(argv != (char ***) NULL);
  vector=(char **) AllocateMemory((*argc+1)*sizeof(char *));
  for (i=1; i < *argc; i++)
    if (Extent((*argv)[i]) > (MaxTextExtent/2-1))
      {
        MagickWarning(OptionWarning,"Token length exceeds limit",(*argv)[i]);
        return(False);
      }
  if (vector == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to expand filenames",
        (char *) NULL);
      return(False);
    }
  /*
    Expand any wildcard filenames.
  */
  (void) getcwd(home_directory,MaxTextExtent-1);
  expanded=False;
  count=0;
  for (i=0; i < *argc; i++)
  {
    option=(*argv)[i];
    vector[count++]=option;
    if ((Extent(option) > 1) && ((*option == '-') || (*option == '+')))
      continue;
    if ((*option == '"') || (*option == '\''))
      continue;
    if (!IsGlob(option))
      {
        /*
          Silently skip directories.
        */
        if (IsDirectory(option))
          count--;
        continue;
      }
    /*
      Get the list of image file names.
    */
    (void) getcwd(working_directory,MaxTextExtent-1);
    for (p=option+Extent(option)-1; p > option; p--)
      if (IsBasenameSeparator(*p))
        {
          /*
            Filename includes a directory name.
          */
          q=working_directory;
          for (j=0; j < (p-option+1); j++)
            *q++=option[j];
          *q='\0';
          p++;
          break;
        }
    filelist=ListFiles(working_directory,p,&number_files);
    if (filelist == (char **) NULL)
      continue;
    for (j=0; j < number_files; j++)
      if (!IsDirectory(filelist[j]))
        break;
    if (j == number_files)
      {
        for (j=0; j < number_files; j++)
          FreeMemory((char *) filelist[j]);
        FreeMemory((char *) filelist);
        continue;
      }
    /*
      Transfer file list to argument vector.
    */
    vector=(char **)
      ReallocateMemory(vector,(*argc+count+number_files)*sizeof(char *));
    if (vector == (char **) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to expand filenames",
          (char *) NULL);
        return(False);
      }
    count--;
    for (j=0; j < number_files; j++)
    {
      if (IsDirectory(filelist[j]))
        {
          FreeMemory((char *) filelist[j]);
          continue;
        }
      expanded=True;
      vector[count]=(char *) AllocateMemory(((p-option)+Extent(filelist[j])+
        MaxTextExtent+1)*sizeof(char));
      if (vector[count] == (char *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to expand filenames",
            (char *) NULL);
          for ( ; j < number_files; j++)
            FreeMemory((char *) filelist[j]);
          FreeMemory((char *) filelist);
          return(False);
        }
      FormatString(vector[count],"%.*s%.1024s",(int) (p-option),option,
        filelist[j]);
      FreeMemory((char *) filelist[j]);
      count++;
    }
    FreeMemory((char *) filelist);
  }
  (void) chdir(home_directory);
  if (!expanded)
    {
      FreeMemory((char *) vector);
      return(False);
    }
  *argc=count;
  *argv=vector;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t S t r i n g                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FormatString prints formatted output of a variable argument list.
%
%  The format of the FormatString routine is:
%
%       FormatString(string,format,...)
%
%  A description of each parameter follows.
%
%   o  string:  Method FormatString returns the formatted string in this
%      character buffer.
%
%   o  format:  A string describing the format to use to write the remaining
%      arguments.
%
%
*/
Export void FormatString(char *string,const char *format,...)
{
  va_list
    operands;

  va_start(operands,format);
#if !defined(HAVE_VSNPRINTF)
  (void) vsprintf(string,format,operands);
#else
  (void) vsnprintf(string,MaxTextExtent,format,operands);
  va_end(operands);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G l o b E x p r e s s i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GlobExpression returns True if the expression matches the pattern.
%
%  The format of the GlobExpression function is:
%
%      GlobExpression(expression,pattern)
%
%  A description of each parameter follows:
%
%    o expression: Specifies a pointer to a text string containing a file name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%
*/
Export int GlobExpression(char *expression,const char *pattern)
{
  ImageInfo
    image_info;

  unsigned int
    done,
    exempt;

  /*
    Return on empty pattern or '*'.
  */
  if (pattern == (char *) NULL)
    return(True);
  if (Extent(pattern) == 0)
    return(True);
  if (Latin1Compare(pattern,"*") == 0)
    return(True);
  /*
    Determine if pattern is a subimage, i.e. img0001.pcd[2].
  */
  GetImageInfo(&image_info);
  (void) strcpy(image_info.filename,pattern);
  SetImageInfo(&image_info,True);
  exempt=(Latin1Compare(image_info.magick,"VID") == 0) ||
    (image_info.subimage &&
    (Latin1Compare(expression,image_info.filename) == 0));
  if (exempt)
    return(False);
  /*
    Evaluate glob expression.
  */
  done=False;
  while ((*pattern != '\0') && !done)
  {
    if (*expression == '\0')
      if ((*pattern != '{') && (*pattern != '*'))
        break;
    switch (*pattern)
    {
      case '\\':
      {
        pattern++;
        if (*pattern != '\0')
          pattern++;
        break;
      }
      case '*':
      {
        int
          status;

        pattern++;
        status=False;
        while ((*expression != '\0') && !status)
          status=GlobExpression((char *) expression++,pattern);
        if (status)
          {
            while (*expression != '\0')
              expression++;
            while (*pattern != '\0')
              pattern++;
          }
        break;
      }
      case '[':
      {
        char
          c;

        pattern++;
        for ( ; ; )
        {
          if ((*pattern == '\0') || (*pattern == ']'))
            {
              done=True;
              break;
            }
          if (*pattern == '\\')
            {
              pattern++;
              if (*pattern == '\0')
                {
                  done=True;
                  break;
                }
             }
          if (*(pattern+1) == '-')
            {
              c=(*pattern);
              pattern+=2;
              if (*pattern == ']')
                {
                  done=True;
                  break;
                }
              if (*pattern == '\\')
                {
                  pattern++;
                  if (*pattern == '\0')
                    {
                      done=True;
                      break;
                    }
                }
              if ((*expression < c) || (*expression > *pattern))
                {
                  pattern++;
                  continue;
                }
            }
          else
            if (*pattern != *expression)
              {
                pattern++;
                continue;
              }
          pattern++;
          while ((*pattern != ']') && (*pattern != '\0'))
          {
            if ((*pattern == '\\') && (*(pattern+1) != '\0'))
              pattern++;
            pattern++;
          }
          if (*pattern != '\0')
            {
              pattern++;
              expression++;
            }
          break;
        }
        break;
      }
      case '?':
      {
        pattern++;
        expression++;
        break;
      }
      case '{':
      {
        int
          match;

        register char
          *p;

        pattern++;
        while ((*pattern != '}') && (*pattern != '\0'))
        {
          p=expression;
          match=True;
          while ((*p != '\0') && (*pattern != '\0') &&
                 (*pattern != ',') && (*pattern != '}') && match)
          {
            if (*pattern == '\\')
              pattern++;
            match=(*pattern == *p);
            p++;
            pattern++;
          }
          if (*pattern == '\0')
            {
              match=False;
              done=True;
              break;
            }
          else
            if (match)
              {
                expression=p;
                while ((*pattern != '}') && (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if (*pattern == '}')
                        pattern++;
                    }
                }
              }
            else
              {
                while ((*pattern != '}') && (*pattern != ',') &&
                       (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if ((*pattern == '}') || (*pattern == ','))
                        pattern++;
                    }
                }
              }
            if (*pattern != '\0')
              pattern++;
          }
        break;
      }
      default:
      {
        if (*expression != *pattern)
          done=True;
        else
          {
            expression++;
            pattern++;
          }
      }
    }
  }
  while (*pattern == '*')
    pattern++;
  return((*expression == '\0') && (*pattern == '\0'));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s A c c e s s i b l e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsAccessible returns True if the file as defined by filename is
%  accessible.
%
%  The format of the IsAccessible routine is:
%
%       status=IsAccessible(filename)
%
%  A description of each parameter follows.
%
%   o  status:  Method IsAccessible returns True is the file as defined by
%      filename is accessible, otherwise False is returned.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
Export unsigned int IsAccessible(const char *filename)
{
  FILE
    *file;

  unsigned int
    status;

  /*
    Return False if the file cannot be opened.
  */
  assert(filename != (char *) NULL);
  file=fopen(filename,ReadBinaryType);
  if (file == (FILE *) NULL)
    return(False);
  (void) fgetc(file);
  status=!feof(file) && !ferror(file);
  (void) fclose(file);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s D i r e c t o r y                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDirectory returns True if the file as defined by filename is
%  a directory.  Once MetroWerks write a stat(2) function, we can remove the
%  chdir(2) function.
%
%  The format of the IsAccessible routine is:
%
%       status=IsDirectory(filename)
%
%  A description of each parameter follows.
%
%   o  status:  Method IsDirectory returns True is the file as defined by
%      filename is a directory, otherwise False is returned.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
Export unsigned int IsDirectory(const char *filename)
{
  int
    status;

#if !defined(WIN32)
  struct stat
    file_info;

  status=stat(filename,&file_info);
  if (status != 0)
    return(False);
  return(S_ISDIR(file_info.st_mode));
#else
  char
    current_directory[MaxTextExtent];

  (void) getcwd(current_directory,MaxTextExtent-1);
  status=chdir(filename);
  if (status == 0)
    (void) chdir(current_directory);
  return(status == 0);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 C o m p a r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Latin1Compare compares two null terminated Latin-1 strings,
%  ignoring case differences, and returns an integer greater than, equal
%  to, or less than 0, according to whether first is lexicographically
%  greater than, equal to, or less than second.  The two strings are
%  assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Compare routine is:
%
%      Latin1Compare(p,q)
%
%  A description of each parameter follows:
%
%    o p: A pointer to the string to convert to Latin1 string.
%
%    o q: A pointer to the string to convert to Latin1 string.
%
%
*/
Export int Latin1Compare(const char *p,const char *q)
{
  register int
    i,
    j;

  if (p == q)
    return(0);
  if (p == (char *) NULL)
    return(-1);
  if (q == (char *) NULL)
    return(1);
  while ((*p != '\0') && (*q != '\0'))
  {
    i=(*p);
    if (islower(i))
      i=toupper(i);
    j=(*q);
    if (islower(j))
      j=toupper(j);
    if (i != j)
      break;
    p++;
    q++;
  }
  return(*p-(*q));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   L a t i n 1 U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Latin1Upper copies a null terminated string from source to
%  destination (including the null), changing all Latin-1 lowercase letters
%  to uppercase.  The string is assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Upper routine is:
%
%      Latin1Upper(string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Latin1.
%
%
*/
Export void Latin1Upper(char *string)
{
  register int
    c;

  assert(string != (char *) NULL);
  for ( ; *string != '\0'; string++)
  {
    c=(*string);
    if (isupper(c))
      continue;
    c=toupper(c);
    *string=c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t C o l o r s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListColors reads the X client color database and returns a list
%  of colors contained in the database sorted in ascending alphabetic order.
%
%  The format of the ListColors function is:
%
%      filelist=ListColors(pattern,number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method ListColors returns a list of colors contained
%      in the database.  If the database cannot be read, a NULL list is
%      returned.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/

static int ColorCompare(const void *x,const void *y)
{
  register char
    **p,
    **q;

  p=(char **) x;
  q=(char **) y;
  return(Latin1Compare(*p,*q));
}

Export char **ListColors(const char *pattern,int *number_colors)
{
  char
    color[MaxTextExtent],
    **colorlist,
    text[MaxTextExtent];

  FILE
    *database;

  int
    blue,
    count,
    green,
    red;

  unsigned int
    max_colors;

  /*
    Allocate color list.
  */
  assert(pattern != (char *) NULL);
  assert(number_colors != (int *) NULL);
  max_colors=sizeof(Colorlist)/sizeof(XColorlist);
  colorlist=(char **) AllocateMemory(max_colors*sizeof(char *));
  if (colorlist == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to read color name database",
        "Memory allocation failed");
      return((char **) NULL);
    }
  /*
    Open database.
  */
  *number_colors=0;
  database=fopen(RGBColorDatabase,"r");
  if (database == (FILE *) NULL)
    {
      register const XColorlist
        *p;

      /*
        Can't find server color database-- use our color list.
      */
      for (p=Colorlist; p->name != (char *) NULL; p++)
        if (GlobExpression(p->name,pattern))
          {
            colorlist[*number_colors]=(char *)
              AllocateMemory(Extent(p->name)+1);
            if (colorlist[*number_colors] == (char *) NULL)
              break;
            (void) strcpy(colorlist[*number_colors],p->name);
            (*number_colors)++;
          }
      return(colorlist);
    }
  while (fgets(text,MaxTextExtent,database) != (char *) NULL)
  {
    count=sscanf(text,"%d %d %d %[^\n]\n",&red,&green,&blue,color);
    if (count != 4)
      continue;
    if (GlobExpression(color,pattern))
      {
        if (*number_colors >= (int) max_colors)
          {
            max_colors<<=1;
            colorlist=(char **)
              ReallocateMemory((char **) colorlist,max_colors*sizeof(char *));
            if (colorlist == (char **) NULL)
              {
                MagickWarning(ResourceLimitWarning,
                  "Unable to read color name database",
                  "Memory allocation failed");
                (void) fclose(database);
                return((char **) NULL);
              }
          }
        colorlist[*number_colors]=(char *) AllocateMemory(Extent(color)+1);
        if (colorlist[*number_colors] == (char *) NULL)
          break;
        (void) strcpy(colorlist[*number_colors],color);
        (*number_colors)++;
      }
  }
  (void) fclose(database);
  /*
    Sort colorlist in ascending order.
  */
  qsort((void *) colorlist,*number_colors,sizeof(char **),
    (int (*)(const void *, const void *)) ColorCompare);
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t F i l e s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListFiles reads the directory specified and returns a list
%  of filenames contained in the directory sorted in ascending alphabetic
%  order.
%
%  The format of the ListFiles function is:
%
%      filelist=ListFiles(directory,pattern,number_entries)
%
%  A description of each parameter follows:
%
%    o filelist: Method ListFiles returns a list of filenames contained
%      in the directory.  If the directory specified cannot be read or it is
%      a file a NULL list is returned.
%
%    o directory: Specifies a pointer to a text string containing a directory
%      name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_entries:  This integer returns the number of filenames in the
%      list.
%
%
*/

static int FileCompare(const void *x,const void *y)
{
  register char
    **p,
    **q;

  p=(char **) x;
  q=(char **) y;
  return(Latin1Compare(*p,*q));
}

Export char **ListFiles(char *directory,const char *pattern,int *number_entries)
{
  char
    **filelist;

  DIR
    *current_directory;

  int
    status;

  struct dirent
    *entry;

  unsigned int
    max_entries;

  /*
    Open directory.
  */
  assert(directory != (char *) NULL);
  assert(pattern != (char *) NULL);
  assert(number_entries != (int *) NULL);
  *number_entries=0;
  status=chdir(directory);
  if (status != 0)
    return((char **) NULL);
  (void) getcwd(directory,MaxTextExtent-1);
  current_directory=opendir(directory);
  if (current_directory == (DIR *) NULL)
    return((char **) NULL);
  /*
    Allocate filelist.
  */
  max_entries=2048;
  filelist=(char **) AllocateMemory(max_entries*sizeof(char *));
  if (filelist == (char **) NULL)
    {
      (void) closedir(current_directory);
      return((char **) NULL);
    }
  /*
    Save the current and change to the new directory.
  */
  (void) chdir(directory);
  entry=readdir(current_directory);
  while (entry != (struct dirent *) NULL)
  {
    if (*entry->d_name == '.')
      {
        entry=readdir(current_directory);
        continue;
      }
    if (IsDirectory(entry->d_name) || GlobExpression(entry->d_name,pattern))
      {
        if (*number_entries >= (int) max_entries)
          {
            /*
              Extend the file list.
            */
            max_entries<<=1;
            filelist=(char **)
              ReallocateMemory((char **) filelist,max_entries*sizeof(char *));
            if (filelist == (char **) NULL)
              {
                (void) closedir(current_directory);
                return((char **) NULL);
              }
          }
#if defined(vms)
        {
          register char
            *p;

          p=strchr(entry->d_name,';');
          if (p)
            *p='\0';
          if (*number_entries > 0)
            if (Latin1Compare(entry->d_name,filelist[*number_entries-1]) == 0)
              {
                entry=readdir(current_directory);
                continue;
              }
        }
#endif
        filelist[*number_entries]=(char *)
          AllocateMemory(Extent(entry->d_name)+2);
        if (filelist[*number_entries] == (char *) NULL)
          break;
        (void) strcpy(filelist[*number_entries],entry->d_name);
        if (IsDirectory(entry->d_name))
          (void) strcat(filelist[*number_entries],DirectorySeparator);
        (*number_entries)++;
      }
    entry=readdir(current_directory);
  }
  (void) closedir(current_directory);
  /*
    Sort filelist in ascending order.
  */
  qsort((void *) filelist,*number_entries,sizeof(char **),
    (int (*)(const void *, const void *)) FileCompare);
  return(filelist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L o c a l e F i l e n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleFilename replaces the contents of the string pointed to
%  by filename by a unique file name relative to the directory.
%
%  The format of the LocaleFilename routine is:
%
%       LocaleFilename(filename)
%
%  A description of each parameter follows.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
Export void LocaleFilename(char *filename)
{
  register char
    *p,
    *q;

  assert(filename != (char *) NULL);
  p=filename+Extent(filename)-1;
  while ((p > filename) && !IsBasenameSeparator(*p))
    p--;
  p++;
  TemporaryFilename(p);
  q=filename+Extent(filename)-1;
  while ((q >= p) && !IsBasenameSeparator(*q))
    q--;
  q++;
  (void) strcpy(p,q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L S B F i r s t R e a d L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstReadLong reads a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstReadLong routine is:
%
%       value=LSBFirstReadLong(file)
%
%  A description of each parameter follows.
%
%    o value:  Method LSBFirstReadLong returns an unsigned long read from
%      the file.
%
%   o  file:  Specifies the file to read the data from.
%
%
*/
Export unsigned long LSBFirstReadLong(FILE *file)
{
  unsigned char
    buffer[4];

  unsigned int
    status;

  unsigned long
    value;

  assert(file != (FILE *) NULL);
  status=ReadData((char *) buffer,1,4,file);
  if (status == False)
    return((unsigned long) ~0);
  value=(unsigned int) (buffer[3] << 24);
  value|=(unsigned int) (buffer[2] << 16);
  value|=(unsigned int) (buffer[1] << 8);
  value|=(unsigned int) (buffer[0]);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L S B F i r s t R e a d S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstReadShort reads a short value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstReadShort routine is:
%
%       value=LSBFirstReadShort(file)
%
%  A description of each parameter follows.
%
%    o value:  Method LSBFirstReadShort returns an unsigned short read from
%      the file.
%
%   o  file:  Specifies the file to read the data from.
%
%
*/
Export unsigned short LSBFirstReadShort(FILE *file)
{
  unsigned char
    buffer[2];

  unsigned int
    status;

  unsigned short
    value;

  assert(file != (FILE *) NULL);
  status=ReadData((char *) buffer,1,2,file);
  if (status == False)
    return((unsigned short) ~0);
  value=(unsigned short) (buffer[1] << 8);
  value|=(unsigned short) (buffer[0]);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L S B F i r s t W r i t e L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstWriteLong writes a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstWriteLong routine is:
%
%       LSBFirstWriteLong(value,file)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
Export void LSBFirstWriteLong(const unsigned long value,FILE *file)
{
  unsigned char
    buffer[4];

  assert(file != (FILE *) NULL);
  buffer[0]=(unsigned char) (value);
  buffer[1]=(unsigned char) ((value) >> 8);
  buffer[2]=(unsigned char) ((value) >> 16);
  buffer[3]=(unsigned char) ((value) >> 24);
  (void) fwrite((char *) buffer,1,4,file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L S B F i r s t W r i t e S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LSBFirstWriteShort writes a long value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstWriteShort routine is:
%
%       LSBFirstWriteShort(value,file)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
Export void LSBFirstWriteShort(const unsigned int value,FILE *file)
{
  unsigned char
    buffer[2];

  assert(file != (FILE *) NULL);
  buffer[0]=(unsigned char) (value);
  buffer[1]=(unsigned char) ((value) >> 8);
  (void) fwrite((char *) buffer,1,2,file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t O r d e r L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstOrderLong converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBFirstOrderLong routine is:
%
%       MSBFirstOrderLong(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
Export void MSBFirstOrderLong(register char *p,const unsigned int length)
{
  register char
    c,
    *q,
    *sp;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    sp=p+3;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    sp=p+1;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    p+=2;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t O r d e r S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstOrderShort converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBFirstOrderShort routine is:
%
%       MSBFirstOrderLongShort(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
Export void MSBFirstOrderShort(register char *p,const unsigned int length)
{
  register char
    c,
    *q;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t R e a d S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstReadShort reads a short value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstReadShort routine is:
%
%       value=MSBFirstReadShort(file)
%
%  A description of each parameter follows.
%
%    o value:  Method MSBFirstReadShort returns an unsigned short read from
%      the file.
%
%   o  file:  Specifies the file to read the data from.
%
%
*/
Export unsigned short MSBFirstReadShort(FILE *file)
{
  unsigned char
    buffer[2];

  unsigned int
    status;

  unsigned short
    value;

  assert(file != (FILE *) NULL);
  status=ReadData((char *) buffer,1,2,file);
  if (status == False)
    return((unsigned short) ~0);
  value=(unsigned int) (buffer[0] << 8);
  value|=(unsigned int) (buffer[1]);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t R e a d L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstReadLong reads a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstReadLong routine is:
%
%       value=MSBFirstReadLong(file)
%
%  A description of each parameter follows.
%
%    o value:  Method MSBFirstReadLong returns an unsigned long read from
%      the file.
%
%   o  file:  Specifies the file to read the data from.
%
%
*/
Export unsigned long MSBFirstReadLong(FILE *file)
{
  unsigned char
    buffer[4];

  unsigned int
    status;

  unsigned long
    value;

  assert(file != (FILE *) NULL);
  status=ReadData((char *) buffer,1,4,file);
  if (status == False)
    return((unsigned long) ~0);
  value=(unsigned int) (buffer[0] << 24);
  value|=(unsigned int) (buffer[1] << 16);
  value|=(unsigned int) (buffer[2] << 8);
  value|=(unsigned int) (buffer[3]);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t W r i t e L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstWriteLong writes a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstWriteLong routine is:
%
%       MSBFirstWriteLong(value,file)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
Export void MSBFirstWriteLong(const unsigned long value,FILE *file)
{
  unsigned char
    buffer[4];

  assert(file != (FILE *) NULL);
  buffer[0]=(unsigned char) ((value) >> 24);
  buffer[1]=(unsigned char) ((value) >> 16);
  buffer[2]=(unsigned char) ((value) >> 8);
  buffer[3]=(unsigned char) (value);
  (void) fwrite((char *) buffer,1,4,file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t W r i t e S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBFirstWriteShort writes a long value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the MSBFirstWriteShort routine is:
%
%       MSBFirstWriteShort(value,file)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
Export void MSBFirstWriteShort(const unsigned int value,FILE *file)
{
  unsigned char
    buffer[2];

  assert(file != (FILE *) NULL);
  buffer[0]=(unsigned char) ((value) >> 8);
  buffer[1]=(unsigned char) (value);
  (void) fwrite((char *) buffer,1,2,file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M u l t i l i n e C e n s u s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MultilineCensus returns the number of lines within a label.  A line
%  is represented by a \n character.
%
%  The format of the MultilineCenus routine is:
%
%       MultilineCenus(label)
%
%  A description of each parameter follows.
%
%   o  label:  This character string is the label.
%
%
*/
Export int MultilineCensus(const char *label)
{
  int
    number_lines;

  /*
    Determine the number of lines within this label.
  */
  if (label == (char *) NULL)
    return(0);
  for (number_lines=1; *label != '\0'; label++)
    if (*label == '\n')
      number_lines++;
  return(number_lines);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  P o s t s c r i p t G e o m e t r y                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PostscriptGeometry replaces any page mneumonic with the equivalent
%  size in picas.
%
%  The format of the PostscriptGeometry routine is:
%
%       geometry=PostscriptGeometry(page)
%
%  A description of each parameter follows.
%
%   o  page:  Specifies a pointer to an array of characters.  The string is
%      either a Postscript page name (e.g. A4) or a postscript page geometry
%      (e.g. 612x792+36+36).
%
%
*/

Export void DestroyPostscriptGeometry(char *geometry)
{
  free(geometry);
}

Export char *PostscriptGeometry(const char *page)
{
  static const char
    *PageSizes[][2]=
    {
      { "11x17", "792x1224>" },
      { "Ledger", "1224x792>" },
      { "Legal", "612x1008>" },
      { "Letter", "612x792>" },
      { "LetterSmall", "612x792>" },
      { "ArchE", "2592x3456>" },
      { "ArchD", "1728x2592>" },
      { "ArchC", "1296x1728>" },
      { "ArchB", "864x1296>" },
      { "ArchA", "648x864>" },
      { "A0", "2380x3368>" },
      { "A1", "1684x2380>" },
      { "A2", "1190x1684>" },
      { "A3", "842x1190>" },
      { "A4", "595x842>" },
      { "A4Small", "595x842>" },
      { "A5", "421x595>" },
      { "A6", "297x421>" },
      { "A7", "210x297>" },
      { "A8", "148x210>" },
      { "A9", "105x148>" },
      { "A10", "74x105>" },
      { "B0", "2836x4008>" },
      { "B1", "2004x2836>" },
      { "B2", "1418x2004>" },
      { "B3", "1002x1418>" },
      { "B4", "709x1002>" },
      { "B5", "501x709>" },
      { "C0", "2600x3677>" },
      { "C1", "1837x2600>" },
      { "C2", "1298x1837>" },
      { "C3", "918x1298>" },
      { "C4", "649x918>" },
      { "C5", "459x649>" },
      { "C6", "323x459>" },
      { "Flsa", "612x936>" },
      { "Flse", "612x936>" },
      { "HalfLetter", "396x612>" },
      { (char *) NULL, (char *) NULL }
    };

  char
    c,
    *geometry;

  register char
    *p;

  register int
    i;

  /*
    Allocate page geometry memory.
  */
  geometry=(char *) AllocateMemory((Extent(page)+MaxTextExtent)*sizeof(char));
  if (geometry == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to translate page geometry",
        "Memory allocation failed");
      return((char *) NULL);
    }
  *geometry='\0';
  if (page == (char *) NULL)
    return(geometry);
  /*
    Comparison is case insensitive.
  */
  (void) strcpy(geometry,page);
  if (!isdigit((int) (*geometry)))
    for (p=geometry; *p != '\0'; p++)
    {
      c=(*p);
      if (islower((int) c))
        *p=toupper(c);
    }
  /*
    Comparison is case insensitive.
  */
  for (i=0; *PageSizes[i] != (char *) NULL; i++)
    if (strncmp(PageSizes[i][0],geometry,Extent(PageSizes[i][0])) == 0)
      {
        /*
          Replace mneumonic with the equivalent size in dots-per-inch.
        */
        (void) strcpy(geometry,PageSizes[i][1]);
        (void) strcat(geometry,page+Extent(PageSizes[i][0]));
        break;
      }
  return(geometry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadData reads data from the image file and returns it.  If it
%  cannot read the requested number of items, False is returned indicating
%  an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,size,number_items,file)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadData returns True if all the data requested
%      is obtained without error, otherwise False.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o size:  Specifies an integer representing the length of an
%      individual item to be read from the file.
%
%    o number_items:  Specifies an integer representing the number of items
%      to read from the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
Export unsigned int ReadData(char *data,const unsigned int size,
  const unsigned int number_items,FILE *file)
{
  long
    bytes,
    count;

  assert(data != (char *) NULL);
  assert(file != (FILE *) NULL);
  count=0;
  for (bytes=size*number_items; bytes > 0; bytes-=count)
  {
    count=(long) fread(data,1,bytes,file);
    if (count <= 0)
      return(False);
    data+=count;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDataBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  If
%  ReadDataBlock cannot read the requested number of items, `-1' is returned
%  indicating an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,file)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadData returns the number of characters read
%      unless there is an error, otherwise `-1'.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
Export int ReadDataBlock(char *data,FILE *file)
{
  unsigned char
    count;

  unsigned int
    status;

  assert(data != (char *) NULL);
  assert(file != (FILE *) NULL);
  status=ReadData((char *) &count,1,1,file);
  if (status == False)
    return(-1);
  if (count == 0)
    return(0);
  status=ReadData(data,1,(unsigned int) count,file);
  if (status == False)
    return(-1);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientName sets the client name if the name is specified.
%  Otherwise the current client name is returned.
%
%  The format of the SetClientName routine is:
%
%      client_name=SetClientName(name)
%
%  A description of each parameter follows:
%
%    o client_name: Method SetClientName returns the current client name.
%
%    o status: Specifies the new client name.
%
%
*/
Export char *SetClientName(const char *name)
{
  static char
    client_name[MaxTextExtent] = "Magick";

  if (name != (char *) NULL)
    {
      (void) strcpy(client_name,BaseFilename(name));
      setlocale(LC_ALL,"");
      setlocale(LC_NUMERIC,"C");
    }
  return(client_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o A r g v                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToArgv converts a text string into command line arguments.
%
%  The format of the StringToArgv routine is:
%
%      argv=StringToArgv(text,argc)
%
%  A description of each parameter follows:
%
%    o argv:  Method StringToArgv returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%    o argc:  This integer pointer returns the number of arguments in the
%      list.
%
%
*/
Export char **StringToArgv(const char *text,int *argc)
{
  char
    **argv;

  register char
    *p,
    *q;

  register int
    i;

  *argc=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int) (*p)))
      p++;
    (*argc)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  (*argc)++;
  argv=(char **) AllocateMemory((*argc+1)*sizeof(char *));
  if (argv == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to convert text",
        "Memory allocation failed");
      return((char **) NULL);
    }
  /*
    Convert string to an ASCII list.
  */
  argv[0]="magick";
  p=(char *) text;
  for (i=1; i < *argc; i++)
  {
    while (isspace((int) (*p)))
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++);
      }
    else
      if (*p == '\'')
        {
          p++;
          for (q++; (*q != '\'') && (*q != '\0'); q++);
        }
      else
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
    argv[i]=(char *) AllocateMemory((q-p+1)*sizeof(char));
    if (argv[i] == (char *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Unable to convert text",
          "Memory allocation failed");
        return((char **) NULL);
      }
    (void) strncpy(argv[i],p,q-p);
    argv[i][q-p]='\0';
    p=q;
    while (!isspace((int) (*p)) && (*p != '\0'))
      p++;
  }
  argv[i]=(char *) NULL;
  return(argv);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o L i s t                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToList converts a text string into a list by segmenting the
%  text string at each carriage return discovered.  The list is converted to
%  HEX characters if any control characters are discovered within the text
%  string.
%
%  The format of the StringToList routine is:
%
%      list=StringToList(text)
%
%  A description of each parameter follows:
%
%    o list:  Method StringToList returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%
*/
Export char **StringToList(char *text)
{
  char
    **textlist;

  register char
    *p,
    *q;

  register int
    i;

  unsigned int
    lines;

  if (text == (char *) NULL)
    return((char **) NULL);
  for (p=text; *p != '\0'; p++)
    if (((unsigned char) *p < 32) && !isspace((int) (*p)))
      break;
  if (*p == '\0')
    {
      /*
        Convert string to an ASCII list.
      */
      lines=1;
      for (p=text; *p != '\0'; p++)
        if (*p == '\n')
          lines++;
      textlist=(char **) AllocateMemory((lines+1)*sizeof(char *));
      if (textlist == (char **) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to convert text",
            "Memory allocation failed");
          return((char **) NULL);
        }
      p=text;
      for (i=0; i < (int) lines; i++)
      {
        for (q=p; *q != '\0'; q++)
          if ((*q == '\r') || (*q == '\n'))
            break;
        textlist[i]=(char *) AllocateMemory((q-p+1)*sizeof(char));
        if (textlist[i] == (char *) NULL)
          {
            MagickWarning(ResourceLimitWarning,"Unable to convert text",
              "Memory allocation failed");
            return((char **) NULL);
          }
        (void) strncpy(textlist[i],p,q-p);
        textlist[i][q-p]='\0';
        if (*q == '\r')
          q++;
        p=q+1;
      }
    }
  else
    {
      char
        hex_string[MaxTextExtent];

      register int
        j;

      /*
        Convert string to a HEX list.
      */
      lines=(Extent(text)/0x14)+1;
      textlist=(char **) AllocateMemory((lines+1)*sizeof(char *));
      if (textlist == (char **) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to convert text",
            "Memory allocation failed");
          return((char **) NULL);
        }
      p=text;
      for (i=0; i < (int) lines; i++)
      {
        textlist[i]=(char *) AllocateMemory(900*sizeof(char));
        if (textlist[i] == (char *) NULL)
          {
            MagickWarning(ResourceLimitWarning,"Unable to convert text",
              "Memory allocation failed");
            return((char **) NULL);
          }
        FormatString(textlist[i],"0x%08x: ",(unsigned int) (i*0x14));
        q=textlist[i]+Extent(textlist[i]);
        for (j=1; j <= Min(Extent(p),0x14); j++)
        {
          FormatString(hex_string,"%02x",(unsigned int) (*(p+j)));
          (void) strcpy(q,hex_string);
          q+=2;
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        for (; j <= 0x14; j++)
        {
          *q++=' ';
          *q++=' ';
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        *q++=' ';
        for (j=1; j <= Min(Extent(p),0x14); j++)
        {
          if (isprint((int) (*p)))
            *q++=(*p);
          else
            *q++='-';
          p++;
        }
        *q='\0';
      }
    }
  textlist[i]=(char *) NULL;
  return(textlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i p                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Strip strips the whitespace from the beginning and end of a string
%  of characters.
%
%  The format of the Strip routine is:
%
%     Strip(data)
%
%  A description of each parameter follows:
%
%    o data: Specifies an array of characters.
%
%
*/
Export void Strip(char *data)
{
  long
    count;

  register char
    *p,
    *q;

  register int
    i;

  assert(data != (char *) NULL);
  if (*data == '\0')
    return;
  p=data;
  while (isspace((int) (*p)))
    p++;
  q=data+Extent(data)-1;
  while (isspace((int) (*q)) && (q > p))
    q--;
  count=q-p+1;
  q=data;
  for (i=0; i < count; i++)
    *q++=(*p++);
  *q='\0';
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y s t e m C o m m a n d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SystemCommand executes the specified command and waits until it
%  terminates.  The returned value is the exit status of the command.
%
%  The format of the SystemCommand routine is:
%
%      status=SystemCommand(verbose,command)
%
%  A description of each parameter follows:
%
%    o status: Method SystemCommand returns False if the command is 
%      executed successfully.
%
%    o verbose: An unsigned integer other than 0 prints the executed
%      command before it is invoked.
%
%    o command: This string is the command to execute.
%
%
*/
Export int SystemCommand(unsigned int verbose,char *command)
{
  int
    status;

#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  status=system(command);
#else
#if defined(vms)
  status=!system(command);
#endif
#if defined(macintosh)
  status=MACSystemCommand(command);
#endif
#if defined(WIN32)
  status=NTSystemCommand(command);
#endif
#endif
  if (verbose)
    MagickWarning(UndefinedWarning,command,
      status ? strerror(errno) : (char *) NULL);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T e m p o r a r y F i l e n a m e                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TemporaryFilename replaces the contents of the string pointed to
%  by filename by a unique file name.  Some delegates do not like % or .
%  in their filenames.
%
%  The format of the TemporaryFilename routine is:
%
%       TemporaryFilename(filename)
%
%  A description of each parameter follows.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
Export void TemporaryFilename(char *filename)
{
  register int
    i;

  assert(filename != (char *) NULL);
  *filename='\0';
  for (i=0; i < 256; i++)
  {
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
    register char
      *p;

    p=(char *) tempnam((char *) NULL,TemporaryTemplate);
    if (p != (char *) NULL)
      {
        (void) strcpy(filename,p);
        FreeMemory((char *) p);
      }
#else
#if defined(WIN32)
    (void) NTTemporaryFilename(filename);
#else
#if defined(macintosh)
    (void) getcwd(filename,MaxTextExtent >> 1);
#endif
    (void) tmpnam(filename+strlen(filename));
#endif
#endif
    if ((strchr(filename,'%') == (char *) NULL) &&
        (strchr(filename,'.') == (char *) NULL))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s l a t e T e x t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TranslateText replaces any embedded formating characters with
%  the appropriate image attribute and returns the translated text.
%
%  The format of the TranslateText routine is:
%
%      TranslateText(image_info,image,text)
%
%  A description of each parameter follows:
%
%    o image_info: The address of a structure of type ImageInfo.
%
%    o image: The address of a structure of type Image.
%
%    o text: The address of a character string containing the embedded
%      formatting characters.
%
%
*/
Export char *TranslateText(const ImageInfo *image_info,Image *image,char *text)
{
  char
    *translated_text;

  register char
    *p,
    *q;

  Image
    *local_image;

  ImageInfo
    local_info;

  unsigned int
    indirection,
    length;

  assert((image_info != (ImageInfo *) NULL) || (image != (Image *) NULL));
  if ((text == (char *) NULL) || (*text == '\0'))
    return((char *) NULL);
  indirection=(*text == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      /*
        Read text from a file.
      */
      file=(FILE *) fopen(text+1,"r");
      if (file == (FILE *) NULL)
        {
          MagickWarning(FileOpenWarning,"Unable to read text file",text+1);
          return((char *) NULL);
        }
      length=MaxTextExtent;
      text=(char *) AllocateMemory(length);
      for (q=text; text != (char *) NULL; q++)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-text+1) >= (int) length)
          {
            *q='\0';
            length<<=1;
            text=(char *) ReallocateMemory((char *) text,length);
            if (text == (char *) NULL)
              break;
            q=text+Extent(text);
          }
        *q=(unsigned char) c;
      }
      (void) fclose(file);
      if (text == (char *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Unable to translate text",
            "Memory allocation failed");
          return((char *) NULL);
        }
      *q='\0';
    }
  /*
    Allocate and initialize image text.
  */
  p=text;
  length=Extent(text)+MaxTextExtent;
  translated_text=(char *) AllocateMemory(length);
  if (translated_text == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to translate text",
        "Memory allocation failed");
      if (indirection)
        FreeMemory((char *) text);
      return((char *) NULL);
    }
  if (image_info == (ImageInfo *) NULL)
    {
      GetImageInfo(&local_info);
      image_info=(&local_info);
    }
  local_image=(Image *) NULL;
  if (image == (Image *) NULL)
    {
      local_image=AllocateImage(image_info);
      image=local_image;
    }
  if ((image_info == (ImageInfo *) NULL) || (image == (Image *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to translate text",
        "Memory allocation failed");
      if (indirection)
        FreeMemory((char *) text);
      FreeMemory((char *) translated_text);
      return((char *) NULL);
    }
  /*
    Translate any embedded format characters.
  */
  for (q=translated_text; *p != '\0'; p++)
  {
    *q='\0';
    if ((q-translated_text+MaxTextExtent) >= (int) length)
      {
        length<<=1;
        translated_text=(char *)
          ReallocateMemory((char *) translated_text,length);
        if (translated_text == (char *) NULL)
          break;
        q=translated_text+Extent(translated_text);
      }
    /*
      Process formatting characters in text.
    */
    if ((*p == '\\') && (*(p+1) == 'r'))
      {
        *q++='\r';
        p++;
        continue;
      }
    if ((*p == '\\') && (*(p+1) == 'n'))
      {
        *q++='\n';
        p++;
        continue;
      }
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'b':
      {
        if (image->filesize >= (1 << 24))
          FormatString(q,"%ldmb",image->filesize/1024/1024);
        else
          if (image->filesize >= (1 << 14))
            FormatString(q,"%ldkb",image->filesize/1024);
          else
            FormatString(q,"%ldb",image->filesize);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'd':
      case 'e':
      case 'f':
      case 't':
      {
        char
          directory[MaxTextExtent],
          *extension,
          *filename;

        /*
          Label segment is the base of the filename.
        */
        if (Extent(image->magick_filename) == 0)
          break;
        (void) strcpy(directory,image->magick_filename);
        extension=directory+Extent(directory);
        filename=extension;
        while ((filename > directory) && !IsBasenameSeparator(*(filename-1)))
        {
          if (*filename == '.')
            if (*extension == '\0')
              extension=filename+1;
          filename--;
        }
        switch (*p)
        {
          case 'd':
          {
            *filename='\0';
            (void) strcpy(q,directory);
            q+=Extent(directory);
            break;
          }
          case 'e':
          {
            (void) strcpy(q,extension);
            q+=Extent(extension);
            break;
          }
          case 'f':
          {
            (void) strcpy(q,filename);
            q+=Extent(filename);
            break;
          }
          case 't':
          {
            *(extension-1)='\0';
            (void) strcpy(q,filename);
            q+=Extent(filename);
            break;
          }
        }
        break;
      }
      case 'g':
      {
        FormatString(q,"0x%lx",image_info->group);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'h':
      {
        FormatString(q,"%u",image->magick_rows ? image->magick_rows : 256);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'i':
      {
        (void) strcpy(q,image->filename);
        q+=Extent(image->filename);
        break;
      }
      case 'l':
      {
        if (image->label == (char *) NULL)
          break;
        (void) strcpy(q,image->label);
        q+=Extent(image->label);
        break;
      }
      case 'm':
      {
        (void) strcpy(q,image->magick);
        q+=Extent(image->magick);
        break;
      }
      case 'n':
      {
        FormatString(q,"%u",image_info->subrange);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'o':
      {
        (void) strcpy(q,image_info->filename);
        q+=Extent(image_info->filename);
        break;
      }
      case 'p':
      {
        register Image
          *p;

        unsigned int
          page;

        p=image;
        for (page=1; p->previous != (Image *) NULL; page++)
          p=p->previous;
        FormatString(q,"%u",page);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'q':
      {
        FormatString(q,"%u",image->depth);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 's':
      {
        FormatString(q,"%u",image->scene);
        if (image_info->subrange != 0)
          FormatString(q,"%u",image_info->subimage);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'u':
      {
        (void) strcpy(q,image_info->unique);
        q+=Extent(image_info->unique);
        break;
      }
      case 'w':
      {
        FormatString(q,"%u",
          image->magick_columns ? image->magick_columns : 256);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'x':
      {
        FormatString(q,"%u",(unsigned int) image->x_resolution);
        q=translated_text+Extent(translated_text);
        break;
      }
      case 'y':
      {
        FormatString(q,"%u",(unsigned int) image->y_resolution);
        q=translated_text+Extent(translated_text);
        break;
      }
      case '%':
      {
        *q++=(*p);
        break;
      }
      default:
      {
        *q++='%';
        *q++=(*p);
        break;
      }
    }
  }
  *q='\0';
  if (local_image != (Image *) NULL)
    DestroyImage(local_image);
  if (indirection)
    FreeMemory((char *) text);
  return(translated_text);
}
