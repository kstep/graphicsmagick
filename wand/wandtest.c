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
%  Copyright (C) 2003 ImageMagick Studio, a non-profit organization dedicated %
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
%
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

int main(int argc,char **argv)
{
#define ThrowAPIException(wand) \
{ \
  description=MagickGetException(wand,&severity); \
  (void) fprintf(stderr,"%s %s %d %.1024s\n",GetMagickModule(),description); \
  free(description); \
  exit(-1); \
}

  char
    *description;

  DrawingWand
    *drawing_wand;

  ExceptionType
    severity;

  MagickWand
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

  magick_wand=NewMagickWand();
  MagickSetSize(magick_wand,640,480);
  MagickGetSize(magick_wand,&columns,&rows);
  if ((columns != 640) || (rows != 480))
    {
      (void) fprintf(stderr,"Unexpected magick wand size\n");
      exit(1);
    }
  {
    char
      *p,
      path[MaxTextExtent];

    path[0]=0;
    p=getenv("SRCDIR");
    if (p)
      strcpy(path,p);
    strcat(path,"sequence.miff");
    status=MagickReadImage(magick_wand,path);
  }
  if (status == False)
    ThrowAPIException(magick_wand);
  if (MagickGetNumberOfImages(magick_wand) != 5)
    (void) fprintf(stderr,"read %ld images; expected 5\n",
      MagickGetNumberOfImages(magick_wand));
  status=MagickRemoveImage(magick_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  status=MagickCropImage(magick_wand,60,60,10,10);
  if (status == False)
    ThrowAPIException(magick_wand);
  (void) MagickSetImage(magick_wand,0);
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
  (void) MagickSetImage(magick_wand,1);
  status=MagickDrawImage(magick_wand,drawing_wand);
  if (status == False)
    ThrowAPIException(magick_wand);
  DestroyDrawingWand(drawing_wand);
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

    (void) MagickSetImage(magick_wand,2);
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
  (void) MagickSetImage(magick_wand,3);
  status=MagickResizeImage(magick_wand,50,50,UndefinedFilter,1.0);
  if (status == False)
    ThrowAPIException(magick_wand);
  status=MagickWriteImages(magick_wand,"image.miff",True);
  if (status == False)
    ThrowAPIException(magick_wand);
  DestroyMagickWand(magick_wand);
  return(0);
}
