/*
 */

#ifndef _EDTLIB_H_
#define _EDTLIB_H_

#include <tcl.h>

#define DEBUG 1

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
