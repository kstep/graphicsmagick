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
%                       Read/Write/Invoke Delegates                           %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                               October 1998                                  %
%                                                                             %
%                                                                             %
%  Copyright 1998 E. I. du Pont de Nemours and Company                        %
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
%  Method ReadDelegates reads one or more delegates from a file on disk
%  specified by the given path and directory name.  True is returned if
%  the delegates file is read and at least one delegate is noted.
%
%  The format of the ReadDelegates routine is:
%
%     status=ReadDelegates(path,directory)
%
%  A description of each parameter follows:
%
%    o status: Method ReadDelegates returns True if at least one delegate
%      can be read otherwise False.
%
%    o path:  The path component of the delegates filename.
%
%    o directory:  The directory component of the delegates filename.
%
%
*/
static unsigned int ReadDelegates(char *path,char *directory)
{
  char
    filename[MaxTextExtent],
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
    Determine delegate filename.
  */
  *filename='\0';
  if (path != (char *) NULL)
    (void) strcat(filename,path);
  if (directory != (char *) NULL)
    (void) strcat(filename,directory);
  (void) strcat(filename,DelegateFilename);
  /*
    Read delegate file.
  */
  number_delegates=0;
  file=fopen(filename,"r");
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
        delegate_info.direction=(-1);
        *p++;
      }
    if (*p == '=')
      *p++;
    if (*p == '>')
      {
        delegate_info.direction=1;
        *p++;
      }
    while (isspace(*p))
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
        delegate_info.commands=(char *) ReallocateMemory(delegate_info.commands,
          (strlen(delegate_info.commands)+strlen(text)+2)*sizeof(char));
      else
        {
          delegate_info.commands=(char *)
            AllocateMemory((strlen(text)+2)*sizeof(char));
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
      {
        MagickWarning(DelegateWarning,"no commands for this delgate",
          delegate_info.decode_tag);
        continue;
      }
    /*
      Add delegate to the delegate list.
    */
    Strip(delegate_info.commands);
    (void) SetDelegateInfo(&delegate_info);
    number_delegates++;
    FreeMemory((char *) delegate_info.commands);
  }
  return(number_delegates != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y D e l e g a t e s                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyDelegates deallocates memory associated with the delegates
%  list.
%
%  The format of the DestroyDelegates routine is:
%
%      DestroyDelegates(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a Delegates structure.
%
%
*/
Export void DestroyDelegates(void)
{
  DelegateInfo
    *delegate,
    *delegates;

  register DelegateInfo
    *p;

  delegates=SetDelegateInfo((DelegateInfo *) NULL);
  if (delegates == (DelegateInfo *) NULL)
    return;
  for (p=delegates; p != (DelegateInfo *) NULL; )
  {
    if (p->commands != (char *) NULL)
      FreeMemory((char *) p->commands);
    delegate=p;
    p=p->next;
    FreeMemory((char *) delegate);
  }
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
%  The format of the GetDelegateInfo routine is:
%
%     status=GetDelegateInfo(tag,decode,delegate_info)
%
%  A description of each parameter follows:
%
%    o status: Method GetDelegateInfo returns True if a delegate is found,
%      otherwise False.
%
%    o tag: Specifies the delegate we are searching for as a character
%      string.
%
%    o decode: An unsigned integer other than zero means to match decode
%      tages in the delegate list otherwise encode tags.
%
%    o delegate_info:  A structure of type DelegateInfo.  On return this
%      structure contains the delegate information for the specified
%      tag.
%
%
*/
Export unsigned int GetDelegateInfo(char *tag,unsigned int decode,
  DelegateInfo *delegate_info)
{
  DelegateInfo
    *delegates;

  assert(delegate_info != (DelegateInfo *) NULL);
  delegates=SetDelegateInfo((DelegateInfo *) NULL);
  if (delegates == (DelegateInfo *) NULL)
    {
      DelegateInfo
        delegate_info;

      /*
        The delegate list is empty, read delegates from the configuration file.
      */
      *delegate_info.decode_tag='\0';
      delegate_info.commands=(char *) NULL;
      *delegate_info.encode_tag='\0';
      delegate_info.direction=0;
      (void) SetDelegateInfo(&delegate_info);
      (void) ReadDelegates(DelegatePath,"/ImageMagick/");
      (void) ReadDelegates((char *) getenv("HOME"),"/.magick/");
      (void) ReadDelegates((char *) NULL,(char *) NULL);
      delegates=SetDelegateInfo((DelegateInfo *) NULL);
      if (delegates->next == (DelegateInfo *) NULL)
        MagickWarning(DelegateWarning,"no delegates configuration file found",
          DelegateFilename);
    }
  if (decode)
    {
      /*
        Search for requested decode delegate.
      */
      for ( ; delegates != (DelegateInfo *) NULL; delegates=delegates->next)
        if (delegates->direction >= 0)
          if (Latin1Compare(delegates->decode_tag,tag) == 0)
            {
              *delegate_info=(*delegates);
              return(True);
            }
      return(False);
    }
  /*
    Search for requested encode delegate.
  */
  for ( ; delegates != (DelegateInfo *) NULL; delegates=delegates->next)
    if (delegates->direction <= 0)
      if (Latin1Compare(delegates->encode_tag,tag) == 0)
        {
          *delegate_info=(*delegates);
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
%  Method GetDelegateCommand replaces any embedded formating characters with
%  the appropriate image attribute and returns the resulting command.
%
%  The format of the GetDelegateCommand routine is:
%
%      command=GetDelegateCommand(image_info,image,tag,decode)
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
%    o tag: Specifies the delegate we are searching for as a character
%      string.
%
%    o decode: An unsigned integer other than zero means to match decode
%      tages in the delegate list otherwise encode tags.
%
%
*/
Export char *GetDelegateCommand(const ImageInfo *image_info,Image *image,
  char *tag,unsigned int decode)
{
  char
    *command,
    **commands;

  DelegateInfo
    delegate_info;

  register int
    i;

  if (!GetDelegateInfo(tag,decode,&delegate_info))
    {
      MagickWarning(MissingDelegateWarning,"no tag found",tag);
      return((char *) NULL);
    }
  commands=StringToList(delegate_info.commands);
  if (commands == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",tag);
      return((char *) NULL);
    }
  command=TranslateText(image_info,image,commands[0]);
  if (command == (char *) NULL)
    MagickWarning(ResourceLimitWarning,"Memory allocation failed",commands[0]);
  /*
    Free resources.
  */
  for (i=0; commands[i] != (char *) NULL; i++)
    FreeMemory((char *) commands[i]);
  FreeMemory((char *) commands);
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
%  Method InvokeDelegate replaces any embedded formating characters with
%  the appropriate image attribute and executes the resulting command.  False
%  is returned if the commands execute with success otherwise True.
%
%  The format of the InvokeDelegate routine is:
%
%      InvokeDelegate(image_info,image,tag,decode)
%
%  A description of each parameter follows:
%
%    o image_info: The address of a structure of type ImageInfo.
%
%    o image: The address of a structure of type Image.
%
%    o tag: Specifies the delegate we are searching for as a character
%      string.
%
%    o decode: An unsigned integer other than zero means to match decode
%      tages in the delegate list otherwise encode tags.
%
%
*/
Export unsigned int InvokeDelegate(const ImageInfo *image_info,Image *image,
  char *tag,unsigned int decode)
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

  (void) strcpy(filename,image->filename);
  if (!GetDelegateInfo(tag,decode,&delegate_info))
    {
      MagickWarning(MissingDelegateWarning,"no tag found",tag);
      return(True);
    }
  if (Latin1Compare(delegate_info.decode_tag,"YUV") == 0)
    if ((Latin1Compare(delegate_info.encode_tag,"M2V") == 0) ||
        (Latin1Compare(delegate_info.encode_tag,"MPG") == 0))
      {
        FILE
          *file;

        unsigned int
          mpeg;

        /*
          Write parameter file (see mpeg2encode documentation for details).
        */
        mpeg=Latin1Compare(delegate_info.encode_tag,"M2V") != 0;
        file=fopen(image_info->unique,"w");
        if (file == (FILE *) NULL)
          {
            MagickWarning(DelegateWarning,"delegate failed",tag);
            return(True);
          }
        (void) fprintf(file,"MPEG\n");
        (void) fprintf(file,"%s%%d\n",image->filename);
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"-\n");
        (void) fprintf(file,"%s.log\n",image_info->unique);
        (void) fprintf(file,"1\n");
        (void) fprintf(file,"%u\n",image->number_scenes);
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
  if ((decode && (*delegate_info.encode_tag != '\0')) ||
      (!decode && (*delegate_info.decode_tag != '\0')))
    {
      char
        filename[MaxTextExtent],
        *magick;

      ImageInfo
        local_info;

      register Image
        *p;

      /*
        Delegate requires a particular image format.
      */
      magick=TranslateText(image_info,image,decode ?
        delegate_info.encode_tag : delegate_info.decode_tag);
      if (magick == (char *) NULL)
        {
          MagickWarning(DelegateWarning,"delegate failed",tag);
          return(True);
        }
      Latin1Upper(magick);
      local_info=(*image_info);
      local_info.adjoin=False;
      (void) strcpy((char *) image_info->magick,magick);
      (void) strcpy(image->magick,magick);
      FreeMemory((char *) magick);
      (void) strcpy(filename,image->filename);
      for (p=image; p != (Image *) NULL; p=p->next)
      {
        (void) strcpy(p->filename,filename);
        status=WriteImage(&local_info,p);
        if (status == False)
          {
            MagickWarning(DelegateWarning,"delegate failed",tag);
            return(False);
          }
      }
    }
  (void) strcpy(image->filename,filename);
  commands=StringToList(delegate_info.commands);
  if (commands == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",tag);
      return(True);
    }
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
    status=SystemCommand(command);
    FreeMemory((char *) command);
    if (status != False)
      break;
    FreeMemory((char *) commands[i]);
  }
  if (status != False)
    MagickWarning(DelegateWarning,"delegate failed",command);
  /*
    Free resources.
  */
  for ( ; commands[i] != (char *) NULL; i++)
    FreeMemory((char *) commands[i]);
  FreeMemory((char *) commands);
  return(status);
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
%  The format of the SetDelegateInfo routine is:
%
%     delegate_info=SetDelegateInfo(delegate_info)
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
Export DelegateInfo *SetDelegateInfo(DelegateInfo *delegate_info)
{
  DelegateInfo
    *new_delegate;

  register DelegateInfo
    *p;

  static DelegateInfo
    *delegates = (DelegateInfo *) NULL;

  if (delegate_info == (DelegateInfo *) NULL)
    return(delegates);
  /*
    Initialize new delegate.
  */
  new_delegate=(DelegateInfo *) AllocateMemory(sizeof(DelegateInfo));
  if (new_delegate == (DelegateInfo *) NULL)
    return(delegates);
  (void) strcpy(new_delegate->decode_tag,delegate_info->decode_tag);
  (void) strcpy(new_delegate->encode_tag,delegate_info->encode_tag);
  new_delegate->direction=delegate_info->direction;
  new_delegate->commands=(char *) NULL;
  if (delegate_info->commands != (char *) NULL)
    {
      /*
        Note commands associated with this delegate.
      */
      new_delegate->commands=(char *)
        AllocateMemory((strlen(delegate_info->commands)+1)*sizeof(char));
      if (new_delegate->commands == (char *) NULL)
        return(delegates);
      (void) strcpy(new_delegate->commands,delegate_info->commands);
    }
  new_delegate->previous=(DelegateInfo *) NULL;
  new_delegate->next=(DelegateInfo *) NULL;
  if (delegates == (DelegateInfo *) NULL)
    {
      delegates=new_delegate;
      return(delegates);
    }
  for (p=delegates; p != (DelegateInfo *) NULL; p=p->next)
  {
    if ((Latin1Compare(p->decode_tag,delegate_info->decode_tag) == 0) &&
        (Latin1Compare(p->encode_tag,delegate_info->encode_tag) == 0) &&
        (p->direction == delegate_info->direction))
      {
        /*
          Delegate overrides an existing one with the same tags.
        */
        FreeMemory((char *) p->commands);
        p->commands=new_delegate->commands;
        FreeMemory((char *) new_delegate);
        return(delegates);
      }
    if (p->next == (DelegateInfo *) NULL)
      break;
  }
  /*
    Place new delegate at the end of the delegate list.
  */
  new_delegate->previous=p;
  p->next=new_delegate;
  return(delegates);
}
