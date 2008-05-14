/*
% Copyright (C) 2004 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to support quantum operators.
% Written by Bob Friesenhahn, March 2004.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/pixel_row_iterator.h"
#include "magick/utility.h"
#include "magick/operator.h"

static const char *ChannelTypeToString(const ChannelType channel)
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
    }

  return channel_type;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorImage() performs the requested arithmetic,
%  bitwise-logical, or value operation on the selected channels of
%  the entire image.
%
%  These operations are on the DirectClass pixels of the image and do not
%  update pixel indexes or colormap.
%
%  The format of the QuantumOperatorImage method is:
%
%      MagickPassFail QuantumOperatorImage(Image *image,
%        ChannelType channel, QuantumOperator operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on.
%
%    o quantum_operator: Operator to use (AddQuantumOp, AndQuantumOp,
%        AssignQuantumOp, DivideQuantumOp, LShiftQuantumOp, MultiplyQuantumOp,
%        OrQuantumOp, RShiftQuantumOp, SubtractQuantumOp, ThresholdQuantumOp,
%        ThresholdBlackQuantumOp, ThresholdWhiteQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/
MagickExport MagickPassFail QuantumOperatorImage(Image *image,
  const ChannelType channel,const QuantumOperator quantum_operator,
  const double rvalue,ExceptionInfo *exception)
{
  return QuantumOperatorRegionImage(image,0,0,image->columns,image->rows,
                                    channel,quantum_operator,rvalue,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r R e g i o n I m a g e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorRegionImage() performs the requested arithmetic,
%  bitwise-logical, or value operation on the selected channels of
%  the image over the specified region.
%
%  These operations are on the DirectClass pixels of the image and do not
%  update pixel indexes or colormap.
%
%  The format of the QuantumOperatorRegionImage method is:
%
%      MagickPassFail QuantumOperatorRegionImage(Image *image,
%        long x, long y, unsigned long columns, unsigned long rows,
%        ChannelType channel, QuantumOperator quantum_operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on.  The AllChannels type only updates
%        color channels.
%
%    o x: Ordinate of left row of region.
%
%    o y: Orginate of top column of region.
%
%    o columns: Width of region.
%
%    o rows: Height of region.
%
%    o quantum_operator: Operator to use (AddQuantumOp, AndQuantumOp,
%        AssignQuantumOp, DivideQuantumOp, LShiftQuantumOp, MultiplyQuantumOp,
%        OrQuantumOp, RShiftQuantumOp, SubtractQuantumOp, ThresholdQuantumOp,
%        ThresholdBlackQuantumOp, ThresholdWhiteQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/

#define ApplyArithmeticOperator(lvalue,op,rvalue)       \
{                                                       \
    double                                              \
      result;                                           \
                                                        \
    result=(double) lvalue op (double) rvalue;          \
    lvalue=RoundSignedToQuantum(result);                \
}

typedef struct _QuantumContext
{
  ChannelType channel;
  Quantum quantum_value;
  double double_value;
} QuantumContext;

static MagickPassFail
QuantumAdd(void *user_data,
           const long x,
           const long y,
           Image *image,
           PixelPacket *pixels,
           const long npixels,
           ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  register long
    i;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,+,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,+,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,+,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,+,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,+,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,+,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,+,context->double_value);
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAnd(void *user_data,
           const long x,
           const long y,
           Image *image,
           PixelPacket *pixels,
           const long npixels,
           ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red &= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green &= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue &= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity &= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red &= context->quantum_value;
          pixels[i].green &= context->quantum_value;
          pixels[i].blue &= context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAssign(void *user_data,
              const long x,
              const long y,
              Image *image,
              PixelPacket *pixels,
              const long npixels,
              ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red = context->quantum_value;
          pixels[i].green = context->quantum_value;
          pixels[i].blue = context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumDivide(void *user_data,
              const long x,
              const long y,
              Image *image,
              PixelPacket *pixels,
              const long npixels,
              ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,/,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,/,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,/,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,/,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,/,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,/,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,/,context->double_value);
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumLShift(void *user_data,
              const long x,
              const long y,
              Image *image,
              PixelPacket *pixels,
              const long npixels,
              ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red <<= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green <<= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue <<= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity <<= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red <<= context->quantum_value;
          pixels[i].green <<= context->quantum_value;
          pixels[i].blue <<= context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMultiply(void *user_data,
                const long x,
                const long y,
                Image *image,
                PixelPacket *pixels,
                const long npixels,
                ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,*,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,*,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,*,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,*,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,*,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,*,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,*,context->double_value);
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumOr(void *user_data,
          const long x,
          const long y,
          Image *image,
          PixelPacket *pixels,
          const long npixels,
          ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red |= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green |= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue |= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity |= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red |= context->quantum_value;
          pixels[i].green |= context->quantum_value;
          pixels[i].blue |= context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumRShift(void *user_data,
              const long x,
              const long y,
              Image *image,
              PixelPacket *pixels,
              const long npixels,
              ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red >>= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green >>= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue >>= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity >>= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red >>= context->quantum_value;
          pixels[i].green >>= context->quantum_value;
          pixels[i].blue >>= context->quantum_value;
        }
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumSubtract(void *user_data,
                const long x,
                const long y,
                Image *image,
                PixelPacket *pixels,
                const long npixels,
                ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].red,-,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].green,-,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].blue,-,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        ApplyArithmeticOperator(pixels[i].opacity,-,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          ApplyArithmeticOperator(pixels[i].red,-,context->double_value);
          ApplyArithmeticOperator(pixels[i].green,-,context->double_value);
          ApplyArithmeticOperator(pixels[i].blue,-,context->double_value);
        }
      break;
    }

  return (MagickPass);
}

static inline Quantum ApplyThresholdOperator(const Quantum quantum,
                                             const Quantum threshold)
{
  Quantum
    result;

  if (quantum > threshold)
    result=MaxRGB;
  else
    result=0U;

  return result;
}
static MagickPassFail
QuantumThreshold(void *user_data,
                const long x,
                const long y,
                Image *image,
                PixelPacket *pixels,
                const long npixels,
                ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red   = ApplyThresholdOperator(pixels[i].red,context->quantum_value);
          pixels[i].green = ApplyThresholdOperator(pixels[i].green,context->quantum_value);
          pixels[i].blue  = ApplyThresholdOperator(pixels[i].blue,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static inline Quantum ApplyThresholdBlackOperator(const Quantum quantum,
                                                  const Quantum threshold)
{
  Quantum
    result;

  if (quantum < threshold)
    result=0U;
  else
    result=quantum;

  return result;
}
static MagickPassFail
QuantumThresholdBlack(void *user_data,
                      const long x,
                      const long y,
                      Image *image,
                      PixelPacket *pixels,
                      const long npixels,
                      ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdBlackOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdBlackOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdBlackOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdBlackOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red   = ApplyThresholdBlackOperator(pixels[i].red,context->quantum_value);
          pixels[i].green = ApplyThresholdBlackOperator(pixels[i].green,context->quantum_value);
          pixels[i].blue  = ApplyThresholdBlackOperator(pixels[i].blue,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static inline Quantum ApplyThresholdWhiteOperator(const Quantum quantum,
                                                  const Quantum threshold)
{
  Quantum
    result;

  if (quantum > threshold)
    result=MaxRGB;
  else
    result=quantum;

  return result;
}
static MagickPassFail
QuantumThresholdWhite(void *user_data,
                      const long x,
                      const long y,
                      Image *image,
                      PixelPacket *pixels,
                      const long npixels,
                      ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red = ApplyThresholdWhiteOperator(pixels[i].red,context->quantum_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green = ApplyThresholdWhiteOperator(pixels[i].green,context->quantum_value);
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue = ApplyThresholdWhiteOperator(pixels[i].blue,context->quantum_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity = ApplyThresholdWhiteOperator(pixels[i].opacity,context->quantum_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red   = ApplyThresholdWhiteOperator(pixels[i].red,context->quantum_value);
          pixels[i].green = ApplyThresholdWhiteOperator(pixels[i].green,context->quantum_value);
          pixels[i].blue  = ApplyThresholdWhiteOperator(pixels[i].blue,context->quantum_value);
        }
      break;
    }
  return (MagickPass);
}

static MagickPassFail
QuantumXor(void *user_data,
           const long x,
           const long y,
           Image *image,
           PixelPacket *pixels,
           const long npixels,
           ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  register long
    i;

  ARG_NOT_USED(x);
  ARG_NOT_USED(y);
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      for (i=0; i < npixels; i++)
        pixels[i].red ^= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      for (i=0; i < npixels; i++)
        pixels[i].green ^= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      for (i=0; i < npixels; i++)
        pixels[i].blue ^= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      for (i=0; i < npixels; i++)
        pixels[i].opacity ^= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      for (i=0; i < npixels; i++)
        {
          pixels[i].red ^= context->quantum_value;
          pixels[i].green ^= context->quantum_value;
          pixels[i].blue ^= context->quantum_value;
        }
      break;
    }
  return (MagickPass);
}
MagickExport MagickPassFail
QuantumOperatorRegionImage(Image *image,
                           const long x,const long y,
                           const unsigned long columns,
                           const unsigned long rows,
                           const ChannelType channel,
                           const QuantumOperator quantum_operator,
                           const double rvalue,
                           ExceptionInfo *exception)
{
  const char
    *operator_text = "undefined";

  char
    description[MaxTextExtent];

  QuantumContext
    context;

  MagickPassFail
    status = MagickFail;

  PixelRowIteratorMonoModifyCallback
    call_back = 0;

  image->storage_class=DirectClass;

  context.channel=channel;
  context.double_value=rvalue;
  context.quantum_value=RoundSignedToQuantum(rvalue);

  switch (quantum_operator)
    {
    case UndefinedQuantumOp:
      break;
    case AddQuantumOp:
      call_back=QuantumAdd;
      operator_text="add";
      break;
    case AndQuantumOp:
      call_back=QuantumAnd;
      operator_text="and";
      break;
    case AssignQuantumOp:
      call_back=QuantumAssign;
      operator_text="assign";
      break;
    case DivideQuantumOp:
      call_back=QuantumDivide;
      operator_text="divide";
      break;
    case LShiftQuantumOp:
      call_back=QuantumLShift;
      operator_text="lshift";
      break;
    case MultiplyQuantumOp:
      call_back=QuantumMultiply;
      operator_text="multiply";
      break;
    case OrQuantumOp:
      call_back=QuantumOr;
      operator_text="or";
      break;
    case RShiftQuantumOp:
      call_back=QuantumRShift;
      operator_text="rshift";
      break;
    case SubtractQuantumOp:
      call_back=QuantumSubtract;
      operator_text="subtract";
      break;
    case ThresholdQuantumOp:
      call_back=QuantumThreshold;
      operator_text="threshold";
      break;
    case ThresholdBlackQuantumOp:
      call_back=QuantumThresholdBlack;
      operator_text="threshold-black";
      break;
    case ThresholdWhiteQuantumOp:
      call_back=QuantumThresholdWhite;
      operator_text="threshold-white";
      break;
    case XorQuantumOp:
      call_back=QuantumXor;
      operator_text="xor";
      break;
    }

  if (call_back)
    {
      FormatString(description,"Apply operator '%s %g' to channel '%s' ...",
                   operator_text,rvalue,ChannelTypeToString(channel));
      status=PixelRowIterateMonoModify(call_back,
                                       description,
                                       (void *) &context,x,y,columns,rows,
                                       image,exception);

      /*
        If we are assigning all the color channels in the entire image
        then set monochrome and grayscale flags.
      */
      if ((quantum_operator == AssignQuantumOp) &&
          (channel == AllChannels) && (x == 0) && (y == 0) &&
          (columns == image->columns) && (rows == image->rows))
        {
          image->is_monochrome=MagickTrue;
          image->is_grayscale=MagickTrue;
        }
    }
  return (status);
}
