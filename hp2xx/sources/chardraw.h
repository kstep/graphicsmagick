/*
   Copyright (c) 1991 - 1994 Heinz W. Werntges.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of HP2xx.

HP2xx is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the GNU General Public License, Version 2 or later, for full details.

Everyone is granted permission to copy, modify and redistribute
HP2xx, but only under the conditions described in the GNU General Public
License.  A copy of this license is supposed to have been
given to you along with HP2xx so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

/** chardraw.h
 **
 ** 92/02/19  V 1.01  HWW  Derived from sprite.h (V 2.01)
 ** 92/05/28  V 1.02  HWW  plot_symbol_char() added
 ** 99/02/01  V 1.03  MK   charsets 5 and 7 added
 **
 ** Structure definition as used by pplib for character drawing
 **/


#define JOFF    4

#define	_BS	'\010'		/* Backspace		*/
#define	_HT	'\011'		/* Horizontal Tab	*/
#define	_LF	'\012'		/* Line Feed		*/
#define	_VT	'\013'		/* Vertical Tab		*/
#define	_CR	'\015'		/* Carriage Return	*/
#define	_SO	'\016'		/* Shift Out		*/
#define	_SI	'\017'		/* Shift In		*/

/**
 ** Description of struct TextPar (used for internal font drawing):
 **
 ** A character size is defined by its WIDTH and HEIGHT.
 ** The distance from char. to char. is given by SPACE, from
 ** text line to text line by LINE.
 ** WIDTH, SPACE are fractions of the P1,P2 window width,
 ** SPACE, LINE  are fractions of the P1,P2 window height.
 ** The writing direction is DIR, which is the angle [0,2*M_PI]
 ** between text line & x direction.
 ** Set SLANT to an angle != 0 if characters are to appear e.g. italics-like.
 ** Stroked fonts selectable by setting FONT to > 0
 **      (not yet supported).
 **
 ** NOTE: struct TextPar was inherited from former project "plib" (a plot
 **       library which also featured character drawing). It was not
 **       designed from scratch for the purpose of HP-GL font management.
 **       However, for charset 0 it does a fair job here, mainly because
 **       plib itself had been inspired by HP-GL.
 **/

typedef struct
{
  float width;	/* Width of a char (x dirc.)		*/
  float height;	/* Height of a char (y dirc.)		*/
  float space;	/* Distance between characters		*/
  float line;	/* Distance betw. char. lines		*/
  float espace;	/* Extra char space rel. to 'space'	*/
  float eline;	/* Extra line space rel. to 'line'	*/
  float dir;	/* Direction to x axis (rad)		*/
  float slant;	/* Character slant (tan angle)		*/
  int  font;	/* Active Font number                   */
  int  stdfont; /* Designated tandard font number       */
  int  altfont; /* Designated alternate font number     */
  int  orig;	/* Label origin code			*/


/**
 ** Internally needed for character resizing and positioning
 **
 ** T = matrix, mapping (relative) sprite coordinates into norm coord.,
 ** chardiff & linediff are used to advance the graphical text cursor,
 ** pref is a pointer to the current text reference point (origin):
 **/

  double	Txx,Txy,Tyx,Tyy;	/* Transformation matrix	*/
  HPGL_Pt	chardiff,	/* Horiz. distance between characters	*/
		linediff,	/* Vertical distance between characters	*/
		refpoint,	/* Current reference point	*/
		CR_point,	/* Returns point after a <CR>	*/
		offset;	/* Needed for HP-GL command ``LO;''	*/
}
  TEXTPAR, *TextPar;




typedef	enum {LB_direct, LB_buffered, LB_buffered_in_use}
	LB_Mode;		/* LB and PB work differently !	*/


/**
 ** Remnant of former BGI font support by project plib,
 ** currently inactive. Leave it here in case of a future revival
 **/

#ifdef STROKED_FONTS
typedef struct {
  unsigned int    buff[16000];	/* Buffer for font data		*/
  unsigned int    vector_off;	/* Offset of start of plot data	*/
  unsigned int    size_off;	/* Offset of size tab		*/
  unsigned int    first;	/* ASCII code of first character*/
  unsigned int    num;		/* Number of defined  character	*/
  unsigned int    height;
  unsigned int    depth;
  char           *name;
}
  FONT, *Font;
#endif


/**
 ** Prototypes:
 **/

#ifdef	__cplusplus
extern	"C" {
#endif

/* void	code_to_ucoord	(char, HPGL_Pt *); */

int	init_font	(int);
void	init_text_par	(void);
void	adjust_text_par	(void);
/* void	ASCII_to_char	(int);*/
void	plot_string	(char *, LB_Mode);
void	plot_symbol_char(char);

#ifdef	__cplusplus
}
#endif



