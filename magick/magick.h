/*
  ImageMagick Application Programming Interface declarations.
*/
#ifndef _MAGICK_H
#define _MAGICK_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(__hpux)
#define _HPUX_SOURCE  1
#endif

#if defined(vms)
#define _POSIX_C_SOURCE  1
#endif

/*
  System include declarations.
*/
#include <stdio.h>
#include <stdlib.h>
#if defined(_VISUALC_)
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#if !defined(__MWERKS__)
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <SIOUX.h>
#include <console.h>
#include <unix.h>
#include <types.h>
#include <stat.h>
#endif

/*
  ImageMagick include declarations.
*/
#if defined(__cplusplus) || defined(c_plusplus)
#define class  c_class
#endif

#if !defined(WIN32)
#define Export
#else
#define Export  __declspec(dllexport)
#pragma warning( disable : 4018 )
#pragma warning( disable : 4244 )
#endif

#undef index

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include "delegates.h"
#include "image.h"
#include "gems.h"
#include "compress.h"
#include "utility.h"
#include "monitor.h"
#include "error.h"
#include "X.h"
#include "widget.h"
#include "PreRvIcccm.h"
#include "memory.h"

/*
  Define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define DownShift(x) (((int) ((x)+(1L << 13))) >> 14)
#define Extent(string)  ((int) strlen(string))
#define False  0
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#if !defined(M_PI)
#define M_PI  3.14159265358979323846
#endif
#define QuantumTick(i,image) \
  (((i+1) == image->packets) || ((i % image->rows) == 0))
#define Swap(x,y) ((x)^=(y), (y)^=(x), (x)^=(y))
#if !defined(STDIN_FILENO)
#define STDIN_FILENO  0
#endif
#define True  1
#define UpShift(x) ((int) (x) << 14)
#define UpShifted(x) ((int) ((x)*(1L << 14)+0.5))

/*
  Review these definitions and change them to suit your local requirements.
*/
#define DefaultDisplayGamma  "2.2"
#define DefaultFont  "-adobe-helvetica-medium-r-*-*-14-*-*-*-*-*-iso8859-*"
#define DefaultImageQuality  "75"
#define DefaultInterlace  NoInterlace
#define DefaultPointSize  "12"
#define DefaultPreviewGeometry  "204x204+10+10"
#define DefaultPreviewMatte  "#dfdfdf"
#define DefaultPreviewPageGeometry  "3x3"
#define DefaultTextBackground  "#ffffff"
#define DefaultTextForeground  "#000000"
#define DefaultTileBackground  "#696e7e"
#define DefaultTileForeground  "#000000"
#define DefaultTileFrame  "15x15+3+3"
#define DefaultTileGeometry  "120x120+10+10>"
#define DefaultTileLabel  "%f"
#define DefaultTileMatte  "#bdbdbd"
#define DefaultTilePageGeometry  "6x4"
#define DefaultThumbnailGeometry  "106x106+0+0>"
#define DocumentationURL  \
  "http://www.wizards.dupont.com/cristy/ImageMagick.html"
#define PostscriptColorDevice  "pnmraw"
#define PostscriptMonoDevice  "pbmraw"
#define ReadBinaryType  "rb"
#define TemporaryDirectory  "/tmp"
#define UndoCache  "16"
#define WriteBinaryType  "wb"
/*
  Review these machine specific definitions.
*/
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#define ApplicationDefaults  "/usr/X11R6/lib/X11/app-defaults/"
#define EditorOptions  " -title \"Edit Image Comment\" -e vi"
#define Exit  exit
#define LaunchFormat  "pnm"
#define PreferencesDefaults  "~/."
#define PrinterFormat  "ps"
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "/usr/X11R6/lib/X11/rgb.txt"
#endif
#else
#if defined(vms)
#define ApplicationDefaults  "decw$system_defaults:"
#define EditorOptions  ""
#define Exit(errno)  errno ? exit(-1) : exit(0)
#define LaunchFormat  "psd"
#define PreferencesDefaults  "decw$user_defaults:"
#define PrinterFormat  "ps"
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "sys$common:[sysmgr]decw$rgb.dat"
#endif
#endif
#if defined(macintosh)
#define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
#define EditorOptions ""
#define HasJPEG
#define HasLZW
#define HasPNG
#define HasTIFF
#define HasZLIB
#define HasTTF
#define LaunchFormat  "psd"
#define PreferencesDefaults  "~/."
#define PrinterFormat  "ps"
#define ReadCommandlLine(argc,argv)  argc=ccommand(argv); puts(Version);
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "../xlib/lib/X11/rgb.txt"
#endif
#endif
#if defined(WIN32)
#define ApplicationDefaults  "/usr/lib/X11/app-defaults/"
#define EditorOptions ""
#define HasJBIG
#define HasJPEG
#define HasLZW
#define HasPNG
#define HasTIFF
#define HasTTF
#define HasZLIB
#define LaunchFormat  "bmp"
#undef isatty
#define isatty(filedes)  1
#define PreferencesDefaults  "~/."
#define PrinterFormat  "pcl"
#define ReadCommandlLine(argc,argv)
#if !defined(RGBColorDatabase)
#define RGBColorDatabase  "../xlib/lib/X11/rgb.txt"
#endif
#undef sleep
#define sleep(seconds)  Sleep(seconds*1000)
#endif
#endif

/*
  Page geometries:
*/
#define PCLPageGeometry  "612x792>"
#define PCLDensityGeometry  "75x75"
#define PDFPageGeometry  "612x792>"
#define PSDensityGeometry  "72x72"
#define PSPageGeometry  "612x792>"
#define TextPageGeometry  "612x792+43+43"
/*
  3D effects.
*/
#define AccentuateModulate  UpScale(80)
#define HighlightModulate  UpScale(125)
#define ShadowModulate  UpScale(135)
#define DepthModulate  UpScale(185)
#define TroughModulate  UpScale(110)

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
