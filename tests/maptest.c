/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

 Test MagickMap key,value map functionality

 Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 
*/

#include <magick/api.h>
#include <magick/map.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _KeyValMap
{
  char *key;
  char *value;
} KeyValMap;


int main ( int argc, char **argv )
{
  int
    exit_status = 0,
    i;

  ExceptionInfo
    exception;

  MagickMap
    map;

  MagickMapIterator
    iterator;

  const char
    *key;

  static const KeyValMap
    KeyVal[] =
    {
      { "0", "number 0" },
      { "1", "number 1" },
      { "2", "number 2" },
      { "3", "number 3" },
      { "4", "number 4" },
      { "5", "number 5" },
      { "6", "number 6" },
      { "7", "number 7" },
      { "8", "number 8" },
      { "9", "number 9" },
      { 0, 0 }
    };

  (void) argc;
  (void) argv;

  GetExceptionInfo(&exception);
  map=MagickMapAllocateMap(MagickMapCopyString,MagickMapDeallocateString);

  (void) printf("Adding map entries ...\n");
  for (i=0; KeyVal[i].key; i++)
    {
      (void) MagickMapAddEntry(map,KeyVal[i].key,(void *)KeyVal[i].value,0,&exception);
    }


  {
    char
      str_index[MaxTextExtent];

    (void) printf("Keyed access ...\n");
    for (i=0; KeyVal[i].key; i++)
      {
        FormatString(str_index,"%u",i);
        (void) printf("key=\"%s\" value=\"%s\"\n", str_index,
                      (char *)MagickMapAccessEntry(map,str_index,0));
      }
  }

  iterator=MagickMapAllocateIterator(map);
  (void) printf("Iterate forward ...\n");
  while(MagickMapIterateNext(iterator,&key))
    (void) printf("key=%s value=%s\n",key,
                  (char *) MagickMapDereferenceIterator(iterator,0));

  (void) printf("Iterate reverse ...\n");
  while(MagickMapIteratePrevious(iterator,&key))
    (void) printf("key=%s value=%s\n",key,
                  (char *) MagickMapDereferenceIterator(iterator,0));

  i=2;
  (void) printf("Remove entry for key \"%s\" and then iterate forward ...\n",
                KeyVal[i].key);
  (void) MagickMapRemoveEntry(map,KeyVal[i].key);
  while(MagickMapIterateNext(iterator,&key))
    (void) printf("key=%s value=%s\n",key,
                  (char *)MagickMapDereferenceIterator(iterator,0));

  (void) printf("Iterate reverse ...\n");
  while(MagickMapIteratePrevious(iterator,&key))
    (void) printf("key=%s value=%s\n",key,
                  (char *)MagickMapDereferenceIterator(iterator,0));

  MagickMapDeallocateIterator(iterator);

  i=2;
  (void) MagickMapAddEntry(map,KeyVal[i].key,(void *)KeyVal[i].value,0,&exception);
  (void) printf("Add entry for key \"%s\" and then iterate forward ...\n",
                KeyVal[i].key);

  iterator=MagickMapAllocateIterator(map);  
  while(MagickMapIterateNext(iterator,&key))
    (void) printf("key=%s value=%s\n",key,
                  (char *)MagickMapDereferenceIterator(iterator,0));

  MagickMapDeallocateIterator(iterator);
  MagickMapDeallocateMap(map);

  DestroyExceptionInfo(&exception);
  return exit_status;
}
