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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#if defined(HasLTDL) || defined(_MAGICKMOD_)
#  include "modules.h"
#endif

/*
  Global declarations.
*/
static MagickInfo
  *magick_list = (MagickInfo *) NULL;

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
%      void DestroyMagickInfo()
%
*/
Export void DestroyMagickInfo()
{
  MagickInfo
    *entry;

  register MagickInfo
    *p;

  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    entry=p;
    p=p->next;
    FreeMemory((void **) &entry->tag);
    FreeMemory((void **) &entry->description);
    FreeMemory((void **) &entry->module);
    FreeMemory((void **) &entry);
  }
  magick_list=(MagickInfo *) NULL;
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
Export MagickInfo *GetMagickInfo(const char *tag)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

#if defined(HasPTHREADS)
  static pthread_mutex_t
    magick_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&magick_mutex);
#endif
  if (magick_list == (MagickInfo *) NULL)
    {
      /*
        Register image formats.
      */
#if defined(HasLTDL) || defined(_MAGICKMOD_)
      InitializeModules();
#else
      Register8BIMImage();
      RegisterAVSImage();
      RegisterBMPImage();
      RegisterCACHEImage();
      RegisterCMYKImage();
      RegisterDCMImage();
      RegisterFAXImage();
      RegisterFITSImage();
      RegisterFPXImage();
      RegisterGIFImage();
      RegisterGRAYImage();
      RegisterGRADATIONImage();
      RegisterHDFImage();
      RegisterHISTOGRAMImage();
      RegisterHTMLImage();
      RegisterICCImage();
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
      RegisterXImage();
      RegisterXBMImage();
      RegisterXCImage();
      RegisterXPMImage();
      RegisterXWDImage();
      RegisterYUVImage();
#endif
    }
  magick_info=magick_list;
  if ((tag != (char *) NULL) && (*tag != '\0'))
    {
      /*
        Find tag in list
      */
      magick_info=(MagickInfo *) NULL;
      for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
        if (LocaleCompare(p->tag,tag) == 0)
          {
            magick_info=p;
            break;
          }
#if defined(HasLTDL) || defined(_MAGICKMOD_)
      /*
        Try loading format module, and see if support is added to list.
      */
      if ((p == (MagickInfo *) NULL) && LoadDynamicModule(tag))
        for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
          if (LocaleCompare(p->tag,tag) == 0)
            {
              magick_info=p;
              break;
            }
#endif
    }
#if defined(HasPTHREADS)
  pthread_mutex_unlock(&magick_mutex);
#endif
  return(magick_info);
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
Export void ListMagickInfo(FILE *file)
{
  register MagickInfo
    *p;

#if defined(HasLTDL) || defined(_MAGICKMOD_)
  /*
    Initialize ltdl and load modules.
  */
  InitializeModules();
  LoadAllModules();
#endif
  if (file == (FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"\nHere is a list of image formats recognized by "
    "ImageMagick.  Mode 'rw+'\nmeans ImageMagick can read, write, and "
    "save more than one image of a\nsequence to the same blob or file.\n\n");
  (void) fprintf(file,"    Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------\n");
  for (p=GetMagickInfo((char *) NULL); p != (MagickInfo *) NULL; p=p->next)
    (void) fprintf(file,"%10s%c  %c%c%c  %s\n",p->tag ? p->tag : "",
      p->blob_support ? '*' : ' ',p->decoder ? 'r' : '-',p->encoder ? 'w' : '-',
      p->encoder && p->adjoin ? '+' : '-',p->description ? p->description : "");
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
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
Export MagickInfo *RegisterMagickInfo(MagickInfo *entry)
{
  register MagickInfo
    *p;

  /*
    Add tag info to the image format list.
  */
  p=(MagickInfo *) NULL;
  if (magick_list != (MagickInfo *) NULL)
    for (p=magick_list; p->next != (MagickInfo *) NULL; p=p->next)
    {
      if (LocaleCompare(p->tag,entry->tag) >= 0)
        {
          if (LocaleCompare(p->tag,entry->tag) == 0)
            {
              p=p->previous;
              UnregisterMagickInfo(entry->tag);
            }
          break;
        }
    }
  if (magick_list == (MagickInfo *) NULL)
    {
      magick_list=entry;
      return(entry);
    }
  entry->previous=p;
  entry->next=p->next;
  if (p->next != (MagickInfo *) NULL)
    p->next->previous=entry;
  p->next=entry;
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
Export MagickInfo *SetMagickInfo(const char *tag)
{
  MagickInfo
    *entry;

  entry=(MagickInfo *) AllocateMemory(sizeof(MagickInfo));
  if (entry == (MagickInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  entry->tag=AllocateString(tag);
  entry->decoder=(Image *(*)(const ImageInfo *,ExceptionInfo *)) NULL;
  entry->encoder=(unsigned int (*)(const ImageInfo *,Image *)) NULL;
  entry->magick=
    (unsigned int (*)(const unsigned char *,const unsigned int)) NULL;
  entry->adjoin=True;
  entry->blob_support=True;
  entry->raw=False;
  entry->description=(char *) NULL;
  entry->module=(char *) NULL;
  entry->data=(void *) NULL;
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
Export unsigned int UnregisterMagickInfo(const char *tag)
{
  MagickInfo
    *p;

  p=magick_list;
  while (p != (MagickInfo *) NULL)
  {
    if (LocaleCompare(p->tag,tag) == 0)
      {
        if (p->next != (MagickInfo *) NULL)
          p->next->previous=p->previous;
        if (p->previous != (MagickInfo *) NULL)
          p->previous->next=p->next;
        else
          magick_list=p->next;
        FreeMemory((void **) &p->tag);
        FreeMemory((void **) &p->description);
        FreeMemory((void **) &p->module);
        FreeMemory((void **) &p);
        return(True);
      }
    p=p->next;
  }
  return(False);
}
