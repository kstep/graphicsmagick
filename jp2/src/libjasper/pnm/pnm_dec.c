/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 * 
 * JasPer Software License
 * 
 * IMAGE POWER JPEG-2000 PUBLIC LICENSE
 * ************************************
 * 
 * GRANT:
 * 
 * Permission is hereby granted, free of charge, to any person (the "User")
 * obtaining a copy of this software and associated documentation, to deal
 * in the JasPer Software without restriction, including without limitation
 * the right to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the JasPer Software (in source and binary forms),
 * and to permit persons to whom the JasPer Software is furnished to do so,
 * provided further that the License Conditions below are met.
 * 
 * License Conditions
 * ******************
 * 
 * A.  Redistributions of source code must retain the above copyright notice,
 * and this list of conditions, and the following disclaimer.
 * 
 * B.  Redistributions in binary form must reproduce the above copyright
 * notice, and this list of conditions, and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 * 
 * C.  Neither the name of Image Power, Inc. nor any other contributor
 * (including, but not limited to, the University of British Columbia and
 * Michael David Adams) may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * D.  User agrees that it shall not commence any action against Image Power,
 * Inc., the University of British Columbia, Michael David Adams, or any
 * other contributors (collectively "Licensors") for infringement of any
 * intellectual property rights ("IPR") held by the User in respect of any
 * technology that User owns or has a right to license or sublicense and
 * which is an element required in order to claim compliance with ISO/IEC
 * 15444-1 (i.e., JPEG-2000 Part 1).  "IPR" means all intellectual property
 * rights worldwide arising under statutory or common law, and whether
 * or not perfected, including, without limitation, all (i) patents and
 * patent applications owned or licensable by User; (ii) rights associated
 * with works of authorship including copyrights, copyright applications,
 * copyright registrations, mask work rights, mask work applications,
 * mask work registrations; (iii) rights relating to the protection of
 * trade secrets and confidential information; (iv) any right analogous
 * to those set forth in subsections (i), (ii), or (iii) and any other
 * proprietary rights relating to intangible property (other than trademark,
 * trade dress, or service mark rights); and (v) divisions, continuations,
 * renewals, reissues and extensions of the foregoing (as and to the extent
 * applicable) now existing, hereafter filed, issued or acquired.
 * 
 * E.  If User commences an infringement action against any Licensor(s) then
 * such Licensor(s) shall have the right to terminate User's license and
 * all sublicenses that have been granted hereunder by User to other parties.
 * 
 * F.  This software is for use only in hardware or software products that
 * are compliant with ISO/IEC 15444-1 (i.e., JPEG-2000 Part 1).  No license
 * or right to this Software is granted for products that do not comply
 * with ISO/IEC 15444-1.  The JPEG-2000 Part 1 standard can be purchased
 * from the ISO.
 * 
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS LICENSE.
 * NO USE OF THE JASPER SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE JASPER SOFTWARE IS PROVIDED BY THE LICENSORS AND
 * CONTRIBUTORS UNDER THIS LICENSE ON AN ``AS-IS'' BASIS, WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION,
 * WARRANTIES THAT THE JASPER SOFTWARE IS FREE OF DEFECTS, IS MERCHANTABLE,
 * IS FIT FOR A PARTICULAR PURPOSE OR IS NON-INFRINGING.  THOSE INTENDING
 * TO USE THE JASPER SOFTWARE OR MODIFICATIONS THEREOF FOR USE IN HARDWARE
 * OR SOFTWARE PRODUCTS ARE ADVISED THAT THEIR USE MAY INFRINGE EXISTING
 * PATENTS, COPYRIGHTS, TRADEMARKS, OR OTHER INTELLECTUAL PROPERTY RIGHTS.
 * THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE JASPER SOFTWARE
 * IS WITH THE USER.  SHOULD ANY PART OF THE JASPER SOFTWARE PROVE DEFECTIVE
 * IN ANY RESPECT, THE USER (AND NOT THE INITIAL DEVELOPERS, THE UNIVERSITY
 * OF BRITISH COLUMBIA, IMAGE POWER, INC., MICHAEL DAVID ADAMS, OR ANY
 * OTHER CONTRIBUTOR) SHALL ASSUME THE COST OF ANY NECESSARY SERVICING,
 * REPAIR OR CORRECTION.  UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY,
 * WHETHER TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE, SHALL THE
 * INITIAL DEVELOPER, THE UNIVERSITY OF BRITISH COLUMBIA, IMAGE POWER, INC.,
 * MICHAEL DAVID ADAMS, ANY OTHER CONTRIBUTOR, OR ANY DISTRIBUTOR OF THE
 * JASPER SOFTWARE, OR ANY SUPPLIER OF ANY OF SUCH PARTIES, BE LIABLE TO
 * THE USER OR ANY OTHER PERSON FOR ANY INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES OF ANY CHARACTER INCLUDING, WITHOUT LIMITATION,
 * DAMAGES FOR LOSS OF GOODWILL, WORK STOPPAGE, COMPUTER FAILURE OR
 * MALFUNCTION, OR ANY AND ALL OTHER COMMERCIAL DAMAGES OR LOSSES, EVEN IF
 * SUCH PARTY HAD BEEN INFORMED, OR OUGHT TO HAVE KNOWN, OF THE POSSIBILITY
 * OF SUCH DAMAGES.  THE JASPER SOFTWARE AND UNDERLYING TECHNOLOGY ARE NOT
 * FAULT-TOLERANT AND ARE NOT DESIGNED, MANUFACTURED OR INTENDED FOR USE OR
 * RESALE AS ON-LINE CONTROL EQUIPMENT IN HAZARDOUS ENVIRONMENTS REQUIRING
 * FAIL-SAFE PERFORMANCE, SUCH AS IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL, DIRECT
 * LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH THE FAILURE OF THE
 * JASPER SOFTWARE OR UNDERLYING TECHNOLOGY OR PRODUCT COULD LEAD DIRECTLY
 * TO DEATH, PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE
 * ("HIGH RISK ACTIVITIES").  LICENSOR SPECIFICALLY DISCLAIMS ANY EXPRESS
 * OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.  USER WILL NOT
 * KNOWINGLY USE, DISTRIBUTE OR RESELL THE JASPER SOFTWARE OR UNDERLYING
 * TECHNOLOGY OR PRODUCTS FOR HIGH RISK ACTIVITIES AND WILL ENSURE THAT ITS
 * CUSTOMERS AND END-USERS OF ITS PRODUCTS ARE PROVIDED WITH A COPY OF THE
 * NOTICE SPECIFIED IN THIS SECTION.
 * 
 * __END_OF_JASPER_LICENSE__
 */

/*
 * Portable Pixmap/Graymap Format Support
 *
 * $Id$
 */

/******************************************************************************\
* Includes.
\******************************************************************************/

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "jasper/jas_types.h"
#include "jasper/jas_stream.h"
#include "jasper/jas_image.h"

#include "pnm_cod.h"

/******************************************************************************\
* Local function prototypes.
\******************************************************************************/

static int pnm_gethdr(jas_stream_t *in, pnm_hdr_t *hdr);
static int pnm_getuint16(jas_stream_t *in, uint_fast16_t *val);
static int pnm_getint32(jas_stream_t *in, int_fast32_t *val);
static int pnm_getuint32(jas_stream_t *in, uint_fast32_t *val);
static int pnm_getc(jas_stream_t *in);
static int pnm_getdata(jas_stream_t *in, pnm_hdr_t *hdr, jas_image_t *image);

/******************************************************************************\
* Local data.
\******************************************************************************/

static int pnm_allowtrunc = 1;

/******************************************************************************\
* Load function.
\******************************************************************************/

jas_image_t *pnm_decode(jas_stream_t *in, char *opts)
{
	pnm_hdr_t hdr;
	jas_image_t *image;
	jas_image_cmptparm_t cmptparms[3];
	jas_image_cmptparm_t *cmptparm;
	int i;
	uint_fast16_t numcmpts;
	int colormodel;

	if (opts) {
		fprintf(stderr, "warning: ignoring options\n");
	}

	/* Read the file header. */
	if (pnm_gethdr(in, &hdr)) {
		return 0;
	}

	/* Determine the number of components and the color model. */
	switch (pnm_type(hdr.magic)) {
	case PNM_TYPE_PGM:
		numcmpts = 1;
		colormodel = JAS_IMAGE_CM_GRAY;
		break;
	case PNM_TYPE_PPM:
		numcmpts = 3;
		colormodel = JAS_IMAGE_CM_RGB;
		break;
	default:
		/* Prevent annoying compiler warnings about possibly
		  uninitialized variables. */
		numcmpts = 0;
		colormodel = 0;
		abort();
		break;
	}

	/* Create an image of the correct size. */
	for (i = 0, cmptparm = cmptparms; i < numcmpts; ++i, ++cmptparm) {
		cmptparm->tlx = 0;
		cmptparm->tly = 0;
		cmptparm->hstep = 1;
		cmptparm->vstep = 1;
		cmptparm->width = hdr.width;
		cmptparm->height = hdr.height;
		cmptparm->prec = pnm_maxvaltodepth(hdr.maxval);
		cmptparm->sgnd = false;
	}
	if (!(image = jas_image_create(numcmpts, cmptparms, colormodel))) {
		return 0;
	}

	/* Read image data from stream into image. */
	if (pnm_getdata(in, &hdr, image)) {
		jas_image_destroy(image);
		return 0;
	}

	return image;
}

/******************************************************************************\
* Validation function.
\******************************************************************************/

int pnm_validate(jas_stream_t *in)
{
	uchar buf[2];
	int i;
	int n;

	assert(JAS_STREAM_MAXPUTBACK >= 2);

	/* Read the first two characters that constitute the signature. */
	if ((n = jas_stream_read(in, buf, 2)) < 0) {
		return -1;
	}
	/* Put these characters back to the stream. */
	for (i = n - 1; i >= 0; --i) {
		if (jas_stream_ungetc(in, buf[i]) == EOF) {
			return -1;
		}
	}
	/* Did we read enough data? */
	if (n < 2) {
		return -1;
	}
	/* Is this the correct signature for a PNM file? */
	if (buf[0] == 'P' && isdigit(buf[1])) {
		return 0;
	}
	return -1;
}

/******************************************************************************\
* Functions for reading the header.
\******************************************************************************/

static int pnm_gethdr(jas_stream_t *in, pnm_hdr_t *hdr)
{
	if (pnm_getuint16(in, &hdr->magic) || pnm_getuint32(in, &hdr->width) ||
	  pnm_getuint32(in, &hdr->height) || pnm_getuint32(in, &hdr->maxval)) {
		return -1;
	}
	return 0;
}

/******************************************************************************\
* Functions for processing the sample data.
\******************************************************************************/

static int pnm_getdata(jas_stream_t *in, pnm_hdr_t *hdr, jas_image_t *image)
{
	int ret;
	int numcmpts;
	uint_fast16_t cmptno;
	int fmt;
	jas_matrix_t *data[3];
	uint_fast32_t x;
	uint_fast32_t y;
	int_fast32_t v;
	int c;

	ret = -1;

	numcmpts = jas_image_numcmpts(image);
	fmt = pnm_fmt(hdr->magic);

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
		if (!(data[cmptno] = jas_matrix_create(1, hdr->width))) {
			goto done;
		}
	}

	for (y = 0; y < hdr->height; ++y) {
		for (x = 0; x < hdr->width; ++x) {
			for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
				if (fmt == PNM_FMT_BIN && hdr->maxval <= 255) {
					if ((c = jas_stream_getc(in)) == EOF) {
						if (pnm_allowtrunc) {
							v = 0;
						} else {
							goto done;
						}
					} else {
						v = c;
					}
				} else {
					if (pnm_getint32(in, &v)) {
						if (pnm_allowtrunc) {
							v = 0;
						} else {
							goto done;
						}
					}
				}
				jas_matrix_set(data[cmptno], 0, x, (v));
			}
		}
		for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
			if (jas_image_writecmpt(image, cmptno, 0, y, hdr->width, 1,
			  data[cmptno])) {
				goto done;
			}
		}
	}

	ret = 0;

done:

	for (cmptno = 0; cmptno < numcmpts; ++cmptno) {
		if (data[cmptno]) {
			jas_matrix_destroy(data[cmptno]);
		}
	}

	return ret;
}

/******************************************************************************\
* Miscellaneous functions.
\******************************************************************************/

static int pnm_getuint32(jas_stream_t *in, uint_fast32_t *val)
{
	int_fast32_t v;
	if (pnm_getint32(in, &v)) {
		return -1;
	}
	if (v < 0) {
		return -1;
	}
	if (val) {
		*val = v;
	}
	return 0;
}

static int pnm_getint32(jas_stream_t *in, int_fast32_t *val)
{
	int c;
	int s;
	int_fast32_t v;

	do {
		if ((c = pnm_getc(in)) == EOF) {
			return -1;
		}
	} while (isspace(c));

	s = 1;
	if (c == '-') {
		s = -1;
		if ((c = pnm_getc(in)) == EOF) {
			return -1;
		}
	}
	v = 0;
	while (isdigit(c)) {
		v = 10 * v + c - '0';
		if ((c = pnm_getc(in)) < 0) {
			return -1;
		}
	}
	if (!isspace(c)) {
		return -1;
	}
	*val = (s >= 0) ? v : (-v);

	return 0;
}

static int pnm_getc(jas_stream_t *in)
{
	int c;
	for (;;) {
		if ((c = jas_stream_getc(in)) == EOF) {
			return -1;
		}
		if (c != '#') {
			return c;
		}
		do {
			if ((c = jas_stream_getc(in)) == EOF) {
				return -1;
			}
		} while (c != '\n' && c != '\r');
	}
}

static int pnm_getuint16(jas_stream_t *in, uint_fast16_t *val)
{
	uint_fast16_t v;
	int c;

	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = c & 0xff;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | (c & 0xff);
	*val = v;

	return 0;
}
