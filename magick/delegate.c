/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%           DDDD   EEEEE  L      EEEEE   GGGG   AAA   TTTTT  EEEEE            %
%           D   D  E      L      E      G      A   A    T    E                %
%           D   D  EEE    L      EEE    G  GG  AAAAA    T    EEE              %
%           D   D  E      L      E      G   G  A   A    T    E                %
%           DDDD   EEEEE  LLLLL  EEEEE   GGG   A   A    T    EEEEE            %
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
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Delegates methods associate a set of commands with a particular
%  image format.  GraphicsMagick uses delegates for formats it does not handle
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
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#if defined(WIN32) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define DelegateFilename  "delegates.mgk"

/*
  Declare delegate map.
*/
static char
  *DelegateMap = (char *)
    "<?xml version=\"1.0\"?>"
    "<delegatemap>"
    "  <delegate stealth=\"True\" />"
    "</delegatemap>";

/*
  Global declaractions.
*/
static SemaphoreInfo
  *delegate_semaphore = (SemaphoreInfo *) NULL;

static DelegateInfo
  *delegate_list = (DelegateInfo *) NULL;

/*
  Forward declaractions.
*/
static unsigned int
  ReadConfigureFile(const char *,const unsigned long,ExceptionInfo *);

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
    *delegate_info;

  register DelegateInfo
    *p;

  AcquireSemaphoreInfo(&delegate_semaphore);
  for (p=delegate_list; p != (DelegateInfo *) NULL; )
  {
    delegate_info=p;
    p=p->next;
    if (delegate_info->path != (char *) NULL)
      MagickFreeMemory(delegate_info->path);
    if (delegate_info->decode != (char *) NULL)
      MagickFreeMemory(delegate_info->decode);
    if (delegate_info->encode != (char *) NULL)
      MagickFreeMemory(delegate_info->encode);
    if (delegate_info->commands != (char *) NULL)
      MagickFreeMemory(delegate_info->commands);
    MagickFreeMemory(delegate_info);
  }
  delegate_list=(DelegateInfo *) NULL;
  LiberateSemaphoreInfo(&delegate_semaphore);
  DestroySemaphoreInfo(&delegate_semaphore);
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
%        const char *decode,const char *encode,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o command: Method GetDelegateCommand returns the command associated
%      with specified delegate tag.
%
%    o image_info: The image info.
%
%    o image: The image.
%
%    o decode: Specifies the decode delegate we are searching for as a
%      character string.
%
%    o encode: Specifies the encode delegate we are searching for as a
%      character string.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport char *GetDelegateCommand(const ImageInfo *image_info,Image *image,
  const char *decode,const char *encode,ExceptionInfo *exception)
{
  char
    *command,
    **commands;

  const DelegateInfo
    *delegate_info;

  register long
    i;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  delegate_info=GetDelegateInfo(decode,encode,exception);
  if (delegate_info == (const DelegateInfo *) NULL)
    {
      ThrowException(exception,DelegateError,NoTagFound,
        decode ? decode : encode);
      return((char *) NULL);
    }
  commands=StringToList(delegate_info->commands);
  if (commands == (char **) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
        decode ? decode : encode);
      return((char *) NULL);
    }
  command=TranslateText(image_info,image,commands[0]);
  if (command == (char *) NULL)
    ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
      commands[0]);
  /*
    Free resources.
  */
  for (i=0; commands[i] != (char *) NULL; i++)
    MagickFreeMemory(commands[i]);
  MagickFreeMemory(commands);
  return(command);
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
%  tag.
%
%  The format of the GetDelegateInfo method is:
%
%      const DelegateInfo *GetDelegateInfo(const char *decode,
%        const char *encode,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o delgate_info: Method GetDelegateInfo returns any delegates associated
%      with the specified tag.
%
%    o decode: Specifies the decode delegate we are searching for as a
%      character string.
%
%    o encode: Specifies the encode delegate we are searching for as a
%      character string.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const DelegateInfo *GetDelegateInfo(const char *decode,
  const char *encode,ExceptionInfo *exception)
{
  register DelegateInfo
    *p;

  if (delegate_list == (DelegateInfo *) NULL)
    {
      AcquireSemaphoreInfo(&delegate_semaphore);
      if (delegate_list == (DelegateInfo *) NULL)
        (void) ReadConfigureFile(DelegateFilename,0,exception);
      LiberateSemaphoreInfo(&delegate_semaphore);
    }
  if ((LocaleCompare(decode,"*") == 0) && (LocaleCompare(encode,"*") == 0))
    return((const DelegateInfo *) delegate_list);
  /*
    Search for requested delegate.
  */
  AcquireSemaphoreInfo(&delegate_semaphore);
  for (p=delegate_list; p != (const DelegateInfo *) NULL; p=p->next)
  {
    if (p->mode > 0)
      {
        if (LocaleCompare(p->decode,decode) == 0)
          break;
        continue;
      }
    if (p->mode < 0)
      {
        if (LocaleCompare(p->encode,encode) == 0)
          break;
        continue;
      }
    if (LocaleCompare(decode,p->decode) == 0)
      if (LocaleCompare(encode,p->encode) == 0)
        break;
    if (LocaleCompare(decode,"*") == 0)
      if (LocaleCompare(encode,p->encode) == 0)
        break;
    if (LocaleCompare(decode,p->decode) == 0)
      if (LocaleCompare(encode,"*") == 0)
        break;
  }
  if (p != (DelegateInfo *) NULL)
    if (p != delegate_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (DelegateInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (DelegateInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(DelegateInfo *) NULL;
        p->next=delegate_list;
        delegate_list->previous=p;
        delegate_list=p;
      }
  LiberateSemaphoreInfo(&delegate_semaphore);
  return((const DelegateInfo *) p);
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
%      unsigned int InvokeDelegate(ImageInfo *image_info,Image *image,
%        const char *decode,const char *encode,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The imageInfo.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int InvokeDelegate(ImageInfo *image_info,Image *image,
  const char *decode,const char *encode,ExceptionInfo *exception)
{
  char
    *command,
    **commands,
    filename[MaxTextExtent];

  const DelegateInfo
    *delegate_info;

  register long
    i;

  unsigned int
    status,
    temporary_image_filename;

  /*
    Get delegate.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  temporary_image_filename=(*image->filename == '\0');
  if (temporary_image_filename)
    {
      /* Allocate a temporary filename if image is unnamed.  */
      if(!AcquireTemporaryFileName(image->filename))
        {
          (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image->filename);
          return(False);
        }
    }
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  delegate_info=GetDelegateInfo(decode,encode,exception);
  if (delegate_info == (DelegateInfo *) NULL)
    {
      if (temporary_image_filename)
        LiberateTemporaryFile(image->filename);
      (void) ThrowException(exception,DelegateError,NoTagFound,
        decode ? decode : encode);
      return(False);
    }

  if (*image_info->filename == '\0')
    {
      /* ReadImage will normally have already set image_info->filename
         to the name of a temporary file.  If not, then assign
         one. Setting image_info->temporary to True indicates that
         there is a temporary file to be removed later.  */
      if(!AcquireTemporaryFileName(image_info->filename))
        {
          if (temporary_image_filename)
            LiberateTemporaryFile(image->filename);
          (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image_info->filename);
          return(False);
        }
      image_info->temporary=True;
    }

  if (delegate_info->mode != 0)
    if ((decode && (delegate_info->encode != (char *) NULL)) ||
        (encode && (delegate_info->decode != (char *) NULL)))
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

        if (!AcquireTemporaryFileName(image_info->unique))
        {
          if (temporary_image_filename)
            LiberateTemporaryFile(image->filename);
          (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image_info->unique);
          return(False);
        }

        if (!AcquireTemporaryFileName(image_info->zero))
        {
          if (temporary_image_filename)
            LiberateTemporaryFile(image->filename);
          LiberateTemporaryFile(image_info->unique);
          (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image_info->zero);
          return(False);
        }
        /* Expand sprintf-style codes in delegate command to command string */
        magick=TranslateText(image_info,image,decode != (char *) NULL ?
          delegate_info->encode : delegate_info->decode);
        if (magick == (char *) NULL)
          {
            LiberateTemporaryFile(image_info->unique);
            LiberateTemporaryFile(image_info->zero);
            if (temporary_image_filename)
              LiberateTemporaryFile(image->filename);
            (void) ThrowException(exception,DelegateError,DelegateFailed,
              decode ? decode : encode);
            return(False);
          }
        LocaleUpper(magick);
        clone_info=CloneImageInfo(image_info);
        (void) strncpy((char *) clone_info->magick,magick,MaxTextExtent-1);
        (void) strncpy(image->magick,magick,MaxTextExtent-1);
        MagickFreeMemory(magick);
        (void) strncpy(filename,image->filename,MaxTextExtent-1);
        FormatString(clone_info->filename,"%.1024s:",delegate_info->decode);
        (void) SetImageInfo(clone_info,True,exception);
        (void) strncpy(clone_info->filename,image_info->filename,
          MaxTextExtent-1);
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          FormatString(p->filename,"%.1024s:%.1024s",delegate_info->decode,
            filename);
          status=WriteImage(clone_info,p);
          if (status == False)
            {
              LiberateTemporaryFile(image_info->unique);
              LiberateTemporaryFile(image_info->zero);
              if (temporary_image_filename)
                LiberateTemporaryFile(image->filename);
              DestroyImageInfo(clone_info);
              (void) ThrowException(exception,DelegateError,DelegateFailed,
                decode ? decode : encode);
              return(False);
            }
          if (clone_info->adjoin)
            break;
        }
        LiberateTemporaryFile(image_info->unique);
        LiberateTemporaryFile(image_info->zero);
        DestroyImageInfo(clone_info);
      }
  /*
    Invoke delegate.
  */
  (void) strncpy(image->filename,filename,MaxTextExtent-1);
  commands=StringToList(delegate_info->commands);
  if (commands == (char **) NULL)
    {
      if (temporary_image_filename)
        LiberateTemporaryFile(image->filename);
      (void) ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,decode ? decode : encode);
      return(False);
    }
  command=(char *) NULL;
  status=True;
  /* For each delegate command ... */
  for (i=0; commands[i] != (char *) NULL; i++)
  {
    status=True;
    /* Allocate convenience temporary files */
    if (!AcquireTemporaryFileName(image_info->unique))
    {
      (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image_info->unique);
      status=False;
      goto error_exit;
    }
    if (!AcquireTemporaryFileName(image_info->zero))
    {
      (void) ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,image_info->zero);
      LiberateTemporaryFile(image_info->unique);
      status=False;
      goto error_exit;
    }
    /* Expand sprintf-style codes in delegate command to command string */
    command=TranslateText(image_info,image,commands[i]);
    if (command == (char *) NULL)
      break;
    /* Handle commands which should be backgrounded */
    if (delegate_info->spawn)
      (void) ConcatenateString(&command," &");
    /* Execute delegate.  */
    status=SystemCommand(image_info->verbose,command);
    MagickFreeMemory(command);
    /* Liberate convenience temporary files */
    LiberateTemporaryFile(image_info->unique);
    LiberateTemporaryFile(image_info->zero);
    if (status != False)
      {
        (void) ThrowException(exception,DelegateError,DelegateFailed,
          commands[i]);
        goto error_exit;
      }
    MagickFreeMemory(commands[i]);
  }
  /*
    Free resources.
  */
 error_exit:
  if (temporary_image_filename)
    LiberateTemporaryFile(image->filename);
  for ( ; commands[i] != (char *) NULL; i++)
    MagickFreeMemory(commands[i]);
  MagickFreeMemory(commands);
  return(status != False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n v o k e P o s t s r i p t D e l e g a t e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InvokePostscriptDelegate() executes the postscript interpreter with the
%  specified command.
%
%  The format of the InvokePostscriptDelegate method is:
%
%      unsigned int InvokePostscriptDelegate(const unsigned int verbose,
%        const char *command)
%
%  A description of each parameter follows:
%
%    o status:  Method InvokePostscriptDelegate returns True is the command
%      is successfully executed, otherwise False.
%
%    o verbose: A value other than zero displays the command prior to
%      executing it.
%
%    o command: The address of a character string containing the command to
%      execute.
%
%
*/
MagickExport unsigned int InvokePostscriptDelegate(const unsigned int verbose,
  const char *command)
{
#if defined(HasGS) || defined(WIN32)
  char
    **argv;

  gs_main_instance
    *interpreter;

  int
    argc,
    code,
    status;

  register long
    i;

#if defined(WIN32)
  const GhostscriptVectors
    *gs_func;

  gs_func=NTGhostscriptDLLVectors();
#elif defined(HasGS)
  GhostscriptVectors
    gs_func_struct;

  const GhostscriptVectors
    *gs_func;

  gs_func=(&gs_func_struct);
  gs_func_struct.exit=gsapi_exit;
  gs_func_struct.init_with_args=gsapi_init_with_args;
  gs_func_struct.new_instance=gsapi_new_instance;
  gs_func_struct.run_string=gsapi_run_string;
  gs_func_struct.delete_instance=gsapi_delete_instance;
#endif
  if (gs_func == (GhostscriptVectors *) NULL)
    {
      if (verbose)
        (void) fputs(command,stdout);
      return(SystemCommand(verbose,command));
    }
  if (verbose)
    {
      (void) fputs("[ghostscript library]",stdout);
      (void) fputs(strchr(command,' '),stdout);
    }
  status=(gs_func->new_instance)(&interpreter,(void *) NULL);
  if (status < 0)
    return(False);
  argv=StringToArgv(command,&argc);
  if (argv == (char **) NULL)
    return(False);
  status=(gs_func->init_with_args)(interpreter,argc-1,argv+1);
  if (status == 0)
    status=(gs_func->run_string)
     (interpreter,"systemdict /start get exec\n",0,&code);
  (gs_func->exit)(interpreter);
  (gs_func->delete_instance)(interpreter);
  for (i=0; i < argc; i++)
    MagickFreeMemory(argv[i]);
  MagickFreeMemory(argv);
  if ((status == 0) || (status == -101))
    return(False);
  return(True);
#else
  return(SystemCommand(verbose,command));
#endif
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
%      unsigned int ListDelegateInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ListDelegateInfo(FILE *file,ExceptionInfo *exception)
{
  char
    **commands,
    delegate[MaxTextExtent];

  register long
    i;

  register const DelegateInfo
    *p;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetDelegateInfo("*","*",exception);
  AcquireSemaphoreInfo(&delegate_semaphore);
  for (p=delegate_list; p != (const DelegateInfo *) NULL; p=p->next)
  {
    if ((p->previous == (DelegateInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (DelegateInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,"Delegate             Command\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;
    *delegate='\0';
    if (p->encode != (char *) NULL)
      (void) strncpy(delegate,p->encode,MaxTextExtent-1);
    (void) strcat(delegate,"        ");
    delegate[8]='\0';
    commands=StringToList(p->commands);
    if (commands == (char **) NULL)
      continue;
    {
      int
        command_start_column,
        command_length,
        formatted_chars=0,
        length=0,
        screen_width=79,
        strip_length;
      
      char
        *s;

      /* Format output so that command spans multiple lines if
         necessary */
      if (getenv("COLUMNS"))
        screen_width=atoi(getenv("COLUMNS"))-1;
      command_length=strlen(commands[0]);
      command_start_column=fprintf(file,"%8s%c=%c%s  ",p->decode ? p->decode : "",
        p->mode <= 0 ? '<' : ' ',p->mode >= 0 ? '>' : ' ',delegate);
      for (s=commands[0]; length < command_length; s+=formatted_chars)
        {
          if (s != commands[0])
            fprintf(file,"%*s",command_start_column,"");
          strip_length=screen_width-command_start_column;
          if (length+strip_length < command_length)
            {
              char
                *e;

              for(e=s+strip_length; (*e != ' ') && (e > s) ; e--);
              strip_length=e-s;
            }
          formatted_chars=fprintf(file,"%.*s",strip_length,s);
          length+=formatted_chars;
          fprintf(file,"\n");          
        }
    }
    for (i=0; commands[i] != (char *) NULL; i++)
      MagickFreeMemory(commands[i]);
  }
  (void) fflush(file);
  LiberateSemaphoreInfo(&delegate_semaphore);
  return(True);
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
    Read the delegates configure file.
  */
  (void) strcpy(path,basename);
  if (depth == 0)
    xml=(char *) GetConfigureBlob(basename,path,&length,exception);
  else
    xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    xml=AllocateString(DelegateMap);
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
                    ThrowException(exception,ConfigureError,IncludeElementNestedTooDeeply,path);
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
                  if (delegate_list != (DelegateInfo *) NULL)
                    while (delegate_list->next != (DelegateInfo *) NULL)
                      delegate_list=delegate_list->next;
                }
            }
          continue;
        }
      if (LocaleCompare(keyword,"<delegate") == 0)
        {
          DelegateInfo
            *delegate_info;

          /*
            Allocate memory for the delegate list.
          */
          delegate_info=MagickAllocateMemory(DelegateInfo *,sizeof(DelegateInfo));
          if (delegate_info == (DelegateInfo *) NULL)
            MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                             UnableToAllocateDelegateInfo);
          (void) memset(delegate_info,0,sizeof(DelegateInfo));
          delegate_info->path=AcquireString(path);
          delegate_info->signature=MagickSignature;
          if (delegate_list == (DelegateInfo *) NULL)
            {
              delegate_list=delegate_info;
              continue;
            }
          delegate_list->next=delegate_info;
          delegate_info->previous=delegate_list;
          delegate_list=delegate_list->next;
          continue;
        }
      if (delegate_list == (DelegateInfo *) NULL)
        continue;
      GetToken(q,(char **) NULL,token);
      if (*token != '=')
        continue;
      GetToken(q,&q,token);
      GetToken(q,&q,token);
      switch (*keyword)
        {
        case 'C':
        case 'c':
          {
            if (LocaleCompare((char *) keyword,"command") == 0)
              {
                delegate_list->commands=AllocateString(token);
#if defined(WIN32)
                if (strchr(delegate_list->commands,'@') != (char *) NULL)
                  {
                    char
                      BinPath[MaxTextExtent],
                      path[MaxTextExtent];

                    BinPath[0]=0;
                    /* Substitute @PSDelegate@ with path to Ghostscript */
                    NTGhostscriptEXE(path,MaxTextExtent-1);
                    SubstituteString((char **) &delegate_list->commands,
                                     "@PSDelegate@",path);

# if defined(UseInstalledMagick)
#  if defined(MagickBinPath)
                    strcpy(BinPath,MagickBinPath);
#  else
                    {
                      char
                        *key,
                        *key_value;
                    
                      /* Obtain installation path from registry */
                      key="BinPath";
                      key_value=NTRegistryKeyLookup(key);
                      if (!key_value)
                        {
                          ThrowException(exception,ConfigureError,
                              RegistryKeyLookupFailed,key);
                        }
                      else
                        {
                          strcpy(BinPath,key_value);
                          MagickFreeMemory(key_value);
                        }
                    }
#  endif /* defined(MagickBinPath) */
# else
                    /* Base path off of client path */
                    strcpy(BinPath,SetClientPath(NULL));
# endif /* defined(UseInstalledMagick) */
                    if ((BinPath[0] != 0) &&
                        (BinPath[strlen(BinPath)-1] != *DirectorySeparator))
                      strcat(BinPath,DirectorySeparator);

                    /* Substitute @GMDelegate@ with path to gm.exe */
                    strcpy(path,BinPath);
                    strcat(path,"gm.exe");
                    SubstituteString((char **) &delegate_list->commands,
                                     "@GMDelegate@",path);

                    /* Substitute @GMDisplayDelegate@ with path to
                       gmdisplay.exe */
                    strcpy(path,BinPath);
                    strcat(path,"gmdisplay.exe");
                    SubstituteString((char **) &delegate_list->commands,
                                     "@GMDisplayDelegate@",path);

                    /* Substitute @MPEGDecodeDelegate@ with path to
                       mpeg2dec.exe */
                    strcpy(path,BinPath);
                    strcat(path,"mpeg2dec.exe");
                    SubstituteString((char **) &delegate_list->commands,
                                     "@MPEGDecodeDelegate@",path);

                    /* Substitute @MPEGEncodeDelegate@ with path to
                       mpeg2enc.exe */
                    strcpy(path,BinPath);
                    strcat(path,"mpeg2enc.exe");
                    SubstituteString((char **) &delegate_list->commands,
                                     "@MPEGEncodeDelegate@",path);

                    /* Substitute @HPGLDecodeDelegate@ with path to
                       hp2xx.exe */
                    strcpy(path,BinPath);
                    strcat(path,"hp2xx.exe");
                    SubstituteString((char **) &delegate_list->commands,
                                     "@HPGLDecodeDelegate@",path);
                  }
#endif /* defined(WIN32) */
              } /* LocaleCompare */
            break;
          } /*  case 'c': */
          {
            break;
          }
        case 'D':
        case 'd':
          {
            if (LocaleCompare((char *) keyword,"decode") == 0)
              {
                delegate_list->decode=AcquireString(token);
                delegate_list->mode=1;
                break;
              }
            break;
          }
        case 'E':
        case 'e':
          {
            if (LocaleCompare((char *) keyword,"encode") == 0)
              {
                delegate_list->encode=AcquireString(token);
                delegate_list->mode=(-1);
                break;
              }
            break;
          }
        case 'M':
        case 'm':
          {
            if (LocaleCompare((char *) keyword,"mode") == 0)
              {
                delegate_list->mode=1;
                if (LocaleCompare(token,"bi") == 0)
                  delegate_list->mode=0;
                else
                  if (LocaleCompare(token,"encode") == 0)
                    delegate_list->mode=(-1);
                break;
              }
            break;
          }
        case 'S':
        case 's':
          {
            if (LocaleCompare((char *) keyword,"spawn") == 0)
              {
                delegate_list->spawn=LocaleCompare(token,"True") == 0;
                break;
              }
            if (LocaleCompare((char *) keyword,"stealth") == 0)
              {
                delegate_list->stealth=LocaleCompare(token,"True") == 0;
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
  if (delegate_list == (DelegateInfo *) NULL)
    return(False);
  while (delegate_list->previous != (DelegateInfo *) NULL)
    delegate_list=delegate_list->previous;
  return(True);
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
  register DelegateInfo
    *p;

  DelegateInfo
    *delegate;

  /*
    Initialize new delegate.
  */
  assert(delegate_info != (DelegateInfo *) NULL);
  assert(delegate_info->signature == MagickSignature);
  delegate=MagickAllocateMemory(DelegateInfo *,sizeof(DelegateInfo));
  if (delegate == (DelegateInfo *) NULL)
    return((DelegateInfo *) delegate_list);
  delegate->decode=AcquireString(delegate_info->decode);
  delegate->encode=AcquireString(delegate_info->encode);
  delegate->mode=delegate_info->mode;
  delegate->commands=(char *) NULL;
  if (delegate_info->commands != (char *) NULL)
    delegate->commands=AllocateString(delegate_info->commands);
  delegate->previous=(DelegateInfo *) NULL;
  delegate->next=(DelegateInfo *) NULL;
  if (delegate_list == (DelegateInfo *) NULL)
    {
      delegate_list=delegate;
      return((DelegateInfo *) delegate_list);
    }
  for (p=delegate_list; p != (DelegateInfo *) NULL; p=p->next)
  {
    if ((LocaleCompare(p->decode,delegate_info->decode) == 0) &&
        (LocaleCompare(p->encode,delegate_info->encode) == 0) &&
        (p->mode == delegate_info->mode))
      {
        /*
          Delegate overrides an existing one with the same tags.
        */
        MagickFreeMemory(p->commands);
        p->commands=delegate->commands;
        MagickFreeMemory(delegate);
        return((DelegateInfo *) delegate_list);
      }
    if (p->next == (DelegateInfo *) NULL)
      break;
  }
  /*
    Place new delegate at the end of the delegate list.
  */
  delegate->previous=p;
  p->next=delegate;
  return((DelegateInfo *) delegate_list);
}
