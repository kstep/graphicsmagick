/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  PPPP   M   M                              %
%                             X X   P   P  MM MM                              %
%                              X    PPPP   M M M                              %
%                             X X   P      M   M                              %
%                            X   X  P      M   M                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Constant declaractions.
*/
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
static unsigned int
  WriteXPMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s X P M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsXPM returns True if the image format type, identified by the
%  magick string, is XPM.
%
%  The format of the IsXPM method is:
%
%      unsigned int IsXPM(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsXPM returns True if the image format type is XPM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsXPM(const unsigned char *magick,const unsigned int length)
{
  if (length < 9)
    return(False);
  if (strncmp((char *) magick,"/* XPM */",9) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X P M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXPMImage reads an X11 pixmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXPMImage method is:
%
%      Image *ReadXPMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXPMImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static char *ParseColor(char *data)
{
#define NumberTargets  6

  static const char
    *targets[NumberTargets] = { "c ", "g ", "g4 ", "m ", "b ", "s " };

  register char
     *p,
     *r;

  register const char
     *q;

  register int
    i;

  for (i=0; i < NumberTargets; i++)
  {
    r=data;
    for (q=targets[i]; *r != '\0'; r++)
    {
      if (*r != *q)
        continue;
      if (!isspace((int) (*(r-1))))
        continue;
      p=r;
      for ( ; ; )
      {
        if (*q == '\0')
          return(r);
        if (*p++ != *q++)
          break;
      }
      q=targets[i];
    }
  }
  return((char *) NULL);
}

static Image *ReadXPMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    key[MaxTextExtent],
    **keys,
    target[MaxTextExtent],
    **textlist,
    *xpm_buffer;

  Image
    *image;

  int
    count,
    j,
    none,
    y;

  register char
    *p,
    *q;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *r;

  unsigned int
    length,
    status,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read XPM file.
  */
  length=MaxTextExtent;
  xpm_buffer=(char *) AllocateMemory(length);
  p=xpm_buffer;
  if (xpm_buffer != (char *) NULL)
    while (GetStringBlob(image,p) != (char *) NULL)
    {
      if (*p == '#')
        if ((p == xpm_buffer) || (*(p-1) == '\n'))
          continue;
      if ((*p == '}') && (*(p+1) == ';'))
        break;
      p+=Extent(p);
      if ((p-xpm_buffer+MaxTextExtent+1) < length)
        continue;
      length<<=1;
      xpm_buffer=(char *) ReallocateMemory((char *) xpm_buffer,length);
      if (xpm_buffer == (char *) NULL)
        break;
      p=xpm_buffer+Extent(xpm_buffer);
    }
  if (xpm_buffer == (char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Remove comments.
  */
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if ((*p == '"') || (*p == '\''))
      {
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          for (p++; *p != '\0'; p++)
            if ((*p == '\'') && (*(p-1) != '\\'))
              break;
        if (*p == '\0')
          break;
        continue;
      }
    if ((*p != '/') || (*(p+1) != '*'))
      continue;
    for (q=p+2; *q != '\0'; q++)
      if ((*q == '*') && (*(q+1) == '/'))
        break;
    (void) strcpy(p,q+2);
  }
  /*
    Remove unquoted characters.
  */
  i=0;
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if (*p != '"')
      continue;
    for (q=p+1; *q != '\0'; q++)
      if (*q == '"')
        break;
    (void) strncpy(xpm_buffer+i,p+1,q-p-1);
    i+=q-p-1;
    xpm_buffer[i++]='\n';
    p=q+1;
  }
  xpm_buffer[i]='\0';
  textlist=StringToList(xpm_buffer);
  FreeMemory((void *) &xpm_buffer);
  if (textlist == (char **) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read hints.
  */
  image->class=PseudoClass;
  count=sscanf(textlist[0],"%u %u %u %u",&image->columns,&image->rows,
    &image->colors,&width);
  if ((count != 4) || (width > 2) ||
      ((image->columns*image->rows*image->colors) == 0))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(CorruptImageWarning,"Not a XPM image file",image);
    }
  image->depth=8;
  /*
    Initialize image structure.
  */
  image->colormap=(PixelPacket *)
    AllocateMemory(image->colors*sizeof(PixelPacket));
  keys=(char **) AllocateMemory(image->colors*sizeof(char *));
  if ((image->colormap == (PixelPacket *) NULL) || (keys == (char **) NULL))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    }
  /*
    Read image colormap.
  */
  i=1;
  none=(-1);
  for (x=0; x < (int) image->colors; x++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    keys[x]=(char *) AllocateMemory(width+1);
    if (keys[x] == (char *) NULL)
      {
        for (i=0; textlist[i] != (char *) NULL; i++)
          FreeMemory((void *) &textlist[i]);
        FreeMemory((void *) &textlist);
        FreeMemory((void *) &keys);
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      }
    keys[x][width]='\0';
    (void) strncpy(keys[x],p,width);
    /*
      Parse color.
    */
    (void) strcpy(target,"gray");
    q=ParseColor(p+width);
    if (q != (char *) NULL)
      {
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
        (void) strcpy(target,q);
        q=ParseColor(target);
        if (q != (char *) NULL)
          *q='\0';
      }
    Strip(target);
    if (Latin1Compare(target,"none") == 0)
      {
        image->class=DirectClass;
        image->matte=True;
        none=x;
        (void) strcpy(target,"black");
      }
    (void) QueryColorDatabase(target,&image->colormap[x]);
  }
  if (x < (int) image->colors)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((void *) &textlist[i]);
      FreeMemory((void *) &textlist);
      ThrowReaderException(CorruptImageWarning,"Corrupt XPM image file",image);
    }
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  j=0;
  key[width]='\0';
  for (y=0; y < (int) image->rows; y++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    r=SetImagePixels(image,0,y,image->columns,1);
    if (r == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
    {
      (void) strncpy(key,p,width);
      if (strcmp(key,keys[j]) != 0)
        for (j=0; j < (int) (image->colors-1); j++)
          if (strcmp(key,keys[j]) == 0)
            break;
      if (image->class == PseudoClass)
        indexes[x]=j;
      *r=image->colormap[j];
      r->opacity=j == none ? Transparent : Opaque;
      r++;
      p+=width;
    }
    if (!SyncImagePixels(image))
      break;
  }
  /*
    Free resources.
  */
  for (x=0; x < (int) image->colors; x++)
    FreeMemory((void *) &keys[x]);
  FreeMemory((void *) &keys);
  for (i=0; textlist[i] != (char *) NULL; i++)
    FreeMemory((void *) &textlist[i]);
  FreeMemory((void *) &textlist);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X P M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXPMImage adds attributes for the XPM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXPMImage method is:
%
%      RegisterXPMImage(void)
%
*/
Export void RegisterXPMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("XPM");
  entry->decoder=ReadXPMImage;
  entry->encoder=WriteXPMImage;
  entry->magick=IsXPM;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system pixmap (color)");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X P M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXPMImage writes an image to a file in the X pixmap format.
%
%  The format of the WriteXPMImage method is:
%
%      unsigned int WriteXPMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXPMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteXPMImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    buffer[MaxTextExtent],
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  double
    distance,
    distance_squared,
    min_distance;

  int
    j,
    k,
    y;

  long
    mean;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p;

  register const ColorlistInfo
    *q;

  unsigned int
    characters_per_pixel,
    colors,
    status,
    transparent;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  transparent=False;
  if (image->class == PseudoClass)
    colors=image->colors;
  else
    {
      QuantizeInfo
        quantize_info;

      /*
        Convert DirectClass to PseudoClass image.
      */
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              p->opacity=p->opacity == Transparent;
              if (p->opacity == Transparent)
                transparent=True;
              p++;
            }
            if (!SyncImagePixels(image))
              break;
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      colors=image->colors;
      if (transparent)
        {
          colors++;
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (int) image->columns; x++)
            {
              if (p->opacity)
                indexes[x]=image->colors;
              p++;
            }
            if (!SyncImagePixels(image))
              break;
          }
        }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (int) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) strcpy(buffer,"/* XPM */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"static char *magick[] = {\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"/* columns rows colors chars-per-pixel */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  FormatString(buffer,"\"%u %u %u %d\",\n",image->columns,
    image->rows,colors,characters_per_pixel);
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (i=0; i < (int) colors; i++)
  {
    PixelPacket
      *p;

    /*
      Define XPM color.
    */
    min_distance=0;
    p=image->colormap+i;
    FormatString(name,HexColorFormat,(unsigned int) p->red,
      (unsigned int) p->green,(unsigned int) p->blue);
    for (q=XPMColorlist; q->name != (char *) NULL; q++)
    {
      mean=(p->red+(int) q->red)/2;
      distance=p->red-(int) q->red;
      distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
      distance=p->green-(int) q->green;
      distance_squared+=4.0*(distance*distance);
      distance=p->blue-(int) q->blue;
      distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
      if ((q == XPMColorlist) || (distance_squared <= min_distance))
        {
          min_distance=distance_squared;
          if (min_distance == 0.0)
            (void) strcpy(name,q->name);
        }
    }
    if (transparent)
      if (i == (int) (colors-1))
        (void) strcpy(name,"None");
    /*
      Write XPM color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    FormatString(buffer,"\"%.1024s c %.1024s\",\n",symbol,name);
    (void) WriteBlob(image,strlen(buffer),buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) strcpy(buffer,"/* pixels */\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) strcpy(buffer,"\"");
    (void) WriteBlob(image,strlen(buffer),buffer);
    for (x=0; x < (int) image->columns; x++)
    {
      k=indexes[x] % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) indexes[x]-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      FormatString(buffer,"%.1024s",symbol);
      (void) WriteBlob(image,strlen(buffer),buffer);
      p++;
    }
    FormatString(buffer,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? "" : ","));
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SaveImageText,y,image->rows);
  }
  (void) strcpy(buffer,"};\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  CloseBlob(image);
  return(True);
}
