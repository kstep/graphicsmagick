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
%                                  GGGG  M   M                                %
%                                 G      MM MM                                %
%                                 G GG   M M M                                %
%                                 G   G  M   M                                %
%                                  GGG   M   M                                %
%                                                                             %
%                                                                             %
%                             GraphicsMagick Driver                           %
%                                                                             %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                            Glenn Randers-Pehrson                            %
%                                December 2002                                %
%                              Header Centered By                             %
%                               Bob Friesenhahn                               %
%                                  May 2003                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Gm is a common wrapper around a set of commands, which include animate,
%  composite, conjure, convert, display, identify, import, mogrify, and
%  montage. Please see the manual page gm.1 for detailed information.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/command.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/utility.h"
#include "magick/version.h"

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
    status=True;

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
    if (argc < 3 && (LocaleCompare("help",option+1) == 0 ||
        LocaleCompare("?",option+1) == 0))
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

  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);

  option=argv[0];

  if (LocaleCompare("animate",option) == 0 ||
      LocaleCompare("animate",option+1) == 0)
    {
      SetClientName("animate");
      if (help_wanted)
        AnimateUsage();
      else
        status=AnimateImageCommand(argc,argv);
    }

  else if (LocaleCompare("composite",option) == 0 ||
      LocaleCompare("composite",option+1) == 0)
    {
      SetClientName("composite");
      if (help_wanted)
        CompositeUsage();
      else
        status=CompositeImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("conjure",option) == 0 ||
      LocaleCompare("conjure",option+1) == 0)
    {
      SetClientName("conjure");
      if (help_wanted)
        ConjureUsage();
      else
        status=ConjureImageCommand(argc,argv);
    }

  else if (LocaleCompare("convert",option) == 0 ||
      LocaleCompare("convert",option+1) == 0)
    {
      SetClientName("convert");
      if (help_wanted)
        ConvertUsage();
      else
        status=ConvertImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("display",option) == 0 ||
      LocaleCompare("display",option+1) == 0)
    {
      SetClientName("display");
      if (help_wanted)
        DisplayUsage();
      else
        status=DisplayImageCommand(argc,argv);
    }

  else if (LocaleCompare("identify",option) == 0 ||
      LocaleCompare("identify",option+1) == 0)
    {
      SetClientName("identify");
      if (help_wanted)
        IdentifyUsage();
      else
        {
          text=(char *) NULL;
          status=IdentifyImageCommand(image_info,argc,argv,&text,&exception);
          if (text != (char *) NULL)
            {
              (void) fputs(text,stdout);
              (void) fputc('\n',stdout);
              MagickFreeMemory(text);
            }
        }
    }

  else if (LocaleCompare("import",option) == 0 ||
      LocaleCompare("import",option+1) == 0)
    {
      SetClientName("import");
      if (help_wanted)
        ImportUsage();
      else
        status=ImportImageCommand(argc,argv);
    }

  else if (LocaleCompare("mogrify",option) == 0 ||
      LocaleCompare("mogrify",option+1) == 0)
    {
      SetClientName("mogrify");
      if (help_wanted)
        MogrifyUsage();
      else
        status=MogrifyImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else if (LocaleCompare("montage",option) == 0 ||
      LocaleCompare("montage",option+1) == 0)
    {
      SetClientName("montage");
      if (help_wanted)
        MontageUsage();
      else
        status=MontageImageCommand(image_info,argc,argv,(char **) NULL,
          &exception);
    }

  else
    GMUsage();

  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  DestroyMagick();

  Exit(!status);
  return(False);
}
