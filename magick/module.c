/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   OOO   DDDD   U   U  L      EEEEE                   %
%                  MM MM  O   O  D   D  U   U  L      E                       %
%                  M M M  O      D   D  U   U  L      EEE                     %
%                  M   M  O   O  D   D  U   U  L      E                       %
%                  M   M   OOO   DDDD    UUU   LLLLL  EEEEE                   %
%                                                                             %
%                                                                             %
%                       GraphicsMagick Module Methods                         %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2000                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
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
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "log.h"
#include "magic.h"
#include "magick.h"
#include "module.h"
#include "utility.h"
#if defined(HasLTDL)
#  include "ltdl.h"
   typedef lt_dlhandle ModuleHandle;
#else
   typedef void *ModuleHandle;
#endif

/*
  Define declarations.
*/
#define ModuleFilename  "modules.mgk"
#if defined(HasLTDL)
#  define ModuleGlobExpression "*.la"
#else
#  if defined(_DEBUG)
#    define ModuleGlobExpression "IM_MOD_DB_*.dll"
#  else
#    define ModuleGlobExpression "IM_MOD_RL_*.dll"
#  endif
#endif

/*
  Declare module map.
*/
#if defined(SupportMagickModules)
static char
  *ModuleMap = (char *)
    "<?xml version=\"1.0\"?>"
    "<modulemap>"
    "  <module />"
    "</modulemap>";
#endif /* defined(SupportMagickModules) */

/*
  Coder module list
  Maintains a loader handle corresponding to each module tag.
  Used to support coder_list, which is initialized as coders are
  loaded, and used to obtain the handle when the coder is unloaded.
*/
typedef struct _CoderInfo
{
  const char
    *tag;
  
  void
    *handle;
  
  time_t
    load_time;

  unsigned long
    signature;

  struct _CoderInfo
    *previous,
    *next;
} CoderInfo;


/*
  Global declarations.
*/
static SemaphoreInfo
  *module_semaphore = (SemaphoreInfo *) NULL;

static CoderInfo
  *coder_list = (CoderInfo *) NULL;

static ModuleInfo
  *module_list = (ModuleInfo *) NULL;

/*
  Forward declarations.
*/
#if defined(SupportMagickModules)
static void
  *GetModuleBlob(const char *filename,char *path,size_t *length,
    ExceptionInfo *exception),
  TagToCoderModuleName(const char *tag,char *module_name),
  TagToFilterModuleName(const char *tag, char *module_name),
  TagToFunctionName(const char *tag,const char *format,char *function);

static unsigned int
  FindMagickModule(const char *filename,MagickModuleType module_type,
    char *path,ExceptionInfo *exception);

static CoderInfo
  *RegisterModule(CoderInfo *,ExceptionInfo *),
  *SetCoderInfo(const char *);

#if FUNCTION_UNUSED
static const CoderInfo
  *GetCoderInfo(const char *,ExceptionInfo *);
#endif

#endif /* defined(SupportMagickModules) */

static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *),
  UnloadModule(const CoderInfo *,ExceptionInfo *),
  UnregisterModule(const char *,ExceptionInfo *);

#if !defined(SupportMagickModules)
#if !defined(WIN32)
static int lt_dlexit(void)
{
  return(0);
}

static int lt_dlinit(void)
{
  return(0);
}

#if defined(SupportMagickModules)
static void *lt_dlopen(char *filename)
{
  return((void *) NULL);
}
#endif /* defined(SupportMagickModules) */

static void lt_dlclose(void *handle)
{
}

static const char *lt_dlerror(void)
{
  return("");
}

static void *lt_dlsym(void *handle,char *symbol)
{
  return((void *) NULL);
}
#endif
#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M o d u l e I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyModuleInfo() deallocates memory associated with the ModuleInfo list.
%
%  The format of the DestroyModuleInfo method is:
%
%      void DestroyModuleInfo(void)
%
*/

MagickExport void DestroyModuleInfo(void)
{
  CoderInfo
    *coder_info;

  ExceptionInfo
    exception;

  ModuleInfo
    *module_info;

  register CoderInfo
    *p;

  register ModuleInfo
    *q;

  /*
    Free module & coder list.
  */
  GetExceptionInfo(&exception);
  AcquireSemaphoreInfo(&module_semaphore);
  for (p=coder_list; p != (CoderInfo *) NULL; )
  {
    coder_info=p;
    p=p->next;
    (void) UnregisterModule(coder_info->tag,&exception);
  }
  DestroyExceptionInfo(&exception);
  coder_list=(CoderInfo *) NULL;
  for (q=module_list; q != (ModuleInfo *) NULL; )
  {
    module_info=q;
    q=q->next;
    if (module_info->path != (char *) NULL)
      LiberateMemory((void **) &module_info->path);
    if (module_info->magick != (char *) NULL)
      LiberateMemory((void **) &module_info->magick);
    if (module_info->name != (char *) NULL)
      LiberateMemory((void **) &module_info->name);
    LiberateMemory((void **) &module_info);
  }
  module_list=(ModuleInfo *) NULL;
  (void) lt_dlexit();
  DestroySemaphoreInfo(&module_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e M o d u l e P r o c e s s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExecuteModuleProcess executes a dynamic modules.  It is passed the
%  specified parameters and returns True on success otherwise False.
%
%  The format of the ExecuteModuleProcess method is:
%
%      unsigned int ExecuteModuleProcess(const char *tag,Image **image,
%        const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o status: ExecuteModuleProcess() returns True if the dynamic module is
%      loaded and returns successfully, otherwise False.
%
%    o tag: a character string that represents the name of the particular
%      module.
%
%    o image: The image.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
*/
MagickExport unsigned int ExecuteModuleProcess(const char *tag,Image **image,
  const int argc,char **argv)
{
#if defined(SupportMagickModules)
  ModuleHandle
    handle;

  unsigned int
    status;

  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  status=False;
  {
    char
      module_name[MaxTextExtent],
      module_path[MaxTextExtent];

    /* Find the module */
    TagToFilterModuleName(tag,module_name);
    if (!FindMagickModule(module_name,MagickFilterModule,module_path,
      &(*image)->exception))
      return( False);

    /* Open the module */
    handle=lt_dlopen(module_path);
    if (handle == (ModuleHandle) NULL)
      {
        char
          message[MaxTextExtent];
        
        FormatString(message,"\"%.256s: %.256s\"",module_path,lt_dlerror());
        ThrowException(&(*image)->exception,ModuleError,"UnableToLoadModule",
          message);
        return(status);
      }
  }
  {
    char
      method_name[MaxTextExtent];

    unsigned int
      (*method)(Image **,const int,char **);

    /* Locate module method */
    FormatString(method_name,"%.64sImage",tag);
    method=(unsigned int (*)(Image **,const int,char **))
      lt_dlsym(handle,method_name);

    /* Execute module method */
    if (method != (unsigned int (*)(Image **,const int,char **)) NULL)
      status=(*method)(image,argc,argv);
  }
  /* Close the module */
  (void) lt_dlclose(handle);
  return(status);
#else
  return(ExecuteStaticModuleProcess(tag,image,argc,argv));
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o d e r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCoderInfo() returns a pointer to a CoderInfo structure that matches the
%  specified tag.  If tag is NULL, the head of the module list is returned. If
%  no modules are loaded, or the requested module is not found, NULL is
%  returned.
%
%  The format of the GetCoderInfo method is:
%
%      const CoderInfo *GetCoderInfo(const char *tag,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o coder_list: Method GetCoderInfo returns a pointer CoderInfo
%      structure that matches the specified tag.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#if FUNCTION_UNUSED
static const CoderInfo *GetCoderInfo(const char *tag,
  ExceptionInfo *exception)
{
  register CoderInfo
    *p;

  (void) GetMagicInfo((unsigned char *) NULL,0,exception);
  (void) GetModuleInfo(tag,exception);
  if ((tag == (const char *) NULL) || (LocaleCompare(tag,"*") == 0))
    return((const CoderInfo *) coder_list);
  for (p=coder_list; p != (CoderInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,tag) == 0)
      break;
  if (p == (CoderInfo *) NULL)
    ThrowException(exception,ModuleError,"UnrecognizedModule",tag);
  else
    if (p != coder_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (CoderInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (CoderInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(CoderInfo *) NULL;
        p->next=coder_list;
        coder_list->previous=p;
        coder_list=p;
      }
  return((const CoderInfo *) p);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F i n d M a g i c k M o d u l e                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FindMagickModule() finds a module with the specified module type and
%  file name. The buffer pointed to by 'path' is updated with the file path
%  if the file is found. True is returned if the module is found.
%
%  The format of the FindMagickModule method is:
%
%      unsigned int FindMagickModule(const char *filename,
%        MagickModuleType module_type,char *path,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The module file name.
%
%    o module_type: The module type (MagickCoderModule or MagickFilterModule)

%    o path: A pointer to the buffer to place the path to the module file. The
%            buffer must be at least MaxTextExtent characters in size.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#if !defined(UseInstalledImageMagick) && defined(POSIX)
static void ChopPathComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path);
  if (*p == *DirectorySeparator)
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == *DirectorySeparator)
      {
        *p='\0';
        count++;
      }
}
#endif

static unsigned int FindMagickModule(const char *filename,
  MagickModuleType module_type,char *path,ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Searching for module file \"%s\" ...",filename);
#if defined(UseInstalledImageMagick)
#if defined(WIN32)
  {
    char
      *key=NULL,
      *key_value;

    switch (module_type)
      {
      case MagickCoderModule:
      default:
        key="CoderModulesPath";
        break;
      case MagickFilterModule:
        key="FilterModulesPath";
        break;
      }

    /*
      Locate path via registry key.
    */
    key_value=NTRegistryKeyLookup(key);
    if (key_value != (char *) NULL)
      {
        FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
          filename);
        if (!IsAccessible(path))
          ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",
            path);
        return(True);
      }
  }
#endif
#if defined(MagickCoderModulesPath)
  {
    char
      *module_directory=NULL;

    switch (module_type)
      {
      case MagickCoderModule:
      default:
        module_directory=MagickCoderModulesPath;
        break;
      case MagickFilterModule:
        module_directory=MagickFilterModulesPath;
        break;
      }

  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",module_directory,filename);
  if (!IsAccessible(path))
    ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",path);
  return(True);
  }
#endif
#else
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        *format=NULL,
        prefix[MaxTextExtent];
      
      switch (module_type)
        {
        case MagickCoderModule:
        default:
          format="%.1024s/lib/%s/modules-Q%d/coders/%.1024s";
          break;
        case MagickFilterModule:
          format="%.1024s/lib/%s/modules-Q%d/filters/%.1024s";
          break;
        }

      /*
        Search based on executable directory if directory is known.
      */
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,format,prefix,MagickLibSubdir,QuantumDepth,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(True);
    }
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      /*
        Search MAGICK_HOME.
      */
#if defined(POSIX)
      char
        *subdir=NULL;

      switch (module_type)
        {
        case MagickCoderModule:
        default:
          subdir=MagickCoderModulesSubdir;
          break;
        case MagickFilterModule:
          subdir=MagickFilterModulesSubdir;
          break;
        }

      FormatString(path,"%.1024s/lib/%s/%.1024s",getenv("MAGICK_HOME"),
        subdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(True);
    }
  if (getenv("HOME") != (char *) NULL)
    {
      /*
        Search $HOME/.magick.
      */
      FormatString(path,"%.1024s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,filename);
      if (IsAccessible(path))
        return(True);
    }
  /*
    Search current directory.
  */
  if (IsAccessible(path))
    return(True);
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",path);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o d u l e L i s t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetModuleList() returns a list containing the names of modules which may be
%  loaded.
%
%  The format of the GetModuleList function is:
%
%      char **GetModuleList(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o module_list: Method GetModuleList returns a list of available modules.
%      If an error occurs a NULL list is returned.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
#if defined(SupportMagickModules)
static char **GetModuleList(ExceptionInfo *exception)
{
  char
    **modules,
    path[MaxTextExtent];

  DIR
    *directory;

  register long
    i;

  struct dirent
    *entry;

  unsigned long
    max_entries;

  max_entries=255;
  modules=(char **) AcquireMemory((max_entries+1)*sizeof(char *));
  if (modules == (char **) NULL)
    return((char **) NULL);
  *modules=(char *) NULL;
  GetPathComponent(module_list->path,HeadPath,path);
  directory=opendir(path);
  if (directory == (DIR *) NULL)
    return((char **) NULL);
  i=0;
  entry=readdir(directory);
  while (entry != (struct dirent *) NULL)
  {
    if (!GlobExpression(entry->d_name,ModuleGlobExpression))
      {
        entry=readdir(directory);
        continue;
      }
    if (i >= (long) max_entries)
      {
        max_entries<<=1;
        ReacquireMemory((void **) &modules,max_entries*sizeof(char *));
        if (modules == (char **) NULL)
          break;
      }
    /*
      Add new module name to list.
    */
    modules[i]=AllocateString((char *) NULL);
    GetPathComponent(entry->d_name,BasePath,modules[i]);
    LocaleUpper(modules[i]);
    if (LocaleNCompare("IM_MOD_",modules[i],7) == 0)
      {
        (void) strcpy(modules[i],modules[i]+10);
        modules[i][strlen(modules[i])-1]='\0';
      }
    i++;
    modules[i]=(char *) NULL;
    entry=readdir(directory);
  }
  (void) closedir(directory);
  return(modules);
}
#endif /* defined(SupportMagickModules) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t M o d u l e B l o b                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetModuleBlob() returns the specified file from the coder modules
%  directory as a blob.
%
%  The format of the GetModuleBlob method is:
%
%      void *GetModuleBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The module file name.
%
%    o path: return the full path information of the module file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void *GetModuleBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  /*
    Search for the file
  */
  if(FindMagickModule(filename,MagickCoderModule,path,exception))
    return(FileToBlob(path,length,exception));
#if defined(WIN32)
  {
    unsigned char
      *blob;

    /*
      Try to obtain the file from a Windows resource
    */
    DestroyExceptionInfo(exception);
    if ((blob=NTResourceToBlob(filename)) != (unsigned char *)NULL)
      return blob;
    else
      ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",
        filename);
  }
#endif
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o d u l e I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetModuleInfo() returns a pointer to a ModuleInfo structure that matches
%  the specified tag.  If tag is NULL, the head of the module alias list is
%  returned. If no modules magick are loaded, or the requested alias is not
%  found, NULL is returned.
%
%  The format of the GetModuleInfo method is:
%
%      const ModuleInfo *GetModuleInfo(const char *name,
%        ExceptionMagick *exception)
%
%  A description of each parameter follows:
%
%    o module_info: GetModuleInfo() returns a pointer ModuleInfo structure
%      that matches the specified tag.
%
%    o name: a character string that represents the module alias we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const ModuleInfo *GetModuleInfo(const char *name,
  ExceptionInfo *exception)
{
  register ModuleInfo
    *p;

  if (module_list == (const ModuleInfo *) NULL)
    {
      AcquireSemaphoreInfo(&module_semaphore);
      if (module_list == (const ModuleInfo *) NULL)
        {
          /*
            Read modules.
          */
          if (lt_dlinit() != 0)
            MagickFatalError(ModuleFatalError,"UnableToInitializeModuleLoader",
              lt_dlerror());
          RegisterStaticModules();
          (void) ReadConfigureFile(ModuleFilename,0,exception);
        }
      LiberateSemaphoreInfo(&module_semaphore);
    }
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const ModuleInfo *) module_list);
  AcquireSemaphoreInfo(&module_semaphore);
  for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      break;
  if ((p != (ModuleInfo *) NULL) && (p != module_list))
    {
      /*
        Self-adjusting list.
      */
      if (p->previous != (ModuleInfo *) NULL)
        p->previous->next=p->next;
      if (p->next != (ModuleInfo *) NULL)
        p->next->previous=p->previous;
      p->previous=(ModuleInfo *) NULL;
      p->next=module_list;
      module_list->previous=p;
      module_list=p;
    }
  LiberateSemaphoreInfo(&module_semaphore);
  return((const ModuleInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M o d u l e I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModuleInfo lists the module info to a file.
%
%  The format of the ListModuleInfo method is:
%
%      unsigned int ListModuleInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListModuleInfo(FILE *file,ExceptionInfo *exception)
{
  register const ModuleInfo
    *p;

  register long
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetModuleInfo("*",exception);
  AcquireSemaphoreInfo(&module_semaphore);
  for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
  {
    if ((p->previous == (ModuleInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (ModuleInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,"Magick      Module\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->magick);
    for (i=(long) strlen(p->magick); i <= 10; i++)
      (void) fprintf(file," ");
    if (p->name != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->name);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&module_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n M o d u l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpenModule() loads a module, and invokes its registration method.  It
%  returns True on success, and False if there is an error.
%
%  The format of the OpenModule method is:
%
%      unsigned int OpenModule(const char *module,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method OpenModule returns True if the specified module is
%      loaded, otherwise False.
%
%    o module: a character string that indicates the module to load.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int OpenModule(const char *module,
  ExceptionInfo *exception)
{
#if defined(SupportMagickModules)
  {
    char
      message[MaxTextExtent],
      module_file[MaxTextExtent],
      module_name[MaxTextExtent],
      name[MaxTextExtent],
      path[MaxTextExtent];

    CoderInfo
      *coder_info;

    ModuleHandle
      handle;

    register ModuleInfo
      *p;

    void
      (*method)(void);

    /*
      Assign module name from alias.
    */
    assert(module != (const char *) NULL);
    (void) strncpy(module_name,module,MaxTextExtent-1);
    if (module_list != (ModuleInfo *) NULL)
      for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
        if (LocaleCompare(p->magick,module) == 0)
          {
            (void) strncpy(module_name,p->name,MaxTextExtent-1);
            break;
          }
    /*
      Find module file.
    */
    handle=(ModuleHandle) NULL;
    TagToCoderModuleName(module_name,module_file);
    *path='\0';
#if 1
  if (!FindMagickModule(module_file,MagickCoderModule,path,exception))
    return(False);
#else
    if ((module_list != (ModuleInfo *) NULL) &&
        (module_list->path != (char *) NULL))
      GetPathComponent(module_list->path,HeadPath,path);
    (void) strcat(path,DirectorySeparator);
    (void) strncat(path,module_file,MaxTextExtent-strlen(path)-1);
#endif
    /*
      Load module
    */
    handle=lt_dlopen(path);
    if (handle == (ModuleHandle) NULL)
      {
        FormatString(message,"\"%.1024s: %.1024s\"",path,lt_dlerror());
        ThrowException(exception,ModuleError,"UnableToLoadModule",message);
      }
    if (handle == (ModuleHandle) NULL)
      return(False);
    /*
      Add module to coder module list.
    */
    coder_info=SetCoderInfo(module_name);
    if (coder_info == (CoderInfo*) NULL)
      {
        (void) lt_dlclose(handle);
        return(False);
      }
    coder_info->handle=handle;
    (void) time(&coder_info->load_time);
    if (!RegisterModule(coder_info,exception))
      return(False);
    /*
      Locate and execute RegisterFORMATImage function
    */
    TagToFunctionName(module_name,"Register%sImage",name);
    method=(void (*)(void)) lt_dlsym(handle,name);
    if (method == (void (*)(void)) NULL)
      {
        FormatString(message,"\"%.1024s: %.1024s\"",module_name,lt_dlerror());
        ThrowException(exception,ModuleError,"UnableToRegisterImageFormat",
          message);
        return(False);
      }
    method();
  }
#endif
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n M o d u l e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenModules loads all available modules.
%
%  The format of the OpenModules method is:
%
%      unsigned int OpenModules(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method OpenModules returns True if the modules are loaded,
%      otherwise False.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport unsigned int OpenModules(ExceptionInfo *exception)
{
  (void) GetMagickInfo((char *) NULL,exception);
#if defined(SupportMagickModules)
  {
    char
      **modules;

    register char
      **p;

    register long
      i;

    /*
      Load all modules.
    */
    modules=GetModuleList(exception);
    if (modules == (char **) NULL)
      return(False);
    for (p=modules; *p != (char *) NULL; p++)
      (void) OpenModule(*p,exception);
    /*
      Free resources.
    */
    for (i=0; modules[i]; i++)
      LiberateMemory((void **) &modules[i]);
    LiberateMemory((void **) &modules);
  }
#endif
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: ReadConfigureFile() returns True if at least one color is
%      defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigureFile(const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
#if defined(SupportMagickModules)
  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the module configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetModuleBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(ModuleMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,
                  "IncludeElementNestedTooDeeply",path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strcat(filename,DirectorySeparator);
                  (void) strncat(filename,token,MaxTextExtent-
                    strlen(filename)-1);
                  (void) ReadConfigureFile(filename,depth+1,exception);
                }
              if (module_list != (ModuleInfo *) NULL)
                while (module_list->next != (ModuleInfo *) NULL)
                  module_list=module_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<module") == 0)
      {
        ModuleInfo
          *module_info;

        /*
          Allocate memory for the module list.
        */
        module_info=(ModuleInfo *) AcquireMemory(sizeof(ModuleInfo));
        if (module_info == (ModuleInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateModuleInfo");
        (void) memset(module_info,0,sizeof(ModuleInfo));
        module_info->path=AcquireString(path);
        module_info->signature=MagickSignature;
        if (module_list == (ModuleInfo *) NULL)
          {
            module_list=module_info;
            continue;
          }
        module_list->next=module_info;
        module_info->previous=module_list;
        module_list=module_list->next;
        continue;
      }
    if (module_list == (ModuleInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'M':
      case 'm':
      {
        if (LocaleCompare((char *) keyword,"magick") == 0)
          {
            module_list->magick=AcquireString(token);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            module_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            module_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  LiberateMemory((void **) &xml);
  if (module_list == (ModuleInfo *) NULL)
    return(False);
  while (module_list->previous != (ModuleInfo *) NULL)
    module_list=module_list->previous;
#endif
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M o d u l e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterModule() adds an entry to the module list.  It returns a pointer to
%  the registered entry on success.
%
%  The format of the RegisterModule method is:
%
%      CoderInfo *RegisterModule(CoderInfo *entry,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o info: a pointer to the registered entry is returned.
%
%    o entry: a pointer to the CoderInfo structure to register.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#if defined(SupportMagickModules)
static CoderInfo *RegisterModule(CoderInfo *entry,ExceptionInfo *exception)
{
  register CoderInfo
    *p;

  /*
    Delete any existing tag.
  */
  assert(entry != (CoderInfo *) NULL);
  assert(entry->signature == MagickSignature);
  AcquireSemaphoreInfo(&module_semaphore);
  entry->previous=(CoderInfo *) NULL;
  entry->next=(CoderInfo *) NULL;
  if (coder_list == (CoderInfo *) NULL)
    {
      /*
        Start module list.
      */
      coder_list=entry;
      LiberateSemaphoreInfo(&module_semaphore);
      return(entry);
    }
  /*
    Tag is added in lexographic order.
  */
  for (p=coder_list; p->next != (CoderInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,entry->tag) >= 0)
      break;
  if (LocaleCompare(p->tag,entry->tag) == 0)
    {
      /*
        Module already registered.
      */
      LiberateSemaphoreInfo(&module_semaphore);
      return(entry);
    }
  if (LocaleCompare(p->tag,entry->tag) < 0)
    {
      /*
        Add entry after target.
      */
      entry->next=p->next;
      p->next=entry;
      entry->previous=p;
      if (entry->next != (CoderInfo *) NULL)
        entry->next->previous=entry;
      LiberateSemaphoreInfo(&module_semaphore);
      return(entry);
    }
  /*
    Add entry before target.
  */
  entry->next=p;
  entry->previous=p->previous;
  p->previous=entry;
  if (entry->previous != (CoderInfo *) NULL)
    entry->previous->next=entry;
  if (p == coder_list)
    coder_list=entry;
  LiberateSemaphoreInfo(&module_semaphore);
  return(entry);
}
#endif /* defined(SupportMagickModules) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C o d e r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetCoderInfo() allocates a CoderInfo structure and initializes the members
%  to default values.
%
%  The format of the SetCoderInfo method is:
%
%      CoderInfo *SetCoderInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o coder_list: SetCoderInfo() returns the allocated and initialized
%      CoderInfo structure.
%
%    o tag: a character string that represents the image format associated
%      with the CoderInfo structure.
%
%
*/
#if defined(SupportMagickModules)
static CoderInfo *SetCoderInfo(const char *tag)
{
  CoderInfo
    *entry;

  assert(tag != (const char *) NULL);
  entry=(CoderInfo *) AcquireMemory(sizeof(CoderInfo));
  if (entry == (CoderInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToAllocateModuleInfo");
  (void) memset(entry,0,sizeof(CoderInfo));
  entry->tag=AcquireString(tag);
  entry->signature=MagickSignature;
  return(entry);
}
#endif /* defined(SupportMagickModules) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T a g T o C o d e r M o d u l e N a m e                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TagToCoderModuleName() takes a module tag and obtains the filename of the
%  corresponding coder module.
%
%  The format of the TagToCoderModuleName method is:
%
%      char *TagToCoderModuleName(const char *tag)
%
%  A description of each parameter follows:
%
%    o tag: a character string representing the module tag.
%
%    o module_name: A buffer to write the module name.  Should be at least
%      MaxTextExtent long.
%
*/
static void TagToCoderModuleName(const char *tag,char *module_name)
{
  assert(tag != (char *) NULL);
  assert(module_name != (char *) NULL);
#if defined(HasLTDL)
  (void) FormatString(module_name,"%.1024s.la",tag);
  (void) LocaleLower(module_name);
#else
#if defined(WIN32)
  if (LocaleNCompare("IM_MOD_",tag,7) == 0)
    (void) strncpy(module_name,tag,MaxTextExtent-1);
  else
    {
#  if defined(_DEBUG)
      FormatString(module_name,"IM_MOD_DB_%.1024s_.dll",tag);
#  else
      FormatString(module_name,"IM_MOD_RL_%.1024s_.dll",tag);
#  endif /* defined(_DEBUG) */
    }
#endif  /* defined(WIN32) */
#endif /* defined(HasLTDL) */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T a g T o F i l t e r M o d u l e N a m e                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TagToFilterModuleName takes a module tag and returns the filename of the
%  corresponding filter module.
%
%  The format of the TagToFilterModuleName method is:
%
%      void TagToFilterModuleName(const char *tag)
%
%  A description of each parameter follows:
%
%    o tag: a character string representing the module tag.
%
*/
#if defined(SupportMagickModules)
static void TagToFilterModuleName(const char *tag, char *module_name)
{
  assert(tag != (char *) NULL);
  assert(module_name != (char *) NULL);
#if defined(HasLTDL)
  (void) FormatString(module_name,"%.1024s.la",tag);
  (void) LocaleLower(module_name);
#else
  (void) FormatString(module_name,"%.1024s.dll",tag);
#endif
}
#endif /* defined(SupportMagickModules) */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T a g T o F u n c t i o n N a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TagToFunctionName() formats the module tag name to a string using the
%  upper-case tag name as the input string, and a user-provided format.
%  This function is used to prepare the RegisterTAGImage and UnregisterTAGImage
%  function names passed lt_dlsym.
%
%  The format of the TagToFunctionName method is:
%
%      TagToFunctionName(const char *tag, const char *format, char *function)
%
%  A description of each parameter follows:
%
%    o tag: the module tag.
%
%    o format: a sprintf-compatible format string containing %s where the
%              upper-case tag name is to be inserted.
%
%    o function: pointer to a destination buffer for the formatted result.
%
*/
static void TagToFunctionName(const char *tag,const char *format,char *function)
{
  char
    *function_name;

  assert(tag != (const char *) NULL);
  assert(format != (const char *) NULL);
  assert(function != (char *) NULL);
  function_name=AllocateString(tag);
  LocaleUpper(function_name);
  FormatString(function,format,function_name);
  LiberateMemory((void **) &function_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n l o a d M o d u l e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnloadModule() unloads a module, and invokes its de-registration method.
%  Returns True on success, and False if there is an error.
%
%  The format of the UnloadModule method is:
%
%      unsigned int UnloadModule(const CoderInfo *coder_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o coder_info: The coder info.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int UnloadModule(const CoderInfo *coder_info,
  ExceptionInfo *exception)
{
  char
    name[MaxTextExtent];

  void
    (*method)(void);

  /*
    Locate and execute UnregisterFORMATImage function
  */
  assert(coder_info != (const CoderInfo *) NULL);
  TagToFunctionName(coder_info->tag,"Unregister%sImage",name);
  method=(void (*)(void)) lt_dlsym((ModuleHandle) coder_info->handle,name);
  if (method == (void (*)(void)) NULL)
    MagickError(ModuleError,"FailedToFindSymbol",lt_dlerror());
  else
    method();
  (void) lt_dlclose((ModuleHandle) coder_info->handle);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M o d u l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnregisterModule() removes a tag from the module info list.  It returns
%  False if the tag does not exist in the list otherwise True.
%
%  The format of the UnregisterModule method is:
%
%      unsigned int UnregisterModule(const char *tag,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterModule returns False if the tag does not
%      exist in the list otherwise it returns True.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int UnregisterModule(const char *tag,ExceptionInfo *exception)
{
  CoderInfo
    *coder_info;

  register CoderInfo
    *p;

  assert(tag != (const char *) NULL);
  for (p=coder_list; p != (CoderInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->tag,tag) != 0)
      continue;
    (void) UnloadModule((CoderInfo *) p,exception);
    LiberateMemory((void **) &p->tag);
    if (p->previous != (CoderInfo *) NULL)
      p->previous->next=p->next;
    else
      {
        coder_list=p->next;
        if (p->next != (CoderInfo *) NULL)
          p->next->previous=(CoderInfo *) NULL;
      }
    if (p->next != (CoderInfo *) NULL)
      p->next->previous=p->previous;
    coder_info=p;
    LiberateMemory((void **) &coder_info);
    break;
  }
  return(p != (CoderInfo *) NULL);
}
