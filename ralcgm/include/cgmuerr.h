/*   RAL-CGM Interpreter header file:  @(#) cgmuerr.h  version 3.3
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the author.
 * It is not in the public domain.
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the author should be marked as
 * such.
 *
 * R T Platon @ Rutherford Appleton Laboratory (rtp@uk.ac.rl.ib)
 *
 * Description:  CGM error messages and codes
 *
 * Modification log:
 *
 *   9 Sept 91 CDS  Split this file of from cgmerr.h
 */

#include "cgmerr.h"

struct errmessage{
   int number;
   Index rc;
   char  *mess;
};

struct errmessage cgmerrs[] =
{

   ERR_2MANYFILES,   RC_FATAL, "Too many metafiles in input file.",
   ERR_APNDTXT,      RC_NULL, "Append Text in wrong state",
   ERR_BADCSETS,     RC_NULL, "Character set file corrupt",
   ERR_BADDSN,       RC_NULL, "Design group file corrupt",
   ERR_BADEDGETAB,   RC_NULL, "Edge table corrupt",
   ERR_BADFAMS,      RC_NULL, "Font file corrupt",
   ERR_BADFAMSYN,    RC_NULL, "Font alias file corrupt",
   ERR_BADFSETS,     RC_NULL, "Fudge character set definition file corrupt",
   ERR_BEZDBAD,      RC_NULL, " ",
   ERR_DBUNAV,       RC_NULL, "Font database unavailable",
   ERR_BEZDIR,       RC_NULL, " ",
   ERR_FILCOR,       RC_NULL, "Bezier file corrupt",
   ERR_BEZNODEF,     RC_NULL, "No defined characters in font",
   ERR_NOFONTS,      RC_NULL, "No fonts in directory",
   ERR_BEZOPEN,      RC_NULL, " ",
   ERR_BIGCELL,      RC_NULL, "Cell Array too big",
   ERR_BIGEDGES,     RC_NULL, "Too many edges",
   ERR_BIGINTS,      RC_NULL, "Too many intersections",
   ERR_BIGPATT,      RC_NULL, "Patterns size too large",
   ERR_BIGPOINTS,    RC_NULL, "Too many points",
   ERR_BIGSTR,       RC_NULL, "String too long",
   ERR_COMPZERO,     RC_NULL, "Both character components are zero",
   ERR_CSETRANGE,    RC_NULL, "Character set index out of range",
   ERR_CSETUNKNOWN,  RC_NULL, "Character set index unknown",
   ERR_CURVE2BIG,    RC_NULL, "Expanded curve too big for system",
   ERR_DIRCELL,      RC_NULL, "Direct Colour Cell arrays not supported",
   ERR_DIRCOL,      RC_FATAL, "Direct Colour not supported",
   ERR_DIRPATT,      RC_NULL, "Direct Colour Patterns not supported",
   ERR_ELEMENT,      RC_NULL, "Unknown Element",
   ERR_ENCODING,    RC_FATAL, "Unknown CGM encoding",
   ERR_EOF,          RC_NULL, "End of File reached",
   ERR_FEWPNTS,      RC_NULL, "Too few points",
   ERR_FNTNOTOPEN,   RC_NULL, "Font file could not be opened",
   ERR_FNTNOTSUP,    RC_NULL, "Font not supported",
   ERR_FONTFILE,     RC_NULL, "Supported fonts file invalid",
   ERR_FONTRANGE,    RC_NULL, "Font index outside CGM fontlist range",
   ERR_HUFFMAN,      RC_NULL, "Huffman points list not supported",
   ERR_INTCHR,       RC_NULL, "Invalid character in integer",
   ERR_INVARG,       RC_NULL, "Invalid argument",
   ERR_INVASF,       RC_NULL, "Invalid Aspect Source flag",
   ERR_INVATT,       RC_NULL, "Invalid Attribute",
   ERR_INVCOLR,      RC_NULL, "Invalid color type",
   ERR_INVCONT,      RC_NULL, "Invalid Control Element",
   ERR_INVDELIM,     RC_NULL, "Invalid Delimiter",
   ERR_INVELEM,      RC_NULL, "Invalid CGM Element",
   ERR_INVENUM,      RC_NULL, "Invalid Enumerated type",
   ERR_INVESC,       RC_NULL, "Invalid Escape Element",
   ERR_INVMFDESC,    RC_NULL, "Invalid Metafile Descriptor Element",
   ERR_INVOPCODE,    RC_NULL, "Invalid Opcode",
   ERR_INVOPT,       RC_NULL, "Invalid Option",
   ERR_INVOUT,       RC_NULL, "Invalid Output Element",
   ERR_INVPDESC,     RC_NULL, "Invalid Picture Descriptor Element",
   ERR_INVPRIM,      RC_NULL, "Invalid Graphics Primitive",
   ERR_MANYERR,     RC_FATAL, "Too many errors",
   ERR_MAXCOLR,      RC_NULL, "Maximum Colour Index Exceeded,",
   ERR_MENUFONT,     RC_NULL, "Menu font not supported",
   ERR_METHOD,       RC_NULL, "Text method outside allowed range",
   ERR_METHRANGE,    RC_NULL, "Text method outside allowed range",
   ERR_MFOPEN,       RC_NULL, "Metafile already open",
   ERR_NAME2LONG,    RC_NULL, "Illegal length in filename",
   ERR_NOALIAS,      RC_NULL, "No alias found in file alias.psd for font",
   ERR_NOBACKSP,     RC_NULL, "This input file cannot move backwards",
   ERR_NOCLT,        RC_NULL, "File is not a clear text metafile" ,
   ERR_NOCODE,      RC_FATAL, "Unknown Output Code",
   ERR_NOCSETS,      RC_NULL, "Character set file not opened",
   ERR_NODRIVER,    RC_FATAL, "Output driver not supported",
   ERR_NODSN,        RC_NULL, "Design group file not opened",
   ERR_NOENDPIC,     RC_NULL, "Missing ENDPIC",
   ERR_NOFAMS,       RC_NULL, "Font file not opened",
   ERR_NOFAMSYN,     RC_NULL, "Font alias file not opened",
   ERR_NOFMATCH,     RC_NULL, "No trace of requested font found",
   ERR_NOFNTSPACE,   RC_NULL, "No free memory for font system element",
   ERR_NOFONT,       RC_NULL, "Unable to obtain fontlist from X server",
   ERR_NOFILE,      RC_FATAL, "No file name specified",
   ERR_CELL2SMALL,   RC_NULL, "Cell array cells to small for this device",
   ERR_ELMNOTSUP,    RC_NULL, "Element not supported",
   ERR_BADPENDEF,    RC_NULL, "Bad HPGL pen definition",
   ERR_PICLGRPAP,   RC_FATAL, "Metric picture lager than page",
   ERR_NOFSETS,      RC_NULL, "Fudge character set definition file not opened",
   ERR_NOGDP,        RC_NULL, "Unknown GDP",
   ERR_NOMEMORY,    RC_FATAL, "Out of memory",
   ERR_NOMEMPSL,     RC_NULL, "Out of memory for Polygon Section List",
   ERR_NOMETHODS,    RC_NULL, "No text emulation methods supplied",
   ERR_NOPROLG,      RC_NULL, "PostScript prologue file not found",
   ERR_NOREQFONTS,   RC_NULL, "Cannot open requestable fonts list",
   ERR_NOTAPNDTXT,   RC_NULL, "Text not allowed in Append Text State",
   ERR_NOTCGM,      RC_FATAL, "Not a recognisable CGM",
   ERR_NOTERM,       RC_NULL, "Terminal address must be specified",
   ERR_NPCINSTR,     RC_NULL, "Non-printable character in string",
   ERR_OPENFILE,   RC_FILERR, "Cannot open file",
   ERR_PACKMISSING,  RC_NULL, "Fonts specified but not found for package",
   ERR_PATINDEX,     RC_NULL, "Pattern Index too high",
   ERR_PATTABLE,     RC_NULL, "Pattern Table Full",
   ERR_PICOPEN,      RC_NULL, "Picture already open",
   ERR_RALGKS111,    RC_NULL, "RAL-GKS 1.11 CGM has invalid structure",
   ERR_RANDOM,      RC_FATAL, "Random frame access structure uninitialised",
   ERR_REDIRECT,   RC_FILERR, "Redirected input invalid for this encoding",
   ERR_SEEKEOF,      RC_NULL, "Seeking past end of file",
   ERR_STATE,        RC_NULL, "Element in wrong CGM state",
   ERR_TXTSYSNFND,   RC_NULL, "Did not find a valid text system",
   ERR_VERSION,     RC_FATAL, "Wrong Version number",
   ERR_XFONTLIST,    RC_NULL, "Inconsistency in X font list string",
   ERR_XSALIAS,      RC_NULL, "Too many aliases",
   ERR_XSDESIGN,     RC_NULL, "Too many design codes",
   ERR_XSFAMILY,     RC_NULL, "Too many type families",
   ERR_XSFPERM,      RC_NULL, "Too many fonts per implementation method for",
   ERR_XSISOCSETS,   RC_NULL, "Too many ISO character sets",
   ERR_XSISOFONTS,   RC_NULL, "Too many ISO fonts",
   ERR_XOPENDISP,   RC_FATAL, "Cannot open display",
   ERR_XSRFONTS,     RC_NULL, "Too many requestable fonts",
   ERR_XSSFONTS,     RC_NULL, "Too many special fonts",
   ERR_XSSOURCE,     RC_NULL, "Too many source codes",

/* NOTE: This code must be last */

   ERR_NOTFOUND,     RC_NULL, "Unknown error message"
};
