/*
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
%                  Windows NT Utility Methods for ImageMagick                 %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1996                                %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#if defined(WIN32)
/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#include "nt.h"

/*
  Static declarations.
*/
static char
  *lt_slsearchpath = (char *) NULL;

/*
  External declarations.
*/
#if !defined(WIN32)
extern "C" BOOL WINAPI
  DllMain(HINSTANCE instance,DWORD data_segment,LPVOID reserved);
#endif

extern char
  *SetClientName(const char *);

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
MagickExport unsigned int NTIsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
  if (strlen(magick) > 1)
    return(False);
  return((GetLogicalDrives()) & (1 << ((toupper((int) (*magick)))-'A')));
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
%      long lt_dlsetsearchpath(char *path)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing the search path
%            for DLL's that can be dynamically loaded.
%
*/
void lt_dlsetsearchpath(char *path)
{
  if (lt_slsearchpath)
    {
      (void) LiberateMemory((void **) &lt_slsearchpath);
      lt_slsearchpath=(char *) NULL;
    }
  if (path != (char *) NULL)
    lt_slsearchpath=AllocateString(path);
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
%      void *lt_dlopen(char *filename)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to string representing dynamic module that
%            is to be loaded.
%
*/
void *lt_dlopen(char *filename)
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
  if (lt_slsearchpath == (char *) NULL)
    {
      handle=(void *) LoadLibrary(filename);
      return(handle);
    }
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
%   l t _ d l c l o s e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlclose unloads the module associated with the passed handle.
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
void lt_dlclose(void *handle)
{
  FreeLibrary(handle);
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
%      void *lt_dlsym(void *handle,char *name)
%
%  A description of each parameter follows:
%
%    o handle: Specifies a handle to the previously loaded dynamic module.
%
%    o name: Specifies the procedure entry point to be returned.
%
*/
void *lt_dlsym(void *h,char *s)
{
  LPFNDLLFUNC1
    lpfnDllFunc1;

  lpfnDllFunc1=(LPFNDLLFUNC1) GetProcAddress(h,s);
  if (!lpfnDllFunc1)
    return((void *) NULL);
  return((void *) lpfnDllFunc1);
}

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
      map=(void *) MapViewOfFile(handle,FILE_MAP_READ,0,0,length);
      CloseHandle(handle);
      break;
    }
    case PROT_WRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,PAGE_READWRITE,0,
        length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_WRITE,0,0,length);
      CloseHandle(handle);
      break;
    }
    case PROT_READWRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,PAGE_READWRITE,0,
        length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_ALL_ACCESS,0,0,length);
      CloseHandle(handle);
      break;
    }
  }
  if (map == (void *) NULL)
    return((void *) MAP_FAILED);
  return((void *) ((char *) map+offset));
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
  (void) MessageBox(NULL,buffer,"ImageMagick Exception",MB_OK | MB_TASKMODAL |
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
%      char *NTGetExecutionPath(void)
%
%
*/
MagickExport char *NTGetExecutionPath(void)
{
  char
    *path;

  path=AllocateString((char *) NULL);
  GetModuleFileName(0,path,MaxTextExtent);
  return(path);
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
      reason=AllocateString(buffer);
      LocalFree(buffer);
    }
  return(reason);
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
    return((char *) NULL);
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
%   Method NTSystemCommand executes the specified command and waits until it
%   terminates.  The returned value is the exit status of the command.
%
%  The format of the NTSystemCommand method is:
%
%      int NTSystemCommand(const char *command)
%
%  A description of each parameter follows:
%
%    o command: This string is the command to execute.
%
%
*/
MagickExport int NTSystemCommand(const char *command)
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
  (void) MessageBox(NULL,buffer,"ImageMagick Warning",MB_OK | MB_TASKMODAL |
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

  static struct dirent
    file_info;

  if (entry == (DIR *) NULL)
    return((struct dirent *) NULL);
  if (!entry->firsttime)
    {
      status=FindNextFile(entry->hSearch,&entry->Win32FindData);
      if (status == 0)
        return((struct dirent *) NULL);
    }
  entry->firsttime=FALSE;
  (void) strncpy(file_info.d_name,entry->Win32FindData.cFileName,
    MaxTextExtent-1);
  file_info.d_namlen=strlen(file_info.d_name);
  return(&file_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d W M F I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadWMFImage reads an Apple Macintosh QuickDraw/WMF image
%  file using MacOS QuickDraw methods and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  This method was written and contributed by spd@daphne.cps.unizar.es
%  (feel free to copy and use it as you want. No warranty).
%
%  The format of the ReadWMFImage method is:
%
%      Image *ReadWMFImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadWMFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o image_info: The image info..
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

/*
  This method reads either an enhanced metafile, a regular 16bit Windows
  metafile, or an Aldus Placeable metafile and converts it into an enhanced
  metafile.
*/
static HENHMETAFILE ReadEnhMetaFile(const char *szFileName,long *width,
  long *height)
{
#pragma pack( push )
#pragma pack( 2 )
  typedef struct
  {
    DWORD dwKey;
    WORD hmf;
    SMALL_RECT bbox;
    WORD wInch;
    DWORD dwReserved;
    WORD wCheckSum;
  } APMHEADER, *PAPMHEADER;
#pragma pack( pop )

  DWORD
    dwSize;

  ENHMETAHEADER
    emfh;

  HANDLE
    hFile;

  HDC
    hDC;

  HENHMETAFILE
    hTemp;

  LPBYTE
    pBits;

  METAFILEPICT
    mp;

  HMETAFILE
    hOld;

  *width=512;
  *height=512;
  hTemp=GetEnhMetaFile(szFileName);
  if (hTemp != (HENHMETAFILE) NULL)
    {
      /*
        Enhanced metafile.
      */
      GetEnhMetaFileHeader(hTemp,sizeof(ENHMETAHEADER),&emfh);
      *width=emfh.rclFrame.right-emfh.rclFrame.left;
      *height=emfh.rclFrame.bottom-emfh.rclFrame.top;
      return(hTemp);
    }
  hOld=GetMetaFile(szFileName);
  if (hOld != (HMETAFILE) NULL)
    {
      /*
        16bit windows metafile.
      */
      dwSize=GetMetaFileBitsEx(hOld,0,NULL);
      if (dwSize == 0)
        {
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      pBits=(LPBYTE) AcquireMemory(dwSize);
      if (pBits == (LPBYTE) NULL)
        {
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      if (GetMetaFileBitsEx(hOld,dwSize,pBits) == 0)
        {
          LiberateMemory((void **) &pBits);
          DeleteMetaFile(hOld);
          return((HENHMETAFILE) NULL);
        }
      /*
        Make an enhanced metafile from the windows metafile.
      */
      mp.mm=MM_ANISOTROPIC;
      mp.xExt=1000;
      mp.yExt=1000;
      mp.hMF=NULL;
      hDC=GetDC(NULL);
      hTemp=SetWinMetaFileBits(dwSize,pBits,hDC,&mp);
      ReleaseDC(NULL,hDC);
      DeleteMetaFile(hOld);
      LiberateMemory((void **) &pBits);
      GetEnhMetaFileHeader(hTemp,sizeof(ENHMETAHEADER),&emfh);
      *width=emfh.rclFrame.right-emfh.rclFrame.left;
      *height=emfh.rclFrame.bottom-emfh.rclFrame.top;
      return(hTemp);
    }
  /*
    Aldus Placeable metafile.
  */
  hFile=CreateFile(szFileName,GENERIC_READ,0,NULL,OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return(NULL);
  dwSize=GetFileSize(hFile,NULL);
  pBits=(LPBYTE) AcquireMemory(dwSize);
  ReadFile(hFile,pBits,dwSize,&dwSize,NULL);
  CloseHandle(hFile);
  if (((PAPMHEADER) pBits)->dwKey != 0x9ac6cdd7l)
    {
      LiberateMemory((void **) *pBits);
      return((HENHMETAFILE) NULL);
    }
  /*
    Make an enhanced metafile from the placable metafile.
  */
  mp.mm=MM_ANISOTROPIC;
  mp.xExt=((PAPMHEADER) pBits)->bbox.Right-((PAPMHEADER) pBits)->bbox.Left;
  *width=mp.xExt;
  mp.xExt=(mp.xExt*2540l)/(DWORD) (((PAPMHEADER) pBits)->wInch);
  mp.yExt=((PAPMHEADER)pBits)->bbox.Bottom-((PAPMHEADER) pBits)->bbox.Top;
  *height=mp.yExt;
  mp.yExt=(mp.yExt*2540l)/(DWORD) (((PAPMHEADER) pBits)->wInch);
  mp.hMF=NULL;
  hDC=GetDC(NULL);
  hTemp=SetWinMetaFileBits(dwSize,&(pBits[sizeof(APMHEADER)]),hDC,&mp);
  ReleaseDC(NULL,hDC);
  LiberateMemory((void **) &pBits);
  return(hTemp);
}

#define CENTIMETERS_INCH 2.54

MagickExport Image *ReadWMFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  BITMAPINFO
    DIBinfo;

  HBITMAP
    hBitmap,
    hOldBitmap;

  HDC
    hDC;

  HENHMETAFILE
    hemf;

  Image
    *image;

  long
    height,
    width,
    y;

  RECT
    rect;

  register long
    x;

  register PixelPacket
    *q;

  RGBQUAD
    *pBits,
    *ppBits;

  image=AllocateImage(image_info);
  /* NOTE: height and width are returned in .01mm units */
  hemf=ReadEnhMetaFile(image_info->filename,&width,&height);
  if (!hemf)
    ThrowReaderException(FatalException,"file is not a metafile",image);
  if ((image->columns == 0) || (image->rows == 0))
    {
      double
        y_resolution = 72.0,
        x_resolution = 72.0;

      if(image->y_resolution > 0)
        {
          y_resolution = image->y_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            y_resolution *= CENTIMETERS_INCH;
        }

      if(image->x_resolution > 0)
        {
          x_resolution = image->x_resolution;
          if (image->units == PixelsPerCentimeterResolution)
            x_resolution *= CENTIMETERS_INCH;
        }

      image->rows=
        ceil((height*CENTIMETERS_INCH)/1000*y_resolution);
      image->columns=
        ceil((width*CENTIMETERS_INCH)/1000*x_resolution);
    }
  if (image_info->size != (char *) NULL)
    {
      int
        x,
        y;

      unsigned int
        flags;

      x=y=0;
      image->rows=height;
      image->columns=width;
      flags=ParseImageGeometry(image_info->size,&x,&y,
        &image->columns,&image->rows);
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      long
        sans;

      register char
        *p;

      unsigned int
        flags;

      geometry=PostscriptGeometry(image_info->page);
      p=strchr(geometry,'>');
      if (!p)
        {
          flags=ParseImageGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              ((image->columns*image->x_resolution)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              ((image->rows*image->y_resolution)+0.5);
        }
      else
        {
          *p='\0';
          flags=ParseImageGeometry(geometry,&sans,&sans,&image->columns,
            &image->rows);
          if (image->x_resolution != 0.0)
            image->columns=(unsigned int)
              (((image->columns*image->x_resolution)/72.0)+0.5);
          if (image->y_resolution != 0.0)
            image->rows=(unsigned int)
              (((image->rows*image->y_resolution)/72.0)+0.5);
        }
      LiberateMemory((void **) &geometry);
    }
  hDC=GetDC(NULL);
  if (!hDC)
    ThrowReaderException(FatalException,"failed to create a DC",image);
  /*
    Initialize the bitmap header info.
  */
  memset(&DIBinfo,0,sizeof(BITMAPINFO));
  DIBinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  DIBinfo.bmiHeader.biWidth=image->columns;
  DIBinfo.bmiHeader.biHeight=(-1)*image->rows;
  DIBinfo.bmiHeader.biPlanes=1;
  DIBinfo.bmiHeader.biBitCount=32;
  DIBinfo.bmiHeader.biCompression=BI_RGB;
  hBitmap=
    CreateDIBSection(hDC,&DIBinfo,DIB_RGB_COLORS,(void **) &ppBits,NULL,0);
  ReleaseDC(NULL,hDC);
  if (!hBitmap)
    ThrowReaderException(FatalException,"failed to create bitmap",image);
  hDC=CreateCompatibleDC(NULL);
  if (!hDC)
    {
      DeleteObject(hBitmap);
      ThrowReaderException(FatalException,"failed to create a memory DC",image);
    }
  hOldBitmap=(HBITMAP) SelectObject(hDC,hBitmap);
  if (!hOldBitmap)
    {
      DeleteDC(hDC);
      DeleteObject(hBitmap);
      ThrowReaderException(FatalException,"failed to create bitmap",image);
    }
  /*
    Initialize the bitmap to the image background color.
  */
  pBits=ppBits;
  for (y=0; y < (long) image->rows; y++)
  {
    for (x=0; x < (long) image->columns; x++)
    {
      pBits->rgbRed=DownScale(image->background_color.red);
      pBits->rgbGreen=DownScale(image->background_color.green);
      pBits->rgbBlue=DownScale(image->background_color.blue);
      pBits++;
    }
  }
  rect.top=0;
  rect.left=0;
  rect.right=image->columns;
  rect.bottom=image->rows;
  /*
    Convert metafile pixels.
  */
  PlayEnhMetaFile(hDC,hemf,&rect);
  pBits=ppBits;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=UpScale(pBits->rgbRed);
      q->green=UpScale(pBits->rgbGreen);
      q->blue=UpScale(pBits->rgbBlue);
      q->opacity=OpaqueOpacity;
      pBits++;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if (hemf)
    DeleteEnhMetaFile(hemf);
  SelectObject(hDC,hOldBitmap);
  DeleteDC(hDC);
  DeleteObject(hBitmap);
  return(image);
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
