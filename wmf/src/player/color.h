/* libwmf ("player/color.h"): library for wmf conversion
   Copyright (C) 2000 - various; see CREDITS, ChangeLog, and sources

   The libwmf Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The libwmf Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the libwmf Library; see the file COPYING.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */


#ifndef WMFPLAYER_COLOR_H
#define WMFPLAYER_COLOR_H

static wmfRGB rgb (wmfAPI* API,U16 one,U16 two)
{	wmfRGB color;

	color.r = (unsigned char)  (one & 0x00FF);
	color.g = (unsigned char) ((one & 0xFF00) >> 8);
	color.b = (unsigned char)  (two & 0x00FF);

	return (color);
}

/**
 * The color white.
 * 
 * @param API the API handle
 * 
 * @return Returns \b wmf_white.
 */
wmfRGB wmf_rgb_white (wmfAPI* API)
{	wmfRGB color;

	color.r = 0xff;
	color.g = 0xff;
	color.b = 0xff;

	return (color);
}

/**
 * The color black.
 * 
 * @param API the API handle
 * 
 * @return Returns \b wmf_black.
 */
wmfRGB wmf_rgb_black (wmfAPI* API)
{	wmfRGB color;

	color.r = 0;
	color.g = 0;
	color.b = 0;

	return (color);
}

/**
 * Create a color with specified fractions of red, green and blue.
 * 
 * @param API   the API handle
 * @param red   fraction (0 to 1 inclusive) of color red
 * @param green fraction (0 to 1 inclusive) of color green
 * @param blue  fraction (0 to 1 inclusive) of color blue
 * 
 * \b wmf_rgb_color (API,1,1,1) returns \b wmf_white.
 * 
 * \b wmf_rgb_color (API,0,0,0) returns \b wmf_black.
 * 
 * @return Returns the color.
 */
wmfRGB wmf_rgb_color (wmfAPI* API,float red,float green,float blue)
{	wmfRGB color;

	int i_red;
	int i_green;
	int i_blue;

	if (red	  > 1) red   = 1;
	if (green > 1) green = 1;
	if (blue  > 1) blue  = 1;

	if (red	  < 0) red   = 0;
	if (green < 0) green = 0;
	if (blue  < 0) blue  = 0;

	i_red   = (int) (red   * (float) 256);
	i_green = (int) (green * (float) 256);
	i_blue  = (int) (blue  * (float) 256);

	if (i_red   > 255) i_red   = 255;
	if (i_green > 255) i_green = 255;
	if (i_blue  > 255) i_blue  = 255;

	if (i_red   < 0) i_red   = 0;
	if (i_green < 0) i_green = 0;
	if (i_blue  < 0) i_blue  = 0;

	color.r = (unsigned char) i_red;
	color.g = (unsigned char) i_green;
	color.b = (unsigned char) i_blue;

	return (color);
}

#endif /* ! WMFPLAYER_COLOR_H */
