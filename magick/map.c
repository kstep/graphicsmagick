/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Key,value associative map interface
% Created by Bob Friesenhahn, September 2003
%
*/

#include "magick/studio.h"
#include "magick/map.h"
#include "magick/utility.h"
#include "magick/semaphore.h"


/*
  Structure to represent a contained object
*/
typedef struct _MagickMapObject
{
  /* Object key value */
  char
   *key;

  /* Pointer to stored object */
  void
    *object;

  /* Object size. Optional.  May be set to zero depending on needs of
     object type */
  size_t
    object_size;

  /* Function which clones (copies) contained objects */
  MagickMapObjectClone
    clone_function;

  /* Function which deallocates contained object */
  MagickMapObjectDeallocator
    deallocate_function;

  long
    reference_count;

  struct _MagickMapObject
    *previous,
    *next;
} MagickMapObject;

/*
  Structure to act as the user handle to a key,value map
*/
typedef struct _MagickMapHandle
{
  /* Function which clones (copies) contained objects */
  MagickMapObjectClone
    clone_function;

  /* Function which deallocates contained object */
  MagickMapObjectDeallocator
    deallocate_function;

  /* Global semaphore for all operations pertaining to this map */
  SemaphoreInfo
    *semaphore;

  /* Reference count */
  long
    reference_count;

  MagickMapObject
    *list;
} MagickMapHandle;

typedef enum
{
  InListPosition,
  FrontPosition,
  BackPosition
} MagickMapIteratorPosition;

/*
  Structure to act as a map iterator
*/
typedef struct _MagickMapIteratorHandle
{
  /* Pointer to base map */
  MagickMap
    map;

  /* Pointer to member of map */
  const MagickMapObject
    *member;

  /* Iterator position */
  MagickMapIteratorPosition
    position;
} MagickMapIteratorHandle;

/*
  Allocate a MagickMapObject
*/
static MagickMapObject *
MagickMapAllocateObject(const char *key, const void *object,
  const size_t object_size, MagickMapObjectClone clone,
  MagickMapObjectDeallocator deallocate)
{
  MagickMapObject *
    map_object;

  assert(key != 0);
  assert(object != 0);
  assert(clone != 0);
  assert(deallocate != 0);

  map_object=MagickAllocateMemory(MagickMapObject *,sizeof(MagickMapObject));
  if (map_object)
    {
      map_object->key=AcquireString(key);
      map_object->object=(clone)(object,object_size);
      map_object->object_size=object_size;
      map_object->clone_function=clone;
      map_object->deallocate_function=deallocate;
      map_object->reference_count=0;
      map_object->previous=0;
      map_object->next=0;
    }

  return map_object;
}

/*
  Destroy a MagickMapObject
*/
static void
MagickMapDestroyObject(MagickMapObject *object)
{
  assert(object != 0);
  assert(object->reference_count == 0);

  MagickFreeMemory(object->key);
  (object->deallocate_function)(object->object);

  memset((void *)object,0xbf,sizeof(MagickMapObject));
}

/*
  Access an entry
  Object_size is updated with the object size.
  It is acceptable to pass a null object_size argument if the size is not needed.
*/
MagickExport const void *
MagickMapAccessEntry(MagickMap map,const char *key,
  size_t *object_size)
{
  MagickMapObject
    *p;

  assert(map != 0);
  assert(key != 0);

  if (object_size)
    *object_size=0;

  LockSemaphoreInfo(map->semaphore);

  for (p=map->list; p != 0; p=p->next)
    if (LocaleCompare(key,p->key) == 0)
      {
        if (object_size)
          *object_size=p->object_size;
        UnlockSemaphoreInfo(map->semaphore);
        return(p->object);
      }

  UnlockSemaphoreInfo(map->semaphore);

  return 0;
}

/*
  Add an entry to the Magick Map.
  Argument is copied into the map.
  It is acceptable to pass object_size=0 argument if the size is not needed.
*/
MagickExport void
MagickMapAddEntry(MagickMap map,const char *key, const void *object,
  const size_t object_size)
{
  MagickMapObject
    *new_object;

  assert(map != 0);
  assert(key != 0);
  assert(object != 0);

  new_object=MagickMapAllocateObject(key, object, object_size,
                                     map->clone_function,
                                     map->deallocate_function);

  LockSemaphoreInfo(map->semaphore);

  if (!map->list)
    {
      /*
        If list is empty, object becomes new list.
      */
      map->list=new_object;
    }
  else
    {
      MagickMapObject
        *last_object=0,
        *p;

      /*
        Search for, and replace any existing object with
        same key.
      */

      for (p=map->list; p != 0; p=p->next)
        {
          last_object=p;
          if (LocaleCompare(key,p->key) == 0)
            {
              /*
                Splice in new object
              */
              new_object->previous=p->previous;
              new_object->next=p->next;
              if (new_object->previous)
                new_object->previous->next=new_object;
              if (new_object->next)
                new_object->next->previous=new_object;
              if (map->list == p)
                map->list=new_object;
              p->previous=0;
              p->next=0;
              
              /*
                Remove old object
              */
              MagickMapDestroyObject(p);
              break;
            }
        }

      if (p == 0)
        {
          /*
            No existing matching object was found.  The last_object
            pointer points to the last object in the list.  Append new
            object to end of list.
          */
          new_object->previous=last_object;
          last_object->next=new_object;
        }
    }

  UnlockSemaphoreInfo(map->semaphore);

  return;
}

/*
  Allocate a Magick Map
  All objects contained in the map need to be of the same type.
  Copy specifies a function to copy objects into the map.
  Deallocate specifies a function to destroy a contained object.
*/
MagickExport MagickMap
MagickMapAllocateMap(MagickMapObjectClone clone,
  MagickMapObjectDeallocator deallocate)
{
  MagickMap
    map;

  assert(clone != 0);
  assert(deallocate != 0);

  map=MagickAllocateMemory(MagickMap,sizeof(MagickMapHandle));
  if (map)
    {
      map->clone_function=clone;
      map->deallocate_function=deallocate;
      map->semaphore=AllocateSemaphoreInfo();
      map->reference_count=1;
      map->list=0;
    }
  return map;
}

/*
  Deep clone a Magick Map
  All objects contained in the map are cloned and inserted into
  a new map which is returned.
*/
MagickExport MagickMap
MagickMapCloneMap(MagickMap map)
{
  MagickMap map_clone;
  MagickMapIterator iterator;
  size_t size;
  const char *key;

  assert(map != 0);
  LockSemaphoreInfo(map->semaphore);

  map_clone=MagickMapAllocateMap(map->clone_function,map->deallocate_function);
  iterator=MagickMapAllocateIterator(map);
  while(MagickMapIterateNext(iterator,&key))
  {
    const void *object=MagickMapDereferenceIterator(iterator,&size);
    /* Add clones key and object on insertion */
    MagickMapAddEntry(map_clone,key,object,size);
  }
  MagickMapDeallocateIterator(iterator);

  UnlockSemaphoreInfo(map->semaphore);
  return map_clone;
}

/*
  Deallocate (clear) all objects in the map.
*/
MagickExport void
MagickMapClearMap(MagickMap map)
{
  assert(map != 0);

  LockSemaphoreInfo(map->semaphore);
  if (map->list)
  {
    register MagickMapObject
      *current,
      *p;

    for (p=map->list; p != 0; )
      {
        current=p;
        p=p->next;
        MagickMapDestroyObject(current);
      }
    map->list=0;
  }
  UnlockSemaphoreInfo(map->semaphore);
}

/*
  Allocate an iterator
*/
MagickExport MagickMapIterator
MagickMapAllocateIterator(MagickMap map)
{
  MagickMapIterator
    iterator;

  assert(map != 0);

  if (LockSemaphoreInfo(map->semaphore) == False)
    return 0;

  iterator=MagickAllocateMemory(MagickMapIterator,
                                sizeof(MagickMapIteratorHandle));
  if (iterator)
    {
      iterator->map=map;
      iterator->member=0;
      iterator->position=FrontPosition;
      iterator->map->reference_count++;
    }

  UnlockSemaphoreInfo(map->semaphore);

  return iterator;
}

/*
  Destroy a Magick Map (and any contained objects)
*/
MagickExport void
MagickMapDeallocateMap(MagickMap map)
{
  assert(map != 0);

  LockSemaphoreInfo(map->semaphore);

  map->reference_count--;

  /* For the moment, do not allow outstanding references */
  assert(map->reference_count == 0);

  if (map->list)
  {
    register MagickMapObject
      *current,
      *p;

    for (p=map->list; p != 0; )
      {
        current=p;
        p=p->next;
        MagickMapDestroyObject(current);
      }        
  }

  UnlockSemaphoreInfo(map->semaphore);

  memset((void *)map,0xbf,sizeof(MagickMapHandle));
  MagickFreeMemory(map);
}

/*
  Deallocate an iterator
*/
MagickExport void
MagickMapDeallocateIterator(MagickMapIterator iterator)
{
  assert(iterator != 0);

  LockSemaphoreInfo(iterator->map->semaphore);

  iterator->map->reference_count--;

  UnlockSemaphoreInfo(iterator->map->semaphore);

  memset((void *)iterator,0xbf,sizeof(MagickMapIteratorHandle));
  MagickFreeMemory(iterator);
}

/*
  Access an element via the iterator
  Object_size is updated with the object size.
*/
MagickExport const void *
MagickMapDereferenceIterator(const MagickMapIterator iterator,
    size_t *object_size)
{
  const void
    *value=0;

  assert(iterator != 0);
  assert(iterator->member != 0);

  if (object_size)
    *object_size=0;

  if (iterator->member)
    {
      value=iterator->member->object;
      if (object_size)
        *object_size=iterator->member->object_size;
    }

  return value;
}

/*
  Position the iterator to one-beyond the last element
  The MagickMapIteratePrevious() function must be executed once
  to access the last element.
*/
MagickExport void
MagickMapIterateToBack(MagickMapIterator iterator)
{
  assert(iterator != 0);

  iterator->member=0;
  iterator->position=BackPosition;
}

/*
  Position the iterator to one-before the first element (default
  position). The MagickMapIterateNext() function must be executed
  once to access the first element.
*/
MagickExport void
MagickMapIterateToFront(MagickMapIterator iterator)
{
  assert(iterator != 0);

  iterator->member=0;
  iterator->position=FrontPosition;
}

/*
  Iterate to next element
  Key is updated with pointer to element key
  True is returned if not pointing to end of list.
*/
MagickExport unsigned int
MagickMapIterateNext(MagickMapIterator iterator,const char **key)
{
  assert(iterator != 0);
  assert(key != 0);

  if (LockSemaphoreInfo(iterator->map->semaphore) != True)
    return False;

  switch (iterator->position)
    {
    case FrontPosition:
      iterator->member=iterator->map->list;
      if (iterator->member)
        iterator->position=InListPosition;
      break;
    case InListPosition:
      assert(iterator->member != 0);
      iterator->member=iterator->member->next;
      if (!iterator->member)
        iterator->position=BackPosition;
      break;
    case BackPosition:
      break;
    }

  if (iterator->member)
    *key=iterator->member->key;

  UnlockSemaphoreInfo(iterator->map->semaphore);
  
  return (iterator->member != 0);
}

/*
  Iterate to previous element
  Key is updated with pointer to element key
  True is returned if not pointing to beginning of list.
*/
MagickExport unsigned int
MagickMapIteratePrevious(MagickMapIterator iterator,const char **key)
{
  assert(iterator != 0);
  assert(key != 0);

  if(LockSemaphoreInfo(iterator->map->semaphore) != True)
    return False;

  switch (iterator->position)
    {
    case FrontPosition:
      break;
    case InListPosition:
      assert(iterator->member != 0);
      iterator->member=iterator->member->previous;
      if (!iterator->member)
        iterator->position=FrontPosition;
      break;
    case BackPosition:
      {
        for (iterator->member=iterator->map->list;
             (iterator->member != 0) && (iterator->member->next != 0);
             iterator->member=iterator->member->next);
        if (iterator->member)
          iterator->position=InListPosition;
      }
      break;
    }

  if (iterator->member)
    *key=iterator->member->key;

  UnlockSemaphoreInfo(iterator->map->semaphore);

  return (iterator->member != 0);
}

/*
  Remove (destroy) an entry from the Magick Map. False is returned if
  the object doesn't exist.
*/
MagickExport unsigned int
MagickMapRemoveEntry(MagickMap map,const char *key)
{
  unsigned int
    status = False;

  assert(map != 0);
  assert(key != 0);

  if(LockSemaphoreInfo(map->semaphore) != True)
    return False;

  if (map->list)
    {
      MagickMapObject
        *p;

      for (p=map->list; p != 0; p=p->next)
        {
          if (LocaleCompare(key,p->key) == 0)
            {
              if (p->previous)
                p->previous->next=p->next;

              if (p->next)
                p->next->previous=p->previous;

              MagickMapDestroyObject(p);
              status=True;
              break;
            }
        }
    }

  UnlockSemaphoreInfo(map->semaphore);

  return status;
}

/*
  Function to copy a C string
*/
MagickExport void *
MagickMapCopyString(const void *string, const size_t size)
{
  return (void *) AllocateString((const char *)string);
}

/*
  Function to deallocate a C string
*/
extern MagickExport void
MagickMapDeallocateString(void *string)
{
  MagickFreeMemory(string);
}

/*
  Function to copy a BLOB
*/
MagickExport void *
MagickMapCopyBlob(const void *blob, const size_t size)
{
  void
    *memory;

  memory=MagickAllocateMemory(void *,size);
  if (memory)
    memcpy(memory,blob,size);

  return (memory);
}

/*
  Function to deallocate a BLOB
*/
extern MagickExport void
MagickMapDeallocateBlob(void *blob)
{
  MagickFreeMemory(blob);
}
