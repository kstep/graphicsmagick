#include "MagickArgs.h"
  
using namespace Magick;

int
MagickArgGeometry::operator()(const char * & arg, CmdLine & cmd)
{
  try
  {
    Geometry temp(arg);
    if (!temp.isValid())
      return 1;
    return compile(arg, cmd, val);
  }
  catch( exception error_ )
    {
      arg = NULL;
      return 1;
    }
  arg = NULL;
  return  1;
}

int
MagickArgColor::operator()(const char * & arg, CmdLine & cmd)
{
  try
  {
    Color temp(arg);
    if (!temp.isValid())
      return 1;
    return compile(arg, cmd, val);
  }
  catch( exception error_ )
    {
      arg = NULL;
      return 1;
    }
  arg = NULL;
  return  1;
}

int
MagickArgColorType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedColorspace;
  if (stricmp("cmyk",arg) == 0)
    val=CMYKColorspace;
  if (stricmp("gray",arg) == 0)
    val=GRAYColorspace;
  if (stricmp("ohta",arg) == 0)
    val=OHTAColorspace;
  if (stricmp("rgb",arg) == 0)
    val=RGBColorspace;
  if (stricmp("srgb",arg) == 0)
    val=sRGBColorspace;
  if (stricmp("transparent",arg) == 0)
    val=TransparentColorspace;
  if (stricmp("xyz",arg) == 0)
    val=XYZColorspace;
  if (stricmp("ycbcr",arg) == 0)
    val=YCbCrColorspace;
  if (stricmp("ycc",arg) == 0)
    val=YCCColorspace;
  if (stricmp("yiq",arg) == 0)
    val=YIQColorspace;
  if (stricmp("ypbpr",arg) == 0)
    val=YPbPrColorspace;
  if (stricmp("yuv",arg) == 0)
    val=YUVColorspace;
  if (val == UndefinedColorspace)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgComposeType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedCompositeOp;
  if (stricmp("Over",arg) == 0)
    val=OverCompositeOp;
  if (stricmp("In",arg) == 0)
    val=InCompositeOp;
  if (stricmp("out",arg) == 0)
    val=OutCompositeOp;
  if (stricmp("Atop",arg) == 0)
    val=AtopCompositeOp;
  if (stricmp("Xor",arg) == 0)
    val=XorCompositeOp;
  if (stricmp("Plus",arg) == 0)
    val=PlusCompositeOp;
  if (stricmp("Minus",arg) == 0)
    val=MinusCompositeOp;
  if (stricmp("Add",arg) == 0)
    val=AddCompositeOp;
  if (stricmp("Subtract",arg) == 0)
    val=SubtractCompositeOp;
  if (stricmp("Difference",arg) == 0)
    val=DifferenceCompositeOp;
  if (stricmp("Bumpmap",arg) == 0)
    val=BumpmapCompositeOp;
  if (stricmp("Replace",arg) == 0)
    val=ReplaceCompositeOp;
  if (stricmp("ReplaceRed",arg) == 0)
    val=ReplaceRedCompositeOp;
  if (stricmp("ReplaceGreen",arg) == 0)
    val=ReplaceGreenCompositeOp;
  if (stricmp("ReplaceBlue",arg) == 0)
    val=ReplaceBlueCompositeOp;
  if (stricmp("ReplaceMatte",arg) == 0)
    val=ReplaceMatteCompositeOp;
  if (stricmp("Blend",arg) == 0)
    val=BlendCompositeOp;
  if (stricmp("Displace",arg) == 0)
    val=DisplaceCompositeOp;
  if (stricmp("Annotate",arg) == 0)
    val=AnnotateCompositeOp;
  if (stricmp("Modulalate",arg) == 0)
    val=ModulateCompositeOp;
  if (val == UndefinedCompositeOp)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgCompressionType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedCompression;
  if (stricmp("none",arg) == 0)
    val=NoCompression;
  if (stricmp("bzip",arg) == 0)
    val=BZipCompression;
  if (stricmp("fax",arg) == 0)
    val=FaxCompression;
  if (stricmp("group4",arg) == 0)
    val=Group4Compression;
  if (stricmp("jpeg",arg) == 0)
    val=JPEGCompression;
  if (stricmp("lzw",arg) == 0)
    val=LZWCompression;
  if (stricmp("runlengthencode",arg) == 0)
    val=RunlengthEncodedCompression;
  if (stricmp("zip",arg) == 0)
    val=ZipCompression;
  if (val == UndefinedCompression)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgFilterType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedFilter;
  if (stricmp("point",arg) == 0)
    val=PointFilter;
  if (stricmp("box",arg) == 0)
    val=BoxFilter;
  if (stricmp("triangle",arg) == 0)
    val=TriangleFilter;
  if (stricmp("hermite",arg) == 0)
    val=HermiteFilter;
  if (stricmp("hanning",arg) == 0)
    val=HanningFilter;
  if (stricmp("hamming",arg) == 0)
    val=HammingFilter;
  if (stricmp("blackman",arg) == 0)
    val=BlackmanFilter;
  if (stricmp("gaussian",arg) == 0)
    val=GaussianFilter;
  if (stricmp("quadratic",arg) == 0)
    val=QuadraticFilter;
  if (stricmp("gaussian",arg) == 0)
    val=ZipCompression;
  if (stricmp("cubic",arg) == 0)
    val=CubicFilter;
  if (stricmp("catrom",arg) == 0)
    val=CatromFilter;
  if (stricmp("mitchell",arg) == 0)
    val=MitchellFilter;
  if (stricmp("lanczos",arg) == 0)
    val=LanczosFilter;
  if (stricmp("bessel",arg) == 0)
    val=BesselFilter;
  if (stricmp("sinc",arg) == 0)
    val=SincFilter;
  if (val == UndefinedFilter)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgResolutionType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedResolution;
  if (stricmp("PixelsPerInch",arg) == 0)
    val=PixelsPerInchResolution;
  if (stricmp("PixelsPerCentimeter",arg) == 0)
    val=PixelsPerCentimeterResolution;
  if (val == UndefinedResolution)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgGravityType::operator()(const char * & arg, CmdLine & cmd)
{
  val=ForgetGravity;
  if (stricmp("northWest",arg) == 0)
    val=NorthWestGravity;
  if (stricmp("north",arg) == 0)
    val=NorthGravity;
  if (stricmp("northEast",arg) == 0)
    val=NorthEastGravity;
  if (stricmp("west",arg) == 0)
    val=WestGravity;
  if (stricmp("center",arg) == 0)
    val=CenterGravity;
  if (stricmp("east",arg) == 0)
    val=EastGravity;
  if (stricmp("southWest",arg) == 0)
    val=SouthWestGravity;
  if (stricmp("south",arg) == 0)
    val=SouthGravity;
  if (stricmp("southEast",arg) == 0)
    val=SouthEastGravity;
  if (val == ForgetGravity)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgPreviewType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedPreview;
  if (stricmp("Rotate",arg) == 0)
    val=RotatePreview;
  if (stricmp("Shear",arg) == 0)
    val=ShearPreview;
  if (stricmp("Roll",arg) == 0)
    val=RollPreview;
  if (stricmp("Hue",arg) == 0)
    val=HuePreview;
  if (stricmp("Saturation",arg) == 0)
    val=SaturationPreview;
  if (stricmp("Brightness",arg) == 0)
    val=BrightnessPreview;
  if (stricmp("Gamma",arg) == 0)
    val=GammaPreview;
  if (stricmp("Spiff",arg) == 0)
    val=SpiffPreview;
  if (stricmp("Dull",arg) == 0)
    val=DullPreview;
  if (stricmp("Grayscale",arg) == 0)
    val=GrayscalePreview;
  if (stricmp("Quantize",arg) == 0)
    val=QuantizePreview;
  if (stricmp("Despeckle",arg) == 0)
    val=DespecklePreview;
  if (stricmp("ReduceNoise",arg) == 0)
    val=ReduceNoisePreview;
  if (stricmp("AddNoise",arg) == 0)
    val=AddNoisePreview;
  if (stricmp("Sharpen",arg) == 0)
    val=SharpenPreview;
  if (stricmp("Blur",arg) == 0)
    val=BlurPreview;
  if (stricmp("Threshold",arg) == 0)
    val=ThresholdPreview;
  if (stricmp("EdgeDetect",arg) == 0)
    val=EdgeDetectPreview;
  if (stricmp("Spread",arg) == 0)
    val=SpreadPreview;
  if (stricmp("Shade",arg) == 0)
    val=ShadePreview;
  if (stricmp("Raise",arg) == 0)
    val=RaisePreview;
  if (stricmp("Segment",arg) == 0)
    val=SegmentPreview;
  if (stricmp("Solarize",arg) == 0)
    val=SolarizePreview;
  if (stricmp("Swirl",arg) == 0)
    val=SwirlPreview;
  if (stricmp("Implode",arg) == 0)
    val=ImplodePreview;
  if (stricmp("Wave",arg) == 0)
    val=WavePreview;
  if (stricmp("OilPaint",arg) == 0)
    val=OilPaintPreview;
  if (stricmp("CharcoalDrawing",arg) == 0)
    val=CharcoalDrawingPreview;
  if (stricmp("JPEG",arg) == 0)
    val=JPEGPreview;
  if (val == UndefinedPreview)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgNoiseType::operator()(const char * & arg, CmdLine & cmd)
{
  val=-1;
  if (stricmp("gaussian",arg) == 0)
    val=GaussianNoise;
  if (stricmp("multiplicative",arg) == 0)
    val=MultiplicativeGaussianNoise;
  if (stricmp("impulse",arg) == 0)
    val=ImpulseNoise;
  if (stricmp("laplacian",arg) == 0)
    val=LaplacianNoise;
  if (stricmp("poisson",arg) == 0)
    val=PoissonNoise;
  if (val == -1)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgLayerType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedLayer;
  if (stricmp("Red",arg) == 0)
    val=RedLayer;
  if (stricmp("Green",arg) == 0)
    val=GreenLayer;
  if (stricmp("Blue",arg) == 0)
    val=BlueLayer;
  if (stricmp("Matte",arg) == 0)
    val=MatteLayer;
  if (val == UndefinedLayer)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

int
MagickArgInterlaceType::operator()(const char * & arg, CmdLine & cmd)
{
  val=UndefinedInterlace;
  if (stricmp("none",arg) == 0)
    val=NoInterlace;
  if (stricmp("line",arg) == 0)
    val=LineInterlace;
  if (stricmp("plane",arg) == 0)
    val=PlaneInterlace;
  if (stricmp("partition",arg) == 0)
    val=PartitionInterlace;
  if (val == UndefinedInterlace)
  {
    arg = NULL;
    return 1;
  }
  arg = NULL;
  return 0;
}

//
// ---- CONVERT RELATED OPTIONS --------------------------------------------
//
//MagickActionArgSet(MagickArgAdjoin, "join images into a single multi-image file");
void
MagickArgAdjoin::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgAppend, "append an image sequence");
void
MagickArgAppend::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgAverage, "average an image sequence");
void
MagickArgAverage::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgCoalesce, "combine an image sequence");
void
MagickArgCoalesce::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgDeconstruct, "break an image sequence into constituent parts");
void
MagickArgDeconstruct::do_something(Magick::Image &image)
{
}
//MagickActionArgInt(MagickArgMorph, "morph an image sequence");
void
MagickArgMorph::do_something(Magick::Image &image)
{
  //
  // NOTE: Not implemented yet, needs a container of images to do
  // its job.
  //
}
//MagickActionArgSet(MagickArgMosaic, "create a mosaic from a sequence of images");
void
MagickArgMosaic::do_something(Magick::Image &image)
{
}

//
// ---- COMBINE RELATED OPTIONS --------------------------------------------
//
//MagickActionArgFloat(MagickArgBlend, "blend the two images a given percent");
void
MagickArgBlend::do_something(Magick::Image &image)
{
}
//MagickActionArgIntLike(MagickArgComposeType, MagickArgCompose, "composite operator");
void
MagickArgCompose::do_something(Magick::Image &image)
{
  //switch ((ComposeType)val)
  //{
  //}
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgDisplace, "shift image pixels as defined by a displacement map");
void
MagickArgDisplace::do_something(Magick::Image &image)
{
}
//MagickActionArgIntLike(MagickArgGravityType, MagickArgGravity, "vertical and horizontal text placement");
void
MagickArgGravity::do_something(Magick::Image &image)
{
  //image.xxxxxx( (GravityType)val );
  //
  // Yikes! Turns out the "gravity" is an internal variable inside of
  // MogrifyImage. It does not exist inside of the ImageMagick structures
  // at all. This is another case of having to pass information between
  // various command line options that has to be solved.
}
//MagickActionArgInt(MagickArgStegano, "hide watermark within an image");
void
MagickArgStegano::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgStereo, "combine two image to create a stereo anaglyph");
void
MagickArgStereo::do_something(Magick::Image &image)
{
}
//MagickActionArgSet(MagickArgTile, "repeat composite operation across image");
void
MagickArgTile::do_something(Magick::Image &image)
{
}


//
// ---- MOGRIFY RELATED OPTIONS --------------------------------------------
//
//MagickActionArgStr(MagickArgFormat, "image format type");
void
MagickArgFormat::do_something(Magick::Image &image)
{
}


//MagickActionArgSet(MagickArgAntialias, "remove pixel-aliasing");
void
MagickArgAntialias::do_something(Magick::Image &image)
{
  image.antiAlias( val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgBackground, "set the background color");
void
MagickArgBackground::do_something(Magick::Image &image)
{
  image.backgroundColor( (std::string)val );
}
//MagickActionArgFloat(MagickArgBlur, "apply a filter to blur the image");
void
MagickArgBlur::do_something(Magick::Image &image)
{
  image.blur( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgBorder, "surround image with a border of color");
void
MagickArgBorder::do_something(Magick::Image &image)
{
  image.border( (std::string)val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgBordercolor, "color for annotation bounding box");
void
MagickArgBordercolor::do_something(Magick::Image &image)
{
  image.borderColor( (std::string)val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgBoxcolor, "color for annotation bounding box");
void
MagickArgBoxcolor::do_something(Magick::Image &image)
{
  image.boxColor( (std::string)val );
}
//MagickActionArgFloat(MagickArgCharcoal, "simulate a charcoal drawing");
void
MagickArgCharcoal::do_something(Magick::Image &image)
{
  image.charcoal( val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgColorize, "colorize the image with the pen color");
void
MagickArgColorize::do_something(Magick::Image &image)
{
  Color temp = val;
  image.colorize( temp, image.penColor() );
}
//MagickActionArgInt(MagickArgColors, "preferred number of colors in the image");
void
MagickArgColors::do_something(Magick::Image &image)
{
  image.quantizeColors( val );
}
//MagickActionArgIntLike(MagickArgColorType, MagickArgColorspace, "alternate image colorspace");
void
MagickArgColorspace::do_something(Magick::Image &image)
{
  switch ((ColorspaceType)val)
  {
    case CMYKColorspace:
    case RGBColorspace:
    {
      // Discovered some missing functionality in Magick++
      MagickLib::RGBTransformImage(image.image(), (ColorspaceType)val);
      break;
    }
    case GRAYColorspace:
    {
      if (image.quantizeColors() == 0)
        image.quantizeColors(256);
      image.quantizeTreeDepth(8);
      break;
    }
  }
  image.quantizeColorSpace( (ColorspaceType)val );
  //
  // NOTE: We may or may not want this to quantize at this point
  //
  image.quantize();
}
//MagickActionArgStr(MagickArgComment, "annotate image with comment");
void
MagickArgComment::do_something(Magick::Image &image)
{
  image.comment( (std::string)val );
}
//MagickActionArgIntLike(MagickArgCompressionType, MagickArgCompress, "type of image compression");
void
MagickArgCompress::do_something(Magick::Image &image)
{
  image.compressType( (CompressionType)val );
}
//MagickActionArgInt(MagickArgContrast, "enhance or reduce the image contrast");
void
MagickArgContrast::do_something(Magick::Image &image)
{
  image.contrast( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgCrop, "preferred size and location of the cropped image");
void
MagickArgCrop::do_something(Magick::Image &image)
{
  image.crop( (std::string)val );
}
//MagickActionArgInt(MagickArgCycle, "cycle the image colormap");
void
MagickArgCycle::do_something(Magick::Image &image)
{
  image.cycleColormap( val );
}
//MagickActionArgInt(MagickArgDelay, "display the next image after pausing");
void
MagickArgDelay::do_something(Magick::Image &image)
{
  image.animationDelay( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgDensity, "vertical and horizontal density of the image");
void
MagickArgDensity::do_something(Magick::Image &image)
{
  image.density( (std::string)val );
}
//MagickActionArgInt(MagickArgDepth, "depth of the image");
void
MagickArgDepth::do_something(Magick::Image &image)
{
  image.depth( val );
}
//MagickActionArgSet(MagickArgDespeckle, "reduce the speckles within an image");
void
MagickArgDespeckle::do_something(Magick::Image &image)
{
  image.despeckle();
}
//MagickActionArgStr(MagickArgDisplay, "obtain image or font from this X server");
void
MagickArgDisplay::do_something(Magick::Image &image)
{
  image.x11Display( (std::string)val );
}
//MagickActionArgInt(MagickArgDispose, "GIF disposal method");
void
MagickArgDispose::do_something(Magick::Image &image)
{
  image.gifDisposeMethod( val );
}
//MagickActionArgSet(MagickArgDither, "apply Floyd/Steinberg error diffusion to image");
void
MagickArgDither::do_something(Magick::Image &image)
{
  image.quantizeDither( val );
}
//MagickActionArgStr(MagickArgDraw, "annotate the image with a graphic primitive");
void
MagickArgDraw::do_something(Magick::Image &image)
{
  MagickLib::AnnotateInfo annotateInfo;
  MagickLib::GetAnnotateInfo( image.options()->imageInfo(), &annotateInfo );
  //
  // NOTE: This all needs more work. It is not clear how to handle the
  // "state" of a previous command line option like "geometry" and have
  // it effect another command line option such as "draw".
  //
  // MogrifyImage does the following internally
  //
  // if (geometry != (char *) NULL)
  //   (void) CloneString(&annotate_info.geometry,geometry);
  // if (gravity != ForgetGravity)
  //   annotate_info.gravity=gravity;
  //
  //Magick::CloneString( &annotateInfo.primitive, (std::string)val );
  //MagickLib::DrawImage( image.image(), &annotateInfo );
  //MagickLib::DestroyAnnotateInfo( &annotateInfo );
}
//MagickActionArgFloat(MagickArgEdge, "apply a filter to detect edges in the image");
void
MagickArgEdge::do_something(Magick::Image &image)
{
  image.charcoal( val );
}
//MagickActionArgSet(MagickArgEmboss, "emboss an image");
void
MagickArgEmboss::do_something(Magick::Image &image)
{
  image.emboss();
}
//MagickActionArgSet(MagickArgEnhance, "apply a digital filter to enhance a noisy image");
void
MagickArgEnhance::do_something(Magick::Image &image)
{
  image.enhance();
}
//MagickActionArgSet(MagickArgEqualize, "perform histogram equalization to an image");
void
MagickArgEqualize::do_something(Magick::Image &image)
{
  image.equalize();
}
//MagickActionArgIntLike(MagickArgFilterType, MagickArgFilter, "use this filter when resizing an image");
void
MagickArgFilter::do_something(Magick::Image &image)
{
  image.filterType( (FilterType)val );
}
//MagickActionArgSet(MagickArgFlip, "flip image in the vertical direction");
void
MagickArgFlip::do_something(Magick::Image &image)
{
  image.flip();
}
//MagickActionArgSet(MagickArgFlop, "flop image in the horizontal direction");
void
MagickArgFlop::do_something(Magick::Image &image)
{
  image.flop();
}
//MagickActionArgStr(MagickArgFont, "X11 font for displaying text");
void
MagickArgFont::do_something(Magick::Image &image)
{
  image.font( (std::string)val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgFrame, "surround image with an ornamental border");
void
MagickArgFrame::do_something(Magick::Image &image)
{
  image.frame( (std::string)val );
}
//MagickActionArgInt(MagickArgFuzz, "colors within this distance are considered equal");
void
MagickArgFuzz::do_something(Magick::Image &image)
{
  image.colorFuzz( val );
}
//MagickActionArgFloat(MagickArgGamma, "level of gamma correction");
void
MagickArgGamma::do_something(Magick::Image &image)
{
  image.gamma( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgZoom, "preferred size or location of the image");
void
MagickArgZoom::do_something(Magick::Image &image)
{
  image.zoom( (std::string)val );
}
//MagickActionArgFloat(MagickArgImpload, "implode image pixels about the center");
void
MagickArgImpload::do_something(Magick::Image &image)
{
  image.implode( val );
}
//MagickActionArgIntLike(MagickArgInterlaceType, MagickArgInterlace, "None, Line, Plane, or Partition");
void
MagickArgInterlace::do_something(Magick::Image &image)
{
  image.interlaceType( (InterlaceType)val );
}
//MagickActionArgStr(MagickArgLabel, "assign a label to an image");
void
MagickArgLabel::do_something(Magick::Image &image)
{
  image.label( (std::string)val );
}
//MagickActionArgIntLike(MagickArgLayerType, MagickArgLayer, "Red, Green, Blue, Matte");
void
MagickArgLayer::do_something(Magick::Image &image)
{
  image.layer( (LayerType)val );
}
//MagickActionArgInt(MagickArgLinewidth, "width of line in pixels");
void
MagickArgLinewidth::do_something(Magick::Image &image)
{
  image.lineWidth( val );
}
//MagickActionArgInt(MagickArgLoop, "add Netscape loop extension to your GIF animation");
void
MagickArgLoop::do_something(Magick::Image &image)
{
  image.animationIterations( val );
}
//MagickActionArgSet(MagickArgMap, "transform image colors to match this set of colors");
void
MagickArgMap::do_something(Magick::Image &image)
{
  //
  // NOTE: This is different then what the real MogrifyImage does. It
  // waits till all the other command line options have been processed
  // and then does the "map" at then end. This is mostly so that the
  // state of the "dither" flag can be interrogated.
  //
  Image temp;
  temp.read( (std::string)val );
  image.map( temp, image.quantizeDither() );
}
//MagickActionArgSet(MagickArgMatte, "store matte channel if the image has one");
void
MagickArgMatte::do_something(Magick::Image &image)
{
  image.matte( val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgMattecolor, "set the color of the matte channel");
void
MagickArgMattecolor::do_something(Magick::Image &image)
{
  Color temp = val;
  image.matteColor( temp );
}
//MagickActionArgStr(MagickArgModulate, "vary the brightness, saturation, and hue");
void
MagickArgModulate::do_something(Magick::Image &image)
{
  //
  // NOTE: This is one case were we could really use some additional
  // parsing functionality.
  //image.modulate( val, val, val );

  //modifyImage();
  MagickLib::ModulateImage( image.image(), val );
  //throwMagickError();
}
//MagickActionArgSet(MagickArgMonochrome, "transform image to black and white");
void
MagickArgMonochrome::do_something(Magick::Image &image)
{
  image.monochrome( true );
  image.quantizeColors(2);
  image.quantizeTreeDepth(8);
  image.quantizeColorSpace( GRAYColorspace );
  //
  // NOTE: We may or may not want this to quantize at this point
  //
  image.quantize();
}
//MagickActionArgSet(MagickArgNegate, "replace every pixel with its complementary color");
void
MagickArgNegate::do_something(Magick::Image &image)
{
  image.negate();
}
//MagickActionArgIntLike(MagickArgNoiseType, MagickArgNoise, "add or reduce noise in an image");
void
MagickArgNoise::do_something(Magick::Image &image)
{
  if (val < 0)
    image.reduceNoise();
  else
    image.addNoise( (NoiseType)val );
}
//MagickActionArgSet(MagickArgNormalize, "transform image to span the full range of colors");
void
MagickArgNormalize::do_something(Magick::Image &image)
{
  image.normalize();
}
//MagickActionArgStrLike(MagickArgColor, MagickArgOpaque, "change this color to the pen color");
void
MagickArgOpaque::do_something(Magick::Image &image)
{
  Color temp = val;
  image.opaque( temp, image.penColor() );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgPage, "size and location of an image canvas");
void
MagickArgPage::do_something(Magick::Image &image)
{
  image.page( (std::string)val );
}
//MagickActionArgInt(MagickArgPaint, "simulate an oil painting");
void
MagickArgPaint::do_something(Magick::Image &image)
{
  image.oilPaint( val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgPen, "color for annotating or changing opaque color");
void
MagickArgPen::do_something(Magick::Image &image)
{
  Color temp = val;
  image.penColor( temp );
}
//MagickActionArgSet(MagickArgPing, "forces image read to read header only");
void
MagickArgPing::do_something(Magick::Image &image)
{
  //ImageInfo *ip = image.options()->imageInfo();
  //if (ip)
  //  ip->ping = val;
}
//MagickActionArgInt(MagickArgPointsize, "pointsize of Postscript font");
void
MagickArgPointsize::do_something(Magick::Image &image)
{
  image.fontPointsize ( val );
}
//MagickActionArgIntLike(MagickArgPreviewType, MagickArgPreview, "image preview type");
void
MagickArgPreview::do_something(Magick::Image &image)
{
  //ImageInfo *ip = image.options()->imageInfo();
  //if (ip)
  // ip->preview_type = (MagickLib::PreviewType)val;
}
//MagickActionArgStr(MagickArgProfile, "add ICC or IPTC information profile to image");
void
MagickArgProfile::do_something(Magick::Image &image)
{
#ifdef MOGRIFY_LOGIC
  Image *profile;

  if (*option == '+')
    {
      /*
        Remove a ICC or IPTC profile from the image.
      */
      option=argv[++i];
      if (Latin1Compare("icc",option) == 0)
        {
          if ((*image)->color_profile.length != 0)
            FreeMemory((char *) (*image)->color_profile.info);
          (*image)->color_profile.length=0;
          (*image)->color_profile.info=(unsigned char *) NULL;
        }
      if (Latin1Compare("iptc",option) == 0)
        {
          if ((*image)->iptc_profile.length != 0)
            FreeMemory((char *) (*image)->iptc_profile.info);
          (*image)->iptc_profile.length=0;
          (*image)->iptc_profile.info=(unsigned char *) NULL;
        }
      continue;
    }
  /*
    Add a ICC or IPTC profile to the image.
  */
  (void) strcpy(local_info->filename,argv[++i]);
  profile=ReadImage(local_info);
  if (profile == (Image *) NULL)
    continue;
  if (Latin1Compare("icc",profile->magick) == 0)
    {
      if ((*image)->color_profile.length != 0)
        FreeMemory((char *) (*image)->color_profile.info);
      (*image)->color_profile.length=profile->color_profile.length;
      (*image)->color_profile.info=profile->color_profile.info;
      profile->color_profile.length=0;
      profile->color_profile.info=(unsigned char *) NULL;
    }
  if (Latin1Compare("iptc",profile->magick) == 0)
    {
      if ((*image)->iptc_profile.length != 0)
        FreeMemory((char *) (*image)->iptc_profile.info);
      (*image)->iptc_profile.length=profile->iptc_profile.length;
      (*image)->iptc_profile.info=profile->iptc_profile.info;
      profile->iptc_profile.length=0;
      profile->iptc_profile.info=(unsigned char *) NULL;
    }
  DestroyImage(profile);
#endif
}
//MagickActionArgInt(MagickArgQuality, "JPEG/MIFF/PNG compression level");
void
MagickArgQuality::do_something(Magick::Image &image)
{
  image.quality( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgRaise, "lighten/darken image edges to create a 3-D effect");
void
MagickArgRaise::do_something(Magick::Image &image)
{
  Geometry temp = (std::string)val;
  // NOTE: need to figure out how to set the raisedFlag value. MogrifyImage
  // uses the +- option setting to determine this. Also the logic in Magick++
  // for the raise operation seems to differ quite a bit from the logic in
  // MogrifyImage.
  bool raisedFlag = false;
  image.raise( temp, raisedFlag );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgRegion, "apply options to a portion of the image");
void
MagickArgRegion::do_something(Magick::Image &image)
{
  Geometry region_info;

  region_info.xOff(0);
  region_info.yOff(0);
  region_info.width(image.columns());
  region_info.height(image.rows());

  region_info = val;
  image.crop( region_info );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgRoll, "roll an image vertically or horizontally");
void
MagickArgRoll::do_something(Magick::Image &image)
{
  image.roll( (std::string)val );
}
// NOTE: special case not handled like Mogrify - it allows a prefix os < or >
//MagickActionArgFloat(MagickArgRotate, "apply rotation to the image");
void
MagickArgRotate::do_something(Magick::Image &image)
{
  // NOTE: this function has additional capabilities to "crop" and "sharpen"
  // that can not be controlled via this interface.
  image.rotate( val );
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgSample, "scale image with pixel sampling");
void
MagickArgSample::do_something(Magick::Image &image)
{
  image.sample( (std::string)val );
}
//MagickActionArgInt(MagickArgScene, "image scene number");
void
MagickArgScene::do_something(Magick::Image &image)
{
  image.scene( val );
}
//MagickActionArgInt(MagickArgSeed, "pseudo-random number generator seed value");
void
MagickArgSeed::do_something(Magick::Image &image)
{
  srand( val );
}
//MagickActionArgSet(MagickArgSegment, "segment an image");
void
MagickArgSegment::do_something(Magick::Image &image)
{
  // NOTE: This operation defaults to the settings of:
  //  double clusterThreshold_ = 1.0,  double smoothingThreshold_ = 1.5
  // we need to figure out how to provide these values on the command
  // as well.
  image.segment();
}
//MagickActionArgFloat(MagickArgShade, "shade the image using a distant light source");
void
MagickArgShade::do_something(Magick::Image &image)
{
  // NOTE: This operation also has two other settings that can not be
  // accessed through this interface. These can be passed via the
  //  double elevation_ = 30, bool  colorShading_ = false
  // values on the calling interface.
  image.shade( val ); // passing only the azimuth
}
//MagickActionArgFloat(MagickArgSharpen, "apply a filter to sharpen the image");
void
MagickArgSharpen::do_something(Magick::Image &image)
{
  image.sharpen( val );
}
//MagickActionArgFloat(MagickArgShear, "slide one edge of the image along the X or Y axis");
void
MagickArgShear::do_something(Magick::Image &image)
{
  // NOTE: This operation has two other settings that cannot be accessed via
  // this interface. These are:  double yShearAngle_, bool crop_ = false
  image.shear( val, val ); // passing only the xShearAngle
}
//MagickActionArgStrLike(MagickArgGeometry, MagickArgSize, "width and height of image");
void
MagickArgSize::do_something(Magick::Image &image)
{
  image.size( (std::string)val );
}
//MagickActionArgInt(MagickArgSolarize, "negate all pixels above the threshold level");
void
MagickArgSolarize::do_something(Magick::Image &image)
{
  image.solarize( val );
}
//MagickActionArgInt(MagickArgSpread, "displace image pixels by a random amount");
void
MagickArgSpread::do_something(Magick::Image &image)
{
  image.spread( val );
}
//MagickActionArgFloat(MagickArgSwirl, "swirl image pixels about the center");
void
MagickArgSwirl::do_something(Magick::Image &image)
{
  image.swirl( val );
}
//MagickActionArgStr(MagickArgTexture, "name of texture to tile onto the image background");
void
MagickArgTexture::do_something(Magick::Image &image)
{
  image.texture( (std::string)val );
}
//MagickActionArgInt(MagickArgThreshold, "threshold the image");
void
MagickArgThreshold::do_something(Magick::Image &image)
{
  image.threshold( val );
}
//MagickActionArgStrLike(MagickArgColor, MagickArgTransparent, "make this color transparent within the image");
void
MagickArgTransparent::do_something(Magick::Image &image)
{
  Color temp = (std::string)val;
  image.transparent( temp );
}
//MagickActionArgInt(MagickArgTreedepth, "depth of the color classification tree");
void
MagickArgTreedepth::do_something(Magick::Image &image)
{
  image.quantizeTreeDepth( val );
}
//MagickActionArgIntLike(MagickArgResolutionType, MagickArgUnits, "Inch, Centimeter, or Undefined");
void
MagickArgUnits::do_something(Magick::Image &image)
{
  image.resolutionUnits( (ResolutionType)val );
}
//MagickActionArgSet(MagickArgVerbose, "print detailed information about the image");
void
MagickArgVerbose::do_something(Magick::Image &image)
{
  //ImageInfo *ip = image.options()->imageInfo();
  //if (ip)
  //  ip->verbose = val;
}
//MagickActionArgStr(MagickArgView, "FlashPix viewing transforms");
void
MagickArgView::do_something(Magick::Image &image)
{
  image.view( (std::string)val );
}
//MagickActionArgFloat(MagickArgWave, "alter an image along a sine wave");
void
MagickArgWave::do_something(Magick::Image &image)
{
  // NOTE: this operation actually takes two floats: amplitude and wavelength
  // but this interface currently only permits the first to be set and the
  // second simply defaults.
  image.wave( val );
}
