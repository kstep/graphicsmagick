//  ------------------------------------------------------------------------------------------------
//
//  This source code is the property of FITS Imaging inc.
//
//  MODULE    : ToolkitIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Tuesday, November 23rd 1993
//  DESCRIPTION : 
//  COMMENT   :
//  SCCSID      : @(#)ri_lib.h  1.2 12:19:09 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef IVUEToolkit_h
  #define IVUEToolkit_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  "ri_libio.h"

//  Constants
//  ----------

//  Class declarations 
//  ------------------

  class  PageImage;
  class  ViewWorld;
  class  PIVUEImageView;
  class  ViewWindow;
  
//  Accelerator Toolkit Class Definitions
//  -------------------------------------

  typedef ViewWorld     IVUEWorld;
  typedef PIVUEImageView  AdvancedIVUE;
  typedef ViewWindow    IVUEWindow;
  typedef PageImage     PageIVUE;

//  Class definitions 
//  -----------------

// 'inline' Functions
//  -----------------

// 'extern' Functions
//  -----------------


//  Accelerator Toolkit Functions Definitions
//  -----------------------------------------

  extern "C" {
  
     // Formatted output tools :
     long IVUE_SetPageSetup (IVUE* ivue, PageIVUE** page, long width, long height, float rotation);
     long IVUE_ClosePage(PageIVUE* page);
     long IVUE_ReadPage (PageIVUE* page, Pixel* image);
     long IVUE_ReadPageLine (PageIVUE* page, long lineNumber, Pixel* line);
  }
     

  extern "C" {
     // World tools :
     long IVUE_CreateWorld(IVUEWorld** world, float width, float height, Pixel color);
     long IVUE_DeleteWorld(IVUEWorld* world);
     
     // List of images management :
     long IVUE_AddImage(IVUEWorld* world, AdvancedIVUE** advIvue, IVUE_FileName* filName);
     long IVUE_DeleteImage(IVUEWorld* world, AdvancedIVUE* advIvue);
     long IVUE_AddRefImage(IVUEWorld* world, AdvancedIVUE** advIvue, IVUE* theReference);
     long IVUE_DeleteRefImage(IVUEWorld* world, AdvancedIVUE* advIvue);
     long IVUE_GetReference(AdvancedIVUE* theImage, IVUE** theReference);
     
     long IVUE_GetFirstImage(IVUEWorld* world, AdvancedIVUE** advIvue);
     long IVUE_GetLastImage(IVUEWorld* world, AdvancedIVUE** advIvue);
     long IVUE_GetCurrentImage(IVUEWorld* world, AdvancedIVUE** advIvue);
     
     long IVUE_NextImage(IVUEWorld* world, AdvancedIVUE** advIvue);
     long IVUE_PreviousImage(IVUEWorld* world, AdvancedIVUE** advIvue);
     
     // Image position tools :
     long IVUE_UseAlphaChannel(AdvancedIVUE* advIvue, Boolean useAlpha);
     long IVUE_InvertAlphaChannel(AdvancedIVUE* advIvue, Boolean inverseAlpha);
     long IVUE_SetImageSize(AdvancedIVUE* advIvue, float width, float height);
     long IVUE_SetImageTransform(AdvancedIVUE* advIvue, float x0, float y0, float m11, float m12, float m21, float m22);
     long IVUE_ApplyImageTransform(AdvancedIVUE* advIvue, float x0, float y0, float m11, float m12, float m21, float m22);
     long IVUE_TranslateImage(AdvancedIVUE* advIvue, float dx, float dy);
     long IVUE_RotateImage(AdvancedIVUE* advIvue, float x0, float y0, float theta);
     long IVUE_ScaleImage(AdvancedIVUE* advIvue, float x0, float y0, float sx, float sy);
     long IVUE_ShearHorizontal(AdvancedIVUE* advIvue, float x0, float y0, float sh);
     long IVUE_ShearVertical(AdvancedIVUE* advIvue, float x0, float y0, float sh);
     long IVUE_FlipHorizontal(AdvancedIVUE* advIvue, float x0, float y0);
     long IVUE_FlipVertical(AdvancedIVUE* advIvue, float x0, float y0);
     
     long IVUE_GetOutlineParallelogram(AdvancedIVUE* advIvue, float* x0, float* y0, float* x1, float* y1, 
                                  float* x2, float* y2, float* x3, float* y3);
     long IVUE_GetOutlineRectangle(AdvancedIVUE* advIvue, float* x0, float* y0, float* x1, float* y1);
     long IVUE_GetOrigin(AdvancedIVUE* advIvue, float* x0, float* y0);
     
     long IVUE_SetImageCrop(AdvancedIVUE* advIvue, float x0, float y0, float x1, float y1);
     long IVUE_ResetImageCrop(AdvancedIVUE* advIvue);
     
     // View Window  tools :
     long IVUE_CreateWindow(IVUEWorld* world, IVUEWindow** window, float x0, float y0, float width, float height, float resolution);
     long IVUE_DeleteWindow(IVUEWindow* window);

     long IVUE_ReadWindowSample(IVUEWindow* window, long x0, long y0, Pixel* sample);
     long IVUE_RefreshWindow(IVUEWindow* window, Pixel* map);
     
     long IVUE_TranslateWindow(IVUEWindow* window, float  dx, float  dy);
     long IVUE_ResizeWindow(IVUEWindow* window, float width, float height);
     long IVUE_ZoomWindow(IVUEWindow* window, float zoomRatio);
     
     long IVUE_GetWindowDefinition(IVUEWindow* window, float* x0, float* y0, float* width, float* height, float* resolution);
     long IVUE_WindowToWorld(IVUEWindow* window, long i, long j, float* x, float* y);
     long IVUE_WorldToWindow(IVUEWindow* window, float x, float y, long* i, long* j);
  }
  

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // IVUEToolkit_h
//  ------------------------------------------------------------------------------------------------
