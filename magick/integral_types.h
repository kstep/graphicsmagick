/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick integral types typedefs
*/

#ifndef _MAGICK_INTEGRAL_TYPES_H
#define _MAGICK_INTEGRAL_TYPES_H

/*
  Assign ANSI C stdint.h-like typedefs based on the sizes of native types
  gm_int16_t  --                    -32,768 to 32,767
  gm_uint16_t --                          0 to 65,535
  gm_int32_t  --             -2,147,483,648 to 2,147,483,647
  gm_uint32_t --                          0 to 4,294,967,295
  gm_int64_t  -- -9,223,372,036,854,775,807 to 9,223,372,036,854,775,807
  gm_uint64_t --                          0 to 18,446,744,073,709,551,615
*/

#if !defined(SIZEOF_SHORT)
# define SIZEOF_SHORT 2
#endif

#if !defined(SIZEOF_UNSIGNED_SHORT)
# define SIZEOF_UNSIGNED_SHORT 2
#endif

#if !defined(SIZEOF_INT)
# define SIZEOF_INT 4
#endif

#if !defined(SIZEOF_UNSIGNED_INT)
# define SIZEOF_UNSIGNED_INT 4
#endif

#if !defined(SIZEOF_LONG)
# define SIZEOF_LONG 4
#endif

#if !defined(SIZEOF_UNSIGNED_LONG)
# define SIZEOF_UNSIGNED_LONG 4
#endif

#if !defined(SIZEOF_LONG_LONG)
# define SIZEOF_LONG_LONG 0
#endif

#if !defined(SIZEOF_UNSIGNED_LONG_LONG)
# define SIZEOF_UNSIGNED_LONG_LONG 0
#endif

#if (SIZEOF_SHORT == 2)
  typedef short gm_int16_t;
#else
#  error sizeof(short) != 2
#endif

#if (SIZEOF_UNSIGNED_SHORT == 2)
  typedef unsigned short gm_uint16_t;
#else
#  error sizeof(unsigned short) != 2
#endif

#if (SIZEOF_INT == 4)
  typedef int gm_int32_t;
#else
#  error sizeof(int) != 4
#endif

#if (SIZEOF_UNSIGNED_INT == 4)
  typedef unsigned int gm_uint32_t;
#else
#  error sizeof(unsigned int) != 4)
#endif

#if (SIZEOF_LONG == 8)
  typedef long gm_int64_t;
#elif (SIZEOF_LONG_LONG == 8) && !defined(_VISUALC_)
  typedef long long gm_int64_t;
#elif defined(_VISUALC_)
  typedef __int64 gm_int64_t;
#else
  typedef double gm_int64_t;
#endif

#if (SIZEOF_UNSIGNED_LONG == 8)
  typedef unsigned long gm_uint64_t;
#elif (SIZEOF_UNSIGNED_LONG_LONG == 8) && !defined(_VISUALC_)
  typedef unsigned long long gm_uint64_t;
#elif defined(_VISUALC_)
  typedef unsigned __int64 gm_uint64_t;
#else
  typedef double gm_uint64_t;
#endif

#endif /* _MAGICK_INTEGRAL_TYPES_H */
