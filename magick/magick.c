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
#include "defines.h"
#include "magic.h"
#if defined(HasLTDL) || defined(_MAGICKMOD_)
#include "modules.h"
#endif

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

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

MagickExport void DestroyMagickInfo(void)
{
  MagickInfo
    *entry;

  register MagickInfo
    *p;

  AcquireSemaphore(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    entry=p;
    p=p->next;
    if (entry->tag != (char *) NULL)
      LiberateMemory((void **) &entry->tag);
    if (entry->description != (char *) NULL)
      LiberateMemory((void **) &entry->description);
    if (entry->module != (char *) NULL)
      LiberateMemory((void **) &entry->module);
    LiberateMemory((void **) &entry);
  }
  magick_list=(MagickInfo *) NULL;
  LiberateSemaphore(magick_semaphore);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

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
%  specified magick string.  If one is found the tag is returned otherwise
%  NULL.
%
%  The format of the GetImageMagick method is:
%
%      char *GetImageMagick(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o tag: Method GetImageMagick returns a tag that matches the
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
  const unsigned int length)
{
  register MagickInfo
    *p;

  assert(magick != (const unsigned char *) NULL);
  AcquireSemaphore(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (p->magick)
      if (p->magick(magick,length))
        break;
  LiberateSemaphore(magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p->tag);
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
%             MagickConfigurePath
%             $HOME/.magick/
%             $MAGICK_HOME/
%             <program directory>/
%             MagickModulesPath
%             X11ConfigurePath
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
  FormatString(path,"%.1024s%.1024s",MagickModulesPath,filename);
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
  FormatString(path,"%.1024s%.1024s%.1024s",SetClientPath((char *) NULL),
    DirectorySeparator,filename);
  if (IsAccessible(path))
    return(path);
  FormatString(path,"%.1024s%.1024s",MagickConfigurePath,filename);
  if (IsAccessible(path))
    return(path);
  FormatString(path,"%.1024s%.1024s",X11ConfigurePath,filename);
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
%  the specified tag.  If tag is NULL, the head of the image format list is
%  returned.
%
%  The format of the GetMagickInfo method is:
%
%      MagickInfo *GetMagickInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o magick_info: Method GetMagickInfo returns a pointer MagickInfo
%      structure that matches the specified tag.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%
*/
MagickExport MagickInfo *GetMagickInfo(const char *tag)
{
  register MagickInfo
    *p;

  unsigned int
    initialize;

  AcquireSemaphore(magick_semaphore);
  initialize=magick_list == (MagickInfo *) NULL;
  LiberateSemaphore(magick_semaphore);
  if (initialize)
    {
      /*
        Register image formats.
      */
      atexit(DestroyMagickInfo);
#if defined(HasLTDL) || defined(_MAGICKMOD_)
      InitializeModules();
#else
      Register8BIMImage();
      RegisterAVIImage();
      RegisterAVSImage();
      RegisterBMPImage();
      RegisterCACHEImage();
      RegisterCMYKImage();
      RegisterCUTImage();
      RegisterDCMImage();
      RegisterEPTImage();
      RegisterFAXImage();
      RegisterFITSImage();
      RegisterFPXImage();
      RegisterGIFImage();
      RegisterGRAYImage();
      RegisterGRADIENTImage();
      RegisterHDFImage();
      RegisterHISTOGRAMImage();
      RegisterHTMLImage();
      RegisterICMImage();
      RegisterICONImage();
      RegisterIPTCImage();
      RegisterJBIGImage();
      RegisterJPEGImage();
      RegisterLABELImage();
      RegisterLOGOImage();
      RegisterMAPImage();
      RegisterMATTEImage();
      RegisterMIFFImage();
      RegisterMONOImage();
      RegisterMTVImage();
      RegisterMVGImage();
      RegisterNULLImage();
      RegisterPCDImage();
      RegisterPCLImage();
      RegisterPCXImage();
      RegisterPDBImage();
      RegisterPDFImage();
      RegisterPICTImage();
      RegisterPIXImage();
      RegisterPLASMAImage();
      RegisterPNGImage();
      RegisterPNMImage();
      RegisterPREVIEWImage();
      RegisterPSImage();
      RegisterPS2Image();
      RegisterPS3Image();
      RegisterPSDImage();
      RegisterPWPImage();
      RegisterRGBImage();
      RegisterRLAImage();
      RegisterRLEImage();
      RegisterSCTImage();
      RegisterSGIImage();
      RegisterSTEGANOImage();
      RegisterSUNImage();
      RegisterSVGImage();
      RegisterTGAImage();
      RegisterTIFFImage();
      RegisterTILEImage();
      RegisterTIMImage();
      RegisterTTFImage();
      RegisterTXTImage();
      RegisterUILImage();
      RegisterUYVYImage();
      RegisterVICARImage();
      RegisterVIDImage();
      RegisterVIFFImage();
      RegisterWBMPImage();
      RegisterWPGImage();
      RegisterXImage();
      RegisterXBMImage();
      RegisterXCImage();
      RegisterXPMImage();
      RegisterXWDImage();
      RegisterYUVImage();
#endif
    }
  if ((tag == (char *) NULL) || (*tag == '\0'))
    return(magick_list);
  /*
    Find tag in list
  */
  AcquireSemaphore(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,tag) == 0)
      break;
  LiberateSemaphore(magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p);
#if defined(HasLTDL) || defined(_MAGICKMOD_)
  (void) OpenModule(tag);
  AcquireSemaphore(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (LocaleCompare(p->tag,tag) == 0)
      break;
  LiberateSemaphore(magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p);
#endif
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
%      void ListMagickInfo(FILE *file)
%
%  A description of each parameter follows.
%
%    o file: A pointer to a FILE structure.
%
%
*/
MagickExport void ListMagickInfo(FILE *file)
{
  register MagickInfo
    *p;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"\nHere is a list of image formats recognized by "
    "ImageMagick.  Mode 'rw+'\nmeans ImageMagick can read, write, and "
    "save more than one image of a\nsequence to the same blob or file.\n\n");
  (void) fprintf(file,"    Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------\n");
  (void) GetMagickInfo((char *) NULL);
#if defined(HasLTDL) || defined(_MAGICKMOD_)
  OpenModules();
#endif    
  AcquireSemaphore(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    (void) fprintf(file,"%10s%c  %c%c%c  %s\n",p->tag ? p->tag : "",
      p->blob_support ? '*' : ' ',p->decoder ? 'r' : '-',p->encoder ? 'w' : '-',
      p->encoder && p->adjoin ? '+' : '-',p->description ? p->description : "");
  LiberateSemaphore(magick_semaphore);
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k I n c a r n a t e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickIncarnate initializes the ImageMagick environment.
%
%  The format of the MagickIncarnate function is:
%
%      MagickIncarnate(const char *path)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to the execution path of the current
%      ImageMagick client.
%
%
*/
MagickExport void MagickIncarnate(const char *path)
{
  char
    directory[MaxTextExtent],
    magic[MaxTextExtent];

  unsigned char
    magick[MaxTextExtent];

  assert(path != (const char *) NULL);
  (void) getcwd(directory,MaxTextExtent);
  (void) SetClientPath(directory);
  if (path != (const char *) NULL)
    {
      char
        *filename;

      register char
        *p;

      /*
        Set client path.
      */
      filename=GetExecutionPath(path);
      for (p=filename+(Extent(filename)-1); p > filename; p--)
        if (IsBasenameSeparator(*p))
          {
            p++;
            (void) strncpy(directory,filename,p-filename);
            directory[p-filename]='\0';
            SetClientPath(directory);
            (void) strcpy(filename,p);
            break;
          }
      /*
        Set client name.
      */
      for (p=filename+(Extent(filename)-1); p > filename; p--)
        if (*p == '.')
          {
            *p='\0';
            break;
          }
      SetClientName(filename);
      LiberateMemory((void **) &filename);
    }
  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");
  *magick='\0';
  (void) SetImageMagic(magick,MaxTextExtent,magic);
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
%  the list of supported formats.  The attributes include the image format tag,
%  a method to read and/or write the format, whether the format supports
%  the saving of more than one frame to the same file or blob, whether the
%  format supports native in-memory I/O, and a brief description of the format.
%
%  The format of the RegisterMagickInfo method is:
%
%      MagickInfo *RegisterMagickInfo(MagickInfo *entry)
%
%  A description of each parameter follows:
%
%    o magick_info: Method RegisterMagickInfo returns a pointer MagickInfo
%      structure that contains the specified tag info.
%
%    o entry: A pointer to a structure of type MagickInfo.
%
*/
MagickExport MagickInfo *RegisterMagickInfo(MagickInfo *entry)
{
  register MagickInfo
    *p;

  /*
    Add tag info to the image format list.
  */
  assert(entry != (MagickInfo *) NULL);
  assert(entry->signature == MagickSignature);
  AcquireSemaphore(magick_semaphore);
  p=(MagickInfo *) NULL;
  if (magick_list != (MagickInfo *) NULL)
    for (p=magick_list; p->next != (MagickInfo *) NULL; p=p->next)
    {
      if (LocaleCompare(p->tag,entry->tag) < 0)
        continue;
      if (LocaleCompare(p->tag,entry->tag) == 0)
        {
          p=p->previous;
          UnregisterMagickInfo(entry->tag);
        }
    }
  if (magick_list == (MagickInfo *) NULL)
    {
      entry->previous=(MagickInfo *) NULL;
      entry->next=(MagickInfo *) NULL;
      magick_list=entry;
    }
  else
    {
      entry->previous=p;
      entry->next=p->next;
      if (p->next != (MagickInfo *) NULL)
        p->next->previous=entry;
      p->next=entry;
    }
  LiberateSemaphore(magick_semaphore);
  return(entry);
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
%      MagickInfo *SetMagickInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o magick_info: Method SetMagickInfo returns the allocated and initialized
%      MagickInfo structure.
%
%    o tag: a character string that represents the image format associated
%      with the MagickInfo structure.
%
%
*/
MagickExport MagickInfo *SetMagickInfo(const char *tag)
{
  MagickInfo
    *entry;

  assert(tag != (const char *) NULL);
  entry=(MagickInfo *) AcquireMemory(sizeof(MagickInfo));
  if (entry == (MagickInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  entry->tag=AllocateString(tag);
  entry->image_info=(ImageInfo *) NULL;
  entry->decoder=(Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL;
  entry->encoder=(unsigned int (*)(const ImageInfo *,Image *)) NULL;
  entry->magick=
    (unsigned int (*)(const unsigned char *,const unsigned int)) NULL;
  entry->adjoin=True;
  entry->blob_support=True;
  entry->raw=False;
  entry->description=(char *) NULL;
  entry->module=(char *) NULL;
  entry->client_data=(void *) NULL;
  entry->signature=MagickSignature;
  entry->previous=(MagickInfo *) NULL;
  entry->next=(MagickInfo *) NULL;
  return(entry);
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
%  Method UnregisterMagickInfo removes a tag from the magick info list.  It
%  returns False if the tag does not exist in the list otherwise True.
%
%  The format of the UnregisterMagickInfo method is:
%
%      unsigned int UnregisterMagickInfo(const char *tag)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterMagickInfo returns False if the tag does not
%      exist in the list otherwise True.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
*/
MagickExport unsigned int UnregisterMagickInfo(const char *tag)
{
  MagickInfo
    *p;

  unsigned int
    status;

  assert(tag != (const char *) NULL);
  AcquireSemaphore(magick_semaphore);
  status=False;
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->tag,tag) != 0)
      continue;
    if (p->next != (MagickInfo *) NULL)
      p->next->previous=p->previous;
    if (p->previous != (MagickInfo *) NULL)
      p->previous->next=p->next;
    else
      magick_list=p->next;
    LiberateMemory((void **) &p->tag);
    LiberateMemory((void **) &p->description);
    LiberateMemory((void **) &p->module);
    LiberateMemory((void **) &p);
    status=True;
    break;
  }
  LiberateSemaphore(magick_semaphore);
  return(status);
}
