/* $Header$ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define	_TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"
#include "tiffvers.h"

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef	struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * to pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double) and because we defined pseudo-tags that are
 *     outside the range of legal Aldus-assigned tags.
 * NB: tsize_t is int32 and not uint32 because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets being the most important, and to ensure
 *     that it is unsigned, rather than signed.
 */
typedef	uint32 ttag_t;		/* directory tag */
typedef	uint16 tdir_t;		/* directory index */
typedef	uint16 tsample_t;	/* sample number */
typedef	uint32 tstrip_t;	/* strip number */
typedef uint32 ttile_t;		/* tile number */
typedef	int32 tsize_t;		/* i/o size in bytes */
typedef	void* tdata_t;		/* image data ref */
typedef	uint32 toff_t;		/* file offset */

#if !defined(__WIN32__) && (defined(_WIN32) || defined(WIN32))
#define __WIN32__
#endif

/*
 * On windows you should define USE_WIN32_FILEIO if you are using tif_win32.c
 * or AVOID_WIN32_FILEIO if you are using something else (like tif_unix.c).
 *
 * By default tif_win32.c is assumed on windows if not using the cygwin
 * environment.
 */

#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#  if !defined(__CYGWIN) && !defined(AVOID_WIN32_FILEIO) && !defined(USE_WIN32_FILIO)
#    define USE_WIN32_FILEIO
#  endif
#endif

#if defined(USE_WIN32_FILEIO)
#include <windows.h>
#ifdef __WIN32__
DECLARE_HANDLE(thandle_t);	/* Win32 file handle */
#else
typedef	HFILE thandle_t;	/* client data handle */
#endif
#else
typedef	void* thandle_t;	/* client data handle */
#endif

#ifndef NULL
#define	NULL	0
#endif

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define	TIFFPRINT_NONE		0x0		/* no extra info */
#define	TIFFPRINT_STRIPS	0x1		/* strips/tiles info */
#define	TIFFPRINT_CURVES	0x2		/* color/gray response curves */
#define	TIFFPRINT_COLORMAP	0x4		/* colormap */
#define	TIFFPRINT_JPEGQTABLES	0x100		/* JPEG Q matrices */
#define	TIFFPRINT_JPEGACTABLES	0x200		/* JPEG AC tables */
#define	TIFFPRINT_JPEGDCTABLES	0x200		/* JPEG DC tables */

/*
 * RGBA-style image support.
 */
typedef	unsigned char TIFFRGBValue;		/* 8-bit samples */
typedef struct _TIFFRGBAImage TIFFRGBAImage;
/*
 * The image reading and conversion routines invoke
 * ``put routines'' to copy/image/whatever tiles of
 * raw image data.  A default set of routines are 
 * provided to convert/copy raw image data to 8-bit
 * packed ABGR format rasters.  Applications can supply
 * alternate routines that unpack the data into a
 * different format or, for example, unpack the data
 * and draw the unpacked raster on the display.
 */
typedef void (*tileContigRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*);
typedef void (*tileSeparateRoutine)
    (TIFFRGBAImage*, uint32*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*, unsigned char*, unsigned char*, unsigned char*);
/*
 * RGBA-reader state.
 */
typedef struct {				/* YCbCr->RGB support */
	TIFFRGBValue* clamptab;			/* range clamping table */
	int*	Cr_r_tab;
	int*	Cb_b_tab;
	int32*	Cr_g_tab;
	int32*	Cb_g_tab;
	float	coeffs[3];			/* cached for repeated use */
} TIFFYCbCrToRGB;

struct _TIFFRGBAImage {
	TIFF*	tif;				/* image handle */
	int	stoponerr;			/* stop on read error */
	int	isContig;			/* data is packed/separate */
	int	alpha;				/* type of alpha data present */
	uint32	width;				/* image width */
	uint32	height;				/* image height */
	uint16	bitspersample;			/* image bits/sample */
	uint16	samplesperpixel;		/* image samples/pixel */
	uint16	orientation;			/* image orientation */
	uint16	photometric;			/* image photometric interp */
	uint16*	redcmap;			/* colormap pallete */
	uint16*	greencmap;
	uint16*	bluecmap;
						/* get image data routine */
	int	(*get)(TIFFRGBAImage*, uint32*, uint32, uint32);
	union {
	    void (*any)(TIFFRGBAImage*);
	    tileContigRoutine	contig;
	    tileSeparateRoutine	separate;
	} put;					/* put decoded strip/tile */
	TIFFRGBValue* Map;			/* sample mapping array */
	uint32** BWmap;				/* black&white map */
	uint32** PALmap;			/* palette image map */
	TIFFYCbCrToRGB* ycbcr;			/* YCbCr conversion state */

        int	row_offset;
        int     col_offset;
};

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define	TIFFGetR(abgr)	((abgr) & 0xff)
#define	TIFFGetG(abgr)	(((abgr) >> 8) & 0xff)
#define	TIFFGetB(abgr)	(((abgr) >> 16) & 0xff)
#define	TIFFGetA(abgr)	(((abgr) >> 24) & 0xff)

/*
 * A CODEC is a software package that implements decoding,
 * encoding, or decoding+encoding of a compression algorithm.
 * The library provides a collection of builtin codecs.
 * More codecs may be registered through calls to the library
 * and/or the builtin implementations may be overridden.
 */
typedef	int (*TIFFInitMethod)(TIFF*, int);
typedef struct {
	char*		name;
	uint16		scheme;
	TIFFInitMethod	init;
} TIFFCodec;

#include <stdio.h>
#include <stdarg.h>

#if !defined(_VISUALC_)
#  error Something is very very wrong. This header must only be used under Visual C++.
#endif
/**
 * Under VISUALC we have single threaded static libraries, or
 * mutli-threaded DLLs using the multithreaded runtime DLLs.
 **/
#if defined(_MT) && defined(_DLL) && !defined(_LIB)
#  pragma warning( disable: 4273 )	/* Disable the stupid dll linkage warnings */
#  if !defined(_TIFFLIB_)
#    define TIFFEXPORT __declspec(dllimport)
#  else
#   define TIFFEXPORT __declspec(dllexport)
#  endif
#else
#  define TIFFEXPORT
#endif

#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)

#if defined(__cplusplus)
extern "C" {
#endif
typedef	void (*TIFFErrorHandler)(const char*, const char*, va_list);
typedef	tsize_t (*TIFFReadWriteProc)(thandle_t, tdata_t, tsize_t);
typedef	toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
typedef	int (*TIFFCloseProc)(thandle_t);
typedef	toff_t (*TIFFSizeProc)(thandle_t);
typedef	int (*TIFFMapFileProc)(thandle_t, tdata_t*, toff_t*);
typedef	void (*TIFFUnmapFileProc)(thandle_t, tdata_t, toff_t);
typedef	void (*TIFFExtendProc)(TIFF*); 

extern TIFFEXPORT const char* TIFFGetVersion(void);

extern TIFFEXPORT const TIFFCodec* TIFFFindCODEC(uint16);
extern TIFFEXPORT TIFFCodec* TIFFRegisterCODEC(uint16, const char*, TIFFInitMethod);
extern TIFFEXPORT void TIFFUnRegisterCODEC(TIFFCodec*);

extern TIFFEXPORT tdata_t _TIFFmalloc(tsize_t);
extern TIFFEXPORT tdata_t _TIFFrealloc(tdata_t, tsize_t);
extern TIFFEXPORT void _TIFFmemset(tdata_t, int, tsize_t);
extern TIFFEXPORT void _TIFFmemcpy(tdata_t, const tdata_t, tsize_t);
extern TIFFEXPORT int _TIFFmemcmp(const tdata_t, const tdata_t, tsize_t);
extern TIFFEXPORT void _TIFFfree(tdata_t);

extern TIFFEXPORT void TIFFClose(TIFF*);
extern TIFFEXPORT int TIFFFlush(TIFF*);
extern TIFFEXPORT int TIFFFlushData(TIFF*);
extern TIFFEXPORT int TIFFGetField(TIFF*, ttag_t, ...);
extern TIFFEXPORT int TIFFVGetField(TIFF*, ttag_t, va_list);
extern TIFFEXPORT int TIFFGetFieldDefaulted(TIFF*, ttag_t, ...);
extern TIFFEXPORT int TIFFVGetFieldDefaulted(TIFF*, ttag_t, va_list);
extern TIFFEXPORT int TIFFReadDirectory(TIFF*);
extern TIFFEXPORT tsize_t TIFFScanlineSize(TIFF*);
extern TIFFEXPORT tsize_t TIFFRasterScanlineSize(TIFF*);
extern TIFFEXPORT tsize_t TIFFStripSize(TIFF*);
extern TIFFEXPORT tsize_t TIFFVStripSize(TIFF*, uint32);
extern TIFFEXPORT tsize_t TIFFTileRowSize(TIFF*);
extern TIFFEXPORT tsize_t TIFFTileSize(TIFF*);
extern TIFFEXPORT tsize_t TIFFVTileSize(TIFF*, uint32);
extern TIFFEXPORT uint32 TIFFDefaultStripSize(TIFF*, uint32);
extern TIFFEXPORT void TIFFDefaultTileSize(TIFF*, uint32*, uint32*);
extern TIFFEXPORT int TIFFFileno(TIFF*);
extern TIFFEXPORT int TIFFGetMode(TIFF*);
extern TIFFEXPORT int TIFFIsTiled(TIFF*);
extern TIFFEXPORT int TIFFIsByteSwapped(TIFF*);
extern TIFFEXPORT int TIFFIsUpSampled(TIFF*);
extern TIFFEXPORT int TIFFIsMSB2LSB(TIFF*);
extern TIFFEXPORT uint32 TIFFCurrentRow(TIFF*);
extern TIFFEXPORT tdir_t TIFFCurrentDirectory(TIFF*);
extern TIFFEXPORT tdir_t TIFFNumberOfDirectories(TIFF*);
extern TIFFEXPORT uint32 TIFFCurrentDirOffset(TIFF*);
extern TIFFEXPORT tstrip_t TIFFCurrentStrip(TIFF*);
extern TIFFEXPORT ttile_t TIFFCurrentTile(TIFF*);
extern TIFFEXPORT int TIFFReadBufferSetup(TIFF*, tdata_t, tsize_t);
extern TIFFEXPORT int TIFFWriteBufferSetup(TIFF*, tdata_t, tsize_t);
extern TIFFEXPORT int TIFFWriteCheck(TIFF*, int, const char *);
extern TIFFEXPORT int TIFFCreateDirectory(TIFF*);
extern TIFFEXPORT int TIFFLastDirectory(TIFF*);
extern TIFFEXPORT int TIFFSetDirectory(TIFF*, tdir_t);
extern TIFFEXPORT int TIFFSetSubDirectory(TIFF*, uint32);
extern TIFFEXPORT int TIFFUnlinkDirectory(TIFF*, tdir_t);
extern TIFFEXPORT int TIFFSetField(TIFF*, ttag_t, ...);
extern TIFFEXPORT int TIFFVSetField(TIFF*, ttag_t, va_list);
extern TIFFEXPORT int TIFFWriteDirectory(TIFF *);
extern TIFFEXPORT int TIFFReassignTagToIgnore(enum TIFFIgnoreSense, int);

#if defined(c_plusplus) || defined(__cplusplus)
extern TIFFEXPORT void TIFFPrintDirectory(TIFF*, FILE*, long = 0);
extern TIFFEXPORT int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern TIFFEXPORT int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern TIFFEXPORT int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int = 0);
#else
extern TIFFEXPORT void TIFFPrintDirectory(TIFF*, FILE*, long);
extern TIFFEXPORT int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t);
extern TIFFEXPORT int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t);
extern TIFFEXPORT int TIFFReadRGBAImage(TIFF*, uint32, uint32, uint32*, int);
#endif

extern TIFFEXPORT int TIFFReadRGBAStrip(TIFF*, tstrip_t, uint32 * );
extern TIFFEXPORT int TIFFReadRGBATile(TIFF*, uint32, uint32, uint32 * );
extern TIFFEXPORT int TIFFRGBAImageOK(TIFF*, char [1024]);
extern TIFFEXPORT int TIFFRGBAImageBegin(TIFFRGBAImage*, TIFF*, int, char [1024]);
extern TIFFEXPORT int TIFFRGBAImageGet(TIFFRGBAImage*, uint32*, uint32, uint32);
extern TIFFEXPORT void TIFFRGBAImageEnd(TIFFRGBAImage*);
extern TIFFEXPORT TIFF* TIFFOpen(const char*, const char*);
extern TIFFEXPORT TIFF* TIFFFdOpen(int, const char*, const char*);
extern TIFFEXPORT TIFF* TIFFClientOpen(const char*, const char*,
	    thandle_t,
	    TIFFReadWriteProc, TIFFReadWriteProc,
	    TIFFSeekProc, TIFFCloseProc,
	    TIFFSizeProc,
	    TIFFMapFileProc, TIFFUnmapFileProc);
extern TIFFEXPORT const char* TIFFFileName(TIFF*);
extern TIFFEXPORT void TIFFError(const char*, const char*, ...);
extern TIFFEXPORT void TIFFWarning(const char*, const char*, ...);
extern TIFFEXPORT TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler);
extern TIFFEXPORT TIFFErrorHandler TIFFSetWarningHandler(TIFFErrorHandler);
extern TIFFEXPORT TIFFExtendProc TIFFSetTagExtender(TIFFExtendProc);
extern TIFFEXPORT ttile_t TIFFComputeTile(TIFF*, uint32, uint32, uint32, tsample_t);
extern TIFFEXPORT int TIFFCheckTile(TIFF*, uint32, uint32, uint32, tsample_t);
extern TIFFEXPORT ttile_t TIFFNumberOfTiles(TIFF*);
extern TIFFEXPORT tsize_t TIFFReadTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
extern TIFFEXPORT tsize_t TIFFWriteTile(TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t);
extern TIFFEXPORT tstrip_t TIFFComputeStrip(TIFF*, uint32, tsample_t);
extern TIFFEXPORT tstrip_t TIFFNumberOfStrips(TIFF*);
extern TIFFEXPORT tsize_t TIFFReadEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFReadRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFReadEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFReadRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFWriteEncodedStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFWriteRawStrip(TIFF*, tstrip_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFWriteEncodedTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern TIFFEXPORT tsize_t TIFFWriteRawTile(TIFF*, ttile_t, tdata_t, tsize_t);
extern TIFFEXPORT void TIFFSetWriteOffset(TIFF*, toff_t);
extern TIFFEXPORT void TIFFSwabShort(uint16*);
extern TIFFEXPORT void TIFFSwabLong(uint32*);
extern TIFFEXPORT void TIFFSwabDouble(double*);
extern TIFFEXPORT void TIFFSwabArrayOfShort(uint16*, unsigned long);
extern TIFFEXPORT void TIFFSwabArrayOfLong(uint32*, unsigned long);
extern TIFFEXPORT void TIFFSwabArrayOfDouble(double*, unsigned long);
extern TIFFEXPORT void TIFFReverseBits(unsigned char *, unsigned long);
extern TIFFEXPORT const unsigned char* TIFFGetBitRevTable(int);
#if defined(__cplusplus)
}
#endif
#endif /* _TIFFIO_ */
