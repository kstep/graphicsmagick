/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

 Test MagickMap key,value map functionality

 Written by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
 
*/

#if !defined(_VISUALC_)
#include <magick_config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#if defined(_VISUALC_)
#include <stdlib.h>
#include <sys\types.h>
#endif
#include <time.h>
#include <magick/api.h>
#include <magick/map.h>

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


  GetExceptionInfo(&exception);
  map=MagickMapAllocateMap(MagickMapCopyString,MagickMapDeallocateString);

  printf("Adding map entries ...\n");
  for (i=0; KeyVal[i].key; i++)
    {
      MagickMapAddEntry(map,KeyVal[i].key,(void *)KeyVal[i].value,0,&exception);
    }


  {
    char
      str_index[MaxTextExtent];

    printf("Keyed access ...\n");
    for (i=0; KeyVal[i].key; i++)
      {
        FormatString(str_index,"%u",i);
        printf("key=\"%s\" value=\"%s\"\n", str_index,
               (char *)MagickMapAccessEntry(map,str_index,0));
      }
  }

  iterator=MagickMapAllocateIterator(map);
  printf("Iterate forward ...\n");
  while(MagickMapIterateNext(iterator,&key))
    printf("key=%s value=%s\n",key,
      (char *)MagickMapDereferenceIterator(iterator,0));

  printf("Iterate reverse ...\n");
  while(MagickMapIteratePrevious(iterator,&key))
    printf("key=%s value=%s\n",key,
      (char *)MagickMapDereferenceIterator(iterator,0));

  i=2;
  printf("Remove entry for key \"%s\" and then iterate forward ...\n",
    KeyVal[i].key);
  MagickMapRemoveEntry(map,KeyVal[i].key);
  while(MagickMapIterateNext(iterator,&key))
    printf("key=%s value=%s\n",key,
      (char *)MagickMapDereferenceIterator(iterator,0));

  printf("Iterate reverse ...\n");
  while(MagickMapIteratePrevious(iterator,&key))
    printf("key=%s value=%s\n",key,
      (char *)MagickMapDereferenceIterator(iterator,0));

  MagickMapDeallocateIterator(iterator);

  i=2;
  MagickMapAddEntry(map,KeyVal[i].key,(void *)KeyVal[i].value,0,&exception);
  printf("Add entry for key \"%s\" and then iterate forward ...\n",
         KeyVal[i].key);

  iterator=MagickMapAllocateIterator(map);  
  while(MagickMapIterateNext(iterator,&key))
    printf("key=%s value=%s\n",key,
           (char *)MagickMapDereferenceIterator(iterator,0));

  MagickMapDeallocateIterator(iterator);
  MagickMapDeallocateMap(map);

  DestroyExceptionInfo(&exception);
  return exit_status;
}
