//------------------------------------------------------------------------
// ^FILE: cmdline.h - declare the basic classes used in the CmdLine library
//
// ^DESCRIPTION:
//    This file declares the three basic classes used in the CmdLine library.
//    These three classes are "CmdArg" (a command-argument object),
//    "CmdLineArgIter" (an object to iterate over a set of arguments),
//    and "CmdLine" (the command-line object itself).
//
// ^HISTORY:
//    03/19/92	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/01/93	Brad Appleton	<bradapp@enteract.com>
//    - Added arg_sequence field to CmdArg
//    - Added cmd_nargs_parsed field to CmdLine
//    - Added cmd_description field to CmdLine
//    - Added exit_handler() and quit() member-functions to CmdLine
//    - Added ALLOW_PLUS to list of CmdLine configuration flags
//-^^---------------------------------------------------------------------

#ifndef _usr_include_cmdline_h
#define _usr_include_cmdline_h

#include <string>
#include <iostream>
#include <iomanip>
#include <strstream>
#include <stdarg.h>

using namespace std;

//-----------------------------------------------------------------------------

   // A CmdArg is an abstract command-line argument.
   // At this level (being the base class), all a command argument
   // contains is the "interface" (on the command-line) of the
   // argument, and some information (after the command-line has
   // been parsed) that says "how" the argument appeared (if it did).
   //
   // The interface of a CmdArg consists of 6 things:
   //    1) a character name
   //    2) a keyword name
   //    3) a value name (if the argument takes a value)
   //    4) an argument description
   //    5) a set of flags describing the syntax of the argument.
   //    6) a set of flags to record how (and if) the argument
   //         appeared on the command-line.
   //
   // When constructing a CmdArg, the most common syntax-flags can be
   // inferred from the syntax used in the argument description,
   // and the argument value name.  If the first non-white character
   // of the argument description is ';', then the argument is considered
   // to be "secret" and is NOT printed in usage messages.
   //
   // When specifiying a value, one may enclose the value name between
   // '[' and ']' to indicate the value is optional. Also, one may follow
   // the actual value name with "..." to indicate that the value corresponds
   // to a LIST of values.
   //
   // Hence, the only time you really need to explicitly specify any syntax
   // flags is when you want a value to be "strictly sticky" or "strictly
   // separate"  and/or you want an argument to be able to be matched both
   // positionally AND by (long or short) keyword.
   //
class  CmdArg {
public:
   friend class CmdLine ;

      // Flags that define the argument syntax
   enum  CmdArgSyntax {
      isOPT       = 0x00,  // argument is optional
      isREQ       = 0x01,  // argument is required
      isVALOPT    = 0x02,  // argument value is optional
      isVALREQ    = 0x04,  // argument value is required
      isVALSEP    = 0x08,  // argument value must be in a separate token
      isVALSTICKY = 0x10,  // argument value must be in the same token
      isLIST      = 0x20,  // argument is a list
      isPOS       = 0x40,  // argument is positional
      isHIDDEN    = 0x80,  // argument is not to be printed in usage
      isVALTAKEN  = (isVALREQ | isVALOPT),    // argument takes a value
      isOPTVALOPT = (isOPT | isVALOPT),
      isOPTVALREQ = (isOPT | isVALREQ),
      isPOSVALOPT = (isPOS | isVALOPT),
      isPOSVALREQ = (isPOS | isVALREQ),
   } ;

     // Flags that say how the argument was specied on the command-line
   enum  CmdArgFlags {
      GIVEN      = 0x01,  // argument was given
      VALGIVEN   = 0x02,  // argument value was given
      OPTION     = 0x04,  // item was matched as an option
      KEYWORD    = 0x08,  // item was matched as a keyword
      POSITIONAL = 0x10,  // item was matched as a positional argument
      VALSEP     = 0x20,  // value was in a separate token
   } ;

      // Create an option that takes a value.
      //
      // The default flags are to assume that the argument is optional
      // and that the value is required.
      //
      // Some examples:
      //
      //    CmdArg('c', "count", "number", "specify the # of copies to use");
      //
      //    CmdArg('d', "debug", "[level]", "turn on debugging and optionally "
      //                                    "specify the debug level");
      //
      //    CmdArg('l', "list", "items ...", "specify a list of items.");
      //
   CmdArg(char         optchar,
          const char * keyword,
          const char * value,
          const char * description,
          unsigned     syntax_flags =isOPTVALREQ);

      // Create an option that takes no value.
      //
      // The default flags are to assume that the argument is optional.
      //
      // Some examples:
      //
      //    CmdArg('m', "mode", "turn on this mode");
      //
   CmdArg(char         optchar,
          const char * keyword,
          const char * description,
          unsigned     syntax_flags =isOPT);

      // Create a positional argument.
      //
      // The default flags are to assume that the argument is positional
      // and that the argument value is required.
      //
      // Some examples:
      //
      //    CmdArg("file", "file to read");
      //
      //    CmdArg("[file]", "optional file to read");
      //
      //    CmdArg("file ...", "list of files to read");
      //
      //    CmdArg("[file ...]", "optional list of files to read");
      //
   CmdArg(const char * value,
          const char * description,
          unsigned     syntax_flags =isPOSVALREQ);


   CmdArg(const CmdArg & cp);

   virtual ~CmdArg(void);

      // over-ride this function to return a non-zero value if you
      // wish the argument to be ignored (except for usage messages).
      //
   virtual  int
   is_dummy(void);

      // Here is the "primary" function that makes everything work ...
      //
      // Whenever we actually "match" an argument on the command-line,
      // we need to tell the argument it was matched (and how), and
      // give it the string value (if there is one) to associate with it.
      //
      // At this point, the argument object is then responsible for
      // performing whatever "magic" is to be done. This might be going
      // off and reading a file, performing some other actions, or just
      // "compiling" the argument into some internal value (specified
      // by the derived class) to be queried at a later time.
      //
      // The parameters to this function are as follows:
      //
      //   PARAMETER 1: arg
      //      The string value on the command-line to associate with this
      //      argument. If this argument does not take a value, or the
      //      value is optional and was NOT supplied, then NULL is passed.
      //      This parameter is a reference parameter and before returning,
      //      "arg" should either be set to NULL (to indicate that the entire
      //      argument was used) or should point to the first unused character
      //      of "arg".
      //
      //   PARAMETER 2: cmd
      //      A reference to the command-line object that is currently being
      //      parsed.  There are parts of this object that may be helpful in
      //      determining what to do. In particular, we may want to look at
      //      the "flags" of the command.  If the QUIET flag is set, then
      //      this routine should suppress the printing of any error messages.
      //      If the TEMP flag is set, then "arg" (the first parameter) points
      //      to storage that may not be around for much longer and we may
      //      want to make a copy of it.
      //
      // Before, this function is called, the CmdLine object that is parsing
      // the arguments has already set the "flags()" of this argument to tell
      // us how we appeared on the command-line this time around (were we
      // specified as an option or positionally? Is "arg" in a separate argv[]
      // element?).
      //
      // After we have done our "magic" and set the reference parameter
      // "arg", this function should return a value of 0 if everything
      // is A-OK and "arg" was a correctly specified value for this type of
      // argument. If something went wrong (like a syntax error in "arg"),
      // then we should return a non-zero value.
      //
   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

      // Retrieve the syntax flags for this argument.
   unsigned
   syntax(void) const  { return  arg_syntax; }

      // Get the flags that say how this argument was specified.
   unsigned
   flags(void) const  { return  arg_flags; }

      // Get the sequence number corresponding to the last
      // time this argument was matched on the command-line.
      //
      // If this argument was not matched, the sequence number
      // will be zero, otherwise it will be 'N' where the last
      // time this argument was matched, it was the 'N'th argument
      // encountered.
   unsigned
   sequence(void) const { return arg_sequence; }

      // Get the character (short-option) name of this argument.
      // Returns '\0' if there isnt one.
   char
   char_name(void) const  { return  arg_char_name; }

      // Get the keyword (long-option) name of this argument
      // Returns NULL if there isnt one.
   const char *
   keyword_name(void) const  { return  arg_keyword_name; }

      // Get the value name of this argument.
      // Returns NULL if this argument takes no value.
   const char *
   value_name(void) const  { return  arg_value_name; }

      // Get the description (help-message) of this argument.
   const char *
   description(void) const  { return  arg_description; }

      // If we were compiled for dubugging, then dump this argument
   virtual  void
   dump(ostream & os, unsigned level =0) const;

private:
   // Member functions for internal use

   void
   adjust_syntax(void);

   void
   parse_description(void);

   void
   parse_value(void);

   void
   flags(unsigned newflags)  { arg_flags = newflags; }

   void
   set(unsigned flags)  { arg_flags |= flags; }

   void
   clear(unsigned flags =~0)  { arg_flags &= ~flags; }

      // set sequence number
   void
   sequence(unsigned  num) { arg_sequence = num; }

   // Private data members

   unsigned     alloc_value_name : 1 ;

   unsigned     arg_flags : 8 ;
   unsigned     arg_syntax : 8 ;

   unsigned     arg_sequence;

   char         arg_char_name;
   const char * arg_keyword_name;
   const char * arg_value_name;
   const char * arg_description;
} ;

//-----------------------------------------------------------------------------

   // In order to parse arguments, we need to have an input source where
   // the arguments are coming from.  CmdLineArgIter is an abstract
   // iterator class for cycling through all the command-line arguments
   // from an arbitrary input source.
   //
class  CmdLineArgIter {
public:
   CmdLineArgIter(void);

   virtual ~CmdLineArgIter(void);

      // Return the current argument and advance to the next one.
      // Returns NULL if we are already at the end of the arguments
      //
   virtual const char *
   operator()(void) = 0;

      // Are the args returned by operator() pointing to temporary storage?
   virtual int
   is_temporary(void) const = 0;

private:
   CmdLineArgIter(const CmdLineArgIter &) ;

   CmdLineArgIter &
   operator=(const CmdLineArgIter &) ;
} ;


   // CmdArgvIter is a class used to iterate through command arguments
   // that come from an array of strings (like argv[] from main()).
   //
class  CmdArgvIter : public CmdLineArgIter {
public:
   CmdArgvIter(int argc, const char * const argv[])
      : count(argc), array(argv), index(0) {}

   CmdArgvIter(const char * const argv[])
      : array(argv), index(0), count(-1) {}

   virtual ~CmdArgvIter(void);

   virtual const char *
   operator()(void);

      // is_temporary returns 0 for CmdArgvIter
   virtual int
   is_temporary(void) const;

      // Restart using a different string array.
   void
   reset(int argc, const char * const argv[])
      { count = argc; array = argv; index = 0; }

   void
   reset(const char * const argv[])
      { array = argv; index = 0; count = -1; }

private:
   CmdArgvIter(const CmdArgvIter &) ;

   CmdArgvIter &
   operator=(const CmdArgvIter &) ;

   int   count;
   int   index;
   const char * const *  array;
} ;


   // CmdStrTok iterator is a class for iterating over arguments that
   // are specified in a string of tokens that are delimited by a
   // particular set of characters.  The strtok(3C) library function
   // is used to extract tokens from the string.
   //
   // If NULL is given as the delimiter-set, then whitespace is assumed.
   //
class  CmdStrTokIter : public CmdLineArgIter {
public:
   CmdStrTokIter(const char * tokens, const char * delimiters =0);

   virtual ~CmdStrTokIter(void);

   virtual const char *
   operator()(void);

      // Reset using a new token-string and delimiter set.
   void
   reset(const char * tokens, const char * delimiters =0);

      // Get the current delimiter set
   const char *
   delimiters(void) const { return  seps; }

      // Change the current delimiter set
   void
   delimiters(const char * new_delimiters)  { seps = new_delimiters; }

      // is_temporary returns 1 for CmdStrTokIter
   virtual int
   is_temporary(void) const;

private:
   CmdStrTokIter(const CmdStrTokIter &) ;

   CmdStrTokIter &
   operator=(const CmdStrTokIter &) ;

   char       * tokstr;
   const char * seps;
   const char * token;
} ;


   // CmdIstreamIter is a class for iterating over arguments that come
   // from an input stream. Each line of the input stream is considered
   // to be a set of white-space separated tokens. If the the first
   // non-white character on a line is '#' ('!' for VMS systems) then
   // the line is considered a comment and is ignored.
   //
   // *Note:: If a line is more than 1022 characters in length then we
   // treat it as if it were several lines of length 1022 or less.
   //
class  CmdIstreamIter : public CmdLineArgIter {
public:
   static const unsigned  MAX_LINE_LEN ;

   CmdIstreamIter(istream & input);

   virtual ~CmdIstreamIter(void);

   virtual const char *
   operator()(void);

      // is_temporary returns 1 for CmdIstreamIter
   virtual int
   is_temporary(void) const;

private:
   istream & is ;
   CmdStrTokIter * tok_iter ;
} ;

//-----------------------------------------------------------------------------

   // Here is the class that represents a command-line object. A command
   // line object is a parsing machine (with machine states), whose parsing
   // behavior may be configured at run-time by specifying various CmdFlags
   // (defined below). A command-line object also contains a command-name
   // and a list of CmdArg objects that correspond to the various arguments
   // that are allowed to occur on the command line.
   //
class  CmdLine {
public:
   friend class CmdLineCmdArgIter;

      // Flags that define parsing behavior
      //   The default flags (for Unix) are OPTS_FIRST.
   enum CmdFlags {
      ANY_CASE_OPTS = 0x001, // Ignore character-case for short-options
      NO_ABORT      = 0x002, // Dont quit upon syntax error
      OPTS_FIRST    = 0x004, // No options after positional parameters
      OPTS_ONLY     = 0x008, // Dont accept long-options
      KWDS_ONLY     = 0x010, // Dont accept short-options
      TEMP          = 0x020, // Assume all arg-strings are temporary
      QUIET         = 0x040, // Dont print syntax error messages
      NO_GUESSING   = 0x080, // Dont guess if cant match an option. 
                             // Unless this flag is given, then
                             // when we see an unmatched option,
                             // we will try to see if it matches
                             // a keyword (and vice-versa).
      ALLOW_PLUS    = 0x100, // Allow "+" (as well as "--") as a prefix
                             // indicating long-options.
   } ;

      // Flags to convey parsing-status
   enum CmdStatus {
      NO_ERROR      = 0x000,  // No problems
      ARG_MISSING   = 0x001,  // A required argument was not specified
      VAL_MISSING   = 0x002,  // A required argument value was not specified
      VAL_NOTSTICKY = 0x004,  // Value needs to be in same token
      VAL_NOTSEP    = 0x008,  // Value needs to be in separate token
      KWD_AMBIGUOUS = 0x010,  // An ambiguous keyword prefix was specified
      BAD_OPTION    = 0x020,  // An invalid option was specified
      BAD_KEYWORD   = 0x040,  // An invalid keyword was specified
      BAD_VALUE     = 0x080,  // An invalid value was specified for an arg
      TOO_MANY_ARGS = 0x100,  // Too many positional args were specified
   } ;

      // Contructors and Destructors ...
      //
      //   It is not necessary to supply a command-name at construction
      //   time, but one SHOULD be specified before parsing a command-line
      //   or printing a usage message.
      //
      //   Similarly, CmdArgs are not required at construction time and may
      //   even be added on the fly. All desired arguments should be added
      //   before any parsing happens and before printing usage.
      //
      //   The order in which CmdArgs are added to a CmdLine is important
      //   because for positional parameters, this specifies the order in
      //   which they are expected to appear on the command-line.
      //
   CmdLine(const char * name =0);

   CmdLine(const char * name, CmdArg * cmdarg1 ...); // last arg should be NULL

   CmdLine(CmdArg * cmdarg1 ...);  // last arg should be NULL

   virtual ~CmdLine(void);

      // Get the command name.
   const char *
   name(void)  const  { return  cmd_name; }

      // Specify a command name.
   void
   name(const char * progname);

      // Get the command description.
   const char *
   description(void)  const  { return  cmd_description; }

      // Specify a command description.
   void
   description(const char * the_description) {
      cmd_description = the_description;
   }

      // Append an argument
   CmdLine &
   append(CmdArg * cmdarg);

   CmdLine &
   append(CmdArg & cmdarg)  { return  append(& cmdarg); }

      // The insertion operator (operator<<) is merely a convenient
      // shorthand for the append() member function.
      //
   CmdLine &
   operator<<(CmdArg * cmdarg)  { return  append(cmdarg) ; }

   CmdLine &
   operator<<(CmdArg & cmdarg)  { return  append(& cmdarg) ; }

   //
   // Messages and Status
   //

      // Specify the verboseness of usage messages
   enum CmdUsageLevel {
      NO_USAGE      = 0,  // Dont print usage at all.
      TERSE_USAGE   = 1,  // Just print command-line syntax.
      VERBOSE_USAGE = 2,  // Print command-line syntax & argument-descriptions.
      DEFAULT_USAGE = 3,  // Read the $USAGE_LEVEL environment variable for
                             // the usage-level: 0=none, 1=terse, 2=verbose.
                             // if $USAGE_LEVEL is empty or is not 0, 1, or 2
                             // then VERBOSE_USAGE is used.
   } ;

      // Print usage on the given output stream using the given verboseness
   ostream &
   usage(ostream & os, CmdUsageLevel level =DEFAULT_USAGE) const ;

      // Print usage on the CmdLine's error-outstream
   ostream &
   usage(CmdUsageLevel level =DEFAULT_USAGE) const ;

      // Obtain the current status of the command. The status will be
      // zero if everything is alright; otherwise it will correspond
      // to a combination of CmdStatus bitmasks telling us precisely
      // what went wrong.
      //
   unsigned
   status(void)  const  { return  cmd_status; }

      // Obtain the current state of the command. The state will be
      // one of the values in states.h in the enum cmd_parse_state_t
   unsigned
   state(void)  const  { return  cmd_parse_state; }

   
      // Print an error message prefix on the error output stream
      // associated with this command. The prefix printed is the
      // basename of the command followed by a ':'.
      //
      // Hence error messages associated with this command may be
      // printed as follows:
      //
      //    my_cmd.error() << "This is what went wrong!" << endl;
      //
      // If QUIET (or for that matter, any non-zero value) is given
      // as a parameter, then nothing is printed on the ostream
      // (which may be useful if you wish only to obtain a reference
      // this CmdLine's error-outstream).
      //
   ostream &
   error(unsigned  quiet =0) const;

      // If the QUIET-flag is not set, then we need to know where
      // to print any error messages (the default is cerr).
      //
      // Use this member function to specify the desired output stream
      // for error messages.
      //
   void
   error(ostream & os) { cmd_err = &os; }

   //
   // Get & set the command-parsing-flags
   //

      // Get the current set of command-flags
   unsigned
   flags(void) const  { return  cmd_flags; }

      // Specify a new set of command-flags
   void
   flags(unsigned newflags)  { cmd_flags = newflags; }

      // Set only the given command-flags
   void
   set(unsigned flags)  { cmd_flags |= flags; }

      // Clear only the given command-flags
   void
   clear(unsigned flags =~0)  { cmd_flags &= ~flags; }

   //
   // We are somewhat flexible in the way we parse arguments.
   // Before any parsing can occur, some preprocessing needs to
   // be done. After we have parsed all the arguments, some
   // post-processing needs to be done.  If you use the "parse()"
   // member function, then this pre- and post- processing is
   // automatically performed for you UNLESS you specify a second
   // parameter of NO_PROCESSING.  If you have arguments that are
   // coming from more then one input source, you will have to
   // manually activate pre-processing (by calling prologue()),
   // then parse all your arguments (using parse() with NO_PROCESSING
   // and/or using arg_parse()), and then manually activate
   // post-processing (by calling epilogue()).
   //

      // Parse a set of arguments (pre- and post- processing is
      // automatically performed UNLESS "NO_PROCESSING" is given
      // as the second argument).
      //
      // Return the resultant command status.
      //
   enum { NO_PROCESSING = 0, AUTO_PROCESSING = 1 } ;

   unsigned
   parse(CmdLineArgIter & arg_iter, int  processing =AUTO_PROCESSING) ;

      // Perform the necessary pre-processing.
      // Return the resultant command status.
      //
   unsigned
   prologue(void) ;

      // Parse a single argument (pre- and post- processing is
      // NOT performed).
      //
   unsigned
   parse_arg(const char * arg) ;

      // Perform the necessary post-processing.
      // Return the resultant command status.
      //
   unsigned
   epilogue(void) ;

   //
   // Find out the number of arguments parsed so far
   //
   unsigned
   nargs_parsed(void) const { return cmd_nargs_parsed; }

   //
   // Find out the last matched argument that was parsed so far
   CmdArg *
   matched_arg(void) const { return cmd_matched_arg; }

   //
   // Exception handling (well -- not really)
   //

   typedef  void (* quit_func_t)(int);

      // When a fatal error is encountered or when parsing needs to
      // terminate immediately, the quit() member function is called.
      // If the programmer has used quit_handler() to setup his own
      // handler-function, then that function is called; otherwise
      // exit() is called with the given status.
      // 
   void
   quit(int status);

      // Set the quit-handler. The quit-handler is a pointer to
      // a function that has no return value and takes a single
      // integer parameter.
      //
   void
   quit_handler(quit_func_t  quit_func) { cmd_quit_handler = quit_func ; }

      // Get the current quit-handler (returns NULL if there isnt one)
      //
   quit_func_t
   quit_handler(void) const { return  cmd_quit_handler; }


   //
   // Retrieve a specific argument
   //

      // Retrieve an argument based on its character-name.
      // Returns NULL if no argument matches the given character.
      //
   CmdArg *
   operator[](char optchar) const { return  opt_match(optchar); }

      // Retrieve an argument based on its keyword-name.
      // (if an argument has no keyword-name then we try to match its
      // value name instead).
      //
      // Returns NULL if no argument matches the given keyword or
      // the keyword is ambiguous.
      //
   CmdArg *
   operator[](const char * keyword) const
      { int ambig = 0;  return  kwd_match(keyword, -1, ambig, 1); }

   //
   // Version specific information
   //

      // get the release number
   static  unsigned
   release(void);

      // get the patchlevel number
   static  unsigned
   patchlevel(void);

      // get the SCCS identifier string
   static  const char *
   ident(void);

   //
   // These next few functions are used internally but are general
   // enough in purpose as to possibly be useful to others.
   //

      // return type for an attempted keyword match
   enum strmatch_t { str_NONE, str_PARTIAL, str_EXACT } ;

      // Try to match "attempt" against "src", if len is 0 then
      // only the first "len" characters are compared.
      //
      // Returns str_EXACT for an exact-match str_PARTIAL for a
      // partial-match, and str_NONE otherwise.
      //
   static  strmatch_t
   strmatch(const char * src, const char * attempt, unsigned  len =0);

      // Print a hanging indented paragraph on an outstream. Long lines
      // are broken at word boundaries and are wrapped to line up with
      // the rest of the paragraph.  The format looks like the following
      // (text starts on a new line is the strlen(title) >= indent):
      //
      // <------------------------- maxcols ------------------------------->
      // <--- margin ---><--- indent --->
      //                 title           This is the first sentence.  This
      //                                 is the second sentence. etc ...
      //
   static  void
   strindent(ostream    & os,
             unsigned     maxcols,
             unsigned     margin,
             const char * title,
             unsigned     indent,
             const char * text);

   //
   // Debugging stuff ...
   //

      // If we were compiled for dubugging, then dump this command
   virtual  void
   dump(ostream & os, unsigned level =0) const;

      // If we were compiled for dubugging, then dump the argument list
   virtual  void
   dump_args(ostream & os, unsigned level =0) const;

private:
      // Private data members
   unsigned          cmd_parse_state : 8 ;
   unsigned          cmd_state  : 8 ;
   unsigned          cmd_flags  : 16 ;
   unsigned          cmd_status : 16 ;
   unsigned          cmd_nargs_parsed ;
   const char      * cmd_name ;
   const char      * cmd_description ;
   CmdArg          * cmd_matched_arg ;
   CmdArgListList  * cmd_args ;
   ostream         * cmd_err ;
   quit_func_t       cmd_quit_handler ;

      // Disallow copying and assignment
   CmdLine(const CmdLine & );

   CmdLine &
   operator=(const CmdLine & );

   //
   // Member functions for internal use
   //

      // Specify the command syntax to use for usage messages
   enum CmdLineSyntax { cmd_OPTS_ONLY = 0, cmd_KWDS_ONLY = 1, cmd_BOTH = 2 } ;

   int
   handle_arg(CmdArg * cmdarg, const char * & arg);

   void
   ck_need_val(void);

   CmdLineSyntax
   syntax(void) const;

   unsigned
   missing_args(void);

   CmdArg *
   opt_match(char optchar) const;

   CmdArg *
   kwd_match(const char * kwd,
             int          len,
             int &        is_ambiguous,
             int          match_value =0) const;

   CmdArg *
   pos_match(void) const;

   unsigned
   parse_option(const char * arg);

   unsigned
   parse_keyword(const char * arg);

   unsigned
   parse_value(const char * arg);

   ostream &
   arg_error(const char * error_str, const CmdArg * cmdarg) const;

   unsigned
   fmt_arg(const CmdArg * cmdarg,
           char         * buf,
           unsigned       bufsize,
           CmdLineSyntax  syntax,
           CmdUsageLevel  level) const;

   static  CmdUsageLevel
   get_usage_level(void);

   unsigned
   print_synopsis(CmdLineSyntax syntax,
                  ostream     & os,
                  int           cols) const;

   void
   print_descriptions(CmdLineSyntax   syntax,
                      ostream       & os,
                      int             cols,
                      unsigned        longest) const;

} ;


   // "os << cmd" is equivalent to "cmd.usage(os)"
inline ostream &
operator <<(ostream & os, CmdLine & cmd)  { return  cmd.usage(os); }


//-----------------------------------------------------------------------------

   // We want to provide the user with a means to iterate over all the
   // arguments in the argument list of a command-line.  We will provide
   // a class named "CmdLineCmdArgIter" to do this.

class  CmdLineCmdArgIter {
public:
   CmdLineCmdArgIter(CmdLine & cmd);

   CmdLineCmdArgIter(CmdLine * cmd);

   virtual ~CmdLineCmdArgIter(void);

      // Return the current argument and advance to the next one.
      // Returns NULL if we are already at the end of the list.
      //
   CmdArg *
   operator()(void);

private:
   CmdLineCmdArgIter(const CmdLineCmdArgIter &);

   CmdLineCmdArgIter &
   operator=(const CmdLineCmdArgIter &);

   CmdArgListIter * iter;
} ;

#endif /* _usr_include_cmdline_h */
