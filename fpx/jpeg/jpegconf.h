
/**
 * Under VISUALC we have single threaded static libraries, or
 * mutli-threaded DLLs using the multithreaded runtime DLLs.
 **/
#if defined(_VISUALC_)
#   if defined(_MT) && defined(_DLL) && !defined(_FPXDLL_) && !defined(_LIB)
#      define _FPXDLL_
#   endif
#   if defined(_FPXDLL_)
#      pragma warning( disable: 4273 )  /* Disable the stupid dll linkage warnings */
#      if !defined(_FPXLIB_)
#         define JPEGEXPORT extern __declspec(dllimport)
#         define FPXEXPORT extern __declspec(dllimport)
#         pragma message( "Compiling as dll import" ) 
#      else
#         define JPEGEXPORT extern __declspec(dllexport)
#         define FPXEXPORT extern __declspec(dllexport)
#         pragma message( "Compiling as dll export" ) 
#      endif
#   else
#      define FPXEXPORT extern
#      define JPEGEXPORT extern
#      pragma message( "Compiling as simple library" ) 
#   endif
#   pragma warning(disable : 4018)
#   pragma warning(disable : 4244)
#   pragma warning(disable : 4142)
#else
#   define JPEGEXPORT
#   define FPXEXPORT
#endif
