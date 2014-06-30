/* Copyright (C) 2003-2014 GraphicsMagick Group */
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
%                              Software Design                                %
%                                John Cristy                                  %
%                                March 2003                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 1999-2004 ImageMagick Studio LLC, a non-profit organization  %
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
%  This is the tentative public API for ImageMagick.  Use it with caution
%  because it is subject to change until it is finished somewhere around
%  January of 2004.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/error.h"
#include "wand/magick_wand.h"
#include "wand/wand_private.h"

/*
  Typedef declarations.
*/

typedef struct _SuperDoublePixelPacket
{
  double
    red,
    green,
    blue,
    opacity,
    index;
} SuperDoublePixelPacket;

struct _PixelWand
{
  ExceptionInfo
    exception;

  ColorspaceType
    colorspace;
                                                                                
  unsigned int
    matte;

  SuperDoublePixelPacket
    pixel;

  unsigned long
    count,
    signature;
};

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e P i x e l W a n d                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClonePixelWand() creates an exact copy of a PixelWand. PixelWand may not be
%  a null pointer.
%
%  The format of the ClonePixelWand method is:
%
%      PixelWand *ClonePixelWand(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand to clone.
%
%
*/
WandExport PixelWand *
ClonePixelWand(const PixelWand *wand)
{
  PixelWand
    *clone_wand;

  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);

  clone_wand=NewPixelWand();
  clone_wand->colorspace=wand->colorspace;
  clone_wand->matte=wand->matte;
  clone_wand->pixel=wand->pixel;
  clone_wand->count=wand->count;

  return clone_wand;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e P i x e l W a n d s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClonePixelWands creates a deep-copy an array of PixelWands. 
%
%  The format of the ClonePixelWands method is:
%
%      PixelWand **ClonePixelWands(const PixelWand **wands,
%                                  const unsigned long number_wands)
%
%  A description of each parameter follows:
%
%    o wands: The pixel wands to clone.
%
%    o number_wands: The number of wands in the array
%
*/
WandExport PixelWand **
ClonePixelWands(const PixelWand **wands,const unsigned long number_wands)
{
  PixelWand
    **clone_wands;

  unsigned long
    i;
  
  assert(wands != (const PixelWand **) NULL);
  assert(number_wands > 0);

  clone_wands=MagickAllocateArray(PixelWand **,
				  sizeof(PixelWand *),number_wands);
  if (clone_wands == (PixelWand **) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateWand);

  for (i=0; i < number_wands; i++)
    clone_wands[i]=ClonePixelWand(wands[i]);

  return clone_wands;
}

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
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  MagickFreeMemory(wand);
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

  /*
    Initialize GraphicsMagick in case it is not already initialized.
  */
  InitializeMagick(NULL);

  wand=MagickAllocateMemory(struct _PixelWand *,sizeof(struct _PixelWand));
  if (wand == (PixelWand *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateWand);
  (void) memset(wand,0,sizeof(PixelWand));
  GetExceptionInfo(&wand->exception);
  wand->colorspace=RGBColorspace;
  wand->signature=MagickSignature;
  return(wand);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w P i x e l W a n d s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewPixelWands() returns an array of pixel wands.
%
%  The format of the NewPixelWand method is:
%
%      PixelWand NewPixelWands(const unsigned long number_wands)
%
%  A description of each parameter follows:
%
%    o number_wands: The number of wands.
%
*/
WandExport PixelWand **NewPixelWands(const unsigned long number_wands)
{
  register long
    i;

  struct _PixelWand
    **wands;

  wands=MagickAllocateMemory(struct _PixelWand **,
                             (size_t) number_wands*sizeof(struct _PixelWand *));
  if (wands == (PixelWand **) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
		      UnableToAllocateWand);
  for (i=0; i < (long) number_wands; i++)
    wands[i]=NewPixelWand();
  return(wands);
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

WandExport unsigned int PixelGetException(PixelWand *wand,char **description)
{
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(description != (char **) NULL);
  *description=(char *) malloc(2*MaxTextExtent);
  if (*description == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  **description='\0';
  if (wand->exception.reason != (char *) NULL)
    (void) strlcpy(*description,GetLocaleExceptionMessage(
      wand->exception.severity,wand->exception.reason),MaxTextExtent);
  if (wand->exception.description != (char *) NULL)
    {
      (void) strlcat(*description," (",MaxTextExtent);
      (void) strlcat(*description,GetLocaleExceptionMessage(
        wand->exception.severity,wand->exception.description),MaxTextExtent);
      (void) strlcat(*description,")",MaxTextExtent);
    }
  return(wand->exception.severity);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t B l a c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetBlack() returns the normalized black color of the pixel wand.
%
%  The format of the PixelGetBlack method is:
%
%      double PixelGetBlack(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetBlack(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.index);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t B l a c k Q u a n t u m                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetBlackQuantum() returns the black color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetBlackQuantum method is:
%
%      Quantum PixelGetBlackQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetBlackQuantum(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.index+0.5));
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
  assert(wand != (const PixelWand *) NULL);
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
  assert(wand != (const PixelWand *) NULL);
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

  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  (void) FormatString(color,"%u,%u,%u",
    (Quantum) (MaxRGB*wand->pixel.red+0.5),
    (Quantum) (MaxRGB*wand->pixel.green+0.5),
    (Quantum) (MaxRGB*wand->pixel.blue+0.5));
  if (wand->colorspace == CMYKColorspace)
    (void) FormatString(color,"%.1024s,%u",color,
      (Quantum) (MaxRGB*wand->pixel.index+0.5));
  if (wand->matte != False)
    (void) FormatString(color,"%.1024s,%u",color,
      (Quantum) (MaxRGB*wand->pixel.opacity+0.5));
  return(AcquireString(color));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t C o l o r C o u n t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetColorCount() returns the color count associated with this color.
%
%  The format of the PixelGetColorCount method is:
%
%      unsigned long PixelGetColorCount(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport unsigned long PixelGetColorCount(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t C y a n                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetCyan() returns the normalized cyan color of the pixel wand.
%
%  The format of the PixelGetCyan method is:
%
%      double PixelGetCyan(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetCyan(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.red);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t C y a n Q u a n t u m                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetCyanQuantum() returns the cyan color of the pixel wand.  The color
%  is in the range of [0..MaxRGB]
%
%  The format of the PixelGetCyanQuantum method is:
%
%      Quantum PixelGetCyanQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetCyanQuantum(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.red+0.5));
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
  assert(wand != (const PixelWand *) NULL);
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
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.green+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t M a g e n t a                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetMagenta() returns the normalized magenta color of the pixel wand.
%
%  The format of the PixelGetMagenta method is:
%
%      double PixelGetMagenta(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetMagenta(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.green);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t M a g e n t a Q u a n t u m                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetMagentaQuantum() returns the magenta color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetMagentaQuantum method is:
%
%      Quantum PixelGetMagentaQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetMagentaQuantum(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
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
  assert(wand != (const PixelWand *) NULL);
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
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.opacity+0.5));
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
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(color != (PixelPacket *) NULL);
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
  assert(wand != (const PixelWand *) NULL);
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
%  PixelGetRedQuantum(const ) returns the red color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
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
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.red+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t Y e l l o w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetYellow() returns the normalized yellow color of the pixel wand.
%
%  The format of the PixelGetYellow method is:
%
%      double PixelGetYellow(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport double PixelGetYellow(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return(wand->pixel.blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l G e t Y e l l o w Q u a n t u m                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelGetYellowQuantum() returns the yellow color of the pixel wand.  The
%  color is in the range of [0..MaxRGB]
%
%  The format of the PixelGetYellowQuantum method is:
%
%      Quantum PixelGetYellowQuantum(const PixelWand *wand)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
*/
WandExport Quantum PixelGetYellowQuantum(const PixelWand *wand)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  return((Quantum) (MaxRGB*wand->pixel.blue+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t B l a c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetBlack() sets the normalized black color of the pixel wand.
%
%  The format of the PixelSetBlack method is:
%
%      unsigned int PixelSetBlack(PixelWand *wand,const double black)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o black: The black color.
%
*/
WandExport void PixelSetBlack(PixelWand *wand,const double black)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (black > 1.0)
    wand->pixel.index=1.0;
  else
    if (black < 0.0)
      wand->pixel.index=0.0;
    else
      wand->pixel.index=black;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t B l a c k Q u a n t u m                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetBlackQuantum() sets the black color of the pixel wand.  The color
%  must be in the range of [0..MaxRGB]
%
%  The format of the PixelSetBlackQuantum method is:
%
%      unsigned int PixelSetBlackQuantum(PixelWand *wand,
%        const Quantum black)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o black: The black color.
%
*/
WandExport void PixelSetBlackQuantum(PixelWand *wand,const Quantum black)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.index=(double) black/MaxRGB;
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
  assert(wand != (const PixelWand *) NULL);
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
%   P i x e l S e t B l u e Q u a n t u m                                     %
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
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.blue=(double) blue/MaxRGB;
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
%  PixelSetColor() sets the color of the pixel wand with a string (e.g.
%  "blue", "#0000ff", "rgb(0,0,255)", etc.).
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
  PixelPacket
    pixel;

  unsigned int
    status;

  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);

  status=QueryColorDatabase(color,&pixel,&wand->exception);
  if (status == False)
    return(status);

  wand->colorspace=RGBColorspace;
  wand->matte=0;
  wand->pixel.red=(double) pixel.red/MaxRGB;
  wand->pixel.green=(double) pixel.green/MaxRGB;
  wand->pixel.blue=(double) pixel.blue/MaxRGB;
  wand->pixel.opacity=(double) pixel.opacity/MaxRGB;
  wand->pixel.index=0;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t C o l o r C o u n t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetColorCount() sets the color count of the pixel wand.
%
%  The format of the PixelSetColorCount method is:
%
%      unsigned int PixelSetColorCount(PixelWand *wand,
%        const unsigned long count)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o count: The number of this particular color.
%
*/
WandExport void PixelSetColorCount(PixelWand *wand,const unsigned long count)
{
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->count=count;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t C y a n                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetCyan() sets the normalized cyan color of the pixel wand.
%
%  The format of the PixelSetCyan method is:
%
%      unsigned int PixelSetCyan(PixelWand *wand,const double cyan)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o cyan: The cyan color.
%
*/
WandExport void PixelSetCyan(PixelWand *wand,const double cyan)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (cyan > 1.0)
    wand->pixel.red=1.0;
  else
    if (cyan < 0.0)
      wand->pixel.red=0.0;
    else
      wand->pixel.red=cyan;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t C y a n Q u a n t u m                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetCyanQuantum() sets the cyan color of the pixel wand.  The color must
%  be in the range of [0..MaxRGB]
%
%  The format of the PixelSetCyanQuantum method is:
%
%      unsigned int PixelSetCyanQuantum(PixelWand *wand,const Quantum cyan)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o cyan: The cyan color.
%
*/
WandExport void PixelSetCyanQuantum(PixelWand *wand,const Quantum cyan)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.red=(double) cyan/MaxRGB;
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
  assert(wand != (PixelWand *) NULL);
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
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.green=(double) green/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t M a g e n t a                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetMagenta() sets the normalized magenta color of the pixel wand.
%
%  The format of the PixelSetMagenta method is:
%
%      unsigned int PixelSetMagenta(PixelWand *wand,const double magenta)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o magenta: The magenta color.
%
*/
WandExport void PixelSetMagenta(PixelWand *wand,const double magenta)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (magenta > 1.0)
    wand->pixel.green=1.0;
  else
    if (magenta < 0.0)
      wand->pixel.green=0.0;
    else
      wand->pixel.green=magenta;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t M a g e n t a Q u a n t u m                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetMagentaQuantum() sets the magenta color of the pixel wand.  The
%  color must be in the range of [0..MaxRGB]
%
%  The format of the PixelSetMagentaQuantum method is:
%
%      unsigned int PixelSetMagentaQuantum(PixelWand *wand,
%        const Quantum magenta)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o magenta: The magenta color.
%
*/
WandExport void PixelSetMagentaQuantum(PixelWand *wand,const Quantum magenta)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.green=(double) magenta/MaxRGB;
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
%    o opacity: The opacity value.
%
*/
WandExport void PixelSetOpacity(PixelWand *wand,const double opacity)
{
  assert(wand != (PixelWand *) NULL);
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
%    o opacity: The opacity value.
%
*/
WandExport void PixelSetOpacityQuantum(PixelWand *wand,const Quantum opacity)
{
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.opacity=(double) opacity/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t Q u a n t u m C o l o r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetQuantumColor() sets the color of the pixel wand.
%
%  The format of the PixelSetQuantumColor method is:
%
%      PixelSetQuantumColor(PixelWand *wand,PixelPacket *color)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o color: The pixel wand color (expressed as a PixelPacket).
%
*/
/* FIXME: color argument should be const! */
WandExport void PixelSetQuantumColor(PixelWand *wand,PixelPacket *color)
{
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  assert(color != (PixelPacket *) NULL);
  wand->pixel.red=(double) color->red/MaxRGB;
  wand->pixel.green=(double) color->green/MaxRGB;
  wand->pixel.blue=(double) color->blue/MaxRGB;
  wand->pixel.opacity=(double) color->opacity/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t R e d                                                     %
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
  assert(wand != (PixelWand *) NULL);
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
  assert(wand != (PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.red=(double) red/MaxRGB;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t Y e l l o w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetYellow() sets the normalized yellow color of the pixel wand.
%
%  The format of the PixelSetYellow method is:
%
%      unsigned int PixelSetYellow(PixelWand *wand,const double yellow)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o yellow: The yellow color.
%
*/
WandExport void PixelSetYellow(PixelWand *wand,const double yellow)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  if (yellow > 1.0)
    wand->pixel.blue=1.0;
  else
    if (yellow < 0.0)
      wand->pixel.blue=0.0;
    else
      wand->pixel.blue=yellow;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l S e t Y e l l o w Q u a n t u m                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelSetYellowQuantum() sets the yellow color of the pixel wand.  The color
%  must be in the range of [0..MaxRGB]
%
%  The format of the PixelSetYellowQuantum method is:
%
%      unsigned int PixelSetYellowQuantum(PixelWand *wand,const Quantum yellow)
%
%  A description of each parameter follows:
%
%    o wand: The pixel wand.
%
%    o yellow: The yellow color.
%
*/
WandExport void PixelSetYellowQuantum(PixelWand *wand,const Quantum yellow)
{
  assert(wand != (const PixelWand *) NULL);
  assert(wand->signature == MagickSignature);
  wand->pixel.blue=(double) yellow/MaxRGB;
}
