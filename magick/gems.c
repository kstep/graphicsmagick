/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                           GGGG  EEEEE  M   M SSSSS                          %
%                          G      E      MM MM SS                             %
%                          G GG   EEE    M M M  SSS                           %
%                          G   G  E      M   M    SS                          %
%                           GGGG  EEEEE  M   M SSSSS                          %
%                                                                             %
%                                                                             %
%                    Graphic Gems - Graphic Support Methods                   %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 August 1996                                 %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n s t r a s t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Contrast enhances the intensity differences between the lighter
%  and darker elements of the image.
%
%  The format of the ContrastImage routine is:
%
%      Contrast(sign,red,green,blue)
%
%  A description of each parameter follows:
%
%    o sign: A positive value enhances the contrast otherwise it is reduced.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
Export void Contrast(const int sign,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    brightness,
    hue,
    saturation,
    theta;

  /*
    Enhance contrast: dark color become darker, light color become lighter.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  theta=(brightness-0.5)*M_PI;
  brightness+=(((((sin(theta)+1.0))*0.5)-brightness)*sign)*0.5;
  if (brightness > 1.0)
    brightness=1.0;
  else
    if (brightness < 0)
      brightness=0.0;
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e n e r a t e N o i s e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GenerateNoise adds noise to a pixel.
%
%  The format of the GenerateNoise routine is:
%
%      GenerateNoise(pixel,noise_type)
%
%  A description of each parameter follows:
%
%    o pixel: A structure of type Quantum.
%
%    o noise_type:  The type of noise: gaussian, multiplicative gaussian,
%      impulse, laplacian, or poisson.
%
%
*/
Export Quantum GenerateNoise(Quantum pixel,NoiseType noise_type)
{
#define NoiseEpsilon  1.0e-5
#define NoiseMask  0x7fff
#define SigmaUniform  4.0
#define SigmaGaussian  4.0
#define SigmaImpulse  0.10
#define SigmaLaplacian 10.0
#define SigmaMultiplicativeGaussian  0.5
#define SigmaPoisson  0.05
#define TauGaussian  20.0

  double
    alpha,
    beta,
    sigma,
    value;

  alpha=(double) (rand() & NoiseMask)/NoiseMask;
  if (alpha == 0.0)
    alpha=1.0;
  switch (noise_type)
  {
    case UniformNoise:
    default:
    {
      value=(double) pixel+SigmaUniform*(alpha-0.5);
      break;
    }
    case GaussianNoise:
    {
      double
        tau;

      beta=(double) (rand() & NoiseMask)/NoiseMask;
      sigma=sqrt(-2.0*log(alpha))*cos(2.0*M_PI*beta);
      tau=sqrt(-2.0*log(alpha))*sin(2.0*M_PI*beta);
      value=(double) pixel+
        (sqrt((double) pixel)*SigmaGaussian*sigma)+(TauGaussian*tau);
      break;
    }
    case MultiplicativeGaussianNoise:
    {
      if (alpha <= NoiseEpsilon)
        sigma=MaxRGB;
      else
        sigma=sqrt(-2.0*log(alpha));
      beta=(rand() & NoiseMask)/NoiseMask;
      value=(double) pixel+
        pixel*SigmaMultiplicativeGaussian*sigma*cos(2.0*M_PI*beta);
      break;
    }
    case ImpulseNoise:
    {
      if (alpha < (SigmaImpulse/2.0))
        value=0;
       else
         if (alpha >= (1.0-(SigmaImpulse/2.0)))
           value=MaxRGB;
         else
           value=pixel;
      break;
    }
    case LaplacianNoise:
    {
      if (alpha <= 0.5)
        {
          if (alpha <= NoiseEpsilon)
            value=(double) pixel-MaxRGB;
          else
            value=(double) pixel+SigmaLaplacian*log(2.0*alpha);
          break;
        }
      beta=1.0-alpha;
      if (beta <= (0.5*NoiseEpsilon))
        value=(double) pixel+MaxRGB;
      else
        value=(double) pixel-SigmaLaplacian*log(2.0*beta);
      break;
    }
    case PoissonNoise:
    {
      register int
        i;

      for (i=0; alpha > exp(-SigmaPoisson*pixel); i++)
      {
        beta=(double) (rand() & NoiseMask)/NoiseMask;
        alpha=alpha*beta;
      }
      value=i/SigmaPoisson;
      break;
    }
  }
  if (value < 0.0)
    return(0);
  if (value > MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H S L T r a n s f o r m                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HSLTransform converts a (hue, saturation, luminosity) to a
%  (red, green, blue) triple.
%
%  The format of the HSLTransformImage routine is:
%
%      HSLTransform(hue,saturation,luminosity,red,green,blue)
%
%  A description of each parameter follows:
%
%    o hue, saturation, luminosity: A double value representing a
%      component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
Export void HSLTransform(double hue,const double saturation,
  const double luminosity,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    g,
    r,
    v,
    x,
    y,
    z;

  /*
    Convert HSL to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  v=(luminosity <= 0.5) ? (luminosity*(1.0+saturation)) :
    (luminosity+saturation-luminosity*saturation);
  if ((saturation == 0.0) || (hue == -1.0))
    {
      *red=(Quantum) floor((luminosity*(double) MaxRGB)+0.5);
      *green=(Quantum) floor((luminosity*(double) MaxRGB)+0.5);
      *blue=(Quantum) floor((luminosity*(double) MaxRGB)+0.5);
      return;
    }
  hue*=6.0;
  y=2*luminosity-v;
  x=y+(v-y)*(hue-(int) hue);
  z=v-(v-y)*(hue-(int) hue);
  switch ((int) hue)
  {
    default: r=v; g=x; b=y; break;
    case 0: r=v; g=x; b=y; break;
    case 1: r=z; g=v; b=y; break;
    case 2: r=y; g=v; b=x; break;
    case 3: r=y; g=z; b=v; break;
    case 4: r=x; g=y; b=v; break;
    case 5: r=v; g=y; b=z; break;
  }
  *red=(Quantum) floor((r*(double) MaxRGB)+0.5);
  *green=(Quantum) floor((g*(double) MaxRGB)+0.5);
  *blue=(Quantum) floor((b*(double) MaxRGB)+0.5);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u l l                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Hull implements the eight hull algorithm described in Applied
%  Optics, Vol. 24, No. 10, 15 May 1985, "Geometric filter for Speckle
%  Reduction", by Thomas R Crimmins.  Each pixel in the image is replaced by
%  one of its eight of its surrounding pixels using a polarity and negative
%  hull function.
%
%  The format of the Hull routine is:
%
%      Hull(x_offset,y_offset,polarity,columns,rows,f,g)
%
%  A description of each parameter follows:
%
%    o x_offset, y_offset: An integer value representing the offset of the
%      current pixel within the image.
%
%    o polarity: An integer value declaring the polarity (+,-).
%
%    o columns, rows: Specifies the number of rows and columns in the image.
%
%    o f, g: A pointer to an image pixel and one of it's neighbor.
%
%
*/
Export void Hull(int x_offset,int y_offset,int polarity,unsigned int columns,
  unsigned int rows,Quantum *f,Quantum *g)
{
  int
    y;

  register int
    x;

  register Quantum
    *p,
    *q,
    *r,
    *s;

  Quantum
    v;

  assert(f != (Quantum *) NULL);
  assert(g != (Quantum *) NULL);
  p=f+(columns+2);
  q=g+(columns+2);
  r=p+(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < (int) rows; y++)
  {
    p++;
    q++;
    r++;
    if (polarity > 0)
      for (x=0; x < (int) columns; x++)
      {
        v=(*p);
        if (*r > v)
          v++;
        *q=v;
        p++;
        q++;
        r++;
      }
    else
      for (x=0; x < (int) columns; x++)
      {
        v=(*p);
        if (v > (Quantum) (*r+1))
          v--;
        *q=v;
        p++;
        q++;
        r++;
      }
    p++;
    q++;
    r++;
  }
  p=f+(columns+2);
  q=g+(columns+2);
  r=q+(y_offset*((int) columns+2)+x_offset);
  s=q-(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < (int) rows; y++)
  {
    p++;
    q++;
    r++;
    s++;
    if (polarity > 0)
      for (x=0; x < (int) columns; x++)
      {
        v=(*q);
        if (((Quantum) (*s+1) > v) && (*r > v))
          v++;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    else
      for (x=0; x < (int) columns; x++)
      {
        v=(*q);
        if (((Quantum) (*s+1) < v) && (*r < v))
          v--;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    p++;
    q++;
    r++;
    s++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n s i d e P r i m i t i v e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InsidePrimitive returns True if the (x,y) position is inside the
%  primitive as defined in primitive_info.  Otherwise False is returned.
%
%  The format of the InsidePrimitive routine is:
%
%      status=InsidePrimitive(primitive_info,annotate_info,x,y,image)
%
%  A description of each parameter follows:
%
%    o status:  Method InsidePrimitive returns True if the (x,y) position
%      is inside the primitive as defined in primitive_info.  Otherwise False
%      is returned.
%
%    o primitive_info: Specifies a pointer to a PrimitiveInfo structure.
%
%    o annotate_info: Specifies a pointer to a AnnotateInfo structure.
%
%    o x,y: Integers representing the (x,y) location in the image.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int InsidePrimitive(PrimitiveInfo *primitive_info,
  AnnotateInfo *annotate_info,int x,int y,Image *image)
{
  double
    mid,
    slope,
    trix,
    triy;

  register int
    i;

  register PrimitiveInfo
    *p,
    *q;

  register unsigned int
    inside;

  RunlengthPacket
    target;

  XColor
    border_color;

  assert(primitive_info != (PrimitiveInfo *) NULL);
  assert(annotate_info != (AnnotateInfo *) NULL);
  assert(image != (Image *) NULL);
  inside=False;
  mid=annotate_info->linewidth/2.0;
  p=primitive_info;
  while (p->primitive != UndefinedPrimitive)
  {
    q=p+p->coordinates-1;
    switch (p->primitive)
    {
      case PointPrimitive:
      default:
      {
        for ( ; (p < q) && !inside; p++)
          inside=(x == p->x) && (y == p->y);
        break;
      }
      case LinePrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          if (p->x == (p+1)->x)
            inside=(x >= (p->x-mid)) && (x < (p->x+mid)) &&
              (y >= Min(p->y,(p+1)->y)) && (y <= Max(p->y,(p+1)->y));
          else
            if (p->y == (p+1)->y)
              inside=(x >= Min(p->x,(p+1)->x)) && (x <= Max(p->x,(p+1)->x)) &&
                (y >= (p->y-mid)) && (y < (p->y+mid));
            else
              {
                slope=(double) (p->y-(p+1)->y)/(p->x-(p+1)->x);
                trix=(slope*p->x+x/slope+y-p->y)/(slope+1.0/slope);
                triy=slope*(trix-p->x)+p->y;
                inside=(((x-trix)*(x-trix)+(y-triy)*(y-triy)) <= (mid*mid)) &&
                  (trix >= Min(p->x,(p+1)->x)) && (trix <= Max(p->x,(p+1)->x));
              }
          p++;
        }
        break;
      }
      case RectanglePrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          inside=(x >= Min(p->x-mid,(p+1)->x+mid)) &&
            (x < Max(p->x-mid,(p+1)->x+mid)) &&
            (y >= Min(p->y-mid,(p+1)->y+mid)) &&
            (y < Max(p->y-mid,(p+1)->y+mid));
          inside&=!((x >= Min(p->x+mid,(p+1)->x-mid)) &&
            (x < Max(p->x+mid,(p+1)->x-mid)) &&
            (y >= Min(p->y+mid,(p+1)->y-mid)) &&
            (y < Max(p->y+mid,(p+1)->y-mid)));
          p++;
        }
        break;
      }
      case FillRectanglePrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          inside=(x >= Min(p->x,(p+1)->x)) && (x <= Max(p->x,(p+1)->x)) &&
            (y >= Min(p->y,(p+1)->y)) && (y <= Max(p->y,(p+1)->y));
          p++;
        }
        break;
      }
      case CirclePrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          inside=(((p->y-y)*(p->y-y))+((p->x-x)*(p->x-x))) <=
            (((p->y-(p+1)->y-mid)*(p->y-(p+1)->y-mid))+
            ((p->x-(p+1)->x-mid)*(p->x-(p+1)->x-mid)));
          inside&=!((((p->y-y)*(p->y-y))+((p->x-x)*(p->x-x))) <=
            (((p->y-(p+1)->y+mid)*(p->y-(p+1)->y+mid))+
            ((p->x-(p+1)->x+mid)*(p->x-(p+1)->x+mid))));
          p++;
        }
        break;
      }
      case FillCirclePrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          inside=((p->y-y)*(p->y-y))+((p->x-x)*(p->x-x)) <=
            ((p->y-(p+1)->y)*(p->y-(p+1)->y))+((p->x-(p+1)->x)*(p->x-(p+1)->x));
          p++;
        }
        break;
      }
      case PolygonPrimitive:
      {
        for ( ; (p < q) && !inside; p++)
        {
          if (p->x == (p+1)->x)
            {
              inside|=(x >= (p->x-mid)) && (x < (p->x+mid)) &&
                (y >= (Min(p->y,(p+1)->y)-mid)) &&
                (y < (Max(p->y,(p+1)->y)+mid));
              continue;
            }
          if (p->y == (p+1)->y)
            {
              inside|=(x >= Min(p->x,(p+1)->x)-mid) &&
                (x < Max(p->x,(p+1)->x)+mid) &&
                (y >= (p->y-mid)) && (y < (p->y+mid));
              continue;
            }
          slope=(double) (p->y-(p+1)->y)/(p->x-(p+1)->x);
          trix=(slope*p->x+x/slope+y-p->y)/(slope+1.0/slope);
          triy=slope*(trix-p->x)+p->y;
          inside|=(((x-trix)*(x-trix)+(y-triy)*(y-triy)) <= (mid*mid)) &&
            (trix >= Min(p->x,(p+1)->x)) && (trix <= Max(p->x,(p+1)->x));
        }
        while (p <= q)
          p++;
        if (inside)
          break;
        p--;
        q=primitive_info;
        if (p->x == q->x)
          inside=(x >= (p->x-mid)) && (x < (p->x+mid)) &&
            (y >= (Min(p->y,q->y)-mid)) && (y < (Max(p->y,q->y)+mid));
        else
          if (p->y == q->y)
            inside=(x >= (Min(p->x,q->x)-mid)) && (x < (Max(p->x,q->x)+mid)) &&
              (y >= (p->y-mid)) && (y < (p->y+mid));
          else
            {
              slope=(double) (p->y-q->y)/(p->x-q->x);
              trix=(slope*p->x+x/slope+y-p->y)/(slope+1.0/slope);
              triy=slope*(trix-p->x)+p->y;
              inside=(((x-trix)*(x-trix)+(y-triy)*(y-triy)) <= (mid*mid)) &&
                (trix >= Min(p->x,q->x)) && (trix <= Max(p->x,q->x));
            }
        p++;
        break;
      }
      case FillPolygonPrimitive:
      {
        int
          crossing,
          crossings;

        crossings=0;
        if ((q->y >= y) != (p->y >= y))
          {
            crossing=q->x >= x;
            if (crossing != (p->x >= x))
              crossings+=(q->x-(q->y-y)*(p->x-q->x)/(p->y-q->y)) >= x;
            else
              if (crossing)
                crossings++;
          }
        for (p++; p <= q; p++)
        {
          if ((p-1)->y >= y)
            {
              while ((p <= q) && (p->y >= y))
                p++;
              if (p > q)
                break;
              crossing=(p-1)->x >= x;
              if (crossing != (p->x >= x))
                crossings+=
                  ((p-1)->x-((p-1)->y-y)*(p->x-(p-1)->x)/(p->y-(p-1)->y)) >= x;
              else
                if (crossing)
                  crossings++;
              continue;
            }
         while ((p <= q) && (p->y < y))
           p++;
         if (p > q)
           break;
         crossing=(p-1)->x >= x;
         if (crossing != (p->x >= x))
           crossings+=
             ((p-1)->x-((p-1)->y-y)*(p->x-(p-1)->x)/(p->y-(p-1)->y)) >= x;
         else
           if (crossing)
             crossings++;
        }
        inside=crossings & 0x01;
        break;
      }
      case ColorPrimitive:
      {
        for ( ; (p <= q) && !inside; p++)
          switch (p->method)
          {
            case PointMethod:
            default:
            {
              if ((p->x != x) || (p->y != y))
                break;
              inside=True;
              break;
            }
            case ReplaceMethod:
            {
              RunlengthPacket
                color;

              if ((x == 0) && (y == 0))
                target=image->pixels[p->y*image->columns+p->x];
              color=image->pixels[y*image->columns+x];
              inside=ColorMatch(color,target,(int) image->fuzz);
              break;
            }
            case FloodfillMethod:
            case FillToBorderMethod:
            {
              ColorPacket
                color;

              if ((p->x != x) || (p->y != y))
                break;
              target=image->pixels[y*image->columns+x];
              if (p->method == FillToBorderMethod)
                {
                  (void) XQueryColorDatabase(annotate_info->border_color,
                    &border_color);
                  target.red=XDownScale(border_color.red);
                  target.green=XDownScale(border_color.green);
                  target.blue=XDownScale(border_color.blue);
                }
              ColorFloodfillImage(image,&target,annotate_info->pen,x,y,
                p->method);
              break;
            }
            case ResetMethod:
            {
              inside=True;
              break;
            }
          }
        break;
      }
      case MattePrimitive:
      {
        if (!image->matte)
          {
            /*
              Initialize matte image.
            */
            image->class=DirectClass;
            image->matte=True;
            for (i=0; i < (int) image->packets; i++)
              image->pixels[i].index=Opaque;
          }
        for ( ; p <= q; p++)
          switch (p->method)
          {
            case PointMethod:
            default:
            {
              if ((p->x != x) || (p->y != y))
                break;
              image->pixels[y*image->columns+x].index=Transparent;
              break;
            }
            case ReplaceMethod:
            {
              RunlengthPacket
                color;

              static RunlengthPacket
                target;

              if ((x == 0) && (y == 0))
                target=image->pixels[p->y*image->columns+p->x];
              color=image->pixels[y*image->columns+x];
              if (ColorMatch(color,target,(int) image->fuzz))
                image->pixels[y*image->columns+x].index=Transparent;
              break;
            }
            case FloodfillMethod:
            case FillToBorderMethod:
            {
              if ((p->x != x) || (p->y != y))
                break;
              target=image->pixels[y*image->columns+x];
              if (p->method == FillToBorderMethod)
                {
                  (void) XQueryColorDatabase(annotate_info->border_color,
                    &border_color);
                  target.red=XDownScale(border_color.red);
                  target.green=XDownScale(border_color.green);
                  target.blue=XDownScale(border_color.blue);
                }
              MatteFloodfillImage(image,&target,Transparent,x,y,p->method);
              break;
            }
            case ResetMethod:
            {
              image->pixels[y*image->columns+x].index=Opaque;
              break;
            }
          }
        break;
      }
      case TextPrimitive:
      case ImagePrimitive:
      {
        register char
          *r;

        for ( ; p <= q; p++)
        {
          if ((p->x != x) || (p->y != y))
            continue;
          if (p->text == (char *) NULL)
            continue;
          r=p->text;
          if (*r == '"')
            {
              p->text++;
              for (r++; *r != '\0'; r++)
                if ((*r == '"') && (*(r-1) != '\\'))
                  break;
            }
          else
            if (*r == '\'')
              {
                p->text++;
                for (r++; *r != '\0'; r++)
                  if ((*r == '\'') && (*(r-1) != '\\'))
                    break;
              }
            else
              for (r++;  *r != '\0'; r++)
                if (isspace((int) *r) && (*(r-1) != '\\'))
                  break;
          (void) strncpy(annotate_info->text,p->text,r-p->text);
          annotate_info->text[r-p->text]='\0';
          if (p->primitive == TextPrimitive)
            {
              FormatString(annotate_info->geometry,"%+d%+d",p->x,p->y);
              AnnotateImage(image,annotate_info);
            }
          else
            {
              Image
                *composite_image;

              ImageInfo
                composite_info;

              GetImageInfo(&composite_info);
              (void) strcpy(composite_info.filename,annotate_info->text);
              composite_image=ReadImage(&composite_info);
              if (composite_image != (Image *) NULL)
                {
                  CompositeImage(image,ReplaceCompositeOp,composite_image,
                    p->x,p->y);
                  DestroyImage(composite_image);
                }
            }
        }
        break;
      }
    }
    if (inside)
      return(True);
  }
  return(inside);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n t e r p o l a t e C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InterpolateColor applies bi-linear interpolation between a pixel and
%  it's neighbors.
%
%  The format of the InterpolateColor routine is:
%
%      InterpolateColor(image,x,y)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o x,y: A double representing the current (x,y) position of the pixel.
%
%
*/
Export ColorPacket InterpolateColor(Image *image,double x,double y)
{
  ColorPacket
    interpolated_pixel;

  double
    alpha,
    beta;

  register RunlengthPacket
    *p,
    *q,
    *r,
    *s;

  RunlengthPacket
    background_pixel;

  assert(image != (Image *) NULL);
  if (image->packets != (image->columns*image->rows))
    if (!UncondenseImage(image))
      return(image->background_color);
  if ((x < -1) || (x >= image->columns) || (y < -1) || (y >= image->rows))
    return(image->background_color);
  background_pixel.red=image->background_color.red;
  background_pixel.green=image->background_color.green;
  background_pixel.blue=image->background_color.blue;
  background_pixel.index=image->background_color.index;
  if ((x >= 0) && (y >= 0))
    {
      p=image->pixels+((int) y)*image->columns+(int) x;
      q=p+1;
      if ((x+1) >= image->columns)
        q=(&background_pixel);
      r=p+image->columns;
      if ((y+1) >= image->rows)
        r=(&background_pixel);
      s=p+image->columns+1;
      if (((x+1) >= image->columns) || ((y+1) >= image->rows))
        s=(&background_pixel);
    }
  else
    {
      p=(&background_pixel);
      q=(&background_pixel);
      r=image->pixels+(int) x;
      s=r+1;
      if ((x >= -1) && (x < 0))
        {
          q=image->pixels+(int) y*image->columns;
          r=(&background_pixel);
          s=q+(int) image->columns;
          if ((y >= -1) && (y < 0))
            {
              q=(&background_pixel);
              s=image->pixels;
            }
        }
    }
  x-=floor(x);
  y-=floor(y);
  alpha=1.0-x;
  beta=1.0-y;
  interpolated_pixel.red=(Quantum)
    (beta*(alpha*p->red+x*q->red)+y*(alpha*r->red+x*s->red));
  interpolated_pixel.green=(Quantum)
    (beta*(alpha*p->green+x*q->green)+y*(alpha*r->green+x*s->green));
  interpolated_pixel.blue=(Quantum)
    (beta*(alpha*p->blue+x*q->blue)+y*(alpha*r->blue+x*s->blue));
  interpolated_pixel.index=(unsigned short)
    (beta*(alpha*p->index+x*q->index)+y*(alpha*r->index+x*s->index));
  return(interpolated_pixel);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d u l a t e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Modulate modulates the hue, saturation, and brightness of an
%  image.
%
%  The format of the ModulateImage routine is:
%
%      Modulate(percent_hue,percent_saturation,percent_brightness,red,green,
%        blue)
%
%  A description of each parameter follows:
%
%    o percent_hue, percent_saturation, percent_brightness: A double value
%      representing the percent change in a component of the HSL color space.
%
%    o red, green, blue: A pointer to a pixel component of type Quantum.
%
%
*/
Export void Modulate(double percent_hue,double percent_saturation,
  double percent_brightness,Quantum *red,Quantum *green,Quantum *blue)
{
  double
    brightness,
    hue,
    saturation;

  /*
    Increase or decrease color brightness, saturation, or hue.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  TransformHSL(*red,*green,*blue,&hue,&saturation,&brightness);
  brightness+=percent_brightness/100.0;
  if (brightness < 0.0)
    brightness=0.0;
  else
    if (brightness > 1.0)
      brightness=1.0;
  saturation+=percent_saturation/100.0;
  if (saturation < 0.0)
    saturation=0.0;
  else
    if (saturation > 1.0)
      saturation=1.0;
  if (hue != -1.0)
    {
      hue+=percent_hue/100.0;
      if (hue < 0.0)
        hue+=1.0;
      else
        if (hue > 1.0)
          hue-=1.0;
    }
  HSLTransform(hue,saturation,brightness,red,green,blue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m H S L                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformHSL converts a (red, green, blue) to a (hue, saturation,
%  luminosity) triple.
%
%  The format of the TransformHSL routine is:
%
%      TransformHSL(red,green,blue,hue,saturation,luminosity)
%
%  A description of each parameter follows:
%
%    o red, green, blue: A Quantum value representing the red, green, and
%      blue component of a pixel..
%
%    o hue, saturation, luminosity: A pointer to a double value representing a
%      component of the HSL color space.
%
%
*/
Export void TransformHSL(const Quantum red,const Quantum green,
  const Quantum blue,double *hue,double *saturation,double *luminosity)
{
  double
    b,
    delta,
    g,
    max,
    min,
    r;

  /*
    Convert RGB to HSL colorspace.
  */
  assert(hue != (double *) NULL);
  assert(saturation != (double *) NULL);
  assert(luminosity != (double *) NULL);
  r=(double) red/(double) MaxRGB;
  g=(double) green/(double) MaxRGB;
  b=(double) blue/(double) MaxRGB;
  max=Max(r,Max(g,b));
  min=Min(r,Min(g,b));
  *hue=(-1.0);
  *saturation=0.0;
  *luminosity=(min+max)/2.0;
  delta=max-min;
  if (delta == 0.0)
    return;
  *saturation=delta;
  *saturation/=(*luminosity <= 0.5) ? (min+max) : (2.0-max-min) ;
  if (r == max)
    *hue=(g == min ? 5.0+(max-b)/delta : 1.0-(max-g)/delta);
  else
    if (g == max)
      *hue=(b == min ? 1.0+(max-r)/delta : 3.0-(max-b)/delta);
    else
      *hue=(r == min ? 3.0+(max-g)/delta : 5.0-(max-r)/delta);
  *hue/=6.0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U p s a m p l e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Upsample
%
%  The format of the UpSample routine is:
%
%      Upsample(width,height,scaled_width,pixels)
%
%  A description of each parameter follows:
%
%    o width,height:  Unsigned values representinf the width and height of
%      the image pixel array.
%
%    o scaled_width:  Specifies the final width of the upsampled pixel array.
%
%    o pixels:  An unsigned char containing the pixel data.  On output the
%      upsampled pixels are returned here.
%
%
*/
Export void Upsample(unsigned int width,unsigned int height,
  unsigned int scaled_width,unsigned char *pixels)
{
  register int
    x,
    y;

  register unsigned char
    *p,
    *q,
    *r;

  /*
    Create a new image that is a integral size greater than an existing one.
  */
  assert(pixels != (unsigned char *) NULL);
  for (y=0; y < (int) height; y++)
  {
    p=pixels+(height-1-y)*scaled_width+(width-1);
    q=pixels+((height-1-y) << 1)*scaled_width+((width-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < (int) width; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      *(q+1)=(((int) *p)+((int) *(p+1))+1) >> 1;
    }
  }
  for (y=0; y < (int) (height-1); y++)
  {
    p=pixels+(y << 1)*scaled_width;
    q=p+scaled_width;
    r=q+scaled_width;
    for (x=0; x < (int) (width-1); x++)
    {
      *q=(((int) *p)+((int) *r)+1) >> 1;
      *(q+1)=(((int) *p)+((int) *(p+2))+((int) *r)+((int) *(r+2))+2) >> 2;
      q+=2;
      p+=2;
      r+=2;
    }
    *q++=(((int) *p++)+((int) *r++)+1) >> 1;
    *q++=(((int) *p++)+((int) *r++)+1) >> 1;
  }
  p=pixels+(2*height-2)*scaled_width;
  q=pixels+(2*height-1)*scaled_width;
  for (x=0; x < (int) width; x++)
  {
    *q++=(*p++);
    *q++=(*p++);
  }
}
