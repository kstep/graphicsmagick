/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  CCCC   OOO   L       OOO   RRRR   SSSSS                    %
%                 C      O   O  L      O   O  R   R  SS                       %
%                 C      O   O  L      O   O  RRRR    SSS                     %
%                 C      O   O  L      O   O  R R       SS                    %
%                  CCCC   OOO   LLLLL   OOO   R  R   SSSSS                    %
%                                                                             %
%                                                                             %
%                  methods to Count the Colors in an image                    %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 imagemagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("imagemagick"),  %
%  to deal in imagemagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of imagemagick, and to permit persons to whom the       %
%  imagemagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  the above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of imagemagick.                         %
%                                                                             %
%  the software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  in no event shall   %
%  imagemagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with imagemagick or the use or other dealings in          %
%  imagemagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the imagemagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in imagemagick without prior written authorization from the %
%  imagemagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  include declarations.
*/
#include "magick.h"
#include "defines.h"
#if defined(HasX11)
#include "xwindows.h"
#endif

/*
  Structures.
*/
typedef struct _ColorlistInfo
{
  char
    *name;

  Quantum
    red,
    green,
    blue,
    opacity;
} ColorlistInfo;

typedef struct _ColorPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned short
    index;

  unsigned long
    count;
} ColorPacket;

typedef struct _NodeInfo
{
  unsigned char
    level;

  unsigned long
    number_unique;

  ColorPacket
    *list;

  struct _NodeInfo
    *child[8];
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned int
    progress;

  unsigned long
    colors;

  unsigned int
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_list;
} CubeInfo;

/*
  Color list.
*/
static const ColorlistInfo
  Colorlist[251] =
  {
    { (char *) "aliceblue", UpScale(240), UpScale(248), UpScale(255), OpaqueOpacity },
    { (char *) "antiquewhite", UpScale(250), UpScale(235), UpScale(215), OpaqueOpacity },
    { (char *) "aqua", UpScale(0), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "aquamarine", UpScale(127), UpScale(255), UpScale(212), OpaqueOpacity },
    { (char *) "azure", UpScale(240), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "beige", UpScale(245), UpScale(245), UpScale(220), OpaqueOpacity },
    { (char *) "bisque", UpScale(255), UpScale(228), UpScale(196), OpaqueOpacity },
    { (char *) "black", UpScale(0), UpScale(0), UpScale(0), OpaqueOpacity },
    { (char *) "blanchedalmond", UpScale(255), UpScale(235), UpScale(205), OpaqueOpacity },
    { (char *) "blue", UpScale(0), UpScale(0), UpScale(255), OpaqueOpacity },
    { (char *) "blueviolet", UpScale(138), UpScale(43), UpScale(226), OpaqueOpacity },
    { (char *) "brown", UpScale(165), UpScale(42), UpScale(42), OpaqueOpacity },
    { (char *) "burlywood", UpScale(222), UpScale(184), UpScale(135), OpaqueOpacity },
    { (char *) "cadetblue", UpScale(95), UpScale(158), UpScale(160), OpaqueOpacity },
    { (char *) "chartreuse", UpScale(127), UpScale(255), UpScale(0), OpaqueOpacity },
    { (char *) "chocolate", UpScale(210), UpScale(105), UpScale(30), OpaqueOpacity },
    { (char *) "coral", UpScale(255), UpScale(127), UpScale(80), OpaqueOpacity },
    { (char *) "cornflowerblue", UpScale(100), UpScale(149), UpScale(237), OpaqueOpacity },
    { (char *) "cornsilk", UpScale(255), UpScale(248), UpScale(220), OpaqueOpacity },
    { (char *) "crimson", UpScale(220), UpScale(20), UpScale(60), OpaqueOpacity },
    { (char *) "cyan", UpScale(0), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "darkblue", UpScale(0), UpScale(0), UpScale(139), OpaqueOpacity },
    { (char *) "darkcyan", UpScale(0), UpScale(139), UpScale(139), OpaqueOpacity },
    { (char *) "darkgoldenrod", UpScale(184), UpScale(134), UpScale(11), OpaqueOpacity },
    { (char *) "darkgray", UpScale(169), UpScale(169), UpScale(169), OpaqueOpacity },
    { (char *) "darkgreen", UpScale(0), UpScale(100), UpScale(0), OpaqueOpacity },
    { (char *) "darkgrey", UpScale(169), UpScale(169), UpScale(169), OpaqueOpacity },
    { (char *) "darkkhaki", UpScale(189), UpScale(183), UpScale(107), OpaqueOpacity },
    { (char *) "darkmagenta", UpScale(139), UpScale(0), UpScale(139), OpaqueOpacity },
    { (char *) "darkolivegreen", UpScale(85), UpScale(107), UpScale(47), OpaqueOpacity },
    { (char *) "darkorange", UpScale(255), UpScale(140), UpScale(0), OpaqueOpacity },
    { (char *) "darkorchid", UpScale(153), UpScale(50), UpScale(204), OpaqueOpacity },
    { (char *) "darkred", UpScale(139), UpScale(0), UpScale(0), OpaqueOpacity },
    { (char *) "darksalmon", UpScale(233), UpScale(150), UpScale(122), OpaqueOpacity },
    { (char *) "darkseagreen", UpScale(143), UpScale(188), UpScale(143), OpaqueOpacity },
    { (char *) "darkslateblue", UpScale(72), UpScale(61), UpScale(139), OpaqueOpacity },
    { (char *) "darkslategray", UpScale(47), UpScale(79), UpScale(79), OpaqueOpacity },
    { (char *) "darkslategrey", UpScale(47), UpScale(79), UpScale(79), OpaqueOpacity },
    { (char *) "darkturquoise", UpScale(0), UpScale(206), UpScale(209), OpaqueOpacity },
    { (char *) "darkviolet", UpScale(148), UpScale(0), UpScale(211), OpaqueOpacity },
    { (char *) "deeppink", UpScale(255), UpScale(20), UpScale(147), OpaqueOpacity },
    { (char *) "deepskyblue", UpScale(0), UpScale(191), UpScale(255), OpaqueOpacity },
    { (char *) "dimgray", UpScale(105), UpScale(105), UpScale(105), OpaqueOpacity },
    { (char *) "dimgrey", UpScale(105), UpScale(105), UpScale(105), OpaqueOpacity },
    { (char *) "dodgerblue", UpScale(30), UpScale(144), UpScale(255), OpaqueOpacity },
    { (char *) "firebrick", UpScale(178), UpScale(34), UpScale(34), OpaqueOpacity },
    { (char *) "floralwhite", UpScale(255), UpScale(250), UpScale(240), OpaqueOpacity },
    { (char *) "forestgreen", UpScale(34), UpScale(139), UpScale(34), OpaqueOpacity },
    { (char *) "fractal", UpScale(128), UpScale(128), UpScale(128), OpaqueOpacity },
    { (char *) "fuchsia", UpScale(255), UpScale(0), UpScale(255), OpaqueOpacity },
    { (char *) "gainsboro", UpScale(220), UpScale(220), UpScale(220), OpaqueOpacity },
    { (char *) "ghostwhite", UpScale(248), UpScale(248), UpScale(255), OpaqueOpacity },
    { (char *) "gold", UpScale(255), UpScale(215), UpScale(0), OpaqueOpacity },
    { (char *) "goldenrod", UpScale(218), UpScale(165), UpScale(32), OpaqueOpacity },
    { (char *) "gray", UpScale(126), UpScale(126), UpScale(126), OpaqueOpacity },
    { (char *) "gray0", UpScale(0), UpScale(0), UpScale(0), OpaqueOpacity },
    { (char *) "gray1", UpScale(3), UpScale(3), UpScale(3), OpaqueOpacity },
    { (char *) "gray10", UpScale(26), UpScale(26), UpScale(26), OpaqueOpacity },
    { (char *) "gray100", UpScale(255), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "gray11", UpScale(28), UpScale(28), UpScale(28), OpaqueOpacity },
    { (char *) "gray12", UpScale(31), UpScale(31), UpScale(31), OpaqueOpacity },
    { (char *) "gray13", UpScale(33), UpScale(33), UpScale(33), OpaqueOpacity },
    { (char *) "gray14", UpScale(36), UpScale(36), UpScale(36), OpaqueOpacity },
    { (char *) "gray15", UpScale(38), UpScale(38), UpScale(38), OpaqueOpacity },
    { (char *) "gray16", UpScale(41), UpScale(41), UpScale(41), OpaqueOpacity },
    { (char *) "gray17", UpScale(43), UpScale(43), UpScale(43), OpaqueOpacity },
    { (char *) "gray18", UpScale(46), UpScale(46), UpScale(46), OpaqueOpacity },
    { (char *) "gray19", UpScale(48), UpScale(48), UpScale(48), OpaqueOpacity },
    { (char *) "gray2", UpScale(5), UpScale(5), UpScale(5), OpaqueOpacity },
    { (char *) "gray20", UpScale(51), UpScale(51), UpScale(51), OpaqueOpacity },
    { (char *) "gray21", UpScale(54), UpScale(54), UpScale(54), OpaqueOpacity },
    { (char *) "gray22", UpScale(56), UpScale(56), UpScale(56), OpaqueOpacity },
    { (char *) "gray23", UpScale(59), UpScale(59), UpScale(59), OpaqueOpacity },
    { (char *) "gray24", UpScale(61), UpScale(61), UpScale(61), OpaqueOpacity },
    { (char *) "gray25", UpScale(64), UpScale(64), UpScale(64), OpaqueOpacity },
    { (char *) "gray26", UpScale(66), UpScale(66), UpScale(66), OpaqueOpacity },
    { (char *) "gray27", UpScale(69), UpScale(69), UpScale(69), OpaqueOpacity },
    { (char *) "gray28", UpScale(71), UpScale(71), UpScale(71), OpaqueOpacity },
    { (char *) "gray29", UpScale(74), UpScale(74), UpScale(74), OpaqueOpacity },
    { (char *) "gray3", UpScale(8), UpScale(8), UpScale(8), OpaqueOpacity },
    { (char *) "gray30", UpScale(77), UpScale(77), UpScale(77), OpaqueOpacity },
    { (char *) "gray31", UpScale(79), UpScale(79), UpScale(79), OpaqueOpacity },
    { (char *) "gray32", UpScale(82), UpScale(82), UpScale(82), OpaqueOpacity },
    { (char *) "gray33", UpScale(84), UpScale(84), UpScale(84), OpaqueOpacity },
    { (char *) "gray34", UpScale(87), UpScale(87), UpScale(87), OpaqueOpacity },
    { (char *) "gray35", UpScale(89), UpScale(89), UpScale(89), OpaqueOpacity },
    { (char *) "gray36", UpScale(92), UpScale(92), UpScale(92), OpaqueOpacity },
    { (char *) "gray37", UpScale(94), UpScale(94), UpScale(94), OpaqueOpacity },
    { (char *) "gray38", UpScale(97), UpScale(97), UpScale(97), OpaqueOpacity },
    { (char *) "gray39", UpScale(99), UpScale(99), UpScale(99), OpaqueOpacity },
    { (char *) "gray4", UpScale(10), UpScale(10), UpScale(10), OpaqueOpacity },
    { (char *) "gray40", UpScale(102), UpScale(102), UpScale(102), OpaqueOpacity },
    { (char *) "gray41", UpScale(105), UpScale(105), UpScale(105), OpaqueOpacity },
    { (char *) "gray42", UpScale(107), UpScale(107), UpScale(107), OpaqueOpacity },
    { (char *) "gray43", UpScale(110), UpScale(110), UpScale(110), OpaqueOpacity },
    { (char *) "gray44", UpScale(112), UpScale(112), UpScale(112), OpaqueOpacity },
    { (char *) "gray45", UpScale(115), UpScale(115), UpScale(115), OpaqueOpacity },
    { (char *) "gray46", UpScale(117), UpScale(117), UpScale(117), OpaqueOpacity },
    { (char *) "gray47", UpScale(120), UpScale(120), UpScale(120), OpaqueOpacity },
    { (char *) "gray48", UpScale(122), UpScale(122), UpScale(122), OpaqueOpacity },
    { (char *) "gray49", UpScale(125), UpScale(125), UpScale(125), OpaqueOpacity },
    { (char *) "gray5", UpScale(13), UpScale(13), UpScale(13), OpaqueOpacity },
    { (char *) "gray50", UpScale(127), UpScale(127), UpScale(127), OpaqueOpacity },
    { (char *) "gray51", UpScale(130), UpScale(130), UpScale(130), OpaqueOpacity },
    { (char *) "gray52", UpScale(133), UpScale(133), UpScale(133), OpaqueOpacity },
    { (char *) "gray53", UpScale(135), UpScale(135), UpScale(135), OpaqueOpacity },
    { (char *) "gray54", UpScale(138), UpScale(138), UpScale(138), OpaqueOpacity },
    { (char *) "gray55", UpScale(140), UpScale(140), UpScale(140), OpaqueOpacity },
    { (char *) "gray56", UpScale(143), UpScale(143), UpScale(143), OpaqueOpacity },
    { (char *) "gray57", UpScale(145), UpScale(145), UpScale(145), OpaqueOpacity },
    { (char *) "gray58", UpScale(148), UpScale(148), UpScale(148), OpaqueOpacity },
    { (char *) "gray59", UpScale(150), UpScale(150), UpScale(150), OpaqueOpacity },
    { (char *) "gray6", UpScale(15), UpScale(15), UpScale(15), OpaqueOpacity },
    { (char *) "gray60", UpScale(153), UpScale(153), UpScale(153), OpaqueOpacity },
    { (char *) "gray61", UpScale(156), UpScale(156), UpScale(156), OpaqueOpacity },
    { (char *) "gray62", UpScale(158), UpScale(158), UpScale(158), OpaqueOpacity },
    { (char *) "gray63", UpScale(161), UpScale(161), UpScale(161), OpaqueOpacity },
    { (char *) "gray64", UpScale(163), UpScale(163), UpScale(163), OpaqueOpacity },
    { (char *) "gray65", UpScale(166), UpScale(166), UpScale(166), OpaqueOpacity },
    { (char *) "gray66", UpScale(168), UpScale(168), UpScale(168), OpaqueOpacity },
    { (char *) "gray67", UpScale(171), UpScale(171), UpScale(171), OpaqueOpacity },
    { (char *) "gray68", UpScale(173), UpScale(173), UpScale(173), OpaqueOpacity },
    { (char *) "gray69", UpScale(176), UpScale(176), UpScale(176), OpaqueOpacity },
    { (char *) "gray7", UpScale(18), UpScale(18), UpScale(18), OpaqueOpacity },
    { (char *) "gray70", UpScale(179), UpScale(179), UpScale(179), OpaqueOpacity },
    { (char *) "gray71", UpScale(181), UpScale(181), UpScale(181), OpaqueOpacity },
    { (char *) "gray72", UpScale(184), UpScale(184), UpScale(184), OpaqueOpacity },
    { (char *) "gray73", UpScale(186), UpScale(186), UpScale(186), OpaqueOpacity },
    { (char *) "gray74", UpScale(189), UpScale(189), UpScale(189), OpaqueOpacity },
    { (char *) "gray75", UpScale(191), UpScale(191), UpScale(191), OpaqueOpacity },
    { (char *) "gray76", UpScale(194), UpScale(194), UpScale(194), OpaqueOpacity },
    { (char *) "gray77", UpScale(196), UpScale(196), UpScale(196), OpaqueOpacity },
    { (char *) "gray78", UpScale(199), UpScale(199), UpScale(199), OpaqueOpacity },
    { (char *) "gray79", UpScale(201), UpScale(201), UpScale(201), OpaqueOpacity },
    { (char *) "gray8", UpScale(20), UpScale(20), UpScale(20), OpaqueOpacity },
    { (char *) "gray80", UpScale(204), UpScale(204), UpScale(204), OpaqueOpacity },
    { (char *) "gray81", UpScale(207), UpScale(207), UpScale(207), OpaqueOpacity },
    { (char *) "gray82", UpScale(209), UpScale(209), UpScale(209), OpaqueOpacity },
    { (char *) "gray83", UpScale(212), UpScale(212), UpScale(212), OpaqueOpacity },
    { (char *) "gray84", UpScale(214), UpScale(214), UpScale(214), OpaqueOpacity },
    { (char *) "gray85", UpScale(217), UpScale(217), UpScale(217), OpaqueOpacity },
    { (char *) "gray86", UpScale(219), UpScale(219), UpScale(219), OpaqueOpacity },
    { (char *) "gray87", UpScale(222), UpScale(222), UpScale(222), OpaqueOpacity },
    { (char *) "gray88", UpScale(224), UpScale(224), UpScale(224), OpaqueOpacity },
    { (char *) "gray89", UpScale(227), UpScale(227), UpScale(227), OpaqueOpacity },
    { (char *) "gray9", UpScale(23), UpScale(23), UpScale(23), OpaqueOpacity },
    { (char *) "gray90", UpScale(229), UpScale(229), UpScale(229), OpaqueOpacity },
    { (char *) "gray91", UpScale(232), UpScale(232), UpScale(232), OpaqueOpacity },
    { (char *) "gray92", UpScale(235), UpScale(235), UpScale(235), OpaqueOpacity },
    { (char *) "gray93", UpScale(237), UpScale(237), UpScale(237), OpaqueOpacity },
    { (char *) "gray94", UpScale(240), UpScale(240), UpScale(240), OpaqueOpacity },
    { (char *) "gray95", UpScale(242), UpScale(242), UpScale(242), OpaqueOpacity },
    { (char *) "gray96", UpScale(245), UpScale(245), UpScale(245), OpaqueOpacity },
    { (char *) "gray97", UpScale(247), UpScale(247), UpScale(247), OpaqueOpacity },
    { (char *) "gray98", UpScale(250), UpScale(250), UpScale(250), OpaqueOpacity },
    { (char *) "gray99", UpScale(252), UpScale(252), UpScale(252), OpaqueOpacity },
    { (char *) "green", UpScale(0), UpScale(128), UpScale(0), OpaqueOpacity },
    { (char *) "greenyellow", UpScale(173), UpScale(255), UpScale(47), OpaqueOpacity },
    { (char *) "grey", UpScale(128), UpScale(128), UpScale(128), OpaqueOpacity },
    { (char *) "honeydew", UpScale(240), UpScale(255), UpScale(240), OpaqueOpacity },
    { (char *) "hotpink", UpScale(255), UpScale(105), UpScale(180), OpaqueOpacity },
    { (char *) "indianred", UpScale(205), UpScale(92), UpScale(92), OpaqueOpacity },
    { (char *) "indigo", UpScale(75), UpScale(0), UpScale(130), OpaqueOpacity },
    { (char *) "ivory", UpScale(255), UpScale(255), UpScale(240), OpaqueOpacity },
    { (char *) "khaki", UpScale(240), UpScale(230), UpScale(140), OpaqueOpacity },
    { (char *) "lavender", UpScale(230), UpScale(230), UpScale(250), OpaqueOpacity },
    { (char *) "lavenderblush", UpScale(255), UpScale(240), UpScale(245), OpaqueOpacity },
    { (char *) "lawngreen", UpScale(124), UpScale(252), UpScale(0), OpaqueOpacity },
    { (char *) "lemonchiffon", UpScale(255), UpScale(250), UpScale(205), OpaqueOpacity },
    { (char *) "lightblue", UpScale(173), UpScale(216), UpScale(230), OpaqueOpacity },
    { (char *) "lightcoral", UpScale(240), UpScale(128), UpScale(128), OpaqueOpacity },
    { (char *) "lightcyan", UpScale(224), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "lightgoldenrodyellow", UpScale(250), UpScale(250), UpScale(210), OpaqueOpacity },
    { (char *) "lightgray", UpScale(211), UpScale(211), UpScale(211), OpaqueOpacity },
    { (char *) "lightgreen", UpScale(144), UpScale(238), UpScale(144), OpaqueOpacity },
    { (char *) "lightgrey", UpScale(211), UpScale(211), UpScale(211), OpaqueOpacity },
    { (char *) "lightpink", UpScale(255), UpScale(182), UpScale(193), OpaqueOpacity },
    { (char *) "lightsalmon", UpScale(255), UpScale(160), UpScale(122), OpaqueOpacity },
    { (char *) "lightseagreen", UpScale(32), UpScale(178), UpScale(170), OpaqueOpacity },
    { (char *) "lightskyblue", UpScale(135), UpScale(206), UpScale(250), OpaqueOpacity },
    { (char *) "lightslategray", UpScale(119), UpScale(136), UpScale(153), OpaqueOpacity },
    { (char *) "lightslategrey", UpScale(119), UpScale(136), UpScale(153), OpaqueOpacity },
    { (char *) "lightsteelblue", UpScale(176), UpScale(196), UpScale(222), OpaqueOpacity },
    { (char *) "lightyellow", UpScale(255), UpScale(255), UpScale(224), OpaqueOpacity },
    { (char *) "lime", UpScale(0), UpScale(255), UpScale(0), OpaqueOpacity },
    { (char *) "limegreen", UpScale(50), UpScale(205), UpScale(50), OpaqueOpacity },
    { (char *) "linen", UpScale(250), UpScale(240), UpScale(230), OpaqueOpacity },
    { (char *) "magenta", UpScale(255), UpScale(0), UpScale(255), OpaqueOpacity },
    { (char *) "maroon", UpScale(128), UpScale(0), UpScale(0), OpaqueOpacity },
    { (char *) "mediumaquamarine", UpScale(102), UpScale(205), UpScale(170), OpaqueOpacity },
    { (char *) "mediumblue", UpScale(0), UpScale(0), UpScale(205), OpaqueOpacity },
    { (char *) "mediumorchid", UpScale(186), UpScale(85), UpScale(211), OpaqueOpacity },
    { (char *) "mediumpurple", UpScale(147), UpScale(112), UpScale(219), OpaqueOpacity },
    { (char *) "mediumseagreen", UpScale(60), UpScale(179), UpScale(113), OpaqueOpacity },
    { (char *) "mediumslateblue", UpScale(123), UpScale(104), UpScale(238), OpaqueOpacity },
    { (char *) "mediumspringgreen", UpScale(0), UpScale(250), UpScale(154), OpaqueOpacity },
    { (char *) "mediumturquoise", UpScale(72), UpScale(209), UpScale(204), OpaqueOpacity },
    { (char *) "mediumvioletred", UpScale(199), UpScale(21), UpScale(133), OpaqueOpacity },
    { (char *) "midnightblue", UpScale(25), UpScale(25), UpScale(112), OpaqueOpacity },
    { (char *) "mintcream", UpScale(245), UpScale(255), UpScale(250), OpaqueOpacity },
    { (char *) "mistyrose", UpScale(255), UpScale(228), UpScale(225), OpaqueOpacity },
    { (char *) "moccasin", UpScale(255), UpScale(228), UpScale(181), OpaqueOpacity },
    { (char *) "navajowhite", UpScale(255), UpScale(222), UpScale(173), OpaqueOpacity },
    { (char *) "navy", UpScale(0), UpScale(0), UpScale(128), OpaqueOpacity },
    { (char *) "none", UpScale(0), UpScale(0), UpScale(0), TransparentOpacity },
    { (char *) "oldlace", UpScale(253), UpScale(245), UpScale(230), OpaqueOpacity },
    { (char *) "olive", UpScale(128), UpScale(128), UpScale(0), OpaqueOpacity },
    { (char *) "olivedrab", UpScale(107), UpScale(142), UpScale(35), OpaqueOpacity },
    { (char *) "orange", UpScale(255), UpScale(165), UpScale(0), OpaqueOpacity },
    { (char *) "orangered", UpScale(255), UpScale(69), UpScale(0), OpaqueOpacity },
    { (char *) "orchid", UpScale(218), UpScale(112), UpScale(214), OpaqueOpacity },
    { (char *) "palegoldenrod", UpScale(238), UpScale(232), UpScale(170), OpaqueOpacity },
    { (char *) "palegreen", UpScale(152), UpScale(251), UpScale(152), OpaqueOpacity },
    { (char *) "paleturquoise", UpScale(175), UpScale(238), UpScale(238), OpaqueOpacity },
    { (char *) "palevioletred", UpScale(219), UpScale(112), UpScale(147), OpaqueOpacity },
    { (char *) "papayawhip", UpScale(255), UpScale(239), UpScale(213), OpaqueOpacity },
    { (char *) "peachpuff", UpScale(255), UpScale(218), UpScale(185), OpaqueOpacity },
    { (char *) "peru", UpScale(205), UpScale(133), UpScale(63), OpaqueOpacity },
    { (char *) "pink", UpScale(255), UpScale(192), UpScale(203), OpaqueOpacity },
    { (char *) "plum", UpScale(221), UpScale(160), UpScale(221), OpaqueOpacity },
    { (char *) "powderblue", UpScale(176), UpScale(224), UpScale(230), OpaqueOpacity },
    { (char *) "purple", UpScale(128), UpScale(0), UpScale(128), OpaqueOpacity },
    { (char *) "red", UpScale(255), UpScale(0), UpScale(0), OpaqueOpacity },
    { (char *) "rosybrown", UpScale(188), UpScale(143), UpScale(143), OpaqueOpacity },
    { (char *) "royalblue", UpScale(65), UpScale(105), UpScale(225), OpaqueOpacity },
    { (char *) "saddlebrown", UpScale(139), UpScale(69), UpScale(19), OpaqueOpacity },
    { (char *) "salmon", UpScale(250), UpScale(128), UpScale(114), OpaqueOpacity },
    { (char *) "sandybrown", UpScale(244), UpScale(164), UpScale(96), OpaqueOpacity },
    { (char *) "seagreen", UpScale(46), UpScale(139), UpScale(87), OpaqueOpacity },
    { (char *) "seashell", UpScale(255), UpScale(245), UpScale(238), OpaqueOpacity },
    { (char *) "sienna", UpScale(160), UpScale(82), UpScale(45), OpaqueOpacity },
    { (char *) "silver", UpScale(192), UpScale(192), UpScale(192), OpaqueOpacity },
    { (char *) "skyblue", UpScale(135), UpScale(206), UpScale(235), OpaqueOpacity },
    { (char *) "slateblue", UpScale(106), UpScale(90), UpScale(205), OpaqueOpacity },
    { (char *) "slategray", UpScale(112), UpScale(128), UpScale(144), OpaqueOpacity },
    { (char *) "slategrey", UpScale(112), UpScale(128), UpScale(144), OpaqueOpacity },
    { (char *) "snow", UpScale(255), UpScale(250), UpScale(250), OpaqueOpacity },
    { (char *) "springgreen", UpScale(0), UpScale(255), UpScale(127), OpaqueOpacity },
    { (char *) "steelblue", UpScale(70), UpScale(130), UpScale(180), OpaqueOpacity },
    { (char *) "tan", UpScale(210), UpScale(180), UpScale(140), OpaqueOpacity },
    { (char *) "teal", UpScale(0), UpScale(128), UpScale(128), OpaqueOpacity },
    { (char *) "thistle", UpScale(216), UpScale(191), UpScale(216), OpaqueOpacity },
    { (char *) "tomato", UpScale(255), UpScale(99), UpScale(71), OpaqueOpacity },
    { (char *) "turquoise", UpScale(64), UpScale(224), UpScale(208), OpaqueOpacity },
    { (char *) "violet", UpScale(238), UpScale(130), UpScale(238), OpaqueOpacity },
    { (char *) "wheat", UpScale(245), UpScale(222), UpScale(179), OpaqueOpacity },
    { (char *) "white", UpScale(255), UpScale(255), UpScale(255), OpaqueOpacity },
    { (char *) "whitesmoke", UpScale(245), UpScale(245), UpScale(245), OpaqueOpacity },
    { (char *) "yellow", UpScale(255), UpScale(255), UpScale(0), OpaqueOpacity },
    { (char *) "yellowgreen", UpScale(154), UpScale(205), UpScale(50), OpaqueOpacity },
    { (char *) NULL, 0, 0, 0, 0 }
  };

/*
  Forward declarations.
*/
static void
  Histogram(CubeInfo *,const NodeInfo *,FILE *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s C o l o r m a p                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CompressColormap compresses an image colormap removing any
%  duplicate and unused color entries.
%
%  The format of the CompressColormap method is:
%
%      void CompressColormap(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport void CompressColormap(Image *image)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!IsPseudoClass(image))
    return;
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=image->colors;
  quantize_info.tree_depth=8;
  (void) QuantizeImage(&quantize_info,image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y L i s t                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyList traverses the color cube tree and free the list of
%  unique colors.
%
%  The format of the DestroyList method is:
%
%      void DestroyList(const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyList(const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyList(node_info->child[id]);
  if (node_info->list != (ColorPacket *) NULL)
    LiberateMemory((void **) &node_info->list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t N o d e I n f o                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNodeInfo allocates memory for a new node in the color cube tree
%  and presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      node_info=GetNodeInfo(color_cube,level)
%
%  A description of each parameter follows.
%
%    o color_cube: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *color_cube,const unsigned int level)
{
  NodeInfo
    *node_info;

  if (color_cube->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AcquireMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=color_cube->node_list;
      color_cube->node_list=nodes;
      color_cube->node_info=nodes->nodes;
      color_cube->free_nodes=NodesInAList;
    }
  color_cube->free_nodes--;
  node_info=color_cube->node_info++;
  memset(node_info,0,sizeof(NodeInfo));
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t N u m b e r C o l o r s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberColors returns the number of unique colors in an image.
%
%  The format of the GetNumberColors method is:
%
%      number_colors=GetNumberColors(image,file)
%
%  A description of each parameter follows.
%
%    o number_colors: Method GetNumberColors returns the number of unique
%      colors in the specified image.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your source image.  The sum of the
%      run-length counts in the source image must be equal to or exceed
%      the number of pixels.
%
%    o file:  An pointer to a FILE.  If it is non-null a list of unique pixel
%      field values and the number of times each occurs in the image is
%      written to the file.
%
%
%
*/
MagickExport unsigned long GetNumberColors(Image *image,FILE *file)
{
#define NumberColorsImageText  "  Compute image colors...  "

  CubeInfo
    color_cube;

  int
    y;

  NodeInfo
    *node_info;

  Nodes
    *nodes;

  register int
    i,
    x;

  register PixelPacket
    *p;

  register unsigned int
    id,
    index,
    level;

  /*
    Initialize color description tree.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  color_cube.node_list=(Nodes *) NULL;
  color_cube.progress=0;
  color_cube.colors=0;
  color_cube.free_nodes=0;
  color_cube.root=GetNodeInfo(&color_cube,0);
  if (color_cube.root == (NodeInfo *) NULL)
    {
      ThrowException(&image->exception,ResourceLimitWarning,
        "unable to determine the number of image colors",
        "memory allocation failed");
      return(0);
    }
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      return(False);
    for (x=0; x < (int) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=color_cube.root;
      index=MaxTreeDepth-1;
      for (level=1; level <= MaxTreeDepth; level++)
      {
        id=(((Quantum) DownScale(p->red) >> index) & 0x01) << 2 |
           (((Quantum) DownScale(p->green) >> index) & 0x01) << 1 |
           (((Quantum) DownScale(p->blue) >> index) & 0x01);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(&color_cube,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(&image->exception,ResourceLimitWarning,
                  "unable to determine the number of image colors",
                  "memory allocation failed");
                return(0);
              }
          }
        node_info=node_info->child[id];
        index--;
        if (level != MaxTreeDepth)
          continue;
        for (i=0; i < (int) node_info->number_unique; i++)
           if (ColorMatch(*p,node_info->list[i],0))
             break;
        if (i < (int) node_info->number_unique)
          {
            node_info->list[i].count++;
            continue;
          }
        if (node_info->number_unique == 0)
          node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
        else
          ReacquireMemory((void **) &node_info->list,
            (i+1)*sizeof(ColorPacket));
        if (node_info->list == (ColorPacket *) NULL)
          {
            ThrowException(&image->exception,ResourceLimitWarning,
              "unable to determine the number of image colors",
              "memory allocation failed");
            return(0);
          }
        node_info->list[i].red=p->red;
        node_info->list[i].green=p->green;
        node_info->list[i].blue=p->blue;
        node_info->list[i].count=1;
        node_info->number_unique++;
        color_cube.colors++;
      }
      p++;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(NumberColorsImageText,y,image->rows);
  }
  if (file != (FILE *) NULL)
    {
      (void) fputs("\n",file);
      Histogram(&color_cube,color_cube.root,file);
      (void) fflush(file);
    }
  /*
    Release color cube tree storage.
  */
  DestroyList(color_cube.root);
  do
  {
    nodes=color_cube.node_list->next;
    LiberateMemory((void **) &color_cube.node_list);
    color_cube.node_list=nodes;
  }
  while (color_cube.node_list != (Nodes *) NULL);
  return(color_cube.colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  H i s t o g r a m                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Histogram traverses the color cube tree and produces a list of
%  unique pixel field values and the number of times each occurs in the image.
%
%  The format of the Histogram method is:
%
%      void Histogram(CubeInfo *color_cube,const NodeInfo *node_info,
%        FILE *file)
%
%  A description of each parameter follows.
%
%    o color_cube: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Histogram(CubeInfo *color_cube,const NodeInfo *node_info,FILE *file)
{
#define HistogramImageText  "  Compute image histogram...  "

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      Histogram(color_cube,node_info->child[id],file);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent];

      PixelPacket
        color;

      register ColorPacket
        *p;

      register int
        i;

      p=node_info->list;
      for (i=0; i < (int) node_info->number_unique; i++)
      {
        (void) fprintf(file,"%10lu: (%3d,%3d,%3d)  #%02x%02x%02x",
          p->count,p->red,p->green,p->blue,(unsigned int) p->red,
          (unsigned int) p->green,(unsigned int) p->blue);
        (void) fprintf(file,"  ");
        color.red=p->red;
        color.green=p->green;
        color.blue=p->blue;
        (void) QueryColorName(&color,name);
        (void) fprintf(file,"%.1024s",name);
        (void) fprintf(file,"\n");
      }
      if (QuantumTick(color_cube->progress,color_cube->colors))
        MagickMonitor(HistogramImageText,color_cube->progress,
          color_cube->colors);
      color_cube->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGrayImage returns True if the image is grayscale otherwise
%  False is returned.  If the image is DirectClass and grayscale, it is demoted
%  to PseudoClass.
%
%  The format of the IsGrayImage method is:
%
%      unsigned int IsGrayImage(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int IsGrayImage(Image *image)
{
  register int
    i;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) IsPseudoClass(image);
  if (image->storage_class != PseudoClass)
    return(False);
  for (i=0; i < (int) image->colors; i++)
    if (!IsGray(image->colormap[i]))
      return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s M a t t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMatteImage returns True if the image has one or more pixels that
%  are transparent otherwise False is returned.
%  to PseudoClass.
%
%  The format of the IsMatteImage method is:
%
%      unsigned int IsMatteImage(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method IsMatteImage returns True if the image has one or more
%      pixels that are transparent otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int IsMatteImage(Image *image)
{
  int
    y;

  register int
    x;

  register PixelPacket
    *p;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(False);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      return(False);
    for (x=0; x < (int) image->columns; x++)
    {
      if (p->opacity != OpaqueOpacity)
        return(True);
      p++;
    }
  }
  image->matte=False;
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMonochromeImage returns True if the image is monochrome otherwise
%  False is returned.  If the image is DirectClass and monochrome, it is
%  demoted to PseudoClass.
%
%  The format of the IsMonochromeImage method is:
%
%      status=IsMonochromeImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method IsMonochromeImage returns True if the image is
%      monochrome otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int IsMonochromeImage(Image *image)
{
  /*
    Determine if image is monochrome.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!IsGrayImage(image))
    return(False);
  if (image->colors > 2)
    return(False);
  if (((int) Intensity(image->colormap[0]) != 0) &&
      ((int) Intensity(image->colormap[0]) != MaxRGB))
    return(False);
  if (image->colors == 2)
    if (((int) Intensity(image->colormap[1]) != 0) &&
        ((int) Intensity(image->colormap[1]) != MaxRGB))
      return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P s e u d o C l a s s                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPseudoClass returns True if the image is PseudoClass and has 256
%  unique colors or less.  If the image is DirectClass and has 256 colors
%  or less, the image is demoted to PseudoClass.
%
%  The format of the IsPseudoClass method is:
%
%      unsigned int IsPseudoClass(Image *image)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPseudoClass returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your source image.  The sum of the
%      run-length counts in the source image must be equal to or exceed
%      the number of pixels.
%
%
*/
MagickExport unsigned int IsPseudoClass(Image *image)
{
  CubeInfo
    color_cube;

  int
    y;

  Nodes
    *nodes;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register NodeInfo
    *node_info;

  register PixelPacket
    *p,
    *q;

  unsigned int
    id,
    index,
    level;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class == PseudoClass) && (image->colors <= 256))
    return(True);
  if (image->matte)
    return(False);
  if (image->colorspace == CMYKColorspace)
    return(False);
  /*
    Initialize color description tree.
  */
  color_cube.node_list=(Nodes *) NULL;
  color_cube.colors=0;
  color_cube.free_nodes=0;
  color_cube.root=GetNodeInfo(&color_cube,0);
  if (color_cube.root == (NodeInfo *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Unable to determine image class",
      "Memory allocation failed");
  for (y=0; (y < (int) image->rows) && (color_cube.colors <= 256); y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      return(False);
    for (x=0; (x < (int) image->columns) && (color_cube.colors <= 256); x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=color_cube.root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        id=((DownScale(p->red) >> index) & 0x01) << 2 |
           ((DownScale(p->green) >> index) & 0x01) << 1 |
           ((DownScale(p->blue) >> index) & 0x01);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(&color_cube,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              ThrowBinaryException(ResourceLimitWarning,
                "Unable to determine image class","Memory allocation failed");
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (int) node_info->number_unique; i++)
        if (ColorMatch(*p,node_info->list[i],0))
          break;
      if (i == (int) node_info->number_unique)
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
          else
            ReacquireMemory((void **) &node_info->list,
              (i+1)*sizeof(ColorPacket));
          if (node_info->list == (ColorPacket *) NULL)
            ThrowBinaryException(ResourceLimitWarning,
              "Unable to determine image class","Memory allocation failed");
          node_info->list[i].red=p->red;
          node_info->list[i].green=p->green;
          node_info->list[i].blue=p->blue;
          node_info->list[i].index=color_cube.colors++;
          node_info->number_unique++;
        }
      p++;
    }
  }
  if (color_cube.colors <= 256)
    {
      IndexPacket
        index;

      /*
        Create colormap.
      */
      image->storage_class=PseudoClass;
      image->colors=color_cube.colors;
      if (image->colormap == (PixelPacket *) NULL)
        image->colormap=(PixelPacket *)
          AcquireMemory(image->colors*sizeof(PixelPacket));
      else
        ReacquireMemory((void **) &image->colormap,
          image->colors*sizeof(PixelPacket));
      if (image->colormap == (PixelPacket *) NULL)
        ThrowBinaryException(ResourceLimitWarning,
          "Unable to determine image class","Memory allocation failed");
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (int) image->columns; x++)
        {
          /*
            Start at the root and proceed level by level.
          */
          node_info=color_cube.root;
          index=MaxTreeDepth-1;
          for (level=1; level < MaxTreeDepth; level++)
          {
            id=((DownScale(q->red) >> index) & 0x01) << 2 |
               ((DownScale(q->green) >> index) & 0x01) << 1 |
               ((DownScale(q->blue) >> index) & 0x01);
            node_info=node_info->child[id];
            index--;
          }
          for (i=0; i < (int) node_info->number_unique; i++)
            if (ColorMatch(*q,node_info->list[i],0))
              break;
          index=node_info->list[i].index;
          indexes[x]=index;
          image->colormap[index].red=node_info->list[i].red;
          image->colormap[index].green=node_info->list[i].green;
          image->colormap[index].blue=node_info->list[i].blue;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  /*
    Release color cube tree storage.
  */
  DestroyList(color_cube.root);
  do
  {
    nodes=color_cube.node_list->next;
    LiberateMemory((void **) &color_cube.node_list);
    color_cube.node_list=nodes;
  } while (color_cube.node_list != (Nodes *) NULL);
  return((image->storage_class == PseudoClass) && (image->colors <= 256));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t C o l o r s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListColors reads the X client color database and returns a list
%  of colors contained in the database sorted in ascending alphabetic order.
%
%  The format of the ListColors function is:
%
%      filelist=ListColors(pattern,number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method ListColors returns a list of colors contained
%      in the database.  If the database cannot be read, a NULL list is
%      returned.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int ColorCompare(const void *x,const void *y)
{
  register char
    **p,
    **q;

  p=(char **) x;
  q=(char **) y;
  return(LocaleCompare(*p,*q));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport char **ListColors(const char *pattern,int *number_colors)
{
  char
    color[MaxTextExtent],
    **colorlist,
    *path,
    text[MaxTextExtent];

  FILE
    *file;

  int
    blue,
    count,
    green,
    red;

  unsigned int
    max_colors;

  /*
    Allocate color list.
  */
  assert(pattern != (char *) NULL);
  assert(number_colors != (int *) NULL);
  max_colors=sizeof(Colorlist)/sizeof(ColorlistInfo);
  colorlist=(char **) AcquireMemory(max_colors*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);
  /*
    Open database.
  */
  *number_colors=0;
  file=(FILE *) NULL;
  path=GetMagickConfigurePath("rgb.txt");
  if (path != (char *) NULL)
    {
      file=fopen(path,"r");
      LiberateMemory((void **) &path);
    }
  if (file == (FILE *) NULL)
    {
      register const ColorlistInfo
        *p;

      /*
        Can't find server color database-- use our color list.
      */
      for (p=Colorlist; p->name != (char *) NULL; p++)
        if (GlobExpression(p->name,pattern))
          {
            colorlist[*number_colors]=(char *) AcquireMemory(Extent(p->name)+1);
            if (colorlist[*number_colors] == (char *) NULL)
              break;
            (void) strcpy(colorlist[*number_colors],p->name);
            (*number_colors)++;
          }
      return(colorlist);
    }
  while (fgets(text,MaxTextExtent,file) != (char *) NULL)
  {
    count=sscanf(text,"%d %d %d %[^\n]\n",&red,&green,&blue,color);
    if (count != 4)
      continue;
    if (GlobExpression(color,pattern))
      {
        if (*number_colors >= (int) max_colors)
          {
            max_colors<<=1;
            ReacquireMemory((void **) &colorlist,max_colors*sizeof(char *));
            if (colorlist == (char **) NULL)
              {
                (void) fclose(file);
                return((char **) NULL);
              }
          }
        colorlist[*number_colors]=(char *) AcquireMemory(Extent(color)+1);
        if (colorlist[*number_colors] == (char *) NULL)
          break;
        (void) strcpy(colorlist[*number_colors],color);
        (*number_colors)++;
      }
  }
  (void) fclose(file);
  /*
    Sort colorlist in ascending order.
  */
  qsort((void *) colorlist,*number_colors,sizeof(char **),ColorCompare);
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method QueryColorDatabase looks up a RGB values for a color given in the
%  target string.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *target,PixelPacket *color)
%
%  A description of each parameter follows:
%
%    o status:  Method QueryColorDatabase returns True if the RGB values
%      of the target color is defined, otherwise False is returned.
%
%    o target: Specifies the color to lookup in the X color database.
%
%    o color: A pointer to an PixelPacket structure.  The RGB value of the
%      target color is returned as this value.
%
%
*/
MagickExport unsigned int QueryColorDatabase(const char *target,
  PixelPacket *color)
{
  char
    colorname[MaxTextExtent],
    *path,
    text[MaxTextExtent];

  int
    blue,
    count,
    green,
    left,
    mid,
    opacity,
    red,
    right;

  register int
    i;

  FILE
    *file;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  GetPixelPacket(color);
  if ((target == (char *) NULL) || (*target == '\0'))
    target=BackgroundColor;
  while (isspace((int) (*target)))
    target++;
  if (*target == '#')
    {
      char
        c;

      unsigned long
        n;

      green=0;
      blue=0;
      opacity=(-1);
      target++;
      n=Extent(target);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            red=green;
            green=blue;
            blue=0;
            for (i=(int) n-1; i >= 0; i--)
            {
              c=(*target++);
              blue<<=4;
              if ((c >= '0') && (c <= '9'))
                blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    blue|=c-('a'-10);
                  else
                    return(False);
            }
          } while (*target != '\0');
        }
      else
        if ((n != 4) && (n != 8) && (n != 16))
          return(False);
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              red=green;
              green=blue;
              blue=opacity;
              opacity=0;
              for (i=(int) n-1; i >= 0; i--)
              {
                c=(*target++);
                opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      opacity|=c-('a'-10);
                    else
                      return(False);
              }
            } while (*target != '\0');
          }
      n<<=2;
      color->red=((unsigned long) (MaxRGB*red)/((1 << n)-1));
      color->green=((unsigned long) (MaxRGB*green)/((1 << n)-1));
      color->blue=((unsigned long) (MaxRGB*blue)/((1 << n)-1));
      color->opacity=OpaqueOpacity;
      if (opacity >= 0)
        color->opacity=((unsigned long) (MaxRGB*opacity)/((1 << n)-1));
      return(True);
    }
  if (LocaleNCompare(target,"rgb(",4) == 0)
    {
      (void) sscanf(target,"%*[^(](%d%*[ ,]%d%*[ ,]%d",&red,&green,&blue);
      color->red=UpScale(red);
      color->green=UpScale(green);
      color->blue=UpScale(blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  /*
    Search our internal color database.
  */
  left=0;
  right=sizeof(Colorlist)/sizeof(ColorlistInfo)-2;
  for (mid=(right+left)/2 ; right != left; mid=(right+left)/2)
  {
    i=LocaleCompare(target,Colorlist[mid].name);
    if (i < 0)
      {
        if (right == mid)
          mid--;
        right=mid;
        continue;
      }
    if (i > 0)
      {
        if (left == mid)
          mid++;
        left=mid;
        continue;
      }
    color->red=Colorlist[mid].red;
    color->green=Colorlist[mid].green;
    color->blue=Colorlist[mid].blue;
    color->opacity=Colorlist[mid].opacity;
    return(True);
  }
#if defined(HasX11)
  {
    XColor
      xcolor;

    unsigned int
      status;

    /*
      Let the X server define the color for us.
    */
    status=XQueryColorDatabase(target,&xcolor);
    color->red=XDownScale(xcolor.red);
    color->green=XDownScale(xcolor.green);
    color->blue=XDownScale(xcolor.blue);
    color->opacity=OpaqueOpacity;
    return(status);
  }
#endif
  /*
    Match color against the X color database.
  */
  path=GetMagickConfigurePath("rgb.txt");
  if (path == (char *) NULL)
    return(False);
  file=fopen(path,"r");
  LiberateMemory((void **) &path);  
  if (file == (FILE *) NULL)
    return(False);
  while (fgets(text,MaxTextExtent,file) != (char *) NULL)
  {
    count=sscanf(text,"%d %d %d %[^\n]\n",&red,&green,&blue,colorname);
    if (count != 4)
      continue;
    if (LocaleCompare(colorname,target) != 0)
      continue;
    color->red=UpScale(red);
    color->green=UpScale(green);
    color->blue=UpScale(blue);
    color->opacity=OpaqueOpacity;
    return(True);
  }
  (void) fclose(file);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r N a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method QueryColorName returns the name of the color that is closest to the
%  supplied color in RGB space.
%
%  The format of the QueryColorName method is:
%
%      unsigned int QueryColorName(const PixelPacket *color,char *name)
%
%  A description of each parameter follows.
%
%    o distance: Method QueryColorName returns the distance-squared in RGB
%      space as well as the color name that is at a minimum distance.
%
%    o color: This is a pointer to a PixelPacket structure that contains the
%      color we are searching for.
%
%    o name: The name of the color that is closest to the supplied color is
%      returned in this character buffer.
%
%
*/
MagickExport unsigned int QueryColorName(const PixelPacket *color,char *name)
{
  double
    distance,
    distance_squared,
    min_distance;

  register const ColorlistInfo
    *p;

  *name='\0';
  min_distance=0;
  for (p=Colorlist; p->name != (char *) NULL; p++)
  {
    distance=color->red-(int) p->red;
    distance_squared=distance*distance;
    distance=color->green-(int) p->green;
    distance_squared+=distance*distance;
    distance=color->blue-(int) p->blue;
    distance_squared+=distance*distance;
    if ((p == Colorlist) || (distance_squared < min_distance))
      {
        min_distance=distance_squared;
        (void) strcpy(name,p->name);
      }
  }
  if (min_distance != 0.0)
    FormatString(name,HexColorFormat,(unsigned int) color->red,
      (unsigned int) color->green,(unsigned int) color->blue,
      (unsigned int) color->opacity);
  return((unsigned int) min_distance);
}
