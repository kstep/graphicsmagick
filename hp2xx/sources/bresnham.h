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

/** bresnham.h: Header for Bresenham utility
 **
 ** 91/01/04  V 1.00  HWW Originating
 ** 92/01/12  V 1.01  HWW ANSI prototypes required now
 **/


#define	BRESENHAM_EOL	0x04
#define	BRESENHAM_ERR	0xff



typedef struct {
	int	x,y;		/* 2d - pseudo device coord	*/
} DevPt;


DevPt	*bresenham_init	(DevPt *, DevPt *);
int	bresenham_next	(void);
