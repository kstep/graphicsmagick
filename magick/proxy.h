/*
  Proxy include definitions.
*/
#ifndef _PROXY_H
#define _PROXY_H

#if defined(HasBZLIB)
#include "bzlib.h"
#endif
#if defined(HasDPS)
#include <DPS/dpsXclient.h>
#include <DPS/dpsXpreview.h>
#endif
#if defined(HasFPX)
#include "Fpxlib.h"
#endif
#if defined(HasJBIG)
#include "jbig.h"
#endif
#if defined(HasJPEG)
#if !defined(HasPNG)
#include <setjmp.h>
#endif
#include "jpeglib.h"
#include "jerror.h"
#define ICC_MARKER  (JPEG_APP0+2)
#endif
#if defined(HasPNG)
#include "png.h"
#endif
#if defined(HasTIFF)
#include "tiffio.h"
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif
#if defined(HasTTF)
#include "freetype.h"
#endif
#if defined(HasZLIB)
#include "zlib.h"
#endif

#endif
