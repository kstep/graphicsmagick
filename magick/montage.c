/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                %
%              MM MM  O   O  NN  N    T    A   A  G      E                    %
%              M M M  O   O  N N N    T    AAAAA  G  GG  EEE                  %
%              M   M  O   O  N  NN    T    A   A  G   G  E                    %
%              M   M   OOO   N   N    T    A   A   GGG   EEEEE                %
%                                                                             %
%                                                                             %
%              ImageMagick Methods to Create Image Thumbnails                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e M o n t a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneMontageInfo makes a duplicate of the given montage info, or if
%  montage info is NULL, a new one.
%
%  The format of the CloneMontageInfo method is:
%
%      MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
%        const MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneMontageInfo returns a duplicate of the given
%      annotate info, or if annotate info is NULL a new one.
%
%    o image_info: a structure of type ImageInfo.
%
%    o montage_info: a structure of type MontageInfo.
%
%
*/
MagickExport MontageInfo *CloneMontageInfo(const ImageInfo *image_info,
  const MontageInfo *montage_info)
{
  MontageInfo
    *clone_info;

  clone_info=(MontageInfo *) AcquireMemory(sizeof(MontageInfo));
  if (clone_info == (MontageInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone montage info",
      "Memory allocation failed");
  if (montage_info == (MontageInfo *) NULL)
    {
      GetMontageInfo(image_info,clone_info);
      return(clone_info);
    }
  *clone_info=(*montage_info);
  if (montage_info->geometry != (char *) NULL)
    clone_info->geometry=AllocateString(montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(montage_info->tile);
  if (montage_info->frame != (char *) NULL)
    clone_info->frame=AllocateString(montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(montage_info->texture);
  if (montage_info->font != (char *) NULL)
    clone_info->font=AllocateString(montage_info->font);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M o n t a g e I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMontageInfo deallocates memory associated with an MontageInfo
%  structure.
%
%  The format of the DestroyMontageInfo method is:
%
%      void DestroyMontageInfo(MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o montage_info: Specifies a pointer to an MontageInfo structure.
%
%
*/
MagickExport void DestroyMontageInfo(MontageInfo *montage_info)
{
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  if (montage_info->geometry != (char *) NULL)
    LiberateMemory((void **) &montage_info->geometry);
  if (montage_info->tile != (char *) NULL)
    LiberateMemory((void **) &montage_info->tile);
  if (montage_info->title != (char *) NULL)
    LiberateMemory((void **) &montage_info->title);
  if (montage_info->frame != (char *) NULL)
    LiberateMemory((void **) &montage_info->frame);
  if (montage_info->texture != (char *) NULL)
    LiberateMemory((void **) &montage_info->texture);
  if (montage_info->font != (char *) NULL)
    LiberateMemory((void **) &montage_info->font);
  LiberateMemory((void **) &montage_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M o n t a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMontageInfo initializes the MontageInfo structure.
%
%  The format of the GetMontageInfo method is:
%
%      void GetMontageInfo(const ImageInfo *image_info,
%        MontageInfo *montage_info)
%
%  A description of each parameter follows:
%
%    o image_info: a structure of type ImageInfo.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%
*/
MagickExport void GetMontageInfo(const ImageInfo *image_info,
  MontageInfo *montage_info)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  (void) memset(montage_info,0,sizeof(MontageInfo));
  (void) strcpy(montage_info->filename,image_info->filename);
  montage_info->geometry=AllocateString(DefaultTileGeometry);
  montage_info->tile=AllocateString("6x4");
  montage_info->font=AllocateString(image_info->font);
  montage_info->pointsize=image_info->pointsize;
  montage_info->gravity=CenterGravity;
  montage_info->compose=OverCompositeOp;
  montage_info->fill=image_info->pen;
  montage_info->stroke=image_info->pen;
  montage_info->background_color=image_info->background_color;
  montage_info->border_color=image_info->border_color;
  montage_info->matte_color=image_info->matte_color;
  montage_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o n t a g e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MontageImages creates a composite image by combining several
%  separate images.
%
%  The format of the MontageImages method is:
%
%      Image *MontageImages(Image *image,const MontageInfo *montage_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to an array of Image structures.
%
%    o montage_info: Specifies a pointer to a MontageInfo structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int SceneCompare(const void *x,const void *y)
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) ((*image_1)->scene-(*image_2)->scene));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *MontageImages(Image *image,const MontageInfo *montage_info,
  ExceptionInfo *exception)
{
#define MontageImageText  "  Create visual image directory...  "
#define TileImageText  "  Create image tiles...  "

  char
    geometry[MaxTextExtent],
    *title;

  DrawInfo
    *draw_info;

  FrameInfo
    frame_info;

  Image
    **next_list,
    **master_list,
    *montage_next,
    *texture,
    *tile_next;

  ImageAttribute
    *attribute;

  ImageInfo
    *image_info;

  int
    x,
    x_offset,
    y,
    y_offset;

  MonitorHandler
    handler;

  register Image
    *next;

  register int
    i;

  register PixelPacket
    *q;

  RectangleInfo
    bounds,
    tile_info;

  unsigned int
    border_width,
    bevel_width,
    concatenate,
    font_height,
    height,
    images_per_page,
    max_height,
    number_images,
    number_lines,
    tile,
    tiles,
    tiles_per_column,
    tiles_per_row,
    tiles_per_page,
    title_offset,
    total_tiles,
    width;

  unsigned long
    count;

  /*
    Create next tiles.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(montage_info != (MontageInfo *) NULL);
  assert(montage_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  next_list=ListToGroupImage(image,&number_images);
  master_list=next_list;
  for (tile=0; tile < number_images; tile++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    width=next_list[tile]->columns;
    height=next_list[tile]->rows;
    x=0;
    y=0;
    (void) ParseImageGeometry(montage_info->geometry,&x,&y,&width,&height);
    next_list[tile]->orphan=True;
    tile_next=ZoomImage(next_list[tile],width,height,exception);
    if (tile_next == (Image *) NULL)
      {
        for (i=0; i < (int) tile; i++)
          DestroyImage(next_list[i]);
        (void) SetMonitorHandler(handler);
        return((Image *) NULL);
      }
    next_list[tile]=tile_next;
    (void) SetMonitorHandler(handler);
    MagickMonitor(TileImageText,tile,number_images);
  }
  /*
    Sort next_list by increasing tile number.
  */
  for (tile=0; tile < number_images; tile++)
    if (next_list[tile]->scene == 0)
      break;
  if (tile == number_images)
    qsort((void *) next_list,number_images,sizeof(Image *),SceneCompare);
  /*
    Determine tiles per row and column.
  */
  tiles_per_row=1;
  tiles_per_column=1;
  while ((tiles_per_row*tiles_per_column) < number_images)
  {
    tiles_per_row++;
    tiles_per_column++;
  }
  if (montage_info->tile != (char *) NULL)
    {
      tiles_per_column=number_images;
      x=0;
      y=0;
      (void) ParseGeometry(montage_info->tile,&x,&y,&tiles_per_row,
        &tiles_per_column);
    }
  /*
    Determine tile sizes.
  */
  border_width=montage_info->border_width;
  bevel_width=0;
  if (montage_info->frame != (char *) NULL)
    {
      int
        flags;

      frame_info.width=0;
      frame_info.height=0;
      frame_info.outer_bevel=0;
      frame_info.inner_bevel=0;
      flags=ParseGeometry(montage_info->frame,&frame_info.outer_bevel,
        &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
      if ((flags & HeightValue) == 0)
        frame_info.height=frame_info.width;
      if ((flags & XValue) == 0)
        frame_info.outer_bevel=(frame_info.width >> 2)+1;
      if ((flags & YValue) == 0)
        frame_info.inner_bevel=frame_info.outer_bevel;
      frame_info.x=frame_info.width;
      frame_info.y=frame_info.height;
      bevel_width=Max(frame_info.inner_bevel,frame_info.outer_bevel);
      border_width=Max(frame_info.width,frame_info.height);
    }
  tile_info.x=montage_info->border_width;
  tile_info.y=montage_info->border_width;
  tile_info.width=next_list[0]->columns;
  tile_info.height=next_list[0]->rows;
  concatenate=False;
  if (montage_info->geometry != (char *) NULL)
    {
      int
        flags;

      /*
        Initialize tile geometry.
      */
      (void) strcpy(geometry,montage_info->geometry);
      tile_info.x=0;
      tile_info.y=0;
      if (strchr(geometry,'%') == (char *) NULL)
        flags=GetGeometry(geometry,&tile_info.x,&tile_info.y,
          &tile_info.width,&tile_info.height);
      else
        flags=ParseImageGeometry(geometry,&tile_info.x,&tile_info.y,
          &tile_info.width,&tile_info.height);
      if ((tile_info.x == 0) && (tile_info.y == 0))
        concatenate=!((flags & WidthValue) || (flags & HeightValue));
      if (tile_info.x < 0)
        tile_info.x=0;
      if (tile_info.y < 0)
        tile_info.y=0;
    }
  for (tile=1; tile < number_images; tile++)
  {
    if (next_list[tile]->columns > tile_info.width)
      tile_info.width=next_list[tile]->columns;
    if (next_list[tile]->rows > tile_info.height)
      tile_info.height=next_list[tile]->rows;
  }
  /*
    Initialize annotate info.
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->background_color=montage_info->background_color;
  image_info->border_color=montage_info->border_color;
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  (void) CloneString(&draw_info->font,montage_info->font);
  draw_info->pointsize=montage_info->pointsize;
  draw_info->gravity=CenterGravity;
  draw_info->stroke=montage_info->stroke;
  draw_info->fill=montage_info->fill;
  font_height=(unsigned int)
    (ExpandAffine(&draw_info->affine)*draw_info->pointsize);
  texture=(Image *) NULL;
  if (montage_info->texture != (char *) NULL)
    {
      (void) strcpy(image_info->filename,montage_info->texture);
      texture=ReadImage(image_info,exception);
    }
  /*
    Determine the number of lines in an next label.
  */
  title=TranslateText(image_info,image,montage_info->title);
  title_offset=0;
  if (montage_info->title != (char *) NULL)
    title_offset=2*font_height*MultilineCensus(title)+2*tile_info.y;
  number_lines=0;
  for (tile=0; tile < number_images; tile++)
  {
    attribute=GetImageAttribute(next_list[tile],"label");
    if (attribute == (ImageAttribute *) NULL)
      continue;
    if (MultilineCensus(attribute->value) > (int) number_lines)
      number_lines=MultilineCensus(attribute->value);
  }
  /*
    Allocate next structure.
  */
  montage_next=AllocateImage(image_info);
  montage_next->scene=1;
  images_per_page=(number_images-1)/(tiles_per_row*tiles_per_column)+1;
  tiles=0;
  total_tiles=number_images;
  for (i=0; i < (int) images_per_page; i++)
  {
    /*
      Determine bounding box.
    */
    tiles_per_page=Min(number_images,tiles_per_row*tiles_per_column);
    x_offset=0;
    y_offset=title_offset;
    max_height=0;
    bounds.width=0;
    bounds.height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      width=concatenate ? next_list[tile]->columns : tile_info.width;
      x_offset+=width+(tile_info.x+border_width)*2;
      if (x_offset > (int) bounds.width)
        bounds.width=x_offset;
      if (next_list[tile]->rows > max_height)
        max_height=next_list[tile]->rows;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=0;
          height=concatenate ? max_height : tile_info.height;
          y_offset+=height+(tile_info.y+border_width)*2+(font_height+4)*
            number_lines+(montage_info->shadow ? 4 : 0)+(concatenate ? 0 : 2);
          if (y_offset > (int) bounds.height)
            bounds.height=y_offset;
          max_height=0;
        }
    }
    bounds.height-=(concatenate ? 0 : 2);
    /*
      Initialize Image structure.
    */
    (void) strcpy(montage_next->filename,montage_info->filename);
    montage_next->columns=bounds.width;
    montage_next->rows=bounds.height;
    SetImage(montage_next,OpaqueOpacity);
    /*
      Set montage geometry.
    */
    montage_next->montage=AllocateString((char *) NULL);
    count=1;
    for (tile=0; tile < tiles_per_page; tile++)
      count+=strlen(next_list[tile]->filename)+1;
    montage_next->directory=(char *) AcquireMemory(count);
    if ((montage_next->montage == (char *) NULL) ||
        (montage_next->directory == (char *) NULL))
      {
        DestroyImages(montage_next);
        ThrowImageException(ResourceLimitWarning,"Unable to montage next_list",
          "Memory allocation failed")
      }
    x_offset=0;
    y_offset=title_offset;
    FormatString(montage_next->montage,"%dx%d%+d%+d",
      (int) (tile_info.width+(tile_info.x+border_width)*2),
      (int) (tile_info.height+(tile_info.y+border_width)*2+(font_height+4)*
      number_lines+(montage_info->shadow ? 4 : 0)),x_offset,y_offset);
    *montage_next->directory='\0';
    for (tile=0; tile < tiles_per_page; tile++)
    {
      (void) strcat(montage_next->directory,next_list[tile]->filename);
      (void) strcat(montage_next->directory,"\n");
    }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    if (texture != (Image *) NULL)
      TextureImage(montage_next,texture);
    if (montage_info->title != (char *) NULL)
      {
        /*
          Annotate composite next with title.
        */
        FormatString(geometry,"%ux%u%+d%+d",montage_next->columns,
          font_height << 1,0,font_height+tile_info.y+4);
        (void) CloneString(&draw_info->geometry,geometry);
        (void) CloneString(&draw_info->text,title);
        (void) AnnotateImage(montage_next,draw_info);
      }
    (void) SetMonitorHandler(handler);
    /*
      Copy tile next to the composite next.
    */
    x_offset=tile_info.x;
    y_offset=title_offset+tile_info.y;
    max_height=0;
    for (tile=0; tile < tiles_per_page; tile++)
    {
      /*
        Copy this tile to the composite next.
      */
      handler=SetMonitorHandler((MonitorHandler) NULL);
      next=next_list[tile];
      width=concatenate ? next->columns : tile_info.width;
      if (next->rows > max_height)
        max_height=next->rows;
      height=concatenate ? max_height : tile_info.height;
      if (border_width != 0)
        {
          Image
            *bordered_next;

          RectangleInfo
            border_info;

          /*
            Put a border around the next.
          */
          border_info.width=border_width;
          border_info.height=border_width;
          if (montage_info->frame != (char *) NULL)
            {
              border_info.width=(width-next->columns+1)/2;
              border_info.height=(height-next->rows+1)/2;
            }
          next->orphan=True;
          bordered_next=BorderImage(next,&border_info,exception);
          if (bordered_next != (Image *) NULL)
            {
              DestroyImage(next);
              next=bordered_next;
            }
        }
      /*
        Gravitate next as specified by the tile gravity.
      */
      switch (montage_info->gravity)
      {
        case NorthWestGravity:
        {
          x=0;
          y=0;
          break;
        }
        case NorthGravity:
        {
          x=((width+2*border_width)-(int) next->columns)/2;
          y=0;
          break;
        }
        case NorthEastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=0;
          break;
        }
        case WestGravity:
        {
          x=0;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case ForgetGravity:
        case StaticGravity:
        case CenterGravity:
        default:
        {
          x=((width+2*border_width)-(int) next->columns)/2;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case EastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=((height+2*border_width)-(int) next->rows)/2;
          break;
        }
        case SouthWestGravity:
        {
          x=0;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
        case SouthGravity:
        {
          x=((width+2*border_width)-(int) next->columns)/2;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
        case SouthEastGravity:
        {
          x=(width+2*border_width)-(int) next->columns;
          y=(height+2*border_width)-(int) next->rows;
          break;
        }
      }
      if ((montage_info->frame != (char *) NULL) && (bevel_width != 0))
        {
          FrameInfo
            tile_info;

          Image
            *framed_next;

          /*
            Put an ornamental border around this tile.
          */
          tile_info=frame_info;
          tile_info.width=width+2*frame_info.width;
          tile_info.height=height+2*frame_info.height;
          attribute=GetImageAttribute(next,"label");
          if (attribute != (ImageAttribute *) NULL)
            tile_info.height+=(font_height+4)*MultilineCensus(attribute->value);
          next->orphan=True;
          framed_next=FrameImage(next,&tile_info,exception);
          if (framed_next != (Image *) NULL)
            {
              DestroyImage(next);
              next=framed_next;
            }
          x=0;
          y=0;
        }
      if (LocaleCompare(next->magick,"NULL") != 0)
        {
          /*
            Composite background next with tile next.
          */
          (void) CompositeImage(montage_next,montage_info->compose,next,
            x_offset+x,y_offset+y);
          if (montage_info->shadow)
            {
              register int
                columns,
                rows;

              /*
                Put a shadow under the tile to show depth.
              */
              for (rows=0; rows < ((int) next->rows-4); rows++)
              {
                q=GetImagePixels(montage_next,x+x_offset+next->columns,
                  y_offset+y+rows+4,Min(tile_info.x,4),1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < Min(tile_info.x,4); columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*columns,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage_next))
                  break;
              }
              for (rows=0; rows < Min(tile_info.y,4); rows++)
              {
                q=GetImagePixels(montage_next,x+x_offset+4,y_offset+y+
                  next->rows+rows,next->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (columns=0; columns < (int) next->columns; columns++)
                {
                  Modulate(0.0,0.0,-25.0+4*rows,&q->red,&q->green,&q->blue);
                  q++;
                }
                if (!SyncImagePixels(montage_next))
                  break;
              }
            }
          attribute=GetImageAttribute(next,"label");
          if (attribute != (ImageAttribute *) NULL)
            {
              /*
                Annotate composite next tile with label.
              */
              FormatString(geometry,"%ux%u%+d%+d",
                (montage_info->frame ? next->columns : width)-
                2*border_width,font_height,(int) (x_offset+border_width),
                (int) (montage_info->frame ? y_offset+height+
                2*border_width-bevel_width-2 : y_offset+tile_info.height+
                2*border_width+(montage_info->shadow ? 4 : 0)));
              (void) CloneString(&draw_info->geometry,geometry);
              (void) CloneString(&draw_info->text,attribute->value);
              (void) AnnotateImage(montage_next,draw_info);
            }
        }
      x_offset+=width+(tile_info.x+border_width)*2;
      if (((tile+1) == tiles_per_page) || (((tile+1) % tiles_per_row) == 0))
        {
          x_offset=tile_info.x;
          y_offset+=height+(tile_info.y+border_width)*2+(font_height+4)*
            number_lines+(montage_info->shadow ? 4 : 0);
          max_height=0;
        }
      if (next != next_list[tile])
        DestroyImage(next);
      (void) SetMonitorHandler(handler);
      MagickMonitor(MontageImageText,tiles,total_tiles);
      tiles++;
    }
    montage_next->matte=False;
    if ((i+1) < (int) images_per_page)
      {
        /*
          Allocate next next structure.
        */
        AllocateNextImage(image_info,montage_next);
        if (montage_next->next == (Image *) NULL)
          {
            DestroyImages(montage_next);
            return((Image *) NULL);
          }
        montage_next=montage_next->next;
        next_list+=tiles_per_page;
        number_images-=tiles_per_page;
      }
  }
  if (texture != (Image *) NULL)
    LiberateMemory((void **) &texture);
  LiberateMemory((void **) &master_list);
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(image_info);
  while (montage_next->previous != (Image *) NULL)
    montage_next=montage_next->previous;
  return(montage_next);
}
