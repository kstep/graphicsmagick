/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        ZZZZZ   OOO    OOO   M   M                           %
%                           ZZ  O   O  O   O  MM MM                           %
%                         ZZZ   O   O  O   O  M M M                           %
%                        ZZ     O   O  O   O  M   M                           %
%                        ZZZZZ   OOO    OOO   M   M                           %
%                                                                             %
%                                                                             %
%                      ImageMagick Image Zoom Methods                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "magick.h"
#include "define.h"

/*
  Typedef declarations.
*/
typedef struct _ContributionInfo
{
  int
    pixel;

  double
    weight;
} ContributionInfo;

typedef struct _FilterInfo
{
  double
    (*function)(double),
    support;
} FilterInfo;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   B e s s e l O r d e r O n e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BesselOrderOne computes the Bessel function of x of the first kind
%  of order 0:
%
%    Reduce x to |x| since j1(x)= -j1(-x), and for x in (0,8]
%
%       j1(x) = x*j1(x);
%
%    For x in (8,inf)
%
%       j1(x) = sqrt(2/(pi*x))*(p1(x)*cos(x1)-q1(x)*sin(x1))
%
%    where x1 = x-3*pi/4. Compute sin(x1) and cos(x1) as follow:
%
%       cos(x1) =  cos(x)cos(3pi/4)+sin(x)sin(3pi/4)
%               =  1/sqrt(2) * (sin(x) - cos(x))
%       sin(x1) =  sin(x)cos(3pi/4)-cos(x)sin(3pi/4)
%               = -1/sqrt(2) * (sin(x) + cos(x))
%
%  The format of the BesselOrderOne method is:
%
%      double BesselOrderOne(double x)
%
%  A description of each parameter follows:
%
%    o value: Method BesselOrderOne returns the Bessel function of x of the
%      first kind of orders 1.
%
%    o x: double value.
%
%
*/

static double J1(double x)
{
  double
    p,
    q;

  register int
    i;

  static const double
    Pone[] =
    {
       0.581199354001606143928050809e+21,
      -0.6672106568924916298020941484e+20,
       0.2316433580634002297931815435e+19,
      -0.3588817569910106050743641413e+17,
       0.2908795263834775409737601689e+15,
      -0.1322983480332126453125473247e+13,
       0.3413234182301700539091292655e+10,
      -0.4695753530642995859767162166e+7,
       0.270112271089232341485679099e+4
    },
    Qone[] =
    {
      0.11623987080032122878585294e+22,
      0.1185770712190320999837113348e+20,
      0.6092061398917521746105196863e+17,
      0.2081661221307607351240184229e+15,
      0.5243710262167649715406728642e+12,
      0.1013863514358673989967045588e+10,
      0.1501793594998585505921097578e+7,
      0.1606931573481487801970916749e+4,
      0.1e+1
    };

  p=Pone[8];
  q=Qone[8];
  for (i=7; i >= 0; i--)
  {
    p=p*x*x+Pone[i];
    q=q*x*x+Qone[i];
  }
  return(p/q);
}

static double P1(double x)
{
  double
    p,
    q;

  register int
    i;

  static const double
    Pone[] =
    {
      0.352246649133679798341724373e+5,
      0.62758845247161281269005675e+5,
      0.313539631109159574238669888e+5,
      0.49854832060594338434500455e+4,
      0.2111529182853962382105718e+3,
      0.12571716929145341558495e+1
    },
    Qone[] =
    {
      0.352246649133679798068390431e+5,
      0.626943469593560511888833731e+5,
      0.312404063819041039923015703e+5,
      0.4930396490181088979386097e+4,
      0.2030775189134759322293574e+3,
      0.1e+1
    };

  p=Pone[5];
  q=Qone[5];
  for (i=4; i >= 0; i--)
  {
    p=p*(8.0/x)*(8.0/x)+Pone[i];
    q=q*(8.0/x)*(8.0/x)+Qone[i];
  }
  return(p/q);
}

static double Q1(double x)
{
  double
    p,
    q;

  register int
    i;

  static const double
    Pone[] =
    {
      0.3511751914303552822533318e+3,
      0.7210391804904475039280863e+3,
      0.4259873011654442389886993e+3,
      0.831898957673850827325226e+2,
      0.45681716295512267064405e+1,
      0.3532840052740123642735e-1
    },
    Qone[] =
    {
      0.74917374171809127714519505e+4,
      0.154141773392650970499848051e+5,
      0.91522317015169922705904727e+4,
      0.18111867005523513506724158e+4,
      0.1038187585462133728776636e+3,
      0.1e+1
    };

  p=Pone[5];
  q=Qone[5];
  for (i=4; i >= 0; i--)
  {
    p=p*(8.0/x)*(8.0/x)+Pone[i];
    q=q*(8.0/x)*(8.0/x)+Qone[i];
  }
  return(p/q);
}

static double BesselOrderOne(double x)
{
  double
    p,
    q;

  if (x == 0.0)
    return(0.0);
  p=x;
  if (x < 0.0)
    x=(-x);
  if (x < 8.0)
    return(p*J1(x));
  q=sqrt(2.0/(MagickPI*x))*(P1(x)*(1.0/sqrt(2.0)*(sin(x)-cos(x)))-8.0/x*Q1(x)*
    (-1.0/sqrt(2.0)*(sin(x)+cos(x))));
  if (p < 0.0)
    q=(-q);
  return(q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g n i f y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagnifyImage() is a convenience method that scales an image proportionally
%  to twice its size.  If a NULL image is returned, check the exception member
%  of image to determine why the image failed to magnify.
%
%  The format of the MagnifyImage method is:
%
%      Image *MagnifyImage(image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MagnifyImage(Image *image,ExceptionInfo *exception)
{
#define MagnifyImageText  "  Magnify image...  "

  Image
    *magnify_image;

  int
    rows,
    y;

  PixelPacket
    *scanline;

  register int
    x;

  register PixelPacket
    *p,
    *q,
    *s;

  /*
    Initialize magnify image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  magnify_image=CloneImage(image,2*image->columns,2*image->rows,False,
    exception);
  if (magnify_image == (Image *) NULL)
    return((Image *) NULL);
  magnify_image->storage_class=DirectClass;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=(PixelPacket *)
    AcquireMemory(magnify_image->columns*sizeof(PixelPacket));
  if (scanline == (PixelPacket *) NULL)
    {
      DestroyImage(magnify_image);
      ThrowImageException(ResourceLimitWarning,"Unable to magnify image",
        "Memory allocation failed")
    }
  /*
    Initialize zoom image pixels.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(magnify_image,0,y,magnify_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    (void) memcpy(q,p,image->columns*sizeof(PixelPacket));
    if (!SyncImagePixels(magnify_image))
      break;
  }
  /*
    Magnify each row.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(magnify_image,0,image->rows-1-y,magnify_image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    (void) memcpy(scanline,p,magnify_image->columns*sizeof(PixelPacket));
    q=GetImagePixels(magnify_image,0,2*(image->rows-1-y),
      magnify_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    p=scanline+image->columns-1;
    q+=2*(image->columns-1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (int) image->columns; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      (q+1)->red=(((int) p->red)+((int) (p+1)->red)+1)/2;
      (q+1)->green=(((int) p->green)+((int) (p+1)->green)+1)/2;
      (q+1)->blue=(((int) p->blue)+((int) (p+1)->blue)+1)/2;
      (q+1)->opacity=(((int) p->opacity)+((int) (p+1)->opacity)+1)/2;
    }
    if (!SyncImagePixels(magnify_image))
      break;
  }
  for (y=0; y < (int) image->rows; y++)
  {
    rows=Min(image->rows-y,3);
    p=GetImagePixels(magnify_image,0,2*y,magnify_image->columns,rows);
    if (p == (PixelPacket *) NULL)
      break;
    q=p;
    if (rows > 1)
      q=p+magnify_image->columns;
    s=p;
    if (rows > 2)
      s=q+magnify_image->columns;
    for (x=0; x < (int) (image->columns-1); x++)
    {
      q->red=(((int) p->red)+((int) s->red)+1)/2;
      q->green=(((int) p->green)+((int) s->green)+1)/2;
      q->blue=(((int) p->blue)+((int) s->blue)+1)/2;
      q->opacity=(((int) p->opacity)+((int) s->opacity)+1)/2;
      (q+1)->red=(((int) p->red)+((int) (p+2)->red)+((int) s->red)+
        ((int) (s+2)->red)+2) >> 2;
      (q+1)->green=(((int) p->green)+((int) (p+2)->green)+((int) s->green)+
        ((int) (s+2)->green)+2) >> 2;
      (q+1)->blue=(((int) p->blue)+((int) (p+2)->blue)+((int) s->blue)+
        ((int) (s+2)->blue)+2) >> 2;
      (q+1)->opacity=(((int) p->opacity)+((int) (p+2)->opacity)+
        ((int) s->opacity)+((int) (s+2)->opacity)+2) >> 2;
      q+=2;
      p+=2;
      s+=2;
    }
    q->red=(((int) p->red)+((int) s->red)+1)/2;
    q->green=(((int) p->green)+((int) s->green)+1)/2;
    q->blue=(((int) p->blue)+((int) s->blue)+1)/2;
    q->opacity=(((int) p->opacity)+((int) s->opacity)+1)/2;
    p++;
    q++;
    s++;
    q->red=(((int) p->red)+((int) s->red)+1)/2;
    q->green=(((int) p->green)+((int) s->green)+1)/2;
    q->blue=(((int) p->blue)+((int) s->blue)+1)/2;
    q->opacity=(((int) p->opacity)+((int) s->opacity)+1)/2;
    if (!SyncImagePixels(magnify_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(MagnifyImageText,y,image->rows);
  }
  p=GetImagePixels(magnify_image,0,2*image->rows-2,magnify_image->columns,1);
  if (p != (PixelPacket *) NULL)
    (void) memcpy(scanline,p,magnify_image->columns*sizeof(PixelPacket));
  q=GetImagePixels(magnify_image,0,2*image->rows-1,magnify_image->columns,1);
  if (q != (PixelPacket *) NULL)
    (void) memcpy(q,scanline,magnify_image->columns*sizeof(PixelPacket));
  (void) SyncImagePixels(magnify_image);
  LiberateMemory((void **) &scanline);
  return(magnify_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M i n i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MinifyImage() is a convenience method that scales an image proportionally
%  to half its size.  If a NULL image is returned, check the exception member
%  of image to determine why the image failed to minify.
%
%  The format of the MinifyImage method is:
%
%      Image *MinifyImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MinifyImage(Image *image,ExceptionInfo *exception)
{
#define Minify(weight) \
  total_red+=(weight)*(s->red); \
  total_green+=(weight)*(s->green); \
  total_blue+=(weight)*(s->blue); \
  total_opacity+=(weight)*(s->opacity); \
  s++;
#define MinifyImageText  "  Minify image...  "

  Image
    *minify_image;

  int
    y;

  register int
    x;

  register PixelPacket
    *p,
    *q,
    *s;

  unsigned long
    total_blue,
    total_green,
    total_opacity,
    total_red;

  /*
    Initialize minified image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 4) || (image->rows < 4))
    return((Image *) NULL);
  minify_image=CloneImage(image,image->columns/2,image->rows/2,False,exception);
  if (minify_image == (Image *) NULL)
    return((Image *) NULL);
  minify_image->storage_class=DirectClass;
  /*
    Reduce each row.
  */
  for (y=0; y < (int) minify_image->rows; y++)
  {
    p=GetImagePixels(image,0,Min(2*y,(int) image->rows-4),
      (int) image->columns,4);
    q=SetImagePixels(minify_image,0,y,minify_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (int) (minify_image->columns-1); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      total_opacity=0;
      s=p;
      Minify(3L); Minify(7L);  Minify(7L);  Minify(3L);
      s=p+image->columns;
      Minify(7L); Minify(15L); Minify(15L); Minify(7L);
      s=p+2*image->columns;
      Minify(7L); Minify(15L); Minify(15L); Minify(7L);
      s=p+3*image->columns;
      Minify(3L); Minify(7L);  Minify(7L);  Minify(3L);
      q->red=((total_red+63L) >> 7L);
      q->green=((total_green+63L) >> 7L);
      q->blue=((total_blue+63L) >> 7L);
      q->opacity=((total_opacity+63L) >> 7L);
      p+=2;
      q++;
    }
    p++;
    *q++=(*p);
    if (!SyncImagePixels(minify_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(MinifyImageText,y,image->rows-1);
  }
  return(minify_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s i z e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResizeImage() scales an image to the desired dimensions with one of these
%  filters:
%
%    Bessel   Blackman   Box
%    Catrom   Cubic      Gaussian
%    Hanning  Hermite    Lanczos
%    Mitchell Point      Quandratic
%    Sinc     Triangle
%
%  If a NULL image is returned, check exception to determine why the image
%  failed to scale.
%
%  The format of the ResizeImage method is:
%
%      Image *ResizeImage(Image *image,const unsigned int columns,
%        const unsigned int rows,const FilterType filter,const double blur,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o filter: Image filter to use.
%
%    o blur: The blur factor where > 1 is blurry, < 1 is sharp.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static double Box(double x)
{
  if ((x >= -0.5) && (x < 0.5))
    return(1.0);
  return(0.0);
}

static double Bessel(double x)
{
  if (x == 0.0)
    return(MagickPI/4.0);
  return(BesselOrderOne(MagickPI*x)/(2.0*x));
}

static double Blackman(double x)
{
  return(0.42+0.50*cos(MagickPI*x)+0.08*cos(2.0*MagickPI*x));
}

static double Catrom(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return(0.5*(2.0+x*x*(-5.0+x*3.0)));
  if (x < 2.0)
    return(0.5*(4.0+x*(-8.0+x*(5.0-x))));
  return(0.0);
}

static double Cubic(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return((0.5*x*x*x)-x*x+(2.0/3.0));
  if (x < 2.0)
    {
      x=2.0-x;
      return((1.0/6.0)*x*x*x);
    }
  return(0.0);
}

static double Gaussian(double x)
{
  return(exp(-2.0*x*x)*sqrt(2.0/MagickPI));
}

static double Hanning(double x)
{
  return(0.5+0.5*cos(MagickPI*x));
}

static double Hamming(double x)
{
  return(0.54+0.46*cos(MagickPI*x));
}

static double Hermite(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return((2.0*x-3.0)*x*x+1.0);
  return(0.0);
}

static double Sinc(double x)
{
  x*=MagickPI;
  if (x != 0.0)
    return(sin(x)/x);
  return(1.0);
}

static double Lanczos(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 3.0)
    return(Sinc(x)*Sinc(x/3.0));
  return(0.0);
}

static double Mitchell(double x)
{
  double
    b,
    c;

  b=1.0/3.0;
  c=1.0/3.0;
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    {
      x=((12.0-9.0*b-6.0*c)*(x*x*x))+((-18.0+12.0*b+6.0*c)*x*x)+(6.0-2.0*b);
      return(x/6.0);
    }
 if (x < 2.0)
   {
     x=((-1.0*b-6.0*c)*(x*x*x))+((6.0*b+30.0*c)*x*x)+((-12.0*b-48.0*c)*x)+
       (8.0*b+24.0*c);
     return(x/6.0);
   }
  return(0.0);
}

static double Quadratic(double x)
{
  if (x < 0)
    x=(-x);
  if (x < 0.5)
    return(0.75-x*x);
  if (x < 1.5)
    {
      x-=1.5;
      return(0.5*x*x);
    }
  return(0.0);
}

static double Triangle(double x)
{
  if (x < 0.0)
    x=(-x);
  if (x < 1.0)
    return(1.0-x);
  return(0.0);
}

static unsigned int HorizontalFilter(Image *source,Image *destination,
  double x_factor,const FilterInfo *filter_info,const double blur,
  ContributionInfo *contribution,const size_t span,unsigned int *quantum)
{
#define ResizeImageText  "  Resize image...  "

  double
    blue,
    center,
    density,
    green,
    opacity,
    red,
    scale,
    support;

  int
    end,
    j,
    n,
    start,
    y;

  register int
    i,
    x;

  register IndexPacket
    *indexes;

  register PixelPacket
    *p,
    *q;

  destination->storage_class=source->storage_class;
  if ((source->columns == destination->columns) &&
      (source->rows == destination->rows))
    {
      /*
        Equal width and height-- just copy pixels.
      */
      for (y=0; y < (int) destination->rows; y++)
      {
        p=SetImagePixels(source,0,y,source->columns,1);
        q=SetImagePixels(destination,0,y,destination->columns,1);
        if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        (void) memcpy(q,p,source->columns*sizeof(PixelPacket));
        indexes=GetIndexes(source);
        if (indexes != (IndexPacket *) NULL)
          (void) memcpy(GetIndexes(destination),indexes,
            source->columns*sizeof(IndexPacket));
        if (!SyncImagePixels(destination))
          break;
        if (QuantumTick(*quantum,span))
          MagickMonitor(ResizeImageText,*quantum,span);
        (*quantum)++;
      }
      return(y == (int) destination->rows);
    }
  /*
    Apply filter to resize horizontally from source to destination.
  */
  scale=blur*Max(1.0/x_factor,1.0);
  support=Max(scale*filter_info->support,0.5);
  if (support > 0.5)
    destination->storage_class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5+MagickEpsilon;
      scale=1.0;
    }
  for (x=0; x < (int) destination->columns; x++)
  {
    density=0.0;
    n=0;
    center=(double) x/x_factor;
    start=(int) Max(center-support+0.5,0);
    end=(int) Min(center+support+0.5,source->columns);
    for (i=start; i < end; i++)
    {
      contribution[n].pixel=i;
      contribution[n].weight=filter_info->function((i-center+0.5)/scale);
      contribution[n].weight/=scale;
      density+=contribution[n].weight;
      n++;
    }
    density=density == 0.0 ? 1.0 : 1.0/density;
    for (i=0; i < n; i++)
      contribution[i].weight*=density;  /* normalize */
    p=GetImagePixels(source,contribution[0].pixel,0,
      contribution[n-1].pixel-contribution[0].pixel+1,source->rows);
    q=SetImagePixels(destination,x,0,1,destination->rows);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(destination);
    for (y=0; y < (int) destination->rows; y++)
    {
      j=0;
      blue=0.0;
      green=0.0;
      red=0.0;
      opacity=0.0;
      for (i=0; i < n; i++)
      {
        j=y*(contribution[n-1].pixel-contribution[0].pixel+1)+
          (contribution[i].pixel-contribution[0].pixel);
        red+=contribution[i].weight*(p+j)->red;
        green+=contribution[i].weight*(p+j)->green;
        blue+=contribution[i].weight*(p+j)->blue;
        opacity+=contribution[i].weight*(p+j)->opacity;
      }
      if (indexes != (IndexPacket *) NULL)
        indexes[y]=(GetIndexes(source))[j];
      q->red=(Quantum)
        ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
      q->green=(Quantum)
        ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
      q->blue=(Quantum)
        ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
      q->opacity=(Quantum)
        ((opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5);
      q++;
    }
    if (!SyncImagePixels(destination))
      break;
    if (QuantumTick(*quantum,span))
      MagickMonitor(ResizeImageText,*quantum,span);
    (*quantum)++;
  }
  return(x == (int) destination->columns);
}

static unsigned int VerticalFilter(Image *source,Image *destination,
  double y_factor,const FilterInfo *filter_info,const double blur,
  ContributionInfo *contribution,const size_t span,unsigned int *quantum)
{
  double
    blue,
    center,
    density,
    green,
    opacity,
    red,
    scale,
    support;

  int
    end,
    j,
    n,
    start,
    x;

  register int
    i,
    y;

  register IndexPacket
    *indexes;

  register PixelPacket
    *p,
    *q;

  destination->storage_class=source->storage_class;
  if ((source->columns == destination->columns) &&
      (source->rows == destination->rows))
    {
      /*
        Equal width and height-- just copy pixels.
      */
      for (y=0; y < (int) destination->rows; y++)
      {
        p=SetImagePixels(source,0,y,source->columns,1);
        q=SetImagePixels(destination,0,y,destination->columns,1);
        if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        (void) memcpy(q,p,source->columns*sizeof(PixelPacket));
        indexes=GetIndexes(source);
        if (indexes != (IndexPacket *) NULL)
          (void) memcpy(GetIndexes(destination),indexes,
            source->columns*sizeof(IndexPacket));
        if (!SyncImagePixels(destination))
          break;
        if (QuantumTick(*quantum,span))
          MagickMonitor(ResizeImageText,*quantum,span);
        (*quantum)++;
      }
      return(y == (int) destination->rows);
    }
  /*
    Apply filter to resize vertically from source to destination.
  */
  scale=blur*Max(1.0/y_factor,1.0);
  support=Max(scale*filter_info->support,0.5);
  if (support > 0.5)
    destination->storage_class=DirectClass;
  else
    {
      /*
        Reduce to point sampling.
      */
      support=0.5+MagickEpsilon;
      scale=1.0;
    }
  for (y=0; y < (int) destination->rows; y++)
  {
    density=0.0;
    n=0;
    center=(double) y/y_factor;
    start=(int) Max(center-support+0.5,0);
    end=(int) Min(center+support+0.5,source->rows);
    for (i=start; i < end; i++)
    {
      contribution[n].pixel=i;
      contribution[n].weight=filter_info->function((i-center+0.5)/scale);
      contribution[n].weight/=scale;
      density+=contribution[n].weight;
      n++;
    }
    density=density == 0.0 ? 1.0 : 1.0/density;
    for (i=0; i < n; i++)
      contribution[i].weight*=density;  /* normalize */
    p=GetImagePixels(source,0,contribution[0].pixel,source->columns,
      contribution[n-1].pixel-contribution[0].pixel+1);
    q=SetImagePixels(destination,0,y,destination->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(destination);
    for (x=0; x < (int) destination->columns; x++)
    {
      j=0;
      blue=0.0;
      green=0.0;
      red=0.0;
      opacity=0.0;
      for (i=0; i < n; i++)
      {
        j=(contribution[i].pixel-contribution[0].pixel)*source->columns+x;
        red+=contribution[i].weight*(p+j)->red;
        green+=contribution[i].weight*(p+j)->green;
        blue+=contribution[i].weight*(p+j)->blue;
        opacity+=contribution[i].weight*(p+j)->opacity;
      }
      if (indexes != (IndexPacket *) NULL)
        indexes[x]=(GetIndexes(source))[j];
      q->red=(Quantum)
        ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
      q->green=(Quantum)
        ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
      q->blue=(Quantum)
        ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
      q->opacity=(Quantum)
        ((opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5);
      q++;
    }
    if (!SyncImagePixels(destination))
      break;
    if (QuantumTick(*quantum,span))
      MagickMonitor(ResizeImageText,*quantum,span);
    (*quantum)++;
  }
  return(y == (int) destination->rows);
}

MagickExport Image *ResizeImage(Image *image,const unsigned int columns,
  const unsigned int rows,const FilterTypes filter,const double blur,
  ExceptionInfo *exception)
{
  ContributionInfo
    *contribution;

  double
    scale,
    support,
    x_factor,
    x_support,
    y_factor,
    y_support;

  Image
    *source_image,
    *resize_image;

  static const FilterInfo
    filters[SincFilter+1] =
    {
      { Box, 0.0 },
      { Box, 0.0 },
      { Box, 0.5 },
      { Triangle, 1.0 },
      { Hermite, 1.0 },
      { Hanning, 1.0 },
      { Hamming, 1.0 },
      { Blackman, 1.0 },
      { Gaussian, 1.25 },
      { Quadratic, 1.5 },
      { Cubic, 2.0 },
      { Catrom, 2.0 },
      { Mitchell, 2.0 },
      { Lanczos, 3.0 },
      { Bessel, 3.2383 },
      { Sinc, 4.0 }
    };

  size_t
    span;

  unsigned int
    quantum,
    status;

  /*
    Initialize resize image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  assert((filter >= 0) && (filter <= SincFilter));
  if ((columns == 0) || (rows == 0))
    ThrowImageException(OptionWarning,"Unable to resize image",
      "image dimensions are zero");
  if ((columns == image->columns) && (rows == image->rows))
    return(CloneImage(image,0,0,False,exception));
  resize_image=CloneImage(image,columns,rows,False,exception);
  if (resize_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate filter contribution info.
  */
  x_factor=(double) resize_image->columns/image->columns;
  scale=blur*Max(1.0/x_factor,1.0);
  x_support=Max(scale*filters[filter].support,0.5);
  y_factor=(double) resize_image->rows/image->rows;
  scale=blur*Max(1.0/y_factor,1.0);
  y_support=Max(scale*filters[filter].support,0.5);
  support=Max(x_support,y_support);
  if (support < filters[filter].support)
    support=filters[filter].support;
  contribution=(ContributionInfo *)
    AcquireMemory((int) (support*2+3)*sizeof(ContributionInfo));
  if (contribution == (ContributionInfo *) NULL)
    {
      DestroyImage(resize_image);
      ThrowImageException(ResourceLimitWarning,"Unable to resize image",
        "Memory allocation failed")
    }
  /*
    Resize image.
  */
  quantum=0;
  if ((size_t) (columns*(image->rows+rows)) <
      (size_t) (rows*(image->columns+columns)))
    {
      source_image=CloneImage(image,columns,image->rows,True,exception);
      if (source_image == (Image *) NULL)
        {
          LiberateMemory((void **) &contribution);
          DestroyImage(resize_image);
          return((Image *) NULL);
        }
      span=source_image->columns+resize_image->rows;
      status=HorizontalFilter(image,source_image,x_factor,&filters[filter],blur,
        contribution,span,&quantum);
      status|=VerticalFilter(source_image,resize_image,y_factor,
        &filters[filter],blur,contribution,span,&quantum);
    }
  else
    {
      source_image=CloneImage(image,image->columns,rows,True,exception);
      if (source_image == (Image *) NULL)
        {
          LiberateMemory((void **) &contribution);
          DestroyImage(resize_image);
          return((Image *) NULL);
        }
      span=resize_image->columns+source_image->columns;
      status=VerticalFilter(image,source_image,y_factor,&filters[filter],blur,
        contribution,span,&quantum);
      status|=HorizontalFilter(source_image,resize_image,x_factor,
        &filters[filter],blur,contribution,span,&quantum);
    }
  /*
    Free allocated memory.
  */
  LiberateMemory((void **) &contribution);
  DestroyImage(source_image);
  if (status == False)
    {
      DestroyImage(resize_image);
      ThrowImageException(CacheWarning,"Unable to resize image",(char *) NULL)
    }
  return(resize_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S a m p l e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SampleImage() scales an image to the desired dimensions with pixel
%  sampling.  Unlike other scaling methods, this method does not introduce
%  any additional color into the scaled image.  If a NULL image is
%  returned, check exception to determine why the image failed to scale.
%
%  The format of the SampleImage method is:
%
%      Image *SampleImage(Image *image,const unsigned int columns,
%        const unsigned int rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the sampled image.
%
%    o rows: The number of rows in the sampled image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SampleImage(Image *image,const unsigned int columns,
  const unsigned int rows,ExceptionInfo *exception)
{
#define SampleImageText  "  Sample image...  "

  double
    *x_offset,
    *y_offset;

  Image
    *sample_image;

  int
    j,
    y;

  PixelPacket
    *pixels;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Initialize sampled image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((columns == 0) || (rows == 0))
    ThrowImageException(OptionWarning,"Unable to resize image",
      "image dimensions are zero");
  if ((columns == image->columns) && (rows == image->rows))
    return(CloneImage(image,0,0,False,exception));
  sample_image=CloneImage(image,columns,rows,False,exception);
  if (sample_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate scan line buffer and column offset buffers.
  */
  pixels=(PixelPacket *) AcquireMemory(image->columns*sizeof(PixelPacket));
  x_offset=(double *) AcquireMemory(sample_image->columns*sizeof(double));
  y_offset=(double *) AcquireMemory(sample_image->rows*sizeof(double));
  if ((pixels == (PixelPacket *) NULL) || (x_offset == (double *) NULL) ||
      (y_offset == (double *) NULL))
    {
      DestroyImage(sample_image);
      ThrowImageException(ResourceLimitWarning,"Unable to sample image",
        "Memory allocation failed")
    }
  /*
    Initialize pixel offsets.
  */
  for (x=0; x < (int) sample_image->columns; x++)
    x_offset[x]=x*image->columns/(double) sample_image->columns;
  for (y=0; y < (int) sample_image->rows; y++)
    y_offset[y]=y*image->rows/(double) sample_image->rows;
  /*
    Sample each row.
  */
  j=(-1);
  for (y=0; y < (int) sample_image->rows; y++)
  {
    q=SetImagePixels(sample_image,0,y,sample_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    if (j != y_offset[y])
      {
        /*
          Read a scan line.
        */
        j=(int) y_offset[y];
        p=GetImagePixels(image,0,j,image->columns,1);
        if (p == (PixelPacket *) NULL)
          break;
        (void) memcpy(pixels,p,image->columns*sizeof(PixelPacket));
      }
    /*
      Sample each column.
    */
    for (x=0; x < (int) sample_image->columns; x++)
      *q++=pixels[(int) x_offset[x]];
    indexes=GetIndexes(sample_image);
    if (indexes != (IndexPacket *) NULL)
      for (x=0; x < (int) sample_image->columns; x++)
        indexes[x]=(GetIndexes(image))[(int) x_offset[x]];
    if (!SyncImagePixels(sample_image))
      break;
    if (QuantumTick(y,sample_image->rows))
      MagickMonitor(SampleImageText,y,sample_image->rows);
  }
  LiberateMemory((void **) &y_offset);
  LiberateMemory((void **) &x_offset);
  LiberateMemory((void **) &pixels);
  return(sample_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S c a l e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ScaleImage() changes the size of an image to the given dimensions.  If
%  a NULL image is returned, check exception to determine why the image failed
%  to scale.
%
%  The format of the ScaleImage method is:
%
%      Image *ScaleImage(Image *image,const unsigned int columns,
%        const unsigned int rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the scaled image.
%
%    o rows: The number of rows in the scaled image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ScaleImage(Image *image,const unsigned int columns,
  const unsigned int rows,ExceptionInfo *exception)
{
#define ScaleImageText  "  Scale image...  "

  typedef struct ScalePacket
  {
    double
      red,
      green,
      blue,
      opacity;
  } ScalePacket;

  double
    blue,
    green,
    opacity,
    red,
    x_scale,
    x_span,
    y_scale,
    y_span;

  Image
    *scale_image;

  int
    number_rows,
    y;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  register ScalePacket
    *s,
    *t;

  ScalePacket
    *scale_scanline,
    *scanline,
    *x_vector,
    *y_vector;

  unsigned int
    next_column,
    next_row;

  /*
    Initialize scaled image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  scale_image=CloneImage(image,columns,rows,False,exception);
  if (scale_image == (Image *) NULL)
    return((Image *) NULL);
  scale_image->storage_class=DirectClass;
  /*
    Allocate memory.
  */
  x_vector=(ScalePacket *) AcquireMemory(image->columns*sizeof(ScalePacket));
  scanline=x_vector;
  if (image->rows != scale_image->rows)
    scanline=(ScalePacket *)
      AcquireMemory(image->columns*sizeof(ScalePacket));
  scale_scanline=(ScalePacket *)
    AcquireMemory(scale_image->columns*sizeof(ScalePacket));
  y_vector=(ScalePacket *) AcquireMemory(image->columns*sizeof(ScalePacket));
  if ((scanline == (ScalePacket *) NULL) ||
      (scale_scanline == (ScalePacket *) NULL) ||
      (x_vector == (ScalePacket *) NULL) || (y_vector == (ScalePacket *) NULL))
    {
      DestroyImage(scale_image);
      ThrowImageException(ResourceLimitWarning,"Unable to scale image",
        "Memory allocation failed")
    }
  /*
    Scale image.
  */
  number_rows=0;
  next_row=True;
  y_span=1.0;
  y_scale=(double) scale_image->rows/image->rows;
  for (x=0; x < (int) image->columns; x++)
  {
    y_vector[x].red=0;
    y_vector[x].green=0;
    y_vector[x].blue=0;
    y_vector[x].opacity=0;
  }
  i=0;
  for (y=0; y < (int) scale_image->rows; y++)
  {
    q=SetImagePixels(scale_image,0,y,scale_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    if (scale_image->rows == image->rows)
      {
        /*
          Read a new scanline.
        */
        p=GetImagePixels(image,0,i++,image->columns,1);
        if (p == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          x_vector[x].red=p->red;
          x_vector[x].green=p->green;
          x_vector[x].blue=p->blue;
          x_vector[x].opacity=p->opacity;
          p++;
        }
      }
    else
      {
        /*
          Scale Y direction.
        */
        while (y_scale < y_span)
        {
          if (next_row && (number_rows < (int) image->rows))
            {
              /*
                Read a new scanline.
              */
              p=GetImagePixels(image,0,i++,image->columns,1);
              if (p == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) image->columns; x++)
              {
                x_vector[x].red=p->red;
                x_vector[x].green=p->green;
                x_vector[x].blue=p->blue;
                x_vector[x].opacity=p->opacity;
                p++;
              }
              number_rows++;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            y_vector[x].red+=y_scale*x_vector[x].red;
            y_vector[x].green+=y_scale*x_vector[x].green;
            y_vector[x].blue+=y_scale*x_vector[x].blue;
            y_vector[x].opacity+=y_scale*x_vector[x].opacity;
          }
          y_span-=y_scale;
          y_scale=(double) scale_image->rows/image->rows;
          next_row=True;
        }
        if (next_row && (number_rows < (int) image->rows))
          {
            /*
              Read a new scanline.
            */
            p=GetImagePixels(image,0,i++,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              x_vector[x].red=p->red;
              x_vector[x].green=p->green;
              x_vector[x].blue=p->blue;
              x_vector[x].opacity=p->opacity;
              p++;
            }
            number_rows++;
            next_row=False;
          }
        s=scanline;
        for (x=0; x < (int) image->columns; x++)
        {
          red=y_vector[x].red+y_span*x_vector[x].red;
          green=y_vector[x].green+y_span*x_vector[x].green;
          blue=y_vector[x].blue+y_span*x_vector[x].blue;
          opacity=y_vector[x].opacity+y_span*x_vector[x].opacity;
          s->red=red > MaxRGB ? MaxRGB : red+0.5;
          s->green=green > MaxRGB ? MaxRGB : green+0.5;
          s->blue=blue > MaxRGB ? MaxRGB : blue+0.5;
          s->opacity=opacity > MaxRGB ? MaxRGB : opacity+0.5;
          s++;
          y_vector[x].red=0;
          y_vector[x].green=0;
          y_vector[x].blue=0;
          y_vector[x].opacity=0;
        }
        y_scale-=y_span;
        if (y_scale <= 0)
          {
            y_scale=(double) scale_image->rows/image->rows;
            next_row=True;
          }
        y_span=1.0;
      }
    if (scale_image->columns == image->columns)
      {
        /*
          Transfer scanline to scaled image.
        */
        s=scanline;
        for (x=0; x < (int) scale_image->columns; x++)
        {
          q->red=(Quantum) s->red;
          q->green=(Quantum) s->green;
          q->blue=(Quantum) s->blue;
          q->opacity=(Quantum) s->opacity;
          q++;
          s++;
        }
      }
    else
      {
        /*
          Scale X direction.
        */
        red=0;
        green=0;
        blue=0;
        opacity=0;
        next_column=False;
        x_span=1.0;
        s=scanline;
        t=scale_scanline;
        for (x=0; x < (int) image->columns; x++)
        {
          x_scale=(double) scale_image->columns/image->columns;
          while (x_scale >= x_span)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                opacity=0;
                t++;
              }
            red+=x_span*s->red;
            green+=x_span*s->green;
            blue+=x_span*s->blue;
            opacity+=x_span*s->opacity;
            t->red=red > MaxRGB ? MaxRGB : red+0.5;
            t->green=green > MaxRGB ? MaxRGB : green+0.5;
            t->blue=blue > MaxRGB ? MaxRGB : blue+0.5;
            t->opacity=opacity > MaxRGB ? MaxRGB : opacity+0.5;
            x_scale-=x_span;
            x_span=1.0;
            next_column=True;
          }
        if (x_scale > 0)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                opacity=0;
                next_column=False;
                t++;
              }
            red+=x_scale*s->red;
            green+=x_scale*s->green;
            blue+=x_scale*s->blue;
            opacity+=x_scale*s->opacity;
            x_span-=x_scale;
          }
        s++;
      }
      if (x_span > 0)
        {
          s--;
          red+=x_span*s->red;
          green+=x_span*s->green;
          blue+=x_span*s->blue;
          opacity+=x_span*s->opacity;
        }
      if (!next_column && ((t-scale_scanline) < (int) scale_image->columns))
        {
          t->red=red > MaxRGB ? MaxRGB : red+0.5;
          t->green=green > MaxRGB ? MaxRGB : green+0.5;
          t->blue=blue > MaxRGB ? MaxRGB : blue+0.5;
          t->opacity=opacity > MaxRGB ? MaxRGB : opacity+0.5;
        }
      /*
        Transfer scanline to scaled image.
      */
      t=scale_scanline;
      for (x=0; x < (int) scale_image->columns; x++)
      {
        q->red=(Quantum) t->red;
        q->green=(Quantum) t->green;
        q->blue=(Quantum) t->blue;
        q->opacity=(Quantum) t->opacity;
        q++;
        t++;
      }
    }
    if (!SyncImagePixels(scale_image))
      break;
    if (QuantumTick(y,scale_image->rows))
      MagickMonitor(ScaleImageText,y,scale_image->rows);
  }
  /*
    Free allocated memory.
  */
  LiberateMemory((void **) &y_vector);
  LiberateMemory((void **) &scale_scanline);
  if (scale_image->rows != image->rows)
    LiberateMemory((void **) &scanline);
  LiberateMemory((void **) &x_vector);
  return(scale_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   Z o o m I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZoomImage creates a new image that is a scaled size of an
%  existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.  The Point filter gives
%  fast pixel replication, Triangle is equivalent to bi-linear interpolation,
%  and Mitchel giver slower, very high-quality results.  See Graphic Gems III
%  for details on this algorithm.
%
%  The filter member of the Image structure specifies which image filter to
%  use. Blur specifies the blur factor where > 1 is blurry, < 1 is sharp.
%
%  The format of the ZoomImage method is:
%
%      Image *ZoomImage(Image *image,const unsigned int columns,
%        const unsigned int rows,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o zoom_image: Method ZoomImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The image.
%
%    o columns: An integer that specifies the number of columns in the zoom
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ZoomImage(Image *image,const unsigned int columns,
  const unsigned int rows,ExceptionInfo *exception)
{
  Image
    *zoom_image;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  zoom_image=ResizeImage(image,columns,rows,image->filter,image->blur,
    exception);
  return(zoom_image);
}
