/*
$Id$
 */

#ifndef _TCLMAGICK_H_
#define _TCLMAGICK_H_

#include <tcl.h>
#include <wand/magick_wand.h>

#define DEBUG 1

static CONST char *objTypeNames[] = {
    "wand", "drawing", "pixel", (char *) NULL
};
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

char *getMagickObjName(TclMagickObj *mPtr);
TclMagickObj *newMagickObj(Tcl_Interp  *interp, int type,
			   void *wandPtr, char *name);
TclMagickObj *findMagickObj(Tcl_Interp *interp, int type, char *name);
int myMagickError(Tcl_Interp  *interp, MagickWand *wandPtr );


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


#endif
