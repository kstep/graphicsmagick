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
%  Copyright (C) 2000 imagemagick Studio, a non-profit organization dedicated %
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
#define NumberXColors  757

const ColorlistInfo
  XColorlist[NumberXColors] =
  {
    { "alice blue", UpScale(240), UpScale(248), UpScale(255) },
    { "aliceblue", UpScale(240), UpScale(248), UpScale(255) },
    { "antique white", UpScale(250), UpScale(235), UpScale(215) },
    { "antiquewhite", UpScale(250), UpScale(235), UpScale(215) },
    { "antiquewhite1", UpScale(255), UpScale(239), UpScale(219) },
    { "antiquewhite2", UpScale(238), UpScale(223), UpScale(204) },
    { "antiquewhite3", UpScale(205), UpScale(192), UpScale(176) },
    { "antiquewhite4", UpScale(139), UpScale(131), UpScale(120) },
    { "aquamarine", UpScale(127), UpScale(255), UpScale(212) },
    { "aquamarine1", UpScale(127), UpScale(255), UpScale(212) },
    { "aquamarine2", UpScale(118), UpScale(238), UpScale(198) },
    { "aquamarine3", UpScale(102), UpScale(205), UpScale(170) },
    { "aquamarine4", UpScale(69), UpScale(139), UpScale(116) },
    { "azure", UpScale(240), UpScale(255), UpScale(255) },
    { "azure1", UpScale(240), UpScale(255), UpScale(255) },
    { "azure2", UpScale(224), UpScale(238), UpScale(238) },
    { "azure3", UpScale(193), UpScale(205), UpScale(205) },
    { "azure4", UpScale(131), UpScale(139), UpScale(139) },
    { "beige", UpScale(245), UpScale(245), UpScale(220) },
    { "bisque", UpScale(255), UpScale(228), UpScale(196) },
    { "bisque1", UpScale(255), UpScale(228), UpScale(196) },
    { "bisque2", UpScale(238), UpScale(213), UpScale(183) },
    { "bisque3", UpScale(205), UpScale(183), UpScale(158) },
    { "bisque4", UpScale(139), UpScale(125), UpScale(107) },
    { "black", UpScale(0), UpScale(0), UpScale(0) },
    { "blanched almond", UpScale(255), UpScale(235), UpScale(205) },
    { "blanchedalmond", UpScale(255), UpScale(235), UpScale(205) },
    { "blue", UpScale(0), UpScale(0), UpScale(255) },
    { "blue violet", UpScale(138), UpScale(43), UpScale(226) },
    { "blue1", UpScale(0), UpScale(0), UpScale(255) },
    { "blue2", UpScale(0), UpScale(0), UpScale(238) },
    { "blue3", UpScale(0), UpScale(0), UpScale(205) },
    { "blue4", UpScale(0), UpScale(0), UpScale(139) },
    { "blueviolet", UpScale(138), UpScale(43), UpScale(226) },
    { "brown", UpScale(165), UpScale(42), UpScale(42) },
    { "brown1", UpScale(255), UpScale(64), UpScale(64) },
    { "brown2", UpScale(238), UpScale(59), UpScale(59) },
    { "brown3", UpScale(205), UpScale(51), UpScale(51) },
    { "brown4", UpScale(139), UpScale(35), UpScale(35) },
    { "burlywood", UpScale(222), UpScale(184), UpScale(135) },
    { "burlywood1", UpScale(255), UpScale(211), UpScale(155) },
    { "burlywood2", UpScale(238), UpScale(197), UpScale(145) },
    { "burlywood3", UpScale(205), UpScale(170), UpScale(125) },
    { "burlywood4", UpScale(139), UpScale(115), UpScale(85) },
    { "cadet blue", UpScale(95), UpScale(158), UpScale(160) },
    { "cadetblue", UpScale(95), UpScale(158), UpScale(160) },
    { "cadetblue1", UpScale(152), UpScale(245), UpScale(255) },
    { "cadetblue2", UpScale(142), UpScale(229), UpScale(238) },
    { "cadetblue3", UpScale(122), UpScale(197), UpScale(205) },
    { "cadetblue4", UpScale(83), UpScale(134), UpScale(139) },
    { "chartreuse", UpScale(127), UpScale(255), UpScale(0) },
    { "chartreuse1", UpScale(127), UpScale(255), UpScale(0) },
    { "chartreuse2", UpScale(118), UpScale(238), UpScale(0) },
    { "chartreuse3", UpScale(102), UpScale(205), UpScale(0) },
    { "chartreuse4", UpScale(69), UpScale(139), UpScale(0) },
    { "chocolate", UpScale(210), UpScale(105), UpScale(30) },
    { "chocolate1", UpScale(255), UpScale(127), UpScale(36) },
    { "chocolate2", UpScale(238), UpScale(118), UpScale(33) },
    { "chocolate3", UpScale(205), UpScale(102), UpScale(29) },
    { "chocolate4", UpScale(139), UpScale(69), UpScale(19) },
    { "coral", UpScale(255), UpScale(127), UpScale(80) },
    { "coral1", UpScale(255), UpScale(114), UpScale(86) },
    { "coral2", UpScale(238), UpScale(106), UpScale(80) },
    { "coral3", UpScale(205), UpScale(91), UpScale(69) },
    { "coral4", UpScale(139), UpScale(62), UpScale(47) },
    { "cornflower blue", UpScale(100), UpScale(149), UpScale(237) },
    { "cornflowerblue", UpScale(100), UpScale(149), UpScale(237) },
    { "cornsilk", UpScale(255), UpScale(248), UpScale(220) },
    { "cornsilk1", UpScale(255), UpScale(248), UpScale(220) },
    { "cornsilk2", UpScale(238), UpScale(232), UpScale(205) },
    { "cornsilk3", UpScale(205), UpScale(200), UpScale(177) },
    { "cornsilk4", UpScale(139), UpScale(136), UpScale(120) },
    { "crimson", UpScale(220), UpScale(20), UpScale(60) },
    { "cyan", UpScale(0), UpScale(255), UpScale(255) },
    { "cyan1", UpScale(0), UpScale(255), UpScale(255) },
    { "cyan2", UpScale(0), UpScale(238), UpScale(238) },
    { "cyan3", UpScale(0), UpScale(205), UpScale(205) },
    { "cyan4", UpScale(0), UpScale(139), UpScale(139) },
    { "dark blue", UpScale(0), UpScale(0), UpScale(139) },
    { "dark cyan", UpScale(0), UpScale(139), UpScale(139) },
    { "dark goldenrod", UpScale(184), UpScale(134), UpScale(11) },
    { "dark gray", UpScale(169), UpScale(169), UpScale(169) },
    { "dark green", UpScale(0), UpScale(100), UpScale(0) },
    { "dark grey", UpScale(169), UpScale(169), UpScale(169) },
    { "dark khaki", UpScale(189), UpScale(183), UpScale(107) },
    { "dark magenta", UpScale(139), UpScale(0), UpScale(139) },
    { "dark olive green", UpScale(85), UpScale(107), UpScale(47) },
    { "dark orange", UpScale(255), UpScale(140), UpScale(0) },
    { "dark orchid", UpScale(153), UpScale(50), UpScale(204) },
    { "dark red", UpScale(139), UpScale(0), UpScale(0) },
    { "dark salmon", UpScale(233), UpScale(150), UpScale(122) },
    { "dark sea green", UpScale(143), UpScale(188), UpScale(143) },
    { "dark slate blue", UpScale(72), UpScale(61), UpScale(139) },
    { "dark slate gray", UpScale(47), UpScale(79), UpScale(79) },
    { "dark slate grey", UpScale(47), UpScale(79), UpScale(79) },
    { "dark turquoise", UpScale(0), UpScale(206), UpScale(209) },
    { "dark violet", UpScale(148), UpScale(0), UpScale(211) },
    { "darkblue", UpScale(0), UpScale(0), UpScale(139) },
    { "darkcyan", UpScale(0), UpScale(139), UpScale(139) },
    { "darkgoldenrod", UpScale(184), UpScale(134), UpScale(11) },
    { "darkgoldenrod1", UpScale(255), UpScale(185), UpScale(15) },
    { "darkgoldenrod2", UpScale(238), UpScale(173), UpScale(14) },
    { "darkgoldenrod3", UpScale(205), UpScale(149), UpScale(12) },
    { "darkgoldenrod4", UpScale(139), UpScale(101), UpScale(8) },
    { "darkgray", UpScale(169), UpScale(169), UpScale(169) },
    { "darkgreen", UpScale(0), UpScale(100), UpScale(0) },
    { "darkgrey", UpScale(169), UpScale(169), UpScale(169) },
    { "darkkhaki", UpScale(189), UpScale(183), UpScale(107) },
    { "darkmagenta", UpScale(139), UpScale(0), UpScale(139) },
    { "darkolivegreen", UpScale(85), UpScale(107), UpScale(47) },
    { "darkolivegreen1", UpScale(202), UpScale(255), UpScale(112) },
    { "darkolivegreen2", UpScale(188), UpScale(238), UpScale(104) },
    { "darkolivegreen3", UpScale(162), UpScale(205), UpScale(90) },
    { "darkolivegreen4", UpScale(110), UpScale(139), UpScale(61) },
    { "darkorange", UpScale(255), UpScale(140), UpScale(0) },
    { "darkorange1", UpScale(255), UpScale(127), UpScale(0) },
    { "darkorange2", UpScale(238), UpScale(118), UpScale(0) },
    { "darkorange3", UpScale(205), UpScale(102), UpScale(0) },
    { "darkorange4", UpScale(139), UpScale(69), UpScale(0) },
    { "darkorchid", UpScale(153), UpScale(50), UpScale(204) },
    { "darkorchid1", UpScale(191), UpScale(62), UpScale(255) },
    { "darkorchid2", UpScale(178), UpScale(58), UpScale(238) },
    { "darkorchid3", UpScale(154), UpScale(50), UpScale(205) },
    { "darkorchid4", UpScale(104), UpScale(34), UpScale(139) },
    { "darkred", UpScale(139), UpScale(0), UpScale(0) },
    { "darksalmon", UpScale(233), UpScale(150), UpScale(122) },
    { "darkseagreen", UpScale(143), UpScale(188), UpScale(143) },
    { "darkseagreen1", UpScale(193), UpScale(255), UpScale(193) },
    { "darkseagreen2", UpScale(180), UpScale(238), UpScale(180) },
    { "darkseagreen3", UpScale(155), UpScale(205), UpScale(155) },
    { "darkseagreen4", UpScale(105), UpScale(139), UpScale(105) },
    { "darkslateblue", UpScale(72), UpScale(61), UpScale(139) },
    { "darkslategray", UpScale(47), UpScale(79), UpScale(79) },
    { "darkslategray1", UpScale(151), UpScale(255), UpScale(255) },
    { "darkslategray2", UpScale(141), UpScale(238), UpScale(238) },
    { "darkslategray3", UpScale(121), UpScale(205), UpScale(205) },
    { "darkslategray4", UpScale(82), UpScale(139), UpScale(139) },
    { "darkslategrey", UpScale(47), UpScale(79), UpScale(79) },
    { "darkturquoise", UpScale(0), UpScale(206), UpScale(209) },
    { "darkviolet", UpScale(148), UpScale(0), UpScale(211) },
    { "deep pink", UpScale(255), UpScale(20), UpScale(147) },
    { "deep sky blue", UpScale(0), UpScale(191), UpScale(255) },
    { "deeppink", UpScale(255), UpScale(20), UpScale(147) },
    { "deeppink1", UpScale(255), UpScale(20), UpScale(147) },
    { "deeppink2", UpScale(238), UpScale(18), UpScale(137) },
    { "deeppink3", UpScale(205), UpScale(16), UpScale(118) },
    { "deeppink4", UpScale(139), UpScale(10), UpScale(80) },
    { "deepskyblue", UpScale(0), UpScale(191), UpScale(255) },
    { "deepskyblue1", UpScale(0), UpScale(191), UpScale(255) },
    { "deepskyblue2", UpScale(0), UpScale(178), UpScale(238) },
    { "deepskyblue3", UpScale(0), UpScale(154), UpScale(205) },
    { "deepskyblue4", UpScale(0), UpScale(104), UpScale(139) },
    { "dim gray", UpScale(105), UpScale(105), UpScale(105) },
    { "dim grey", UpScale(105), UpScale(105), UpScale(105) },
    { "dimgray", UpScale(105), UpScale(105), UpScale(105) },
    { "dimgrey", UpScale(105), UpScale(105), UpScale(105) },
    { "dodger blue", UpScale(30), UpScale(144), UpScale(255) },
    { "dodgerblue", UpScale(30), UpScale(144), UpScale(255) },
    { "dodgerblue1", UpScale(30), UpScale(144), UpScale(255) },
    { "dodgerblue2", UpScale(28), UpScale(134), UpScale(238) },
    { "dodgerblue3", UpScale(24), UpScale(116), UpScale(205) },
    { "dodgerblue4", UpScale(16), UpScale(78), UpScale(139) },
    { "firebrick", UpScale(178), UpScale(34), UpScale(34) },
    { "firebrick1", UpScale(255), UpScale(48), UpScale(48) },
    { "firebrick2", UpScale(238), UpScale(44), UpScale(44) },
    { "firebrick3", UpScale(205), UpScale(38), UpScale(38) },
    { "firebrick4", UpScale(139), UpScale(26), UpScale(26) },
    { "floral white", UpScale(255), UpScale(250), UpScale(240) },
    { "floralwhite", UpScale(255), UpScale(250), UpScale(240) },
    { "forest green", UpScale(34), UpScale(139), UpScale(34) },
    { "forestgreen", UpScale(34), UpScale(139), UpScale(34) },
    { "fractal", UpScale(128), UpScale(128), UpScale(128) },
    { "gainsboro", UpScale(220), UpScale(220), UpScale(220) },
    { "ghost white", UpScale(248), UpScale(248), UpScale(255) },
    { "ghostwhite", UpScale(248), UpScale(248), UpScale(255) },
    { "gold", UpScale(255), UpScale(215), UpScale(0) },
    { "gold1", UpScale(255), UpScale(215), UpScale(0) },
    { "gold2", UpScale(238), UpScale(201), UpScale(0) },
    { "gold3", UpScale(205), UpScale(173), UpScale(0) },
    { "gold4", UpScale(139), UpScale(117), UpScale(0) },
    { "goldenrod", UpScale(218), UpScale(165), UpScale(32) },
    { "goldenrod1", UpScale(255), UpScale(193), UpScale(37) },
    { "goldenrod2", UpScale(238), UpScale(180), UpScale(34) },
    { "goldenrod3", UpScale(205), UpScale(155), UpScale(29) },
    { "goldenrod4", UpScale(139), UpScale(105), UpScale(20) },
    { "gray", UpScale(190), UpScale(190), UpScale(190) },
    { "gray0", UpScale(0), UpScale(0), UpScale(0) },
    { "gray1", UpScale(3), UpScale(3), UpScale(3) },
    { "gray10", UpScale(26), UpScale(26), UpScale(26) },
    { "gray100", UpScale(255), UpScale(255), UpScale(255) },
    { "gray11", UpScale(28), UpScale(28), UpScale(28) },
    { "gray12", UpScale(31), UpScale(31), UpScale(31) },
    { "gray13", UpScale(33), UpScale(33), UpScale(33) },
    { "gray14", UpScale(36), UpScale(36), UpScale(36) },
    { "gray15", UpScale(38), UpScale(38), UpScale(38) },
    { "gray16", UpScale(41), UpScale(41), UpScale(41) },
    { "gray17", UpScale(43), UpScale(43), UpScale(43) },
    { "gray18", UpScale(46), UpScale(46), UpScale(46) },
    { "gray19", UpScale(48), UpScale(48), UpScale(48) },
    { "gray2", UpScale(5), UpScale(5), UpScale(5) },
    { "gray20", UpScale(51), UpScale(51), UpScale(51) },
    { "gray21", UpScale(54), UpScale(54), UpScale(54) },
    { "gray22", UpScale(56), UpScale(56), UpScale(56) },
    { "gray23", UpScale(59), UpScale(59), UpScale(59) },
    { "gray24", UpScale(61), UpScale(61), UpScale(61) },
    { "gray25", UpScale(64), UpScale(64), UpScale(64) },
    { "gray26", UpScale(66), UpScale(66), UpScale(66) },
    { "gray27", UpScale(69), UpScale(69), UpScale(69) },
    { "gray28", UpScale(71), UpScale(71), UpScale(71) },
    { "gray29", UpScale(74), UpScale(74), UpScale(74) },
    { "gray3", UpScale(8), UpScale(8), UpScale(8) },
    { "gray30", UpScale(77), UpScale(77), UpScale(77) },
    { "gray31", UpScale(79), UpScale(79), UpScale(79) },
    { "gray32", UpScale(82), UpScale(82), UpScale(82) },
    { "gray33", UpScale(84), UpScale(84), UpScale(84) },
    { "gray34", UpScale(87), UpScale(87), UpScale(87) },
    { "gray35", UpScale(89), UpScale(89), UpScale(89) },
    { "gray36", UpScale(92), UpScale(92), UpScale(92) },
    { "gray37", UpScale(94), UpScale(94), UpScale(94) },
    { "gray38", UpScale(97), UpScale(97), UpScale(97) },
    { "gray39", UpScale(99), UpScale(99), UpScale(99) },
    { "gray4", UpScale(10), UpScale(10), UpScale(10) },
    { "gray40", UpScale(102), UpScale(102), UpScale(102) },
    { "gray41", UpScale(105), UpScale(105), UpScale(105) },
    { "gray42", UpScale(107), UpScale(107), UpScale(107) },
    { "gray43", UpScale(110), UpScale(110), UpScale(110) },
    { "gray44", UpScale(112), UpScale(112), UpScale(112) },
    { "gray45", UpScale(115), UpScale(115), UpScale(115) },
    { "gray46", UpScale(117), UpScale(117), UpScale(117) },
    { "gray47", UpScale(120), UpScale(120), UpScale(120) },
    { "gray48", UpScale(122), UpScale(122), UpScale(122) },
    { "gray49", UpScale(125), UpScale(125), UpScale(125) },
    { "gray5", UpScale(13), UpScale(13), UpScale(13) },
    { "gray50", UpScale(127), UpScale(127), UpScale(127) },
    { "gray51", UpScale(130), UpScale(130), UpScale(130) },
    { "gray52", UpScale(133), UpScale(133), UpScale(133) },
    { "gray53", UpScale(135), UpScale(135), UpScale(135) },
    { "gray54", UpScale(138), UpScale(138), UpScale(138) },
    { "gray55", UpScale(140), UpScale(140), UpScale(140) },
    { "gray56", UpScale(143), UpScale(143), UpScale(143) },
    { "gray57", UpScale(145), UpScale(145), UpScale(145) },
    { "gray58", UpScale(148), UpScale(148), UpScale(148) },
    { "gray59", UpScale(150), UpScale(150), UpScale(150) },
    { "gray6", UpScale(15), UpScale(15), UpScale(15) },
    { "gray60", UpScale(153), UpScale(153), UpScale(153) },
    { "gray61", UpScale(156), UpScale(156), UpScale(156) },
    { "gray62", UpScale(158), UpScale(158), UpScale(158) },
    { "gray63", UpScale(161), UpScale(161), UpScale(161) },
    { "gray64", UpScale(163), UpScale(163), UpScale(163) },
    { "gray65", UpScale(166), UpScale(166), UpScale(166) },
    { "gray66", UpScale(168), UpScale(168), UpScale(168) },
    { "gray67", UpScale(171), UpScale(171), UpScale(171) },
    { "gray68", UpScale(173), UpScale(173), UpScale(173) },
    { "gray69", UpScale(176), UpScale(176), UpScale(176) },
    { "gray7", UpScale(18), UpScale(18), UpScale(18) },
    { "gray70", UpScale(179), UpScale(179), UpScale(179) },
    { "gray71", UpScale(181), UpScale(181), UpScale(181) },
    { "gray72", UpScale(184), UpScale(184), UpScale(184) },
    { "gray73", UpScale(186), UpScale(186), UpScale(186) },
    { "gray74", UpScale(189), UpScale(189), UpScale(189) },
    { "gray75", UpScale(191), UpScale(191), UpScale(191) },
    { "gray76", UpScale(194), UpScale(194), UpScale(194) },
    { "gray77", UpScale(196), UpScale(196), UpScale(196) },
    { "gray78", UpScale(199), UpScale(199), UpScale(199) },
    { "gray79", UpScale(201), UpScale(201), UpScale(201) },
    { "gray8", UpScale(20), UpScale(20), UpScale(20) },
    { "gray80", UpScale(204), UpScale(204), UpScale(204) },
    { "gray81", UpScale(207), UpScale(207), UpScale(207) },
    { "gray82", UpScale(209), UpScale(209), UpScale(209) },
    { "gray83", UpScale(212), UpScale(212), UpScale(212) },
    { "gray84", UpScale(214), UpScale(214), UpScale(214) },
    { "gray85", UpScale(217), UpScale(217), UpScale(217) },
    { "gray86", UpScale(219), UpScale(219), UpScale(219) },
    { "gray87", UpScale(222), UpScale(222), UpScale(222) },
    { "gray88", UpScale(224), UpScale(224), UpScale(224) },
    { "gray89", UpScale(227), UpScale(227), UpScale(227) },
    { "gray9", UpScale(23), UpScale(23), UpScale(23) },
    { "gray90", UpScale(229), UpScale(229), UpScale(229) },
    { "gray91", UpScale(232), UpScale(232), UpScale(232) },
    { "gray92", UpScale(235), UpScale(235), UpScale(235) },
    { "gray93", UpScale(237), UpScale(237), UpScale(237) },
    { "gray94", UpScale(240), UpScale(240), UpScale(240) },
    { "gray95", UpScale(242), UpScale(242), UpScale(242) },
    { "gray96", UpScale(245), UpScale(245), UpScale(245) },
    { "gray97", UpScale(247), UpScale(247), UpScale(247) },
    { "gray98", UpScale(250), UpScale(250), UpScale(250) },
    { "gray99", UpScale(252), UpScale(252), UpScale(252) },
    { "green", UpScale(0), UpScale(255), UpScale(0) },
    { "green yellow", UpScale(173), UpScale(255), UpScale(47) },
    { "green1", UpScale(0), UpScale(255), UpScale(0) },
    { "green2", UpScale(0), UpScale(238), UpScale(0) },
    { "green3", UpScale(0), UpScale(205), UpScale(0) },
    { "green4", UpScale(0), UpScale(139), UpScale(0) },
    { "greenyellow", UpScale(173), UpScale(255), UpScale(47) },
    { "grey", UpScale(190), UpScale(190), UpScale(190) },
    { "grey0", UpScale(0), UpScale(0), UpScale(0) },
    { "grey1", UpScale(3), UpScale(3), UpScale(3) },
    { "grey10", UpScale(26), UpScale(26), UpScale(26) },
    { "grey100", UpScale(255), UpScale(255), UpScale(255) },
    { "grey11", UpScale(28), UpScale(28), UpScale(28) },
    { "grey12", UpScale(31), UpScale(31), UpScale(31) },
    { "grey13", UpScale(33), UpScale(33), UpScale(33) },
    { "grey14", UpScale(36), UpScale(36), UpScale(36) },
    { "grey15", UpScale(38), UpScale(38), UpScale(38) },
    { "grey16", UpScale(41), UpScale(41), UpScale(41) },
    { "grey17", UpScale(43), UpScale(43), UpScale(43) },
    { "grey18", UpScale(46), UpScale(46), UpScale(46) },
    { "grey19", UpScale(48), UpScale(48), UpScale(48) },
    { "grey2", UpScale(5), UpScale(5), UpScale(5) },
    { "grey20", UpScale(51), UpScale(51), UpScale(51) },
    { "grey21", UpScale(54), UpScale(54), UpScale(54) },
    { "grey22", UpScale(56), UpScale(56), UpScale(56) },
    { "grey23", UpScale(59), UpScale(59), UpScale(59) },
    { "grey24", UpScale(61), UpScale(61), UpScale(61) },
    { "grey25", UpScale(64), UpScale(64), UpScale(64) },
    { "grey26", UpScale(66), UpScale(66), UpScale(66) },
    { "grey27", UpScale(69), UpScale(69), UpScale(69) },
    { "grey28", UpScale(71), UpScale(71), UpScale(71) },
    { "grey29", UpScale(74), UpScale(74), UpScale(74) },
    { "grey3", UpScale(8), UpScale(8), UpScale(8) },
    { "grey30", UpScale(77), UpScale(77), UpScale(77) },
    { "grey31", UpScale(79), UpScale(79), UpScale(79) },
    { "grey32", UpScale(82), UpScale(82), UpScale(82) },
    { "grey33", UpScale(84), UpScale(84), UpScale(84) },
    { "grey34", UpScale(87), UpScale(87), UpScale(87) },
    { "grey35", UpScale(89), UpScale(89), UpScale(89) },
    { "grey36", UpScale(92), UpScale(92), UpScale(92) },
    { "grey37", UpScale(94), UpScale(94), UpScale(94) },
    { "grey38", UpScale(97), UpScale(97), UpScale(97) },
    { "grey39", UpScale(99), UpScale(99), UpScale(99) },
    { "grey4", UpScale(10), UpScale(10), UpScale(10) },
    { "grey40", UpScale(102), UpScale(102), UpScale(102) },
    { "grey41", UpScale(105), UpScale(105), UpScale(105) },
    { "grey42", UpScale(107), UpScale(107), UpScale(107) },
    { "grey43", UpScale(110), UpScale(110), UpScale(110) },
    { "grey44", UpScale(112), UpScale(112), UpScale(112) },
    { "grey45", UpScale(115), UpScale(115), UpScale(115) },
    { "grey46", UpScale(117), UpScale(117), UpScale(117) },
    { "grey47", UpScale(120), UpScale(120), UpScale(120) },
    { "grey48", UpScale(122), UpScale(122), UpScale(122) },
    { "grey49", UpScale(125), UpScale(125), UpScale(125) },
    { "grey5", UpScale(13), UpScale(13), UpScale(13) },
    { "grey50", UpScale(127), UpScale(127), UpScale(127) },
    { "grey51", UpScale(130), UpScale(130), UpScale(130) },
    { "grey52", UpScale(133), UpScale(133), UpScale(133) },
    { "grey53", UpScale(135), UpScale(135), UpScale(135) },
    { "grey54", UpScale(138), UpScale(138), UpScale(138) },
    { "grey55", UpScale(140), UpScale(140), UpScale(140) },
    { "grey56", UpScale(143), UpScale(143), UpScale(143) },
    { "grey57", UpScale(145), UpScale(145), UpScale(145) },
    { "grey58", UpScale(148), UpScale(148), UpScale(148) },
    { "grey59", UpScale(150), UpScale(150), UpScale(150) },
    { "grey6", UpScale(15), UpScale(15), UpScale(15) },
    { "grey60", UpScale(153), UpScale(153), UpScale(153) },
    { "grey61", UpScale(156), UpScale(156), UpScale(156) },
    { "grey62", UpScale(158), UpScale(158), UpScale(158) },
    { "grey63", UpScale(161), UpScale(161), UpScale(161) },
    { "grey64", UpScale(163), UpScale(163), UpScale(163) },
    { "grey65", UpScale(166), UpScale(166), UpScale(166) },
    { "grey66", UpScale(168), UpScale(168), UpScale(168) },
    { "grey67", UpScale(171), UpScale(171), UpScale(171) },
    { "grey68", UpScale(173), UpScale(173), UpScale(173) },
    { "grey69", UpScale(176), UpScale(176), UpScale(176) },
    { "grey7", UpScale(18), UpScale(18), UpScale(18) },
    { "grey70", UpScale(179), UpScale(179), UpScale(179) },
    { "grey71", UpScale(181), UpScale(181), UpScale(181) },
    { "grey72", UpScale(184), UpScale(184), UpScale(184) },
    { "grey73", UpScale(186), UpScale(186), UpScale(186) },
    { "grey74", UpScale(189), UpScale(189), UpScale(189) },
    { "grey75", UpScale(191), UpScale(191), UpScale(191) },
    { "grey76", UpScale(194), UpScale(194), UpScale(194) },
    { "grey77", UpScale(196), UpScale(196), UpScale(196) },
    { "grey78", UpScale(199), UpScale(199), UpScale(199) },
    { "grey79", UpScale(201), UpScale(201), UpScale(201) },
    { "grey8", UpScale(20), UpScale(20), UpScale(20) },
    { "grey80", UpScale(204), UpScale(204), UpScale(204) },
    { "grey81", UpScale(207), UpScale(207), UpScale(207) },
    { "grey82", UpScale(209), UpScale(209), UpScale(209) },
    { "grey83", UpScale(212), UpScale(212), UpScale(212) },
    { "grey84", UpScale(214), UpScale(214), UpScale(214) },
    { "grey85", UpScale(217), UpScale(217), UpScale(217) },
    { "grey86", UpScale(219), UpScale(219), UpScale(219) },
    { "grey87", UpScale(222), UpScale(222), UpScale(222) },
    { "grey88", UpScale(224), UpScale(224), UpScale(224) },
    { "grey89", UpScale(227), UpScale(227), UpScale(227) },
    { "grey9", UpScale(23), UpScale(23), UpScale(23) },
    { "grey90", UpScale(229), UpScale(229), UpScale(229) },
    { "grey91", UpScale(232), UpScale(232), UpScale(232) },
    { "grey92", UpScale(235), UpScale(235), UpScale(235) },
    { "grey93", UpScale(237), UpScale(237), UpScale(237) },
    { "grey94", UpScale(240), UpScale(240), UpScale(240) },
    { "grey95", UpScale(242), UpScale(242), UpScale(242) },
    { "grey96", UpScale(245), UpScale(245), UpScale(245) },
    { "grey97", UpScale(247), UpScale(247), UpScale(247) },
    { "grey98", UpScale(250), UpScale(250), UpScale(250) },
    { "grey99", UpScale(252), UpScale(252), UpScale(252) },
    { "honeydew", UpScale(240), UpScale(255), UpScale(240) },
    { "honeydew1", UpScale(240), UpScale(255), UpScale(240) },
    { "honeydew2", UpScale(224), UpScale(238), UpScale(224) },
    { "honeydew3", UpScale(193), UpScale(205), UpScale(193) },
    { "honeydew4", UpScale(131), UpScale(139), UpScale(131) },
    { "hot pink", UpScale(255), UpScale(105), UpScale(180) },
    { "hotpink", UpScale(255), UpScale(105), UpScale(180) },
    { "hotpink1", UpScale(255), UpScale(110), UpScale(180) },
    { "hotpink2", UpScale(238), UpScale(106), UpScale(167) },
    { "hotpink3", UpScale(205), UpScale(96), UpScale(144) },
    { "hotpink4", UpScale(139), UpScale(58), UpScale(98) },
    { "indian red", UpScale(205), UpScale(92), UpScale(92) },
    { "indianred", UpScale(205), UpScale(92), UpScale(92) },
    { "indianred1", UpScale(255), UpScale(106), UpScale(106) },
    { "indianred2", UpScale(238), UpScale(99), UpScale(99) },
    { "indianred3", UpScale(205), UpScale(85), UpScale(85) },
    { "indianred4", UpScale(139), UpScale(58), UpScale(58) },
    { "indigo", UpScale(75), UpScale(0), UpScale(130) },
    { "indigo2", UpScale(33), UpScale(136), UpScale(104) },
    { "ivory", UpScale(255), UpScale(255), UpScale(240) },
    { "ivory1", UpScale(255), UpScale(255), UpScale(240) },
    { "ivory2", UpScale(238), UpScale(238), UpScale(224) },
    { "ivory3", UpScale(205), UpScale(205), UpScale(193) },
    { "ivory4", UpScale(139), UpScale(139), UpScale(131) },
    { "khaki", UpScale(240), UpScale(230), UpScale(140) },
    { "khaki1", UpScale(255), UpScale(246), UpScale(143) },
    { "khaki2", UpScale(238), UpScale(230), UpScale(133) },
    { "khaki3", UpScale(205), UpScale(198), UpScale(115) },
    { "khaki4", UpScale(139), UpScale(134), UpScale(78) },
    { "lavender", UpScale(230), UpScale(230), UpScale(250) },
    { "lavender blush", UpScale(255), UpScale(240), UpScale(245) },
    { "lavenderblush", UpScale(255), UpScale(240), UpScale(245) },
    { "lavenderblush1", UpScale(255), UpScale(240), UpScale(245) },
    { "lavenderblush2", UpScale(238), UpScale(224), UpScale(229) },
    { "lavenderblush3", UpScale(205), UpScale(193), UpScale(197) },
    { "lavenderblush4", UpScale(139), UpScale(131), UpScale(134) },
    { "lawn green", UpScale(124), UpScale(252), UpScale(0) },
    { "lawngreen", UpScale(124), UpScale(252), UpScale(0) },
    { "lemon chiffon", UpScale(255), UpScale(250), UpScale(205) },
    { "lemonchiffon", UpScale(255), UpScale(250), UpScale(205) },
    { "lemonchiffon1", UpScale(255), UpScale(250), UpScale(205) },
    { "lemonchiffon2", UpScale(238), UpScale(233), UpScale(191) },
    { "lemonchiffon3", UpScale(205), UpScale(201), UpScale(165) },
    { "lemonchiffon4", UpScale(139), UpScale(137), UpScale(112) },
    { "light blue", UpScale(173), UpScale(216), UpScale(230) },
    { "light coral", UpScale(240), UpScale(128), UpScale(128) },
    { "light cyan", UpScale(224), UpScale(255), UpScale(255) },
    { "light goldenrod", UpScale(238), UpScale(221), UpScale(130) },
    { "light goldenrod yellow", UpScale(250), UpScale(250), UpScale(210) },
    { "light gray", UpScale(211), UpScale(211), UpScale(211) },
    { "light green", UpScale(144), UpScale(238), UpScale(144) },
    { "light grey", UpScale(211), UpScale(211), UpScale(211) },
    { "light pink", UpScale(255), UpScale(182), UpScale(193) },
    { "light salmon", UpScale(255), UpScale(160), UpScale(122) },
    { "light sea green", UpScale(32), UpScale(178), UpScale(170) },
    { "light sky blue", UpScale(135), UpScale(206), UpScale(250) },
    { "light slate blue", UpScale(132), UpScale(112), UpScale(255) },
    { "light slate gray", UpScale(119), UpScale(136), UpScale(153) },
    { "light slate grey", UpScale(119), UpScale(136), UpScale(153) },
    { "light steel blue", UpScale(176), UpScale(196), UpScale(222) },
    { "light yellow", UpScale(255), UpScale(255), UpScale(224) },
    { "lightblue", UpScale(173), UpScale(216), UpScale(230) },
    { "lightblue1", UpScale(191), UpScale(239), UpScale(255) },
    { "lightblue2", UpScale(178), UpScale(223), UpScale(238) },
    { "lightblue3", UpScale(154), UpScale(192), UpScale(205) },
    { "lightblue4", UpScale(104), UpScale(131), UpScale(139) },
    { "lightcoral", UpScale(240), UpScale(128), UpScale(128) },
    { "lightcyan", UpScale(224), UpScale(255), UpScale(255) },
    { "lightcyan1", UpScale(224), UpScale(255), UpScale(255) },
    { "lightcyan2", UpScale(209), UpScale(238), UpScale(238) },
    { "lightcyan3", UpScale(180), UpScale(205), UpScale(205) },
    { "lightcyan4", UpScale(122), UpScale(139), UpScale(139) },
    { "lightgoldenrod", UpScale(238), UpScale(221), UpScale(130) },
    { "lightgoldenrod1", UpScale(255), UpScale(236), UpScale(139) },
    { "lightgoldenrod2", UpScale(238), UpScale(220), UpScale(130) },
    { "lightgoldenrod3", UpScale(205), UpScale(190), UpScale(112) },
    { "lightgoldenrod4", UpScale(139), UpScale(129), UpScale(76) },
    { "lightgoldenrodyellow", UpScale(250), UpScale(250), UpScale(210) },
    { "lightgray", UpScale(211), UpScale(211), UpScale(211) },
    { "lightgreen", UpScale(144), UpScale(238), UpScale(144) },
    { "lightgrey", UpScale(211), UpScale(211), UpScale(211) },
    { "lightpink", UpScale(255), UpScale(182), UpScale(193) },
    { "lightpink1", UpScale(255), UpScale(174), UpScale(185) },
    { "lightpink2", UpScale(238), UpScale(162), UpScale(173) },
    { "lightpink3", UpScale(205), UpScale(140), UpScale(149) },
    { "lightpink4", UpScale(139), UpScale(95), UpScale(101) },
    { "lightsalmon", UpScale(255), UpScale(160), UpScale(122) },
    { "lightsalmon1", UpScale(255), UpScale(160), UpScale(122) },
    { "lightsalmon2", UpScale(238), UpScale(149), UpScale(114) },
    { "lightsalmon3", UpScale(205), UpScale(129), UpScale(98) },
    { "lightsalmon4", UpScale(139), UpScale(87), UpScale(66) },
    { "lightseagreen", UpScale(32), UpScale(178), UpScale(170) },
    { "lightskyblue", UpScale(135), UpScale(206), UpScale(250) },
    { "lightskyblue1", UpScale(176), UpScale(226), UpScale(255) },
    { "lightskyblue2", UpScale(164), UpScale(211), UpScale(238) },
    { "lightskyblue3", UpScale(141), UpScale(182), UpScale(205) },
    { "lightskyblue4", UpScale(96), UpScale(123), UpScale(139) },
    { "lightslateblue", UpScale(132), UpScale(112), UpScale(255) },
    { "lightslategray", UpScale(119), UpScale(136), UpScale(153) },
    { "lightslategrey", UpScale(119), UpScale(136), UpScale(153) },
    { "lightsteelblue", UpScale(176), UpScale(196), UpScale(222) },
    { "lightsteelblue1", UpScale(202), UpScale(225), UpScale(255) },
    { "lightsteelblue2", UpScale(188), UpScale(210), UpScale(238) },
    { "lightsteelblue3", UpScale(162), UpScale(181), UpScale(205) },
    { "lightsteelblue4", UpScale(110), UpScale(123), UpScale(139) },
    { "lightyellow", UpScale(255), UpScale(255), UpScale(224) },
    { "lightyellow1", UpScale(255), UpScale(255), UpScale(224) },
    { "lightyellow2", UpScale(238), UpScale(238), UpScale(209) },
    { "lightyellow3", UpScale(205), UpScale(205), UpScale(180) },
    { "lightyellow4", UpScale(139), UpScale(139), UpScale(122) },
    { "lime green", UpScale(50), UpScale(205), UpScale(50) },
    { "limegreen", UpScale(50), UpScale(205), UpScale(50) },
    { "linen", UpScale(250), UpScale(240), UpScale(230) },
    { "magenta", UpScale(255), UpScale(0), UpScale(255) },
    { "magenta1", UpScale(255), UpScale(0), UpScale(255) },
    { "magenta2", UpScale(238), UpScale(0), UpScale(238) },
    { "magenta3", UpScale(205), UpScale(0), UpScale(205) },
    { "magenta4", UpScale(139), UpScale(0), UpScale(139) },
    { "maroon", UpScale(176), UpScale(48), UpScale(96) },
    { "maroon1", UpScale(255), UpScale(52), UpScale(179) },
    { "maroon2", UpScale(238), UpScale(48), UpScale(167) },
    { "maroon3", UpScale(205), UpScale(41), UpScale(144) },
    { "maroon4", UpScale(139), UpScale(28), UpScale(98) },
    { "medium aquamarine", UpScale(102), UpScale(205), UpScale(170) },
    { "medium blue", UpScale(0), UpScale(0), UpScale(205) },
    { "medium orchid", UpScale(186), UpScale(85), UpScale(211) },
    { "medium purple", UpScale(147), UpScale(112), UpScale(219) },
    { "medium sea green", UpScale(60), UpScale(179), UpScale(113) },
    { "medium slate blue", UpScale(123), UpScale(104), UpScale(238) },
    { "medium spring green", UpScale(0), UpScale(250), UpScale(154) },
    { "medium turquoise", UpScale(72), UpScale(209), UpScale(204) },
    { "medium violet red", UpScale(199), UpScale(21), UpScale(133) },
    { "mediumaquamarine", UpScale(102), UpScale(205), UpScale(170) },
    { "mediumblue", UpScale(0), UpScale(0), UpScale(205) },
    { "mediumorchid", UpScale(186), UpScale(85), UpScale(211) },
    { "mediumorchid1", UpScale(224), UpScale(102), UpScale(255) },
    { "mediumorchid2", UpScale(209), UpScale(95), UpScale(238) },
    { "mediumorchid3", UpScale(180), UpScale(82), UpScale(205) },
    { "mediumorchid4", UpScale(122), UpScale(55), UpScale(139) },
    { "mediumpurple", UpScale(147), UpScale(112), UpScale(219) },
    { "mediumpurple1", UpScale(171), UpScale(130), UpScale(255) },
    { "mediumpurple2", UpScale(159), UpScale(121), UpScale(238) },
    { "mediumpurple3", UpScale(137), UpScale(104), UpScale(205) },
    { "mediumpurple4", UpScale(93), UpScale(71), UpScale(139) },
    { "mediumseagreen", UpScale(60), UpScale(179), UpScale(113) },
    { "mediumslateblue", UpScale(123), UpScale(104), UpScale(238) },
    { "mediumspringgreen", UpScale(0), UpScale(250), UpScale(154) },
    { "mediumturquoise", UpScale(72), UpScale(209), UpScale(204) },
    { "mediumvioletred", UpScale(199), UpScale(21), UpScale(133) },
    { "midnight blue", UpScale(25), UpScale(25), UpScale(112) },
    { "midnightblue", UpScale(25), UpScale(25), UpScale(112) },
    { "mint cream", UpScale(245), UpScale(255), UpScale(250) },
    { "mintcream", UpScale(245), UpScale(255), UpScale(250) },
    { "misty rose", UpScale(255), UpScale(228), UpScale(225) },
    { "mistyrose", UpScale(255), UpScale(228), UpScale(225) },
    { "mistyrose1", UpScale(255), UpScale(228), UpScale(225) },
    { "mistyrose2", UpScale(238), UpScale(213), UpScale(210) },
    { "mistyrose3", UpScale(205), UpScale(183), UpScale(181) },
    { "mistyrose4", UpScale(139), UpScale(125), UpScale(123) },
    { "moccasin", UpScale(255), UpScale(228), UpScale(181) },
    { "navajo white", UpScale(255), UpScale(222), UpScale(173) },
    { "navajowhite", UpScale(255), UpScale(222), UpScale(173) },
    { "navajowhite1", UpScale(255), UpScale(222), UpScale(173) },
    { "navajowhite2", UpScale(238), UpScale(207), UpScale(161) },
    { "navajowhite3", UpScale(205), UpScale(179), UpScale(139) },
    { "navajowhite4", UpScale(139), UpScale(121), UpScale(94) },
    { "navy", UpScale(0), UpScale(0), UpScale(128) },
    { "navy blue", UpScale(0), UpScale(0), UpScale(128) },
    { "navyblue", UpScale(0), UpScale(0), UpScale(128) },
    { "old lace", UpScale(253), UpScale(245), UpScale(230) },
    { "oldlace", UpScale(253), UpScale(245), UpScale(230) },
    { "olive drab", UpScale(107), UpScale(142), UpScale(35) },
    { "olivedrab", UpScale(107), UpScale(142), UpScale(35) },
    { "olivedrab1", UpScale(192), UpScale(255), UpScale(62) },
    { "olivedrab2", UpScale(179), UpScale(238), UpScale(58) },
    { "olivedrab3", UpScale(154), UpScale(205), UpScale(50) },
    { "olivedrab4", UpScale(105), UpScale(139), UpScale(34) },
    { "orange", UpScale(255), UpScale(165), UpScale(0) },
    { "orange red", UpScale(255), UpScale(69), UpScale(0) },
    { "orange1", UpScale(255), UpScale(165), UpScale(0) },
    { "orange2", UpScale(238), UpScale(154), UpScale(0) },
    { "orange3", UpScale(205), UpScale(133), UpScale(0) },
    { "orange4", UpScale(139), UpScale(90), UpScale(0) },
    { "orangered", UpScale(255), UpScale(69), UpScale(0) },
    { "orangered1", UpScale(255), UpScale(69), UpScale(0) },
    { "orangered2", UpScale(238), UpScale(64), UpScale(0) },
    { "orangered3", UpScale(205), UpScale(55), UpScale(0) },
    { "orangered4", UpScale(139), UpScale(37), UpScale(0) },
    { "orchid", UpScale(218), UpScale(112), UpScale(214) },
    { "orchid1", UpScale(255), UpScale(131), UpScale(250) },
    { "orchid2", UpScale(238), UpScale(122), UpScale(233) },
    { "orchid3", UpScale(205), UpScale(105), UpScale(201) },
    { "orchid4", UpScale(139), UpScale(71), UpScale(137) },
    { "pale goldenrod", UpScale(238), UpScale(232), UpScale(170) },
    { "pale green", UpScale(152), UpScale(251), UpScale(152) },
    { "pale turquoise", UpScale(175), UpScale(238), UpScale(238) },
    { "pale violet red", UpScale(219), UpScale(112), UpScale(147) },
    { "palegoldenrod", UpScale(238), UpScale(232), UpScale(170) },
    { "palegreen", UpScale(152), UpScale(251), UpScale(152) },
    { "palegreen1", UpScale(154), UpScale(255), UpScale(154) },
    { "palegreen2", UpScale(144), UpScale(238), UpScale(144) },
    { "palegreen3", UpScale(124), UpScale(205), UpScale(124) },
    { "palegreen4", UpScale(84), UpScale(139), UpScale(84) },
    { "paleturquoise", UpScale(175), UpScale(238), UpScale(238) },
    { "paleturquoise1", UpScale(187), UpScale(255), UpScale(255) },
    { "paleturquoise2", UpScale(174), UpScale(238), UpScale(238) },
    { "paleturquoise3", UpScale(150), UpScale(205), UpScale(205) },
    { "paleturquoise4", UpScale(102), UpScale(139), UpScale(139) },
    { "palevioletred", UpScale(219), UpScale(112), UpScale(147) },
    { "palevioletred1", UpScale(255), UpScale(130), UpScale(171) },
    { "palevioletred2", UpScale(238), UpScale(121), UpScale(159) },
    { "palevioletred3", UpScale(205), UpScale(104), UpScale(137) },
    { "palevioletred4", UpScale(139), UpScale(71), UpScale(93) },
    { "papaya whip", UpScale(255), UpScale(239), UpScale(213) },
    { "papayawhip", UpScale(255), UpScale(239), UpScale(213) },
    { "peach puff", UpScale(255), UpScale(218), UpScale(185) },
    { "peachpuff", UpScale(255), UpScale(218), UpScale(185) },
    { "peachpuff1", UpScale(255), UpScale(218), UpScale(185) },
    { "peachpuff2", UpScale(238), UpScale(203), UpScale(173) },
    { "peachpuff3", UpScale(205), UpScale(175), UpScale(149) },
    { "peachpuff4", UpScale(139), UpScale(119), UpScale(101) },
    { "peru", UpScale(205), UpScale(133), UpScale(63) },
    { "pink", UpScale(255), UpScale(192), UpScale(203) },
    { "pink1", UpScale(255), UpScale(181), UpScale(197) },
    { "pink2", UpScale(238), UpScale(169), UpScale(184) },
    { "pink3", UpScale(205), UpScale(145), UpScale(158) },
    { "pink4", UpScale(139), UpScale(99), UpScale(108) },
    { "plum", UpScale(221), UpScale(160), UpScale(221) },
    { "plum1", UpScale(255), UpScale(187), UpScale(255) },
    { "plum2", UpScale(238), UpScale(174), UpScale(238) },
    { "plum3", UpScale(205), UpScale(150), UpScale(205) },
    { "plum4", UpScale(139), UpScale(102), UpScale(139) },
    { "powder blue", UpScale(176), UpScale(224), UpScale(230) },
    { "powderblue", UpScale(176), UpScale(224), UpScale(230) },
    { "purple", UpScale(160), UpScale(32), UpScale(240) },
    { "purple1", UpScale(155), UpScale(48), UpScale(255) },
    { "purple2", UpScale(145), UpScale(44), UpScale(238) },
    { "purple3", UpScale(125), UpScale(38), UpScale(205) },
    { "purple4", UpScale(85), UpScale(26), UpScale(139) },
    { "red", UpScale(255), UpScale(0), UpScale(0) },
    { "red1", UpScale(255), UpScale(0), UpScale(0) },
    { "red2", UpScale(238), UpScale(0), UpScale(0) },
    { "red3", UpScale(205), UpScale(0), UpScale(0) },
    { "red4", UpScale(139), UpScale(0), UpScale(0) },
    { "rosy brown", UpScale(188), UpScale(143), UpScale(143) },
    { "rosybrown", UpScale(188), UpScale(143), UpScale(143) },
    { "rosybrown1", UpScale(255), UpScale(193), UpScale(193) },
    { "rosybrown2", UpScale(238), UpScale(180), UpScale(180) },
    { "rosybrown3", UpScale(205), UpScale(155), UpScale(155) },
    { "rosybrown4", UpScale(139), UpScale(105), UpScale(105) },
    { "royal blue", UpScale(65), UpScale(105), UpScale(225) },
    { "royalblue", UpScale(65), UpScale(105), UpScale(225) },
    { "royalblue1", UpScale(72), UpScale(118), UpScale(255) },
    { "royalblue2", UpScale(67), UpScale(110), UpScale(238) },
    { "royalblue3", UpScale(58), UpScale(95), UpScale(205) },
    { "royalblue4", UpScale(39), UpScale(64), UpScale(139) },
    { "saddle brown", UpScale(139), UpScale(69), UpScale(19) },
    { "saddlebrown", UpScale(139), UpScale(69), UpScale(19) },
    { "salmon", UpScale(250), UpScale(128), UpScale(114) },
    { "salmon1", UpScale(255), UpScale(140), UpScale(105) },
    { "salmon2", UpScale(238), UpScale(130), UpScale(98) },
    { "salmon3", UpScale(205), UpScale(112), UpScale(84) },
    { "salmon4", UpScale(139), UpScale(76), UpScale(57) },
    { "sandy brown", UpScale(244), UpScale(164), UpScale(96) },
    { "sandybrown", UpScale(244), UpScale(164), UpScale(96) },
    { "sea green", UpScale(46), UpScale(139), UpScale(87) },
    { "seagreen", UpScale(46), UpScale(139), UpScale(87) },
    { "seagreen1", UpScale(84), UpScale(255), UpScale(159) },
    { "seagreen2", UpScale(78), UpScale(238), UpScale(148) },
    { "seagreen3", UpScale(67), UpScale(205), UpScale(128) },
    { "seagreen4", UpScale(46), UpScale(139), UpScale(87) },
    { "seashell", UpScale(255), UpScale(245), UpScale(238) },
    { "seashell1", UpScale(255), UpScale(245), UpScale(238) },
    { "seashell2", UpScale(238), UpScale(229), UpScale(222) },
    { "seashell3", UpScale(205), UpScale(197), UpScale(191) },
    { "seashell4", UpScale(139), UpScale(134), UpScale(130) },
    { "sienna", UpScale(160), UpScale(82), UpScale(45) },
    { "sienna1", UpScale(255), UpScale(130), UpScale(71) },
    { "sienna2", UpScale(238), UpScale(121), UpScale(66) },
    { "sienna3", UpScale(205), UpScale(104), UpScale(57) },
    { "sienna4", UpScale(139), UpScale(71), UpScale(38) },
    { "sky blue", UpScale(135), UpScale(206), UpScale(235) },
    { "skyblue", UpScale(135), UpScale(206), UpScale(235) },
    { "skyblue1", UpScale(135), UpScale(206), UpScale(255) },
    { "skyblue2", UpScale(126), UpScale(192), UpScale(238) },
    { "skyblue3", UpScale(108), UpScale(166), UpScale(205) },
    { "skyblue4", UpScale(74), UpScale(112), UpScale(139) },
    { "slate blue", UpScale(106), UpScale(90), UpScale(205) },
    { "slate gray", UpScale(112), UpScale(128), UpScale(144) },
    { "slate grey", UpScale(112), UpScale(128), UpScale(144) },
    { "slateblue", UpScale(106), UpScale(90), UpScale(205) },
    { "slateblue1", UpScale(131), UpScale(111), UpScale(255) },
    { "slateblue2", UpScale(122), UpScale(103), UpScale(238) },
    { "slateblue3", UpScale(105), UpScale(89), UpScale(205) },
    { "slateblue4", UpScale(71), UpScale(60), UpScale(139) },
    { "slategray", UpScale(112), UpScale(128), UpScale(144) },
    { "slategray1", UpScale(198), UpScale(226), UpScale(255) },
    { "slategray2", UpScale(185), UpScale(211), UpScale(238) },
    { "slategray3", UpScale(159), UpScale(182), UpScale(205) },
    { "slategray4", UpScale(108), UpScale(123), UpScale(139) },
    { "slategrey", UpScale(112), UpScale(128), UpScale(144) },
    { "snow", UpScale(255), UpScale(250), UpScale(250) },
    { "snow1", UpScale(255), UpScale(250), UpScale(250) },
    { "snow2", UpScale(238), UpScale(233), UpScale(233) },
    { "snow3", UpScale(205), UpScale(201), UpScale(201) },
    { "snow4", UpScale(139), UpScale(137), UpScale(137) },
    { "spring green", UpScale(0), UpScale(255), UpScale(127) },
    { "springgreen", UpScale(0), UpScale(255), UpScale(127) },
    { "springgreen1", UpScale(0), UpScale(255), UpScale(127) },
    { "springgreen2", UpScale(0), UpScale(238), UpScale(118) },
    { "springgreen3", UpScale(0), UpScale(205), UpScale(102) },
    { "springgreen4", UpScale(0), UpScale(139), UpScale(69) },
    { "steel blue", UpScale(70), UpScale(130), UpScale(180) },
    { "steelblue", UpScale(70), UpScale(130), UpScale(180) },
    { "steelblue1", UpScale(99), UpScale(184), UpScale(255) },
    { "steelblue2", UpScale(92), UpScale(172), UpScale(238) },
    { "steelblue3", UpScale(79), UpScale(148), UpScale(205) },
    { "steelblue4", UpScale(54), UpScale(100), UpScale(139) },
    { "tan", UpScale(210), UpScale(180), UpScale(140) },
    { "tan1", UpScale(255), UpScale(165), UpScale(79) },
    { "tan2", UpScale(238), UpScale(154), UpScale(73) },
    { "tan3", UpScale(205), UpScale(133), UpScale(63) },
    { "tan4", UpScale(139), UpScale(90), UpScale(43) },
    { "thistle", UpScale(216), UpScale(191), UpScale(216) },
    { "thistle1", UpScale(255), UpScale(225), UpScale(255) },
    { "thistle2", UpScale(238), UpScale(210), UpScale(238) },
    { "thistle3", UpScale(205), UpScale(181), UpScale(205) },
    { "thistle4", UpScale(139), UpScale(123), UpScale(139) },
    { "tomato", UpScale(255), UpScale(99), UpScale(71) },
    { "tomato1", UpScale(255), UpScale(99), UpScale(71) },
    { "tomato2", UpScale(238), UpScale(92), UpScale(66) },
    { "tomato3", UpScale(205), UpScale(79), UpScale(57) },
    { "tomato4", UpScale(139), UpScale(54), UpScale(38) },
    { "turquoise", UpScale(64), UpScale(224), UpScale(208) },
    { "turquoise1", UpScale(0), UpScale(245), UpScale(255) },
    { "turquoise2", UpScale(0), UpScale(229), UpScale(238) },
    { "turquoise3", UpScale(0), UpScale(197), UpScale(205) },
    { "turquoise4", UpScale(0), UpScale(134), UpScale(139) },
    { "violet", UpScale(238), UpScale(130), UpScale(238) },
    { "violet red", UpScale(208), UpScale(32), UpScale(144) },
    { "violetred", UpScale(208), UpScale(32), UpScale(144) },
    { "violetred1", UpScale(255), UpScale(62), UpScale(150) },
    { "violetred2", UpScale(238), UpScale(58), UpScale(140) },
    { "violetred3", UpScale(205), UpScale(50), UpScale(120) },
    { "violetred4", UpScale(139), UpScale(34), UpScale(82) },
    { "wheat", UpScale(245), UpScale(222), UpScale(179) },
    { "wheat1", UpScale(255), UpScale(231), UpScale(186) },
    { "wheat2", UpScale(238), UpScale(216), UpScale(174) },
    { "wheat3", UpScale(205), UpScale(186), UpScale(150) },
    { "wheat4", UpScale(139), UpScale(126), UpScale(102) },
    { "white", UpScale(255), UpScale(255), UpScale(255) },
    { "white smoke", UpScale(245), UpScale(245), UpScale(245) },
    { "whitesmoke", UpScale(245), UpScale(245), UpScale(245) },
    { "yellow", UpScale(255), UpScale(255), UpScale(0) },
    { "yellow green", UpScale(154), UpScale(205), UpScale(50) },
    { "yellow1", UpScale(255), UpScale(255), UpScale(0) },
    { "yellow2", UpScale(238), UpScale(238), UpScale(0) },
    { "yellow3", UpScale(205), UpScale(205), UpScale(0) },
    { "yellow4", UpScale(139), UpScale(139), UpScale(0) },
    { "yellowgreen", UpScale(154), UpScale(205), UpScale(50) },
    { (char *) NULL, 0, 0, 0 }
  };

const ColorlistInfo
  XPMColorlist[235] =
  {
    { "AliceBlue", UpScale(240), UpScale(248), UpScale(255) },
    { "AntiqueWhite", UpScale(250), UpScale(235), UpScale(215) },
    { "Aquamarine", UpScale(50), UpScale(191), UpScale(193) },
    { "Azure", UpScale(240), UpScale(255), UpScale(255) },
    { "Beige", UpScale(245), UpScale(245), UpScale(220) },
    { "Bisque", UpScale(255), UpScale(228), UpScale(196) },
    { "Black", UpScale(0), UpScale(0), UpScale(0) },
    { "BlanchedAlmond", UpScale(255), UpScale(235), UpScale(205) },
    { "Blue", UpScale(0), UpScale(0), UpScale(255) },
    { "BlueViolet", UpScale(138), UpScale(43), UpScale(226) },
    { "Brown", UpScale(165), UpScale(42), UpScale(42) },
    { "burlywood", UpScale(222), UpScale(184), UpScale(135) },
    { "CadetBlue", UpScale(95), UpScale(146), UpScale(158) },
    { "chartreuse", UpScale(127), UpScale(255), UpScale(0) },
    { "chocolate", UpScale(210), UpScale(105), UpScale(30) },
    { "Coral", UpScale(255), UpScale(114), UpScale(86) },
    { "CornflowerBlue", UpScale(34), UpScale(34), UpScale(152) },
    { "cornsilk", UpScale(255), UpScale(248), UpScale(220) },
    { "Cyan", UpScale(0), UpScale(255), UpScale(255) },
    { "DarkGoldenrod", UpScale(184), UpScale(134), UpScale(11) },
    { "DarkGreen", UpScale(0), UpScale(86), UpScale(45) },
    { "DarkKhaki", UpScale(189), UpScale(183), UpScale(107) },
    { "DarkOliveGreen", UpScale(85), UpScale(86), UpScale(47) },
    { "DarkOrange", UpScale(255), UpScale(140), UpScale(0) },
    { "DarkOrchid", UpScale(139), UpScale(32), UpScale(139) },
    { "DarkSalmon", UpScale(233), UpScale(150), UpScale(122) },
    { "DarkSeaGreen", UpScale(143), UpScale(188), UpScale(143) },
    { "DarkSlateBlue", UpScale(56), UpScale(75), UpScale(102) },
    { "DarkSlateGray", UpScale(47), UpScale(79), UpScale(79) },
    { "DarkTurquoise", UpScale(0), UpScale(166), UpScale(166) },
    { "DarkViolet", UpScale(148), UpScale(0), UpScale(211) },
    { "DeepPink", UpScale(255), UpScale(20), UpScale(147) },
    { "DeepSkyBlue", UpScale(0), UpScale(191), UpScale(255) },
    { "DimGray", UpScale(84), UpScale(84), UpScale(84) },
    { "DodgerBlue", UpScale(30), UpScale(144), UpScale(255) },
    { "Firebrick", UpScale(142), UpScale(35), UpScale(35) },
    { "FloralWhite", UpScale(255), UpScale(250), UpScale(240) },
    { "ForestGreen", UpScale(80), UpScale(159), UpScale(105) },
    { "gainsboro", UpScale(220), UpScale(220), UpScale(220) },
    { "GhostWhite", UpScale(248), UpScale(248), UpScale(255) },
    { "Gold", UpScale(218), UpScale(170), UpScale(0) },
    { "Goldenrod", UpScale(239), UpScale(223), UpScale(132) },
    { "Gray", UpScale(126), UpScale(126), UpScale(126) },
    { "Green", UpScale(0), UpScale(255), UpScale(0) },
    { "GreenYellow", UpScale(173), UpScale(255), UpScale(47) },
    { "honeydew", UpScale(240), UpScale(255), UpScale(240) },
    { "HotPink", UpScale(255), UpScale(105), UpScale(180) },
    { "IndianRed", UpScale(107), UpScale(57), UpScale(57) },
    { "ivory", UpScale(255), UpScale(255), UpScale(240) },
    { "Khaki", UpScale(179), UpScale(179), UpScale(126) },
    { "lavender", UpScale(230), UpScale(230), UpScale(250) },
    { "LavenderBlush", UpScale(255), UpScale(240), UpScale(245) },
    { "LawnGreen", UpScale(124), UpScale(252), UpScale(0) },
    { "LemonChiffon", UpScale(255), UpScale(250), UpScale(205) },
    { "LightBlue", UpScale(176), UpScale(226), UpScale(255) },
    { "LightCoral", UpScale(240), UpScale(128), UpScale(128) },
    { "LightCyan", UpScale(224), UpScale(255), UpScale(255) },
    { "LightGoldenrod", UpScale(238), UpScale(221), UpScale(130) },
    { "LightGoldenrodYellow", UpScale(250), UpScale(250), UpScale(210) },
    { "LightGray", UpScale(168), UpScale(168), UpScale(168) },
    { "LightPink", UpScale(255), UpScale(182), UpScale(193) },
    { "LightSalmon", UpScale(255), UpScale(160), UpScale(122) },
    { "LightSeaGreen", UpScale(32), UpScale(178), UpScale(170) },
    { "LightSkyBlue", UpScale(135), UpScale(206), UpScale(250) },
    { "LightSlateBlue", UpScale(132), UpScale(112), UpScale(255) },
    { "LightSlateGray", UpScale(119), UpScale(136), UpScale(153) },
    { "LightSteelBlue", UpScale(124), UpScale(152), UpScale(211) },
    { "LightYellow", UpScale(255), UpScale(255), UpScale(224) },
    { "LimeGreen", UpScale(0), UpScale(175), UpScale(20) },
    { "linen", UpScale(250), UpScale(240), UpScale(230) },
    { "Magenta", UpScale(255), UpScale(0), UpScale(255) },
    { "Maroon", UpScale(143), UpScale(0), UpScale(82) },
    { "MediumAquamarine", UpScale(0), UpScale(147), UpScale(143) },
    { "MediumBlue", UpScale(50), UpScale(50), UpScale(204) },
    { "MediumForestGreen", UpScale(50), UpScale(129), UpScale(75) },
    { "MediumGoldenrod", UpScale(209), UpScale(193), UpScale(102) },
    { "MediumOrchid", UpScale(189), UpScale(82), UpScale(189) },
    { "MediumPurple", UpScale(147), UpScale(112), UpScale(219) },
    { "MediumSeaGreen", UpScale(52), UpScale(119), UpScale(102) },
    { "MediumSlateBlue", UpScale(106), UpScale(106), UpScale(141) },
    { "MediumSpringGreen", UpScale(35), UpScale(142), UpScale(35) },
    { "MediumTurquoise", UpScale(0), UpScale(210), UpScale(210) },
    { "MediumVioletRed", UpScale(213), UpScale(32), UpScale(121) },
    { "MidnightBlue", UpScale(47), UpScale(47), UpScale(100) },
    { "MintCream", UpScale(245), UpScale(255), UpScale(250) },
    { "MistyRose", UpScale(255), UpScale(228), UpScale(225) },
    { "moccasin", UpScale(255), UpScale(228), UpScale(181) },
    { "NavajoWhite", UpScale(255), UpScale(222), UpScale(173) },
    { "Navy", UpScale(35), UpScale(35), UpScale(117) },
    { "NavyBlue", UpScale(35), UpScale(35), UpScale(117) },
    { "OldLace", UpScale(253), UpScale(245), UpScale(230) },
    { "OliveDrab", UpScale(107), UpScale(142), UpScale(35) },
    { "Orange", UpScale(255), UpScale(135), UpScale(0) },
    { "OrangeRed", UpScale(255), UpScale(69), UpScale(0) },
    { "Orchid", UpScale(239), UpScale(132), UpScale(239) },
    { "PaleGoldenrod", UpScale(238), UpScale(232), UpScale(170) },
    { "PaleGreen", UpScale(115), UpScale(222), UpScale(120) },
    { "PaleTurquoise", UpScale(175), UpScale(238), UpScale(238) },
    { "PaleVioletRed", UpScale(219), UpScale(112), UpScale(147) },
    { "PapayaWhip", UpScale(255), UpScale(239), UpScale(213) },
    { "PeachPuff", UpScale(255), UpScale(218), UpScale(185) },
    { "peru", UpScale(205), UpScale(133), UpScale(63) },
    { "Pink", UpScale(255), UpScale(181), UpScale(197) },
    { "Plum", UpScale(197), UpScale(72), UpScale(155) },
    { "PowderBlue", UpScale(176), UpScale(224), UpScale(230) },
    { "purple", UpScale(160), UpScale(32), UpScale(240) },
    { "Red", UpScale(255), UpScale(0), UpScale(0) },
    { "RosyBrown", UpScale(188), UpScale(143), UpScale(143) },
    { "RoyalBlue", UpScale(65), UpScale(105), UpScale(225) },
    { "SaddleBrown", UpScale(139), UpScale(69), UpScale(19) },
    { "Salmon", UpScale(233), UpScale(150), UpScale(122) },
    { "SandyBrown", UpScale(244), UpScale(164), UpScale(96) },
    { "SeaGreen", UpScale(82), UpScale(149), UpScale(132) },
    { "seashell", UpScale(255), UpScale(245), UpScale(238) },
    { "Sienna", UpScale(150), UpScale(82), UpScale(45) },
    { "SkyBlue", UpScale(114), UpScale(159), UpScale(255) },
    { "SlateBlue", UpScale(126), UpScale(136), UpScale(171) },
    { "SlateGray", UpScale(112), UpScale(128), UpScale(144) },
    { "snow", UpScale(255), UpScale(250), UpScale(250) },
    { "SpringGreen", UpScale(65), UpScale(172), UpScale(65) },
    { "SteelBlue", UpScale(84), UpScale(112), UpScale(170) },
    { "Tan", UpScale(222), UpScale(184), UpScale(135) },
    { "Thistle", UpScale(216), UpScale(191), UpScale(216) },
    { "tomato", UpScale(255), UpScale(99), UpScale(71) },
    { "Transparent", UpScale(0), UpScale(0), UpScale(1) },
    { "Turquoise", UpScale(25), UpScale(204), UpScale(223) },
    { "Violet", UpScale(156), UpScale(62), UpScale(206) },
    { "VioletRed", UpScale(243), UpScale(62), UpScale(150) },
    { "Wheat", UpScale(245), UpScale(222), UpScale(179) },
    { "White", UpScale(255), UpScale(255), UpScale(255) },
    { "WhiteSmoke", UpScale(245), UpScale(245), UpScale(245) },
    { "Yellow", UpScale(255), UpScale(255), UpScale(0) },
    { "YellowGreen", UpScale(50), UpScale(216), UpScale(56) },
    { "Gray0", UpScale(0), UpScale(0), UpScale(0) },
    { "Gray1", UpScale(3), UpScale(3), UpScale(3) },
    { "Gray10", UpScale(26), UpScale(26), UpScale(26) },
    { "Gray100", UpScale(255), UpScale(255), UpScale(255) },
    { "Gray11", UpScale(28), UpScale(28), UpScale(28) },
    { "Gray12", UpScale(31), UpScale(31), UpScale(31) },
    { "Gray13", UpScale(33), UpScale(33), UpScale(33) },
    { "Gray14", UpScale(36), UpScale(36), UpScale(36) },
    { "Gray15", UpScale(38), UpScale(38), UpScale(38) },
    { "Gray16", UpScale(41), UpScale(41), UpScale(41) },
    { "Gray17", UpScale(43), UpScale(43), UpScale(43) },
    { "Gray18", UpScale(46), UpScale(46), UpScale(46) },
    { "Gray19", UpScale(48), UpScale(48), UpScale(48) },
    { "Gray2", UpScale(5), UpScale(5), UpScale(5) },
    { "Gray20", UpScale(51), UpScale(51), UpScale(51) },
    { "Gray21", UpScale(54), UpScale(54), UpScale(54) },
    { "Gray22", UpScale(56), UpScale(56), UpScale(56) },
    { "Gray23", UpScale(59), UpScale(59), UpScale(59) },
    { "Gray24", UpScale(61), UpScale(61), UpScale(61) },
    { "Gray25", UpScale(64), UpScale(64), UpScale(64) },
    { "Gray26", UpScale(66), UpScale(66), UpScale(66) },
    { "Gray27", UpScale(69), UpScale(69), UpScale(69) },
    { "Gray28", UpScale(71), UpScale(71), UpScale(71) },
    { "Gray29", UpScale(74), UpScale(74), UpScale(74) },
    { "Gray3", UpScale(8), UpScale(8), UpScale(8) },
    { "Gray30", UpScale(77), UpScale(77), UpScale(77) },
    { "Gray31", UpScale(79), UpScale(79), UpScale(79) },
    { "Gray32", UpScale(82), UpScale(82), UpScale(82) },
    { "Gray33", UpScale(84), UpScale(84), UpScale(84) },
    { "Gray34", UpScale(87), UpScale(87), UpScale(87) },
    { "Gray35", UpScale(89), UpScale(89), UpScale(89) },
    { "Gray36", UpScale(92), UpScale(92), UpScale(92) },
    { "Gray37", UpScale(94), UpScale(94), UpScale(94) },
    { "Gray38", UpScale(97), UpScale(97), UpScale(97) },
    { "Gray39", UpScale(99), UpScale(99), UpScale(99) },
    { "Gray4", UpScale(10), UpScale(10), UpScale(10) },
    { "Gray40", UpScale(102), UpScale(102), UpScale(102) },
    { "Gray41", UpScale(105), UpScale(105), UpScale(105) },
    { "Gray42", UpScale(107), UpScale(107), UpScale(107) },
    { "Gray43", UpScale(110), UpScale(110), UpScale(110) },
    { "Gray44", UpScale(112), UpScale(112), UpScale(112) },
    { "Gray45", UpScale(115), UpScale(115), UpScale(115) },
    { "Gray46", UpScale(117), UpScale(117), UpScale(117) },
    { "Gray47", UpScale(120), UpScale(120), UpScale(120) },
    { "Gray48", UpScale(122), UpScale(122), UpScale(122) },
    { "Gray49", UpScale(125), UpScale(125), UpScale(125) },
    { "Gray5", UpScale(13), UpScale(13), UpScale(13) },
    { "Gray50", UpScale(127), UpScale(127), UpScale(127) },
    { "Gray51", UpScale(130), UpScale(130), UpScale(130) },
    { "Gray52", UpScale(133), UpScale(133), UpScale(133) },
    { "Gray53", UpScale(135), UpScale(135), UpScale(135) },
    { "Gray54", UpScale(138), UpScale(138), UpScale(138) },
    { "Gray55", UpScale(140), UpScale(140), UpScale(140) },
    { "Gray56", UpScale(143), UpScale(143), UpScale(143) },
    { "Gray57", UpScale(145), UpScale(145), UpScale(145) },
    { "Gray58", UpScale(148), UpScale(148), UpScale(148) },
    { "Gray59", UpScale(150), UpScale(150), UpScale(150) },
    { "Gray6", UpScale(15), UpScale(15), UpScale(15) },
    { "Gray60", UpScale(153), UpScale(153), UpScale(153) },
    { "Gray61", UpScale(156), UpScale(156), UpScale(156) },
    { "Gray62", UpScale(158), UpScale(158), UpScale(158) },
    { "Gray63", UpScale(161), UpScale(161), UpScale(161) },
    { "Gray64", UpScale(163), UpScale(163), UpScale(163) },
    { "Gray65", UpScale(166), UpScale(166), UpScale(166) },
    { "Gray66", UpScale(168), UpScale(168), UpScale(168) },
    { "Gray67", UpScale(171), UpScale(171), UpScale(171) },
    { "Gray68", UpScale(173), UpScale(173), UpScale(173) },
    { "Gray69", UpScale(176), UpScale(176), UpScale(176) },
    { "Gray7", UpScale(18), UpScale(18), UpScale(18) },
    { "Gray70", UpScale(179), UpScale(179), UpScale(179) },
    { "Gray71", UpScale(181), UpScale(181), UpScale(181) },
    { "Gray72", UpScale(184), UpScale(184), UpScale(184) },
    { "Gray73", UpScale(186), UpScale(186), UpScale(186) },
    { "Gray74", UpScale(189), UpScale(189), UpScale(189) },
    { "Gray75", UpScale(191), UpScale(191), UpScale(191) },
    { "Gray76", UpScale(194), UpScale(194), UpScale(194) },
    { "Gray77", UpScale(196), UpScale(196), UpScale(196) },
    { "Gray78", UpScale(199), UpScale(199), UpScale(199) },
    { "Gray79", UpScale(201), UpScale(201), UpScale(201) },
    { "Gray8", UpScale(20), UpScale(20), UpScale(20) },
    { "Gray80", UpScale(204), UpScale(204), UpScale(204) },
    { "Gray81", UpScale(207), UpScale(207), UpScale(207) },
    { "Gray82", UpScale(209), UpScale(209), UpScale(209) },
    { "Gray83", UpScale(212), UpScale(212), UpScale(212) },
    { "Gray84", UpScale(214), UpScale(214), UpScale(214) },
    { "Gray85", UpScale(217), UpScale(217), UpScale(217) },
    { "Gray86", UpScale(219), UpScale(219), UpScale(219) },
    { "Gray87", UpScale(222), UpScale(222), UpScale(222) },
    { "Gray88", UpScale(224), UpScale(224), UpScale(224) },
    { "Gray89", UpScale(227), UpScale(227), UpScale(227) },
    { "Gray9", UpScale(23), UpScale(23), UpScale(23) },
    { "Gray90", UpScale(229), UpScale(229), UpScale(229) },
    { "Gray91", UpScale(232), UpScale(232), UpScale(232) },
    { "Gray92", UpScale(235), UpScale(235), UpScale(235) },
    { "Gray93", UpScale(237), UpScale(237), UpScale(237) },
    { "Gray94", UpScale(240), UpScale(240), UpScale(240) },
    { "Gray95", UpScale(242), UpScale(242), UpScale(242) },
    { "Gray96", UpScale(245), UpScale(245), UpScale(245) },
    { "Gray97", UpScale(247), UpScale(247), UpScale(247) },
    { "Gray98", UpScale(250), UpScale(250), UpScale(250) },
    { "Gray99", UpScale(252), UpScale(252), UpScale(252) },
    { (char *) NULL, 0, 0, 0 }
  };

/*
  Forward declarations.
*/
static NodeInfo
  *InitializeNode(CubeInfo *,const unsigned int);

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
Export void CompressColormap(Image *image)
{
  QuantizeInfo
    quantize_info;

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
    FreeMemory((void *) &node_info->list);
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
Export unsigned long GetNumberColors(Image *image,FILE *file)
{
#define NumberColorsImageText  "  Computing image colors...  "

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
  color_cube.node_list=(Nodes *) NULL;
  color_cube.progress=0;
  color_cube.colors=0;
  color_cube.free_nodes=0;
  color_cube.root=InitializeNode(&color_cube,0);
  if (color_cube.root == (NodeInfo *) NULL)
    {
      ThrowException(&image->exception,ResourceLimitWarning,
        "Unable to determine the number of image colors",
        "Memory allocation failed");
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
            node_info->child[id]=InitializeNode(&color_cube,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(&image->exception,ResourceLimitWarning,
                  "Unable to determine the number of image colors",
                  "Memory allocation failed");
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
          node_info->list=(ColorPacket *) AllocateMemory(sizeof(ColorPacket));
        else
          node_info->list=(ColorPacket *)
            ReallocateMemory(node_info->list,(i+1)*sizeof(ColorPacket));
        if (node_info->list == (ColorPacket *) NULL)
          {
            ThrowException(&image->exception,ResourceLimitWarning,
              "Unable to determine the number of image colors",
              "Memory allocation failed");
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
      ProgressMonitor(NumberColorsImageText,y,image->rows);
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
    FreeMemory((void *) &color_cube.node_list);
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
#define HistogramImageText  "  Computing image histogram...  "

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
        ProgressMonitor(HistogramImageText,color_cube->progress,
          color_cube->colors);
      color_cube->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I n i t i a l i z e N o d e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeNode allocates memory for a new node in the color cube
%  tree and presets all fields to zero.
%
%  The format of the InitializeNode method is:
%
%      node_info=InitializeNode(color_cube,level)
%
%  A description of each parameter follows.
%
%    o color_cube: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the classification the node resides.
%
%
*/
static NodeInfo *InitializeNode(CubeInfo *color_cube,const unsigned int level)
{
  register int
    i;

  NodeInfo
    *node_info;

  if (color_cube->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AllocateMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=color_cube->node_list;
      color_cube->node_list=nodes;
      color_cube->node_info=nodes->nodes;
      color_cube->free_nodes=NodesInAList;
    }
  color_cube->free_nodes--;
  node_info=color_cube->node_info++;
  for (i=0; i < 8; i++)
    node_info->child[i]=(NodeInfo *) NULL;
  node_info->level=level;
  node_info->number_unique=0;
  node_info->list=(ColorPacket *) NULL;
  return(node_info);
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
Export unsigned int IsGrayImage(Image *image)
{
  register int
    i;

  /*
    Determine if image is grayscale.
  */
  assert(image != (Image *) NULL);
  if (!IsPseudoClass(image))
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
Export unsigned int IsMatteImage(Image *image)
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
  if (!image->matte)
    return(False);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      return(False);
    for (x=0; x < (int) image->columns; x++)
    {
      if (p->opacity != Opaque)
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
Export unsigned int IsMonochromeImage(Image *image)
{
  /*
    Determine if image is monochrome.
  */
  assert(image != (Image *) NULL);
  if (!IsGrayImage(image))
    return(False);
  if (image->colors > 2)
    return(False);
  if ((Intensity(image->colormap[0]) != 0) &&
      (Intensity(image->colormap[0]) != MaxRGB))
    return(False);
  if (image->colors == 2)
    if ((Intensity(image->colormap[1]) != 0) &&
        (Intensity(image->colormap[1]) != MaxRGB))
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
Export unsigned int IsPseudoClass(Image *image)
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
  if ((image->class == PseudoClass) && (image->colors <= 256))
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
  color_cube.root=InitializeNode(&color_cube,0);
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
            node_info->child[id]=InitializeNode(&color_cube,level);
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
            node_info->list=(ColorPacket *) AllocateMemory(sizeof(ColorPacket));
          else
            node_info->list=(ColorPacket *)
              ReallocateMemory(node_info->list,(i+1)*sizeof(ColorPacket));
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
      image->class=PseudoClass;
      image->colors=color_cube.colors;
      if (image->colormap == (PixelPacket *) NULL)
        image->colormap=(PixelPacket *)
          AllocateMemory(image->colors*sizeof(PixelPacket));
      else
        image->colormap=(PixelPacket *) ReallocateMemory((char *)
          image->colormap,image->colors*sizeof(PixelPacket));
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
    FreeMemory((void *) &color_cube.node_list);
    color_cube.node_list=nodes;
  } while (color_cube.node_list != (Nodes *) NULL);
  return((image->class == PseudoClass) && (image->colors <= 256));
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
Export unsigned int QueryColorDatabase(const char *target,PixelPacket *color)
{
  int
    blue,
    green,
    left,
    mid,
    opacity,
    red,
    right;

  register int
    i;

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
      color->opacity=Opaque;
      if (opacity >= 0)
        color->opacity=((unsigned long) (MaxRGB*opacity)/((1 << n)-1));
      return(True);
    }
  /*
    Search our internal color database.
  */
  left=0;
  right=NumberXColors-2;
  for (mid=(right+left)/2 ; right != left; mid=(right+left)/2)
  {
    i=Latin1Compare(target,XColorlist[mid].name);
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
    color->red=XColorlist[mid].red;
    color->green=XColorlist[mid].green;
    color->blue=XColorlist[mid].blue;
    color->opacity=Opaque;
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
    color->opacity=Opaque;
    return(status);
  }
#else
  {
    char
      colorname[MaxTextExtent],
      text[MaxTextExtent];

    int
      count;

    static FILE
      *database = (FILE *) NULL;

    /*
      Match color against the X color database.
    */
    if (database == (FILE *) NULL)
      database=fopen(RGBColorDatabase,"r");
    if (database != (FILE *) NULL)
      {
        (void) rewind(database);
        while (fgets(text,MaxTextExtent,database) != (char *) NULL)
        {
          count=sscanf(text,"%d %d %d %[^\n]\n",&red,&green,&blue,colorname);
          if (count != 4)
            continue;
          if (Latin1Compare(colorname,target) == 0)
            {
              color->red=UpScale(red);
              color->green=UpScale(green);
              color->blue=UpScale(blue);
              color->opacity=Opaque;
              return(True);
            }
        }
      }
    return(False);
  }
#endif
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
Export unsigned int QueryColorName(const PixelPacket *color,char *name)
{
  double
    distance,
    distance_squared,
    min_distance;

  register const ColorlistInfo
    *p;

  *name='\0';
  min_distance=0;
  for (p=XColorlist; p->name != (char *) NULL; p++)
  {
    distance=color->red-(int) p->red;
    distance_squared=distance*distance;
    distance=color->green-(int) p->green;
    distance_squared+=distance*distance;
    distance=color->blue-(int) p->blue;
    distance_squared+=distance*distance;
    if ((p == XColorlist) || (distance_squared < min_distance))
      {
        min_distance=distance_squared;
        (void) strcpy(name,p->name);
      }
  }
  if (min_distance != 0.0)
    FormatString(name,HexColorFormat,(unsigned int) color->red,
      (unsigned int) color->green,(unsigned int) color->blue);
  return((unsigned int) min_distance);
}
