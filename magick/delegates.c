/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        DDDD   EEEEE  L      EEEEE   GGGG   AAA   TTTTT  EEEEE  SSSSS        %
%        D   D  E      L      E      G      A   A    T    E      SS           %
%        D   D  EEE    L      EEE    G  GG  AAAAA    T    EEE     SSS         %
%        D   D  E      L      E      G   G  A   A    T    E         SS        %
%        DDDD   EEEEE  LLLLL  EEEEE   GGG   A   A    T    EEEEE  SSSSS        %
%                                                                             %
%                                                                             %
%                   Methods to Read/Write/Invoke Delegates                    %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                               October 1998                                  %
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
%  The Delegates methods associate a set of commands with a particular
%  image format.  ImageMagick uses delegates for formats it does not handle
%  directly.
%
%  Thanks to Bob Friesenhahn for the initial inspiration and design of the
%  delegates methods.
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Constant declaractions.
*/
const char
  *DelegateFilename = "delegates.mgk";

/*
  Global declaractions.
*/
static DelegateInfo
  *delegates = (DelegateInfo *) NULL;

static SemaphoreInfo *
  delegate_semaphore = (SemaphoreInfo *) NULL;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D e l e g a t e I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyDelegateInfo deallocates memory associated with the delegates
%  list.
%
%  The format of the DestroyDelegateInfo method is:
%
%      DestroyDelegateInfo(void)
%
*/
MagickExport void DestroyDelegateInfo(void)
{
  DelegateInfo
    *delegate;

  register DelegateInfo
    *p;

  AcquireSemaphore(delegate_semaphore);
  for (p=delegates; p != (DelegateInfo *) NULL; )
  {
    if (p->commands != (char *) NULL)
      LiberateMemory((void **) &p->commands);
    delegate=p;
    p=p->next;
    LiberateMemory((void **) &delegate);
  }
  delegates=(DelegateInfo *) NULL;
  LiberateSemaphore(delegate_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t D e l e g a t e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDelegateInfo returns any delegates associated with the specified
%  tag.  True is returned if a delegate is found, otherwise False.
%
%  The format of the GetDelegateInfo method is:
%
%      unsigned int GetDelegateInfo(const char *decode_tag,
%        const char *encode_tag,DelegateInfo *delegate_info)
%
%  A description of each parameter follows:
%
%    o status: Method GetDelegateInfo returns True if a delegate is found,
%      otherwise False.
%
%    o decode_tag: Specifies the decode delegate we are searching for as a
%      character string.
%
%    o encode_tag: Specifies the encode delegate we are searching for as a
%      character string.
%
%    o delegate_info:  A structure of type DelegateInfo.  On return this
%      structure contains the delegate information for the specified
%      tag.
%
%
*/
MagickExport unsigned int GetDelegateInfo(const char *decode_tag,
  const char *encode_tag,DelegateInfo *delegate_info)
{
  DelegateInfo
    *delegates;

  assert(delegate_info != (DelegateInfo *) NULL);
  AcquireSemaphore(delegate_semaphore);
  delegates=SetDelegateInfo((DelegateInfo *) NULL);
  LiberateSemaphore(delegate_semaphore);
  if (delegates == (DelegateInfo *) NULL)
    MagickWarning(DelegateWarning,"no delegates configuration file found",
      DelegateFilename);
  /*
    Search for requested delegate.
  */
  for ( ; delegates != (DelegateInfo *) NULL; delegates=delegates->next)
  {
    *delegate_info=(*delegates);
    if (delegates->direction > 0)
      {
        if (LocaleCompare(delegates->decode_tag,decode_tag) == 0)
          return(True);
        continue;
      }
    if (delegates->direction < 0)
      {
        if (LocaleCompare(delegates->encode_tag,encode_tag) == 0)
          return(True);
        continue;
      }
    if (LocaleCompare(decode_tag,delegates->decode_tag) == 0)
      if (LocaleCompare(encode_tag,delegates->encode_tag) == 0)
        return(True);
    if (LocaleCompare(decode_tag,"*") == 0)
      if (LocaleCompare(encode_tag,delegates->encode_tag) == 0)
        return(True);
    if (LocaleCompare(decode_tag,delegates->decode_tag) == 0)
      if (LocaleCompare(encode_tag,"*") == 0)
        return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t D e l e g a t e C o m m a n d                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetDelegateCommand replaces any embedded formatting characters with
%  the appropriate image attribute and returns the resulting command.
%
%  The format of the GetDelegateCommand method is:
%
%      char *GetDelegateCommand(const ImageInfo *image_info,Image *image,
%        const char *decode_tag,const char *encode_tag)
%
%  A description of each parameter follows:
%
%    o command: Method GetDelegateCommand returns the command associated
%      with specified delegate tag.
%
%    o image_info: The address of a structure of type ImageInfo.
%
%    o image: The address of a structure of type Image.
%
%    o decode_tag: Specifies the decode delegate we are searching for as a
%      character string.
%
%    o encode_tag: Specifies the encode delegate we are searching for as a
%      character string.
%
%
*/
MagickExport char *GetDelegateCommand(const ImageInfo *image_info,Image *image,
  const char *decode_tag,const char *encode_tag)
{
  char
    *command,
    **commands;

  DelegateInfo
    delegate_info;

  register int
    i;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(decode_tag != (char *) NULL);
  if (!GetDelegateInfo(decode_tag,encode_tag,&delegate_info))
    {
      ThrowException(&image->exception,MissingDelegateWarning,"no tag found",
        decode_tag ? decode_tag : encode_tag);
      return((char *) NULL);
    }
  commands=StringToList(delegate_info.commands);
  if (commands == (char **) NULL)
    {
      ThrowException(&image->exception,ResourceLimitWarning,
        "Memory allocation failed",decode_tag ? decode_tag : encode_tag);
      return((char *) NULL);
    }
  command=TranslateText(image_info,image,commands[0]);
  if (command == (char *) NULL)
    ThrowException(&image->exception,ResourceLimitWarning,
      "Memory allocation failed",commands[0]);
  /*
    Free resources.
  */
  for (i=0; commands[i] != (char *) NULL; i++)
    LiberateMemory((void **) &commands[i]);
  LiberateMemory((void **) &commands);
  return(command);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n v o k e D e l e g a t e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InvokeDelegate replaces any embedded formatting characters with
%  the appropriate image attribute and executes the resulting command.  False
%  is returned if the commands execute with success otherwise True.
%
%  The format of the InvokeDelegate method is:
%
%      unsigned int InvokeDelegate(const ImageInfo *image_info,
%        Image *image,const char *decode_tag,const char *encode_tag)
%
%  A description of each parameter follows:
%
%    o image_info: The address of a structure of type ImageInfo.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned int InvokeDelegate(const ImageInfo *image_info,
  Image *image,const char *decode_tag,const char *encode_tag)
{
  char
    *command,
    **commands,
    filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  register int
    i;

  unsigned int
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) strcpy(filename,image->filename);
  if (!GetDelegateInfo(decode_tag,encode_tag,&delegate_info))
    ThrowBinaryException(MissingDelegateWarning,"no tag found",
      decode_tag ? decode_tag : encode_tag);
  if (LocaleCompare(delegate_info.decode_tag,"YUV") == 0)
    if ((LocaleCompare(delegate_info.encode_tag,"M2V") == 0) ||
        (LocaleCompare(delegate_info.encode_tag,"MPG") == 0))
      {
        FILE
          *file;

        unsigned int
          mpeg;

        /*
          Write parameter file (see mpeg2encode documentation for details).
        */
        CoalesceImages(image,&image->exception);
        mpeg=LocaleCompare(delegate_info.encode_tag,"M2V") != 0;
        file=fopen(image_info->unique,"w");
        if (file == (FILE *) NULL)
          ThrowBinaryException(DelegateWarning,"delegate failed",
            decode_tag ? decode_tag : encode_tag);
        (void) fprintf(file,"MPEG\n");
        (void) fprintf(file,"%.1024s%%d\n",image->filename);
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"%s\n",image_info->zero);
        (void) fprintf(file,"1\n");
        (void) fprintf(file,"%u\n",GetNumberScenes(image));
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"00:00:00:00\n");
        (void) fprintf(file,"%d\n",mpeg ? 12 : 15);
        (void) fprintf(file,"3\n");
        (void) fprintf(file,"%d\n",mpeg ? 1 : 0);
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"%u\n",image->columns+
          (image->columns & 0x01 ? 1 : 0));
        (void) fprintf(file,"%u\n",image->rows+(image->rows & 0x01 ? 1 : 0));
        (void) fprintf(file,"%d\n",mpeg ? 8 : 2);
        (void) fprintf(file,"%d\n",mpeg ? 3 : 5);
        (void) fprintf(file,"%.1f\n",mpeg ? 1152000.0 : 5000000.0);
        (void) fprintf(file,"%d\n",mpeg ? 20 : 112);
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"%d\n",mpeg ? 1 : 0);
        (void) fprintf(file,"4\n");
        (void) fprintf(file,"8\n");
        (void) fprintf(file,"%d\n",mpeg ? 1 : 0);
        (void) fprintf(file,"1\n");
        (void) fprintf(file,"%d\n",mpeg ? 1 : 2);
        (void) fprintf(file,"5\n");
        (void) fprintf(file,"5\n");
        (void) fprintf(file,"%d\n",mpeg ? 5 : 4);
        (void) fprintf(file,"%u\n",image->columns+
          (image->columns & 0x01 ? 1 : 0));
        (void) fprintf(file,"%u\n",image->rows+(image->rows & 0x01 ? 1 : 0));
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"%d\n",mpeg ? 0 : 1);
        (void) fprintf(file,"%d %d %d\n",mpeg ? 1 : 0,mpeg ? 1 : 0,
          mpeg ? 1 : 0);
        (void) fprintf(file,"0 0 0\n");
        (void) fprintf(file,"%d %d %d\n",mpeg ? 0 : 1,mpeg ? 0 : 1,
          mpeg ? 0 : 1);
        (void) fprintf(file,"%d 0 0\n",mpeg ? 0 : 1);
        (void) fprintf(file,"0 0 0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"%d\n",mpeg ? 1 : 0);
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"0\n");
        (void) fprintf(file,"2 2 11 11\n");
        (void) fprintf(file,"1 1 3 3\n");
        (void) fprintf(file,"1 1 7 7\n");
        (void) fprintf(file,"1 1 7 7\n");
        (void) fprintf(file,"1 1 3 3\n");
        (void) fclose(file);
        (void) strcat(image->filename,"%d.yuv");
      }
  if (delegate_info.direction != 0)
    if ((decode_tag && (*delegate_info.encode_tag != '\0')) ||
        (encode_tag && (*delegate_info.decode_tag != '\0')))
      {
        char
          filename[MaxTextExtent],
          *magick;

        ImageInfo
          *clone_info;

        register Image
          *p;

        /*
          Delegate requires a particular image format.
        */
        magick=TranslateText(image_info,image,decode_tag != (char *) NULL ?
          delegate_info.encode_tag : delegate_info.decode_tag);
        if (magick == (char *) NULL)
          ThrowBinaryException(DelegateWarning,"delegate failed",
            decode_tag ? decode_tag : encode_tag);
        LocaleUpper(magick);
        (void) strcpy((char *) image_info->magick,magick);
        (void) strcpy(image->magick,magick);
        LiberateMemory((void **) &magick);
        (void) strcpy(filename,image->filename);
        clone_info=CloneImageInfo(image_info);
        if (clone_info == (ImageInfo *) NULL)
          ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
            (char *) NULL);
        FormatString(clone_info->filename,"%.1024s:",delegate_info.decode_tag);
        SetImageInfo(clone_info,True);
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          FormatString(p->filename,"%.1024s:%.1024s",delegate_info.decode_tag,
            filename);
          status=WriteImage(clone_info,p);
          if (status == False)
            ThrowBinaryException(DelegateWarning,"delegate failed",
              decode_tag ? decode_tag : encode_tag);
          if (clone_info->adjoin)
            break;
        }
        DestroyImageInfo(clone_info);
      }
  (void) strcpy(image->filename,filename);
  commands=StringToList(delegate_info.commands);
  if (commands == (char **) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      decode_tag ? decode_tag : encode_tag);
  command=(char *) NULL;
  status=True;
  for (i=0; commands[i] != (char *) NULL; i++)
  {
    status=True;
    command=TranslateText(image_info,image,commands[i]);
    if (command == (char *) NULL)
      break;
    /*
      Execute delegate.
    */
    status=SystemCommand(image_info->verbose,command);
    LiberateMemory((void **) &command);
    if (status != False)
      ThrowBinaryException(DelegateWarning,"delegate failed",commands[i]);
    LiberateMemory((void **) &commands[i]);
  }
  /*
    Free resources.
  */
  (void) remove(image_info->unique);
  (void) remove(image_info->zero);
  for ( ; commands[i] != (char *) NULL; i++)
    LiberateMemory((void **) &commands[i]);
  LiberateMemory((void **) &commands);
  return(!status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t D e l e g a t e I n f o                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListDelegateInfo lists the image formats to a file.
%
%  The format of the ListDelegateInfo method is:
%
%      unsigned int ListDelegateInfo(FILE *file)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%
*/
MagickExport unsigned int ListDelegateInfo(FILE *file)
{
  char
    delegate[MaxTextExtent],
    tag[MaxTextExtent];

  DelegateInfo
    *delegates;

  register DelegateInfo
    *p;

  register int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;
  AcquireSemaphore(delegate_semaphore);
  delegates=SetDelegateInfo((DelegateInfo *) NULL);
  LiberateSemaphore(delegate_semaphore);
  if (delegates == (DelegateInfo *) NULL)
    {
      MagickWarning(DelegateWarning,"no delegates configuration file found",
        DelegateFilename);
      return(False);
    }
  (void) fprintf(file,"\nImageMagick uses these delegates to read or write "
    "image formats it does not\ndirectly support:\n\n");
  (void) fprintf(file,"Decode-Tag   Encode-Tag  Delegate\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------\n");
  for (p=delegates->next; p != (DelegateInfo *) NULL; p=p->next)
  {
    i=0;
    if (p->commands != (char *) NULL)
      for (i=0; !isspace((int) p->commands[i]); i++)
        delegate[i]=p->commands[i];
    delegate[i]='\0';
    for (i=0; i < 10; i++)
      tag[i]=' ';
    tag[i]='\0';
    if (p->encode_tag != (char *) NULL)
      (void) strncpy(tag,p->encode_tag,strlen(p->encode_tag));
    (void) fprintf(file,"%10s%s=%s%s  %s\n",
      p->decode_tag ? p->decode_tag : "",p->direction <= 0 ? "<" : " ",
      p->direction >= 0 ? ">" : " ",tag,delegate);
  }
  (void) fflush(file);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d D e l e g a t e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDelegates reads the delegates configuration file.
%
%  The format of the ReadDelegates method is:
%
%      unsigned int ReadDelegates(void)
%
%  A description of each parameter follows:
%
%    o status: Method ReadDelegates returns True if at least one delegate
%      can be read otherwise False.
%
%
*/
static unsigned int ReadDelegates(void)
{
  char
    *path,
    text[MaxTextExtent];

  DelegateInfo
    delegate_info;

  FILE
    *file;

  register char
    *p;

  unsigned int
    number_delegates;

  /*
    Read delegate file.
  */
  number_delegates=0;
  path=GetMagickConfigurePath(DelegateFilename);
  if (path == (char *) NULL)
    return(False);
  file=fopen(path,"r");
  LiberateMemory((void **) &path);
  if (file == (FILE *) NULL)
    return(False);
  while (fgets(text,MaxTextExtent,file) != (char *) NULL)
  {
    if (*text == '#')
      continue;
    Strip(text);
    if (*text == '\0')
      continue;
    *delegate_info.decode_tag='\0';
    *delegate_info.encode_tag='\0';
    for (p=text; (*p != '<') && (*p != '=') && (*p != '\0'); p++);
    (void) strncpy(delegate_info.decode_tag,text,p-text);
    delegate_info.decode_tag[p-text]='\0';
    Strip(delegate_info.decode_tag);
    delegate_info.direction=0;
    if (*p == '<')
      {
        delegate_info.direction--;
        p++;
      }
    if (*p == '=')
      p++;
    if (*p == '>')
      {
        delegate_info.direction++;
        p++;
      }
    while (isspace((int) *p))
      p++;
    if (*p != '0')
      (void) strcpy(delegate_info.encode_tag,p);
    Strip(delegate_info.encode_tag);
    delegate_info.commands=(char *) NULL;
    while (fgets(text,MaxTextExtent,file) != (char *) NULL)
    {
      if (*text != '\t')
        break;
      Strip(text);
      if (delegate_info.commands != (char *) NULL)
        ReacquireMemory((void **) &delegate_info.commands,
          (strlen(delegate_info.commands)+strlen(text)+3));
      else
        {
          delegate_info.commands=(char *) AcquireMemory(strlen(text)+3);
          if (delegate_info.commands != (char *) NULL)
            *delegate_info.commands='\0';
        }
      if (delegate_info.commands == (char *) NULL)
        break;
      (void) strcat(delegate_info.commands,text);
      if (delegate_info.commands[strlen(delegate_info.commands)-1] != '\\')
        (void) strcat(delegate_info.commands,"\n");
      else
        delegate_info.commands[strlen(delegate_info.commands)-1]='\0';
    }
    if (delegate_info.commands == (char *) NULL)
      MagickWarning(DelegateWarning,"no command for this delegate",
        delegate_info.decode_tag);
    else
      {
        /*
          Add delegate to the delegate list.
        */
        Strip(delegate_info.commands);
        (void) SetDelegateInfo(&delegate_info);
        number_delegates++;
        LiberateMemory((void **) &delegate_info.commands);
      }
  }
  (void) fclose(file);
  return(number_delegates != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t D e l e g a t e I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetDelegateInfo adds or replaces a delegate in the delegate list and
%  returns the address of the first delegate.  If the delegate is NULL, just
%  the address of the first delegate is returned.
%
%  The format of the SetDelegateInfo method is:
%
%      DelegateInfo *SetDelegateInfo(DelegateInfo *delegate_info)
%
%  A description of each parameter follows:
%
%    o delegate_info: Method SetDelegateInfo returns the address of the
%      first delegate in the delegates list.
%
%    o delegate_info:  A structure of type DelegateInfo.  This information
%      is added to the end of the delegates linked-list.
%
%
*/
MagickExport DelegateInfo *SetDelegateInfo(DelegateInfo *delegate_info)
{
  DelegateInfo
    *delegate;

  register DelegateInfo
    *p;

  if (delegate_info == (DelegateInfo *) NULL)
    {
      /*
        Read delegate configuration file.
      */
      (void) ReadDelegates();
      atexit(DestroyDelegateInfo);
      return(delegates);
    }
  /*
    Initialize new delegate.
  */
  delegate=(DelegateInfo *) AcquireMemory(sizeof(DelegateInfo));
  if (delegate == (DelegateInfo *) NULL)
    return(delegates);
  (void) strcpy(delegate->decode_tag,delegate_info->decode_tag);
  (void) strcpy(delegate->encode_tag,delegate_info->encode_tag);
  delegate->direction=delegate_info->direction;
  delegate->commands=(char *) NULL;
  if (delegate_info->commands != (char *) NULL)
    {
      /*
        Note commands associated with this delegate.
      */
      delegate->commands=(char *)
        AcquireMemory(strlen(delegate_info->commands)+1);
      if (delegate->commands == (char *) NULL)
        return(delegates);
      (void) strcpy(delegate->commands,delegate_info->commands);
    }
  delegate->signature=MagickSignature;
  delegate->previous=(DelegateInfo *) NULL;
  delegate->next=(DelegateInfo *) NULL;
  if (delegates == (DelegateInfo *) NULL)
    {
      delegates=delegate;
      LiberateSemaphore(delegate_semaphore);
      return(delegates);
    }
  for (p=delegates; p != (DelegateInfo *) NULL; p=p->next)
  {
    if ((LocaleCompare(p->decode_tag,delegate_info->decode_tag) == 0) &&
        (LocaleCompare(p->encode_tag,delegate_info->encode_tag) == 0) &&
        (p->direction == delegate_info->direction))
      {
        /*
          Delegate overrides an existing one with the same tags.
        */
        LiberateMemory((void **) &p->commands);
        p->commands=delegate->commands;
        LiberateMemory((void **) &delegate);
        return(delegates);
      }
    if (p->next == (DelegateInfo *) NULL)
      break;
  }
  /*
    Place new delegate at the end of the delegate list.
  */
  delegate->previous=p;
  p->next=delegate;
  return(delegates);
}
