/*  RAL-CGM Interpreter header file: @(#) cgmatt.h  version 3.1
 *
 * Copyright (C) Rutherford Appleton Laboratory 1990, All Rights Reserved.
 *
 * This source may be copied, distributed, altered or used, but not sold
 * for profit or incorporated into a product except under licence from
 * the authors.
 *
 * It is not in the public domain.
 *
 * This notice should remain in the source unaltered, and any changes to
 * the source made by persons other than the authors should be marked as
 * such.
 *
 * Description: CGM output driver structures related to attributes
 *
 * Modification log:
 *
 *   2 Aug 91 RTP  Add SCCS id
 */

#ifndef CGMATT_H
#define CGMATT_H
#ifdef CGMATT_C
#define Extern
#else
#define Extern extern
#endif

     /*  Include the files on which this is dependent.  */

#include "cgmtypes.h"

struct sharedatt {
     Index       auxindex;
     RGBcolour   aux;
     Enum        colmode,
                 transparency;
     float       xgrain,
                 ygrain;
};

typedef struct sharedatt Sharedatt;

struct lineatt {
     Index       type;
     float       width;
     Enum        widthmode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct lineatt Lineatt;

struct markeratt {
     Index       type;
     float       size;
     Enum        sizemode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct markeratt Markeratt;

struct textatt {
     int         fontcount;             /*  environmental variables  */
     char      **fontlist;
     int         csetcount;
     char      **csetlist;
     Enum        csetannouncer;

     Index       fontindex;             /*  bundleable attributes  */
     Enum        precision;
     float       expansion;
     float       spacing;
     Index       colindex;
     RGBcolour   col;

     float       height;                /*  device-independent variables  */
     float       xup,
                 yup,
                 xbase,
                 ybase;
     Enum        path;
     Enum        halign,
                 valign;
     float       hcont,
                 vcont;
     Index       csindex,
                 acsindex;
     Sharedatt  *shared;
};

typedef struct textatt Textatt;

struct fillatt {
     Index       style;
     Index       colindex;
     RGBcolour   col;
     Index       hatch,
                 pattern;
     Sharedatt  *shared;
};

typedef struct fillatt Fillatt;

struct edgeatt {
     Index       type;
     float       width;
     Enum        widthmode;
     Index       colindex;
     RGBcolour   col;
     Sharedatt  *shared;
};

typedef struct edgeatt Edgeatt;

struct linebundle {
     Index   type;
     float   width;
     Index   colindex;
     Colourentry col;
};

typedef struct linebundle Linebundle;

struct markerbundle {
     Index   type;
     float   size;
     Index   colindex;
     Colourentry col;
};

typedef struct markerbundle Markerbundle;

struct textbundle {
     Index   font;
     Enum    precision;
     float   expansion;
     float   spacing;
     Index   colindex;
     Colourentry col;
};

typedef struct textbundle Textbundle;

struct fillbundle {
     Index   style;
     Index   pattern;
     Index   hatch;
     Index   colindex;
     Colourentry col;
};

typedef struct fillbundle Fillbundle;

struct edgebundle {
     Index   type;
     float   width;
     Index   colindex;
     Colourentry col;
};

typedef struct edgebundle Edgebundle;

#ifdef PROTO
Extern void ATTline(int, struct linebundle *,
                    int, struct lineatt *);
Extern void ATTmarker(int, struct markerbundle *,
                    int, struct markeratt *);
Extern void ATTtext(int, struct textbundle *,
                    int, struct textatt *);
Extern void ATTfill(int, struct fillbundle *,
                    int, struct fillatt *);
Extern void ATTedge(int, struct edgebundle *,
                    int, struct edgeatt *);
#else
Extern void ATTline();
Extern void ATTmarker();
Extern void ATTtext();
Extern void ATTfill();
Extern void ATTedge();
#endif         /*  ANSI block  */

#undef Extern

#endif   /*  end of cgmatt.h */
