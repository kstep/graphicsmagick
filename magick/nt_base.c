/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                 N   N  TTTTT                                %
%                                 NN  N    T                                  %
%                                 N N N    T                                  %
%                                 N  NN    T                                  %
%                                 N   N    T                                  %
%                                                                             %
%                                                                             %
%                Windows NT Utility Methods for GraphicsMagick                %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1996                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#if defined(WIN32)
/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasLTDL)
#  include "ltdl.h"
#endif /* defined(HasLTDL) */
#include "magick/nt_base.h"

/*
  Static declarations.
*/
#if !defined(HasLTDL)
static char
  *lt_slsearchpath = (char *) NULL;
#endif
static void
  *gs_dll_handle = (void *)NULL;
static GhostscriptVectors
    gs_vectors;


/*
  External declarations.
*/
#if !defined(WIN32)
extern "C" BOOL WINAPI
  DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved);
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   c l o s e d i r                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method closedir closes the named directory stream and frees the DIR
%  structure.
%
%  The format of the closedir method is:
%
%      void closedir(DIR *entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport void closedir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  FindClose(entry->hSearch);
  LiberateMemory((void **) &entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e b u g S t r i n g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DebugString formats and sends a prtinf like message to the NT
%  debug trace API, a file, or to the Event Log
%
%  The format of the DebugString method is:
%
%      void DebugString(const char *format,...)
%
%  A description of each parameter follows.
%
%   o  format:  A string describing the format to use to write the
%      remaining arguments.
%
*/

#define IM_DEBUG_WIN32  1
#define IM_DEBUG_FILE   2
#define IM_DEBUG_EVENT  4

static CRITICAL_SECTION
  critical_section;

static unsigned int
  critical_section_exists = False;

static int
  tracings_level = 0;

static unsigned int
  tracings_sequence = 0,
  tracings_counter = 0;

static char
  tracings_filepath[MaxTextExtent] =
    "C:\\";

static TimerInfo
  tracings_timer;

FILE
  *trace_file = (FILE *) NULL;

MagickExport void DestroyTracingCriticalSection(void)
{
  if (critical_section_exists)
    DeleteCriticalSection(&critical_section);
}

MagickExport void InitializeTracingCriticalSection(void)
{
  char
    *debug = getenv("MAGICK_DEBUG");

  if (debug)
    tracings_level |= atoi(debug);
  GetTimerInfo(&tracings_timer);
  if (!critical_section_exists)
    InitializeCriticalSection(&critical_section);
  critical_section_exists=True;
}

static void EnterTracingCriticalSection(void)
{
  if (critical_section_exists)
    EnterCriticalSection(&critical_section);
}

static void LeaveTracingCriticalSection(void)
{
  if (critical_section_exists)
    LeaveCriticalSection(&critical_section);
}

MagickExport void DebugLevel(const int level)
{
  if (critical_section_exists)
    {
      EnterTracingCriticalSection();
      if (level > 0)
        tracings_level = level;
      LeaveTracingCriticalSection();
    }
}

MagickExport void DebugFilePath(const char *s)
{
  if (critical_section_exists)
    {
      EnterTracingCriticalSection();
      (void) strncpy(tracings_filepath,s,MaxTextExtent-1);
      LeaveTracingCriticalSection();
    }
}

MagickExport void DebugString(char *format,...)
{
  va_list
    operands;

  char
    string[MaxTextExtent],
    trace_name[MaxTextExtent];

  va_start(operands, format);

  if (tracings_level <= 0)
    return;

  if (critical_section_exists)
    EnterTracingCriticalSection();

  (void) _snprintf(string,MaxTextExtent-1,"%08d - %010.1fu  ",
    (int) GetCurrentThreadId(), GetElapsedTime(&tracings_timer));
  (void) ContinueTimer(&tracings_timer);
  (void) _vsnprintf(&string[24],MaxTextExtent-25,format,operands);

  if (tracings_level & IM_DEBUG_WIN32)
    OutputDebugString(string);
  if (tracings_level & IM_DEBUG_FILE)
    {
      /* we send the data to files, and keep the number of items
         down to less the 0x2000 to that they are managable */
      if (trace_file == (FILE *) NULL)
        {
          tracings_counter=0;
          (void) _snprintf(trace_name, MaxTextExtent-1,
            "%sIM_%08X.log",tracings_filepath,tracings_sequence);
          trace_file=fopen(trace_name,"wS");
        }
      if (trace_file != (FILE *) NULL)
        {
          fputs(string, trace_file);
          fflush(trace_file);
          tracings_counter++;
          if (tracings_counter > 0x2000)
            {
              fclose(trace_file);
              trace_file = (FILE *) NULL;
              tracings_sequence++;
            }
        }
    }
  if (tracings_level & IM_DEBUG_EVENT)
    {
    }

  if (critical_section_exists)
    LeaveTracingCriticalSection();
  va_end(operands);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   DllMain                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DllMain is an entry point to the DLL which is called when processes
%  and threads are initialized and terminated, or upon calls to the
%  Windows LoadLibrary and FreeLibrary functions.
%
%  The function returns TRUE of it succeeds, or FALSE if initialization fails.
%
%  The format of the DllMain method is:
%
%    BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
%
%  A description of each parameter follows:
%
%    o hinstDLL: handle to the DLL module
%   
%    o fdwReason: reason for calling function.
%
%          May have values:
%             DLL_PROCESS_ATTACH - DLL is being loaded into virtual address
%                                  space of current process.
%             DLL_THREAD_ATTACH - Indicates that the current process is
%                                 creating a new thread.  Called under the
%                                 context of the new thread.
%             DLL_THREAD_DETACH - Indicates that the thread is exiting.
%                                 Called under the context of the exiting
%                                 thread.
%             DLL_PROCESS_DETACH - Indicates that the DLL is being unloaded
%                                  from the virtual address space of the
%                                  current process.
%
%    o lpvReserved: Used for passing additional info during DLL_PROCESS_ATTACH
%                   and DLL_PROCESS_DETACH.
%
*/
#if defined(_DLL) && defined( ProvideDllMain )
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
  switch ( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
      InitializeMagick(NULL);
      break;
    case DLL_PROCESS_DETACH:
      DestroyMagick();
      break;
    default:
      {
        /* Do nothing */
      }
    }
  return TRUE;
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x i t                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Exit calls TerminateProcess for Win95.
%
%  The format of the Exit method is:
%
%      int Exit(int status)
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
MagickExport int Exit(int status)
{
  if (IsWindows95())
    TerminateProcess(GetCurrentProcess(),(unsigned int) status);
  exit(status);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   f t r u n c a t e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ftruncate truncates a file to the specified size.  If the file is
%  longer than the specified size, it is shortened to the specified size. If
%  the file is shorter than the specified size, it is extended to the
%  specified size by filling with zeros.
%  This is a POSIX compatability function.
%
%  The format of the ftruncate method is:
%
%      int ftruncate(int filedes, off_t length)
%
%  A description of each parameter follows:
%
%    o status: Zero is returned on successful completion. Otherwise -1
%        is returned and errno is set to indicate the error.
%
%    o filedes: File descriptor from the _open() call.
%
%    o length: Desired file length.
%
*/
MagickExport int ftruncate(int filedes, off_t length)
{
  int
    status;

  MagickOffset
    current_pos;

  status=0;
  current_pos=MagickTell(filedes);

  /*
    Truncate file to size, filling any extension with nulls.
    Notice that this interface is limited to 2GB due to its
    use of a 'long' offset. Ftruncate also has this shortcoming
    if off_t is a 'long'.

    A way to support more than 2GB is to use SetFilePointerEx()
    to set the file position followed by SetEndOfFile() to set
    the file EOF to the current file position. This approach does
    not ensure that bytes in the extended portion are null
  */ 
  status=chsize(filedes,length);

  /*
    It is not documented if _chsize preserves the seek 
    position, so restore the seek position like ftruncate
    does
  */
  if (!status)
    status=MagickSeek(filedes,current_pos,SEEK_SET);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W i n d o w s 9 5                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsWindows95 returns true if the system is Windows 95.
%
%  The format of the IsWindows95 method is:
%
%      int IsWindows95()
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
MagickExport int IsWindows95()
{
  OSVERSIONINFO
    version_info;

  version_info.dwOSVersionInfoSize=sizeof(version_info);
  if (GetVersionEx(&version_info) &&
      (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
    return(1);
  return(0);
}

#if !defined(HasLTDL)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l c l o s e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlclose unloads the module associated with the passed handle.
%   Zero is returned on success.
%
%  The format of the lt_dlclose method is:
%
%      void lt_dlclose(void *handle)
%
%  A description of each parameter follows:
%
%    o handle: Specifies a handle to a previously loaded dynamic module.
%
*/
int lt_dlclose(void *handle)
{
  /* FreeLibrary returns zero for failure */
  return (!(FreeLibrary(handle)));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l e r r o r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlerror returns a pointer to a string describing the last error
%   associated with a lt_dl* function. Note that this function is not thread
%   safe so it should only be used under the protection of a lock.
%
%  The format of the lt_dlerror method is:
%
%      const char * lt_dlerror(void)
%
%  A description of each parameter follows:
%
*/
const char *lt_dlerror(void)
{
  static char
    last_error[MaxTextExtent];

  char
    *error;

  last_error[0]='\0';
  error=NTGetLastError();
  if (error)
    strncpy(last_error,error,MaxTextExtent-1);
  LiberateMemory((void **) &error);
  return (last_error);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l e x i t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   lt_dlexit() exits the dynamic module loading subsystem.
%
%  The format of the lt_dlexit method is:
%
%      int lt_dlexit(void)
%
*/
int lt_dlexit(void)
{
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l i n i t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlinit initializes the dynamic module loading subsystem.
%
%  The format of the lt_dlinit method is:
%
%      int lt_dlinit(void)
%
*/
int lt_dlinit(void)
{
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l o p e n                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlopen loads a dynamic module into memory and returns a handle
%   that can be used to access the various procedures in the module.
%
%  The format of the lt_dlopen method is:
%
%      void *lt_dlopen(const char *filename)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing dynamic module that
%            is to be loaded.
%
*/
void *lt_dlopen(const char *filename)
{
#define MaxPathElements  31

  char
    buffer[MaxTextExtent];

  int
    index;

  register char
    *p,
    *q;

  register int
    i;

  void
    *handle;

  handle=(void *) NULL;
  handle=(void *) LoadLibrary(filename);

  if (handle != (void *) NULL)
    return(handle);

  if (lt_slsearchpath == (char *) NULL)
    return handle;

  p=lt_slsearchpath;
  index=0;
  while (index < MaxPathElements)
  {
    q=strchr(p,DirectoryListSeparator);
    if (q == (char *) NULL)
      {
        (void) strncpy(buffer,p,MaxTextExtent-strlen(buffer)-1);
        (void) strcat(buffer,"\\");
        (void) strncat(buffer,filename,MaxTextExtent-strlen(buffer)-1);
        handle=(void *) LoadLibrary(buffer);
        break;
      }
    i=q-p;
    (void) strncpy(buffer,p,i);
    buffer[i]='\0';
    (void) strcat(buffer,"\\");
    (void) strncat(buffer,filename,MaxTextExtent-strlen(buffer)-1);
    handle=(void *) LoadLibrary(buffer);
    if (handle)
      break;
    p=q+1;
  }
  return(handle);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l s e t s e a r c h p a t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlsetsearchpath sets the current locations that the subsystem
%   should look at to find dynamically loadable modules.
%
%  The format of the lt_dlsetsearchpath method is:
%
%      int lt_dlsetsearchpath(char *path)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing the search path
%            for DLL's that can be dynamically loaded.
%
*/
int lt_dlsetsearchpath(const char *path)
{
  if (lt_slsearchpath)
    {
      (void) LiberateMemory((void **) &lt_slsearchpath);
      lt_slsearchpath=(char *) NULL;
    }
  if (path != (char *) NULL)
    lt_slsearchpath=AllocateString(path);
  return (0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   l t _ d l s y m                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlsym retrieve the procedure address of the procedure specified
%   by the passed character string.
%
%  The format of the lt_dlsym method is:
%
%      void *lt_dlsym(void *handle,const char *name)
%
%  A description of each parameter follows:
%
%    o handle: Specifies a handle to the previously loaded dynamic module.
%
%    o name: Specifies the procedure entry point to be returned.
%
*/
void *lt_dlsym(void *h,const char *s)
{
  LPFNDLLFUNC1
    lpfnDllFunc1;

  lpfnDllFunc1=(LPFNDLLFUNC1) GetProcAddress(h,s);
  if (!lpfnDllFunc1)
    return((void *) NULL);
  return((void *) lpfnDllFunc1);
}
#endif /* !defined(HasLTDL) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  m m a p                                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method mmap emulates the Unix method of the same name.
%
%  The format of the mmap method is:
%
%    MagickExport void *mmap(char *address,size_t length,int protection,
%      int access,int file,off_t offset)
%
%
*/
MagickExport void *mmap(char *address,size_t length,int protection,int access,
  int file,off_t offset)
{
  void
    *map;

  HANDLE
    handle;

  map=(void *) NULL;
  handle=INVALID_HANDLE_VALUE;
  switch (protection)
  {
    case PROT_READ:
    default:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,PAGE_READONLY,0,
        length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_READ,0,offset,length);
      CloseHandle(handle);
      break;
    }
    case PROT_WRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,PAGE_READWRITE,0,
        length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_WRITE,0,offset,length);
      CloseHandle(handle);
      break;
    }
    case PROT_READWRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,PAGE_READWRITE,0,
        length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_ALL_ACCESS,0,offset,length);
      CloseHandle(handle);
      break;
    }
  }
  if (map == (void *) NULL)
    return((void *) MAP_FAILED);
  return((void *) ((char *) map));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  m u n m a p                                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method munmap emulates the Unix method with the same name.
%
%  The format of the munmap method is:
%
%      int munmap(void *map,size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method munmap returns 0 on success; otherwise, it
%      returns -1 and sets errno to indicate the error.
%
%    o map: The address of the binary large object.
%
%    o length: The length of the binary large object.
%
%
*/
MagickExport int munmap(void *map,size_t length)
{
  if (!UnmapViewOfFile(map))
    return(-1);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T E l a p s e d T i m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTElapsedTime returns the elapsed time (in seconds) since the last
%  call to StartTimer().
%
%  The format of the ElapsedTime method is:
%
%      double NTElapsedTime(void)
%
%
*/
MagickExport double NTElapsedTime(void)
{
  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } elapsed_time;

  SYSTEMTIME
    system_time;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time,&elapsed_time.filetime);
  return((double) 1.0e-7*elapsed_time.filetime64);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   N T E r r o r H a n d l e r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTErrorHandler displays an error reason and then terminates
%  the program.
%
%  The format of the NTErrorHandler method is:
%
%      void NTErrorHandler(const ExceptionType error,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o error: Specifies the numeric error category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void NTErrorHandler(const ExceptionType error,const char *reason,
  const char *description)
{
  char
    buffer[3*MaxTextExtent];

  if (reason == (char *) NULL)
    {
      DestroyMagick();
      Exit(0);
    }
  if ((description != (char *) NULL) && errno)
    FormatString(buffer,"%.1024s: %.1024s (%.1024s) [%.1024s].\n",
      SetClientName((char *) NULL),reason,description,strerror(errno));
  else
    if (description != (char *) NULL)
      FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
        SetClientName((char *) NULL),reason,description);
    else
      if (errno)
        FormatString(buffer,"%.1024s: %.1024s [%.1024s].\n",
          SetClientName((char *) NULL),reason,strerror(errno));
      else
        FormatString(buffer,"%.1024s: %.1024s.\n",SetClientName((char *) NULL),
          reason);
  (void) MessageBox(NULL,buffer,"GraphicsMagick Exception",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONEXCLAMATION);
  DestroyMagick();
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G e t E x e c u t i o n P a t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTGetExecutionPath returns the execution path of a program.
%
%  The format of the GetExecutionPath method is:
%
%      unsigned int NTGetExecutionPath(char *path)
%
%
*/
MagickExport unsigned int NTGetExecutionPath(char *path)
{
  GetModuleFileName(0,path,MaxTextExtent);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G e t L a s t E r r o r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGetLastError returns the last error that occurred.
%
%  The format of the NTGetLastError method is:
%
%      char *NTGetLastError(void)
%
%  A description of each parameter follows:
%
*/
char *NTGetLastError(void)
{
  char
    *reason;

  int
    status;

  LPVOID
    buffer;

  status=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR) &buffer,0,NULL);
  if (!status)
    reason=AllocateString("An unknown error occurred");
  else
    {
      reason=AllocateString((const char *)buffer);
      LocalFree(buffer);
    }
  return(reason);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLL obtains the path to the latest Ghostscript DLL.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptDLL method is:
%
%      int NTGhostscriptDLL(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/

/* Portions of the following code fall under the following copyright: */

/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this file ("Software"), to deal in the Software without
  restriction, including without limitation the rights to use, copy,
  modify, merge, publish, distribute, sublicense, and/or sell copies of
  this Software, and to permit persons to whom this file is furnished to
  do so, subject to the following conditions:

  This Software is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.
*/

#define GS_PRODUCT_AFPL "AFPL Ghostscript"
#define GS_PRODUCT_ALADDIN "Aladdin Ghostscript"
#define GS_PRODUCT_GNU "GNU Ghostscript"
#define GS_MINIMUM_VERSION 550

/* Get Ghostscript versions for given product.
 * Store results starting at pver + 1 + offset.
 * Returns total number of versions in pver.
 */
static int NTGhostscriptProductVersions(int *pver, int offset,
    const char *gs_productfamily)
{
  HKEY
    hkey,
    hkeyroot;

  DWORD
    cbData;

  char
    key[256],
    *p;

  int
    n = 0,
    ver;

  sprintf(key, "Software\\%s", gs_productfamily);
  hkeyroot = HKEY_LOCAL_MACHINE;
  if (RegOpenKeyExA(hkeyroot, key, 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
    /* Now enumerate the keys */
    cbData = sizeof(key) / sizeof(char);
    while (RegEnumKeyA(hkey, n, key, cbData) == ERROR_SUCCESS) {
      n++;
      ver = 0;
      p = key;
      while (*p && (*p!='.')) {
        ver = (ver * 10) + (*p - '0')*100;
        p++;
      }
      if (*p == '.')
        p++;
      if (*p) {
        ver += (*p - '0') * 10;
        p++;
      }
      if (*p)
        ver += (*p - '0');
      if (n + offset < pver[0])
        pver[n+offset] = ver;
    }
  }
  return n+offset;
}

/* Query registry to find which versions of Ghostscript are installed.
 * Return version numbers in an integer array.
 * On entry, the first element in the array must be the array size
 * in elements.
 * If all is well, TRUE is returned.
 * On exit, the first element is set to the number of Ghostscript
 * versions installed, and subsequent elements to the version
 * numbers of Ghostscript.
 * e.g. on entry {5, 0, 0, 0, 0}, on exit {3, 550, 600, 596, 0}
 * Returned version numbers may not be sorted.
 *
 * If Ghostscript is not installed at all, return FALSE
 * and set pver[0] to 0.
 * If the array is not large enough, return FALSE
 * and set pver[0] to the number of Ghostscript versions installed.
 */

static int NTGhostscriptEnumerateVersions(int *pver)
{
  int
    n;

  assert(pver != (int *) NULL);

  n = NTGhostscriptProductVersions(pver, 0, GS_PRODUCT_AFPL);
  n = NTGhostscriptProductVersions(pver, n, GS_PRODUCT_ALADDIN);
  n = NTGhostscriptProductVersions(pver, n, GS_PRODUCT_GNU);

  if (n >= pver[0]) {
    pver[0] = n;
    return FALSE;  /* too small */
  }

  if (n == 0) {
    pver[0] = 0;
    return FALSE;  /* not installed */
  }
  pver[0] = n;
  return TRUE;
}

/*
 * Get a named registry value.
 * Key = hkeyroot\\key, named value = name.
 * name, ptr, plen and return values are the same as in gp_getenv();
 */

static int  NTGetRegistryValue(HKEY hkeyroot, const char *key, const char *name,
    char *ptr, int *plen)
{
  HKEY
    hkey;

  DWORD
    cbData,
    keytype;

  BYTE
    b,
    *bptr = (BYTE *)ptr;

  LONG
    rc;

  if (RegOpenKeyExA(hkeyroot, key, 0, KEY_READ, &hkey)
      == ERROR_SUCCESS) {
    keytype = REG_SZ;
    cbData = *plen;
    if (bptr == (BYTE *)NULL)
      bptr = &b;  /* Registry API won't return ERROR_MORE_DATA */
    /* if ptr is NULL */
    rc = RegQueryValueExA(hkey, (char *)name, 0, &keytype, bptr, &cbData);
    RegCloseKey(hkey);
    if (rc == ERROR_SUCCESS) {
      *plen = cbData;
      return 0;  /* found environment variable and copied it */
    } else if (rc == ERROR_MORE_DATA) {
      /* buffer wasn't large enough */
      *plen = cbData;
      return -1;
    }
  }
  return 1;  /* not found */
}

static int NTGhostscriptGetProductString(int gs_revision, const char *name,
    char *ptr, int len, const char *gs_productfamily)
{
  /* If using Win32, look in the registry for a value with
   * the given name.  The registry value will be under the key
   * HKEY_CURRENT_USER\Software\AFPL Ghostscript\N.NN
   * or if that fails under the key
   * HKEY_LOCAL_MACHINE\Software\AFPL Ghostscript\N.NN
   * where "AFPL Ghostscript" is actually gs_productfamily
   * and N.NN is obtained from gs_revision.
   */

  int
    code,
    length;

  char
    dotversion[16],
    key[256];

  DWORD version = GetVersion();

  if (((HIWORD(version) & 0x8000) != 0)
      && ((HIWORD(version) & 0x4000) == 0)) {
    /* Win32s */
    return FALSE;
  }

  sprintf(dotversion, "%d.%02d",
          (int)(gs_revision / 100), (int)(gs_revision % 100));
  sprintf(key, "Software\\%s\\%s", gs_productfamily, dotversion);

  length = len;
  code = NTGetRegistryValue(HKEY_CURRENT_USER, key, name, ptr, &length);
  if ( code == 0 )
    return TRUE;  /* found it */

  length = len;
  code = NTGetRegistryValue(HKEY_LOCAL_MACHINE, key, name, ptr, &length);

  if ( code == 0 )
    return TRUE;  /* found it */

  return FALSE;
}

static int NTGhostscriptGetString(int gs_revision, const char *name, char *ptr, int len)
{
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_AFPL))
    return TRUE;
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_ALADDIN))
    return TRUE;
  if (NTGhostscriptGetProductString(gs_revision, name, ptr, len, GS_PRODUCT_GNU))
    return TRUE;
  return FALSE;
}

static int NTGetLatestGhostscript( void )
{
  int
    count,
    i,
    gsver,
    *ver;

  DWORD version = GetVersion();
  if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)==0) )
    return FALSE;  // win32s

  count = 1;
  NTGhostscriptEnumerateVersions(&count);
  if (count < 1)
    return FALSE;
  ver = (int *)malloc((count+1)*sizeof(int));
  if (ver == (int *)NULL)
    return FALSE;
  ver[0] = count+1;
  if (!NTGhostscriptEnumerateVersions(ver)) {
    free(ver);
    return FALSE;
  }
  gsver = 0;
  for (i=1; i<=ver[0]; i++) {
    if (ver[i] > gsver)
      gsver = ver[i];
  }
  free(ver);
  return gsver;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLL obtains the path to the latest Ghostscript DLL.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptDLL method is:
%
%      int NTGhostscriptDLL( char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to path buffer to update
%
%    o path_length: Allocation size of path buffer.
%%
*/
MagickExport int NTGhostscriptDLL(char *path, int path_length)
{
  int
    gsver;

  char
    buf[256];

  *path='\0';
  gsver = NTGetLatestGhostscript();
  if ((gsver == FALSE) || (gsver < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(gsver, "GS_DLL", buf, sizeof(buf)))
    return FALSE;

  strncpy(path, buf, path_length-1);
  return TRUE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t D L L V e c t o r s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptDLLVectors returns a GhostscriptVectors structure
%   containing function vectors to invoke Ghostscript DLL functions. A null
%   pointer is returned if there is an error with loading the DLL or
%   retrieving the function vectors.
%
%  The format of the NTGhostscriptDLLVectors method is:
%
%      const GhostscriptVectors *NTGhostscriptDLLVectors( void )
%
%%
*/
MagickExport const GhostscriptVectors *NTGhostscriptDLLVectors( void )
{
  if (NTGhostscriptLoadDLL())
    return &gs_vectors;

  return (GhostscriptVectors*) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t E X E                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptEXE obtains the path to the latest Ghostscript
%   executable.  The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptEXE method is:
%
%      int NTGhostscriptEXE(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/
MagickExport int NTGhostscriptEXE(char *path, int path_length)
{
  int
    gsver;

  char
    buf[256],
    *p;

  *path='\0';
  gsver = NTGetLatestGhostscript();
  if ((gsver == FALSE) || (gsver < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(gsver, "GS_DLL", buf, sizeof(buf)))
    return FALSE;

  p = strrchr(buf, '\\');
  if (p) {
    p++;
    *p = 0;
    strncpy(p, "gswin32c.exe", sizeof(buf)-1-strlen(buf));
    strncpy(path, buf, path_length-1);
    return TRUE;
  }

  return FALSE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t F o n t s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptFonts obtains the path to the Ghostscript fonts.
%   The method returns False if a value is not obtained.
%
%  The format of the NTGhostscriptFonts method is:
%
%      int NTGhostscriptFonts(char *path, int path_length)
%
%  A description of each parameter follows:
%
%    o path: Pointer to buffer in which to return result.
%
%    o path_length: Length of buffer
%
*/
MagickExport int NTGhostscriptFonts(char *path, int path_length)
{
  int
    gsver;

  char
    buf[256],
    *p;

  *path='\0';
  gsver = NTGetLatestGhostscript();
  if ((gsver == FALSE) || (gsver < GS_MINIMUM_VERSION))
    return FALSE;

  if (!NTGhostscriptGetString(gsver, "GS_LIB", buf, sizeof(buf)))
    return FALSE;

  p = strrchr(buf, ';');
  if (p) {
    p++;
    strncpy(path,p,path_length-1);
    return TRUE;
  }

  return FALSE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t L o a d D L L                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptLoadDLL attempts to load the Ghostscript DLL
%   and returns True if it succeeds.
%
%  The format of the NTGhostscriptLoadDLL method is:
%
%      int NTGhostscriptLoadDLL(void)
%
%%
*/
MagickExport int NTGhostscriptLoadDLL(void)
{
  char
    dll_path[MaxTextExtent];

  if (gs_dll_handle != (void *) NULL)
    return True;

  if(!NTGhostscriptDLL(dll_path,sizeof(dll_path)))
    return False;

  gs_dll_handle = lt_dlopen(dll_path);
  if (gs_dll_handle == (void *) NULL)
    return False;

  memset((void*)&gs_vectors, 0, sizeof(GhostscriptVectors));

  gs_vectors.exit=(int (MagickDLLCall *)(gs_main_instance*))lt_dlsym(gs_dll_handle,"gsapi_exit");
  gs_vectors.init_with_args=(int (MagickDLLCall *)(gs_main_instance *, int, char **))(lt_dlsym(gs_dll_handle,"gsapi_init_with_args"));
  gs_vectors.new_instance=(int (MagickDLLCall *)(gs_main_instance **, void *))(lt_dlsym(gs_dll_handle,"gsapi_new_instance"));
  gs_vectors.run_string=(int (MagickDLLCall *)(gs_main_instance *, const char *, int, int *))(lt_dlsym(gs_dll_handle,"gsapi_run_string"));
  gs_vectors.delete_instance=(void (MagickDLLCall *)(gs_main_instance *))(lt_dlsym(gs_dll_handle,"gsapi_delete_instance"));

  if ((gs_vectors.exit==NULL) ||
      (gs_vectors.init_with_args==NULL) ||
      (gs_vectors.new_instance==NULL) ||
      (gs_vectors.run_string==NULL) ||
      (gs_vectors.delete_instance==NULL))
    return False;

  return True;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T G h o s t s c r i p t U n L o a d D L L                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTGhostscriptUnLoadDLL unloads the Ghostscript DLL if it is loaded.
%
%  The format of the NTGhostscriptUnLoadDLL method is:
%
%      int NTGhostscriptUnLoadDLL(void)
%
%%
*/
MagickExport int NTGhostscriptUnLoadDLL(void)
{
  if (gs_dll_handle != (void *) NULL)
    {
      lt_dlclose(gs_dll_handle);
      gs_dll_handle=(void *) NULL;
      memset((void*)&gs_vectors, 0, sizeof(GhostscriptVectors));
      return True;
    }

  return False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T R e g i s t r y K e y L o o k u p                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTRegistryKeyLookup returns package installation path settings
%  stored in the Windows Registry. Path settings are specific to the
%  installed package version so that multiple package installations may
%  coexist.
%
%  Values are stored in the registry under a base path path similar to
%  "HKEY_LOCAL_MACHINE/SOFTWARE\GraphicsMagick\1.0\Q:8". The provided subkey
%  is appended to this base path to form the full key.
%
%  The format of the NTRegistryKeyLookup method is:
%
%      char *NTRegistryKeyLookup(const char *subkey)
%
%  A description of each parameter follows:
%
%    o return: Returns an allocated string containing the value of the key.
%           This allocation should be freed by the user once it is no longer
%           needed.
%
%    o key: Specifies a string that identifies the registry object.
%           Currently supported sub-keys include: "BinPath", "ConfigurePath",
%           "LibPath", "CoderModulesPath", "FilterModulesPath", "SharePath".
%
*/
MagickExport char *NTRegistryKeyLookup(const char *subkey)
{
  static HKEY
    reg_key = (HKEY) INVALID_HANDLE_VALUE;

  /*
    Look-up base-key for first access only
  */
  if (reg_key == (HKEY) INVALID_HANDLE_VALUE)
    {
      char
        package_key[MaxTextExtent];

      LONG
        res;
      
      FormatString(package_key,"SOFTWARE\\%s\\%s\\Q:%d", MagickPackageName,
                   MagickLibVersionText,QuantumDepth);

      res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, package_key, 0, KEY_READ,
                           &reg_key);

      if (res != ERROR_SUCCESS)
        {
          reg_key = (HKEY) INVALID_HANDLE_VALUE;
          return 0;
        }
    }

  /*
    Look-up sub-key
  */
  {
    char
      *dest;
    
    DWORD
      size,
      type;

    LONG
      res;
    
    size = 32;
    dest = (char *) AcquireMemory(size);
    
    res = RegQueryValueExA (reg_key, subkey, 0, &type, dest, &size);
    if (res == ERROR_MORE_DATA && type == REG_SZ)
      {
        ReacquireMemory((void**) &dest,size);
        res = RegQueryValueExA (reg_key, subkey, 0, &type, dest, &size);
      }
    
    if (type != REG_SZ || res != ERROR_SUCCESS)
      {
        LiberateMemory((void**) &dest);
      }
    
    return dest;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T R e s o u r c e T o B l o b                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTResourceToBlob returns a blob containing the contents of the
%  resource in the current executable specified by the id parameter. This
%  is currently used to retrieve MGK files tha have been embedded into
%  the various command line utilities.
%
%  The format of the telldir method is:
%
%      unsigned char *NTResourceToBlob(const char *id)
%
%  A description of each parameter follows:
%
%    o id: Specifies a string that identifies the resource.
%
%
*/
MagickExport unsigned char *NTResourceToBlob(const char *id)
{
  char
    directory[MaxTextExtent];

  DWORD
    length;

  HGLOBAL
    global;

  HMODULE
    handle;

  HRSRC
    resource;

  unsigned char
    *blob,
    *value;

  assert(id != (const char *) NULL);
  FormatString(directory,"%.1024s%.1024s%.1024s",SetClientPath((char *) NULL),
    DirectorySeparator,SetClientName((char *) NULL));
  if (IsAccessible(directory))
    handle=GetModuleHandle(directory);
  else
    handle=GetModuleHandle(0);
  if (!handle)
    return((char *) NULL);
  resource=FindResource(handle,id,"IMAGEMAGICK");
  if (!resource)
  {
    (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
      "Tried: windows resource \"%.1024s\"",id);
    return((char *) NULL);
  }
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Found: windows resource \"%.1024s\"",id);
  global=LoadResource(handle,resource);
  if (!global)
    return((char *) NULL);
  length=SizeofResource(handle,resource);
  value=(unsigned char *) LockResource(global);
  if (!value)
    {
      FreeResource(global);
      return((char *) NULL);
    }
  blob=(unsigned char *) AcquireMemory(length+1);
  if (blob != (unsigned char *) NULL)
    {
      (void) memcpy(blob,value,length);
      blob[length]='\0';
    }
  UnlockResource(global);
  FreeResource(global);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T S y s t e m C o m m a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTSystemComman executes the specified command and waits until it
%   terminates.  The returned value is the exit status of the command.
%
%  The format of the NTSystemComman method is:
%
%      int NTSystemComman(const char *command)
%
%  A description of each parameter follows:
%
%    o command: This string is the command to execute.
%
%
*/
MagickExport int NTSystemComman(const char *command)
{
  char
    local_command[MaxTextExtent];

  DWORD
    child_status;

  int
    status;

  PROCESS_INFORMATION
    process_info;

  STARTUPINFO
    startup_info;

  unsigned int
    background_process;

  if (command == (char *) NULL)
    return(-1);
  GetStartupInfo(&startup_info);
  startup_info.dwFlags=STARTF_USESHOWWINDOW;
  startup_info.wShowWindow=SW_SHOWMINNOACTIVE;
  (void) strncpy(local_command,command,MaxTextExtent-1);
  background_process=command[strlen(command)-1] == '&';
  if (background_process)
    local_command[strlen(command)-1]='\0';
  if (command[strlen(command)-1] == '|')
     local_command[strlen(command)-1]='\0';
   else
     startup_info.wShowWindow=SW_SHOWDEFAULT;
  status=CreateProcess((LPCTSTR) NULL,local_command,
    (LPSECURITY_ATTRIBUTES) NULL,(LPSECURITY_ATTRIBUTES) NULL,(BOOL) FALSE,
    (DWORD) NORMAL_PRIORITY_CLASS,(LPVOID) NULL,(LPCSTR) NULL,&startup_info,
    &process_info);
  if (status == 0)
    return(-1);
  if (background_process)
    return(status == 0);
  status=WaitForSingleObject(process_info.hProcess,INFINITE);
  if (status != WAIT_OBJECT_0)
    return(status);
  status=GetExitCodeProcess(process_info.hProcess,&child_status);
  if (status == 0)
    return(-1);
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);
  return((int) child_status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T U s e r T i m e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTUserTime returns the total time the process has been scheduled (i
%  seconds) since the last call to StartTimer().
%
%  The format of the UserTime method is:
%
%      double NTUserTime(void)
%
%
*/
MagickExport double NTUserTime(void)
{
  DWORD
    status;

  FILETIME
    create_time,
    exit_time;

  OSVERSIONINFO
    OsVersionInfo;

  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } kernel_time;

  union
  {
    FILETIME
      filetime;

    __int64
      filetime64;
  } user_time;

  OsVersionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  GetVersionEx(&OsVersionInfo);
  if (OsVersionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
    return(NTElapsedTime());
  status=GetProcessTimes(GetCurrentProcess(),&create_time,&exit_time,
    &kernel_time.filetime,&user_time.filetime);
  if (status != TRUE)
    return(0.0);
  return((double) 1.0e-7*(kernel_time.filetime64+user_time.filetime64));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T W a r n i n g H a n d l e r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTWarningHandler displays a warning reason.
%
%  The format of the NTWarningHandler method is:
%
%      void NTWarningHandler(const ExceptionType warning,const char *reason,
%        const char *description)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o reason: Specifies the reason to display before terminating the
%      program.
%
%    o description: Specifies any description to the reason.
%
%
*/
MagickExport void NTWarningHandler(const ExceptionType warning,
  const char *reason,const char *description)
{
  char
    buffer[2*MaxTextExtent];

  if (reason == (char *) NULL)
    return;
  if (description == (char *) NULL)
    FormatString(buffer,"%.1024s: %.1024s.\n",
      SetClientName((char *) NULL),reason);
  else
    FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
      SetClientName((char *) NULL),reason,description);
  (void) MessageBox(NULL,buffer,"GraphicsMagick Warning",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONINFORMATION);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   o p e n d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method opendir opens the directory named by filename and associates
%  a directory stream with it.
%
%  The format of the opendir method is:
%
%      DIR *opendir(char *path)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport DIR *opendir(char *path)
{
  char
    file_specification[MaxTextExtent];

  DIR
    *entry;

  assert(path != (char *) NULL);
  (void) strncpy(file_specification,path,MaxTextExtent-1);
  (void) strcat(file_specification,DirectorySeparator);
  entry=(DIR *) AcquireMemory(sizeof(DIR));
  if (entry != (DIR *) NULL)
    {
      entry->firsttime=TRUE;
      entry->hSearch=FindFirstFile(file_specification,&entry->Win32FindData);
    }
  if (entry->hSearch == INVALID_HANDLE_VALUE)
    {
      (void) strcat(file_specification,"\\*.*");
      entry->hSearch=FindFirstFile(file_specification,&entry->Win32FindData);
      if (entry->hSearch == INVALID_HANDLE_VALUE)
        {
          LiberateMemory((void **) &entry);
          return (DIR *)NULL;
        }
    }
  return(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   r e a d d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method readdir returns a pointer to a structure representing the
%  directory entry at the current position in the directory stream to
%  which entry refers.
%
%  The format of the readdir
%
%      readdir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport struct dirent *readdir(DIR *entry)
{
  int
    status;

  if (entry == (DIR *) NULL)
    return((struct dirent *) NULL);
  if (!entry->firsttime)
    {
      status=FindNextFile(entry->hSearch,&entry->Win32FindData);
      if (status == 0)
        return((struct dirent *) NULL);
    }
  entry->firsttime=FALSE;
  (void) strncpy(entry->file_info.d_name,entry->Win32FindData.cFileName,
    MaxTextExtent-1);
  entry->file_info.d_namlen=strlen(entry->file_info.d_name);
  return(&entry->file_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   s e e k d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method seekdir sets the position of the next readdir() operation
%   on the directory stream.
%
%  The format of the seekdir method is:
%
%      void seekdir(DIR *entry,long position)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%    o position: specifies the position associated with the directory
%      stream.
%
%
%
*/
MagickExport void seekdir(DIR *entry,long position)
{
  assert(entry != (DIR *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   t e l l d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method telldir returns the current location associated  with  the
%   named directory stream.
%
%  The format of the telldir method is:
%
%      long telldir(DIR *entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
MagickExport long telldir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  return(0);
}
#endif
