/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%               Methods to Read or List ImageMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Global declarations.
*/
static MagickInfo
  *magick_list = (MagickInfo *) NULL;

static SemaphoreInfo *
  magick_semaphore = (SemaphoreInfo *) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeMagick destroys the ImageMagick environment.
%
%  The format of the DestroyMagick function is:
%
%      DestroyMagick(void)
%
%
*/
MagickExport void DestroyMagick(void)
{
  DestroyColorInfo();
  DestroyDelegateInfo();
  DestroyTypeInfo();
  DestroyModuleInfo();
  DestroyMagicInfo();
  DestroyMagickInfo();
  DestroyConstitute();
  DestroyCache();
  DestroySemaphore();
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagickInfo deallocates memory associated MagickInfo list.
%
%  The format of the DestroyMagickInfo method is:
%
%      void DestroyMagickInfo(void)
%
*/
MagickExport void DestroyMagickInfo(void)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    magick_info=p;
    p=p->next;
    if (magick_info->name != (char *) NULL)
      LiberateMemory((void **) &magick_info->name);
    if (magick_info->description != (char *) NULL)
      LiberateMemory((void **) &magick_info->description);
    if (magick_info->module != (char *) NULL)
      LiberateMemory((void **) &magick_info->module);
    LiberateMemory((void **) &magick_info);
  }
  magick_list=(MagickInfo *) NULL;
  DestroySemaphoreInfo(magick_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e M a g i c k                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageMagick searches for an image format that matches the
%  specified magick string.  If one is found the name is returned otherwise
%  NULL.
%
%  The format of the GetImageMagick method is:
%
%      char *GetImageMagick(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o name: Method GetImageMagick returns a name that matches the
%      specified magick string.
%
%    o magick: a character string that represents the image format we are
%      looking for.
%
%    o length: The length of the binary string.
%
%
*/
MagickExport char *GetImageMagick(const unsigned char *magick,
  const size_t length)
{
  register MagickInfo
    *p;

  assert(magick != (const unsigned char *) NULL);
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (p->magick && p->magick(magick,length))
      break;
  LiberateSemaphoreInfo(&magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p->name);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k C o n f i g u r e P a t h                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagickConfigurePath searches a number of pre-defined locations
%  for the specified ImageMagick configuration file and returns the path.
%  The search order follows:
%
%             <current directory>/
%             <client path>/
%             $MAGICK_HOME/
%             $HOME/.magick/
%             MagickLibPath
%             MagickModulesPath
%             MagickSharePath
%
%  The format of the GetMagickConfigurePath method is:
%
%      char *GetMagickConfigurePath(const char *filename)
%
%  A description of each parameter follows:
%
%    o path:  Method GetMagickConfigurePath returns the path if the
%      configuration file is found, otherwise NULL is returned.
%
%    o filename: A character string representing the desired configuration
%      file.
%
%
*/
MagickExport char *GetMagickConfigurePath(const char *filename)
{
  char
    *path;

  path=AllocateString(filename);
  FormatString(path,"%.1024s",filename);
  if (IsAccessible(path))
    return(path);
  FormatString(path,"%.1024s%.1024s%.1024s",SetClientPath((char *) NULL),
    DirectorySeparator,filename);
  if (IsAccessible(path))
    return(path);
  if (getenv("HOME") != (char *) NULL)
    {
      FormatString(path,"%.1024s%.1024s%.1024s%.1024s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,
        DirectorySeparator,filename);
      if (IsAccessible(path))
        return(path);
    }
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      FormatString(path,"%.1024s%.1024s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
      if (IsAccessible(path))
        return(path);
    }
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  if (IsAccessible(path))
    return(path);
  FormatString(path,"%.1024s%.1024s",MagickModulesPath,filename);
  if (IsAccessible(path))
    return(path);
  FormatString(path,"%.1024s%.1024s",MagickSharePath,filename);
  if (IsAccessible(path))
    return(path);
  LiberateMemory((void **) &path);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagickInfo returns a pointer MagickInfo structure that matches
%  the specified name.  If name is NULL, the head of the image format list is
%  returned.
%
%  The format of the GetMagickInfo method is:
%
%      MagickInfo *GetMagickInfo(const char *name,Exception *exception)
%
%  A description of each parameter follows:
%
%    o magick_info: Method GetMagickInfo returns a pointer MagickInfo
%      structure that matches the specified name.
%
%    o name: a character string that represents the image format we are
%      looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickInfo *GetMagickInfo(const char *name,
  ExceptionInfo *exception)
{
  register MagickInfo
    *p;

  if ((name != (const char *) NULL) && (LocaleCompare(name,"*") == 0))
    OpenModules(exception);
  AcquireSemaphoreInfo(&magick_semaphore);
  if (magick_list != (MagickInfo *) NULL)
    LiberateSemaphoreInfo(&magick_semaphore);
  else
    {
      /*
        Register image formats.
      */
      LiberateSemaphoreInfo(&magick_semaphore);
      (void) GetModuleInfo((char *) NULL,exception);
    }
  if ((name == (const char *) NULL) ||  (LocaleCompare(name,"*") == 0))
    return(magick_list);
  /*
    Find name in list
  */
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      break;
  if (p != (MagickInfo *) NULL)
    {
      LiberateSemaphoreInfo(&magick_semaphore);
      return(p);
    }
  LiberateSemaphoreInfo(&magick_semaphore);
  (void) OpenModule(name,exception);
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,name) == 0)
      break;
  if (p != (MagickInfo *) NULL)
    {
      LiberateSemaphoreInfo(&magick_semaphore);
      return(p);
    }
  LiberateSemaphoreInfo(&magick_semaphore);
  return((MagickInfo *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k V e r s i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagickVersion returns the ImageMagick API version as a string
%  and as a number.
%
%  The format of the GetMagickVersion method is:
%
%      char *GetMagickVersion(unsigned int *version)
%
%  A description of each parameter follows:
%
%    o string: Method GetMagickVersion returns the ImageMagick version
%      string.
%
%    o version: The ImageMagick version is returned as a number.
%
*/
MagickExport char *GetMagickVersion(unsigned int *version)
{
  assert(version != (unsigned int *) NULL);
  *version=MagickLibVersion;
  return(MagickVersion);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e M a g i c k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeMagick initializes the ImageMagick environment.
%
%  The format of the InitializeMagick function is:
%
%      InitializeMagick(const char *path)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to the execution path of the current
%      ImageMagick client.
%
%
*/
MagickExport void InitializeMagick(const char *path)
{
  char
    directory[MaxTextExtent],
    filename[MaxTextExtent];

  assert(path != (const char *) NULL);
  InitializeSemaphore();
  (void) getcwd(directory,MaxTextExtent);
  (void) SetClientPath(directory);
  GetPathComponent(path,HeadPath,filename);
  SetClientPath(filename);
#if defined(WIN32)
  GetPathComponent(path,TailPath,filename);
#else
  GetPathComponent(path,BasePath,filename);
#endif
  SetClientName(filename);
  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
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
MagickExport int unsigned IsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
#if defined(macintosh)
  return(MACIsMagickConflict(magick));
#endif
#if defined(vms)
  return(VMSIsMagickConflict(magick));
#endif
#if defined(WIN32)
  return(NTIsMagickConflict(magick));
#endif
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagickInfo lists the image formats to a file.
%
%  The format of the ListMagickInfo method is:
%
%      unsigned int ListMagickInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file: A pointer to a FILE structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListMagickInfo(FILE *file,ExceptionInfo *exception)
{
  register MagickInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"Here is a list of image formats recognized by "
    "ImageMagick.  Mode 'rw+'\nmeans ImageMagick can read, write, and "
    "save more than one image of a\nsequence to the same blob or file.\n\n");
  (void) fprintf(file,"    Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------------\n");
  p=GetMagickInfo("*",exception);
  if (p == (MagickInfo *) NULL)
    return(False);
  AcquireSemaphoreInfo(&magick_semaphore);
  for ( ; p != (MagickInfo *) NULL; p=p->next)
    if (p->stealth != True)
      (void) fprintf(file,"%10s%c  %c%c%c  %s\n",p->name ? p->name : "",
        p->blob_support ? '*' : ' ',p->decoder ? 'r' : '-',
        p->encoder ? 'w' : '-',p->encoder && p->adjoin ? '+' : '-',
        p->description ? p->description : "");
  LiberateSemaphoreInfo(&magick_semaphore);
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M a g i c k I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMagickInfo adds attributes for a particular image format to
%  the list of supported formats.  The attributes include the image format name,
%  a method to read and/or write the format, whether the format supports
%  the saving of more than one frame to the same file or blob, whether the
%  format supports native in-memory I/O, and a brief description of the format.
%
%  The format of the RegisterMagickInfo method is:
%
%      MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
%
%  A description of each parameter follows:
%
%    o magick_info: Method RegisterMagickInfo returns a pointer MagickInfo
%      structure that contains the specified name info.
%
%    o magick_info: A pointer to a structure of type MagickInfo.
%
*/
MagickExport MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
{
  register MagickInfo
    *p;

  /*
    Delete any existing name.
  */
  assert(magick_info != (MagickInfo *) NULL);
  assert(magick_info->signature == MagickSignature);
  UnregisterMagickInfo(magick_info->name);
  AcquireSemaphoreInfo(&magick_semaphore);
  magick_info->previous=(MagickInfo *) NULL;
  magick_info->next=(MagickInfo *) NULL;
  if (magick_list == (MagickInfo *) NULL)
    {
      /*
        Start magick list.
      */
      magick_list=magick_info;
      LiberateSemaphoreInfo(&magick_semaphore);
      return(magick_info);
    }
  /*
    Tag is added in lexographic order.
  */
  for (p=magick_list; p->next != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->name,magick_info->name) >= 0)
      break;
  if (LocaleCompare(p->name,magick_info->name) < 0)
    {
      /*
        Add entry after target.
      */
      magick_info->next=p->next;
      p->next=magick_info;
      magick_info->previous=p;
      if (magick_info->next != (MagickInfo *) NULL)
        magick_info->next->previous=magick_info;
      LiberateSemaphoreInfo(&magick_semaphore);
      return(magick_info);
    }
  /*
    Add entry before target.
  */
  magick_info->next=p;
  magick_info->previous=p->previous;
  p->previous=magick_info;
  if (magick_info->previous != (MagickInfo *) NULL)
    magick_info->previous->next=magick_info;
  if (p == magick_list)
    magick_list=magick_info;
  LiberateSemaphoreInfo(&magick_semaphore);
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageInfo() initializes the `magick' field of the ImageInfo
%  structure.  It is set to a type of image format based on the prefix or
%  suffix of the filename.  For example, `ps:image' returns PS indicating
%  a Postscript image.  JPEG is returned for this filename: `image.jpg'.
%  The filename prefix has precendence over the suffix.  Use an optional
%  index enclosed in brackets after a file name to specify a desired subimage
%  of a multi-resolution image format like Photo CD (e.g. img0001.pcd[4]).
%  A True (non-zero) return value indicates success.
%
%  The format of the SetImageInfo method is:
%
%      unsigned int SetImageInfo(ImageInfo *image_info,
%        const unsigned int rectify,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int SetImageInfo(ImageInfo *image_info,
  const unsigned int rectify,ExceptionInfo *exception)
{
  char
    magic[MaxTextExtent];

  Image
    *image;

  MagicInfo
    *magic_info;

  register char
    *p,
    *q;

  unsigned char
    magick[2*MaxTextExtent];

  unsigned int
    affirm,
    status;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  *magic='\0';
  p=image_info->filename+Max(strlen(image_info->filename)-1,0);
  if (*p == ']')
    for (q=p-1; q > image_info->filename; q--)
    {
      char
        *tile;

      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      if (*q != '[')
        continue;
      if (!IsGeometry(q+1))
        break;
      tile=(char *) AcquireMemory(p-q);
      if (tile == (char *) NULL)
        break;
      (void) strncpy(tile,q+1,p-q-1);
      tile[p-q-1]='\0';
      *q='\0';
      p=q;
      (void) CloneString(&image_info->tile,tile);
      LiberateMemory((void **) &tile);
      if (!IsSubimage(image_info->tile,True))
        break;
      /*
        Determine sub-image range.
      */
      image_info->subimage=atoi(image_info->tile);
      image_info->subrange=atoi(image_info->tile);
      (void) sscanf(image_info->tile,"%lu-%lu",&image_info->subimage,
        &image_info->subrange);
      if (image_info->subrange < image_info->subimage)
        Swap(image_info->subimage,image_info->subrange);
      else
        {
          LiberateMemory((void **) &image_info->tile);
          image_info->tile=(char *) NULL;
        }
      image_info->subrange-=image_info->subimage-1;
      break;
    }
  while ((*p != '.') && (p > (image_info->filename+1)))
    p--;
  if ((LocaleCompare(p,".gz") == 0) || (LocaleCompare(p,".Z") == 0) ||
      (LocaleCompare(p,".bz2") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > (image_info->filename+1)));
  if ((*p == '.') && (strlen(p) < (int) sizeof(magic)))
    {
      /*
        User specified image format.
      */
      (void) strcpy(magic,p+1);
      for (q=magic; *q != '\0'; q++)
        if (*q == '.')
          {
            *q='\0';
            break;
          }
      LocaleUpper(magic);
      /*
        SGI and RGB are ambiguous;  TMP must be set explicitly.
      */
      if (((LocaleNCompare(image_info->magick,"SGI",3) != 0) ||
          (LocaleCompare(magic,"RGB") != 0)) &&
          (LocaleCompare(magic,"TMP") != 0))
        (void) strcpy(image_info->magick,magic);
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  affirm=False;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
#if defined(vms)
  if (*(p+1) == ':')
    p+=2;  /* skip DECnet node spec */
#endif
  if ((*p == ':') && ((p-image_info->filename) < (int) sizeof(magic)))
    {
      char
        format[MaxTextExtent];

      /*
        User specified image format.
      */
      (void) strncpy(format,image_info->filename,p-image_info->filename);
      format[p-image_info->filename]='\0';
      if (LocaleCompare(format,"GRADATION") == 0)
        (void) strcpy(format,"GRADIENT");
      LocaleUpper(format);
      if (!IsMagickConflict(format))
        {
          /*
            Strip off image format prefix.
          */
          p++;
          (void) strcpy(image_info->filename,p);
          (void) strcpy(magic,format);
          if (LocaleCompare(magic,"IMPLICIT") != 0)
            {
              (void) strcpy(image_info->magick,magic);
              if (LocaleCompare(magic,"TMP") != 0)
                affirm=True;
              else
                image_info->temporary=True;
            }
        }
    }
  if (rectify)
    {
      char
        filename[MaxTextExtent];

      MagickInfo
        *magick_info;

      /*
        Rectify multi-image file support.
      */
      FormatString(filename,image_info->filename,0);
      if ((LocaleCompare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=False;
      magick_info=(MagickInfo *) GetMagickInfo(magic,exception);
      if (magick_info != (MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return(True);
    }
  if (affirm)
    return(True);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return(False);
  /*
    Determine the image format from the first few bytes of the file.
  */
  (void) strcpy(image->filename,image_info->filename);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    {
      CloseBlob(image);
      DestroyImage(image);
      return(False);
    }
  if ((image->blob->data != (unsigned char *) NULL)  || !image->exempt)
    (void) ReadBlob(image,2*MaxTextExtent,magick);
  else
    {
      FILE
        *file;

      register int
        c,
        i;

      /*
        Copy standard input or pipe to temporary file.
      */
      image_info->file=(FILE *) NULL;
      TemporaryFilename(image->filename);
      image_info->temporary=True;
      FormatString(image_info->filename,"%.1024s",image->filename);
      file=fopen(image->filename,WriteBinaryType);
      if (file == (FILE *) NULL)
        ThrowBinaryException(MissingDelegateWarning,"Unable to write file",
          image->filename);
      i=0;
      for (c=fgetc(image->file); c != EOF; c=fgetc(image->file))
      {
        if (i < MaxTextExtent)
          magick[i++]=(unsigned char) c;
        (void) fputc(c,file);
      }
      (void) fclose(file);
    }
  CloseBlob(image);
  DestroyImage(image);
  /*
    Check magic.mgk configuration file.
  */
  magic_info=GetMagicInfo(magick,2*MaxTextExtent,exception);
  if ((magic_info != (MagicInfo *) NULL) && (magic_info->name != (char *) NULL))
    {
      (void) strcpy(image_info->magick,magic_info->name);
      return(True);
    }
  /*
    Check module IsImage() methods.
  */
  p=GetImageMagick(magick,2*MaxTextExtent);
  if (p != (char *) NULL)
    {
      (void) strcpy(image_info->magick,p);
      return(True);
    }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetMagickInfo allocates a MagickInfo structure and initializes the
%  members to default values.
%
%  The format of the SetMagickInfo method is:
%
%      MagickInfo *SetMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o magick_info: Method SetMagickInfo returns the allocated and initialized
%      MagickInfo structure.
%
%    o name: a character string that represents the image format associated
%      with the MagickInfo structure.
%
%
*/
MagickExport MagickInfo *SetMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  assert(name != (const char *) NULL);
  magick_info=(MagickInfo *) AcquireMemory(sizeof(MagickInfo));
  if (magick_info == (MagickInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  memset(magick_info,0,sizeof(MagickInfo));
  magick_info->name=AllocateString(name);
  magick_info->adjoin=True;
  magick_info->blob_support=True;
  magick_info->thread_support=True;
  magick_info->signature=MagickSignature;
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M a g i c k I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMagickInfo removes a name from the magick info list.  It
%  returns False if the name does not exist in the list otherwise True.
%
%  The format of the UnregisterMagickInfo method is:
%
%      unsigned int UnregisterMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterMagickInfo returns False if the name does not
%      exist in the list otherwise True.
%
%    o name: a character string that represents the image format we are
%      looking for.
%
*/
MagickExport unsigned int UnregisterMagickInfo(const char *name)
{
  MagickInfo
    *p;

  unsigned int
    status;

  assert(name != (const char *) NULL);
  status=False;
  AcquireSemaphoreInfo(&magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->name,name) != 0)
      continue;
    if (p->next != (MagickInfo *) NULL)
      p->next->previous=p->previous;
    if (p->previous != (MagickInfo *) NULL)
      p->previous->next=p->next;
    else
      magick_list=p->next;
    LiberateMemory((void **) &p->name);
    LiberateMemory((void **) &p->description);
    LiberateMemory((void **) &p->module);
    LiberateMemory((void **) &p);
    status=True;
    break;
  }
  LiberateSemaphoreInfo(&magick_semaphore);
  return(status);
}
