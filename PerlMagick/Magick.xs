/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                         PPPP   EEEEE  RRRR   L                              %
%                         P   P  E      R   R  L                              %
%                         PPPP   EEE    RRRR   L                              %
%                         P      E      R  R   L                              %
%                         P      EEEEE  R   R  LLLLL                          %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%                Object-oriented Perl interface to ImageMagick                %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                              Kyle Shorter                                   %
%                              John Cristy                                    %
%                             February 1997                                   %
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
% PerlMagick is an objected-oriented Perl interface to ImageMagick.  Use
% the module to read, manipulate, or write an image or image sequence from
% within a Perl script.  This makes PerlMagick suitable for Web CGI scripts.
%
*/

/*
  Include declarations.
*/
#if !defined(WIN32)
#define MagickExport
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <math.h>
#include <magick/api.h>
#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)
#define False 0
#define True 1
#undef tainted
#if !defined(WIN32)
#include <setjmp.h>
#else
#undef setjmp
#undef longjmp
#include <setjmpex.h>
#endif

#ifdef __cplusplus
}
#endif

/*
  Define declarations.
*/
#define ArrayReference  (char **) 4
#define DoubleReference  (char **) 2
#define EndOf(array)  (&array[NumberOf(array)])
#define ImageReference  (char **) 3
#define IntegerReference  (char **) 1
#define MaxArguments  20
#define NumberOf(array)  (sizeof(array)/sizeof(*array))
#define PackageName   "Image::Magick"
#define StringReference  (char **) 0

/*
  Perl 5.006 no longer defines na and sv_undef.
*/
#ifndef na
#define na  PL_na
#endif
#ifndef sv_undef
#define sv_undef  PL_sv_undef
#endif

/*
  Typedef and structure declarations.
*/
typedef void
  *Image__Magick;  /* data type for the Image::Magick package */

/*
  The data type for remembering options set by the user, which basically
  correspond to ImageMagick's command line options.  Each AV* type of
  Image__Magick has a special variable created for it (see GetPackageInfo) that
  holds one.
*/
struct PackageInfo
{
  ImageInfo
    *image_info;

  QuantizeInfo
    *quantize_info;
};

/*
  The different types of arguments that can be passed as arguments from Perl.
*/
union ArgumentList
{
  int
    int_reference;

  double
    double_reference;

  char
    *string_reference;

  Image
    *image_reference;

  SV
    *array_reference;
};

/*
  Static declarations.
*/
/*
  These arrays match the ImageMagick enums.  The positions must match between
  both.  The names are the minimum length to match, e.g., in NoiseTypes
  AddNoise("laplacian") and AddNoise("LaplacianNoise") both work.
*/
static char
  *BooleanTypes[] =
  {
    "False", "True", (char *) NULL
  },
  *ClassTypes[] =
  {
    "Undefined", "DirectClass", "PseudoClass", (char *) NULL
  },
  *ColorspaceTypes[] =
  {
    "Undefined", "RGB", "Gray", "Transparent", "OHTA", "XYZ", "YCbCr",
    "YCC", "YIQ", "YPbPr", "YUV", "CMYK", "sRGB", (char *) NULL
  },
  *CompositeTypes[] =
  {
    "Undefined", "Over", "In", "Out", "Atop", "Xor", "Plus", "Minus",
    "Add", "Subtract", "Difference", "Multiply", "Bumpmap", "Copy",
    "CopyRed", "CopyGreen", "CopyBlue", "CopyOpacity", "Clear", "Dissolve",
    "Displace", "Modulate", "Threshold", (char *) NULL
  },
  *CompressionTypes[] =
  {
    "Undefined", "None", "BZip", "Fax", "Group4", "JPEG", "LosslessJPEG",
    "LZW", "Runlength", "Zip", (char *) NULL
  },
  *FilterTypess[] =
  {
    "Undefined", "Point", "Box", "Triangle", "Hermite", "Hanning",
    "Hamming", "Blackman", "Gaussian", "Quadratic", "Cubic", "Catrom",
    "Mitchell", "Lanczos", "Bessel", "Sinc", (char *) NULL
  },
  *GravityTypes[] =
  {
    "Forget", "NorthWest", "North", "NorthEast", "West", "Center",
    "East", "SouthWest", "South", "SouthEast", "Static", (char *) NULL
  },
  *ImageTypes[] =
  {
    "Undefined", "bilevel", "grayscale", "palette", "palette with transparency",
    "true color", "true color with transparency", "color separated",
    (char *) NULL
  },
  *IntentTypes[] =
  {
    "Undefined", "Saturation", "Perceptual", "Absolute", "Relative",
    (char *) NULL
  },
  *InterlaceTypes[] =
  {
    "Undefined", "None", "Line", "Plane", "Partition", (char *) NULL
  },
  *LayerTypes[] =
  {
    "Undefined", "Red", "Green", "Blue", "Matte", (char *) NULL
  },
  *MethodTypes[] =
  {
    "Point", "Replace", "Floodfill", "FillToBorder", "Reset", (char *) NULL
  },
  *ModeTypes[] =
  {
    "Undefined", "Frame", "Unframe", "Concatenate", (char *) NULL
  },
  *NoiseTypes[] =
  {
    "Uniform", "Gaussian", "Multiplicative", "Impulse", "Laplacian",
    "Poisson", (char *) NULL
  },
  *ProfileTypes[] =
  {
    "Undefined", "ICM", "IPTC", (char *) NULL
  },
  *PreviewTypes[] =
  {
    "Undefined", "Rotate", "Shear", "Roll", "Hue", "Saturation",
    "Brightness", "Gamma", "Spiff", "Dull", "Grayscale", "Quantize",
    "Despeckle", "ReduceNoise", "AddNoise", "Sharpen", "Blur",
    "Threshold", "EdgeDetect", "Spread", "Solarize", "Shade", "Raise",
    "Segment", "Swirl", "Implode", "Wave", "OilPaint", "Charcoal",
    "JPEG", (char *) NULL
  },
  *PrimitiveTypes[] =
  {
    "Undefined", "point", "line", "rectangle", "roundRectangle", "arc",
    "ellipse", "circle", "polyline", "polygon", "bezier", "path", "color",
    "matte", "text", "image", (char *) NULL
  },
  *ResolutionTypes[] =
  {
    "Undefined", "PixelsPerInch", "PixelsPerCentimeter", (char *) NULL
  };

/*
  The list of ImageMagick methods and their parameters currently
  supported by this interface (except for Read, Write, and new).  I did
  it this way to avoid a lot of duplicated code, and to use the neato
  ALIASes that XS provides.  Parameter names are matched up to the
  name's length so that e.g. the parameter name "comment" matches
  "commentstuff".
*/
static struct
  Methods
  {
    char
      *name;

    struct arguments
    {
      char
        *method,
        **type;
    } arguments[MaxArguments];
  } Methods[] =
  {
    { "Comment", { {"comment", StringReference} } },
    { "Label", { {"label", StringReference} } },
    { "AddNoise", { {"noise", NoiseTypes} } },
    { "Colorize", { {"fill", StringReference}, {"opacity", StringReference} } },
    { "Border", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"fill", StringReference},
      {"color", StringReference} } },
    { "Blur", { {"geom", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Chop", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Crop", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Despeckle", },
    { "Edge", { {"geom", StringReference}, {"radius", DoubleReference} } },
    { "Emboss", { {"geom", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Enhance", },
    { "Flip", },
    { "Flop", },
    { "Frame", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"inner", IntegerReference},
      {"outer", IntegerReference}, {"fill", StringReference},
      {"color", StringReference} } },
    { "Implode", { {"factor", DoubleReference} } },
    { "Magnify", },
    { "MedianFilter", { {"radius", DoubleReference} } },
    { "Minify", },
    { "OilPaint", { {"radius", DoubleReference} } },
    { "ReduceNoise", { {"radius", DoubleReference} } },
    { "Roll", { {"geom", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference} } },
    { "Rotate", { {"degree", DoubleReference} } },
    { "Sample", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference} } },
    { "Scale", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference} } },
    { "Shade", { {"geom", StringReference}, {"azimuth", DoubleReference},
      {"elevat", DoubleReference}, {"color", BooleanTypes} } },
    { "Sharpen", { {"geom", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Shear", { {"geom", StringReference}, {"x", DoubleReference},
      {"y", DoubleReference} } },
    { "Spread", { {"amount", IntegerReference} } },
    { "Swirl", { {"degree", DoubleReference} } },
    { "Resize", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"filter", FilterTypess},
      {"blur", DoubleReference } } },
    { "Zoom", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"filter", FilterTypess},
      {"blur", DoubleReference } } },
    { "Annotate", { {"text", StringReference}, {"font", StringReference},
      {"point", DoubleReference}, {"density", StringReference},
      {"box", StringReference}, {"stroke", StringReference},
      {"fill", StringReference}, {"geom", StringReference},
      {"sans", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"grav", GravityTypes},
      {"translate", StringReference}, {"scale", StringReference},
      {"rotate", DoubleReference}, {"skewX", DoubleReference},
      {"skewY", DoubleReference}, {"stroke_width", IntegerReference},
      {"antialias", BooleanTypes} } },
    { "ColorFloodfill", { {"geom", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"fill", StringReference},
      {"bordercolor", StringReference} } },
    { "Composite", { {"compos", CompositeTypes}, {"image", ImageReference},
      {"geom", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"grav", GravityTypes},
      {"opacity", StringReference}, {"tile", BooleanTypes} } },
    { "Contrast", { {"sharp", BooleanTypes} } },
    { "CycleColormap", { {"amount", IntegerReference} } },
    { "Draw", { {"prim", PrimitiveTypes}, {"points", StringReference},
      {"meth", MethodTypes}, {"stroke", StringReference},
      {"fill", StringReference}, {"stroke_width", DoubleReference},
      {"sans", StringReference}, {"borderc", StringReference},
      {"x", DoubleReference}, {"y", DoubleReference},
      {"translate", StringReference}, {"scale", StringReference},
      {"rotate", DoubleReference}, {"skewX", DoubleReference},
      {"skewY", DoubleReference}, {"tile", ImageReference},
      {"pen", StringReference}, {"antialias", BooleanTypes} } },
    { "Equalize", },
    { "Gamma", { {"gamma", StringReference}, {"red", DoubleReference},
      {"green", DoubleReference}, {"blue", DoubleReference} } },
    { "Map", { {"image", ImageReference}, {"dither", BooleanTypes} } },
    { "MatteFloodfill", { {"geom", StringReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"matte", IntegerReference},
      {"bordercolor", StringReference} } },
    { "Modulate", { {"factor", StringReference}, {"bright", DoubleReference},
      {"satur", DoubleReference}, {"hue", DoubleReference} } },
    { "Negate", { {"gray", BooleanTypes} } },
    { "Normalize", },
    { "NumberColors", },
    { "Opaque", { {"color", StringReference}, {"fill", StringReference},
      {"pen", StringReference} } },
    { "Quantize", { {"colors", IntegerReference}, {"tree", IntegerReference},
      {"colorsp", ColorspaceTypes}, {"dither", BooleanTypes},
      {"measure", BooleanTypes}, {"global", BooleanTypes} } },
    { "Raise", { {"geom", StringReference}, {"width", IntegerReference},
      {"height", IntegerReference}, {"x", IntegerReference},
      {"y", IntegerReference}, {"raise", BooleanTypes} } },
    { "Segment", { {"colorsp", ColorspaceTypes}, {"verbose", BooleanTypes},
      {"clust", DoubleReference}, {"smooth", DoubleReference} } },
    { "Signature", },
    { "Solarize", { {"factor", DoubleReference} } },
    { "Sync", },
    { "Texture", { {"texture", ImageReference} } },
    { "Sans", { {"geom", StringReference}, {"crop", StringReference},
      {"filter", FilterTypess} } },
    { "Transparent", { {"color", StringReference} } },
    { "Threshold", { {"threshold", DoubleReference} } },
    { "Charcoal", { {"factor", StringReference} } },
    { "Trim", },
    { "Wave", { {"geom", StringReference}, {"ampli", DoubleReference},
      {"wave", DoubleReference} } },
    { "Layer", { {"layer", LayerTypes} } },
    { "Condense", },
    { "Stereo", { {"image", ImageReference} } },
    { "Stegano", { {"image", ImageReference}, {"offset", IntegerReference} } },
    { "Deconstruct", },
    { "GaussianBlur", { {"geom", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference} } },
    { "Convolve", { {"coefficients", ArrayReference} } },
    { "Profile", { {"filen", StringReference}, {"profile", StringReference} } },
    { "UnsharpMask", { {"geom", StringReference}, {"radius", DoubleReference},
      {"sigma", DoubleReference}, {"sigma", DoubleReference},
      {"threshold", DoubleReference} } },
  };

/*
  Variable declarations.
*/
char
  *client_name = "PerlMagick";

int
  warning_flag = False;  /* if != 0: error messages call Perl warn */

SV
  *error_list;  /* Perl variable for storing messages */

static jmp_buf
  *error_jump;  /* long jump return for FATAL errors */

/*
  Forward declarations.
*/
static int
  strEQcase(const char *,const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e P a c k a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClonePackageInfo makes a duplicate of the given info, or if info is
%  NULL, a new one.
%
%  The format of the ClonePackageInfo routine is:
%
%      cloned_info=ClonePackageInfo(info)
%
%  A description of each parameter follows:
%
%    o cloned_info: Method ClonePackageInfo returns a duplicate of the given
%      info, or if info is NULL, a new one.
%
%    o info: a structure of type info.
%
%
*/
static struct PackageInfo *ClonePackageInfo(struct PackageInfo *info)
{
  struct PackageInfo
    *cloned_info;

  cloned_info=(struct PackageInfo *) AcquireMemory(sizeof(struct PackageInfo));
  if (!info)
    {
      MagickIncarnate(client_name);
      cloned_info->image_info=CloneImageInfo((ImageInfo *) NULL);
      cloned_info->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
      return(cloned_info);
    }
  *cloned_info=(*info);
  cloned_info->image_info=CloneImageInfo(info->image_info);
  cloned_info->quantize_info=CloneQuantizeInfo(info->quantize_info);
  return(cloned_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   c o n s t a n t                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method constant returns a double value for the specified name.
%
%  The format of the constant routine is:
%
%      value=constant(name,sans)
%
%  A description of each parameter follows:
%
%    o value: Method constant returns a double value for the specified name.
%
%    o name: The name of the constant.
%
%    o sans: This integer value is not used.
%
%
*/
static double constant(char *name,int sans)
{
  errno=0;
  switch (*name)
  {
    case 'B':
    {
      if (strEQ(name,"BlobError"))
        return(BlobError);
      if (strEQ(name,"BlobWarning"))
        return(BlobWarning);
      break;
    }
    case 'C':
    {
      if (strEQ(name,"CacheError"))
        return(CacheError);
      if (strEQ(name,"CacheWarning"))
        return(CacheWarning);
      if (strEQ(name,"CorruptImageError"))
        return(CorruptImageError);
      if (strEQ(name,"CorruptImageWarning"))
        return(CorruptImageWarning);
      break;
    }
    case 'D':
    {
      if (strEQ(name,"DelegateError"))
        return(DelegateError);
      if (strEQ(name,"DelegateWarning"))
        return(DelegateWarning);
      break;
    }
    case 'F':
    {
      if (strEQ(name,"FatalException"))
        return(FatalException);
      if (strEQ(name,"FileOpenError"))
        return(FileOpenError);
      if (strEQ(name,"FileOpenWarning"))
        return(FileOpenWarning);
      break;
    }
    case 'M':
    {
      if (strEQ(name,"MissingDelegateError"))
        return(MissingDelegateError);
      if (strEQ(name,"MissingDelegateWarning"))
        return(MissingDelegateWarning);
      if (strEQ(name,"MissingPluginWarning"))
        return(MissingDelegateWarning);
      break;
    }
    case 'O':
    {
      if (strEQ(name,"Opaque"))
        return(OpaqueOpacity);
      if (strEQ(name,"OptionError"))
        return(OptionError);
      if (strEQ(name,"OptionWarning"))
        return(OptionWarning);
      break;
    }
    case 'P':
    {
      if (strEQ(name,"PluginWarning"))
        return(DelegateWarning);
      break;
    }
    case 'R':
    {
      if (strEQ(name,"ResourceLimitError"))
        return(ResourceLimitError);
      if (strEQ(name,"ResourceLimitWarning"))
        return(ResourceLimitWarning);
      break;
    }
    case 'S':
    {
      if (strEQ(name,"StreamError"))
        return(StreamError);
      if (strEQ(name,"StreamWarning"))
        return(StreamWarning);
      if (strEQ(name,"Success"))
        return(0);
      break;
    }
    case 'T':
    {
      if (strEQ(name,"Transparent"))
        return(TransparentOpacity);
      break;
    }
    case 'X':
    {
      if (strEQ(name,"XServerError"))
        return(XServerError);
      if (strEQ(name,"XServerWarning"))
        return(XServerWarning);
      break;
    }
  }
  errno=EINVAL;
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y P a c k a g e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPackageInfo frees a previously created info structure.
%
%  The format of the DestroyPackageInfo routine is:
%
%      DestroyPackageInfo(info)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static void DestroyPackageInfo(struct PackageInfo *info)
{
  DestroyImageInfo(info->image_info);
  DestroyQuantizeInfo(info->quantize_info);
  LiberateMemory((void **) &info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   e r r o r h a n d l e r                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method errorhandler replaces ImageMagick's fatal error handler.  This
%  stores the message in a Perl variable,and longjmp's to return the error to
%  Perl.  If the error_flag variable is set, it also calls the Perl warn
%  routine.  Note that this doesn't exit but returns control to Perl; the
%  Image::Magick handle may be left in a bad state.
%
%  The format of the errorhandler routine is:
%
%      errorhandler(error,message,qualifier)
%
%  A description of each parameter follows:
%
%    o error: Specifies the numeric error category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
static void errorhandler(const ExceptionType error,const char *message,
  const char *qualifier)
{
  char
    text[MaxTextExtent];

  int
    error_number;

  error_number=errno;
  errno=0;
  FormatString(text,"Exception %d: %.1024s%s%.1024s%s%s%.64s%s",error,
    (message ? message : "ERROR"),
    qualifier ? " (" : "",qualifier ? qualifier : "",qualifier ? ")" : "",
    error_number ? " [" : "",error_number ? strerror(error_number) : "",
    error_number? "]" : "");
  if ((error_list == NULL) || (error_jump == NULL) || warning_flag)
    {
      /*
        Set up message buffer.
      */
      warn("%s",text);
      if (error_jump == NULL)
        exit((int) error % 100);
    }
  if (error_list)
    {
      if (SvCUR(error_list))
        sv_catpv(error_list,"\n");
      sv_catpv(error_list,text);
    }
  longjmp(*error_jump,(int) error);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t L i s t                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetList is recursively called by SetupList to traverse the
%  Image__Magick reference.  If building an reference_vector (see SetupList),
%  *current is the current position in *reference_vector and *last is the final
%  entry in *reference_vector.
%
%  The format of the GetList routine is:
%
%      GetList(info)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static Image *GetList(SV *reference,SV ***reference_vector,int *current,
  int *last)
{
  Image
    *image;

  if (!reference)
    return(NULL);
  switch (SvTYPE(reference))
  {
    case SVt_PVAV:
    {
      AV
        *av;

      Image
        *head,
        *previous;

      int
        n;

      register int
        i;

      /*
        Array of images.
      */
      previous=(Image *) NULL;
      head=(Image *) NULL;
      av=(AV *) reference;
      n=av_len(av);
      for (i=0; i <= n; i++)
      {
        SV
          **rv;

        rv=av_fetch(av,i,0);
        if (rv && *rv && sv_isobject(*rv))
          {
            image=GetList(SvRV(*rv),reference_vector,current,last);
            if (!image)
              continue;
            if (image == previous)
              {
                ExceptionInfo
                  exception;

                GetExceptionInfo(&exception);
                image=CloneImage(image,0,0,False,&exception);
                if (image == (Image *) NULL)
                  {
                    MagickWarning(exception.severity,exception.reason,
                      exception.description);
                    return(NULL);
                  }
              }
            image->previous=previous;
            *(previous ? &previous->next : &head)=image;
            for (previous=image; previous->next; previous=previous->next);
          }
      }
      return(head);
    }
    case SVt_PVMG:
    {
      /*
        Blessed scalar, one image.
      */
      image=(Image *) SvIV(reference);
      if (!image)
        return(NULL);
      image->previous=(Image *) NULL;
      image->next=(Image *) NULL;
      if (reference_vector)
        {
          if (*current == *last)
            {
              *last+=256;
              if (*reference_vector)
                ReacquireMemory((void **) & (*reference_vector),
                  *last*sizeof(*reference_vector));
              else
                *reference_vector=(SV **)
                  AcquireMemory(*last*sizeof(*reference_vector));
            }
        (*reference_vector)[*current]=reference;
        (*reference_vector)[++(*current)]=NULL;
      }
      return(image);
    }
  }
  (void) fprintf(stderr,"GetList: Invalid reference type %ld\n",
    SvTYPE(reference));
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P a c k a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPackageInfo looks up or creates an info structure for the given
%  Image__Magick reference.  If it does create a new one, the information in
%  oldinfo is used to initialize it.
%
%  The format of the GetPackageInfo routine is:
%
%      GetPackageInfo(info)
%
%  A description of each parameter follows:
%
%    o info: a structure of type info.
%
%
*/
static struct PackageInfo *GetPackageInfo(void *reference,
  struct PackageInfo *oldinfo)
{
  char
    message[MaxTextExtent];

  struct PackageInfo
    *info;

  SV
    *sv;

  FormatString(message,"%s::A_%lx_Z",PackageName,(long) reference);
  sv=perl_get_sv(message,(TRUE | 0x02));
  if (!sv)
    {
      MagickWarning(ResourceLimitWarning,"Unable to create info variable",
        message);
      return(oldinfo);
    }
  if (SvREFCNT(sv) == 0)
    (void) SvREFCNT_inc(sv);
  if (SvIOKp(sv) && (info=(struct PackageInfo *) SvIV(sv)))
    return(info);
  info=ClonePackageInfo(oldinfo);
  sv_setiv(sv,(IV) info);
  return(info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o o k u p S t r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LookupStr searches through a list of strings matching it to string
%  and return its index in the list, or -1 for not found .
%
%  The format of the LookupStr routine is:
%
%      status=LookupStr(list,string)
%
%  A description of each parameter follows:
%
%    o status: Method LookupStr returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static int LookupStr(char **list,const char *string)
{
  int
    longest,
    offset;

  register char
    **p;

  offset=(-1);
  longest=0;
  for (p=list; *p; p++)
    if (strEQcase(string,*p) > longest)
      {
        offset=p-list;
        longest=strEQcase(string,*p);
      }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t A t t r i b u t e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetAttribute sets the attribute to the value in sval.  This can
%  change either or both of image or info.
%
%  The format of the SetAttribute routine is:
%
%      status=SetAttribute(list,string)
%
%  A description of each parameter follows:
%
%    o status: Method SetAttribute returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static void SetAttribute(struct PackageInfo *info,Image *image,char *attribute,
  SV *sval)
{
  int
    blue,
    green,
    opacity,
    red,
    sp;

  PixelPacket
    *color,
    target_color;

  register int
    i;

  switch (*attribute)
  {
    case 'A':
    case 'a':
    {
      if (strEQcase(attribute,"adjoin"))
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid adjoin type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->adjoin=sp;
          return;
        }
      if (strEQcase(attribute,"antialias"))
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid antialias type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->antialias=sp;
          return;
        }
      break;
    }
    case 'B':
    case 'b':
    {
      if (strEQcase(attribute,"background"))
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->background_color=target_color;
          for ( ; image; image=image->next)
            image->background_color=target_color;
          return;
        }
      if (strEQcase(attribute,"blue-") || strEQcase(attribute,"blue_"))
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf,%lf",
              &image->chromaticity.blue_primary.x,
              &image->chromaticity.blue_primary.y);
          return;
        }
      if (strEQcase(attribute,"bordercolor"))
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->border_color=target_color;
          for ( ; image; image=image->next)
            image->border_color=target_color;
          return;
        }
      break;
    }
    case 'C':
    case 'c':
    {
      if (strEQcase(attribute,"cache"))
        {
          SetCacheThreshold(SvIV(sval));
          return;
        }
      if (strEQcase(attribute,"colormap"))
        {
          for ( ; image; image=image->next)
          {
            int
              i;

            if (image->storage_class == DirectClass)
              continue;
            i=0;
            (void) sscanf(attribute,"%*[^[][%d",&i);
            if (i > image->colors)
              i%=image->colors;
            if (strchr(SvPV(sval,na),',') == 0)
              QueryColorDatabase(SvPV(sval,na),image->colormap+i);
            else
              {
                color=image->colormap+i;
                red=color->red;
                green=color->green;
                blue=color->blue;
                (void) sscanf(SvPV(sval,na),"%d,%d,%d",&red,&green,&blue);
                color->red=((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red);
                color->green=
                  ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green);
                color->blue=((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue);
              }
          }
          return;
        }
      if (strEQcase(attribute,"colorsp"))
        {
          sp=SvPOK(sval) ? LookupStr(ColorspaceTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid colorspace type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->colorspace=(ColorspaceType) sp;
          for ( ; image; image=image->next)
            RGBTransformImage(image,(ColorspaceType) sp);
          return;
        }
      if (strEQcase(attribute,"colors"))
        return;
      if (strEQcase(attribute,"compres"))
        {
          sp=SvPOK(sval) ? LookupStr(CompressionTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid compression type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->compression=(CompressionType) sp;
          for ( ; image; image=image->next)
            image->compression=(CompressionType) sp;
          return;
        }
      break;
    }
    case 'D':
    case 'd':
    {
      if (strEQcase(attribute,"delay"))
        {
          for ( ; image; image=image->next)
            image->delay=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"density"))
        {
          if (!IsGeometry(SvPV(sval,na)))
            {
              MagickWarning(OptionWarning,"Invalid geometry on density",
                SvPV(sval,na));
              return;
            }
          if (info)
            (void) CloneString(&info->image_info->density,SvPV(sval,na));
          for ( ; image; image=image->next)
          {
            int
              count;

            count=sscanf(info->image_info->density,"%lfx%lf",
              &image->x_resolution,&image->y_resolution);
            if (count != 2)
              image->y_resolution=image->x_resolution;
          }
          return;
        }
      if (strEQcase(attribute,"depth"))
        {
          if (info)
            info->image_info->depth=SvIV(sval);
          for ( ; image; image=image->next)
            image->depth=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"dispose"))
        {
          for (; image; image=image->next)
            image->dispose=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"dither"))
        {
          if (info)
            {
              sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) :
                SvIV(sval);
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid dither type",
                    SvPV(sval,na));
                  return;
                }
              info->image_info->dither=sp;
            }
          return;
        }
      if (strEQcase(attribute,"display"))
        {
          display:
          if (info)
            (void) CloneString(&info->image_info->server_name,SvPV(sval,na));
          return;
        }
      break;
    }
    case 'E':
    case 'e':
      break;
    case 'F':
    case 'f':
    {
      if (strEQcase(attribute,"filen"))
        {
          if (info)
            (void) strncpy(info->image_info->filename,SvPV(sval,na),
              MaxTextExtent-1);
          for ( ; image; image=image->next)
            (void) strncpy(image->filename,SvPV(sval,na),MaxTextExtent-1);
          return;
        }
      if (strEQcase(attribute,"file"))
        {
          if (info)
            info->image_info->file=IoIFP(sv_2io(sval));
          return;
        }
      if (strEQcase(attribute,"font"))
        {
          if (info)
            (void) CloneString(&info->image_info->font,SvPV(sval,na));
          return;
        }
      if (strEQcase(attribute,"fuzz"))
        {
          if (info)
            info->image_info->fuzz=SvNV(sval);
          for ( ; image; image=image->next)
            image->fuzz=SvNV(sval);
          return;
        }
      break;
    }
    case 'G':
    case 'g':
    {
      if (strEQcase(attribute,"green-") || strEQcase(attribute,"green_"))
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf,%lf",
              &image->chromaticity.green_primary.x,
              &image->chromaticity.green_primary.y);
          return;
        }
      break;
    }
    case 'H':
    case 'h':
      break;
    case 'I':
    case 'i':
    {
      if (strEQcase(attribute,"index"))
        {
          int
            index,
            x,
            y;

          IndexPacket
            *indexes;

          PixelPacket
            *pixel;

          for ( ; image; image=image->next)
          {
            if (image->class != PseudoClass)
              continue;
            x=0;
            y=0;
            (void) sscanf(attribute,"%*[^[][%d,%d",&x,&y);
            pixel=GetImagePixels(image,x % image->columns,y % image->rows,1,1);
            if (pixel == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            (void) sscanf(SvPV(sval,na),"%d",&index);
            if ((index >= 0) && (index < image->colors))
              *indexes=index;
            (void) SyncImagePixels(image);
          }
          return;
        }
      if (strEQcase(attribute,"iterat"))
        {
  iterations:
          for ( ; image; image=image->next)
            image->iterations=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"interla"))
        {
          sp=SvPOK(sval) ? LookupStr(InterlaceTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid interlace value",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->interlace=(InterlaceType) sp;
          for ( ; image; image=image->next)
            image->interlace=(InterlaceType) sp;
          return;
        }
      break;
    }
    case 'J':
    case 'j':
      break;
    case 'K':
    case 'k':
      break;
    case 'L':
    case 'l':
    {
      if (strEQcase(attribute,"loop"))
        goto iterations;
      break;
    }
    case 'M':
    case 'm':
    {
      if (strEQcase(attribute,"magick"))
        {
          if (info)
            {
              FormatString(info->image_info->filename,"%.1024s:",SvPV(sval,na));
              SetImageInfo(info->image_info,True);
              if (*info->image_info->magick == '\0')
                MagickWarning(OptionWarning,"Unrecognized image format",
                  info->image_info->filename);
              else
                for ( ; image; image=image->next)
                  (void) strcpy(image->magick,info->image_info->magick);
            }
          return;
        }
      if (strEQcase(attribute,"mattec") || strEQcase(attribute,"matte-color"))
        {
          (void) QueryColorDatabase(SvPV(sval,na),&target_color);
          if (info)
            info->image_info->matte_color=target_color;
          for ( ; image; image=image->next)
            image->matte_color=target_color;
          return;
        }
      if (strEQcase(attribute,"matte"))
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid matte type",SvPV(sval,na));
              return;
            }
          for ( ; image; image=image->next)
            image->matte=sp;
          return;
        }
      if (strEQcase(attribute,"monoch"))
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid monochrome type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->monochrome=sp;
          return;
        }
      break;
    }
    case 'O':
    case 'o':
      break;
    case 'P':
    case 'p':
    {
      if (strEQcase(attribute,"page"))
        {
          char
            *p;

          p=PostscriptGeometry(SvPV(sval,na));
          if (!p)
            return;
          if (info)
            (void) CloneString(&info->image_info->page,p);
          for ( ; image; image=image->next)
            ParseImageGeometry(PostscriptGeometry(p),
              &image->page.x,&image->page.y,
              &image->page.width,&image->page.height);
          DestroyPostscriptGeometry(p);
          return;
        }
      if (strEQcase(attribute,"pen"))
        {
          if (info)
            (void) QueryColorDatabase(SvPV(sval,na),&info->image_info->pen);
          return;
        }
      if (strEQcase(attribute,"pixel"))
        {
          int
            x,
            y;

          PixelPacket
            *pixel;

          for ( ; image; image=image->next)
          {
            x=0;
            y=0;
            (void) sscanf(attribute,"%*[^[][%d,%d",&x,&y);
            pixel=GetImagePixels(image,x % image->columns,y % image->rows,1,1);
            if (pixel == (PixelPacket *) NULL)
              break;
            image->storage_class=DirectClass;
            if (strchr(SvPV(sval,na),',') == 0)
              QueryColorDatabase(SvPV(sval,na),pixel);
            else
              {
                red=pixel->red;
                green=pixel->green;
                blue=pixel->blue;
                opacity=pixel->opacity;
                (void) sscanf(SvPV(sval,na),"%d,%d,%d,%d",&red,&green,&blue,
                  &opacity);
                pixel->red=((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red);
                pixel->green=
                  ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green);
                pixel->blue=((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue);
                pixel->opacity=
                  ((opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity);
              }
            (void) SyncImagePixels(image);
          }
          return;
        }
      if (strEQcase(attribute,"points"))
        {
          if (info)
            (void) sscanf(SvPV(sval,na),"%lf",&info->image_info->pointsize);
          return;
        }
      if (strEQcase(attribute,"preview"))
        {
          sp=SvPOK(sval) ? LookupStr(PreviewTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid preview type",SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->preview_type=(PreviewType) sp;
          return;
        }
      break;
    }
    case 'Q':
    case 'q':
    {
      if (strEQcase(attribute,"qualit"))
        {
          if (info && (info->image_info->quality=SvIV(sval)) <= 0)
            info->image_info->quality=75;
          return;
        }
      break;
    }
    case 'R':
    case 'r':
    {
      if (strEQcase(attribute,"red-") || strEQcase(attribute,"red_"))
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf,%lf",
              &image->chromaticity.red_primary.x,
              &image->chromaticity.red_primary.y);
          return;
        }
      if (strEQcase(attribute,"render"))
        {
          sp=SvPOK(sval) ? LookupStr(IntentTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid rendering intent",
                SvPV(sval,na));
              return;
            }
         for ( ; image; image=image->next)
           image->rendering_intent=(RenderingIntent) sp;
         return;
       }
      break;
    }
    case 'S':
    case 's':
    {
      if (strEQcase(attribute,"scene"))
        {
          for ( ; image; image=image->next)
            image->scene=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"subim"))
        {
          if (info)
            info->image_info->subimage=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"subra"))
        {
          if (info)
            info->image_info->subrange=SvIV(sval);
          return;
        }
      if (strEQcase(attribute,"server"))
        goto display;
      if (strEQcase(attribute,"size"))
        {
          if (info)
            {
              if (!IsGeometry(SvPV(sval,na)))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on size",
                    SvPV(sval,na));
                  return;
                }
              (void) CloneString(&info->image_info->size,SvPV(sval,na));
            }
          return;
        }
      break;
    }
    case 'T':
    case 't':
    {
      if (strEQcase(attribute,"tile"))
        {
          if (info)
            (void) CloneString(&info->image_info->tile,SvPV(sval,na));
          return;
        }
      if (strEQcase(attribute,"texture"))
        {
          if (info)
            (void) CloneString(&info->image_info->texture,SvPV(sval,na));
          return;
        }
      break;
    }
    case 'U':
    case 'u':
    {
      if (strEQcase(attribute,"unit"))
        {
          sp=SvPOK(sval) ? LookupStr(ResolutionTypes,SvPV(sval,na)) :
            SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid resolution unit",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->units=(ResolutionType) sp;
          for ( ; image; image=image->next)
            image->units=(ResolutionType) sp;
          return;
        }
      break;
    }
    case 'V':
    case 'v':
    {
      if (strEQcase(attribute,"verbose"))
        {
          sp=SvPOK(sval) ? LookupStr(BooleanTypes,SvPV(sval,na)) : SvIV(sval);
          if (sp < 0)
            {
              MagickWarning(OptionWarning,"Invalid verbose type",
                SvPV(sval,na));
              return;
            }
          if (info)
            info->image_info->verbose=sp;
          return;
        }
      if (strEQcase(attribute,"view"))
        {
          if (info)
            (void) CloneString(&info->image_info->view,SvPV(sval,na));
          return;
        }
      break;
    }
    case 'W':
    case 'w':
    {
      if (strEQcase(attribute,"white-") || strEQcase(attribute,"white_"))
        {
          for ( ; image; image=image->next)
            (void) sscanf(SvPV(sval,na),"%lf,%lf",
              &image->chromaticity.white_point.x,
              &image->chromaticity.white_point.y);
          return;
        }
      break;
    }
    case 'X':
    case 'x':
      break;
    case 'Y':
    case 'y':
      break;
    case 'Z':
    case 'z':
      break;
  }
  MagickWarning(OptionWarning,"Invalid attribute",attribute);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t u p L i s t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetupList returns the list of all the images linked by their
%  image->next and image->previous link lists for use with ImageMagick.  If
%  info is non-NULL, an info structure is returned in *info.  If
%  reference_vector is non-NULL,an array of SV* are returned in
%  *reference_vector.  Reference_vector is used when the images are going to be
%  replaced with new Image*'s.
%
%  The format of the SetupList routine is:
%
%      status=SetupList(list,string)
%
%  A description of each parameter follows:
%
%    o status: Method SetAttribute returns the index of string in the list
%      otherwise -1.
%
%    o list: a list of strings.
%
%    o string: a character string.
%
%
*/
static Image *SetupList(SV *reference,struct PackageInfo **info,
  SV ***reference_vector)
{
  Image
    *image;

  int
    current,
    last;

  if (reference_vector)
    *reference_vector=NULL;
  if (info)
    *info=NULL;
  current=0;
  last=0;
  image=GetList(reference,reference_vector,&current,&last);
  if (info && (SvTYPE(reference) == SVt_PVAV))
    *info=GetPackageInfo((void *) reference,(struct PackageInfo *) NULL);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   s t r E Q c a s e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method strEQcase compares two strings and returns 0 if they are the
%  same or if the second string runs out first.  The comparison is case
%  insensitive.
%
%  The format of the strEQcase routine is:
%
%      status=strEQcase(p,q)
%
%  A description of each parameter follows:
%
%    o status: Method strEQcase returns zero if strings p and q are the
%      same or if the second string runs out first.
%
%    o p: a character string.
%
%    o q: a character string.
%
%
*/
static int strEQcase(const char *p,const char *q)
{
  char
    c;

  register int
    i;

  for (i=0 ; (c=(*q)); i++)
  {
    if ((isUPPER(c) ? toLOWER(c) : c) != (isUPPER(*p) ? toLOWER(*p) : *p))
      return(0);
    p++;
    q++;
  }
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   w a r n i n g h a n d l e r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method warninghandler replaces the ImageMagick warning handler.  This stores
%  the (possibly multiple) messages in a Perl variable for later returning.  If
%  the warning_flag variable is set, it also calls the Perl warn routine.
%
%  The format of the warninghandler routine is:
%
%      warninghandler(warning,message,qualifier)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
static void warninghandler(const ExceptionType warning,const char *message,
  const char *qualifier)
{
  char
    text[MaxTextExtent];

  int
    error_number;

  error_number=errno;
  errno=0;
  if (!message)
    return;
  FormatString(text,"Warning %d: %.1024s%s%.1024s%s%s%.64s%s",warning,
    message,qualifier ? " (" : "",qualifier ? qualifier : "",
    qualifier? ")" : "",error_number ? " [" : "",
    error_number ? strerror(error_number) : "",error_number ? "]" : "");
  if ((error_list == NULL) || warning_flag)
    {
      /*
        Set up message buffer.
      */
      warn("%s",text);
      if (error_list == NULL)
        return;
    }
  if (SvCUR(error_list))
    sv_catpv(error_list,"\n");  /* add \n separator between messages */
  sv_catpv(error_list,text);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e : : M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
MODULE = Image::Magick PACKAGE = Image::Magick

PROTOTYPES: ENABLE

BOOT:
  SetWarningHandler(warninghandler);
  SetErrorHandler(errorhandler);

double
constant(name,argument)
  char *name
  int argument

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A n i m a t e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Animate(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    AnimateImage  = 1
    animate       = 2
    animateimage  = 3
  PPCODE:
  {
    jmp_buf
      error_jmp;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    volatile int
      status;

    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    status=0;
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to animate",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"server",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,NULL,SvPV(ST(i-1),na),ST(i));
    AnimateImages(package_info->image_info,image);
    CatchImageException(image);

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A p p e n d                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Append(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    AppendImage  = 1
    append       = 2
    appendimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute,
      *p;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next;

    int
      stack;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    attribute=NULL;
    av=NULL;
    reference_vector=NULL;
    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to append",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get options.
    */
    stack=True;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'S':
        case 's':
        {
          if (strEQcase(attribute,"stack"))
            {
              stack=LookupStr(BooleanTypes,SvPV(ST(i),na));
              if (stack < 0)
                {
                  MagickWarning(OptionWarning,"Invalid stack type",
                    SvPV(ST(i),na));
                  return;
                }
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    next=AppendImages(image,stack,&image->exception);
    if (!next)
      {
        MagickWarning(image->exception.severity,image->exception.reason,
          image->exception.description);
        goto MethodException;
      }
    for ( ; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   A v e r a g e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Average(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    AverageImage   = 1
    average        = 2
    averageimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to average",NULL);
        goto MethodException;
      }
    next=AverageImages(image,&image->exception);
    if (!next)
      {
        MagickWarning(image->exception.severity,image->exception.reason,
          image->exception.description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) next);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    FormatString(info->image_info->filename,"average-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    (void) strcpy(image->filename,info->image_info->filename);
    SetImageInfo(info->image_info,False);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   B l o b T o I m a g e                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
BlobToImage(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    BlobToImage  = 1
    blobtoimage  = 2
    blobto       = 3
  PPCODE:
  {
    AV
      *av;

    char
      **keep,
      **list;


    HV
      *hv;

    Image
      *image;

    int
      ac,
      n;

    jmp_buf
      error_jmp;

    register char
      **p;

    register int
      i;

    struct PackageInfo
      *info;

    STRLEN
      *length;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      number_images;

    number_images=0;
    error_list=newSVpv("",0);
    ac=(items < 2) ? 1 : items-1;
    list=(char **) AcquireMemory((ac+1)*sizeof(*list));
    length=(STRLEN *) AcquireMemory((ac+1)*sizeof(length));
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    if (SvTYPE(reference) != SVt_PVAV)
      {
        MagickWarning(OptionWarning,"Unable to read into a single image",NULL);
        goto ReturnIt;
      }
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    n=1;
    if (items <= 1)
      {
        MagickWarning(OptionWarning,"no blobs to convert",NULL);
        goto ReturnIt;
      }
    for (n=0, i=0; i < ac; i++)
    {
      list[n]=(char *) (SvPV(ST(i+1),length[n]));
      if ((items >= 3) && strEQcase((char *) SvPV(ST(i+1),na),"blob"))
        {
          list[n]=(char *) (SvPV(ST(i+2),length[n]));
          continue;
        }
      n++;
    }
    list[n]=(char *) NULL;
    keep=list;
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    for (i=number_images=0; i < n; i++)
    {
      ExceptionInfo
        exception;

      image=BlobToImage(info->image_info,list[i],length[i],&exception);
      if (image == (Image *) NULL)
        MagickWarning(exception.severity,exception.reason,exception.description);
      for ( ; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
        number_images++;
      }
    }
    /*
      Free resources.
    */
    for (i=0; i < n; i++)
      if (list[i])
        for (p=keep; list[i] != *p++; )
          if (*p == NULL)
            {
              LiberateMemory((void **) &list[i]);
              break;
            }

  ReturnIt:
    LiberateMemory((void **) &list);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   C o a l e s c e                                                           #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Coalesce(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    CoalesceImage   = 1
    coalesce        = 2
    coalesceimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    ExceptionInfo
      exception;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to coalesce",NULL);
        goto MethodException;
      }
    GetExceptionInfo(&exception);
    image=CoalesceImages(image,&exception);
    if (!image)
      {
        MagickWarning(exception.severity,exception.reason,exception.description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) image);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    FormatString(info->image_info->filename,"average-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    (void) strcpy(image->filename,info->image_info->filename);
    SetImageInfo(info->image_info,False);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   C o p y                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Copy(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    CopyImage   = 1
    copy        = 2
    copyimage   = 3
    CloneImage  = 4
    clone       = 5
    cloneimage  = 6
    Clone  = 7
  PPCODE:
  {
    AV
      *av;

    HV
      *hv;

    Image
      *image,
      *next;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!(image=SetupList(reference,&info,(SV ***) NULL)))
      {
        MagickWarning(OptionWarning,"No images to Copy",NULL);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    for (next=image; next; next=next->next)
    {
      image=CloneImage(next,0,0,False,&next->exception);
      if (!image)
        {
          MagickWarning(next->exception.severity,next->exception.reason,
            next->exception.description);
          continue;
        }
      sv=newSViv((IV) image);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    info=GetPackageInfo((void *) av,info);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   D e s t r o y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
DESTROY(ref)
  Image::Magick ref=NO_INIT
  PPCODE:
  {
    SV
      *reference;

    if (!sv_isobject(ST(0)))
      croak("reference is not my type");
    reference=SvRV(ST(0));
    switch (SvTYPE(reference))
    {
      case SVt_PVAV:
      {
        char
          message[MaxTextExtent];

        struct PackageInfo
          *info;

        SV
          *sv;

        /*
          Array (AV *) reference
        */
        FormatString(message,"%s::A_%lx_Z",PackageName,(long) reference);
        sv=perl_get_sv(message,FALSE);
        if (sv)
          {
            if ((SvREFCNT(sv) == 1) && SvIOK(sv) &&
                (info=(struct PackageInfo *) SvIV(sv)))
              {
                DestroyPackageInfo(info);
                sv_setiv(sv,0);
              }
          }
        break;
      }
      case SVt_PVMG:
      {
        Image
          *image;

        /*
          Blessed scalar = (Image *) SvIV(reference)
        */
        image=(Image *) SvIV(reference);
        if (image)
          {
            image->orphan=True;
            DestroyImage(image);
            sv_setiv(reference,0);
          }
        break;
      }
      default:
        break;
    }
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   D i s p l a y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Display(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    DisplayImage  = 1
    display       = 2
    displayimage  = 3
  PPCODE:
  {
    jmp_buf
      error_jmp;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    volatile int
      status;

    status=0;
    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!(image=SetupList(reference,&info,(SV ***) NULL)))
      {
        MagickWarning(OptionWarning,"No images to display",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"server",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,NULL,SvPV(ST(i-1),na),ST(i));
    DisplayImages(package_info->image_info,image);

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) status);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   G e t                                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Get(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    GetAttributes = 1
    GetAttribute  = 2
    get           = 3
    getattributes = 4
    getattribute  = 5
  PPCODE:
  {
    char
      *attribute,
      color[MaxTextExtent];

    Image
      *image;

    int
      j;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,
      *s;

    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        XSRETURN_EMPTY;
      }
    reference=SvRV(ST(0));
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image && !info)
      {
        MagickWarning(OptionWarning,"Nothing to get attributes from",NULL);
        XSRETURN_EMPTY;
      }
    EXTEND(sp,items-1);
    for (i=1; i < items; i++)
    {
      attribute=(char *) SvPV(ST(i),na);
      s=NULL;
      switch (*attribute)
      {
        case 'A':
        case 'a':
        {
          if (strEQcase(attribute,"adjoin"))
            {
              if (info)
                s=newSViv(info->image_info->adjoin);
              break;
            }
          if (strEQcase(attribute,"antialias"))
            {
              if (info)
                s=newSViv(info->image_info->antialias);
              break;
            }
          break;
        }
        case 'B':
        case 'b':
        {
          if (strEQcase(attribute,"background") ||
              strEQcase(attribute,"background-color"))
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->background_color.red,
                image->background_color.green,image->background_color.blue,
                image->background_color.opacity);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"base-column") ||
              strEQcase(attribute,"base_column"))
            {
              if (image)
                s=newSViv(image->magick_columns);
              break;
            }
          if (strEQcase(attribute,"base-filename") ||
              strEQcase(attribute,"base_filename"))
            {
              if (image)
                s=newSVpv(image->magick_filename,0);
              break;
            }
          if (strEQcase(attribute,"base-row") ||
              strEQcase(attribute,"base_row"))
            {
              if (image)
                s=newSViv(image->magick_rows);
              break;
            }
          if (strEQcase(attribute,"blue-") || strEQcase(attribute,"blue_"))
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.blue_primary.x,
                image->chromaticity.blue_primary.y);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"bordercolor") ||
              strEQcase(attribute,"border-color"))
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->border_color.red,
                image->border_color.green,image->border_color.blue,
                image->border_color.opacity);
              s=newSVpv(color,0);
              break;
            }
          break;
        }
        case 'C':
        case 'c':
        {
          if (strEQcase(attribute,"comment"))
            {
              ImageAttribute
                *attribute;

              attribute=GetImageAttribute(image,"Comment");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              break;
            }
          if (strEQcase(attribute,"class"))
            {
              if (!image)
                break;
#if defined(__cplusplus) || defined(c_plusplus)
              j=image->c_class;
#else
              j=image->storage_class;
#endif
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(ClassTypes)-1))
                {
                  (void) sv_setpv(s,ClassTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          if (strEQcase(attribute,"compress"))
            {
              j=info ? info->image_info->compression : image->compression;
              if (info)
                if (info->image_info->compression == UndefinedCompression)
                  j=image->compression;
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(CompressionTypes)-1))
                {
                  (void) sv_setpv(s,CompressionTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          if (strEQcase(attribute,"colorspace"))
            {
              j=image ? image->colorspace : RGBColorspace;
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(ColorspaceTypes)-1))
                {
                  (void) sv_setpv(s,ColorspaceTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          if (strEQcase(attribute,"colors"))
            {
              if (image)
                s=newSViv(GetNumberColors(image,(FILE *) NULL));
              break;
            }
          if (strEQcase(attribute,"colormap"))
            {
              if (!image || !image->colormap)
                break;
              j=0;
              (void) sscanf(attribute,"%*[^[][%d",&j);
              if (j > image->colors)
                j%=image->colors;
              FormatString(color,"%u,%u,%u,%u",image->colormap[j].red,
                image->colormap[j].green,image->colormap[j].blue,
                image->colormap[j].opacity);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"column"))
            {
              if (image)
                s=newSViv(image->columns);
              break;
            }
          break;
        }
        case 'D':
        case 'd':
        {
          if (strEQcase(attribute,"density"))
            {
              if (info && info->image_info->density)
                s=newSVpv(info->image_info->density,0);
              break;
            }
          if (strEQcase(attribute,"dispose"))
            {
              if (image)
                s=newSViv(image->dispose);
              break;
            }
          if (strEQcase(attribute,"delay"))
            {
              if (image)
                s=newSViv(image->delay);
              break;
            }
          if (strEQcase(attribute,"depth"))
            {
              if (info)
                s=newSViv(info->image_info->depth);
              if (image)
                s=newSViv(image->depth);
              break;
            }
          if (strEQcase(attribute,"dither"))
            {
              if (info)
                s=newSViv(info->image_info->dither);
              break;
            }
          if (strEQcase(attribute,"display"))  /* same as server */
            {
              if (info && info->image_info->server_name)
                s=newSVpv(info->image_info->server_name,0);
              break;
            }
          if (strEQcase(attribute,"directory"))
            {
              if (image && image->directory)
                s=newSVpv(image->directory,0);
              break;
            }
          break;
        }
        case 'E':
        case 'e':
        {
          if (strEQcase(attribute,"error"))
            {
              if (image)
                s=newSViv(image->mean_error_per_pixel);
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (strEQcase(attribute,"filesize"))
            {
              if (image)
                s=newSViv(image->filesize);
              break;
            }
          if (strEQcase(attribute,"filen"))
            {
              if (image)
                s=newSVpv(image->filename,0);
              else
                if (info && info->image_info->filename &&
                    *info->image_info->filename)
                  s=newSVpv(info->image_info->filename,0);
              break;
            }
          if (strEQcase(attribute,"filter"))
            {
              j=image->filter;
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(FilterTypess)-1))
                {
                  (void) sv_setpv(s,FilterTypess[j]);
                  SvIOK_on(s);
                }
              break;
            }
          if (strEQcase(attribute,"font"))
            {
              if (info && info->image_info->font)
                s=newSVpv(info->image_info->font,0);
              break;
            }
          if (strEQcase(attribute,"format"))
            {
              MagickInfo
                *magick_info;

              if (info && (*info->image_info->magick != '\0'))
                magick_info=(MagickInfo *)
                  GetMagickInfo(info->image_info->magick);
              else
                if (image)
                  magick_info=(MagickInfo *) GetMagickInfo(image->magick);
                if ((magick_info != (MagickInfo *) NULL) &&
                    (*magick_info->description != '\0'))
                  s=newSVpv((char *) magick_info->description,0);
              break;
            }
          if (strEQcase(attribute,"fuzz"))
            {
              if (info)
                s=newSVnv(info->image_info->fuzz);
              else
                if (image)
                  s=newSVnv(image->fuzz);
              break;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (strEQcase(attribute,"gamma"))
            {
              if (image)
                s=newSVnv(image->gamma);
              break;
            }
          if (strEQcase(attribute,"geom"))
            {
              if (image && image->geometry)
                s=newSVpv(image->geometry,0);
              break;
            }
          if (strEQcase(attribute,"green-") || strEQcase(attribute,"green_"))
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.green_primary.x,
                image->chromaticity.green_primary.y);
              s=newSVpv(color,0);
              break;
            }
          break;
        }
        case 'H':
        case 'h':
        {
          if (strEQcase(attribute,"height"))
            {
              if (image)
                s=newSViv(image->rows);
              break;
            }
          break;
        }
        case 'I':
        case 'i':
        {
          if (strEQcase(attribute,"index"))
            {
              char
                name[MaxTextExtent];

              int
                x,
                y;

              IndexPacket
                *indexes;

              PixelPacket
                pixel;

              if (!image)
                break;
              if (image->class != PseudoClass)
                break;
              x=0;
              y=0;
              (void) sscanf(attribute,"%*[^[][%d,%d",&x,&y);
              pixel=GetOnePixel(image,x % image->columns,y % image->rows);
              indexes=GetIndexes(image);
              FormatString(name,"%u",*indexes);
              s=newSVpv(name,0);
              break;
            }
          if (strEQcase(attribute,"iterat"))  /* same as loop */
            {
              if (image)
                s=newSViv(image->iterations);
              break;
            }
          if (strEQcase(attribute,"interlace"))
            {
              j=info ? info->image_info->interlace : image->interlace;
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(InterlaceTypes)-1))
                {
                  (void) sv_setpv(s,InterlaceTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          break;
        }
        case 'J':
        case 'j':
          break;
        case 'K':
        case 'k':
          break;
        case 'L':
        case 'l':
        {
          if (strEQcase(attribute,"label"))
            {
              ImageAttribute
                *attribute;

              if (!image)
                break;
              attribute=GetImageAttribute(image,"Label");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              break;
            }
          if (strEQcase(attribute,"loop"))  /* same as iterations */
            {
              if (image)
                s=newSViv(image->iterations);
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (strEQcase(attribute,"magick"))
            {
              if (info && *info->image_info->magick)
                s=newSVpv(info->image_info->magick,0);
              else
                if (image)
                  s=newSVpv(image->magick,0);
              break;
            }
          if (strEQcase(attribute,"maximum-error"))
            {
              if (image)
                s=newSVnv(image->normalized_maximum_error);
              break;
            }
          if (strEQcase(attribute,"mean-error"))
            {
              if (image)
                s=newSVnv(image->normalized_mean_error);
              break;
            }
          if (strEQcase(attribute,"monoch"))
            {
              j=info ? info->image_info->monochrome : IsMonochromeImage(image);
              s=newSViv(j);
              break;
            }
          if (strEQcase(attribute,"mattecolor") ||
              strEQcase(attribute,"matte-color"))
            {
              if (!image)
                break;
              FormatString(color,"%u,%u,%u,%u",image->matte_color.red,
                image->matte_color.green,image->matte_color.blue,
                image->matte_color.opacity);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"matte"))
            {
              if (image)
                s=newSViv(image->matte);
              break;
            }
          if (strEQcase(attribute,"montage"))
            {
              if (image && image->montage)
                s=newSVpv(image->montage,0);
              break;
            }
          break;
        }
        case 'O':
        case 'o':
          break;
        case 'P':
        case 'p':
        {
          if (strEQcase(attribute,"page"))
            {
              if (info && info->image_info->page)
                s=newSVpv(info->image_info->page,0);
              else
                if (image)
                  {
                    char
                      geometry[MaxTextExtent];

                    FormatString(geometry,"%ux%u%+d%+d",image->page.width,
                      image->page.height,image->page.x,image->page.y);
                    s=newSVpv(geometry,0);
                  }
              break;
            }
          if (strEQcase(attribute,"pipe"))
            {
              if (image)
                s=newSViv(image->pipe);
              break;
            }
          if (strEQcase(attribute,"pixel"))
            {
              char
                name[MaxTextExtent];

              int
                x,
                y;

              PixelPacket
                pixel;

              if (!image)
                break;
              x=0;
              y=0;
              (void) sscanf(attribute,"%*[^[][%d,%d",&x,&y);
              pixel=GetOnePixel(image,x % image->columns,y % image->rows);
              FormatString(name,"%u,%u,%u,%u",pixel.red,pixel.green,pixel.blue,
                pixel.opacity);
              s=newSVpv(name,0);
              break;
            }
          if (strEQcase(attribute,"points"))
            {
              if (info)
                s=newSViv(info->image_info->pointsize);
              break;
            }
          if (strEQcase(attribute,"preview"))
            {
              s=newSViv(info->image_info->preview_type);
              if ((info->image_info->preview_type >= 0) &&
                  (info->image_info->preview_type < NumberOf(PreviewTypes)-1))
                {
                  (void) sv_setpv(s,PreviewTypes[info->image_info->preview_type]);
                  SvIOK_on(s);
                }
              break;
            }
          break;
        }
        case 'Q':
        case 'q':
        {
          if (strEQcase(attribute,"quality"))
            {
              if (info)
                s=newSViv(info->image_info->quality);
              break;
            }
          break;
        }
        case 'R':
        case 'r':
        {
          if (strEQcase(attribute,"render"))
            {
              j=image->rendering_intent;
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(IntentTypes)-1))
                {
                  (void) sv_setpv(s,IntentTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          if (strEQcase(attribute,"red-") || strEQcase(attribute,"red_"))
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.red_primary.x,
                image->chromaticity.red_primary.y);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"row"))
            {
              if (image)
                s=newSViv(image->rows);
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (strEQcase(attribute,"subimage"))
            {
              if (info)
                s=newSViv(info->image_info->subimage);
              break;
            }
          if (strEQcase(attribute,"subrange"))
            {
              if (info)
                s=newSViv(info->image_info->subrange);
              break;
            }
          if (strEQcase(attribute,"server"))  /* same as display */
            {
              if (info && info->image_info->server_name)
                s=newSVpv(info->image_info->server_name,0);
              break;
            }
          if (strEQcase(attribute,"size"))
            {
              if (info && info->image_info->size)
                s=newSVpv(info->image_info->size,0);
              break;
            }
          if (strEQcase(attribute,"scene"))
            {
              if (image)
                s=newSViv(image->scene);
              break;
            }
          if (strEQcase(attribute,"sign"))
            {
              ImageAttribute
                *attribute;

              if (!image)
                break;
              SignatureImage(image);
              attribute=GetImageAttribute(image,"Signature");
              if (attribute != (ImageAttribute *) NULL)
                s=newSVpv(attribute->value,0);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (strEQcase(attribute,"taint"))
            {
              if (image)
                s=newSViv(IsImageTainted(image));
              break;
            }
          if (strEQcase(attribute,"temporary"))
            {
              if (image)
                s=newSViv(image->temporary);
              break;
            }
          if (strEQcase(attribute,"tile"))
            {
              if (info && info->image_info->tile)
                s=newSVpv(info->image_info->tile,0);
              break;
            }
          if (strEQcase(attribute,"texture"))
            {
              if (info && info->image_info->texture)
                s=newSVpv(info->image_info->texture,0);
              break;
            }
          if (strEQcase(attribute,"type"))
            {
              if (!image)
                break;
              j=(int) GetImageType(image);
              s=newSViv(j);
              if ((j >= 0) && (j < NumberOf(ImageTypes)-1))
                {
                  (void) sv_setpv(s,ImageTypes[j]);
                  SvIOK_on(s);
                }
              break;
            }
          break;
        }
        case 'U':
        case 'u':
        {
          if (strEQcase(attribute,"units"))
            {
              j=info ? info->image_info->units : image->units;
              if (info)
                if (info->image_info->units == UndefinedResolution)
                  j=image->units;
              if (j == UndefinedResolution)
                s=newSVpv("undefined units",0);
              else
                if (j == PixelsPerInchResolution)
                  s=newSVpv("pixels / inch",0);
                else
                  s=newSVpv("pixels / centimeter",0);
              break;
            }
          break;
        }
        case 'V':
        case 'v':
        {
          if (strEQcase(attribute,"verbose"))
            {
              if (info)
                s=newSViv(info->image_info->verbose);
              break;
            }
          if (strEQcase(attribute,"view"))
            {
              if (info && info->image_info->view)
                s=newSVpv(info->image_info->view,0);
              break;
            }
          break;
        }
        case 'W':
        case 'w':
        {
          if (strEQcase(attribute,"white-") || strEQcase(attribute,"white_"))
            {
              if (!image)
                break;
              FormatString(color,"%g,%g",image->chromaticity.white_point.x,
                image->chromaticity.white_point.y);
              s=newSVpv(color,0);
              break;
            }
          if (strEQcase(attribute,"width"))
            {
              if (image)
                s=newSViv(image->columns);
              break;
            }
          break;
        }
        case 'X':
        case 'x':
        {
          if (strEQcase(attribute,"x"))
            {
              if (image)
                s=newSVnv(image->x_resolution);
              break;
            }
          break;
        }
        case 'Y':
        case 'y':
        {
          if (strEQcase(attribute,"y"))
            {
              if (image)
                s=newSVnv(image->y_resolution);
              break;
            }
          break;
        }
        case 'Z':
        case 'z':
          break;
      }
      PUSHs(s ? sv_2mortal(s) : &sv_undef);
    }
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   I m a g e T o B l o b                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#  Blobs are not very interesting until ImageMagick implements direct to memory
#  image formats (via memory-mapped I/O).
#
void
ImageToBlob(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    ImageToBlob  = 1
    imagetoblob  = 2
    toblob       = 3
    blob         = 4
  PPCODE:
  {
    char
      filename[MaxTextExtent];

    Image
      *image,
      *next;

    int
      scene;

    register int
      i;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info,
      *package_info;

    size_t
      length;

    SV
      *reference;

    void
      *blob;

    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to blob",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    for (i=2; i < items; i+=2)
      SetAttribute(package_info,NULL,SvPV(ST(i-1),na),ST(i));
    (void) strcpy(filename,package_info->image_info->filename);
    scene=0;
    for (next=image; next; next=next->next)
    {
      (void) strcpy(next->filename,filename);
      next->scene=scene++;
    }
    SetImageInfo(package_info->image_info,True);
    for (next=image; next; next=next->next)
    {
      ExceptionInfo
        exception;

      length=0;
      blob=ImageToBlob(package_info->image_info,next,&length,&exception);
      if (blob == (void *) NULL)
        MagickWarning(exception.severity,exception.reason,
          exception.description);
      if (blob != (char *) NULL)
        {
          PUSHs(sv_2mortal(newSVpv(blob,length)));
          LiberateMemory((void **) &blob);
        }
      if (package_info->image_info->adjoin)
        break;
    }

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    SvREFCNT_dec(error_list);  /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o g r i f y                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Mogrify(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    Comment            =   1
    CommentImage       =   2
    Label              =   3
    LabelImage         =   4
    AddNoise           =   5
    AddNoiseImage      =   6
    Colorize           =   7
    ColorizeImage      =   8
    Border             =   9
    BorderImage        =  10
    Blur               =  11
    BlurImage          =  12
    Chop               =  13
    ChopImage          =  14
    Crop               =  15
    CropImage          =  16
    Despeckle          =  17
    DespeckleImage     =  18
    Edge               =  19
    EdgeImage          =  20
    Emboss             =  21
    EmbossImage        =  22
    Enhance            =  23
    EnhanceImage       =  24
    Flip               =  25
    FlipImage          =  26
    Flop               =  27
    FlopImage          =  28
    Frame              =  29
    FrameImage         =  30
    Implode            =  31
    ImplodeImage       =  32
    Magnify            =  33
    MagnifyImage       =  34
    MedianFilter       =  35
    MedianFilterImage  =  36
    Minify             =  37
    MinifyImage        =  38
    OilPaint           =  39
    OilPaintImage      =  40
    ReduceNoise        =  41
    ReduceNoiseImage   =  42
    Roll               =  43
    RollImage          =  44
    Rotate             =  45
    RotateImage        =  46
    Sample             =  47
    SampleImage        =  48
    Scale              =  49
    ScaleImage         =  50
    Shade              =  51
    ShadeImage         =  52
    Sharpen            =  53
    SharpenImage       =  54
    Shear              =  55
    ShearImage         =  56
    Spread             =  57
    SpreadImage        =  58
    Swirl              =  59
    SwirlImage         =  60
    Resize             =  61
    ResizeImage        =  62
    Zoom               =  63
    ZoomImage          =  64
    Annotate           =  65
    AnnotateImage      =  66
    ColorFloodfill     =  67
    ColorFloodfillImage=  68
    Composite          =  69
    CompositeImage     =  70
    Contrast           =  71
    ContrastImage      =  72
    CycleColormap      =  73
    CycleColormapImage =  74
    Draw               =  75
    DrawImage          =  76
    Equalize           =  77
    EqualizeImage      =  78
    Gamma              =  79
    GammaImage         =  80
    Map                =  81
    MapImage           =  82
    MatteFloodfill     =  83
    MatteFloodfillImage=  84
    Modulate           =  85
    ModulateImage      =  86
    Negate             =  87
    NegateImage        =  88
    Normalize          =  89
    NormalizeImage     =  90
    NumberColors       =  91
    NumberColorsImage  =  92
    Opaque             =  93
    OpaqueImage        =  94
    Quantize           =  95
    QuantizeImage      =  96
    Raise              =  97
    RaiseImage         =  98
    Segment            =  99
    SegmentImage       = 100
    Signature          = 101
    SignatureImage     = 102
    Solarize           = 103
    SolarizeImage      = 104
    Sync               = 105
    SyncImage          = 106
    Texture            = 107
    TextureImage       = 108
    Sans               = 109
    SansImage          = 110
    Transparent        = 111
    TransparentImage   = 112
    Threshold          = 113
    ThresholdImage     = 114
    Charcoal           = 115
    CharcoalImage      = 116
    Trim               = 117
    TrimImage          = 118
    Wave               = 119
    WaveImage          = 120
    Layer              = 121
    LayerImage         = 122
    Stereo             = 125
    StereoImage        = 126
    Stegano            = 127
    SteganoImage       = 128
    Deconstruct        = 129
    DeconstructImage   = 130
    GaussianBlur       = 131
    GaussianBlurImage  = 132
    Convolve           = 133
    ConvolveImage      = 134
    Profile            = 135
    ProfileImage       = 136
    UnsharpMask        = 137
    UnsharpMaskImage   = 138
    MogrifyRegion      = 666
  PPCODE:
  {
    AffineMatrix
      affine,
      current;

    char
      *attribute,
      attribute_flag[MaxArguments],
      *commands[10],
      message[MaxTextExtent],
      *value;

    double
      angle;

    ExceptionInfo
      exception;

    FrameInfo
      frame_info;

    jmp_buf
      error_jmp;

    Image
      *image,
      *next,
      *region_image;

    int
      base,
      first,
      flags;

    PixelPacket
      fill_color;

    RectangleInfo
      rectangle_info,
      region_info;

    register int
      i,
      j,
      k;

    struct PackageInfo
      *info;

    struct Methods
      *rp;

    SV
      **pv,
      *reference,
      **reference_vector;

    union ArgumentList
      argument_list[MaxArguments];

    volatile int
      number_images;

    reference_vector=NULL;
    region_image=NULL;
    number_images=0;
    base=2;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    region_info.width=0;
    region_info.height=0;
    region_info.x=0;
    region_info.y=0;
    region_image=(Image *) NULL;
    if (ix && (ix != 666))
      {
        /*
          Called as Method(...)
        */
        ix=(ix+1)/2;
        rp=(&Methods[ix-1]);
        attribute=rp->name;
      }
    else
      {
        /*
          Called as Mogrify("Method",...)
        */
        attribute=(char *) SvPV(ST(1),na);
        if (ix)
          {
            flags=ParseGeometry(attribute,&region_info.x,&region_info.y,
              &region_info.width,&region_info.height);
            if (!(flags & HeightValue))
              region_info.height=region_info.width;
            attribute=(char *) SvPV(ST(2),na);
            base++;
          }
        for (rp=Methods; ; rp++)
        {
          if (rp >= EndOf(Methods))
            {
              MagickWarning(OptionWarning,"Invalid magick method",attribute);
              goto ReturnIt;
            }
          if (strEQcase(attribute,rp->name))
            break;
        }
        ix=rp-Methods+1;
        base++;
      }
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"no images to mogrify",attribute);
        goto ReturnIt;
      }
    Zero(&argument_list,NumberOf(argument_list),union ArgumentList);
    Zero(&attribute_flag,NumberOf(attribute_flag),char);
    for (i=base; (i < items) || (i == items) && (base == items); i+=2)
    {
      int
        longest;

      struct arguments
        *pp,
        *qq;

      union ArgumentList
        *al;

      SV
        *sv;

      longest=0;
      pp=rp->arguments,
      qq=rp->arguments;
      if (i == items)
        sv=ST(i-1);
      else
        for (sv=ST(i), attribute=(char *) SvPV(ST(i-1),na); ; qq++)
        {
          if ((qq >= EndOf(rp->arguments)) || (qq->method == NULL))
            {
              if (longest > 0)
                break;
              goto continue_outer_loop;
            }
          if (strEQcase(attribute,qq->method) > longest)
            {
              pp=qq;
              longest=strEQcase(attribute,qq->method);
            }
        }
      al=(&argument_list[pp-rp->arguments]);
      if (pp->type == IntegerReference)
        al->int_reference=SvIV(sv);
      else
        if (pp->type == StringReference)
          al->string_reference=(char *) SvPV(sv,na);
        else
          if (pp->type == DoubleReference)
            al->double_reference=SvNV(sv);
          else
            if (pp->type == ImageReference)
              {
                if (!sv_isobject(sv) ||
                    !(al->image_reference=SetupList(SvRV(sv),
                     (struct PackageInfo **) NULL,(SV ***) NULL)))
                  {
                    MagickWarning(OptionWarning,"Reference is not my type",
                      PackageName);
                    goto ReturnIt;
                  }
              }
            else
              if (pp->type == ArrayReference)
                al->array_reference=SvRV(sv);
              else
                if (!SvPOK(sv))  /* not a string; just get number */
                  al->int_reference=SvIV(sv);
                else
                  {
                    /*
                      Is a string; look up name.
                    */
                    al->int_reference=LookupStr(pp->type,SvPV(sv,na));
                    if ((al->int_reference < 0) &&
                        ((al->int_reference=SvIV(sv)) <= 0))
                      {
                        FormatString(message,"invalid %.60s value",pp->method);
                        MagickWarning(OptionWarning,message,attribute);
                        goto continue_outer_loop;
                      }
                  }
      attribute_flag[pp-rp->arguments]++;
      continue_outer_loop: ;
    }
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    (void) memset((char *) &fill_color,0,sizeof(PixelPacket));
    first=True;
    pv=reference_vector;
    for (next=image; next; first=False, next=next->next)
    {
      image=next;
      rectangle_info.width=image->columns;
      rectangle_info.height=image->rows;
      rectangle_info.x=rectangle_info.y=0;
      GetExceptionInfo(&exception);
      if ((region_info.width*region_info.height) != 0)
        {
          region_image=image;
          image=CropImage(image,&region_info,&exception);
          if (!image)
            {
              MagickWarning(exception.severity,exception.reason,
                exception.description);
              continue;
            }
        }
      switch (ix)
      {
        default:
        {
          FormatString(message,"%d",(int) ix);
          MagickWarning(OptionWarning,"Routine value out of range",message);
          goto ReturnIt;
        }
        case 1:  /* Comment */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=(char *) NULL;
          while (SetImageAttribute(image,"Comment",(char *) NULL) != False);
          (void) SetImageAttribute(image,"Comment",
            argument_list[0].string_reference);
          break;
        }
        case 2:  /* Label */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=(char *) NULL;
          while (SetImageAttribute(image,"Label",(char *) NULL) != False);
          (void) SetImageAttribute(image,"Label",
            argument_list[0].string_reference);
          break;
        }
        case 3:  /* AddNoise */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=UniformNoise;
          image=AddNoiseImage(image,(NoiseType) argument_list[0].int_reference,
            &exception);
          break;
        }
        case 4:  /* Colorize */
        {
          PixelPacket
            target;

          target=GetOnePixel(image,0,0);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          if (!attribute_flag[1])
            argument_list[1].string_reference="100";
          image=ColorizeImage(image,argument_list[1].string_reference,target,
            &exception);
          break;
        }
        case 5:  /* Border */
        {
          if (first)
            {
              rectangle_info.width=6;
              rectangle_info.height=6;
              if (attribute_flag[0])
                {
                  flags=ParseGeometry(argument_list[0].string_reference,
                    &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                    &rectangle_info.height);
                  if (!(flags & HeightValue))
                    rectangle_info.height=rectangle_info.width;
                }
              if (attribute_flag[1])
                rectangle_info.width=argument_list[1].int_reference;
              if (attribute_flag[2])
                rectangle_info.height=argument_list[2].int_reference;
              if (attribute_flag[3])
                QueryColorDatabase(argument_list[3].string_reference,
                  &fill_color);
              if (attribute_flag[4])
                QueryColorDatabase(argument_list[4].string_reference,
                  &fill_color);
            }
          if (attribute_flag[3] || attribute_flag[4])
            image->border_color=fill_color;
          image=BorderImage(image,&rectangle_info,&exception);
          break;
        }
        case 6:  /* Blur */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=BlurImage(image,radius,sigma,&exception);
          break;
        }
        case 7:  /* Chop */
        {
          if (attribute_flag[0])
            {
              flags=ParseGeometry(argument_list[0].string_reference,
                &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                &rectangle_info.height);
              if (!(flags & HeightValue))
                rectangle_info.height=rectangle_info.width;
            }
          if (attribute_flag[1])
            rectangle_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            rectangle_info.x=argument_list[3].int_reference;
          if (attribute_flag[4])
            rectangle_info.y=argument_list[4].int_reference;
          image=ChopImage(image,&rectangle_info,&exception);
          break;
        }
        case 59:  /* Trim */
        {
          attribute_flag[0]++;
          argument_list[0].string_reference="0x0";
        }
        case 8:  /* Crop */
        {
          if (attribute_flag[0])
            {
              flags=ParseGeometry(argument_list[0].string_reference,
                &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                &rectangle_info.height);
              if (!(flags & HeightValue))
                rectangle_info.height=rectangle_info.width;
            }
          if (attribute_flag[1])
            rectangle_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.height=argument_list[2].int_reference;
          if (attribute_flag[3])
            rectangle_info.x=argument_list[3].int_reference;
          if (attribute_flag[4])
            rectangle_info.y=argument_list[4].int_reference;
          image=CropImage(image,&rectangle_info,&exception);
          break;
        }
        case 9:  /* Despeckle */
        {
          image=DespeckleImage(image,&exception);
          break;
        }
        case 10:  /* Edge */
        {
          double
            radius;

          radius=0.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lf",&radius);
          image=EdgeImage(image,radius,&exception);
          break;
        }
        case 11:  /* Emboss */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=EmbossImage(image,radius,sigma,&exception);
          break;
        }
        case 12:  /* Enhance */
        {
          image=EnhanceImage(image,&exception);
          break;
        }
        case 13:  /* Flip */
        {
          image=FlipImage(image,&exception);
          break;
        }
        case 14:  /* Flop */
        {
          image=FlopImage(image,&exception);
          break;
        }
        case 15:  /* Frame */
        {
          if (first)
            {
              frame_info.height=frame_info.width=25;
              frame_info.x=frame_info.y=0;
              frame_info.inner_bevel=frame_info.outer_bevel=6;
              if (attribute_flag[0])
                {
                  flags=ParseGeometry(argument_list[0].string_reference,
                    &frame_info.outer_bevel,&frame_info.inner_bevel,
                    &frame_info.width,&frame_info.height);
                  if (!(flags & HeightValue))
                    frame_info.height=frame_info.width;
                  if (!(flags & XValue))
                    frame_info.outer_bevel=(frame_info.width >> 2)+1;
                  if (!(flags & YValue))
                    frame_info.inner_bevel=(frame_info.height >> 2)+1;
                }
              if (attribute_flag[1])
                frame_info.width=argument_list[1].int_reference;
              if (attribute_flag[2])
                frame_info.height=argument_list[2].int_reference;
              if (attribute_flag[3])
                frame_info.inner_bevel=argument_list[3].int_reference;
              if (attribute_flag[4])
                frame_info.outer_bevel=argument_list[4].int_reference;
              frame_info.x=frame_info.width;
              frame_info.y=frame_info.height;
              if (attribute_flag[5])
                QueryColorDatabase(argument_list[5].string_reference,
                  &fill_color);
              if (attribute_flag[6])
                QueryColorDatabase(argument_list[6].string_reference,
                  &fill_color);
            }
          frame_info.width=image->columns+(frame_info.x << 1);
          frame_info.height=image->rows+(frame_info.y << 1);
          if (attribute_flag[5] || attribute_flag[6])
            image->matte_color=fill_color;
          image=FrameImage(image,&frame_info,&exception);
          break;
        }
        case 16:  /* Implode */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=30.0;
          image=ImplodeImage(image,argument_list[0].double_reference,
            &exception);
          break;
        }
        case 17:  /* Magnify */
        {
          image=MagnifyImage(image,&exception);
          break;
        }
        case 18:  /* MedianFilter */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=MedianFilterImage(image,argument_list[0].double_reference,
            &exception);
          break;
        }
        case 19:  /* Minify */
        {
          image=MinifyImage(image,&exception);
          break;
        }
        case 20:  /* OilPaint */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=OilPaintImage(image,argument_list[0].double_reference,
            &exception);
          break;
        }
        case 21:  /* ReduceNoise */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=0.0;
          image=ReduceNoiseImage(image,argument_list[0].double_reference,
            &exception);
          break;
        }
        case 22:  /* Roll */
        {
          if (attribute_flag[1])
            rectangle_info.x=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.y=argument_list[2].int_reference;
          if (attribute_flag[0])
            (void) ParseGeometry(argument_list[0].string_reference,
              &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
              &rectangle_info.height);
          image=RollImage(image,rectangle_info.x,rectangle_info.y,&exception);
          break;
        }
        case 23:  /* Rotate */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=90.0;
          image=RotateImage(image,argument_list[0].double_reference,&exception);
          break;
        }
        case 24:  /* Sample */
        {
          if (attribute_flag[1])
            rectangle_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.height=argument_list[2].int_reference;
          if (attribute_flag[0])
            (void) ParseImageGeometry(argument_list[0].string_reference,
              &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
              &rectangle_info.height);
          image=SampleImage(image,rectangle_info.width,rectangle_info.height,
            &exception);
          break;
        }
        case 25:  /* Scale */
        {
          if (attribute_flag[1])
            rectangle_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.height=argument_list[2].int_reference;
          if (attribute_flag[0])
            (void) ParseImageGeometry(argument_list[0].string_reference,
              &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
              &rectangle_info.height);
          image=ScaleImage(image,rectangle_info.width,rectangle_info.height,
            &exception);
          break;
        }
        case 26:  /* Shade */
        {
          double
            azimuth,
            elevation;

          azimuth=30.0;
          if (attribute_flag[1])
            azimuth=argument_list[1].double_reference;
          elevation=30.0;
          if (attribute_flag[2])
            elevation=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",&azimuth,
              &elevation);
          image=ShadeImage(image,argument_list[3].int_reference,azimuth,
            elevation,&exception);
          break;
        }
        case 27:  /* Sharpen */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=SharpenImage(image,radius,sigma,&exception);
          break;
        }
        case 28:  /* Shear */
        {
          double
            x_shear,
            y_shear;

          x_shear=45.0;
          y_shear=45.0;
          if (attribute_flag[1])
            x_shear=argument_list[1].double_reference;
          if (attribute_flag[2])
            y_shear=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",&x_shear,
              &y_shear);
          image=ShearImage(image,x_shear,y_shear,&exception);
          break;
        }
        case 29:  /* Spread */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=3;
          image=SpreadImage(image,argument_list[0].int_reference,&exception);
          break;
        }
        case 30:  /* Swirl */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=50.0;
          image=SwirlImage(image,argument_list[0].double_reference,&exception);
          break;
        }
        case 31:  /* Resize */
        case 32:  /* Zoom */
        {
          if (attribute_flag[1])
            rectangle_info.width=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.height=argument_list[2].int_reference;
          if (attribute_flag[0])
            (void) ParseImageGeometry(argument_list[0].string_reference,
              &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
              &rectangle_info.height);
          if (!attribute_flag[3])
            argument_list[3].int_reference=(int) LanczosFilter;
          if (!attribute_flag[4])
            argument_list[4].double_reference=1.0;
          image=ResizeImage(image,rectangle_info.width,rectangle_info.height,
            (FilterTypes) argument_list[3].int_reference,
            argument_list[4].double_reference,&exception);
          break;
        }
        case 33:  /* Annotate */
        {
          DrawInfo
            *draw_info;

          draw_info=CloneDrawInfo(info ? info->image_info :
            (ImageInfo *) NULL,(DrawInfo *) NULL);
          if (attribute_flag[1])
            (void) CloneString(&draw_info->font,
              argument_list[1].string_reference);
          if (attribute_flag[2])
            draw_info->pointsize=argument_list[2].double_reference;
          if (attribute_flag[3])
            (void) CloneString(&draw_info->density,
              argument_list[3].string_reference);
          if (attribute_flag[0])
            (void) CloneString(&draw_info->text,
              argument_list[0].string_reference);
          if (attribute_flag[4])
            (void) QueryColorDatabase(argument_list[4].string_reference,
              &draw_info->box);
          if (attribute_flag[5])
            (void) QueryColorDatabase(argument_list[5].string_reference,
              &draw_info->stroke);
          if (attribute_flag[6])
            (void) QueryColorDatabase(argument_list[6].string_reference,
              &draw_info->fill);
          if (attribute_flag[7])
            (void) CloneString(&draw_info->geometry,
              argument_list[7].string_reference);
          if (attribute_flag[9] || attribute_flag[10])
            {
              if (!attribute_flag[9])
                argument_list[9].int_reference=0;
              if (!attribute_flag[10])
                argument_list[10].int_reference=0;
              FormatString(message,"%+d%+d",argument_list[9].int_reference,
                argument_list[10].int_reference);
              (void) CloneString(&draw_info->geometry,message);
            }
          if (attribute_flag[11])
            draw_info->gravity=(GravityType) argument_list[11].int_reference;
          for (j=12; j < 17; j++)
          {
            if (!attribute_flag[j])
              continue;
            value=argument_list[j].string_reference;
            angle=argument_list[j].double_reference;
            current=draw_info->affine;
            IdentityAffine(&affine);
            switch (j)
            {
              case 12:
              {
                /*
                  Translate.
                */
                k=sscanf(value,"%lf%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  k=sscanf(value,"%lf,%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  affine.ty=affine.tx;
                break;
              }
              case 13:
              {
                /*
                  Scale.
                */
                k=sscanf(value,"%lf%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  k=sscanf(value,"%lf,%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  affine.sy=affine.sx;
                break;
              }
              case 14:
              {
                /*
                  Rotate.
                */
                if (angle == 0.0)
                  break;
                affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 15:
              {
                /*
                  SkewX.
                */
                affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 16:
              {
                /*
                  SkewY.
                */
                affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
            }
            draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
            draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
            draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
            draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
            draw_info->affine.tx=current.sx*affine.tx+current.ry*affine.ty+
              current.tx;
            draw_info->affine.ty=current.rx*affine.tx+current.sy*affine.ty+
              current.ty;
          }
          if (attribute_flag[17])
            draw_info->stroke_width=argument_list[17].int_reference;
          if (attribute_flag[18])
            draw_info->text_antialias=argument_list[18].int_reference;
          AnnotateImage(image,draw_info);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 34:  /* ColorFloodfill */
        {
          DrawInfo
            *draw_info;

          PixelPacket
            target;

          draw_info=CloneDrawInfo(info ? info->image_info : (ImageInfo *) NULL,
            (DrawInfo *) NULL);
          if (attribute_flag[0])
            (void) ParseGeometry(argument_list[0].string_reference,
              &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
              &rectangle_info.height);
          if (attribute_flag[1])
            rectangle_info.x=argument_list[1].int_reference;
          if (attribute_flag[2])
            rectangle_info.y=argument_list[2].int_reference;
          if (attribute_flag[3])
            (void) QueryColorDatabase(argument_list[3].string_reference,
              &draw_info->fill);
          if (attribute_flag[4])
            QueryColorDatabase(argument_list[4].string_reference,&fill_color);
          target=GetOnePixel(image,rectangle_info.x % image->columns,
            rectangle_info.y % image->rows);
          if (attribute_flag[4])
            target=fill_color;
          ColorFloodfillImage(image,draw_info,target,rectangle_info.x,
            rectangle_info.y,attribute_flag[4] ? FillToBorderMethod :
            FloodfillMethod);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 35:  /* Composite */
        {
          Image
            *composite_image;
          
          CompositeOperator
            compose;

          int
            tile,
            x,
            y;
        
          compose=OverCompositeOp;
          if (attribute_flag[0])
            compose=(CompositeOperator) argument_list[0].int_reference;
          if (attribute_flag[1])
            composite_image=argument_list[1].image_reference;
          else
            {
              MagickWarning(OptionWarning,"Missing image in composite",NULL);
              goto ReturnIt;
            }
          if (!attribute_flag[3])
            argument_list[3].int_reference=0;
          if (!attribute_flag[4])
            argument_list[4].int_reference=0;
          rectangle_info.x=argument_list[3].int_reference;
          rectangle_info.y=argument_list[4].int_reference;
          if (attribute_flag[2])
            {
              CloneString(&composite_image->geometry,
                argument_list[2].string_reference);
              flags=ParseImageGeometry(argument_list[2].string_reference,
                &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                &rectangle_info.height);
              if (!(flags & HeightValue))
                rectangle_info.height=rectangle_info.width;
            }
          tile=attribute_flag[7] ? argument_list[7].int_reference : False;
          if (attribute_flag[5] && !tile)
            switch (argument_list[5].int_reference)
            {
              case NorthWestGravity:
              {
                rectangle_info.x=0;
                rectangle_info.y=0;
                break;
              }
              case NorthGravity:
              {
                rectangle_info.x=(image->columns-
                  argument_list[1].image_reference->columns) >> 1;
                rectangle_info.y=0;
                break;
              }
              case NorthEastGravity:
              {
                rectangle_info.x=image->columns-
                  argument_list[1].image_reference->columns;
                rectangle_info.y=0;
                break;
              }
              case WestGravity:
              {
                rectangle_info.x=0;
                rectangle_info.y=(image->rows-
                  argument_list[1].image_reference->rows) >> 1;
                break;
              }
              case ForgetGravity:
              case StaticGravity:
              case CenterGravity:
              default:
              {
                rectangle_info.x=(image->columns-
                  argument_list[1].image_reference->columns) >> 1;
                rectangle_info.y=(image->rows-
                  argument_list[1].image_reference->rows) >> 1;
                break;
              }
              case EastGravity:
              {
                rectangle_info.x=image->columns-
                  argument_list[1].image_reference->columns;
                rectangle_info.y=(image->rows-
                  argument_list[1].image_reference->rows) >> 1;
                break;
              }
              case SouthWestGravity:
              {
                rectangle_info.x=0;
                rectangle_info.y=image->rows-
                  argument_list[1].image_reference->rows;
                break;
              }
              case SouthGravity:
              {
                rectangle_info.x=(image->columns-
                  argument_list[1].image_reference->columns) >> 1;
                rectangle_info.y=image->rows-
                  argument_list[1].image_reference->rows;
                break;
              }
              case SouthEastGravity:
              {
                rectangle_info.x=image->columns-
                  argument_list[1].image_reference->columns;
                rectangle_info.y=image->rows-
                  argument_list[1].image_reference->rows;
                break;
              }
            }
          if (!attribute_flag[6])
            argument_list[6].string_reference="0.0";
          if (compose == DissolveCompositeOp)
            {
              register PixelPacket 
                *q;

              double 
                blend;

              blend=atof(argument_list[6].string_reference);
              for (y=0; y < (int) composite_image->rows; y++)
              {
                q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=0; x < (int) composite_image->columns; x++)
                {
                  if (composite_image->matte)
                    q->opacity=((MaxRGB-q->opacity)*blend)/100;
                  else
                    q->opacity=(MaxRGB*blend)/100;
                  q++;
                }
                if (!SyncImagePixels(composite_image))
                  break;
              }
              composite_image->storage_class=DirectClass;
              composite_image->matte=True;
            }
          if (!tile)
            {
              CompositeImage(image,compose,composite_image,
                rectangle_info.x,rectangle_info.y);
              break;
            }
          for (y=0; y < (int) image->rows; y+=(int) composite_image->rows)
            for (x=0; x < (int) image->columns; x+=(int) composite_image->columns)
            {
              int
                status;

              status=CompositeImage(image,compose,composite_image,x,y);
              if (status == False)
                CatchImageException(image);
            }
          break;
        }
        case 36:  /* Contrast */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=0.0;
          ContrastImage(image,argument_list[0].int_reference);
          break;
        }
        case 37:  /* CycleColormap */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=6;
          CycleColormapImage(image,argument_list[0].int_reference);
          break;
        }
        case 38:  /* Draw */
        {
          DrawInfo
            *draw_info;

          draw_info=CloneDrawInfo(info ? info->image_info : (ImageInfo *) NULL,
            (DrawInfo *) NULL);
          (void) CloneString(&draw_info->primitive,"Point");
          if (attribute_flag[0] && (argument_list[0].int_reference > 0))
            (void) CloneString(&draw_info->primitive,
              PrimitiveTypes[argument_list[0].int_reference]);
          if (attribute_flag[1])
            {
              (void) strcat(draw_info->primitive," ");
              if (!IsGeometry(argument_list[1].string_reference))
                (void) strcat(draw_info->primitive,"'");
              (void) strcat(draw_info->primitive,
                argument_list[1].string_reference);
              if (!IsGeometry(argument_list[1].string_reference))
                (void) strcat(draw_info->primitive,"'");
            }
          if (attribute_flag[2])
            {
              (void) strcat(draw_info->primitive," ");
              (void) strcat(draw_info->primitive,
                MethodTypes[argument_list[2].int_reference]);
            }
          if (attribute_flag[3])
            (void) QueryColorDatabase(argument_list[3].string_reference,
              &draw_info->stroke);
          if (attribute_flag[4])
            (void) QueryColorDatabase(argument_list[4].string_reference,
              &draw_info->fill);
          if (attribute_flag[5])
            draw_info->stroke_width=argument_list[5].double_reference;
          if (attribute_flag[7])
            (void) QueryColorDatabase(argument_list[7].string_reference,
              &draw_info->border_color);
          for (j=10; j < 15; j++)
          {
            if (!attribute_flag[j])
              continue;
            value=argument_list[j].string_reference;
            angle=argument_list[j].double_reference;
            current=draw_info->affine;
            IdentityAffine(&affine);
            switch (j)
            {
              case 10:
              {
                /*
                  Translate.
                */
                k=sscanf(value,"%lf%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  k=sscanf(value,"%lf,%lf",&affine.tx,&affine.ty);
                if (k == 1)
                  affine.ty=affine.tx;
                break;
              }
              case 11:
              {
                /*
                  Scale.
                */
                k=sscanf(value,"%lf%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  k=sscanf(value,"%lf,%lf",&affine.sx,&affine.sy);
                if (k == 1)
                  affine.sy=affine.sx;
                break;
              }
              case 12:
              {
                /*
                  Rotate.
                */
                if (angle == 0.0)
                  break;
                affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 13:
              {
                /*
                  SkewX.
                */
                affine.ry=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
              case 14:
              {
                /*
                  SkewY.
                */
                affine.rx=tan(DegreesToRadians(fmod(angle,360.0)));
                break;
              }
            }
            draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
            draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
            draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
            draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
            draw_info->affine.tx=
              current.sx*affine.tx+current.ry*affine.ty+current.tx;
            draw_info->affine.ty=
              current.rx*affine.tx+current.sy*affine.ty+current.ty;
          }
          if (attribute_flag[15])
            draw_info->tile=
              CloneImage(argument_list[15].image_reference,0,0,True,&exception);
          if (attribute_flag[16])
            (void) QueryColorDatabase(argument_list[16].string_reference,
              &draw_info->fill);
          if (attribute_flag[17])
            {
              draw_info->stroke_antialias=argument_list[17].int_reference;
              draw_info->text_antialias=argument_list[17].int_reference;
            }
          DrawImage(image,draw_info);
          DestroyDrawInfo(draw_info);
          break;
        }
        case 39:  /* Equalize */
        {
          EqualizeImage(image);
          break;
        }
        case 40:  /* Gamma */
        {
          if (first)
            {
              if (!attribute_flag[1])
                argument_list[1].double_reference=1.0;
              if (!attribute_flag[2])
                argument_list[2].double_reference=1.0;
              if (!attribute_flag[3])
                argument_list[3].double_reference=1.0;
              if (!attribute_flag[0])
                {
                  FormatString(message,"%lf/%lf/%lf",
                    argument_list[1].double_reference,
                    argument_list[2].double_reference,
                    argument_list[3].double_reference);
                  argument_list[0].string_reference=message;
                }
            }
          GammaImage(image,argument_list[0].string_reference);
          break;
        }
        case 41:  /* Map */
        {
          if (!attribute_flag[1])
            argument_list[1].int_reference=1;
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in map",NULL);
              goto ReturnIt;
            }
          (void) MapImages(image,argument_list[0].image_reference,
            argument_list[1].int_reference);
          break;
        }
        case 42:  /* MatteFloodfill */
        {
          PixelPacket
            target;

          unsigned int
            matte;

          if (first)
            {
              if (attribute_flag[0])
                {
                  flags=ParseGeometry(argument_list[0].string_reference,
                    &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                    &rectangle_info.height);
                }
              if (attribute_flag[1])
                rectangle_info.x=argument_list[1].int_reference;
              if (attribute_flag[2])
                rectangle_info.y=argument_list[2].int_reference;
              if (attribute_flag[4])
                QueryColorDatabase(argument_list[4].string_reference,
                  &fill_color);
            }
          matte=TransparentOpacity;
          if (attribute_flag[3])
            matte=argument_list[3].int_reference;
          if (!image->matte)
            SetImageOpacity(image,OpaqueOpacity);
          target=GetOnePixel(image,rectangle_info.x % image->columns,
            rectangle_info.y % image->rows);
          if (attribute_flag[4])
            target=fill_color;
          MatteFloodfillImage(image,target,matte,rectangle_info.x,
            rectangle_info.y,attribute_flag[4] ? FillToBorderMethod :
            FloodfillMethod);
          break;
        }
        case 43:  /* Modulate */
        {
          if (first)
            {
              if (!attribute_flag[1])
                argument_list[1].double_reference=100.0;
              if (!attribute_flag[2])
                argument_list[2].double_reference=100.0;
              if (!attribute_flag[3])
                argument_list[3].double_reference=100.0;
              FormatString(message,"%lf/%lf/%lf",
                argument_list[1].double_reference,
                argument_list[2].double_reference,
                argument_list[3].double_reference);
              if (!attribute_flag[0])
                argument_list[0].string_reference=message;
            }
          ModulateImage(image,argument_list[0].string_reference);
          break;
        }
        case 44:  /* Negate */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=0;
          NegateImage(image,argument_list[0].int_reference);
          break;
        }
        case 45:  /* Normalize */
        {
          NormalizeImage(image);
          break;
        }
        case 46:  /* NumberColors */
          break;
        case 47:  /* Opaque */
        {
          PixelPacket
            fill_color,
            target;

          target=GetOnePixel(image,0,0);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          fill_color=GetOnePixel(image,0,0);
          if (attribute_flag[1])
            (void) QueryColorDatabase(argument_list[1].string_reference,
              &fill_color);
          if (attribute_flag[2])
            (void) QueryColorDatabase(argument_list[2].string_reference,
              &fill_color);
          OpaqueImage(image,target,fill_color);
          break;
        }
        case 48:  /* Quantize */
        {
          QuantizeInfo
            quantize_info;

          GetQuantizeInfo(&quantize_info);
          quantize_info.number_colors=
            attribute_flag[0] ? argument_list[0].int_reference : (info ?
            info->quantize_info->number_colors : MaxRGB + 1);
          quantize_info.tree_depth=attribute_flag[1] ?
            argument_list[1].int_reference :
            (info ? info->quantize_info->tree_depth : 8);
          quantize_info.colorspace=(ColorspaceType)
            (attribute_flag[2] ? argument_list[2].int_reference :
            (info? info->quantize_info->colorspace : RGBColorspace));
          quantize_info.dither=attribute_flag[3] ?
            argument_list[3].int_reference :
            (info ? info->quantize_info->dither : False);
          quantize_info.measure_error=attribute_flag[4] ?
            argument_list[4].int_reference :
            (info ? info->quantize_info->measure_error : False);
          if (attribute_flag[5] && argument_list[5].int_reference)
            {
              (void) QuantizeImages(&quantize_info,image);
              goto ReturnIt;
            }
          if ((image->storage_class == DirectClass) ||
              (image->colors > quantize_info.number_colors))
            (void) QuantizeImage(&quantize_info,image);
          else
            CompressColormap(image);
          break;
        }
        case 49:  /* Raise */
        {
          if (first)
            {
              rectangle_info.height=rectangle_info.width=6;
              if (attribute_flag[0])
                {
                  flags=ParseGeometry(argument_list[0].string_reference,
                    &rectangle_info.x,&rectangle_info.y,&rectangle_info.width,
                    &rectangle_info.height);
                  if (!(flags & HeightValue))
                    rectangle_info.height=rectangle_info.width;
                }
              if (attribute_flag[1])
                rectangle_info.width=argument_list[1].int_reference;
              if (attribute_flag[2])
                rectangle_info.height=argument_list[2].int_reference;
              if (attribute_flag[3])
                rectangle_info.x=argument_list[3].int_reference;
              if (attribute_flag[4])
                rectangle_info.y=argument_list[4].int_reference;
              if (attribute_flag[5])
                argument_list[5].int_reference=1;
            }
          RaiseImage(image,&rectangle_info,argument_list[5].int_reference);
          break;
        }
        case 50:  /* Segment */
        {
          if (first)
            {
              if (!attribute_flag[0])
                argument_list[0].int_reference=
                  info ? info->quantize_info->colorspace : RGBColorspace;
              if (!attribute_flag[1])
                argument_list[1].int_reference=0;
              if (!attribute_flag[2])
                argument_list[2].double_reference=1.0;
              if (!attribute_flag[3])
                argument_list[3].double_reference=1.5;
            }
          (void) SegmentImage(image,(ColorspaceType)
            argument_list[0].int_reference,argument_list[1].int_reference,
            argument_list[2].double_reference,argument_list[3].double_reference);
          break;
        }
        case 51:  /* Signature */
        {
          SignatureImage(image);
          break;
        }
        case 52:  /* Solarize */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=50.0;
          SolarizeImage(image,argument_list[0].double_reference);
          break;
        }
        case 53:  /* Sync */
        {
          SyncImage(image);
          break;
        }
        case 54:  /* Texture */
        {
          if (!attribute_flag[0])
            break;
          TextureImage(image,argument_list[0].image_reference);
          break;
        }
        case 55:  /* Sans */
          break;
        case 56:  /* Transparent */
        {
          PixelPacket
            target;

          target=GetOnePixel(image,0,0);
          if (attribute_flag[0])
            (void) QueryColorDatabase(argument_list[0].string_reference,
              &target);
          TransparentImage(image,target);
          break;
        }
        case 57:  /* Threshold */
        {
          if (!attribute_flag[0])
            argument_list[0].double_reference=(MaxRGB+1)/2.0;
          ThresholdImage(image,argument_list[0].double_reference);
          break;
        }
        case 58:  /* Charcoal */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference="0";
          commands[0]=client_name;
          commands[1]="-charcoal";
          commands[2]=argument_list[0].string_reference;
          MogrifyImage(info->image_info,3,commands,&image);
          if (next != image)
            next=NULL;  /* 'cause it's been blown away */
          break;
        }
        case 60:  /* Wave */
        {
          double
            amplitude,
            wavelength;

          amplitude=25.0;
          wavelength=150.0;
          if (attribute_flag[1])
            amplitude=argument_list[1].double_reference;
          if (attribute_flag[2])
            wavelength=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &amplitude,&wavelength);
          image=WaveImage(image,amplitude,wavelength,&exception);
          break;
        }
        case 61:  /* Layer */
        {
          if (!attribute_flag[0])
            argument_list[0].int_reference=0;
          LayerImage(image,(LayerType) argument_list[0].int_reference);
          break;
        }
        case 63:  /* Stereo */
        {
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in stereo",NULL);
              goto ReturnIt;
            }
          image=StereoImage(image,argument_list[0].image_reference,&exception);
          break;
        }
        case 64:  /* Stegano */
        {
          if (!attribute_flag[1])
            argument_list[1].int_reference=0;
          if (!attribute_flag[0])
            {
              MagickWarning(OptionWarning,"Missing image in stegano",NULL);
              goto ReturnIt;
            }
          image->offset=argument_list[1].int_reference;
          image=SteganoImage(image,argument_list[0].image_reference,&exception);
          break;
        }
        case 65:  /* Deconstruct */
        {
          image=DeconstructImages(image,&exception);
          break;
        }
        case 66:  /* GaussianBlur */
        {
          double
            radius,
            sigma;

          radius=0.0;
          sigma=1.0;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=GaussianBlurImage(image,radius,sigma,&exception);
          break;
        }
        case 67:  /* Convolve */
        {
          AV
            *av;

          double
            *kernel;

          unsigned int
            radius;

          if (!attribute_flag[0])
            break;
          av=(AV*) argument_list[0].array_reference;
          radius=sqrt(av_len(av)+1);
          kernel=(double *) AcquireMemory(radius*radius*sizeof(double));
          for (j=0; j < (av_len(av)+1); j++)
            kernel[j]=(double) SvNV(*(av_fetch(av,j,0)));
          for ( ; j < (radius*radius); j++)
            kernel[j]=0.0;
          image=ConvolveImage(image,radius,kernel,&exception);
          LiberateMemory((void **) &kernel);
          break;
        }
        case 68:  /* Profile */
        {
          if (!attribute_flag[0])
            argument_list[0].string_reference=(char *) NULL;
          if (!attribute_flag[1])
            argument_list[1].string_reference=(char *) NULL;
          (void) ProfileImage(image,argument_list[1].string_reference,
            argument_list[0].string_reference);
        }
        case 69:  /* UnsharpMask */
        {
          double
            amount,
            radius,
            sigma,
            threshold;

          radius=0.0;
          sigma=1.0;
          amount=1.0;
          threshold=0.05;
          if (attribute_flag[1])
            radius=argument_list[1].double_reference;
          if (attribute_flag[2])
            sigma=argument_list[2].double_reference;
          if (attribute_flag[3])
            amount=argument_list[3].double_reference;
          if (attribute_flag[4])
            threshold=argument_list[4].double_reference;
          if (attribute_flag[0])
            (void) sscanf(argument_list[0].string_reference,"%lfx%lf",
              &radius,&sigma);
          image=
            UnsharpMaskImage(image,radius,sigma,amount,threshold,&exception);
          break;
        }
      }
      if (image == (Image *) NULL)
        MagickWarning(exception.severity,exception.reason,exception.description);
      else
        if (next != (Image *) NULL)
          CatchImageException(next);
      if (region_image != (Image *) NULL)
        {
          unsigned int
            status;

          /*
            Composite region.
          */
          status=CompositeImage(region_image,CopyCompositeOp,image,
            region_info.x,region_info.y);
          if (status == False)
            CatchImageException(region_image);
          image->orphan=True;
          DestroyImage(image);
          image=region_image;
        }
      if (image)
        {
          number_images++;
          if (next && (next != image))
            {
              image->next=next->next;
              next->orphan=True;
              DestroyImage(next);
            }
          sv_setiv(*pv,(IV) image);
          next=image;
        }
      if (*pv)
        pv++;
    }

  ReturnIt:
    if (reference_vector)
      LiberateMemory((void **) &reference_vector);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o n t a g e                                                             #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Montage(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    MontageImage  = 1
    montage       = 2
    montageimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute,
      *p;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *image,
      *next;

    int
      concatenate,
      sp;

    jmp_buf
      error_jmp;

    MontageInfo
      *montage_info;

    PixelPacket
      transparent_color;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      **reference_vector,
      *sv;

    volatile int
      status;

    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to montage",NULL);
        goto MethodException;
      }
    /*
      Get options.
    */
    info=GetPackageInfo((void *) av,info);
    montage_info=CloneMontageInfo(info->image_info,(MontageInfo *) NULL);
    FormatString(montage_info->filename,"montage-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    concatenate=0;
    (void) QueryColorDatabase("none",&transparent_color);
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'B':
        case 'b':
        {
          if (strEQcase(attribute,"background"))
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->background_color);
              break;
            }
          if (strEQcase(attribute,"bordercolor"))
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->border_color);
              break;
            }
          if (strEQcase(attribute,"borderwidth"))
            {
              montage_info->border_width=SvIV(ST(i));
              break;
            }
        }
        case 'C':
        case 'c':
        {
          if (strEQcase(attribute,"compos"))
            {
              sp=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(CompositeTypes,SvPV(ST(i),na));
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid composite type",
                    SvPV(ST(i),na));
                  break;
                }
              montage_info->compose=(CompositeOperator) sp;
              break;
            }
          break;
        }
        case 'F':
        case 'f':
        {
          if (strEQcase(attribute,"frame"))
            {
              char
                *p;

              p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on frame",p);
                  break;
                }
              (void) CloneString(&montage_info->frame,p);
              if (*p == '\0')
                montage_info->frame=(char *) NULL;
              break;
            }
          if (strEQcase(attribute,"filen"))
            {
              (void) strncpy(montage_info->filename,SvPV(ST(i),na),
                MaxTextExtent);
              break;
            }
          if (strEQcase(attribute,"fill"))
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),&montage_info->fill);
              break;
            }
          if (strEQcase(attribute,"font"))
            {
              (void) CloneString(&montage_info->font,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'G':
        case 'g':
        {
          if (strEQcase(attribute,"geometry"))
            {
              char
                *p;

              p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on geometry",p);
                  break;
                }
             (void) CloneString(&montage_info->geometry,p);
             if (*p == '\0')
               montage_info->geometry=(char *) NULL;
             break;
           }
         if (strEQcase(attribute,"gravity"))
           {
             int
               in;

             in=!SvPOK(ST(i)) ? SvIV(ST(i)) :
               LookupStr(GravityTypes,SvPV(ST(i),na));
             if (in < 0)
               {
                 MagickWarning(OptionWarning,"Invalid gravity type",
                   SvPV(ST(i),na));
                 return;
               }
             montage_info->gravity=(GravityType) in;
             break;
           }
         break;
        }
        case 'L':
        case 'l':
        {
          if (strEQcase(attribute,"label"))
            {
              for (next=image; next; next=next->next)
                (void) SetImageAttribute(next,"Label",SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'M':
        case 'm':
        {
          if (strEQcase(attribute,"mattec"))
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),
                &montage_info->matte_color);
              break;
            }
          if (strEQcase(attribute,"mode"))
            {
              int
                in;

              in=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(ModeTypes,SvPV(ST(i),na));
              switch (in)
              {
                default:
                {
                  MagickWarning(OptionWarning,"Invalid mode value",
                    SvPV(ST(i),na));
                  break;
                }
                case FrameMode:
                {
                  (void) CloneString(&montage_info->frame,"15x15+3+3");
                  montage_info->shadow=True;
                  break;
                }
                case UnframeMode:
                {
                  montage_info->frame=(char *) NULL;
                  montage_info->shadow=False;
                  montage_info->border_width=0;
                  break;
                }
                case ConcatenateMode:
                {
                  montage_info->frame=(char *) NULL;
                  montage_info->shadow=False;
                  (void) CloneString(&montage_info->geometry,"+0+0");
                  montage_info->border_width=0;
                  concatenate=True;
                }
              }
              break;
            }
          break;
        }
        case 'P':
        case 'p':
        {
          if (strEQcase(attribute,"point"))
            {
              montage_info->pointsize=SvIV(ST(i));
              break;
            }
          break;
        }
        case 'S':
        case 's':
        {
          if (strEQcase(attribute,"shadow"))
            {
              sp=!SvPOK(ST(i)) ? SvIV(ST(i)) :
                LookupStr(BooleanTypes,SvPV(ST(i),na));
              if (sp < 0)
                {
                  MagickWarning(OptionWarning,"Invalid shadow type",
                    SvPV(ST(i),na));
                  break;
                }
             montage_info->shadow=sp;
             break;
            }
          if (strEQcase(attribute,"stroke"))
            {
              (void) QueryColorDatabase(SvPV(ST(i),na),&montage_info->stroke);
              break;
            }
          break;
        }
        case 'T':
        case 't':
        {
          if (strEQcase(attribute,"texture"))
            {
              (void) CloneString(&montage_info->texture,SvPV(ST(i),na));
              break;
            }
          if (strEQcase(attribute,"tile"))
            {
              char *p=SvPV(ST(i),na);
              if (!IsGeometry(p))
                {
                  MagickWarning(OptionWarning,"Invalid geometry on tile",p);
                  break;
                }
              (void) CloneString(&montage_info->tile,p);
              if (*p == '\0')
                montage_info->tile=(char *) NULL;
              break;
            }
          if (strEQcase(attribute,"title"))
            {
              (void) CloneString(&montage_info->title,SvPV(ST(i),na));
              break;
            }
          if (strEQcase(attribute,"trans"))
            {
              transparent_color=GetOnePixel(image,0,0);
              QueryColorDatabase(SvPV(ST(i),na),&transparent_color);
              for (next=image; next; next=next->next)
                TransparentImage(next,transparent_color);
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    GetExceptionInfo(&exception);
    image=MontageImages(image,montage_info,&exception);
    DestroyMontageInfo(montage_info);
    if (!image)
      {
        MagickWarning(exception.severity,exception.reason,exception.description);
        goto MethodException;
      }
    if (transparent_color.opacity != TransparentOpacity)
      for (next=image; next; next=next->next)
        TransparentImage(next,transparent_color);
    (void) strcpy(info->image_info->filename,montage_info->filename);
    (void) SetImageInfo(info->image_info,False);
    for (next=image; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o r p h                                                                 #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Morph(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    MorphImage  = 1
    morph       = 2
    morphimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *next,
      *image;

    jmp_buf
      error_jmp;

    int
      number_frames;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    av=NULL;
    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to montage",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get attribute.
    */
    number_frames=1;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'F':
        case 'f':
        {
          if (strEQcase(attribute,"frame"))
            {
              number_frames=SvIV(ST(i));
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    GetExceptionInfo(&exception);
    image=MorphImages(image,number_frames,&exception);
    if (!image)
      {
        MagickWarning(exception.severity,exception.reason,exception.description);
        goto MethodException;
      }
    for (next=image; next; next=next->next)
    {
      sv=newSViv((IV) next);
      rv=newRV(sv);
      av_push(av,sv_bless(rv,hv));
      SvREFCNT_dec(sv);
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   M o s a i c                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Mosaic(ref)
  Image::Magick ref=NO_INIT
  ALIAS:
    MosaicImage   = 1
    mosaic        = 2
    mosaicimage   = 3
  PPCODE:
  {
    AV
      *av;

    char
      *p;

    ExceptionInfo
      exception;

    HV
      *hv;

    jmp_buf
      error_jmp;

    Image
      *image;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    volatile int
      status;

    status=0;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to mosaic",NULL);
        goto MethodException;
      }
    GetExceptionInfo(&exception);
    image=MosaicImages(image,&exception);
    if (!image)
      {
        MagickWarning(exception.severity,exception.reason,exception.description);
        goto MethodException;
      }
    /*
      Create blessed Perl array for the returned image.
    */
    av=newAV();
    ST(0)=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    sv=newSViv((IV) image);
    rv=newRV(sv);
    av_push(av,sv_bless(rv,hv));
    SvREFCNT_dec(sv);
    info=GetPackageInfo((void *) av,info);
    FormatString(info->image_info->filename,"average-%.*s",MaxTextExtent-9,
      ((p=strrchr(image->filename,'/')) ? p+1 : image->filename));
    (void) strcpy(image->filename,info->image_info->filename);
    SetImageInfo(info->image_info,False);
    SvREFCNT_dec(error_list);
    error_jump=NULL;
    XSRETURN(1);

  MethodException:
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);  /* return messages in string context */
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   P i n g                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Ping(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    PingImage  = 1
    ping       = 2
    pingimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      message[MaxTextExtent];

    ExceptionInfo
      exception;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,  /* reference is the SV* of ref=SvIV(reference) */
      *s;

    EXTEND(sp,items-1);
    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    for (i=1; i < items; i++)
    {
      (void) strcpy(info->image_info->filename,(char *) SvPV(ST(i),na));
      if ((items >= 3) && strEQcase(info->image_info->filename,"filen"))
        continue;
      if ((items >= 3) && strEQcase(info->image_info->filename,"file"))
        {
          info->image_info->file=IoIFP(sv_2io(ST(i)));
          continue;
        }
      image=PingImage(info->image_info,&exception);
      if (image == (Image *) NULL)
        {
          MagickWarning(exception.severity,exception.reason,
            exception.description);
          s=(&sv_undef);
        }
      else
        {
          FormatString(message,"%u,%u,%u,%.1024s",image->columns,image->rows,
            image->filesize,image->magick);
          s=sv_2mortal(newSVpv(message,0));
          DestroyImage(image);
        }
      PUSHs(s);
    }
    info->image_info->file=(FILE *) NULL;
    SvREFCNT_dec(error_list);  /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y C o l o r                                                       #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryColor(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    querycolor = 1
  PPCODE:
  {
    char
      *attribute,
      message[MaxTextExtent];

    PixelPacket
      target_color;

    register int
      i;

    SV
      *s;

    EXTEND(sp,items-1);
    error_list=newSVpv("",0);
    for (i=1; i < items; i++)
    {
      attribute=(char *) SvPV(ST(i),na);
      if (!QueryColorDatabase(attribute,&target_color))
        s=(&sv_undef);
      else
        {
          FormatString(message,"%u,%u,%u,%u",target_color.red,
            target_color.green,target_color.blue,target_color.opacity);
          s=sv_2mortal(newSVpv(message,0));
        }
      PUSHs(s);
    }
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y C o l o r N a m e                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryColorName(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    querycolorname = 1
  PPCODE:
  {
    char
      message[MaxTextExtent];

    PixelPacket
      target_color;

    register int
      i;

    SV
      *s;

    EXTEND(sp,items-1);
    error_list=newSVpv("",0);
    for (i=1; i < items; i++)
    {
      (void) QueryColorDatabase(SvPV(ST(i),na),&target_color);
      if (!QueryColorName(&target_color,message))
        s=(&sv_undef);
      else
        s=sv_2mortal(newSVpv(message,0));
      PUSHs(s);
    }
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   Q u e r y F o n t M e t r i c s                                           #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
QueryFontMetrics(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    queryfontmetrics = 1
  PPCODE:
  {
    AffineMatrix
      affine,
      current;

    AV
      *av;

    char
      *attribute,
      message[MaxTextExtent];

    double
      x,
      y;

    DrawInfo
      *draw_info;

    FontMetric
      metrics;

    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,  /* reference is the SV* of ref=SvIV(reference) */
      *s;

    unsigned int
      status;

    EXTEND(sp,items-1);
    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to animate",NULL);
        goto MethodException;
      }
    draw_info=CloneDrawInfo(info->image_info,(DrawInfo *) NULL);
    CloneString(&draw_info->text,"");
    current=draw_info->affine;
    IdentityAffine(&affine);
    x=0.0;
    y=0.0;
    for (i=1; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'd':
        case 'D':
        {
          if (strEQcase(attribute,"density"))
            {
              CloneString(&draw_info->density,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'f':
        case 'F':
        {
          if (strEQcase(attribute,"font"))
            {
              CloneString(&draw_info->font,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'g':
        case 'G':
        {
          if (strEQcase(attribute,"geometry"))
            {
              CloneString(&draw_info->geometry,SvPV(ST(i),na));
              break;
            }
          if (strEQcase(attribute,"gravity"))
            {
              draw_info->gravity=(GravityType)
                LookupStr(GravityTypes,SvPV(ST(i),na));
              break;
            }
          break;
        }
        case 'p':
        case 'P':
        {
          if (strEQcase(attribute,"pointsize"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&draw_info->pointsize);
              break;
            }
          break;
        }
        case 'r':
        case 'R':
        {
          if (strEQcase(attribute,"rotate"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf,%lf",&affine.rx,&affine.ry);
              break;
            }
          break;
        }
        case 's':
        case 'S':
        {
          if (strEQcase(attribute,"scale"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf,%lf",&affine.sx,&affine.sy);
              break;
            }
          if (strEQcase(attribute,"skew"))
            {
              double
                x_angle,
                y_angle;

              x_angle=0.0;
              y_angle=0.0;
              (void) sscanf(SvPV(ST(i),na),"%lf,%lf",&x_angle,&y_angle);
              affine.ry=tan(DegreesToRadians(fmod(x_angle,360.0)));
              affine.rx=tan(DegreesToRadians(fmod(y_angle,360.0)));
              break;
            }
          break;
        }
        case 't':
        case 'T':
        {
          if (strEQcase(attribute,"text"))
            {
              CloneString(&draw_info->text,SvPV(ST(i),na));
              break;
            }
          if (strEQcase(attribute,"translate"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf,%lf",&affine.tx,&affine.ty);
              break;
            }
          break;
        }
        case 'x':
        case 'X':
        {
          if (strEQcase(attribute,"x"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&x);
              break;
            }
          break;
        }
        case 'y':
        case 'Y':
        {
          if (strEQcase(attribute,"y"))
            {
              (void) sscanf(SvPV(ST(i),na),"%lf",&y);
              break;
            }
          break;
        }
        default:
          break;
      }
    }
    draw_info->affine.sx=current.sx*affine.sx+current.ry*affine.rx;
    draw_info->affine.rx=current.rx*affine.sx+current.sy*affine.rx;
    draw_info->affine.ry=current.sx*affine.ry+current.ry*affine.sy;
    draw_info->affine.sy=current.rx*affine.ry+current.sy*affine.sy;
    draw_info->affine.tx=current.sx*affine.tx+current.ry*affine.ty+current.tx;
    draw_info->affine.ty=current.rx*affine.tx+current.sy*affine.ty+current.ty;
    if (draw_info->geometry == (char *) NULL)
      {
        draw_info->geometry=AllocateString((char *) NULL);
        FormatString(draw_info->geometry,"%f,%f",x,y);
      }
    status=GetFontMetrics(image,draw_info,&metrics);
    if (status != False)
      {
        FormatString(message,"%g,%g,%d,%d,%d,%d,%d",metrics.pixels_per_em.x,
          metrics.pixels_per_em.y,metrics.ascent,metrics.descent,metrics.width,
          metrics.height,metrics.max_advance);
        s=sv_2mortal(newSVpv(message,0));
        PUSHs(s);
      }
    DestroyDrawInfo(draw_info);

  MethodException:
    SvREFCNT_dec(error_list);
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   R e a d                                                                   #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Read(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    ReadImage  = 1
    read       = 2
    readimage  = 3
  PPCODE:
  {
    AV
      *av;

    char
      **keep,
      **list;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *image;

    int
      ac,
      n;

    jmp_buf
      error_jmp;

    register char
      **p;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference,
      *rv,
      *sv;

    unsigned int
      status;

    volatile int
      number_images;

    number_images=0;
    error_list=newSVpv("",0);
    ac=(items < 2) ? 1 : items-1;
    list=(char **) AcquireMemory((ac+1)*sizeof(*list));
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto ReturnIt;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    if (SvTYPE(reference) != SVt_PVAV)
      {
        MagickWarning(OptionWarning,"Unable to read into a single image",NULL);
        goto ReturnIt;
      }
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
    n=1;
    if (items <= 1)
      *list=
        (*info->image_info->filename ? info->image_info->filename : "XC:black");
    else
      for (n=0, i=0; i < ac; i++)
      {
        list[n]=(char *) SvPV(ST(i+1),na);
        if ((items >= 3) && strEQcase(list[n],"filen"))
          continue;
        if ((items >= 3) && strEQcase(list[n],"file"))
          {
            info->image_info->file=IoIFP(sv_2io(ST(i+2)));
            continue;
          }
        n++;
      }
    list[n]=(char *) NULL;
    keep=list;
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto ReturnIt;
    status=ExpandFilenames(&n,&list);
    if (status == False)
      {
        MagickWarning(ResourceLimitWarning,"Memory allocation failed",NULL);
        goto ReturnIt;
      }
    for (i=number_images=0; i < n; i++)
    {
      (void) strncpy(info->image_info->filename,list[i],MaxTextExtent-1);
      image=ReadImage(info->image_info,&exception);
      if (image == (Image *) NULL)
        MagickWarning(exception.severity,exception.reason,exception.description);
      for ( ; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
        number_images++;
      }
    }
    info->image_info->file=(FILE *) NULL;
    /*
      Free resources.
    */
    for (i=0; i < n; i++)
      if (list[i])
        for (p=keep; list[i] != *p++; )
          if (*p == NULL)
            {
              LiberateMemory((void **) &list[i]);
              break;
            }

  ReturnIt:
    LiberateMemory((void **) &list);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   R e m o t e                                                               #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Remote(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    RemoteCommand  = 1
    remote         = 2
    remoteCommand  = 3
  PPCODE:
  {
    AV
      *av;

    SV
      *reference;

    struct PackageInfo
      *info;

    EXTEND(sp,items-1);
    error_list=newSVpv("",0);
    reference=SvRV(ST(0));
    av=(AV *) reference;
    info=GetPackageInfo((void *) av,(struct PackageInfo *) NULL);
#if defined(XlibSpecificationRelease)
    {
      Display
        *display;

      register int
        i;

      display=XOpenDisplay(info->image_info->server_name);
      for (i=1; i < items; i++)
        XRemoteCommand(display,(char *) NULL,(char *) SvPV(ST(i),na));
    }
#endif
    SvREFCNT_dec(error_list);    /* throw away all errors */
    error_list=NULL;
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   S e t                                                                     #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Set(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    SetAttributes  = 1
    SetAttribute   = 2
    set            = 3
    setattributes  = 4
    setattribute   = 5
  PPCODE:
  {
    Image
      *image;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *reference;  /* reference is the SV* of ref=SvIV(reference) */

    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    image=SetupList(reference,&info,(SV ***) NULL);
    for (i=2; i < items; i+=2)
      SetAttribute(info,image,SvPV(ST(i-1),na),ST(i));

  MethodException:
    sv_setiv(error_list,(IV) (SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   T r a n s f e r                                                           #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Transform(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    TransformImage = 1
    transform      = 2
    transformimage = 3
  PPCODE:
  {
    AV
      *av;

    char
      *attribute,
      *crop_geometry,
      *geometry;

    ExceptionInfo
      exception;

    HV
      *hv;

    Image
      *clone,
      *next,
      *image;

    jmp_buf
      error_jmp;

    register int
      i;

    struct PackageInfo
      *info;

    SV
      *av_reference,
      *reference,
      *rv,
      *sv,
      **reference_vector;

    volatile int
      status;

    av=NULL;
    reference_vector=NULL;
    status=0;
    attribute=NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    hv=SvSTASH(reference);
    av=newAV();
    av_reference=sv_2mortal(sv_bless(newRV((SV *) av),hv));
    SvREFCNT_dec(av);
    error_jump=(&error_jmp);
    status=setjmp(error_jmp);
    if (status)
      goto MethodException;
    image=SetupList(reference,&info,&reference_vector);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to montage",NULL);
        goto MethodException;
      }
    info=GetPackageInfo((void *) av,info);
    /*
      Get attribute.
    */
    crop_geometry=(char *) NULL;
    geometry=(char *) NULL;
    for (i=2; i < items; i+=2)
    {
      attribute=(char *) SvPV(ST(i-1),na);
      switch (*attribute)
      {
        case 'c':
        case 'C':
        {
          if (strEQcase(attribute,"crop"))
            {
              crop_geometry=SvPV(ST(i),na);
              break;
            }
          break;
        }
        case 'g':
        case 'G':
        default:
        {
          if (strEQcase(attribute,"geometry"))
            {
              geometry=SvPV(ST(i),na);
              break;
            }
          break;
        }
      }
      MagickWarning(OptionWarning,"Invalid attribute",attribute);
    }
    GetExceptionInfo(&exception);
    for (next=image; next; next=next->next)
    {
      clone=CloneImage(next,0,0,False,&exception);
      if (clone)
        TransformImage(&clone,crop_geometry,geometry);
      if (!image)
        {
          MagickWarning(exception.severity,exception.reason,
            exception.description);
          goto MethodException;
        }
      for (image=clone; image; image=image->next)
      {
        sv=newSViv((IV) image);
        rv=newRV(sv);
        av_push(av,sv_bless(rv,hv));
        SvREFCNT_dec(sv);
      }
    }
    ST(0)=av_reference;
    error_jump=NULL;
    SvREFCNT_dec(error_list);  /* can't return warning messages */
    error_list=NULL;
    XSRETURN(1);

  MethodException:
    error_jump=NULL;
    sv_setiv(error_list,(IV) (status ? status : SvCUR(error_list) != 0));
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }

#
###############################################################################
#                                                                             #
#                                                                             #
#                                                                             #
#   W r i t e                                                                 #
#                                                                             #
#                                                                             #
#                                                                             #
###############################################################################
#
#
void
Write(ref,...)
  Image::Magick ref=NO_INIT
  ALIAS:
    WriteImage    = 1
    write         = 2
    writeimage    = 3
  PPCODE:
  {
    char
      filename[MaxTextExtent];

    Image
      *image,
      *next;

    int
      scene;

    register int
      i;

    jmp_buf
      error_jmp;

    struct PackageInfo
      *info,
      *package_info;

    SV
      *reference;

    unsigned int
      status;

    volatile int
      number_images;

    number_images=0;
    package_info=(struct PackageInfo *) NULL;
    error_list=newSVpv("",0);
    if (!sv_isobject(ST(0)))
      {
        MagickWarning(OptionWarning,"Reference is not my type",PackageName);
        goto MethodException;
      }
    reference=SvRV(ST(0));
    error_jump=(&error_jmp);
    if (setjmp(error_jmp))
      goto MethodException;
    image=SetupList(reference,&info,(SV ***) NULL);
    if (!image)
      {
        MagickWarning(OptionWarning,"No images to write",NULL);
        goto MethodException;
      }
    package_info=ClonePackageInfo(info);
    if (items == 2)
      SetAttribute(package_info,NULL,"filen",ST(1));
    else
      if (items > 2)
        for (i=2; i < items; i+=2)
          SetAttribute(package_info,NULL,SvPV(ST(i-1),na),ST(i));
    (void) strcpy(filename,package_info->image_info->filename);
    scene=0;
    for (next=image; next; next=next->next)
    {
      (void) strcpy(next->filename,filename);
      next->scene=scene++;
    }
    SetImageInfo(package_info->image_info,True);
    for (next=image; next; next=next->next)
    {
      status=WriteImage(package_info->image_info,next);
      if (status == False)
        CatchImageException(next);
      number_images++;
      if (package_info->image_info->adjoin)
        break;
    }
    package_info->image_info->file=(FILE *) NULL;

  MethodException:
    if (package_info)
      DestroyPackageInfo(package_info);
    sv_setiv(error_list,(IV) number_images);
    SvPOK_on(error_list);
    ST(0)=sv_2mortal(error_list);
    error_list=NULL;
    error_jump=NULL;
    XSRETURN(1);
  }
