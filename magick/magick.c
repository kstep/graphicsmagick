/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%            Methods to Read or List GraphicsMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#if defined(WIN32) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/module.h"
#include "magick/registry.h"
#include "magick/resource.h"
#include "magick/render.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"

/*
  Global declarations.
*/
typedef enum  {
  InitDefault,
  InitUninitialized,
  InitInitialized
} MagickInitializationState;

static SemaphoreInfo
  *magick_semaphore = (SemaphoreInfo *) NULL;

static MagickInfo
  *magick_list = (MagickInfo *) NULL;

static MagickInitializationState MagickInitialized = InitDefault;


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagick() destroys the GraphicsMagick environment.
%
%  The format of the DestroyMagick function is:
%
%      DestroyMagick(void)
%
%
*/
MagickExport void DestroyMagick(void)
{

  if (MagickInitialized == InitUninitialized)
    return;

  DestroyColorInfo();
  DestroyDelegateInfo();
  DestroyTypeInfo();
  DestroyModuleInfo();
  DestroyMagicInfo();
  DestroyMagickInfo();
  DestroyLogInfo();
  DestroyConstitute();
  DestroyMagickRegistry();
  DestroyMagickResources();
  DestroyTemporaryFiles();
  DestroySemaphore();
#if defined(WIN32)
  DestroyTracingCriticalSection();
  NTGhostscriptUnLoadDLL();
#endif

  MagickInitialized=InitUninitialized;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickInfo() deallocates memory associated MagickInfo list.
%
%  The format of the DestroyMagickInfo method is:
%
%      void DestroyMagickInfo(void)
%
*/
MagickExport void DestroyMagickInfo(void)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    magick_info=p;
    p=p->next;
    if (magick_info->name != (char *) NULL)
      MagickFreeMemory(magick_info->name);
    if (magick_info->description != (char *) NULL)
      MagickFreeMemory(magick_info->description);
    if (magick_info->version != (char *) NULL)
      MagickFreeMemory(magick_info->version);
    if (magick_info->note != (char *) NULL)
      MagickFreeMemory(magick_info->note);
    if (magick_info->module != (char *) NULL)
      MagickFreeMemory(magick_info->module);
    MagickFreeMemory(magick_info);
  }
  magick_list=(MagickInfo *) NULL;
  DestroySemaphoreInfo(&magick_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e M a g i c k                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageMagick() searches for an image format that matches the specified
%  magick string.  If one is found the name is returned otherwise NULL.
%
%  The format of the GetImageMagick method is:
%
%      const char *GetImageMagick(const unsigned char *magick,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: The image format we are searching for.
%
%    o length: The length of the binary string.
%
%
*/
MagickExport const char *GetImageMagick(const unsigned char *magick,
  const size_t length)
{
  register MagickInfo
    *p;

  assert(magick != (const unsigned char *) NULL);
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (p->magick && p->magick(magick,length))
      break;
  LiberateSemaphoreInfo(&magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p->name);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfo() returns a pointer MagickInfo structure that matches
%  the specified name.  If name is NULL, the head of the image format list
%  is returned.
%
%  The format of the GetMagickInfo method is:
%
%     const MagickInfo *GetMagickInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The image format we are looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const MagickInfo *GetMagickInfo(const char *name,
  ExceptionInfo *exception)
{
  register MagickInfo
    *p;

  if ((name != (const char *) NULL) && (LocaleCompare(name,"*") == 0))
    (void) OpenModules(exception);
  if (exception->severity > UndefinedException)
    return 0;

  AcquireSemaphoreInfo(&magick_semaphore);
  if (magick_list != (MagickInfo *) NULL)
    LiberateSemaphoreInfo(&magick_semaphore);
  else
    {
      MagickInfo
        *entry;

      /*
        Register image formats.
      */
      LiberateSemaphoreInfo(&magick_semaphore);
      entry=SetMagickInfo("IMPLICIT");
      entry->stealth=True;
      (void) RegisterMagickInfo(entry);

      /*
        Load modules and check for any error
      */
      (void) GetModuleInfo((char *) NULL,exception);
      if (exception->severity > UndefinedException)
        return 0;
    }
  if ((name == (const char *) NULL) ||  (LocaleCompare(name,"*") == 0))
    return((const MagickInfo *) magick_list);
  /*
    Find name in list
  */
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      break;
  if (p == (MagickInfo *) NULL)
    {
      if (*name != '\0')
        {
          ExceptionInfo
            module_exception;

          LiberateSemaphoreInfo(&magick_semaphore);
          GetExceptionInfo(&module_exception);
          (void) OpenModule(name,&module_exception);
          DestroyExceptionInfo(&module_exception);
          AcquireSemaphoreInfo(&magick_semaphore);
        }
      for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
        if (LocaleCompare(p->name,name) == 0)
          break;
    }
  if (p != (MagickInfo *) NULL)
    if (p != magick_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (MagickInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (MagickInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(MagickInfo *) NULL;
        p->next=magick_list;
        magick_list->previous=p;
        magick_list=p;
      }
  LiberateSemaphoreInfo(&magick_semaphore);
  return((const MagickInfo *) p);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k A r r a y                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfoArray() returns a sorted null-terminated array of MagickInfo
%  pointers corresponding to the available format registrations. If necessarly
%  all modules are loaded in order to return a complete list. This function
%  should be used to access the entire list rather than GetMagickInfo since
%  the list returned by GetMagickInfo may be re-ordered every time it is
%  invoked.
%
%  The format of the GetMagickList method is:
%
%      const MagickInfo **GetMagickInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two MagickInfo structures based on their name
*/
static int MagickInfoCompare(const void *x, const void *y)
{
  const MagickInfo
    *xx=*((const MagickInfo **) x),
    *yy=*((const MagickInfo **) y);

  return (strcmp(xx->name, yy->name));
}
MagickExport const MagickInfo **GetMagickInfoArray(ExceptionInfo *exception)
{
  const MagickInfo
    **array,
    *p;

  const MagickInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load all modules and obtain pointer to head of list
  */
  list=GetMagickInfo("*",exception);
  if ((!list) || (exception->severity > UndefinedException))
    return 0;

  AcquireSemaphoreInfo(&magick_semaphore);

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateMemory(const MagickInfo **,sizeof(MagickInfo *)*entries+1);
  memset((void **)array,0,entries+1);

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;

  LiberateSemaphoreInfo(&magick_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(MagickInfo *), MagickInfoCompare);

  return array;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e M a g i c k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagick() initializes the GraphicsMagick environment.
%
%  The format of the InitializeMagick function is:
%
%      InitializeMagick(const char *path)
%
%  A description of each parameter follows:
%
%    o path: The execution path of the current GraphicsMagick client.
%
%
*/

/*
  Due to legacy compilers, POSIX.1 actually allows the signal function
  to return 'int' or 'void'. The compiler is not so lenient so we must
  ensure that the return type matches the prototype in the headers.
*/
#if !defined(RETSIGTYPE)
# define RETSIGTYPE void
#endif
typedef RETSIGTYPE Sigfunc(int);

#if !defined(SIG_ERR)
# define SIG_ERR (Sigfunc *)-1
#endif
#if !defined(SIG_DFL)
# define SIG_DFL (Sigfunc *)0
#endif

/*
  Signal function which prevents interrupted system calls from
  automatically being restarted. From W. Richard Stevens "Advanced
  Programming in the UNIX Environment", Chapter 10.14.
*/
static Sigfunc *MagickSignal(int signo, Sigfunc *func)
{
#if defined(HAVE_SIGACTION) && defined(HAVE_SIGEMPTYSET)
  struct sigaction
    act,
    oact;

  act.sa_handler=func;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
#  if defined(SA_INTERRUPT)  /* SunOS */
  act.sa_flags |= SA_INTERRUPT;
#  endif
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return (oact.sa_handler);
#else
  return signal(signo, func);
#endif
}

/*
  Signal function which only sets the signal handler if it its handling
  is still set to SIG_DFL. This ensures that signal handlers are not
  registered for signals which are being ignored, or that the API
  user has already registered before invoking InitializeMagick.

  If an API user registers its own signal hander, then it is responsible
  for invoking DestroyMagick when a signal is received.
*/
static Sigfunc *MagickCondSignal(int signo, Sigfunc *func)
{
  Sigfunc *
    o_handler;

  if ((o_handler=MagickSignal(signo,func)) != SIG_ERR)
    {
      /*
        If handler is not the default, then restore the previous
        setting.
      */
      if (o_handler != SIG_DFL)
        MagickSignal(signo,o_handler);
      else
        (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
          "Registered signal handler for signal ID %d",signo);
    }
  return (o_handler);
}

/*
  Signal handler to ensure that DestroyMagick is invoked in case the
  user aborts the program.

  The WIN32 documentation says that SIGINT is not supported under any
  version of Windows. It also says that a new thread is created to
  handle the interrupt caused by CNTRL+C. The Windows signal
  documentation also says that it is unsafe to do anything useful from
  within a signal handler.  This means that malloc/free and any system
  call should not be executed. Theoretically DestroyMagick() should
  not be executed since its purpose is to invoke unsafe functions.
  SIGILL, SIGSEGV, and SIGTERM are documented to never be generated by
  Windows NT.

  While support for signal is definitely broken under Windows, the good
  news is that it seems to be unlikely to generate a signal we care about.
*/
static RETSIGTYPE MagickSignalHandler(int signo)
{
  /*
    Release resources
  */
  DestroyMagick();

  /*
    Restore default handling for the signal
  */
  MagickSignal(signo,SIG_DFL);

  /*
    Raise signal again to invoke default handler
    This may cause a core dump or immediate exit.
  */
#if defined(HAVE_RAISE)
  raise(signo);
#endif

  Exit(signo);
}

MagickExport void InitializeMagick(const char *path)
{
  char
    execution_path[MaxTextExtent];

  if (MagickInitialized == InitInitialized)
    return;

  MagickInitialized=InitInitialized;

  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");

  /* Seed the random number generator */
  srand(time(0));

  /* Initialize semaphores */
  InitializeSemaphore();

  /*
    Set logging flags using value of MAGICK_DEBUG if it is set in
    the environment.
  */
  {
    const char
      *magick_debug=getenv("MAGICK_DEBUG");
    
    if (magick_debug)
      (void) SetLogEventMask(magick_debug);
  }

  /*
    Compute memory allocation and memory map resource limits based
    on available memory
  */
  InitializeMagickResources();

  *execution_path='\0';
#if !defined(UseInstalledMagick)
#if defined(POSIX) || defined(WIN32)
  /* passing NULL means that we want the code to try to figure it out via
     asking the operating system
   */
  if (path == (const char *) NULL)
    {
      (void) GetExecutionPath(execution_path);
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Path to app is NULL - OS says: \"%s\"",execution_path);
    }
  /* we also check to see if the path seems to be a path by looking for
     the presence of directory seperators and some reasonable length
   */
  else if ((strchr(path,*DirectorySeparator) == (char *) NULL) ||
      (strlen(path) <= 2))
    {
      (void) GetExecutionPath(execution_path);
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Path to app \"%s\" is bad - OS says: \"%s\"",path,
          execution_path);
    }
  /* We get here if the caller seems to have called us with a legit path
     to the application.
   */
  else
    {
#if defined(POSIX)
      /* For POSIX we check the first character to see of it is a file
         system path seperator. If not then we ignore the passed data
       */
      if (*path == *DirectorySeparator)
        {
          (void) strncpy(execution_path,path,MaxTextExtent-1);
        }
#elif defined(WIN32)
      /* For Windows we check to see if the path passed seems to be a UNC
         path or one with a drive letter spec in it: \\Server\share, C:\
       */
      if (((*path == *DirectorySeparator) &&
           (*(path+1) == *DirectorySeparator)) || (*(path+1) == ':'))
        {
          (void) strncpy(execution_path,path,MaxTextExtent-1);
        }
#else
      /* In any other case, we just let it go right through unscathed */
      if (1)
        {
          (void) strncpy(execution_path,path,MaxTextExtent-1);
        }
#endif
      else
        {
          /* This logic is UNIX only and tries for figure out the path to
             application using a call to getcwd and then assuming that the
             passed string is just the name of the application. It then
             combines those two things.
           */
          (void) getcwd(execution_path,MaxTextExtent-2);
          (void) strcat(execution_path,DirectorySeparator);
          if((*path == '.') && (*(path+1) == *DirectorySeparator))
            (void) strncat(execution_path,path+2,MaxTextExtent-
              strlen(execution_path)-1);
          else
            (void) strncat(execution_path,path,MaxTextExtent-
              strlen(execution_path)-1);
          (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
            "Path to app \"%s\" is bad - getcwd says: \"%s\"",path,
              execution_path);
        }
    }
#endif
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Path to the app set to: \"%s\"",execution_path);
#endif
  if (*execution_path == '\0')
    {
      if (path != (char *) NULL)
        (void) SetClientName(path);
    }
  else
    {
      char
        filename[MaxTextExtent];

      GetPathComponent(execution_path,HeadPath,filename);
      (void) SetClientPath(filename);
      GetPathComponent(execution_path,TailPath,filename);
      (void) SetClientName(filename);
    }
#if defined(WIN32)
    InitializeTracingCriticalSection();
# if defined(_DEBUG) && !defined(__BORLANDC__)
    /* if (IsEventLogging()) */
      {
        int
          debug;

        debug=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        debug|=_CRTDBG_CHECK_ALWAYS_DF;
        debug|=_CRTDBG_DELAY_FREE_MEM_DF;
        debug|=_CRTDBG_LEAK_CHECK_DF;
        // debug=_CrtSetDbgFlag(debug);
        // _ASSERTE(_CrtCheckMemory());
      }
# endif /* defined(_DEBUG) */
#endif /* defined(WIN32) */

  /*
    Establish signal handlers for common signals
  */

  /* hangup, default terminate */
#if defined(SIGHUP)
  (void) MagickCondSignal(SIGHUP,MagickSignalHandler);
#endif
  /* interrupt (rubout), default terminate */
#if defined(SIGINT) && !defined(WIN32)
  (void) MagickCondSignal(SIGINT,MagickSignalHandler);
#endif
  /* quit (ASCII FS), default terminate with core */
#if defined(SIGQUIT)
  (void) MagickCondSignal(SIGQUIT,MagickSignalHandler);
#endif
  /* software-triggered abort, default terminate with core */
#if defined(SIGABRT)
  (void) MagickCondSignal(SIGABRT,MagickSignalHandler);
#endif
  /* floating point exception, default terminate with core */
#if defined(SIGFPE)
  (void) MagickCondSignal(SIGFPE,MagickSignalHandler);
#endif
  /* software termination signal from kill, default terminate */
#if defined(SIGTERM)
  (void) MagickCondSignal(SIGTERM,MagickSignalHandler);
#endif
  /* exceeded cpu limit, default terminate with core */
#if defined(SIGXCPU)
  (void) MagickCondSignal(SIGXCPU,MagickSignalHandler);
#endif
  /* exceeded file size limit, default terminate with core */
#if defined(SIGXFSZ)
  (void) MagickCondSignal(SIGXFSZ,MagickSignalHandler);
#endif

}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M a g i c k C o n f l i c t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMagickConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the IsMagickConflict method is:
%
%      unsigned int IsMagickConflict(const char *magick)
%
%  A description of each parameter follows:
%
%    o status: Method IsMagickConflict returns true if the image format
%      conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
MagickExport int unsigned IsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
#if defined(macintosh)
  return(MACIsMagickConflict(magick));
#endif
#if defined(vms)
  return(VMSIsMagickConflict(magick));
#endif
#if defined(WIN32) || defined(__CYGWIN__)
  return(NTIsMagickConflict(magick));
#endif
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListMagickInfo() lists the image formats to a file.
%
%  The format of the ListMagickInfo method is:
%
%      unsigned int ListMagickInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file: A file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagickInfo(FILE *file,ExceptionInfo *exception)
{

  const MagickInfo
    **magick_array;

  int
    i;

  if (file == (FILE *) NULL)
    file=stdout;

  magick_array=GetMagickInfoArray(exception);
  if ((!magick_array) || (exception->severity > UndefinedException))
    return False;

  (void) fprintf(file,"   Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------------\n");
  for (i=0; magick_array[i] != 0; i++)
  {
    if (magick_array[i]->stealth)
      continue;
    (void) fprintf(file,"%9s%c  %c%c%c",magick_array[i]->name ? magick_array[i]->name : "",
      magick_array[i]->blob_support ? '*' : ' ',magick_array[i]->decoder ? 'r' : '-',
      magick_array[i]->encoder ? 'w' : '-',magick_array[i]->encoder && magick_array[i]->adjoin ? '+' : '-');
    if (magick_array[i]->description != (char *) NULL)
      (void) fprintf(file,"  %.1024s",magick_array[i]->description);
    if (magick_array[i]->version != (char *) NULL)
      (void) fprintf(file," (%.1024s)",magick_array[i]->version);
    (void) fprintf(file,"\n");
    if (magick_array[i]->note != (char *) NULL)
      {
        char
          **text;

        text=StringToList(magick_array[i]->note);
        if (text != (char **) NULL)
          {
            register long
              i;

            for (i=0; text[i] != (char *) NULL; i++)
              {
                (void) fprintf(file,"            %.1024s\n",text[i]);
                MagickFreeMemory(text[i]);
              }
            MagickFreeMemory(text);
          }
      }
  }
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
  MagickFreeMemory(magick_array);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M o d u l e M a p                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModuleMap lists the module alias info to a file in the XML
%  format used by modules.mgk. True is returned on success.
%
%  The format of the ListModuleMap method is:
%
%      unsigned int ListModuleMap(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListModuleMap(FILE *file,ExceptionInfo *exception)
{
  const MagickInfo
    **magick_array;

  int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;

   magick_array=GetMagickInfoArray(exception);
   if ((!magick_array) || (exception->severity > UndefinedException))
     return False;

   (void) fprintf(file, "<?xml version=\"1.0\"?>\n");
   (void) fprintf(file, "<!-- %s -->\n",GetMagickCopyright());
   (void) fprintf(file, "<!-- Magick Module Alias Map (modules.mgk) -->\n");
   (void) fprintf(file, "<modulemap>\n");

   for (i=0; magick_array[i] != 0; i++)
     {
       if (LocaleCompare(magick_array[i]->name,magick_array[i]->module) != 0)
         {
/*            if (i != 0) */
/*              (void) fprintf(file, "\n"); */
/*            (void) fprintf(file, "  <!-- %s -->\n",magick_array[i]->description); */
           (void) fprintf(file, "  <module magick=\"%s\" name=\"%s\" />\n",
                          magick_array[i]->name, magick_array[i]->module);
         }
     }
   (void) fprintf(file, "</modulemap>\n");
   (void) fflush(file);

   MagickFreeMemory(magick_array);

   return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k T o M i m e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickToMime returns the officially registered (or de facto) MIME
%  media-type corresponding to a magick string.  If there is no registered
%  media-type, then the string "image/x-magick" (all lower case) is returned.
%  The returned string must be deallocated by the user.
%
%  The format of the MagickToMime method is:
%
%      char *MagickToMime(const char *magick)
%
%  A description of each parameter follows.
%
%   o  magick:  GraphicsMagick format specification "magick" tag.
%
%
*/
MagickExport char *MagickToMime(const char *magick)
{
  typedef struct _MediaType
  {
    const char
      *magick,
      *media;
  } MediaType;

  char
    media[MaxTextExtent];

  MediaType
    *entry;

  static MediaType
    MediaTypes[] =
    {
      { "avi",   "video/avi" },
      { "cgm",   "image/cgm;Version=4;ProfileId=WebCGM" }, /* W3 WebCGM */
      { "dcm",   "application/dicom" }, /* Incomplete.  See RFC 3240 */
      { "epdf",  "application/pdf" },
      { "epi",   "application/postscript" },
      { "eps",   "application/postscript" },
      { "eps2",  "application/postscript" },
      { "eps3",  "application/postscript" },
      { "epsf",  "application/postscript" },
      { "ept",   "application/postscript" },
      { "fax",   "image/g3fax" },
      { "fpx",   "image/vnd.fpx" },
      { "g3",    "image/g3fax" },
      { "gif",   "image/gif" },
      { "gif87", "image/gif" },
      { "jpeg",  "image/jpeg" },
      { "mng",   "video/x-mng" },
      { "mpeg",  "video/mpeg" },
      { "png",   "image/png" },
      { "pdf",   "application/pdf" },
      { "ps",    "application/postscript" },
      { "ps2",   "application/postscript" },
      { "ps3",   "application/postscript" },
      { "svg",   "image/svg+xml" },
      { "tif",   "image/tiff" },
      { "tiff",  "image/tiff" },
      { "wbmp",  "image/vnd.wap.wbmp" },
      { (char *) NULL, (char *) NULL }
    };

  for (entry=MediaTypes; entry->magick != (char *) NULL; entry++)
    if (LocaleCompare(entry->magick,magick) == 0)
      return(AllocateString(entry->media));
  FormatString(media,"image/x-%.1024s",magick);
  LocaleLower(media+8);
  return(AllocateString(media));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M a g i c k I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterMagickInfo() adds attributes for a particular image format to the
%  list of supported formats.  The attributes include the image format name,
%  a method to read and/or write the format, whether the format supports the
%  saving of more than one frame to the same file or blob, whether the format
%  supports native in-memory I/O, and a brief description of the format.
%
%  The format of the RegisterMagickInfo method is:
%
%      MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
%
%  A description of each parameter follows:
%
%    o magick_info: The magick info.
%
*/
MagickExport MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
{
  register MagickInfo
    *p;

  /*
    Delete any existing name.
  */
  assert(magick_info != (MagickInfo *) NULL);
  assert(magick_info->signature == MagickSignature);
  (void) UnregisterMagickInfo(magick_info->name);
  AcquireSemaphoreInfo(&magick_semaphore);
  magick_info->previous=(MagickInfo *) NULL;
  magick_info->next=(MagickInfo *) NULL;
  if (magick_list == (MagickInfo *) NULL)
    {
      /*
        Start magick list.
      */
      magick_list=magick_info;
      LiberateSemaphoreInfo(&magick_semaphore);
      return(magick_info);
    }
  /*
    Tag is added in lexographic order.
  */
  for (p=magick_list; p->next != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,magick_info->name) >= 0)
      break;
  if (LocaleCompare(p->name,magick_info->name) < 0)
    {
      /*
        Add entry after target.
      */
      magick_info->next=p->next;
      p->next=magick_info;
      magick_info->previous=p;
      if (magick_info->next != (MagickInfo *) NULL)
        magick_info->next->previous=magick_info;
      LiberateSemaphoreInfo(&magick_semaphore);
      return(magick_info);
    }
  /*
    Add entry before target.
  */
  magick_info->next=p;
  magick_info->previous=p->previous;
  p->previous=magick_info;
  if (magick_info->previous != (MagickInfo *) NULL)
    magick_info->previous->next=magick_info;
  if (p == magick_list)
    magick_list=magick_info;
  LiberateSemaphoreInfo(&magick_semaphore);
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetMagickInfo allocates a MagickInfo structure and initializes the
%  members to default values.
%
%  The format of the SetMagickInfo method is:
%
%      MagickInfo *SetMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o magick_info: Method SetMagickInfo returns the allocated and initialized
%      MagickInfo structure.
%
%    o name: a character string that represents the image format associated
%      with the MagickInfo structure.
%
%
*/
MagickExport MagickInfo *SetMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  assert(name != (const char *) NULL);
  magick_info=MagickAllocateMemory(MagickInfo *,sizeof(MagickInfo));
  if (magick_info == (MagickInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateMagickInfo");
  (void) memset(magick_info,0,sizeof(MagickInfo));
  magick_info->name=AcquireString(name);
  magick_info->adjoin=True;
  magick_info->blob_support=True;
  magick_info->thread_support=True;
  magick_info->signature=MagickSignature;
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M a g i c k I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMagickInfo removes a name from the magick info list.  It
%  returns False if the name does not exist in the list otherwise True.
%
%  The format of the UnregisterMagickInfo method is:
%
%      unsigned int UnregisterMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterMagickInfo returns False if the name does not
%      exist in the list otherwise True.
%
%    o name: a character string that represents the image format we are
%      looking for.
%
*/
MagickExport unsigned int UnregisterMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

  unsigned int
    status;

  assert(name != (const char *) NULL);
  status=False;
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->name,name) != 0)
      continue;
    if (p->next != (MagickInfo *) NULL)
      p->next->previous=p->previous;
    if (p->previous != (MagickInfo *) NULL)
      p->previous->next=p->next;
    else
      magick_list=p->next;
    magick_info=p;
    MagickFreeMemory(magick_info->name);
    if (magick_info->description != (char *) NULL)
      MagickFreeMemory(magick_info->description);
    if (magick_info->version != (char *) NULL)
      MagickFreeMemory(magick_info->version);
    if (magick_info->note != (char *) NULL)
      MagickFreeMemory(magick_info->note);
    if (magick_info->module != (char *) NULL)
      MagickFreeMemory(magick_info->module);
    MagickFreeMemory(magick_info);
    status=True;
    break;
  }
  LiberateSemaphoreInfo(&magick_semaphore);
  return(status);
}
