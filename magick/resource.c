/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           RRRR    EEEEE   SSSSS   OOO   U   U  RRRR    CCCC  EEEEE          %
%           R   R   E       SS     O   O  U   U  R   R  C      E              %
%           RRRR    EEE      SSS   O   O  U   U  RRRR   C      EEE            %
%           R R     E          SS  O   O  U   U  R R    C      E              %
%           R  R    EEEEE   SSSSS   OOO    UUU   R  R    CCCC  EEEEE          %
%                                                                             %
%                                                                             %
%                        Get/Set ImageMagick Resources.                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/log.h"
#include "magick/resource.h"
#include "magick/utility.h"

/*
  Define  declarations.
*/
#define ResourceInfinity  (~0UL)
#define ResourceToMegabytes(value) ((double) (value)*1024.0*1024.0)
#define MegabytesToResource(value) ((unsigned long) ((value)/1024.0/1024.0))
#define GigabytesToResource(value) \
  ((unsigned long) ((value)/1024.0/1024.0/1024.0))

/*
  Typedef declarations.
*/
typedef struct _ResourceInfo
{
  double
    file,
    memory,
    map,
    disk;

  unsigned long
    file_limit,
    memory_limit,
    map_limit,
    disk_limit;
} ResourceInfo;

/*
  Global declarations.
*/
static SemaphoreInfo
  *resource_semaphore = (SemaphoreInfo *) NULL;

static ResourceInfo
  resource_info =
  {
    0, 0, 0, 0, 256, 1024, 4096, ResourceInfinity
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e M a g i c k R e s o u r c e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireMagickResource() acquires resources of the specified type.  True is
%  returned if the specified resource is available otherwise False.
%
%  The format of the AcquireMagickResource() method is:
%
%      unsigned int AcquireMagickResource(const ResourceType type,
%        const magick_int64_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The number of bytes needed from for this resource.
%
%
*/
MagickExport unsigned int AcquireMagickResource(const ResourceType type,
  const magick_int64_t size)
{
  char
    message[MaxTextExtent];

  unsigned int
    status;

  status=True;
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case FileResource:
    {
      resource_info.file+=size;
      if (resource_info.file_limit == ResourceInfinity)
        break;
      status=resource_info.file <= resource_info.file_limit;
      FormatString(message,"file +%lu/%lu/%lu",(unsigned long) size,
        (unsigned long) resource_info.file,resource_info.file_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case MemoryResource:
    {
      resource_info.memory+=size;
      if (resource_info.memory_limit == ResourceInfinity)
        break;
      status=resource_info.memory <=
        ResourceToMegabytes(resource_info.memory_limit);
      FormatString(message,"memory +%lumb/%lumb/%lumb",
        MegabytesToResource(size),MegabytesToResource(resource_info.memory),
        resource_info.memory_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case MapResource:
    {
      resource_info.map+=size;
      if (resource_info.map_limit == ResourceInfinity)
        break;
      status=resource_info.disk <=
        ResourceToMegabytes(resource_info.map_limit);
      FormatString(message,"map +%lumb/%lumb/%lumb",MegabytesToResource(size),
        MegabytesToResource(resource_info.map),resource_info.map_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case DiskResource:
    {
      resource_info.disk+=size;
      if (resource_info.disk_limit == ResourceInfinity)
        break;
      status=resource_info.disk <=
        ResourceToMegabytes(resource_info.disk_limit);
      FormatString(message,"disk +%lumb/%lugb/%lugb",MegabytesToResource(size),
        GigabytesToResource(resource_info.disk),resource_info.disk_limit/1024);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c k R e s o u r c e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickResources() destroys the resource environment.
%
%  The format of the DestroyMagickResources() method is:
%
%      DestroyMagickResources(void)
%
%
*/
MagickExport void DestroyMagickResources(void)
{
#if defined(JUST_FOR_DOCUMENTATION)
  /* The first two calls should bracket any code that deals with the data
     structurees being released */
  AcquireSemaphoreInfo(&resource_semaphore);
  LiberateSemaphoreInfo(&resource_semaphore);
#endif
  /* The final call actually releases the associated mutex used to prevent
     multiple threads from accessing the data */
  DestroySemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k R e s o u r c e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickResource() returns the the specified resource in megabytes.
%
%  The format of the GetMagickResource() method is:
%
%      unsigned long GetMagickResource(const ResourceType type)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%
*/
MagickExport unsigned long GetMagickResource(const ResourceType type)
{
  unsigned long
    resource;

  resource=0;
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case FileResource:
    {
      resource=(unsigned long) resource_info.file;
      break;
    }
    case MemoryResource:
    {
      resource=MegabytesToResource(resource_info.memory);
      break;
    }
    case MapResource:
    {
      resource=MegabytesToResource(resource_info.map);
      break;
    }
    case DiskResource:
    {
      resource=MegabytesToResource(resource_info.disk);
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(resource);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e M a g i c k R e s o u r c e s                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagickResources() initializes the resource environment.
%
%  The format of the InitializeMagickResources() method is:
%
%      InitializeMagickResources(void)
%
%
*/
MagickExport void InitializeMagickResources(void)
{
  unsigned long
    max_files=256,
    max_disk=ResourceInfinity,
    max_map=4096,
    max_memory=1024;

  /*
    Set Magick resource limits.
  */
#if defined(POSIX)
  {
    long
      files=-1,
      total_memory=0;

#  if defined(HAVE_SYSCONF) && defined(_SC_OPEN_MAX)
    files=sysconf(_SC_OPEN_MAX);
#  endif

#  if defined(_SC_PHYS_PAGES) && (defined(_SC_PAGE_SIZE) || defined(HAVE_GETPAGESIZE))
    {
    long
      pagesize=-1,
      pages=-1;
    /*
      Compute total physical memory based on number of memory pages,
      and page size.
    */
#    if defined(HAVE_SYSCONF) && defined(_SC_PHYS_PAGES)
    pages=sysconf(_SC_PHYS_PAGES);
#    endif /* defined(HAVE_SYSCONF) && defined(_SC_PHYS_PAGES) */

#    if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
    pagesize=sysconf(_SC_PAGE_SIZE);
#    endif /* defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE) */

#    if defined(HAVE_GETPAGESIZE)
    if (pagesize <= 0)
      pagesize=getpagesize();
#    endif /* defined(HAVE_GETPAGESIZE) */

    if (pages > 0 && pagesize > 0)
      total_memory=((pages+512)/1024)*((pagesize+512)/1024);
    (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
      "Total physical memory %ld MB (%ld pages and %ld bytes per page)",
        total_memory, pages, pagesize);
    }
#  elif defined(MAGICK_PHYSICAL_MEMORY_COMMAND)
    {
      double
        bytes=0;

      FILE
        *command;
      /*
        Execute the external command defined by
        MAGICK_PHYSICAL_MEMORY_COMMAND to obtain the total physical
        memory in bytes.  This external command should be quite speedy
        or else it will impact the time to start GraphicsMagick.
      */
      if ((command = popen(MAGICK_PHYSICAL_MEMORY_COMMAND, "r")) != NULL)
        {
          if (fscanf(command,"%lf",&bytes) == 1)
            total_memory=(bytes/(1024*1024));
          (void) pclose(command);
          (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
            "Total physical memory %ld MB",total_memory);
        }
    }
#  endif

    if (files > 0)
      max_files=files/2;

    if (total_memory > 0)
      max_memory=2*total_memory;

    if (total_memory > 0)
      max_map=8*total_memory;
  }
#endif /* defined(POSIX) */

#if defined(WIN32)
  {
/*     MEMORYSTATUSEX */
/*       stat_ex; */

    long
      total_physical_memory,
      total_virtual_memory;

/*     if (GlobalMemoryStatusEx(&stat_ex)) */
/*     { */
/*       total_physical_memory=(long)(stat_ex.ullTotalPhys/1048576); */
/*       total_virtual_memory=(long)(stat_ex.ullTotalVirtual/1048576); */
/*     } */
/*     else */
/*     { */
      MEMORYSTATUS
        stat;

      GlobalMemoryStatus(&stat);
      total_physical_memory=stat.dwTotalPhys/1048576;
      total_virtual_memory=stat.dwTotalVirtual/1048576;
  /*  }*/

    if (total_virtual_memory > 3*total_physical_memory)
      max_memory=2*total_physical_memory;
    else
      max_memory=(long)(0.7*total_virtual_memory);
    max_map=(long)(8*total_physical_memory);
    (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
      "Total physical memory %ld MB, Total virtual memory %ld MB",
        total_physical_memory, total_virtual_memory);

    /*
      Windows lowio level supports up to 2048 open files.
      Reserve 512 handles for other uses.
    */
    max_files=2048-512;
  }
#endif /* defined(WIN32) */

  /*
    Support using environment variables to set limits
  */
  {
    const char
      *envp;

    if ((envp=getenv("MAGICK_LIMIT_DISK")))
      max_disk=atol(envp);

    if ((envp=getenv("MAGICK_LIMIT_FILES")))
      max_files=atol(envp);

    if ((envp=getenv("MAGICK_LIMIT_MEMORY")))
      max_memory=atol(envp);

    if ((envp=getenv("MAGICK_LIMIT_MAP")))
      max_map=atol(envp);
  }

  SetMagickResourceLimit(DiskResource,max_disk);
  SetMagickResourceLimit(FileResource,max_files);
  SetMagickResourceLimit(MapResource,max_map);
  SetMagickResourceLimit(MemoryResource,max_memory);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e M a g i c k R e s o u r c e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateMagickResource() liberates resources of the specified type.
%
%  The format of the LiberateMagickResource() method is:
%
%      void LiberateMagickResource(const ResourceType type,
%        const magick_int64_t size)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o size: The size of the resource.
%
%
*/
MagickExport void LiberateMagickResource(const ResourceType type,
  const magick_int64_t size)
{
  char
    message[MaxTextExtent];

  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case FileResource:
    {
      resource_info.file-=size;
      FormatString(message,"file -%lu/%lu/%lu",(unsigned long) size,
        (unsigned long) resource_info.file,resource_info.file_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case MemoryResource:
    {
      resource_info.memory-=size;
      FormatString(message,"memory -%lumb/%lumb/%lumb",
        MegabytesToResource(size),MegabytesToResource(resource_info.memory),
        resource_info.memory_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case MapResource:
    {
      resource_info.map-=size;
      FormatString(message,"map -%lumb/%lumb/%lumb",MegabytesToResource(size),
        MegabytesToResource(resource_info.map),resource_info.map_limit);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    case DiskResource:
    {
      resource_info.disk-=size;
      FormatString(message,"disk -%lumb/%lugb/%lugb",MegabytesToResource(size),
        GigabytesToResource(resource_info.disk),resource_info.disk_limit/1024);
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),message);
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k R e s o u r c e I n f o                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagickResourceInfo lists the resource info to a file.
%
%  The format of the ListMagickResourceInfo method is:
%
%      unsigned int ListMagickResourceInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagickResourceInfo(FILE *file,
  ExceptionInfo *exception)
{
  if (file == (const FILE *) NULL)
    file=stdout;
  AcquireSemaphoreInfo(&resource_semaphore);
  (void) fprintf(file,"File    Memory       Map       Disk\n");
  (void) fprintf(file,"-----------------------------------\n");
  (void) fprintf(file,"%4lu  %6lumb  %6lumb  %6lugb\n",
    resource_info.file_limit,resource_info.memory_limit,resource_info.map_limit,
    resource_info.disk_limit/1024);
  (void) fflush(file);
  LiberateSemaphoreInfo(&resource_semaphore);
  return(True);
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k R e s o u r c e L i m i t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetMagickResourceLimit() sets the limit for a particular resource in
%  megabytes.
%
%  The format of the SetMagickResourceLimit() method is:
%
%      void SetMagickResourceLimit(const ResourceType type,
%        const unsigned long limit)
%
%  A description of each parameter follows:
%
%    o type: The type of resource.
%
%    o limit: The maximum limit for the resource.
%
%
*/
MagickExport unsigned int SetMagickResourceLimit(const ResourceType type,
  const unsigned long limit)
{
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case FileResource:
    {
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
        "Setting file open limit to %lu descriptors", limit);
      resource_info.file_limit=limit;
      break;
    }
    case MemoryResource:
    {
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
        "Setting memory allocation limit to %lu MB",
          limit);
      resource_info.memory_limit=limit;
      break;
    }
    case MapResource:
    {
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
        "Setting memory map limit to %lu MB", limit);
      resource_info.map_limit=limit;
      break;
    }
    case DiskResource:
    {
      (void) LogMagickEvent(ResourceEvent,GetMagickModule(),
        "Setting disk file size limit to %lu GB", limit);
      resource_info.disk_limit=limit;
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(True);
}
