//------------------------------------------------------------------------
// ^FILE: cmdline.c - implement CmdLine member functions.
//
// ^DESCRIPTION:
//    All of the member functions of a CmdLine are implemented
//    in this file. They are as follows:
//
// ^HISTORY:
//    03/21/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#include "cmdstd.h"

#ifdef DEBUG_CMDLINE
# include  <iostream>
# include  <string>
#endif

// Need a portable version of tolower
#define TO_LOWER(c)  ((isupper(c)) ? tolower(c) : c)

#define  va_CmdArgP(ap)  va_arg(ap, CmdArg *)

// Record the version-identifier for this configuration of the project.
//
// My source-code management system lets me use a symbolic-name
// to identify a given configuration of the project. From this
// symbolic-name I can easily find out the exact version of each
// file that makes up this version of the project.
//
static const char ident[] =
   "@(#)SMS  task: cmdline-1.04" ;

// Release and patchlevel information
#define  CMDLINE_RELEASE     1
#define  CMDLINE_PATCHLEVEL  4
#define  CMDLINE_IDENT       "@(#)CmdLine	1.04"

unsigned
CmdLine::release(void)  { return  CMDLINE_RELEASE; }

unsigned
CmdLine::patchlevel(void)  { return  CMDLINE_PATCHLEVEL; }

const char *
CmdLine::ident(void) {
   static const char Ident[] = CMDLINE_IDENT ;

   return  Ident;
}

//-------------------
// ^FUNCTION: init_args - initialize the arg_list member of a CmdLine
//
// ^SYNOPSIS:
//    init_args(list)
//
// ^PARAMETERS:
//    CmdArgListList & * list;
//    -- a reference to the list that is to be initialized.
//
// ^DESCRIPTION:
//    Allocate space for the list of command-arguments and insert
//    The default arguments onto the list.
//
// ^REQUIREMENTS:
//    list should be NULL upon entry
//
// ^SIDE-EFFECTS:
//    creates the arg-list and makes "list" point to it.
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    - Create a new arg-list (sure it is NOT self-cleaning, the user is
//                             responsible for deleting inserted items)
//    - Insert the default arguments onto the list
//    - Make list point to the newly created list
//-^^----------------
static void
init_args(CmdArgListList * & list)
{
   static  CmdArgUsage  default_help1('?', "?",    "; print usage and exit.");
   static  CmdArgUsage  default_help2('H', "Help", "; print usage and exit.");

   list = new CmdArgListList ;
   list->self_cleaning(1);

   CmdArgList * arg_list = new CmdArgList;
   arg_list->self_cleaning(0);

   CmdArgList * default_list = new CmdArgList;
   default_list->self_cleaning(0);
   default_list->add(&default_help1);
   default_list->add(&default_help2);

   list->add(arg_list);
   list->add(default_list);
}

//---------------------------------------------------------------- filebasename

//-------
// ^FUNCTION: filebasename
//
// ^SYNOPSIS:
//    static const char * filebasename(filename);
//
// ^PARAMETERS:
//    const char * filename;
//    -- the filename to get the "base" of.
//
// ^DESCRIPTION:
//    Extract and return the basename of "filename".
//
// ^REQUIREMENTS:
//    "filename" should be non-NULL and non-empty.
//
// ^SIDE-EFFECTS:
//    On VAX/VMS, MS-DOS, and OS/2 systems space is allocated (using malloc)
//    for the returned value.
//
// ^RETURN-VALUE:
//    The basename portion of the filename.
//
// ^ALGORITHM:
//    For Unix systems:
//       return everything following the rightmost '/'
//
//    For VAX/VMS systems:
//       make a copy of filename.
//       strip off any device name, any directory name.
//       strip off any "." extension.
//       strip off any version number.
//
//    For MS-DOS systems:
//       make a copy of filename.
//       strip off any drive and/or directory path.
//       strip off any "." extension.
//-^^----
static const char *
filebasename(const char * filename)
{

   if (filename == NULL)  return  filename ;

#if (defined(msdos) || defined(os2))
   const char * start, * p1, * p2 ;
   char * str, * ext;

   // remove leading directory and/or drive name
   p1 = ::strrchr(filename, '/');
   p2 = ::strrchr(filename, '\\');
   if ((p1 == NULL) && (p2 == NULL)) {
      start = filename ;
   } else if (p1 && (p2 == NULL)) {
      start = p1 + 1;
   } else if (p2 && (p1 == NULL)) {
      start = p2 + 1;
   } else {
      start = ((p1 > p2) ? p1 : p2) + 1;
   }

   str = new char[strlen(start) + 1];
   (void) ::strcpy(str, start);

   // remove the extension
   ext = ::strrchr(str, '.');
   if (ext)  *ext = '\0' ;

   return  str ;

#else

   char * p = ::strrchr(filename, '/') ;
   return  (p == NULL) ? filename : (p + 1) ;

#endif /* if (msdos || os2) */

}

//--------------------------------------------------------------- class CmdLine

  // Contructor with a command-name
CmdLine::CmdLine(const char * cmdname)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   name(cmdname);
   ::init_args(cmd_args);
}

   // Constructor with a name and CmdArgs
CmdLine::CmdLine(const char * cmdname, CmdArg * cmdarg1 ...)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   name(cmdname);
   ::init_args(cmd_args);

   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();

   va_list  ap;
   va_start(ap, cmdarg1);
   for (CmdArg * cmdarg = cmdarg1 ; cmdarg ; cmdarg = va_CmdArgP(ap)) {
      arg_list->add(cmdarg);
   }
   va_end(ap);
}


   // Constructor with CmdArgs
CmdLine::CmdLine(CmdArg * cmdarg1 ...)
   : cmd_parse_state(cmd_START_STATE),
     cmd_state(cmd_START_STATE),
     cmd_flags(DEFAULT_CMDFLAGS),
     cmd_status(CmdLine::NO_ERROR),
     cmd_nargs_parsed(0),
     cmd_name(NULL),
     cmd_description(NULL),
     cmd_matched_arg(NULL),
     cmd_args(NULL),
     cmd_err(NULL),
     cmd_quit_handler(NULL)
{
   if (cmdarg1 == NULL)  return;
   ::init_args(cmd_args);

   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();

   va_list  ap;
   va_start(ap, cmdarg1);
   for (CmdArg * cmdarg = cmdarg1 ; cmdarg ; cmdarg = va_CmdArgP(ap)) {
      arg_list->add(cmdarg);
   }
   va_end(ap);
}


   // Destructor
CmdLine::~CmdLine(void)
{
   delete  cmd_args;

#if (defined(msdos) || defined(os2))
   delete [] cmd_name;
#endif
}


// Set the name of the command
void
CmdLine::name(const char * progname)
{
   static  const char unknown_progname[] = "<unknown-program>" ;

#if (defined(msdos) || defined(os2))
   delete [] cmd_name;
#endif
   cmd_name = ::filebasename((progname) ? progname : unknown_progname);
}


// Print an error message prefix and return a reference to the
// error output stream for this command
ostream &
CmdLine::error(unsigned  quiet) const
{
   ostream * os = (cmd_err) ? cmd_err : &cerr ;
   if (cmd_name && *cmd_name && !quiet)  *os << cmd_name << ": " ;
   return  *os;
}


// Add an argument to the current list of CmdArgs
CmdLine &
CmdLine::append(CmdArg * cmdarg)
{
   CmdArgListListIter  iter(cmd_args);
   CmdArgList * arg_list = iter();
   arg_list->add(cmdarg);

   return  *this ;
}

// terminate parsing altogether
void
CmdLine::quit(int status) {
   if (cmd_quit_handler != NULL) {
      (*cmd_quit_handler)(status);
   } else {
      ::exit(status);
   }
}

//---------------------------------------------------------- CmdLineCmdArgIter

// Constructors and Destructors
CmdLineCmdArgIter::CmdLineCmdArgIter(CmdLine & cmd)
   : iter(NULL)
{
   if (cmd.cmd_args) {
      CmdArgListListIter  listlist_iter(cmd.cmd_args);
      CmdArgList  * list = listlist_iter();
      if (list)  iter = new CmdArgListIter(list);
   }
}

CmdLineCmdArgIter::CmdLineCmdArgIter(CmdLine * cmd)
   : iter(NULL)
{
   if (cmd->cmd_args) {
      CmdArgListListIter  listlist_iter(cmd->cmd_args);
      CmdArgList  * list = listlist_iter();
      if (list)  iter = new CmdArgListIter(list);
   }
}

CmdLineCmdArgIter::~CmdLineCmdArgIter(void)
{
   delete  iter;
}

// Return the current argument and advance to the next one.
// Returns NULL if we are already at the end of the list.
CmdArg *
CmdLineCmdArgIter::operator()(void)
{
   return  (iter) ? iter->operator()() : NULL ;
}

//-------------------------------------------------------- class CmdLineArgIter

CmdLineArgIter::CmdLineArgIter(void) {}

CmdLineArgIter::~CmdLineArgIter(void) {}

//----------------------------------------------------------- class CmdArgvIter

CmdArgvIter::~CmdArgvIter(void) {}

const char *
CmdArgvIter::operator()(void) {
   return  ((index != count) && (array[index])) ? array[index++] : 0 ;
}

int
CmdArgvIter::is_temporary(void) const { return  0; }

//--------------------------------------------------------- class CmdStrTokIter

static const char WHITESPACE[] = " \t\n\r\v\f" ;

// Constructor
CmdStrTokIter::CmdStrTokIter(const char * tokens, const char * delimiters)
   : tokstr(NULL), seps(NULL), token(NULL)
{
   reset(tokens, delimiters);
}

// Destructor
CmdStrTokIter::~CmdStrTokIter(void)
{
   delete  tokstr;
}

// Use a new string and a new set of delimiters
void
CmdStrTokIter::reset(const char * tokens, const char * delimiters)
{
   seps = delimiters;
   if (seps == NULL)  seps = WHITESPACE ;  // use default delimiters

   delete  tokstr;
   tokstr = NULL;
   token = NULL;
   if (tokens) {
      // Make a copy of the token-string (because ::strtok() modifies it)
      // and get the first token from the string
      tokstr = new char[::strlen(tokens) + 1] ;
      (void) ::strcpy(tokstr, tokens);
      token = ::strtok(tokstr, seps);
   }
}

// Iterator function -- operator()
//   Just use ::strtok to get the next token from the string
const char *
CmdStrTokIter::operator()(void)
{
   if (seps == NULL)  seps = WHITESPACE ;
   const char * result = token;
   if (token) token = ::strtok(NULL, seps);
   return  result;
}

// The storage that we return pointers to (in operator())
// always points to temporary space.
int
CmdStrTokIter::is_temporary(void) const
{
   return  1;
}

//-------------------------------------------------------- class CmdIstreamIter

const unsigned  CmdIstreamIter::MAX_LINE_LEN = 1024 ;

// Constructor
CmdIstreamIter::CmdIstreamIter(istream & input) : is(input), tok_iter(NULL)
{
}

// Destructor
CmdIstreamIter::~CmdIstreamIter(void)
{
   delete  tok_iter;
}

enum { c_COMMENT = '!' } ;

// Iterator function -- operator()
//
// What we do is this: for each line of text in the istream, we use
// a CmdStrTokIter to iterate over each token on the line.
//
// If the first non-white character on a line is c_COMMENT, then we
// consider the line to be a comment and we ignore it.
const char *
CmdIstreamIter::operator()(void)
{
   const char * result = NULL;
   if (tok_iter)  result = tok_iter->operator()();
   if (result)  return  result;
   if (! is)  return  NULL;

   char buf[CmdIstreamIter::MAX_LINE_LEN];
   do {
      *buf = '\0';
      is.getline(buf, sizeof(buf));
      char * ptr = buf;
      while (isspace(*ptr)) ++ptr;
      if (*ptr && (*ptr != c_COMMENT)) {
         if (tok_iter) {
            tok_iter->reset(ptr);
         } else {
            tok_iter = new CmdStrTokIter(ptr);
         }
         return  tok_iter->operator()();
      }
   } while (is);
   return  NULL;
}

// We use a CmdStrTokIterator that is always temporary, thus the
// the tokens we return are always in temporary storage
int
CmdIstreamIter::is_temporary(void) const
{
   return  1;
}

//---------------------------------------------------------------------- CmdArg

int  CmdArg::is_dummy(void) { return  0; }

// Copy-Constructor
CmdArg::CmdArg(const CmdArg & cp)
   : alloc_value_name(cp.alloc_value_name),
     arg_flags(cp.arg_flags),
     arg_syntax(cp.arg_syntax),
     arg_sequence(cp.arg_sequence),
     arg_char_name(cp.arg_char_name),
     arg_keyword_name(cp.arg_keyword_name),
     arg_value_name(cp.arg_value_name),
     arg_description(cp.arg_description)
{
   if (alloc_value_name) {
      char * val_name = new char[::strlen(cp.arg_value_name) + 1] ;
      ::strcpy((char *)val_name, cp.arg_value_name);
      arg_value_name = val_name;
   }
}

// Constructors
CmdArg::CmdArg(char         optchar,
               const char * keyword,
               const char * value,
               const char * description,
               unsigned     syntax_flags)
   : alloc_value_name(0),
     arg_flags(0),
     arg_syntax(syntax_flags),
     arg_sequence(0),
     arg_char_name(optchar),
     arg_keyword_name(keyword),
     arg_value_name(value),
     arg_description(description)
{
   parse_description();
   parse_value();
   adjust_syntax();
}


CmdArg::CmdArg(char         optchar,
               const char * keyword,
               const char * description,
               unsigned     syntax_flags)
   : alloc_value_name(0),
     arg_flags(0),
     arg_syntax(syntax_flags),
     arg_sequence(0),
     arg_char_name(optchar),
     arg_keyword_name(keyword),
     arg_value_name(NULL),
     arg_description(description)
{
   parse_description();
   adjust_syntax();
}


CmdArg::CmdArg(const char * value,
               const char * description,
               unsigned     syntax_flags)
   : alloc_value_name(0),
     arg_flags(0),
     arg_syntax(syntax_flags),
     arg_sequence(0),
     arg_char_name(0),
     arg_keyword_name(NULL),
     arg_value_name(value),
     arg_description(description)
{
   parse_description();
   parse_value();
   adjust_syntax();
}


// Destructor
CmdArg::~CmdArg(void)
{
   if (alloc_value_name)    delete [] (char *)arg_value_name;
}


//-------------------
// ^FUNCTION: adjust_syntax - adjust command argument syntax
//
// ^SYNOPSIS:
//    CmdArg::adjust_syntax(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//    This routine tries to "iron out" any inconsistencies (such as
//    conflicting syntax flags) in the way a command-argument is specified
//    and makes its best guess at what the user eally intended.
//
// ^REQUIREMENTS:
//    parse_value() and parse_description() must already have been called.
//
// ^SIDE-EFFECTS:
//    Modifies the argument syntax flags.
//    Modifies is keyword and value names if they are ""
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    Follow along in the code ...
//-^^----------------
void
CmdArg::adjust_syntax(void)
{
   static const char default_value_name[] = "value" ;

  // If the value is specified as both OPTIONAL and REQUIRED
  // then assume it is required.
  if ((arg_syntax & isVALREQ) && (arg_syntax & isVALOPT)) {
    arg_syntax &= ~isVALOPT ;
  }

  // If they said the argument was both STICKY and SEPARATE then
  // I dont know what to think just just ignore both of them.
  if ((arg_syntax & isVALSTICKY) && (arg_syntax & isVALSEP)) {
    arg_syntax &= ~(isVALSTICKY | isVALSEP);
  }

  // If a non-NULL, non-empty value-name was given but we werent
  // told that the argument takes a value, then assume that it
  // does take a value and that the value is required.
  if (arg_value_name && *arg_value_name && (! (arg_syntax & isVALTAKEN))) {
    arg_syntax |= isVALREQ;
  }

  // If a value is taken and the argument is positional, then
  // we need to make isREQ and isOPT consistent with isVALREQ
  // and isVALOPT
  if ((arg_syntax & isVALTAKEN) && (arg_syntax & isPOS)) {
    arg_syntax &= ~(isREQ | isOPT);
    if (arg_syntax & isVALREQ)  arg_syntax |= isREQ;
    if (arg_syntax & isVALOPT)  arg_syntax |= isOPT;
  }

  // If the keyword name is empty then just use NULL
  if (arg_keyword_name  &&  (! *arg_keyword_name)) {
    arg_keyword_name = NULL;
  }

  // If the value name is empty then just use NULL
  if (arg_value_name  &&  (! *arg_value_name)) {
    arg_value_name = NULL;
  }

  // If a value is taken but no value name was given,
  // then default the value name.
  if ((arg_syntax & isVALTAKEN) && (! arg_value_name)) {
    arg_value_name = default_value_name;
  }

  // If no keyword name or character name was given, then the
  // argument had better take a value and it must be positional
  if ((! arg_char_name) && (arg_keyword_name == NULL) &&
                          (! (arg_syntax & isPOS))) {
    if (arg_syntax & isVALTAKEN) {
       arg_syntax |= isPOS;
    } else {
       cerr << "*** Error: non-positional CmdArg "
            << "has no character or keyword name!\n"
            << "\t(error occurred in CmdArg constructor)" << endl ;
    }
  }
}


//-------------------
// ^FUNCTION: parse_description - parse the argument description string
//
// ^SYNOPSIS:
//    CmdLine::parse_description(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//    All we have to do is see if the first non-white character of
//    the description is string is ';'. If it is, then the argument
//    is a "hidden" argument and the description starts with the
//    next non-white character.
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    Modifies arg_description
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    Trivial.
//-^^----------------
enum { c_HIDDEN = ';', c_OPEN = '[', c_CLOSE = ']', c_LIST = '.' } ;

void
CmdArg::parse_description(void)
{
   if (arg_description == NULL)  return;
   while (isspace(*arg_description))  ++arg_description;
   if (*arg_description == c_HIDDEN) {
      arg_syntax |= isHIDDEN ;
      ++arg_description;
      while (isspace(*arg_description))  ++arg_description;
   }
}


//-------------------
// ^FUNCTION: parse_value - parse the argument value name
//
// ^SYNOPSIS:
//    CmdLine::parse_value(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//    This routine parses the argument value string. If the value name is
//    is enclosed between '[' and ']', then the value is optional (not
//    required) and we need to modify the arg_syntax flags. Also, if the
//    value name is suffixed by "..." then it means the value is a LIST
//    of values and we need to modify the arg_syntax flags.
//
// ^REQUIREMENTS:
//    This routine must be called BEFORE, adjust_syntax() is called/
//
// ^SIDE-EFFECTS:
//    Modifies the arg_value_name and the arg_syntax flags.
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    Its kind of hairy so follow along.
//-^^----------------
void
CmdArg::parse_value(void)
{
   const char * save_value = arg_value_name;
   int  brace = 0;
   int  errors = 0;

   // Skip whitespace as necessary and look for a '['
   while (isspace(*arg_value_name))  ++arg_value_name;
   if (*arg_value_name == c_OPEN) {
      ++brace;
      ++arg_value_name;
      while (isspace(*arg_value_name))  ++arg_value_name;
      arg_syntax &= ~isVALREQ;
      arg_syntax |= isVALOPT;
   }

   // Now that arg_value_name points to the beginning of the value,
   // lets find the end of it.
   const char * ptr = arg_value_name;
   while ((*ptr) && (! isspace(*ptr)) &&
          (*ptr != c_LIST) && (*ptr != c_CLOSE)) {
      ++ptr;
   }

   // See if we dont need to allocate a new string
   if ((! *ptr) && (save_value == arg_value_name))  return;

   // Copy the value name
   alloc_value_name = 1;
   int  len = (int) (ptr - arg_value_name);
   char * copied_value = new char[len + 1];
   (void) ::strncpy(copied_value, arg_value_name, len);
   copied_value[len] = '\0';
   arg_value_name = copied_value;

   // Did we end on a ']' ?
   if (*ptr == c_CLOSE) {
      if (! brace) {
         cerr << "Error: unmatched '" << char(c_CLOSE) << "'." << endl ;
         ++errors;
         arg_syntax &= ~isVALREQ;
         arg_syntax |= isVALOPT;
      }
      brace = 0;
      ++ptr;
   }

   // Skip whitespace and see if we are finished.
   while (isspace(*ptr))  ++ptr;
   if (! *ptr) {
      // Was there an unmatched ']'
      if (brace) {
         cerr << "Error: unmatched '" << char(c_OPEN) << "'." << endl ;
         ++errors;
      }
      if (errors) {
         cerr << "*** Syntax error in value \"" << save_value << "\".\n"
              << "\t(error occurred in CmdArg constructor)" << endl ;
      }
      return;
   }

   // Not done - we had better see a "..."
   if (::strncmp(ptr, "...", 3) != 0) {
      cerr << "Error: unexpected token \"" << ptr << "\"." << endl ;
      ++errors;
   } else {
      arg_syntax |= isLIST;
      ptr += 3;
      while (isspace(*ptr))  ++ptr;
      if (brace && (*ptr != c_CLOSE)) {
         cerr << "Error: unmatched '" << char(c_OPEN) << "'." << endl ;
         ++errors;
      } else {
        // If theres anything left (except ']') it's an error
        if (brace && (*ptr == c_CLOSE))  ++ptr;
        while (isspace(*ptr))  ++ptr;
        if (*ptr) {
           cerr << "Error: unexpected token \"" << ptr << "\"." << endl ;
           ++errors;
        }
      }
   }

   // Were there any errors?
   if (errors) {
      cerr << "*** Syntax error in value \"" << save_value << "\".\n"
           << "\t(error occurred in CmdArg constructor)" << endl ;
   }
}

//-------
// ^FUNCTION: CmdLine::prologue - initialize before parsing
//
// ^SYNOPSIS:
//    unsigned CmdLine::prologue(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//    Before we can begin parsing argument from the command-line, we need
//    to set (or reset) certain attributes of the CmdLine object. Among
//    other things, we need to reset its state and status, and we need to
//    reset the state of each of its arguments.
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    Modifies all parts of the CmdLine object and its arguments.
//
// ^RETURN-VALUE:
//    A combination of bitmasks of type CmdLine::CmdStatus corresponding to
//    what (if anything) went wrong.
//
// ^ALGORITHM:
//    Follow along - its not too complicated.
//-^^----
unsigned
CmdLine::prologue(void)
{
   // reset parse-specific attributes
   cmd_parse_state = cmd_START_STATE ;
   cmd_state = 0 ;
   cmd_status = NO_ERROR ;
   cmd_nargs_parsed = 0 ;

   // reset parse-specific attributes for each argument
   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         cmdarg->clear();
      }
   }

   return  cmd_status ;
}

//-------
// ^FUNCTION: CmdLine::epilogue - clean up after parsing
//
// ^SYNOPSIS:
//    unsigned CmdLine::epilogue(void)
//
// ^PARAMETERS:
//    None.
//
// ^DESCRIPTION:
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    Modifies the command-line obejct.
//
//    Prints messages on cerr (if QUIET is not set) if there are
//    missing required arguments or values.
//
//    Prints a usage message if there were syntax error.
//
//    Terminates program execution by calling quit(e_SYNTAX) if
//    (NO_ABORT is NOT set and the command-status is NOT NO_ERROR).
//
// ^RETURN-VALUE:
//    A combination of bitmasks of type CmdLine::CmdStatus corresponding to
//    what (if anything) went wrong.
//
// ^ALGORITHM:
//    - See if we left an argument dangling without a required value,
//    - Check for missing required arguments
//    - Print usage if required
//    - Exit if required
//-^^----
unsigned
CmdLine::epilogue(void)
{
   if (cmd_err == NULL)  cmd_err = &cerr;

   // see if we left an argument dangling without a value
   ck_need_val() ;

   // check for any missing required arguments
   cmd_status |= missing_args();

   // print usage if necessary
   if (cmd_status & ARG_MISSING) {
      usage();
      quit(e_SYNTAX);
   } else if (cmd_status && (! (cmd_flags & NO_ABORT))) {
      usage();
      quit(e_SYNTAX);
   }

   return  cmd_status ;
}

//-------------------
// ^FUNCTION: parse - parse arguments from an iterator
//
// ^SYNOPSIS:
//    parse(arg_iter, auto_processing)
//
// ^PARAMETERS:
//    CmdLineArgIter & arg_iter;
//    -- collection of string arguments from the command-line
//
//    int  auto_processing;
//    -- if this is NON-zero, then automatically call prologue() and
//       epilogue() to do pre- and post- processing.
//
// ^DESCRIPTION:
//    Parse all the argument in a given argument iterator.
//    If auto_processing is NON-zero then the programmer is
//    directly responsible for calling prologue() and epilogue().
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    - Uses up all remaining arguments in arg_iter
//    - Modifies the CmdLine
//
// ^RETURN-VALUE:
//    The resultant status of the CmdLine:
//
// ^ALGORITHM:
//    Trivial - just iterate through calling parse_arg.
//-^^----------------
unsigned
CmdLine::parse(CmdLineArgIter & arg_iter, int  auto_processing)
{
   // NOTE: If arg_iter.is_temporary() is TRUE then we MUST remember
   //       to set the CmdLine::TEMP flags before parsing (and put it
   //       back the way it was when we are finished.
   //
   if (auto_processing)  (void) prologue();
   unsigned  save_flags = cmd_flags;
   if (arg_iter.is_temporary())  cmd_flags |= TEMP;
   for (const char * arg = arg_iter() ; arg ; arg = arg_iter()) {
      (void) parse_arg(arg);
   }
   if (arg_iter.is_temporary())  cmd_flags = save_flags;
   if (auto_processing)  (void) epilogue();
   return  cmd_status ;
}

//-------
// ^FUNCTION: CmdLine::get_usage_level
//
// ^SYNOPSIS:
//    CmdLine::CmdUsageLevel CmdLine::get_usage_level(void)
//
// ^PARAMETERS:
//    NONE.
//
// ^DESCRIPTION:
//    Gets the usage_level that tells us how "verbose" we should be
//    when printing usage-messages. This usage_level is recorded in
//    the environment variable $USAGE_LEVEL. This variable may have the
//    following values:
//
//       0 : Dont print usage at all.
//       1 : Print a terse usage message (command-line syntax only).
//       2 : Print a verbose usage message (include argument descriptions).
//
//    If $USAGE_LEVEL is not defined or is empty, then the default
//    usage_level is 2.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    The usage level to use.
//
// ^ALGORITHM:
//    Read the usage_level from the environment and return it.
//-^^----
CmdLine::CmdUsageLevel
CmdLine::get_usage_level(void)
{
   long level;
   char * end_scan, * level_str = ::getenv("USAGE_LEVEL");

   if (level_str == NULL)  return  VERBOSE_USAGE ;
   if (*level_str == '\0') return  NO_USAGE ;

   level = ::strtol(level_str, &end_scan, 0);
   if (end_scan == level_str)  return  VERBOSE_USAGE ;

   switch(level) {
      case 0 :  return NO_USAGE ;
      case 1 :  return TERSE_USAGE ;
      default:  return VERBOSE_USAGE ;
   }
}

//-------
// ^FUNCTION: CmdLine::print_synopsis
//
// ^SYNOPSIS:
//    unsigned CmdLine::print_synopsis(syntax, os, cols)
//
// ^PARAMETERS:
//    CmdLine::CmdLineSyntax syntax;
//    -- the syntax to use (long-option, short-option, or both)
//       when printing the synopsis.
//
//    ostream & os;
//    -- where to print.
//
//    int cols;
//    -- the maximum width of a line.
//
// ^DESCRIPTION:
//    Print a command-line synopsis (the command-line syntax).
//    The synopsis should be printed to "os" using the desired syntax,
//    in lines that are no more than "cols" characters wide.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//     Prints on "os".
//
// ^RETURN-VALUE:
//     The length of the longest argument-buf that was printed.
//
// ^ALGORITHM:
//     It's kind of complicated so follow along!
//-^^----
unsigned
CmdLine::print_synopsis(CmdLine::CmdLineSyntax  syntax,
                        ostream    & os,
                        int          cols) const
{
   static  char  usg_prefix[] = "Usage: ";

   unsigned  ll, positionals, longest = 0;

   // first print the command name
   os << usg_prefix << cmd_name ;
   ll = (cmd_name ? ::strlen(cmd_name) : 0) + (sizeof(usg_prefix) - 1);

   // set margin so that we always start printing arguments in a column
   // that is *past* the command name.
   unsigned  margin = ll + 1;

   // print option-syntax followed by positional parameters
   int  first;
   char buf[256] ;
   for (positionals = 0 ; positionals < 2 ; positionals++) {
      first = 1;
      CmdArgListListIter  list_iter(cmd_args);
      for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
         CmdArgListIter  iter(alist);
         for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
            unsigned  len, pl;

            // don't display hidden arguments
            if (cmdarg->syntax() & CmdArg::isHIDDEN)  continue;
            if (!positionals  &&  (cmdarg->syntax() & CmdArg::isPOS))  continue;
            if (positionals  &&  !(cmdarg->syntax() & CmdArg::isPOS))  continue;

            // figure out how wide this parameter is (for printing)
            pl = len = fmt_arg(cmdarg, buf, sizeof(buf), syntax, VERBOSE_USAGE);
            if (! len)  continue;

            if (cmdarg->syntax() & CmdArg::isLIST)  pl -= 4 ;  // " ..."
            if (! (cmdarg->syntax() & CmdArg::isREQ))  pl -= 2 ;   // "[]"
            if (pl > longest)  longest = pl;

            //  Will this fit?
            if ((int)(ll + len + 1) > (cols - first)) {
               os << char('\n') ;
               os.width(margin);
               os << "" ;  // No - start a new line;
               ll = margin;
            } else {
               os << char(' ') ;  // Yes - just throw in a space
               ++ll;
            }
            ll += len;
            os << buf;

            first = 0;
         } //for each cmdarg
      } //for each arg-list
   } //for each parm-type
   os << endl ;

   return  longest ;
}


//-------
// ^FUNCTION: CmdLine::print_descriptions
//
// ^SYNOPSIS:
//    unsigned CmdLine::print_descriptions(syntax, os, cols, longest)
//
// ^PARAMETERS:
//    CmdLine::CmdLineSyntax syntax;
//    -- the syntax to use (long-option, short-option, or both)
//       when printing the synopsis.
//
//    ostream & os;
//    -- where to print.
//
//    int cols;
//    -- the maximum width of a line.
//
//    unsigned longest;
//    -- value returned by print_synopsis.
//
// ^DESCRIPTION:
//    Print a command argument descriptions (using the command-line syntax).
//    The descriptions should be printed to "os" using the desired syntax,
//    in lines that are no more than "cols" characters wide.
//
// ^REQUIREMENTS:
//    "longest" should correspond to a value returned by print_synopsis
//    that used the same "cmd" and syntax.
//
// ^SIDE-EFFECTS:
//     Prints on "os".
//
// ^RETURN-VALUE:
//     None.
//
// ^ALGORITHM:
//     Print the description for each argument.
//-^^----
void
CmdLine::print_descriptions(CmdLine::CmdLineSyntax   syntax,
                            ostream                & os,
                            int                      cols,
                            unsigned                 longest) const
{
   int positionals, options = 0;
   char buf[256];

   for (positionals = 0 ; positionals < 2 ; positionals++) {
      CmdArgListListIter  list_iter(cmd_args);
      for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
         CmdArgListIter  iter(alist);
         for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
            // don't display hidden arguments
            if (cmdarg->syntax() & CmdArg::isHIDDEN)  continue;
            if (!positionals  &&  (cmdarg->syntax() & CmdArg::isPOS))  continue;
            if (positionals  &&  !(cmdarg->syntax() & CmdArg::isPOS))  continue;
            if ( !options++ )   os << "Options/Arguments:\n" ;
            if (! fmt_arg(cmdarg, buf, sizeof(buf), syntax, TERSE_USAGE)) {
               continue;
            }
            const char * description = cmdarg->description();
            if ((description == NULL) || (! *description))  continue;
            strindent(os, cols, 8, buf, (longest + 2), description);
         } //for each cmdarg
      } //for each arg-list
   } //for each parm-type
}

//-------
// ^FUNCTION: CmdLine::usage - print command-usage
//
// ^SYNOPSIS:
//    void CmdLine::usage(os, usage_level);
//
// ^PARAMETERS:
//    ostream & os;
//    -- where to print.
//
//    CmdLine::CmdUsageLevel  usage_level;
//    -- verboseness to use.
//
// ^DESCRIPTION:
//    Print the usage for the given CmdLine object on "os".
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    Prints on "os".
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    - get the usage level.
//    - determine which syntax to use
//    - get the max-columns for "os".
//    - print synopsis if required.
//    - print descriptions if required.
//-^^----
ostream &
CmdLine::usage(ostream & os, CmdLine::CmdUsageLevel usage_level) const
{
   // get user-specified usage-level
   //   (if status is zero this must be an explicit request so force verbose)
   if (usage_level == DEFAULT_USAGE)  usage_level = get_usage_level();
   if (usage_level == NO_USAGE)  return  os;

   // determine syntax to use
   CmdLineSyntax  cmd_syntax = syntax() ;

   // get screen size (dont know how to do this yet)
   int  max_cols = 79;

   // print command-line synopsis
   unsigned  longest = print_synopsis(cmd_syntax, os, max_cols) ;
   if (usage_level == TERSE_USAGE)  return  os;

   // now print argument descriptions
   os << "\n" ;
   print_descriptions(cmd_syntax, os, max_cols, longest) ;

   // now print the command-description if there is one
   if (cmd_description && *cmd_description) {
      os << "\nDescription:" << endl;
      strindent(os, max_cols, 8, "", 0, cmd_description);
   }

   return  os;
}

ostream &
CmdLine::usage(CmdLine::CmdUsageLevel usage_level) const {
   return  usage(*cmd_err, usage_level);
}


//-------
// ^FUNCTION: CmdLine::handle_arg - compile the string value of an argument
//
// ^SYNOPSIS:
//    extern int CmdLine::handle_arg(cmdarg, arg);
//
// ^PARAMETERS:
//    CmdArg * cmdarg;
//    -- the matched argument whose value is to be "handled"
//
//    const char * & arg;
//    -- the string value for the argument (from the command-line).
//       upon exit, this will be NULL (if all of "arg" was used) or will
//       point to the first character or "arg" that was not used by the
//       argument's "compile" function.
//
// ^DESCRIPTION:
//    After we have matched an argument on the command-line to an argument
//    in the "cmd" object, we need to "handle" the value supplied for that
//    argument. This entails updating the state of the argument and calling
//    its "compile" function as well as updating the state of the command.
//
// ^REQUIREMENTS:
//    None that weren't covered in the PARAMETERS section.
//
// ^SIDE-EFFECTS:
//    - modifies the value pointed to by "arg"
//    - prints a message on stderr if "arg" is invalid and QUIET is NOT set.
//    - modifies the state of "cmd".
//    - modifies the "value" and "flags" of "cmdarg".
//
// ^RETURN-VALUE:
//    The value returned by calling the "compile" function associated
//    with the argument "cmdarg".
//
// ^ALGORITHM:
//    - if this is a cmdargUsage argument then print usage and call quit()
//    - call the operator() of "cmdarg" and save the result.
//    - if the above call returned SUCCESS then set the GIVEN and VALGIVEN
//      flags of the argument.
//    - update the parse_state of "cmd" if we were waiting for this value.
//    - if "cmdarg" corresponds to a LIST then set things up so that succeeding
//      arguments will be values for this "cmdarg"'s list.
//-^^----
int
CmdLine::handle_arg(CmdArg * cmdarg, const char * & arg)
{
   ++cmd_nargs_parsed;  // update the number of parsed args

   // call the argument compiler
   const char * save_arg = arg ;  // just in case someone forgets to set it
   int  bad_val = (*cmdarg)(arg, *this);
   if (! bad_val) {
      cmdarg->set(CmdArg::GIVEN) ;
      cmdarg->sequence(cmd_nargs_parsed) ;
      if (arg != save_arg) {
         cmdarg->set(CmdArg::VALGIVEN) ;
      }
   }

   // if we were waiting for a value - we just got it
   if (arg != save_arg) {
      if (cmdarg == cmd_matched_arg)  cmd_parse_state = cmd_START_STATE ;
   }

   // if this is a list - optional values may follow the one given
   if ((cmdarg->syntax() & CmdArg::isLIST) && (arg != save_arg)) {
      cmd_matched_arg = cmdarg ;
      cmd_parse_state = cmd_WANT_VAL ;
   }

   return  bad_val ;
}


//-------
// ^FUNCTION: CmdLine::ck_need_val - See if an argument needs a value
//
// ^SYNOPSIS:
//    extern void CmdLine::ck_needval(void)
//
// ^PARAMETERS:
//    NONE.
//
// ^DESCRIPTION:
//    We parse command-lines using something akin to a deterministic
//    finite state machine. Each argv[] element on the command-line is
//    considered a single input to the machine and we keep track of an
//    associated machine-state that tells us what to do next for a given
//    input.
//
//    In this function, we are merely trying to query the "state" of the
//    machine by asking it if it is expecting to see a value for an
//    argument that was matched in a previous argv[] element.
//
//    It is assumed that this function is called only after it has already
//    been determined that the current argv[] element is NOT an argument
//    value.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    - updates the "state" of the command.
//    - updates the "status" of the command.
//    - modifies the last matched argument if it takes an optional value.
//    - prints a message on stderr if cmd_QUIET is NOT set and we were
//      expecting a required value.
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    If we were expecting an optional value then
//    - set the GIVEN flag of the last arg we matched (DO NOT set VALGIVEN).
//    - call the compiler-function of the last-matched arg using NULL
//      as the argument (unless the arg is a LIST and VALGIVEN is set).
//    - reset the command-state
//    Else if we were expecting a required value then
//    - print a an error message if cmd_QUIET is not set
//    - set the command-status to VAL_MISSING
//    - reset the command-state
//    Endif
//-^^----
void
CmdLine::ck_need_val(void)
{
   const char * null_str = NULL;
   if (cmd_parse_state == cmd_WANT_VAL) {
      // argument was given but optional value was not
      cmd_matched_arg->set(CmdArg::GIVEN) ;
      if ((! (cmd_matched_arg->syntax() & CmdArg::isLIST)) ||
          (! (cmd_matched_arg->flags()  & CmdArg::VALGIVEN))) {
         (void) handle_arg(cmd_matched_arg, null_str) ;
      }
      cmd_parse_state = cmd_START_STATE ;
   } else if (cmd_parse_state == cmd_NEED_VAL) {
      // argument was given but required value was not
      if (! (cmd_flags & QUIET)) {
         arg_error("value required for", cmd_matched_arg) << "." << endl ;
      }
      cmd_status |= VAL_MISSING ;
      cmd_parse_state = cmd_START_STATE ;
   }
}


//-------
// ^FUNCTION: CmdLine::syntax - determine usage message syntax
//
// ^SYNOPSIS:
//    CmdLine::CmdLineSyntax CmdLine::syntax(void);
//
// ^PARAMETERS:
//
// ^DESCRIPTION:
//    One of the things we keep track of in the CmdLine object is whether
//    options and/or keywords (long-options) were used on the command-line.
//    If a command-line syntax error occurs and only options (keywords)
//    were used then the usage message will only contain option (keyword)
//    syntax. If BOTH were used or if usage was specifically requested via
//    a cmdargUsage option (which we also keep track of) then we want the
//    the usage message to contain the syntac for both options and keywords.
//
//    If neither options nor keywords were given (meaning only positional
//    parameters were used) then we only use option-syntax (for brevity).
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    The desired usage message syntax to use.
//
// ^ALGORITHM:
//    Trivial.
//-^^----
CmdLine::CmdLineSyntax
CmdLine::syntax(void) const
{
   if (cmd_flags & KWDS_ONLY) {
      return  cmd_KWDS_ONLY;
   } else if (cmd_flags & OPTS_ONLY) {
      return  cmd_OPTS_ONLY;
   } else if ((cmd_state & cmd_OPTIONS_USED) &&
              (cmd_state & cmd_KEYWORDS_USED)) {
      return  cmd_BOTH ;
   } else if (cmd_state & cmd_KEYWORDS_USED) {
      return  cmd_KWDS_ONLY ;
   } else {
      return  cmd_OPTS_ONLY ;
   }
}


//-------
// ^FUNCTION: CmdLine::missing_args - check for missing required arguments
//
// ^SYNOPSIS:
//    unsigned CmdLine::missing_args(void);
//
// ^PARAMETERS:
//
// ^DESCRIPTION:
//    This function checks to see if there is a required argument in the
//    CmdLine object that was NOT specified on the command.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    - modifies the status of "cmd".
//    - terminates execution by calling quit() if cmd_NOABORT is NOT
//      set and a required argument (that was not properly supplied by
//      the user) is not given.
//    - prints on stderr if an argument is missing and cmd_QUIET is NOT set.
//
// ^RETURN-VALUE:
//    The current value of the (possibly modified) command status. This is a
//    combination of bitmasks of type cmdline_flags_t defined in <cmdline.h>
//
// ^ALGORITHM:
//    Foreach argument in cmd
//       if argument is required and was not given
//          add ARG_MISSING to cmd-status
//       endif
//    endfor
//    return the current cmd-status
//-^^----
unsigned
CmdLine::missing_args(void)
{
   char buf[256];

   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         if (cmdarg->is_dummy())  continue;
         if ((cmdarg->syntax() & CmdArg::isREQ) &&
             (! (cmdarg->flags() & CmdArg::GIVEN)))
         {
            if (! (cmd_flags & QUIET)) {
               fmt_arg(cmdarg,  buf, sizeof(buf), syntax(), TERSE_USAGE);
               error() << buf << " required." << endl ;
            }
            if (cmd_status & ARG_MISSING) {
               // user didnt supply the missing argument
               return  cmd_status ;
            } else if ((! (cmd_flags & NO_ABORT)) && cmd_status) {
               // other problems
               return  cmd_status ;
            } else {
               cmd_status |= ARG_MISSING ;
            }
         } //if
      } //for iter
   } //for list_iter

   return  cmd_status ;
}


//-------
// ^FUNCTION: CmdLine::opt_match - attempt to match on option
//
// ^SYNOPSIS:
//    CmdArg * CmdLine::opt_match(optchar);
//
// ^PARAMETERS:
//    char optchar;
//    -- a possible option for "cmd"
//
// ^DESCRIPTION:
//    If "cmd" has an argument that has "optchar" as a single-character
//    option then this function will find and return that argument.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    If we find a match, then we return a pointer to its argdesc,
//    otherwise we return NULL.
//
// ^ALGORITHM:
//    Trivial.
//-^^----
CmdArg *
CmdLine::opt_match(char optchar) const
{
   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         if (cmdarg->is_dummy())  continue;
         if (optchar == cmdarg->char_name()) {
            // exact match
            return  cmdarg ;
         } else if (cmd_flags & ANY_CASE_OPTS) {
            // case-insensitive match
            if (TO_LOWER(optchar) == TO_LOWER(cmdarg->char_name())) {
               return  cmdarg ;
            }
         }
      } //for iter
   } //for list_iter
   return  NULL ;
}

//-------
// ^FUNCTION: CmdLine::kwd_match - purpose
//
// ^SYNOPSIS:
//    extern CmdArg * CmdLine::kwd_match(kwd, len, is_ambiguous, match_value);
//
// ^PARAMETERS:
//    const char * kwd;
//    -- a possible kewyord of "cmd"
//
//    int len;
//    -- the number of character of "kwd" to consider (< 0 if all characters
//       of "kwd" should be used).
//
//    int & is_ambiguous;
//    -- upon return, the value pointed to is set to 1 if the keyword
//       matches more than 1 keyword in "cmd"; Otherwise it is set to 0.
//
//    int match_value;
//    -- if this is non-zero, then if a keyword_name is NULL use the 
//       value_name instead.
//
// ^DESCRIPTION:
//    If "cmd" has an argument that matches "kwd" as a kewyord
//    then this function will find and return that argument.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    If we find a match, then we return a pointer to its argdesc,
//    otherwise we return NULL.
//
// ^ALGORITHM:
//    Set is_ambigous to 0.
//    For each argument in cmd
//       if argument's keyword-name matches kwd then
//          if this was an exact match then return this argument
//          else if we had a previous partial match of this argument then
//             if argument is a default argument return the previous match
//             else set is_ambiguous to 1 and return NULL
//          else remember we had a partial match here and keep trying
//          endif
//       endif
//    end for
//    if we has a partial match and we get to here then it is NOT ambiguous do
//       go ahead and return the argument we matched.
//-^^----
CmdArg *
CmdLine::kwd_match(const char * kwd,
                   int          len,
                   int &        is_ambiguous,
                   int          match_value) const
{
   CmdArg * matched = NULL;

   is_ambiguous = 0 ;

   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         if (cmdarg->is_dummy())  continue;

         // attempt to match this keyword
         strmatch_t  result  = str_NONE ;
         const char * source = cmdarg->keyword_name();
         if (source && *source) {
            result = strmatch(source, kwd, len) ;
         } else if (match_value) {
            result = strmatch(cmdarg->value_name(), kwd, len) ;
         }

         if (result == str_EXACT) {
            return  cmdarg ;
         } else if (result == str_PARTIAL) {
            if (matched) {
               is_ambiguous = 1 ;
               return  NULL ;  // ambiguous keyword
            }
            matched = cmdarg ;  // we matched this one
         }
      } //for iter
      if (matched) break;
   } //for list_iter
   return  matched ;
}

//-------
// ^FUNCTION: CmdLine::pos_match - match a positional argument
//
// ^SYNOPSIS:
//    CmdArg * CmdLine::pos_match(void)
//
// ^PARAMETERS:
//
// ^DESCRIPTION:
//    If "cmd" has an positional argument that has not yet been given,
//    or that corresponds to a list, then this function will find and
//    return the first such argument.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    If we find a match, then we return a pointer to its argument,
//    otherwise we return NULL.
//
// ^ALGORITHM:
//    First look for the first unmatched positional argument!!
//    If there aren't any, then look for the LAST positional list!
//-^^----
CmdArg *
CmdLine::pos_match(void) const
{
   CmdArg * last_pos_list = NULL;
   CmdArgListListIter  list_iter(cmd_args);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter  iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         if (cmdarg->is_dummy())  continue;
         if (cmdarg->syntax() & CmdArg::isPOS) {
            if (! (cmdarg->flags() & CmdArg::GIVEN)) {
               return  cmdarg ;
            } else if (cmdarg->flags() & CmdArg::isLIST) {
               last_pos_list = cmdarg;
            }
         }
      } //for iter
   } //for list_iter
   return  last_pos_list ;
}

//-------
// ^FUNCTION: CmdLine::parse_option - parse a Unix option
//
// ^SYNOPSIS:
//    unsigned CmdLine::parse_option(arg);
//
// ^PARAMETERS:
//    const char * arg;
//    -- the command-line argument containing the prospective option
//
// ^DESCRIPTION:
//    This routine will attempt to "handle" all options specified in
//    the string "arg". For each option found, its compile-function
//    is called and the corresponding state of both the command
//    and of the matched option(s) is (are) updated.
//
// ^REQUIREMENTS:
//    "arg" should point past any leading option prefix (such as "-").
//
// ^SIDE-EFFECTS:
//    "cmd" is modified accordingly as each option is parsed (as are its
//    constituent arguments). If there are syntax errors then error messages
//    are printed if QUIET is NOT set.
//
// ^RETURN-VALUE:
//    NO_ERROR is returned if no errors were encountered. Otherwise the
//    return value is a combination of bitmasks of type CmdLine::CmdStatus
//    (defined in <cmdline.h>) indicating all the problems that occurred.
//
// ^ALGORITHM:
//    see if we left an option dangling without parsing its value.
//    for each option bundled in "arg"
//       try to match the option
//       if no match issue a message (unless QUIET is set)
//       else
//          if the option takes NO argument than handle that
//          else if the option takes an argument
//             if the rest or arg is not empty then
//                call the option's compile-function using the rest of
//                                                    arg as the value.
//                skip past whatever portion of value was used
//             else
//                update the state of the command to show that we are expecting
//                       to see the value for this option as the next argument.
//             endif
//          endif
//          update the state of the argument.
//       endif
//    endfor
//-^^----
unsigned
CmdLine::parse_option(const char * arg)
{
   const  char * save_arg = arg;
   unsigned  save_flags = 0, rc = 0 ;
   CmdArg * cmdarg = NULL;
   int  bad_val;

   // see if we left an argument dangling without a value
   ck_need_val() ;

   do {  // loop over bundled options
      cmdarg = opt_match(*arg);
      if (cmdarg == NULL) {
         // If we were in the middle of a guess - sorry no cigar, otherwise
         // guess that maybe this is a keyword and not an keyword.
         //
         if (cmd_state & cmd_GUESSING) {
            if (arg == save_arg)  return  BAD_OPTION;
         } else {
            if (! (cmd_flags & NO_GUESSING)) {
               cmd_state |= cmd_GUESSING;
               rc = parse_keyword(arg);
               cmd_state &= ~cmd_GUESSING;
               if (rc != BAD_KEYWORD)  return  rc;
            }
         }
         if (! (cmd_flags & QUIET)) {
            error() << "unknown option \"" << OptionPrefix() << char(*arg)
                    << "\"." << endl ;
         }
         rc |= BAD_OPTION ;
         ++arg ;  // skip bad option
         continue ;
      }
      ++arg ;  // skip past option character
      save_flags = cmdarg->flags() ;
      cmdarg->clear();
      cmdarg->set(CmdArg::OPTION) ;
      if ((! *arg) && (cmdarg->syntax() & CmdArg::isVALTAKEN)) {
         // End of string -- value must be in next arg
         // Save this cmdarg-pointer for later and set the parse_state to
         // indicate that we are expecting a value.
         //

         if (cmdarg->syntax() & CmdArg::isVALSTICKY) {
            // If this argument is sticky we already missed our chance
            // at seeing a value.
            //
            if (cmdarg->syntax() & CmdArg::isVALREQ) {
               if (! (cmd_flags & QUIET)) {
                  error() << "value required in same argument for "
                          << OptionPrefix() << char(cmdarg->char_name())
                          << " option." << endl;
               }
               rc |= (VAL_MISSING | VAL_NOTSTICKY) ;
               cmdarg->flags(save_flags);
            } else {
               // The value is optional - set the GIVEN flag and call
               // handle_arg with NULL (and check the result).
               //
               const char * null_str = NULL;
               cmdarg->set(CmdArg::GIVEN) ;
               cmd_parse_state = cmd_START_STATE ;
               bad_val = handle_arg(cmdarg, null_str);
               if (bad_val) {
                  if (! (cmd_flags & QUIET)) {
                     arg_error("bad value for", cmdarg) << "." << endl ;
                  }
                  rc |= BAD_VALUE ;
                  cmdarg->flags(save_flags);
               }
            }
         } else {
            // Wait for the value to show up next time around
            cmdarg->set(CmdArg::GIVEN) ;
            cmd_matched_arg = cmdarg ;
            cmd_parse_state = cmd_WANT_VAL ;
            if (cmdarg->syntax() & CmdArg::isVALREQ) {
               cmd_parse_state += cmd_TOK_REQUIRED ;
            }
         }
         return  rc ;
      }

      // If this option is an isVALSEP and "arg" is not-empty then we
      // have an error.
      //
      if ((cmdarg->syntax() & CmdArg::isVALTAKEN) &&
          (cmdarg->syntax() & CmdArg::isVALSEP)) {
         if (! (cmd_flags & QUIET)) {
            error() << "value required in separate argument for "
                    << OptionPrefix() << char(cmdarg->char_name())
                    << " option." << endl;
         }
         rc |= (VAL_MISSING | VAL_NOTSEP) ;
         cmdarg->flags(save_flags);
         return  rc;
      } else {
         // handle the option
         const char * save_arg = arg;
         bad_val = handle_arg(cmdarg, arg);
         if (bad_val) {
            if (! (cmd_flags & QUIET)) {
               arg_error("bad value for", cmdarg) << "." << endl ;
            }
            rc |= BAD_VALUE ;
            cmdarg->flags(save_flags);
         }
         cmdarg->set(CmdArg::GIVEN);
         if (arg != save_arg)  cmdarg->set(CmdArg::VALGIVEN);
      }
   } while (arg && *arg) ;

   return  rc ;
}


//-------
// ^FUNCTION: CmdLine::parse_keyword - parse a Unix keyword
//
// ^SYNOPSIS:
//    unsigned CmdLine::parse_keyword(arg);
//
// ^PARAMETERS:
//    const char * arg;
//    -- the command-line argument containing the prospective keyword
//
// ^DESCRIPTION:
//    This routine will attempt to "handle" the keyword specified in
//    the string "arg". For any keyword found, its compile-function
//    is called and the corresponding state of both the command
//    and of the matched keyword(s) is (are) updated.
//
// ^REQUIREMENTS:
//    "arg" should point past any leading keyword prefix (such as "--").
//
// ^SIDE-EFFECTS:
//    "cmd" is modified accordingly as the keyword is parsed (as are its
//    constituent arguments). If there are syntax errors then error messages
//    are printed if QUIET is NOT set.
//
// ^RETURN-VALUE:
//    NO_ERROR is returned if no errors were encountered. Otherwise the
//    return value is a combination of bitmasks of type CmdLine::CmdStatus
//    (defined in <cmdline.h>) indicating all the problems that occurred.
//
// ^ALGORITHM:
//    see if we left an option dangling without parsing its value.
//    look for a possible value for this keyword denoted by ':' or '='
//    try to match "arg" as a keyword
//    if no match issue a message (unless QUIET is set)
//    else
//       if the keyword takes NO argument than handle that
//       else if the keyword takes an argument
//          if a value was found "arg"
//             call the keyword's compile-function with the value found.
//          else
//             update the state of the command to show that we are expecting
//                    to see the value for this option as the next argument.
//          endif
//       endif
//       update the state of the argument.
//    endif
//-^^----
unsigned
CmdLine::parse_keyword(const char * arg)
{
   unsigned  save_flags = 0, rc = 0 ;
   CmdArg * cmdarg = NULL ;
   int  ambiguous = 0, len = -1, bad_val;
   const char * val = NULL ;

   int  plus = (cmd_flags & ALLOW_PLUS) ;  // Can we use "+"?

   // see if we left an argument dangling without a value
   ck_need_val() ;

   // If there is a value with this argument, get it now!
   val = ::strpbrk(arg, ":=") ;
   if (val) {
      len = val - arg ;
      ++val ;
   }

   cmdarg = kwd_match(arg, len, ambiguous);
   if (cmdarg == NULL) {
      // If we were in the middle of a guess - sorry no cigar, otherwise
      // guess that maybe this is an option and not a keyword.
      //
      if (cmd_state & cmd_GUESSING) {
         return  BAD_KEYWORD;
      } else if ((! ambiguous) || (len == 1)) {
         if (! (cmd_flags & NO_GUESSING)) {
            cmd_state |= cmd_GUESSING;
            rc = parse_option(arg);
            cmd_state &= ~cmd_GUESSING;
            if (rc != BAD_OPTION)  return  rc;
         }
      }
      if (! (cmd_flags & QUIET)) {
         error() << ((ambiguous) ? "ambiguous" : "unknown") << " option "
                 << "\"" << ((cmd_flags & KWDS_ONLY) ? OptionPrefix()
                                                     : KeywordPrefix(plus))
                 << arg << "\"." << endl ;
      }
      rc |= ((ambiguous) ? KWD_AMBIGUOUS : BAD_KEYWORD) ;
      return  rc ;
   }

   save_flags = cmdarg->flags() ;
   cmdarg->clear();
   cmdarg->set(CmdArg::KEYWORD) ;
   if ((cmdarg->syntax() & CmdArg::isVALTAKEN) && (val == NULL)) {
      // Value must be in the next argument.
      // Save this cmdarg for later and indicate that we are
      // expecting a value.
      //
      if (cmdarg->syntax() & CmdArg::isVALSTICKY) {
         // If this argument is sticky we already missed our chance
         // at seeing a value.
         //
         if (cmdarg->syntax() & CmdArg::isVALREQ) {
            if (! (cmd_flags & QUIET)) {
               error() << "value required in same argument for "
                       << ((cmd_flags & KWDS_ONLY) ? OptionPrefix()
                                                   : KeywordPrefix(plus))
                       << cmdarg->keyword_name() << " option." << endl;
            }
            rc |= (VAL_MISSING | VAL_NOTSTICKY) ;
            cmdarg->flags(save_flags);
         } else {
            // The value is optional - set the GIVEN flag and call
            // handle_arg with NULL (and check the result).
            //
            const char * null_str = NULL;
            cmdarg->set(CmdArg::GIVEN) ;
            cmd_parse_state = cmd_START_STATE ;
            bad_val = handle_arg(cmdarg, null_str);
            if (bad_val) {
               if (! (cmd_flags & QUIET)) {
                  arg_error("bad value for", cmdarg) << "." << endl ;
               }
               rc |= BAD_VALUE ;
               cmdarg->flags(save_flags);
            }
         }
      } else {
         // Wait for the value to show up next time around
         cmdarg->set(CmdArg::GIVEN) ;
         cmd_matched_arg = cmdarg ;
         cmd_parse_state = cmd_WANT_VAL ;
         if (cmdarg->syntax() & CmdArg::isVALREQ) {
            cmd_parse_state += cmd_TOK_REQUIRED ;
         }
      }
      return  rc ;
   }

   // If this option is an isVALSEP and "val" is not-NULL then we
   // have an error.
   //
   if (val  &&  (cmdarg->syntax() & CmdArg::isVALTAKEN) &&
       (cmdarg->syntax() & CmdArg::isVALSEP)) {
      if (! (cmd_flags & QUIET)) {
         error() << "value required in separate argument for "
                 << ((cmd_flags & KWDS_ONLY) ? OptionPrefix()
                                             : KeywordPrefix(plus))
                 << cmdarg->keyword_name() << " option." << endl;
      }
      rc |= (VAL_MISSING | VAL_NOTSEP) ;
      cmdarg->flags(save_flags);
      return  rc;
   }
   // handle the keyword
   bad_val = handle_arg(cmdarg, val);
   if (bad_val) {
      if (! (cmd_flags & QUIET)) {
         arg_error("bad value for", cmdarg) << "." << endl ;
      }
      rc |= BAD_VALUE ;
      cmdarg->flags(save_flags);
   }

   return  rc ;
}


//-------
// ^FUNCTION: CmdLine::parse_value - parse a Unix value
//
// ^SYNOPSIS:
//    unsigned CmdLine::parse_value(arg);
//
// ^PARAMETERS:
//    const char * arg;
//    -- the command-line argument containing the prospective value
//
// ^DESCRIPTION:
//    This routine will attempt to "handle" the value specified in
//    the string "arg". The compile-function of the corresponding
//    argument-value is called and the corresponding state of both
//    the command and of the matched option(s) is (are) updated.
//    If the value corresponds to a multi-valued argument, then that
//    is handled here.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    "cmd" is modified accordingly for the value that is parsed (as are its
//    constituent arguments). If there are syntax errors then error messages
//    are printed if QUIET is NOT set.
//
// ^RETURN-VALUE:
//    NO_ERROR is returned if no errors were encountered. Otherwise the
//    return value is a combination of bitmasks of type CmdLine::CmdStatus
//    (defined in <cmdline.h>) indicating all the problems that occurred.
//
// ^ALGORITHM:
//    If the command-state says we are waiting for the value of an option
//       find the option that we matched last
//    else
//       match the next positional parameter in "cmd"
//       if there isnt one issue a "too many args" message
//               (unless cmd_QUIETs is set) and return.
//    endif
//    handle the given value and update the argument and command states.
//-^^----
unsigned
CmdLine::parse_value(const char * arg)
{
   unsigned  save_flags = 0, rc = 0 ;
   int  bad_val;
   CmdArg * cmdarg = NULL;

   if (cmd_parse_state & cmd_WANT_VAL) {
      if (cmd_matched_arg == NULL) {
         cerr << "*** Internal error in class CmdLine.\n"
              << "\tparse-state is inconsistent with last-matched-arg."
              << endl ;
         ::exit(e_INTERNAL);
      }
      // get back the cmdarg that we saved for later
      // - here is the value it was expecting
      //
      cmdarg = cmd_matched_arg ;
      save_flags = cmdarg->flags() ;
   } else {
      // argument is positional - find out which one it is
      cmdarg = pos_match() ;
      if (cmdarg == NULL) {
         if (! (cmd_flags & QUIET)) {
            error() << "too many arguments given." << endl ;
         }
         rc |= TOO_MANY_ARGS ;
         return  rc ;
      }
      save_flags = cmdarg->flags() ;
      cmdarg->clear();
      cmdarg->set(CmdArg::POSITIONAL) ;
      if (cmd_flags & OPTS_FIRST) {
         cmd_state |= cmd_END_OF_OPTIONS ;
      }
   }

   // handle this value
   cmdarg->set(CmdArg::VALSEP) ;
   bad_val = handle_arg(cmdarg, arg);
   if (bad_val) {
      if (! (cmd_flags & QUIET)) {
         arg_error("bad value for", cmdarg) << "." << endl ;
      }
      rc |= BAD_VALUE ;
      cmdarg->flags(save_flags);
      if (! (cmdarg->syntax() & CmdArg::isLIST)) {
         cmd_parse_state = cmd_START_STATE;
      }
   }

   // If the value was okay and we were requiring a value, then
   // a value is no longer required.
   //
   if ((! bad_val) && (cmdarg->syntax() & CmdArg::isLIST)) {
      cmd_parse_state &= ~cmd_TOK_REQUIRED ;
   }

   return  rc ;
}


//-------
// ^FUNCTION: CmdLine::parse_arg - parse an argv[] element unix-style
//
// ^SYNOPSIS:
//    unsigned CmdLine::parse_arg(arg)
//
// ^PARAMETERS:
//    const char * arg;
//    -- an argument string (argv[] element) from the command-line
//
// ^DESCRIPTION:
//    This routine will determine whether "arg" is an option, a long-option,
//    or a value and call the appropriate parse_xxxx function defined above.
//
// ^REQUIREMENTS:
//
// ^SIDE-EFFECTS:
//    "cmd" is modified accordingly for the string that is parsed (as are its
//    constituent arguments). If there are syntax errors then error messages
//    are printed if QUIET is NOT set.
//
// ^RETURN-VALUE:
//    NO_ERROR is returned if no errors were encountered. Otherwise the
//    return value is a combination of bitmasks of type CmdLine::CmdStatus
//    (defined in <cmdline.h>) indicating all the problems that occurred.
//
// ^ALGORITHM:
//    if we are expecting a required value
//       call parse_value()
//    else if "arg" is an option
//       skip past the option prefix
//       call parse_option()
//    else if "arg" is a keyword
//       skip past the kewyord prefix
//       call parse_keyword()
//    else if "arg" is "--" (meaning end of options)
//       see that we didnt leave an option dangling without a value
//       indicate end-of-options in the command-state
//    else
//       call parse_value()
//    endif
//-^^----
unsigned
CmdLine::parse_arg(const char * arg)
{
   if (arg == NULL)  return  cmd_status ;

   int  plus = (cmd_flags & ALLOW_PLUS) ;  // Can we use "+"?

   if (cmd_parse_state & cmd_TOK_REQUIRED) {
      // If a required value is expected, then this argument MUST be
      // the value (even if it looks like an option
      //
      cmd_status |= parse_value(arg) ;
   } else if (isOPTION(arg) && (! (cmd_state & cmd_END_OF_OPTIONS))) {
      ++arg ;  // skip '-' option character
      if (cmd_flags & KWDS_ONLY) {
         cmd_state  |= cmd_KEYWORDS_USED ;
         cmd_status |=  parse_keyword(arg) ;
      } else {
         cmd_state  |= cmd_OPTIONS_USED ;
         cmd_status |=  parse_option(arg) ;
      }
   } else if ((! (cmd_flags & OPTS_ONLY))
              && isKEYWORD(arg, plus) && (! (cmd_state & cmd_END_OF_OPTIONS))) {
      cmd_state |= cmd_KEYWORDS_USED ;
      if (*arg == '+') {
         ++arg ;  // skip over '+' keyword prefix
      } else {
         arg += 2 ;  // skip over '--' keyword prefix
      }
      cmd_status |= parse_keyword(arg) ;
   } else if (isENDOPTIONS(arg) && (! (cmd_state & cmd_END_OF_OPTIONS))) {
      cmd_state |= cmd_END_OF_OPTIONS ;
      // see if we left an argument dangling without a value
      ck_need_val() ;
   } else {
      cmd_status |= parse_value(arg) ;
   }

   return  cmd_status ;
}

//-------
// ^FUNCTION: CmdLine::arg_error - format an argument for error messages
//
// ^SYNOPSIS:
//    ostream & arg_error(error_str, cmdarg);
//
// ^PARAMETERS:
//    const char * error_str;
//    -- the problem with the argument
//
//    const CmdArg * cmdarg;
//    -- the argument to be formatted
//
// ^DESCRIPTION:
//    This function will write to "os" the argument corresponding to
//    "cmdarg" as we would like it to appear in error messages that pertain
//    to this argument.
//
// ^REQUIREMENTS:
//    None.
//
// ^SIDE-EFFECTS:
//    writes to "os"
//
// ^RETURN-VALUE:
//    A reference to os.
//
// ^ALGORITHM:
//    Pretty straightforward, just print to os the way we
//    want the argument to appear in usage messages.
//-^^----
ostream &
CmdLine::arg_error(const char * error_str, const CmdArg * cmdarg) const
{
   int  plus = (cmd_flags & ALLOW_PLUS) ;  // Can we use "+"?

   ostream & os = error() << error_str << char(' ') ;

   if (cmdarg->flags() & CmdArg::GIVEN) {
       if (cmdarg->flags() & CmdArg::KEYWORD) {
          os << ((cmd_flags & KWDS_ONLY) ? OptionPrefix()
                                         : KeywordPrefix(plus))
             << cmdarg->keyword_name() << " option" ;
       } else if (cmdarg->flags() & CmdArg::OPTION) {
          os << OptionPrefix() << (char)cmdarg->char_name() << " option" ;
       } else {
          os << cmdarg->value_name() << " argument" ;
       }
   } else {
       if (cmdarg->syntax() & CmdArg::isPOS) {
          os << cmdarg->value_name() << " argument" ;
       } else {
          if (cmd_flags & KWDS_ONLY) {
             os << OptionPrefix() << cmdarg->keyword_name() << " option" ;
          } else {
             os << OptionPrefix() << (char)cmdarg->char_name() << " option" ;
          }
       }
   }
   return  os;
}


//-------
// ^FUNCTION: CmdLine::fmt_arg - format an argument for usage messages
//
// ^SYNOPSIS:
//    unsigned CmdLine::fmt_arg(cmdarg, buf, bufsize, syntax, level);
//
// ^PARAMETERS:
//    const CmdArg * cmdarg;
//    -- the argument to be formatted
//
//    char * buf;
//    -- where to print the formatted result
//
//    unsigned bufsize;
//    -- number of bytes allocated for buf.
//
//    CmdLine::CmdLineSyntax syntax;
//    -- the syntax to use (option, long-option, or both).
//
//    CmdLine::CmdUsageLevel;
//    -- the usage-level corresponding to this portion of the
//       usage message.
//
// ^DESCRIPTION:
//    This function will write into "buf" the argument corresponding to
//    "cmdarg" as we would like it to appear in usage messages.
//
// ^REQUIREMENTS:
//    "buf" must be large enough to hold the result.
//
// ^SIDE-EFFECTS:
//    writes to "buf"
//
// ^RETURN-VALUE:
//    the length of the formatted result.
//
// ^ALGORITHM:
//    Its kind of tedious so follow along.
//-^^----
unsigned
CmdLine::fmt_arg(const CmdArg           * cmdarg,
                 char                   * buf,
                 unsigned                 bufsize,
                 CmdLine::CmdLineSyntax   syntax,
                 CmdLine::CmdUsageLevel   level) const
{
   ostrstream  oss(buf, bufsize);
   *buf = '\0';

   int  plus = (cmd_flags & ALLOW_PLUS) ;  // Can we use "+"?
   char optchar = cmdarg->char_name();
   const char * keyword = cmdarg->keyword_name();

   // Need to adjust the syntax if optchar or keyword is empty
   if ((! (cmdarg->syntax() & CmdArg::isPOS)) &&
       ((! optchar) || (keyword == NULL))) {
      if (keyword == NULL) {
         if ((cmd_flags & KWDS_ONLY) && (cmd_flags & NO_GUESSING)) {
            return  0;
         } else {
            syntax = cmd_OPTS_ONLY;
         }
      }
      if (! optchar) {
         if ((cmd_flags & OPTS_ONLY) && (cmd_flags & NO_GUESSING)) {
            return  0;
         } else {
            syntax = cmd_KWDS_ONLY;
         }
      }
   }

   // If the argument is optional - print the leading '['
   if ((level == VERBOSE_USAGE) && (! (cmdarg->syntax() & CmdArg::isREQ))) {
      oss << char('[') ;
   }

   // If we have a sticky-argument and usage is cmd_BOTH then it gets
   // really hairy so we just treat this as a special case right here
   // and now.
   //
   if ((syntax == cmd_BOTH) &&
       (! (cmdarg->syntax() & CmdArg::isPOS)) &&
       (cmdarg->syntax() & CmdArg::isVALTAKEN) &&
       (cmdarg->syntax() & CmdArg::isVALSTICKY))
   {
      if (cmdarg->syntax() & CmdArg::isVALOPT) {
         oss << OptionPrefix() << char(optchar) << char('[')
             << cmdarg->value_name() << "]|" << KeywordPrefix(plus)
             << keyword << "[=" << cmdarg->value_name() << char(']') ;
      } else {
         oss << OptionPrefix() << optchar << cmdarg->value_name()
             << char('|') << KeywordPrefix(plus) << keyword << char('=')
             << cmdarg->value_name() ;
      }
      if ((level == VERBOSE_USAGE) && (cmdarg->syntax() & CmdArg::isLIST)) {
         oss << " ..." ;
      }
      if ((level == VERBOSE_USAGE) && (! (cmdarg->syntax() & CmdArg::isREQ))) {
         oss << char(']') ;
      }
      oss << ends ;
      return  (oss.pcount() - 1);
   }

   if (! (cmdarg->syntax() & CmdArg::isPOS)) {
      switch(syntax) {
         case cmd_OPTS_ONLY :
            oss << OptionPrefix() << char(optchar) ;
            break ;

         case cmd_KWDS_ONLY :
            oss << ((cmd_flags & KWDS_ONLY) ? OptionPrefix()
                                            : KeywordPrefix(plus)) << keyword ;
            break ;

         case cmd_BOTH :
            oss << OptionPrefix() << char(optchar) << char('|')
                << KeywordPrefix(plus) << keyword ;
            break ;

         default :
            cerr << "*** Internal error in class CmdLine.\n"
                 << "\tunknown CmdLineSyntax value (" << int(syntax) << ")."
                 << endl ;
            ::exit(e_INTERNAL);
      } //switch
      if (cmdarg->syntax() & CmdArg::isVALTAKEN) {
         if (! (cmdarg->syntax() & CmdArg::isVALSTICKY)) {
            oss << char(' ') ;
         }
      }
   }

   // If the argument takes a value then print the value
   if (cmdarg->syntax() & CmdArg::isVALTAKEN) {
      if ((! (cmdarg->syntax() & CmdArg::isPOS)) &&
          (cmdarg->syntax() & CmdArg::isVALOPT))
      {
         oss << char('[') ;
      }
      if (cmdarg->syntax() & CmdArg::isVALSTICKY) {
         if (syntax == cmd_KWDS_ONLY)  oss << char('=') ;
      }
      oss << cmdarg->value_name() ;
      if ((level == VERBOSE_USAGE) && (cmdarg->syntax() & CmdArg::isLIST)) {
         oss << " ..." ;
      }
      if ((! (cmdarg->syntax() & CmdArg::isPOS)) &&
          (cmdarg->syntax() & CmdArg::isVALOPT))
      {
         oss << char(']') ;
      }
   }

   if ((level == VERBOSE_USAGE) && (! (cmdarg->syntax() & CmdArg::isREQ))) {
      oss << char(']') ;
   }
   oss << ends ;

   return  (oss.pcount() - 1) ;
}


//-------
// ^FUNCTION: strmatch - match a keyword
//
// ^SYNOPSIS:
//    static CmdLine::strmatch_t CmdLine::strmatch(src, attempt, len);
//
// ^PARAMETERS:
//    const char * src;
//    -- the actual keyword to match against
//
//    const char * attempt;
//    -- the "candidate" that may or may not match the keyword
//
//    unsigned len;
//    -- the number of character of "attempt" to consider (==0 if all
//       characters of "attempt" should be used).
//
// ^DESCRIPTION:
//    See if "attempt" matches "src" (either partially or completely) and
//    return the result.
//
// ^REQUIREMENTS:
//    None that havent been discusses in the PARAMETERS section.
//
// ^SIDE-EFFECTS:
//    None.
//
// ^RETURN-VALUE:
//    str_EXACT if "attempt" completely matches "src"
//    str_PARTIAL is "attempt" partially matches "src"
//    str_NONE otherwise
//
// ^ALGORITHM:
//    For each character (in order) of "attempt" to be considered
//       if attempt[i] != src[i] (case insensitive) return str_NONE
//    end-for
//    if we have exhausted "src" return str_EXACT,
//    else return str_PARTIAL
//-^^----
CmdLine::strmatch_t
CmdLine::strmatch(const char * src, const char * attempt, unsigned len)
{
   unsigned  i;

   if (src == attempt)  return  str_EXACT ;
   if ((src == NULL) || (attempt == NULL))  return  str_NONE ;
   if ((! *src) && (! *attempt))  return  str_EXACT ;
   if ((! *src) || (! *attempt))  return  str_NONE ;

   for (i = 0 ; ((i < len) || (! len)) && (attempt[i] != '\0') ; i++) {
      if (TO_LOWER(src[i]) != TO_LOWER(attempt[i]))  return  str_NONE ;
   }

   return  (src[i] == '\0') ? str_EXACT : str_PARTIAL ;
}


//--------------------------------------------------------------------------
// ^FUNCTION: strindent - print a hanging indented paragraph
//
// ^SYNOPSIS:
//    void CmdLine::strindent(os, maxcols, margin, title, indent, text)
//
// ^PARAMETERS:
//    ostream & os;
//    -- the stream to which output is sent
//
//    unsigned maxcols;
//    -- the maximum width (in characters) of the output
//
//    unsigned margin;
//    -- the number of spaces to use as the left margin
//
//    char * title;
//    -- the paragraph title
//
//    unsigned indent;
//    -- the distance between the title and the paragraph body
//
//    char * text;
//    -- the body of the paragraph
//
// ^DESCRIPTION:
//    Strindent will print on os, a titled, indented paragraph as follows:
//
//    <----------------------- maxcols --------------------------->
//    <--- margin --><----- indent ---->
//                   title              This is the first sentence
//                                      of the paragraph. Etc ...
//
// ^REQUIREMENTS:
//    - maxcols and indent must be positive numbers with maxcols > indent.
//    - title should NOT contain any tabs or newlines.
//
// ^SIDE-EFFECTS:
//    Output is printed to os.
//
// ^RETURN-VALUE:
//    None.
//
// ^ALGORITHM:
//    Print the paragraph title and then print the text.
//    Lines are automatically adjusted so that each one starts in the
//    appropriate column.
//-^^-----------------------------------------------------------------------
void
CmdLine::strindent(ostream    & os,
                   unsigned     maxcols,
                   unsigned     margin,
                   const char * title,
                   unsigned     indent,
                   const char * text)
{
   // If we were given non-sensical parameters then dont use them
   if (margin > maxcols)  margin = 0;
   if ((indent + margin) >= maxcols)  indent = 1;

   // print the title (left-justified)
   os.width(margin);
   os << "" ;
   long  save_flags = os.flags();
   os.setf(ios::left, ios::adjustfield);
   os.width(indent);
   os << ((title) ? title : "");
   os.flags(save_flags);

   if (text == NULL) {
      os << endl ;
      return;
   }

   // If the title is too big, start the paragraph on a new line
   if (title  &&  (::strlen(title) > indent)) {
      os << endl ;
      os.width(margin + indent);
      os << "";
   }

   // Loop through the paragraph text witing to print until we absolutely
   // have to.
   //
   unsigned  col = margin + indent + 1;
   unsigned  index = 0 ;
   unsigned  last_white = 0 ;
   const char * p = text ;

   while (p[index]) {
      switch (p[index]) {
         // If we have a space - just remember where it is
      case ' ' :
         last_white = index;
         ++col;
         ++index;
         break;

         // If we have a tab - remember where it is and assume it
         // will take up 8 spaces in the output.
         //
      case '\t' :
         last_white = index;
         col += 8;
         ++index;
         break;

         // If we have a form-feed, carriage-return, or newline, then
         // print what we have so far (including this character) and
         // start a new line.
         //
      case '\n' :
      case '\r' :
      case '\f' :
         os.write(p, index + 1);
         p += index + 1;
         col = margin + indent + 1;
         index = last_white = 0;
         if (*p) {
            os.width(margin + indent);
            os << "";
         }
         break;

      default:
         ++col;
         ++index;
         break;
      } //switch

         // Are we forced to start a new line?
      if (col > maxcols) {
            // Yes - if possible, print upto the last whitespace character
            //       and start the next line on a word-boundary
         if (last_white) {
            os.write(p, last_white);
            p += last_white;
            while (*p == ' ')  ++p;
         } else {
               // No word boundary in sight - just split the line here!
            os.write(p, index);
            p += index;
         }
         os << endl ;

            // We just printed a newline - dont print another one right now
         while ((*p == '\n') || (*p == '\r') || (*p == '\f'))  ++p;
         col = margin + indent + 1;
         index = last_white = 0;
         if (*p) {
            os.width(margin + indent);
            os << "";
         }
      } else if (index && (! p[index])) {
         os << p << endl ;
      }
   } //while
}


#ifdef DEBUG_CMDLINE

// Indent a line corresponding to a given indent level.
// The number of spaces to indent is 3x the indent level
static ostream &
indent(ostream & os, unsigned level)
{
   os.width(level * 3);
   return  (os << "");
}


// Dump the arg_syntax field of a CmdArg in a mnemonic format
static ostream &
dump_arg_syntax(ostream & os, unsigned  syntax)
{
   if (syntax & CmdArg::isREQ) {
      os << "isREQ" ;
   } else {
      os << "isOPT" ;
   }

   if (syntax & CmdArg::isVALREQ) {
      os << "+isVALREQ" ;
   }
   if (syntax & CmdArg::isVALOPT) {
      os << "+isVALOPT" ;
   }
   if (syntax & CmdArg::isVALSEP) {
      os << "+isVALSEP" ;
   }
   if (syntax & CmdArg::isVALSTICKY) {
      os << "+isVALSTICKY" ;
   }
   if (syntax & CmdArg::isLIST) {
      os << "+isLIST" ;
   }
   if (syntax & CmdArg::isPOS) {
      os << "+isPOS" ;
   }
   if (syntax & CmdArg::isHIDDEN) {
      os << "+isHID" ;
   }

   return  os;
}


// Dump the arg_flags field of a CmdArg in a mnemonic format
static ostream &
dump_arg_flags(ostream & os, unsigned  flags)
{
   if (flags & CmdArg::GIVEN) {
      os << "GIVEN" ;
   } else {
      os << "NOTGIVEN";
   }
   if (flags & CmdArg::VALGIVEN) {
      os << "+VALGIVEN";
   }
   if (flags & CmdArg::OPTION) {
      os << "+OPTION";
   }
   if (flags & CmdArg::KEYWORD) {
      os << "+KEYWORD";
   }
   if (flags & CmdArg::POSITIONAL) {
      os << "+POSITIONAL";
   }
   if (flags & CmdArg::VALSEP) {
      os << "+VALSEP";
   } else if (flags & CmdArg::VALGIVEN) {
      os << "+VALSAME";
   }

   return  os;
}


// Dump the cmd_flags field of a CmdLine in a mnemonic format
static ostream &
dump_cmd_flags(ostream & os, unsigned flags)
{
   if (flags & CmdLine::NO_ABORT) {
      os << "NO_ABORT" ;
   } else {
      os << "ABORT" ;
   }
   if (flags & CmdLine::ANY_CASE_OPTS) {
      os << "+ANY_CASE_OPTS";
   }
   if (flags & CmdLine::OPTS_FIRST) {
      os << "+OPTS_FIRST";
   }
   if (flags & CmdLine::OPTS_ONLY) {
      os << "+OPTS_ONLY";
   }
   if (flags & CmdLine::KWDS_ONLY) {
      os << "+KWDS_ONLY";
   }
   if (flags & CmdLine::TEMP) {
      os << "+TEMP";
   }
   if (flags & CmdLine::QUIET) {
      os << "+QUIET";
   }
   if (flags & CmdLine::NO_GUESSING) {
      os << "+NO_GUESSING";
   }

   return  os;
}


// Dump the status of a CmdLine in a mnemonic format
static ostream &
dump_cmd_status(ostream & os, unsigned  status)
{
   if (! status) {
      os << "NO_ERROR";
      return  os;
   } else {
      os << "ERROR";
   }
   if (status & CmdLine::ARG_MISSING) {
      os << "+ARG_MISSING";
   }
   if (status & CmdLine::VAL_MISSING) {
      os << "+VAL_MISSING";
   }
   if (status & CmdLine::VAL_NOTSTICKY) {
      os << "+VAL_NOTSTICKY";
   }
   if (status & CmdLine::VAL_NOTSEP) {
      os << "+VAL_NOTSEP";
   }
   if (status & CmdLine::KWD_AMBIGUOUS) {
      os << "+KWD_AMBIG";
   }
   if (status & CmdLine::BAD_OPTION) {
      os << "+BAD_OPT";
   }
   if (status & CmdLine::BAD_KEYWORD) {
      os << "+BAD_KWD";
   }
   if (status & CmdLine::BAD_VALUE) {
      os << "+BAD_VAL";
   }
   if (status & CmdLine::TOO_MANY_ARGS) {
      os << "+TOO_MANY";
   }

   return  os;
}


// Dump the state of a CmdLine in a mnemonic format
static ostream &
dump_cmd_state(ostream & os, unsigned  state)
{
   if (! state) {
      os << "NO_OPTIONS";
   } else {
      os << "ARGS";
   }
   if (state & cmd_END_OF_OPTIONS) {
      os << "+ENDOPTS";
   }
   if (state & cmd_OPTIONS_USED) {
      os << "+OPTS_USED";
   }
   if (state & cmd_KEYWORDS_USED) {
      os << "+KWDS_USED";
   }
   if (state & cmd_GUESSING) {
      os << "+GUESSING";
   }

   return  os;
}


// Dump the parse_state of a CmdLine in a mnemonic format
static ostream &
dump_cmd_parse_state(ostream & os, unsigned parse_state)
{
   switch (parse_state) {
   case cmd_START_STATE :
      os << "START_STATE";
      break;

   case cmd_TOK_REQUIRED :
      os << "TOK_REQUIRED";
      break;

   case cmd_WANT_VAL :
      os << "WANT_VAL";
      break;

   case cmd_NEED_VAL :
      os << "NEED_VAL";
      break;

   default :
      os << parse_state;
   }

   return  os;
}


// Dump the arguments (including the default arguments) in an arg_list
static ostream &
dump_cmd_args(ostream & os, CmdArgListList * arg_list, unsigned level)
{
   ::indent(os, level) << "CmdLine::cmd_args {\n" ;

   CmdArgListListIter  list_iter(arg_list);
   for (CmdArgList * alist = list_iter() ; alist ; alist = list_iter()) {
      CmdArgListIter iter(alist);
      for (CmdArg * cmdarg = iter() ; cmdarg ; cmdarg = iter()) {
         cmdarg->dump(os, level + 1);
      }
   }

   ::indent(os, level) << "}" << endl;
   return  os;
}

#endif  /* DEBUG_CMDLINE */


// Dump a CmdArg
void
CmdArg::dump(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   ::indent(os, level) << "CmdArg {\n" ;

   ::indent(os, level + 1) << "option='" << char(arg_char_name) << "', "
                         << "keyword=\"" << arg_keyword_name << "\", "
                         << "value=\"" << arg_value_name << "\"\n" ;

   ::indent(os, level + 1) << "syntax=" ;
   dump_arg_syntax(os, arg_syntax) << "\n";

   ::indent(os, level + 1) << "flags=" ;
   dump_arg_flags(os, arg_flags) << "\n";

   ::indent(os, level + 1) << "sequence=" << arg_sequence << "\n";

   ::indent(os, level) << "}" << endl;
#endif
}


// Dump a CmdLine
void
CmdLine::dump(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   ::indent(os, level) << "CmdLine {\n" ;

   ::indent(os, level + 1) << "name=\"" << cmd_name << "\"\n";

   ::indent(os, level + 1) << "description=\"" << cmd_description << "\"\n";

   ::indent(os, level + 1) << "flags=" ;
   dump_cmd_flags(os, cmd_flags) << "\n";

   ::indent(os, level + 1) << "status=" ;
   dump_cmd_status(os, cmd_status) << "\n";

   ::indent(os, level + 1) << "state=" ;
   dump_cmd_state(os, cmd_state) << "\n";

   ::indent(os, level + 1) << "parse_state=" ;
   dump_cmd_parse_state(os, cmd_parse_state) << "\n";

   ::indent(os, level + 1);
   if (cmd_matched_arg == NULL) {
      os << "matched_arg=NULL\n";
   } else {
      os << "matched_arg=" << (void *)cmd_matched_arg << "\n";
   }

   ::indent(os, level + 1) << "# valid-args-parsed="
                           << cmd_nargs_parsed << "\n" ;

   ::indent(os, level) << "}" << endl;
#endif
}


// Dump the arguments of a CmdLine
void
CmdLine::dump_args(ostream & os, unsigned level) const
{
#ifdef DEBUG_CMDLINE
   dump_cmd_args(os, cmd_args, level);
#endif
}
