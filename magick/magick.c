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

/*
  Include declarations.
*/
#include "studio.h"

/*
  Global declarations.
*/
static MagickInfo
  *magick_list = (MagickInfo *) NULL;

static SemaphoreInfo
  *magick_semaphore = (SemaphoreInfo *) NULL;

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
  DestroyConstitute();
  DestroyMagickRegistry();
  DestroyCache();
  DestroySemaphore();
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
%  GetImageMagick() searches for an image format that matches the
%  specified magick string.  If one is found the name is returned otherwise
%  NULL.
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
%   G e t M a g i c k C o n f i g u r e P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickConfigurePath() searches a number of pre-defined locations
%  for the specified ImageMagick configuration file and returns the path.
%  The search order follows:
%
%             <current directory>/
%             <client path>/
%             $MAGICK_HOME/
%             $HOME/.magick/
%             $MAGICK_FONT_PATH
%             MagickLibPath
%             MagickModulesPath
%             MagickSharePath
%             Windows Resource
%
%  The format of the GetMagickConfigurePath method is:
%
%      char *GetMagickConfigurePath(const char *filename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o path:  Method GetMagickConfigurePath returns the path if the
%      configuration file is found, otherwise NULL is returned.
%
%    o filename: A character string representing the desired configuration
%      file.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport char *GetMagickConfigurePath(const char *filename,
  ExceptionInfo *exception)
{
  char
    *path,
    *search_path;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  path=AllocateString(filename);
  if (IsAccessible(path))
    return(path);
  search_path=AllocateString(path);
  FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
    DirectorySeparator,filename);
  if (IsAccessible(path))
    {
      LiberateMemory((void **) &search_path);
      return(path);
    }
  ConcatenateString(&search_path,"; Client Path:");
  ConcatenateString(&search_path,path);
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
      if (IsAccessible(path))
        {
          LiberateMemory((void **) &search_path);
          return(path);
        }
      ConcatenateString(&search_path,"; MAGICK_HOME:");
      ConcatenateString(&search_path,path);
    }
  if (getenv("HOME") != (char *) NULL)
    {
      FormatString(path,"%.1024s%s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,
        DirectorySeparator,filename);
      if (IsAccessible(path))
        {
          LiberateMemory((void **) &search_path);
          return(path);
        }
      ConcatenateString(&search_path,"; HOME:");
      ConcatenateString(&search_path,path);
    }
  if (getenv("MAGICK_FONT_PATH") != (char *) NULL)
    {
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_FONT_PATH"),
        DirectorySeparator,filename);
      if (IsAccessible(path))
        {
          LiberateMemory((void **) &search_path);
          return(path);
        }
      ConcatenateString(&search_path,"; MAGICK_FONT_PATH:");
      ConcatenateString(&search_path,path);
    }
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  if (IsAccessible(path))
    {
      LiberateMemory((void **) &search_path);
      return(path);
    }
  ConcatenateString(&search_path,"; MagickLibPath:");
  ConcatenateString(&search_path,path);
  FormatString(path,"%.1024s%.1024s",MagickModulesPath,filename);
  if (IsAccessible(path))
    {
      LiberateMemory((void **) &search_path);
      return(path);
    }
  ConcatenateString(&search_path,"; MagickModulesPath:");
  ConcatenateString(&search_path,path);
  FormatString(path,"%.1024s%.1024s",MagickSharePath,filename);
  if (IsAccessible(path))
    {
      LiberateMemory((void **) &search_path);
      return(path);
    }
  ConcatenateString(&search_path,"; MagickSharePath:");
  ConcatenateString(&search_path,path);
#if defined(WIN32)
  {
    void
      *blob;

    FormatString(path,"%.1024s",filename);
    blob=NTResourceToBlob(path);
    if (blob != (unsigned char *) NULL)
      {
        LiberateMemory((void **) &blob);
        LiberateMemory((void **) &search_path);
        return(path);
      }
    ConcatenateString(&search_path,"; Windows Registry:");
    ConcatenateString(&search_path,path);
  }
#endif
  ThrowException(exception,ConfigurationError,
    "Unable to open configuration file",search_path);
  LiberateMemory((void **) &search_path);
  LiberateMemory((void **) &path);
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
    return(magick_list);
  /*
    Find name in list
  */
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      break;
  if (p == (MagickInfo *) NULL)
    {
      LiberateSemaphoreInfo(&magick_semaphore);
      if (*name != '\0')
        {
          ExceptionInfo
            module_exception;

          GetExceptionInfo(&module_exception);
          (void) OpenModule(name,&module_exception);
          DestroyExceptionInfo(&module_exception);
        }
      AcquireSemaphoreInfo(&magick_semaphore);
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
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k V e r s i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickVersion() returns the ImageMagick API version as a string and
%  as a number.
%
%  The format of the GetMagickVersion method is:
%
%      const char *GetMagickVersion(unsigned int *version)
%
%  A description of each parameter follows:
%
%    o version: The ImageMagick version is returned as a number.
%
*/
MagickExport const char *GetMagickVersion(unsigned int *version)
{
  assert(version != (unsigned int *) NULL);
  *version=MagickLibVersion;
  return(MagickVersion);
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
    directory[MaxTextExtent],
    filename[MaxTextExtent];

  InitializeSemaphore();
  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
  (void) getcwd(directory,MaxTextExtent);
  (void) SetClientPath(directory);
  if (path != (const char *) NULL)
    {
      GetPathComponent(path,HeadPath,filename);
      (void) SetClientPath(filename);
      GetPathComponent(path,BasePath,filename);
      (void) SetClientName(filename);
    }
#if defined(WIN32)
  {
    char
      *execution_path;

#if defined(_DEBUG)
    int
      debug;

    debug=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    debug|=_CRTDBG_CHECK_ALWAYS_DF;
    debug|=_CRTDBG_DELAY_FREE_MEM_DF;
    debug|=_CRTDBG_LEAK_CHECK_DF;
    // debug=_CrtSetDbgFlag(debug);
#endif
    execution_path=NTGetExecutionPath();
    GetPathComponent(execution_path,HeadPath,filename);
    (void) SetClientPath(filename);
    GetPathComponent(execution_path,TailPath,filename);
    (void) SetClientName(filename);
    LiberateMemory((void **) &execution_path);
  }
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
#if defined(WIN32)
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
  char
    filename[MaxTextExtent],
    *module_file,
    *path;

  ExceptionInfo
    path_exception;

  register const MagickInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) GetMagickInfo("*",exception);
  module_file=TagToModule("MIFF");
  GetExceptionInfo(&path_exception);
  path=GetMagickConfigurePath(module_file,&path_exception);
  DestroyExceptionInfo(&path_exception);
  if (path != (char *) NULL)
    {
      GetPathComponent(path,HeadPath,filename);
      (void) fprintf(file,"Module path: %.1024s\n\n",filename);
      LiberateMemory((void **) &path);
    }
  LiberateMemory((void **) &module_file);
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
    MagickFatalError(ResourceLimitFatalError,"Unable to allocate image",
      "Memory allocation failed");
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
    LiberateMemory((void **) &p->name);
    if (p->description != (char *) NULL)
      LiberateMemory((void **) &p->description);
    if (p->version != (char *) NULL)
      LiberateMemory((void **) &p->version);
    if (p->module != (char *) NULL)
      LiberateMemory((void **) &p->module);
    LiberateMemory((void **) &p);
    status=True;
    break;
  }
  LiberateSemaphoreInfo(&magick_semaphore);
  return(status);
}
