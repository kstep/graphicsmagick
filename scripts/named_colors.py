#!/usr/bin/env python
# vim:ts=4:sw=4:expandtab

#* Author: Mark Mitchell
#* Copyright 2008  Mark Mitchell
#* License: see __license__ below.

import sys
import re

import html_fragments

__doc__ = """Creates an HTML page with a table of named colors.

Usage:  named_colors.py  path/to/color.mgk

The HTML is written to stdout.

In the GraphicsMagick source tree, the path is TOPDIR/config/colors.mgk
"""

__copyright__ = "2008, Mark Mitchell"

__license__ = """
Copyright 2008, Mark Mitchell

Permission is hereby granted, free of charge, to any person obtaining
a copy of this Software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

The Software is provided "as is", without warranty of any kind,
express or implied, including but not limited to the warranties of
merchantability, fitness for a particular purpose and noninfringement.
In no event shall the authors or copyright holders be liable for any
claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with Software
or the use or other dealings in the Software.
"""


builtin_named_colors_rgb = [
    ('aliceblue', 240, 248, 255), #f0f8ff
    ('antiquewhite', 250, 235, 215), #faebd7
    ('aqua', 0, 255, 255), #00ffff
    ('aquamarine', 127, 255, 212), #7fffd4
    ('azure', 240, 255, 255), #f0ffff
    ('beige', 245, 245, 220), #f5f5dc
    ('bisque', 255, 228, 196), #ffe4c4
    ('black', 0, 0, 0), #000000
    ('blanchedalmond', 255, 235, 205), #ffebcd
    ('blue', 0, 0, 255), #0000ff
    ('blueviolet', 138, 43, 226), #8a2be2
    ('brown', 165, 42, 42), #a52a2a
    ('burlywood', 222, 184, 135), #deb887
    ('cadetblue', 95, 158, 160), #5f9ea0
    ('chartreuse', 127, 255, 0), #7fff00
    ('chocolate', 210, 105, 30), #d2691e
    ('coral', 255, 127, 80), #ff7f50
    ('cornflowerblue', 100, 149, 237), #6495ed
    ('cornsilk', 255, 248, 220), #fff8dc
    ('crimson', 220, 20, 60), #dc143c
    ('cyan', 0, 255, 255), #00ffff
    ('darkblue', 0, 0, 139), #00008b
    ('darkcyan', 0, 139, 139), #008b8b
    ('darkgoldenrod', 184, 134, 11), #b8860b
    ('darkgray', 169, 169, 169), #a9a9a9
    ('darkgreen', 0, 100, 0), #006400
    ('darkgrey', 169, 169, 169), #a9a9a9
    ('darkkhaki', 189, 183, 107), #bdb76b
    ('darkmagenta', 139, 0, 139), #8b008b
    ('darkolivegreen', 85, 107, 47), #556b2f
    ('darkorange', 255, 140, 0), #ff8c00
    ('darkorchid', 153, 50, 204), #9932cc
    ('darkred', 139, 0, 0), #8b0000
    ('darksalmon', 233, 150, 122), #e9967a
    ('darkseagreen', 143, 188, 143), #8fbc8f
    ('darkslateblue', 72, 61, 139), #483d8b
    ('darkslategray', 47, 79, 79), #2f4f4f
    ('darkslategrey', 47, 79, 79), #2f4f4f
    ('darkturquoise', 0, 206, 209), #00ced1
    ('darkviolet', 148, 0, 211), #9400d3
    ('deeppink', 255, 20, 147), #ff1493
    ('deepskyblue', 0, 191, 255), #00bfff
    ('dimgray', 105, 105, 105), #696969
    ('dimgrey', 105, 105, 105), #696969
    ('dodgerblue', 30, 144, 255), #1e90ff
    ('firebrick', 178, 34, 34), #b22222
    ('floralwhite', 255, 250, 240), #fffaf0
    ('forestgreen', 34, 139, 34), #228b22
    ('fractal', 128, 128, 128), #808080
    ('fuchsia', 255, 0, 255), #ff00ff
    ('gainsboro', 220, 220, 220), #dcdcdc
    ('ghostwhite', 248, 248, 255), #f8f8ff
    ('gold', 255, 215, 0), #ffd700
    ('goldenrod', 218, 165, 32), #daa520
    ('gray0', 0, 0, 0), #000000
    ('gray1', 3, 3, 3), #030303
    ('gray2', 5, 5, 5), #050505
    ('gray3', 8, 8, 8), #080808
    ('gray4', 10, 10, 10), #0a0a0a
    ('gray5', 13, 13, 13), #0d0d0d
    ('gray6', 15, 15, 15), #0f0f0f
    ('gray7', 18, 18, 18), #121212
    ('gray8', 20, 20, 20), #141414
    ('gray9', 23, 23, 23), #171717
    ('gray10', 26, 26, 26), #1a1a1a
    ('gray11', 28, 28, 28), #1c1c1c
    ('gray12', 31, 31, 31), #1f1f1f
    ('gray13', 33, 33, 33), #212121
    ('gray14', 36, 36, 36), #242424
    ('gray15', 38, 38, 38), #262626
    ('gray16', 41, 41, 41), #292929
    ('gray17', 43, 43, 43), #2b2b2b
    ('gray18', 46, 46, 46), #2e2e2e
    ('gray19', 48, 48, 48), #303030
    ('gray20', 51, 51, 51), #333333
    ('gray21', 54, 54, 54), #363636
    ('gray22', 56, 56, 56), #383838
    ('gray23', 59, 59, 59), #3b3b3b
    ('gray24', 61, 61, 61), #3d3d3d
    ('gray25', 64, 64, 64), #404040
    ('gray26', 66, 66, 66), #424242
    ('gray27', 69, 69, 69), #454545
    ('gray28', 71, 71, 71), #474747
    ('gray29', 74, 74, 74), #4a4a4a
    ('gray30', 77, 77, 77), #4d4d4d
    ('gray31', 79, 79, 79), #4f4f4f
    ('gray32', 82, 82, 82), #525252
    ('gray33', 84, 84, 84), #545454
    ('gray34', 87, 87, 87), #575757
    ('gray35', 89, 89, 89), #595959
    ('gray36', 92, 92, 92), #5c5c5c
    ('gray37', 94, 94, 94), #5e5e5e
    ('gray38', 97, 97, 97), #616161
    ('gray39', 99, 99, 99), #636363
    ('gray40', 102, 102, 102), #666666
    ('gray41', 105, 105, 105), #696969
    ('gray42', 107, 107, 107), #6b6b6b
    ('gray43', 110, 110, 110), #6e6e6e
    ('gray44', 112, 112, 112), #707070
    ('gray45', 115, 115, 115), #737373
    ('gray46', 117, 117, 117), #757575
    ('gray47', 120, 120, 120), #787878
    ('gray48', 122, 122, 122), #7a7a7a
    ('gray49', 125, 125, 125), #7d7d7d
    ('gray50', 127, 127, 127), #7f7f7f
    ('gray51', 130, 130, 130), #828282
    ('gray52', 133, 133, 133), #858585
    ('gray53', 135, 135, 135), #878787
    ('gray54', 138, 138, 138), #8a8a8a
    ('gray55', 140, 140, 140), #8c8c8c
    ('gray56', 143, 143, 143), #8f8f8f
    ('gray57', 145, 145, 145), #919191
    ('gray58', 148, 148, 148), #949494
    ('gray59', 150, 150, 150), #969696
    ('gray60', 153, 153, 153), #999999
    ('gray61', 156, 156, 156), #9c9c9c
    ('gray62', 158, 158, 158), #9e9e9e
    ('gray63', 161, 161, 161), #a1a1a1
    ('gray64', 163, 163, 163), #a3a3a3
    ('gray65', 166, 166, 166), #a6a6a6
    ('gray66', 168, 168, 168), #a8a8a8
    ('gray67', 171, 171, 171), #ababab
    ('gray68', 173, 173, 173), #adadad
    ('gray69', 176, 176, 176), #b0b0b0
    ('gray70', 179, 179, 179), #b3b3b3
    ('gray71', 181, 181, 181), #b5b5b5
    ('gray72', 184, 184, 184), #b8b8b8
    ('gray73', 186, 186, 186), #bababa
    ('gray74', 189, 189, 189), #bdbdbd
    ('gray75', 191, 191, 191), #bfbfbf
    ('gray76', 194, 194, 194), #c2c2c2
    ('gray77', 196, 196, 196), #c4c4c4
    ('gray78', 199, 199, 199), #c7c7c7
    ('gray79', 201, 201, 201), #c9c9c9
    ('gray80', 204, 204, 204), #cccccc
    ('gray81', 207, 207, 207), #cfcfcf
    ('gray82', 209, 209, 209), #d1d1d1
    ('gray83', 212, 212, 212), #d4d4d4
    ('gray84', 214, 214, 214), #d6d6d6
    ('gray85', 217, 217, 217), #d9d9d9
    ('gray86', 219, 219, 219), #dbdbdb
    ('gray87', 222, 222, 222), #dedede
    ('gray88', 224, 224, 224), #e0e0e0
    ('gray89', 227, 227, 227), #e3e3e3
    ('gray90', 229, 229, 229), #e5e5e5
    ('gray91', 232, 232, 232), #e8e8e8
    ('gray92', 235, 235, 235), #ebebeb
    ('gray93', 237, 237, 237), #ededed
    ('gray94', 240, 240, 240), #f0f0f0
    ('gray95', 242, 242, 242), #f2f2f2
    ('gray96', 245, 245, 245), #f5f5f5
    ('gray97', 247, 247, 247), #f7f7f7
    ('gray98', 250, 250, 250), #fafafa
    ('gray99', 252, 252, 252), #fcfcfc
    ('gray100', 255, 255, 255), #ffffff
    ('gray', 126, 126, 126), #7e7e7e
    ('green', 0, 128, 0), #008000
    ('greenyellow', 173, 255, 47), #adff2f
    ('grey', 128, 128, 128), #808080
    ('honeydew', 240, 255, 240), #f0fff0
    ('hotpink', 255, 105, 180), #ff69b4
    ('indianred', 205, 92, 92), #cd5c5c
    ('indigo', 75, 0, 130), #4b0082
    ('ivory', 255, 255, 240), #fffff0
    ('khaki', 240, 230, 140), #f0e68c
    ('lavender', 230, 230, 250), #e6e6fa
    ('lavenderblush', 255, 240, 245), #fff0f5
    ('lawngreen', 124, 252, 0), #7cfc00
    ('lemonchiffon', 255, 250, 205), #fffacd
    ('lightblue', 173, 216, 230), #add8e6
    ('lightcoral', 240, 128, 128), #f08080
    ('lightcyan', 224, 255, 255), #e0ffff
    ('lightgoldenrodyellow', 250, 250, 210), #fafad2
    ('lightgray', 211, 211, 211), #d3d3d3
    ('lightgreen', 144, 238, 144), #90ee90
    ('lightgrey', 211, 211, 211), #d3d3d3
    ('lightpink', 255, 182, 193), #ffb6c1
    ('lightsalmon', 255, 160, 122), #ffa07a
    ('lightseagreen', 32, 178, 170), #20b2aa
    ('lightskyblue', 135, 206, 250), #87cefa
    ('lightslategray', 119, 136, 153), #778899
    ('lightslategrey', 119, 136, 153), #778899
    ('lightsteelblue', 176, 196, 222), #b0c4de
    ('lightyellow', 255, 255, 224), #ffffe0
    ('lime', 0, 255, 0), #00ff00
    ('limegreen', 50, 205, 50), #32cd32
    ('linen', 250, 240, 230), #faf0e6
    ('magenta', 255, 0, 255), #ff00ff
    ('maroon', 128, 0, 0), #800000
    ('mediumaquamarine', 102, 205, 170), #66cdaa
    ('mediumblue', 0, 0, 205), #0000cd
    ('mediumorchid', 186, 85, 211), #ba55d3
    ('mediumpurple', 147, 112, 219), #9370db
    ('mediumseagreen', 60, 179, 113), #3cb371
    ('mediumslateblue', 123, 104, 238), #7b68ee
    ('mediumspringgreen', 0, 250, 154), #00fa9a
    ('mediumturquoise', 72, 209, 204), #48d1cc
    ('mediumvioletred', 199, 21, 133), #c71585
    ('midnightblue', 25, 25, 112), #191970
    ('mintcream', 245, 255, 250), #f5fffa
    ('mistyrose', 255, 228, 225), #ffe4e1
    ('moccasin', 255, 228, 181), #ffe4b5
    ('navajowhite', 255, 222, 173), #ffdead
    ('navy', 0, 0, 128), #000080
    ('none', 0, 0, 0), #000000
    ('oldlace', 253, 245, 230), #fdf5e6
    ('olive', 128, 128, 0), #808000
    ('olivedrab', 107, 142, 35), #6b8e23
    ('orange', 255, 165, 0), #ffa500
    ('orangered', 255, 69, 0), #ff4500
    ('orchid', 218, 112, 214), #da70d6
    ('palegoldenrod', 238, 232, 170), #eee8aa
    ('palegreen', 152, 251, 152), #98fb98
    ('paleturquoise', 175, 238, 238), #afeeee
    ('palevioletred', 219, 112, 147), #db7093
    ('papayawhip', 255, 239, 213), #ffefd5
    ('peachpuff', 255, 218, 185), #ffdab9
    ('peru', 205, 133, 63), #cd853f
    ('pink', 255, 192, 203), #ffc0cb
    ('plum', 221, 160, 221), #dda0dd
    ('powderblue', 176, 224, 230), #b0e0e6
    ('purple', 128, 0, 128), #800080
    ('red', 255, 0, 0), #ff0000
    ('rosybrown', 188, 143, 143), #bc8f8f
    ('royalblue', 65, 105, 225), #4169e1
    ('saddlebrown', 139, 69, 19), #8b4513
    ('salmon', 250, 128, 114), #fa8072
    ('sandybrown', 244, 164, 96), #f4a460
    ('seagreen', 46, 139, 87), #2e8b57
    ('seashell', 255, 245, 238), #fff5ee
    ('sienna', 160, 82, 45), #a0522d
    ('silver', 192, 192, 192), #c0c0c0
    ('skyblue', 135, 206, 235), #87ceeb
    ('slateblue', 106, 90, 205), #6a5acd
    ('slategray', 112, 128, 144), #708090
    ('slategrey', 112, 128, 144), #708090
    ('snow', 255, 250, 250), #fffafa
    ('springgreen', 0, 255, 127), #00ff7f
    ('steelblue', 70, 130, 180), #4682b4
    ('tan', 210, 180, 140), #d2b48c
    ('teal', 0, 128, 128), #008080
    ('thistle', 216, 191, 216), #d8bfd8
    ('tomato', 255, 99, 71), #ff6347
    ('turquoise', 64, 224, 208), #40e0d0
    ('violet', 238, 130, 238), #ee82ee
    ('wheat', 245, 222, 179), #f5deb3
    ('white', 255, 255, 255), #ffffff
    ('whitesmoke', 245, 245, 245), #f5f5f5
    ('yellow', 255, 255, 0), #ffff00
    ('yellowgreen', 154, 205, 50), #9acd32
]


html_fragments.url_prefix = ''
banner = html_fragments.make_banner()
nav = html_fragments.make_nav()
footer = html_fragments.make_footer()
stylesheet_url = 'docutils-articles.css'


head = """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
	<meta http-equiv="content-type" content="text/html; charset=iso-8859-1">
	<title>GraphicsMagick Color Selections</title>
	<meta name="description" content="GraphicsMagick is a robust collection of tools
and libraries to read, write, and manipulate an image in any of the more
popular image formats including GIF, JPEG, PNG, PDF, and Photo CD.  With
GraphicsMagick you can create GIFs dynamically making it suitable for Web
applications.  You can also resize, rotate, sharpen, color reduce, or add
special effects to an image and save your completed work in the same or
differing image format.">
    <meta name="keywords" content="GraphicsMagick, Image Magick, Image
Magic, PerlMagick, Perl Magick, Perl Magic, CineMagick, PixelMagick,
Pixel Magic, WebMagick, Web Magic, visualization, image processing,
software development, simulation, image, software, AniMagick, Animagic,
Magick++">
<link rel="stylesheet" href="%(stylesheet_url)s" type="text/css" />
	<style>
	<!--
        body {font-family: sans-serif;}
        table.named-colors th, table.named-colors td { padding : .5ex; }
        table.named-colors th { background-color : #E0E0E0; }
        table.named-colors td.dark { text-align : center; color : white; }
        table.named-colors td.light { text-align : center; color : black; }
	-->
	</style>
</head>
<body>
%(banner)s
%(nav)s
<div class="document" id="graphicsmagick-programming-interfaces">
<hr>
<p>Here are some example color selections to choose from or define
your own. GraphicsMagick understands color names or hex values (.e.g.
white or #ffffff). 
</p>
<table class="named-colors">
<colgroup>
<col width="20%%" />
<col width="40%%" />
<col width="20%%" />
<col width="20%%" />
</colgroup>
<thead valign="bottom">
<tr><th class="head">Name</th>
<th class="head">Color</th>
<th class="head">RGB</th>
<th class="head">Hex</th>
</tr>
</thead>
<tbody valign="top">
""" % { 'stylesheet_url' : stylesheet_url,
        'nav' : nav,
        'banner' : banner,
        'footer' : footer,
      }

tail = """
</tbody>
</table>
</div>
</body>
</html>
"""



def from_magick_file():
    f = file(sys.argv[1])
    first_line = f.readline()
    if not first_line.startswith('<?xml version='):
        print >> sys.stderr, "'%s' does not seem like a color.mgk file"
        sys.exit(1)

    re_colors = re.compile(r'name="([^"]+)"\s+red="([^"]+)"\s+green="([^"]+)"\s+blue="([^"]+)"\s+')
    magick_colors_rgb = []
    for line in f:
        line = line.strip()
        if line.startswith('<color'):
            m = re_colors.search(line)
            if m:
                name, r, g, b = m.group(1), m.group(2), m.group(3), m.group(4)
                magick_colors_rgb.append((name, int(r), int(g), int(b)))
            else:
                pass
    f.close()
    return magick_colors_rgb



if __name__ == '__main__':
    if len(sys.argv) >= 2:
        named_colors_rgb = from_magick_file()
    else:
        named_colors_rgb = builtin_named_colors_rgb

    #<tr><td>Name</td><td>Color</td><td>R,G,B</td><td>Hex</td></tr>
    tr = '<tr><td>%s</td><td class="%s" bgcolor="#%02x%02x%02x">%s</td><td>%s, %s, %s</td><td>#%02x%02x%02x</td></tr>'
    print head
    for name, r, g, b in named_colors_rgb:
        if r+g+b < 320:
            level = 'dark'
        else:
            level = 'light'
        print tr % (name, level, r, g, b, name, r, g, b, r, g, b)
    print tail

