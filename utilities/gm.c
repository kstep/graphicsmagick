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
%  montage. Please see the manual page gm.1 for detailed usage information.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/command.h"
#include "magick/magick.h"
#include "magick/utility.h"
#include "magick/version.h"

static void PrintUsage(void)
{
  (void) printf("Version: %.1024s\n",GetMagickVersion((unsigned long *) NULL));
  (void) printf("Copyright: %.1024s\n\n",GetMagickCopyright());
  (void) printf("Use:\n\n");
  (void) printf("  %.1024s help\n\n", SetClientName((char *) NULL));
  (void) printf("to obtain a list of available subcommands, or\n\n");
  (void) printf("  %.1024s help 'command'\n\n", SetClientName((char *) NULL));
  (void) printf("to obtain help information for 'command'\n");
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
    *text;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  unsigned int
    status=True;

  ReadCommandlLine(argc,&argv);
#if defined(WIN32)
  InitializeMagick((char *) NULL);
#else
  InitializeMagick(*argv);
#endif

  SetClientName(*argv);

  if (argc < 2)
    {
      PrintUsage();
      Exit(1);
    }

  argc--;
  argv++;

  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  text=(char *) NULL;
  status=MagickCommand(image_info,argc,argv,&text,&exception);
  if (text != (char *) NULL)
    {
      (void) fputs(text,stdout);
      (void) fputc('\n',stdout);
      MagickFreeMemory(text);
    }
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  DestroyMagick();

  Exit(!status);
  return(False);
}
