dnl @synopsis AC_CHECK_CC_OPT(flag, cachevarname)
dnl 
dnl AC_CHECK_CC_OPT(-fvomit-frame,vomitframe)
dnl would show a message as like 
dnl "checking wether gcc accepts -fvomit-frame ... no"
dnl and sets the shell-variable $vomitframe to either "-fvomit-frame"
dnl or (in this case) just a simple "". In many cases you would then call 
dnl AC_SUBST(_fvomit_frame_,$vomitframe) to create a substitution that
dnl could be fed as "CFLAGS = @_funsigned_char_@ @_fvomit_frame_@
dnl
dnl in consequence this function is much more general than their 
dnl specific counterparts like ac_cxx_rtti.m4 that will test for
dnl -fno-rtti -fno-exceptions
dnl 
dml @author  Guido Draheim <guidod@gmx.de>

AC_DEFUN([AC_CHECK_CC_OPT],
[AC_CACHE_CHECK(whether ${CC-cc} accepts [$1], [$2],
[AC_SUBST($2)
echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} -c $1 conftest.c 2>&1`"; then
  $2="$1"
else
  $2=""
fi
rm -f conftest*
])])

dnl @synopsis AC_C_LONG_LONG
dnl
dnl Provides a test for the existance of the long long int type and
dnl defines HAVE_LONG_LONG if it is found.
dnl
dnl @author Caolan McNamara <caolan@skynet.ie>
dnl
AC_DEFUN([AC_C_LONG_LONG],
[AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
[if test "$GCC" = yes; then
  ac_cv_c_long_long=yes
  else
        AC_TRY_COMPILE(,[long long int i;],
   ac_cv_c_long_long=yes,
   ac_cv_c_long_long=no)
   fi])
   if test $ac_cv_c_long_long = yes; then
     AC_DEFINE(HAVE_LONG_LONG)
   fi
])

# RSSH_CHECK_SUNPROC_CC([ACTION-IF-YES], [ACTION-IF-NOT])
# ------------------------------------------------------
# check : are we using SUN C++ compiler.
#  Corresponding cache value: rssh_cv_check_sunpro_cc is set to yes or no
#
#@author  Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2000
#@version $Id$
#
#  RSSH_CHECK_SUNPRO_CC([ACTION-IF-YES],[ACTION-IF-NOT])
#
AC_DEFUN([RSSH_CHECK_SUNPRO_CC],
[AC_CACHE_CHECK([whether using Sun C++ compiler],
                [rssh_cv_check_sunpro_cc],

[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([],
[#ifndef __SUNPRO_CC
# include "error: this is not Sun C++ Compiler."
#endif
],
               rssh_cv_check_sunpro_cc=yes,
               rssh_cv_check_sunpro_cc=no)
AC_LANG_RESTORE])
if test ${rssh_cv_check_sunpro_cc} = yes
then
  $2
  :
else 
  $3
  :
fi
])# RSSH_CHECK_SUNPROC_CC

# RSSH_CHECK_SUNPROC_C([ACTION-IF-YES], [ACTION-IF-NOT])
# ------------------------------------------------------
# check : are we using SUN C compiler.
#  Corresponding cache value: rssh_cv_check_sunpro_c is set to yes or no
#
#@author  Ruslan Shevchenko <Ruslan@Shevchenko.Kiev.UA>, 1998, 2000
#@version $Id$
#
#  RSSH_CHECK_SUNPRO_C([ACTION-IF-YES],[ACTION-IF-NOT])
#
AC_DEFUN([RSSH_CHECK_SUNPRO_C],
[AC_CACHE_CHECK([whether using Sun C compiler],
                [rssh_cv_check_sunpro_c],

[AC_LANG_SAVE
 AC_LANG_C
 AC_TRY_COMPILE([],
[#ifndef __SUNPRO_C
# include "error: this is not Sun C Compiler."
#endif
],
               rssh_cv_check_sunpro_c=yes,
               rssh_cv_check_sunpro_c=no)
AC_LANG_RESTORE])
if test ${rssh_cv_check_sunpro_c} = yes
then
  $2
  :
else 
  $3
  :
fi
])# RSSH_CHECK_SUNPRO_C


dnl @synopsis AC_CXX_BOOL
dnl
dnl If the compiler recognizes bool as a separate built-in type,
dnl define HAVE_BOOL. Note that a typedef is not a separate
dnl type since you cannot overload a function such that it accepts either
dnl the basic type or the typedef.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_BOOL],
[AC_CACHE_CHECK(whether the compiler recognizes bool as a built-in type,
ac_cv_cxx_bool,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([int f(int  x){return 1;}
int f(char x){return 1;}
int f(bool x){return 1;}
],[bool b = true; return f(b);],
 ac_cv_cxx_bool=yes, ac_cv_cxx_bool=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_bool" = yes; then
  AC_DEFINE(HAVE_BOOL,,[define if bool is a built-in type])
fi
])


dnl @synopsis AC_CXX_CONST_CAST
dnl
dnl If the compiler supports const_cast<>, define HAVE_CONST_CAST.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_CONST_CAST],
[AC_CACHE_CHECK(whether the compiler supports const_cast<>,
ac_cv_cxx_const_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[int x = 0;
const int& y = x;
int& z = const_cast<int&>(y);
return z;],
 ac_cv_cxx_const_cast=yes, ac_cv_cxx_const_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_const_cast" = yes; then
  AC_DEFINE(HAVE_CONST_CAST,,[define if the compiler supports const_cast<>])
fi
])


dnl @synopsis AC_CXX_DEFAULT_TEMPLATE_PARAMETERS
dnl
dnl If the compiler supports default template parameters,
dnl define HAVE_DEFAULT_TEMPLATE_PARAMETERS.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_DEFAULT_TEMPLATE_PARAMETERS],
[AC_CACHE_CHECK(whether the compiler supports default template parameters,
ac_cv_cxx_default_template_parameters,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([template<class T = double, int N = 10> class A {public: int f() {return 0;}};
],[A<float> a; return a.f();],
 ac_cv_cxx_default_template_parameters=yes, ac_cv_cxx_default_template_parameters=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_default_template_parameters" = yes; then
  AC_DEFINE(HAVE_DEFAULT_TEMPLATE_PARAMETERS,,
            [define if the compiler supports default template parameters])
fi
rm -rf SunWS_cache
])


dnl @synopsis AC_CXX_EXCEPTIONS
dnl
dnl If the C++ compiler supports exceptions handling (try,
dnl throw and catch), define HAVE_EXCEPTIONS.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_EXCEPTIONS],
[AC_CACHE_CHECK(whether the compiler supports exceptions,
ac_cv_cxx_exceptions,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[try { throw  1; } catch (int i) { return i; }],
 ac_cv_cxx_exceptions=yes, ac_cv_cxx_exceptions=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_exceptions" = yes; then
  AC_DEFINE(HAVE_EXCEPTIONS,,[define if the compiler supports exceptions])
fi
])

dnl @synopsis AC_CXX_EXPLICIT
dnl
dnl If the compiler can be asked to prevent using implicitly one argument
dnl constructors as converting constructors with the explicit
dnl keyword, define HAVE_EXPLICIT.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_EXPLICIT],
[AC_CACHE_CHECK(whether the compiler supports the explicit keyword,
ac_cv_cxx_explicit,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([class A{public:explicit A(double){}};],
[double c = 5.0;A x(c);return 0;],
 ac_cv_cxx_explicit=yes, ac_cv_cxx_explicit=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_explicit" = yes; then
  AC_DEFINE(HAVE_EXPLICIT,,[define if the compiler supports the explicit keyword])
fi
])


dnl @synopsis AC_CXX_HAVE_STD
dnl
dnl If the compiler supports ISO C++ standard library (i.e., can include the
dnl files iostream, map, and iomanip), define HAVE_STD.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_HAVE_STD],
[AC_CACHE_CHECK(whether the compiler supports ISO C++ standard library,
ac_cv_cxx_have_std,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <iostream>
#include <map>
#include <iomanip>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[return 0;],
 ac_cv_cxx_have_std=yes, ac_cv_cxx_have_std=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_std" = yes; then
  AC_DEFINE(HAVE_STD,,[define if the compiler supports ISO C++ standard library])
fi
])


dnl @synopsis AC_CXX_HAVE_STL
dnl
dnl If the compiler supports the Standard Template Library, define HAVE_STL.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_HAVE_STL],
[AC_CACHE_CHECK(whether the compiler supports Standard Template Library,
ac_cv_cxx_have_stl,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <list>
#include <deque>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[list<int> x; x.push_back(5);
list<int>::iterator iter = x.begin(); if (iter != x.end()) ++iter; return 0;],
 ac_cv_cxx_have_stl=yes, ac_cv_cxx_have_stl=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_stl" = yes; then
  AC_DEFINE(HAVE_STL,,[define if the compiler supports Standard Template Library])
fi
rm -rf SunWS_cache
])

dnl @synopsis AC_CXX_IOS_BINARY
dnl
dnl If the compiler supports ios::binary
dnl
dnl @author The Written Word, Inc.
dnl
AC_DEFUN([AC_CXX_IOS_BINARY],
[AC_CACHE_CHECK(whether the compiler supports ios::binary,
ac_cv_cxx_ios_binary,
[AC_LANG_PUSH(C++)
 AC_TRY_COMPILE([#include <fstream>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[
ifstream in( "/dev/null", ios::binary );],
 ac_cv_cxx_ios_binary=yes, ac_cv_cxx_ios_binary=no)
 AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_ios_binary" = no; then
  AC_DEFINE(MISSING_STD_IOS_BINARY,,[define if the compiler lacks ios::binary])
fi
rm -rf SunWS_cache
])

dnl @synopsis AC_CXX_MEMBER_TEMPLATES_OUTSIDE_CLASS
dnl
dnl If the compiler supports member templates outside the class declaration,
dnl define HAVE_MEMBER_TEMPLATES_OUTSIDE_CLASS.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_MEMBER_TEMPLATES_OUTSIDE_CLASS],
[AC_CACHE_CHECK(whether the compiler supports member templates outside the class declaration,
ac_cv_cxx_member_templates_outside_class,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([
#template<class T, int N> class A
{ public :
  template<int N2> A<T,N> operator=(const A<T,N2>& z);
};
template<class T, int N> template<int N2>
A<T,N> A<T,N>::operator=(const A<T,N2>& z){ return A<T,N>(); }],[
A<double,4> x; A<double,7> y; x = y; return 0;],
 ac_cv_cxx_member_templates_outside_class=yes, ac_cv_cxx_member_templates_outside_class=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_member_templates_outside_class" = yes; then
  AC_DEFINE(HAVE_MEMBER_TEMPLATES_OUTSIDE_CLASS,,
            [define if the compiler supports member templates outside the class declaration])
fi
rm -rf SunWS_cache
])


dnl @synopsis AC_CXX_MUTABLE
dnl
dnl If the compiler allows modifying class data members flagged with
dnl the mutable keyword even in const objects (for example in the
dnl body of a const member function), define HAVE_MUTABLE.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_MUTABLE],
[AC_CACHE_CHECK(whether the compiler supports the mutable keyword,
ac_cv_cxx_mutable,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([class A { mutable int i;
          public:
          int f (int n) const { i = n; return i; }
        };
],[A a; return a.f (1);],
 ac_cv_cxx_mutable=yes, ac_cv_cxx_mutable=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_mutable" = yes; then
  AC_DEFINE(HAVE_MUTABLE,,[define if the compiler supports the mutable keyword])
fi
])



dnl @synopsis AC_CXX_NAMESPACES
dnl
dnl If the compiler can prevent names clashes using namespaces, define
dnl HAVE_NAMESPACES.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])


dnl @synopsis AC_CXX_NEW_FOR_SCOPING
dnl
dnl If the compiler accepts the new for scoping rules (the scope of a
dnl variable declared inside the parentheses is restricted to the
dnl for-body), define HAVE_NEW_FOR_SCOPING.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_NEW_FOR_SCOPING],
[AC_CACHE_CHECK(whether the compiler accepts the new for scoping rules,
ac_cv_cxx_new_for_scoping,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[  int z = 0;
  for (int i = 0; i < 10; ++i)
    z = z + i;
  for (int i = 0; i < 10; ++i)
    z = z - i;
  return z;],
 ac_cv_cxx_new_for_scoping=yes, ac_cv_cxx_new_for_scoping=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_new_for_scoping" = yes; then
  AC_DEFINE(HAVE_NEW_FOR_SCOPING,,[define if the compiler accepts the new for scoping rules])
fi
])

dnl @synopsis AC_CXX_STATIC_CAST
dnl
dnl If the compiler supports static_cast<>, define HAVE_STATIC_CAST.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_STATIC_CAST],
[AC_CACHE_CHECK(whether the compiler supports static_cast<>,
ac_cv_cxx_static_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0; };
class Derived : public Base { public : Derived () {} virtual void f () {} };
int g (Derived&) { return 0; }],[
Derived d; Base& b = d; Derived& s = static_cast<Derived&> (b); return g (s);],
 ac_cv_cxx_static_cast=yes, ac_cv_cxx_static_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_cast" = yes; then
  AC_DEFINE(HAVE_STATIC_CAST,,
            [define if the compiler supports static_cast<>])
fi
])


dnl @synopsis AC_CXX_TEMPLATES
dnl
dnl If the compiler supports basic templates, define HAVE_TEMPLATES.
dnl
dnl @author Luc Maisonobe
dnl
AC_DEFUN([AC_CXX_TEMPLATES],
[AC_CACHE_CHECK(whether the compiler supports basic templates,
ac_cv_cxx_templates,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([template<class T> class A {public:A(){}};
template<class T> void f(const A<T>& ){}],[
A<double> d; A<int> i; f(d); f(i); return 0;],
 ac_cv_cxx_templates=yes, ac_cv_cxx_templates=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_templates" = yes; then
  AC_DEFINE(HAVE_TEMPLATES,,[define if the compiler supports basic templates])
fi
rm -rf SunWS_cache
])


dnl @synopsis ACX_PTHREAD([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
dnl
dnl This macro figures out how to build C programs using POSIX
dnl threads.  It sets the PTHREAD_LIBS output variable to the threads
dnl library and linker flags, and the PTHREAD_CFLAGS output variable
dnl to any special C compiler flags that are needed.  (The user can also
dnl force certain compiler flags/libs to be tested by setting these
dnl environment variables.)
dnl
dnl Also sets PTHREAD_CC to any special C compiler that is needed for
dnl multi-threaded programs (defaults to the value of CC otherwise).
dnl (This is necessary on AIX to use the special cc_r compiler alias.)
dnl
dnl If you are only building threads programs, you may wish to
dnl use these variables in your default LIBS, CFLAGS, and CC:
dnl
dnl        LIBS="$PTHREAD_LIBS $LIBS"
dnl        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
dnl        CC="$PTHREAD_CC"
dnl
dnl In addition, if the PTHREAD_CREATE_JOINABLE thread-attribute
dnl constant has a nonstandard name, defines PTHREAD_CREATE_JOINABLE
dnl to that name (e.g. PTHREAD_CREATE_UNDETACHED on AIX).
dnl
dnl ACTION-IF-FOUND is a list of shell commands to run if a threads
dnl library is found, and ACTION-IF-NOT-FOUND is a list of commands
dnl to run it if it is not found.  If ACTION-IF-FOUND is not specified,
dnl the default action will define HAVE_PTHREAD.
dnl
dnl Please let the authors know if this macro fails on any platform,
dnl or if you have any other suggestions or comments.  This macro was
dnl based on work by SGJ on autoconf scripts for FFTW (www.fftw.org)
dnl (with help from M. Frigo), as well as ac_pthread and hb_pthread
dnl macros posted by AFC to the autoconf macro repository.  We are also
dnl grateful for the helpful feedback of numerous users.
dnl
dnl @version $Id$
dnl @author Steven G. Johnson <stevenj@alum.mit.edu> and Alejandro Forero Cuervo <bachue@bachue.com>

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all.

acx_pthread_flags="-lpthreads none -Kthread -kthread -llthread -pthread -pthreads -mthreads -lpthread --thread-safe -mt"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthread or
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthread -pthreads -lpthread -mt $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -l*)
                AC_MSG_CHECKING([for the pthreads library $flag])
                PTHREAD_LIBS="$flag"
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: threads are created detached by default
        # and the JOINABLE attribute has a nonstandard name (UNDETACHED).
        AC_MSG_CHECKING([for joinable pthread attribute])
        AC_TRY_LINK([#include <pthread.h>],
                    [int attr=PTHREAD_CREATE_JOINABLE;],
                    ok=PTHREAD_CREATE_JOINABLE, ok=unknown)
        if test x"$ok" = xunknown; then
                AC_TRY_LINK([#include <pthread.h>],
                            [int attr=PTHREAD_CREATE_UNDETACHED;],
                            ok=PTHREAD_CREATE_UNDETACHED, ok=unknown)
        fi
        if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
                AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
                          [Define to the necessary symbol if this constant
                           uses a non-standard name on your system.])
        fi
        AC_MSG_RESULT(${ok})
        if test x"$ok" = xunknown; then
                AC_MSG_WARN([we do not know how to create joinable pthreads])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
                *-aix* | *-freebsd*)     flag="-D_THREAD_SAFE";;
                *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
                PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r (or xlc_r)
        case "${host_os}" in
          aix* )
            case "$CC" in
              *xlc )
                AC_CHECK_PROG(PTHREAD_CC, xlc_r, xlc_r, ${CC}) ;;
              *cc )
                AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC}) ;;
            esac
            case "$CXX" in
              *xlC )
                AC_CHECK_PROG(PTHREAD_CXX, xlC_r, xlC_r, ${CXX}) ;;
            esac
            ;;
        esac
fi

if test "${PTHREAD_CC}x" = "x"
then
  PTHREAD_CC="$CC"
fi
if test "${PTHREAD_CXX}x" = "x"
then
  PTHREAD_CXX="$CXX"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)
AC_SUBST(PTHREAD_CXX)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

#
# Macro to test for pthread library
# Written by Bob Friesenhahn based on test in ACX_PTHREAD
# MAGICK_CHECK_PTHREAD_LIB(LIBRARY,
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
AC_DEFUN([MAGICK_CHECK_PTHREAD_LIB], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
magick_pthread_lib_ok=no

LIB=-l$1
save_LIBS="$LIBS"
LIBS="$LIBS $LIB"

AC_MSG_CHECKING([for the pthreads library $LIB])
AC_TRY_LINK([#include <pthread.h>],
  [  pthread_t th;
  pthread_join(th, 0);
  pthread_attr_init(0);
  pthread_cleanup_push(0, 0);
  pthread_create(0,0,0,0);
  pthread_cleanup_pop(0); ],
  [magick_pthread_lib_ok=yes])

AC_MSG_RESULT(${magick_pthread_lib_ok})
if test "$magick_pthread_lib_ok" = yes
then
  $2
  :
else
  $3
  :
fi

LIBS="$save_LIBS"

AC_LANG_RESTORE
])dnl MAGICK_CHECK_PTHREAD_LIB


# GM_FUNC_MMAP_FILEIO
# ------------
AC_DEFUN([GM_FUNC_MMAP_FILEIO],
[AC_CHECK_HEADERS(stdlib.h unistd.h)
AC_CHECK_FUNCS(getpagesize)
AC_CACHE_CHECK(for working mmap file i/o, gm_cv_func_mmap_fileio,
[AC_RUN_IFELSE([AC_LANG_SOURCE([AC_INCLUDES_DEFAULT]
[[/* malloc might have been renamed as rpl_malloc. */
#undef malloc

/*
   This test is derived from GNU Autoconf's similar macro.
   The purpose of this test is to verify that files may be memory
   mapped, and that memory mapping and file I/O are coherent.

   The test creates a test file, memory maps the file, updates
   the file using the memory map, and then reads the file using
   file I/O to verify that the file contains the updates.
*/

#include <fcntl.h>
#include <sys/mman.h>

#if !STDC_HEADERS && !HAVE_STDLIB_H
char *malloc ();
#endif

/* This mess was copied from the GNU getpagesize.h.  */
#if !HAVE_GETPAGESIZE
/* Assume that all systems that can run configure have sys/param.h.  */
# if !HAVE_SYS_PARAM_H
#  define HAVE_SYS_PARAM_H 1
# endif

# ifdef _SC_PAGESIZE
#  define getpagesize() sysconf(_SC_PAGESIZE)
# else /* no _SC_PAGESIZE */
#  if HAVE_SYS_PARAM_H
#   include <sys/param.h>
#   ifdef EXEC_PAGESIZE
#    define getpagesize() EXEC_PAGESIZE
#   else /* no EXEC_PAGESIZE */
#    ifdef NBPG
#     define getpagesize() NBPG * CLSIZE
#     ifndef CLSIZE
#      define CLSIZE 1
#     endif /* no CLSIZE */
#    else /* no NBPG */
#     ifdef NBPC
#      define getpagesize() NBPC
#     else /* no NBPC */
#      ifdef PAGESIZE
#       define getpagesize() PAGESIZE
#      endif /* PAGESIZE */
#     endif /* no NBPC */
#    endif /* no NBPG */
#   endif /* no EXEC_PAGESIZE */
#  else /* no HAVE_SYS_PARAM_H */
#   define getpagesize() 8192	/* punt totally */
#  endif /* no HAVE_SYS_PARAM_H */
# endif /* no _SC_PAGESIZE */

#endif /* no HAVE_GETPAGESIZE */

int
main ()
{
  char *data, *data2, *data3;
  int i, pagesize;
  int fd;

  pagesize = getpagesize ();

  /* First, make a file with some known garbage in it. */
  data = (char *) malloc (pagesize);
  if (!data)
    exit (1);
  for (i = 0; i < pagesize; ++i)
    *(data + i) = rand ();
  umask (0);
  fd = creat ("conftest.mmap", 0600);
  if (fd < 0)
    exit (1);
  if (write (fd, data, pagesize) != pagesize)
    exit (1);
  close (fd);

  /* Mmap the file as read/write/shared and verify that we see the
  same garbage. */
  fd = open ("conftest.mmap", O_RDWR);
  if (fd < 0)
    exit (1);
  data2 = mmap (0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0L);
  if (data2 == 0)
    exit (1);
  for (i = 0; i < pagesize; ++i)
    if (*(data + i) != *(data2 + i))
      exit (1);

  /* Finally, make sure that changes to the mapped area 
     percolate back to the file as seen by read().  */
  for (i = 0; i < pagesize; ++i)
    *(data2 + i) = *(data2 + i) + 1;
  data3 = (char *) malloc (pagesize);
  if (!data3)
    exit (1);
  if (read (fd, data3, pagesize) != pagesize)
    exit (1);
  for (i = 0; i < pagesize; ++i)
    if (*(data2 + i) != *(data3 + i))
      exit (1);
  close (fd);
  exit (0);
}]])],
	       [gm_cv_func_mmap_fileio=yes],
	       [gm_cv_func_mmap_fileio=no],
	       [gm_cv_func_mmap_fileio=no])])
if test $gm_cv_func_mmap_fileio = yes; then
  AC_DEFINE(HAVE_MMAP_FILEIO, 1,
	    [Define to 1 if you have a `mmap' system call which handles coherent file I/O.])
fi
rm -f conftest.mmap
])# GM_FUNC_MMAP_FILEIO

dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_prog_perl_version.html
dnl
AC_DEFUN([AC_PROG_PERL_VERSION],[dnl
# Make sure we have perl
if test -z "$PERL"; then
AC_CHECK_PROG(PERL,perl,perl)
fi

# Check if version of Perl is sufficient
ac_perl_version="$1"

if test "x$PERL" != "x"; then
  AC_MSG_CHECKING(for perl version greater than or equal to $ac_perl_version)
  # NB: It would be nice to log the error if there is one, but we cannot rely
  # on autoconf internals
  $PERL -e "use $ac_perl_version;" > /dev/null 2>&1
  if test $? -ne 0; then
    AC_MSG_RESULT(no);
    $3
  else
    AC_MSG_RESULT(ok);
    $2
  fi
else
  AC_MSG_WARN(could not find perl)
fi
])dnl # AC_PROG_PERL_VERSION