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

/*
  Global declarations.
*/
static MagickInfo
  *magick_info = (MagickInfo *) NULL;

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

  for (p=magick_info; p != (MagickInfo *) NULL; )
  {
    entry=p;
    p=p->next;
    if (entry->tag != (char *) NULL)
      FreeMemory((void *) &entry->tag);
    if (entry->description != (char *) NULL)
      FreeMemory((void *) &entry->description);
    FreeMemory((void *) &entry);
  }
  magick_info=(MagickInfo *) NULL;
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
  register MagickInfo
    *p;

  if (magick_info == (MagickInfo *) NULL)
    {
#if defined(HasLTDL)
      char
        **file_list,
        *coder_dir,
        *func_name,
        *base_name;

      int
        i,
        number_files;

      lt_dlhandle
        handle;

      number_files = 256;

      /* Use ImageMagick's memory allocators */
      lt_dlmalloc = AllocateMemory;
      lt_dlfree = FreeMemory;

      /* Initialize ltdl */
      if( lt_dlinit() != 0 )
        {
          const char *dlerror = lt_dlerror();
          printf("ERROR: failed to initialise ltdl: %s\n", dlerror);
          exit(1);
        }

      coder_dir=AllocateMemory(MaxTextExtent-1);
      strcpy(coder_dir, CoderModuleDirectory);

      /* Set ltdl module search path */
      lt_dlsetsearchpath( coder_dir );

      /* List module files */
      file_list=ListFiles(coder_dir,
        	      "*.la", &number_files);
      if (file_list == (char **) NULL)
        {
          FreeMemory((void *) &coder_dir);
          return (MagickInfo *)NULL;
        }

      func_name=AllocateMemory(MaxTextExtent-1);
      for ( i = 0; i < number_files; ++i )
        {
          void (*func)(void);

	  /* Load module */
	  printf("Loading %s\n", file_list[i]);
	  if( ( handle=lt_dlopen(file_list[i]) ) == 0)
	    {
	      printf("ERROR: failed to load module %s: %s\n", file_list[i], lt_dlerror());
	      continue;
	      /*exit(1);*/
	    }

	  /* Locate and execute RegisterFORMATImage function */
	  strcpy(func_name, "Register");
	  base_name = BaseFilename( file_list[i] );
	  Latin1Upper(base_name);

	  /* Hack due to 8BIM vs bim.c naming difference */
	  if(!strcmp("BIM", base_name))
	     strcat(func_name,"8");

	  strcat(func_name,base_name);
	  FreeMemory((void *) &base_name);
	  strcat(func_name, "Image");

#if 0
	  func=(void (*)(void))lt_dlsym(handle, func_name);
	  if (func == NULL)
	    {
	      printf("ERROR: failed to find symbol : %s\n", lt_dlerror());
	      continue;
	    }
	  func();
#endif
        }
      FreeMemory((void *) &func_name);
      FreeMemory((void *) &coder_dir);

      for (i=0; i < number_files; i++)
          FreeMemory((void *) &file_list[i]);
      if (file_list != (char **) NULL)
          FreeMemory((void *) &file_list);

#else
      Register8BIMImage();
      RegisterAVSImage();
      RegisterBMPImage();
      RegisterCacheImage();
      RegisterCMYKImage();
      RegisterDCMImage();
      RegisterFAXImage();
      RegisterFITSImage();
      RegisterFPXImage();
      RegisterGIFImage();
      RegisterGRAYImage();
      RegisterGradationImage();
      RegisterHDFImage();
      RegisterHistogramImage();
      RegisterHTMLImage();
      RegisterICCImage();
      RegisterIconImage();
      RegisterIPTCImage();
      RegisterJBIGImage();
      RegisterJPEGImage();
      RegisterLabelImage();
      RegisterLOGOImage();
      RegisterMAPImage();
      RegisterMatteImage();
      RegisterMIFFImage();
      RegisterMONOImage();
      RegisterMTVImage();
      RegisterPCDImage();
      RegisterPCLImage();
      RegisterPCXImage();
      RegisterPDBImage();
      RegisterPDFImage();
      RegisterPICTImage();
      RegisterPIXImage();
      RegisterPlasmaImage();
      RegisterPNGImage();
      RegisterPNMImage();
      RegisterPreviewImage();
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
      RegisterSteganoImage();
      RegisterSUNImage();
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
#if defined(HasLTDL)
  printf("MagickInfo List:\n");
  ListMagickInfo(stdout);
#endif /* HasLTDL */
  if (tag == (char *) NULL)
    return(magick_info);
  for (p=magick_info; p != (MagickInfo *) NULL; p=p->next)
    if (Latin1Compare(p->tag,tag) == 0)
      return(p);
  return((MagickInfo *) NULL);
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

#if defined(HasLTDL)
  p=entry;
  (void) printf("%10s%c  %c%c%c  %s\n",p->tag ? p->tag : "",
    p->blob_support ? '*' : ' ',p->decoder ? 'r' : '-',p->encoder ? 'w' : '-',
    p->encoder && p->adjoin ? '+' : '-',p->description ? p->description : "");
#endif /* HasLTDL */

  /*
    Add tag info to the image format list.
  */
  p=(MagickInfo *) NULL;
  if (magick_info != (MagickInfo *) NULL)
    {
      for (p=magick_info; p->next != (MagickInfo *) NULL; p=p->next)
      {
        if (Latin1Compare(p->tag,entry->tag) >= 0)
          {
            if (Latin1Compare(p->tag,entry->tag) == 0)
              {
                p=p->previous;
                UnregisterMagickInfo(entry->tag);
              }
            break;
          }
      }
    }
  if (magick_info == (MagickInfo *) NULL)
    {
      magick_info=entry;
      return(entry);
    }
  entry->previous=p;
  entry->next=p->next;
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
    *magick_info;

  register MagickInfo
    *p;

  for (p=GetMagickInfo((char *) NULL); p != (MagickInfo *) NULL; p=p->next)
  {
    if (Latin1Compare(p->tag,tag) == 0)
      {
        if (p->tag != (char *) NULL)
          FreeMemory((void *) &p->tag);
        if (p->description != (char *) NULL)
          FreeMemory((void *) &p->description);
        if (p->previous != (MagickInfo *) NULL)
          p->previous->next=p->next;
        else
          {
            magick_info=p->next;
            if (p->next != (MagickInfo *) NULL)
              p->next->previous=(MagickInfo *) NULL;
          }
        if (p->next != (MagickInfo *) NULL)
          p->next->previous=p->previous;
        magick_info=p;
        FreeMemory((void *) &magick_info);
        return(True);
    }
  }
  return(False);
}
