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

acx_pthread_ok=no

dnl variable to keep track of whether we have already added -D_THREAD_SAFE
using_THREAD_SAFE=no

dnl First, check if the POSIX threads header, pthread.h, is available.
dnl If it isn't, don't bother looking for the threads libraries.
AC_CHECK_HEADER(pthread.h, , acx_pthread_ok=noheader)

dnl We must check for the threads library under a number of different
dnl names; the ordering is very important because some systems
dnl (e.g. DEC) have both -lpthread and -lpthreads, where one of the
dnl libraries is broken (non-POSIX).

dnl First of all, check if the user has set any of the PTHREAD_LIBS,
dnl etcetera environment variables, and if threads linking works using
dnl them:
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

dnl POSIX threads library is plural on AIX (need to check for
dnl this *first* due to AIX brokenness; also, need to check
dnl for pthread_attr_init instead of pthread_create due to
dnl DEC craziness):
if test x"$acx_pthread_ok" = xno; then
        AC_CHECK_LIB(pthreads, pthread_attr_init,
                     [PTHREAD_LIBS="-lpthreads"
                      acx_pthread_ok=yes])
fi

dnl Check if no explicit threads library is needed; this should be
dnl done before -kthread/-Kthread, since otherwise those may work but
dnl simply produce continual annoying compiler warnings.  Also, include
dnl pthread.h since on the Sequent -Kthread is needed for this header
dnl file to work (see below).
if test x"$acx_pthread_ok" = xno; then
        dnl Check for pthread_join because of Irix, which has pthread_create
        dnl in libc.so and pthread_join in libpthread.so.  Lose, lose, lose.
        AC_MSG_CHECKING(whether threads work without any explicit flags)
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);],
                    [acx_pthread_ok=yes])
        AC_MSG_RESULT($acx_pthread_ok)
fi

dnl Try -Kthread for Sequent systems: it is required to parse pthread.h,
dnl although not for linking (threads are in libc).  (Thanks to Chris
dnl Lattner of Sequent for his help with this machine.)
if test x"$acx_pthread_ok" = xno; then
        AC_MSG_CHECKING(for Sequent Kthread compiler flag)
        save_CFLAGS="$CFLAGS"
        PTHREAD_CFLAGS="-Kthread"
        CFLAGS="$save_CFLAGS $PTHREAD_CFLAGS"
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);],
                    [acx_pthread_ok=yes])
        if  test x"$acx_pthread_ok" = xyes; then
                AC_MSG_RESULT($PTHREAD_CFLAGS)
        else
                PTHREAD_CFLAGS=""
                AC_MSG_RESULT(no)
        fi
        CFLAGS="$save_CFLAGS"
fi

dnl Now, try -kthread, for FreeBSD kernel threads.  We should do
dnl this before checking for -lpthread, because -kthread actually
dnl uses -lpthread.
if test x"$acx_pthread_ok" = xno; then
        AC_MSG_CHECKING(for bsd kthread compiler flags)
        save_CFLAGS="$CFLAGS"
        PTHREAD_CFLAGS="-kthread -D_THREAD_SAFE"
        CFLAGS="$save_CFLAGS $PTHREAD_CFLAGS"
        dnl Use AC_TRY_LINK_FUNC instead of AC_CHECK_FUNC, to prevent
        dnl the latter's results-caching from screwing us.  We check for
        dnl pthread_join and not pthread_create because of Irix; see above.
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        if  test x"$acx_pthread_ok" = xyes; then
                AC_MSG_RESULT($PTHREAD_CFLAGS)
                using_THREAD_SAFE=yes
        else
                PTHREAD_CFLAGS=""
                AC_MSG_RESULT(no)
        fi
        CFLAGS="$save_CFLAGS"
fi

dnl Normally (e.g. on Linux), POSIX threads are in -lpthread.
dnl We can't just use AC_CHECK_LIB, though, because DEC lossage
dnl requires that pthread.h be included for linking to work.
if test x"$acx_pthread_ok" = xno; then
        AC_MSG_CHECKING([for pthread_create in -lpthread])
        save_LIBS="$LIBS"
        LIBS="-lpthread $LIBS"
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_create(0,0,0,0);],
                    [PTHREAD_LIBS="-lpthread"
                     acx_pthread_ok=yes])
        LIBS="$save_LIBS"
        AC_MSG_RESULT(${acx_pthread_ok})
fi

dnl Now, try -llthread, for LinuxThreads (e.g. the LinuxThreads port
dnl on FreeBSD).  Do this before -pthread, below, because LinuxThreads
dnl are kernel threads and take advantage of SMP, unlike userland threads.
if test x"$acx_pthread_ok" = xno; then
        AC_CHECK_LIB(lthread, pthread_create,
                     [PTHREAD_LIBS="-llthread"
                      acx_pthread_ok=yes])
fi

dnl Next, try -pthread for FreeBSD userland threads.  We do this
dnl after checking for -lpthread, because -pthread actually is a
dnl valid gcc flag on several systems and links the threads library,
dnl but I prefer -lpthread because -pthread seems to be undocumented
dnl (and thus, to my mind, untrustworthy) on non-BSD systems.
if test x"$acx_pthread_ok" = xno; then
        AC_MSG_CHECKING(for bsd pthread compiler flags)
        save_CFLAGS="$CFLAGS"
        PTHREAD_CFLAGS="-pthread -D_THREAD_SAFE"
        CFLAGS="$save_CFLAGS $PTHREAD_CFLAGS"
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        if  test x"$acx_pthread_ok" = xyes; then
                AC_MSG_RESULT($PTHREAD_CFLAGS)
                using_THREAD_SAFE=yes
        else
                PTHREAD_CFLAGS=""
                AC_MSG_RESULT(no)
        fi
        CFLAGS="$save_CFLAGS"
fi

dnl Various other checks:
if test x"$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        dnl Detect AIX lossage: threads are created detached by default
        dnl and the JOINABLE attribute has a nonstandard name (UNDETACHED).
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

        dnl More AIX/DEC lossage: must compile with -D_THREAD_SAFE
        dnl (also on FreeBSD) or -D_REENTRANT: (cc_r subsumes this on AIX,
        dnl but it doesn't hurt to -D as well, esp. if cc_r is not available.)
        AC_MSG_CHECKING([if more special flags are required for pthreads])
        ok=no
        AC_REQUIRE([AC_CANONICAL_HOST])
        case "${host_cpu}-${host_os}" in
                *-aix* | *-freebsd*)
                if test x"$using_THREAD_SAFE" = xno; then
                        PTHREAD_CFLAGS="-D_THREAD_SAFE $PTHREAD_CFLAGS"
                        ok="-D_THREAD_SAFE"
                fi;;
                alpha*-osf*)  PTHREAD_CFLAGS="-D_REENTRANT $PTHREAD_CFLAGS"
                        ok="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${ok})

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        dnl More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

dnl Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi

])dnl ACX_PTHREADS



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
 AC_TRY_COMPILE([
int f(int  x){return 1;}
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
 AC_TRY_COMPILE(,[int x = 0;const int& y = x;int& z = const_cast<int&>(y);return z;],
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
 AC_TRY_COMPILE([
template<class T = double, int N = 10> class A {public: int f() {return 0;}};
],[A<float> a; return a.f();],
 ac_cv_cxx_default_template_parameters=yes, ac_cv_cxx_default_template_parameters=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_default_template_parameters" = yes; then
  AC_DEFINE(HAVE_DEFAULT_TEMPLATE_PARAMETERS,,
            [define if the compiler supports default template parameters])
fi
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
dnl files iostream, map, iomanip and cmath}), define HAVE_STD.
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
#include <cmath>
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
template<class T, int N> class A
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
 AC_TRY_COMPILE([
class A { mutable int i;
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
 AC_TRY_COMPILE(,[
  int z = 0;
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
])


