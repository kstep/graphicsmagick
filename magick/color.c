/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                       CCCC   OOO   L       OOO   RRRR                       %
%                      C      O   O  L      O   O  R   R                      %
%                      C      O   O  L      O   O  RRRR                       %
%                      C      O   O  L      O   O  R R                        %
%                       CCCC   OOO   LLLLL   OOO   R  R                       %
%                                                                             %
%                                                                             %
%                  Methods to Count the Colors in an Image                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
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

/*
  Include declarations.
*/
#include "studio.h"
#include "color.h"
#include "cache.h"
#include "blob.h"
#include "quantize.h"
#include "monitor.h"
#include "utility.h"

/*
  Define declarations.
*/
#define ColorFilename  "colors.mgk"

#define ColorToNodeId(red,green,blue,index) ((unsigned int) \
            (((ScaleQuantumToChar(red) >> index) & 0x01) << 2 | \
             ((ScaleQuantumToChar(green) >> index) & 0x01) << 1 | \
             ((ScaleQuantumToChar(blue) >> index) & 0x01)))

/*
  Declare color map.
*/
static char
  *ColorMap = (char *) 
    "<?xml version=\"1.0\"?>"
    "<colormap>"
    "  <color name=\"none\" red=\"0\" green=\"0\" blue=\"0\" opacity=\"255\" />"
    "  <color name=\"white\" red=\"255\" green=\"255\" blue=\"255\" />"
    "  <color name=\"red\" red=\"255\" green=\"0\" blue=\"0\" />"
    "  <color name=\"green\" red=\"0\" green=\"255\" blue=\"0\" />"
    "  <color name=\"gray\" red=\"190\" green=\"190\" blue=\"190\" />"
    "  <color name=\"blue\" red=\"0\" green=\"0\" blue=\"255\" />"
    "  <color name=\"cyan\" red=\"0\" green=\"255\" blue=\"255\" />"
    "  <color name=\"magenta\" red=\"255\" green=\"0\" blue=\"255\" />"
    "  <color name=\"yellow\" red=\"255\" green=\"255\" blue=\"0\" />"
    "  <color name=\"black\" red=\"0\" green=\"0\" blue=\"0\" />"
    "</colormap>";

/*
  Structures.
*/
typedef struct _ColorPacket
{
  PixelPacket
    pixel;

  unsigned short
    index;

  unsigned long
    count;
} ColorPacket;

typedef struct _NodeInfo
{
  struct _NodeInfo
    *child[8];

  ColorPacket
    *list;

  unsigned long
    number_unique;

  unsigned char
    level;
} NodeInfo;

typedef struct _Nodes
{
  NodeInfo
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _CubeInfo
{
  NodeInfo
    *root;

  unsigned long
    progress,
    colors,
    free_nodes;

  NodeInfo
    *node_info;

  Nodes
    *node_queue;
} CubeInfo;

/*
  Static declarations.
*/
static SemaphoreInfo
  *color_semaphore = (SemaphoreInfo *) NULL;

static ColorInfo
  *color_list = (ColorInfo *) NULL;

/*
  Forward declarations.
*/
static NodeInfo
  *GetNodeInfo(CubeInfo *,const unsigned int);

static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

static void
  DestroyColorList(const NodeInfo *),
  Histogram(const Image *,CubeInfo *,const NodeInfo *,FILE *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n s t r a i n C o l o r m a p I n d e x                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConstrainColormapIndex() validates the colormap index.  If the index does
%  not range from 0 to the number of colors in the colormap an exception
%  is issued and 0 is returned.
%
%  The format of the ConstrainColormapIndex method is:
%
%      IndexPacket ConstrainColormapIndex(Image *image,const unsigned int index)
%
%  A description of each parameter follows:
%
%    o index: Method ConstrainColormapIndex returns colormap index if it is
%      valid other an exception is issued and 0 is returned.
%
%    o image: The image.
%
%    o index: This integer is the colormap index.
%
%
*/
MagickExport IndexPacket ConstrainColormapIndex(Image *image,
  unsigned long index)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  VerifyColormapIndex(image,index);
  return((IndexPacket) index);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o l o r I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorInfo deallocates memory associated with the color list.
%
%  The format of the DestroyColorInfo method is:
%
%      DestroyColorInfo(void)
%
%
*/
MagickExport void DestroyColorInfo(void)
{
  ColorInfo
    *color_info;

  register ColorInfo
    *p;

  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; )
  {
    color_info=p;
    p=p->next;
    if (color_info->path != (char *) NULL)
      LiberateMemory((void **) &color_info->path);
    if (color_info->name != (char *) NULL)
      LiberateMemory((void **) &color_info->name);
    LiberateMemory((void **) &color_info);
  }
  color_list=(ColorInfo *) NULL;
  DestroySemaphoreInfo(&color_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C u b e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyCubeInfo() deallocates memory associated with a CubeInfo structure.
%
%  The format of the DestroyCubeInfo method is:
%
%      DestroyCubeInfo(CubeInfo *cube_info)
%
%  A description of each parameter follows:
%
%    o cube_info: The address of a structure of type CubeInfo.
%
%
*/
static void DestroyCubeInfo(CubeInfo *cube_info)
{
  register Nodes
    *nodes;

  /*
    Release color cube tree storage.
  */
  DestroyColorList(cube_info->root);
  do
  {
    nodes=cube_info->node_queue->next;
    LiberateMemory((void **) &cube_info->node_queue);
    cube_info->node_queue=nodes;
  } while (cube_info->node_queue != (Nodes *) NULL);
  LiberateMemory((void **) &cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D e s t r o y C o l o r L i s t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorList traverses the color cube tree and frees the list of
%  unique colors.
%
%  The format of the DestroyColorList method is:
%
%      void DestroyColorList(const NodeInfo *node_info)
%
%  A description of each parameter follows.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void DestroyColorList(const NodeInfo *node_info)
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      DestroyColorList(node_info->child[id]);
  if (node_info->list != (ColorPacket *) NULL)
    LiberateMemory((void **) &node_info->list);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   F u z z y C o l o r M a t c h                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FuzzyColorMatch() returns true if two pixels are identical in color.
%
%  The format of the ColorMatch method is:
%
%      void ColorMatch(const PixelPacket *p,const PixelPacket *q,
%        const double fuzz)
%
%  A description of each parameter follows:
%
%    o p: Pixel p.
%
%    o q: Pixel q.
%
%    o distance:  Define how much tolerance is acceptable to consider
%      two colors as the same.
%
%
*/
MagickExport unsigned int FuzzyColorMatch(const PixelPacket *p,
  const PixelPacket *q,const double fuzz)
{
  DoublePixelPacket
    pixel;

  register double
    distance,
    fuzz_squared;

  if ((fuzz == 0.0) && ColorMatch(q,p))
    return(True);
  fuzz_squared=fuzz*fuzz;
  pixel.red=p->red-(double) q->red;
  distance=pixel.red*pixel.red;
  if (distance > (fuzz_squared))
    return(False);
  pixel.green=p->green-(double) q->green;
  distance+=pixel.green*pixel.green;
  if (distance > (fuzz_squared))
    return(False);
  pixel.blue=p->blue-(double) q->blue;
  distance+=pixel.blue*pixel.blue;
  if (distance > (fuzz_squared))
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorInfo searches the color list for the specified name and if
%  found returns attributes for that color.
%
%  The format of the GetColorInfo method is:
%
%      const PixelPacket *GetColorInfo(const char *name,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o color_info: Method GetColorInfo searches the color list for the
%      specified name and if found returns attributes for that color.
%
%    o name: The color name.
%
%    o compliance: Define the required color standard.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const ColorInfo *GetColorInfo(const char *name,
  ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  register char
    *q;

  register ColorInfo
    *p;

  if (color_list == (ColorInfo *) NULL)
    {
      AcquireSemaphoreInfo(&color_semaphore);
      if (color_list == (ColorInfo *) NULL)
        (void) ReadConfigureFile(ColorFilename,0,exception);
      LiberateSemaphoreInfo(&color_semaphore);
    }
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const ColorInfo *) color_list);
  /*
    Search for named color.
  */
  (void) strncpy(colorname,name,MaxTextExtent-1);
  for (q=colorname; *q != '\0'; q++)
  {
    if (*q != ' ')
      continue;
    (void) strcpy(q,q+1);
    q--;
  }
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (LocaleCompare(colorname,p->name) == 0)
      break;
  if (p == (ColorInfo *) NULL)
    ThrowException(exception,OptionWarning,"UnrecognizedColor",name);
  else
    if (p != color_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (ColorInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (ColorInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(ColorInfo *) NULL;
        p->next=color_list;
        color_list->previous=p;
        color_list=p;
      }
  LiberateSemaphoreInfo(&color_semaphore);
  return((const ColorInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r l i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern
%  and color standard.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,unsigned long *number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern and color standard.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/
MagickExport char **GetColorList(const char *pattern,
  unsigned long *number_colors)
{
  char
    **colorlist;

  ExceptionInfo
    exception;

  register long
    i;

  register const ColorInfo
    *p;

  /*
    Allocate color list.
  */
  assert(pattern != (char *) NULL);
  assert(number_colors != (unsigned long *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const ColorInfo *) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    i++;
  colorlist=(char **) AcquireMemory(i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AllocateString(p->name);
  }
  *number_colors=i;
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r T u p l e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorTuple() returns a color as a color tuple string.
%
%  The format of the GetColorTuple method is:
%
%      GetColorTuple(const PixelPacket *color,const unsigned int depth,
%        const unsigned int matte,const unsigned int hex,char *tuple)
%
%  A description of each parameter follows.
%
%    o color: The color.
%
%    o depth: The color depth.
%
%    o matte: A value other than zero returns the opacity in the tuple.
%
%    o hex: A value other than zero returns the tuple in a hexidecimal format.
%
%    o tuple: Return the color tuple as this string.
%
%
*/
MagickExport void GetColorTuple(const PixelPacket *color,
  const unsigned int depth,const unsigned int matte,const unsigned int hex,
  char *tuple)
{
  assert(color != (const PixelPacket *) NULL);
  assert(tuple != (char *) NULL);
  if (matte)
    {
      if (depth <= 8)
        {
          FormatString(tuple,hex ? "#%02X%02X%02X%02X" : "(%3u,%3u,%3u,%3u)",
            ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
            ScaleQuantumToChar(color->blue),ScaleQuantumToChar(color->opacity));
          return;
        }
      if (depth <= 16)
        {
          FormatString(tuple,hex ? "#%04X%04X%04X%04X" : "(%5u,%5u,%5u,%5u)",
            ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
            ScaleQuantumToShort(color->blue),
            ScaleQuantumToShort(color->opacity));
          return;
        }
      FormatString(tuple,
        hex ? "#%08lX%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu,%10lu)",
        ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
        ScaleQuantumToLong(color->blue),ScaleQuantumToLong(color->opacity));
      return;
    }
  if (depth <= 8)
    {
      FormatString(tuple,hex ? "#%02X%02X%02X" : "(%3u,%3u,%3u)",
        ScaleQuantumToChar(color->red),ScaleQuantumToChar(color->green),
        ScaleQuantumToChar(color->blue));
      return;
    }
  if (depth <= 16)
    {
      FormatString(tuple,hex ? "#%04X%04X%04X" : "(%5u,%5u,%5u)",
        ScaleQuantumToShort(color->red),ScaleQuantumToShort(color->green),
        ScaleQuantumToShort(color->blue));
      return;
    }
  FormatString(tuple,hex ? "#%08lX%08lX%08lX" : "(%10lu,%10lu,%10lu)",
    ScaleQuantumToLong(color->red),ScaleQuantumToLong(color->green),
    ScaleQuantumToLong(color->blue));
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C u b e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetCubeInfo initialize the CubeInfo data structure.
%
%  The format of the GetCubeInfo method is:
%
%      cube_info=GetCubeInfo()
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the Cube structure.
%
%
*/
static CubeInfo *GetCubeInfo(void)
{
  CubeInfo
    *cube_info;

  /*
    Initialize tree to describe color cube.
  */
  cube_info=(CubeInfo *) AcquireMemory(sizeof(CubeInfo));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(CubeInfo));
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  return(cube_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t N o d e I n f o                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNodeInfo allocates memory for a new node in the color cube tree
%  and presets all fields to zero.
%
%  The format of the GetNodeInfo method is:
%
%      node_info=GetNodeInfo(cube_info,level)
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o level: Specifies the level in the storage_class the node resides.
%
%
*/
static NodeInfo *GetNodeInfo(CubeInfo *cube_info,const unsigned int level)
{
  NodeInfo
    *node_info;

  if (cube_info->free_nodes == 0)
    {
      Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) AcquireMemory(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((NodeInfo *) NULL);
      nodes->next=cube_info->node_queue;
      cube_info->node_queue=nodes;
      cube_info->node_info=nodes->nodes;
      cube_info->free_nodes=NodesInAList;
    }
  cube_info->free_nodes--;
  node_info=cube_info->node_info++;
  (void) memset(node_info,0,sizeof(NodeInfo));
  node_info->level=level;
  return(node_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t N u m b e r C o l o r s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetNumberColors returns the number of unique colors in an image.
%
%  The format of the GetNumberColors method is:
%
%      unsigned long GetNumberColors(const Image *image,FILE *file,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o file:  Write a histogram of the color distribution to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned long GetNumberColors(const Image *image,FILE *file,
  ExceptionInfo *exception)
{
#define ComputeImageColorsText  "  Compute image colors...  "

  CubeInfo
    *cube_info;

  long
    y;

  NodeInfo
    *node_info;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned int
    id,
    index,
    level;

  unsigned long
    number_colors;

  /*
    Initialize color description tree.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitError,"MemoryAllocationFailed",
        "UnableToDetermineTheNumberOfImageColors");
      return(0);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level <= MaxTreeDepth; level++)
      {
        id=ColorToNodeId(p->red,p->green,p->blue,index);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitError,
                  "MemoryAllocationFailed",
                  "UnableToDetermineTheNumberOfImageColors");
                return(0);
              }
          }
        node_info=node_info->child[id];
        index--;
        if (level != MaxTreeDepth)
          continue;
        for (i=0; i < (long) node_info->number_unique; i++)
          if (ColorMatch(p,&node_info->list[i].pixel))
            break;
        if (i < (long) node_info->number_unique)
          {
            node_info->list[i].count++;
            continue;
          }
        if (node_info->number_unique == 0)
          node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
        else
          ReacquireMemory((void **) &node_info->list,
            (i+1)*sizeof(ColorPacket));
        if (node_info->list == (ColorPacket *) NULL)
          {
            ThrowException(exception,ResourceLimitError,
              "MemoryAllocationFailed",
              "UnableToDetermineTheNumberOfImageColors");
            return(0);
          }
        node_info->list[i].pixel=(*p);
        node_info->list[i].count=1;
        node_info->number_unique++;
        cube_info->colors++;
      }
      p++;
    }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ComputeImageColorsText,y,image->rows,exception))
        break;
  }
  if (file != (FILE *) NULL)
    {
      (void) fprintf(file,"\n");
      Histogram(image,cube_info,cube_info->root,file,exception);
      (void) fflush(file);
    }
  number_colors=cube_info->colors;
  DestroyCubeInfo(cube_info);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  H i s t o g r a m                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Histogram traverses the color cube tree and produces a list of
%  unique pixel field values and the number of times each occurs in the image.
%
%  The format of the Histogram method is:
%
%      void Histogram(const Image *image,CubeInfo *cube_info,
%        const NodeInfo *node_info,FILE *file,ExceptionInfo *exception
%
%  A description of each parameter follows.
%
%    o cube_info: A pointer to the CubeInfo structure.
%
%    o node_info: The address of a structure of type NodeInfo which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Histogram(const Image *image,CubeInfo *cube_info,
  const NodeInfo *node_info,FILE *file,ExceptionInfo *exception)
{
#define HistogramImageText  "  Compute image histogram...  "

  register unsigned int
    id;

  /*
    Traverse any children.
  */
  for (id=0; id < 8; id++)
    if (node_info->child[id] != (NodeInfo *) NULL)
      Histogram(image,cube_info,node_info->child[id],file,exception);
  if (node_info->level == MaxTreeDepth)
    {
      char
        name[MaxTextExtent],
        tuple[MaxTextExtent];

      register ColorPacket
        *p;

      register long
        i;

      p=node_info->list;
      for (i=0; i < (long) node_info->number_unique; i++)
      {
        GetColorTuple(&p->pixel,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"%10lu: %.1024s  ",p->count,tuple);
        (void) fprintf(file,"  ");
        (void) QueryColorname(image,&p->pixel,SVGCompliance,name,exception);
        (void) fprintf(file,"%.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
      if (QuantumTick(cube_info->progress,cube_info->colors))
        (void) MagickMonitor(HistogramImageText,cube_info->progress,
          cube_info->colors,exception);
      cube_info->progress++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsGrayImage() returns True if all the pixels in the image have the same
%  red, green, and blue intensities.
%
%  The format of the IsGrayImage method is:
%
%      unsigned int IsGrayImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsGrayImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->is_grayscale)
    return(True);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=(long) image->columns; x > 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=(long) image->colors; x > 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            return(False);
          p++;
        }
      break;
    }
  }
  ((Image *)image)->is_grayscale=True;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   I s M o n o c h r o m e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsMonochromeImage() returns True if all the pixels in the image have
%  the same red, green, and blue intensities and the intensity is either
%  0 or MaxRGB.
%
%  The format of the IsMonochromeImage method is:
%
%      unsigned int IsMonochromeImage(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsMonochromeImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->is_monochrome)
    return(True);
  switch (image->storage_class)
  {
    case DirectClass:
    case UndefinedClass:
    {
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,exception);
        if (p == (const PixelPacket *) NULL)
          return(False);
        for (x=(long) image->columns; x > 0; x--)
        {
          if ((p->red != p->green) || (p->green != p->blue) ||
              ((p->red != 0) && (p->red != MaxRGB)))
            return(False);
          p++;
        }
      }
      break;
    }
    case PseudoClass:
    {
      p=image->colormap;
      for (x=(long) image->colors; x > 0; x--)
      {
        if ((p->red != p->green) || (p->green != p->blue) ||
            ((p->red != 0) && (p->red != MaxRGB)))
          return(False);
        p++;
      }
      break;
    }
  }
  ((Image *)image)->is_monochrome=True;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s O p a q u e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsOpaqueImage() returns True if none of the pixels in the image have an
%  opacity value other than opaque (0).
%
%  The format of the IsOpaqueImage method is:
%
%      unsigned int IsOpaqueImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method IsOpaqueImage returns False if the image has one or more
%      pixels that are transparent otherwise True is returned.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsOpaqueImage(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  /*
    Determine if image is opaque.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (!image->matte)
    return(True);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=(long) image->columns; x > 0; x--)
    {
      if (p->opacity != OpaqueOpacity)
        return(False);
      p++;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s P a l e t t e I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPaletteImage returns True if the image is PseudoClass and has 256
%  unique colors or less.
%
%  The format of the IsPaletteImage method is:
%
%      unsigned int IsPaletteImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status:  Method IsPaletteImage returns True is the image is
%      PseudoClass or has 256 color or less.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int IsPaletteImage(const Image *image,
  ExceptionInfo *exception)
{
  CubeInfo
    *cube_info;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register NodeInfo
    *node_info;

  register long
    i;

  unsigned long
    index,
    level;

  unsigned int
    id;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->storage_class == PseudoClass) && (image->colors <= 256))
    return(True);
  if (image->storage_class == PseudoClass)
    return(False);
  /*
    Initialize color description tree.
  */
  cube_info=GetCubeInfo();
  if (cube_info == (CubeInfo *) NULL)
    {
      ThrowException(exception,ResourceLimitError,"MemoryAllocationFailed",
        "UnableToDetermineImageClass");
      return(False);
    }
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      return(False);
    for (x=0; x < (long) image->columns; x++)
    {
      /*
        Start at the root and proceed level by level.
      */
      node_info=cube_info->root;
      index=MaxTreeDepth-1;
      for (level=1; level < MaxTreeDepth; level++)
      {
        id=ColorToNodeId(p->red,p->green,p->blue,index);
        if (node_info->child[id] == (NodeInfo *) NULL)
          {
            node_info->child[id]=GetNodeInfo(cube_info,level);
            if (node_info->child[id] == (NodeInfo *) NULL)
              {
                ThrowException(exception,ResourceLimitError,
                  "MemoryAllocationFailed","UnableToDetermineImageClass");
                return(False);
              }
          }
        node_info=node_info->child[id];
        index--;
      }
      for (i=0; i < (long) node_info->number_unique; i++)
        if (ColorMatch(p,&node_info->list[i].pixel))
          break;
      if (i == (long) node_info->number_unique)
        {
          /*
            Add this unique color to the color list.
          */
          if (node_info->number_unique == 0)
            node_info->list=(ColorPacket *) AcquireMemory(sizeof(ColorPacket));
          else
            ReacquireMemory((void **) &node_info->list,
              (i+1)*sizeof(ColorPacket));
          if (node_info->list == (ColorPacket *) NULL)
            {
              ThrowException(exception,ResourceLimitError,
                "MemoryAllocationFailed","UnableToDetermineImageClass");
              return(False);
            }
          node_info->list[i].pixel=(*p);
          node_info->list[i].index=(unsigned short) cube_info->colors++;
          node_info->number_unique++;
          if (cube_info->colors > 256)
            {
              DestroyCubeInfo(cube_info);
              return(False);
            }
        }
      p++;
    }
  }
  DestroyCubeInfo(cube_info);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t C o l o r I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListColorInfo() lists color names to the specified file.  Color names
%  are a convenience.  Rather than defining a color by its red, green, and
%  blue intensities just use a color name such as white, blue, or yellow.
%
%  The format of the ListColorInfo method is:
%
%      unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  List color names to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
{
  register long
    i;

  register const ColorInfo
    *p;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetColorInfo("*",exception);
  AcquireSemaphoreInfo(&color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if ((p->previous == (ColorInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (ColorInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,
          "Name                   Color                   Compliance\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 22; i++)
      (void) fprintf(file," ");
    if (p->color.opacity == OpaqueOpacity)
      (void) fprintf(file,"%5d,%5d,%5d       ",p->color.red,p->color.green,
        p->color.blue);
    else
      (void) fprintf(file,"%5d,%5d,%5d,%5d ",p->color.red,p->color.green,
        p->color.blue,p->color.opacity);
    if (p->compliance & SVGCompliance)
      (void) fprintf(file,"SVG ");
    if (p->compliance & X11Compliance)
      (void) fprintf(file,"X11 ");
    if (p->compliance & XPMCompliance)
      (void) fprintf(file,"XPM ");
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&color_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorDatabase() returns the red, green, blue, and opacity intensities
%  for a given color name.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *name,PixelPacket *color,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The color name (e.g. white, blue, yellow).
%
%    o color: The red, green, blue, and opacity intensities values of the
%      named color in this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorDatabase(const char *name,
  PixelPacket *color,ExceptionInfo *exception)
{
  double
    scale;

  DoublePixelPacket
    pixel;

  register const ColorInfo
    *p;

  register long
    i;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  (void) memset(color,0,sizeof(PixelPacket));
  color->opacity=TransparentOpacity;
  if ((name == (char *) NULL) || (*name == '\0'))
    name=BackgroundColor;
  while (isspace((int) (*name)))
    name++;
  if (*name == '#')
    {
      char
        c;

      long
        n;

      LongPixelPacket
        pixel;

      memset(&pixel,0,sizeof(LongPixelPacket));
      name++;
      for (n=0; isxdigit((int) name[n]); n++);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            pixel.red=pixel.green;
            pixel.green=pixel.blue;
            pixel.blue=0;
            for (i=n-1; i >= 0; i--)
            {
              c=(*name++);
              pixel.blue<<=4;
              if ((c >= '0') && (c <= '9'))
                pixel.blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  pixel.blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    pixel.blue|=c-('a'-10);
                  else
                    return(False);
            }
          } while (isxdigit((int) *name));
        }
      else
        if ((n != 4) && (n != 8) && (n != 16))
          return(False);
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              pixel.red=pixel.green;
              pixel.green=pixel.blue;
              pixel.blue=pixel.opacity;
              pixel.opacity=0;
              for (i=n-1; i >= 0; i--)
              {
                c=(*name++);
                pixel.opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  pixel.opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    pixel.opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      pixel.opacity|=c-('a'-10);
                    else
                      return(False);
              }
            } while (isxdigit((int) *name));
          }
      n<<=2;
      color->red=(Quantum)
        (((double) MaxRGB*pixel.red)/((1 << n)-1)+0.5);
      color->green=(Quantum)
        (((double) MaxRGB*pixel.green)/((1 << n)-1)+0.5);
      color->blue=(Quantum)
        (((double) MaxRGB*pixel.blue)/((1 << n)-1)+0.5);
      color->opacity=OpaqueOpacity;
      if ((n != 3) && (n != 6) && (n != 9) && (n != 12))
        color->opacity=(Quantum)
          (((double) MaxRGB*pixel.opacity)/((1 << n)-1)+0.5);
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  if (LocaleNCompare(name,"rgba(",5) == 0)
    {
      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue,&pixel.opacity);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=ScaleCharToQuantum(scale*pixel.opacity);
      return(True);
    }
  p=GetColorInfo(name,exception);
  if (p == (const ColorInfo *) NULL)
    return(False);
  if ((LocaleCompare(p->name,"opaque") == 0) ||
      (LocaleCompare(p->name,"transparent") == 0))
    {
      color->opacity=p->color.opacity;
      return(True);
    }
  *color=p->color;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorname() returns a named color for the given color intensity.  If
%  an exact match is not found, a hex value is return instead.  For example
%  an intensity of rgb:(0,0,0) returns black whereas rgb:(223,223,223)
%  returns #dfdfdf.
%
%  The format of the QueryColorname method is:
%
%      unsigned int QueryColorname(const Image *image,const PixelPacket *color,
%        const ComplianceType compliance,char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o color: The color intensities.
%
%    o Compliance: Adhere to this color standard: SVG, X11, or XPM.
%
%    o name: Return the color name or hex value.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int QueryColorname(const Image *image,
  const PixelPacket *color,const ComplianceType compliance,char *name,
  ExceptionInfo *exception)
{
  register const ColorInfo
    *p;

  *name='\0';
  p=GetColorInfo("*",exception);
  if (p != (const ColorInfo *) NULL)
    {
      for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
      {
        if (!(p->compliance & compliance))
          continue;
        if ((p->color.red != color->red) || (p->color.green != color->green) ||
            (p->color.blue != color->blue) ||
            (p->color.opacity != color->opacity))
          continue;
        (void) strncpy(name,p->name,MaxTextExtent-1);
        return(True);
      }
    }
  GetColorTuple(color,image->depth,image->matte,True,name);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ReadConfigureFile(const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  size_t
    length;

  /*
    Read the color configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetConfigureBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(ColorMap);
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strncpy(keyword,token,MaxTextExtent-1);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        /*
          Comment element.
        */
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        continue;
      }
    if (LocaleCompare(keyword,"<include") == 0)
      {
        /*
          Include element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strncpy(keyword,token,MaxTextExtent-1);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,
                  "IncludeElementNestedTooDeeply",path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strcat(filename,DirectorySeparator);
                  (void) strncat(filename,token,MaxTextExtent-
                    strlen(filename)-1);
                  (void) ReadConfigureFile(filename,depth+1,exception);
                }
              if (color_list != (ColorInfo *) NULL)
                while (color_list->next != (ColorInfo *) NULL)
                  color_list=color_list->next;
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<color") == 0)
      {
        ColorInfo
          *color_info;

        /*
          Allocate memory for the color list.
        */
        color_info=(ColorInfo *) AcquireMemory(sizeof(ColorInfo));
        if (color_info == (ColorInfo *) NULL)
          MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
            "UnableToAllocateColorInfo");
        (void) memset(color_info,0,sizeof(ColorInfo));
        color_info->path=AcquireString(path);
        color_info->signature=MagickSignature;
        if (color_list == (ColorInfo *) NULL)
          {
            color_list=color_info;
            continue;
          }
        color_list->next=color_info;
        color_info->previous=color_list;
        color_list=color_list->next;
        continue;
      }
    if (color_list == (ColorInfo *) NULL)
      continue;
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
    GetToken(q,&q,token);
    GetToken(q,&q,token);
    switch (*keyword)
    {
      case 'B':
      case 'b':
      {
        if (LocaleCompare((char *) keyword,"blue") == 0)
          {
            color_list->color.blue=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'C':
      case 'c':
      {
        if (LocaleCompare((char *) keyword,"compliance") == 0)
          {
            long
              compliance;

            compliance=color_list->compliance;
            if (GlobExpression(token,"*SVG*"))
              compliance|=SVGCompliance;
            if (GlobExpression(token,"*X11*"))
              compliance|=X11Compliance;
            if (GlobExpression(token,"*XPM*"))
              compliance|=XPMCompliance;
            color_list->compliance=(ComplianceType) compliance;
            break;
          }
        break;
      }
      case 'G':
      case 'g':
      {
        if (LocaleCompare((char *) keyword,"green") == 0)
          {
            color_list->color.green=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'N':
      case 'n':
      {
        if (LocaleCompare((char *) keyword,"name") == 0)
          {
            color_list->name=AcquireString(token);
            break;
          }
        break;
      }
      case 'O':
      case 'o':
      {
        if (LocaleCompare((char *) keyword,"opacity") == 0)
          {
            color_list->color.opacity=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'R':
      case 'r':
      {
        if (LocaleCompare((char *) keyword,"red") == 0)
          {
            color_list->color.red=ScaleCharToQuantum(atol(token));
            break;
          }
        break;
      }
      case 'S':
      case 's':
      {
        if (LocaleCompare((char *) keyword,"stealth") == 0)
          {
            color_list->stealth=LocaleCompare(token,"True") == 0;
            break;
          }
        break;
      }
      default:
        break;
    }
  }
  LiberateMemory((void **) &token);
  LiberateMemory((void **) &xml);
  if (color_list == (ColorInfo *) NULL)
    return(False);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(True);
}
