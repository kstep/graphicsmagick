/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                 N   N  TTTTT                                %
%                                 NN  N    T                                  %
%                                 N N N    T                                  %
%                                 N  NN    T                                  %
%                                 N   N    T                                  %
%                                                                             %
%                                                                             %
%                  Windows NT Feature Methods for ImageMagick                 %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1996                                %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#if defined(WIN32) || defined(__CYGWIN__)
/*
  Include declarations.
*/
#include "studio.h"
#include "cache.h"
#include "utility.h"
#include "nt_feature.h"
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e T o H B i t m a p                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CropImageToHBITMAP extracts a specified region of the image and
%  returns it as a Windows HBITMAP. While the same functionality can be
%  accomplished by invoking CropImage() followed by ImageToHBITMAP(), this
%  method is more efficient since it copies pixels directly to the HBITMAP.
%
%  The format of the CropImageToHBITMAP method is:
%
%      HBITMAP CropImageToHBITMAP(Image* image,const RectangleInfo *geometry,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o geometry: Define the region of the image to crop with members
%      x, y, width, and height.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *CropImageToHBITMAP(const Image *image,
  const RectangleInfo *geometry,ExceptionInfo *exception)
{
#define CropImageText  "  Crop image...  "

  Image
    *crop_image;

  long
    y;

  RectangleInfo
    page;

  register const PixelPacket
    *p;

  register IndexPacket
    *crop_indexes,
    *indexes;

  BITMAP
    bitmap;

  HBITMAP
    bitmapH;

  HANDLE
    bitmap_bitsH;

  register RGBQUAD
    *q;

  RGBQUAD
    *bitmap_bits;

  /*
    Check crop geometry.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(geometry != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((geometry->width != 0) || (geometry->height != 0))
    {
      if (((geometry->x+(long) geometry->width) < 0) ||
          ((geometry->y+(long) geometry->height) < 0) ||
          (geometry->x >= (long) image->columns) ||
          (geometry->y >= (long) image->rows))
        ThrowImageException(OptionError,"GeometryDoesNotContainImage",
          "UnableToCropImage");
    }
  page=(*geometry);
  if ((page.width != 0) || (page.height != 0))
    {
      if ((page.x+(long) page.width) > (long) image->columns)
        page.width=image->columns-page.x;
      if ((page.y+(long) page.height) > (long) image->rows)
        page.height=image->rows-page.y;
      if (page.x < 0)
        {
          page.width+=page.x;
          page.x=0;
        }
      if (page.y < 0)
        {
          page.height+=page.y;
          page.y=0;
        }
    }
  else
    {
      /*
        Set bounding box to the image dimensions.
      */
      page=GetImageBoundingBox(image,exception);
      page.width+=geometry->x*2;
      page.height+=geometry->y*2;
      page.x-=geometry->x;
      if (page.x < 0)
        page.x=0;
      page.y-=geometry->y;
      if (page.y < 0)
        page.y=0;
      if ((((long) page.width+page.x) > (long) image->columns) ||
          (((long) page.height+page.y) > (long) image->rows))
        ThrowImageException(OptionError,"GeometryDoesNotContainImage",
          "UnableToCropImage");
    }
  if ((page.width == 0) || (page.height == 0))
    ThrowImageException(OptionError,"GeometryDimensionsAreZero",
      "UnableToCropImage");
  /*
    Initialize crop image attributes.
  */
  bitmap.bmType         = 0;
  bitmap.bmWidth        = page.width;
  bitmap.bmHeight       = page.height;
  bitmap.bmWidthBytes   = bitmap.bmWidth * 4;
  bitmap.bmPlanes       = 1;
  bitmap.bmBitsPixel    = 32;
  bitmap.bmBits         = NULL;

  bitmap_bitsH = (HANDLE) GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE,
               page.width*page.height*bitmap.bmBitsPixel);
  if (bitmap_bitsH == NULL)
    return( NULL ); 

  bitmap_bits = (RGBQUAD *) GlobalLock((HGLOBAL) bitmap_bitsH);

  if ( bitmap.bmBits == NULL )
    bitmap.bmBits = bitmap_bits;

  if (image->colorspace != RGBColorspace)
    (void) TransformRGBImage(image,image->colorspace);

  /*
    Extract crop image.
  */
  q = bitmap_bits;

  for (y=0; y < (long) page.height; y++)
  {
    p=AcquireImagePixels(image,page.x,page.y+y,page.width,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;

#if QuantumDepth == 8
      /* Form of PixelPacket is identical to RGBQUAD when QuantumDepth==8 */
      memcpy((void*)q,(const void*)p,page.width*sizeof(PixelPacket));
      q += page.width;

#else  /* 16 or 32 bit Quantum */
      {
        long
          x;

        /* Transfer pixels, scaling to Quantum */
        for( x=page.width ; x> 0 ; x-- )
          {
            q->rgbRed = ScaleQuantumToChar(p->red);
            q->rgbGreen = ScaleQuantumToChar(p->green);
            q->rgbBlue = ScaleQuantumToChar(p->blue);
            q->rgbReserved = 0;
            ++q;
            ++p;
          }
      }
#endif
    if (QuantumTick(y,page.height))
      if (!MagickMonitor(CropImageText,y,page.height-1,exception))
        break;
  }
  if (y < (long) page.height)
    {
      GlobalUnlock((HGLOBAL) bitmap_bitsH);
      GlobalFree((HGLOBAL) bitmap_bitsH);
      return((void *) NULL);
    }

  bitmap.bmBits = bitmap_bits;
  bitmapH = CreateBitmapIndirect( &bitmap );

  GlobalUnlock((HGLOBAL) bitmap_bitsH);

  return (void *)bitmapH;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M a g i c k C o n f l i c t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMagickConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the IsMagickConflict method is:
%
%      unsigned int IsMagickConflict(const char *magick)
%
%  A description of each parameter follows:
%
%    o status: Method IsMagickConflict returns true if the image format
%      conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
MagickExport unsigned int NTIsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
  if (strlen(magick) > 1)
    return(False);
  return((GetLogicalDrives()) & (1 << ((toupper((int) (*magick)))-'A')));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   N T G e t T y pe L i s t                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTGetTypeList returns a TypeInfo list corresponding to installed
%  Windows TrueType fonts. The user is responsible for destroying the
%  returned list once it is no longer needed. NULL is returned if a fatal
%  error is encountered.
%
%  The format of the NTGetTypeList method is:
%
%      TypeInfo* NTGetTypeList( void )
%
%  A description of each parameter follows:
%
%    o returns: A linked list of TypeInfo structures is returned.
%
*/
static int TypeInfoCompare(const void *x,const void *y)
{
  TypeInfo
    **info_1,
    **info_2;

  info_1=(TypeInfo **) x;
  info_2=(TypeInfo **) y;
  return( strcmp((*info_1)->name, (*info_2)->name) );
}
MagickExport TypeInfo* NTGetTypeList( void )
{
  TypeInfo
    *type_list;

  HKEY
    reg_key = (HKEY) INVALID_HANDLE_VALUE;

  LONG
    res;


  int
    list_entries = 0;

  char
    buffer[MaxTextExtent],
    font_root[MaxTextExtent];

  type_list = (TypeInfo*) NULL;

  res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                       0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &reg_key);

  if (res != ERROR_SUCCESS)
    res = RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
                         0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &reg_key);

  if (res != ERROR_SUCCESS)
    return (TypeInfo *) type_list;

  *font_root='\0';
  if ( getenv("SystemRoot") != (char *) NULL)
    {
      strncpy(buffer,getenv("SystemRoot"),sizeof(buffer)-1);
      strcat(buffer,"\\fonts\\arial.ttf");
      if (IsAccessible(buffer))
        {
          strncpy(font_root,getenv("SystemRoot"),sizeof(buffer)-1);
          strcat(font_root,"\\fonts\\");
        }
      else
        {
          strncpy(font_root,getenv("SystemRoot"),sizeof(buffer)-1);
          strcat(font_root,"\\");
        }
    }

  {
    TypeInfo
      *type_info;

    DWORD
      registry_index = 0,
      type,
      value_data_size,
      value_name_length;

    char
      value_data[MaxTextExtent],
      value_name[MaxTextExtent];

    res = ERROR_SUCCESS;

    while ( (res != ERROR_NO_MORE_ITEMS) && (res == ERROR_SUCCESS) )
      {
        char
          *family_extent,
          token[MaxTextExtent],
          *pos,
          *q;

        value_name_length = sizeof(value_name) - 1;
        value_data_size = sizeof(value_data) - 1;
        res = RegEnumValueA ( reg_key, registry_index, value_name, &value_name_length,
                              0, &type, (BYTE*)value_data, &value_data_size);
        registry_index++;
        if ( (pos = strstr(value_name, " (TrueType)")) == (char*) NULL )
          continue;
        *pos='\0'; /* Remove (TrueType) from string */

        type_info=(TypeInfo *) AcquireMemory(sizeof(TypeInfo));
        if (type_info == (TypeInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateTypeInfo");
        (void) memset(type_info,0,sizeof(TypeInfo));

        type_info->path=AcquireString("Windows Fonts");
        type_info->signature=MagickSignature;

        /* Name */
        strncpy(buffer,value_name,MaxTextExtent-1);
        for(pos = buffer; *pos != 0 ; pos++)
          if (*pos == ' ')
            *pos = '-';
        type_info->name=AcquireString(buffer);

        /* Fullname */
        type_info->description=AcquireString(value_name);

        /* Format */
        type_info->format=AcquireString("truetype");

        /* Glyphs */
        if (strchr(value_data,'\\') == (char *) NULL)
          {
            strncpy(buffer,font_root,MaxTextExtent-1);
            strcat(buffer,value_data);
          }
        else
          strncpy(buffer,value_data,MaxTextExtent-1);

        LocaleLower(buffer);
        type_info->glyphs=AcquireString(buffer);

        type_info->stretch=NormalStretch;
        type_info->style=NormalStyle;
        type_info->weight=400;

        /* Some fonts are known to require special encodings */
        if ( (LocaleCompare(type_info->name, "Symbol") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings-2") == 0 ) ||
             (LocaleCompare(type_info->name, "Wingdings-3") == 0 ) )
          type_info->encoding=AcquireString("AppleRoman");

        family_extent=value_name;

        for (q=value_name; *q != '\0'; )
          {
            GetToken(q,&q,token);
            if (*token == '\0')
              break;

            if (LocaleCompare(token,"Italic") == 0)
              {
                type_info->style=ItalicStyle;
              }

            else if (LocaleCompare(token,"Oblique") == 0)
              {
                type_info->style=ObliqueStyle;
              }

            else if (LocaleCompare(token,"Bold") == 0)
              {
                type_info->weight=700;
              }

            else if (LocaleCompare(token,"Thin") == 0)
              {
                type_info->weight=100;
              }

            else if ( (LocaleCompare(token,"ExtraLight") == 0) ||
                      (LocaleCompare(token,"UltraLight") == 0) )
              {
                type_info->weight=200;
              }

            else if (LocaleCompare(token,"Light") == 0)
              {
                type_info->weight=300;
              }

            else if ( (LocaleCompare(token,"Normal") == 0) ||
                      (LocaleCompare(token,"Regular") == 0) )
              {
                type_info->weight=400;
              }

            else if (LocaleCompare(token,"Medium") == 0)
              {
                type_info->weight=500;
              }

            else if ( (LocaleCompare(token,"SemiBold") == 0) ||
                      (LocaleCompare(token,"DemiBold") == 0) )
              {
                type_info->weight=600;
              }

            else if ( (LocaleCompare(token,"ExtraBold") == 0) ||
                      (LocaleCompare(token,"UltraBold") == 0) )
              {
                type_info->weight=800;
              }

            else if ( (LocaleCompare(token,"Heavy") == 0) ||
                      (LocaleCompare(token,"Black") == 0) )
              {
                type_info->weight=900;
              }

            else if (LocaleCompare(token,"Condensed") == 0)
              {
                type_info->stretch = CondensedStretch;
              }

            else if (LocaleCompare(token,"Expanded") == 0)
              {
                type_info->stretch = ExpandedStretch;
              }

            else if (LocaleCompare(token,"ExtraCondensed") == 0)
              {
                type_info->stretch = ExtraCondensedStretch;
              }

            else if (LocaleCompare(token,"ExtraExpanded") == 0)
              {
                type_info->stretch = ExtraExpandedStretch;
              }

            else if (LocaleCompare(token,"SemiCondensed") == 0)
              {
                type_info->stretch = SemiCondensedStretch;
              }

            else if (LocaleCompare(token,"SemiExpanded") == 0)
              {
                type_info->stretch = SemiExpandedStretch;
              }

            else if (LocaleCompare(token,"UltraCondensed") == 0)
              {
                type_info->stretch = UltraCondensedStretch;
              }

            else if (LocaleCompare(token,"UltraExpanded") == 0)
              {
                type_info->stretch = UltraExpandedStretch;
              }

            else
              {
                family_extent=q;
              }
          }

        strncpy(buffer,value_name,family_extent-value_name);
        buffer[family_extent-value_name]='\0';
        type_info->family=AcquireString(buffer);

        list_entries++;
        if (type_list == (TypeInfo *) NULL)
          {
            type_list=type_info;
            continue;
          }
        type_list->next=type_info;
        type_info->previous=type_list;
        type_list=type_list->next;
        continue;
      }
  }

  RegCloseKey ( reg_key );

  /* Sort entries */
  {
    TypeInfo
      **type_array;

    int
      array_index = 0;

    type_array = (TypeInfo**) AcquireMemory(sizeof(TypeInfo*)*list_entries);

    while (type_list->previous != (TypeInfo *) NULL)
      type_list=type_list->previous;

    for (array_index=0; array_index< list_entries; array_index++)
      {
        type_array[array_index] = type_list;
        type_list=type_list->next;
      }

    qsort((void *) type_array, list_entries, sizeof(TypeInfo *),TypeInfoCompare);

    type_list=type_array[0];
    type_list->previous=(TypeInfo *) NULL;
    for(array_index=1; array_index < list_entries; array_index++)
      {
        type_array[array_index-1]->next = type_array[array_index];
        type_array[array_index]->previous = type_array[array_index-1];
        type_array[array_index]->next=(TypeInfo *) NULL;
      }

    LiberateMemory((void**) &type_array);
  }

  return (TypeInfo *) type_list;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o H B i t m a p                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageToHBITMAP creates a Windows HBITMAP from an IM Image
%
%  The format of the ImageToHBITMAP method is:
%
%      HBITMAP ImageToHBITMAP(Image* image)
%
%  A description of each parameter follows:
%
%    o image: the image to convert.
%
%
*/
MagickExport void *ImageToHBITMAP(Image* image)
{
  unsigned long 
    nPixels,
    row;

  long
    memSize;

  const PixelPacket
    *pPixels;

  BITMAP
    bitmap;

  HBITMAP
    bitmapH;

  HANDLE
    bitmap_bitsH;

  RGBQUAD
    *pDestPixel,
    *bitmap_bits;

  nPixels = image->columns * image->rows;

  bitmap.bmType         = 0;
  bitmap.bmWidth        = image->columns;
  bitmap.bmHeight       = image->rows;
  bitmap.bmWidthBytes   = bitmap.bmWidth * 4;
  bitmap.bmPlanes       = 1;
  bitmap.bmBitsPixel    = 32;
  bitmap.bmBits         = NULL;

  memSize = nPixels * bitmap.bmBitsPixel;
  bitmap_bitsH = (HANDLE) GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, memSize);

  if (bitmap_bitsH == NULL)
    return( NULL ); 
  
  bitmap_bits = (RGBQUAD *) GlobalLock((HGLOBAL) bitmap_bitsH);
  pDestPixel = bitmap_bits;

  if ( bitmap.bmBits == NULL )
    bitmap.bmBits = bitmap_bits;

  (void) TransformRGBImage(image,image->colorspace);
  for( row = 0 ; row < image->rows ; row++ )
    {
      pPixels = AcquireImagePixels(image,0,row,image->columns,1,
                                   &image->exception);
#if QuantumDepth == 8
      /* Form of PixelPacket is identical to RGBQUAD when QuantumDepth==8 */
      memcpy((void*)pDestPixel,(const void*)pPixels,
             sizeof(PixelPacket)*image->columns);
      pDestPixel += image->columns;

#else  /* 16 or 32 bit Quantum */
      {
        unsigned long nPixelCount;

        /* Transfer pixels, scaling to Quantum */
        for( nPixelCount = image->columns; nPixelCount ; nPixelCount-- )
          {
            pDestPixel->rgbRed = ScaleQuantumToChar(pPixels->red);
            pDestPixel->rgbGreen = ScaleQuantumToChar(pPixels->green);
            pDestPixel->rgbBlue = ScaleQuantumToChar(pPixels->blue);
            pDestPixel->rgbReserved = 0;
            ++pDestPixel;
            ++pPixels;
          }
      }
#endif
    }

  bitmap.bmBits = bitmap_bits;
  bitmapH = CreateBitmapIndirect( &bitmap );

  GlobalUnlock((HGLOBAL) bitmap_bitsH);

  return (void *)bitmapH;
}

#endif
