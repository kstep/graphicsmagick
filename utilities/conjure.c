/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   N   N  JJJJJ  U   U  RRRR   EEEEE               %
%               C      O   O  NN  N    J    U   U  R   R  E                   %
%               C      O   O  N N N    J    U   U  RRRR   EEE                 %
%               C      O   O  N  NN  J J    U   U  R R    E                   %
%                CCCC   OOO   N   N  JJJ     UUU   R  R   EEEEE               %
%                                                                             %
%                     Interpret Magick Scripting Language.                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               December 2001                                 %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
%  Conjure interprets and executes scripts in the Magick Scripting Language
%  (MSL). The Magick scripting language (MSL) will primarily benefit those
%  that want to accomplish custom image processing tasks but do not wish
%  to program, or those that do not have access to a Perl interpreter or a
%  compiler. The interpreter is called conjure and here is an example script:
%
%      <?xml version="1.0" encoding="UTF-8"?>
%      <image size="400x400" >
%      <read filename="image.gif" />
%      <get width="base-width" height="base-height" />
%      <resize geometry="%[dimensions]" />
%      <get width="width" height="height" />
%      <print output="Image sized from %[base-width]x%[base-height]
%          to %[width]x%[height].\n" />
%      <write filename="image.png" />
%      </image>
%
%  The conjure program syntax is:
%
%  Usage: conjure [ -option value ... ] file [ [ -option value ... ] file ... ]
%
%  Where options include:
%    -debug    display copious debugging information
%    -help     print program options
%    -verbose  print detailed information about the image
%  
%  In addition, define any key value pairs required by your script.  For
%  example,
%  
%       conjure -dimensions 400x400 incantation.msl
%
%  All operations will closely follow the key/value pairs defined in
%  PerlMagick, unless otherwise noted.
%
*/

/*
  Include declarations.
*/
#include "magick/magick.h"
#include "magick/define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n j u r e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConjureUsage() displays the program command syntax.
%
%  The format of the ConjureUsage method is:
%
%      void ConjureUsage()
%
*/
static void ConjureUsage(void)
{
  static const char
    *options[]=
    {
      "-debug    display copious debugging information",
      "-help     print program options",
      "-verbose  print detailed information about the image",
      (char *) NULL
    };

  const char
    **p;

  unsigned int
    version;

  (void) printf("Version: %.1024s\n",GetMagickVersion(&version));
  (void) printf("Copyright: %.1024s\n\n",MagickCopyright);
  (void) printf("ConjureUsage: %.1024s [ -option value ... ] file [ [ -option value "
    "... ] file ... ]\n",SetClientName((char *) NULL));
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf("\nIn additiion, define any key value pairs required by "
    "your script.  For\nexample,\n\n");
  (void) printf("    conjure -size 100x100 -color blue -foo bar script.msl\n");
  Exit(0);
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
    *option;

  ExceptionInfo
    exception;

  Image
    *image;

  ImageInfo
    *image_info;

  register long
    i;

  unsigned int
    status;

  /*
    Set defaults.
  */
  ReadCommandlLine(argc,&argv);
  InitializeMagick(*argv);
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickError(ResourceLimitError,"Memory allocation failed",(char *) NULL);
  if (argc < 2)
    ConjureUsage();
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->attributes=AllocateImage(image_info);
	status=True;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      {

        if (LocaleCompare("debug",option+1) == 0)
          {
            image_info->debug=(*option == '-');
            continue;
          }
        if (LocaleCompare("help",option+1) == 0)
          {
            if (*option == '-')
              ConjureUsage();
            continue;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose=(*option == '-');
            continue;
          }
        /*
          Persist key/value pair.
        */
        status=SetImageAttribute(image_info->attributes,option+1,(char *) NULL);
        status=SetImageAttribute(image_info->attributes,option+1,argv[i+1]);
        if (status == False)
          MagickError(ResourceLimitError,"Unable to persist key",option);
        i++;
        continue;
      }
    /*
      Interpret MSL script.
    */
    status&=SetImageAttribute(image_info->attributes,"filename",(char *) NULL);
    status&=SetImageAttribute(image_info->attributes,"filename",argv[i]);
    if (status == False)
      MagickError(ResourceLimitError,"Unable to persist key",argv[i]);
    (void) FormatString(image_info->filename,"msl:%.1024s",argv[i]);
    image=ReadImage(image_info,&exception);
    if (exception.severity != UndefinedException)
      MagickWarning(exception.severity,exception.reason,exception.description);
    status&=image != (Image *) NULL;
    if (image != (Image *) NULL)
      DestroyImageList(image);
  }
  DestroyImageInfo(image_info);
  DestroyMagick();
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
