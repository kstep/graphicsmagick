/**********************************************************************
 * Tcl Image Magick - a Tcl-only wrapper of imageMagick's Wand-API
 **********************************************************************
 * For a brief command description see "doc/TclMagick.txt"
 *
 * For a list of changes see "ChangeLog"
 **********************************************************************/

#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "TclMagick.h"
#include <wand/magick_wand.h>

static char *getMagickObjName(TclMagickObj *mPtr);
static TclMagickObj *newMagickObj(Tcl_Interp  *interp, int type, void *wandPtr, char *name);

/**********************************************************************/
/* Workaround for bugs: */

#define MAGICK_NEW          /* new interfaces */
#ifdef MAGICK_NEW

#endif

/* Debug TclMagick */
#define MAGICK_DEBUG
/**********************************************************************/

struct {
    int             initialized; /* Whether module has been initialized */
    Tcl_HashTable   hashTable;   /* HasTable for MagickWand objects */
} TM
= {0};

static CONST char *objTypeNames[] = {
    "wand", "drawing", "pixel", (char *) NULL
};

static Tcl_ObjCmdProc    magickCmd;
static Tcl_CmdDeleteProc magickObjDeleteCmd;
static Tcl_ObjCmdProc    wandObjCmd;
static Tcl_ObjCmdProc    pixelObjCmd;
static Tcl_ObjCmdProc    drawObjCmd;

/*----------------------------------------------------------------------
 * Return the name of a TclMagickObj
 *----------------------------------------------------------------------
 */
static char *getMagickObjName(TclMagickObj *mPtr)
{
    if( mPtr == NULL ) {
        return (char *)NULL;
    } else {
        return (char *)Tcl_GetHashKey(&TM.hashTable, mPtr->hashPtr);
    }
}

/*----------------------------------------------------------------------
 * Delete TclMagick objects
 *----------------------------------------------------------------------
 */
static void deleteMagickObj(TclMagickObj *mPtr)
{
    /*
     * Delete the Tcl command deletes also the object itself
     * by calling magickObjDeleteCmd()
     */
    if( mPtr->magickCmd != NULL ) {
        Tcl_DeleteCommandFromToken(mPtr->interp, mPtr->magickCmd);
        mPtr->magickCmd = NULL;
    }
}
static void magickObjDeleteCmd(ClientData clientData)
{
    TclMagickObj *mPtr = (TclMagickObj *)clientData;

    if( mPtr == NULL ) {
        return;
    }
    /*
     * Called before the Tcl command is deleted
     */
    if( mPtr->wandPtr != NULL ) {
        switch(mPtr->type) {
        case TM_TYPE_WAND:
            DestroyMagickWand(mPtr->wandPtr);
            mPtr->wandPtr=(MagickWand *) NULL;;
            break;
        case TM_TYPE_DRAWING:
            DestroyDrawingWand(mPtr->wandPtr);
            mPtr->wandPtr=(DrawingWand *) NULL;
            break;
        case TM_TYPE_PIXEL:
            DestroyPixelWand(mPtr->wandPtr);
            mPtr->wandPtr=(PixelWand *) NULL;
            break;
        }
    }
    if( mPtr->hashPtr != NULL ) {
        Tcl_DeleteHashEntry(mPtr->hashPtr);
        mPtr->hashPtr=(Tcl_HashEntry *) NULL;
    }
    ckfree((char *)mPtr);
}

/*----------------------------------------------------------------------
 * Create TclMagick objects
 *----------------------------------------------------------------------
 */
static TclMagickObj *newMagickObj(Tcl_Interp  *interp, int type, void *wandPtr, char *name)
{
    int isNew;
    char idString[16 + TCL_INTEGER_SPACE];
    TclMagickObj    *mPtr;
    Tcl_HashEntry   *hPtr;

    /*
     * Create the TclMagick object
     */
    mPtr = (TclMagickObj *)ckalloc(sizeof(TclMagickObj));
    if( mPtr == NULL ) {
        return NULL;
    }

    /*
     * if no wandPtr is specified create one
     */
    if( wandPtr == NULL ) {
        switch(type) {
        case TM_TYPE_WAND:
            wandPtr = NewMagickWand();
            break;
        case TM_TYPE_DRAWING:
            wandPtr = NewDrawingWand();
            break;
        case TM_TYPE_PIXEL:
            wandPtr = NewPixelWand();
            break;
        }
    }

    /*
     * if no creation ?name? is specified
     * create one from the MagickWand address
     */
    if( name == NULL ) {
        sprintf(idString, "magick%lX", (unsigned long)wandPtr );
        name = idString;
    }

    /*
     * Create the hash table entry
     * Delete already existing object
     */
    hPtr = Tcl_CreateHashEntry( &TM.hashTable, name, &isNew );
    if( ! isNew ) {
        deleteMagickObj(Tcl_GetHashValue(hPtr));
        hPtr = Tcl_CreateHashEntry( &TM.hashTable, name, &isNew );
    }
    Tcl_SetHashValue(hPtr, mPtr);

    /*
     * Initialize MagickObj fields
     * and create the Tcl command
     */
    mPtr->type    = type;
    mPtr->interp  = interp;
    mPtr->hashPtr = hPtr;
    mPtr->wandPtr = wandPtr;
    switch(type) {
    case TM_TYPE_WAND:
	mPtr->magickCmd = Tcl_CreateObjCommand(interp, name, wandObjCmd, (ClientData)mPtr, magickObjDeleteCmd);
	break;
    case TM_TYPE_DRAWING:
	mPtr->magickCmd = Tcl_CreateObjCommand(interp, name, drawObjCmd, (ClientData)mPtr, magickObjDeleteCmd);
	break;
    case TM_TYPE_PIXEL:
	mPtr->magickCmd = Tcl_CreateObjCommand(interp, name, pixelObjCmd,(ClientData)mPtr, magickObjDeleteCmd);
	break;
    }

    return mPtr;
}
static char *newWandObj(Tcl_Interp  *interp, MagickWand *wandPtr, char *name)
{
    TclMagickObj *mPtr = newMagickObj(interp, TM_TYPE_WAND, wandPtr, name);
    return getMagickObjName(mPtr);
}

#if 0     /* Currently not referenced, avoid gcc warnings */
static char *newDrawingObj(Tcl_Interp  *interp, DrawingWand *wandPtr, char *name)
{
    TclMagickObj *mPtr = newMagickObj(interp, TM_TYPE_DRAWING, wandPtr, name);
    return getMagickObjName(mPtr);
}
#endif

static char *newPixelObj(Tcl_Interp  *interp, PixelWand *wandPtr, char *name)
{
    TclMagickObj *mPtr = newMagickObj(interp, TM_TYPE_PIXEL, wandPtr, name);
    return getMagickObjName(mPtr);
}

/*----------------------------------------------------------------------
 * Check whether an empty object name is specified
 * - Should we extend this to use "none" alternatively to "" ?
 *   This reads better, but disallows "none" as an object name
 *----------------------------------------------------------------------
 */
static int noWandObj(const char *name)
{
    return (strlen(name) == 0);
}


 /*----------------------------------------------------------------------
 * Find TclMagick objects
 *----------------------------------------------------------------------
 */
TclMagickObj *findMagickObj(Tcl_Interp *interp, int type, char *name)
{
    Tcl_HashEntry *hPtr = Tcl_FindHashEntry( &TM.hashTable, name );
    TclMagickObj  *mPtr;

    if( hPtr == NULL ) {
        Tcl_AppendResult(interp, "Magick object '", name, "' not found", NULL);
        return (TclMagickObj *)NULL;
    } else {
        mPtr = (TclMagickObj *)Tcl_GetHashValue(hPtr);
        if( (type != TM_TYPE_ANY) && (mPtr->type != type) ) {
            Tcl_AppendResult(interp, "Magick object '", name, "' is not a ", objTypeNames[type], " object", NULL);
            return (TclMagickObj *)NULL;
        }
        return mPtr;
    }
}
static MagickWand *findMagickWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_WAND, name);
    if( mPtr == NULL ) {
        return (MagickWand *)NULL;
    }
    return (MagickWand *)mPtr->wandPtr;
}
static DrawingWand *findDrawingWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_DRAWING, name);
    if( mPtr == NULL ) {
        return (DrawingWand *)NULL;
    }
    return (DrawingWand *)mPtr->wandPtr;
}
static PixelWand *findPixelWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_PIXEL, name);
    if( mPtr == NULL ) {
        return (PixelWand *)NULL;
    }
    return (PixelWand *)mPtr->wandPtr;
}

/*----------------------------------------------------------------------
 * encoding functions
 *----------------------------------------------------------------------
 */
static void SetResultAsExternalString(Tcl_Interp *interp, char *ext) {
    Tcl_DString extrep;
    Tcl_ExternalToUtfDString(NULL, ext, -1, &extrep);
    Tcl_DStringResult(interp, &extrep);
    Tcl_DStringFree(&extrep);
}


/*----------------------------------------------------------------------
 * magick command
 *----------------------------------------------------------------------
 *
 * magick create type ?name?
 * magick delete name name ...
 * magick names ?type?
 * magick type name
 * magick types
 * magick resourcelimit resource ?limit?
 * magick fonts pattern
 * magick formats pattern
 *
 *----------------------------------------------------------------------
 */

static int magickCmd(
    ClientData  clientData,         /* Unused */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    int index, stat, result;

    static CONST char *subCmds[] = {
        "create",        "delete", "names",   "type",    "types",
        "resourcelimit", "fonts",  "formats", "version", "library",
        (char *) NULL};
    enum subIndex {
        TM_CREATE,   TM_DELETE, TM_NAMES,   TM_TYPE,    TM_TYPES,
        TM_RESOURCE, TM_FONTS,  TM_FORMATS, TM_VERSION, TM_LIBRARY
    };
    static CONST char *resourceNames[] = {
        "undefined", "area", "disk",
        "file",      "map",  "memory",
        "pixels", "threads", "width",
        "height",
        (char *) NULL
    };
    static ResourceType resourceTypes[] = {
        UndefinedResource, AreaResource, DiskResource,
        FileResource,      MapResource,  MemoryResource,
        PixelsResource,    ThreadsResource, WidthResource,
        HeightResource
    };

    (void) clientData; /* Unused */

    if( objc < 2 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "subcmd ?args?" );
        return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], subCmds, "subcmd", 0, &index) != TCL_OK) {
        return TCL_ERROR;
    }
    switch ((enum subIndex)index) {


    case TM_CREATE:    /* magick create type ?name? */
    {
	char *name;
	int   type;
	TclMagickObj *mPtr;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "type ?name?");
	    return TCL_ERROR;
	}
	/*
	 * Get type parameter
	 * If no creation ?name? is specified, set name=NULL
	 */
	if (Tcl_GetIndexFromObj(interp, objv[2], objTypeNames, "type", 0, &type) != TCL_OK) {
	    return TCL_ERROR;
	}
	if( objc > 3 ) {
	    name = Tcl_GetString(objv[3]);
	} else {
	    name = NULL;
	}
	mPtr = newMagickObj(interp, type, NULL, name);
	name = getMagickObjName(mPtr);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_DELETE:    /* magick delete name name ...*/
    {
	int	 i;
	char *name;
	TclMagickObj *mPtr;

	for (i = 2; i < objc; i++) {
	    name = Tcl_GetString(objv[i]);
	    mPtr = findMagickObj(interp, TM_TYPE_ANY, name);
	    if (mPtr == NULL) {
		return TCL_ERROR;
	    }
	    deleteMagickObj(mPtr);
	}
	break;
    }

    case TM_NAMES:    /* magick names ?type? */
    {
	int type = TM_TYPE_ANY;
	TclMagickObj   *mPtr;
	Tcl_HashEntry  *hPtr;
	Tcl_HashSearch search;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?type?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    if (Tcl_GetIndexFromObj(interp, objv[2], objTypeNames, "type", 0, &type) != TCL_OK) {
		return TCL_ERROR;
	    }
	}
	hPtr = Tcl_FirstHashEntry(&TM.hashTable, &search);
	for ( ; hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    mPtr = Tcl_GetHashValue(hPtr);
	    if( (type == TM_TYPE_ANY) || (type == mPtr->type) ) {
		Tcl_AppendElement(interp, getMagickObjName(mPtr));
	    }
	}
	break;
    }

    case TM_TYPE:    /* magick type name */
    {
	char *name;
	TclMagickObj *mPtr;

	if( objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	mPtr = findMagickObj(interp, TM_TYPE_ANY, name);
	if (mPtr == NULL) {
	    return TCL_ERROR;
	}
	Tcl_SetResult(interp, (char *)objTypeNames[mPtr->type], TCL_VOLATILE);

	break;
    }

    case TM_TYPES:    /* magick types */
    {
	int type;

	if( objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	for ( type = 0; type < TM_TYPE_ANY; type++ ) {
	    Tcl_AppendElement(interp, objTypeNames[type]);
	}
	break;
    }

    case TM_RESOURCE:      /* resourcelimit resourceType ?limit? */
    {
        long limit;
        int  resourceIdx;

	if( (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "resourceType ?limit?");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], resourceNames,
				"resourceType", 0, &resourceIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set channel depth
	     */
	    if( (stat = Tcl_GetLongFromObj(interp, objv[3], &limit)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetResourceLimit(resourceTypes[resourceIdx], (unsigned long) limit);
	    if (!result) {
		return myMagickError(interp, NULL);
	    }
	} else {
	    /*
	     * Get channel depth
	     */
	    limit = MagickGetResourceLimit(resourceTypes[resourceIdx]);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)limit));
	}
	break;
    }

    case TM_FONTS:    /* fonts pattern */
    {
        unsigned long  i;
	unsigned long  listLen = 0;
        Tcl_Obj        *listPtr;
        char           *pattern = "*";
        char          **fonts;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pattern?");
	    return TCL_ERROR;
	}
        if( objc > 2 ) {
            pattern = Tcl_GetString(objv[2]);
        }

	fonts = MagickQueryFonts(pattern, &listLen);
	if( (fonts != NULL) && (listLen > 0) ) {
            listPtr = Tcl_NewListObj(0, NULL);
	    for( i=0; i < listLen; i++ ) {
                Tcl_ListObjAppendElement(interp, listPtr,
					 Tcl_NewStringObj(fonts[i], (int)strlen(fonts[i])) );
            }
    	    Tcl_SetObjResult(interp, listPtr);
        }
	if (fonts != NULL)
	    MagickRelinquishMemory(fonts); /* Free TclMagick resource */

        break;
    }

    case TM_FORMATS:    /* formats pattern */
    {
        unsigned long   i;
	unsigned long   listLen = 0;
        Tcl_Obj        *listPtr;
        char           *pattern = "*";
        char          **fonts;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pattern?");
	    return TCL_ERROR;
	}
        if( objc > 2 ) {
            pattern = Tcl_GetString(objv[2]);
        }

	fonts = MagickQueryFormats(pattern, &listLen);
	if( (fonts != NULL) && (listLen > 0) ) {
            listPtr = Tcl_NewListObj(0, NULL);
	    for( i=0; i < listLen; i++ ) {
                Tcl_ListObjAppendElement(interp, listPtr,
					 Tcl_NewStringObj(fonts[i], (int)strlen(fonts[i])) );
            }
    	    Tcl_SetObjResult(interp, listPtr);
        }
	if (fonts != NULL)
	    MagickRelinquishMemory(fonts); /* Free TclMagick resource */

        break;
    }

    case TM_VERSION:    /* magick version */
    {
	if( objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	Tcl_SetResult(interp, VERSION, TCL_VOLATILE);

	break;
    }

    case TM_LIBRARY:    /* magick library ?-option? */
    {
	static CONST char *options[] = {
	    "-copyright",  "-date",    "-name",       "-quantumdepth", 
	    "-url",        "-version", "-versionstr", "-maxrgb",
	    (char *) NULL
	};
        char  buf[40];
        const char *str;
        unsigned long depth=0, version=0, maxrgb=0;

        if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?-option?");
	    return TCL_ERROR;
	}
	/*
	 * If called without parameters, return a list of
	 * -option value -option value ...
	 */
	if( objc == 2 ) {
	    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);

            str = MagickGetCopyright();
	    Tcl_ListObjAppendElement(
		interp, listPtr,
		Tcl_NewStringObj(options[0], -1));
	    Tcl_ListObjAppendElement(
		interp, listPtr, Tcl_NewStringObj(str, -1));

            str = MagickGetReleaseDate();
	    Tcl_ListObjAppendElement(
		interp, listPtr,
		Tcl_NewStringObj(options[1], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(str, -1));

            str = MagickGetPackageName();
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[2], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(str, -1));

            str = MagickGetQuantumDepth(&depth);
            maxrgb = (1 << depth) - 1;
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[3], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj((signed long)depth));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[7], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj((signed long)maxrgb));

            str = MagickGetHomeURL();
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[4], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(str, -1));

            str = MagickGetVersion(&version);
            sprintf( buf, "%ld.%ld.%ld", version >> 8, (version >> 4) & 0x0F, version & 0x0F);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[5], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(buf, -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(options[6], -1));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewStringObj(str, -1));


            Tcl_SetObjResult(interp, listPtr);
        } else {
            int idx;

            if (Tcl_GetIndexFromObj(interp, objv[2], options, "-option", 0, &idx) != TCL_OK) {
	        return TCL_ERROR;
            }
            switch( idx ) {
            case 0: /* -copyright */
                str = MagickGetCopyright();
                Tcl_SetObjResult(interp, Tcl_NewStringObj(str, -1));
                break;
            case 1: /* -date */
                str = MagickGetReleaseDate();
                Tcl_SetObjResult(interp, Tcl_NewStringObj(str, -1));
                break;
            case 2: /* -name */
                str = MagickGetPackageName();
                Tcl_SetObjResult(interp, Tcl_NewStringObj(str, -1));
                break;
            case 3: /* -quantumdepth */
                MagickGetQuantumDepth(&depth);
                Tcl_SetObjResult(interp, Tcl_NewLongObj((signed long)depth));
                break;
            case 4: /* -url */
                str = MagickGetHomeURL();
                Tcl_SetObjResult(interp, Tcl_NewStringObj(str, -1));
                break;
            case 5: /* -version */
                MagickGetVersion(&version);
                sprintf( buf, "%ld.%ld.%ld", version >> 8, (version >> 4) & 0x0F, version & 0x0F);
                Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
                break;
            case 6: /* -versionstr */
                str = MagickGetVersion(&version);
                Tcl_SetObjResult(interp, Tcl_NewStringObj(str, -1));
                break;
            case 7: /* -maxrgb */
                MagickGetQuantumDepth(&depth);
		maxrgb = (1 << depth) - 1;
                Tcl_SetObjResult(interp, Tcl_NewLongObj((signed long)maxrgb));
                break;
            default:
                break;
            }

        }

	break;
    }

    } /* switch(index) */

    return(TCL_OK);
}


/*----------------------------------------------------------------------
 * MagickWand object command
 *----------------------------------------------------------------------
 *
 * MagickWand subcmd ?args?
 *
 *----------------------------------------------------------------------
 */
static int wandObjCmd(
    ClientData  clientData,         /* MagickWand pointer */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    static CONST char *subCmds[] = {
        "adaptivethreshold","AdaptiveThresholdImage",
        "add",              "AddImage",
        "addnoise",         "AddNoiseImage",
        "affinetransform",  "AffineTransformImage",
        "annotate",         "AnnotateImage",
        "append",           "AppendImages",
        "average",          "AverageImages",
        "blackthreshold",   "BlackThresholdImage",
        "blur",             "BlurImage",
        "border",           "BorderImage",
        "charcoal",         "CharcoalImage",
        "chop",             "ChopImage",
        "clip",             "ClipImage",
        "clippath",         "ClipPathImage",
        "clone",            "CloneWand",
        "coalesce",         "CoalesceImages",
        "colorfloodfill",   "ColorFloodfillImage",
        "colorize",         "ColorizeImage",
        "comment",          "CommentImage",
        "comparechannels",  "CompareImageChannels",
        "compare",          "CompareImages",
        "composite",        "CompositeImage",
        "contrast",         "ContrastImage",
        "convolve",         "ConvolveImage",
        "crop",             "CropImage",
        "cyclecolormap",    "CycleColormapImage",
        "deconstruct",      "DeconstructImages",
        "describe",         "DescribeImage",
        "despeckle",        "DespeckleImage",
        "draw",             "DrawImage",
        "edge",             "EdgeImage",
        "emboss",           "EmbossImage",
        "enhance",          "EnhanceImage",
        "equalize",         "EqualizeImage",
        "flatten",          "FlattenImages",
        "flip",             "FlipImage",
        "flop",             "FlopImage",
        "frame",            "FrameImage",
        "fx",               "FxImage",
        "fxchannel",        "FxImageChannel",
        "gammaimage",       "GammaImage",
        "gammachannel",     "GammaImageChannel",
        "filename",         "GetFilename",          "SetFilename",
        "backgroundcolor",  "GetBackgroundColor",   "SetBackgroundColor",
        "blueprimary",      "GetBluePrimary",       "SetBluePrimary",
        "bordercolor",      "GetBorderColor",       "SetBorderColor",
        "channeldepth",     "GetChannelDepth",      "SetChannelDepth",
        "channelextrema",   "GetChannelExtrema",
        "channelmean",      "GetChannelMean",
        "colormapcolor",    "GetColormapColor",     "SetColormapColor",
        "colors",           "GetColors",
        "colorspace",       "GetColorspace",        "SetColorspace",
        "compose",          "GetCompose",           "SetCompose",
        "compression",      "GetCompression",       "SetCompression",
        "delay",            "GetDelay",             "SetDelay",
        "depth",            "GetDepth",             "SetDepth",
        "dispose",          "GetDispose",           "SetDispose",
        "extrema",          "GetExtrema",
        "format",           "GetFormat",            "SetFormat",
        "getimage",         "GetImage",
        "imagefilename",    "GetImageFilename",     "SetImageFilename",
        "gamma",            "GetGamma",             "SetGamma",
        "greenprimary",     "GetGreenPrimary",      "SetGreenPrimary",
        "height",           "GetHeight",
        "index",            "GetIndex",             "SetIndex",
        "interlace",        "GetInterlaceScheme",   "SetInterlaceScheme",
        "iterations",       "GetIterations",        "SetIterations",
        "mattecolor",       "GetMatteColor",        "SetMatteColor",
        "pixels",           "GetPixels",            "SetPixels",
        "profile",          "GetProfile",           "SetProfile",
        "ProfileImage",     "RemoveProfile",
        "redprimary",       "GetRedPrimary",        "SetRedPrimary",
        "renderingintent",  "GetRenderingIntent",   "SetRenderingIntent",
        "resolution",       "GetResolution",        "SetResolution",
        "scene",            "GetScene",             "SetScene",
        "signature",        "GetSignature",
        "imagesize",        "GetImageSize",
        "imagetype",        "GetImageType",         "SetImageType",
        "imageunits",       "GetImageUnits",        "SetImageUnits",
        "virtualpixelmethod","GetVirtualPixelMethod","SetVirtualPixelMethod",
        "whitepoint",       "GetWhitePoint",        "SetWhitePoint",
        "width",            "GetWidth",
        "number",           "GetNumberImages",
        "samplingfactors",  "GetSamplingFactors",   "SetSamplingFactors",
        "size",             "GetSize",              "SetSize",
        "hasnext",          "HasNext",
        "hasprevious",      "HasPrevious",
        "implode",          "ImplodeImage",
        "label",            "LabelImage",
        "level",            "LevelImage",
        "levelchannel",     "LevelImageChannel",
        "magnify",          "MagnifyImage",
        "map",              "MapImage",
        "mattefloodfill",   "MatteFloodfillImage",
        "medianfilter",     "MedianFilterImage",
        "minify",           "MinifyImage",
        "modulate",         "ModulateImage",
        "montage",          "MontageImage",
        "morph",            "MorphImages",
        "mosaic",           "MosaicImages",
        "motionblur",       "MotionBlurImage",
        "negate",           "NegateImage",
        "negatechannel",    "NegateImageChannel",
        "next",             "NextImage",
        "normalize",        "NormalizeImage",
        "oilpaint",         "OilPaintImage",
        "opaque",           "OpaqueImage",
        "ping",             "PingImage",
        "preview",          "PreviewImages",
        "previous",         "PreviousImage",
        "quantize",         "QuantizeImage",        "QuantizeImages",
        "queryfontmetrics", "QueryFontMetrics",
        "raise",            "RaiseImage",
        "read",             "ReadImage",
        "readblob",         "ReadImageBlob",
        "reducenoise",      "ReduceNoiseImage",
        "remove",           "RemoveImage",
        "resetiterator",    "ResetIterator",
        "resample",         "ResampleImage",
        "resize",           "ResizeImage",
        "roll",             "RollImage",
        "rotate",           "RotateImage",
        "sample",           "SampleImage",
        "scale",            "ScaleImage",
        "separate",         "SeparateImageChannel",
        "setimage",         "SetImage",
        "setoption",        "SetOption",
        "passphrase",       "SetPassphrase",
        "sharpen",          "SharpenImage",
        "shave",            "ShaveImage",
        "shear",            "ShearImage",
        "solarize",         "SolarizeImage",
        "spread",           "SpreadImage",
        "stegano",          "SteganoImage",
        "stereo",           "StereoImage",
        "strip",            "StripImage",
        "swirl",            "SwirlImage",
        "texture",          "TextureImage",
        "threshold",        "ThresholdImage",
        "thresholdchannel", "ThresholdImageChannel",
        "tint",             "TintImage",
        "transform",        "TransformImage",
        "transparent",      "TransparentImage",
        "trim",             "TrimImage",
        "unsharpmask",      "UnsharpMaskImage",
        "wave",             "WaveImage",
        "whitethreshold",   "WhiteThresholdImage",
        "write",            "WriteImage",           "WriteImages",
        "writeblob",        "WriteImageBlob",
        (char *) NULL
    };
    enum subIndex {
        TM_ADAPTIVE,        TM_ADAPTIVE_THRESHOLD_IMAGE,
        TM_ADD,             TM_ADD_IMAGE,
        TM_ADDNOISE,        TM_ADD_NOISE_IMAGE,
        TM_AFFINE_TRANSFORM,TM_AFFINE_TRANSFORM_IMAGE,
        TM_ANNOTATE,        TM_ANNOTATE_IMAGE,
        TM_APPEND,          TM_APPEND_IMAGES,
        TM_AVERAGE,         TM_AVERAGE_IMAGES,
        TM_BLACK_THRESHOLD, TM_BLACK_THRESHOLD_IMAGE,
        TM_BLUR,            TM_BLUR_IMAGE,
        TM_BORDER,          TM_BORDER_IMAGE,
        TM_CHARCOAL,        TM_CHARCOAL_IMAGE,
        TM_CHOP,            TM_CHOP_IMAGE,
        TM_CLIP,            TM_CLIP_IMAGE,
        TM_CLIP_PATH,       TM_CLIP_PATH_IMAGE,
        TM_CLONE,           TM_CLONE_WAND,
        TM_COALESCE,        TM_COALESCE_IMAGES,
        TM_COLOR_FLOODFILL, TM_COLOR_FLOODFILL_IMAGE,
        TM_COLORIZE,        TM_COLORIZE_IMAGE,
        TM_COMMENT,         TM_COMMENT_IMAGE,
        TM_COMPARE_CHANNELS,TM_COMPARE_IMAGE_CHANNELS,
        TM_COMPARE,         TM_COMPARE_IMAGES,
        TM_COMPOSITE,       TM_COMPOSITE_IMAGE,
        TM_CONTRAST,        TM_CONTRAST_IMAGE,
        TM_CONVOLVE,        TM_CONVOLVE_IMAGE,
        TM_CROP,            TM_CROP_IMAGE,
        TM_CYCLE_COLORMAP,  TM_CYCLE_COLORMAP_IMAGE,
        TM_DECONSTRUCT,     TM_DECONSTRUCT_IMAGES,
        TM_DESCRIBE,        TM_DESCRIBE_IMAGE,
        TM_DESPECKLE,       TM_DESPECKLE_IMAGE,
        TM_DRAW,            TM_DRAW_IMAGE,
        TM_EDGE,            TM_EDGE_IMAGE,
        TM_EMBOSS,          TM_EMBOSS_IMAGE,
        TM_ENHANCE,         TM_ENHANCE_IMAGE,
        TM_EQUALIZE,        TM_EQUALIZE_IMAGE,
        TM_FLATTEN,         TM_FLATTEN_IMAGES,
        TM_FLIP,            TM_FLIP_IMAGE,
        TM_FLOP,            TM_FLOP_IMAGE,
        TM_FRAME,           TM_FRAME_IMAGE,
        TM_FX,              TM_FX_IMAGE,
        TM_FX_CHANNEL,      TM_FX_IMAGE_CHANNEL,
        TM_GAMMAIMAGE,      TM_GAMMA_IMAGE,
        TM_GAMMACHANNEL,    TM_GAMMA_IMAGE_CHANNEL,
        TM_FILENAME,        TM_GET_FILENAME,            TM_SET_FILENAME,
        TM_BACKGROUND_COLOR,TM_GET_BACKGROUND_COLOR,    TM_SET_BACKGROUND_COLOR,
        TM_BLUE_PRIMARY,    TM_GET_BLUE_PRIMARY,        TM_SET_BLUE_PRIMARY,
        TM_BORDER_COLOR,    TM_GET_BORDER_COLOR,        TM_SET_BORDER_COLOR,
        TM_CHANNEL_DEPTH,   TM_GET_CHANNEL_DEPTH,       TM_SET_CHANNEL_DEPTH,
        TM_CHANNEL_EXTREMA, TM_GET_CHANNEL_EXTREMA,
        TM_CHANNEL_MEAN,    TM_GET_CHANNEL_MEAN,
        TM_COLORMAP_COLOR,  TM_GET_COLORMAP_COLOR,      TM_SET_COLORMAP_COLOR,
        TM_COLORS,          TM_GET_COLORS,
        TM_COLORSPACE,      TM_GET_COLORSPACE,          TM_SET_COLORSPACE,
        TM_COMPOSE,         TM_GET_COMPOSE,             TM_SET_COMPOSE,
        TM_COMPRESSION,     TM_GET_COMPRESSION,         TM_SET_COMPRESSION,
        TM_DELAY,           TM_GET_DELAY,               TM_SET_DELAY,
        TM_DEPTH,           TM_GET_DEPTH,               TM_SET_DEPTH,
        TM_DISPOSE,         TM_GET_DISPOSE,             TM_SET_DISPOSE,
        TM_EXTREMA,         TM_GET_EXTREMA,
        TM_FORMAT,          TM_GET_FORMAT,              TM_SET_FORMAT,
        TM_GETIMAGE,        TM_GET_IMAGE,
        TM_IMAGE_FILENAME,  TM_GET_IMAGE_FILENAME,      TM_SET_IMAGE_FILENAME,
        TM_GAMMA,           TM_GET_GAMMA,               TM_SET_GAMMA,
        TM_GREEN_PRIMARY,   TM_GET_GREEN_PRIMARY,       TM_SET_GREEN_PRIMARY,
	TM_HEIGHT,          TM_GET_HEIGHT,
        TM_INDEX,           TM_GET_INDEX,               TM_SET_INDEX,
        TM_INTERLACE,       TM_GET_INTERLACE,           TM_SET_INTERLACE,
        TM_ITERATIONS,      TM_GET_ITERATIONS,          TM_SET_ITERATIONS,
        TM_MATTE_COLOR,     TM_GET_MATTE_COLOR,         TM_SET_MATTE_COLOR,
        TM_PIXELS,          TM_GET_PIXELS,              TM_SET_PIXELS,
        TM_PROFILE,         TM_GET_PROFILE,             TM_SET_PROFILE,
        TM_PROFILE_IMAGE,   TM_REMOVE_PROFILE,
        TM_RED_PRIMARY,     TM_GET_RED_PRIMARY,         TM_SET_RED_PRIMARY,
        TM_RENDERING,       TM_GET_RENDERING,           TM_SET_RENDERING,
        TM_RESOLUTION,      TM_GET_RESOLUTION,          TM_SET_RESOLUTION,
        TM_SCENE,           TM_GET_SCENE,               TM_SET_SCENE,
        TM_SIGNATURE,       TM_GET_SIGNATURE,
        TM_IMAGE_SIZE,      TM_GET_IMAGE_SIZE,
        TM_IMAGE_TYPE,      TM_GET_IMAGE_TYPE,          TM_SET_IMAGE_TYPE,
        TM_IMAGE_UNITS,     TM_GET_IMAGE_UNITS,         TM_SET_IMAGE_UNITS,
        TM_VIRTUALPIXEL,    TM_GET_VIRTUALPIXEL,        TM_SET_VIRTUALPIXEL,
        TM_WHITE_POINT,     TM_GET_WHITE_POINT,         TM_SET_WHITE_POINT,
	TM_WIDTH,           TM_GET_WIDTH,
        TM_NUMBER,          TM_GET_NUMBER_IMAGES,
        TM_SAMPLING_FACTORS,TM_GET_SAMPLING_FACTORS,    TM_SET_SAMPLING_FACTORS,
        TM_SIZE,            TM_GET_SIZE,                TM_SET_SIZE,
	TM_HASNEXT,         TM_HAS_NEXT_IMAGE,
	TM_HASPREVIOUS,     TM_HAS_PREVIOUS_IMAGE,
        TM_IMPLODE,         TM_IMPLODE_IMAGE,
        TM_LABEL,           TM_LABEL_IMAGE,
        TM_LEVEL,           TM_LEVEL_IMAGE,
        TM_LEVEL_CHANNEL,   TM_LEVEL_IMAGE_CHANNEL,
        TM_MAGNIFY,         TM_MAGNIFY_IMAGE,
        TM_MAP,             TM_MAP_IMAGE,
        TM_MATTE_FLOODFILL, TM_MATTE_FLOODFILL_IMAGE,
        TM_MEDIANFILTER,    TM_MEDIANFILTER_IMAGE,
        TM_MINIFY,          TM_MINIFY_IMAGE,
        TM_MODULATE,        TM_MODULATE_IMAGE,
        TM_MONTAGE,         TM_MONTAGE_IMAGE,
        TM_MORPH,           TM_MORPH_IMAGES,
        TM_MOSAIC,          TM_MOSAIC_IMAGES,
        TM_MOTIONBLUR,      TM_MOTIONBLUR_IMAGE,
        TM_NEGATE,          TM_NEGATE_IMAGE,
        TM_NEGATE_CHANNEL,  TM_NEGATE_IMAGE_CHANNEL,
        TM_NEXT,            TM_NEXT_IMAGE,
        TM_NORMALIZE,       TM_NORMALIZE_IMAGE,
        TM_OILPAINT,        TM_OILPAINT_IMAGE,
        TM_OPAQUE,          TM_OPAQUE_IMAGE,
        TM_PING,            TM_PING_IMAGE,
        TM_PREVIEW,         TM_PREVIEW_IMAGES,
        TM_PREVIOUS,        TM_PREVIOUS_IMAGE,
        TM_QUANTIZE,        TM_QUANTIZE_IMAGE,      TM_QUANTIZE_IMAGES,
        TM_QUERYFONTMETRICS,TM_QUERY_FONT_METRICS,
        TM_RAISE,           TM_RAISE_IMAGE,
        TM_READ,            TM_READ_IMAGE,
        TM_READ_BLOB,       TM_READ_IMAGE_BLOB,
        TM_REDUCENOISE,     TM_REDUCENOISE_IMAGE,
        TM_REMOVE,          TM_REMOVE_IMAGE,
        TM_RESETITERATOR,   TM_RESET_ITERATOR,
        TM_RESAMPLE,        TM_RESAMPLE_IMAGE,
        TM_RESIZE,          TM_RESIZE_IMAGE,
        TM_ROLL,            TM_ROLL_IMAGE,
        TM_ROTATE,          TM_ROTATE_IMAGE,
        TM_SAMPLE,          TM_SAMPLE_IMAGE,
        TM_SCALE,           TM_SCALE_IMAGE,
        TM_SEPARATE,        TM_SEPARATE_CHANNEL,
        TM_SETIMAGE,        TM_SET_IMAGE,
        TM_SETOPTION,       TM_SET_OPTION,
        TM_PASSPHRASE,      TM_SET_PASSPHRASE,
        TM_SHARPEN,         TM_SHARPEN_IMAGE,
        TM_SHAVE,           TM_SHAVE_IMAGE,
        TM_SHEAR,           TM_SHEAR_IMAGE,
        TM_SOLARIZE,        TM_SOLARIZE_IMAGE,
        TM_SPREAD,          TM_SPREAD_IMAGE,
        TM_STEGANO,         TM_STEGANO_IMAGE,
        TM_STEREO,          TM_STEREO_IMAGE,
        TM_STRIP,           TM_STRIP_IMAGE,
        TM_SWIRL,           TM_SWIRL_IMAGE,
        TM_TEXTURE,         TM_TEXTURE_IMAGE,
        TM_THRESHOLD,       TM_THRESHOLD_IMAGE,
        TM_THRESHOLDCHANNEL,TM_THRESHOLD_IMAGE_CHANNEL,
        TM_TINT,            TM_TINT_IMAGE,
        TM_TRANSFORM,       TM_TRANSFORM_IMAGE,
        TM_TRANSPARENT,     TM_TRANSPARENT_IMAGE,
        TM_TRIM,            TM_TRIM_IMAGE,
        TM_UNSHARPMASK,     TM_UNSHARPMASK_IMAGE,
        TM_WAVE,            TM_WAVE_IMAGE,
        TM_WHITE_THRESHOLD, TM_WHITE_THRESHOLD_IMAGE,
        TM_WRITE,           TM_WRITE_IMAGE,         TM_WRITE_IMAGES,
        TM_WRITE_BLOB,      TM_WRITE_IMAGE_BLOB,
        TM_END_OF_TABLE
    };
    static CONST char *filterNames[] = {
        "undefined", "point",   "box",      "triangle", "hermite",
        "hanning",   "hamming", "blackman", "gaussian", "quadratic",
        "cubic",     "catrom",  "mitchell", "lanczos",  "bessel",
        "sinc",
        (char *) NULL
    };
    static FilterTypes filterTypes[] = {
        UndefinedFilter, PointFilter,   BoxFilter,      TriangleFilter, HermiteFilter,
        HanningFilter,   HammingFilter, BlackmanFilter, GaussianFilter, QuadraticFilter,
        CubicFilter,     CatromFilter,  MitchellFilter, LanczosFilter,  BesselFilter,
        SincFilter
    };
    static CONST char *chanNames[] = {
        "undefined", "red",    "cyan",    "green", "magenta",
        "blue",      "yellow", "opacity", "black", "matte",
        "index",
        (char *) NULL
    };
    static ChannelType chanTypes[] = {
        UndefinedChannel, RedChannel, CyanChannel, GreenChannel, MagentaChannel,
        BlueChannel, YellowChannel, OpacityChannel, BlackChannel, MatteChannel,
        IndexChannel
    };
    static CONST char *metricNames[] = {
        "meanabsoluteerror",       "meansquarederror",    "peakabsoluteerror",
        "peaksignaltonoiseratio",  "rootmeansquarederror",
        (char *) NULL
    };
    static ChannelType metricTypes[] = {
        MeanAbsoluteErrorMetric, MeanSquaredErrorMetric,  PeakAbsoluteErrorMetric,
        PeakSignalToNoiseRatioMetric, RootMeanSquaredErrorMetric
    };
    static CONST char *csNames[] = {
        "undefined", "RGB",   "GRAY",  "transparent",
        "OHTA",      "LAB",   "XYZ",   "YCbCr",
        "YCC",       "YIQ",   "YPbPr", "YUV",
        "CMYK",      "sRGB",  "HSL",   "HWB",
        (char *) NULL
    };
    static ColorspaceType csTypes[] = {
        UndefinedColorspace, RGBColorspace,   GRAYColorspace,  TransparentColorspace,
        OHTAColorspace,      LABColorspace,   XYZColorspace,   YCbCrColorspace,
        YCCColorspace,       YIQColorspace,   YPbPrColorspace, YUVColorspace,
        CMYKColorspace,      sRGBColorspace,  HSLColorspace,   HWBColorspace
    };
    static CONST char *opNames[] = {
        "undefined", "over",        "in",         "out",
        "atop",      "xor",         "plus",       "minus",
        "add",       "subtract",    "difference", "multiply",
        "bumpmap",   "copy",        "copyred",    "copygreen",
        "copyblue",  "copyopacity", "clear",      "dissolve"
        "displace",  "modulate",    "threshold",  "no",
        "darken",    "lighten",     "hue",        "saturate",
        "colorize",  "luminize",    "screen",     "overlay",
        "copycyan",  "copymagenta", "copyyellow", "copyblack",
        "divide",    "hardlight",   "exclusion",  "colordodge",
        "colorburn", "softlight",   "linearburn", "lineardodge",
        "linearlight","vividlight", "pinlight",   "hardmix",
	(char *) NULL
    };
    static CompositeOperator opTypes[] = {
        UndefinedCompositeOp, OverCompositeOp,        InCompositeOp,         OutCompositeOp,
        AtopCompositeOp,      XorCompositeOp,         PlusCompositeOp,       MinusCompositeOp,
        AddCompositeOp,       SubtractCompositeOp,    DifferenceCompositeOp, MultiplyCompositeOp,
        BumpmapCompositeOp,   CopyCompositeOp,        CopyRedCompositeOp,    CopyGreenCompositeOp,
        CopyBlueCompositeOp,  CopyOpacityCompositeOp, ClearCompositeOp,      DissolveCompositeOp,
        DisplaceCompositeOp,  ModulateCompositeOp,    ThresholdCompositeOp,  NoCompositeOp,
        DarkenCompositeOp,    LightenCompositeOp,     HueCompositeOp,        SaturateCompositeOp,
        ColorizeCompositeOp,  LuminizeCompositeOp,    ScreenCompositeOp,     OverlayCompositeOp,
        CopyCyanCompositeOp,  CopyMagentaCompositeOp, CopyYellowCompositeOp, CopyBlackCompositeOp,
        DivideCompositeOp,    HardLightCompositeOp,   ExclusionCompositeOp,  ColorDodgeCompositeOp,
	ColorBurnCompositeOp, SoftLightCompositeOp,   LinearBurnCompositeOp, LinearDodgeCompositeOp,
        LinearLightCompositeOp,VividLightCompositeOp, PinLightCompositeOp,   HardMixCompositeOp
    };
    static CONST char *compressNames[] = {
        "undefined", "none",   "bzip",
        "fax",       "group4", "jpeg",
        "jpeg-ls",   "lzw",    "rle",
        "zip"
#if MagickLibInterfaceNewest > 9
	/* These enumerations are introduced by GraphicsMagick 1.3.13 */
        , "lzma",   "jpeg2000", "jbig1",     "jbig2"
#endif /* MagickLibInterfaceNewest > 9 */
        ,(char *) NULL
    };
    static CompressionType compressTypes[] = {
        UndefinedCompression,    NoCompression,     BZipCompression,
        FaxCompression,          Group4Compression, JPEGCompression,
        LosslessJPEGCompression, LZWCompression,    RLECompression,
        ZipCompression
#if MagickLibInterfaceNewest > 9
	/* These enumerations are introduced by GraphicsMagick 1.3.13 */
        ,LZMACompression,         JPEG2000Compression,
	JBIG1Compression,        JBIG2Compression
#endif /* MagickLibInterfaceNewest > 9 */
    };
    static CONST char *disposeNames[] = {
        "undefined", "none",    "background",    "previous",
        (char *) NULL
    };
    static DisposeType disposeTypes[] = {
        UndefinedDispose, NoneDispose, BackgroundDispose, PreviousDispose
    };
    static CONST char *interlaceNames[] = {
        "undefined", "none",    "line",    "plane", "partition",
        (char *) NULL
    };
    static InterlaceType interlaceTypes[] = {
        UndefinedInterlace, NoInterlace, LineInterlace, PlaneInterlace, PartitionInterlace,
    };
    static CONST char *renderNames[] = {
        "undefined", "saturation", "perceptual", "absolute", "relative",
        (char *) NULL
    };
    static RenderingIntent renderTypes[] = {
        UndefinedIntent, SaturationIntent, PerceptualIntent, AbsoluteIntent, RelativeIntent
    };
    static CONST char *typeNames[] = {
        "undefined",            "bilevel",        "grayscale",
        "grayscalematte",       "palette",        "palettematte",
        "truecolor",            "truecolormatte", "colorseparation",
        "colorseparationmatte", "optimize",
        (char *) NULL
    };
    static ImageType typeTypes[] = {
        UndefinedType,            BilevelType,        GrayscaleType,
        GrayscaleMatteType,       PaletteType,        PaletteMatteType,
        TrueColorType,            TrueColorMatteType, ColorSeparationType,
        ColorSeparationMatteType, OptimizeType
    };
    static CONST char *unitNames[] = {
        "undefined", "ppi", "ppcm",
        (char *) NULL
    };
    static ResolutionType unitTypes[] = {
        UndefinedResolution, PixelsPerInchResolution, PixelsPerCentimeterResolution
    };
    static CONST char *methodNames[] = {
        "undefined", "constant", "edge",
        "mirror",    "tile",
        (char *) NULL
    };
    static VirtualPixelMethod methodTypes[] = {
        UndefinedVirtualPixelMethod, ConstantVirtualPixelMethod, EdgeVirtualPixelMethod,
        MirrorVirtualPixelMethod,    TileVirtualPixelMethod
    };
    static CONST char *previewNames[] = {
        "undefined", "rotate",      "shear",      "roll",
        "hue",       "saturation",  "brightness", "gamma",
        "spiff",     "dull",        "grayscale",  "quantize",
        "despeckle", "reducenoise", "addnoise",   "sharpen",
        "blur",      "threshold",   "edgedetect", "spread",
        "solarize",  "shade",       "raise",      "segment",
        "swirl",     "implode",     "wave",       "oilpaint",
        "charcoal",  "jpeg",
        (char *) NULL
    };
    static PreviewType previewTypes[] = {
        UndefinedPreview,       RotatePreview,      ShearPreview,      RollPreview,
        HuePreview,             SaturationPreview,  BrightnessPreview, GammaPreview,
        SpiffPreview,           DullPreview,        GrayscalePreview,  QuantizePreview,
        DespecklePreview,       ReduceNoisePreview, AddNoisePreview,   SharpenPreview,
        BlurPreview,            ThresholdPreview,   EdgeDetectPreview, SpreadPreview,
        SolarizePreview,        ShadePreview,       RaisePreview,      SegmentPreview,
        SwirlPreview,           ImplodePreview,     WavePreview,       OilPaintPreview,
        CharcoalDrawingPreview, JPEGPreview
    };

    int index, stat, result=TCL_OK;
    TclMagickObj *magickPtr = (TclMagickObj *) clientData;
    MagickWand   *wandPtr   = (MagickWand *) magickPtr->wandPtr;

#ifdef MAGICK_DEBUG
    /*
     * Verify subCmds table size
     * No idea how to do this at compile-time
     */
    if( sizeof(subCmds)/sizeof(subCmds[0]) != TM_END_OF_TABLE + 1 ) {
        Tcl_Panic("magickObjCmd: Invalid subCmds[] table");
    }
#endif

    if( objc < 2 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "subcmd ?args?" );
        return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], subCmds, "subcmd", 0, &index) != TCL_OK) {
        return TCL_ERROR;
    }

    switch ((enum subIndex)index) {

    case TM_ADAPTIVE:                   /* adaptive ?width? ?height? ?ofs? */
    case TM_ADAPTIVE_THRESHOLD_IMAGE:   /* AdaptiveThresholdImage ?width? ?height? ?ofs? */
    {
	unsigned long width=1, height=1;
	long ofs=0;

	if( (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?width? ?height? ?ofs?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 3) && ((stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &height)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], &ofs)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickAdaptiveThresholdImage(wandPtr, width, height, ofs);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ADD:            /* add name */
    case TM_ADD_IMAGE:      /* AddImage name */
    {
	MagickWand      *addWand;
	char            *name;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (addWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	result = MagickAddImage( wandPtr, addWand );
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_ADDNOISE:          /* addnoise ?noiseType? */
    case TM_ADD_NOISE_IMAGE:   /* AddNoiseImage ?noiseType? */
    {
	static CONST char *noiseNames[] = {
	    "uniform", "gaussian", "multiplicativegaussian", "impulse", "laplacian", "poisson", "random",
	    (char *) NULL
	};
	static NoiseType noiseTypes[] = {
	    UniformNoise, GaussianNoise, MultiplicativeGaussianNoise, ImpulseNoise, LaplacianNoise, PoissonNoise, RandomNoise
	};
	int noiseIdx=0;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?noiseType=uniform?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && (Tcl_GetIndexFromObj(interp, objv[2], noiseNames, "noiseType", 0, &noiseIdx) != TCL_OK) ) {
	    return TCL_ERROR;
	}
	result = MagickAddNoiseImage(wandPtr, noiseTypes[noiseIdx]);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_AFFINE_TRANSFORM:       /* affinetransform draw */
    case TM_AFFINE_TRANSFORM_IMAGE: /* AffineTransformImage draw */
    {
	char	*name;
	DrawingWand	*drawPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "draw");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}

	result = MagickAffineTransformImage(wandPtr, drawPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ANNOTATE:       /* annotate draw ?x y? ?angle? txt */
    case TM_ANNOTATE_IMAGE: /* AnnotateImage draw ?x y? ?angle? txt */
        {
	    char	*name;
	    DrawingWand	*drawPtr;
            double      x=0.0, y=0.0, angle=0.0;
            char        *txt="";

            if( (objc < 4) || (objc > 7) || (objc == 5) ) {
                Tcl_WrongNumArgs(interp, 2, objv, "draw ?x=0.0 y=0.0? ?angle=0.0? txt");
                return TCL_ERROR;
            }
            name = Tcl_GetString(objv[2]);
            if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
            if( objc >= 6 ) {	/* we have parameters x y */
                if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &x)) != TCL_OK ) {
                    return stat;
		}
                if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &y)) != TCL_OK ) {
                    return stat;
		}
            }
            if( objc == 7 ) {	/* we have parameter angle */
                if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &angle)) != TCL_OK ) {
                    return stat;
		}
            }
	    txt = Tcl_GetString(objv[objc-1]);

            result = MagickAnnotateImage(wandPtr, drawPtr, x, y, angle, txt);
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
            break;
        }

    case TM_APPEND:         /* append ?stack=0? ?newName? */
    case TM_APPEND_IMAGES:  /* AppendImages ?stack=0? ?newName? */
    {
	int stack=0;
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?stack=0? ?newName?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetBooleanFromObj(interp, objv[2], &stack)) != TCL_OK) ) {
	    return stat;
	}
	if( objc > 3 ) {
	    name = Tcl_GetString(objv[3]);
	}
	newWand = MagickAppendImages(wandPtr, stack);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_AVERAGE:        /* average ?newName? */
    case TM_AVERAGE_IMAGES: /* AverageImages ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickAverageImages(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }


    case TM_BLACK_THRESHOLD:        /* blackthreshold thresholdPixel */
    case TM_BLACK_THRESHOLD_IMAGE:  /* BlackThresholdImage thresholdPixel */
    {
	char	    *name;
	PixelWand   *threshPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "thresholdPixel");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (threshPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}

	result = MagickBlackThresholdImage(wandPtr, threshPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_BLUR:        /* blur ?radius? ?sigma? */
    case TM_BLUR_IMAGE:  /* BlurImage ?radius? ?sigma? */
    {
	double  radius=0.0, sigma=1.0;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=1.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickBlurImage(wandPtr, radius, sigma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_BORDER:        /* border borderPixel ?width? ?height? */
    case TM_BORDER_IMAGE:  /* BorderImage borderPixel ?width? ?height? */
    {
	unsigned long width=1, height=1;
	char        *name;
	PixelWand   *borderPtr;

	if( (objc < 3) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "borderPixel ?width=1? ?height=width?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (borderPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], (long int *) &height)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickBorderImage(wandPtr, borderPtr, width, height);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CHARCOAL:        /* charcoal ?radius? ?sigma? */
    case TM_CHARCOAL_IMAGE:  /* CharcoalImage ?radius? ?sigma? */
    {
	double  radius=0.0, sigma=1.0;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=1.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickCharcoalImage(wandPtr, radius, sigma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CHOP:        /* chop width height ?x y? */
    case TM_CHOP_IMAGE:  /* ChopImage width height ?x y? */
    {
	unsigned long width, height;
	long x=0, y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetLongFromObj(interp, objv[5], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickChopImage(wandPtr, width, height, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CLIP:        /* clip */
    case TM_CLIP_IMAGE:  /* ClipImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickClipImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }


    case TM_CLIP_PATH:          /* clip pathname ?inside=0? */
    case TM_CLIP_PATH_IMAGE:    /* ClipImage pathname ?inside=0? */
    {
	int inside=0;
	char *pathname=NULL;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "pathname ?inside=0?");
	    return TCL_ERROR;
	}
        pathname = Tcl_GetString(objv[2]);
	if( (objc > 3) && ((stat = Tcl_GetBooleanFromObj(interp, objv[3], &inside)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickClipPathImage(wandPtr, pathname, inside );
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CLONE:       /* clone ?newName? */
    case TM_CLONE_WAND:  /* CloneWand ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = CloneMagickWand(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_COALESCE:        /* coalesce ?newName? */
    case TM_COALESCE_IMAGES: /* CoalesceImages ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickCoalesceImages(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_COLOR_FLOODFILL:        /* colorfloodfill fillPixel ?fuzz? ?borderPix? ?x y? */
    case TM_COLOR_FLOODFILL_IMAGE:  /* ColorFloodfillImage ?fuzz? ?borderPix? ?x y? */
				    /* - empty borderPix is allowed */
    {
	int	x=0, y=0;
	double  fuzz=0.0;
	char	*name;
	PixelWand *fillPtr, *borderPtr = NULL;

	if( (objc < 3) || (objc > 7) || (objc == 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "fillPixel ?fuzz=0.0? ?borderPix=fillPix? ?x=0 y=0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (fillPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	if( objc > 4 ) {
	    name = Tcl_GetString(objv[4]);
	    if( noWandObj(name) ) {
		borderPtr = NULL;
	    } else if( (borderPtr = findPixelWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 6) && ((stat = Tcl_GetIntFromObj(interp, objv[6], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickColorFloodfillImage(wandPtr, fillPtr, fuzz, borderPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_COLORIZE:       /* colorize fillPixel opacityPixel */
    case TM_COLORIZE_IMAGE: /* ColorizeImage fillPixel opacityPixel */
    {
	char	*name;
	PixelWand	*fillPtr, *opacityPtr;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "fillPixel opacityPixel");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (fillPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[3]);
	if( (opacityPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}

	result = MagickColorizeImage(wandPtr, fillPtr, opacityPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_COMMENT:        /* comment str */
    case TM_COMMENT_IMAGE:  /* CommentImage str */
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "str");
	    return TCL_ERROR;
	}
	result = MagickCommentImage(wandPtr, Tcl_GetString(objv[2]));
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_COMPARE_CHANNELS:        /* comparechannels refName chan metric */
    case TM_COMPARE_IMAGE_CHANNELS:  /* CompareImageChannels refName chan metric */
    {
	MagickWand      *refWand, *newWand;
        int             metricIdx, chanIdx;
	char            *name;
	double          value;

	if( objc != 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "refName");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (refWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[3], chanNames,
				"channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[4], metricNames,
				"metricType", 0, &metricIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	newWand = MagickCompareImageChannels( wandPtr, refWand, chanTypes[chanIdx],
					      metricTypes[metricIdx], &value );
	if (newWand != NULL) {
            DestroyMagickWand(newWand);
	} else {
	    myMagickError(interp, wandPtr);
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(value));

	break;
    }

    case TM_COMPARE:         /* compare refName metric */
    case TM_COMPARE_IMAGES:  /* CompareImages refName metric */
    {
	MagickWand      *refWand, *newWand;
        int             metricIdx;
	char            *name;
	double          value;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "refName metric");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (refWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[3], metricNames,
				"metricType", 0, &metricIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	newWand = MagickCompareImages( wandPtr, refWand, metricTypes[metricIdx], &value );
	if (newWand != NULL) {
            DestroyMagickWand(newWand);
	} else {
	    myMagickError(interp, wandPtr);
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, Tcl_NewDoubleObj(value));

	break;
    }

    case TM_COMPOSITE:          /* composite compWand opType ?x y? */
    case TM_COMPOSITE_IMAGE:    /* CompositeImage compWand opType ?x y? */
    {
	int opIdx;

	char	    *name;
	MagickWand  *compPtr;
	int x=0;
	int y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "opType compWand ?x=0 y=0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (compPtr = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[3], opNames, "opType", 0, &opIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickCompositeImage(wandPtr, compPtr, opTypes[opIdx], x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_CONTRAST:        /* contrast ?sharpen? */
    case TM_CONTRAST_IMAGE:  /* ContrastImage ?sharpen? */
    {
	int sharpen=1;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?sharpen=yes?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetBooleanFromObj(interp, objv[2], &sharpen)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickContrastImage(wandPtr, sharpen);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CONVOLVE:       /* convolve order kernelList */
    case TM_CONVOLVE_IMAGE: /* ConvolveImage order kernelList */
    {
	int     i, listLen;
	unsigned long order;
	double  *kernel;
	Tcl_Obj **listPtr;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "order kernelList");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &order)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_ListObjGetElements(interp, objv[3], &listLen, &listPtr)) != TCL_OK) {
	    return stat;
	}
	if( (unsigned long)listLen != order * order ) {
	    Tcl_AppendResult(
		interp,
		"ConvolveImage: Invalid kernelList length, should be = (order x order)", NULL);
	    return TCL_ERROR;
	}
	kernel = (double *)ckalloc(listLen * sizeof(double));
	if( kernel == NULL ) {
	    Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
	    return TCL_ERROR;
	}
	for( i=0; i < listLen; i++ ) {
	    if( (stat = Tcl_GetDoubleFromObj(interp, listPtr[i], &kernel[i])) != TCL_OK) {
		ckfree((char *)kernel);
		return stat;
	    }
	}

	result = MagickConvolveImage(wandPtr, order, kernel);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CROP:        /* crop width height ?x y? */
    case TM_CROP_IMAGE:  /* CropImage width height ?x y? */
    {
	unsigned long width, height;
	long x=0, y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetLongFromObj(interp, objv[5], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickCropImage(wandPtr, width, height, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CYCLE_COLORMAP:         /* cyclecolormap ?displace? */
    case TM_CYCLE_COLORMAP_IMAGE:   /* CycleColormapImage ?displace? */
    {
	int displace=1;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?displace=1?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetIntFromObj(interp, objv[2], &displace)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickCycleColormapImage(wandPtr, displace);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_DECONSTRUCT:        /* deconstruct ?newName? */
    case TM_DECONSTRUCT_IMAGES: /* DeconstructImage ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickDeconstructImages(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_DESCRIBE:       /* describe */
    case TM_DESCRIBE_IMAGE: /* DescribeImage */
    {
	char *txt;

	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	txt = MagickDescribeImage(wandPtr);
	if(txt != NULL) {
	    Tcl_SetResult(interp, txt, TCL_VOLATILE);
	    MagickRelinquishMemory(txt); /* Free TclMagick resource */
	}
	break;
    }

    case TM_DESPECKLE:          /* despeckle */
    case TM_DESPECKLE_IMAGE:    /* DespeckleImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickDespeckleImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_DRAW:       /* draw draw */
    case TM_DRAW_IMAGE: /* DrawImage draw */
    {
	char	*name;
	DrawingWand	*drawPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "draw");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	result = MagickDrawImage(wandPtr, drawPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_EDGE:       /* edge ?radius? */
    case TM_EDGE_IMAGE: /* EdgeImage ?radius? */
    {
	double  radius=0.0;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickEdgeImage(wandPtr, radius);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_EMBOSS:        /* emboss ?radius? ?sigma? */
    case TM_EMBOSS_IMAGE:  /* EmbossImage ?radius? ?sigma? */
    {
	double  radius=0.0, sigma=1.0;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=1.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickEmbossImage(wandPtr, radius, sigma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ENHANCE:          /* enhance */
    case TM_ENHANCE_IMAGE:    /* EnhanceImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickEnhanceImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_EQUALIZE:          /* equalize */
    case TM_EQUALIZE_IMAGE:    /* EqualizeImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickEqualizeImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_FLATTEN:          /* flatten ?newName? */
    case TM_FLATTEN_IMAGES:   /* FlattenImages ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickFlattenImages(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_FLIP:          /* flip */
    case TM_FLIP_IMAGE:    /* FlipImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickFlipImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }


    case TM_FLOP:          /* flop */
    case TM_FLOP_IMAGE:    /* FlopImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickFlopImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_FRAME:          /* frame framePixel ?width? ?height? ?inner? ?outer? */
    case TM_FRAME_IMAGE:    /* FrameImage framePixel ?width? ?height? ?inner? ?outer? */
    {
	unsigned long width=1, height=1;
	long inner=0, outer=0;
	char        *name;
	PixelWand	*framePtr;

	if( (objc < 3) || (objc > 7) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "framePixel ?width=1? ?height=width? ?inner=0? ?outer=inner?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (framePtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4],  (long int *) &height)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetLongFromObj(interp, objv[5], &inner)) != TCL_OK) ) {
	    return stat;
	}
	outer = inner; /* default */
	if( (objc > 6) && ((stat = Tcl_GetLongFromObj(interp, objv[6], &outer)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickFrameImage(wandPtr, framePtr, width, height, inner, outer);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_FX:         /* fx expr ?newName? */
    case TM_FX_IMAGE:   /* fxImage expr ?newName? */
    {
        char	   *expr;
	MagickWand *newWand;
	char       *name=NULL;

	if( (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "expr ?newName?");
	    return TCL_ERROR;
	}
	expr = Tcl_GetString(objv[2]);

	if( objc > 3 ) {
	    name = Tcl_GetString(objv[3]);
	}
	newWand = MagickFxImage(wandPtr, expr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_FX_CHANNEL:         /* fxchannel channelType expr ?newName? */
    case TM_FX_IMAGE_CHANNEL:   /* fxImageChannel channelType expr ?newName? */
    {
	int        chanIdx;
        char       *expr;
	MagickWand *newWand;
	char       *name=NULL;

	if( (objc != 4) && (objc != 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType expr ?newName?");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	expr = Tcl_GetString(objv[3]);

	if( objc > 4 ) {
	    name = Tcl_GetString(objv[4]);
	}
	newWand = MagickFxImageChannel(wandPtr, chanTypes[chanIdx], expr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_GAMMAIMAGE:  /* gammaimage level */
    case TM_GAMMA_IMAGE: /* GammaImage level */
    {
	double gamma;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "level");
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[2], &gamma) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickGammaImage(wandPtr, gamma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_GAMMACHANNEL:        /* gamma channelType level */
    case TM_GAMMA_IMAGE_CHANNEL: /* GammaImage channelType level */
    {
	int    chanIdx;
	double gamma;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType level");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (Tcl_GetDoubleFromObj(interp, objv[3], &gamma) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickGammaImageChannel(wandPtr, chanTypes[chanIdx], gamma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_FILENAME:        /* filename ?filename? */
    case TM_GET_FILENAME:    /* GetFilename */
    case TM_SET_FILENAME:    /* SetFilename filename */
    {
	char *filename;

	if( ((enum subIndex)index == TM_FILENAME) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?filename?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_FILENAME) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_FILENAME) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	if (objc > 2) { /* Set filename */
	    Tcl_DString extrep;

	    filename = Tcl_UtfToExternalDString (NULL, Tcl_GetString(objv[2]), -1, &extrep);
	    MagickSetFilename(wandPtr, filename);

	    Tcl_DStringFree (&extrep);
	} else {    /* Get filename */
	    filename = (char *)MagickGetFilename(wandPtr);
	    if(filename != NULL) {
		SetResultAsExternalString(interp, filename);
		MagickRelinquishMemory(filename); /* Free TclMagick resource */
	    }
	}
	break;
    }

    case TM_BACKGROUND_COLOR:      /* backgroundcolor ?pixel? */
    case TM_GET_BACKGROUND_COLOR:  /* GetBackgroundColor ?pixel? */
    case TM_SET_BACKGROUND_COLOR:  /* SetBackgroundColor pixel */
    {
	char *name = NULL;
        PixelWand *pixPtr = NULL;

	if( ((enum subIndex)index == TM_SET_BACKGROUND_COLOR) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "pixel");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index != TM_SET_BACKGROUND_COLOR) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pixel?");
	    return TCL_ERROR;
	}
	if (objc > 2) { /* Set color / Get color with new name */
	    name = Tcl_GetString(objv[2]);
	    pixPtr = findPixelWand(interp, name);
        }
        /*
         * SET color requires existing pixel object
         */
        if( ((enum subIndex)index == TM_SET_BACKGROUND_COLOR) && (pixPtr == NULL) ) {
	        return TCL_ERROR;
        }
        /*
         * GET color if GET_COLOR or pixel object doesn't exists
         * otherwise SET color
         */
        if( ((enum subIndex)index == TM_GET_BACKGROUND_COLOR) || (pixPtr == NULL) ) {
            if( pixPtr == NULL ) {
                pixPtr = NewPixelWand();
                name = newPixelObj(interp, pixPtr, name );
            }
            result = MagickGetImageBackgroundColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
	    }
            Tcl_SetResult(interp, name, TCL_VOLATILE);
        } else {
            result = MagickSetImageBackgroundColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
        }
	break;
    }

    case TM_BLUE_PRIMARY:      /* blueprimary ?x y? */
    case TM_GET_BLUE_PRIMARY:  /* GetBluePrimary */
    case TM_SET_BLUE_PRIMARY:  /* SetBluePrimary x y */
    {
        double x, y;

	if( ((enum subIndex)index == TM_BLUE_PRIMARY) && (objc != 2) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?x y?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_BLUE_PRIMARY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_BLUE_PRIMARY) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set primary = x y
	     */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetImageBluePrimary(wandPtr, x, y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get primary={x y}
	     */
	    Tcl_Obj *listPtr;

	    result = MagickGetImageBluePrimary(wandPtr, &x, &y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	    listPtr = Tcl_NewListObj(0, NULL);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_BORDER_COLOR:      /* bordercolor ?pixel? */
    case TM_GET_BORDER_COLOR:  /* GetBorderColor ?pixel? */
    case TM_SET_BORDER_COLOR:  /* SetBorderColor pixel */
    {
	char *name = NULL;
        PixelWand *pixPtr = NULL;

	if( ((enum subIndex)index == TM_SET_BORDER_COLOR) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "pixel");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index != TM_SET_BORDER_COLOR) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pixel?");
	    return TCL_ERROR;
	}
	if (objc > 2) { /* Set color / Get color with new name */
	    name = Tcl_GetString(objv[2]);
	    pixPtr = findPixelWand(interp, name);
        }
        /*
         * SET color requires existing pixel object
         */
        if( ((enum subIndex)index == TM_SET_BORDER_COLOR) && (pixPtr == NULL) ) {
	        return TCL_ERROR;
        }
        /*
         * GET color if GET_COLOR or pixel object doesn't exists
         * otherwise SET color
         */
        if( ((enum subIndex)index == TM_GET_BORDER_COLOR) || (pixPtr == NULL) ) {
            if( pixPtr == NULL ) {
                pixPtr = NewPixelWand();
                name = newPixelObj(interp, pixPtr, name );
            }
            result = MagickGetImageBorderColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
	    }
            Tcl_SetResult(interp, name, TCL_VOLATILE);
        } else {
            result = MagickSetImageBorderColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
        }
	break;
    }

    case TM_CHANNEL_DEPTH:      /* channeldepth channel ?depth? */
    case TM_GET_CHANNEL_DEPTH:  /* GetChannelDepth channel */
    case TM_SET_CHANNEL_DEPTH:  /* SetChannelDepth channel depth */
    {
        unsigned long depth;
        int chanIdx;

	if( ((enum subIndex)index == TM_CHANNEL_DEPTH) && (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channel ?depth?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_CHANNEL_DEPTH) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channel");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_CHANNEL_DEPTH) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channel depth");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set channel depth
	     */
	    if( (stat = Tcl_GetLongFromObj(interp, objv[3],  (long int *) &depth)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetImageChannelDepth(wandPtr, chanTypes[chanIdx], depth);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get channel depth
	     */
	    depth = MagickGetImageChannelDepth(wandPtr, chanTypes[chanIdx]);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)depth));
	}
	break;
    }

    case TM_CHANNEL_EXTREMA:      /* channelextrema channelType */
    case TM_GET_CHANNEL_EXTREMA:  /* GetChannelExtrema channelType */
    {
        unsigned long min, max;
        int chanIdx;
        Tcl_Obj *listPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType" );
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickGetImageChannelExtrema(wandPtr, chanTypes[chanIdx], &min, &max);
        if (!result) {
	    return myMagickError(interp, wandPtr);
	}
        listPtr = Tcl_NewListObj(0, NULL);

        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(min));
        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(max));
        Tcl_SetObjResult(interp, listPtr);

        break;
    }

    case TM_CHANNEL_MEAN:      /* channelmean channelType */
    case TM_GET_CHANNEL_MEAN:  /* GetChannelMean channelType */
    {
        double mean, deviation;
        int chanIdx;
        Tcl_Obj *listPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType" );
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickGetImageChannelMean(wandPtr, chanTypes[chanIdx], &mean, &deviation);
        if (!result) {
	    return myMagickError(interp, wandPtr);
	}
        listPtr = Tcl_NewListObj(0, NULL);

        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(mean));
        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(deviation));
        Tcl_SetObjResult(interp, listPtr);

        break;
    }

    case TM_COLORMAP_COLOR:      /* colormapcolor index ?pixel? */
    case TM_GET_COLORMAP_COLOR:  /* GetColormapColor index ?pixel? */
    case TM_SET_COLORMAP_COLOR:  /* SetColormapColor index pixel */
    {
	char *name = NULL;
        unsigned long idx;
        PixelWand *pixPtr = NULL;

	if( ((enum subIndex)index == TM_SET_COLORMAP_COLOR) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "index pixel");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index != TM_SET_COLORMAP_COLOR) && ((objc < 3) || (objc > 4)) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "index ?pixel?");
	    return TCL_ERROR;
	}
	if (Tcl_GetLongFromObj(interp, objv[2], (long int *) &idx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (objc > 3) { /* Set color / Get color with new name */
	    name = Tcl_GetString(objv[3]);
	    pixPtr = findPixelWand(interp, name);
        }
        /*
         * SET color requires existing pixel object
         */
        if( ((enum subIndex)index == TM_SET_COLORMAP_COLOR) && (pixPtr == NULL) ) {
	        return TCL_ERROR;
        }
        /*
         * GET color if GET_COLOR or pixel object doesn't exists
         * otherwise SET color
         */
        if( ((enum subIndex)index == TM_GET_COLORMAP_COLOR) || (pixPtr == NULL) ) {
            if( pixPtr == NULL ) {
                pixPtr = NewPixelWand();
                name = newPixelObj(interp, pixPtr, name );
            }
            result = MagickGetImageColormapColor( wandPtr, idx, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
	    }
            Tcl_SetResult(interp, name, TCL_VOLATILE);
        } else {
            result = MagickSetImageColormapColor( wandPtr, idx, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
        }
	break;
    }

    case TM_COLORS:      /* colors */
    case TM_GET_COLORS:  /* GetColors */
    {
        long colors;

	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	colors = MagickGetImageColors(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewLongObj(colors));
	break;
    }

    case TM_COLORSPACE:      /* colorspace ?colorspaceType? */
    case TM_GET_COLORSPACE:  /* GetColorspace */
    case TM_SET_COLORSPACE:  /* SetColorspace colorspaceType */
    {
        int csIdx;
        ColorspaceType cs;

	if( ((enum subIndex)index == TM_COLORSPACE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?colorspaceType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_COLORSPACE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_COLORSPACE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "colorspaceType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set colorspace
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], csNames, "csType", 0, &csIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageColorspace(wandPtr, csTypes[csIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get colorspace
	     */
	    cs = MagickGetImageColorspace(wandPtr);
	    for (csIdx = 0; (size_t) csIdx < sizeof(csTypes)/sizeof(csTypes[0]); csIdx++) {
		if( csTypes[csIdx] == cs ) {
		    Tcl_SetResult(interp, (char *)csNames[csIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)cs));
	}
	break;
    }

    case TM_COMPOSE:      /* compose ?opType? */
    case TM_GET_COMPOSE:  /* GetCompose */
    case TM_SET_COMPOSE:  /* SetCompose opType */
    {
        int opIdx;
        CompositeOperator op;

	if( ((enum subIndex)index == TM_COMPOSE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?opType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_COMPOSE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_COMPOSE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "opType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set compose operator
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], opNames, "opType", 0, &opIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageColorspace(wandPtr, opTypes[opIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get compose operator
	     */
	    op = MagickGetImageColorspace(wandPtr);
	    for (opIdx = 0; (size_t) opIdx < sizeof(opTypes)/sizeof(opTypes[0]); opIdx++) {
		if( opTypes[opIdx] == op ) {
		    Tcl_SetResult(interp, (char *)opNames[opIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)op));
	}
	break;
    }

    case TM_COMPRESSION:      /* compression ?compressionType? */
    case TM_GET_COMPRESSION:  /* GetCompression */
    case TM_SET_COMPRESSION:  /* SetCompression compressionType */
    {
        int csIdx;
        CompressionType cs;

	if( ((enum subIndex)index == TM_COMPRESSION) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?compressionType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_COMPRESSION) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_COMPRESSION) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "compressionType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set compression
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], compressNames, "compressType", 0, &csIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageCompression(wandPtr, compressTypes[csIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get compression
	     */
	    cs = MagickGetImageCompression(wandPtr);
	    for (csIdx = 0; (size_t) csIdx < sizeof(compressTypes)/sizeof(compressTypes[0]); csIdx++) {
		if( compressTypes[csIdx] == cs ) {
		    Tcl_SetResult(interp, (char *)compressNames[csIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)cs));
	}
	break;
    }

    case TM_DELAY:      /* delay ?delay? */
    case TM_GET_DELAY:  /* GetDelay */
    case TM_SET_DELAY:  /* SetDelay delay */
    {
        unsigned long delay;

	if( ((enum subIndex)index == TM_DELAY) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?delay?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_DELAY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_DELAY) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "delay");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set delay
	     */
		if (Tcl_GetLongFromObj(interp, objv[2], (long int *) &delay) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageDelay(wandPtr, delay);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get delay
	     */
	    delay = MagickGetImageDelay(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)delay));
	}
	break;
    }

    case TM_DEPTH:      /* depth ?depth? */
    case TM_GET_DEPTH:  /* GetDepth */
    case TM_SET_DEPTH:  /* SetDepth depth */
    {
        unsigned long depth;

	if( ((enum subIndex)index == TM_DEPTH) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?depth?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_DEPTH) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_DEPTH) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "depth");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set depth
	     */
	    if (Tcl_GetLongFromObj(interp, objv[2], (long int *) &depth) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageDepth(wandPtr, depth);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get depth
	     */
	    depth = MagickGetImageDepth(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)depth));
	}
	break;
    }

    case TM_DISPOSE:      /* dispose ?disposeType? */
    case TM_GET_DISPOSE:  /* GetDispose */
    case TM_SET_DISPOSE:  /* SetDispose disposeType */
    {
        int disposeIdx;
        DisposeType dispose;

	if( ((enum subIndex)index == TM_DISPOSE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?disposeType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_DISPOSE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_DISPOSE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "disposeType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set dispose
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], disposeNames, "disposeType", 0, &disposeIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageDispose(wandPtr, disposeTypes[disposeIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get dispose
	     */
	    dispose = MagickGetImageDispose(wandPtr);
	    for (disposeIdx = 0; (size_t) disposeIdx < sizeof(disposeTypes)/sizeof(disposeTypes[0]); disposeIdx++) {
		if( disposeTypes[disposeIdx] == dispose ) {
		    Tcl_SetResult(interp, (char *)disposeNames[disposeIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)dispose));
	}
	break;
    }

    case TM_EXTREMA:      /* extrema */
    case TM_GET_EXTREMA:  /* GetExtrema */
    {
        unsigned long min, max;
        Tcl_Obj *listPtr;

	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL );
	    return TCL_ERROR;
	}
	result = MagickGetImageExtrema(wandPtr, &min, &max);
        if (!result) {
	    return myMagickError(interp, wandPtr);
	}
        listPtr = Tcl_NewListObj(0, NULL);

        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(min));
        Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(max));
        Tcl_SetObjResult(interp, listPtr);

        break;
    }

    case TM_FORMAT:     /* format */
    case TM_GET_FORMAT: /* GetImageFormat */
    case TM_SET_FORMAT: /* SetImageFormat */
    {
	char *fmt;

        if( ((enum subIndex)index == TM_FORMAT) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?format?");
	    return TCL_ERROR;
	}
        if( ((enum subIndex)index == TM_GET_FORMAT) && (objc != 2) ) {
          Tcl_WrongNumArgs(interp, 2, objv, NULL);
          return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_FORMAT) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "format");
	    return TCL_ERROR;
	}
        if (objc > 2) { /* Set format */
#if defined(HAVE_MAGICKSETIMAGEFORMAT)
	    Tcl_DString extrep;

            fmt = Tcl_UtfToExternalDString (NULL, Tcl_GetString(objv[2]), -1, &extrep);
	    MagickSetImageFormat(wandPtr, fmt);
	    Tcl_DStringFree (&extrep);
#else
            char msg[1024];
            sprintf(msg, "%.500s: MagickSetImageFormat() is not implemented!", MagickGetPackageName());
            Tcl_AppendResult(interp, msg, NULL);
	    return TCL_ERROR;
#endif /* HAVE_MAGICKSETIMAGEFORMAT */
        } else { /* Get format */
          fmt = (char *)MagickGetImageFormat(wandPtr);
          if (fmt != NULL) {
	    Tcl_SetResult(interp, fmt, TCL_VOLATILE);
	    MagickRelinquishMemory(fmt); /* Free TclMagick resource */
          }
	}
	break;
    }

    case TM_GETIMAGE:   /* getimage ?newName? */
    case TM_GET_IMAGE:  /* GetImage ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickGetImage(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_IMAGE_FILENAME:        /* imagefilename ?filename? */
    case TM_GET_IMAGE_FILENAME:    /* GetImageFilename */
    case TM_SET_IMAGE_FILENAME:    /* SetImageFilename filename */
    {
	char *filename;

	if( ((enum subIndex)index == TM_IMAGE_FILENAME) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?filename?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_IMAGE_FILENAME) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_IMAGE_FILENAME) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	if (objc > 2) { /* Set filename */
	    Tcl_DString extrep;
	    filename = Tcl_UtfToExternalDString(NULL, Tcl_GetString(objv[2]), -1, &extrep);
	    result = MagickSetImageFilename(wandPtr, filename);
	    Tcl_DStringFree(&extrep);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {    /* Get filename */
	    filename = (char *)MagickGetImageFilename(wandPtr);
	    if(filename != NULL) {
		SetResultAsExternalString(interp, filename);
		MagickRelinquishMemory(filename); /* Free TclMagick resource */
	    }
	}
	break;
    }

    case TM_GAMMA:      /* gamma ?gamma? */
    case TM_GET_GAMMA:  /* GetGamma */
    case TM_SET_GAMMA:  /* SetGamma gamma */
    {
        double gamma;

	if( ((enum subIndex)index == TM_GAMMA) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?gamma?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_GAMMA) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_GAMMA) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "gamma");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set gamma
	     */
	    if (Tcl_GetDoubleFromObj(interp, objv[2], &gamma) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageGamma(wandPtr, gamma);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get gamma
	     */
	    gamma = MagickGetImageGamma(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(gamma));
	}
	break;
    }

    case TM_GREEN_PRIMARY:      /* greenprimary ?x y? */
    case TM_GET_GREEN_PRIMARY:  /* GetGreenPrimary */
    case TM_SET_GREEN_PRIMARY:  /* SetGreenPrimary x y */
    {
        double x, y;

	if( ((enum subIndex)index == TM_GREEN_PRIMARY) && (objc != 2) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?x y?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_GREEN_PRIMARY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_GREEN_PRIMARY) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set primary = x y
	     */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetImageGreenPrimary(wandPtr, x, y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get primary={x y}
	     */
	    Tcl_Obj *listPtr;

	    result = MagickGetImageGreenPrimary(wandPtr, &x, &y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	    listPtr = Tcl_NewListObj(0, NULL);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_HEIGHT:     /* height */
    case TM_GET_HEIGHT: /* GetHeight */
    {
	int value;

        if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	value = MagickGetImageHeight(wandPtr);
        Tcl_SetObjResult(interp, Tcl_NewLongObj(value));

	break;
    }

    case TM_INDEX:      /* index ?index? */
    case TM_GET_INDEX:  /* GetIndex */
    case TM_SET_INDEX:  /* SetIndex index */
    {
	long idx;

	if( ((enum subIndex)index == TM_INDEX) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?index?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_INDEX) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_INDEX) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "index");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set index
	     */
	    if (Tcl_GetLongFromObj(interp, objv[2], &idx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageIndex(wandPtr, (long) idx);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get depth
	     */
	    idx = MagickGetImageIndex(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)idx));
	}
	break;
    }

    case TM_INTERLACE:      /* interlace ?interlaceType? */
    case TM_GET_INTERLACE:  /* GetInterlace */
    case TM_SET_INTERLACE:  /* SetInterlace interlaceType */
    {
        int interlaceIdx;
        InterlaceType interlace;

	if( ((enum subIndex)index == TM_INTERLACE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?interlaceType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_INTERLACE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_INTERLACE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "interlaceType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set interlace
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], interlaceNames,
				    "interlaceType", 0, &interlaceIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageInterlaceScheme(wandPtr, interlaceTypes[interlaceIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get interlace
	     */
	    interlace = MagickGetImageInterlaceScheme(wandPtr);
	    for (interlaceIdx = 0;
		 (size_t) interlaceIdx < sizeof(interlaceTypes)/sizeof(interlaceTypes[0]);
		 interlaceIdx++) {
		if( interlaceTypes[interlaceIdx] == interlace ) {
		    Tcl_SetResult(interp, (char *)interlaceNames[interlaceIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)interlace));
	}
	break;
    }

    case TM_ITERATIONS:      /* iterations ?num? */
    case TM_GET_ITERATIONS:  /* GetIterations */
    case TM_SET_ITERATIONS:  /* SetIterations num */
    {
        unsigned long num;

	if( (index == TM_ITERATIONS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?num?");
	    return TCL_ERROR;
	}
	if( (index == TM_GET_ITERATIONS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( (index == TM_SET_ITERATIONS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "num");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set iterations
	     */
	    if (Tcl_GetLongFromObj(interp, objv[2], (long int *) &num) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageIterations(wandPtr, num);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get depth
	     */
	    num = MagickGetImageIterations(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)num));
	}
	break;
    }

    case TM_MATTE_COLOR:      /* mattecolor ?pixel? */
    case TM_GET_MATTE_COLOR:  /* GetMatteColor ?pixel? */
    case TM_SET_MATTE_COLOR:  /* SetMatteColor pixel */
    {
	char *name = NULL;
        PixelWand *pixPtr = NULL;

	if( ((enum subIndex)index == TM_SET_MATTE_COLOR) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "pixel");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index != TM_SET_MATTE_COLOR) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pixel?");
	    return TCL_ERROR;
	}
	if (objc > 2) { /* Set color / Get color with new name */
	    name = Tcl_GetString(objv[2]);
	    pixPtr = findPixelWand(interp, name);
        }
        /*
         * SET color requires existing pixel object
         */
        if( ((enum subIndex)index == TM_SET_MATTE_COLOR) && (pixPtr == NULL) ) {
	        return TCL_ERROR;
        }
        /*
         * GET color if GET_COLOR or pixel object doesn't exists
         * otherwise SET color
         */
        if( ((enum subIndex)index == TM_GET_MATTE_COLOR) || (pixPtr == NULL) ) {
            if( pixPtr == NULL ) {
                pixPtr = NewPixelWand();
                name = newPixelObj(interp, pixPtr, name );
            }
            result = MagickGetImageMatteColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
	    }
            Tcl_SetResult(interp, name, TCL_VOLATILE);
        } else {
            result = MagickSetImageMatteColor( wandPtr, pixPtr );
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
        }
	break;
    }

    case TM_PIXELS:       /* pixels x0 y0 cols rows map storageType ?data? */
    case TM_GET_PIXELS:   /* GetPixels x0 y0 cols rows map storageType */
    case TM_SET_PIXELS:   /* GetPixels x0 y0 cols rows map storageType data */
    {
	static CONST char *storNames[] = {
	    "char", "short", "integer", "long", "float", "double",
	    (char *) NULL
	};
	static StorageType storTypes[] = {
	    CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel, DoublePixel
	};
	static size_t storSize[] = {
	    sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(float), sizeof(double)
	};
	unsigned long cols, rows, size;
	long x0, y0;
	int  storIdx, len;
	char *map;
	unsigned char *pixels;

	if( ((enum subIndex)index == TM_PIXELS) && ((objc < 8) || (objc > 9)) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType ?data?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_PIXELS) && (objc != 8) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_PIXELS) && (objc != 9) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType data");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], &x0)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], &y0)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[4], (long int *) &cols)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[5], (long int *) &rows)) != TCL_OK ) {
	    return stat;
	}
	map = Tcl_GetString(objv[6]);
	if (Tcl_GetIndexFromObj(interp, objv[7], storNames, "storageType", 0, &storIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	size = cols * rows * storSize[storIdx] * strlen(map);

	if( objc == 8 ) {
            /*
             * GetImagePixels: Allocate memory for storage
             */
		pixels = (unsigned char *) ckalloc(size);
		if( pixels == NULL ) {
			Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
			return TCL_ERROR;
		}

	    result = MagickGetImagePixels(wandPtr, x0, y0, cols, rows, map,
					  storTypes[storIdx], pixels);
	    if (!result) {
		    ckfree((char *) pixels);
		return myMagickError(interp, wandPtr);
	    }

	    /*
	     * Return ByteArray object
	     */
	    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(pixels, (int)size));
	    ckfree((char *) pixels);
	} else {
	    pixels = Tcl_GetByteArrayFromObj( objv[8], &len);
	    if( (unsigned long)len < size ) {
		Tcl_AppendResult(interp, "TclMagick: not enough data bytes", NULL);
		return TCL_ERROR;
	    }
	    result = MagickSetImagePixels(wandPtr, x0, y0, cols, rows, map,
					  storTypes[storIdx], pixels);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	}
	break;
    }

    case TM_PROFILE_IMAGE:  /* ProfileImage name ?profile? */
    {
	char *name;
	unsigned char *profile = NULL;
        int length = 0;

        if( (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?profile?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);

        if( objc > 3 ) {
	    profile = Tcl_GetByteArrayFromObj(objv[3], &length);
        }
        result = MagickProfileImage(wandPtr, name, profile, (unsigned long)length);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
        break;
    }

    case TM_REMOVE_PROFILE: /* RemoveProfile name */
    {
	char *name;
	unsigned char *profile;
        unsigned long length;

        if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);

        profile = MagickRemoveImageProfile(wandPtr, (const char*)name, &length);
        if(profile != NULL) {
	    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(profile, (long)length));
	    MagickRelinquishMemory(profile); /* Free TclMagick resource */
        }
        break;
    }

    case TM_PROFILE:        /* profile name ?profile? */
    case TM_GET_PROFILE:    /* GetProfile name */
    case TM_SET_PROFILE:    /* SetProfile name profile */
    {
	char *name;
	unsigned char *profile;

	if( ((enum subIndex)index == TM_PROFILE) && (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?profile?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_PROFILE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_PROFILE) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name profile");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);

        if( objc == 4 ) {
            int length;
            /*
             * Set/Add image profile
             */
	    profile = Tcl_GetByteArrayFromObj(objv[3], &length);
	    result = MagickSetImageProfile(wandPtr, (const char*)name, profile, (unsigned long)length);
	    if (!result) {
	        return myMagickError(interp, wandPtr);
	    }
        } else {
            unsigned long length;

            profile = MagickGetImageProfile(wandPtr, name, &length);
	    if(profile != NULL) {
	        Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(profile, (long)length));
	        MagickRelinquishMemory(profile); /* Free TclMagick resource */
            }
	}
	break;
    }

    case TM_RED_PRIMARY:      /* redprimary ?x y? */
    case TM_GET_RED_PRIMARY:  /* GetRedPrimary */
    case TM_SET_RED_PRIMARY:  /* SetRedPrimary x y */
    {
        double x, y;

	if( ((enum subIndex)index == TM_RED_PRIMARY) && (objc != 2) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?x y?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_RED_PRIMARY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_RED_PRIMARY) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set primary = x y
	     */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetImageRedPrimary(wandPtr, x, y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get primary={x y}
	     */
	    Tcl_Obj *listPtr;

	    result = MagickGetImageRedPrimary(wandPtr, &x, &y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	    listPtr = Tcl_NewListObj(0, NULL);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_RENDERING:      /* rendering ?renderType? */
    case TM_GET_RENDERING:  /* GetRenderingIntent */
    case TM_SET_RENDERING:  /* SetRenderingIntent renderType */
    {
        int renderIdx;
        RenderingIntent render;

	if( ((enum subIndex)index == TM_RENDERING) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?renderType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_RENDERING) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_RENDERING) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "renderType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set render
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], renderNames, "renderType", 0, &renderIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageRenderingIntent(wandPtr, renderTypes[renderIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get render
	     */
	    render = MagickGetImageRenderingIntent(wandPtr);
	    for (renderIdx = 0; (size_t) renderIdx < sizeof(renderTypes)/sizeof(renderTypes[0]); renderIdx++) {
		if( renderTypes[renderIdx] == render ) {
		    Tcl_SetResult(interp, (char *)renderNames[renderIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)render));
	}
	break;
    }

    case TM_RESOLUTION:     /* resolution ?x? ?y? */
    case TM_GET_RESOLUTION: /* GetResolution */
    case TM_SET_RESOLUTION: /* SetResolution x ?y? */
    {
	double  x, y;

	if( ((enum subIndex)index == TM_RESOLUTION) && (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?x? ?y?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_RESOLUTION) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_RESOLUTION) && (objc != 3) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x ?y?");
	    return TCL_ERROR;
	}
	if (objc >= 3) {
	    /*
	     * Set resolution = x y, default: y=x
	     */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
            y = x;
	    if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK) ) {
		return stat;
	    }
	    result = MagickSetImageResolution(wandPtr, x, y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get resolution={x y}
	     */
	    Tcl_Obj *listPtr;

	    result = MagickGetImageResolution(wandPtr, &x, &y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	    listPtr = Tcl_NewListObj(0, NULL);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_SCENE:      /* scene ?num? */
    case TM_GET_SCENE:  /* GetScene */
    case TM_SET_SCENE:  /* SetScene num */
    {
        unsigned long num;

	if( (index == TM_SCENE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?num?");
	    return TCL_ERROR;
	}
	if( (index == TM_GET_SCENE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( (index == TM_SET_SCENE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "num");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set scene number
	     */
		if (Tcl_GetLongFromObj(interp, objv[2], (long int *) &num) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageScene(wandPtr, num);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get scene number
	     */
	    num = MagickGetImageScene(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewLongObj((long)num));
	}
	break;
    }

    case TM_SIGNATURE:     /* signature */
    case TM_GET_SIGNATURE: /* GetSignature */
    {
	char *signature;

	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	signature = (char *)MagickGetImageSignature(wandPtr);
	if(signature != NULL) {
	    Tcl_SetResult(interp, signature, TCL_VOLATILE);
	    MagickRelinquishMemory(signature); /* Free TclMagick resource */
	}
	break;
    }

    case TM_IMAGE_SIZE:      /* imagesize */
    case TM_GET_IMAGE_SIZE:  /* GetImageSize */
    {
	Tcl_WideInt size;

        if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
        /*
	 * Get image size={xSize ySize}
	 */
        size = MagickGetImageSize(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewWideIntObj(size));

	break;
    }

    case TM_IMAGE_TYPE:      /* imagetype ?type? */
    case TM_GET_IMAGE_TYPE:  /* GetImageType */
    case TM_SET_IMAGE_TYPE:  /* SetImageType type */
    {
        int typeIdx;
        ImageType type;

	if( ((enum subIndex)index == TM_IMAGE_TYPE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?type?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_IMAGE_TYPE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_IMAGE_TYPE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "type");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set image type
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], typeNames,
				    "typeType", 0, &typeIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageType(wandPtr, typeTypes[typeIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get image type
	     */
	    type = MagickGetImageType(wandPtr);
	    for (typeIdx = 0; (size_t) typeIdx < sizeof(typeTypes)/sizeof(typeTypes[0]); typeIdx++) {
		if( typeTypes[typeIdx] == type ) {
		    Tcl_SetResult(interp, (char *)typeNames[typeIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)type));
	}
	break;
    }

    case TM_IMAGE_UNITS:      /* imageunits ?unitType? */
    case TM_GET_IMAGE_UNITS:  /* GetImageUnits */
    case TM_SET_IMAGE_UNITS:  /* SetImageUnits unitType */
    {
        int unitIdx;
        ResolutionType unit;

	if( ((enum subIndex)index == TM_IMAGE_UNITS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?unitType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_IMAGE_UNITS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_IMAGE_UNITS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "unitType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set image unit
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], unitNames, "unitType", 0, &unitIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageUnits(wandPtr, unitTypes[unitIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get image type
	     */
	    unit = MagickGetImageUnits(wandPtr);
	    for (unitIdx = 0; (size_t) unitIdx < sizeof(unitTypes)/sizeof(unitTypes[0]); unitIdx++) {
		if( unitTypes[unitIdx] == unit ) {
		    Tcl_SetResult(interp, (char *)unitNames[unitIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)unit));
	}
	break;
    }

    case TM_VIRTUALPIXEL:      /* virtualpixelmethod ?methodType? */
    case TM_GET_VIRTUALPIXEL:  /* GetVirtualPixelMethod */
    case TM_SET_VIRTUALPIXEL:  /* SetVirtualPixelMethod methodType */
    {
        int methodIdx;
        VirtualPixelMethod method;

	if( ((enum subIndex)index == TM_IMAGE_UNITS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?methodType?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_VIRTUALPIXEL) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_VIRTUALPIXEL) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "methodType");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set image virtual pixel method
	     */
            if (Tcl_GetIndexFromObj(interp, objv[2], methodNames, "methodType", 0, &methodIdx) != TCL_OK) {
	        return TCL_ERROR;
	    }
	    result = MagickSetImageVirtualPixelMethod(wandPtr, methodTypes[methodIdx]);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get image virtual pixel method
	     */
	    method = MagickGetImageVirtualPixelMethod(wandPtr);
	    for (methodIdx = 0; (size_t) methodIdx < sizeof(methodTypes)/sizeof(methodTypes[0]); methodIdx++) {
		if( methodTypes[methodIdx] == method ) {
		    Tcl_SetResult(interp, (char *)methodNames[methodIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)method));
	}
	break;
    }

    case TM_WHITE_POINT:      /* whitepoint ?x y? */
    case TM_GET_WHITE_POINT:  /* GetWhitePoint */
    case TM_SET_WHITE_POINT:  /* SetWhitePoint x y */
    {
        double x, y;

	if( ((enum subIndex)index == TM_WHITE_POINT) && (objc != 2) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?x y?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_WHITE_POINT) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_WHITE_POINT) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set primary = x y
	     */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
		return stat;
	    }
	    result = MagickSetImageWhitePoint(wandPtr, x, y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get primary={x y}
	     */
	    Tcl_Obj *listPtr;

	    result = MagickGetImageWhitePoint(wandPtr, &x, &y);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	    listPtr = Tcl_NewListObj(0, NULL);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_WIDTH:     /* width */
    case TM_GET_WIDTH: /* GetWidth */
    {
	int value;

        if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	value = MagickGetImageWidth(wandPtr);
        Tcl_SetObjResult(interp, Tcl_NewLongObj(value));

	break;
    }

    case TM_NUMBER:             /* number */
    case TM_GET_NUMBER_IMAGES:  /* GetNumberImages */
    {
        long num;

	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	num = MagickGetNumberImages(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewLongObj(num));
	break;
    }

    case TM_SAMPLING_FACTORS:      /* samplingfactors ?factorList? */
    case TM_GET_SAMPLING_FACTORS:  /* GetSamplingFactors */
    case TM_SET_SAMPLING_FACTORS:  /* GetSamplingFactors factorList */
    {
        double  *factors;

	if( ((enum subIndex)index == TM_SAMPLING_FACTORS) && (objc != 2) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?factorList?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_SAMPLING_FACTORS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_SAMPLING_FACTORS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "factorList");
	    return TCL_ERROR;
	}
	if (objc == 3) {
	    /*
	     * Set sampling factors
	     */
            Tcl_Obj **listPtr;
            int     i, listLen = 0;

	    if( (stat = Tcl_ListObjGetElements(interp, objv[2], &listLen, &listPtr)) != TCL_OK) {
	        return stat;
	    }
	    factors = (double *)ckalloc(listLen * sizeof(double));
            if( factors == NULL ) {
	        Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
	        return TCL_ERROR;
	    }
	    for( i=0; i < listLen; i++ ) {
	        if( (stat = Tcl_GetDoubleFromObj(interp, listPtr[i], &factors[i])) != TCL_OK) {
		    ckfree((char *)factors);
		    return stat;
	        }
	    }
	    result = MagickSetSamplingFactors(wandPtr, (unsigned long)listLen, factors);
	    if (!result) {
	        return myMagickError(interp, wandPtr);
	    }
	} else {
	    /*
	     * Get size={xSize ySize}
	     */
            Tcl_Obj *listPtr = NULL;
            unsigned long i, uLen;

	    factors = MagickGetSamplingFactors(wandPtr, &uLen);
	    if( (factors != NULL) && (uLen > 0) ) {
                listPtr = Tcl_NewListObj(0, NULL);
	        for( i=0; i < uLen; i++ ) {
                    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(factors[i]));
                }
    	        Tcl_SetObjResult(interp, listPtr);
            }
	    if (factors != NULL)
		MagickRelinquishMemory(factors); /* Free TclMagick resource */
	}
	break;
    }

    case TM_SIZE:      /* size ?x y? */
    case TM_GET_SIZE:  /* size */
    case TM_SET_SIZE:  /* size x y */
    {
	unsigned long x, y;

	if( ((enum subIndex)index == TM_SIZE) && (objc != 2) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?xSize ySize?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_SIZE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_SIZE) && (objc != 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "xSize ySize");
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Set size = x y
	     */

		if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y)) != TCL_OK ) {
		return stat;
	    }
	    MagickSetSize(wandPtr, x, y);
	} else {
	    /*
	     * Get size={xSize ySize}
	     */
	    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);

	    result = MagickGetSize(wandPtr, &x, &y);
	    if (!result) {
	        return myMagickError(interp, wandPtr);
	    }
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj((long)x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj((long)y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_HASNEXT:
    case TM_HAS_NEXT_IMAGE:
    {
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}
        result = MagickHasNextImage(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_HASPREVIOUS:
    case TM_HAS_PREVIOUS_IMAGE:
    {
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}
        result = MagickHasPreviousImage(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_IMPLODE:       /* implode ?amount=0.0? */
    case TM_IMPLODE_IMAGE: /* ImplodeImage ?amount? */
    {
	double  amount=0.0;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?amount=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &amount)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickImplodeImage(wandPtr, amount);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_LABEL:        /* label str */
    case TM_LABEL_IMAGE:  /* LabelImage str */
    {
	Tcl_DString extrep;
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "str");
	    return TCL_ERROR;
	}
	result = MagickLabelImage(wandPtr, 
	    Tcl_UtfToExternalDString(NULL, Tcl_GetString(objv[2]), -1, &extrep));

	Tcl_DStringFree(&extrep);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_LEVEL:        /* level ?black=0.0? ?gamma=1.0? ?white=MaxRGB? */
    case TM_LEVEL_IMAGE:  /* LevelImage ?black? ?gamma=1.0? ?white=MaxRGB?  */
    {
	double  black=0.0, white=MaxRGB, gamma=1.0;

	if( objc > 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?black=0.0? ?gamma=1.0? ?white=MaxRGB? ");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &black)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &gamma)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &white)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickLevelImage(wandPtr, black, gamma, white);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_LEVEL_CHANNEL:        /* levelchannel channelType ?black=0.0? ?gamma=1.0? ?white=MaxRGB? */
    case TM_LEVEL_IMAGE_CHANNEL:  /* LevelImageChannel channelType ?black? ?gamma=1.0? ?white=MaxRGB?  */
    {
	int     chanIdx;
	double  black=0.0, white=MaxRGB, gamma=1.0;

	if( objc > 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType ?black=0.0? ?gamma=1.0? ?white=MaxRGB? ");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &black)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &gamma)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetDoubleFromObj(interp, objv[5], &white)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickLevelImageChannel(wandPtr, chanTypes[chanIdx], black, gamma, white);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_MAGNIFY:        /* magnify */
    case TM_MAGNIFY_IMAGE:  /* MagnifyImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickMagnifyImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_MAP:        /* map mapName ?dither=false? */
    case TM_MAP_IMAGE:  /* MapImage mapName ?dither? */
    {
	MagickWand      *mapWand;
	char            *name;
	int             dither=0;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "mapName ?dither=no?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (mapWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetBooleanFromObj(interp, objv[3], &dither)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickMapImage( wandPtr, mapWand, dither );
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_MATTE_FLOODFILL:        /* mattefloodfill opacity ?fuzz=0.0? ?borderPix=none? ?x=0 y=0? */
    case TM_MATTE_FLOODFILL_IMAGE:  /* MatteFloodfillImage opacity ?fuzz? ?borderPix? ?x y? */
    {
	unsigned int	opacity;
	long x = 0, y = 0;
	double  fuzz = 0.0;
	char	*name;
	PixelWand	*borderPtr=NULL;

	if( (objc < 3) || (objc > 7) || (objc == 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "opacity ?fuzz=0.0? ?borderPix=none? ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], (int *) &opacity)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	if( objc > 4 ) {
	    name = Tcl_GetString(objv[4]);
	    if( (borderPtr = findPixelWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
	}
	if( (objc > 5) && ((stat = Tcl_GetLongFromObj(interp, objv[5], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 6) && ((stat = Tcl_GetLongFromObj(interp, objv[6], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickMatteFloodfillImage(wandPtr, (Quantum)(opacity), fuzz, borderPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_MEDIANFILTER:       /* medianfilter ?radius? */
    case TM_MEDIANFILTER_IMAGE: /* MedianFilterImage ?radius? */
    {
	double  radius=0.0;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickMedianFilterImage(wandPtr, radius);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_MINIFY:        /* minify */
    case TM_MINIFY_IMAGE:  /* MinifyImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickMinifyImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}

	break;
    }

    case TM_MODULATE:       /* modulate ?brightness=0.0? ?saturation=0.0? ?hue=0.0? */
    case TM_MODULATE_IMAGE: /* ModulateImage ?brightness? ?saturation? ?hue? */
    {
	double  brightness=0.0, saturation=0.0, hue=0.0;

	if( objc > 5) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?brightness=0.0? ?saturation=0.0? ?hue=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &brightness)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &saturation)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &hue)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickModulateImage(wandPtr, brightness, saturation, hue);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_MONTAGE:       /* montage draw tileGeom thumbGeom mode frameGeom ?newName? */
    case TM_MONTAGE_IMAGE: /* MontageImage draw tileGeom thumbGeom mode frameGeom ?newName? */
        {
	static CONST char *modeNames[] = {
	    "frame", "unframe", "concatenate",
	    (char *) NULL
	};
	static CompositeOperator modeTypes[] = {
	    FrameMode, UnframeMode, ConcatenateMode
	};
        char	    *name, *newName=NULL;
        DrawingWand *drawPtr;
	MagickWand  *newWand;
        int         modeIdx;
        char        *tileGeom, *thumbGeom, *frameGeom;

        if( (objc < 7) || (objc > 8) ) {
            Tcl_WrongNumArgs(interp, 2, objv, "draw tileGeom thumbGeom mode frameGeom ?newName?");
            return TCL_ERROR;
        }
        name = Tcl_GetString(objv[2]);
        if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
            return TCL_ERROR;
	}
        tileGeom  = Tcl_GetString(objv[3]);
        thumbGeom = Tcl_GetString(objv[4]);
	if (Tcl_GetIndexFromObj(interp, objv[5], modeNames, "mode", 0, &modeIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
        frameGeom = Tcl_GetString(objv[6]);
	if( objc > 7 ) {
	    newName = Tcl_GetString(objv[7]);
	}
	newWand = MagickMontageImage(wandPtr, drawPtr, tileGeom, thumbGeom,
				     modeTypes[modeIdx], frameGeom);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	newName = newWandObj(interp, newWand, newName);
	Tcl_SetResult(interp, newName, TCL_VOLATILE);

        break;
    }

    case TM_MORPH:         /* morph num ?newName? */
    case TM_MORPH_IMAGES:  /* MorphImages num ?newName? */
    {
	unsigned long num;
	MagickWand *newWand;
	char *name=NULL;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "num ?newName?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &num)) != TCL_OK ) {
	    return stat;
	}
	if( objc > 3 ) {
	    name = Tcl_GetString(objv[3]);
	}
	newWand = MagickMorphImages(wandPtr, num);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_MOSAIC:         /* mosaic ?newName? */
    case TM_MOSAIC_IMAGES:  /* MosaicImages ?newName? */
    {
	MagickWand *newWand;
	char *name=NULL;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?newName?");
	    return TCL_ERROR;
	}
	if( objc > 2 ) {
	    name = Tcl_GetString(objv[2]);
	}
	newWand = MagickMosaicImages(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_MOTIONBLUR:       /* motionblur ?radius=0.0? ?sigma=0.0? ?angle=0.0? */
    case TM_MOTIONBLUR_IMAGE: /* MotionBlurImage ?radius? ?sigma? ?angle? */
    {
	double  radius=0.0, sigma=0.0, angle=0.0;

	if( objc > 5) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=0.0? ?angle=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &angle)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickMotionBlurImage(wandPtr, radius, sigma, angle);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_NEGATE:       /* negate ?gray=false? */
    case TM_NEGATE_IMAGE: /* NegateImage ?gray=false? */
    {
	int gray=0;

	if (objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?gray=no?");
	    return TCL_ERROR;
	}
        if( (objc > 2) && ((stat = Tcl_GetBooleanFromObj(interp, objv[2], &gray)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickNegateImage(wandPtr, gray);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_NEGATE_CHANNEL:       /* negatechannel channelType ?gray=false? */
    case TM_NEGATE_IMAGE_CHANNEL: /* NegateImageChannel channelType ?gray=false? */
    {
	int gray=0;
        int chanIdx;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType ?gray=no?");
	    return TCL_ERROR;
        }
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
        if( (objc > 3) && ((stat = Tcl_GetBooleanFromObj(interp, objv[3], &gray)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickNegateImageChannel(wandPtr, chanTypes[chanIdx], gray);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_NEXT:          /* next ?offset=1? */
    case TM_NEXT_IMAGE:    /* NextImage ?offset=1? */
    {
	int i, ofs=1;

	if (objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?offset=1?");
	    return TCL_ERROR;
	} else if (objc == 3) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &ofs)) != TCL_OK ) {
		return stat;
	    }
	}
	for( i=0; i<ofs; i++) {
	    result = MagickNextImage(wandPtr);
	}
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_NORMALIZE:       /* normalize */
    case TM_NORMALIZE_IMAGE: /* NormalizeImage */
    {
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickNormalizeImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_OILPAINT:       /* oilpaint ?radius? */
    case TM_OILPAINT_IMAGE: /* OilPaintImage ?radius? */
    {
	double  radius=0.0;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickOilPaintImage(wandPtr, radius);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_OPAQUE:        /* opaque targetPix fillPix ?fuzz=0.0? */
    case TM_OPAQUE_IMAGE:  /* OpaqueImage targetPix fillPix ?fuzz=0.0? */
    {
	double    fuzz=0.0;
	char      *name;
	PixelWand *targetPtr, *fillPtr;

	if( (objc < 4) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "targetPix fillPix ?fuzz=0.0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (targetPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[3]);
	if( (fillPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickOpaqueImage(wandPtr, targetPtr, fillPtr, fuzz);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_PING:          /* ping filename */
    case TM_PING_IMAGE:    /* PingImage filename */
    {
	char *filename;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	filename = Tcl_GetString(objv[2]);
	result = MagickPingImage(wandPtr, filename);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_PREVIEW:         /* preview previewType ?newName? */
    case TM_PREVIEW_IMAGES:  /* PreviewImages previewType ?newName? */
    {
	int previewIdx;
	MagickWand *newWand;
	char *name=NULL;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "previewType ?newName?");
	    return TCL_ERROR;
	}
        if (Tcl_GetIndexFromObj(interp, objv[2], previewNames, "previewType", 0, &previewIdx) != TCL_OK) {
            return TCL_ERROR;
	}
	if( objc > 3 ) {
	    name = Tcl_GetString(objv[3]);
	}
	newWand = MagickPreviewImages(wandPtr, previewTypes[previewIdx]);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }

    case TM_PREVIOUS:          /* previous ?offset=1? */
    case TM_PREVIOUS_IMAGE:    /* PreviousImage ?offset=1? */
    {
	int i, ofs=1;

	if (objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?offset?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetIntFromObj(interp, objv[2], &ofs)) != TCL_OK) ) {
	    return stat;
	}
	for( i=0; i<ofs; i++) {
	    result = MagickPreviousImage(wandPtr);
	}
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_QUANTIZE:        /* quantize numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    case TM_QUANTIZE_IMAGE:  /* QuantizeImage numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    case TM_QUANTIZE_IMAGES: /* QuantizeImage numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    {
	unsigned long numColors, treeDepth=0;
	int dither=0, measureError=0;
	int csIdx=RGBColorspace;

	if( (objc < 3) || (objc > 7) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "numColors ?colorspaceType=RGB? ?treedepth=0? ?dither=no? ?measureError=no?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &numColors)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 3) && (Tcl_GetIndexFromObj(interp, objv[3], csNames, "colorspaceType", 0, &csIdx) != TCL_OK) ) {
	    return TCL_ERROR;
	}
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], (long int *) &treeDepth)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetBooleanFromObj(interp, objv[5], &dither)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 6) && ((stat = Tcl_GetBooleanFromObj(interp, objv[6], &measureError)) != TCL_OK) ) {
	    return stat;
	}
	if( (enum subIndex)index == TM_QUANTIZE_IMAGES ) {
	    result = MagickQuantizeImages(wandPtr, numColors, csTypes[csIdx], treeDepth, dither, measureError);
	} else {
	    result = MagickQuantizeImage(wandPtr, numColors, csTypes[csIdx], treeDepth, dither, measureError);
	}
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_QUERYFONTMETRICS:     /* queryfontmetrics draw text */
    case TM_QUERY_FONT_METRICS:   /* QueryFontMetrics draw text */
    {
        double  *metrics;
        int     i;
        Tcl_Obj *listPtr;
        char    *name, *text;
	DrawingWand *drawPtr;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "draw text");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	text = Tcl_GetString(objv[3]);

	metrics = MagickQueryFontMetrics(wandPtr, drawPtr, text);
	if( (metrics != NULL) ) {
            listPtr = Tcl_NewListObj(0, NULL);
	    for( i=0; i < 7; i++ ) {
                Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(metrics[i]));
            }
    	    Tcl_SetObjResult(interp, listPtr);
        }
	if (metrics != NULL)
	    MagickRelinquishMemory(metrics); /* Free TclMagick resource */

        break;
    }

    case TM_RAISE:        /* raise width height ?x y? ?raise? */
    case TM_RAISE_IMAGE:  /* RaiseImage width height ?x y? ?raise? */
    {
	unsigned long width, height;
	long x=0, y=0;
	int raise=0;

	if( (objc < 4) || (objc > 7) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x y? ?raise=no?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetLongFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetLongFromObj(interp, objv[5], &y)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 6) && ((stat = Tcl_GetBooleanFromObj(interp, objv[6], &raise)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickRaiseImage(wandPtr, width, height, x, y, raise);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_READ:          /* read filename */
    case TM_READ_IMAGE:    /* ReadImage filename */
    {
	char *filename;
	Tcl_DString extrep;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	filename = Tcl_UtfToExternalDString(NULL, Tcl_GetString(objv[2]), -1, &extrep);
	result = MagickReadImage(wandPtr, filename);
	Tcl_DStringFree(&extrep);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_READ_BLOB:          /* readblob data */
    case TM_READ_IMAGE_BLOB:    /* ReadImageBlob data */
    {
	unsigned char *data;
        int length;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "data");
	    return TCL_ERROR;
	}
	data = Tcl_GetByteArrayFromObj(objv[2], &length);
	result = MagickReadImageBlob(wandPtr, data, (size_t)length);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_REDUCENOISE:       /* reducenoise ?radius? */
    case TM_REDUCENOISE_IMAGE: /* ReduceNoiseImage ?radius? */
    {
	double  radius=0.0;

	if( objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickReduceNoiseImage(wandPtr, radius);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_REMOVE:        /* remove */
    case TM_REMOVE_IMAGE:  /* RemoveImage */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickRemoveImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_RESETITERATOR:   /* resetiterator */
    case TM_RESET_ITERATOR:  /* ResetIterator */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	MagickResetIterator(wandPtr);
	break;
    }

    case TM_RESAMPLE:        /* resample xRes ?yRes? ?filter? ?blur=1.0? */
    case TM_RESAMPLE_IMAGE:  /* ResampleImage xRes ?yRes? ?filter? ?blur=1.0? */
    {
	int     filterIdx=0;
	double  xRes, yRes, blur=1.0;

	if( (objc < 3) || (objc > 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "xRes ?yRes=xRes? ?filterType=undefined? ?blur=1.0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &xRes)) != TCL_OK ) {
	    return stat;
	}
	yRes = xRes; /* default */
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &yRes)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && (Tcl_GetIndexFromObj(interp, objv[4], filterNames, "filterType",
					       0, &filterIdx) != TCL_OK) ) {
	    return TCL_ERROR;
	}
	if( (objc > 5) && ((stat = Tcl_GetDoubleFromObj(interp, objv[5], &blur)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickResampleImage(wandPtr, xRes, yRes, filterTypes[filterIdx], blur);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_RESIZE:        /* resize x y ?filter? ?blur=1.0? */
    case TM_RESIZE_IMAGE:  /* ResizeImage x y ?filter? ?blur=1.0? */
    {
	int     filterIdx=0;
	unsigned long x, y;
	double  blur=1.0;

	if( (objc < 4) || (objc > 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y ?filterType=undefined? ?blur=1.0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && (Tcl_GetIndexFromObj(interp, objv[4], filterNames, "filterType", 0, &filterIdx) != TCL_OK) ) {
	    return TCL_ERROR;
	}
	if( (objc > 5) && ((stat = Tcl_GetDoubleFromObj(interp, objv[5], &blur)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickResizeImage(wandPtr, x, y, filterTypes[filterIdx], blur);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ROLL:       /* roll xOfs yOfs */
    case TM_ROLL_IMAGE: /* RollImage xOfs yOfs */
    {
	int  xOfs, yOfs;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "xOfs yOfs");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &xOfs)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &yOfs)) != TCL_OK ) {
	    return stat;
	}
	result = MagickRollImage(wandPtr, xOfs, yOfs);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ROTATE:        /* rotate background degrees */
    case TM_ROTATE_IMAGE:  /* RotateImage background degrees */
    {
	double    degrees;
	PixelWand *bgPtr;
	char      *name;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "background degrees");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (bgPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &degrees)) != TCL_OK ) {
	    return stat;
	}
	result = MagickRotateImage(wandPtr, bgPtr, degrees);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SAMPLE:       /* sample x y */
    case TM_SAMPLE_IMAGE: /* SampleImage x y */
    {
	unsigned long  x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y)) != TCL_OK ) {
	    return stat;
	}
	result = MagickSampleImage(wandPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SCALE:       /* scale x y */
    case TM_SCALE_IMAGE: /* ScaleImage x y */
    {
	unsigned long  x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y)) != TCL_OK ) {
	    return stat;
	}
	result = MagickScaleImage(wandPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SEPARATE:         /* separate channelType */
    case TM_SEPARATE_CHANNEL: /* SeparateImageChannel channelType */
    {
	int chanIdx;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickSeparateImageChannel(wandPtr, chanTypes[chanIdx]);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SETIMAGE:  /* setimage name */
    case TM_SET_IMAGE: /* SetImage name */
    {
	char *name = NULL;
	MagickWand *setWand;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "setwand");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (setWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	result = MagickSetImage(wandPtr, setWand);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SETOPTION:  /* setoption format key value */
    case TM_SET_OPTION: /* SetOption format key value */
    {
	char *format, *key, *value;

	if (objc != 5) {
	    Tcl_WrongNumArgs(interp, 2, objv, "format key value");
	    return TCL_ERROR;
	}
	format  = Tcl_GetString(objv[2]);
	key = Tcl_GetString(objv[3]);
	value = Tcl_GetString(objv[4]);
	result = MagickSetImageOption(wandPtr, format, key, value);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_PASSPHRASE:     /* passphrase phrase */
    case TM_SET_PASSPHRASE: /* SetPassphrase phrase */
    {
	char *phrase;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "phrase");
	    return TCL_ERROR;
	}
	phrase  = Tcl_GetString(objv[2]);
	result = MagickSetPassphrase(wandPtr, phrase);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SHARPEN:       /* sharpen ?radius? ?sigma? */
    case TM_SHARPEN_IMAGE: /* SharpenImage ?radius? ?sigma? */
    {
	double  radius=0.0, sigma=1.0;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=1.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickSharpenImage(wandPtr, radius, sigma);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SHAVE:       /* shave x y */
    case TM_SHAVE_IMAGE: /* ShaveImage x y */
    {
	unsigned long x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y)) != TCL_OK ) {
	    return stat;
	}
	result = MagickShaveImage(wandPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SHEAR:        /* shear background xShear ?yShear? */
    case TM_SHEAR_IMAGE:  /* ShearImage background xShear ?yShear? */
    {
	double    xShear, yShear;
	PixelWand *bgPtr;
	char      *name;

	if( (objc < 4) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "background xShear ?yShear=xShear?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (bgPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &xShear)) != TCL_OK ) {
	    return stat;
	}
	yShear = xShear; /* default */
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &yShear)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickShearImage(wandPtr, bgPtr, xShear, yShear);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SOLARIZE:        /* solarize ?threshold? */
    case TM_SOLARIZE_IMAGE:  /* SolarizeImage ?threshold? */
    {
	double    threshold=0.0;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?threshold=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &threshold)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickSolarizeImage(wandPtr, threshold);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SPREAD:       /* spread ?radius? */
    case TM_SPREAD_IMAGE: /* SpreadImage ?radius? */
    {
	double  radius=0.0;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickSpreadImage(wandPtr, radius);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_STEGANO:       /* stegano watermark ?offset=0? ?newName? */
    case TM_STEGANO_IMAGE: /* SteganoImage watermark ?offset=0? ?newName? */
    {
	MagickWand      *watermarkPtr, *newWand;
	char            *name, *newName=NULL;
	int             offset=0;

	if( objc > 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "watermark ?offset=0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (watermarkPtr = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], &offset)) != TCL_OK) ) {
	    return stat;
	}
	if( objc > 4 ) {
	    newName = Tcl_GetString(objv[4]);
	}
	newWand = MagickSteganoImage( wandPtr, watermarkPtr, offset );
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	newName = newWandObj(interp, newWand, newName);
	Tcl_SetResult(interp, newName, TCL_VOLATILE);

	break;
    }

    case TM_STEREO:       /* stereo anotherWand ?newName? */
    case TM_STEREO_IMAGE: /* StereoImage anotherwand ?newName? */
    {
	MagickWand      *anotherPtr, *newWand;
	char            *name, *newName=NULL;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "anotherWand ?newName?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (anotherPtr = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( objc > 3 ) {
	    newName = Tcl_GetString(objv[3]);
	}
	newWand = MagickStereoImage( wandPtr, anotherPtr );
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	newName = newWandObj(interp, newWand, newName);
	Tcl_SetResult(interp, newName, TCL_VOLATILE);

	break;
    }

    case TM_STRIP:       /* strip */
    case TM_STRIP_IMAGE: /* StripImage */
    {
	if( objc > 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickStripImage(wandPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_SWIRL:       /* swirl ?degrees? */
    case TM_SWIRL_IMAGE: /* SwirlImage ?degrees? */
    {
	double  degrees=0.0;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?degrees=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &degrees)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickSwirlImage(wandPtr, degrees);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_TEXTURE:       /* texture textWand ?newName? */
    case TM_TEXTURE_IMAGE: /* TextureImage textWand ?newName? */
    {
	MagickWand      *textPtr, *newWand;
	char            *name, *newName=NULL;

	if( objc > 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "textWand ?newName?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (textPtr = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( objc > 3 ) {
	    newName = Tcl_GetString(objv[3]);
	}
	newWand = MagickTextureImage( wandPtr, textPtr );
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	newName = newWandObj(interp, newWand, newName);
	Tcl_SetResult(interp, newName, TCL_VOLATILE);

	break;
    }

    case TM_THRESHOLD:        /* threshold threshold */
    case TM_THRESHOLD_IMAGE:  /* ThresholdImage threshold */
    {
	double    threshold=0.0;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "threshold");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &threshold)) != TCL_OK ) {
	    return stat;
	}
	result = MagickThresholdImage(wandPtr, threshold);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_THRESHOLDCHANNEL:         /* thresholdchannel channelType threshold */
    case TM_THRESHOLD_IMAGE_CHANNEL:  /* ThresholdImageChannel channelType threshold */
    {
        int    chanIdx;
	double threshold=0.0;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType threshold");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &threshold)) != TCL_OK ) {
	    return stat;
	}
	result = MagickThresholdImageChannel(wandPtr, chanTypes[chanIdx], threshold);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_TINT:        /* tint tintPixel opacityPixel */
    case TM_TINT_IMAGE:  /* TintImage tintPixel opacityPixel */
    {
	char	  *name;
	PixelWand *tintPtr, *opacityPtr;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "tintPixel opacityPixel");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (tintPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[3]);
	if( (opacityPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}

	result = MagickTintImage(wandPtr, tintPtr, opacityPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_TRANSFORM:       /* transform crop geometry ?newName? */
    case TM_TRANSFORM_IMAGE: /* TransformImage crop geometry ?newName? */
    {
	MagickWand      *newWand;
	char            *crop, *geometry, *newName=NULL;

	if( (objc < 4) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "crop geometry ?newName?");
	    return TCL_ERROR;
	}
	crop = Tcl_GetString(objv[2]);
	geometry = Tcl_GetString(objv[3]);
	if( objc > 4 ) {
	    newName = Tcl_GetString(objv[4]);
	}
	newWand = MagickTransformImage( wandPtr, crop, geometry );
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	newName = newWandObj(interp, newWand, newName);
	Tcl_SetResult(interp, newName, TCL_VOLATILE);

	break;
    }

    case TM_TRANSPARENT:        /* transparent targetPix ?opacity? ?fuzz=0.0? */
    case TM_TRANSPARENT_IMAGE:  /* TransparentImage targetPix ?opacity? ?fuzz=0.0? */
    {
	double    fuzz=0.0;
	unsigned  opacity=0;
	char      *name;
	PixelWand *targetPtr;

	if( (objc < 3) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "targetPix ?opacity=0? ?fuzz=0.0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (targetPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], (int *) &opacity)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickTransparentImage(wandPtr, targetPtr, (const Quantum)opacity, fuzz);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_TRIM:        /* trim ?fuzz=0.0? */
    case TM_TRIM_IMAGE:  /* TrimImage ?fuzz=0.0? */
    {
	double    fuzz=0.0;

	if( objc > 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?fuzz=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickTrimImage(wandPtr, fuzz);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_UNSHARPMASK:        /* unsharpmask ?radius? ?sigma? ?amount? ?threshold? */
    case TM_UNSHARPMASK_IMAGE:  /* UnsharpMaskImage ?radius? ?sigma? ?amount? ?threshold? */
    {
	double radius=0.0, sigma=1.0, amount=0.0, threshold=0.0;

	if( objc > 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?radius=0.0? ?sigma=1.0? ?amount=0.0? ?threshold=0.0?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetDoubleFromObj(interp, objv[2], &radius)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &sigma)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &amount)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetDoubleFromObj(interp, objv[5], &threshold)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickUnsharpMaskImage(wandPtr, radius, sigma, amount, threshold);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_WAVE:        /* wave amplitude wavelength */
    case TM_WAVE_IMAGE:  /* WaveImage amplitude wavelength */
    {
	double amplitude, wavelength;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "amplitude wavelength");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &amplitude)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &wavelength)) != TCL_OK ) {
	    return stat;
	}
	result = MagickWaveImage(wandPtr, amplitude, wavelength);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_WHITE_THRESHOLD:        /* whitethreshold thresholdPixel */
    case TM_WHITE_THRESHOLD_IMAGE:  /* WhiteThresholdImage thresholdPixel */
    {
	char	*name;
	PixelWand	*threshPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "thresholdPixel");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (threshPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}

	result = MagickWhiteThresholdImage(wandPtr, threshPtr);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_WRITE:         /* write filename */
    case TM_WRITE_IMAGE:   /* WriteImage filename */
    case TM_WRITE_IMAGES:  /* WriteImages filename ?adjoin? */
    {
	char *filename;
	int  adjoin=0;
	Tcl_DString extrep;

	if( ((enum subIndex)index == TM_WRITE_IMAGES) && ((objc < 3) || (objc > 4)) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename ?adjoin=no?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index != TM_WRITE_IMAGES) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	filename = Tcl_UtfToExternalDString(NULL, Tcl_GetString(objv[2]), -1, &extrep);
	if( (objc > 3) && ((stat = Tcl_GetBooleanFromObj(interp, objv[3], &adjoin)) != TCL_OK) ) {
	    return stat;
	}
	if( (enum subIndex) index == TM_WRITE_IMAGES ) {
	    result = MagickWriteImages(wandPtr, filename, adjoin);
	} else {
	    result = MagickWriteImage(wandPtr, filename);
	}
	Tcl_DStringFree(&extrep);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_WRITE_BLOB:         /* writeblob */
    case TM_WRITE_IMAGE_BLOB:   /* WriteImageBlob */
    {
	unsigned char *data;
        size_t  length;

	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
        data = MagickWriteImageBlob(wandPtr, &length);
        if(data != NULL) {
            Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(data, length));
            MagickRelinquishMemory(data); /* Free TclMagick resource */
	}
	break;
    }

    /*
     * All subcommands should be matched
     * A DEFAULT or TM_END_OF_TABLE would be a serious fault
     */
    case TM_END_OF_TABLE:
    default:
    {
	Tcl_AppendResult(interp, "TclMagick wandObjCmd: Unmatched subcommand table entry", NULL);
	return TCL_ERROR;
    }

    } /* switch(index) */

    return(TCL_OK);
}



/*----------------------------------------------------------------------
 * PixelWand object command
 *----------------------------------------------------------------------
 *
 * PixelWand subcmd ?args?
 *
 *----------------------------------------------------------------------
 */
static int pixelObjCmd(
    ClientData  clientData,         /* PixelWand pointer */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    static CONST char *subCmds[] = {
        "set",             "get",             "color",
        "SetBlack",        "GetBlack",        "SetBlackQuantum",   "GetBlackQuantum",
        "SetBlue",         "GetBlue",         "SetBlueQuantum",    "GetBlueQuantum",
        "SetCyan",         "GetCyan",         "SetCyanQuantum",    "GetCyanQuantum",
        "SetGreen",        "GetGreen",        "SetGreenQuantum",   "GetGreenQuantum",
        "SetMagenta",      "GetMagenta",      "SetMagentaQuantum", "GetMagentaQuantum",
        "SetOpacity",      "GetOpacity",      "SetOpacityQuantum", "GetOpacityQuantum",
        "SetRed",          "GetRed",          "SetRedQuantum",     "GetRedQuantum",
        "SetYellow",       "GetYellow",       "SetYellowQuantum",  "GetYellowQuantum",
        "SetColor",        "GetColor",        "GetColorAsString",
        "SetQuantumColor", "GetQuantumColor", "quantumcolor",
        (char *) NULL
    };
    enum subIndex {
        TM_SET,         TM_GET,         TM_COLOR,
        TM_SET_BLACK,   TM_GET_BLACK,   TM_SET_BLACK_QUANTUM,   TM_GET_BLACK_QUANTUM,
        TM_SET_BLUE,    TM_GET_BLUE,    TM_SET_BLUE_QUANTUM,    TM_GET_BLUE_QUANTUM,
        TM_SET_CYAN,    TM_GET_CYAN,    TM_SET_CYAN_QUANTUM,    TM_GET_CYAN_QUANTUM,
        TM_SET_GREEN,   TM_GET_GREEN,   TM_SET_GREEN_QUANTUM,   TM_GET_GREEN_QUANTUM,
        TM_SET_MAGENTA, TM_GET_MAGENTA, TM_SET_MAGENTA_QUANTUM, TM_GET_MAGENTA_QUANTUM,
        TM_SET_OPACITY, TM_GET_OPACITY, TM_SET_OPACITY_QUANTUM, TM_GET_OPACITY_QUANTUM,
        TM_SET_RED,     TM_GET_RED,     TM_SET_RED_QUANTUM,     TM_GET_RED_QUANTUM,
        TM_SET_YELLOW,  TM_GET_YELLOW,  TM_SET_YELLOW_QUANTUM,  TM_GET_YELLOW_QUANTUM,
        TM_SET_COLOR,   TM_GET_COLOR,   TM_GET_COLOR_AS_STRING,
        TM_SET_QUANTUM, TM_GET_QUANTUM, TM_QUANTUM,
        TM_END_OF_TABLE
    };
    static CONST char  *colors[]  = {
        "black",   "blue",    "cyan", "green",
        "magenta", "opacity", "red",  "yellow",
        (char *) NULL };

    int          index, stat, result, quantFlag=0;
    int          quantVal;
    double       normVal;
    TclMagickObj *magickPtr = (TclMagickObj *) clientData;
    PixelWand    *wandPtr   = (PixelWand *) magickPtr->wandPtr;

#ifdef MAGICK_DEBUG
    /*
     * Verify subCmds table size
     * No idea how to do this at compile-time
     */
    if( sizeof(subCmds)/sizeof(subCmds[0]) != TM_END_OF_TABLE + 1 ) {
        Tcl_Panic("pixelObjCmd: Invalid subCmds[] table");
    }
#endif

    if( objc < 2 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "subcmd ?args?" );
        return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], subCmds, "subcmd", 0, &index) != TCL_OK) {
        return TCL_ERROR;
    }

    switch ((enum subIndex)index) {

    case TM_SET:    /* set ?-quant*um? color val color val ... */
    {
	int     color;

	if( objc < 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?-quant*? color value color value ...");
	    return TCL_ERROR;
	}
	if( strncmp(Tcl_GetString(objv[2]), "-quant", 6) == 0 ) {
	    quantFlag = 1;
	    objv += 3, objc -= 3;
	} else {
	    quantFlag = 0;
	    objv += 2, objc -= 2;
	}
	while( objc > 0 ) {
	    if (Tcl_GetIndexFromObj(interp, objv[0], colors, "color", 0, &color) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if( objc == 1 ) {
		Tcl_AppendResult(interp, "Magick: Missing value for color ", objv[0], NULL);
		return TCL_ERROR;
	    }
	    if( quantFlag ) {
		if( (stat = Tcl_GetIntFromObj(interp, objv[1], &quantVal)) != TCL_OK ) {
		    return stat;
		}
		switch(color) {
		case 0:  PixelSetBlackQuantum(wandPtr,    (Quantum)quantVal); break;
		case 1:  PixelSetBlueQuantum(wandPtr,     (Quantum)quantVal); break;
		case 2:  PixelSetCyanQuantum(wandPtr,     (Quantum)quantVal); break;
		case 3:  PixelSetGreenQuantum(wandPtr,    (Quantum)quantVal); break;
		case 4:  PixelSetMagentaQuantum(wandPtr,  (Quantum)quantVal); break;
		case 5:  PixelSetOpacityQuantum(wandPtr,  (Quantum)quantVal); break;
		case 6:  PixelSetRedQuantum(wandPtr,      (Quantum)quantVal); break;
		case 7:  PixelSetYellowQuantum(wandPtr,   (Quantum)quantVal); break;
		}
	    } else {
		if( (stat = Tcl_GetDoubleFromObj(interp, objv[1], &normVal)) != TCL_OK ) {
		    return stat;
		}
		switch(color) {
		case 0:  PixelSetBlack(wandPtr,    normVal); break;
		case 1:  PixelSetBlue(wandPtr,     normVal); break;
		case 2:  PixelSetCyan(wandPtr,     normVal); break;
		case 3:  PixelSetGreen(wandPtr,    normVal); break;
		case 4:  PixelSetMagenta(wandPtr,  normVal); break;
		case 5:  PixelSetOpacity(wandPtr,  normVal); break;
		case 6:  PixelSetRed(wandPtr,      normVal); break;
		case 7:  PixelSetYellow(wandPtr,   normVal); break;
		}
	    }
	    objv += 2, objc -= 2;
	}
	break;
    }

    case TM_GET:    /* get -quantum color color ... */
    {
	Tcl_Obj *listPtr;
	int     color;

	if( objc < 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?-quant*? color color ...");
	    return TCL_ERROR;
	}
	if( strncmp(Tcl_GetString(objv[2]), "-quant", 6) == 0 ) {
	    quantFlag = 1;
	    objv += 3, objc -= 3;
	} else {
	    quantFlag = 0;
	    objv += 2, objc -= 2;
	}
	listPtr = Tcl_NewListObj(0, NULL);
	while( objc > 0 ) {
	    if (Tcl_GetIndexFromObj(interp, objv[0], colors, "color", 0, &color) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if( quantFlag ) {
		switch(color) {
		case 0:  quantVal = PixelGetBlackQuantum(wandPtr);    break;
		case 1:  quantVal = PixelGetBlueQuantum(wandPtr);     break;
		case 2:  quantVal = PixelGetCyanQuantum(wandPtr);     break;
		case 3:  quantVal = PixelGetGreenQuantum(wandPtr);    break;
		case 4:  quantVal = PixelGetMagentaQuantum(wandPtr);  break;
		case 5:  quantVal = PixelGetOpacityQuantum(wandPtr);  break;
		case 6:  quantVal = PixelGetRedQuantum(wandPtr);      break;
		case 7:  quantVal = PixelGetYellowQuantum(wandPtr);   break;
		}
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(quantVal));
	    } else {
		switch(color) {
		case 0:  normVal = PixelGetBlack(wandPtr);    break;
		case 1:  normVal = PixelGetBlue(wandPtr);     break;
		case 2:  normVal = PixelGetCyan(wandPtr);     break;
		case 3:  normVal = PixelGetGreen(wandPtr);    break;
		case 4:  normVal = PixelGetMagenta(wandPtr);  break;
		case 5:  normVal = PixelGetOpacity(wandPtr);  break;
		case 6:  normVal = PixelGetRed(wandPtr);      break;
		case 7:  normVal = PixelGetYellow(wandPtr);   break;
		}
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(normVal));
	    }
	    objv += 1, objc -= 1;
	}
	Tcl_SetObjResult(interp, listPtr);

	break;
    }

    case TM_COLOR:                  /* color ?string? */
    case TM_SET_COLOR:              /* SetColor string */
    case TM_GET_COLOR:              /* GetColor */
    case TM_GET_COLOR_AS_STRING:    /* GetColorAsString */
    {
	char *color;

	if( ((enum subIndex)index == TM_COLOR) && (objc != 2) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?string?");
	    return TCL_ERROR;
	} else if ( ((enum subIndex)index == TM_SET_COLOR) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "string");
	    return TCL_ERROR;
	} else if ( ((enum subIndex)index == TM_GET_COLOR) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if ( ((enum subIndex)index == TM_GET_COLOR_AS_STRING) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( objc == 3 ) {   /* Set color */
	    color = Tcl_GetString(objv[2]);
	    result = PixelSetColor(wandPtr, color);
	    if( !result ) {
		Tcl_AppendResult(interp, "Magick: Invalid color ", color, NULL);
		return TCL_ERROR;
	    }
	} else {
	    color = PixelGetColorAsString(wandPtr);
	    Tcl_SetResult(interp, color, TCL_VOLATILE);
	    MagickRelinquishMemory(color); /* Free TclMagick resource */
	}

	break;
    }

    case TM_SET_BLACK_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_BLACK:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetBlackQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetBlack(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_BLUE_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_BLUE:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetBlueQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetBlue(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_CYAN_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_CYAN:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetCyanQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetCyan(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_GREEN_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_GREEN:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetGreenQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetGreen(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_MAGENTA_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_MAGENTA:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetMagentaQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetMagenta(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_OPACITY_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_OPACITY:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetOpacityQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetOpacity(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_RED_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_RED:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetRedQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetRed(wandPtr, normVal);
	}
	break;
    }

    case TM_SET_YELLOW_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_SET_YELLOW:
    {
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &quantVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetYellowQuantum(wandPtr, (Quantum)quantVal);
	} else {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &normVal)) != TCL_OK ) {
		return stat;
	    }
	    PixelSetYellow(wandPtr, normVal);
	}
	break;
    }

    case TM_GET_BLACK_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_BLACK:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetBlackQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetBlack(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_BLUE_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_BLUE:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetBlueQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetBlue(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_CYAN_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_CYAN:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetCyanQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetCyan(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_GREEN_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_GREEN:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetGreenQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetGreen(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_MAGENTA_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_MAGENTA:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetMagentaQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetMagenta(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_OPACITY_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_OPACITY:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetOpacityQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetOpacity(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_RED_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_RED:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetRedQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetRed(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_GET_YELLOW_QUANTUM:
    {
	quantFlag = 1; /* and continue ... */
    }
    case TM_GET_YELLOW:
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( quantFlag ) {
	    quantVal = PixelGetYellowQuantum(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(quantVal));
	} else {
	    normVal = PixelGetYellow(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(normVal));
	}
	break;
    }

    case TM_QUANTUM:                /* quantum ?{r g b o}? */
    case TM_GET_QUANTUM:            /* GetQuantum */
    case TM_SET_QUANTUM:            /* SetQuantum {r g b o} */
    {
        PixelPacket color;

	if( ((enum subIndex)index == TM_QUANTUM) && (objc != 2) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?{r g b o}?");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_GET_QUANTUM) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_QUANTUM) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "{r g b o}");
	    return TCL_ERROR;
	}
	if( objc == 3 ) {
	    /*
	     * Set quantum values
	     */
            Tcl_Obj        **listPtr;
            unsigned long  quant[4];
            int            i, listLen;

	    if( (stat = Tcl_ListObjGetElements(interp, objv[2], &listLen, &listPtr)) != TCL_OK ) {
	        return stat;
	    }
            if( listLen != 4 ) {
	        Tcl_AppendResult(interp, "PixelWand SetQuantum: List of 4 colors {r g b o} required", NULL);
	        return TCL_ERROR;
            }
	    for( i=0; i < 4; i++ ) {
		    if( (stat = Tcl_GetLongFromObj(interp, listPtr[i], (long int *) &quant[i])) != TCL_OK) {
		    return stat;
	        }
	    }
            color.red     = (Quantum) quant[0];
            color.green   = (Quantum) quant[1];
            color.blue    = (Quantum) quant[2];
            color.opacity = (Quantum) quant[3];

	    PixelSetQuantumColor(wandPtr, &color);
	} else {
	    /*
	     * Get quantum values
	     */
            Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);;

	    PixelGetQuantumColor(wandPtr, &color);

            Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(color.red));
            Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(color.green));
            Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(color.blue));
            Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(color.opacity));

            Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }
    /*
     * All subcommands should be matched
     * A DEFAULT or TM_END_OF_TABLE would be a serious fault
     */
    case TM_END_OF_TABLE:
    default:
    {
	Tcl_AppendResult(interp, "TclMagick pixelObjCmd: Unmatched subcommand table entry", NULL);
	return TCL_ERROR;
    }

    } /* switch(index) */

    return(TCL_OK);
}


/*----------------------------------------------------------------------
 * DrawingWand object command
 *----------------------------------------------------------------------
 *
 * DrawWand subcmd ?args?
 *
 *----------------------------------------------------------------------
 */
static int drawObjCmd(
    ClientData  clientData,         /* DrawingWand pointer */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    static CONST char *subCmds[] = {
        "affine",           "Affine",
        "annotation",       "Annotation",
        "arc",              "Arc",
        "bezier",           "Bezier",
        "circle",           "Circle",
        "clippath",         "GetClipPath",      "SetClipPath",
        "cliprule",         "GetClipRule",      "SetClipRule",
        "clipunits",        "GetClipUnits",     "SetClipUnits",
        "color",            "Color",
        "comment",          "Comment",
        "ellipse",          "Ellipse",
        "fillcolor",        "GetFillColor",     "SetFillColor",
        "fillopacity",      "GetFillOpacity",   "SetFillOpacity",
        "fillpatternurl",   			"SetFillPatternURL",
        "fillrule",         "GetFillRule",      "SetFillRule",
        "font",             "GetFont",          "SetFont",
        "fontfamily",       "GetFontFamily",    "SetFontFamily",
        "fontsize",         "GetFontSize",      "SetFontSize",
        "fontstretch",      "GetFontStretch",   "SetFontStretch",
        "fontstyle",        "GetFontStyle",     "SetFontStyle",
        "fontweight",       "GetFontWeight",    "SetFontWeight",
        "gravity",          "GetGravity",       "SetGravity",
        "line",             "Line",
        "matte",            "Matte",
        "point",            "Point",
        "polygon",          "Polygon",
        "polyline",         "Polyline",
        "path",
        "push",             "pop",
        "PushClipPath",     "PopClipPath",
        "PushDefs",         "PopDefs",
        "PushGraphicContext","PopGraphicContext",
        "PushPattern",      "PopPattern",
        "rectangle",        "Rectangle",
        "render",           "Render",
        "rotate",           "Rotate",
        "round",            "RoundRectangle",
        "scale",            "Scale",
        "skewx",            "SkewX",
        "skewy",            "SkewY",
        "strokecolor",      "GetStrokeColor",       "SetStrokeColor",
        "strokeantialias",  "GetStrokeAntialias",   "SetStrokeAntialias",
        "strokedasharray",  "GetStrokeDashArray",   "SetStrokeDashArray",
        "strokedashoffset", "GetStrokeDashOffset",  "SetStrokeDashOffset",
        "strokelinecap",    "GetStrokeLineCap",     "SetStrokeLineCap",
        "strokelinejoin",   "GetStrokeLineJoin",    "SetStrokeLineJoin",
        "strokemiterlimit", "GetStrokeMiterLimit",  "SetStrokeMiterLimit",
        "strokeopacity",    "GetStrokeOpacity",     "SetStrokeOpacity",
        "strokepatternurl",   			    "SetStrokePatternURL",
        "strokewidth",      "GetStrokeWidth",       "SetStrokeWidth",
        "textantialias",    "GetTextAntialias",     "SetTextAntialias",
        "textdecoration",   "GetTextDecoration",    "SetTextDecoration",
        "textencoding",     "GetTextEncoding",      "SetTextEncoding",
        "textundercolor",   "GetTextUnderColor",    "SetTextUnderColor",
        "translate",        "Translate",
        "viewbox",          "SetViewbox",
        (char *) NULL
    };
    enum subIndex {
        TM_AFFINE,          TM_AAFFINE,
        TM_ANNOTATION,      TM_AANNOTATION,
        TM_ARC,             TM_AARC,
        TM_BEZIER,          TM_BBEZIER,
        TM_CIRCLE,          TM_CCIRCLE,
        TM_CLIPPATH,        TM_GET_CLIPPATH,        TM_SET_CLIPPATH,
        TM_CLIPRULE,        TM_GET_CLIPRULE,        TM_SET_CLIPRULE,
        TM_CLIPUNITS,       TM_GET_CLIPUNITS,       TM_SET_CLIPUNITS,
        TM_COLOR,           TM_CCOLOR,
        TM_COMMENT,         TM_CCOMMENT,
        TM_ELLIPSE,         TM_EELLIPSE,
        TM_FILLCOLOR,       TM_GET_FILLCOLOR,       TM_SET_FILLCOLOR,
        TM_FILLOPACITY,     TM_GET_FILLOPACITY,     TM_SET_FILLOPACITY,
        TM_FILLPATTERNURL,                          TM_SET_FILLPATTERNURL,
        TM_FILLRULE,        TM_GET_FILLRULE,        TM_SET_FILLRULE,
        TM_FONT,            TM_GET_FONT,            TM_SET_FONT,
        TM_FONTFAMILY,      TM_GET_FONTFAMILY,      TM_SET_FONTFAMILY,
        TM_FONTSIZE,        TM_GET_FONTSIZE,        TM_SET_FONTSIZE,
        TM_FONTSTRETCH,     TM_GET_FONTSTRETCH,     TM_SET_FONTSTRETCH,
        TM_FONTSTYLE,       TM_GET_FONTSTYLE,       TM_SET_FONTSTYLE,
        TM_FONTWEIGHT,      TM_GET_FONTWEIGHT,      TM_SET_FONTWEIGHT,
        TM_GRAVITY,         TM_GET_GRAVITY,         TM_SET_GRAVITY,
        TM_LINE,            TM_LLINE,
        TM_MATTE,           TM_MMATTE,
        TM_POINT,           TM_PPOINT,
        TM_POLYGON,         TM_PPOLYGON,
        TM_POLYLINE,        TM_PPOLYLINE,
        TM_PATH,
        TM_PUSH,            TM_POP,
        TM_PUSH_CLIPPATH,   TM_POP_CLIPPATH,
        TM_PUSH_DEFS,       TM_POP_DEFS,
        TM_PUSH_GRAPHIC_CTX,TM_POP_GRAPHIC_CTX,
        TM_PUSH_PATTERN,    TM_POP_PATTERN,
        TM_RECTANGLE,       TM_RRECTANGLE,
        TM_RENDER,          TM_RRENDER,
        TM_ROTATE,          TM_RROTATE,
        TM_ROUND,           TM_ROUND_RECTANGLE,
        TM_SCALE,           TM_SSCALE,
        TM_SKEWX,           TM_SKEW_X,
        TM_SKEWY,           TM_SKEW_Y,
        TM_STROKE_COLOR,    TM_GET_STROKE_COLOR,        TM_SET_STROKE_COLOR,
        TM_STROKE_ANTIALIAS,TM_GET_STROKE_ANTIALIAS,    TM_SET_STROKE_ANTIALIAS,
        TM_STROKE_DASHARRAY,TM_GET_STROKE_DASHARRAY,    TM_SET_STROKE_DASHARRAY,
        TM_STROKE_DASHOFFS, TM_GET_STROKE_DASHOFFS,     TM_SET_STROKE_DASHOFFS,
        TM_STROKE_LINECAP,  TM_GET_STROKE_LINECAP,      TM_SET_STROKE_LINECAP,
        TM_STROKE_LINEJOIN, TM_GET_STROKE_LINEJOIN,     TM_SET_STROKE_LINEJOIN,
        TM_STROKE_MITERLIM, TM_GET_STROKE_MITERLIM,     TM_SET_STROKE_MITERLIM,
        TM_STROKE_OPACITY,  TM_GET_STROKE_OPACITY,      TM_SET_STROKE_OPACITY,
        TM_STROKE_PATTERNURL,                           TM_SET_STROKE_PATTERNURL,
        TM_STROKE_WIDTH,    TM_GET_STROKE_WIDTH,        TM_SET_STROKE_WIDTH,
        TM_TEXT_ANTIALIAS,  TM_GET_TEXT_ANTIALIAS,      TM_SET_TEXT_ANTIALIAS,
        TM_TEXT_DECORATION, TM_GET_TEXT_DECORATION,     TM_SET_TEXT_DECORATION,
        TM_TEXT_ENCODING,   TM_GET_TEXT_ENCODING,       TM_SET_TEXT_ENCODING,
        TM_TEXT_UNDERCOLOR, TM_GET_TEXT_UNDERCOLOR,     TM_SET_TEXT_UNDERCOLOR,
        TM_TRANSLATE,       TM_TTRANSLATE,
        TM_VIEWBOX,         TM_SET_VIEWBOX,
        TM_END_OF_TABLE
    };
    static CONST char *pushCmdNames[] = {
        "clippath", "defs", "graphiccontext", "pattern",
        (char *) NULL
    };
    enum pushCmdIndex {
        TM_PUSH_CMD_CLIP, TM_PUSH_CMD_DEFS, TM_PUSH_CMD_GRAPH, TM_PUSH_CMD_PAT,
    };
    static CONST char *fillRuleNames[] = {
        "undefined","evenodd","nonzero",
        (char *) NULL
    };
    static FillRule fillRules[] = {
	UndefinedRule, EvenOddRule, NonZeroRule
    };
    static CONST char *paintMethodNames[] = {
        "point", "replace", "floodfill", "filltoborder", "reset",
	(char *) NULL
    };
    static PaintMethod paintMethods[] = {
	PointMethod, ReplaceMethod, FloodfillMethod, FillToBorderMethod, ResetMethod
    };
    int         index, stat;
    TclMagickObj *magickPtr = (TclMagickObj *) clientData;
    DrawingWand  *wandPtr   = (DrawingWand *) magickPtr->wandPtr;

#ifdef MAGICK_DEBUG
    /*
     * Verify subCmds table size
     * No idea how to do this at compile-time
     */
    if( sizeof(subCmds)/sizeof(subCmds[0]) != TM_END_OF_TABLE + 1 ) {
        Tcl_Panic("magickObjCmd: Invalid subCmds[] table");
    }
#endif

    if( objc < 2 ) {
        Tcl_WrongNumArgs( interp, 1, objv, "subcmd ?args?" );
        return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], subCmds, "subcmd", 0, &index) != TCL_OK) {
        return TCL_ERROR;
    }

    switch ((enum subIndex)index) {

    case TM_AFFINE:     /* affine sx rx ry sy tx ty */
    case TM_AAFFINE:    /* Affine sx rx ry sy tx ty */
    {
	AffineMatrix A;

	if( objc != 8 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "sx rx ry sy tx ty");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &A.sx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &A.rx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &A.ry)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &A.sy)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &A.tx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[7], &A.ty)) != TCL_OK ) {
	    return stat;
	}
	DrawAffine(wandPtr, &A);
	break;
    }

    case TM_ANNOTATION:     /* annotation x y text */
    case TM_AANNOTATION:    /* Annotation x y text */
    {
	double x, y;
	unsigned char *text;

	if( objc != 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y text");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	text = (unsigned char *) Tcl_GetString(objv[4]);
	DrawAnnotation(wandPtr, x, y, text);
	break;
    }

    case TM_ARC:    /* arc sx sy ex ey sd ed */
    case TM_AARC:   /* Arc sx sy ex ey sd ed */
    {
	double sx, sy, ex, ey, sd, ed;

	if( objc != 8 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "sx sy ex ey sd ed");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &sx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &sy)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &ex)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &ey)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &sd)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[7], &ed)) != TCL_OK ) {
	    return stat;
	}
	DrawArc(wandPtr, sx, sy, ex, ey, sd, ed);
	break;
    }

    case TM_BEZIER:     /* bezier   x y x y ... */
    case TM_BBEZIER:    /* Bezier   x y x y ... */
    case TM_POLYGON:    /* polygon  x y x y ... */
    case TM_PPOLYGON:   /* Polygon  x y x y ... */
    case TM_POLYLINE:   /* polyline x y x y ... */
    case TM_PPOLYLINE:  /* Polyline x y x y ... */
    {
	PointInfo *coordinates;
	unsigned long i, num;

	if( (objc-2) % 2 != 0 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y x y ...");
	    return TCL_ERROR;
	}
	num = (objc-2)/2;   /* number of coordinates */
	coordinates = (PointInfo *)ckalloc(num*sizeof(coordinates[0]));
	if( coordinates == NULL ) {
	    Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
	    return TCL_ERROR;
	}
        memset( coordinates, 0, num*sizeof(coordinates[0]) );

	objc -= 2, objv += 2;
	for( i=0; i<num; i++) {
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[0], &coordinates[i].x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[1], &coordinates[i].y)) != TCL_OK ) {
		return stat;
	    }
	    objc -= 2, objv +=2;
	}
	switch ((enum subIndex)index) {
	case TM_BEZIER:   case TM_BBEZIER:
	    DrawBezier(wandPtr, num, coordinates);
            break;
	case TM_POLYGON:  case TM_PPOLYGON:
	    DrawPolygon(wandPtr, num, coordinates);
            break;
	case TM_POLYLINE: case TM_PPOLYLINE:
	    DrawPolyline(wandPtr, num, coordinates);
            break;
	default:
	    break;
	}

	ckfree((char *)coordinates);
	break;
    }

    case TM_CIRCLE:     /* circle ox oy px py */
    case TM_CCIRCLE:    /* Circle ox oy px py */
    {
	double ox, oy, px, py;

	if( objc != 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "ox oy px py");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &ox)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &oy)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &px)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &py)) != TCL_OK ) {
	    return stat;
	}
	DrawCircle(wandPtr, ox, oy, px, py);
	break;
    }

    case TM_COLOR:  /* color x y paintMethod */
    case TM_CCOLOR: /* Color x y paintMethod */
    {
	double x, y;
	int paintIdx;

	if( objc != 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y paintMethod");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	if (Tcl_GetIndexFromObj(interp, objv[4], paintMethodNames, "paintMethod", 0, &paintIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	DrawColor(wandPtr, x, y, paintMethods[paintIdx]);
	break;
    }

    case TM_COMMENT:    /* comment text */
    case TM_CCOMMENT:   /* Comment text */
    {
	char *text;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "text");
	    return TCL_ERROR;
	}
	text = Tcl_GetString(objv[2]);
	DrawComment(wandPtr, text);
	break;
    }

    case TM_ELLIPSE:    /* ellipse ox oyo rx ry start end */
    case TM_EELLIPSE:   /* Ellipse ox oyo rx ry start end */
    {
	double ox, oy, rx, ry, start, end;

	if( objc != 8 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "ox oy rx ry start end");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &ox)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &oy)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &rx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &ry)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &start)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[7], &end)) != TCL_OK ) {
	    return stat;
	}
	DrawEllipse(wandPtr, ox, oy, rx, ry, start, end);
	break;
    }

    case TM_CLIPPATH:        /* clippath ?name? */
    case TM_GET_CLIPPATH:    /* GetClipPath */
    case TM_SET_CLIPPATH:    /* SetClipPath name */
    {
	char *name;

	if( ((enum subIndex)index == TM_CLIPPATH) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?name?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_CLIPPATH) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_CLIPPATH) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set name */
	    name = Tcl_GetString(objv[2]);
	    DrawSetClipPath(wandPtr, name);
	} else {    /* Get name */
	    name = (char *)DrawGetClipPath(wandPtr);
	    if(name != NULL) {
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		MagickRelinquishMemory(name);
	    }
	}
	break;
    }

    case TM_CLIPRULE:        /* cliprule ?rule? */
    case TM_GET_CLIPRULE:    /* GetClipRule */
    case TM_SET_CLIPRULE:    /* SetClipRule rule */
    {
	int      ruleIdx;
	FillRule rule;

	if( ((enum subIndex)index == TM_CLIPRULE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?rule?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_CLIPRULE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_CLIPRULE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "rule");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set rule */
	    if (Tcl_GetIndexFromObj(interp, objv[2], fillRuleNames, "fillRule", 0, &ruleIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetClipRule(wandPtr, fillRules[ruleIdx]);
	} else {    /* Get rule */
	    rule = DrawGetClipRule(wandPtr);
	    for (ruleIdx = 0; (size_t) ruleIdx < sizeof(fillRules)/sizeof(fillRules[0]); ruleIdx++) {
		if( fillRules[ruleIdx] == rule ) {
		    Tcl_SetResult(interp, (char *)fillRuleNames[ruleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)rule));
	}
	break;
    }

    case TM_CLIPUNITS:      /* clipunits ?unit? */
    case TM_GET_CLIPUNITS:  /* GetClipUnits */
    case TM_SET_CLIPUNITS:  /* SetClipunits unit */
    {
	static CONST char *unitNames[] = {
	    "userspace", "userspaceonuse", "objectboundingbox",
	    (char *) NULL
	};
	static ClipPathUnits unitTypes[] = {
	    UserSpace, UserSpaceOnUse, ObjectBoundingBox
	};
	int           unitIdx;
	ClipPathUnits unit;

	if( ((enum subIndex)index == TM_CLIPUNITS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?unit?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_CLIPUNITS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_CLIPUNITS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "unit");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set rule */
	    if (Tcl_GetIndexFromObj(interp, objv[2], unitNames, "unit", 0, &unitIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetClipUnits(wandPtr, unitTypes[unitIdx]);
	} else {    /* Get rule */
	    unit = DrawGetClipUnits(wandPtr);
	    for (unitIdx = 0; (size_t) unitIdx < sizeof(unitTypes)/sizeof(unitTypes[0]); unitIdx++) {
		if( unitTypes[unitIdx] == unit ) {
		    Tcl_SetResult(interp, (char *)unitNames[unitIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)unit));
	}
	break;
    }

    case TM_FILLCOLOR:              /* fillcolor ?pixel? */
    case TM_STROKE_COLOR:           /* strokecolor ?pixel? */
    case TM_TEXT_UNDERCOLOR:        /* textundercolor ?pixel? */
    {
        PixelWand   *pixelPtr = NULL;
        char        *name = NULL;

        if( objc > 3 ) {
            Tcl_WrongNumArgs(interp, 2, objv, "?pixelName?");
            return TCL_ERROR;
        }
	if (objc > 2) { /* Set color / Get color with new name */
	    name = Tcl_GetString(objv[2]);
	    pixelPtr = findPixelWand(interp, name);
        }
        /*
         * GET color if pixel object doesn't exists
         * otherwise SET color
         */
        if( pixelPtr == NULL ) {
            pixelPtr = NewPixelWand();
            name = newPixelObj(interp, pixelPtr, name );

            switch ((enum subIndex)index) {
		case TM_FILLCOLOR:
			DrawGetFillColor(wandPtr, pixelPtr);
			break;
		case TM_STROKE_COLOR:
			DrawGetStrokeColor(wandPtr, pixelPtr);
			break;
		case TM_TEXT_UNDERCOLOR:
			DrawGetTextUnderColor(wandPtr, pixelPtr);
			break;
		default:
			break;
            }
            Tcl_SetResult(interp, name, TCL_VOLATILE);

        } else {
            switch ((enum subIndex)index) {
		case TM_FILLCOLOR:
			DrawSetFillColor(wandPtr, pixelPtr);
			break;
		case TM_STROKE_COLOR:
			DrawSetStrokeColor(wandPtr, pixelPtr);
			break;
		case TM_TEXT_UNDERCOLOR:
			DrawSetTextUnderColor(wandPtr, pixelPtr);
			break;
		default:
			break;
            }
        }
	break;
    }

    case TM_GET_FILLCOLOR:          /* GetFillColor pixel */
    case TM_GET_STROKE_COLOR:       /* GetStrokeColor pixel */
    case TM_GET_TEXT_UNDERCOLOR:    /* GetTextUnderColor pixel */
       {
            PixelWand   *pixelPtr;
            char        *name;

            if( objc != 3 ) {
                Tcl_WrongNumArgs(interp, 2, objv, "pixelName");
                return TCL_ERROR;
            }
            name = Tcl_GetString(objv[2]);
            if( (pixelPtr = findPixelWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
            switch ((enum subIndex)index) {
	    case TM_GET_FILLCOLOR:
		DrawGetFillColor(wandPtr, pixelPtr);
		break;
	    case TM_GET_STROKE_COLOR:
		DrawGetStrokeColor(wandPtr, pixelPtr);
		break;
	    case TM_GET_TEXT_UNDERCOLOR:
		DrawGetTextUnderColor(wandPtr, pixelPtr);
		break;
	    default:
		break;
            }
            break;
        }

    case TM_SET_FILLCOLOR:          /* SetFillColor pixel */
    case TM_SET_STROKE_COLOR:       /* SetStrokeColor pixel */
    case TM_SET_TEXT_UNDERCOLOR:    /* SetTextUnderColor pixel */
       {
            PixelWand   *pixelPtr;
            char        *name;

            if( objc != 3 ) {
                Tcl_WrongNumArgs(interp, 2, objv, "pixelName");
                return TCL_ERROR;
            }
            name = Tcl_GetString(objv[2]);
            if( (pixelPtr = findPixelWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
            switch ((enum subIndex)index) {
	    case TM_SET_FILLCOLOR:
		DrawSetFillColor(wandPtr, pixelPtr);
		break;
	    case TM_SET_STROKE_COLOR:
		DrawSetStrokeColor(wandPtr, pixelPtr);
		break;
	    case TM_SET_TEXT_UNDERCOLOR:
		DrawSetTextUnderColor(wandPtr, pixelPtr);
		break;
	    default:
		break;
            }
            break;
        }

    case TM_FILLOPACITY:     /* fillopacity ?value? */
    case TM_GET_FILLOPACITY: /* GetFillOpacity  */
    case TM_SET_FILLOPACITY: /* SetFillOpacity value */
    {
	double value;

	if( ((enum subIndex)index == TM_FILLOPACITY) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?value?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FILLOPACITY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FILLOPACITY) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set fill opacity */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &value)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetFillOpacity(wandPtr, value);
	} else {    /* Get fill opacity */
	    value = DrawGetFillOpacity(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(value));
	}
	break;
    }

    case TM_FILLPATTERNURL:      /* fillpatternurl url */
    case TM_SET_FILLPATTERNURL:  /* SetFillPatternURL url */
    {
	char *url;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "url");
	    return TCL_ERROR;
	}
	url = Tcl_GetString(objv[2]);
	DrawSetFillPatternURL(wandPtr, url);
	break;
    }

    case TM_FILLRULE:        /* fillrule ?rule? */
    case TM_GET_FILLRULE:    /* GetFillRule */
    case TM_SET_FILLRULE:    /* SetFillRule rule */
    {
	int      ruleIdx;
	FillRule rule;

	if( ((enum subIndex)index == TM_FILLRULE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?rule?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FILLRULE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FILLRULE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "rule");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set rule */
	    if (Tcl_GetIndexFromObj(interp, objv[2], fillRuleNames, "fillRule", 0, &ruleIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetFillRule(wandPtr, fillRules[ruleIdx]);
	} else {    /* Get rule */
	    rule = DrawGetFillRule(wandPtr);
	    for (ruleIdx = 0; (size_t) ruleIdx < sizeof(fillRules)/sizeof(fillRules[0]); ruleIdx++) {
		if( fillRules[ruleIdx] == rule ) {
		    Tcl_SetResult(interp, (char *)fillRuleNames[ruleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)rule));
	}
	break;
    }

    case TM_FONT:        /* font ?fontname? */
    case TM_GET_FONT:    /* GetFont */
    case TM_SET_FONT:    /* SetFont fontname */
    {
	char *fontname;

	if( ((enum subIndex)index == TM_FONT) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?fontname?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONT) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONT) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "fontname");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font */
	    fontname = Tcl_GetString(objv[2]);
	    DrawSetFont(wandPtr, fontname);
	} else {    /* Get font */
	    fontname = DrawGetFont(wandPtr);
	    if(fontname != NULL) {
		Tcl_SetResult(interp, fontname, TCL_VOLATILE);
		MagickRelinquishMemory(fontname); /* Free TclMagick resource */
	    }
	}
	break;
    }

    case TM_FONTFAMILY:        /* fontfamily ?name? */
    case TM_GET_FONTFAMILY:    /* GetFontFamily */
    case TM_SET_FONTFAMILY:    /* SetFontFamily name */
    {
	char *name;

	if( ((enum subIndex)index == TM_FONTFAMILY) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?name?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONTFAMILY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONTFAMILY) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font family */
	    name = Tcl_GetString(objv[2]);
	    DrawSetFontFamily(wandPtr, name);
	} else {    /* Get font family */
	    name = DrawGetFontFamily(wandPtr);
	    if(name != NULL) {
		Tcl_SetResult(interp, name, TCL_VOLATILE);
		MagickRelinquishMemory(name); /* Free TclMagick resource */
	    }
	}
	break;
    }

    case TM_FONTSIZE:        /* fontsize ?value? */
    case TM_GET_FONTSIZE:    /* GetFontSize */
    case TM_SET_FONTSIZE:    /* SetFontSize value */
    {
	double value;

	if( ((enum subIndex)index == TM_FONTSIZE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?value?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONTSIZE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONTSIZE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font size */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &value)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetFontSize(wandPtr, value);
	} else {    /* Get font size */
	    value = DrawGetFontSize(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(value));
	}
	break;
    }

    case TM_FONTSTRETCH:     /* fontstretch ?stretchType? */
    case TM_GET_FONTSTRETCH: /* GetFontStretch */
    case TM_SET_FONTSTRETCH: /* SetFontStretch stretchType */
    {
	static CONST char *stretchNames[] = {
	    "normal",        "ultracondensed", "extracondensed", "condensed",
	    "semicondensed", "semiexpanded",   "expanded",       "extraexpanded",
	    "ultraexpanded", "any",
	    (char *) NULL
	};
	static StretchType stretchTypes[] = {
	    NormalStretch,        UltraCondensedStretch, ExtraCondensedStretch, CondensedStretch,
	    SemiCondensedStretch, SemiExpandedStretch,   ExpandedStretch,       ExtraExpandedStretch,
	    UltraExpandedStretch, AnyStretch

	};
	int stretchIdx;
	StretchType stretch;

	if( ((enum subIndex)index == TM_FONTSTRETCH) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?stretchType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONTSTRETCH) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONTSTRETCH) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "stretchType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font stretch */
	    if (Tcl_GetIndexFromObj(interp, objv[2], stretchNames, "stretchType", 0, &stretchIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetFontStretch(wandPtr, stretchTypes[stretchIdx]);
	} else {    /* Get font stretch */
	    stretch = DrawGetFontStretch(wandPtr);
	    for (stretchIdx = 0; (size_t) stretchIdx < sizeof(stretchTypes)/sizeof(stretchTypes[0]); stretchIdx++) {
		if( stretchTypes[stretchIdx] == stretch ) {
		    Tcl_SetResult(interp, (char *)stretchNames[stretchIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)stretch));
	}
	break;
    }

    case TM_FONTSTYLE:     /* fontstyle ?style? */
    case TM_GET_FONTSTYLE: /* GetFontStyle */
    case TM_SET_FONTSTYLE: /* SetFontStyle style */
    {
	static CONST char *styleNames[] = {
	    "normal", "italic", "oblique", "any",
	    (char *) NULL
	};
	static StyleType styleTypes[] = {
	    NormalStyle, ItalicStyle, ObliqueStyle, AnyStyle
	};
	int styleIdx;
	StyleType style;

	if( ((enum subIndex)index == TM_FONTSTYLE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?styleType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONTSTYLE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONTSTYLE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "styleType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font style */
	    if (Tcl_GetIndexFromObj(interp, objv[2], styleNames, "styleType", 0, &styleIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetFontStyle(wandPtr, styleTypes[styleIdx]);
	} else {    /* Get font style */
	    style = DrawGetFontStyle(wandPtr);
	    for (styleIdx = 0; (size_t) styleIdx < sizeof(styleTypes)/sizeof(styleTypes[0]); styleIdx++) {
		if( styleTypes[styleIdx] == style ) {
		    Tcl_SetResult(interp, (char *)styleNames[styleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)style));
	}
	break;
    }

    case TM_FONTWEIGHT:     /* fontweight ?value? */
    case TM_GET_FONTWEIGHT: /* GetFontWeight */
    case TM_SET_FONTWEIGHT: /* SetFontWeight value */
    {
	unsigned value;

	if( ((enum subIndex)index == TM_FONTSTYLE) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?value?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_FONTSTYLE) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_FONTSTYLE) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "value");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font weight */
		if( (stat = Tcl_GetIntFromObj(interp, objv[2], (int *) &value)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetFontWeight(wandPtr, value);
	} else {    /* Get font weight */
	    value = DrawGetFontWeight(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)value));
	}
	break;
    }

    case TM_GRAVITY:     /* gravity ?gravityType? */
    case TM_GET_GRAVITY: /* gravity */
    case TM_SET_GRAVITY: /* gravity gravityType */
    {
	static CONST char *gravNames[] = {
	    "forget", "northwest", "north",    "northeast", "west",
	    "center", "east",      "southwest","south",     "southeast",
	    "static",
	    (char *) NULL
	};
	static GravityType gravTypes[] = {
	    ForgetGravity, NorthWestGravity, NorthGravity,     NorthEastGravity, WestGravity,
	    CenterGravity, EastGravity,      SouthWestGravity, SouthGravity,     SouthEastGravity,
	    StaticGravity
	};
	int gravIdx;
	GravityType grav;

	if( ((enum subIndex)index == TM_GRAVITY) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?gravityType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_GRAVITY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_GRAVITY) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "gravityType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set gravity */
	    if (Tcl_GetIndexFromObj(interp, objv[2], gravNames, "gravityType", 0, &gravIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetGravity(wandPtr, gravTypes[gravIdx]);
	} else {    /* Get gravity */
	    grav = DrawGetGravity(wandPtr);
	    for (gravIdx = 0; (size_t) gravIdx < sizeof(gravTypes)/sizeof(gravTypes[0]); gravIdx++) {
		if( gravTypes[gravIdx] == grav ) {
		    Tcl_SetResult(interp, (char *)gravNames[gravIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)grav));
	}
	break;
    }

    case TM_LINE:       /* line sx sy ex ey */
    case TM_LLINE:      /* Line sx sy ex ey */
    {
	double sx, sy, ex, ey;

	if( objc != 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "sx sy ex ey");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &sx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &sy)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &ex)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &ey)) != TCL_OK ) {
	    return stat;
	}
	DrawLine(wandPtr, sx, sy, ex, ey);
	break;
    }

    case TM_MATTE:  /* matte x y paintMethod */
    case TM_MMATTE: /* Matte x y paintMethod */
    {
	double x, y;
	int paintIdx=0;

	if( objc != 5 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y paintMethod");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	if (Tcl_GetIndexFromObj(interp, objv[4], paintMethodNames, "paintMethod", 0, &paintIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	DrawMatte(wandPtr, x, y, paintMethods[paintIdx]);
	break;
    }

    case TM_POINT:  /* point x y */
    case TM_PPOINT: /* Point x y */
    {
	double x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	DrawPoint(wandPtr, x, y);
	break;
    }

    case TM_RECTANGLE:  /* rectangle x1 y1 x2 y2 */
    case TM_RRECTANGLE: /* Rectangle x1 y1 x2 y2 */
    {
	double x1, y1, x2, y2;

	if( objc != 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x1 y1 x2 y2");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &x2)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &y2)) != TCL_OK ) {
	    return stat;
	}
	DrawRectangle(wandPtr, x1, y1, x2, y2);
	break;
    }

    case TM_RENDER:  /* render */
    case TM_RRENDER: /* Render */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	/*
	  FIXME: DrawRender() is deprecated.  Use MagickDrawImage() instead.
	  Note that TM_DRAW or TM_DRAW_IMAGE already draw on the image. so "render" is not needed.
	*/
	DrawRender(wandPtr);
	break;
    }

    case TM_ROTATE:     /* rotate degrees */
    case TM_RROTATE:    /* Rotate degrees */
    {
	double degrees;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "degrees");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &degrees)) != TCL_OK ) {
	    return stat;
	}
	DrawRotate(wandPtr, degrees);
	break;
    }

    case TM_ROUND:          /* round x1 y1 x2 y2 rx ry */
    case TM_ROUND_RECTANGLE:/* RoundRectangle x1 y1 x2 y2 rx ry */
    {
	double x1, y1, x2, y2, rx, ry;

	if( objc != 8 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x1 y1 x2 y2 rx ry");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &x2)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &y2)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &rx)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[7], &ry)) != TCL_OK ) {
	    return stat;
	}
	DrawRoundRectangle(wandPtr, x1, y1, x2, y2, rx, ry);
	break;
    }

    case TM_SCALE:  /* scale x y */
    case TM_SSCALE: /* Scale x y */
    {
	double x, y;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x ?y?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	y = x; /* default */
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK) ) {
	    return stat;
	}
	DrawScale(wandPtr, x, y);
	break;
    }

    case TM_SKEWX:     /* skewx degrees */
    case TM_SKEW_X:    /* SkewX degrees */
    {
	double degrees;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "degrees");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &degrees)) != TCL_OK ) {
	    return stat;
	}
	DrawSkewX(wandPtr, degrees);
	break;
    }

    case TM_SKEWY:     /* skewy degrees */
    case TM_SKEW_Y:    /* SkewY degrees */
    {
	double degrees;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "degrees");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &degrees)) != TCL_OK ) {
	    return stat;
	}
	DrawSkewY(wandPtr, degrees);
	break;
    }

    case TM_STROKE_ANTIALIAS:     /* strokeantialias ?flag? */
    case TM_GET_STROKE_ANTIALIAS: /* GetStrokeAntialias */
    case TM_SET_STROKE_ANTIALIAS: /* SetStrokeAntialias flag */
    {
	int flag;

	if( ((enum subIndex)index == TM_STROKE_ANTIALIAS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?flag?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_ANTIALIAS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_ANTIALIAS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "flag");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set antialias flag */
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &flag)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeAntialias(wandPtr, flag);
	} else {    /* Get antialias flag */
	    flag = DrawGetStrokeAntialias(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)flag));
	}
	break;
    }

    case TM_STROKE_DASHARRAY:     /* strokedasharray {} | val val ... */
    case TM_GET_STROKE_DASHARRAY: /* GetStrokeDashArray */
    case TM_SET_STROKE_DASHARRAY: /* SetStrokeDashArray {} | val val ... */
    {
	unsigned long i, num;
	double *arr;

	if( ((enum subIndex)index == TM_GET_STROKE_DASHARRAY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_DASHARRAY) && (objc < 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "{} | val val ...");
	    return TCL_ERROR;
	}
	if (objc == 2) { /* Get dash array */
	    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);

	    arr = DrawGetStrokeDashArray(wandPtr, &num);
	    while ( num > 0 ) {
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(*arr));
		++arr;
	    }
	    Tcl_SetObjResult(interp, listPtr);
	} else {    /* Set dash array */
	    char *str = Tcl_GetString(objv[2]);

	    if( strlen(str) == 0 ) { /* SetStrokeDashArray {} : Remove existing dash array */
		DrawSetStrokeDashArray(wandPtr, 0, NULL);
	    } else {
		num = objc-2;
		arr = (double *)ckalloc(num * sizeof(*arr));
		if( arr == NULL ) {
		    Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
		    return TCL_ERROR;
		}
		for( i=0; i<num; i++ ) {
		    if( (stat = Tcl_GetDoubleFromObj(interp, objv[i+2], &arr[i])) != TCL_OK ) {
			return stat;
		    }
		}
		DrawSetStrokeDashArray(wandPtr, num, arr);
		ckfree((char *)arr);
	    }
	}
	break;
    }

    case TM_STROKE_DASHOFFS:     /* strokedashoffset ?val? */
    case TM_GET_STROKE_DASHOFFS: /* strokedashoffset */
    case TM_SET_STROKE_DASHOFFS: /* strokedashoffset val */
    {
	double val;

	if( ((enum subIndex)index == TM_STROKE_DASHOFFS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?val?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_DASHOFFS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_DASHOFFS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "val");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set stroke dash offset */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &val)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeDashOffset(wandPtr, val);
	} else {    /* Get stroke dash offset */
	    val = DrawGetStrokeDashOffset(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
	}
	break;
    }

    case TM_STROKE_LINECAP:     /* strokelinecap ?linecapType? */
    case TM_GET_STROKE_LINECAP: /* GetStrokeLineCap */
    case TM_SET_STROKE_LINECAP: /* SetStrokeLineCap linecapType */
    {
	static CONST char *capNames[] = {
	    "undefined", "butt", "round", "square",
	    (char *) NULL
	};
	static LineCap capTypes[] = {
	    UndefinedCap, ButtCap, RoundCap, SquareCap
	};
	int capIdx;
	LineCap cap;

	if( ((enum subIndex)index == TM_STROKE_LINECAP) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?linecapType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_LINECAP) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_LINECAP) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "linecapType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font style */
	    if (Tcl_GetIndexFromObj(interp, objv[2], capNames, "linecapType", 0, &capIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetStrokeLineCap(wandPtr, capTypes[capIdx]);
	} else {    /* Get font style */
	    cap = DrawGetStrokeLineCap(wandPtr);
	    for (capIdx = 0; (size_t) capIdx < sizeof(capTypes)/sizeof(capTypes[0]); capIdx++) {
		if( capTypes[capIdx] == cap ) {
		    Tcl_SetResult(interp, (char *)capNames[capIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)cap));
	}
	break;
    }

    case TM_STROKE_LINEJOIN:     /* strokelinejoin ?linejoinType? */
    case TM_GET_STROKE_LINEJOIN: /* GetStrokeLineJoin */
    case TM_SET_STROKE_LINEJOIN: /* SetStrokeLineJoin linejoinType */
    {
	static CONST char *joinNames[] = {
	    "undefined", "miter", "round", "bevel",
	    (char *) NULL
	};
	static LineJoin joinTypes[] = {
	    UndefinedJoin, MiterJoin, RoundJoin, BevelJoin

	};
	int joinIdx;
	LineJoin join;

	if( ((enum subIndex)index == TM_STROKE_LINEJOIN) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?linejoinType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_LINEJOIN) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_LINEJOIN) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "linejoinType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set font style */
	    if (Tcl_GetIndexFromObj(interp, objv[2], joinNames, "linejoinType", 0, &joinIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetStrokeLineCap(wandPtr, joinTypes[joinIdx]);
	} else {    /* Get font style */
	    join = DrawGetStrokeLineCap(wandPtr);
	    for (joinIdx = 0; (size_t) joinIdx < sizeof(joinTypes)/sizeof(joinTypes[0]); joinIdx++) {
		if( joinTypes[joinIdx] == join ) {
		    Tcl_SetResult(interp, (char *)joinNames[joinIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)join));
	}
	break;
    }

    case TM_STROKE_MITERLIM:     /* strokemiterlimit ?val? */
    case TM_GET_STROKE_MITERLIM: /* strokemiterlimit */
    case TM_SET_STROKE_MITERLIM: /* strokemiterlimit val */
    {
	unsigned val;

	if( ((enum subIndex)index == TM_STROKE_MITERLIM) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?val?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_MITERLIM) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_MITERLIM) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "val");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set stroke miterlimit */
		if( (stat = Tcl_GetIntFromObj(interp, objv[2], (int *) &val)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeMiterLimit(wandPtr, val);
	} else {    /* Get stroke miterlimit */
	    val = DrawGetStrokeMiterLimit(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)val));
	}
	break;
    }

    case TM_STROKE_OPACITY:     /* strokeopacity ?val? */
    case TM_GET_STROKE_OPACITY: /* strokeopacity */
    case TM_SET_STROKE_OPACITY: /* strokeopacity val */
    {
	double val;

	if( ((enum subIndex)index == TM_STROKE_OPACITY) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?val?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_OPACITY) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_OPACITY) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "val");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set stroke opacity */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &val)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeOpacity(wandPtr, val);
	} else {    /* Get stroke opacity */
	    val = DrawGetStrokeOpacity(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
	}
	break;
    }

    case TM_STROKE_PATTERNURL:      /* strokepatternurl url */
    case TM_SET_STROKE_PATTERNURL:  /* SetStrokePatternURL url */
    {
	char *url;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "url");
	    return TCL_ERROR;
	}
	url = Tcl_GetString(objv[2]);
	DrawSetStrokePatternURL(wandPtr, url);
	break;
    }

    case TM_STROKE_WIDTH:     /* strokewidth ?val? */
    case TM_GET_STROKE_WIDTH: /* strokewidth */
    case TM_SET_STROKE_WIDTH: /* strokewidth val */
    {
	double val;

	if( ((enum subIndex)index == TM_STROKE_WIDTH) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?val?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_STROKE_WIDTH) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_STROKE_WIDTH) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "val");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set stroke width */
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &val)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeWidth(wandPtr, val);
	} else {    /* Get stroke width */
	    val = DrawGetStrokeWidth(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(val));
	}
	break;
    }

    case TM_TEXT_ANTIALIAS:     /* textantialias ?flag? */
    case TM_GET_TEXT_ANTIALIAS: /* GetTextAntialias */
    case TM_SET_TEXT_ANTIALIAS: /* SetTextAntialias flag */
    {
	int flag;

	if( ((enum subIndex)index == TM_TEXT_ANTIALIAS) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?flag?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_TEXT_ANTIALIAS) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_TEXT_ANTIALIAS) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "flag");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set antialias flag */
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &flag)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetTextAntialias(wandPtr, flag);
	} else {    /* Get antialias flag */
	    flag = DrawGetTextAntialias(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(flag));
	}
	break;
    }

    case TM_TEXT_DECORATION:     /* textdecoration ?decorationType? */
    case TM_GET_TEXT_DECORATION: /* GetTextDecoration */
    case TM_SET_TEXT_DECORATION: /* SetTextDecoration decorationType */
    {
	static CONST char *decoNames[] = {
	    "none", "underline", "overline", "linethrough",
	    (char *) NULL
	};
	static DecorationType decoTypes[] = {
	    NoDecoration, UnderlineDecoration, OverlineDecoration, LineThroughDecoration
	};
	int decoIdx;
	DecorationType deco;

	if( ((enum subIndex)index == TM_TEXT_DECORATION) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?decorationType?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_TEXT_DECORATION) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_TEXT_DECORATION) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "decorationType");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set antialias flag */
	    if (Tcl_GetIndexFromObj(interp, objv[2], decoNames, "decorationType", 0, &decoIdx) != TCL_OK) {
		return TCL_ERROR;
	    }
	    DrawSetTextDecoration(wandPtr, decoTypes[decoIdx]);
	} else {    /* Get antialias flag */
	    deco = DrawGetTextDecoration(wandPtr);
	    for (decoIdx = 0; (size_t) decoIdx < sizeof(decoTypes)/sizeof(decoTypes[0]); decoIdx++) {
		if( decoTypes[decoIdx] == deco ) {
		    Tcl_SetResult(interp, (char *)decoNames[decoIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj((int)deco));
	}
	break;
    }

    case TM_TEXT_ENCODING:      /* textencoding ?string? */
    case TM_GET_TEXT_ENCODING:  /* GetTextEncoding */
    case TM_SET_TEXT_ENCODING:  /* SetTextEncoding string */
    {
	char *str;

	if( ((enum subIndex)index == TM_TEXT_ENCODING) && (objc > 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?string?");
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_GET_TEXT_ENCODING) && (objc != 2) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	} else if( ((enum subIndex)index == TM_SET_TEXT_ENCODING) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "string");
	    return TCL_ERROR;
	}
	if (objc == 3) { /* Set antialias flag */
	    str = Tcl_GetString(objv[2]);
	    DrawSetTextEncoding(wandPtr, str);
	} else {    /* Get antialias flag */
	    str = DrawGetTextEncoding(wandPtr);
	    Tcl_SetResult(interp, str, TCL_VOLATILE);
	    MagickRelinquishMemory(str);
	}
	break;
    }

    case TM_TRANSLATE:  /* translate x y */
    case TM_TTRANSLATE: /* Translate x y */
    {
	double x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	DrawTranslate(wandPtr, x, y);
	break;
    }

    case TM_VIEWBOX:        /* viewbox x1 y1 x2 y2 */
    case TM_SET_VIEWBOX:    /* SetViewbox x1 y1 x2 y2 */
    {
	unsigned long x1, y1, x2, y2;

	if( objc != 6 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x1 y1 x2 y2");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], (long int *) &x1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], (long int *) &y1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[4], (long int *) &x2)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[5], (long int *) &y2)) != TCL_OK ) {
	    return stat;
	}
	DrawSetViewbox(wandPtr, x1, y1, x2, y2);
	break;
    }

    /****************************************************************
     * PUSH & POP COMMANDS
     ****************************************************************/
    case TM_PUSH_CLIPPATH: /* PushClipPath id */
    {
	char *id;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "id");
	    return TCL_ERROR;
	}
	id = Tcl_GetString(objv[2]);
	DrawPushClipPath(wandPtr, id);
	break;
    }

    case TM_PUSH_DEFS: /* PushDefs */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	DrawPushDefs(wandPtr);
	break;
    }

    case TM_PUSH_GRAPHIC_CTX: /* PushGraphicContext */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	DrawPushGraphicContext(wandPtr);
	break;
    }

    case TM_PUSH_PATTERN: /* PushPattern id x y width height */
    {
	char   *id;
	double x, y, width, height;

	if( objc != 7 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "id x y width height");
	    return TCL_ERROR;
	}
	id = Tcl_GetString(objv[3]);
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[3], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &y)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &height)) != TCL_OK ) {
	    return stat;
	}
	DrawPushPattern(wandPtr, id, x, y, width, height);
	break;
    }

    case TM_POP_CLIPPATH:       /* PopClipPath */
    case TM_POP_DEFS:           /* PopDefs */
    case TM_POP_GRAPHIC_CTX:    /* PopGraphicContext */
    case TM_POP_PATTERN:        /* PopPattern */
    {
	if( objc != 2 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	switch((enum subIndex)index) {
	case TM_POP_CLIPPATH:       DrawPopClipPath(wandPtr); break;
	case TM_POP_DEFS:           DrawPopDefs(wandPtr); break;
	case TM_POP_GRAPHIC_CTX:    DrawPopGraphicContext(wandPtr); break;
	case TM_POP_PATTERN:        DrawPopPattern(wandPtr); break;
	default: break;
	}
	break;
    }

    case TM_PUSH:   /* push what ?args? */
    {
	int what;

	if (Tcl_GetIndexFromObj(interp, objv[2], pushCmdNames, "what", 0, &what) != TCL_OK) {
	    return TCL_ERROR;
	}
	switch((enum pushCmdIndex)what) {
	case TM_PUSH_CMD_CLIP: /* clippath */
	{
	    char *id;

	    if( objc != 4 ) {
		Tcl_WrongNumArgs(interp, 2, objv, "id");
		return TCL_ERROR;
	    }
	    id = Tcl_GetString(objv[3]);
	    DrawPushClipPath(wandPtr, id);
	    break;
	}
	case TM_PUSH_CMD_DEFS: /* defs */
	{
	    if( objc != 3 ) {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		return TCL_ERROR;
	    }
	    DrawPushDefs(wandPtr);
	}
	case TM_PUSH_CMD_GRAPH: /* graphiccontext */
	{
	    if( objc != 3 ) {
		Tcl_WrongNumArgs(interp, 2, objv, NULL);
		return TCL_ERROR;
	    }
	    DrawPushGraphicContext(wandPtr);
	    break;
	}
	case TM_PUSH_CMD_PAT: /* pattern */
	{
	    char   *id;
	    double x, y, width, height;

	    if( objc != 8 ) {
		Tcl_WrongNumArgs(interp, 2, objv, "id x y width height");
		return TCL_ERROR;
	    }
	    id = Tcl_GetString(objv[3]);
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[4], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[5], &y)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[6], &width)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetDoubleFromObj(interp, objv[7], &height)) != TCL_OK ) {
		return stat;
	    }
	    DrawPushPattern(wandPtr, id, x, y, width, height);
	    break;
	}
	} /* switch(what) */
	break;
    }

    case TM_POP:   /* pop what */
    {
	int what;

	if (Tcl_GetIndexFromObj(interp, objv[2], pushCmdNames, "what", 0, &what) != TCL_OK) {
	    return TCL_ERROR;
	}
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	switch((enum pushCmdIndex)what) {
	case TM_PUSH_CMD_CLIP:  DrawPopClipPath(wandPtr);       break;
	case TM_PUSH_CMD_DEFS:  DrawPopDefs(wandPtr);           break;
	case TM_PUSH_CMD_GRAPH: DrawPopGraphicContext(wandPtr); break;
	case TM_PUSH_CMD_PAT:   DrawPopPattern(wandPtr);        break;
	} /* switch(what) */
	break;
    }

    /****************************************************************
     * PATH COMMANDS
     ****************************************************************/
    case TM_PATH:   /* path -opt|cmd ?args? -opt|cmd ?args? */
    {
	static CONST char *pathCmdNames[] = {
	    "#",        "-relative",  "-smooth",
	    "close",    "cubic",      "elliptic",
	    "finish",   "horizontal", "line",
	    "move",     "quadratic",  "start",
	    "vertical",
	    (char *) NULL
	};
	enum pathCmdIndex {
	    TM_PATH_COMMENT,      TM_PATH_OPT_RELATIVE,   TM_PATH_OPT_SMOOTH,
	    TM_PATH_CMD_CLOSE,    TM_PATH_CMD_CUBIC,      TM_PATH_CMD_ELLIPTIC,
	    TM_PATH_CMD_FINISH,   TM_PATH_CMD_HORIZONTAL, TM_PATH_CMD_LINE,
	    TM_PATH_CMD_MOVE,     TM_PATH_CMD_QUADRATIC,  TM_PATH_CMD_START,
	    TM_PATH_CMD_VERTICAL,
	};
	int cmd, stat;
	int relFlag=0, smoothFlag=0;
	int argc;
	Tcl_Obj **argv;

	/*
	 * Two ways of calling '$draw path' command:
	 * 1. With many arguments:
	 *    $draw path start -relative on \
	 *         move 10 10   \
	 *         line 10 20   \
	 *         line 10 -20  \
	 *         close finish
	 * 2. With a single argument list
	 *    $draw path {
	 *         start -relative on
	 *         move 10 10
	 *         line 10 20
	 *         line 10 -20
	 *         close finish
	 *    }
	 */
	if( objc == 3 ) {
	    /*
	     * Single argument list
	     */
	    if( (stat = Tcl_ListObjGetElements(interp, objv[2], &argc, &argv) != TCL_OK) ) {
		return stat;
	    }
	} else {
	    argc = objc-2;
	    argv = (Tcl_Obj **)(objv+2);
	}
	while( argc > 0 ) {
	    if (Tcl_GetIndexFromObj(interp, argv[0], pathCmdNames, "option/command", 0, &cmd) != TCL_OK) {
		return TCL_ERROR;
	    }
	    switch((enum pathCmdIndex)cmd) {

	    case TM_PATH_COMMENT:  /* # comment */
		argc -= 2, argv += 2;
		break;

	    case TM_PATH_OPT_RELATIVE:  /* -relative boolean */
		if( argc < 2 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "boolean");
		    return TCL_ERROR;
		}
		if( (stat = Tcl_GetBooleanFromObj(interp, argv[1], &relFlag)) != TCL_OK) {
		    return stat;
		}
		argc -= 2, argv += 2;
		break;

	    case TM_PATH_OPT_SMOOTH:    /* -smooth boolean */
		if( argc < 2 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "boolean");
		    return TCL_ERROR;
		}
		if( (stat = Tcl_GetBooleanFromObj(interp, argv[1], &smoothFlag)) != TCL_OK) {
		    return stat;
		}
		argc -= 2, argv += 2;
		break;

	    case TM_PATH_CMD_CLOSE:     /* close */
		DrawPathClose(wandPtr);
		argc -= 1, argv += 1;
		break;

	    case TM_PATH_CMD_CUBIC:     /* cubic ?x1 y1? x2 y2 x y */
	    {
		double x1, y1, x2, y2, x, y;

		if( smoothFlag ) {
		    /*
		     * -smooth 1 cubic x2 y2 x y
		     */
		    if( argc < 5 ) {
			Tcl_WrongNumArgs(interp, 1, argv, "x2 y2 x y");
			return TCL_ERROR;
		    }
		    if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x2)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y2)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[3], &x )) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[4], &y )) != TCL_OK) )
		    {
			Tcl_AppendResult(interp, "Smooth cubic bezier curve arguments: x2 y2 x y", NULL);
			return stat;
		    }
		    relFlag
			? DrawPathCurveToSmoothRelative(wandPtr, x2, y2, x, y)
			: DrawPathCurveToSmoothAbsolute(wandPtr, x2, y2, x, y);

		    argc -= 5, argv += 5;
		} else {
		    /*
		     * -smooth 0 cubic x1 y1 x2 y2 x y
		     */
		    if( argc < 7 ) {
			Tcl_WrongNumArgs(interp, 1, argv, "x1 y1 x2 y2 x y");
			return TCL_ERROR;
		    }
		    if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x1)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y1)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[3], &x2)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[4], &y2)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[5], &x )) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[6], &y )) != TCL_OK) )
		    {
			Tcl_AppendResult(interp, "Cubic bezier curve arguments: x1 y1 x2 y2 x y", NULL);
			return stat;
		    }
		    relFlag
			? DrawPathCurveToRelative(wandPtr, x1, y1, x2, y2, x, y)
			: DrawPathCurveToAbsolute(wandPtr, x1, y1, x2, y2, x, y);

		    argc -= 7, argv += 7;
		}
		break;
	    }

	    case TM_PATH_CMD_ELLIPTIC:  /* elliptic rx ry rotation large sweep x y */
	    {
		double      rx, ry, rot, x, y;
		int        large, sweep;

		if( argc < 8 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "rx ry rotation large sweep x y");
		    return TCL_ERROR;
		}
		if(    ((stat = Tcl_GetDoubleFromObj(interp,  argv[1], &rx )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp,  argv[2], &ry )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp,  argv[3], &rot)) != TCL_OK)
		       || ((stat = Tcl_GetBooleanFromObj(interp, argv[4], &large )) != TCL_OK)
		       || ((stat = Tcl_GetBooleanFromObj(interp, argv[5], &sweep )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp,  argv[6], &x  )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp,  argv[7], &y  )) != TCL_OK) )
		{
		    Tcl_AppendResult(interp, "Elliptic arc arguments: rx ry rotation large sweep x y", NULL);
		    return stat;
		}
		relFlag
		    ? DrawPathEllipticArcRelative(wandPtr, rx, ry, rot, large, sweep, x, y)
		    : DrawPathEllipticArcAbsolute(wandPtr, rx, ry, rot, large, sweep, x, y);

		argc -= 8, argv += 8;
		break;
	    }

	    case TM_PATH_CMD_FINISH:     /* finish */
		DrawPathFinish(wandPtr);
		argc -= 1, argv += 1;
		break;

	    case TM_PATH_CMD_HORIZONTAL:  /* horizontal x */
	    {
		double x;

		if( argc < 2 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "x");
		    return TCL_ERROR;
		}
		if( (stat = Tcl_GetDoubleFromObj(interp, argv[1], &x)) != TCL_OK) {
		    Tcl_AppendResult(interp, "Horizonal line argument: x", NULL);
		    return stat;
		}
		relFlag
		    ? DrawPathLineToHorizontalRelative(wandPtr, x)
		    : DrawPathLineToHorizontalAbsolute(wandPtr, x);

		argc -= 2, argv += 2;
		break;
	    }

	    case TM_PATH_CMD_LINE:  /* line x y */
	    {
		double x, y;

		if( argc < 3 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "x y");
		    return TCL_ERROR;
		}
		if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y )) != TCL_OK) )
		{
		    Tcl_AppendResult(interp, "Line arguments: x y", NULL);
		    return stat;
		}
		relFlag
		    ? DrawPathLineToRelative(wandPtr, x, y)
		    : DrawPathLineToAbsolute(wandPtr, x, y);

		argc -= 3, argv += 3;
		break;
	    }

	    case TM_PATH_CMD_MOVE:  /* move {x y} */
	    {
		double x, y;

		if( argc < 3 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "x y");
		    return TCL_ERROR;
		}
		if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x )) != TCL_OK)
		       || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y )) != TCL_OK) )
		{
		    Tcl_AppendResult(interp, "Line arguments: x y", NULL);
		    return stat;
		}
		relFlag
		    ? DrawPathMoveToRelative(wandPtr, x, y)
		    : DrawPathMoveToAbsolute(wandPtr, x, y);

		argc -= 3, argv += 3;
		break;
	    }

	    case TM_PATH_CMD_QUADRATIC:     /* quadratic ?x1 y1? x y */
	    {
		double x1, y1, x, y;

		if( smoothFlag ) {
		    /*
		     * -smooth 1 quadratic x y
		     */
		    if( argc < 3 ) {
			Tcl_WrongNumArgs(interp, 1, argv, "x y");
			return TCL_ERROR;
		    }
		    if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x )) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y )) != TCL_OK) )
		    {
			Tcl_AppendResult(interp, "Smooth quadratic bezier curve arguments: x y", NULL);
			return stat;
		    }
		    relFlag
			? DrawPathCurveToQuadraticBezierSmoothRelative(wandPtr, x, y)
			: DrawPathCurveToQuadraticBezierSmoothAbsolute(wandPtr, x, y);

		    argc -= 3, argv += 3;
		} else {
		    /*
		     * -smooth 0 quadratic x1 y1 x y
		     */
		    if( argc < 5 ) {
			Tcl_WrongNumArgs(interp, 1, argv, "x2 y2 x y");
			return TCL_ERROR;
		    }
		    if(    ((stat = Tcl_GetDoubleFromObj(interp, argv[1], &x1)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[2], &y1)) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[3], &x )) != TCL_OK)
			   || ((stat = Tcl_GetDoubleFromObj(interp, argv[4], &y )) != TCL_OK) )
		    {
			Tcl_AppendResult(interp, "Quadratic bezier curve arguments: x1 y1 x y", NULL);
			return stat;
		    }
		    relFlag
			? DrawPathCurveToQuadraticBezierRelative(wandPtr, x1, y1, x, y)
			: DrawPathCurveToQuadraticBezierAbsolute(wandPtr, x1, y1, x, y);

		    argc -= 5, argv += 5;
		}
		break;
	    }

	    case TM_PATH_CMD_START:     /* start */
		DrawPathStart(wandPtr);
		argc -= 1, argv += 1;
		break;

	    case TM_PATH_CMD_VERTICAL:  /* vertical {y} */
	    {
		double y;

		if( argc < 2 ) {
		    Tcl_WrongNumArgs(interp, 1, argv, "y");
		    return TCL_ERROR;
		}
		if( (stat = Tcl_GetDoubleFromObj(interp, argv[1], &y)) != TCL_OK) {
		    Tcl_AppendResult(interp, "Horizonal line argument: y", NULL);
		    return stat;
		}
		relFlag
		    ? DrawPathLineToVerticalRelative(wandPtr, y)
		    : DrawPathLineToVerticalAbsolute(wandPtr, y);

		argc -= 2, argv += 2;
		break;
	    }

	    } /* switch( path-cmd) */
	}
	break; /* path */
    }

    /*
     * All subcommands should be matched
     * A DEFAULT or TM_END_OF_TABLE would be a serious fault
     */
    case TM_END_OF_TABLE:
    default:
    {
	Tcl_AppendResult(interp, "TclMagick drawObjCmd: Unmatched subcommand table entry", NULL);
	return TCL_ERROR;
    }
    } /* switch(index) */

    return(TCL_OK);
}

/*----------------------------------------------------------------------
 *
 * Tclmagick_Init --
 *
 *      Initialize TclMagick module, called from [load TclMagick.dll]
 * Results:
 *          TCL_OK
 *          TCL_ERROR
 *
 * Side effects:
 *      Installs exit handler
 *----------------------------------------------------------------------
 */
static void tmExitHandler(
    ClientData  data )      // Tcl Interpreter which is exiting
{
    (void) data;
    if ( TM.initialized ) {
        DestroyMagick();
        TM.initialized = 0;
    }
}

EXPORT(int, Tclmagick_Init)(Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8", 0) == NULL) {
	return TCL_ERROR;
    }
#endif
    /*
     * Initialize global variables
     */
    if ( ! TM.initialized ) {
        memset(&TM, 0, sizeof(TM));
	
        /*
         * Create Exit handler, hash table
         */
        Tcl_CreateExitHandler(tmExitHandler,(int *) interp);
        Tcl_InitHashTable(&TM.hashTable, TCL_STRING_KEYS);
        InitializeMagick(Tcl_GetString(Tcl_FSGetCwd(interp)));
	
        TM.initialized = 1;
    }
    /*
     * Create commands per interpreter
     */
    Tcl_CreateObjCommand(interp, "magick",  magickCmd,  NULL, NULL);

    if ( Tcl_PkgProvide(interp,"TclMagick", VERSION) != TCL_OK ) {
        return TCL_ERROR;
    }

    return TCL_OK;
}

EXPORT(int, Tclmagick_SafeInit)(Tcl_Interp *interp)
{
    return Tclmagick_Init(interp);
}

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
