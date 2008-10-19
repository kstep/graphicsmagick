/*
% Copyright (C) 2008 GraphicsMagick Group
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
%                 GraphicsMagick OpenMP Thread View Methods                   %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                              September 2008                                 %
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
#include "magick/omp_thread_view.h"


/*
  Declare OpenMP thread view interfaces.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
  Forward declaration.
*/

MagickExport void
DestroyThreadViewSet(ThreadViewSet *view_set)
{
  unsigned int
    i;
  
  if (view_set != (ThreadViewSet *) NULL)
    {
      if (view_set->views != (ViewInfo *) NULL)
        {
          for (i=0; i < view_set->nviews; i++)
            {
              if (view_set->views[i] != (ViewInfo *) NULL)
                {
                  CloseCacheView(view_set->views[i]);
                  view_set->views[i]=(ViewInfo *) NULL;
                }
            }
        }
      view_set->nviews=0;
      MagickFreeMemory(view_set->views);
      MagickFreeMemory(view_set);
    }
}
MagickExport ThreadViewSet *
AllocateThreadViewSet(Image *image,ExceptionInfo *exception)
{
  ThreadViewSet
    *view_set;
  
  unsigned int
    i;
  
  MagickPassFail
    status=MagickPass;
  
  view_set=MagickAllocateMemory(ThreadViewSet *,sizeof(ThreadViewSet));
  if (view_set == (ThreadViewSet *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                      UnableToAllocateCacheView);
  /*
    omp_get_max_threads() returns the # threads which will be used in team by default.
    omp_get_num_threads() returns the # of threads in current team (1 in main thread).
  */
  view_set->nviews=omp_get_max_threads();
  /* printf("Allocated %d cache views ...\n",view_set->nviews); */
  
  view_set->views=MagickAllocateArray(ViewInfo *,view_set->nviews,sizeof(ViewInfo *));
  if (view_set->views == (ViewInfo *) NULL)
    {
      ThrowException(exception,CacheError,UnableToAllocateCacheView,
                     image->filename);
      status=MagickFail;
    }

  if (view_set->views != (ViewInfo *) NULL)
    for (i=0; i < view_set->nviews; i++)
      {
        view_set->views[i]=OpenCacheView(image);
        if (view_set->views[i] == (ViewInfo *) NULL)
          {
            ThrowException(exception,CacheError,UnableToAllocateCacheView,
                           image->filename);
            status=MagickFail;
          }
      }

  if (status == MagickFail)
    {
      DestroyThreadViewSet(view_set);
      view_set=(ThreadViewSet *) NULL;
    }

  return view_set;
}
MagickExport ViewInfo *
AccessThreadView(ThreadViewSet *view_set)
{
  ViewInfo
    *view;

  unsigned int
    thread_num=0;

  thread_num=omp_get_thread_num();
  assert(thread_num < view_set->nviews);
  view=view_set->views[thread_num];

  return view;
}


MagickExport unsigned int
GetThreadViewSetAllocatedViews(ThreadViewSet *view_set)
{
  return view_set->nviews;
}


/*
  AcquireThreadViewPixels() obtains a read-only pixel region from a
  cache thread view.
*/
MagickExport const PixelPacket
*AcquireThreadViewPixels(ThreadViewSet *view_set,
                         const long x,const long y,
                         const unsigned long columns,
                         const unsigned long rows,ExceptionInfo *exception)
{
  return AcquireCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                                exception);
}

/*
  AcquireThreadViewViewIndexes() returns the read-only indexes
  associated with a cache thread view.
*/
MagickExport const IndexPacket
*AcquireThreadViewIndexes(ThreadViewSet *view_set)
{
  return AcquireCacheViewIndexes(AccessThreadView(view_set));
}

/*
  AcquireOneThreadViewPixel() returns one pixel from a cache thread
  view.
*/
MagickExport PixelPacket
AcquireOneThreadViewPixel(ThreadViewSet *view_set,const long x,const long y,
                          ExceptionInfo *exception)
{
  return AcquireOneCacheViewPixel(AccessThreadView(view_set),x,y,exception);
}

/*
  GetThreadViewPixels() obtains a writeable pixel region from a
  cache thread view.
*/
MagickExport PixelPacket
*GetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                     const unsigned long columns,const unsigned long rows,
                     ExceptionInfo *exception)
{
  return GetCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                            exception);
}

/*
  GetThreadViewIndexes() returns the writeable indexes associated
  with a cache thread view.
*/
MagickExport IndexPacket
*GetThreadViewIndexes(ThreadViewSet *view_set)
{
  return GetCacheViewIndexes(AccessThreadView(view_set));
}

/*
  SetThreadViewPixels() gets blank writeable pixels from a cache
  thread view.
*/
MagickExport PixelPacket
*SetThreadViewPixels(ThreadViewSet *view_set,const long x,const long y,
                     const unsigned long columns,const unsigned long rows,
                     ExceptionInfo *exception)
{
  return SetCacheViewPixels(AccessThreadView(view_set),x,y,columns,rows,
                            exception);
}

/*
  SyncThreadViewPixels() saves any changes to pixel cache thread
  view.
*/
MagickExport MagickPassFail
SyncThreadViewPixels(ThreadViewSet *view_set,
                     ExceptionInfo *exception)
{
  return SyncCacheViewPixels(AccessThreadView(view_set),exception);
}



MagickExport void
DestroyThreadViewDataSet(ThreadViewDataSet *data_set)
{
  unsigned int
    i;
  
  if (data_set != (ThreadViewDataSet *) NULL)
    {
      if (data_set->view_data != (void *) NULL)
        {
          if (data_set->destructor != (MagickFreeFunc) NULL)
            {
              for (i=0; i < data_set->nviews; i++)
                {
                  (data_set->destructor)(data_set->view_data[i]);
                  data_set->view_data[i]=(void *) NULL;
                }
            }
          MagickFreeMemory(data_set->view_data);
        }
      data_set->nviews=0;
      MagickFreeMemory(data_set);
    }
}
MagickExport ThreadViewDataSet *
AllocateThreadViewDataSet(const MagickFreeFunc destructor,
                          const Image *image,
                          ExceptionInfo *exception)
{
  ThreadViewDataSet
    *data_set;
  
  MagickPassFail
    status=MagickPass;
  
  data_set=MagickAllocateMemory(ThreadViewDataSet *,sizeof(ThreadViewDataSet));
  if (data_set == (ThreadViewDataSet *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                      UnableToAllocateCacheView);
  data_set->destructor=destructor;
  data_set->nviews=omp_get_max_threads();
  data_set->view_data=MagickAllocateArray(void *,data_set->nviews,sizeof(void *));
  if (data_set->view_data == (void *) NULL)
    {
      ThrowException(exception,CacheError,UnableToAllocateCacheView,
                     image->filename);
      status=MagickFail;
    }

  if (data_set->view_data != (void *) NULL)
    (void) memset(data_set->view_data,0,data_set->nviews*sizeof(void *));
  
  if (status == MagickFail)
    {
      DestroyThreadViewDataSet(data_set);
      data_set=(ThreadViewDataSet *) NULL;
    }

  return data_set;
}

MagickExport void *
AccessThreadViewData(ThreadViewDataSet *data_set)
{
  unsigned int
    thread_num=0;

  thread_num=omp_get_thread_num();
  assert(thread_num < data_set->nviews);
  return data_set->view_data[thread_num];
}

MagickExport void AssignThreadViewData
(ThreadViewDataSet *data_set, unsigned int index, void *data)
{
  assert(index < data_set->nviews);
  MagickFreeMemory(data_set->view_data[index]);
  data_set->view_data[index]=data;
}


MagickExport unsigned int
GetThreadViewDataSetAllocatedViews
(ThreadViewDataSet *data_set)
{
  return data_set->nviews;
}
