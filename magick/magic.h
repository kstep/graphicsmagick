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
#define StringMethodArgumentExtent 64
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
#define MagicTestListExtent 256
typedef struct _MagicTest
{
  char
  *tag;

  struct _MagicTestMember
  *member;

} MagicTest;

/*
  Magic declarations.
*/

extern Export void
  QuitMagic(void);

extern Export unsigned int
  GetMagic(char* magic,const unsigned char *magick,
                const unsigned int magick_length);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
