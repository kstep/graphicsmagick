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

#ifndef _ENUM_STRINGS_H
#define _ENUM_STRINGS_H

#include "magick/image.h"
#include "magick/operator.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  MagickExport const char *ChannelTypeToString(const ChannelType channel);

  MagickExport const char *QuantumOperatorToString(const QuantumOperator quantum_operator);

  MagickExport ChannelType StringToChannelType(const char *option);

  MagickExport QuantumOperator StringToQuantumOperator(const char *option);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _ENUM_STRINGS_H */
