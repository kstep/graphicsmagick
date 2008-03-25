/*
% Copyright (C) 2003 - 2008 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                    M   M  EEEEE  M   M   OOO   RRRR   Y   Y                 %
%                    MM MM  E      MM MM  O   O  R   R   Y Y                  %
%                    M M M  EEE    M M M  O   O  RRRR     Y                   %
%                    M   M  E      M   M  O   O  R R      Y                   %
%                    M   M  EEEEE  M   M   OOO   R  R     Y                   %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Memory Allocation Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1998                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k A c q u i r e M e m o r y                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAcquireMemory() returns a pointer to a block of memory of at least
%  size bytes suitably aligned for any use.  NULL is returned if insufficient
%  memory is available or the requested size is zero.
%
%  The format of the  MagickAcquireMemory method is:
%
%      void * MagickAcquireMemory(const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void * MagickAcquireMemory(const size_t size)
{
  if (size == 0)
    return ((void *) NULL);

  return (MagickAllocateMemory(void *,size));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k A c q u i r e M e m o r y A r r a y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickAcquireMemoryArray() returns a pointer to a block of memory of
%  sufficient size to support an array of elements of a specified size.
%  The returned memory is suitably aligned for any use.  NULL is returned
%  if the required memory exceeds the range of size_t, the specified size
%  is zero, or there is insufficient memory available.
%
%  The format of the MagickAcquireMemoryArray method is:
%
%      void *MagickAcquireMemoryArray(const size_t count,
%                                     const size_t size);
%
%  A description of each parameter follows:
%
%    o count: The number of elements in the array.
%
%    o size: The size of one array element.
%
*/
MagickExport void *MagickAcquireMemoryArray(const size_t count,const size_t size)
{
  size_t
    allocation_size;

  void
    *allocation;

  allocation = 0;
  allocation_size = size * count;
  if ((count != 0) && (size != allocation_size/count))
    allocation_size = 0;
  if (allocation_size)
    allocation = malloc(allocation_size);
  return allocation;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C l o n e M e m o r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCloneMemory() copies size bytes from memory area source to the
%  destination.  Copying between objects that overlap will take place
%  correctly.  It returns destination.
%
%  The format of the MagickCloneMemory method is:
%
%      void *MagickCloneMemory(void *destination,const void *source,const size_t size)
%
%  A description of each parameter follows:
%
%    o size: The size of the memory in bytes to allocate.
%
%
*/
MagickExport void *MagickCloneMemory(void *destination,const void *source,
  const size_t size)
{
  unsigned char
    *d=(unsigned char*) destination;

  const unsigned char
    *s=(const unsigned char*) source;

  assert(destination != (void *) NULL);
  assert(source != (const void *) NULL);

  if (((d+size) < s) || (d > (s+size)))
    return(memcpy(destination,source,size));

  return(memmove(destination,source,size));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R e a l l o c a t e M e m o r y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReallocateMemory() changes the size of the memory and returns a
%  pointer to the (possibly moved) block.  The contents will be unchanged
%  up to the lesser of the new and old sizes.  If size is zero, then the
%  memory is freed and a NULL value is returned.  If the memory allocation
%  fails, then the existing memory is freed, and a NULL value is returned.
%
%  The format of the MagickReallocateMemory method is:
%
%      void *MagickReallocateMemory(void *memory,const size_t size)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a memory allocation.
%
%    o size: The new size of the allocated memory.
%
*/
MagickExport void *MagickReallocateMemory(void *memory,const size_t size)
{
  void
    *new_memory;

  assert(memory != (void *) NULL);

  new_memory = realloc(memory,size);
  if ((new_memory == 0) && (memory != 0) && (size != 0))
    free(memory);

  return new_memory;;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a g i c k R e l e a s e M e m o r y                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickReleaseMemory() frees memory that has already been allocated.
%
%  The format of the MagickReleaseMemory method is:
%
%      void MagickReleaseMemory(void *memory)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a block of memory to free for reuse.
%
%
*/
MagickExport void MagickReleaseMemory(void *memory)
{
  MagickFreeMemory(memory);
}
