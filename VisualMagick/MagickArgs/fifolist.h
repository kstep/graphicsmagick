//------------------------------------------------------------------------
// ^FILE: fifolist.h - generic FIFO list classes
//
// ^DESCRIPTION:
//    This file defines a generic FIFO linked list class and two types
//    of iterators for the list.  The first iterator is just your basic
//    run-of-the-mill iterator.  The second iterator treats the list
//    as if it were an array and allows you to index into the list.
//
//    Once these generic classes are declared, the programmer then
//    declares lists (and iterators) that contain a particular type of
//    item.  On systems where your C++ compiler supports templates,
//    templates are used, otherwise we "fake it".
//
//    typedef  FifoList<CmdArgString> StringList;
//    typedef  FifoListIter<CmdArgString>  StringList;
//    typedef  FifoListArray<CmdArgString> StringList;
//
//
// ^HISTORY:
//    03/21/92	Brad Appleton	<bradapp@enteract.com>	Created
//-^^---------------------------------------------------------------------

#ifndef _fifolist_h
#define _fifolist_h


   // GenericFifoList - a FIFO linked list of void * pointers
   //
class  GenericFifoList {
private:

protected:
   // Need to define what a "node" in the list looks like
   struct GenericFifoListNode {
      GenericFifoListNode * next;
      void * contents;

      GenericFifoListNode(GenericFifoListNode * nd =0, void * val =0)
         : next(nd), contents(val) {}
   } ;

   unsigned  mod : 1;
   unsigned  del_items : 1;
   unsigned  num_items ;
   GenericFifoListNode * head;
   GenericFifoListNode * tail;

   GenericFifoList(void)
      : head(0), tail(0), num_items(0), del_items(0), mod(0) {}

   // Remove the first item from the list
   void *
   remove(void);

   // Add an item to the end of the list
   void
   add(void * item);

public:
   virtual  ~GenericFifoList(void);

   // Was the list modified since the last time we checked?
   int
   modified(void) { return (mod) ? (mod = 0, 1) : 0 ; }

   // Is the list empty?
   int
   is_empty(void) const { return  (num_items == 0); }

   // How many items are in the list?
   unsigned
   count(void) const { return  num_items; }

   // Is the list responsible for deleting the items it contains?
   int
   self_cleaning(void) const { return  int(del_items); }

   // Tell the list who is responsible for deleting the items it contains?
   void
   self_cleaning(int  bool_val)  { del_items = (bool_val) ? 1 : 0; }

   friend class GenericFifoListIter;
   friend class GenericFifoListArray;
} ;


   // GenericFifoListIter -- an iterator for a GenericFifoList
class  GenericFifoListIter {
private:
   GenericFifoList::GenericFifoListNode * current;

protected:
   GenericFifoListIter(GenericFifoList & fifo_list)
      : current(fifo_list.head) {}

   GenericFifoListIter(GenericFifoList * fifo_list)
      : current(fifo_list->head) {}

   // Return the current item in the list and advance to the next item.
   // returns NULL if at end-of-list
   //
   void *
   operator()(void);

public:
   virtual  ~GenericFifoListIter(void);

} ;


   // GenericFifoListArray -- an array-style iterator for a GenericFifoList
class  GenericFifoListArray {
private:
   GenericFifoList &  list;
   unsigned           index;
   GenericFifoList::GenericFifoListNode * current;

protected:
   GenericFifoListArray(GenericFifoList & fifo_list)
      : list(fifo_list), index(0), current(fifo_list.head) {}

   GenericFifoListArray(GenericFifoList * fifo_list)
      : list(*fifo_list), index(0), current(fifo_list->head) {}

   // How many items are in the array?
   unsigned  count(void) const  { return  list.count(); }

   // Return a specified item in the array.
   //   NOTE: the programmer is responsible for making sure the given index
   //         is not out of range. For this base class, NULL is returned
   //         when the index is out of range. Derived classes however
   //         dereference the value returned by this function so using
   //         an out-of-range index in one of the derived classes will
   //         cause a NULL pointer dereferencing error!
   //
   void *
   operator[](unsigned  ndx);

public:
   virtual  ~GenericFifoListArray(void);

} ;

template <class Type>
class FifoList : public GenericFifoList {
public:
   FifoList(void) {}

   virtual ~FifoList(void);

   void
   add(Type * item)  { GenericFifoList::add((void *)item); }

   Type *
   remove(void)  { return  (Type *) GenericFifoList::remove(); }
} ;

template <class Type>
class FifoListIter : public GenericFifoListIter {
public:
   FifoListIter(FifoList<Type> & list) : GenericFifoListIter(list) {}
   FifoListIter(FifoList<Type> * list) : GenericFifoListIter(list) {}

   virtual  ~FifoListIter(void);

   Type *
   operator()(void)  {  return  (Type *) GenericFifoListIter::operator()(); }
} ;

template <class Type>
class FifoListArray : public GenericFifoListArray {
public:
   FifoListArray(FifoList<Type> & list) : GenericFifoListArray(list) {}
   FifoListArray(FifoList<Type> * list) : GenericFifoListArray(list) {}

   virtual  ~FifoListArray(void);

   Type &
   operator[](unsigned  ndx)
      { return  *((Type *) GenericFifoListArray::operator[](ndx)); }
} ;


//-------------------------------------------------------------------- FifoList

   // Destructor
template <class Type>
FifoList<Type>::~FifoList(void) {
   GenericFifoListNode * nd = head;
   head = NULL;
   while (nd) {
      GenericFifoListNode * to_delete = nd;
      nd = nd->next;
      if (del_items)  delete (Type *)to_delete->contents;
      delete  to_delete;
   }
}

template <class Type>
FifoListIter<Type>::~FifoListIter(void) {}

template <class Type>
FifoListArray<Type>::~FifoListArray(void) {}

#endif /* _fifolist_h */

