/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               M   M   OOO   DDDD   U   U  L      EEEEE  SSSSS               %
%               MM MM  O   O  D   D  U   U  L      E      SS                  %
%               M M M  O      D   D  U   U  L      EEE     SSS                %
%               M   M  O   O  D   D  U   U  L      E         SS               %
%               M   M   OOO   DDDD    UUU   LLLLL  EEEEE  SSSSS               %
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
#if defined(HasLTDL) || defined(_MAGICKMOD_)
#include "modules.h"
#include "defines.h"
/*
  Constant declaractions.
*/
const char
  *ModuleFilename = "modules.mgk";

/*
  Global declarations.
*/
static ModuleAliases
  *module_aliases = (ModuleAliases *) NULL;

static ModuleInfo
  *module_list = (ModuleInfo *) NULL;

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
    *entry;

  register ModuleInfo
    *p;

  for (p=module_list; p != (ModuleInfo *) NULL; )
  {
    entry=p;
    p=p->next;
    if (entry->tag != (char *) NULL)
      LiberateMemory((void **) &entry->tag);
    LiberateMemory((void **) &entry);
  }
  module_list=(ModuleInfo *) NULL;
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
%    o image: The address of a structure of type Image.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
*/
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
  if (handle == 0)
    {
      char
        message[MaxTextExtent];

      FormatString(message,"failed to load module \"%.1024s\"",module_name);
      MagickWarning(MissingDelegateWarning,message,lt_dlerror());
    }
  else
    {
      (void) strcpy(module_name,tag);
      (void) strcat(module_name,"Image");
      method=(unsigned int (*)(Image *,const int,char **))
        lt_dlsym(handle,module_name);
      if (method != NULL)
        status=(*method)(image,argc,argv);
      lt_dlclose(handle);
    }
  LiberateMemory((void **) &module_name);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x i t M o d u l e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExitModules de-initializes modules subsystem and prepares for exit.
%  list.
%
%  The format of the ExitModules method is:
%
%      void ExitModules(void)
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport void ExitModules(void)
{
  ModuleAliases
    *alias,
    *entry;

  register ModuleInfo
    *p;

  if (module_list != (ModuleInfo *) NULL)
    {
      /*
        Unload and unregister all loaded modules.
      */
      while ((p=GetModuleInfo((char *) NULL)) != (ModuleInfo *) NULL)
        UnloadDynamicModule(p->tag);
      /*
        Free memory associated with ModuleAliases list.
      */
      for (alias=module_aliases; alias != (ModuleAliases *) NULL; )
      {
        entry=alias;
        alias=alias->next;
        LiberateMemory((void **) &entry->alias);
        LiberateMemory((void **) &entry->module);
        LiberateMemory((void **) &entry);
      }
      module_aliases=(ModuleAliases *) NULL;
    }
  module_list=(ModuleInfo *) NULL;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

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
%      ModuleInfo *GetModuleInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o module_info: Method GetModuleInfo returns a pointer ModuleInfo
%      structure that matches the specified tag.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%
*/
MagickExport ModuleInfo *GetModuleInfo(const char *tag)
{
  register ModuleInfo
    *p;

  if (module_list == (ModuleInfo*) NULL)
    return((ModuleInfo*) NULL);
  if (tag == (char *) NULL)
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
%   I n i t i a l i z e M o d u l e s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeModules initializes the modules subsystem. It must be
%  invoked before the OpenModule method may be used.
%
%  The format of the InitializeModules method is:
%
%      void InitializeModules()
%
*/
MagickExport void InitializeModules(void)
{
#define MaxPathElements  31

  char
    alias[MaxTextExtent],
    module[MaxTextExtent],
    *path;

  FILE
    *file;

  ModuleAliases
    *aliases,
    *entry;

  unsigned int
    match;

  /*
    Initialize ltdl.
  */
  if (module_list != (ModuleInfo *) NULL)
    return;
  if (lt_dlinit() != 0)
    MagickError(DelegateError,"failed to initialise module loader",
      lt_dlerror());
  /*
    Read the module configuration files.
  */
  module_aliases=(ModuleAliases *) NULL;
  path=GetMagickConfigurePath(ModuleFilename);
  if (path == (char *) NULL)
    return;
  file=fopen(path,"r");
  LiberateMemory((void **) &path);
  if (file == (FILE*) NULL)
    return;
  while (!feof(file))
  {
    if (fscanf(file,"%s %s",alias,module) != 2)
      continue;
    match=False;
    entry=module_aliases;
    while (entry != (ModuleAliases *) NULL)
    {
      if (LocaleCompare(entry->alias,alias) == 0)
        {
          match=True;
          break;
        }
      entry=entry->next;
    }
    if (match != False)
      continue;
    entry=(ModuleAliases *) AcquireMemory(sizeof(ModuleAliases));
    if (entry == (ModuleAliases*) NULL)
      continue;
    entry->alias=AllocateString(alias);
    entry->module=AllocateString(module);
    entry->next=(ModuleAliases *) NULL;
    if (module_aliases == (ModuleAliases *) NULL)
      {
        module_aliases=entry;
        aliases=module_aliases;
        continue;
      }
    aliases->next=entry;
    aliases=aliases->next;
  }
  (void) fclose(file);
  atexit(ExitModules);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t M o d u l e s                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModules returns a list containing the names of modules which may
%  be loaded.
%
%  The format of the ListModules function is:
%
%      char **ListModules()
%
%  A description of each parameter follows:
%
%    o modulelist: Method ListModules returns a list of available modules. If
%      an error occurs a NULL list is returned.
%
*/
MagickExport char **ListModules(void)
{
  char
    **module_list,
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
  module_list=(char **) AcquireMemory((max_entries+1)*sizeof(char *));
  if (module_list == (char **) NULL)
    return((char **) NULL);
  *module_list=(char *) NULL;
  path=GetMagickConfigurePath(ModuleFilename);
  if (path == (char *) NULL)
    return((char **) NULL);
  path[Extent(path)-Extent(ModuleFilename)-1]='\0';
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
    if (i >= max_entries)
      {
        max_entries<<=1;
        ReacquireMemory((void **) &module_list,max_entries*sizeof(char *));
        if (module_list == (char **) NULL)
          break;
      }
    /*
      Add new module name to list.
    */
    module_list[i]=BaseFilename(entry->d_name);
    LocaleUpper(module_list[i]);
    if (LocaleNCompare("IM_MOD_",module_list[i],7) == 0)
      {
        (void) strcpy(module_list[i],module_list[i]+10);
        module_list[i][Extent(module_list[i])-1]='\0';
      }
    i++;
    module_list[i]=(char *) NULL;
    entry=readdir(directory);
  }
  (void) closedir(directory);
  return(module_list);
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
%  Method ModuleToTag parser a file system module name into the basic name
%  of the module.
%
%  The format of the ModuleToTag method is:
%
%      int ModuleToTag(const char *filename, const char *format, char *module)
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
void ModuleToTag(const char *filename,const char *format,char *module)
{
  char
    *module_name;

  assert(format != (char *) NULL);
  assert(module != (char *) NULL);
  assert(filename != (char *) NULL);
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
%  Method OpenModule loads a module, and invokes its registration
%  function.  It returns True on success, and False if there is an error.
%
%  The format of the OpenModule method is:
%
%      int OpenModule(const char *module)
%
%  A description of each parameter follows:
%
%    o module: a character string that indicates the module to load.
%
*/
MagickExport int OpenModule(const char *module)
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

  register ModuleAliases
    *p;

  void
    (*method)(void);

  /*
    Assign module name from alias.
  */
  assert(module != (const char *) NULL);
  (void) strcpy(module_name,module);
  if (module_aliases != (ModuleAliases *) NULL)
    {
      for (p=module_aliases; p != (ModuleAliases *) NULL; p=p->next)
        if (LocaleCompare(p->alias,module) == 0)
          {
            (void) strcpy(module_name,p->module);
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
          MagickWarning(MissingDelegateWarning,message,lt_dlerror());
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
      MagickWarning(MissingDelegateWarning,"failed to find symbol",
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
%      void OpenModules(void)
%
*/
MagickExport int OpenModules(void)
{
  char
    **module_list;

  register char
    **p;

  register int
    i;

  /*
    Load all modules.
  */
  module_list=ListModules();
  if (module_list == (char **) NULL)
    return(False);
  p=module_list;
  while (*p)
    OpenModule(*p++);
  /*
    Free resources.
  */
  for (i=0; module_list[i]; i++)
    LiberateMemory((void **) &module_list[i]);
  LiberateMemory((void **) &module_list);
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
MagickExport ModuleInfo *RegisterModuleInfo(ModuleInfo *entry)
{
  register ModuleInfo
    *p;

  assert(entry != (ModuleInfo *) NULL);
  p=(ModuleInfo *) NULL;
  if (module_list != (ModuleInfo *) NULL)
    for (p=module_list; p->next != (ModuleInfo *) NULL; p=p->next)
    {
      if (LocaleCompare(p->tag,entry->tag) >= 0)
        {
          if (LocaleCompare(p->tag,entry->tag) == 0)
            {
              p=p->previous;
              UnregisterModuleInfo(entry->tag);
            }
          break;
        }
    }
  if (module_list == (ModuleInfo *) NULL)
    {
      module_list=entry;
      return(entry);
    }
  entry->signature=MagickSignature;
  entry->previous=p;
  if (p != (ModuleInfo *) NULL)
    {
      entry->next=p->next;
      p->next=entry;
    }
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
MagickExport ModuleInfo *SetModuleInfo(const char *tag)
{
  ModuleInfo
    *entry;

  assert(tag != (const char *) NULL);
  entry=(ModuleInfo *) AcquireMemory(sizeof(ModuleInfo));
  if (entry == (ModuleInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate module info",
      "Memory allocation failed");
  entry->tag=AllocateString(tag);
  entry->handle=(ModuleHandle) NULL;
  entry->load_time=0;
  entry->previous=(ModuleInfo *) NULL;
  entry->next=(ModuleInfo *) NULL;
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
char *TagToProcess(const char *tag)
{
  char
    *module_name;

  assert(tag != (char *) NULL);
  module_name=AllocateString((char *) NULL);
#if !defined(_VISUALC_)
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
%      void TagToModule(const char *tag)
%
%  A description of each parameter follows:
%
%    o tag: a character string that represents the name of the particular
%      module.
%
*/
char *TagToModule(const char *tag)
{
  char
    *module_name;

  assert(tag != (char *) NULL);
  module_name=AllocateString("tag");
#if !defined(_VISUALC_)
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
MagickExport int UnloadDynamicModule(const char *module)
{
  char
    name[MaxTextExtent];

  ModuleInfo
    *module_info;

  void
    (*method)(void);

  assert(module != (const char *) NULL);
  module_info=GetModuleInfo(module);
  if (module_info == (ModuleInfo *) NULL)
    return(False);
  /*
    Locate and execute UnregisterFORMATImage function
  */
  ModuleToTag(module,"Unregister%sImage",name);
  method=(void (*)(void)) lt_dlsym(module_info->handle,name);
  if (method == (void (*)(void)) NULL)
    MagickWarning(DelegateWarning,"failed to find symbol",lt_dlerror());
  else
    method();
  /*
    Close and remove module from list.
  */
  lt_dlclose(module_info->handle);
  UnregisterModuleInfo(module);
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
MagickExport int UnregisterModuleInfo(const char *tag)
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
#endif /* HasLTDL */
