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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "studio.h"
#include "nt.h"

/*
  Static declarations.
*/
#if !defined(HasLTDL)
static char
  *lt_slsearchpath = (char *) NULL;
#endif

/*
  External declarations.
*/
#if !defined(WIN32)
extern "C" BOOL WINAPI
  DllMain(HINSTANCE instance,DWORD data_segment,LPVOID reserved);
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
%   D e b u g T r a c e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DebugTrace formats and sends a prtinf like message to the NT
%  debug trace API
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

/*
 * Uncomment this to enable the tracing code. This can
 * be used to help locate problems in production code
 * including multithreaded programs. It used a critical
 * section to protect the logging.
 */

/* #define ENABLE_TRACING 1 */

#ifdef ENABLE_TRACING
static CRITICAL_SECTION
  critical_section;

static unsigned int
  critical_section_exists = False;

static unsigned int
  tracings_sequence = 0,
  tracings_counter = 0;

FILE
  *trace_file = (FILE *) NULL;
#endif /* ENABLE_TRACING */

MagickExport void DestroyTracingCriticalSection(void)
{
#ifdef ENABLE_TRACING
  if (critical_section_exists)
    DeleteCriticalSection(&critical_section);
#endif
}

MagickExport void InitializeTracingCriticalSection(void)
{
#ifdef ENABLE_TRACING
  if (!critical_section_exists)
    InitializeCriticalSection(&critical_section);
  critical_section_exists=True;
#endif
}

MagickExport void DebugString(char *format,...)
{
#ifdef ENABLE_TRACING
  va_list
    operands;

  char
    string[MaxTextExtent],
    trace_name[MaxTextExtent];

  va_start(operands, format);
  EnterTracingCriticalSection();
  (void) _snprintf(string,MaxTextExtent-1,"%08d  ",(int) GetCurrentThreadId());
  (void) _vsnprintf(&string[9],MaxTextExtent-10,format,operands);
  /* OutputDebugString(string); */
  if (trace_file == (FILE *) NULL)
    {
      tracings_counter=0;
      (void) _snprintf(trace_name, MaxTextExtent-1,
        "C:\\IM_%08x.log",tracings_sequence);
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
  LeaveTracingCriticalSection();
  va_end(operands);
#endif
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
#if defined(HasMODULES)
  FreeLibrary(handle);
#endif
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
#if defined(HasMODULES)
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
#else
  return((void *) NULL);
#endif
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
#if defined(HasMODULES)
  LPFNDLLFUNC1
    lpfnDllFunc1;

  lpfnDllFunc1=(LPFNDLLFUNC1) GetProcAddress(h,s);
  if (!lpfnDllFunc1)
    return((void *) NULL);
  return((void *) lpfnDllFunc1);
#else
  return((void*) NULL);
#endif
}
#endif

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
+   N T G e t T y pe L i s t                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTGetTypeList returns a TypeInfo list corresponding to installed
%  Windows TrueType fonts. The user is responsible for destroying the
%  returned list once it is no longer needed. NULL is returned if a fatal
%  error is encountered.
%
%  The format of the NTGetTypeList method is:
%
%      TypeInfo* NTGetTypeList( void )
%
%  A description of each parameter follows:
%
%    o returns: A linked list of TypeInfo structures is returned.
%
*/
static int TypeInfoCompare(const void *x,const void *y)
{
  TypeInfo
    **info_1,
    **info_2;

  info_1=(TypeInfo **) x;
  info_2=(TypeInfo **) y;
  return( strcmp((*info_1)->name, (*info_2)->name) );
}
MagickExport TypeInfo* NTGetTypeList( void )
{
  TypeInfo
    *type_list;

  HKEY
    reg_key = (HKEY) INVALID_HANDLE_VALUE;

  LONG
    res;


  int
    list_entries = 0;

  char
    buffer[MaxTextExtent],
    font_root[MaxTextExtent];

  type_list = (TypeInfo*) NULL;

  res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                       0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &reg_key);

  if (res != ERROR_SUCCESS)
    res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
                         0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &reg_key);

  if (res != ERROR_SUCCESS)
    return type_list;

  *font_root='\0';
  if ( getenv("SystemRoot") != (char *) NULL)
    {
      strncpy(buffer,getenv("SystemRoot"),sizeof(buffer)-1);
      strcat(buffer,"\\fonts\\arial.ttf");
      if (IsAccessible(buffer))
        {
          strncpy(font_root,getenv("SystemRoot"),sizeof(buffer)-1);
          strcat(font_root,"\\fonts\\");
        }
      else
        {
          strncpy(font_root,getenv("SystemRoot"),sizeof(buffer)-1);
          strcat(font_root,"\\");
        }
    }

  {
    TypeInfo
      *type_info;

    DWORD
      registry_index = 0,
      type,
      value_data_size,
      value_name_length;

    char
      value_data[MaxTextExtent],
      value_name[MaxTextExtent];

    res = ERROR_SUCCESS;

    while ( (res != ERROR_NO_MORE_ITEMS) && (res == ERROR_SUCCESS) )
      {
        char
          *family_extent,
          token[MaxTextExtent],
          *pos,
          *q;

        value_name_length = sizeof(value_name) - 1;
        value_data_size = sizeof(value_data) - 1;
        res = RegEnumValueA ( reg_key, registry_index, value_name, &value_name_length,
                              0, &type, (BYTE*)value_data, &value_data_size);
        registry_index++;
        if ( (pos = strstr(value_name, " (TrueType)")) == (char*) NULL )
          continue;
        *pos='\0'; /* Remove (TrueType) from string */

        type_info=(TypeInfo *) AcquireMemory(sizeof(TypeInfo));
        if (type_info == (TypeInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"Unable to allocate font list",
                           "Memory allocation failed");
        (void) memset(type_info,0,sizeof(TypeInfo));

        type_info->filename=AcquireString("Windows Fonts");
        type_info->signature=MagickSignature;

        /* Name */
        strcpy(buffer,value_name);
        for(pos = buffer; *pos != 0 ; pos++)
          if (*pos == ' ')
            *pos = '-';
        type_info->name=AcquireString(buffer);

        /* Fullname */
        type_info->description=AcquireString(value_name);

        /* Format */
        type_info->format=AcquireString("truetype");

        /* Glyphs */
        if (strchr(value_data,'\\') == (char *) NULL)
          {
            strcpy(buffer,font_root);
            strcat(buffer,value_data);
          }
        else
          strcpy(buffer,value_data);

        LocaleLower(buffer);
        type_info->glyphs=AcquireString(buffer);

        type_info->stretch=NormalStretch;
        type_info->style=NormalStyle;
        type_info->weight=400;

        /* Some fonts are known to require special encodings */
        if ( (LocaleCompare(type_info->name, "Symbol") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings-2") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings-3") == 0 ) )
          type_info->encoding=AcquireString("AppleRoman");

        family_extent=value_name;

        for (q=value_name; *q != '\0'; )
          {
            GetToken(q,&q,token);
            if (*token == '\0')
              break;

            if (LocaleCompare(token,"Italic") == 0)
              {
                type_info->style=ItalicStyle;
              }

            else if (LocaleCompare(token,"Oblique") == 0)
              {
                type_info->style=ObliqueStyle;
              }

            else if (LocaleCompare(token,"Bold") == 0)
              {
                type_info->weight=700;
              }

            else if (LocaleCompare(token,"Thin") == 0)
              {
                type_info->weight=100;
              }
          
            else if ( (LocaleCompare(token,"ExtraLight") == 0) ||
                      (LocaleCompare(token,"UltraLight") == 0) )
              {
                type_info->weight=200;
              }

            else if (LocaleCompare(token,"Light") == 0)
              {
                type_info->weight=300;
              }

            else if ( (LocaleCompare(token,"Normal") == 0) ||
                      (LocaleCompare(token,"Regular") == 0) )
              {
                type_info->weight=400;
              }

            else if (LocaleCompare(token,"Medium") == 0)
              {
                type_info->weight=500;
              }

            else if ( (LocaleCompare(token,"SemiBold") == 0) ||
                      (LocaleCompare(token,"DemiBold") == 0) )
              {
                type_info->weight=600;
              }

            else if ( (LocaleCompare(token,"ExtraBold") == 0) ||
                      (LocaleCompare(token,"UltraBold") == 0) )
              {
                type_info->weight=800;
              }

            else if ( (LocaleCompare(token,"Heavy") == 0) ||
                      (LocaleCompare(token,"Black") == 0) )
              {
                type_info->weight=900;
              }

            else if (LocaleCompare(token,"Condensed") == 0)
              {
                type_info->stretch = CondensedStretch;
              }

            else if (LocaleCompare(token,"Expanded") == 0)
              {
                type_info->stretch = ExpandedStretch;
              }

            else if (LocaleCompare(token,"ExtraCondensed") == 0)
              {
                type_info->stretch = ExtraCondensedStretch;
              }

            else if (LocaleCompare(token,"ExtraExpanded") == 0)
              {
                type_info->stretch = ExtraExpandedStretch;
              }

            else if (LocaleCompare(token,"SemiCondensed") == 0)
              {
                type_info->stretch = SemiCondensedStretch;
              }

            else if (LocaleCompare(token,"SemiExpanded") == 0)
              {
                type_info->stretch = SemiExpandedStretch;
              }

            else if (LocaleCompare(token,"UltraCondensed") == 0)
              {
                type_info->stretch = UltraCondensedStretch;
              }

            else if (LocaleCompare(token,"UltraExpanded") == 0)
              {
                type_info->stretch = UltraExpandedStretch;
              }

            else
              {
                family_extent=q;
              }
          }

        strncpy(buffer,value_name,family_extent-value_name);
        buffer[family_extent-value_name]='\0';
        type_info->family=AcquireString(buffer);

        list_entries++;
        if (type_list == (TypeInfo *) NULL)
          {
            type_list=type_info;
            continue;
          }
        type_list->next=type_info;
        type_info->previous=type_list;
        type_list=type_list->next;
        continue;
      }
  }

  RegCloseKey ( reg_key );

  /* Sort entries */
  {
    TypeInfo
      **type_array;

    int
      array_index = 0;

    type_array = (TypeInfo**) AcquireMemory(sizeof(TypeInfo*)*list_entries);

    while (type_list->previous != (TypeInfo *) NULL)
      type_list=type_list->previous;

    for (array_index=0; array_index< list_entries; array_index++)
      {
        type_array[array_index] = type_list;
        type_list=type_list->next;
      }

    qsort((void *) type_array, list_entries, sizeof(TypeInfo *),TypeInfoCompare);

    type_list=type_array[0];
    type_list->previous=(TypeInfo *) NULL;
    for(array_index=1; array_index < list_entries; array_index++)
      {
        type_array[array_index-1]->next = type_array[array_index];
        type_array[array_index]->previous = type_array[array_index-1];
        type_array[array_index]->next=(TypeInfo *) NULL;
      }

    LiberateMemory((void**) &type_array);
  }

  return type_list;
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
%   N T R e g i s t r y K e y L o o k u p                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTRegistryKeyLookup returns ImageMagick installation path settings
%  stored in the Windows Registry. Path settings are specific to the
%  installed ImageMagick version so that multiple ImageMagick installations
%  may coexist.
%
%  Values are stored in the registry under a path path similar to
%  "HKEY_LOCAL_MACHINE/SOFTWARE/ImageMagick/5.4.7/LibPath".
%
%  The format of the NTRegistryKeyLookup method is:
%
%      char *NTRegistryKeyLookup(const char *key)
%
%  A description of each parameter follows:
%
%    o key: Specifies a string that identifies the registry object.
%           Currently supported keys include: "ApplicationDefaultsPath",
%           "BinPath", "LibPath", "ModulesPath", and "SharePath".
%
*/
MagickExport char *NTRegistryKeyLookup(const char *key)
{
  static HKEY
    reg_key = (HKEY) INVALID_HANDLE_VALUE;

  char
    *dst;

  DWORD
    size,
    type;

  LONG
    res;

  /* Can probably append MagickLibVersionText to string for efficiency */
  if (reg_key == (HKEY) INVALID_HANDLE_VALUE) {
    res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\ImageMagick", 0, KEY_READ, &reg_key);
    
    if (res == ERROR_SUCCESS)
      res = RegOpenKeyExA (reg_key, MagickLibVersionText, 0, KEY_READ, &reg_key);

    if (res == ERROR_SUCCESS)
      {
#if QuantumDepth == 8
        res = RegOpenKeyExA (reg_key, "Q:8", 0, KEY_READ, &reg_key);
#elif QuantumDepth == 16
        res = RegOpenKeyExA (reg_key, "Q:16", 0, KEY_READ, &reg_key);
#else
# error "Specified value of QuantumDepth is not supported"
#endif
      }

    if (res != ERROR_SUCCESS) {
      reg_key = (HKEY) INVALID_HANDLE_VALUE;
      return 0;
    }
  }

  size = 32;
  dst = (char *) AcquireMemory(size);

  res = RegQueryValueExA (reg_key, key, 0, &type, dst, &size);
  if (res == ERROR_MORE_DATA && type == REG_SZ) {
    ReacquireMemory((void**) &dst,size);
    res = RegQueryValueExA (reg_key, key, 0, &type, dst, &size);
  }

  if (type != REG_SZ || res != ERROR_SUCCESS) {
    LiberateMemory((void**) &dst);
  }

  return dst;
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
