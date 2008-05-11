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
#include "magick/pixel_iterator.h"
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
%  QuantumOperatorImage() performs the requested integer arithmetic
%  operation on the selected channels of the image.
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
%    o quantum_operator: arithmetic or bitwise operator to use (AddQuantumOp,
%                        AndQuantumOp, DivideQuantumOp, LShiftQuantumOp,
%                        MultiplyQuantumOp, OrQuantumOp, RShiftQuantumOp,
%                        SubtractQuantumOp, XorQuantumOp).
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
%  QuantumOperatorRegionImage() performs the requested integer arithmetic
%  operation on the selected channels of the image over the specified region.
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
%    o channel: Channel to operate on.
%
%    o x: Ordinate of left row of region.
%
%    o y: Orginate of top column of region.
%
%    o columns: Width of region.
%
%    o rows: Height of region.
%
%    o quantum_operator: arithmetic or bitwise operator to use (AddQuantumOp,
%                        AndQuantumOp, DivideQuantumOp, LShiftQuantumOp,
%                        MultiplyQuantumOp, OrQuantumOp, RShiftQuantumOp,
%                        SubtractQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/

#define ApplyArithmeticOperator(lvalue,op,rvalue)       \
  {                                                     \
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
           const long ARGUNUSED(x),
           const long ARGUNUSED(y),
           const Image *const_image,
           PixelPacket *pixel,
           ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,+,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,+,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,+,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,+,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,+,context->double_value);
      ApplyArithmeticOperator(pixel->green,+,context->double_value);
      ApplyArithmeticOperator(pixel->blue,+,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,+,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAnd(void *user_data,
           const long ARGUNUSED(x),
           const long ARGUNUSED(y),
           const Image *const_image,
           PixelPacket *pixel,
           ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red &= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green &= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue &= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity &= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red &= context->quantum_value;
      pixel->green &= context->quantum_value;
      pixel->blue &= context->quantum_value;
      if (const_image->matte)
        pixel->opacity &= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumDivide(void *user_data,
              const long ARGUNUSED(x),
              const long ARGUNUSED(y),
              const Image *const_image,
              PixelPacket *pixel,
              ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,/,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,/,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,/,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,/,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,/,context->double_value);
      ApplyArithmeticOperator(pixel->green,/,context->double_value);
      ApplyArithmeticOperator(pixel->blue,/,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,/,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumLShift(void *user_data,
              const long ARGUNUSED(x),
              const long ARGUNUSED(y),
              const Image *const_image,
              PixelPacket *pixel,
              ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red <<= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green <<= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue <<= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity <<= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red <<= context->quantum_value;
      pixel->green <<= context->quantum_value;
      pixel->blue <<= context->quantum_value;
      if (const_image->matte)
        pixel->opacity <<= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMultiply(void *user_data,
                const long ARGUNUSED(x),
                const long ARGUNUSED(y),
                const Image *const_image,
                PixelPacket *pixel,
                ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,*,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,*,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,*,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,*,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,*,context->double_value);
      ApplyArithmeticOperator(pixel->green,*,context->double_value);
      ApplyArithmeticOperator(pixel->blue,*,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,*,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumOr(void *user_data,
          const long ARGUNUSED(x),
          const long ARGUNUSED(y),
          const Image *const_image,
          PixelPacket *pixel,
          ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red |= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green |= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue |= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity |= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red |= context->quantum_value;
      pixel->green |= context->quantum_value;
      pixel->blue |= context->quantum_value;
      if (const_image->matte)
        pixel->opacity |= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumRShift(void *user_data,
              const long ARGUNUSED(x),
              const long ARGUNUSED(y),
              const Image *const_image,
              PixelPacket *pixel,
              ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red >>= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green >>= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue >>= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity >>= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red >>= context->quantum_value;
      pixel->green >>= context->quantum_value;
      pixel->blue >>= context->quantum_value;
      if (const_image->matte)
        pixel->opacity >>= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumSubtract(void *user_data,const long ARGUNUSED(x),
  const long ARGUNUSED(y),const Image *const_image,PixelPacket *pixel,
  ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,-,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,-,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,-,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,-,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,-,context->double_value);
      ApplyArithmeticOperator(pixel->green,-,context->double_value);
      ApplyArithmeticOperator(pixel->blue,-,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,-,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumXor(void *user_data,const long ARGUNUSED(x),const long ARGUNUSED(y),
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *ARGUNUSED(exception))
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red ^= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green ^= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue ^= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity ^= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red ^= context->quantum_value;
      pixel->green ^= context->quantum_value;
      pixel->blue ^= context->quantum_value;
      if (const_image->matte)
        pixel->opacity ^= context->quantum_value;
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

  PixelIteratorMonoModifyCallback
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
    case XorQuantumOp:
      call_back=QuantumXor;
      operator_text="xor";
      break;
    }

  if (call_back)
    {
      FormatString(description,"Apply operator '%s %g' to channel '%s' ...",
                   operator_text,rvalue,ChannelTypeToString(channel));
      status=PixelIterateMonoModify(call_back,
                                    description,
                                    (void *) &context,x,y,columns,rows,
                                    image,exception);
    }
  return (status);
}
