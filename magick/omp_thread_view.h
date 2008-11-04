/*
  Copyright (C) 2008 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick OpenMP Cache Thread View Methods.
*/
#ifndef _MAGICK_OMP_THREAD_VIEW
#define _MAGICK_OMP_THREAD_VIEW

#include "magick/pixel_cache.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /****
   *
   * Simplified support for OpenMP thread view sets.
   * These interfaces are subject to change.
   *
   ****/

  typedef struct _ThreadViewSet
  { 
    ViewInfo
    *views;
    
    unsigned int
    nviews;
  } ThreadViewSet;


  /*
    Destroy a thread view set.
  */
  extern MagickExport void
  DestroyThreadViewSet(ThreadViewSet *view_set);

  /*
    Allocate a thread view set.
  */
  extern MagickExport ThreadViewSet
  *AllocateThreadViewSet(Image *image,ExceptionInfo *exception);

  /*
    Obtain the view corresponding to the current thread from the
    thread view set.
  */
  static inline ViewInfo
  *AccessThreadView(ThreadViewSet *view_set)
  {
    return view_set->views[omp_get_thread_num()];
  }

  /*
    Obtain the number of allocated views in the thread view set.
  */
  extern MagickExport unsigned int
  GetThreadViewSetAllocatedViews(ThreadViewSet *view_set);

  /****
   *
   * Simplified support for OpenMP thread-specific data
   * These interfaces are subject to change.
   *
   ****/

  typedef struct _ThreadViewDataSet
  {
    void
    **view_data;

    MagickFreeFunc
    destructor;
    
    unsigned int
    nviews;
  } ThreadViewDataSet;

  extern MagickExport void
  DestroyThreadViewDataSet(ThreadViewDataSet *data_set);
  
  extern MagickExport ThreadViewDataSet
  *AllocateThreadViewDataSet(const MagickFreeFunc destructor,
                             const Image *image,
                             ExceptionInfo *exception);

  extern MagickExport unsigned int
  GetThreadViewDataSetAllocatedViews(ThreadViewDataSet *data_set);

  extern MagickExport void
  *AccessThreadViewData(ThreadViewDataSet *data_set);

  extern MagickExport void
  AssignThreadViewData(ThreadViewDataSet *data_set, unsigned int index,
                       void *data);



  /*
    AcquireThreadViewPixels() obtains a read-only pixel region from a
    cache thread view.
  */
  extern MagickExport const PixelPacket
  *AcquireThreadViewPixels(ThreadViewSet *view_set,
                           const long x,const long y,
                           const unsigned long columns,
                           const unsigned long rows,ExceptionInfo *exception);

  /*
    AcquireThreadViewViewIndexes() returns the read-only indexes
    associated with a cache thread view.
  */
  extern MagickExport const IndexPacket
  *AcquireThreadViewIndexes(ThreadViewSet *view_set);

  /*
    AcquireOneThreadViewPixel() returns one pixel from a cache thread
    view.
  */
  static inline MagickPassFail
  AcquireOneThreadViewPixel(ThreadViewSet *view_set,PixelPacket *pixel,
                            const long x,const long y,
                            ExceptionInfo *exception)
  {
    return AcquireOneCacheViewPixel(AccessThreadView(view_set),pixel,x,y,exception);
  }

  /*
    GetThreadViewPixels() obtains a writeable pixel region from a
    cache thread view.
  */
  extern MagickExport PixelPacket
  *GetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                       const unsigned long columns,const unsigned long rows,
                       ExceptionInfo *exception);

  /*
    GetThreadViewIndexes() returns the writeable indexes associated
    with a cache thread view.
  */
  extern MagickExport IndexPacket
  *GetThreadViewIndexes(ThreadViewSet *view_set);

  /*
    SetThreadViewPixels() gets blank writeable pixels from a cache
    thread view.
  */
  extern MagickExport PixelPacket
  *SetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                       const unsigned long columns,const unsigned long rows,
                       ExceptionInfo *exception);

  /*
    SyncThreadViewPixels() saves any changes to pixel cache thread
    view.
  */
  extern MagickExport MagickPassFail
  SyncThreadViewPixels(ThreadViewSet *view_set,
                       ExceptionInfo *exception);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* defined(__cplusplus) || defined(c_plusplus) */

#endif /* _MAGICK_OMP_THREAD_VIEW */
