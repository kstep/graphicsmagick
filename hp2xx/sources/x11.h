/*
   Copyright (c) 1992  Michael Schoene.  All rights reserved.
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
 ** $Id$
 **
 **
 ** $Log$
 ** Revision 1.1  1992/01/15  15:22:09  michael
 ** Initial revision
 **
 **
 ** DESCRIPTION: Interface for X11
 **				win_open and win_close open and close a window
 **				create_image and show_image are used for b/w bitmap display
 **				win_clear clears the window
 **
 **
 ** AUTHOR	  : Michael Schoene
 **				(see file hp2xx.c for full address)
 **/


#ifndef X11_H
#define X11_H


#define	NO_SERVER	1
#define	SIZE		2
#define	NO_MEM		3

/*#define	CMS_SIZE	16*/
#define	CMS_SIZE	256
#define	BLACK		0
#define	WHITE		15
#define	RED		2
#define	GREEN		3
#define	BLUE		4
#define	CYAN		5
#define	MAGENTA		6
#define	YELLOW		7
#define	LIGHTGRAY	8
#define	LIGHTRED	10
#define	LIGHTGREEN	11
#define	LIGHTBLUE	12
#define	LIGHTCYAN	13
#define	LIGHTMAGENTA	14
#define	GRAY		15


/** Open window at position (x,y) with height h and width w
 ** return value:  0:	OK.
 **                1:	Server does not respond
 **				  2:	Window too big
 **				  3:	Out of memory
 **/


#endif
