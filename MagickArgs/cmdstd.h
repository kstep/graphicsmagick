#ifndef _usr_include_cmdstd_h
#define _usr_include_cmdstd_h

#define unix_style
#define DEBUG_CMDLINE

class CmdArg;

#include <stdlib.h>

#ifndef e_SUCCESS
# define  e_SUCCESS   0
# define  e_USAGE     1
# define  e_SYNTAX    2
# define  e_INTERNAL  127
#endif

//
// Some Helper function for getting and recognizing prefixes
//

// Function to tell us if an argument looks like an option
inline int
isOPTION(const char * s)  {
   return  ((*(s) == '-') && ((*((s)+1) != '-')) && ((*((s)+1) != '\0'))) ;
}

// Function to return the option-prefix
inline const char *
OptionPrefix(void) { return  "-" ; }

// Function to tell us if an argument looks like a long-option.
//
// NOTE: allowing "+" does not preclude the use of "--"
//
inline int
isKEYWORD(const char *s, int allow_plus) {
   return  (((*(s) == '-') && (*((s)+1) == '-')  && (*((s)+2) != '\0')) ||
            (allow_plus && (*(s) == '+') && ((*((s)+1) != '\0')))) ;
}

// Function to return the long-option prefix
inline const char *
KeywordPrefix(int allow_plus) { return  (allow_plus) ? "+" : "--" ; }

// Need to know when an argument means "end-of-options"
inline int
isENDOPTIONS(const char *s) {
   return  ((*(s) == '-') && (*((s)+1) == '-')  && (*((s)+2) == '\0')) ;
}

// Function to return the "end-of-options" string
inline const char *
EndOptions(void) { return "--" ; }

//   This area declares the types that are used by a CmdLine to hold the
//   arguments associated with it.  What we do is keep a list of lists
//   of CmdArgs. The first list is the arguments that were specified
//   by the user/programmer, The second list is the list of default
//   arguments for this command. Only two lists are used at present.
//

#include "fifolist.h"

typedef  FifoList<CmdArg> CmdArgList;
typedef  FifoListIter<CmdArg>  CmdArgListIter;
typedef  FifoListArray<CmdArg> CmdArgListArray;

typedef  FifoList<CmdArgList> CmdArgListList;
typedef  FifoListIter<CmdArgList>  CmdArgListListIter;
typedef  FifoListArray<CmdArgList> CmdArgListListArray;

#include "cmdline.h"

#include "cmdargs.h"

//  This area contains the definitions for the various values of the
//  state and parse-state of a command-line object. It also contains
//  any definitions that are dependent upon the command-line syntax
//  (i.e. unix_style or vms_style).

#ifdef unix_style
   // Default command-flags for a unix-command
   static const unsigned DEFAULT_CMDFLAGS = CmdLine::OPTS_FIRST ;
#endif

//
// cmd_state_t -- Define the bitmasks used to record the command state
//
enum cmd_state_t {
   cmd_END_OF_OPTIONS  = 0x01,  // no more options/keywords?
   cmd_OPTIONS_USED    = 0x02,  // were options used on cmdline?
   cmd_KEYWORDS_USED   = 0x04,  // were keywords used on cmdline?
   cmd_GUESSING        = 0x08,  // are we currently trying to guess?
} ;

//
// cmd_parse_state_t -- Define the possible parse-states for the command
//
// We use "START_STATE" to reset the state. Only one of the NEED*
// states may be set at a time. For any of the NEED* states, TOK_REQUIRED
// may or may not be set. TOK_REQUIRED should NOT be set if none of the
// NEED* states is set.
//
// Note: we have the "states" set up so that one can test for WANT or NEED
// by a bitwise & with a WANT flag. One can test if the particular "WANT"
// is truly "NEEDED" by a bitwise & with the TOK_REQUIRED_FLAG. For
// convenience, each WANT_XXX that is truly REQUIRED may also be
// represented by NEED_XXX.
//
enum cmd_parse_state_t {
   cmd_START_STATE  = 0x00,  // start-state (this MUST be 0)

   cmd_TOK_REQUIRED = 0x01,  // is the "wanted" token required?

   cmd_WANT_VAL     = 0x02,  // are we expecting a value?
   cmd_NEED_VAL     = (cmd_WANT_VAL | cmd_TOK_REQUIRED),

#ifdef vms_style
   cmd_WANT_VALSEP  = 0x04,  // are we expecting ':' or '='
   cmd_NEED_VALSEP  = (cmd_WANT_VALSEP | cmd_TOK_REQUIRED),

   cmd_WANT_LISTSEP = 0x08,  // are we expecting ',' or '+'
   cmd_NEED_LISTSEP = (cmd_WANT_LISTSEP | cmd_TOK_REQUIRED),
#endif
} ;

#endif