/* libwmf ("ipa/ipa.h"): library for wmf conversion
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


#ifndef WMFIPA_IPA_H
#define WMFIPA_IPA_H

/* bmp.h
 */
#include <gd.h> /* Use GD for BMP -> PNG conversion */

typedef struct _ipa_b64_t   ipa_b64_t;

typedef struct _BMPSource   BMPSource;
typedef struct _BMPData     BMPData;

typedef struct _PointInfo   PointInfo;
typedef struct _BMPInfo     BMPInfo;

static int            ipa_b64_sink (void*,const char*,int);
static void           ipa_b64_flush (void*);
static gdImage*       ipa_bmp_gd (wmfAPI*,wmfBMP_Draw_t*);

static size_t         ReadBlob (BMPSource*,size_t,unsigned char*);
static int            ReadBlobByte (BMPSource*);
static unsigned short ReadBlobLSBShort (BMPSource*);
static unsigned long  ReadBlobLSBLong (BMPSource*);
static long           TellBlob (BMPSource*);
static void           DecodeImage (wmfAPI*,wmfBMP*,BMPSource*,unsigned int,unsigned char*);
static void           ReadBMPImage (wmfAPI*,wmfBMP*,BMPSource*);
static int            ExtractColor (wmfAPI*,wmfBMP*,wmfRGB*,unsigned int,unsigned int);
static void           SetColor (wmfAPI*,wmfBMP*,wmfRGB*,unsigned char,unsigned int,unsigned int);

#define IPA_B64_BUFLEN 57 /* must be multiple of three... */
#define IPA_B64_LEQUIV 78 /* (IPA_B64_BUFLEN * 4) / 3 + 2 */

struct _ipa_b64_t
{	wmfAPI* API;

	wmfStream* out;

	char buffer[IPA_B64_BUFLEN];

	int length;
};

struct _BMPSource
{	unsigned char* begin;
	unsigned char* end;

	unsigned char* ptr;
};

struct _BMPData
{	unsigned int NColors;

	wmfRGB* rgb;

	unsigned char* image;

	unsigned short bits_per_pixel;
	unsigned int   bytes_per_line;

	unsigned short masked;
	unsigned short flipped;
};

struct _PointInfo
{	double x;
	double y;
	double z;
};

struct _BMPInfo
{	unsigned long file_size;
	unsigned long ba_offset;
	unsigned long offset_bits;
	unsigned long size;

	long width;
	long height;

	unsigned short planes;
	unsigned short bits_per_pixel;

	unsigned long compression;
	unsigned long image_size;
	unsigned long x_pixels;
	unsigned long y_pixels;
	unsigned long number_colors;
	unsigned long colors_important;

	unsigned short red_mask;
	unsigned short green_mask;
	unsigned short blue_mask;
	unsigned short alpha_mask;

	long colorspace;

	PointInfo red_primary;
	PointInfo green_primary;
	PointInfo blue_primary;
	PointInfo gamma_scale;
};

/* font.h
 */
static void ipa_font_add_wmf (wmfAPI*,wmfFontMap*);
static void ipa_font_add_sub (wmfAPI*,wmfMapping*);
static void ipa_font_add_ps (wmfAPI*,wmfMapping*);
static void ipa_font_add_cache (wmfAPI*,wmfFT_CacheEntry*);
static void ipa_font_add_api (wmfAPI*,char*);

static float ipa_char_position (wmfAPI*,wmfFont*,char*,char*);

static wmfXML_FontInfo* ipa_font_sys_map (wmfAPI*,wmfFont*);
static FT_Face          ipa_font_sys_face (wmfAPI*,wmfFont*,wmfXML_FontInfo*);

#ifdef HAVE_EXPAT
static void exml_start (void*,const char*,const char**);
static void exmlfontmap_read (wmfAPI*,wmfXML_FontData*,char*);

typedef struct _EXML_FontData EXML_FontData;

struct _EXML_FontData
{	wmfAPI* API;

	wmfXML_FontData* FD;
};
#endif /* HAVE_EXPAT */

#ifdef HAVE_XML2
static void xml2_start (void*,const char*,const char**);
static void xml2fontmap_read (wmfAPI*,wmfXML_FontData*,char*);

typedef struct _XML2_FontData XML2_FontData;

struct _XML2_FontData
{	wmfAPI* API;

	wmfXML_FontData* FD;
};
#endif /* HAVE_XML2 */

static void            ipa_font_gs_file (wmfAPI* API,wmfGS_FontData* FD,char* file);
static void            ipa_font_gs_add (wmfAPI* API,wmfGS_FontData* FD,char* name,char* alias);
static char*           ipa_font_gs_alias (wmfAPI* API,wmfGS_FontData* FD,char* name);
static char*           ipa_font_gs_readline (wmfAPI* API,FILE* in);
static wmfGS_FontInfo* ipa_font_gs_map (wmfAPI* API,wmfFont* font);
static FT_Face         ipa_font_gs_face (wmfAPI* API,wmfFont* font,wmfGS_FontInfo* FI);

static FT_Face ipa_font_face (wmfAPI* API,wmfFont* font,char* ps_name);
static FT_Face ipa_font_face_open (wmfAPI* API,char* ps_name,char* glyphs,char* metrics);
static FT_Face ipa_font_face_cached (wmfAPI* API,wmfFont* font,char* ps_name);

static char* ipa_font_path_find (wmfAPI* API,char* file_name);

static char* ipa_font_std (wmfAPI* API,wmfFont* font);

/* ipa.c
 */
typedef struct _ipa_page_info ipa_page_info;

struct _ipa_page_info
{	wmf_page_t type;

	char* format;

	unsigned int width;
	unsigned int height;
};

#endif /* ! WMFIPA_IPA_H */
