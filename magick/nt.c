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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#include "defines.h"
#include "nt.h"

/*
  Static declarations.
*/
static char
  *lt_slsearchpath = (char *) NULL;

/*
  External declarations.
*/
#if !defined(_VISUALC_)
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
  FreeMemory((void **) &entry);
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
%   I m a g e F o r m a t C o n f l i c t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageFormatConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the ImageFormatConflict method is:
%
%      int ImageFormatConflict(const char *magick)
%
%  A description of each parameter follows:
%
%    o status: Method ImageFormatConflict returns true if the image format
%      conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
MagickExport int ImageFormatConflict(const char *magick)
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
      (void) FreeMemory((void **) &lt_slsearchpath);
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
%   l t _ d l e r r o r                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method lt_dlerror returns the last error that occurred in the dynamic
%   module loading subsystem.
%
%  The format of the lt_dlerror method is:
%
%      char *lt_dlerror(void)
%
%  A description of each parameter follows:
%
*/
char *lt_dlerror(void)
{
  return(ntgetlasterror());
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
%   Method lt_dlopen loads a dynamic module into memory and returs a handle
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
        (void) strcpy(buffer,p);
        (void) strcat(buffer,"\\");
        (void) strcat(buffer,filename);
        handle=(void *) LoadLibrary(buffer);
        break;
      }
    i=q-p;
    (void) strncpy(buffer,p,i);
    buffer[i]='\0';
    (void) strcat(buffer,"\\");
    (void) strcat(buffer,filename);
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
%    MagickExport void *mmap(char *address,size_t length,int protection,int access,
%      int file,off_t offset)
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
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,
        PAGE_READONLY,0,length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_READ,0,0,length);
      CloseHandle(handle);
      break;
    }
    case PROT_WRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,
        PAGE_READWRITE,0,length,0);
      if (!handle)
        break;
      map=(void *) MapViewOfFile(handle,FILE_MAP_WRITE,0,0,length);
      CloseHandle(handle);
      break;
    }
    case PROT_READWRITE:
    {
      handle=CreateFileMapping((HANDLE) _get_osfhandle(file),0,
        PAGE_READWRITE,0,length,0);
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
%  Method NTErrorHandler displays an error message and then terminates
%  the program.
%
%  The format of the NTErrorHandler method is:
%
%      void NTErrorHandler(const ExceptionType error,const char *message,
%        const char *qualifier)
%
%  A description of each parameter follows:
%
%    o error: Specifies the numeric error category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
MagickExport void NTErrorHandler(const ExceptionType error,const char *message,
  const char *qualifier)
{
  char
    buffer[3*MaxTextExtent];

  if (message == (char *) NULL)
    Exit(0);
  if ((qualifier != (char *) NULL) && errno)
    FormatString(buffer,"%.1024s: %.1024s (%.1024s) [%.1024s].\n",
      SetClientName((char *) NULL),message,qualifier,strerror(errno));
  else
    if (qualifier != (char *) NULL)
      FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
        SetClientName((char *) NULL),message,qualifier);
    else
      if (errno)
        FormatString(buffer,"%.1024s: %.1024s [%.1024s].\n",
          SetClientName((char *) NULL),message,strerror(errno));
      else
        FormatString(buffer,"%.1024s: %.1024s.\n",SetClientName((char *) NULL),
          message);
  (void) MessageBox(NULL,buffer,"ImageMagick Exception",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONEXCLAMATION);
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   n t g e t l a s t e r r o r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTGetLastError converts the last error generated by a Win32 API call
%  into a human readable string. It is similar to the UNIX perror function.
%
%  The format of the NTGetLastError method is:
%
%      char *NTGetLastError()
%
%
*/
MagickExport char *ntgetlasterror(void)
{
  char
    *message;

  int
    status;

  LPVOID
    buffer;

  status=FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),
    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR) &buffer,0,NULL);
  if (!status)
    message=AllocateString("An unknown error occurred");
  else
    {
      message=AllocateString(buffer);
      LocalFree(buffer);
    }
  return(message);
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
    local_command[2048];

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
  (void) strcpy(local_command,command);
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
%   N T T e m p o r a r y F i l e n a m e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTTemporaryFilename creates a name for a temporary file.  It
%  returns zero if an error occurs.
%
%  The format of the TemporaryFilename method is:
%
%      int NTTemporaryFilename(char *filename)
%
%  A description of each parameter follows:
%
%    o filename: Specifies a pointer to a string to place the filename.
%
%
*/
MagickExport int NTTemporaryFilename(char *filename)
{
  char
    path[1024];

  int
    status;

  assert(filename != (char *) NULL);
  status=GetTempPath(sizeof(path),path);
  if (status != 0)
    status=GetTempFileName(path,"magick",0,filename);
  (void) remove(filename);
  if (strlen(filename) > 4)
    if (LocaleCompare(filename+strlen(filename)-4,".tmp") == 0)
      filename[strlen(filename)-4]='\0';
  return(status);
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
%  Method NTWarningHandler displays a warning message.
%
%  The format of the NTWarningHandler method is:
%
%      void NTWarningHandler(const ExceptionType warning,const char *message,
%        const char *qualifier)
%
%  A description of each parameter follows:
%
%    o warning: Specifies the numeric warning category.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
MagickExport void NTWarningHandler(const ExceptionType warning,
  const char *message,const char *qualifier)
{
  char
    buffer[2048];

  if (message == (char *) NULL)
    return;
  if (qualifier == (char *) NULL)
    FormatString(buffer,"%.1024s: %.1024s.\n",
      SetClientName((char *) NULL),message);
  else
    FormatString(buffer,"%.1024s: %.1024s (%.1024s).\n",
      SetClientName((char *) NULL),message,qualifier);
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
    file_specification[2048];

  DIR
    *entry;

  assert(path != (char *) NULL);
  (void) strcpy(file_specification,path);
  entry=(DIR *) AllocateMemory(sizeof(DIR));
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
          FreeMemory((void **) &entry);
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
  (void) strcpy(file_info.d_name,entry->Win32FindData.cFileName);
  file_info.d_namlen=strlen(file_info.d_name);
  return(&file_info);
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D l l M a i n                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Dllmain is the entry point for the ImageMagick DLL library.
%
%
*/
BOOL WINAPI DllMain(HINSTANCE instance,DWORD data_segment,LPVOID reserved)
{
  switch(data_segment)
  {
    case DLL_PROCESS_ATTACH:
    {
      return(1);
    }
    case DLL_PROCESS_DETACH:
    {
      break;
    }
    default:
    {
      return(1);
    }
  }
  return(0);
}
#endif
