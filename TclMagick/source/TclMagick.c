/**********************************************************************
 * Tcl Image Magick - a Tcl-only wrapper of imageMagick's Wand-API
 **********************************************************************
 * For a brief command description see "TclMagick.txt"
 *
 * For a list of changes see "Changes.txt"
 **********************************************************************/

/* $Id$ */

#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define USE_TCL_STUBS

#include "TclMagick.h"
#include <wand/magick_wand.h>

#define TCLMAGICK_VERSION_STR "0.2"
#define TCLMAGICK_VERSION_HI  0
#define TCLMAGICK_VERSION_LO  2

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


static Tcl_ObjCmdProc    magickCmd;
static Tcl_CmdDeleteProc magickObjDeleteCmd;
static Tcl_ObjCmdProc    wandObjCmd;
static Tcl_ObjCmdProc    pixelObjCmd;
static Tcl_ObjCmdProc    drawObjCmd;

/*----------------------------------------------------------------------
 * Return ImageMagick error description as a TCL result
 *----------------------------------------------------------------------
 */
static int myMagickError(Tcl_Interp  *interp, MagickWand *wandPtr )
{
    char *description;

    ExceptionType severity;
    char msg[40];

    description = MagickGetException(wandPtr, &severity);
    if( (description == NULL) || (strlen(description) == 0) ) {
        Tcl_AppendResult(interp, "ImageMagick: Unknown error", NULL);
    } else {
        sprintf(msg, "ImageMagick #%d:", severity);
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

/*----------------------------------------------------------------------
 * Return the name of a TclMagickObj
 *----------------------------------------------------------------------
 */
char *getMagickObjName(TclMagickObj *mPtr)
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
            break;
        case TM_TYPE_DRAWING:
            DestroyDrawingWand(mPtr->wandPtr);
            break;
        case TM_TYPE_PIXEL:
            DestroyPixelWand(mPtr->wandPtr);
            break;
        }
    }
    if( mPtr->hashPtr != NULL ) {
        Tcl_DeleteHashEntry(mPtr->hashPtr);
    }
    ckfree((char *)mPtr);
}

/*----------------------------------------------------------------------
 * Create TclMagick objects
 *----------------------------------------------------------------------
 */
TclMagickObj *newMagickObj(Tcl_Interp  *interp, int type, void *wandPtr, char *name)
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
static char *newDrawingObj(Tcl_Interp  *interp, DrawingWand *wandPtr, char *name)
{
    TclMagickObj *mPtr = newMagickObj(interp, TM_TYPE_DRAWING, wandPtr, name);
    return getMagickObjName(mPtr);
}
static char *newPixelObj(Tcl_Interp  *interp, PixelWand *wandPtr, char *name)
{
    TclMagickObj *mPtr = newMagickObj(interp, TM_TYPE_PIXEL, wandPtr, name);
    return getMagickObjName(mPtr);
}

/*----------------------------------------------------------------------
 * Find TclMagick objects
 *----------------------------------------------------------------------
 */
static TclMagickObj *findMagickObj(Tcl_Interp *interp, int type, char *name)
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
 * magick command
 *----------------------------------------------------------------------
 *
 * magick create type ?name?
 * magick delete name name ...
 * magick names ?type?
 * magick type name
 * magick types
 *
 *----------------------------------------------------------------------
 */

static int magickCmd(
    ClientData  clientData,         /* Unused */
    Tcl_Interp  *interp,            /* Interpreter containing image */
    int         objc,               /* Number of arguments */
    Tcl_Obj     *CONST objv[] )     /* List of arguments */
{
    int index;

    static CONST char *subCmds[] = {
        "create", "delete", "names", "type", "types",
        (char *) NULL};
    enum subIndex {
        TM_CREATE, TM_DELETE, TM_NAMES, TM_TYPE, TM_TYPES
    };

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
        "addnoise",         "AddNoiseImage",    
        "affinetransform",  "AffineTransformImage",
        "annotate",         "AnnotateImage",
        "blackthreshold",   "BlackThresholdImage",
        "blur",             "BlurImage",
        "border",           "BorderImage",
        "channel",          "ChannelImage",     
        "charcoal",         "CharcoalImage",
        "chop",             "ChopImage",        
        "clip",             "ClipImage",        
        "clone",            "CloneWand",
        "coalesce",         "CoalesceImages",
        "colorfloodfill",   "ColorFloodfillImage",        
        "colorize",         "ColorizeImage",        
        "comment",          "CommentImage",     
        "compare",          "CompareImage",
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
        "filename",         "GetFilename",          "SetFilename",
        "imagefilename",    "GetImageFilename",     "SetImageFilename",
        "frame",            "FrameImage",
        "gamma",            "GammaImage",
        "pixels",           "GetImagePixels",       "SetImagePixels",
        "signature",        "GetImageSignature",
        "number",           "GetNumberOfImages",
        "size",             "GetSize",              "SetSize",
	"height",            "width",
        "implode",          "ImplodeImage",
        "label",            "LabelImage",
        "level",            "LevelImage",
        "magnify",          "MagnifyImage",
        "map",              "MapImage",
        "mattefloodfill",   "MatteFloodfillImage",        
        "medianfilter",     "MedianFilterImage",        
        "minify",           "MinifyImage",
        "modulate",         "ModulateImage",
        "motionblur",       "MotionBlurImage",
        "negate",           "NegateImage",
        "next",             "NextImage",
        "normalize",        "NormalizeImage",
        "oilpaint",          "OilPaintImage",
        "opaque",            "OpaqueImage",
        "previous",         "PreviousImage",
        "profile",          "ProfileImage",
        "quantize",         "QuantizeImage",        "QuantizeImages",
        "seek",             "SetImage",
        "raise",            "RaiseImage",        
        "read",             "ReadImage",        
        "reducenoise",      "ReduceNoiseImage",        
        "remove",           "RemoveImage",
        "resample",         "ResampleImage",      
        "resize",           "ResizeImage",      
        "roll",             "RollImage",      
        "rotate",           "RotateImage",
        "sample",           "SampleImage",
        "scale",            "ScaleImage",
        "fuzzycolor",       "SetFuzzyColorDistance",
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
        "transparent",      "TransparentImage",
        "trim",             "TrimImage",
        "unsharpmask",      "UnsharpMaskImage",
        "wave",             "WaveImage",
        "whitethreshold",   "WhiteThresholdImage",
        "write",            "WriteImage",           "WriteImages",
        (char *) NULL
    };
    enum subIndex {
        TM_ADAPTIVE,        TM_ADAPTIVE_THRESHOLD_IMAGE,
        TM_ADDNOISE,        TM_ADD_NOISE_IMAGE, 
        TM_AFFINE_TRANSFORM,TM_AFFINE_TRANSFORM_IMAGE,
        TM_ANNOTATE,        TM_ANNOTATE_IMAGE,
        TM_BLACK_THRESHOLD, TM_BLACK_THRESHOLD_IMAGE,
        TM_BLUR,            TM_BLUR_IMAGE,
        TM_BORDER,          TM_BORDER_IMAGE,
        TM_CHANNEL,         TM_CHANNEL_IMAGE, 
        TM_CHARCOAL,        TM_CHARCOAL_IMAGE,
        TM_CHOP,            TM_CHOP_IMAGE, 
        TM_CLIP,            TM_CLIP_IMAGE, 
        TM_CLONE,           TM_CLONE_WAND,
        TM_COALESCE,        TM_COALESCE_IMAGES,
        TM_COLOR_FLOODFILL, TM_COLOR_FLOODFILL_IMAGE, 
        TM_COLORIZE,        TM_COLORIZE_IMAGE, 
        TM_COMMENT,         TM_COMMENT_IMAGE,
        TM_COMPARE,         TM_COMPARE_IMAGE,
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
        TM_FILENAME,        TM_GET_FILENAME,        TM_SET_FILENAME, 
        TM_IMAGE_FILENAME,  TM_GET_IMAGE_FILENAME,  TM_SET_IMAGE_FILENAME, 
        TM_FRAME,           TM_FRAME_IMAGE,
        TM_GAMMA,           TM_GAMMA_IMAGE,
        TM_PIXELS,          TM_GET_IMAGE_PIXELS,    TM_SET_IMAGE_PIXELS,
        TM_SIGNATURE,       TM_GET_IMAGE_SIGNATURE,
        TM_NUMBER,          TM_GET_NUMBER_OF_IMAGES, 
        TM_SIZE,            TM_GET_SIZE,            TM_SET_SIZE,
	TM_HEIGHT,          TM_WIDTH,
        TM_IMPLODE,         TM_IMPLODE_IMAGE,
        TM_LABEL,           TM_LABEL_IMAGE,
        TM_LEVEL,           TM_LEVEL_IMAGE,
        TM_MAGNIFY,         TM_MAGNIFY_IMAGE,
        TM_MAP,             TM_MAP_IMAGE,
        TM_MATTE_FLOODFILL, TM_MATTE_FLOODFILL_IMAGE, 
        TM_MEDIANFILTER,    TM_MEDIANFILTER_IMAGE, 
        TM_MINIFY,          TM_MINIFY_IMAGE,
        TM_MODULATE,        TM_MODULATE_IMAGE,
        TM_MOTIONBLUR,      TM_MOTIONBLUR_IMAGE,
        TM_NEGATE,          TM_NEGATE_IMAGE,
        TM_NEXT,            TM_NEXT_IMAGE,
        TM_NORMALIZE,       TM_NORMALIZE_IMAGE,
        TM_OILPAINT,        TM_OILPAINT_IMAGE,
        TM_OPAQUE,          TM_OPAQUE_IMAGE,
        TM_PREVIOUS,        TM_PREVIOUS_IMAGE,
        TM_PROFILE,         TM_PROFILE_IMAGE,
        TM_QUANTIZE,        TM_QUANTIZE_IMAGE,      TM_QUANTIZE_IMAGES,
        TM_SEEK,            TM_SET_IMAGE,
        TM_RAISE,           TM_RAISE_IMAGE, 
        TM_READ,            TM_READ_IMAGE, 
        TM_REDUCENOISE,     TM_REDUCENOISE_IMAGE, 
        TM_REMOVE,          TM_REMOVE_IMAGE, 
        TM_RESAMPLE,        TM_RESAMPLE_IMAGE, 
        TM_RESIZE,          TM_RESIZE_IMAGE, 
        TM_ROLL,            TM_ROLL_IMAGE, 
        TM_ROTATE,          TM_ROTATE_IMAGE, 
        TM_SAMPLE,          TM_SAMPLE_IMAGE, 
        TM_SCALE,           TM_SCALE_IMAGE, 
        TM_FUZZY_COLOR,     TM_SET_FUZZY_COLOR_DISTANCE,
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
        TM_TRANSPARENT,     TM_TRANSPARENT_IMAGE, 
        TM_TRIM,            TM_TRIM_IMAGE, 
        TM_UNSHARPMASK,     TM_UNSHARPMASK_IMAGE, 
        TM_WAVE,            TM_WAVE_IMAGE, 
        TM_WHITE_THRESHOLD, TM_WHITE_THRESHOLD_IMAGE,
        TM_WRITE,           TM_WRITE_IMAGE,         TM_WRITE_IMAGES,
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

    int index, stat, result;
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
	int width=1, height=1, ofs=0;

	if( (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?width? ?height? ?ofs?");
	    return TCL_ERROR;
	}
	if( (objc > 2) && ((stat = Tcl_GetIntFromObj(interp, objv[2], &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], &height)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &ofs)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickAdaptiveThresholdImage(wandPtr, width, height, ofs);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_ADDNOISE:          /* addnoise ?noiseType? */
    case TM_ADD_NOISE_IMAGE:   /* AddNoiseImage ?noiseType? */
    {
	static CONST char *noiseNames[] = {
	    "uniform", "gaussian", "multiplicativegaussian", "impulse", "laplacian", "poisson",
	    (char *) NULL
	};
	static NoiseType noiseTypes[] = {
	    UniformNoise, GaussianNoise, MultiplicativeGaussianNoise, ImpulseNoise, LaplacianNoise, PoissonNoise
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

    case TM_ANNOTATE:       /* annotate draw ?x y? ?txt? */
    case TM_ANNOTATE_IMAGE: /* AnnotateImage draw ?x y? ?txt? */
        {
	    char	*name;
	    DrawingWand	*drawPtr;
            double      x=0, y=0;
            char        *txt="";

            if( (objc < 3) || (objc > 6) || (objc == 4) ) {
                Tcl_WrongNumArgs(interp, 2, objv, "draw ?x=0 y=0? ?txt=\"\"?");
                return TCL_ERROR;
            }
            name = Tcl_GetString(objv[2]);
            if( (drawPtr = findDrawingWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
            if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &x)) != TCL_OK) ) {
                return stat;
            }
            if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &y)) != TCL_OK) ) {
                return stat;
            }
            if( objc > 5 ) {
                name = Tcl_GetString(objv[5]);
            }

            result = MagickAnnotateImage(wandPtr, drawPtr, x, y, txt);
            if (!result) {
                return myMagickError(interp, wandPtr);
            }
            break;
        }

    case TM_BLACK_THRESHOLD:        /* blackthreshold thresholdPixel */
    case TM_BLACK_THRESHOLD_IMAGE:  /* BlackThresholdImage thresholdPixel */
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
	int	        width=1, height=1;
	char        *name;
	PixelWand	*borderPtr;

	if( (objc < 3) || (objc > 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "borderPixel ?width=1? ?height=width?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (borderPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &height)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickBorderImage(wandPtr, borderPtr, width, height);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_CHANNEL:       /* channel channelType */
    case TM_CHANNEL_IMAGE: /* ChannelImage channelType */
    {
	static CONST char *chanNames[] = {
	    "undefined", "red",    "cyan",    "green", "magenta",
	    "blue",      "yellow", "opacity", "black", "matte",
	    (char *) NULL
	};
	static ChannelType chanTypes[] = {
	    UndefinedChannel, RedChannel,    CyanChannel,    GreenChannel, MagentaChannel,
	    BlueChannel,      YellowChannel, OpacityChannel, BlackChannel, MatteChannel
	};
	int chanIdx;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "channelType");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], chanNames, "channelType", 0, &chanIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	result = MagickChannelImage(wandPtr, chanTypes[chanIdx]);
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
	int width, height, x=0, y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &y)) != TCL_OK) ) {
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

#if 0

    case TM_CLONE:        /* clone ?newName? */
    case TM_CLONE_WAND:   /* CloneWand ?newName? */
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
	newWand = MagickCloneWand(wandPtr);
	if (newWand == NULL) {
	    return myMagickError(interp, wandPtr);
	}
	name = newWandObj(interp, newWand, name);
	Tcl_SetResult(interp, name, TCL_VOLATILE);

	break;
    }
#endif

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
    {
	int		x=0, y=0;
	double      fuzz=0.0;
	char	*name;
	PixelWand	*fillPtr, *borderPtr;

	if( (objc < 3) || (objc > 7) || (objc == 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "fillPixel ?fuzz=0.0? ?borderPix=fillPix? ?x=0 y=0?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (fillPtr = findPixelWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	borderPtr = fillPtr; /* default */
	if( (objc > 3) && ((stat = Tcl_GetDoubleFromObj(interp, objv[3], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	if( objc > 4 ) {
	    name = Tcl_GetString(objv[4]);
	    if( (borderPtr = findPixelWand(interp, name)) == NULL ) {
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

    case TM_COMPARE:        /* compare refName */
    case TM_COMPARE_IMAGE:  /* CompareImage refName */
    {
	MagickWand      *refWand;
	char            *name;
	double          pixelErr, normErr, maxErr;
	Tcl_Obj         *listPtr;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "refName");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( (refWand = findMagickWand(interp, name)) == NULL ) {
	    return TCL_ERROR;
	}
	/*
	 * Unlike other API's MagickCompareImage() result=1 is not an error
	 * result: 1=equal 0=different
	 */
	result = MagickCompareImage( wandPtr, refWand, &pixelErr, &normErr, &maxErr );

	listPtr = Tcl_NewListObj(0, NULL);
	Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(result));
	Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(pixelErr));
	Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(normErr));
	Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(maxErr));
	Tcl_SetObjResult(interp, listPtr);

	break;
    }

    case TM_COMPOSITE:          /* composite opType compWand ?x y? */
    case TM_COMPOSITE_IMAGE:    /* CompositeImage opType compWand ?x y? */
    {
	static CONST char *opNames[] = {
	    "undefined", "over",        "in",         "out",
	    "atop",      "xor",         "plus",       "minus",
	    "add",       "subtract",    "difference", "multiply",
	    "bumpmap",   "copy",        "copyred",    "copygreen", 
	    "copyblue",  "copyopacity", "clear",      "dissolve"
	    "displace",  "modulate",    "threshold",  "no", 
	    "darken",    "lighten",     "hue",        "saturate",
	    "colorize",  "luminize",    "screen",     "overlay",  
	    "copycyan",  "copymagenta", "copyyellow", "copyBlack",
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
	    CopyCyanCompositeOp,  CopyMagentaCompositeOp, CopyYellowCompositeOp, CopyBlackCompositeOp
	};

	int opIdx;

	char	*name;
	MagickWand	*compPtr;
	int x=0;
	int y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "opType compWand ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if (Tcl_GetIndexFromObj(interp, objv[2], opNames, "opType", 0, &opIdx) != TCL_OK) {
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[3]);
	if( (compPtr = findMagickWand(interp, name)) == NULL ) {
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
	int     i, order, listLen;
	double  *kernel;
	Tcl_Obj **listPtr;
           
	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "order kernelList");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &order)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_ListObjGetElements(interp, objv[3], &listLen, &listPtr)) != TCL_OK) {
	    return stat;
	}
	if( listLen != order * order ) {
	    Tcl_AppendResult(interp, "ConvolveImage: Invalid kernelList length, should be = (order x order)", NULL);
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
	int width, height, x=0, y=0;

	if( (objc < 4) || (objc > 6) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &y)) != TCL_OK) ) {
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
	txt = (char *)MagickDescribeImage(wandPtr);
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
	    filename = Tcl_GetString(objv[2]);
	    MagickSetFilename(wandPtr, filename);
	} else {    /* Get filename */
	    filename = (char *)MagickGetFilename(wandPtr);
	    if(filename != NULL) {
		Tcl_SetResult(interp, filename, TCL_VOLATILE);
		MagickRelinquishMemory(filename); /* Free TclMagick resource */
	    }
	}
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
	    filename = Tcl_GetString(objv[2]);
	    result = MagickSetImageFilename(wandPtr, filename);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	} else {    /* Get filename */
	    filename = (char *)MagickGetImageFilename(wandPtr);
	    if(filename != NULL) {
		Tcl_SetResult(interp, filename, TCL_VOLATILE);
		MagickRelinquishMemory(filename); /* Free TclMagick resource */
	    }
	}
	break;
    }

    case TM_FRAME:          /* frame framePixel ?width? ?height? ?inner? ?outer? */
    case TM_FRAME_IMAGE:    /* FrameImage framePixel ?width? ?height? ?inner? ?outer? */
    {
	int	        width=1, height=1, inner=0, outer=0;
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
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], &width)) != TCL_OK) ) {
	    return stat;
	}
	height = width; /* default */
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &height)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &inner)) != TCL_OK) ) {
	    return stat;
	}
	outer = inner; /* default */
	if( (objc > 6) && ((stat = Tcl_GetIntFromObj(interp, objv[6], &outer)) != TCL_OK) ) {
	    return stat;
	}

	result = MagickFrameImage(wandPtr, framePtr, width, height, inner, outer);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

    case TM_GAMMA:       /* gamma levelPixel */
    case TM_GAMMA_IMAGE: /* GammaImage levelPixel */
    {
	double gamma;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "gamma");
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

    case TM_PIXELS:             /* pixels x0 y0 cols rows map storageType ?data? */
    case TM_GET_IMAGE_PIXELS:   /* GetImagePixels x0 y0 cols rows map storageType */
    case TM_SET_IMAGE_PIXELS:   /* GetImagePixels x0 y0 cols rows map storageType data */
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
	int  storIdx, x0, y0, cols, rows, size, len;
	char *map;
	unsigned char *pixels;

	if( ((enum subIndex)index == TM_PIXELS) && ((objc < 8) || (objc > 9)) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType ?data?");
	    return TCL_ERROR;
	} 
	if( ((enum subIndex)index == TM_GET_IMAGE_PIXELS) && (objc != 8) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType");
	    return TCL_ERROR;
	}
	if( ((enum subIndex)index == TM_SET_IMAGE_PIXELS) && (objc != 9) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x0 y0 cols rows map storageType data");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x0)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y0)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[4], &cols)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[5], &rows)) != TCL_OK ) {
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
	    pixels = ckalloc(size);
	    if( pixels == NULL ) {
		Tcl_AppendResult(interp, "TclMagick: out of memory", NULL);
		return TCL_ERROR;
	    }

	    result = MagickGetImagePixels(wandPtr, x0, y0, cols, rows, map, storTypes[storIdx], pixels);
	    if (!result) {
		ckfree(pixels);            
		return myMagickError(interp, wandPtr);
	    }

	    /*
	     * Return ByteArray object
	     */
	    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(pixels, size));
	    ckfree(pixels);
	} else {
	    pixels = Tcl_GetByteArrayFromObj( objv[8], &len);
	    if( len < size ) {
		Tcl_AppendResult(interp, "TclMagick: not enough data bytes", NULL);
		return TCL_ERROR;
	    }
	    result = MagickSetImagePixels(wandPtr, x0, y0, cols, rows, map, storTypes[storIdx], pixels);
	    if (!result) {
		return myMagickError(interp, wandPtr);
	    }
	}
	break;
    }

    case TM_SIGNATURE:           /* signature */
    case TM_GET_IMAGE_SIGNATURE: /* GetImageSignature */
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

    case TM_NUMBER:                /* number */
    case TM_GET_NUMBER_OF_IMAGES:  /* GetNumberOfImages */
    {
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	result = MagickGetNumberImages(wandPtr);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_SIZE:      /* size ?x y? */
    case TM_GET_SIZE:  /* size */
    case TM_SET_SIZE:  /* size x y */
    {
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
	    int     x, y;

	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x)) != TCL_OK ) {
		return stat;
	    }
	    if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y)) != TCL_OK ) {
		return stat;
	    }
	    MagickSetSize(wandPtr, x, y);
	} else {
	    /* 
	     * Get size={xSize ySize}
	     */
	    long x, y;
	    Tcl_Obj *listPtr = Tcl_NewListObj(0, NULL);

	    MagickGetSize(wandPtr, &x, &y);
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(x));
	    Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewLongObj(y));
	    Tcl_SetObjResult(interp, listPtr);
	}
	break;
    }

    case TM_WIDTH:
    case TM_HEIGHT:
    {
	Tcl_Obj *retval = NULL;
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 1, objv, "");
	    return TCL_ERROR;
	}
	if ((enum subIndex)index == TM_WIDTH) {
	    retval = Tcl_NewLongObj(MagickGetImageWidth (wandPtr));
	} else {
	    retval = Tcl_NewLongObj(MagickGetImageHeight (wandPtr));
	}
	Tcl_SetObjResult(interp, retval);
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
	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "str");
	    return TCL_ERROR;
	}
	result = MagickLabelImage(wandPtr, Tcl_GetString(objv[2]));
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
	int	opacity, x=0, y=0;
	double  fuzz=0.0;
	char	*name;
	PixelWand	*borderPtr=NULL;

	if( (objc < 3) || (objc > 7) || (objc == 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "opacity ?fuzz=0.0? ?borderPix=none? ?x=0 y=0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &opacity)) != TCL_OK ) {
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
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 6) && ((stat = Tcl_GetIntFromObj(interp, objv[6], &y)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickMatteFloodfillImage(wandPtr, (Quantum)opacity, fuzz, borderPtr, x, y);
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
	} else if (objc == 3) {
	    if( (stat = Tcl_GetBooleanFromObj(interp, objv[2], &gray)) != TCL_OK ) {
		return stat;
	    }
	}
	result = MagickNegateImage(wandPtr, gray);
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

    case TM_PROFILE:       /* profile name ?profile? */
    case TM_PROFILE_IMAGE: /* ProfileImage name ?profile? */
    {
	char          *name;
	unsigned char *profile=NULL;
	int           length;

	if( (objc < 3) || (objc > 4) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?profile?");
	    return TCL_ERROR;
	}
	name = Tcl_GetString(objv[2]);
	if( objc > 3 ) {
	    profile = Tcl_GetByteArrayFromObj(objv[3], &length);
	}
	result = MagickProfileImage(wandPtr, name, profile, length);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }
  
    case TM_QUANTIZE:        /* quantize numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    case TM_QUANTIZE_IMAGE:  /* QuantizeImage numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    case TM_QUANTIZE_IMAGES: /* QuantizeImage numColors ?colorspaceType? ?treedepth? ?dither? ?measureError? */
    {
	static CONST char *csNames[] = {
	    "undefined", "RGB",   "GRAY",  "transparent",
	    "OHTA",      "XYZ",   "YCbCr", "YCC",
	    "YIQ",       "YPbPr", "YUV",   "CMYK",
	    "sRGB",      "HSL",   "HWB",
	    (char *) NULL
	};
	static ColorspaceType csTypes[] = {
	    UndefinedColorspace, RGBColorspace,   GRAYColorspace,  TransparentColorspace,
	    OHTAColorspace,      XYZColorspace,   YCbCrColorspace, YCCColorspace,
	    YIQColorspace,       YPbPrColorspace, YUVColorspace,   CMYKColorspace,
	    sRGBColorspace,      HSLColorspace,   HWBColorspace
	};

	int numColors, treeDepth=0, dither=0, measureError=0;
	int csIdx=RGBColorspace;

	if( (objc < 3) || (objc > 7) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "numColors ?colorspaceType=RGB? ?treedepth=0? ?dither=no? ?measureError=no?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &numColors)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 3) && (Tcl_GetIndexFromObj(interp, objv[3], csNames, "colorspaceType", 0, &csIdx) != TCL_OK) ) {
	    return TCL_ERROR;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &treeDepth)) != TCL_OK) ) {
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

    case TM_SEEK:      /* seek num */
    case TM_SET_IMAGE: /* SetImage num */
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
	Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
	break;
    }

    case TM_RAISE:        /* raise width height ?x y? ?raise? */
    case TM_RAISE_IMAGE:  /* RaiseImage width height ?x y? ?raise? */
    {
	int width, height, x=0, y=0, raise=0;

	if( (objc < 4) || (objc > 7) || (objc == 5) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "width height ?x y? ?raise=no?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &width)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &height)) != TCL_OK ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetIntFromObj(interp, objv[4], &x)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 5) && ((stat = Tcl_GetIntFromObj(interp, objv[5], &y)) != TCL_OK) ) {
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

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	filename = Tcl_GetString(objv[2]);
	result = MagickReadImage(wandPtr, filename);
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
	if( (objc > 4) && (Tcl_GetIndexFromObj(interp, objv[4], filterNames, "filterType", 0, &filterIdx) != TCL_OK) ) {
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
	int     filterIdx=0, x, y;
	double  blur=1.0;

	if( (objc < 4) || (objc > 6) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y ?filterType=undefined? ?blur=1.0?");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y)) != TCL_OK ) {
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
	int  x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y)) != TCL_OK ) {
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
	int  x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y)) != TCL_OK ) {
	    return stat;
	}
	result = MagickScaleImage(wandPtr, x, y);
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
    }

#if 0 /* FIXME - undefined symbol  */
    case TM_FUZZY_COLOR:                /* fuzzycolor distance */
    case TM_SET_FUZZY_COLOR_DISTANCE:   /* SetFuzzyColorDistance distance */
    {
	double  distance;

	if( objc != 3 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "distance");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetDoubleFromObj(interp, objv[2], &distance)) != TCL_OK ) {
	    return stat;
	}
	MagickSetFuzzyColorDistance(wandPtr, distance);
	break;
    }
#endif

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
	int  x, y;

	if( objc != 4 ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "x y");
	    return TCL_ERROR;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[2], &x)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetIntFromObj(interp, objv[3], &y)) != TCL_OK ) {
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
	if( (objc > 3) && ((stat = Tcl_GetIntFromObj(interp, objv[3], &opacity)) != TCL_OK) ) {
	    return stat;
	}
	if( (objc > 4) && ((stat = Tcl_GetDoubleFromObj(interp, objv[4], &fuzz)) != TCL_OK) ) {
	    return stat;
	}
	result = MagickTransparentImage(wandPtr, targetPtr, (Quantum)opacity, fuzz);
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

	if( ((enum subIndex)index == TM_WRITE_IMAGES) && ((objc < 3) || (objc > 4)) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename ?adjoin=no?");
	    return TCL_ERROR;
	} 
	if( ((enum subIndex)index != TM_WRITE_IMAGES) && (objc != 3) ) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}
	filename = Tcl_GetString(objv[2]);
	if( (objc > 3) && ((stat = Tcl_GetBooleanFromObj(interp, objv[3], &adjoin)) != TCL_OK) ) {
	    return stat;
	}
	if( (enum subIndex) index == TM_WRITE_IMAGES ) {
	    result = MagickWriteImages(wandPtr, filename, adjoin);
	} else {
	    result = MagickWriteImage(wandPtr, filename);
	}
	if (!result) {
	    return myMagickError(interp, wandPtr);
	}
	break;
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
        "set",          "get",
        "color",
        "SetBlue",      "GetBlue",      "SetBlueQuantum",   "GetBlueQuantum", 
        "SetGreen",     "GetGreen",     "SetGreenQuantum",  "GetGreenQuantum", 
        "SetRed",       "GetRed",       "SetRedQuantum",    "GetRedQuantum", 
        "SetOpacity",   "GetOpacity",   "SetOpacityQuantum","GetOpacityQuantum", 
        "SetColor",     "GetColor",     "GetColorAsString",
        (char *) NULL
    };
    enum subIndex {
        TM_SET,         TM_GET,
        TM_COLOR,
        TM_SET_BLUE,    TM_GET_BLUE,    TM_SET_BLUE_QUANTUM,    TM_GET_BLUE_QUANTUM,
        TM_SET_GREEN,   TM_GET_GREEN,   TM_SET_GREEN_QUANTUM,   TM_GET_GREEN_QUANTUM,
        TM_SET_RED,     TM_GET_RED,     TM_SET_RED_QUANTUM,     TM_GET_RED_QUANTUM,
        TM_SET_OPACITY, TM_GET_OPACITY, TM_SET_OPACITY_QUANTUM, TM_GET_OPACITY_QUANTUM,
        TM_SET_COLOR,   TM_GET_COLOR,   TM_GET_COLOR_AS_STRING,
        TM_END_OF_TABLE
    };
    static CONST char  *colors[]  = {"blue", "green", "red", "opacity", (char *) NULL};
    
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
	    /* char *str = Tcl_GetString(objv[0]); */
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
		case 0:  PixelSetBlueQuantum(wandPtr,    (Quantum)quantVal); break;
		case 1:  PixelSetGreenQuantum(wandPtr,   (Quantum)quantVal); break;
		case 2:  PixelSetRedQuantum(wandPtr,     (Quantum)quantVal); break;
		default: PixelSetOpacityQuantum(wandPtr, (Quantum)quantVal); break;
		}
	    } else {
		if( (stat = Tcl_GetDoubleFromObj(interp, objv[1], &normVal)) != TCL_OK ) {
		    return stat;
		}
		switch(color) {
		case 0:  PixelSetBlue(wandPtr,    normVal); break;
		case 1:  PixelSetGreen(wandPtr,   normVal); break;
		case 2:  PixelSetRed(wandPtr,     normVal); break;
		default: PixelSetOpacity(wandPtr, normVal); break;
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
		case 0:  quantVal = PixelGetBlueQuantum(wandPtr);    break;
		case 1:  quantVal = PixelGetGreenQuantum(wandPtr);   break;
		case 2:  quantVal = PixelGetRedQuantum(wandPtr);     break;
		default: quantVal = PixelGetOpacityQuantum(wandPtr); break;
		}
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewIntObj(quantVal));
	    } else {
		switch(color) {
		case 0:  normVal = PixelGetBlue(wandPtr);    break;
		case 1:  normVal = PixelGetGreen(wandPtr);   break;
		case 2:  normVal = PixelGetRed(wandPtr);     break;
		default: normVal = PixelGetOpacity(wandPtr); break;
		}
		Tcl_ListObjAppendElement(interp, listPtr, Tcl_NewDoubleObj(normVal));
	    }
	    objv += 1, objc -= 1;
	}
	Tcl_SetObjResult(interp, listPtr);

	break;
    }

    case TM_COLOR:                  /* color ?string? */
    case TM_SET_COLOR:              /* SetColor */
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
    ClientData  clientData,         /* PixelWand pointer */
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
	char *text;

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
	text = Tcl_GetString(objv[4]);
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
	int       i, num;

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
	    DrawBezier(wandPtr, num, coordinates); break;
	case TM_POLYGON:  case TM_PPOLYGON:   
	    DrawPolygon(wandPtr, num, coordinates); break;
	case TM_POLYLINE: case TM_PPOLYLINE:  
	    DrawPolyline(wandPtr, num, coordinates); break;
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
	    for (ruleIdx = 0; ruleIdx < sizeof(fillRules)/sizeof(fillRules[0]); ruleIdx++) {
		if( fillRules[ruleIdx] == rule ) {
		    Tcl_SetResult(interp, (char *)fillRuleNames[ruleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(rule));
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
	    for (unitIdx = 0; unitIdx < sizeof(unitTypes)/sizeof(unitTypes[0]); unitIdx++) {
		if( unitTypes[unitIdx] == unit ) {
		    Tcl_SetResult(interp, (char *)unitNames[unitIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(unit));
	}
	break;
    }

    case TM_FILLCOLOR:              /* fillcolor get|set pixel */
    case TM_STROKE_COLOR:           /* strokecolor get|set pixel */
    case TM_TEXT_UNDERCOLOR:        /* textundercolor get|set pixel */
        {
            static char *subCmds[] = {"get", "set", (char *) NULL};
            int         subIdx;
            PixelWand   *pixelPtr;
            char        *name;

            if( objc != 4 ) {
                Tcl_WrongNumArgs(interp, 2, objv, "get|set pixelName");
                return TCL_ERROR;
            }
            if (Tcl_GetIndexFromObj(interp, objv[2], subCmds, "subcmd", 0, &subIdx) != TCL_OK) {
                return TCL_ERROR;
            }
            name = Tcl_GetString(objv[3]);
            if( (pixelPtr = findPixelWand(interp, name)) == NULL ) {
		return TCL_ERROR;
	    }
            switch ((enum subIndex)index) {
                case TM_FILLCOLOR:
                    (subIdx == 0) ? DrawGetFillColor(wandPtr, pixelPtr) : DrawSetFillColor(wandPtr, pixelPtr);
                    break;
                case TM_STROKE_COLOR:
                    (subIdx == 0) ? DrawGetStrokeColor(wandPtr, pixelPtr) : DrawSetStrokeColor(wandPtr, pixelPtr);
                    break;
                case TM_TEXT_UNDERCOLOR:
                    (subIdx == 0) ? DrawGetTextUnderColor(wandPtr, pixelPtr) : DrawSetTextUnderColor(wandPtr, pixelPtr);
                    break;
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
	    for (ruleIdx = 0; ruleIdx < sizeof(fillRules)/sizeof(fillRules[0]); ruleIdx++) {
		if( fillRules[ruleIdx] == rule ) {
		    Tcl_SetResult(interp, (char *)fillRuleNames[ruleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(rule));
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
	    for (stretchIdx = 0; stretchIdx < sizeof(stretchTypes)/sizeof(stretchTypes[0]); stretchIdx++) {
		if( stretchTypes[stretchIdx] == stretch ) {
		    Tcl_SetResult(interp, (char *)stretchNames[stretchIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(stretch));
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
	    for (styleIdx = 0; styleIdx < sizeof(styleTypes)/sizeof(styleTypes[0]); styleIdx++) {
		if( styleTypes[styleIdx] == style ) {
		    Tcl_SetResult(interp, (char *)styleNames[styleIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(style));
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
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &value)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetFontWeight(wandPtr, value);
	} else {    /* Get font weight */
	    value = DrawGetFontWeight(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(value));
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
	    for (gravIdx = 0; gravIdx < sizeof(gravTypes)/sizeof(gravTypes[0]); gravIdx++) {
		if( gravTypes[gravIdx] == grav ) {
		    Tcl_SetResult(interp, (char *)gravNames[gravIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(grav));
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
	unsigned flag;

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
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(flag));
	}
	break;
    }

    case TM_STROKE_DASHARRAY:     /* strokedasharray {} | val val ... */
    case TM_GET_STROKE_DASHARRAY: /* GetStrokeDashArray */
    case TM_SET_STROKE_DASHARRAY: /* SetStrokeDashArray {} | val val ... */
    {
	size_t i, num;
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
	    for (capIdx = 0; capIdx < sizeof(capTypes)/sizeof(capTypes[0]); capIdx++) {
		if( capTypes[capIdx] == cap ) {
		    Tcl_SetResult(interp, (char *)capNames[capIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(cap));
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
	    for (joinIdx = 0; joinIdx < sizeof(joinTypes)/sizeof(joinTypes[0]); joinIdx++) {
		if( joinTypes[joinIdx] == join ) {
		    Tcl_SetResult(interp, (char *)joinNames[joinIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(join));
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
	    if( (stat = Tcl_GetIntFromObj(interp, objv[2], &val)) != TCL_OK ) {
		return stat;
	    }
	    DrawSetStrokeMiterLimit(wandPtr, val);
	} else {    /* Get stroke miterlimit */
	    val = DrawGetStrokeMiterLimit(wandPtr);
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
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
	unsigned flag;

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
	    for (decoIdx = 0; decoIdx < sizeof(decoTypes)/sizeof(decoTypes[0]); decoIdx++) {
		if( decoTypes[decoIdx] == deco ) {
		    Tcl_SetResult(interp, (char *)decoNames[decoIdx], TCL_VOLATILE);
		    return TCL_OK;
		}
	    }
	    Tcl_SetObjResult(interp, Tcl_NewIntObj(deco));
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
	if( (stat = Tcl_GetLongFromObj(interp, objv[2], &x1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[3], &y1)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[4], &x2)) != TCL_OK ) {
	    return stat;
	}
	if( (stat = Tcl_GetLongFromObj(interp, objv[5], &y2)) != TCL_OK ) {
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
		unsigned    large, sweep;

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
    if ( TM.initialized ) {
        DestroyMagick();
        TM.initialized = 0;
    }
}

EXPORT(int, Tclmagick_Init)(Tcl_Interp *interp)
{
    if (Tcl_InitStubs(interp, "8", 0) == NULL) {
	return TCL_ERROR;
    }
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

    if ( Tcl_PkgProvide(interp,"TclMagick", TCLMAGICK_VERSION_STR) != TCL_OK ) {
        return TCL_ERROR;
    }

    return TCL_OK;
}

EXPORT(int, Tclmagick_SafeInit)(Tcl_Interp *interp)
{
    return Tclmagick_Init(interp);
}
