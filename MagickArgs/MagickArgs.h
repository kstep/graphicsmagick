#ifndef _usr_include_magickargs_h
#define _usr_include_magickargs_h

#include "cmdstd.h"

#include <Magick++.h>

class MagickArg {
public:
   MagickArg() {};
   virtual ~MagickArg(void) {};
   // use this for performing some work on an operand
   virtual  void
   do_something(Magick::Image &image) =0;
};

class MagickArgStr : public CmdArgStr, public MagickArg {
public:
   MagickArgStr(const char * keyword,
             const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgStr('\0', keyword, value, description, syntax_flags) {}

   MagickArgStr(const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgStr(value, description, syntax_flags) {}

   virtual ~MagickArgStr(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgStr::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgStr &
     operator=(const MagickArgStr & cp)  { val = cp.val; return  *this; }

   MagickArgStr &
     operator=(const char * cp)  { val = cp; return  *this; }

   MagickArgStr &
     operator=(const std::string & cp)  { val = cp.c_str(); return  *this; }

   operator const char*(void)  const { return  val.str; }
};

class MagickArgInt : public CmdArgInt, public MagickArg {
public:
   MagickArgInt(const char * keyword,
             const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgInt('\0', keyword, value, description, syntax_flags) {}

   MagickArgInt(const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgInt(value, description, syntax_flags) {}

   virtual ~MagickArgInt(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgInt::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgInt &
     operator=(const MagickArgInt & cp)  { val = cp.val; return  *this; }

   MagickArgInt &
     operator=(int cp)  { val = cp; return  *this; }

   operator const int(void)  const { return  val; }
};

class MagickArgFloat : public CmdArgFloat, public MagickArg {
public:
   MagickArgFloat(const char * keyword,
             const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgFloat('\0', keyword, value, description, syntax_flags) {}

   MagickArgFloat(const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgFloat(value, description, syntax_flags) {}

   virtual ~MagickArgFloat(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgFloat::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgFloat &
     operator=(const MagickArgFloat & cp)  { val = cp.val; return  *this; }

   MagickArgFloat &
     operator=(float cp)  { val = cp; return  *this; }

   operator float(void)  const { return  val; }
};

class MagickArgSet : public CmdArgSet, public MagickArg {
public:
   MagickArgSet(const char * keyword,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArgSet('\0', keyword, description, syntax_flags) {}

   virtual ~MagickArgSet(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgSet::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgSet &
     operator=(const MagickArgSet & cp) { val = cp.val; return  *this; }

   MagickArgSet &
     operator=(int new_value) { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }
};

class MagickArgClear : public CmdArgClear, public MagickArg {
public:
   MagickArgClear(const char * keyword,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgClear('\0', keyword, description, syntax_flags) {}

   virtual ~MagickArgClear(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgClear::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgClear &
     operator=(const MagickArgClear & cp) { val = cp.val; return  *this; }

   MagickArgClear &
     operator=(int new_value) { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }
};

class MagickArgToggle : public CmdArgToggle, public MagickArg {
public:
   MagickArgToggle(const char * keyword,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgToggle('\0', keyword, description, syntax_flags) {}

   virtual ~MagickArgToggle(void) {};

   virtual  int
     operator()(const char * & arg, CmdLine & cmd)
     {
       return CmdArgToggle::operator()(arg, cmd);
     }

   virtual void
     do_something(Magick::Image &image) {}

   MagickArgToggle &
     operator=(const MagickArgToggle & cp) { val = cp.val; return  *this; }

   MagickArgToggle &
     operator=(int new_value) { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }
};

#define MagickCmdArgStr(NewArgType) \
class NewArgType : public CmdArgStr, public MagickArg { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : CmdArgStr('\0', keyword, value, description, syntax_flags) {} \
\
   NewArgType(const char * value, \
             const char * description, \
             unsigned     syntax_flags =CmdArg::isPOSVALREQ) \
      : CmdArgStr(value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd); \
\
   virtual void \
     do_something(Magick::Image &image) {} \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(const char * cp)  { val = cp; return  *this; } \
\
   NewArgType & \
     operator=(const std::string & cp)  { val = cp.c_str(); return  *this; } \
\
   operator const char*(void)  const { return  val.str; } \
}

#define MagickCmdArgInt(NewArgType) \
class NewArgType : public CmdArgInt, public MagickArg { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : CmdArgInt('\0', keyword, value, description, syntax_flags) {} \
\
   NewArgType(const char * value, \
             const char * description, \
             unsigned     syntax_flags =CmdArg::isPOSVALREQ) \
      : CmdArgInt(value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd); \
\
   virtual void \
     do_something(Magick::Image &image) {} \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(int cp)  { val = cp; return  *this; } \
\
   operator int(void)  const { return  val; } \
}

// Define high level command line arguments for special types
MagickCmdArgStr(MagickArgGeometry);
MagickCmdArgStr(MagickArgColor);
MagickCmdArgInt(MagickArgColorType);
MagickCmdArgInt(MagickArgComposeType);
MagickCmdArgInt(MagickArgCompressionType);
MagickCmdArgInt(MagickArgFilterType);
MagickCmdArgInt(MagickArgGravityType);
MagickCmdArgInt(MagickArgInterlaceType);
MagickCmdArgInt(MagickArgLayerType);
MagickCmdArgInt(MagickArgNoiseType);
MagickCmdArgInt(MagickArgPreviewType);
MagickCmdArgInt(MagickArgResolutionType);

#define MagickActionArgStrLike(MagickArgType, NewArgType, Description) \
class NewArgType : public MagickArgType { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : MagickArgType(keyword, value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgType::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(const char * cp)  { val = cp; return  *this; } \
\
   NewArgType & \
     operator=(const std::string & cp)  { val = cp.c_str(); return  *this; } \
\
   operator const char*(void)  const { return  val.str; } \
}

#define MagickActionArgIntLike(MagickArgType, NewArgType, Description) \
class NewArgType : public MagickArgType { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : MagickArgType(keyword, value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgType::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(int cp)  { val = cp; return  *this; } \
\
   operator int(void)  const { return  val; } \
}

#define MagickActionArgSet(NewArgType, Description) \
class NewArgType : public MagickArgSet { \
public: \
   NewArgType(const char * keyword, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPT) \
      : MagickArgSet(keyword, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgSet::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp) { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(int new_value) { val = (new_value) ? 1 : 0; return *this; } \
\
   operator int(void) const { return  val; } \
}

#define MagickActionArgStr(NewArgType, Description) \
class NewArgType : public MagickArgStr { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : MagickArgStr(keyword, value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgStr::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp; return  *this; } \
\
   NewArgType & \
     operator=(const char * cp)  { val = cp; return  *this; } \
\
   NewArgType & \
     operator=(const std::string & cp)  { val = cp.c_str(); return  *this; } \
\
   operator const char*(void)  const { return  val.str; } \
}

#define MagickActionArgInt(NewArgType, Description) \
class NewArgType : public MagickArgInt { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : MagickArgInt(keyword, value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgInt::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(int cp)  { val = cp; return  *this; } \
\
   operator int(void)  const { return  val; } \
}

#define MagickActionArgFloat(NewArgType, Description) \
class NewArgType : public MagickArgFloat { \
public: \
   NewArgType(const char * keyword, \
             const char * value, \
             const char * description =Description, \
             unsigned     syntax_flags =CmdArg::isOPTVALREQ) \
      : MagickArgFloat(keyword, value, description, syntax_flags) {} \
\
   virtual ~NewArgType(void) {}; \
\
   virtual  int \
     operator()(const char * & arg, CmdLine & cmd) \
      { \
       return MagickArgFloat::operator()(arg, cmd); \
      } \
\
   void \
     do_something(Magick::Image &image); \
\
   NewArgType & \
     operator=(const NewArgType & cp)  { val = cp.val; return  *this; } \
\
   NewArgType & \
     operator=(float cp)  { val = cp; return  *this; } \
\
   operator float(void)  const { return  val; } \
}

//
// ---- CONVERT RELATED OPTIONS --------------------------------------------
//
MagickActionArgSet(MagickArgAdjoin, "join images into a single multi-image file");
MagickActionArgSet(MagickArgAppend, "append a sequence of images to each other");
MagickActionArgSet(MagickArgAverage, "average a sequence of images");
MagickActionArgSet(MagickArgCoalesce, "merge a sequence of images");
MagickActionArgSet(MagickArgDeconstruct, "break an image sequence into constituent parts");
MagickActionArgInt(MagickArgMorph, "morph an image sequence");
MagickActionArgSet(MagickArgMosaic, "create a mosaic from a sequence of images");

//
// ---- COMBINE RELATED OPTIONS --------------------------------------------
//
MagickActionArgFloat(MagickArgBlend, "blend the two images a given percent");
MagickActionArgIntLike(MagickArgComposeType, MagickArgCompose, "composite operator");
MagickActionArgStrLike(MagickArgGeometry, MagickArgDisplace, "shift image pixels as defined by a displacement map");
MagickActionArgIntLike(MagickArgGravityType, MagickArgGravity, "vertical and horizontal placement");
MagickActionArgInt(MagickArgStegano, "hide watermark within an image");
MagickActionArgSet(MagickArgStereo, "combine two image to create a stereo anaglyph");
MagickActionArgSet(MagickArgTile, "repeat composite operation across image");

//
// ---- MOGRIFY RELATED OPTIONS --------------------------------------------
//
MagickActionArgStr(MagickArgFormat, "image format type");

MagickActionArgSet(MagickArgAntialias, "remove pixel-aliasing");
MagickActionArgStrLike(MagickArgColor, MagickArgBackground, "set the background color");
MagickActionArgFloat(MagickArgBlur, "apply a filter to blur the image");
MagickActionArgStrLike(MagickArgGeometry, MagickArgBorder, "surround image with a border of color");
MagickActionArgStrLike(MagickArgColor, MagickArgBordercolor, "color for annotation bounding box");
MagickActionArgStrLike(MagickArgColor, MagickArgBoxcolor, "color for annotation bounding box");
MagickActionArgInt(MagickArgCache, "number of megabytes available to the pixel cache");
MagickActionArgFloat(MagickArgCharcoal, "simulate a charcoal drawing");
MagickActionArgStrLike(MagickArgColor, MagickArgColorize, "colorize the image with the pen color");
MagickActionArgInt(MagickArgColors, "preferred number of colors in the image");
MagickActionArgIntLike(MagickArgColorType, MagickArgColorspace, "alternate image colorspace");
MagickActionArgStr(MagickArgComment, "annotate image with comment");
MagickActionArgIntLike(MagickArgCompressionType, MagickArgCompress, "type of image compression");
MagickActionArgInt(MagickArgContrast, "enhance or reduce the image contrast");
MagickActionArgStrLike(MagickArgGeometry, MagickArgCrop, "preferred size and location of the cropped image");
MagickActionArgInt(MagickArgCycle, "cycle the image colormap");
MagickActionArgInt(MagickArgDelay, "display the next image after pausing");
MagickActionArgStrLike(MagickArgGeometry, MagickArgDensity, "vertical and horizontal density of the image");
MagickActionArgInt(MagickArgDepth, "depth of the image");
MagickActionArgSet(MagickArgDespeckle, "reduce the speckles within an image");
MagickActionArgStr(MagickArgDisplay, "obtain image or font from this X server");
MagickActionArgInt(MagickArgDispose, "GIF disposal method");
MagickActionArgSet(MagickArgDither, "apply Floyd/Steinberg error diffusion to image");
MagickActionArgStr(MagickArgDraw, "annotate the image with a graphic primitive");
MagickActionArgFloat(MagickArgEdge, "apply a filter to detect edges in the image");
MagickActionArgSet(MagickArgEmboss, "emboss an image");
MagickActionArgSet(MagickArgEnhance, "apply a digital filter to enhance a noisy image");
MagickActionArgSet(MagickArgEqualize, "perform histogram equalization to an image");
MagickActionArgIntLike(MagickArgFilterType, MagickArgFilter, "use this filter when resizing an image");
MagickActionArgSet(MagickArgFlip, "flip image in the vertical direction");
MagickActionArgSet(MagickArgFlop, "flop image in the horizontal direction");
MagickActionArgStr(MagickArgFont, "X11 font for displaying text");
MagickActionArgStrLike(MagickArgGeometry, MagickArgFrame, "surround image with an ornamental border");
MagickActionArgInt(MagickArgFuzz, "colors within this distance are considered equal");
MagickActionArgFloat(MagickArgGamma, "level of gamma correction");
MagickActionArgFloat(MagickArgImpload, "implode image pixels about the center");
MagickActionArgIntLike(MagickArgInterlaceType, MagickArgInterlace, "None, Line, Plane, or Partition");
MagickActionArgStr(MagickArgLabel, "assign a label to an image");
MagickActionArgIntLike(MagickArgLayerType, MagickArgLayer, "Red, Green, Blue, Matte");
MagickActionArgInt(MagickArgLinewidth, "width of line in pixels");
MagickActionArgInt(MagickArgLoop, "add Netscape loop extension to your GIF animation");
MagickActionArgStr(MagickArgMap, "transform image colors to match this set of colors");
MagickActionArgSet(MagickArgMatte, "store matte channel if the image has one");
MagickActionArgStrLike(MagickArgColor, MagickArgMattecolor, "set the color of the matte channel");
MagickActionArgStr(MagickArgModulate, "vary the brightness, saturation, and hue");
MagickActionArgSet(MagickArgMonochrome, "transform image to black and white");
MagickActionArgSet(MagickArgNegate, "replace every pixel with its complementary color");
// Warning - Mogrify does some extra stuff - means to remove noise + means to add
MagickActionArgIntLike(MagickArgNoiseType, MagickArgNoise, "add or reduce noise in an image");
MagickActionArgSet(MagickArgNormalize, "transform image to span the full range of colors");
MagickActionArgStrLike(MagickArgColor, MagickArgOpaque, "change this color to the pen color");
MagickActionArgStrLike(MagickArgGeometry, MagickArgPage, "size and location of an image canvas");
MagickActionArgInt(MagickArgPaint, "simulate an oil painting");
MagickActionArgStrLike(MagickArgColor, MagickArgPen, "color for annotating or changing opaque color");
MagickActionArgSet(MagickArgPing, "forces image read to read header only");
MagickActionArgInt(MagickArgPointsize, "pointsize of Postscript font");
MagickActionArgIntLike(MagickArgPreviewType, MagickArgPreview, "image preview type");
MagickActionArgStr(MagickArgProfile, "add ICC or IPTC information profile to image");
MagickActionArgInt(MagickArgQuality, "JPEG/MIFF/PNG compression level");
MagickActionArgStrLike(MagickArgGeometry, MagickArgRaise, "lighten/darken image edges to create a 3-D effect");
MagickActionArgStrLike(MagickArgGeometry, MagickArgRegion, "apply options to a portion of the image");
MagickActionArgStrLike(MagickArgGeometry, MagickArgRoll, "roll an image vertically or horizontally");
// NOTE: special case not handled like Mogrify - it allows a prefix os < or >
MagickActionArgFloat(MagickArgRotate, "apply rotation to the image");
MagickActionArgStrLike(MagickArgGeometry, MagickArgSample, "scale image with pixel sampling");
MagickActionArgInt(MagickArgScene, "image scene number");
MagickActionArgInt(MagickArgSeed, "pseudo-random number generator seed value");
MagickActionArgSet(MagickArgSegment, "segment an image");
MagickActionArgFloat(MagickArgShade, "shade the image using a distant light source");
MagickActionArgFloat(MagickArgSharpen, "apply a filter to sharpen the image");
MagickActionArgFloat(MagickArgShear, "slide one edge of the image along the X or Y axis");
MagickActionArgStrLike(MagickArgGeometry, MagickArgSize, "width and height of image");
MagickActionArgInt(MagickArgSolarize, "negate all pixels above the threshold level");
MagickActionArgInt(MagickArgSpread, "displace image pixels by a random amount");
MagickActionArgFloat(MagickArgSwirl, "swirl image pixels about the center");
MagickActionArgStr(MagickArgTexture, "name of texture to tile onto the image background");
MagickActionArgInt(MagickArgThreshold, "threshold the image");
MagickActionArgStrLike(MagickArgColor, MagickArgTransparent, "make this color transparent within the image");
MagickActionArgInt(MagickArgTreedepth, "depth of the color classification tree");
MagickActionArgIntLike(MagickArgResolutionType, MagickArgUnits, "Inch, Centimeter, or Undefined");
MagickActionArgSet(MagickArgVerbose, "print detailed information about the image");
MagickActionArgStr(MagickArgView, "FlashPix viewing transforms");
MagickActionArgFloat(MagickArgWave, "alter an image along a sine wave");
MagickActionArgStrLike(MagickArgGeometry, MagickArgZoom, "preferred size or location of the image");

#endif /* _usr_include_magickargs_h */
