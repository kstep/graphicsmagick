/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                       GGGG  M   M                                           %
%                      G      MM MM                                           %
%                      G GG   M M M (center it yourself if you don't like it) %
%                      G   G  M   M                                           %
%                       GGG   M   M                                           %
%                                                                             %
%                                                                             %
%               GraphicsMagick Driver                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                        Glenn Randers-Pehrson                                %
%                            December 2002                                    %
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
%  Mogrify transforms an image or a sequence of images. These transforms
%  include image scaling, image rotation, color reduction, and others. The
%  transmogrified image overwrites the original image.
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "command.h"
#include "log.h"
#include "magick.h"
#include "utility.h"
#include "version.h"

static void GMUsage(void)
{
  static const char
    *options[]=
    {
      "-version             print version information",
      "animate [options]    run the \"animate\" utility",
      "composite [options]  run the \"composite\" utility",
      "conjure [options]    run the \"conjure\" utility",
      "convert [options]    run the \"convert\" utility",
      "display [options]    run the \"display\" utility",
      "identify [options]   run the \"identify\" utility",
      "import [options]     run the \"import\" utility",
      "mogrify [options]    run the \"mogrify\" utility",
      "montage [options]    run the \"montage\" utility",
      (char *) NULL
    };

  const char
    **p;

  (void) printf("Version: %.1024s\n",GetMagickVersion((unsigned long *) NULL));
  (void) printf("Copyright: %.1024s\n\n",GetMagickCopyright());
  (void) printf("Usage: %.1024s command [options ...]\n",
    SetClientName((char *) NULL));
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
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
    *option,
    *text;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  register int
    i;

  unsigned int
    help_wanted,
    status;

  InitializeMagick(*argv);
  help_wanted=False;
  ReadCommandlLine(argc,&argv);
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("version",option+1) == 0)
      {
        (void) fprintf(stdout,"Version: %.1024s\n",
          GetMagickVersion((unsigned long *) NULL));
        (void) fprintf(stdout,"Copyright: %.1024s\n\n",GetMagickCopyright());
        DestroyMagick();
        Exit(0);
      }
    if (LocaleCompare("debug",option+1) == 0)
      {
        (void) SetLogEventMask("None");
        if (*option == '-')
          {
            i++;
            if (i == argc)
               MagickFatalError(OptionFatalError,"MissingEventMask",option);
            (void) SetLogEventMask(argv[i]);
          }
        break;
      }
    if (LocaleCompare("help",option+1) == 0 ||
        LocaleCompare("?",option+1) == 0)
      {
        help_wanted=True;
        GMUsage();
        DestroyMagick();
        Exit(0);
      }
  }

  if (argc < 2)
    {
      GMUsage();
      DestroyMagick();
      Exit(1);
    }

  argc--;
  argv++;

  option=argv[0];

  if (LocaleCompare("animate",option) == 0 ||
      LocaleCompare("animate",option+1) == 0)
    {
      SetClientName("animate");
      status=AnimateImageCommand(argc,argv);
      Exit(!status);
      return(False);
    }

  else if (LocaleCompare("conjure",option) == 0 ||
      LocaleCompare("conjure",option+1) == 0)
    {
      SetClientName("conjure");
      status=ConjureImageCommand(argc,argv);
      Exit(!status);
      return(False);
    }

  else if (LocaleCompare("display",option) == 0 ||
      LocaleCompare("display",option+1) == 0)
    {
      SetClientName("display");
      status=DisplayImageCommand(argc,argv);
      Exit(!status);
      return(False);
    }

  else if (LocaleCompare("import",option) == 0 ||
      LocaleCompare("import",option+1) == 0)
    {
      SetClientName("import");
      status=ImportImageCommand(argc,argv);
      Exit(!status);
      return(False);
    }

  /* watch out -- the following replaces the contents of argv with a new
     pointer that is now managed by and must be freed by the code that
     follows
   */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      (char *) NULL);
  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);

  if (LocaleCompare("composite",option) == 0 ||
      LocaleCompare("composite",option+1) == 0)
    {
      SetClientName("composite");
      if (help_wanted)
        CompositeUsage();
      status=CompositeImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("convert",option) == 0 ||
      LocaleCompare("convert",option+1) == 0)
    {
      SetClientName("convert");
      if (help_wanted)
        ConvertUsage();
      status=ConvertImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("mogrify",option) == 0 ||
      LocaleCompare("mogrify",option+1) == 0)
    {
      SetClientName("mogrify");
      if (help_wanted)
        MogrifyUsage();
      status=MogrifyImageCommand(image_info,argc,argv,(char **) NULL,
        &exception);
    }

  else if (LocaleCompare("montage",option) == 0 ||
      LocaleCompare("montage",option+1) == 0)
    {
      SetClientName("montage");
      if (help_wanted)
        MontageUsage();
      status=MontageImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("identify",option) == 0 ||
      LocaleCompare("identify",option+1) == 0)
    {
      SetClientName("identify");
      if (help_wanted)
        IdentifyUsage();
      text=(char *) NULL;
      status=IdentifyImageCommand(image_info,argc,argv,&text,&exception);
      if (text != (char *) NULL)
        {
          (void) fputs(text,stdout);
          (void) fputc('\n',stdout);
          LiberateMemory((void **) &text);
        }
    }

   else
    GMUsage();

  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  DestroyMagick();
  /* This looks suspicious, but this is NOT the real argv. It was replaced
     by the call to ExpandFilenames earlier
   */
  LiberateMemory((void **) &argv);
  Exit(!status);
  return(False);
}
