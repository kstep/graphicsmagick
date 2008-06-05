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
    quantum_operator=NoiseGaussianOp;
  else if (LocaleCompare("noise-impulse",option) == 0)
    quantum_operator=NoiseImpulseOp;
  else if (LocaleCompare("noise-laplacian",option) == 0)
    quantum_operator=NoiseLaplacianOp;
  else if (LocaleCompare("noise-multiplicative",option) == 0)
    quantum_operator=NoiseMultiplicativeOp;
  else if (LocaleCompare("noise-poisson",option) == 0)
    quantum_operator=NoisePoissonOp;
  else if (LocaleCompare("noise-uniform",option) == 0)
    quantum_operator=NoiseUniformOp;

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
    case NoiseGaussianOp:
      operator_text="noise-gaussian";
      break;
    case NoiseImpulseOp:
      operator_text="noise-impulse";
      break;
    case NoiseLaplacianOp:
      operator_text="noise-laplacian";
      break;
    case NoiseMultiplicativeOp:
      operator_text="noise-multiplicative";
      break;
    case NoisePoissonOp:
      operator_text="noise-poisson";
      break;
    case NoiseUniformOp:
      operator_text="noise-uniform";
      break;
    }

  return operator_text;
}

