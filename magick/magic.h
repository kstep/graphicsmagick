/*
  ImageMagick Magic Methods.
*/
#ifndef _MODULES_H
#define _MODULES_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Available magic matching methods */
typedef enum
{
  UndefinedMagicMethod,
  StringMagicMethod
} MagicMethod;

/* Argument to support StringMagickMethod */
#define StringMethodArgumentExtent 32
typedef struct _StringMethodArgument
{
  unsigned int
    value_length,
    value_offset;

  unsigned char
    value[StringMethodArgumentExtent];

} StringMethodArgument;

/* List member to support one or more tests for a format */
typedef struct _MagicTestMember
{
  MagicMethod
    method;      /* Method to apply */

  void
    *argument;   /* Method argument, e.g. cast to StringMethodArgument* */

  int
    truth_value; /* Truth value of operation (True or False) */

  struct _MagicTestMember
    *next;

} MagicTestMember;

/* List of formats to test */
typedef struct _MagicTest
{
  char
  *tag;

  struct _MagicTestMember
  *member;

} MagicTest;

/*
  Modules declarations.
*/

extern Export void
  ExitMagic(void);

extern Export unsigned int
  GetImageMagic(unsigned char* magic,const unsigned char *magick,
                const unsigned int magick_length);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
