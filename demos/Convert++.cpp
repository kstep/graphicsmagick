#include <iostream>
#include <string>
#include <list>

#include <..\MagickArgs\MagickArgs.h>

using namespace std;
  
using namespace Magick;

int main(int argc, char * argv[]) {
  // Declare arguments
  MagickArgAdjoin adjoinArg("adjoin");
  MagickArgAntialias antialiasArg("antialias");
  MagickArgAppend appendArg("append");
  MagickArgAverage averageArg("average");
  MagickArgBackground backgroundArg("background", "color");
  MagickArgBlur blurArg("blur", "float");
  MagickArgBorder borderArg("border", "geometry");
  MagickArgBordercolor bordercolorArg("bordercolor", "color");
  MagickArgBoxcolor boxcolorArg("boxcolor", "color");
  MagickArgCharcoal charcoalArg("charcoal", "float");
  MagickArgCoalesce coalesceArg("coalesce");
  MagickArgColorize colorizeArg("colorize", "string");
  MagickArgColors colorsArg("colors", "integer");
  MagickArgColorspace colorspaceArg("colorspace", "colorspace");
  MagickArgComment commentArg("comment", "string");
  MagickArgCompress compressArg("compress", "compression");
  MagickArgContrast contrastArg("contrast", "integer");
  MagickArgCrop cropArg("crop", "geometry");
  MagickArgCycle cycleArg("cycle", "integer");
  MagickArgDelay delayArg("delay", "integer");
  MagickArgDensity densityArg("density", "geometry");
  MagickArgDepth depthArg("depth", "integer");
  MagickArgDespeckle despeckleArg("despeckle");
  MagickArgDisplay displayArg("display", "string");
  MagickArgDispose disposeArg("dispose", "integer");
  MagickArgDither ditherArg("dither");
  MagickArgDraw drawArg("draw", "string");
  MagickArgEdge edgeArg("edge", "float");
  MagickArgEmboss embossArg("emboss");
  MagickArgEnhance enhanceArg("enhance");
  MagickArgEqualize equalizeArg("equalize");
  MagickArgFilter filterArg("filter", "filter");
  MagickArgFlip flipArg("flip");
  MagickArgFlop flopArg("flop");
  MagickArgFont fontArg("font", "string");
  MagickArgFrame frameArg("frame", "geometry");
  MagickArgFuzz fuzzArg("fuzz", "integer");
  MagickArgGamma gammaArg("gamma", "float");
  MagickArgGravity gravityArg("gravity", "gravity");
  MagickArgImpload imploadArg("impload", "float");
  MagickArgInterlace interlaceArg("interlace", "interlace");
  MagickArgLabel labelArg("label", "string");
  MagickArgLayer layerArg("layer", "layer");
  MagickArgLinewidth linewidthArg("linewidth", "integer");
  MagickArgLoop loopArg("loop", "integer");
  MagickArgMap mapArg("map", "string");
  MagickArgMatte matteArg("matte");
  MagickArgMattecolor mattecolorArg("mattecolor", "color");
  MagickArgModulate modulateArg("modulate", "string");
  MagickArgMorph morphArg("morph", "integer");
  MagickArgMonochrome monochromeArg("monochrome");
  MagickArgNegate negateArg("negate");
  MagickArgNoise noiseArg("noise", "noise");
  MagickArgNormalize normalizeArg("normalize");
  MagickArgOpaque opaqueArg("opaque", "color");
  MagickArgPage pageArg("page", "geometry");
  MagickArgPaint paintArg("paint", "integer");
  MagickArgPen penArg("pen", "color");
  MagickArgPing pingArg("ping");
  MagickArgPointsize pointsizeArg("pointsize", "integer");
  MagickArgPreview previewArg("preview", "preview");
  MagickArgProfile profileArg("profile", "string");
  MagickArgQuality qualityArg("quality", "integer");
  MagickArgRaise raiseArg("raise", "geometry");
  MagickArgRegion regionArg("region", "geometry");
  MagickArgRoll rollArg("roll", "geometry");
  MagickArgRotate rotateArg("rotate", "float");
  MagickArgSample sampleArg("sample", "geometry");
  MagickArgScene sceneArg("scene", "integer");
  MagickArgSeed seedArg("seed", "integer");
  MagickArgSegment segmentArg("segment");
  MagickArgShade shadeArg("shade", "float");
  MagickArgSharpen sharpenArg("sharpen", "float");
  MagickArgShear shearArg("shear", "float");
  MagickArgSize sizeArg("size", "geometry");
  MagickArgSolarize solarizeArg("solarize", "integer");
  MagickArgSpread spreadArg("spread", "integer");
  MagickArgSwirl swirlArg("swirl", "float");
  MagickArgTexture textureArg("texture", "string");
  MagickArgThreshold thresholdArg("threshold", "integer");
  MagickArgTransparent transparentArg("transparent", "color");
  MagickArgTreedepth treedepthArg("treedepth", "integer");
  MagickArgUnits unitsArg("units", "resolution");
  MagickArgVerbose verboseArg("verbose");
  MagickArgView viewArg("view", "string");
  MagickArgWave waveArg("wave", "float");
  MagickArgZoom zoomArg("zoom", "geometry");
  CmdArgStrList input("input-file ...", "input files to read.");
  MagickArgStr output("output", "output-file",  "output file to write.");

  // Declare command object and its argument-iterator
  CmdLine  cmd(*argv,
    &adjoinArg,
    &antialiasArg,
    &appendArg,
    &averageArg,
    &backgroundArg,
    &blurArg,
    &borderArg,
    &bordercolorArg,
    &boxcolorArg,
    &charcoalArg,
    &coalesceArg,
    &colorizeArg,
    &colorsArg,
    &colorspaceArg,
    &commentArg,
    &compressArg,
    &contrastArg,
    &cropArg,
    &cycleArg,
    &delayArg,
    &densityArg,
    &depthArg,
    &despeckleArg,
    &displayArg,
    &disposeArg,
    &ditherArg,
    &drawArg,
    &edgeArg,
    &embossArg,
    &enhanceArg,
    &equalizeArg,
    &filterArg,
    &flipArg,
    &flopArg,
    &fontArg,
    &frameArg,
    &fuzzArg,
    &gammaArg,
    &gravityArg,
    &imploadArg,
    &interlaceArg,
    &labelArg,
    &layerArg,
    &linewidthArg,
    &loopArg,
    &mapArg,
    &matteArg,
    &mattecolorArg,
    &modulateArg,
    &morphArg,
    &monochromeArg,
    &negateArg,
    &noiseArg,
    &normalizeArg,
    &opaqueArg,
    &pageArg,
    &paintArg,
    &penArg,
    &pingArg,
    &pointsizeArg,
    &previewArg,
    &profileArg,
    &qualityArg,
    &raiseArg,
    &regionArg,
    &rollArg,
    &rotateArg,
    &sampleArg,
    &sceneArg,
    &seedArg,
    &segmentArg,
    &shadeArg,
    &sharpenArg,
    &shearArg,
    &sizeArg,
    &solarizeArg,
    &spreadArg,
    &swirlArg,
    &textureArg,
    &thresholdArg,
    &transparentArg,
    &treedepthArg,
    &unitsArg,
    &verboseArg,
    &viewArg,
    &waveArg,
    &zoomArg,
    &input,
    &output,
  NULL);
  cmd.flags(CmdLine::ANY_CASE_OPTS | CmdLine::KWDS_ONLY | CmdLine::ALLOW_PLUS);
  CmdArgvIter arg_iter(--argc, ++argv);

  // Parse arguments for simple syntax errors first and spew usage if
  // there are any problems with the values provided.
  cmd.parse(arg_iter);

  // Figure out the number of input images. If the output file was not
  // specified then use the last item in the list of input files as the
  // output file name.
  int imagecount = input.count();
  if (output.isNULL())
  {
    imagecount--;
    output = input[imagecount];
  }

  // Construct list that will contain all of the input images
  list<Image> images;
  Image temp;
  for (int i = 0 ; i < imagecount ; i++) {
    try {
      string filename = input[i];
      temp.read( filename );

      arg_iter.reset(argc, argv);
      cmd.prologue();
      CmdArg *matched = NULL;
      for (const char * arg = arg_iter() ; arg ; arg = arg_iter())
      {
        (void) cmd.parse_arg(arg);
        if (cmd.status() == 0)
        {
          if (isOPTION(arg) || isKEYWORD(arg, 1))
          {
            string str2, str1 = arg;
            str2 = str1.substr(str1.find_first_not_of("-+"));
            str1 = str2.substr(0, str2.find_first_of(":="));
            matched = cmd[ str1.c_str() ];
          }
          if (matched)
          {
            unsigned int sflags = matched->flags();
            unsigned int ssyntax = matched->syntax();
            if ((ssyntax & CmdArg::isVALREQ) && !(sflags & CmdArg::VALGIVEN))
              continue;

            MagickArg *pArg = dynamic_cast<MagickArg *>(matched);
            if (pArg)
            {
              pArg->do_something( temp );
              matched = NULL;
            }
          }
        }
      }
      cmd.epilogue();
      // NOTE: MogrifyImage does some fancy cleanup work like calling
      // "quantize" automatically for you and we need to decide how to
      // handle this, otherwise none of the color quantize stuff will
      // do anything but set various image options. Same thing for the
      // MapImage function.

      images.push_back( temp );
    }
    catch( Exception error_ )
      {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
      }
    catch( exception error_ )
      {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
      }
  }

  string outputname = output;
  list<Image>::iterator theImage;
  int scene = 0;
  for (theImage =  images.begin(); theImage != images.end(); ++theImage)
  {
    (*theImage).scene(scene++);
    (*theImage).write( outputname );
  }

  return 0;
}
