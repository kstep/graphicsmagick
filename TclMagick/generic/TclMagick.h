/*
 * TclMagick definitions
 */
/* $Id$ */

#ifndef _TCLMAGICK_H_
#define _TCLMAGICK_H_

#include <tcl.h>
#include <wand/magick_wand.h>

#define DEBUG 1

enum objTypes {
    TM_TYPE_WAND, TM_TYPE_DRAWING, TM_TYPE_PIXEL, TM_TYPE_ANY
};

typedef struct {
    int             type;
    void            *wandPtr;  /* MagickWand, DrawingWand or PixelWand
				* pointer */
    Tcl_Command     magickCmd; /* Token for magick command, used to
				* delete it */
    Tcl_Interp      *interp;   /* Tcl interpreter owing the object */
    Tcl_HashEntry   *hashPtr;  /* Hash entry for this structure, used
				* to delete it */
} TclMagickObj;

#ifdef __WIN32__
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN

/*
 * VC++ has an alternate entry point called DllMain, so we need to rename
 * our entry point.
 */

#   if defined(_MSC_VER)
#       define EXPORT(a,b) __declspec(dllexport) a b
#       define DllEntryPoint DllMain
#   else
#       if defined(__BORLANDC__)
#           define EXPORT(a,b) a _export b
#       else
#           define EXPORT(a,b) a b
#       endif
#   endif
#else
#   define EXPORT(a,b) a b
#endif

EXPORT(TclMagickObj, *findMagickObj)(Tcl_Interp *interp, int type, char *name);

/*----------------------------------------------------------------------
 * Return Magick error description as a TCL result
 * Used by both TclMagick and TkMagick.
 *----------------------------------------------------------------------
 */
static int myMagickError(Tcl_Interp  *interp, MagickWand *wandPtr )
{
    char *description;

    ExceptionType severity;
    char msg[40];

    description = MagickGetException(wandPtr, &severity);
    if( (description == NULL) || (strlen(description) == 0) ) {
        Tcl_AppendResult(interp, MagickGetPackageName(), ": Unknown error", NULL);
    } else {
        sprintf(msg, "%s: #%d:", MagickGetPackageName(), severity); /* FIXME, not used! */
        Tcl_AppendResult(interp, description, NULL);
    }
    if( description != NULL ) {
        MagickRelinquishMemory(description);
    }
    /*
     * if(severity < ErrorException) --> warning
     * return TCL_OK ???
     */
    return TCL_ERROR;
}


#endif
/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
