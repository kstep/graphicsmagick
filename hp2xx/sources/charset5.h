/*
   Copyright (c) 1992 - 1994 Heinz W. Werntges.  All rights reserved.
   Distributed by Free Software Foundation, Inc.
   Copyright (c) 1999 Martin Kroeker
   
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
/** charset5.h                 HP-GL character set 5, Special Symbols
 **                             MK February 1999
 **/
/**
 ** This file defines character set 5 by elementary
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
 ** The following array of strings contains the basic character set (set 0).
 **
 ** NOTE: A nice way to add a new charset would be, e. g., to introduce a
 ** ``charset1[]'' as the "alternate" charset and implement the HP-GL
 ** commands needed for switching from one to the other.
 **/

char	*charset5[128] =
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

	/* 0x20 ... 0x2f	*/
"",                                                         /*blank*/
"\064\265\066\272",                                         /*exclam*/
"\051\252\111\312",                                         /*dblquote*/
"\044\252\104\312\026\326\030\330",                         /*hash*/
"\064\272\131\251\230\247\307\326\305\225",                 /*dollar*/
"\024\332\051\250\270\271\251\066\265\305\306\266",         /*percent*/
"\124\230\231\252\271\270\226\225\244\264\326",             /*ampersand*/
"\071\312",                                                 /*snglquote*/
"\132\270\266\324",                                         /*lbrace*/
"\024\266\270\232",                                         /*rbrace*/
"\005\351\145\211\072\264",                                 /*asterisk*/
"\065\271\027\327",                                         /*plus*/
"\064\244\245\265\263\242",                                 /*comma*/
"\027\327",                                                 /*minus*/
"\064\244\245\265\264",                                     /*dot*/
"\352",                                                     /*slash*/

	/* 0x30 ... 0x3f	*/
/*
"\025\244\304\325\331\312\252\231\225\331", ** Zero including `/' **
*/
"\025\244\304\325\331\312\252\231\225",                   /*zero*/
"\044\304\064\272\251",                                   /*one*/
"\031\252\312\331\330\225\224\324",                       /*two*/
"\025\244\304\325\326\307\267\332\232",
"\112\227\226\326\107\304",                        
"\132\232\230\310\327\325\304\244\225",                   /*five*/
"\132\272\230\225\244\304\325\326\307\227",
"\032\332\331\226\224",
"\107\330\331\312\252\231\230\247\307\326\325\304\244\225\226\247",
"\044\264\326\331\312\252\231\230\247\327",
"\047\250\270\267\247\045\265\264\244\245",              /*colon*/
"\046\247\267\266\246\064\244\245\265\263\242",          /*semicolon*/
"\112\227\304",                                          /*greater*/
"\030\330\026\326",                                      /*equal*/
"\032\307\224",                                          /*less*/
"\031\252\312\331\330\307\267\266\065\264",              /*questionm*/

	/* 0x40 ... 0x4f	*/
"\103\243\224\230\252\312\331\326\305\266\267\310\330",   /*at*/
"\022\226\326\322\222\066\264",                           /*boxmarker*/
"\023\225\246\306\325\323\302\242\223\066\264",           /*circlemarker*/
"\022\266\322\222\066\264",                               /*trianglemarker*/
"\024\324\066\262",                                       /*plusmarker*/
"\022\326\026\322",                                       /*crossmarker*/       
"\024\266\324\262\224\066\264",                           /*diamondmarker*/
"\024\324\066\262\024\266\324",                           /*arrowmarker*/
"\022\326\026\322\026\326",                               /*xbarmarker*/
"\026\326\222\322\044\304",                               /*zmarker*/
"\026\264\326\064\262",                                   /*ymarker*/
"\022\326\026\322\043\245\305\303\243",                   /*boxedxmarker*/
"\022\326\026\322\024\324",                               /*asteriskmarker*/
"\022\326\026\322\026\326\022\322",                       /*hourglassmarker*/
"\066\262",                                               /*vertlinemarker*/
"\023\266\323\223\62\225\325\262",                        /*starofdavid*/

	/* 0x50 ... 0x5f	*/
"\042\302",                                               /*dashmarker*/
"\066\264",                                               /*shortvline*/
"\024\232\312\331\330\307\227\247\324",                   /*R*/
"\025\244\304\325\326\307\247\230\231\252\312\331",       /*S*/
"\064\272\232\332",
"\032\225\244\304\325\332",
"\032\230\264\330\332",
"\032\224\267\324\332",
"\024\332\124\232",
"\032\231\266\264\066\331\332",
"\032\332\224\324",                                       /*Z*/
"\124\264\272\332",                                       /*rsquarebracket*/
"\032\324",                                               /*backslash*/
"\024\264\272\232",                                       /*lsquarebracket*/
"\030\272\330",                                           /*hataccent*/
"\023\323",                                               /*underline*/

	/* 0x60 ... 0x6f	*/
"\053\310",                                               /*backquote*/
"\025\230\251\311\330\325",                                /*logicalor*/
"\025\305\326\330\311\231",
"\125\245\226\230\251\331",
"\031\226\245\305\326\331",                               /*logicaland*/
"\034\334",                                              /*overline FIXME:should backspace*/
"\025\325\027\327\031\331",                              /*identical*/
"\025\325\027\327\030\251\270\310\331",                  /*approxident*/
"\026\247\266\306\327\030\251\270\310\331",              /*approximately*/
"\027\250\267\307\330",                                  /*proportional*/
"\025\325\227\331",                                        /*lessorequal*/
"\125\225\327\231",                                       /*greaterorequal*/
"\030\330\026\326\045\311",                                  /*notequal*/
"\024\272\324\224",                                       /*Delta*/
"\044\252\032\332\112\304",                                /*Pi*/
"\125\324\224\267\232\332\331",                           /*Sigma*/

	/* 0x70 ... 0x7f	*/
"\065\271\027\327\025\325",                          /*plusminus*/
"\065\271\027\327\032\332",                          /*minusplus*/
"\027\327\106\327\310",                                  /*rightarrow*/
"\072\264\030\272\330",                                  /*uparrow*/
"\027\327\046\227\250",                                  /*leftarrow*/
"\072\264\026\264\326",                                  /*downarrow*/
"\024\243\264\272\313\332",                                  /*integral*/
"\027\327\065\265\071\271",                            /*divide*/
"\025\330\030\325\071\267",                                /*multiply*/
"\032\332\264\232",
"\050\251\272\311\310\267\250",                                      /*degree*/
"\113\273\252\250\227\246\244\263\303",                  /*leftcurlybrace*/
"\073\263",                                              /*vbar*/
"\053\273\312\310\327\306\304\263\243",                  /*rightcurlybrace*/
"\031\252\310\331",                                      /*tilde*/
"",
};
