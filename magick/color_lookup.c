/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% GraphicsMagick Color Lookup Functions
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/semaphore.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define ColorFilename  "colors.mgk"

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
static unsigned int
  ReadColorConfigureFile(const char *,const unsigned long,ExceptionInfo *);

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
    MagickFreeMemory(color_info->path);
    MagickFreeMemory(color_info->name);
    MagickFreeMemory(color_info);
  }
  color_list=(ColorInfo *) NULL;
  LiberateSemaphoreInfo(&color_semaphore);
  DestroySemaphoreInfo(&color_semaphore);
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
        (void) ReadColorConfigureFile(ColorFilename,0,exception);
      LiberateSemaphoreInfo(&color_semaphore);
    }
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const ColorInfo *) color_list);
  /*
    Search for named color.
  */
  (void) strlcpy(colorname,name,MaxTextExtent);
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
    ThrowException(exception,OptionWarning,UnrecognizedColor,name);
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
%   G e t C o l o r I n f o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorInfoArray() returns a sorted null-terminated array of ColorInfo
%  pointers corresponding to the available color definitions. This function
%  should be used to access the entire list rather than GetColorInfo since
%  the list returned by GetColorInfo may be re-ordered every time it is
%  invoked. GetColorList may be used if only a list of color names is desired.
%  The array should be deallocated by the user once it is no longer needed.
%  Do not attempt to deallocate members of the array.
%
%  The format of the GetMagickList method is:
%
%      ColorInfo **GetColorInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two ColorInfo structures based on their name
*/
static int ColorInfoCompare(const void *x, const void *y)
{
  const ColorInfo
    *xx=*((const ColorInfo **) x),
    *yy=*((const ColorInfo **) y);

  return (strcmp(xx->name, yy->name));
}
MagickExport ColorInfo **GetColorInfoArray(ExceptionInfo *exception)
{
  ColorInfo
    **array;

  ColorInfo
    *p;

  ColorInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load color list
  */
  (void) GetColorInfo("*",exception);
  if ((!color_list) || (exception->severity > UndefinedException))
    return 0;

  AcquireSemaphoreInfo(&color_semaphore);

  list=color_list;

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateMemory(ColorInfo **,sizeof(ColorInfo *)*(entries+1));
  if (!array)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,0);
      return False;
    }
  (void) memset((void **)array,0,sizeof(ColorInfo *)*(entries+1));

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;

  LiberateSemaphoreInfo(&color_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(ColorInfo *), ColorInfoCompare);

  return (array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,unsigned long *number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern.
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


  assert(pattern != (char *) NULL);
  assert(number_colors != (unsigned long *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const ColorInfo *) NULL)
    return((char **) NULL);

  /*
    Determine color list size
  */
  AcquireSemaphoreInfo(&color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    i++;
  LiberateSemaphoreInfo(&color_semaphore);

  /*
    Allocate color list.
  */
  colorlist=MagickAllocateMemory(char **,i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);

  /*
    Add colors matching glob specification to list
  */
  AcquireSemaphoreInfo(&color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AcquireString(p->name);
  }
  LiberateSemaphoreInfo(&color_semaphore);

  *number_colors=i;
  return(colorlist);
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
    if ((unsigned int) p->compliance & (unsigned int) SVGCompliance)
      (void) fprintf(file,"SVG ");
    if ((unsigned int) p->compliance & (unsigned int) X11Compliance)
      (void) fprintf(file,"X11 ");
    if ((unsigned int) p->compliance & (unsigned int) XPMCompliance)
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

  register const ColorInfo
    *p;

  register int
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

      int
        n;

      LongPixelPacket
        pixel;

      (void) memset(&pixel,0,sizeof(pixel));
      name++;
      for (n=0; isxdigit((int) name[n]); n++);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12) || (n == 24))
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
                    {
                      ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                      return(False);
                    }
            }
          } while (isxdigit((int) *name));
        }
      else
        if ((n != 4) && (n != 8) && (n != 16) && (n != 32))
          {
            ThrowException(exception,OptionWarning,UnrecognizedColor,name);
            return(False);
          }
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
                      {
                        ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                        return(False);
                      }
              }
            } while (isxdigit((int) *name));
          }
      {
        unsigned int
          divisor=1;
        
        n<<=2;
        for( i=n-1; i; i--)
          {
            divisor <<= 1;
            divisor |=1;
          }
        color->red=(Quantum)
          (((double) MaxRGB*pixel.red)/divisor+0.5);
        color->green=(Quantum)
          (((double) MaxRGB*pixel.green)/divisor+0.5);
        color->blue=(Quantum)
          (((double) MaxRGB*pixel.blue)/divisor+0.5);
        color->opacity=OpaqueOpacity;
        if ((n != 3) && (n != 6) && (n != 9) && (n != 12) && (n != 24))
          color->opacity=(Quantum)
            (((double) MaxRGB*pixel.opacity)/divisor+0.5);
      }
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      DoublePixelPacket
	pixel;

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
      DoublePixelPacket
	pixel;

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
%    o Compliance: Adhere to this color standard: NoCompliance, SVGCompliance,
%                    X11Compliance, XPMCompliance, AllCompliance.
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
        if (!((unsigned int) p->compliance & (unsigned int) compliance))
          continue;
        if ((p->color.red != color->red) || (p->color.green != color->green) ||
            (p->color.blue != color->blue) ||
            (p->color.opacity != color->opacity))
          continue;
        (void) strlcpy(name,p->name,MaxTextExtent);
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
+   R e a d C o l o rC o n f i g u r e F i l e                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadColorConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadColorConfigureFile method is:
%
%      unsigned int ReadColorConfigureFile(const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadColorConfigureFile returns True if at least one color
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
static unsigned int ReadColorConfigureFile(const char *basename,
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
    xml=AcquireString(ColorMap);
  token=AcquireString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strlcpy(keyword,token,MaxTextExtent);
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
          (void) strlcpy(keyword,token,MaxTextExtent);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"file") == 0)
            {
              if (depth > 200)
                ThrowException(exception,ConfigureError,IncludeElementNestedTooDeeply,path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  GetPathComponent(path,HeadPath,filename);
                  if (*filename != '\0')
                    (void) strlcat(filename,DirectorySeparator,MaxTextExtent);
                  (void) strlcat(filename,token,MaxTextExtent);
                  (void) ReadColorConfigureFile(filename,depth+1,exception);
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
        color_info=MagickAllocateMemory(ColorInfo *,sizeof(ColorInfo));
        if (color_info == (ColorInfo *) NULL)
          MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
            UnableToAllocateColorInfo);
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
  MagickFreeMemory(token);
  MagickFreeMemory(xml);
  if (color_list == (ColorInfo *) NULL)
    return(False);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(True);
}
