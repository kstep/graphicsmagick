/*
 * Copyright (c) 2002-2003 Michael David Adams.
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

/******************************************************************************\
* Includes
\******************************************************************************/

#include <jasper/jasper.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

/******************************************************************************\
*
\******************************************************************************/

#define MAXCMPTS	256
#define BIGPAN		0.90
#define SMALLPAN	0.05
#define	BIGZOOM		2.0
#define	SMALLZOOM	1.41421356237310

#define	min(x, y)	(((x) < (y)) ? (x) : (y))
#define	max(x, y)	(((x) > (y)) ? (x) : (y))

typedef struct {

	/* The number of image files to view. */
	int numfiles;

	/* The names of the image files. */
	char **filenames;

	/* The title for the window. */
	char *title;

	/* The time to wait before advancing to the next image (in ms). */
	int tmout;

	/* Loop indefinitely over all images. */
	int loop;

	int verbose;

} cmdopts_t;

typedef struct {

	int width;

	int height;

	GLshort *data;

} pixmap_t;

typedef struct {

	/* The index of the current image file. */
	int filenum;

	/* The image. */
	jas_image_t *image;
	jas_image_t *altimage;

	/* The x-coordinate of viewport center. */
	float vcx;

	/* The y-coordinate of viewport center. */
	float vcy;

	/* The x scale factor. */
	float sx;

	/* The y scale factor. */
	float sy;

	/* The viewport pixmap buffer. */
	pixmap_t vp;

	/* The active timer ID. */
	int activetmid;

	/* The next available timer ID. */
	int nexttmid;

	int monomode;

	int cmptno;

	int dirty;

} gs_t;

/******************************************************************************\
*
\******************************************************************************/

static void display(void);
static void reshape(int w, int h);
static void keyboard(unsigned char key, int x, int y);
static void special(int key, int x, int y);
static void timer(int value);

static void usage(void);
static void nextimage(void);
static void previmage(void);
static void nextcmpt(void);
static void prevcmpt(void);
static int loadimage(void);
static void unloadimage(void);
static void adjust(void);
static int jas_image_render2(jas_image_t *image, int cmptno, float vtlx, float vtly,
  float vsx, float vsy, int vw, int vh, GLshort *vdata);
static int jas_image_render(jas_image_t *image, float vtlx, float vtly,
  float vsx, float vsy, int vw, int vh, GLshort *vdata);

static void dumpstate(void);
static int pixmap_resize(pixmap_t *p, int w, int h);
static void pixmap_clear(pixmap_t *p);
static void cmdinfo(void);

static void cleanupandexit(int);
static void init(void);

/******************************************************************************\
*
\******************************************************************************/

jas_opt_t opts[] = {
	{'V', "version", 0},
	{'v', "v", 0},
	{'h', "help", 0},
	{'w', "wait", JAS_OPT_HASARG},
	{'l', "loop", 0},
	{'t', "title", JAS_OPT_HASARG},
	{-1, 0, 0}
};

char *cmdname = 0;
cmdopts_t cmdopts;
gs_t gs;
jas_stream_t *streamin = 0;

/******************************************************************************\
*
\******************************************************************************/

int main(int argc, char **argv)
{
	int c;

	init();

	/* Determine the base name of this command. */
	if ((cmdname = strrchr(argv[0], '/'))) {
		++cmdname;
	} else {
		cmdname = argv[0];
	}

	/* Initialize the JasPer library. */
	if (jas_init()) {
		abort();
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutCreateWindow(cmdname);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);

	cmdopts.numfiles = 0;
	cmdopts.filenames = 0;
	cmdopts.title = 0;
	cmdopts.tmout = 0;
	cmdopts.loop = 0;
	cmdopts.verbose = 0;

	while ((c = jas_getopt(argc, argv, opts)) != EOF) {
		switch (c) {
		case 'w':
			cmdopts.tmout = atof(jas_optarg) * 1000;
			break;
		case 'l':
			cmdopts.loop = 1;
			break;
		case 't':
			cmdopts.title = jas_optarg;
			break;
		case 'v':
			cmdopts.verbose = 1;
			break;
		case 'V':
			printf("%s\n", JAS_VERSION);
			fprintf(stderr, "libjasper %s\n", jas_getversion());
			cleanupandexit(EXIT_SUCCESS);
			break;
		default:
		case 'h':
			usage();
			break;
		}
	}

	if (jas_optind < argc) {
		/* The images are to be read from one or more explicitly named
		  files. */
		cmdopts.numfiles = argc - jas_optind;
		cmdopts.filenames = &argv[jas_optind];
	} else {
		/* The images are to be read from standard input. */
		static char *null = 0;
		cmdopts.filenames = &null;
		cmdopts.numfiles = 1;
	}

	streamin = jas_stream_fdopen(0, "rb");

	/* Load the next image. */
	nextimage();

	/* Start the GLUT main event handler loop. */
	glutMainLoop();

	return EXIT_SUCCESS;
}

/******************************************************************************\
*
\******************************************************************************/

static void cmdinfo()
{
	fprintf(stderr, "JasPer Image Viewer (Version %s).\n",
	  JAS_VERSION);
	fprintf(stderr, "Copyright (c) 2002-2003 Michael David Adams.\n"
	  "All rights reserved.\n");
	fprintf(stderr, "%s\n", JAS_NOTES);
}

static char *helpinfo[] = {
"The following options are supported:\n",
"    --help                  Print this help information and exit.\n",
"    --version               Print version information and exit.\n",
"    --loop                  Loop indefinitely through images.\n",
"    --wait N                Advance to next image after N seconds.\n",
0
};

static void usage()
{
	char *s;
	int i;
	cmdinfo();
	fprintf(stderr, "usage: %s [options] [file1 file2 ...]\n", cmdname);
	for (i = 0, s = helpinfo[i]; s; ++i, s = helpinfo[i]) {
		fprintf(stderr, "%s", s);
	}
	cleanupandexit(EXIT_FAILURE);
}

/******************************************************************************\
* GLUT Callback Functions
\******************************************************************************/

/* Display callback function. */

static void display()
{
	float vtlx;
	float vtly;

	if (cmdopts.verbose) {
		fprintf(stderr, "display()\n");
		dumpstate();
	}

	if (!gs.dirty) {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawPixels(gs.vp.width, gs.vp.height, GL_RGBA,
		  GL_UNSIGNED_SHORT, gs.vp.data);
		glFlush();
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	pixmap_clear(&gs.vp);
	glDrawPixels(gs.vp.width, gs.vp.height, GL_RGBA, GL_UNSIGNED_SHORT,
	  gs.vp.data);
	glFlush();

	vtlx = gs.vcx - 0.5 * gs.sx * gs.vp.width;
	vtly = gs.vcy - 0.5 * gs.sy * gs.vp.height;
	if (cmdopts.verbose) {
		fprintf(stderr, "vtlx=%f, vtly=%f, vsx=%f, vsy=%f\n",
		  vtlx, vtly, gs.sx, gs.sy);
	}
	if (gs.monomode) {
		if (cmdopts.verbose) {
			fprintf(stderr, "component %d\n", gs.cmptno);
		}
		jas_image_render2(gs.image, gs.cmptno, vtlx, vtly,
		  gs.sx, gs.sy, gs.vp.width, gs.vp.height, gs.vp.data);
	} else {
		if (cmdopts.verbose) {
			fprintf(stderr, "color\n");
		}
		jas_image_render(gs.altimage, vtlx, vtly, gs.sx, gs.sy,
		  gs.vp.width, gs.vp.height, gs.vp.data);
	}
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(gs.vp.width, gs.vp.height, GL_RGBA, GL_UNSIGNED_SHORT,
	  gs.vp.data);
	glFlush();
	gs.dirty = 0;
}

/* Reshape callback function. */

static void reshape(int w, int h)
{
	if (cmdopts.verbose) {
		fprintf(stderr, "reshape(%d, %d)\n", w, h);
		dumpstate();
	}

	if (pixmap_resize(&gs.vp, w, h)) {
		cleanupandexit(EXIT_FAILURE);
	}
	pixmap_clear(&gs.vp);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, 0);
	glRasterPos2i(0, 0);

	if (gs.vp.width > jas_image_width(gs.image) / gs.sx) {
		gs.vcx = (jas_image_tlx(gs.image) + jas_image_brx(gs.image)) / 2.0;
	}
	if (gs.vp.height > jas_image_height(gs.image) / gs.sy) {
		gs.vcy = (jas_image_tly(gs.image) + jas_image_bry(gs.image)) / 2.0;
	}
	gs.dirty = 1;
}

/* Keyboard callback function. */

static void keyboard(unsigned char key, int x, int y)
{
	if (cmdopts.verbose) {
		fprintf(stderr, "keyboard(%d, %d, %d)\n", key, x, y);
	}

	switch (key) {
	case ' ':
		nextimage();
		break;
	case '\b':
		previmage();
		break;
	case '>':
		gs.sx /= BIGZOOM;
		gs.sy /= BIGZOOM;
		gs.dirty = 1;
		glutPostRedisplay();
		break;
	case '.':
		gs.sx /= SMALLZOOM;
		gs.sy /= SMALLZOOM;
		gs.dirty = 1;
		glutPostRedisplay();
		break;
	case '<':
		gs.sx *= BIGZOOM;
		gs.sy *= BIGZOOM;
		adjust();
		gs.dirty = 1;
		glutPostRedisplay();
		break;
	case ',':
		gs.sx *= SMALLZOOM;
		gs.sy *= SMALLZOOM;
		gs.dirty = 1;
		adjust();
		glutPostRedisplay();
		break;
	case 'c':
		nextcmpt();
		break;
	case 'C':
		prevcmpt();
		break;
	case 'q':
		cleanupandexit(EXIT_SUCCESS);
		break;
	}
}

/* Special keyboard callback function. */

static void special(int key, int x, int y)
{
	if (cmdopts.verbose) {
		fprintf(stderr, "special(%d, %d, %d)\n", key, x, y);
	}

	switch (key) {
	case GLUT_KEY_UP:
		{
			float oldvcy;
			float vh;
			float pan;
			if (gs.vp.height < jas_image_height(gs.image) / gs.sy) {
				pan = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ?
				  BIGPAN : SMALLPAN;
				oldvcy = gs.vcy;
				vh = gs.sy * gs.vp.height;
				gs.vcy = max(gs.vcy - pan * vh, jas_image_tly(gs.image) +
				  0.5 * vh);
				if (gs.vcy != oldvcy) {
					gs.dirty = 1;
					glutPostRedisplay();
				}
			}
		}
		break;
	case GLUT_KEY_DOWN:
		{
			float oldvcy;
			float vh;
			float pan;
			if (gs.vp.height < jas_image_height(gs.image) / gs.sy) {
				pan = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ?
				  BIGPAN : SMALLPAN;
				oldvcy = gs.vcy;
				vh = gs.sy * gs.vp.height;
				gs.vcy = min(gs.vcy + pan * vh, jas_image_bry(gs.image) -
				  0.5 * vh);
				if (gs.vcy != oldvcy) {
					gs.dirty = 1;
					glutPostRedisplay();
				}
			}
		}
		break;
	case GLUT_KEY_LEFT:
		{
			float oldvcx;
			float vw;
			float pan;
			if (gs.vp.width < jas_image_width(gs.image) / gs.sx) {
				pan = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ?
				  BIGPAN : SMALLPAN;
				oldvcx = gs.vcx;
				vw = gs.sx * gs.vp.width;
				gs.vcx = max(gs.vcx - pan * vw, jas_image_tlx(gs.image) +
				  0.5 * vw);
				if (gs.vcx != oldvcx) {
					gs.dirty = 1;
					glutPostRedisplay();
				}
			}
		}
		break;
	case GLUT_KEY_RIGHT:
		{
			float oldvcx;
			float vw;
			float pan;
			if (gs.vp.width < jas_image_width(gs.image) / gs.sx) {
				pan = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ?
				  BIGPAN : SMALLPAN;
				oldvcx = gs.vcx;
				vw = gs.sx * gs.vp.width;
				gs.vcx = min(gs.vcx + pan * vw, jas_image_brx(gs.image) -
				  0.5 * vw);
				if (gs.vcx != oldvcx) {
					gs.dirty = 1;
					glutPostRedisplay();
				}
			}
		}
		break;
	default:
		break;
	}
}

/* Timer callback function. */

static void timer(int value)
{
	if (cmdopts.verbose) {
		fprintf(stderr, "timer(%d)\n", value);
	}
	if (value == gs.activetmid) {
		nextimage();
	}
}

/******************************************************************************\
*
\******************************************************************************/

static void nextcmpt()
{
	if (gs.monomode) {
		if (gs.cmptno == jas_image_numcmpts(gs.image) - 1) {
			if (gs.altimage) {
				gs.monomode = 0;
			} else {
				gs.cmptno = 0;
			}
		} else {
			++gs.cmptno;
		}
	} else {
		gs.monomode = 1;
		gs.cmptno = 0;
	}
	gs.dirty = 1;
	glutPostRedisplay();
}

static void prevcmpt()
{
	if (gs.monomode) {
		if (!gs.cmptno) {
			gs.monomode = 0;
		} else {
			--gs.cmptno;
		}
	} else {
		gs.monomode = 1;
		gs.cmptno = jas_image_numcmpts(gs.image) - 1;
	}
	gs.dirty = 1;
	glutPostRedisplay();
}

static void nextimage()
{
	int n;
	unloadimage();
	for (n = cmdopts.numfiles; n > 0; --n) {
		++gs.filenum;
		if (gs.filenum >= cmdopts.numfiles) {
			if (cmdopts.loop) {
				gs.filenum = 0;
			} else {
				cleanupandexit(EXIT_SUCCESS);
			}
		}
		if (!loadimage()) {
			return;
		}
		fprintf(stderr, "cannot load image\n");
	}
	cleanupandexit(EXIT_SUCCESS);
}

static void previmage()
{
	int n;
	unloadimage();
	for (n = cmdopts.numfiles; n > 0; --n) {
		--gs.filenum;
		if (gs.filenum < 0) {
			if (cmdopts.loop) {
				gs.filenum = cmdopts.numfiles - 1;
			} else {
				cleanupandexit(EXIT_SUCCESS);
			}
		}
		if (!loadimage()) {
			return;
		}
	}
	cleanupandexit(EXIT_SUCCESS);
}

static int loadimage()
{
	int reshapeflag;
	jas_stream_t *in;
	int scrnwidth;
	int scrnheight;
	int vh;
	int vw;
	char *pathname;
	jas_cmprof_t *outprof;

	assert(!gs.image);
	assert(!gs.altimage);

	gs.image = 0;
	gs.altimage = 0;

	pathname = cmdopts.filenames[gs.filenum];

	if (pathname && pathname[0] != '\0') {
#if 1
	fprintf(stderr, "opening %s\n", pathname);
#endif
		/* The input image is to be read from a file. */
		if (!(in = jas_stream_fopen(pathname, "rb"))) {
			fprintf(stderr, "error: cannot open file %s\n", pathname);
			goto error;
		}
	} else {
		/* The input image is to be read from standard input. */
		in = streamin;
	}

	/* Get the input image data. */
	if (!(gs.image = jas_image_decode(in, -1, 0))) {
		fprintf(stderr, "error: cannot load image data\n");
		goto error;
	}

	/* Close the input stream. */
	if (in != streamin) {
		jas_stream_close(in);
	}

	if (!(outprof = jas_cmprof_createfromclrspc(JAS_CLRSPC_SRGB)))
		goto error;
	if (!(gs.altimage = jas_image_chclrspc(gs.image, outprof, JAS_CMXFORM_INTENT_PER)))
		goto error;

	if ((scrnwidth = glutGet(GLUT_SCREEN_WIDTH)) < 0) {
		scrnwidth = 256;
	}
	if ((scrnheight = glutGet(GLUT_SCREEN_HEIGHT)) < 0) {
		scrnheight = 256;
	}

	vw = min(jas_image_width(gs.image), 0.95 * scrnwidth);
	vh = min(jas_image_height(gs.image), 0.95 * scrnheight);

	gs.vcx = (jas_image_tlx(gs.image) + jas_image_brx(gs.image)) / 2.0;
	gs.vcy = (jas_image_tly(gs.image) + jas_image_bry(gs.image)) / 2.0;
	gs.sx = 1.0;
	gs.sy = 1.0;
	if (gs.altimage) {
		gs.monomode = 0;
	} else {
		gs.monomode = 1;
		gs.cmptno = 0;
	}

#if 1
	fprintf(stderr, "num of components %d\n", jas_image_numcmpts(gs.image));
#endif

	if (vw < jas_image_width(gs.image)) {
		gs.sx = jas_image_width(gs.image) / ((float) vw);
	}
	if (vh < jas_image_height(gs.image)) {
		gs.sy = jas_image_height(gs.image) / ((float) vh);
	}
	if (gs.sx > gs.sy) {
		gs.sy = gs.sx;
	} else if (gs.sx < gs.sy) {
		gs.sx = gs.sy;
	}
	vw = jas_image_width(gs.image) / gs.sx;
	vh = jas_image_height(gs.image) / gs.sy;
	gs.dirty = 1;

	reshapeflag = 0;
	if (vw != glutGet(GLUT_WINDOW_WIDTH) ||
	  vh != glutGet(GLUT_WINDOW_HEIGHT)) {
		glutReshapeWindow(vw, vh);
		reshapeflag = 1;
	}
	if (cmdopts.title) {
		glutSetWindowTitle(cmdopts.title);
	} else {
		glutSetWindowTitle((pathname && pathname[0] != '\0') ? pathname :
		  "stdin");
	}
	/* If we reshaped the window, GLUT will automatically invoke both
	  the reshape and display callback (in this order).  Therefore, we
	  only need to explicitly force the display callback to be invoked
	  if the window was not reshaped. */
	if (!reshapeflag) {
		glutPostRedisplay();
	}

	if (cmdopts.tmout != 0) {
		glutTimerFunc(cmdopts.tmout, timer, gs.nexttmid);
		gs.activetmid = gs.nexttmid;
		++gs.nexttmid;
	}

	return 0;

error:
	unloadimage();
	return -1;
}

static void unloadimage()
{
	if (gs.image) {
		jas_image_destroy(gs.image);
		gs.image = 0;
	}
	if (gs.altimage) {
		jas_image_destroy(gs.altimage);
		gs.altimage = 0;
	}
}

/******************************************************************************\
*
\******************************************************************************/

static void adjust()
{
	if (gs.vp.width < jas_image_width(gs.image) / gs.sx) {
		float mnx;
		float mxx;
		mnx = jas_image_tlx(gs.image) + 0.5 * gs.vp.width * gs.sx;
		mxx = jas_image_brx(gs.image) - 0.5 * gs.vp.width * gs.sx;
		if (gs.vcx < mnx) {
			gs.vcx = mnx;
		} else if (gs.vcx > mxx) {
			gs.vcx = mxx;
		}
	} else {
		gs.vcx = (jas_image_tlx(gs.image) + jas_image_brx(gs.image)) / 2.0;
	}
	if (gs.vp.height < jas_image_height(gs.image) / gs.sy) {
		float mny;
		float mxy;
		mny = jas_image_tly(gs.image) + 0.5 * gs.vp.height * gs.sy;
		mxy = jas_image_bry(gs.image) - 0.5 * gs.vp.height * gs.sy;
		if (gs.vcy < mny) {
			gs.vcy = mny;
		} else if (gs.vcy > mxy) {
			gs.vcy = mxy;
		}
	} else {
		gs.vcy = (jas_image_tly(gs.image) + jas_image_bry(gs.image)) / 2.0;
	}
}

static void pixmap_clear(pixmap_t *p)
{
	memset(p->data, 0, 4 * p->width * p->height * sizeof(GLshort));
}

static int pixmap_resize(pixmap_t *p, int w, int h)
{
	p->width = w;
	p->height = h;
	if (!(p->data = realloc(p->data, w * h * 4 * sizeof(GLshort)))) {
		return -1;
	}
	return 0;
}

static void dumpstate()
{
	printf("vcx=%f vcy=%f sx=%f sy=%f dirty=%d\n", gs.vcx, gs.vcy, gs.sx, gs.sy, gs.dirty);
}

#define	vctocc(i, co, cs, vo, vs) \
  (((vo) + (i) * (vs) - (co)) / (cs))

static int jas_image_render(jas_image_t *image, float vtlx, float vtly,
  float vsx, float vsy, int vw, int vh, GLshort *vdata)
{
	int i;
	int j;
	int k;
	int x;
	int y;
	int v[3];
	GLshort *vdatap;
	int cmptlut[3];
	int width;
	int height;
	int hs;
	int vs;
	int tlx;
	int tly;

	if ((cmptlut[0] = jas_image_getcmptbytype(image,
	  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R))) < 0 ||
	  (cmptlut[1] = jas_image_getcmptbytype(image,
	  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G))) < 0 ||
	  (cmptlut[2] = jas_image_getcmptbytype(image,
	  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B))) < 0)
		goto error;
	width = jas_image_cmptwidth(image, cmptlut[0]);
	height = jas_image_cmptheight(image, cmptlut[0]);
	tlx = jas_image_cmpttlx(image, cmptlut[0]);
	tly = jas_image_cmpttly(image, cmptlut[0]);
	vs = jas_image_cmptvstep(image, cmptlut[0]);
	hs = jas_image_cmpthstep(image, cmptlut[0]);
	for (i = 1; i < 3; ++i) {
		if (jas_image_cmptwidth(image, cmptlut[i]) != width ||
		  jas_image_cmptheight(image, cmptlut[i]) != height)
			goto error;
	}
	for (i = 0; i < vh; ++i) {
		vdatap = &vdata[(vh - 1 - i) * (4 * vw)];
		for (j = 0; j < vw; ++j) {
			x = vctocc(j, tlx, hs, vtlx, vsx);
			y = vctocc(i, tly, vs, vtly, vsy);
			if (x >= 0 && x < width && y >= 0 && y < height) {
				for (k = 0; k < 3; ++k) {
					v[k] = jas_image_readcmptsample(image, cmptlut[k], x, y);
					v[k] <<= 16 - jas_image_cmptprec(image, cmptlut[k]);
					if (v[k] < 0) {
						v[k] = 0;
					} else if (v[k] > 65535) {
						v[k] = 65535;
					}
				}
			} else {
				v[0] = 0;
				v[1] = 0;
				v[2] = 0;
			}	
			*vdatap++ = v[0];
			*vdatap++ = v[1];
			*vdatap++ = v[2];
			*vdatap++ = 0;
		}
	}
	return 0;
error:
	return -1;
}

int jas_image_render2(jas_image_t *image, int cmptno, float vtlx, float vtly,
  float vsx, float vsy, int vw, int vh, GLshort *vdata)
{
	int i;
	int j;
	int x;
	int y;
	int v;
	GLshort *vdatap;

	if (cmptno < 0 || cmptno >= image->numcmpts_) {
		fprintf(stderr, "bad parameter\n");
		goto error;
	}
	for (i = 0; i < vh; ++i) {
		vdatap = &vdata[(vh - 1 - i) * (4 * vw)];
		for (j = 0; j < vw; ++j) {
			x = vctocc(j, jas_image_cmpttlx(image, cmptno), jas_image_cmpthstep(image, cmptno), vtlx, vsx);
			y = vctocc(i, jas_image_cmpttly(image, cmptno), jas_image_cmptvstep(image, cmptno), vtly, vsy);
			v = (x >= 0 && x < jas_image_cmptwidth(image, cmptno) && y >=0 && y < jas_image_cmptheight(image, cmptno)) ? jas_image_readcmptsample(image, cmptno, x, y) : 0;
			v <<= 16 - jas_image_cmptprec(image, cmptno);
			if (v < 0) {
				v = 0;
			} else if (v > 65535) {
				v = 65535;
			}
			*vdatap++ = v;
			*vdatap++ = v;
			*vdatap++ = v;
			*vdatap++ = 0;
		}
	}
	return 0;
error:
	return -1;
}

#if 0

#define	vctocc(i, co, cs, vo, vs) \
  (((vo) + (i) * (vs) - (co)) / (cs))

static void drawview(jas_image_t *image, float vtlx, float vtly,
  float sx, float sy, pixmap_t *p)
{
	int i;
	int j;
	int k;
	int red;
	int grn;
	int blu;
	int lum;
	GLshort *datap;
	int x;
	int y;
	int *cmptlut;
	int numcmpts;
	int v[4];
	int u[4];
	int color;

	cmptlut = gs.cmptlut;
	switch (jas_image_colorspace(gs.image)) {
	case JAS_IMAGE_CS_RGB:
	case JAS_IMAGE_CS_YCBCR:
		color = 1;
		numcmpts = 3;
		break;
	case JAS_IMAGE_CS_GRAY:
	default:
		numcmpts = 1;
		color = 0;
		break;
	}

	for (i = 0; i < p->height; ++i) {
		datap = &p->data[(p->height - 1 - i) * (4 * p->width)];
		for (j = 0; j < p->width; ++j) {
			if (!gs.monomode && color) {
				for (k = 0; k < numcmpts; ++k) {
					x = vctocc(j, jas_image_cmpttlx(gs.image, cmptlut[k]), jas_image_cmpthstep(gs.image, cmptlut[k]), vtlx, sx);
					y = vctocc(i, jas_image_cmpttly(gs.image, cmptlut[k]), jas_image_cmptvstep(gs.image, cmptlut[k]), vtly, sy);
					v[k] = (x >= 0 && x < jas_image_cmptwidth(gs.image, cmptlut[k]) && y >=0 && y < jas_image_cmptheight(gs.image, cmptlut[k])) ? jas_matrix_get(gs.cmpts[cmptlut[k]], y, x) : 0;
					v[k] <<= 16 - jas_image_cmptprec(gs.image, cmptlut[k]);
				}
				switch (jas_image_colorspace(gs.image)) {
				case JAS_IMAGE_CS_RGB:
					break;
				case JAS_IMAGE_CS_YCBCR:
					u[0] = (1/1.772) * (v[0] + 1.402 * v[2]);
					u[1] = (1/1.772) * (v[0] - 0.34413 * v[1] - 0.71414 * v[2]);
					u[2] = (1/1.772) * (v[0] + 1.772 * v[1]);
					v[0] = u[0];
					v[1] = u[1];
					v[2] = u[2];
					break;
				}
			} else {
				x = vctocc(j, jas_image_cmpttlx(gs.image, gs.cmptno), jas_image_cmpthstep(gs.image, gs.cmptno), vtlx, sx);
				y = vctocc(i, jas_image_cmpttly(gs.image, gs.cmptno), jas_image_cmptvstep(gs.image, gs.cmptno), vtly, sy);
				v[0] = (x >= 0 && x < jas_image_cmptwidth(gs.image, gs.cmptno) && y >=0 && y < jas_image_cmptheight(gs.image, gs.cmptno)) ? jas_matrix_get(gs.cmpts[gs.cmptno], y, x) : 0;
				v[0] <<= 16 - jas_image_cmptprec(gs.image, gs.cmptno);
				v[1] = v[0];
				v[2] = v[0];
				v[3] = 0;
			}

for (k = 0; k < 3; ++k) {
	if (v[k] < 0) {
		v[k] = 0;
	} else if (v[k] > 65535) {
		v[k] = 65535;
	}
}

			*datap++ = v[0];
			*datap++ = v[1];
			*datap++ = v[2];
			*datap++ = 0;
		}
	}
}

#endif

static void cleanupandexit(int status)
{
	unloadimage();
	exit(status);
}

static void init()
{
	gs.filenum = -1;
	gs.image = 0;
	gs.altimage = 0;
	gs.nexttmid = 0;
	gs.vp.width = 0;
	gs.vp.height = 0;
	gs.vp.data = 0;
	gs.dirty = 1;
}
