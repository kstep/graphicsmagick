/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                  GGGG  M   M                                %
%                                 G      MM MM                                %
%                                 G GG   M M M                                %
%                                 G   G  M   M                                %
%                                  GGG   M   M                                %
%                                                                             %
%                                                                             %
%                             GraphicsMagick Driver                           %
%                                                                             %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                            Glenn Randers-Pehrson                            %
%                                December 2002                                %
%                              Header Centered By                             %
%                               Bob Friesenhahn                               %
%                                  May 2003                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Gm is a common wrapper around a set of commands, which include animate,
%  composite, conjure, convert, display, identify, import, mogrify, and
%  montage. Please see the manual page gm.1 for detailed usage information.
%
*/

/*
  Include declarations.
*/
#include "magick/api.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(int argc,char **argv)
{
  return GMCommand(argc,argv);
}
