//------------------------------------------------------------------------
// ^FILE: cmdargs.h - define the most commonly used argument types
//
// ^DESCRIPTION:
//    This file defines classes for the most commonly used types of
//    command-line arguments.  Most command-line arguments are either
//    boolean-flags, a number, a character, or a string (or a list of
//    numbers or strings).  In each of these cases, the call operator
//    (operator()) of the argument just compiles the value given into
//    some internal value and waits for the programmer to query the
//    value at some later time.
//
//    I call these types of arguments "ArgCompilers". For each of the
//    most common argument types, a corresponding abstract ArgCompiler
//    class is declared.  All that this class does is to add a member
//    function named "compile" to the class.  The "compile()" function
//    looks exactly like the call operator but it takes an additional
//    parameter: a reference to the value to be modified by compiling
//    the argument value.  In all other respects, the "compile()" member
//    function behaves exactly like the call operator.  In fact, most
//    of the call-operator member functions simply call the ArgCompiler's
//    "compile()" member function with the appropriate value and return
//    whatever the compile function returned.
//
//    Once all of these ArgCompilers are declared, it is a simple matter
//    to declare a class that holds a single item, or a list of items,
//    by deriving it from the corresponding ArgCompiler type.
//
//    For derived classes of these ArgCompilers that hold a single item,
//    The derived class implements some operators (such as operator=
//    and an appropriate cast operator) to treat the argument as if it
//    were simply an item (instead of an argument that contains an item).
//    The output operator (ostream & operator<<) is also defined.
//
//    For derived classes of ArgCompilers that hold a list of items,
//    the subscript operator[] is defined in order to treat the argument
//    as if it were simply an array of items and not an argument that
//    contains a list of items.
//
//    *NOTE*
//    ======
//    It is important to remember that each subclass of CmdArg MUST be able
//    to handle NULL as the first argument to the call-operator (and it
//    should NOT be considered an error).  This is because NULL will be
//    passed if the argument takes no value, or if it takes an optional
//    value that was NOT provided on the command-line.  Whenever an
//    argument is correctly specified on the command-line, its call
//    operator is always invoked (regardless of whether or not there
//    is a corresponding value from the command-line).
//
// ^HISTORY:
//    03/25/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#ifndef _usr_include_cmdargs_h
#define _usr_include_cmdargs_h

//-------------------------------------------------------------- Dummy Argument

   // A Dummy argument is one that is used only for its appearance in
   // usage messages. It is completely ignored by the CmdLine object
   // when parsing the command-line.
   //
   // Examples:
   //     CmdArgDummy  dummy1('c', "keyword", "value", "dummy argument # 1");
   //     CmdArgDummy  dummy2("value", "dummy argument # 2");
   //
class CmdArgDummy : public CmdArg {
public:
   CmdArgDummy(char         optchar,
               const char * keyword,
               const char * value,
               const char * description,
               unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArg(optchar, keyword, value, description, syntax_flags) {}

   CmdArgDummy(char         optchar,
               const char * keyword,
               const char * description,
               unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArg(optchar, keyword, description, syntax_flags) {}

   CmdArgDummy(const char * value,
               const char * description,
               unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArg(value, description, syntax_flags) {}

   CmdArgDummy(const CmdArgDummy & cp) : CmdArg(cp) {}

   CmdArgDummy(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgDummy(void);

   virtual  int
   is_dummy(void);   // return non-zero

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);  // NO-OP
} ;

//-------------------------------------------------------------- Usage Argument

   // The sole purpose of a usage argument is to immediately print the
   // program usage (as soon as it is matched) and to exit.
   //
   // There is a default usage argument in every CmdLine object.
   //
   // Example:
   //    CmdArgUsage  usage_arg('?', "help", "print usage and exit");
   //
class  CmdArgUsage : public CmdArg {
public:
   CmdArgUsage(char         optchar,
               const char * keyword,
               const char * description,
               ostream    * osp =NULL);   // cout is used if "osp" is NULL

   CmdArgUsage(const CmdArgUsage & cp) : CmdArg(cp) {}

   CmdArgUsage(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgUsage(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

      // get/set the ostream that usage is printed on
   ostream *
   ostream_ptr(void) const { return  os_ptr; }

   void
   ostream_ptr(ostream * osp);
   
private:
   ostream * os_ptr;
} ;

//----------------------------------------------------------- Integer Arguments

   // Look under "List Arguments" for a CmdArg that is a list of ints

   // CmdArgIntCompiler is the base class for all arguments that need to
   // convert the string given on the command-line into an integer.
   //
class  CmdArgIntCompiler : public CmdArg {
public:
   CmdArgIntCompiler(char         optchar,
                     const char * keyword,
                     const char * value,
                     const char * description,
                     unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArg(optchar, keyword, value, description, syntax_flags) {}

   CmdArgIntCompiler(const char * value,
                     const char * description,
                     unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArg(value, description, syntax_flags) {}

   CmdArgIntCompiler(const CmdArgIntCompiler & cp) : CmdArg(cp) {}

   CmdArgIntCompiler(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgIntCompiler(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

   int
   compile(const char * & arg, CmdLine & cmd, int & value);
} ;


   // CmdArgInt - an argument that contains a single integer.
   //
   // The following member functions are provided to treat
   // a CmdArgInt as if it were an integer:
   //
   //    operator=(int);
   //    operator int(void);
   //    operator<<(os, CmdArgInt);
   //
   // The integer value is initialized to zero by the constructor.
   //
   // Examples:
   //     CmdArgInt  count('c', "count", "number", "# of copies to print");
   //     CmdArgInt  nlines("lines", "number of lines to print);
   //
   //     count = 1;
   //     nlines = 0;
   //
   //     if (count > 1) { ... }
   //     cout << "number of lines is " << nlines << endl ;
   //
class  CmdArgInt : public CmdArgIntCompiler {
public:
   CmdArgInt(char         optchar,
             const char * keyword,
             const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgIntCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgInt(const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgIntCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgInt(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   CmdArgInt(const CmdArgInt & cp) : CmdArgIntCompiler(cp), val(cp.val) {}

   CmdArgInt(const CmdArg & cp) : CmdArgIntCompiler(cp), val(0) {}

   CmdArgInt &
   operator=(const CmdArgInt & cp)  { val = cp.val; return  *this; }

   CmdArgInt &
   operator=(int cp)  { val = cp; return  *this; }

   operator int(void)  const { return  val; }

protected:
   int   val;
} ;

ostream &
operator<<(ostream & os, const CmdArgInt & int_arg);

//---------------------------------------------------- Floating-point Arguments

   // Look under "List Arguments" for a CmdArg that is a list of floats

   // CmdArgFloatCompiler is the base class for all arguments
   // need to convert the string given on the command-line into
   // a floating-point value.
   //
class  CmdArgFloatCompiler : public CmdArg {
public:
   CmdArgFloatCompiler(char         optchar,
                       const char * keyword,
                       const char * value,
                       const char * description,
                       unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArg(optchar, keyword, value, description, syntax_flags) {}

   CmdArgFloatCompiler(const char * value,
                       const char * description,
                       unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArg(value, description, syntax_flags) {}

   CmdArgFloatCompiler(const CmdArgFloatCompiler & cp) : CmdArg(cp) {}

   CmdArgFloatCompiler(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgFloatCompiler(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

   int
   compile(const char * & arg, CmdLine & cmd, float & value);
} ;


   // CmdArgFloat - an argument that contains a single floating-point value.
   //
   // The following member functions are provided to treat
   // a CmdArgFloat as if it were a float:
   //
   //    operator=(float);
   //    operator float(void);
   //    operator<<(os, CmdArgFloat);
   //
   // The floating-point value is initialized to zero by the constructor.
   //
   // Examples:
   //     CmdArgFloat  major('m', "major", "#", "major radius of ellipse");
   //     CmdArgFloat  minor("minor", "minor radius of ellipse");
   //
   //     major = 2.71828;
   //     minor = 3.14159;
   //
   //     if (minor > major)  {  /* swap major and minor */ }
   //
   //     cout << "major radius is " << major << endl ;
   //
class  CmdArgFloat : public CmdArgFloatCompiler {
public:
   CmdArgFloat(char         optchar,
               const char * keyword,
               const char * value,
               const char * description,
               unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgFloatCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgFloat(const char * value,
               const char * description,
               unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgFloatCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgFloat(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   CmdArgFloat(const CmdArgFloat & cp)
      : CmdArgFloatCompiler(cp), val(cp.val) {}

   CmdArgFloat(const CmdArg & cp)
      : CmdArgFloatCompiler(cp), val(0) {}

   CmdArgFloat &
   operator=(const CmdArgFloat & cp)  { val = cp.val; return  *this; }

   CmdArgFloat &
   operator=(float cp)  { val = cp; return  *this; }

   operator float(void)  const  { return  val; }

protected:
   float   val;
} ;

ostream &
operator<<(ostream & os, const CmdArgFloat & float_arg);


//--------------------------------------------------------- Character Arguments

   // CmdArgCharCompiler is the base class for all arguments need to
   // convert the string given on the command-line into a character.
   //
class  CmdArgCharCompiler : public CmdArg {
public:
   CmdArgCharCompiler(char         optchar,
                      const char * keyword,
                      const char * value,
                      const char * description,
                      unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArg(optchar, keyword, value, description, syntax_flags) {}

   CmdArgCharCompiler(const char * value,
                      const char * description,
                      unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArg(value, description, syntax_flags) {}

   CmdArgCharCompiler(const CmdArgCharCompiler & cp) : CmdArg(cp) {}

   CmdArgCharCompiler(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgCharCompiler(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

   int
   compile(const char * & arg, CmdLine & cmd, char & value);
} ;


   // CmdArgChar - an argument that contains a single character.
   //
   // The following member functions are provided to treat
   // a CmdArgChar as if it were a character:
   //
   //    operator=(char);
   //    operator char(void);
   //    operator<<(os, CmdArgChar);
   //
   // The character value is initialized to '\0' by the constructor.
   //
   // Examples:
   //     CmdArgChar  ignore('i', "ignore", "character to ignore");
   //     CmdArgChar  sep("field-separator");
   //
   //     ignore = ' ';
   //     sep = ',';
   //
   //     if (sep == '\0') { /* error */ }
   //
   //     cout << "ignore character is '" << ignore << "'" << endl ;
   //
class  CmdArgChar : public CmdArgCharCompiler {
public:
   CmdArgChar(char         optchar,
              const char * keyword,
              const char * value,
              const char * description,
              unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgCharCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgChar(const char * value,
              const char * description,
              unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgCharCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgChar(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   CmdArgChar(const CmdArgChar & cp) : CmdArgCharCompiler(cp), val(cp.val) {}

   CmdArgChar(const CmdArg & cp) : CmdArgCharCompiler(cp), val(0) {}

   CmdArgChar &
   operator=(const CmdArgChar & cp)  { val = cp.val; return  *this; }

   CmdArgChar &
   operator=(char cp)  { val = cp; return  *this; }

   operator char(void)  const  { return  val; }

protected:
   char   val;
} ;

ostream &
operator<<(ostream & os, const CmdArgChar & char_arg);

//------------------------------------------------------------ String Arguments

   // Look under "List Arguments" for a CmdArg that is a list of strings

   // CmdArgIntCompiler is the base class for all arguments need to
   // convert the string given on the command-line into a string.
   //
class  CmdArgStrCompiler : public CmdArg {
public:
   // We need an internal string type here because sometimes we need
   // to allocate new space for the string, and sometimes we dont.
   // We need a string type that knows how it was allocated and
   // to behave accordingly.
   //
   // Since the programmer, will be seeing our string type instead of
   // a "char *" we need to provide some operators for our string
   // type that make it unnecessary to know the difference between
   // it and a "char *" (in most cases).
   //
   struct  casc_string {
      unsigned     is_alloc : 1 ;
      const char * str ;

      casc_string(void) : is_alloc(0), str(0) {}

      casc_string(const char * s) : is_alloc(0), str(s) {}

      void
      copy(unsigned  is_temporary, const char * s);

      casc_string(unsigned  is_temporary, const char * s)
         : is_alloc(0), str(0) { copy(is_temporary, s); }

      casc_string(const casc_string & cp)
         : is_alloc(0), str(0) { copy(cp.is_alloc, cp.str); }

      casc_string &
      operator=(const casc_string & cp)
         { copy(cp.is_alloc, cp.str); return *this; }

      casc_string &
      operator=(const char * cp) { copy(0, cp); return *this; }

      operator const char*(void)  const { return  str; }

      virtual ~casc_string(void)
#ifdef __GNUG__
         { if (is_alloc)   delete [] (char *) str; }
#endif
      ;
   } ;

   CmdArgStrCompiler(char         optchar,
                     const char * keyword,
                     const char * value,
                     const char * description,
                     unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArg(optchar, keyword, value, description, syntax_flags) {}

   CmdArgStrCompiler(const char * value,
                     const char * description,
                     unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArg(value, description, syntax_flags) {}

   CmdArgStrCompiler(const CmdArgStrCompiler & cp) : CmdArg(cp) {}

   CmdArgStrCompiler(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgStrCompiler(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

   int
   compile(const char * & arg, CmdLine & cmd, casc_string & value) ;
} ;


   // CmdArgStr - an argument that holds a single string
   //
   // The following member functions are provided to treat
   // a CmdArgStr as if it were a string:
   //
   //    operator=(char*);
   //    operator char*(void);
   //    operator<<(os, CmdArgStr);
   // 
   // The string value is initialized to NULL by the constructor.
   //
   // Examples:
   //     CmdArgStr  input('i', "input", "filename", "file to read");
   //     CmdArgStr  output("output-file", "file to write);
   //
   //     input = "/usr/input" ;
   //     output = "/usr/output" ;
   //
   //     if (strcmp(input, output) == 0) {
   //        cerr << "input and output are the same file: " << input << endl ;
   //     }
   //
class  CmdArgStr : public CmdArgStrCompiler {
public:
   CmdArgStr(char         optchar,
             const char * keyword,
             const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isOPTVALREQ)
      : CmdArgStrCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgStr(const char * value,
             const char * description,
             unsigned     syntax_flags =CmdArg::isPOSVALREQ)
      : CmdArgStrCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgStr(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   CmdArgStr(const CmdArgStr & cp) : CmdArgStrCompiler(cp), val(cp.val) {}

   CmdArgStr(const CmdArg & cp) : CmdArgStrCompiler(cp), val(0) {}

   CmdArgStr &
   operator=(const CmdArgStr & cp)  { val = cp.val; return  *this; }

   CmdArgStr &
   operator=(const CmdArgStrCompiler::casc_string & cp)
      { val = cp; return  *this; }

   CmdArgStr &
   operator=(const char * cp)  { val = cp; return  *this; }

   operator CmdArgStrCompiler::casc_string(void)  { return  val; }

   operator const char*(void)  const { return  val.str; }

   // use this for comparing to NULL
   int isNULL(void) const { return (val.str) ? 0 : 1; }

protected:
   CmdArgStrCompiler::casc_string  val;
} ;

ostream &
operator<<(ostream & os, const CmdArgStrCompiler::casc_string & str);

ostream &
operator<<(ostream & os, const CmdArgStr & str_arg);

//-------------------------------------------------------------- List Arguments

   // For each of the list argument types:
   //    The list is initially empty. The only way to add to the list
   //    is with operator(). The number of items in the list may
   //    be obtained by the "count()" member function and a given
   //    item may be obtained by treating the list as an array and
   //    using operator[] to index into the list.
   //


   // CmdArgIntList - an argument that holds a list of integers
   //
   // Example:
   //     CmdArgIntList ints('i', "ints", "numbers ...", "a list of integers");
   //     CmdArgIntList ints("numbers ...", "a positional list of integers");
   //
   //     for (int i = 0 ; i < ints.count() ; i++) {
   //        cout << "integer #" << i << " is " << ints[i] << endl ;
   //     }
   //
struct CmdArgIntListPrivate;
class  CmdArgIntList : public CmdArgIntCompiler {
public:
   CmdArgIntList(char    optchar,
            const char * keyword,
            const char * value,
            const char * description,
            unsigned   syntax_flags =(CmdArg::isOPTVALREQ + CmdArg::isLIST))
      : CmdArgIntCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgIntList(const char * value,
            const char * description,
            unsigned   syntax_flags =(CmdArg::isPOSVALREQ + CmdArg::isLIST))
      : CmdArgIntCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgIntList(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   unsigned
   count(void) const;

   int &
   operator[](unsigned  index);

private:
   CmdArgIntList(const CmdArgInt & cp);

   CmdArgIntList &
   operator=(const CmdArgInt & cp);

   CmdArgIntListPrivate * val;
} ;


   // CmdArgFloatList - an argument that holds a list of floats
   //
   // Example:
   //     CmdArgFloatList flts('f', "flts", "numbers ...", "a list of floats");
   //     CmdArgFloatList flts("numbers ...", "a positional list of floats");
   //
   //     for (int i = 0 ; i < flts.count() ; i++) {
   //        cout << "Float #" << i << " is " << flts[i] << endl ;
   //     }
   //
struct CmdArgFloatListPrivate;
class  CmdArgFloatList : public CmdArgFloatCompiler {
public:
   CmdArgFloatList(char    optchar,
              const char * keyword,
              const char * value,
              const char * description,
              unsigned   syntax_flags =(CmdArg::isOPTVALREQ + CmdArg::isLIST))
      : CmdArgFloatCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgFloatList(const char * value,
              const char * description,
              unsigned   syntax_flags =(CmdArg::isPOSVALREQ + CmdArg::isLIST))
      : CmdArgFloatCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgFloatList(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   unsigned
   count(void) const;

   float &
   operator[](unsigned  index);

private:
   CmdArgFloatList(const CmdArgFloat & cp);

   CmdArgFloatList &
   operator=(const CmdArgFloat & cp);

   CmdArgFloatListPrivate * val;
} ;


   // CmdArgStrList - an argument that holds a list of strings
   //
   // Example:
   //     CmdArgStrList strs('s', "strs", "strings ...", "a list of strings");
   //     CmdArgStrList strs("strings ...", "a positional list of strings");
   //
   //     for (int i = 0 ; i < strs.count() ; i++) {
   //        cout << "String #" << i << " is " << strs[i] << endl ;
   //     }
   //
struct CmdArgStrListPrivate;
class  CmdArgStrList : public CmdArgStrCompiler {
public:
   CmdArgStrList(char    optchar,
            const char * keyword,
            const char * value,
            const char * description,
            unsigned   syntax_flags =(CmdArg::isOPTVALREQ + CmdArg::isLIST))
      : CmdArgStrCompiler(optchar, keyword, value, description, syntax_flags),
        val(0) {}

   CmdArgStrList(const char * value,
            const char * description,
            unsigned   syntax_flags =(CmdArg::isPOSVALREQ + CmdArg::isLIST))
      : CmdArgStrCompiler(value, description, syntax_flags), val(0) {}

   virtual ~CmdArgStrList(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

   unsigned
   count(void) const;

   CmdArgStrCompiler::casc_string &
   operator[](unsigned  index);

private:
   CmdArgStrList(const CmdArgStr & cp);

   CmdArgStrList &
   operator=(const CmdArgStr & cp);

   CmdArgStrListPrivate * val;
} ;

//----------------------------------------------------------- Boolean Arguments

   // Boolean arguments are a bit tricky, first of all - we have three
   // different kinds:
   //
   //    1) An argument whose presence SETS a value
   //
   //    2) An argument whose presence UNSETS a value
   //
   //    3) An argument whose presence TOGGLES a value
   //
   // Furthermore, it is not uncommon (especially in VAX/VMS) to have
   // one argument that SETS and value, and another argument that
   // UNSETS the SAME value.
   //

   // CmdArgBoolCompiler is a special type of ArgCompiler, not only does
   // its "compile" member function take a reference to the boolean value,
   // but it also needs to know what default-value to use if no explicit
   // value (such as '0', '1', "ON" or "FALSE") was given.
   //
class CmdArgBoolCompiler : public CmdArg {
public:
   CmdArgBoolCompiler(char         optchar,
                      const char * keyword,
                      const char * description,
                      unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArg(optchar, keyword, description, syntax_flags) {}

   CmdArgBoolCompiler(const CmdArgBoolCompiler & cp) : CmdArg(cp) {}

   CmdArgBoolCompiler(const CmdArg & cp) : CmdArg(cp) {}

   virtual ~CmdArgBoolCompiler(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd) = 0;

   int
   compile(const char * & arg,
           CmdLine      & cmd,
           unsigned     & value,
           unsigned       default_value =1);
} ;


   // CmdArgBool is a boolean ArgCompiler that holds a single
   // boolean value, it has three subclasses:
   //
   //   1) CmdArgSet (which is just an alias for CmdArgBool)
   //      -- This argument SETS a boolean value.
   //         The initial value is 0 (OFF).
   //
   //   2) CmdArgClear
   //      -- This argument CLEARS a boolean value
   //         The initial value is 1 (ON).
   //
   //   3) CmdArgToggle
   //      -- This argument TOGGLES a boolean value
   //         The initial value is 0 (OFF).
   //
   // All of these classes have the following member functions
   // to help make it easier to treat a Boolean Argument as
   // a Boolean Value:
   //
   //   operator=(int);
   //   operator int(void);
   //
   // Examples:
   //    CmdArgBool    xflag('x', "xmode", "turn on xmode);
   //    CmdArgClear   yflag('y', "ymode", "turn on ymode);
   //    CmdArgToggle  zflag('z', "zmode", "turn on zmode);
   //
   //    cout << "xmode is " << (xflag ? "ON" : "OFF") << endl ;
   //
class CmdArgBool : public CmdArgBoolCompiler {
public:
   CmdArgBool(char         optchar,
              const char * keyword,
              const char * description,
              unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArgBoolCompiler(optchar, keyword, description, syntax_flags),
        val(0) {}

   CmdArgBool(const CmdArgBool & cp) : CmdArgBoolCompiler(cp), val(cp.val) {}

   CmdArgBool(const CmdArg & cp) : CmdArgBoolCompiler(cp), val(0) {}

   virtual ~CmdArgBool(void);

   CmdArgBool &
   operator=(const CmdArgBool & cp)
      { val = cp.val; return  *this; }

   CmdArgBool &
   operator=(int new_value)
      { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

protected:
   unsigned  val : 1;
} ;

ostream &
operator<<(ostream & os, const CmdArgBool & bool_arg);

typedef  CmdArgBool  CmdArgSet ;

class CmdArgClear : public CmdArgBool {
public:
   CmdArgClear(char         optchar,
               const char * keyword,
               const char * description,
               unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArgBool(optchar, keyword, description, syntax_flags) { val = 1; }

   CmdArgClear(const CmdArgClear & cp) : CmdArgBool(cp) {}

   CmdArgClear(const CmdArg & cp) : CmdArgBool(cp) { val = 1; }

   virtual ~CmdArgClear(void);

   CmdArgClear &
   operator=(const CmdArgClear & cp)
      { val = cp.val; return  *this; }

   CmdArgClear &
   operator=(int new_value)
      { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);
} ;

class CmdArgToggle : public CmdArgBool {
public:
   CmdArgToggle(char         optchar,
                const char * keyword,
                const char * description,
                unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArgBool(optchar, keyword, description, syntax_flags) {}

   CmdArgToggle(const CmdArgToggle & cp) : CmdArgBool(cp) {}

   CmdArgToggle(const CmdArg & cp) : CmdArgBool(cp) {}

   virtual ~CmdArgToggle(void);

   CmdArgToggle &
   operator=(const CmdArgToggle & cp)
      { val = cp.val; return  *this; }

   CmdArgToggle &
   operator=(int new_value)
      { val = (new_value) ? 1 : 0; return *this; }

   operator int(void) const { return  val; }

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);
} ;


   // Now we come to the Reference Boolean arguments, these are boolean
   // arguments that reference the very same value as some other boolean
   // argument. The constructors for Reference Boolean arguments require
   // a reference to the boolean argument whose value they are referencing.
   //
   // The boolean reference classes are as follows:
   //
   //   1) CmdArgBoolRef and CmdArgSetRef
   //      -- SET the boolean value referenced by a CmdArgBool
   //
   //   2) CmdArgClearRef
   //      -- CLEAR the boolean value referenced by a CmdArgBool
   //
   //   3) CmdArgToggleRef
   //      -- TOGGLE the boolean value referenced by a CmdArgBool
   //
   // Examples:
   //    CmdArgBool    xflag('x', "xmode", "turn on xmode");
   //    CmdArgClear   yflag('Y', "noymode", "turn off ymode");
   //    CmdArgToggle  zflag('z', "zmode", "toggle zmode");
   //
   //    CmdArgClearRef x_off(xflag, 'X', "noxmode", "turn off xmode");
   //    CmdArgBoolRef  y_on(yflag, 'Y', "ymode", "turn on ymode");
   //
   //    cout << "xmode is " << (xflag ? "ON" : "OFF") << endl ;
   //
class CmdArgBoolRef : public CmdArg {
public:
   CmdArgBoolRef(CmdArgBool & bool_arg,
                 char         optchar,
                 const char * keyword,
                 const char * description,
                 unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArg(optchar, keyword, description, syntax_flags), ref(bool_arg) {}

   virtual  ~CmdArgBoolRef(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

protected:
   CmdArgBool & ref;
} ;

typedef CmdArgBoolRef  CmdArgSetRef ;

class CmdArgClearRef : public CmdArg {
public:
   CmdArgClearRef(CmdArgBool & bool_arg,
                  char         optchar,
                  const char * keyword,
                  const char * description,
                  unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArg(optchar, keyword, description, syntax_flags), ref(bool_arg) {}

   virtual  ~CmdArgClearRef(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

protected:
   CmdArgBool & ref;
} ;

class CmdArgToggleRef : public CmdArg {
public:
   CmdArgToggleRef(CmdArgBool & bool_arg,
                  char         optchar,
                  const char * keyword,
                  const char * description,
                  unsigned     syntax_flags =CmdArg::isOPT)
      : CmdArg(optchar, keyword, description, syntax_flags), ref(bool_arg) {}

   virtual  ~CmdArgToggleRef(void);

   virtual  int
   operator()(const char * & arg, CmdLine & cmd);

protected:
   CmdArgBool & ref;
} ;

#endif /* _usr_include_cmdargs_h */
