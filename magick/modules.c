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
  Global declarations.
*/
static char
  **module_path = (char **) NULL;

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
      FreeMemory((void **) &entry->tag);
    FreeMemory((void **) &entry);
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

  status=False;
  module_name=TagToModule(tag);
  handle=lt_dlopen(module_name);
  if (handle == 0)
    {
      char
        message[MaxTextExtent];

      FormatString(message,"failed to load module \"%s\"",module_name);
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
  FreeMemory((void **) &module_name);
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
MagickExport void ExitModules(void)
{
  ModuleAliases
    *alias,
    *entry;

  register int
    i;

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
        FreeMemory((void **) &entry->alias);
        FreeMemory((void **) &entry->module);
        FreeMemory((void **) &entry);
      }
      module_aliases=(ModuleAliases *) NULL;
      /*
        Free memory associated with module directory search list.
      */
      for (i=0; module_path[i]; i++)
        FreeMemory((void **) &module_path[i]);
      FreeMemory((void**) &module_path);
    }
  module_list=(ModuleInfo *) NULL;
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

static void InitializeModuleAliases(void)
{
  FILE
    *file;

  char
    alias[MaxTextExtent],
    aliases[MaxTextExtent],
    module[MaxTextExtent];

  ModuleAliases
    *entry;

  register int
    i;

  register ModuleAliases
    *p;

  unsigned int
    match;

  p=(ModuleAliases*) NULL;
  for (i=0; module_path[i]; i++)
  {
    (void) strcpy(aliases,module_path[i]);
    (void) strcat(aliases,DirectorySeparator);
    (void) strcat(aliases,"modules.mgk");
    file=fopen(aliases,"r");
    if (file == (FILE*) NULL)
      continue;
    while(!feof(file))
    {
      if (fscanf(file,"%s %s",alias,module) != 2)
        continue;
      match=False;
      if (module_aliases != (ModuleAliases *) NULL)
        {
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
        }
      if (match != False)
        continue;
      entry=(ModuleAliases *) AllocateMemory(sizeof(ModuleAliases));
      if (entry == (ModuleAliases*) NULL)
        continue;
      entry->alias=AllocateString(alias);
      entry->module=AllocateString(module);
      entry->next=(ModuleAliases *) NULL;
      if (module_aliases != (ModuleAliases *) NULL)
        {
          p->next=entry;
          p=p->next;
        }
      else
        {
          module_aliases=entry;
          p=module_aliases;
        }
    }
    (void) fclose(file);
  }
}

static void InitializeModuleSearchPath(void)
{
#define MaxPathElements  31

  char
    message[MaxTextExtent];

  register char
    *p,
    *q;

  register int
    i;

  /*
    Add user specified path.
  */
  i=0;
  module_path=(char **) AllocateMemory((MaxPathElements+1)*sizeof(char*));
  if (module_path == (char **) NULL)
    MagickError(ResourceLimitError,"Unable to allocate module path",
      "Memory allocation failed");
  if (getenv("MAGICK_MODULE_PATH") != (char *) NULL)
    {
      p=getenv("MAGICK_MODULE_PATH");
      while (i < MaxPathElements)
      {
        q=strchr(p,DirectoryListSeparator);
        if (q == (char *) NULL)
          {
            module_path[i++]=AllocateString(p);
            break;
          }
        (void) strncpy(message,p,q-p);
        message[q-p]='\0';
        module_path[i++]=AllocateString(message);
        p=q+1;
      }
    }
  /*
    Add HOME/.magick if it exists
  */
  p=getenv("HOME");
  if ((i < MaxPathElements) && (p != (char *) NULL))
    {
      (void) strcpy(message,p);
      (void) strcat(message,"/.magick");
      if (access(message,R_OK) == 0)
        module_path[i++]=AllocateString(message);
    }
  /*
    Add default module installation directory.
  */
  if (i < MaxPathElements)
    module_path[i++]=AllocateString(CoderModuleDirectory);
  module_path[i]=(char *) NULL;
}

MagickExport void InitializeModules(void)
{
  /*
    Initialize ltdl.
  */
  if (module_list != (ModuleInfo *) NULL)
    return;
  if (lt_dlinit() != 0)
    MagickError(DelegateError,"failed to initialise module loader",
      lt_dlerror());
  /*
    Determine and set module search path.
  */
  InitializeModuleSearchPath();
  InitializeModuleAliases();
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
    message[MaxTextExtent],
    **module_list;

#if !defined(_VISUALC_)
  char
    *p,
    *q;
#endif

  DIR
    *directory;

  int
    newentry,
    path_index;

  register int
    i;

  struct dirent
    *entry;

  unsigned int
    entry_index,
#if !defined(_VISUALC_)
    name_length,
#endif
    max_entries;

  max_entries=255;
  entry_index=0;
  module_list=(char **) AllocateMemory((max_entries+1)*sizeof(char *));
  if (module_list == (char **)NULL)
    return((char **) NULL);
  module_list[entry_index]=(char*)NULL;
  for (path_index=0; module_path[path_index]; path_index++)
  {
#if !defined(_VISUALC_)
    directory=opendir(module_path[path_index]);
#else
    (void) strcpy(message,module_path[path_index]);
    (void) strcat(message,"\\");
    (void) strcat(message,ModuleSearchSpec);
    directory=opendir(message);
#endif
    if (directory == (DIR *) NULL)
      continue;
    entry=readdir(directory);
    while (entry != (struct dirent *) NULL)
    {
#if defined(HasLTDL)
      name_length=Extent(entry->d_name);
      p=(entry->d_name+name_length-3);
      if ((name_length < 4) || (*p++ != '.') || (*p++ != 'l') || (*p != 'a'))
        {
          entry=readdir(directory);
          continue;
        }
#endif
      if (entry_index >= max_entries)
        {
          max_entries<<=1;
          ReallocateMemory((void **) &module_list,max_entries*sizeof(char *));
          if (module_list == (char **) NULL)
            break;
        }
      /*
        Only add new module name to list.
      */
      newentry=True;
      for (i=0; module_list[i]; i++)
      {
        if (LocaleCompare(entry->d_name,module_list[i]) == 0)
          {
            newentry=False;
            break;
          }
      }
      if (newentry == True)
        {
#if !defined(_VISUALC_)
          module_list[entry_index]=(char *)AllocateMemory(name_length);
          if (module_list[entry_index] == (char *) NULL)
            break;
          p=module_list[entry_index];
          q=entry->d_name;
          for (i=name_length-3; i != 0; i--)
            *p++=toupper((int) *q++);
          *p=0;
#else
          module_list[entry_index]=AllocateString(entry->d_name);
#endif
          entry_index++;
          module_list[entry_index]=(char *) NULL;
        }
      entry=readdir(directory);
    }
    (void) closedir(directory);
  }
  return(module_list);
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
    module_load_path[MaxTextExtent],
    module_name[MaxTextExtent],
    name[MaxTextExtent];

  ModuleHandle
    handle;

  ModuleInfo
    *module_info;

  register int
    i;

  register ModuleAliases
    *p;

  void
    (*method)(void);

  /*
    Assign module name from alias.
  */
  (void) strcpy(module_name,module);
  if (module_aliases != (ModuleAliases *) NULL)
    {
      for (p=module_aliases; p->next != (ModuleAliases *) NULL; p=p->next)
        if (LocaleCompare(p->alias,module) == 0)
          {
            (void) strcpy(module_name,p->module);
            break;
          }
    }
  /*
    Load module file.
  */
  module_file=TagToModule(module_name);
  handle=(ModuleHandle) NULL;
  for (i=0; module_path[i]; i++)
  {
    /* 
      Only attempt to load module if module file exists.
    */
    (void) strcpy(module_load_path,module_path[i]);
    (void) strcat(module_load_path,DirectorySeparator);
    (void) strcat(module_load_path,module_file);
    if (access(module_load_path,F_OK) != 0)
      continue;
    handle=lt_dlopen(module_load_path);
    if (handle != 0)
      continue;
    FormatString("failed to load module \"%s\"",module_load_path);
    MagickWarning(MissingDelegateWarning,message,lt_dlerror());
    FreeMemory((void **) &module_file);
    return(False);
  }
  FreeMemory((void **) &module_file);
  if (handle == 0)
    return(False);
  /*
    Add module to module list
  */
  module_info=SetModuleInfo(module_name);
  if (module_info==(ModuleInfo*)NULL)
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
    FreeMemory((void **) &module_list[i]);
  FreeMemory((void **) &module_list);
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
#define IsTagSeparator(c)  ((c) == '_')

#if !defined(_VISUALC_)
  assert(format != (char *) NULL);
  assert(module != (char *) NULL);
  assert(filename != (char *) NULL);
  FormatString(module,format,filename);
#else
  char
    *basename;

  register char
    *p;

  int
    count;

  assert(format != (char *) NULL);
  assert(module != (char *) NULL);
  assert(filename != (char *) NULL);
  if (LocaleNCompare("IM_MOD_",filename,6) != 0)
    {
      FormatString(module,format,filename);
      return;
    }
  count=0;
  basename=AllocateString(filename);
  p=basename+(Extent(basename)-1);
  while (p > basename)
  {
    if (IsTagSeparator(*p))
      {
        if (!count)
          {
            *p='\0';
            count++;
          }
        else
          {
            LocaleUpper(&p[1]);
            FormatString(module,format,&p[1]);
            break;
          }
      }
    p--;
  }
  FreeMemory((void **) &basename);
#endif
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
  entry->previous=p;
  entry->next=p->next;
  p->next=entry;
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

  entry=(ModuleInfo *) AllocateMemory(sizeof(ModuleInfo));
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
  module_name=(char *) AllocateMemory(MaxTextExtent);
  if (module_name == (char *) NULL)
    MagickError(ResourceLimitError,"Unable to get module name",
      "Memory allocation failed");
#if !defined(_VISUALC_)
  (void) FormatString(module_name,"%s.la",tag);
  (void) LocaleLower(module_name);
#else
  if (LocaleNCompare("IM_MOD_",tag,6) == 0)
    strcpy(module_name,tag);
  else
    {
#if defined(_DEBUG)
      FormatString(module_name,"IM_MOD_DB_%s_.dll",tag);
#else
      FormatString(module_name,"IM_MOD_RL_%s_.dll",tag);
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
MagickExport int UnloadDynamicModule(const char* module)
{
  char
    name[MaxTextExtent];

  ModuleInfo
    *module_info;

  void
    (*method)(void);

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

  for (p=module_list; p != (ModuleInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->tag,tag) != 0)
      continue;
    FreeMemory((void **) &p->tag);
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
    FreeMemory((void **) &module_info);
    return(True);
  }
  return(False);
}
#endif /* HasLTDL */
