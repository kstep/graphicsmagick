/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                           %
%                                                                           %
%                                                                           %
%           RRRR    EEEEE    GGG   IIIII  SSSSS  TTTTT  RRRR   Y   Y        %
%           R   R   E       G        I    SS       T    R   R   Y Y         %
%           RRRR    EEE     G GGG    I     SSS     T    RRRR     Y          %
%           R R     E       G   G    I       SS    T    R R      Y          %
%           R  R    EEEEE    GGG   IIIII  SSSSS    T    R  R     Y          %
%                                                                           %
%                                                                           %
%                            ImageMagick Registry.                          %
%                                                                           %
%                                                                           %
%                              Software Design                              %
%                                John Cristy                                %
%                                 July 1992                                 %
%                                                                           %
%                                                                           %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                   %
%                                                                           %
%  Permission is hereby granted, free of charge, to any person obtaining a  %
%  copy of this software and associated documentation files ("ImageMagick"),%
%  to deal in ImageMagick without restriction, including without limitation %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense, %
%  and/or sell copies of ImageMagick, and to permit persons to whom the     %
%  ImageMagick is furnished to do so, subject to the following conditions:  %
%                                                                           %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                       %
%                                                                           %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability, %
%  fitness for a particular purpose and noninfringement.  In no event shall %
%  ImageMagick Studio be liable for any claim, damages or other liability,  %
%  whether in an action of contract, tort or otherwise, arising from, out of%
%  or in connection with ImageMagick or the use or other dealings in        %
%  ImageMagick.                                                             %
%                                                                           %
%  Except as contained in this notice, the name of the ImageMagick Studio   %
%  shall not be used in advertising or otherwise to promote the sale, use or%
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                      %
%                                                                           %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Global declarations.
*/
static long
  id = 0;

static RegistryInfo
  *registry_list = (RegistryInfo *) NULL;

static SemaphoreInfo
  *registry_semaphore = (SemaphoreInfo *) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e M a g i c k R e g i s t r y                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteMagickRegistry() deletes an entry in the registry as defined by the id.
%  It returns True if the entry is deleted otherwise False if no entry is found
%  in the registry that matches the id.
%
%  The format of the DeleteMagickRegistry method is:
%
%      unsigned int void DeleteMagickRegistry(const long id)
%
%  A description of each parameter follows:
%
%    o id: The registry id.
%
%
*/
MagickExport unsigned int DeleteMagickRegistry(const long id)
{
  RegistryInfo
    *registry_info;

  register RegistryInfo
    *p;

  unsigned int
    delete;

  delete=False;
  AcquireSemaphoreInfo(&registry_semaphore);
  for (p=registry_list; p != (RegistryInfo *) NULL; p=p->next)
  {
    if (p->id == id)
      {
        registry_info=p;
        switch (p->type)
        {
          case ImageRegistryType:
          {
            DestroyImage((Image *) registry_info->blob);
            break;
          }
          default:
          {
            LiberateMemory((void **) &registry_info->blob);
            break;
          }
        }
        if (registry_info->previous != (RegistryInfo *) NULL)
          registry_info->previous->next=registry_info->next;
        if (registry_info->next != (RegistryInfo *) NULL)
          registry_info->next->previous=registry_info->previous;
        LiberateMemory((void **) &registry_info);
        registry_info=(RegistryInfo *) NULL;
        delete=True;
        break;
      }
  }
  DestroySemaphoreInfo(registry_semaphore);
  return(delete);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k R e g i s t r y                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickRegistry() deallocates memory associated the magick registry.
%
%  The format of the DestroyMagickRegistry method is:
%
%       void DestroyMagickInfo(void)
%
%
*/
MagickExport void DestroyMagickRegistry(void)
{
  RegistryInfo
    *registry_info;

  register RegistryInfo
    *p;

  AcquireSemaphoreInfo(&registry_semaphore);
  for (p=registry_list; p != (RegistryInfo *) NULL; )
  {
    switch (p->type)
    {
      case ImageRegistryType:
      {
        DestroyImage((Image *) p->blob);
        break;
      }
      default:
      {
        LiberateMemory((void **) &p->blob);
        break;
      }
    }
    registry_info=p;
    p=p->next;
    LiberateMemory((void **) &registry_info);
  }
  registry_list=(RegistryInfo *) NULL;
  DestroySemaphoreInfo(registry_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k R e g i s t r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickRegistry() gets a blob from the registry as defined by the id.  If
%  the blob that matches the id is not found, NULL is returned.
%
%  The format of the getMagickRegistry method is:
%
%      const void *GetMagickRegistry(const long id,RegistryType *type,
%        size_t *length)
%
%  A description of each parameter follows:
%
%    o id: The registry id.
%
%    o type: The registry type.
%
%    o length: The blob length in number of bytes.
%
%
*/
MagickExport const void *GetMagickRegistry(const long id,RegistryType *type,
  size_t *length)
{
  RegistryInfo
    *registry_info;

  register RegistryInfo
    *p;

  registry_info=(RegistryInfo *) NULL;
  AcquireSemaphoreInfo(&registry_semaphore);
  for (p=registry_list; p != (RegistryInfo *) NULL; )
  {
    if (p->id == id)
      {
        registry_info=p;
        break;
      }
  }
  DestroySemaphoreInfo(registry_semaphore);
  if (registry_info == (RegistryInfo *) NULL)
    return((RegistryInfo *) NULL);
  *type=registry_info->type;
  *length=registry_info->length;
  return(registry_info->blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k R e g i s t r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetMagickRegistry() sets a blob into the registry and returns a unique ID.
%  If an error occurs, an ID of -1 is returned.
%
%  The format of the SetMagickRegistry method is:
%
%      long SetMagickRegistry(const void *blob,const size_t length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o type: The registry type.
%
%    o blob: The address of a Binary Large OBject.
%
%    o length: The blob length in number of bytes.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport long SetMagickRegistry(const RegistryType type,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  RegistryInfo
    *registry_info;

  registry_info=(RegistryInfo *) AcquireMemory(sizeof(RegistryInfo));
  if (registry_info == (RegistryInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate registry",
      "Memory allocation failed");
  (void) memset(registry_info,0,sizeof(RegistryInfo));
  switch (type)
  {
    case ImageRegistryType:
    {
      Image
        *image;

      image=(Image *) blob;
      if (image->signature != MagickSignature)
        {
          ThrowException(exception,RegistryWarning,"Unable set registry",
            "Image expected");
          return(-1);
        }
      registry_info->blob=(void *) CloneImage(image,0,0,True,exception);
      if (registry_info->blob == (void *) NULL)
        return(-1);
      break;
    }
    default:
    {
      registry_info->blob=(void *) AcquireMemory(length);
      if (registry_info->blob == (void *) NULL)
        return(-1);
      (void) memcpy(registry_info->blob,blob,length);
    }
  }
  registry_info->length=length;
  registry_info->type=type;
  AcquireSemaphoreInfo(&registry_semaphore);
  registry_info->id=id++;
  if (registry_list == (RegistryInfo *) NULL)
    registry_list=registry_info;
  else
    {
      register RegistryInfo
        *p;

      for (p=registry_list; p->next != (RegistryInfo *) NULL; p=p->next);
      registry_info->previous=p;
      p->next=registry_info;
    }
  DestroySemaphoreInfo(registry_semaphore);
  return(registry_info->id);
}
