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
#if defined(HasLTDL)
#include "magick/modules.h"
#include "defines.h"

/*
  Global declarations.
*/
static ModuleInfo
  *module_info = (ModuleInfo *) NULL;
static ModuleAliases
  *module_aliases = (ModuleAliases *) NULL;

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
%      void DestroyModuleInfo()
%
*/
Export void DestroyModuleInfo()
{
  ModuleInfo
    *entry;

  register ModuleInfo
    *p;

  for (p=module_info; p != (ModuleInfo *) NULL; )
  {
    entry=p;
    p=p->next;
    if (entry->tag != (char *) NULL)
      FreeMemory((void **) &entry->tag);
    FreeMemory((void **) &entry);
  }
  module_info=(ModuleInfo *) NULL;
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
%  Method ExitModules de-initialize modules subsystem and prepares for exit.
%  list.
%
%  The format of the ExitModules method is:
%
%      void ExitModules()
%
*/
Export void ExitModules(void)
{
  register ModuleInfo
    *p;

  /* Unload and de-register all loaded modules */
  while((p=GetModuleInfo((char *)NULL))!=(ModuleInfo*)NULL)
    UnloadModule(p->tag);

  /* Shut down libltdl */
 /*  lt_dlexit(void); */
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
Export ModuleInfo *GetModuleInfo(const char *tag)
{
  register ModuleInfo
    *p;

  if (module_info == (ModuleInfo*) NULL)
    return((ModuleInfo*) NULL);
  if (tag == (char *) NULL)
    return(module_info);
  for (p=module_info; p != (ModuleInfo *) NULL; p=p->next)
    if (Latin1Compare(p->tag,tag) == 0)
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
%  invoked before the LoadModule method may be used.
%
%  The format of the InitializeModules method is:
%
%      void InitializeModules()
%
*/
Export void InitializeModules(void)
{
  static int
    initialized = 0;

  char
    aliases[MaxTextExtent],
    alias[MaxTextExtent],
    module[MaxTextExtent];

  ModuleAliases
    *entry,
    *p;

  FILE*
    file;

  if (!initialized)
    {
      /* Initialize ltdl */
      if(lt_dlinit() != 0)
	{
	  const char *dlerror = lt_dlerror();
	  printf("ERROR: failed to initialise ltdl: %s\n", dlerror);
	  exit(1);
	}
      
      /* Set ltdl module search path */
      lt_dlsetsearchpath( CoderModuleDirectory );

      /* Load module aliases */
      strcpy(aliases, DelegatePath);
      strcat(aliases, DirectorySeparator);
      strcat(aliases, "modules.mgk");

      file = fopen(aliases, "r");
      if(file != (FILE*) NULL)
	{
	  p=(ModuleAliases*) NULL;
	  while(!feof(file))
	    {
	      if(fscanf(file, "%s %s", alias, module) == 2)
		{
		  entry = (ModuleAliases*)AllocateMemory(sizeof(ModuleAliases));
		  if(entry != (ModuleAliases*) NULL)
		    {
		      entry->alias = AllocateString(alias);
		      entry->module = AllocateString(module);
		      entry->next = (ModuleAliases*) NULL;

		      if (module_aliases != (ModuleAliases*) NULL)
			{
			  p->next = entry;
			  p=p->next;
			}
		      else
			{
			  module_aliases = entry;
			  p=module_aliases;
			}
		    }
		}
	    }
	  fclose(file);
	}
      /*module_aliases*/
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o a d A l l M o d u l e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LoadAllModules loads all available modules.
%
%  The format of the LoadAllModules method is:
%
%      void LoadAllModules()
%
*/
Export int LoadAllModules(void)
{
  char
    **module_list,
    **p;

  int
    i;

  /* Load all modules */
  module_list = ListModules();
  if(module_list == (char**)NULL)
    return False;

  p = module_list;
  while(*p)
    LoadModule(*p++);

  /* Free list memory */
  i=0;
  while(module_list[i])
    FreeMemory((void**)&module_list[i++]);
  FreeMemory((void **) &module_list);

  return True;
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
*/
Export char **ListModules(void)
{
  char
    **file_list,
    **module_list,
    current_directory[MaxTextExtent];

  int
    entry,
    i;

  static int
    number_files = 256;

  /* Get list of module files */
  (void) getcwd(current_directory,MaxTextExtent-1);
  file_list=ListFiles(CoderModuleDirectory,
		      "*.la", &number_files);
  (void) chdir(current_directory);
  if (file_list == (char **) NULL)
    return (char **)NULL;

  /* Create list of modules (uppercased base names of module files) */
  module_list=(char **) AllocateMemory((number_files+1)*sizeof(char *));
  entry=0;
  while(entry<number_files)
    {
      module_list[entry]=BaseFilename(file_list[entry]);
      Latin1Upper(module_list[entry]);
      ++entry;
    }
  module_list[entry]=(char*)NULL;

  /* Deallocate memory associated with file list */
  for (i=0; i < number_files; i++)
    FreeMemory((void **) &file_list[i]);
  if (file_list != (char **) NULL)
    FreeMemory((void **) &file_list);

  return module_list;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o a d M o d u l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LoadModule loads a module, and invokes its registration function.
%  Returns True on success, and False if there is an error.
%
%  The format of the LoadModule method is:
%
%      int LoadModule(const char *module)
%
%  A description of each parameter follows:
%
%    o module: a character string that indicates the module to load.
%
*/
Export int LoadModule(const char* module)
{
  char
    *dest,
    func_name[MaxTextExtent],
    module_file[MaxTextExtent],
    module_name[MaxTextExtent],
    *source;

  ModuleHandle
    handle;

  ModuleInfo
    *module_info;

  ModuleAliases
    *p;

  void (*register_func)(void);

  /* Assign module name from alias */
  strcpy(module_name,module);
  if(module_aliases != (ModuleAliases*)NULL)
    {
      for(p=module_aliases; p->next != (ModuleAliases*)NULL; p=p->next)
	if(Latin1Compare(p->alias,module) == 0)
	  {
	    strcpy(module_name,p->module);
	    break;
	  }
    }

  /*
    Build module file name from module name
  */
  dest=module_file;
  source=module_name;
  while(*source)
    {
      *dest = tolower(*source);
      ++dest;
      ++source;
    }
  *dest=0;
  strcat(module_file,".la");

  /*
    Load module file
  */
  if( ( handle=lt_dlopen( module_file ) ) == 0)
    {
      printf("WARNING: failed to load module \"%s\": %s\n",
	     module_file, lt_dlerror());
      return False;
    }

  /*
    Add module to module list
  */
  module_info=SetModuleInfo(module_name);
  if (module_info==(ModuleInfo*)NULL)
    {
      lt_dlclose(handle);
      return False;
    }
  module_info->handle=handle;
  time(&module_info->load_time);
  if(!RegisterModuleInfo(module_info))
    return False;

  /*
    Locate and execute RegisterFORMATImage function
  */
  strcpy(func_name, "Register");

  /* Hack due to 8BIM vs bim.c naming difference */
  if(!strcmp("BIM", module_name))
    strcat(func_name,"8");

  strcat(func_name,module_name);
  strcat(func_name, "Image");

  /*
    Locate and invoke module registration function
  */
  register_func=(void (*)(void))lt_dlsym(handle, func_name);
  if (register_func == NULL)
    {
      printf("WARNING: failed to find symbol : %s\n",
	     lt_dlerror());
      return False;
    }
  register_func();

  return True;
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
%  Method RegisterModuleInfo adds an entry to the module list.
%  Returns a pointer to the registered entry on success.
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
Export ModuleInfo *RegisterModuleInfo(ModuleInfo *entry)
{
  register ModuleInfo
    *p;

  p=(ModuleInfo *) NULL;
  if (module_info != (ModuleInfo *) NULL)
    {
      for (p=module_info; p->next != (ModuleInfo *) NULL; p=p->next)
	{
	  if (Latin1Compare(p->tag,entry->tag) >= 0)
	    {
	      if (Latin1Compare(p->tag,entry->tag) == 0)
		{
		  p=p->previous;
		  UnregisterModuleInfo(entry->tag);
		}
	      break;
	    }
	}
    }
  if (module_info == (ModuleInfo *) NULL)
    {
      module_info=entry;
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
Export ModuleInfo *SetModuleInfo(const char *tag)
{
  ModuleInfo
    *entry;

  entry=(ModuleInfo *) AllocateMemory(sizeof(ModuleInfo));
  if (entry == (ModuleInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate module info",
      "Memory allocation failed");
  entry->tag=AllocateString(tag);
  entry->handle=
    (ModuleHandle) NULL;
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
%   U n l o a d M o d u l e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnloadModule unloads a module, and invokes its de-registration
%  function. Returns True on success, and False if there is an error.
%
%  The format of the UnloadModule method is:
%
%      int UnloadModule(const char *module)
%
%  A description of each parameter follows:
%
%    o module: a character string that indicates the module to unload.
%
*/
Export int UnloadModule(const char* module)
{
  char
    func_name[MaxTextExtent];

  ModuleInfo
    *module_info;

  void (*unregister_func)(void);

  module_info=GetModuleInfo(module);
  if(module_info!=(ModuleInfo*)NULL)
    {
      /*
	Locate and execute UnregisterFORMATImage function
      */
      strcpy(func_name, "Unregister");
      strcat(func_name,module);
      strcat(func_name, "Image");

      /*
	Locate and invoke module de-registration function
      */
      unregister_func=(void (*)(void))lt_dlsym(module_info->handle, func_name);
      if (unregister_func == NULL)
	printf("WARNING: failed to find symbol : %s\n",
	       lt_dlerror());
      else
	unregister_func();

      /*
	Close module
      */
      lt_dlclose(module_info->handle);

      /*
	Remove module from list
      */
      UnregisterModuleInfo(module);
      return True;
    }
  return False;
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
Export int UnregisterModuleInfo(const char *tag)
{
  ModuleInfo
    *module_info;

  register ModuleInfo
    *p;

  for (p=GetModuleInfo((char *) NULL); p != (ModuleInfo *) NULL; p=p->next)
  {
    if (Latin1Compare(p->tag,tag) == 0)
      {
        if (p->tag != (char *) NULL)
          FreeMemory((void *) &p->tag);
        if (p->previous != (ModuleInfo *) NULL)
          p->previous->next=p->next;
        else
          {
            module_info=p->next;
            if (p->next != (ModuleInfo *) NULL)
              p->next->previous=(ModuleInfo *) NULL;
          }
        if (p->next != (ModuleInfo *) NULL)
          p->next->previous=p->previous;
        module_info=p;
        FreeMemory((void *) &module_info);
        return(True);
    }
  }
  return(False);
}


#endif /* HasLTDL */
