//------------------------------------------------------------------------
// ^FILE: fifolist.c - implementation of FIFO linked lists
//
// ^DESCRIPTION:
//    This file implements the classes declared in fifolist.h
//
// ^HISTORY:
//    03/21/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#include "fifolist.h"

#ifndef NULL
# define NULL 0L
#endif

//------------------------------------------------------------- GenericFifoList

   // Destructor
GenericFifoList::~GenericFifoList(void) {
   GenericFifoListNode * nd = head;
   head = NULL;
   while (nd) {
      GenericFifoListNode * to_delete = nd;
      nd = nd->next;
      if (del_items)  delete  to_delete->contents;
      delete  to_delete;
   }
}

   // Add an item to the end
void
GenericFifoList::add(void * item) {
   if (item) {
      GenericFifoListNode * nd = new GenericFifoListNode(NULL, item);
      if (head == NULL) {
         head = tail = nd;
      } else {
         tail->next = nd;
         tail = nd;
      }
      ++num_items;
      mod = 1;
   }
}

   // Remove an item off the front
void *
GenericFifoList::remove(void) {
   if (head == NULL)  return  NULL;
   GenericFifoListNode * nd = head;
   void * result = head->contents;
   head = head->next;
   delete  nd;
   --num_items;
   mod = 1;
   return  result;
}

//--------------------------------------------------------- GenericFifoListIter

GenericFifoListIter::~GenericFifoListIter(void) {}

void *
GenericFifoListIter::operator()(void) {
   void * result = NULL;
   if (current) {
      result  = current->contents;
      current = current->next;
   }
   return  result;
}

//-------------------------------------------------------- GenericFifoListArray

GenericFifoListArray::~GenericFifoListArray(void) {}

void *
GenericFifoListArray::operator[](unsigned  ndx) {
   unsigned  max_index = count();
   if (! max_index--)  return  NULL;     // check for underflow
   if (ndx > max_index)  return  NULL;  // check for overflow

   // if we want the first element -- just return the head
   if (ndx == 0)  return  list.head->contents;

   // if we want the last element -- just return the tail
   if (ndx == max_index)  return  list.tail->contents;

   // If we are going backward or stuff has been modified, then rewind
   if ((ndx < index) || list.modified()) {
      index = 0;
      current = list.head;
   }

   // Skip from current to the desired element
   while (index < ndx) {
      current = current->next;
      ++index;
   }

   return  current->contents;
}

