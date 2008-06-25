/*
% Copyright (C) 2008 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to convert to and from the string form of enumeration values.
%
% Written by Bob Friesenhahn, June 2008.
%
*/

#include "magick/studio.h"
#include "magick/enum_strings.h"
#include "magick/utility.h"

MagickExport const char *ChannelTypeToString(const ChannelType channel)
{
  const char
    *channel_type="undefined";

  switch (channel)
    {
    case UndefinedChannel:
      break;
    case RedChannel:
      channel_type="red";
      break;
    case CyanChannel: 
      channel_type="cyan";
      break;
    case GreenChannel:
      channel_type="green";
      break;
    case MagentaChannel:
      channel_type="magenta";
      break;
    case BlueChannel:
      channel_type="blue";
      break;
    case YellowChannel:
      channel_type="yellow";
      break;
    case OpacityChannel:
      channel_type="opacity";
      break;
    case BlackChannel:
      channel_type="black";
      break;
    case MatteChannel:
      channel_type="matte";
      break;
    case AllChannels:
      channel_type="all";
      break;
    case GrayChannel:
      channel_type="gray";
      break;
    }

  return channel_type;
}

MagickExport ChannelType StringToChannelType(const char *option)
{
  ChannelType
    channel;

  channel=UndefinedChannel;
  if (LocaleCompare("Red",option) == 0)
    channel=RedChannel;
  else if (LocaleCompare("Cyan",option) == 0)
    channel=CyanChannel;
  else if (LocaleCompare("Green",option) == 0)
    channel=GreenChannel;
  else if (LocaleCompare("Magenta",option) == 0)
    channel=MagentaChannel;
  else if (LocaleCompare("Blue",option) == 0)
    channel=BlueChannel;
  else if (LocaleCompare("Yellow",option) == 0)
    channel=YellowChannel;
  else if (LocaleCompare("Opacity",option) == 0)
    channel=OpacityChannel;
  else if (LocaleCompare("Black",option) == 0)
    channel=BlackChannel;
  else if (LocaleCompare("Matte",option) == 0)
    channel=MatteChannel;
  else if (LocaleCompare("All",option) == 0)
    channel=AllChannels;
  else if ((LocaleCompare("Gray",option) == 0) ||
           (LocaleCompare("Intensity",option) == 0))
    channel=GrayChannel;

  return channel;
}

MagickExport const char *CompositeOperatorToString(const CompositeOperator composite_op)
{
  const char
    *composite_op_text = "Undefined";

  switch (composite_op)
    {
    case UndefinedCompositeOp:
      composite_op_text = "Undefined";
      break;
    case OverCompositeOp:
      composite_op_text = "Over";
      break;
    case InCompositeOp:
      composite_op_text = "In";
      break;
    case OutCompositeOp:
      composite_op_text = "Out";
      break;
    case AtopCompositeOp:
      composite_op_text = "Atop";
      break;
    case XorCompositeOp:
      composite_op_text = "Xor";
      break;
    case PlusCompositeOp:
      composite_op_text = "Plus";
      break;
    case MinusCompositeOp:
      composite_op_text = "Minus";
      break;
    case AddCompositeOp:
      composite_op_text = "Add";
      break;
    case SubtractCompositeOp:
      composite_op_text = "Subtract";
      break;
    case DifferenceCompositeOp:
      composite_op_text = "Difference";
      break;
    case MultiplyCompositeOp:
      composite_op_text = "Multiply";
      break;
    case BumpmapCompositeOp:
      composite_op_text = "Bumpmap";
      break;
    case CopyCompositeOp:
      composite_op_text = "Copy";
      break;
    case CopyRedCompositeOp:
      composite_op_text = "CopyRed";
      break;
    case CopyGreenCompositeOp:
      composite_op_text = "CopyGreen";
      break;
    case CopyBlueCompositeOp:
      composite_op_text = "CopyBlue";
      break;
    case CopyOpacityCompositeOp:
      composite_op_text = "CopyOpacity";
      break;
    case ClearCompositeOp:
      composite_op_text = "Clear";
      break;
    case DissolveCompositeOp:
      composite_op_text = "Dissolve";
      break;
    case DisplaceCompositeOp:
      composite_op_text = "Displace";
      break;
    case ModulateCompositeOp:
      composite_op_text = "Modulate";
      break;
    case ThresholdCompositeOp:
      composite_op_text = "Threshold";
      break;
    case NoCompositeOp:
      composite_op_text = "No";
      break;
    case DarkenCompositeOp:
      composite_op_text = "Darken";
      break;
    case LightenCompositeOp:
      composite_op_text = "Lighten";
      break;
    case HueCompositeOp:
      composite_op_text = "Hue";
      break;
    case SaturateCompositeOp:
      composite_op_text = "Saturate";
      break;
    case ColorizeCompositeOp:
      composite_op_text = "Colorize";
      break;
    case LuminizeCompositeOp:
      composite_op_text = "Luminize";
      break;
    case ScreenCompositeOp:
      composite_op_text = "Screen";
      break;
    case OverlayCompositeOp:
      composite_op_text = "Overlay";
      break;
    case CopyCyanCompositeOp:
      composite_op_text = "CopyCyan";
      break;
    case CopyMagentaCompositeOp:
      composite_op_text = "CopyMagenta";
      break;
    case CopyYellowCompositeOp:
      composite_op_text = "CopyYellow";
      break;
    case CopyBlackCompositeOp:
      composite_op_text = "CopyBlack";
      break;
    case DivideCompositeOp:
      composite_op_text = "Divide";
      break;
    }

  return composite_op_text;
}

MagickExport CompositeOperator StringToCompositeOperator(const char *option)
{
  CompositeOperator
    composite_op;

  composite_op=UndefinedCompositeOp;
  if (LocaleCompare("Undefined",option) == 0)
    composite_op=UndefinedCompositeOp;
  else if (LocaleCompare("Over",option) == 0)
    composite_op=OverCompositeOp;
  else if (LocaleCompare("In",option) == 0)
    composite_op=InCompositeOp;
  else if (LocaleCompare("Out",option) == 0)
    composite_op=OutCompositeOp;
  else if (LocaleCompare("Atop",option) == 0)
    composite_op=AtopCompositeOp;
  else if (LocaleCompare("Xor",option) == 0)
    composite_op=XorCompositeOp;
  else if (LocaleCompare("Plus",option) == 0)
    composite_op=PlusCompositeOp;
  else if (LocaleCompare("Minus",option) == 0)
    composite_op=MinusCompositeOp;
  else if (LocaleCompare("Add",option) == 0)
    composite_op=AddCompositeOp;
  else if (LocaleCompare("Subtract",option) == 0)
    composite_op=SubtractCompositeOp;
  else if (LocaleCompare("Difference",option) == 0)
    composite_op=DifferenceCompositeOp;
  else if (LocaleCompare("Multiply",option) == 0)
    composite_op=MultiplyCompositeOp;
  else if (LocaleCompare("Bumpmap",option) == 0)
    composite_op=BumpmapCompositeOp;
  else if (LocaleCompare("Copy",option) == 0)
    composite_op=CopyCompositeOp;
  else if (LocaleCompare("CopyRed",option) == 0)
    composite_op=CopyRedCompositeOp;
  else if (LocaleCompare("CopyGreen",option) == 0)
    composite_op=CopyGreenCompositeOp;
  else if (LocaleCompare("CopyBlue",option) == 0)
    composite_op=CopyBlueCompositeOp;
  else if (LocaleCompare("CopyOpacity",option) == 0)
    composite_op=CopyOpacityCompositeOp;
  else if (LocaleCompare("Clear",option) == 0)
    composite_op=ClearCompositeOp;
  else if (LocaleCompare("Dissolve",option) == 0)
    composite_op=DissolveCompositeOp;
  else if (LocaleCompare("Displace",option) == 0)
    composite_op=DisplaceCompositeOp;
  else if (LocaleCompare("Modulate",option) == 0)
    composite_op=ModulateCompositeOp;
  else if (LocaleCompare("Threshold",option) == 0)
    composite_op=ThresholdCompositeOp;
  else if (LocaleCompare("No",option) == 0)
    composite_op=NoCompositeOp;
  else if (LocaleCompare("Darken",option) == 0)
    composite_op=DarkenCompositeOp;
  else if (LocaleCompare("Lighten",option) == 0)
    composite_op=LightenCompositeOp;
  else if (LocaleCompare("Hue",option) == 0)
    composite_op=HueCompositeOp;
  else if (LocaleCompare("Saturate",option) == 0)
    composite_op=SaturateCompositeOp;
  else if (LocaleCompare("Colorize",option) == 0)
    composite_op=ColorizeCompositeOp;
  else if (LocaleCompare("Luminize",option) == 0)
    composite_op=LuminizeCompositeOp;
  else if (LocaleCompare("Screen",option) == 0)
    composite_op=ScreenCompositeOp;
  else if (LocaleCompare("Overlay",option) == 0)
    composite_op=OverlayCompositeOp;
  else if (LocaleCompare("CopyCyan",option) == 0)
    composite_op=CopyCyanCompositeOp;
  else if (LocaleCompare("CopyMagenta",option) == 0)
    composite_op=CopyMagentaCompositeOp;
  else if (LocaleCompare("CopyYellow",option) == 0)
    composite_op=CopyYellowCompositeOp;
  else if (LocaleCompare("CopyBlack",option) == 0)
    composite_op=CopyBlackCompositeOp;
  else if (LocaleCompare("Divide",option) == 0)
    composite_op=DivideCompositeOp;

  return composite_op;
}

MagickExport QuantumOperator StringToQuantumOperator(const char *option)
{
  QuantumOperator
    quantum_operator;

  quantum_operator=UndefinedQuantumOp;
  if (LocaleCompare("add",option) == 0)
    quantum_operator=AddQuantumOp;
  else if (LocaleCompare("and",option) == 0)
    quantum_operator=AndQuantumOp;
  else if (LocaleCompare("assign",option) == 0)
    quantum_operator=AssignQuantumOp;
  else if (LocaleCompare("divide",option) == 0)
    quantum_operator=DivideQuantumOp;
  else if (LocaleCompare("lshift",option) == 0)
    quantum_operator=LShiftQuantumOp;
  else if (LocaleCompare("multiply",option) == 0)
    quantum_operator=MultiplyQuantumOp;
  else if (LocaleCompare("or",option) == 0)
    quantum_operator=OrQuantumOp;
  else if (LocaleCompare("rshift",option) == 0)
    quantum_operator=RShiftQuantumOp;
  else if (LocaleCompare("subtract",option) == 0)
    quantum_operator=SubtractQuantumOp;
  else if (LocaleCompare("threshold",option) == 0)
    quantum_operator=ThresholdQuantumOp;
  else if (LocaleCompare("threshold-black",option) == 0)
    quantum_operator=ThresholdBlackQuantumOp;
  else if (LocaleCompare("threshold-white",option) == 0)
    quantum_operator=ThresholdWhiteQuantumOp;
  else if (LocaleCompare("xor",option) == 0)
    quantum_operator=XorQuantumOp;
  else if (LocaleCompare("noise-gaussian",option) == 0)
    quantum_operator=NoiseGaussianQuantumOp;
  else if (LocaleCompare("noise-impulse",option) == 0)
    quantum_operator=NoiseImpulseQuantumOp;
  else if (LocaleCompare("noise-laplacian",option) == 0)
    quantum_operator=NoiseLaplacianQuantumOp;
  else if (LocaleCompare("noise-multiplicative",option) == 0)
    quantum_operator=NoiseMultiplicativeQuantumOp;
  else if (LocaleCompare("noise-poisson",option) == 0)
    quantum_operator=NoisePoissonQuantumOp;
  else if (LocaleCompare("noise-uniform",option) == 0)
    quantum_operator=NoiseUniformQuantumOp;
  else if (LocaleCompare("invert",option) == 0)
    quantum_operator=InvertQuantumOp;

  return quantum_operator;
}

MagickExport const char *QuantumOperatorToString(const QuantumOperator quantum_operator)
{
  const char
    *operator_text = "undefined";

  switch (quantum_operator)
    {
    case UndefinedQuantumOp:
      break;
    case AddQuantumOp:
      operator_text="add";
      break;
    case AndQuantumOp:
      operator_text="and";
      break;
    case AssignQuantumOp:
      operator_text="assign";
      break;
    case DivideQuantumOp:
      operator_text="divide";
      break;
    case LShiftQuantumOp:
      operator_text="lshift";
      break;
    case MultiplyQuantumOp:
      operator_text="multiply";
      break;
    case OrQuantumOp:
      operator_text="or";
      break;
    case RShiftQuantumOp:
      operator_text="rshift";
      break;
    case SubtractQuantumOp:
      operator_text="subtract";
      break;
    case ThresholdQuantumOp:
      operator_text="threshold";
      break;
    case ThresholdBlackQuantumOp:
      operator_text="threshold-black";
      break;
    case ThresholdWhiteQuantumOp:
      operator_text="threshold-white";
      break;
    case XorQuantumOp:
      operator_text="xor";
      break;
    case NoiseGaussianQuantumOp:
      operator_text="noise-gaussian";
      break;
    case NoiseImpulseQuantumOp:
      operator_text="noise-impulse";
      break;
    case NoiseLaplacianQuantumOp:
      operator_text="noise-laplacian";
      break;
    case NoiseMultiplicativeQuantumOp:
      operator_text="noise-multiplicative";
      break;
    case NoisePoissonQuantumOp:
      operator_text="noise-poisson";
      break;
    case NoiseUniformQuantumOp:
      operator_text="noise-uniform";
      break;
    case InvertQuantumOp:
      operator_text="invert";
      break;
    }

  return operator_text;
}

