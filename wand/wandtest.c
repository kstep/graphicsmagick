/* Copyright (C) 2003-2010 GraphicsMagick Group */
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                         W   W   AAA   N   N  DDDD                           %
%                         W   W  A   A  NN  N  D   D                          %
%                         W W W  AAAAA  N N N  D   D                          %
%                         WW WW  A   A  N  NN  D   D                          %
%                         W   W  A   A  N   N  DDDD                           %
%                                                                             %
%                    ImageMagick MagickWand Validation Tests                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2003                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 1999-2004 ImageMagick Studio, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  This software and documentation is provided "as is," and the copyright     %
%  holders and contributing author(s) make no representations or warranties,  %
%  express or implied, including but not limited to, warranties of            %
%  merchantability or fitness for any particular purpose or that the use of   %
%  the software or documentation will not infringe any third party patents,   %
%  copyrights, trademarks or other rights.                                    %
%                                                                             %
%  The copyright holders and contributing author(s) will not be held liable   %
%  for any direct, indirect, special or consequential damages arising out of  %
%  any use of the software or documentation, even if advised of the           %
%  possibility of such damage.                                                %
%                                                                             %
%  Permission is hereby granted to use, copy, modify, and distribute this     %
%  source code, or portions hereof, documentation and executables, for any    %
%  purpose, without fee, subject to the following restrictions:               %
%                                                                             %
%    1. The origin of this source code must not be misrepresented.            %
%    2. Altered versions must be plainly marked as such and must not be       %
%       misrepresented as being the original source.                          %
%    3. This Copyright notice may not be removed or altered from any source   %
%       or altered source distribution.                                       %
%                                                                             %
%  The copyright holders and contributing author(s) specifically permit,      %
%  without fee, and encourage the use of this source code as a component for  %
%  supporting image processing in commercial products.  If you use this       %
%  source code in a product, acknowledgment is not required but would be      %
%  appreciated.                                                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
 * This version is modified by Bob Friesenhahn so that it may be executed
 * from outside the source directory.
 *
 */


/*
  Include declarations.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wand/magick_wand.h>

#define False  0
#define True  1

#define ThrowAPIException(wand) \
{ \
  description=MagickGetException(wand,&severity); \
  (void) fprintf(stderr,"%s %s %d %.1024s\n",GetMagickModule(),description); \
  free(description); \
  exit(-1); \
}

int main(int argc,char **argv)
{
  char
    *description;

  DrawingWand
    *drawing_wand;

  ExceptionType
    severity;

  MagickWand
    *clone_wand,
    *magick_wand;
 
  PixelWand
    *background,
    *fill;

  register long
    i;

  unsigned int
    status;

  unsigned long
    columns,
    rows;

  (void) argc;

  InitializeMagick(*argv);
  magick_wand=NewMagickWand();
  MagickSetSize(magick_wand,640,480);
  MagickGetSize(magick_wand,&columns,&rows);
  if ((columns != 640) || (rows != 480))
    {
      (void) fprintf(stderr,"Unexpected magick wand size\n");
      exit(1);
    }
  (void) fprintf(stdout,"Reading images...\n");
  {
    char
      *p,
      path[MaxTextExtent];
    
    path[0]=0;
    p=getenv("SRCDIR");
    if (p)
      {
        strcpy(path,p);
        if (path[strlen(path)-1] != '/')
          strcat(path,"/");
      }
    strcat(path,"sequence.miff");
    
    status=MagickReadImage(magick_wand,path);
  }
  if (status == False)
    ThrowAPIException(magick_wand);
  if (MagickGetNumberImages(magick_wand) != 5)
    (void) fprintf(stderr,"read %lu images; expected 5\n",
      (unsigned long) MagickGetNumberImages(magick_wand));
  (void) fprintf(stdout,"Iterate forward...\n");
  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != False)
    (void) fprintf(stdout,"index %ld scene %ld\n",
      MagickGetImageIndex(magick_wand),MagickGetImageScene(magick_wand));
  (void) fprintf(stdout,"Iterate reverse...\n");
  while (MagickPreviousImage(magick_wand) != False)
    (void) fprintf(stdout,"index %ld scene %ld\n",
      MagickGetImageIndex(magick_wand),MagickGetImageScene(magick_wand));
  (void) fprintf(stdout,"Remove scene 1...\n");
  (void) MagickSetImageIndex(magick_wand,1);
  clone_wand=MagickGetImage(magick_wand);
  status=MagickRemoveImage(magick_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != False)
    (void) fprintf(stdout,"index %ld scene %ld\n",
      MagickGetImageIndex(magick_wand),MagickGetImageScene(magick_wand));
  (void) fprintf(stdout,"Insert scene 1 back in sequence...\n");
  (void) MagickSetImageIndex(magick_wand,0);
  status=MagickAddImage(magick_wand,clone_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != False)
    (void) fprintf(stdout,"index %ld scene %ld\n",
      MagickGetImageIndex(magick_wand),MagickGetImageScene(magick_wand));
  (void) fprintf(stdout,"Set scene 2 to scene 1...\n");
  (void) MagickSetImageIndex(magick_wand,2);
  status=MagickSetImage(magick_wand,clone_wand);
  DestroyMagickWand(clone_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != False)
    (void) fprintf(stdout,"index %ld scene %ld\n",
      MagickGetImageIndex(magick_wand),MagickGetImageScene(magick_wand));
  (void) fprintf(stdout,"Apply image processing options...\n");
  status=MagickCropImage(magick_wand,60,60,10,10);
  if (status == False)
    ThrowAPIException(magick_wand);
  MagickResetIterator(magick_wand);
  background=NewPixelWand();
  PixelSetColor(background,"#000000");
  status=MagickRotateImage(magick_wand,background,90.0);
  DestroyPixelWand(background);
  if (status == False)
    ThrowAPIException(magick_wand);
  drawing_wand=NewDrawingWand();
  (void) DrawPushGraphicContext(drawing_wand);
  (void) DrawRotate(drawing_wand,45);
  (void) DrawSetFontSize(drawing_wand,18);
  fill=NewPixelWand();
  PixelSetColor(fill,"green");
  (void) DrawSetFillColor(drawing_wand,fill);
  DestroyPixelWand(fill);
  (void) DrawAnnotation(drawing_wand,15,5,(const unsigned char *) "Magick");
  (void) DrawPopGraphicContext(drawing_wand);
  (void) MagickSetImageIndex(magick_wand,1);
  status=MagickDrawImage(magick_wand,drawing_wand);
  DestroyDrawingWand(drawing_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  {
    unsigned char
      pixels[27],
      primary_colors[27] =
      {
          0,   0,   0,
          0,   0, 255,
          0, 255,   0,
          0, 255, 255,
        255, 255, 255,
        255,   0,   0,
        255,   0, 255,
        255, 255,   0,
        128, 128, 128,
      };

    (void) MagickSetImageIndex(magick_wand,2);
    status=MagickSetImagePixels(magick_wand,10,10,3,3,"RGB",CharPixel,
      primary_colors);
    if (status == False)
      ThrowAPIException(magick_wand);
    status=MagickGetImagePixels(magick_wand,10,10,3,3,"RGB",CharPixel,pixels);
    if (status == False)
      ThrowAPIException(magick_wand);
    for (i=0; i < 9; i++)
      if (pixels[i] != primary_colors[i])
        {
          (void) fprintf(stderr,"Get pixels does not match set pixels\n");
          exit(1);
        }
  }
  (void) MagickSetImageIndex(magick_wand,3);
  status=MagickResizeImage(magick_wand,50,50,UndefinedFilter,1.0);
  if (status == False)
    ThrowAPIException(magick_wand);

  MagickResetIterator(magick_wand);
  while (MagickNextImage(magick_wand) != False)
    {
      MagickSetImageDepth( magick_wand, 8);
      MagickSetImageCompression( magick_wand, RLECompression);
    }
  MagickResetIterator(magick_wand);

  (void) fprintf(stdout,"Write to wandtest_out.miff...\n");
  status=MagickWriteImages(magick_wand,"wandtest_out.miff",True);
  if (status == False)
    ThrowAPIException(magick_wand);
  DestroyMagickWand(magick_wand);
  return(0);
}
