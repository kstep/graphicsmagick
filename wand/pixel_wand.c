/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      PPPP   IIIII  X   X  EEEEE  L                          %
%                      P   P    I     X X   E      L                          %
%                      PPPP     I      X    EEE    L                          %
%                      P        I     X X   E      L                          %
%                      P      IIIII  X   X  EEEEE  LLLLL                      %
%                                                                             %
%                         W   W   AAA   N   N  DDDD                           %
%                         W   W  A   A  NN  N  D   D                          %
%                         W W W  AAAAA  N N N  D   D                          %
%                         WW WW  A   A  N  NN  D   D                          %
%                         W   W  A   A  N   N  DDDD                           %
%                                                                             %
%                                                                             %
%                   ImageMagick Image Pixel Wand Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                March 2003                                   %
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
%  For internal use only!  Subject to change!
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/error.h"
#include "magick/utility.h"
#include "wand/wand_api.h"
#include "wand/magick_compat.h"

/*
  Typedef declarations.
*/
struct _PixelWand
{
  ExceptionInfo
    exception;

  DoublePixelPacket
    pixel;

  unsigned long
    signature;
};

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y P i x e l W a n d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyPixelWand() deallocates resources associated with a PixelWand.
%
%  The format of the DestroyPixelWand method is:
%
%      unsigned int DestroyPixelWand(PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%
*/
WandExport void DestroyPixelWand(PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  RelinquishMagickMemory(wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w P i x e l W a n d                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewPixelWand() returns a new pixel wand.
%
%  The format of the NewPixelWand method is:
%
%      PixelWand NewPixelWand(void)
%
%
*/
WandExport PixelWand *NewPixelWand(void)
{
  struct _PixelWand
    *wand;

  wand=(struct _PixelWand *) AcquireMagickMemory(sizeof(struct _PixelWand));
  if (wand == (PixelWand *) NULL)
    return((PixelWand *) NULL);
  (void) memset(wand,0,sizeof(PixelWand));
  GetExceptionInfo(&wand->exception);
  wand->signature=MagickSignature;
  return(wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t E x c e p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetException() returns the severity, reason, and description of any
%  error that occurs when using the pixel wand methods.
%
%  The format of the PixelGetException method is:
%
%      unsigned int PixelGetException(PixelWand *wand,char **description)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o description: A description of the error.
%
*/

static const char *GetErrorMessageString(const int error_number)
{
  const char
    *message;

  message=strerror(error_number);
  if (message == (const char *) NULL)
    return("Error number is out of range");
  return(message);
}

WandExport unsigned int PixelGetException(PixelWand *wand,char **description)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  assert(description != (char **) NULL);
  *description=(char *) malloc(2*MaxTextExtent);
  if (*description == (char *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateString");
  **description='\0';
  if (wand->exception.reason != (char *) NULL)
    (void) strncpy(*description,GetLocaleExceptionMessage(
      wand->exception.severity,wand->exception.reason),MaxTextExtent-1);
  if (wand->exception.description != (char *) NULL)
    {
      (void) strcat(*description," (");
      (void) strncat(*description,GetLocaleExceptionMessage(
        wand->exception.severity,wand->exception.description),
        MaxTextExtent-strlen(*description)-1);
      (void) strcat(*description,")");
    }
  if ((wand->exception.severity != OptionError) && errno)
    {
      (void) strcat(*description," [");
      (void) strncat(*description,GetErrorMessageString(errno),
        MaxTextExtent-strlen(*description)-1);
      (void) strcat(*description,"]");
    }
  return(wand->exception.severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t B l u e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetBlue(const) returns the normalized blue color of the pixel wand.
%
%  The format of the PixelGetBlue method is:
%
%      double PixelGetBlue(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetBlue(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t B l u e Q u a n t u m                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetBlueQuantum(const ) returns the blue color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetBlueQuantum method is:
%
%      Quantum PixelGetBlueQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetBlueQuantum(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.blue+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t C o l o r A s S t r i n g                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetColorAsString() gets the color of the pixel wand.
%
%  The format of the PixelGetColorAsString method is:
%
%      char *PixelGetColorAsString(PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport char *PixelGetColorAsString(const PixelWand *wand)
{
  char
    color[MaxTextExtent];

  FormatString(color,"%u,%u,%u,%u",
    (Quantum) (MaxRGB*wand->pixel.red+0.5),
    (Quantum) (MaxRGB*wand->pixel.green+0.5),
    (Quantum) (MaxRGB*wand->pixel.blue+0.5),
    (Quantum) (MaxRGB*wand->pixel.opacity+0.5));
  return(AllocateString(color));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t G r e e n                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetGreen(const ) returns the normalized green color of the pixel wand.
%
%  The format of the PixelGetGreen method is:
%
%      double PixelGetGreen(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetGreen(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.green);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t G r e e n Q u a n t u m                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetGreenQuantum(const ) returns the green color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetGreenQuantum method is:
%
%      Quantum PixelGetGreenQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetGreenQuantum(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.green+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t O p a c i t y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetOpacity(const ) returns the normalized opacity color of the pixel
%  wand.
%
%  The format of the PixelGetOpacity method is:
%
%      double PixelGetOpacity(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetOpacity(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.opacity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t O p a c i t y Q u a n t u m                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetOpacityQuantum(const ) returns the opacity color of the pixel wand.
%  The color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetOpacityQuantum method is:
%
%      Quantum PixelGetOpacityQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetOpacityQuantum(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.opacity+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t R e d                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetRed(const ) returns the normalized red color of the pixel wand.
%
%  The format of the PixelGetRed method is:
%
%      double PixelGetRed(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetRed(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.red);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t R e d Q u a n t u m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetRedQuantum(const ) returns the red color of the pixel wand.  The color is
%  in the range of [0..MaxRGB]
%
%  The format of the PixelGetRedQuantum method is:
%
%      Quantum PixelGetRedQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetRedQuantum(const PixelWand *wand)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.red+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P i x e l G e t Q u a n t u m C o l o r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetQuantumColor() gets the color of the pixel wand.
%
%  The format of the PixelGetQuantumColor method is:
%
%      PixelGetQuantumColor(PixelWand *wand,PixelPacket *color)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o color: Return the pixel wand color here.
%
*/
WandExport void PixelGetQuantumColor(const PixelWand *wand,PixelPacket *color)
{
  color->red=(Quantum) (MaxRGB*wand->pixel.red+0.5);
  color->green=(Quantum) (MaxRGB*wand->pixel.green+0.5);
  color->blue=(Quantum) (MaxRGB*wand->pixel.blue+0.5);
  color->opacity=(Quantum) (MaxRGB*wand->pixel.opacity+0.5);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t C o l o r                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetColor() sets the color of the pixel wand.
%
%  The format of the PixelSetColor method is:
%
%      unsigned int PixelSetColor(PixelWand *wand,const char *color)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o color: The pixel wand color.
%
%
*/
WandExport unsigned int PixelSetColor(PixelWand *wand,const char *color)
{
  GeometryInfo
    geometry_info;

  unsigned int
    flags;

  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  flags=ParseGeometry(color,&geometry_info);
  wand->pixel.red=geometry_info.rho/MaxRGB;
  wand->pixel.green=geometry_info.sigma/MaxRGB;
  if (!(flags & SigmaValue))
    wand->pixel.green=wand->pixel.red;
  wand->pixel.blue=geometry_info.xi/MaxRGB;
  if (!(flags & XiValue))
    wand->pixel.blue=wand->pixel.red;
  wand->pixel.opacity=geometry_info.psi/MaxRGB;
  if (!(flags & PsiValue))
    wand->pixel.opacity=(double) OpaqueOpacity/MaxRGB;
  if (flags & PercentValue)
    {
      wand->pixel.red*=MaxRGB/100.0;
      wand->pixel.green*=MaxRGB/100.0;
      wand->pixel.blue*=MaxRGB/100.0;
      wand->pixel.opacity*=MaxRGB/100.0;
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t B l u e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetBlue() sets the normalized blue color of the pixel wand.
%
%  The format of the PixelSetBlue method is:
%
%      unsigned int PixelSetBlue(PixelWand *wand,const double blue)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o blue: The blue color.
%
*/
WandExport void PixelSetBlue(PixelWand *wand,const double blue)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (blue > 1.0)
    wand->pixel.blue=1.0;
  else
    if (blue < 0.0)
      wand->pixel.blue=0.0;
    else
      wand->pixel.blue=blue;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t B l u e Q u a n t u m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetBlueQuantum() sets the blue color of the pixel wand.  The color must
%  be in the range of [0..MaxRGB]
%
%  The format of the PixelSetBlueQuantum method is:
%
%      unsigned int PixelSetBlueQuantum(PixelWand *wand,const Quantum blue)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o blue: The blue color.
%
*/
WandExport void PixelSetBlueQuantum(PixelWand *wand,const Quantum blue)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.blue=(double) blue/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t G r e e n                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetGreen() sets the normalized green color of the pixel wand.
%
%  The format of the PixelSetGreen method is:
%
%      unsigned int PixelSetGreen(PixelWand *wand,const double green)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o green: The green color.
%
*/
WandExport void PixelSetGreen(PixelWand *wand,const double green)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (green > 1.0)
    wand->pixel.green=1.0;
  else
    if (green < 0.0)
      wand->pixel.green=0.0;
    else
      wand->pixel.green=green;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t G r e e n Q u a n t u m                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetGreenQuantum() sets the green color of the pixel wand.  The color must
%  be in the range of [0..MaxRGB]
%
%  The format of the PixelSetGreenQuantum method is:
%
%      unsigned int PixelSetGreenQuantum(PixelWand *wand,const Quantum green)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o green: The green color.
%
*/
WandExport void PixelSetGreenQuantum(PixelWand *wand,const Quantum green)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.green=(double) green/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t O p a c i t y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetOpacity() sets the normalized opacity color of the pixel wand.
%
%  The format of the PixelSetOpacity method is:
%
%      unsigned int PixelSetOpacity(PixelWand *wand,const double opacity)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o opacity: The opacity color.
%
*/
WandExport void PixelSetOpacity(PixelWand *wand,const double opacity)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (opacity > 1.0)
    wand->pixel.opacity=1.0;
  else
    if (opacity < 0.0)
      wand->pixel.opacity=0.0;
    else
      wand->pixel.opacity=opacity;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t O p a c i t y Q u a n t u m                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetOpacityQuantum() sets the opacity color of the pixel wand.  The
%  color must be in the range of [0..MaxRGB]
%
%  The format of the PixelSetOpacityQuantum method is:
%
%      unsigned int PixelSetOpacityQuantum(PixelWand *wand,
%        const Quantum opacity)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o opacity: The opacity color.
%
*/
WandExport void PixelSetOpacityQuantum(PixelWand *wand,const Quantum opacity)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.opacity=(double) opacity/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t O p a c i t y                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetRed() sets the normalized red color of the pixel wand.
%
%  The format of the PixelSetRed method is:
%
%      unsigned int PixelSetRed(PixelWand *wand,const double red)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o red: The red color.
%
*/
WandExport void PixelSetRed(PixelWand *wand,const double red)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  if (red > 1.0)
    wand->pixel.red=1.0;
  else
    if (red < 0.0)
      wand->pixel.red=0.0;
    else
      wand->pixel.red=red;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t R e d Q u a n t u m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetRedQuantum() sets the red color of the pixel wand.  The color must
%  be in the range of [0..MaxRGB]
%
%  The format of the PixelSetRedQuantum method is:
%
%      unsigned int PixelSetRedQuantum(PixelWand *wand,const Quantum red)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o red: The red color.
%
*/
WandExport void PixelSetRedQuantum(PixelWand *wand,const Quantum red)
{
  assert(wand != (void *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.red=(double) red/MaxRGB;
}
