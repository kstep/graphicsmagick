/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      CCCC   AAA    CCCC  H   H  EEEEE                       %
%                     C      A   A  C      H   H  E                           %
%                     C      AAAAA  C      HHHHH  EEE                         %
%                     C      A   A  C      H   H  E                           %
%                      CCCC  A   A   CCCC  H   H  EEEEE                       %
%                                                                             %
%                                                                             %
%                      ImageMagick Pixel Cache Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
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
#include "defines.h"

/*
  Global declarations.
*/
static size_t
  free_memory = PixelCacheThreshold*1024*1024;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClosePixelCache closes the file handle associated with a disk pixel
%  cache.
%
%  The format of the ClosePixelCache method is:
%
%      void ClosePixelCache(CacheInfo *cache_info)
%
%  A description of each parameter follows:
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%
*/
Export void ClosePixelCache(CacheInfo *cache_info)
{
  assert(cache_info != (CacheInfo *) NULL);
  if (cache_info->file != (FILE *) NULL)
    (void) fclose(cache_info->file);
  cache_info->file=(FILE *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y C a c h e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyCacheInfo deallocates memory associated with an CacheInfo
%  structure.
%
%  The format of the DestroyCacheInfo method is:
%
%      void DestroyCacheInfo(CacheInfo *cache_info)
%
%  A description of each parameter follows:
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%
*/
Export void DestroyCacheInfo(CacheInfo *cache_info)
{
  size_t
    length;
    
  assert(cache_info != (CacheInfo *) NULL);
  ClosePixelCache(cache_info);
  if (cache_info->mapped)
    {
      /*
        Unmap memory-mapped pixels and indexes.
      */
      length=cache_info->number_pixels*sizeof(PixelPacket);
      if (cache_info->class == PseudoClass)
        length+=cache_info->number_pixels*sizeof(IndexPacket);
      (void) UnmapBlob(cache_info->pixels,length);
      cache_info->pixels=(PixelPacket *) NULL;
      cache_info->indexes=(IndexPacket *) NULL;
    }
  if (*cache_info->filename != '\0')
    (void) remove(cache_info->filename);
  if (cache_info->pixels != (PixelPacket *) NULL)
    {
      /*
        Free pixels.
      */
      FreeMemory(cache_info->pixels);
      free_memory+=cache_info->number_pixels*sizeof(PixelPacket);
    }
  if (cache_info->indexes != (IndexPacket *) NULL)
    {
      /*
        Free colormap indexes.
      */
      FreeMemory(cache_info->indexes);
      free_memory+=cache_info->number_pixels*sizeof(IndexPacket);
    }
  GetCacheInfo(cache_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C a c h e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCacheInfo initializes the CacheInfo structure.
%
%  The format of the GetCacheInfo method is:
%
%      void GetCacheInfo(CacheInfo *cache_info)
%
%  A description of each parameter follows:
%
%    o cache_info: Specifies a pointer to a CacheInfo structure.
%
%
*/
Export void GetCacheInfo(CacheInfo *cache_info)
{
  assert(cache_info != (CacheInfo *) NULL);
  *cache_info->filename='\0';
  cache_info->file=(FILE *) NULL;
  cache_info->class=UndefinedClass;
  cache_info->mapped=False;
  cache_info->pixels=(PixelPacket *) NULL;
  cache_info->indexes=(IndexPacket *) NULL;
  cache_info->number_pixels=0;
  cache_info->x=0;
  cache_info->y=0;
  cache_info->width=0;
  cache_info->height=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelCache gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the GetPixelCache method is:
%
%      PixelPacket *GetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
Export PixelPacket *GetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  off_t
    offset;

  register PixelPacket
    *q;

  register int
    i;

  size_t
    count;

  assert(image != (Image *) NULL);
  if ((x == image->cache_info.x) && (y == image->cache_info.y) &&
      (columns == image->cache_info.width) &&
      (rows == image->cache_info.height))
    return(image->pixels);
  /*
    Transfer pixels from the pixel cache.
  */
  if (!SetPixelCache(image,x,y,columns,rows))
    return((PixelPacket *) NULL);
  offset=y*image->columns+x;
  q=image->pixels;
  for (i=0; i < (int) rows; i++)
  {
    if (image->cache_info.pixels != (PixelPacket *) NULL)
      (void) memcpy(q,image->cache_info.pixels+offset,
        columns*sizeof(PixelPacket));
    else
      {
        count=fseek(image->cache_info.file,offset*sizeof(PixelPacket),SEEK_SET);
        if (count != 0)
          break;
        count=fread(q,sizeof(PixelPacket),columns,image->cache_info.file);
        if (count != columns)
          break;
      }
    q+=columns;
    offset+=image->columns;
  }
  if (i < (int) rows)
    {
      MagickWarning(CacheWarning,"Unable to get pixels from cache",
        (char *) NULL);
      return((PixelPacket *) NULL);
    }
  if (image->class == PseudoClass)
    {
      register IndexPacket
        *q;

      /*
        Transfer colormap indexes from the pixel cache.
      */
      offset=y*image->columns+x;
      q=image->indexes;
      for (i=0; i < (int) rows; i++)
      {
        if (image->cache_info.indexes != (IndexPacket *) NULL)
          (void) memcpy(q,image->cache_info.indexes+offset,
            columns*sizeof(IndexPacket));
        else
          {
            count=fseek(image->cache_info.file,image->cache_info.number_pixels*
              sizeof(PixelPacket)+offset*sizeof(IndexPacket),SEEK_SET);
            if (count != 0)
              break;
            count=fread(q,sizeof(IndexPacket),columns,image->cache_info.file);
            if (count != columns)
              break;
          }
        q+=columns;
        offset+=image->columns;
      }
    }
  if (i < (int) rows)
    {
      MagickWarning(CacheWarning,"Unable to get pixels from cache",
        (char *) NULL);
      return((PixelPacket *) NULL);
    }
  return(image->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenPixelCache returns True if the pixel cache is already
%  opened.  Otherwise the pixel cache is created in memory or disk.
%  False is returned if there is not enough memory or a disk error
%  occurs.
%
%  The format of the OpenPixelCache method is:
%
%      unsigned int OpenPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method OpenPixelCache returns True if the image pixel
%      cache is already created or succesfully created in memory or on
%      disk.
%
%    o image: The address of a structure of type Image.
%
%
*/
static unsigned int OpenPixelCache(Image *image)
{
  int
    status;

  off_t
    offset;

  size_t
    length,
    number_pixels;

  assert(image != (Image *) NULL);
  number_pixels=image->columns*image->rows;
  if (image->cache_info.class == UndefinedClass)
    {
      length=number_pixels*sizeof(PixelPacket);
      if (image->class == PseudoClass)
        length+=number_pixels*sizeof(IndexPacket);
      if (length <= free_memory)
        {
          /*
            Create in-memory pixel cache.
          */
          image->cache_info.pixels=(PixelPacket *)
            AllocateMemory(number_pixels*sizeof(PixelPacket));
          if (image->cache_info.pixels != (PixelPacket *) NULL)
            {
              image->cache_info.class=DirectClass;
              free_memory-=number_pixels*sizeof(PixelPacket);
              if (image->class == PseudoClass)
                {
                  image->cache_info.indexes=(IndexPacket *)
                    AllocateMemory(number_pixels*sizeof(IndexPacket));
                  if (image->cache_info.indexes != (IndexPacket *) NULL)
                    {
                      image->cache_info.class=PseudoClass;
                      free_memory-=number_pixels*sizeof(IndexPacket);
                    }
                }
            }
        }
      if (image->cache_info.class == UndefinedClass)
        {
          char
            filename[MaxTextExtent];

          /*
            Create pixel cache on disk.
          */
          TemporaryFilename(filename);
          image->cache_info.file=fopen(filename,"wb+");
          if (image->cache_info.file != (FILE *) NULL)
            {
              status=fseek(image->cache_info.file,length-1,SEEK_SET);
              if (status == 0)
                {
                  image->cache_info.class=DirectClass;
                  (void) strcpy(image->cache_info.filename,filename);
                  (void) fputc(0,image->cache_info.file);
                  (void) fclose(image->cache_info.file);
                  image->cache_info.file=(FILE *) NULL;
                  image->cache_info.pixels=(PixelPacket *)
                    MapBlob(image->cache_info.filename,IOMode,&length);
                  if (image->cache_info.pixels == (PixelPacket *) NULL)
                    {
                      if (image->class == PseudoClass)
                        image->cache_info.class=PseudoClass;
                    }
                  else
                    {
                      image->cache_info.mapped=True;
                      if (image->class == PseudoClass)
                        {
                          image->cache_info.class=PseudoClass;
                          image->cache_info.indexes=(IndexPacket *)
                            (image->cache_info.pixels+number_pixels);
                        }
                    }
                }
            }
        }
      image->cache_info.number_pixels=number_pixels;
    }
  if (image->cache_info.class == UndefinedClass)
    return(False);
  if (image->class == PseudoClass)
    {
      if (image->cache_info.class != PseudoClass)
        {
          if (*image->cache_info.filename == '\0')
            {
              /*
                Create in-memory colormap index cache.
              */
              image->cache_info.indexes=(IndexPacket *)
                AllocateMemory(number_pixels*sizeof(IndexPacket));
              if (image->cache_info.indexes != (IndexPacket *) NULL)
                {
                  image->cache_info.class=PseudoClass;
                  free_memory-=number_pixels*sizeof(IndexPacket);
                }
            }
          else
            {
              /*
                Create colormap index cache on disk.
              */
              if (image->cache_info.file == (FILE *) NULL)
                image->cache_info.file=fopen(image->cache_info.filename,"rb+");
              if (image->cache_info.file != (FILE *) NULL)
                {
                  length=number_pixels*sizeof(PixelPacket)+
                    number_pixels*sizeof(IndexPacket);
                  status=fseek(image->cache_info.file,length-1,SEEK_SET);
                  if (status == 0)
                    {
                      image->cache_info.class=PseudoClass;
                      (void) fputc(0,image->cache_info.file);
                      (void) fclose(image->cache_info.file);
                      image->cache_info.file=(FILE *) NULL;
                      offset=number_pixels*sizeof(PixelPacket);
                      if (image->cache_info.mapped)
                        (void) UnmapBlob(image->cache_info.pixels,offset);
                      image->cache_info.pixels=(PixelPacket *)
                        MapBlob(image->cache_info.filename,IOMode,&length);
                      image->cache_info.mapped=
                        image->cache_info.pixels != (PixelPacket *) NULL;
                      if (image->cache_info.mapped)
                        image->cache_info.indexes=(IndexPacket *)
                          (image->cache_info.pixels+number_pixels);
                    }
                }
            }
        }
      if (image->cache_info.class != PseudoClass)
        return(False);
    }
  if ((image->cache_info.pixels == (PixelPacket *) NULL) &&
      (image->cache_info.file == (FILE *) NULL))
    {
      /*
        Open disk pixel cache.
      */
      image->cache_info.file=fopen(image->cache_info.filename,"rb+");
      if (image->cache_info.file == (FILE *) NULL)
        return(False);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPixelCache transfers one or more pixel components from a buffer
%  or file into the image pixel buffer of an image.
%
%  The format of the ReadPixelCache method is:
%
%      void ReadPixelCache(Image *image,QuantumTypes quantum,
%        unsigned char *source)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o quantum: Declare which pixel components to transfer (red, green, blue,
%      opacity, RGB, or RGBA).
%
%    o source:  The pixel components are transferred from this buffer.
%
*/
void ReadPixelCache(Image *image,QuantumTypes quantum,unsigned char *source)
{
  register int
    x;

  register PixelPacket
    *q;

  unsigned char
    *p;

  assert(image != (Image *) NULL);
  p=source;
  q=image->pixels;
  switch (quantum)
  {
    case IndexQuantum:
    case GrayQuantum:
    {
      register IndexPacket
        index;

      if (image->colors <= 256)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                index=UpScale(ReadByte(image));
                image->indexes[x]=index;
                q->red=image->colormap[index].red;
                q->green=image->colormap[index].green;
                q->blue=image->colormap[index].blue;
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            index=UpScale(*p++);
            image->indexes[x]=index;
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            image->indexes[x]=index;
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        index=(*p++ << 8) | (*p++);
        image->indexes[x]=index;
        q->red=image->colormap[index].red;
        q->green=image->colormap[index].green;
        q->blue=image->colormap[index].blue;
        q++;
      }
      break;
    }
    case IndexOpacityQuantum:
    {
      if (image->colors <= 256)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                image->indexes[x]=UpScale(ReadByte(image));
                q->red=image->colormap[x].red;
                q->green=image->colormap[x].green;
                q->blue=image->colormap[x].blue;
                q->opacity=UpScale(*p++);
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            image->indexes[x]=UpScale(*p++);
            q->red=image->colormap[x].red;
            q->green=image->colormap[x].green;
            q->blue=image->colormap[x].blue;
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            image->indexes[x]=
              MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->red=image->colormap[x].red;
            q->green=image->colormap[x].green;
            q->blue=image->colormap[x].blue;
            q->opacity=
              MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        image->indexes[x]=(*p++ << 8) | (*p++);
        q->red=image->colormap[x].red;
        q->green=image->colormap[x].green;
        q->blue=image->colormap[x].blue;
        q->opacity=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case GrayOpacityQuantum:
    {
      if (image->colors <= 256)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=UpScale(ReadByte(image));
                q->green=q->red;
                q->blue=q->red;
                q->opacity=UpScale(*p++);
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->red=q->red;
            q->green=q->red;
            q->blue=q->red;
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->green=q->red;
            q->blue=q->red;
            q->opacity=
              MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p << 8) | (*(p+1));
        q->green=q->red;
        q->blue=q->red;
        q->opacity=(*(p+2) << 8) | (*(p+3));
        p+=4;
        q++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case GreenQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->green=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->green=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->green=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->green=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case BlueQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->blue=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->blue=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->blue=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case OpacityQuantum:
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->opacity=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->opacity=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->opacity=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=UpScale(ReadByte(image));
                q->green=UpScale(ReadByte(image));
                q->blue=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->green=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->blue=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8) | (*p++);
        q->green=(*p++ << 8) | (*p++);
        q->blue=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
    case RGBAQuantum:
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          if (source == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=UpScale(ReadByte(image));
                q->green=UpScale(ReadByte(image));
                q->blue=UpScale(ReadByte(image));
                q->opacity=UpScale(ReadByte(image));
                q++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->opacity=UpScale(*p++);
            q++;
          }
          break;
        }
      if (source == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->green=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->blue=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q->opacity=MSBFirstReadShort(image) >> (image->depth-QuantumDepth);
            q++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=(*p++ << 8) | (*p++);
        q->green=(*p++ << 8) | (*p++);
        q->blue=(*p++ << 8) | (*p++);
        q->opacity=(*p++ << 8) | (*p++);
        q++;
      }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C a c h e T h e s h o l d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheThreshold sets the amount of free memory allocated for the
%  pixel cache.  Once this threshold is exceeded, all subsequent pixels cache
%  operations are to/from disk.
%
%  The format of the SetCacheThreshold method is:
%
%      void SetCacheThreshold(unsigned int threshold)
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
Export void SetCacheThreshold(unsigned int threshold)
{
  free_memory=threshold*1024*1024;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t P i x e l C a c h e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelCache allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with the GetCacheInfo() method
%  or to the cache with SyncPixelCache(). A pointer to the pixels is returned
%  if the pixels are transferred, otherwise a NULL is returned.
%
%  The format of the SetPixelCache method is:
%
%      PixelPacket *SetPixelCache(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelCache returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
Export PixelPacket *SetPixelCache(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  size_t
    number_pixels;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+columns) > (int) image->columns) ||
      ((y+rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      MagickWarning(CacheWarning,"Unable to set pixel cache",
        "image does not contain the cache geometry");
      return((PixelPacket *) NULL);
    }
  if (!OpenPixelCache(image))
    {
      MagickWarning(CacheWarning,"Unable to initialize pixel cache",
        (char *) NULL);
      return((PixelPacket *) NULL);
    }
  /*
    Allocate buffer to get/put pixels from the pixel cache.
  */
  number_pixels=columns*rows;
  if (image->pixels == (PixelPacket *) NULL)
    image->pixels=(PixelPacket *)
      AllocateMemory(number_pixels*sizeof(PixelPacket));
  else
    if ((image->cache_info.width*image->cache_info.height) < number_pixels)
      image->pixels=(PixelPacket *)
        ReallocateMemory(image->pixels,number_pixels*sizeof(PixelPacket));
  /*
    Allocate buffer to get/put colormap indexes from the pixel cache.
  */
  if (image->class == PseudoClass)
    if (image->indexes == (IndexPacket *) NULL)
      image->indexes=(IndexPacket *)
        AllocateMemory(number_pixels*sizeof(IndexPacket));
  if (image->indexes != (IndexPacket *) NULL)
    if ((image->cache_info.width*image->cache_info.height) < number_pixels)
      image->indexes=(IndexPacket *) ReallocateMemory(image->indexes,
        number_pixels*sizeof(IndexPacket));
  if (((image->class == PseudoClass) &&
       (image->indexes == (IndexPacket *) NULL)) ||
      (image->pixels == (PixelPacket *) NULL))
    {
      MagickWarning(CacheWarning,"Unable to set pixel cache",
        "Memory allocation failed");
      return((PixelPacket *) NULL);
    }
  /*
    Set the pixel cache geometry.
  */
  image->cache_info.x=x;
  image->cache_info.y=y;
  image->cache_info.width=columns;
  image->cache_info.height=rows;
  return(image->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c P i x e l C a c h e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncPixelCache saves the image pixels to the in-memory or disk
%  cache.  The method returns True if the pixel region is set, otherwise False.
%
%  The format of the SyncPixelCache method is:
%
%      unsigned int SyncPixelCache(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelCache returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int SyncPixelCache(Image *image)
{
  off_t
    offset;

  register int
    y;

  register PixelPacket
    *p;

  size_t
    count;

  assert(image != (Image *) NULL);
  p=SetPixelCache(image,image->cache_info.x,image->cache_info.y,
    image->cache_info.width,image->cache_info.height);
  if (p == (PixelPacket *) NULL)
    return(False);
  /*
    Transfer pixels to pixel cache.
  */
  image->tainted=True;
  offset=image->cache_info.y*image->columns+image->cache_info.x;
  for (y=0; y < (int) image->cache_info.height; y++)
  {
    if (image->cache_info.pixels != (PixelPacket *) NULL)
      (void) memcpy(image->cache_info.pixels+offset,p,
        image->cache_info.width*sizeof(PixelPacket));
    else
      {
        count=fseek(image->cache_info.file,offset*sizeof(PixelPacket),SEEK_SET);
        if (count != 0)
          break;
        count=fwrite(p,sizeof(PixelPacket),image->cache_info.width,
          image->cache_info.file);
        if (count != image->cache_info.width)
          break;
      }
    p+=image->cache_info.width;
    offset+=image->columns;
  }
  if (y < (int) image->cache_info.height)
    {
      MagickWarning(CacheWarning,"Unable to sync pixel cache",(char *) NULL);
      return(False);
    }
  if (image->class == PseudoClass)
    {
      register IndexPacket
        *p;

      /*
        Transfer colormap indexes to pixel cache.
      */
      offset=image->cache_info.y*image->columns+image->cache_info.x;
      p=image->indexes;
      for (y=0; y < (int) image->cache_info.height; y++)
      {
        if (image->cache_info.indexes != (IndexPacket *) NULL)
          (void) memcpy(image->cache_info.indexes+offset,p,
            image->cache_info.width*sizeof(IndexPacket));
        else
          {
            count=fseek(image->cache_info.file,image->cache_info.number_pixels*
              sizeof(PixelPacket)+offset*sizeof(IndexPacket),SEEK_SET);
            if (count != 0)
              break;
            count=fwrite(p,sizeof(IndexPacket),image->cache_info.width,
              image->cache_info.file);
            if (count != image->cache_info.width)
              break;
          }
        p+=image->cache_info.width;
        offset+=image->columns;
      }
    }
  if (y < (int) image->cache_info.height)
    {
      MagickWarning(CacheWarning,"Unable to sync pixel cache",(char *) NULL);
      return(False);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P i x e l C a c h e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePixelCache transfers one or more pixel components from the image
%  pixel buffer to a buffer or file.
%
%  The format of the WritePixelCache method is:
%
%      void WritePixelCache(Image *,QuantumTypes quantum,
%        unsigned char *destination)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o quantum: Declare which pixel components to transfer (red, green, blue,
%      opacity, RGB, or RGBA).
%
%    o destination:  The components are transferred to this buffer.
%
%
*/
void WritePixelCache(Image *image,QuantumTypes quantum,
  unsigned char *destination)
{
  register int
    x;

  register PixelPacket
    *p;

  unsigned char
    *q;

  assert(image != (Image *) NULL);
  p=image->pixels;
  q=destination;
  switch (quantum)
  {
    case IndexQuantum:
    {
      if (image->colors <= 256)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
                (void) WriteByte(image,image->indexes[x]);
              break;
            }
          for (x=0; x < (int) image->columns; x++)
            *q++=image->indexes[x];
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
            MSBFirstWriteShort(image,image->indexes[x]);
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=image->indexes[x] >> 8;
        *q++=image->indexes[x] & 0xff;
      }
      break;
    }
    case GrayQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,Intensity(*p));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=Intensity(*p);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*Intensity(*p));
            else
              MSBFirstWriteShort(image,Intensity(*p));
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=Intensity(*p) >> 8;
        *q++=Intensity(*p) & 0xff;
        p++;
      }
      break;
    }
    case IndexOpacityQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,image->indexes[x]);
                (void) WriteByte(image,p->opacity);
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=image->indexes[x];
            *q++=p->opacity;
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*image->indexes[x]);
            else
              MSBFirstWriteShort(image,image->indexes[x]);
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->opacity);
            else
              MSBFirstWriteShort(image,p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=image->indexes[x] >> 8;
        *q++=image->indexes[x] & 0xff;
        *q++=p->opacity >> 8;
        *q++=p->opacity & 0xff;
        p++;
      }
      break;
    }
    case GrayOpacityQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,Intensity(*p));
                (void) WriteByte(image,p->opacity);
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=Intensity(*p);
            *q++=p->opacity;
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*Intensity(*p));
            else
              MSBFirstWriteShort(image,Intensity(*p));
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->opacity);
            else
              MSBFirstWriteShort(image,p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=Intensity(*p) >> 8;
        *q++=Intensity(*p) & 0xff;
        *q++=p->opacity >> 8;
        *q++=p->opacity & 0xff;
        p++;
      }
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->red));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->red);
            else
              MSBFirstWriteShort(image,p->red);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red & 0xff;
        p++;
      }
      break;
    }
    case GreenQuantum:
    case YellowQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->green));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->green);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->green);
            else
              MSBFirstWriteShort(image,p->green);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->green >> 8;
        *q++=p->green & 0xff;
        p++;
      }
      break;
    }
    case BlueQuantum:
    case MagentaQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->blue));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->blue);
            else
              MSBFirstWriteShort(image,p->blue);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->blue >> 8;
        *q++=p->blue & 0xff;
        p++;
      }
      break;
    }
    case OpacityQuantum:
    case BlackQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->opacity));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              MSBFirstWriteShort(image,257*p->opacity);
            else
              MSBFirstWriteShort(image,p->opacity);
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->opacity >> 8;
        *q++=p->opacity & 0xff;
        p++;
      }
      break;
    }
    case RGBQuantum:
    default:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->red));
                (void) WriteByte(image,DownScale(p->green));
                (void) WriteByte(image,DownScale(p->blue));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              {
                MSBFirstWriteShort(image,257*p->red);
                MSBFirstWriteShort(image,257*p->green);
                MSBFirstWriteShort(image,257*p->blue);
              }
            else
              {
                MSBFirstWriteShort(image,p->red);
                MSBFirstWriteShort(image,p->green);
                MSBFirstWriteShort(image,p->blue);
              }
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red & 0xff;
        *q++=p->green >> 8;
        *q++=p->green & 0xff;
        *q++=p->blue >> 8;
        *q++=p->blue & 0xff;
        p++;
      }
      break;
    }
    case RGBAQuantum:
    case CMYKQuantum:
    {
      if (image->depth <= 8)
        {
          if (destination == (unsigned char *) NULL)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                (void) WriteByte(image,DownScale(p->red));
                (void) WriteByte(image,DownScale(p->green));
                (void) WriteByte(image,DownScale(p->blue));
                (void) WriteByte(image,DownScale(p->opacity));
                p++;
              }
              break;
            }
          for (x=0; x < (int) image->columns; x++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->opacity);
            p++;
          }
          break;
        }
      if (destination == (unsigned char *) NULL)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if ((QuantumDepth-image->depth) > 0)
              {
                MSBFirstWriteShort(image,257*p->red);
                MSBFirstWriteShort(image,257*p->green);
                MSBFirstWriteShort(image,257*p->blue);
                MSBFirstWriteShort(image,257*p->opacity);
              }
            else
              {
                MSBFirstWriteShort(image,p->red);
                MSBFirstWriteShort(image,p->green);
                MSBFirstWriteShort(image,p->blue);
                MSBFirstWriteShort(image,p->opacity);
              }
            p++;
          }
          break;
        }
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=p->red >> 8;
        *q++=p->red & 0xff;
        *q++=p->green >> 8;
        *q++=p->green & 0xff;
        *q++=p->blue >> 8;
        *q++=p->blue & 0xff;
        *q++=p->opacity >> 8;
        *q++=p->opacity & 0xff;
        p++;
      }
      break;
    }
  }
}
