#pragma once
#ifndef STRICT
#define STRICT
#endif
#ifndef WINVER
#define WINVER 0x0400
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif						
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif
#define _ATL_ATTRIBUTES
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS
#pragma warning(disable: 4103) // pragma pack
#define _WINSOCKAPI_
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atlsafe.h>
using namespace ATL;

#include <asptlb.h>         // Active Server Pages Definitions
#include <comsvcs.h>

#include <iostream>
using namespace std;

namespace MagickLib
{
#define _VISUALC_
//#define _MAGICKDLL_
//#define _MAGICKLIB_
#include <magick/api.h>
#include <magick/nt_base.h>
#include <magick/log.h>
//#undef _MAGICKDLL_
//#undef _MAGICKLIB_
#undef inline
#undef class
}

const DWORD dwErrorBase = 5000;
const int nDefaultArgumentSize = 128;
const LCID lcidDefault = 0;

//
// This routine converts a wide character string to ANSI and stores the new
// string in a buffer allocated by the new operator
//
#define MAKE_WIDEPTR_FROMANSI(ptrname, ansistr) \
    long __l##ptrname = (lstrlen(ansistr) + 1) * sizeof(WCHAR); \
    void * __TempBuffer##ptrname = (void *) new char [ __l##ptrname ]; \
    MultiByteToWideChar(CP_ACP, 0, ansistr, -1, (LPWSTR)__TempBuffer##ptrname.GetBuffer(), __l##ptrname); \
    LPWSTR ptrname = (LPWSTR)__TempBuffer##ptrname

//
// This routine converts an ANSI string to a wide character string in a new
// buffer area allocated using the new operator
// We allocate lstrlenW(widestr) * 2 because its possible for a UNICODE char
// to map to 2 ansi characters this is a quick guarantee that enough space
// will be allocated.
//
#define MAKE_ANSIPTR_FROMWIDE(ptrname, widestr) \
    long __l##ptrname = (lstrlenW(widestr) + 1) * 2 * sizeof(char); \
    void * __TempBuffer##ptrname = (void *) new char [ __l##ptrname]; \
    WideCharToMultiByte(CP_ACP, 0, widestr, -1, (LPSTR)__TempBuffer##ptrname, __l##ptrname, NULL, NULL); \
    LPSTR ptrname = (LPSTR)__TempBuffer##ptrname

#define MAKE_ANSIPTR_FROMWIDE_2(ptrname, widestr, wlen) \
    long __l##ptrname = (wlen + 1) * 2 * sizeof(char); \
    void * __TempBuffer##ptrname = (void *) new char [ __l##ptrname]; \
    WideCharToMultiByte(CP_ACP, 0, widestr, -1, (LPSTR)__TempBuffer##ptrname, __l##ptrname, NULL, NULL); \
    LPSTR ptrname = (LPSTR)__TempBuffer##ptrname
//
// This routine is used to make a copy of an existing ANSI string
//
#define MAKE_COPY_OF_ANSI(ptrname, ansistr) \
    long __l##ptrname = (lstrlen(ansistr) + 1); \
    void * __TempBuffer##ptrname = (void *) new char [ __l##ptrname ]; \
    lstrcpyn((LPSTR)__TempBuffer##ptrname, (LPCSTR)ansistr, __l##ptrname); \
    LPSTR ptrname = (LPSTR)__TempBuffer##ptrname

#define _DbgBreak() __asm { int 3 }
