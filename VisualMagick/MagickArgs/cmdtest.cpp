//------------------------------------------------------------------------
// ^FILE: cmdtest.c - test program for the CmdLine library
//
// ^DESCRIPTION:
//    This program tests as many features of command-line as possible.
//
// ^HISTORY:
//    03/18/92	Brad Appleton	<bradapp@enteract.com>	Created
//
//    03/01/93	Brad Appleton	<bradapp@enteract.com>
//    - Attached a description to the command.
//-^^---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream.h>
#include <ctype.h>
#include <cmdargs.h>

//---------------------------------------------------------------- CmdArgModCmd

   // CmdArgModCmd is a special argument that we use for testing.
   // The argument actually modifies the flags of the associated
   // command before it has finished parsing the arguments, hence
   // the new flags take effect for all remaining arguments.
   //
   // This argument takes a value (which is optional). If no value
   // is given, then the flags are unset, otherwise the value is
   // a list of characters, each of which corresponds to a CmdFlag
   // to turn on.
   //
class  CmdArgModCmd : public CmdArg {
public:
   CmdArgModCmd(char         optchar,
                const char * keyword,
                const char * value,
                const char * description,
                unsigned     syntax_flags =CmdArg::isOPTVALOPT);

   virtual
   ~CmdArgModCmd(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);
} ;

CmdArgModCmd::CmdArgModCmd(char         optchar,
                           const char * keyword,
                           const char * value,
                           const char * description,
                           unsigned     syntax_flags)
   : CmdArg(optchar, keyword, value, description, syntax_flags) {}

CmdArgModCmd::~CmdArgModCmd(void) {}

int CmdArgModCmd::operator()(const char * & arg, CmdLine & cmd)
{
   unsigned  new_flags = 0;
   for (const char * p = arg ; *p ; p++) {
      char ch = *p;
      if (isupper(ch))  ch = tolower(ch);
      switch (ch) {
         case 'c' : new_flags |= CmdLine::ANY_CASE_OPTS;  break;

         case 'n' : new_flags |= CmdLine::NO_ABORT;       break;

         case 'f' : new_flags |= CmdLine::OPTS_FIRST;     break;

         case 'o' : new_flags |= CmdLine::OPTS_ONLY;      break;

         case 'k' : new_flags |= CmdLine::KWDS_ONLY;      break;

         case 't' : new_flags |= CmdLine::TEMP;           break;

         case 'q' : new_flags |= CmdLine::QUIET;          break;

         case 'g' : new_flags |= CmdLine::NO_GUESSING;    break;

         case '+' : new_flags |= CmdLine::ALLOW_PLUS;     break;

         default  : break;
      } //switch
   } //for
   cmd.flags(new_flags);
   arg = NULL;
   return  0;
}


//------------------------------------------------------ Command Line Arguments

static CmdArgModCmd    fflag('f', "flags", "[cpnfoktqg]",
   "Use this argument to change the behavior of \
parsing for all remaining arguments.  If no \
value is given   then the command-flags are \
cleared.  Otherwise each letter specifies a flag \
to set:\n\
   'c' = any-Case-opts\n\
   'n' = No-abort\n\
   'f' = options-First\n\
   'o' = Opts-only\n\
   'k' = Keywords-only\n\
   't' = Temporary-args\n\
   'q' = Quiet!\n\
   'g' = no-Guessing\n\
   '+' = allow-plus\n\
This-is-a-very-long-line-containing-no-whitespace-\
characters-and-I-just-want-to-see-if-it-gets-\
formatted-appropriately!"
   );

static CmdArgStr       str('s', "str", "[string]", "string to parse");
static CmdArgInt       debug ('D', "Debug", "[level]", "turn on debugging",
                              CmdArg::isVALSTICKY);
static CmdArgBool      infile('p', "parse", "parse from cin");

static CmdArgSet       xflag('x', "x", ";turn on X-rated mode");
static CmdArgClearRef  nxflag(xflag, 'n', "nx", ";turn off X-rated mode");
static CmdArgInt       count('c', "count", "number", "number of copies");
static CmdArgChar      delim('d', "delimiter", "char", "delimiter character");
static CmdArgChar      ext('e', "ext", "[char]", "extension to use",
                                                 CmdArg::isVALSTICKY);
static CmdArgChar      code('C', "Code", "char", "code to use",
                                                 CmdArg::isVALSTICKY);
static CmdArgStr       why('y', "why", "[reason]", "specify the reason why",
                                                   CmdArg::isVALSEP);
static CmdArgStr       who('w', "who", "logname", "the user responsible",
                                                  CmdArg::isVALSEP);
static CmdArgIntList   ints('i', "int", "number ...", "list of ints");
static CmdArgStrList   grps('g', "groups", "newsgroup", "list of newsgroups");
static CmdArgDummy     dummy("--", "denote end of options");
static CmdArgStr       name('n', "name", "name", "name of document",
                                                 CmdArg::isPOS);
static CmdArgStrList   files("[files ...]", "files to process");

//------------------------------------------------------------------ print_args

static void
print_args(void) {
   cout << "xflag=" << (xflag ? "ON" : "OFF") << endl ;
   cout << "count=" << count << endl ;

   unsigned  sflags = str.flags();
   if ((sflags & CmdArg::GIVEN) && (! (sflags & CmdArg::VALGIVEN))) {
      cout << "No string given on command-line!" << endl ;
   } else {
      cout << "str=\"" << str << "\"" << endl ;
   }
   cout << "delim='" << delim << "'" << endl ;
   cout << "ext='" << ext << "'" << endl ;
   cout << "code='" << code << "'" << endl ;
   cout << "why=\"" << why << "\"" << endl ;
   cout << "who=\"" << who << "\"" << endl ;

   unsigned  nints = ints.count();
   for (int i = 0; i < nints ; i++) {
      cout << "int[" << i << "]=" << ints[i] << endl ;
   }

   unsigned  ngrps = grps.count();
   for (i = 0; i < ngrps ; i++) {
      cout << "groups[" << i << "]=\"" << grps[i] << "\"" << endl ;
   }

   cout << "name=\"" << name << "\"" << endl ;

   unsigned  nfiles = files.count();
   for (i = 0; i < nfiles ; i++) {
      cout << "files[" << i << "]=\"" << files[i] << "\"" << endl ;
   }
}

//------------------------------------------------------------------------ dump

static void
dump(CmdLine & cmd)
{
   if (debug) {
      cmd.dump(cout);
      if (debug > 1) cmd.dump_args(cout);
   }
}

//------------------------------------------------------------------------ main

int
main(int argc, char * argv[]) {
   CmdLine  cmd(*argv,
                & fflag,
                & str,
                & infile,
                & debug,
                & xflag,
                & nxflag,
                & count,
                & delim,
                & ext,
                & code,
                & why,
                & who,
                & ints,
                & grps,
                & dummy,
                & name,
                & files,
                NULL);

   CmdArgvIter  argv_iter(--argc, ++argv);

   cmd.description(
"This program is intended to statically and dynamically test \
the CmdLine(3C++) class library."
   );

   cout << "Test of " << CmdLine::ident() << endl ;

   xflag = 0;
   count = 1;
   str = NULL;
   delim = '\t';
   name = NULL;

   cout << "Parsing the command-line ..." << endl ;
   unsigned status = cmd.parse(argv_iter);
   if (status)  cmd.error() << "parsing errors occurred!" << endl ;

   print_args();

   unsigned dbg_flags = debug.flags();
   if ((dbg_flags & CmdArg::GIVEN) && (! (dbg_flags & CmdArg::VALGIVEN))) {
      debug = 1;
   }

   dump(cmd);

   int  parse_cin = infile;

   // Parse arguments from a string
   if (! str.isNULL()) {
      CmdStrTokIter  tok_iter(str);

      xflag = 0;
      count = 1;
      str = NULL;
      delim = '\t';
      name = NULL;

      cout << "\n\nParsing the string ..." << endl ;
      status = cmd.parse(tok_iter);
      print_args();
      dump(cmd);
   }


   // Parse arguments from a file
   if (parse_cin) {
      xflag = 0;
      count = 1;
      str = NULL;
      delim = '\t';
      name = NULL;

      CmdIstreamIter  file_iter(cin);
      cout << "\n\nParsing from cin ..." << endl ;
      status = cmd.parse(file_iter);
      print_args();
      dump(cmd);
   }

   return  0;
}

