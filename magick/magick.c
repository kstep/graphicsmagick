/*
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
%               Methods to Read or List ImageMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "cache.h"
#include "constitute.h"
#include "delegate.h"
#include "log.h"
#include "magic.h"
#include "magick.h"
#include "module.h"
#include "random.h"
#include "registry.h"
#include "resource.h"
#include "render.h"
#include "utility.h"

/*
  Global declarations.
*/
static SemaphoreInfo
  *magick_semaphore = (SemaphoreInfo *) NULL;

static MagickInfo
  *magick_list = (MagickInfo *) NULL;


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
%  DestroyMagick() destroys the ImageMagick environment.
%
%  The format of the DestroyMagick function is:
%
%      DestroyMagick(void)
%
%
*/
MagickExport void DestroyMagick(void)
{
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
  DestroyRandomReservoir();
  DestroySemaphore();
#if defined(WIN32)
  DestroyTracingCriticalSection();
  NTGhostscriptUnLoadDLL();
#endif
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
      LiberateMemory((void **) &magick_info->name);
    if (magick_info->description != (char *) NULL)
      LiberateMemory((void **) &magick_info->description);
    if (magick_info->version != (char *) NULL)
      LiberateMemory((void **) &magick_info->version);
    if (magick_info->note != (char *) NULL)
      LiberateMemory((void **) &magick_info->note);
    if (magick_info->module != (char *) NULL)
      LiberateMemory((void **) &magick_info->module);
    LiberateMemory((void **) &magick_info);
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
%      const MagickInfo *GetMagickInfo(const char *name,Exception *exception)
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
      (void) GetModuleInfo((char *) NULL,exception);
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
%   I n i t i a l i z e M a g i c k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagick() initializes the ImageMagick environment.
%
%  The format of the InitializeMagick function is:
%
%      InitializeMagick(const char *path)
%
%  A description of each parameter follows:
%
%    o path: The execution path of the current ImageMagick client.
%
%
*/
MagickExport void InitializeMagick(const char *path)
{
  char
    execution_path[MaxTextExtent];

  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
  InitializeSemaphore();

  {
    /*
      Compute memory allocation and memory map resource limits based
      on available memory
    */
    long
      limit,
      pagesize,
      pages;

    limit=0;
    pagesize=-1;
    pages=-1;
#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
    pagesize=sysconf(_SC_PAGE_SIZE);
#endif
#if defined(HAVE_GETPAGESIZE)
    if (pagesize <= 0)
      pagesize=getpagesize();
#endif
#if defined(HAVE_SYSCONF) && defined(_SC_PHYS_PAGES)
    pages=sysconf(_SC_PHYS_PAGES);
#endif
    if (pages > 0 && pagesize > 0)
      limit=((pages+512)/1024)*((pagesize+512)/1024);
#if defined(PixelCacheThreshold)
    limit=PixelCacheThreshold;
#endif
    SetMagickResourceLimit(MemoryResource,limit > 0 ? limit : 512);
    SetMagickResourceLimit(MapResource,limit > 0 ? 2*limit : 1024);
  }

  (void) SetLogEventMask(getenv("MAGICK_DEBUG"));
  *execution_path='\0';
#if !defined(UseInstalledImageMagick)
#if defined(POSIX) || defined(WIN32)
  /* passing NULL means that we want the code to try to figure it out via
     askging the operating system
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
#if defined(_DEBUG)
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
#endif
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
  register const MagickInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) GetMagickInfo("*",exception);
/* #if defined(SupportMagickModules) */
/*   { */
/*     char */
/*       *module_file, */
/*       path[MaxTextExtent]; */

/*     size_t */
/*       length; */

/*     void */
/*       *blob; */

/*     module_file=TagToModule("MIFF"); */
/*     blob=GetModuleBlob(module_file,path,&length,exception); */
/*     if (blob != (void *) NULL) */
/*       LiberateMemory((void **) &blob); */
/*     LiberateMemory((void **) &module_file); */
/*     GetPathComponent(path,HeadPath,path); */
/*     (void) fprintf(file,"Path: %.1024s\n\n",path); */
/*   } */
/* #endif */
  (void) fprintf(file,"   Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------------\n");
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    (void) fprintf(file,"%9s%c  %c%c%c",p->name ? p->name : "",
      p->blob_support ? '*' : ' ',p->decoder ? 'r' : '-',
      p->encoder ? 'w' : '-',p->encoder && p->adjoin ? '+' : '-');
    if (p->description != (char *) NULL)
      (void) fprintf(file,"  %.1024s",p->description);
    if (p->version != (char *) NULL)
      (void) fprintf(file," (%.1024s)",p->version);
    (void) fprintf(file,"\n");
    if (p->note != (char *) NULL)
      {
        char
          **text;

        text=StringToList(p->note);
        if (text != (char **) NULL)
          {
            register long
              i;

            for (i=0; text[i] != (char *) NULL; i++)
              {
                (void) fprintf(file,"            %.1024s\n",text[i]);
                LiberateMemory((void **) &text[i]);
              }
            LiberateMemory((void **) &text);
          }
      }
  }
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
  LiberateSemaphoreInfo(&magick_semaphore);
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
%   o  magick:  ImageMagick format specification "magick" tag.
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
  magick_info=(MagickInfo *) AcquireMemory(sizeof(MagickInfo));
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
    LiberateMemory((void **) &magick_info->name);
    if (magick_info->description != (char *) NULL)
      LiberateMemory((void **) &magick_info->description);
    if (magick_info->version != (char *) NULL)
      LiberateMemory((void **) &magick_info->version);
    if (magick_info->note != (char *) NULL)
      LiberateMemory((void **) &magick_info->note);
    if (magick_info->module != (char *) NULL)
      LiberateMemory((void **) &magick_info->module);
    LiberateMemory((void **) &magick_info);
    status=True;
    break;
  }
  LiberateSemaphoreInfo(&magick_semaphore);
  return(status);
}
