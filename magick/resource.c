/*
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
%                      Get/Set GraphicsMagick Resources.                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
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
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "resource.h"
#include "utility.h"

/*
  Define  declarations.
*/
#define ResourceInfinity  (~0UL)

/*
  Typedef declarations.
*/
typedef struct _ResourceInfo
{
  long double
    memory,
    map,
    disk;

  unsigned long
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
    0, 0, 0, 512, 1024, ResourceInfinity
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
%        const ExtendedSignedIntegralType size)
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
  const ExtendedSignedIntegralType size)
{
  unsigned int
    status;

  status=True;
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      resource_info.memory+=(double) size;
      if (resource_info.memory_limit == ResourceInfinity)
        break;
      status=resource_info.memory <=
        (long double) resource_info.memory_limit*1024.0*1024.0;
      break;
    }
    case MapResource:
    {
      resource_info.map+=(double) size;
      if (resource_info.map_limit == ResourceInfinity)
        break;
      status=resource_info.disk <=
        (long double) resource_info.map_limit*1024.0*1024.0;
      break;
    }
    case DiskResource:
    {
      resource_info.disk+=(double) size;
      if (resource_info.disk_limit == ResourceInfinity)
        break;
      status=resource_info.disk <=
        (long double) resource_info.disk_limit*1024.0*1024.0;
      break;
    }
    default:
    {
      status=False;
      break;
    }
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
  AcquireSemaphoreInfo(&resource_semaphore);
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
%
*/
MagickExport unsigned long GetMagickResource(const ResourceType type)
{
  unsigned long
    resource;

  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      resource=(unsigned long) (resource_info.memory/1024.0/1024.0+0.5);
      break;
    }
    case MapResource:
    {
      resource=(unsigned long) (resource_info.map/1024.0/1024.0+0.5);
      break;
    }
    case DiskResource:
    {
      resource=(unsigned long) (resource_info.disk/1024.0/1024.0+0.5);
      break;
    }
    default:
    {
      resource=0UL;
      break;
    }
  }
  LiberateSemaphoreInfo(&resource_semaphore);
  return(resource);
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
%        const ExtendedSignedIntegralType size)
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
  const ExtendedSignedIntegralType size)
{
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      resource_info.memory-=size;
      break;
    }
    case MapResource:
    {
      resource_info.map-=size;
      break;
    }
    case DiskResource:
    {
      resource_info.disk-=size;
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
%    o limit: The maximum limit for the resource (in megabytes).
%
%
*/
MagickExport void SetMagickResourceLimit(const ResourceType type,
  const unsigned long limit)
{
  AcquireSemaphoreInfo(&resource_semaphore);
  switch (type)
  {
    case MemoryResource:
    {
      resource_info.memory_limit=limit;
      break;
    }
    case MapResource:
    {
      resource_info.map_limit=limit;
      break;
    }
    case DiskResource:
    {
      resource_info.disk_limit=limit;
      break;
    }
    default:
      break;
  }
  LiberateSemaphoreInfo(&resource_semaphore);
}
