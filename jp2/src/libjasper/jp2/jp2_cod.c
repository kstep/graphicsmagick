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
 * JP2 Library
 *
 * $Id$
 */

/******************************************************************************\
* Includes.
\******************************************************************************/

#include <stdlib.h>

#include "jasper/jas_stream.h"
#include "jasper/jas_malloc.h"

#include "jp2_cod.h"

/******************************************************************************\
* Function prototypes.
\******************************************************************************/

jp2_boxinfo_t *jp2_boxinfolookup(int type);

static int jp2_getuint8(jas_stream_t *in, uint_fast8_t *val);
static int jp2_getuint16(jas_stream_t *in, uint_fast16_t *val);
static int jp2_getuint32(jas_stream_t *in, uint_fast32_t *val);
static int jp2_getuint64(jas_stream_t *in, uint_fast64_t *val);
static int jp2_putuint8(jas_stream_t *out, uint_fast8_t val);
static int jp2_putuint16(jas_stream_t *out, uint_fast16_t val);
static int jp2_putuint32(jas_stream_t *out, uint_fast32_t val);
static int jp2_putuint64(jas_stream_t *out, uint_fast64_t val);

jp2_box_t *jp2_box_get(jas_stream_t *in);

static int jp2_jp_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_jp_putdata(jp2_box_t *box, jas_stream_t *out);
static int jp2_ftyp_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_ftyp_putdata(jp2_box_t *box, jas_stream_t *out);
static int jp2_ihdr_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_ihdr_putdata(jp2_box_t *box, jas_stream_t *out);
static void jp2_bpcc_destroy(jp2_box_t *box);
static int jp2_bpcc_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_bpcc_putdata(jp2_box_t *box, jas_stream_t *out);
static int jp2_colr_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_colr_putdata(jp2_box_t *box, jas_stream_t *out);
static void jp2_cdef_destroy(jp2_box_t *box);
static int jp2_cdef_getdata(jp2_box_t *box, jas_stream_t *in);
static int jp2_cdef_putdata(jp2_box_t *box, jas_stream_t *out);

/******************************************************************************\
* Local data.
\******************************************************************************/

jp2_boxinfo_t jp2_boxinfos[] = {
	{JP2_BOX_JP, "Signature (JP)", 0,
	  {0, 0, jp2_jp_getdata, jp2_jp_putdata}},
	{JP2_BOX_FTYP, "File Type (FTYP)", 0,
	  {0, 0, jp2_ftyp_getdata, jp2_ftyp_putdata}},
	{JP2_BOX_JP2H, "JP2 Header (JP2H)", JP2_BOX_SUPER,
	  {0, 0, 0, 0}},
	{JP2_BOX_IHDR, "Image Header (IHDR)", 0,
	  {0, 0, jp2_ihdr_getdata, jp2_ihdr_putdata}},
	{JP2_BOX_BPCC, "Bits Per Component (BPCC)", 0,
	  {0, jp2_bpcc_destroy, jp2_bpcc_getdata, jp2_bpcc_putdata}},
	{JP2_BOX_COLR, "Color Specification (COLR)", 0,
	  {0, 0, jp2_colr_getdata, jp2_colr_putdata}},
	{JP2_BOX_PCLR, "Palette (PCLR)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_CMAP, "Component Mapping (CMAP)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_CDEF, "Channel Definition (CDEF)", 0,
	  {0, jp2_cdef_destroy, jp2_cdef_getdata, jp2_cdef_putdata}},
	{JP2_BOX_RES, "Resolution (RES)", JP2_BOX_SUPER,
	  {0, 0, 0, 0}},
	{JP2_BOX_RESC, "Capture Resolution (RESC)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_RESD, "Default Display Resolution (RESD)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_JP2C, "Contiguous Code Stream (JP2C)", JP2_BOX_NODATA,
	  {0, 0, 0, 0}},
	{JP2_BOX_JP2I, "Intellectual Property (JP2I)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_XML, "XML (XML)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_UUID, "UUID (UUID)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_UINF, "UUID Info (UINF)", JP2_BOX_SUPER,
	  {0, 0, 0, 0}},
	{JP2_BOX_ULST, "UUID List (ULST)", 0,
	  {0, 0, 0, 0}},
	{JP2_BOX_URL, "URL (URL)", 0,
	  {0, 0, 0, 0}},
	{0, 0, 0, {0, 0, 0, 0}},
};

jp2_boxinfo_t jp2_boxinfo_unk = {
	0, "Unknown", 0, {0, 0, 0, 0}
};

/******************************************************************************\
* Box constructor.
\******************************************************************************/

jp2_box_t *jp2_box_create(int type)
{
	jp2_box_t *box;
	jp2_boxinfo_t *boxinfo;

	if (!(box = jas_malloc(sizeof(jp2_box_t)))) {
		return 0;
	}
	memset(box, 0, sizeof(jp2_box_t));
	box->type = type;
	box->len = 0;
	if (!(boxinfo = jp2_boxinfolookup(type))) {
		return 0;
	}
	box->info = boxinfo;
	box->ops = &boxinfo->ops;
	return box;
}

/******************************************************************************\
* Box destructor.
\******************************************************************************/

void jp2_box_destroy(jp2_box_t *box)
{
	if (box->ops->destroy) {
		(*box->ops->destroy)(box);
	}
	jas_free(box);
}

static void jp2_bpcc_destroy(jp2_box_t *box)
{
	jp2_bpcc_t *bpcc = &box->data.bpcc;
	if (bpcc->bpcs) {
		jas_free(bpcc->bpcs);
		bpcc->bpcs = 0;
	}
}

static void jp2_cdef_destroy(jp2_box_t *box)
{
	jp2_cdef_t *cdef = &box->data.cdef;
	if (cdef->chans) {
		jas_free(cdef->chans);
		cdef->chans = 0;
	}
}

/******************************************************************************\
* Box input.
\******************************************************************************/

jp2_box_t *jp2_box_get(jas_stream_t *in)
{
	jp2_box_t *box;
	jp2_boxinfo_t *boxinfo;
	jas_stream_t *tmpstream;
	uint_fast32_t len;
	uint_fast64_t extlen;
	bool dataflag;

	box = 0;
	tmpstream = 0;

	if (!(box = jas_malloc(sizeof(jp2_box_t)))) {
		goto error;
	}
	box->ops = &jp2_boxinfo_unk.ops;
	if (jp2_getuint32(in, &len) || jp2_getuint32(in, &box->type)) {
		goto error;
	}
	boxinfo = jp2_boxinfolookup(box->type);
	box->info = boxinfo;
	box->ops = &boxinfo->ops;
	box->len = len;
	if (box->len == 1) {
		if (jp2_getuint64(in, &extlen)) {
			goto error;
		}
		box->len = extlen;
	}
	if (box->len != 0 && box->len < 8) {
		goto error;
	}

	dataflag = !(box->info->flags & (JP2_BOX_SUPER | JP2_BOX_NODATA));

	if (dataflag) {
		if (!(tmpstream = jas_stream_memopen(0, 0))) {
			goto error;
		}
		if (jas_stream_copy(tmpstream, in, box->len - JP2_BOX_HDRLEN)) {
			goto error;
		}
		jas_stream_rewind(tmpstream);

		if (box->ops->getdata) {
			if ((*box->ops->getdata)(box, tmpstream)) {
				goto error;
			}
		}
		jas_stream_close(tmpstream);
	}

	return box;
	abort();

error:
	if (box) {
		jp2_box_destroy(box);
	}
	if (tmpstream) {
		jas_stream_close(tmpstream);
	}
	return 0;
}

static int jp2_jp_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_jp_t *jp = &box->data.jp;
	if (jp2_getuint32(in, &jp->magic)) {
		return -1;
	}
	return 0;
}

static int jp2_ftyp_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_ftyp_t *ftyp = &box->data.ftyp;
	int i;
	if (jp2_getuint32(in, &ftyp->majver) || jp2_getuint32(in, &ftyp->minver)) {
		return -1;
	}
	ftyp->numcompatcodes = ((box->len - JP2_BOX_HDRLEN) - 8) / 4;
	if (ftyp->numcompatcodes > JP2_FTYP_MAXCOMPATCODES) {
		return -1;
	}
	for (i = 0; i < ftyp->numcompatcodes; ++i) {
		if (jp2_getuint32(in, &ftyp->compatcodes[i])) {
			return -1;
		}
	}
	return 0;
}

static int jp2_ihdr_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_ihdr_t *ihdr = &box->data.ihdr;
	if (jp2_getuint32(in, &ihdr->height) || jp2_getuint32(in, &ihdr->width) ||
	  jp2_getuint16(in, &ihdr->numcmpts) || jp2_getuint8(in, &ihdr->bpc) ||
	  jp2_getuint8(in, &ihdr->comptype) || jp2_getuint8(in, &ihdr->csunk) ||
	  jp2_getuint8(in, &ihdr->ipr)) {
		return -1;
	}
	return 0;
}

static int jp2_bpcc_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_bpcc_t *bpcc = &box->data.bpcc;
	int i;
	bpcc->numcmpts = box->len - JP2_BOX_HDRLEN;
	if (!(bpcc->bpcs = jas_malloc(bpcc->numcmpts * sizeof(uint_fast8_t)))) {
		return -1;
	}
	for (i = 0; i < bpcc->numcmpts; ++i) {
		if (jp2_getuint8(in, &bpcc->bpcs[i])) {
			return -1;
		}
	}
	return 0;
}

static int jp2_colr_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_colr_t *colr = &box->data.colr;
	if (jp2_getuint8(in, &colr->method) || jp2_getuint8(in, &colr->pri) ||
	  jp2_getuint8(in, &colr->approx)) {
		return -1;
	}
	switch (colr->method) {
	case JP2_COLR_ENUM:
		if (jp2_getuint32(in, &colr->csid)) {
			return -1;
		}
		break;
	case JP2_COLR_ICC:
		/* XXX - Someday we really ought to do something about ICC profile
		  data.  For now, we just read it and throw it away. */
		/* There is no colorspace ID field in the ICC profile case. */
		colr->csid = 0;
		break;
	}
	return 0;
}

static int jp2_cdef_getdata(jp2_box_t *box, jas_stream_t *in)
{
	jp2_cdef_t *cdef = &box->data.cdef;
	jp2_cdefchan_t *chan;
	int channo;
	if (jp2_getuint16(in, &cdef->numchans)) {
		return -1;
	}
	if (!(cdef->chans = jas_malloc(cdef->numchans * sizeof(jp2_cdefchan_t)))) {
		return -1;
	}
	for (channo = 0; channo < cdef->numchans; ++channo) {
		chan = &cdef->chans[channo];
		if (jp2_getuint16(in, &chan->ind) || jp2_getuint16(in, &chan->type) ||
		  jp2_getuint16(in, &chan->assoc)) {
			return -1;
		}
	}
	return 0;
}

/******************************************************************************\
* Box output.
\******************************************************************************/

int jp2_box_put(jp2_box_t *box, jas_stream_t *out)
{
	jas_stream_t *tmpstream;
	bool extlen;
	bool dataflag;

	tmpstream = 0;

	dataflag = !(box->info->flags & (JP2_BOX_SUPER | JP2_BOX_NODATA));

	if (dataflag) {
		tmpstream = jas_stream_memopen(0, 0);
		if (box->ops->putdata) {
			if ((*box->ops->putdata)(box, tmpstream)) {
				goto error;
			}
		}
		box->len = jas_stream_tell(tmpstream) + JP2_BOX_HDRLEN;
		jas_stream_rewind(tmpstream);
	}
	extlen = (box->len >= (((uint_fast64_t)1) << 32)) != 0;
	if (jp2_putuint32(out, extlen ? 1 : box->len)) {
		goto error;
	}
	if (jp2_putuint32(out, box->type)) {
		goto error;
	}
	if (extlen) {
		if (jp2_putuint64(out, box->len)) {
			goto error;
		}
	}

	if (dataflag) {
		if (jas_stream_copy(out, tmpstream, box->len - JP2_BOX_HDRLEN)) {
			goto error;
		}
		jas_stream_close(tmpstream);
	}

	return 0;
	abort();

error:

	if (tmpstream) {
		jas_stream_close(tmpstream);
	}
	return -1;
}

static int jp2_jp_putdata(jp2_box_t *box, jas_stream_t *out)
{
	jp2_jp_t *jp = &box->data.jp;
	if (jp2_putuint32(out, jp->magic)) {
		return -1;
	}
	return 0;
}

static int jp2_ftyp_putdata(jp2_box_t *box, jas_stream_t *out)
{
	jp2_ftyp_t *ftyp = &box->data.ftyp;
	int i;
	if (jp2_putuint32(out, ftyp->majver) || jp2_putuint32(out, ftyp->minver)) {
		return -1;
	}
	for (i = 0; i < ftyp->numcompatcodes; ++i) {
		if (jp2_putuint32(out, ftyp->compatcodes[i])) {
			return -1;
		}
	}
	return 0;
}

static int jp2_ihdr_putdata(jp2_box_t *box, jas_stream_t *out)
{
	jp2_ihdr_t *ihdr = &box->data.ihdr;
	if (jp2_putuint32(out, ihdr->height) || jp2_putuint32(out, ihdr->width) ||
	  jp2_putuint16(out, ihdr->numcmpts) || jp2_putuint8(out, ihdr->bpc) ||
	  jp2_putuint8(out, ihdr->comptype) || jp2_putuint8(out, ihdr->csunk) ||
	  jp2_putuint8(out, ihdr->ipr)) {
		return -1;
	}
	return 0;
}

static int jp2_bpcc_putdata(jp2_box_t *box, jas_stream_t *out)
{
	jp2_bpcc_t *bpcc = &box->data.bpcc;
	int i;
	for (i = 0; i < bpcc->numcmpts; ++i) {
		if (jp2_putuint8(out, bpcc->bpcs[i])) {
			return -1;
		}
	}
	return 0;
}

static int jp2_colr_putdata(jp2_box_t *box, jas_stream_t *out)
{
	jp2_colr_t *colr = &box->data.colr;
	if (jp2_putuint8(out, colr->method) || jp2_putuint8(out, colr->pri) ||
	  jp2_putuint8(out, colr->approx)) {
		return -1;
	}
	switch (colr->method) {
	case JP2_COLR_ENUM:
		if (jp2_putuint32(out, colr->csid)) {
			return -1;
		}
		break;
	case JP2_COLR_ICC:
		/* XXX - not implemented */
		abort();
		break;
	}
	return 0;
}

static int jp2_cdef_putdata(jp2_box_t *box, jas_stream_t *out)
{
	return -1;
}

/******************************************************************************\
* Input operations for primitive types.
\******************************************************************************/

static int jp2_getuint8(jas_stream_t *in, uint_fast8_t *val)
{
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	if (val) {
		*val = c;
	}
	return 0;
}

static int jp2_getuint16(jas_stream_t *in, uint_fast16_t *val)
{
	uint_fast16_t v;
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if (val) {
		*val = v;
	}
	return 0;
}

static int jp2_getuint32(jas_stream_t *in, uint_fast32_t *val)
{
	uint_fast32_t v;
	int c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if ((c = jas_stream_getc(in)) == EOF) {
		return -1;
	}
	v = (v << 8) | c;
	if (val) {
		*val = v;
	}
	return 0;
}

static int jp2_getuint64(jas_stream_t *in, uint_fast64_t *val)
{
	abort();
}

/******************************************************************************\
* Output operations for primitive types.
\******************************************************************************/

static int jp2_putuint8(jas_stream_t *out, uint_fast8_t val)
{
	if (jas_stream_putc(out, val & 0xff) == EOF) {
		return -1;
	}
	return 0;
}

static int jp2_putuint16(jas_stream_t *out, uint_fast16_t val)
{
	if (jas_stream_putc(out, (val >> 8) & 0xff) == EOF ||
	  jas_stream_putc(out, val & 0xff) == EOF) {
		return -1;
	}
	return 0;
}

static int jp2_putuint32(jas_stream_t *out, uint_fast32_t val)
{
	if (jas_stream_putc(out, (val >> 24) & 0xff) == EOF ||
	  jas_stream_putc(out, (val >> 16) & 0xff) == EOF ||
	  jas_stream_putc(out, (val >> 8) & 0xff) == EOF ||
	  jas_stream_putc(out, val & 0xff) == EOF) {
		return -1;
	}
	return 0;
}

static int jp2_putuint64(jas_stream_t *out, uint_fast64_t val)
{
	if (jp2_putuint32(out, (val >> 32) & 0xffffffffUL) ||
	  jp2_putuint32(out, val & 0xffffffffUL)) {
		return -1;
	}
	return 0;
}

/******************************************************************************\
* Miscellaneous code.
\******************************************************************************/

jp2_boxinfo_t *jp2_boxinfolookup(int type)
{
	jp2_boxinfo_t *boxinfo;
	for (boxinfo = jp2_boxinfos; boxinfo->name; ++boxinfo) {
		if (boxinfo->type == type) {
			return boxinfo;
		}
	}
	return &jp2_boxinfo_unk;
}
