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
#ifndef _MAGICK_OPERATOR_H
#define _MAGICK_OPERATOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Include declarations.
*/
typedef enum
{
  UndefinedQuantumOp = 0,
  AddQuantumOp,
  AndQuantumOp,
  DivideQuantumOp,
  LShiftQuantumOp,
  MultiplyQuantumOp,
  OrQuantumOp,
  RShiftQuantumOp,
  SubtractQuantumOp,
  XorQuantumOp
 } QuantumOperator;

extern MagickExport MagickPassFail
  QuantumOperatorImage(Image *image,const ChannelType channel,
    const QuantumOperator quantum_operator,const double rvalue,
    ExceptionInfo *exception),
  QuantumOperatorRegionImage(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows,
    const ChannelType channel,const QuantumOperator quantum_operator,
    const double rvalue,ExceptionInfo *exception);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
