/*  TkMagick.c -- Glue to make combine TclMagick and Tk. */

/* Copyright 2003, 2004 David N. Welton <davidw@dedasys.com> */

/* $Id$ */

#include <tk.h>
#include "TclMagick.h"
#include <string.h>
#include <wand/magick_wand.h>

/*
 *-----------------------------------------------------------------------------
 *
 * MagickToPhoto --
 *
 * 	Implements "magicktophoto".  Takes the name of a "magick wand"
 * 	and a Tk image as arguments.  Both must already exist,
 * 	although they don't necessarily have to contain anything.
 *
 * Results:
 *	A normal Tcl result.
 *
 * Side Effects:
 *	Sets the image photo size.
 *
 *-----------------------------------------------------------------------------
 */

static int MagickToPhoto(
    ClientData  clientData,         /* Unused */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    MagickWand *wand;
    TclMagickObj *tclmagick;
    Tk_PhotoHandle photohandle;
    Tk_PhotoImageBlock magickblock;
    char *magickname = NULL;
    char *photoname = NULL;

    if( objc != 3 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "magickwand image" );
        return TCL_ERROR;
    }

    magickname = Tcl_GetStringFromObj(objv[1], NULL);
    tclmagick = findMagickObj(interp, TM_TYPE_WAND, magickname);
    wand = tclmagick->wandPtr;
    if (tclmagick == NULL) {
	Tcl_AddErrorInfo(interp, "Not a magick wand.");
	return TCL_ERROR;
    }

    photoname = Tcl_GetStringFromObj(objv[2], NULL);
    photohandle = Tk_FindPhoto(interp, photoname);
    if (photohandle == NULL) {
	Tcl_AddErrorInfo(interp, "Not a photo image.");
	return TCL_ERROR;
    }

    /* pixelSize corresponds to "RGB" format below. */
    magickblock.pixelSize = 4;
    magickblock.width = MagickGetImageWidth (wand);
    magickblock.height = MagickGetImageHeight (wand);
    magickblock.pixelPtr = ckalloc((unsigned)magickblock.height *
				   (unsigned)magickblock.width *
				   magickblock.pixelSize);
    magickblock.pitch = magickblock.width * magickblock.pixelSize;
    magickblock.offset[0] = 0;
    magickblock.offset[1] = 1;
    magickblock.offset[2] = 2;
    magickblock.offset[3] = 3;

    /* RGB corresponds to pixelSize above. */
    if (MagickGetImagePixels (
	    wand, 0, 0, (unsigned)magickblock.width, (unsigned)magickblock.height,
	    "RGBO", CharPixel, magickblock.pixelPtr) == False) {
	return myMagickError(interp, wand);
    }

    Tk_PhotoPutBlock(photohandle, &magickblock, 0, 0,
		     magickblock.width, magickblock.height,
		     TK_PHOTO_COMPOSITE_SET);

    ckfree(magickblock.pixelPtr);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 *  PhotoToMagick --
 *
 *      Implements "phototomagick".  Takes a Tk image and a "magick
 *      wand" as arguments.  Transfers the contents of the Tk image to
 *      the magick wand.
 *
 * Results:
 *	Normal Tcl result.
 *
 * Side Effects:
 *
 *	Erases the image that was previously in the magick wand, or
 *	creates a new one.
 *
 *-----------------------------------------------------------------------------
 */

static int PhotoToMagick(
    ClientData  clientData,         /* Unused */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    MagickWand *wand;
    TclMagickObj *tclmagick;
    Tk_PhotoHandle photohandle;
    Tk_PhotoImageBlock photoblock;
    char *magickname = NULL;
    char *photoname = NULL;
    int result = 0;
    char map[4];

    if( objc != 3 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "magickwand image" );
        return TCL_ERROR;
    }

    photoname = Tcl_GetStringFromObj(objv[1], NULL);
    photohandle = Tk_FindPhoto(
	interp, photoname);
    if (photohandle == NULL) {
	Tcl_AddErrorInfo(interp, "Not a photo image.");
	return TCL_ERROR;
    }
    Tk_PhotoGetImage(photohandle, &photoblock);

    magickname = Tcl_GetStringFromObj(objv[2], NULL);
    tclmagick = findMagickObj(interp, TM_TYPE_WAND, magickname);
    wand = tclmagick->wandPtr;
    if (tclmagick == NULL) {
	Tcl_AddErrorInfo(interp, "Not a magick wand.");
	return TCL_ERROR;
    }
    MagickSetSize (wand, (unsigned)photoblock.width, (unsigned)photoblock.height);
    result = MagickReadImage(wand, "xc:white");
    if (!result) {
	return myMagickError(interp, wand);
    }

    /* This could be improved some, but I don't know how Tk deals with
       'unusual' pixelSize's. */
    switch (photoblock.pixelSize) {
    case 3:
	map[photoblock.offset[0]] = 'R';
	map[photoblock.offset[1]] = 'G';
	map[photoblock.offset[2]] = 'B';
	break;
    case 4:
	map[photoblock.offset[0]] = 'R';
	map[photoblock.offset[1]] = 'G';
	map[photoblock.offset[2]] = 'B';
	map[photoblock.offset[3]] = 'A';
	break;
    default:
	Tcl_AddErrorInfo(interp, "Unsupported pixelSize in Tk image.");
	return TCL_ERROR;
    };

    if (MagickSetImagePixels (wand, 0, 0, (unsigned)photoblock.width, 
			      (unsigned)photoblock.height,
			      map, CharPixel, photoblock.pixelPtr) == False) {
	return myMagickError(interp, wand);
    }

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tkmagick_Init --
 *
 * 	Initialize this module.
 *
 * Results:
 *	Normal Tcl results.
 *
 * Side Effects:
 *
 *	Requires TclMagick module, creates magicktophoto and
 *	phototomagick commands.
 *
 *-----------------------------------------------------------------------------
 */

EXPORT(int, Tkmagick_Init)(Tcl_Interp *interp)
{
    if (Tcl_InitStubs(interp, "8", 0) == NULL) {
	return TCL_ERROR;
    }

    if ( Tcl_PkgRequire(interp, "TclMagick", "0.3", 0) == NULL) {
	return TCL_ERROR;
    }
    if ( Tcl_PkgProvide(interp,"TkMagick", "0.1") != TCL_OK ) {
        return TCL_ERROR;
    }

    Tcl_CreateObjCommand(interp, "magicktophoto",  MagickToPhoto,  NULL, NULL);
    Tcl_CreateObjCommand(interp, "phototomagick",  PhotoToMagick,  NULL, NULL);

    return TCL_OK;
}
