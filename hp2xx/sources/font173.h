/*
   Copyright (c) 1992 - 1994 Heinz W. Werntges.  All rights reserved.
   Parts Copyright (c) 1999  Martin Kroeker  All rights reserved.
   
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

/**
 ** This file defines a standard character set by elementary
 ** "draw" & "move" commands. The format is a very compact one from
 ** the old days where every byte was still appreciated.
 **
 ** A font or character set is an array of strings. Each character
 ** corresponds to one of these strings, which is addressed by its ASCII code.
 **
 ** A character is a (NULL-terminated) string of bytes. Each byte
 ** codes for a draw or move action according to the code below:
 **
 **	Bit:	7 6 5 4 3 2 1 0
 **		p x x x y y y y
 **
 **	p:	Plot flag. If set, "draw to" new point, else "move to" it.
 **	xxx:	3-bit unsigned integer  (0...7). X coordinate of new point.
 **	yyyy:	4-bit unsigned integer (0..15). Y coordinate of new point.
 **
 ** The baseline is y = 4 instead of y = 0, so characters with parts
 ** below it can be drawn properly without a need for sign bits.
 ** Function "code_to_ucoord()" transforms these coordinates into
 ** actual user coordinates.
 **
 ** Example:	code for character 'L': "\032\224\324" translates to:
 **		moveto(1,10); drawto(1,4); drawto(5,4);
 **
 ** From the example you can conclude that the font below essentially is
 ** defined on a 5x7 grid:
 **
 **	  	0 1 2 3 4 5 6 7
 **	15	. . . . . . . .		. : unused
 **	14	. . . . . . . .		* : always used
 **	13	. . . . . . . .		o : sometimes used
 **	12	. . . . . . . .
 **	11	. . . . . . . .
 **	10	o * * * * * . .
 **	 9	o * * * * * . .
 **	 8	o * * * * * . .
 **	 7	o * * * * * . .
 **	 6	o * * * * * . .
 **	 5	o * * * * * . .
 **	 4	o * * * * * . .
 **	 3	o o o o o o . .
 **	 2	o o o o o o . .
 **	 1	o o o o o o . .
 **	 0	o o o o o o . .
 **/


/**
 ** The following array of strings contains the ps math font (173),
 ** which differs from ventura math (205) only in the sequence of 
 ** characters in the upper half of the font
 **/

char	*charset173[256] =
{
	/* 0x00 ... 0x1f	*/

/**
 ** Some control codes are valid in HPGL. These are handled elsewhere
 ** in a font-independent manner, so following codes are dummies:
 **/
"","","","", "","","","", "","","","", "","","","",
"","","","", "","","","", "","","","", "","","","",

/**
 ** Unfortunately, some compilers do not process \xNN properly,
 ** so I changed all hex codes (\xNN) into octal codes (\NNN),
 ** thereby losing readability but gaining portability.
 **/
	/* 0x20 ... 0x2f */
"", /*space */
"\064\265\066\272", /*exclam*/
"\031\264\331\047\307", /*forall*/
"\044\252\104\312\026\326\030\330", /*hash*/
"\031\271\265\225\067\227", /*exists*/
"\024\332\051\250\270\271\251\066\265\305\306\266", /* percent*/
"\124\230\231\252\271\270\226\225\244\264\326",  /*ampersand*/
"\031\251\270\267\266\225\067\227", /*ni*/
"\132\270\266\324", /* opening brace */
"\024\266\270\232", /* closing brace */
"\005\351\145\211\072\264", /* asterisk */
"\065\271\027\327", /* plus */
"\064\244\245\265\263\242", /* comma */
"\027\327", /* minus */
"\064\244\245\265\264", /* dot */
"\352", /* slash */

	/* 0x30 ... 0x3f	*/
"\025\244\304\325\331\312\252\231\225",
"\044\304\064\272\251",
"\031\252\312\331\330\225\224\324",
"\025\244\304\325\326\307\267\332\232",
"\112\227\226\326\107\304",
"\132\232\230\310\327\325\304\244\225",
"\132\272\230\225\244\304\325\326\307\227",
"\032\332\331\226\224",
"\107\330\331\312\252\231\230\247\307\326\325\304\244\225\226\247",
"\044\264\326\331\312\252\231\230\247\327",
"\047\250\270\267\247\045\265\264\244\245",
"\046\247\267\266\246\064\244\245\265\263\242",
"\112\227\304",
"\030\330\026\326",
"\032\307\224",
"\031\252\312\331\330\307\267\266\065\264",

	/* 0x40 ... 0x4f	*/
"\025\325\027\327\030\251\270\310\331", /*congruent*/
"\024\231\252\312\331\324\026\326",  /* A */
"\024\232\312\331\330\307\227\024\304\325\326\307", /*B*/
"\024\332\124\232", /* Chi (X) */
"\024\272\324\224",  /*Delta*/
"\124\224\232\332\027\307",/*E*/ 
"\052\312\072\264\044\304\027\251\311\327\305\245\227", /*Phi*/
"\052\332\330\072\264\044\304", /*Gamma*/
"\024\232\124\332\027\327", /*H*/
"\024\324\064\272\032\332", /*I*/
"\027\304\330\331\271\270\326", /*vartheta*/
"\024\232\027\247\324\047\332", /*K*/
"\024\272\324", /*Lambda*/
"\024\232\270\332\324", /*M*/
"\024\232\324\332",/*N*/
"\044\225\231\252\312\331\325\304\244",/*O*/

	/* 0x50 ... 0x5f	*/
"\044\252\032\332\112\304",/*Pi*/
"\044\225\231\252\312\331\325\304\244\027\327", /*Theta*/
"\024\232\312\331\330\307\227",/*Rho (P)*/
"\044\252\032\332\112\304", /*Sigma*/
"\064\272\232\332", /*T*/
"\032\231\266\264\066\331\332", /*Y*/
"\042\262\303\304\225\226\247\307", /*varsigma*/
"\024\244\226\231\252\312\331\326\304\324", /*Omega*/
"\031\232\332\331\050\246\047\307\110\306\025\224\324\325", /*Xi*/
"\030\250\246\306\310\330\052\312\072\264\044\304",/*Psi*/
"\032\332\224\324",/*Z*/
"\124\264\272\332", /*opening bracket*/
"\004\224\225\205\204\104\324\325\305\304\051\271\272\252\251",/*point triangle*/
"\024\264\272\232", /* closing bracket*/
"\024\324\064\270", /*bottom*/
"\023\323", /*underline*/

	/* 0x60 ... 0x6f	*/

"\034\334", /*overline*/
"\025\227\250\270\307\305\264\244\225\107\330\105\324", /* alpha*/
"\044\251\272\311\310\267\306\304\264\245", /* beta */
"\027\250\267\265\304\325\024\330", /* chi*/
"\045\246\267\307\326\325\304\264\245\107\271\312\331", /*delta*/
"\127\310\250\227\246\266\046\225\244\304\325", /* epsilon*/
"\026\247\307\326\305\245\226\064\270", /*phi*/
"\027\226\303\262\243\326\327", /*gamma*/
"\030\247\270\310\327\322\047\245", /* eta*/
"067\264\304\305", /* iota */
"\030\227\246\306\327\310\270\264\244", /*varphi*/
"\050\244\046\266\330\066\305\304\324", /* kappa*/
"\031\251\250\324\024\266", /* lambda*/
"\024\250\246\265\306\310\106\325", /* mu*/
"\030\226\264\326\327\310", /* nu*/
"\044\225\227\250\270\307\305\264\244", /*o*/

	/* 0x70 ... 0x7f	*/
"\044\247\104\307\027\250\267\307\330",	 /* pi */
"\045\264\305\311\272\251\245\047\307", /* theta*/
"\023\246\267\307\326\325\304\264\245", /*rho*/
"\045\246\267\307\326\325\304\264\245\107\330", /*sigma*/
"\026\267\327\067\264\324", /*tau*/
"\030\226\264\326\330", /*v*/
"\050\227\225\244\265\266\065\304\325\327\310\047\307", /*var pi*/
"\050\227\225\244\265\266\065\304\325\327\310", /*omega*/
"\051\250\267\307\330\270\246\265\305\326\266\244\243\262\322\321\301", /*xi*/
"\027\247\265\305\327\110\264", /*psi*/
"\051\250\267\307\330\270\246\244\243\262\322\321\301", /*zeta*/
"\113\273\252\250\227\246\244\263\303", /* opening curly brace*/
"\073\263",  /* vertical bar*/
"\053\273\312\310\327\306\304\263\243", /*closing curly brace*/
"\027\250\267\307\330", /* similar*/
"", /*blank*/

	/* 0x80 ... 0x9f */
"","","","", "","","","", "","","","", "","","","",
"","","","", "","","","", "","","","", "","","","",

	/* 0xa0 ... 0xaf */

"", /* space */
"\030\251\270\311\330\070\264\044\304", /* Upsilon*/
"\071\312", /* prime */
"\025\325\227\331",         /*lessorequal*/
"\352", /* slash */
"\026\226\247\247\266\266\245\245\226\66\266\307\326\305\266", /*infinity*/
"\131\332\312\270\265\244\224\225\050\310", /* math function */
"\026\247\266\245\226\066\307\326\305\266\067\250\271\310\267\067\264", /*clubsuit*/
"\026\271\326\263\226", /* diamondsuit */
"\030\252\271\312\330\265\230", /* heartsuit */
"\026\271\326\305\267\245\226\067\264", /* spadesuit */
"\027\327\106\327\310\046\227\250", /* leftrightarrow */
"\027\327\046\227\250", /* leftarrow */
"\072\264\030\272\330",		/*uparrow*/
"\027\327\106\327\310", 	/*rightarrow*/
"\072\264\026\264\326", /* downarrow */



	/* 0xb0 ... 0xbf */
"\050\251\272\311\310\267\250", /* degree */
"\065\271\027\327\025\325",	/* plusminus*/
"\051\252\111\312", /* two primes */
"\125\225\327\231",     /*greaterorequal*/
"\045\310\050\305", /* times */
"\026\226\247\247\266\266\245\245\226\66\266\307\066\305", /* propto*/
"\050\271\311\305\264\244\225\226\267\306", /* partial */
"\047\270\307\266\247", /* circle */
"\027\327\065\266\071\272",      /*divide*/   
"\030\330\026\326\045\311",     /*notequal*/
"\025\325\027\327\031\331",   /* identity*/
"\026\247\266\306\327\030\251\270\310\331", /* approx*/
"\024\225\064\265\124\325", /* ellipsis*/
"\072\264", /* center part of braces and brackets*/
"\027\327", /* horiz line*/
"\027\327\046\227\250\127\330", /* leftanglearrow*/

/*	0xc0 ... 0xcf */

"\024\245\247\130\307\305\030\324", /* aleph */
"\050\231\252\272\330\132\310\305\264\244\225", /* Im */
"\050\231\252\312\330\307\305\324\107\267\072\265\244\225", /* Re */
"\030\227\245\243\222\224\246\247\270\310\327\325\304\264\245", /*wp*/
"\027\230\251\311\330\326\305\245\226\227\045\310\050\305", /* otimes*/
"\027\230\251\311\330\326\305\245\226\227\071\265\027\327", /*oplus*/
"\027\251\311\327\305\245\227\024\332", /*  oslash */
"\025\230\251\311\330\325", /* cap */
"\031\226\245\305\326\331", /* cup*/
"\025\305\326\330\311\231", /* superset */
"\026\306\327\310\230\024\324", /* supsetequal*/
"\125\245\226\230\251\331\045\311", /* not subset*/
"\125\245\226\230\251\331", /* subset */
"\126\246\227\250\330\024\324", /*subsetequal*/
"\125\245\226\230\251\331\027\327", /* in */
"\125\245\226\230\251\331\027\327\045\311", /* not in */

	/* 0xd0 ... 0xdf */

"\132\224\324", /* angle */
"\032\332\264\232",	/* nabla */
"\027\230\251\311\330\326\305\245\226\227\046\250\270\267\247\266", /* registered */
"\027\230\251\311\330\326\305\245\226\227\110\270\247\266\306", /* copyright*/
"\032\272\052\250\070\272\311\332\330", /* TM */
"\024\231\124\331\031\331", /* prod */
"\026\246\264\311\331", /* root */
"\066\267", 	/* centered dot */
"\026\326\325",		/* neg */
"\024\267\324", /*wedge*/
"\030\264\330", 	/* vee */
"\045\305\047\307\070\226\264\070\326\264", /* leftright doublearrow */
"\045\325\047\327\070\226\264", /* left doublearrow */
"\044\250\104\310\027\272\327", /* up doublearrow */
"\025\305\027\307\070\326\264", /* right doublearrow*/
"\052\245\112\305\026\264\326", /* down doublearrow */

	/* 0xe0 ... 0xef */

"\026\270\326\264\226", /* diamond */
"\112\247\304", 	/* opening angle bracket */
"\027\230\251\311\330\326\305\245\226\227\046\250\270\267\247\266", /*registered*/
"\027\230\251\311\330\326\305\245\226\227\110\270\247\266\306", /* copyright*/
"\032\272\052\250\070\272\311\332\330", /* TM */
"\044\252\032\332\112\304", /*Sigma*/
"\112\270\264", /* top third of opening brace */
"\072\264", /* center part of braces and brackets*/
"\072\266\304", /* lower third of opening brace */
"\112\272\264", /* upper third of opening bracket*/
"\072\264", /* center part of braces and brackets*/
"\072\264\304", /* lower third of opening bracket */
"\112\270\264",		/* top third of opening brace */
"\072\270\247\266\264", /* center part of opening curly brace */
"\072\266\324", /* lower third of opening round brace */
"\072\264", /* center part of braces and brackets*/

	/* 0xf0 ... 0xff */

"", /* empty*/
"\052\307\244",		/* closing angle bracket */
"\024\243\264\272\313\332",	/* integral */
"\112\270\264",		 /* top third of opening brace */
"\072\264",	/* center part of braces*/
"\072\266\224", /* lower third of closing brace */
"\052\270\264",		/* top third of closing brace */
"\072\264",		/* center part of braces*/
"\072\266\224", /* lower third of closing brace */
"\052\272\264", 		/* upper third of closing bracket*/
"\072\264",		/* center part of braces*/
"\072\264\244",		/* lower third of closing bracket */
"\052\270\264",		/* top third of closing brace */
"\072\270\307\266\264",/* center part of closing curly brace */
"\072\266\224",		/* lower third of closing brace */
""			/* blank*/
};
