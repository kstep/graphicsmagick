/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
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
%  Method DestroyModuleInfo deallocates memory associated ModuleInfo list.
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
 * De-initialize modules subsystem and prepare for exit
 *
 */
Export void ExitModules(void)
{
  /* Unload all modules */

  /* Shut down libltdl. */
/*   lt_dlexit (void); */
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
 * Initialize modules subsystem
 *
 */
Export void InitializeModules(void)
{
  static int
    initialized = 0;

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
    }
}

/*
 * List modules available to load
 *
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
 * Load a module
 *
 */
Export int LoadModule(const char* module)
{
  char
    *dest,
    func_name[MaxTextExtent],
    module_file[MaxTextExtent],
    *source;

  lt_dlhandle
    handle;

  ModuleInfo
    *module_info;

  void (*register_func)(void);

  /*
    Build module file name from module name
  */
  dest=module_file;
  source=module;
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
  module_info=SetModuleInfo(module);
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
  if(!strcmp("BIM", module))
    strcat(func_name,"8");

  strcat(func_name,module);

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
    (lt_dlhandle) NULL;
  entry->load_time=0;
  entry->previous=(ModuleInfo *) NULL;
  entry->next=(ModuleInfo *) NULL;
  return(entry);
}

/*
 * Unload a module
 *
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
	Locate and execute UnRegisterFORMATImage function
      */
      strcpy(func_name, "UnRegister");

      /* Hack due to 8BIM vs bim.c naming difference */
      if(!strcmp("BIM", module))
	strcat(func_name,"8");

      strcat(func_name,module);

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
Export unsigned int UnregisterModuleInfo(const char *tag)
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
