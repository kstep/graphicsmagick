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
%                        ImageMagick Module Methods                           %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2000                                   %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasLTDL)
#include "ltdl/ltdl.h"
typedef lt_dlhandle ModuleHandle;
#else
typedef void *ModuleHandle;
#endif

/*
  Define declarations.
*/
#define ModuleFilename  "modules.mgk"
#if !defined(WIN32)
# if !defined(CoderModuleDirectory)
#  define CoderModuleDirectory  ""
# endif
# define ModuleGlobExpression "*.la"
#else
# if defined(_DEBUG)
#  define ModuleGlobExpression "IM_MOD_DB_*.dll"
# else
#  define ModuleGlobExpression "IM_MOD_RL_*.dll"
# endif
#endif

/*
  Declare module map.
*/
static char
  *ModuleMap =
    "<?xml version=\"1.0\"?>"
    "<modulemap>"
    "  <module />"
    "</modulemap>";

/*
  Global declarations.
*/
static ModuleAlias
  *module_aliases = (ModuleAlias *) NULL;

static ModuleInfo
  *module_list = (ModuleInfo *) NULL;

static SemaphoreInfo
  *module_semaphore = (SemaphoreInfo *) NULL;

/*
  Forward declarations.
*/
static char
  *TagToModule(const char *),
  *TagToProcess(const char *);

static int
  UnloadDynamicModule(const char *),
  UnregisterModuleInfo(const char *);

static ModuleInfo
  *RegisterModuleInfo(ModuleInfo *),
  *SetModuleInfo(const char *);

static unsigned int
  ReadConfigurationFile(const char *,ExceptionInfo *);

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
%  Method DestroyModuleInfo deallocates memory associated with the ModuleInfo
%  list.
%
%  The format of the DestroyModuleInfo method is:
%
%      void DestroyModuleInfo(void)
%
*/
MagickExport void DestroyModuleInfo(void)
{
  ModuleInfo
    *module_info;

  register ModuleAlias
    *q;

  register ModuleInfo
    *p;

  /*
    Unload and unregister all loaded modules.
  */
  for (p=module_list; p != (ModuleInfo *) NULL; )
  {
    module_info=p;
    p=p->next;
    UnloadDynamicModule(module_info->tag);
    UnregisterModuleInfo(module_info->tag);
  }
  /*
    Free module list and aliases.
  */
  AcquireSemaphoreInfo(&module_semaphore);
  for (q=module_aliases; q != (ModuleAlias *) NULL; )
  {
    if (q->filename != (char *) NULL)
      LiberateMemory((void **) &q->filename);
    if (q->name != (char *) NULL)
      LiberateMemory((void **) &q->name);
    if (q->alias != (char *) NULL)
      LiberateMemory((void **) &q->alias);
    module_aliases=q;
    q=q->next;
    LiberateMemory((void **) &module_aliases);
  }
  module_aliases=(ModuleAlias *) NULL;
  for (p=module_list; p != (ModuleInfo *) NULL; )
  {
    if (p->tag != (char *) NULL)
      LiberateMemory((void **) &p->tag);
    module_list=p;
    p=p->next;
    LiberateMemory((void **) &module_list);
  }
  module_list=(ModuleInfo *) NULL;
  DestroySemaphoreInfo(module_semaphore);
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
%      unsigned int ExecuteModuleProcess(const char *tag,Image *image,
%        const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o status: Method ExecuteModuleProcess returns True if the dynamic module is
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

#if !defined(HasMODULES)
#if !defined(WIN32)
static int lt_dlinit(void)
{
  return(0);
}
static void *lt_dlopen(char *filename)
{
  return((void *) NULL);
}
static void lt_dlclose(void *handle)
{
}
static const char *lt_dlerror(void)
{
  return((const char *) NULL);
}
static void *lt_dlsym(void *handle,char *symbol)
{
  return((void *) NULL);
}
#endif
#endif

MagickExport unsigned int ExecuteModuleProcess(const char *tag,Image *image,
  const int argc,char **argv)
{
  char
    *module_name;

  ModuleHandle
    handle;

  unsigned int
    (*method)(Image *,const int,char **),
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=False;
  module_name=TagToProcess(tag);
  handle=lt_dlopen(module_name);
  if (handle == (ModuleHandle) NULL)
    {
      char
        message[MaxTextExtent];

      FormatString(message,"failed to load module \"%.1024s\"",module_name);
      MagickWarning(MissingDelegateWarning,message,lt_dlerror());
      LiberateMemory((void **) &module_name);
      return(status);
    }
  (void) strcpy(module_name,tag);
  (void) strcat(module_name,"Image");
  method=(unsigned int (*)(Image *,const int,char **))
    lt_dlsym(handle,module_name);
  if (method != NULL)
    status=(*method)(image,argc,argv);
  lt_dlclose(handle);
  LiberateMemory((void **) &module_name);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o d u l e A l i a s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetModuleAlias returns a pointer to a ModuleAlias structure that
%  matches the specified tag.  If tag is NULL, the head of the module alias
%  list is returned. If no modules aliases are loaded, or the requested alias
%  is not found, NULL is returned.
%
%  The format of the GetModuleAlias method is:
%
%      ModuleAlias *GetModuleAlias(const char *name,ExceptionAlias *exception)
%
%  A description of each parameter follows:
%
%    o module_alias: Method GetModuleAlias returns a pointer ModuleAlias
%      structure that matches the specified tag.
%
%    o name: a character string that represents the module alias we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport ModuleAlias *GetModuleAlias(const char *name,
  ExceptionInfo *exception)
{
  register ModuleAlias
    *p;

  AcquireSemaphoreInfo(&module_semaphore);
  if (module_aliases == (ModuleAlias *) NULL)
    {
      /*
        Initialize ltdl.
      */
      if (lt_dlinit() != 0)
        MagickError(DelegateError,"unable to initialize module loader",
          lt_dlerror());
      OpenStaticModules();
      /*
        Read modules.
      */
      (void) ReadConfigurationFile(ModuleFilename,exception);
    }
  LiberateSemaphoreInfo(&module_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return(module_aliases);
  for (p=module_aliases; p != (ModuleAlias *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      return(p);
  return((ModuleAlias *) NULL);
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
%  Method GetModuleInfo returns a pointer to a ModuleInfo structure that
%  matches the specified tag.  If tag is NULL, the head of the module list
%  is returned. If no modules are loaded, or the requested module is not
%  found, NULL is returned.
%
%  The format of the GetModuleInfo method is:
%
%      ModuleInfo *GetModuleInfo(const char *tag,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o module_info: Method GetModuleInfo returns a pointer ModuleInfo
%      structure that matches the specified tag.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport ModuleInfo *GetModuleInfo(const char *tag,ExceptionInfo *exception)
{
  register ModuleInfo
    *p;

  (void) GetMagicInfo((unsigned char *) NULL,0,exception);
  (void) GetModuleAlias(tag,exception);
  if ((tag == (const char *) NULL) || (LocaleCompare(tag,"*") == 0))
    return(module_list);
  for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,tag) == 0)
      return(p);
  return((ModuleInfo *) NULL);
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
%  Method GetModuleList returns a list containing the names of modules which may
%  be loaded.
%
%  The format of the GetModuleList function is:
%
%      char **GetModuleList()
%
%  A description of each parameter follows:
%
%    o modulelist: Method GetModuleList returns a list of available modules. If
%      an error occurs a NULL list is returned.
%
*/
static char **GetModuleList(void)
{
  char
    filename[MaxTextExtent],
    **modules,
    *path;

  DIR
    *directory;

  register int
    i;

  struct dirent
    *entry;

  unsigned int
    max_entries;

  max_entries=255;
  modules=(char **) AcquireMemory((max_entries+1)*sizeof(char *));
  if (modules == (char **) NULL)
    return((char **) NULL);
  *modules=(char *) NULL;
  path=GetMagickConfigurePath(ModuleFilename);
  if (path == (char *) NULL)
    return((char **) NULL);
  GetPathComponent(path,HeadPath,filename);
  LiberateMemory((void **) &path);
  directory=opendir(filename);
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
    if (i >= (int) max_entries)
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M o d u l e A l i a s e s                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModuleAliases lists the module info to a file.
%
%  The format of the ListModuleAliases method is:
%
%      unsigned int ListModuleAliases(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListModuleAliases(FILE *file,ExceptionInfo *exception)
{
  register ModuleAlias
    *p;

  register int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"ImageMagick understands these module aliases:\n");
  p=GetModuleAlias("*",exception);
  if (p == (ModuleAlias *) NULL)
    return(False);
  if (p->filename != (char *) NULL)
    (void) fprintf(file,"\nFilename: %.1024s\n\n",p->filename);
  (void) fprintf(file,"Name      Alias\n");
  (void) fprintf(file,"-------------------------------------------------------"
    "------------------------\n");
  for ( ; p != (ModuleAlias *) NULL; p=p->next)
  {
    (void) fprintf(file,"%.1024s",p->name);
    for (i=strlen(p->name); i <= 9; i++)
      (void) fprintf(file," ");
    if (p->alias != (char *) NULL)
      (void) fprintf(file,"%.1024s",p->alias);
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d u l e T o T a g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ModuleToTag parser a file system module name into the basic name of
%  the module.
%
%  The format of the ModuleToTag method is:
%
%      ModuleToTag(const char *filename, const char *format, char *module)
%
%  A description of each parameter follows:
%
%    o filename: the filesystem name of the module.
%
%    o format: a string used to format the result of the parsing.
%
%    o module: pointer to a destination buffer for the formatted result.
%
*/
static void ModuleToTag(const char *filename,const char *format,char *module)
{
  char
    *module_name;

  assert(filename != (const char *) NULL);
  assert(format != (const char *) NULL);
  assert(module != (char *) NULL);
  module_name=AllocateString(filename);
  LocaleUpper(module_name);
  FormatString(module,format,module_name);
  LiberateMemory((void **) &module_name);
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
%  Method OpenModule loads a module, and invokes its registration method.
%  It returns True on success, and False if there is an error.
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
  char
    message[MaxTextExtent],
    *module_file,
    module_name[MaxTextExtent],
    name[MaxTextExtent],
    *path;

  ModuleHandle
    handle;

  ModuleInfo
    *module_info;

  register ModuleAlias
    *p;

  void
    (*method)(void);

  /*
    Assign module name from alias.
  */
  assert(module != (const char *) NULL);
  (void) strcpy(module_name,module);
  if (module_aliases != (ModuleAlias *) NULL)
    {
      for (p=module_aliases; p != (ModuleAlias *) NULL; p=p->next)
        if (LocaleCompare(p->alias,module) == 0)
          {
            (void) strcpy(module_name,p->name);
            break;
          }
    }
  /*
    Load module file.
  */
  handle=(ModuleHandle) NULL;
  module_file=TagToModule(module_name);
  path=GetMagickConfigurePath(module_file);
  if (path != (char *) NULL)
    {
      handle=lt_dlopen(path);
      if (handle == (ModuleHandle) NULL)
        {
          FormatString(message,"failed to load module \"%.1024s\"",path);
          ThrowException(exception,MissingDelegateWarning,message,lt_dlerror());
        }
      LiberateMemory((void **) &path);
    }
  LiberateMemory((void **) &module_file);
  if (handle == (ModuleHandle) NULL)
    return(False);
  /*
    Add module to module list.
  */
  module_info=SetModuleInfo(module_name);
  if (module_info == (ModuleInfo*) NULL)
    {
      lt_dlclose(handle);
      return(False);
    }
  module_info->handle=handle;
  time(&module_info->load_time);
  if (!RegisterModuleInfo(module_info))
    return(False);
  /*
    Locate and execute RegisterFORMATImage function
  */
  ModuleToTag(module_name,"Register%sImage",name);
  method=(void (*)(void)) lt_dlsym(handle,name);
  if (method == (void (*)(void)) NULL)
    {
      ThrowException(exception,MissingDelegateWarning,"failed to find symbol",
        lt_dlerror());
      return(False);
    }
  method();
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
  char
    **modules;

  register char
    **p;

  register int
    i;

  /*
    Load all modules.
  */
  (void) GetMagickInfo((char *) NULL,exception);
  modules=GetModuleList();
  if (modules == (char **) NULL)
    return(False);
  for (p=modules; *p != (char *) NULL; p++)
    OpenModule(*p,exception);
  /*
    Free resources.
  */
  for (i=0; modules[i]; i++)
    LiberateMemory((void **) &modules[i]);
  LiberateMemory((void **) &modules);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r a t i o n F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigurationFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigurationFile method is:
%
%      unsigned int ReadConfigurationFile(const char *basename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigurationFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigurationFile(const char *basename,
  ExceptionInfo *exception)
{
#if defined(HasMODULES)
  char
    filename[MaxTextExtent],
    keyword[MaxTextExtent],
    *path,
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the module configuration file.
  */
  FormatString(filename,"%.1024s",basename);
  path=GetMagickConfigurePath(filename);
  if (path != (char *) NULL)
    {
      FormatString(filename,"%.1024s",path);
      LiberateMemory((void **) &path);
    }
  xml=(char *) FileToBlob(filename,&length,exception);
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
    FormatString(keyword,"%.1024s",token);
    if (LocaleCompare(keyword,"<!") == 0)
      {
        /*
          Comment.
        */
        while ((*token != '>') && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<module") == 0)
      {
        ModuleAlias
          *alias_info;

        /*
          Allocate memory for the font list.
        */
        alias_info=(ModuleAlias *) AcquireMemory(sizeof(ModuleAlias));
        if (alias_info == (ModuleAlias *) NULL)
          MagickError(ResourceLimitError,"Unable to allocate module aliases",
            "Memory allocation failed");
        memset(alias_info,0,sizeof(ModuleAlias));
        if (module_aliases == (ModuleAlias *) NULL)
          {
            alias_info->filename=AllocateString(filename);
            module_aliases=alias_info;
            continue;
          }
        module_aliases->next=alias_info;
        alias_info->previous=module_aliases;
        module_aliases=module_aliases->next;
        continue;
      }
    if (module_aliases == (ModuleAlias *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword) 
    {
      case 'A':
      case 'a':
      {
        if (LocaleCompare((char *) keyword,"alias") == 0)
          {
            module_aliases->alias=AllocateString(token);
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            module_aliases->name=AllocateString(token);
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
  if (module_aliases == (ModuleAlias *) NULL)
    return(False);
  while (module_aliases->previous != (ModuleAlias *) NULL)
    module_aliases=module_aliases->previous;
#endif
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M o d u l e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterModuleInfo adds an entry to the module list.  It returns a
%  pointer to the registered entry on success.
%
%  The format of the RegisterModuleInfo method is:
%
%      ModuleInfo *RegisterModuleInfo(ModuleInfo *entry)
%
%  A description of each parameter follows:
%
%    o info: a pointer to the registered entry is returned.
%
%    o entry: a pointer to the ModuleInfo structure to register.
%
*/
static ModuleInfo *RegisterModuleInfo(ModuleInfo *entry)
{
  register ModuleInfo
    *p;

  /*
    Delete any existing tag.
  */
  assert(entry != (ModuleInfo *) NULL);
  assert(entry->signature == MagickSignature);
  UnregisterModuleInfo(entry->tag);
  entry->previous=(ModuleInfo *) NULL;
  entry->next=(ModuleInfo *) NULL;
  if (module_list == (ModuleInfo *) NULL)
    {
      /*
        Start module list.
      */
      module_list=entry;
      return(entry);
    }
  /*
    Tag is added in lexographic order.
  */
  for (p=module_list; p->next != (ModuleInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,entry->tag) >= 0)
      break;
  if (LocaleCompare(p->tag,entry->tag) < 0)
    {
      /*
        Add entry after target.
      */
      entry->next=p->next;
      p->next=entry;
      entry->previous=p;
      if (entry->next != (ModuleInfo *) NULL)
        entry->next->previous=entry;
      return(entry);
    }
  /*
    Add entry before target.
  */
  entry->next=p;
  entry->previous=p->previous;
  p->previous=entry;
  if (entry->previous != (ModuleInfo *) NULL)
    entry->previous->next=entry;
  if (p == module_list)
    module_list=entry;
  return(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M o d u l e I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetModuleInfo allocates a ModuleInfo structure and initializes the
%  members to default values.
%
%  The format of the SetModuleInfo method is:
%
%      ModuleInfo *SetModuleInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o module_info: Method SetModuleInfo returns the allocated and initialized
%      ModuleInfo structure.
%
%    o tag: a character string that represents the image format associated
%      with the ModuleInfo structure.
%
%
*/
static ModuleInfo *SetModuleInfo(const char *tag)
{
  ModuleInfo
    *entry;

  assert(tag != (const char *) NULL);
  entry=(ModuleInfo *) AcquireMemory(sizeof(ModuleInfo));
  if (entry == (ModuleInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate module info",
      "Memory allocation failed");
  memset(entry,0,sizeof(ModuleInfo));
  entry->tag=AllocateString(tag);
  entry->signature=MagickSignature;
  return(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T a g T o P r o c e s s                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TagToProcess takes a module "name" and returnes a complete file
%  system dynamic module name.
%
%  The format of the TagToProcess method is:
%
%      void TagToProcess(const char *tag)
%
%  A description of each parameter follows:
%
%    o tag: a character string that represents the name of the particular
%      processing dynamically loadable module.
%
*/
static char *TagToProcess(const char *tag)
{
  char
    *module_name;

  assert(tag != (char *) NULL);
  module_name=AllocateString((char *) NULL);
#if !defined(WIN32)
  (void) FormatString(module_name,"%.1024s.la",tag);
  (void) LocaleLower(module_name);
#else
  (void) FormatString(module_name,"%.1024s.dll",tag);
#endif
  return(module_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T a g T o M o d u l e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TagToModule takes a module "name" and returnes a complete file
%  system dynamic module name.
%
%  The format of the TagToModule method is:
%
%      char *TagToModule(const char *tag)
%
%  A description of each parameter follows:
%
%    o tag: a character string that represents the name of the particular
%      module.
%
*/
static char *TagToModule(const char *tag)
{
  char
    *module_name;

  assert(tag != (char *) NULL);
  module_name=AllocateString("tag");
#if !defined(WIN32)
  (void) FormatString(module_name,"%.1024s.la",tag);
  (void) LocaleLower(module_name);
#else
  if (LocaleNCompare("IM_MOD_",tag,7) == 0)
    (void) strcpy(module_name,tag);
  else
    {
#if defined(_DEBUG)
      FormatString(module_name,"IM_MOD_DB_%.1024s_.dll",tag);
#else
      FormatString(module_name,"IM_MOD_RL_%.1024s_.dll",tag);
#endif
    }
#endif
  return(module_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n l o a d D y n a m i c M o d u l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnloadDynamicModule unloads a module, and invokes its de-registration
%  function. Returns True on success, and False if there is an error.
%
%  The format of the UnloadDynamicModule method is:
%
%      int UnloadDynamicModule(const char *module)
%
%  A description of each parameter follows:
%
%    o module: a character string that indicates the module to unload.
%
*/
static int UnloadDynamicModule(const char *module)
{
  char
    name[MaxTextExtent];

  ExceptionInfo
    exception;

  ModuleInfo
    *module_info;

  void
    (*method)(void);

  assert(module != (const char *) NULL);
  GetExceptionInfo(&exception);
  module_info=GetModuleInfo(module,&exception);
  if (module_info == (ModuleInfo *) NULL)
    return(False);
  /*
    Locate and execute UnregisterFORMATImage function
  */
  ModuleToTag(module,"Unregister%sImage",name);
  method=(void (*)(void)) lt_dlsym((ModuleHandle) module_info->handle,name);
  if (method == (void (*)(void)) NULL)
    MagickWarning(DelegateWarning,"failed to find symbol",lt_dlerror());
  else
    method();
  /*
    Close and remove module from list.
  */
  lt_dlclose((ModuleHandle) module_info->handle);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M o d u l e I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterModuleInfo removes a tag from the module info list.  It
%  returns False if the tag does not exist in the list otherwise True.
%
%  The format of the UnregisterModuleInfo method is:
%
%      unsigned int UnregisterModuleInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterModuleInfo returns False if the tag does not
%      exist in the list otherwise it returns True.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
*/
static int UnregisterModuleInfo(const char *tag)
{
  ModuleInfo
    *module_info;

  register ModuleInfo
    *p;

  assert(tag != (const char *) NULL);
  for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->tag,tag) != 0)
      continue;
    LiberateMemory((void **) &p->tag);
    if (p->previous != (ModuleInfo *) NULL)
      p->previous->next=p->next;
    else
      {
        module_list=p->next;
        if (p->next != (ModuleInfo *) NULL)
          p->next->previous=(ModuleInfo *) NULL;
      }
    if (p->next != (ModuleInfo *) NULL)
      p->next->previous=p->previous;
    module_info=p;
    LiberateMemory((void **) &module_info);
    return(True);
  }
  return(False);
}
