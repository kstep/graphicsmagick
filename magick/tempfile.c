/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Temporary file allocation manager.
%
*/

#include "studio.h"
#include "error.h"
#include "log.h"
#include "temporary.h"
#include "utility.h"

/*
  Define declarations.
*/
#if defined(S_IRUSR) && defined(S_IWUSR)
#  define S_MODE     (S_IRUSR | S_IWUSR)
#elif defined (WIN32)
#  define S_MODE     (_S_IREAD | _S_IWRITE)
#else
# define S_MODE      0600
#endif

/*
  Type definitions
*/
typedef struct _TempfileInfo
{
  char
    filename[MaxTextExtent];

  struct _TempfileInfo
    *next;
} TempfileInfo;

static TempfileInfo *templist = 0;

static SemaphoreInfo
  *templist_semaphore = 0;

/*
  Add a temporary file to list
*/
static void AddTemporaryFileToList(const char *filename)
{
  /*
    Initialize semaphore
  */
  printf("AddTemporaryFileToList: Adding \"%s\" ...\n",filename);

  AcquireSemaphoreInfo(&templist_semaphore);
  {
    TempfileInfo
      *info;

    info=(TempfileInfo *) AcquireMemory(sizeof(TempfileInfo));
    if (info)
      {
        info->next=0;
        strcpy(info->filename,filename);
        if (!templist)
          templist=info;
        else
          {
            /* Add to front of list */
            info->next=templist;
            templist=info;
          }
      }
  }
  LiberateSemaphoreInfo(&templist_semaphore);
}

/*
  Remove a temporary file from the list.
  Return True if removed or False if not found
*/
static unsigned int RemoveTemporaryFileFromList(const char *filename)
{
  unsigned int
    status=False;

  printf("RemoveTemporaryFileFromList: Removing \"%s\"\n",filename);
  AcquireSemaphoreInfo(&templist_semaphore);
  {
    if (templist)
      {
        TempfileInfo
          *member,
          *remove;

        /* Check first element for match */
        if (strcmp(templist->filename,filename) == 0)
          {
            printf("Removing first element \"%s\"\n",templist->filename);
            fflush(stdout);
            /* Remove first element */
            remove=templist;
            if (templist->next)
              templist=templist->next;
            else
              templist=0;
            LiberateMemory((void **)&remove);
            status=True;
          }
        else
          {
            /* Check remaining elements for match */
            for (member=templist; member->next && member->next->next;
                 member=member->next)
              if (strcmp(member->next->filename,filename) == 0)
                {
                  /* Remove next element */
                  printf("Removing next element  \"%s\"\n",member->next->filename);
                  fflush(stdout);
                  remove=member->next;
                  if (member->next->next)
                    member->next=member->next->next;
                  else
                    member->next=0;
                  LiberateMemory((void **)&remove);
                  status=True;
                  break;
                }
          }
      }
  }
  LiberateSemaphoreInfo(&templist_semaphore);
  return status;
}
/*
  Compose a temporary file name based a template string provided by
  the user.  Any 'X' characters are replaced with characters randomly
  selected from a "safe" set of characters which are unlikely to
  be interpreted by a shell or program and cause confusion.
*/
#if defined(P_tmpdir)
static void ComposeTemporaryFileName(char *name)
{
  static const char SafeChars[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  char
    *c;

  for (c=name; *c; c++)
    {
      if (*c == 'X')
        *c=SafeChars[((sizeof(SafeChars)-1)*rand())/RAND_MAX];
    }
}
#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e N a m e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileName replaces the contents of the string buffer pointed
%  to by filename with a unique file name.  True is returned if a file name is
%  allocated, or False is returned if there is a failure.
%  When the filename is allocated, a temporary file is created on disk with
%  zero length, and read/write permission by the user.
%  The allocated filename should be recovered via the LiberateTemporaryFile()
%  function once it is no longer required.
%
%  The format of the AcquireTemporaryFileName method is:
%
%      unsigned int AcquireTemporaryFileName(char *filename,
%                                        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%   o status: True if a temporary file name is allocated and successfully
%             created on disk.  False if the temporary file name or file
%             creation fails.
%
%   o filename: Specifies a pointer to an array of characters.  The unique
%             file name is returned in this array.
%
*/
MagickExport unsigned int AcquireTemporaryFileName(char *filename)
{
  int
    fd;

  assert(filename != (char *) NULL);
  if ((fd=AcquireTemporaryFileDescriptor(filename)) != -1)
    {
      close(fd);
      return (True);
    }
  return (False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e D e s c r i p t o r               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileDescriptor replaces the contents of the string buffer
%  pointed to by filename with a unique file name created on disk.  A
%  read/write file descriptor (from open()) is returned on success, or -1 is
%  returned on failure. The temporary file is created on disk with read/write
%  permission by the user. The allocated temporary file should be deallocated
%  via the LiberateTemporaryFile() function once it is no longer required.
%
%  The format of the AcquireTemporaryFileDescriptor method is:
%
%      unsigned int AcquireTemporaryFileDescriptor(char *filename)
%
%  A description of each parameter follows.
%
%   o status: The file descriptor for an open file on success, or -1
%             on failure.
%
%   o filename: Specifies a pointer to an array of characters with an
%             allocated length of at least MaxTextExtent.  The unique
%             file name associated with the descriptor is returned in
%             this array.
%
*/
MagickExport int AcquireTemporaryFileDescriptor(char *filename)
{
  char
    fname[MaxTextExtent];

  int
    fd=-1;

  assert(filename != (char *) NULL);
  filename[0]='\0';

#if defined(P_tmpdir)
  {
    /*
      Use our own temporary filename generator
    */
    char
      *tempdir=0,
      tempname[MaxTextExtent];

    int
      tries=0;

    tempdir=getenv("TMPDIR");
    if (!tempdir)
      tempdir=getenv("TMP");
    if (!tempdir)
      tempdir=P_tmpdir;

    for (tries=0; tries < 10; tries++)
      {
        strcpy(tempname,"gmXXXXXX.tmp");
        ComposeTemporaryFileName(tempname);
        strcpy(fname,tempdir);
        if (tempdir[strlen(tempdir)-1] != DirectorySeparator[0])
          strcat(fname,DirectorySeparator);
        strcat(fname,tempname);
        fd=open(fname,O_WRONLY | O_CREAT | O_BINARY | O_EXCL, S_MODE);
        if (fd != -1)
          {
            strcpy(filename,fname);
            AddTemporaryFileToList(filename);
            break;
          }
      }
    return (fd);
  }

#elif HAVE_MKSTEMP
  /*
    Use mkstemp().
    Mkstemp opens the the temporary file to assure that there is
    no race condition between allocating the name and creating the
    file. This helps improve security.
  */
  {
    (void) strcpy(fname,"gmXXXXXX");
    fd=mkstemp(fname);
    if (fd != -1)
      {
        strcpy(filename,fname);
        AddTemporaryFileToList(filename);
      }
    return (fd);
  }
#elif HAVE_TEMPNAM
  /*
    Use tempnam().
    Windows has _tempnam which works similar to Unix tempnam.
    Note that Windows _tempnam only produces temporary file
    names which are unique to the current process so we prepend
    the process ID to help ensure uniqueness. Windows _tempnam
    is documented to place sequential numbers in the file
    extension.
  */
  {
    char
      *name;

    (void) sprintf(fname,"gm%ld",(long) getpid());
    if ((name=tempnam(NULL,fname)))
      {
        (void) remove(filename);
        fd=open(name,O_WRONLY | O_CREAT | O_BINARY | O_EXCL, S_MODE);
        if (fd != -1)
          {
            (void) strncpy(filename,name,MaxTextExtent-1);
            AddTemporaryFileToList(filename);
          }
        LiberateMemory((void **) &name);
      }
    return (fd);
  }
#else
  /*
    Use ANSI C standard tmpnam
  */
  {
    if ((tmpnam(fname) == fname))
      {
        (void) remove(fname);
        fd=open(fname,O_WRONLY | O_CREAT | O_BINARY | O_EXCL, S_MODE);
        if (fd != -1)
          {
            (void) strncpy(filename,fname,MaxTextExtent-1);
            AddTemporaryFileToList(filename);
          }
      }
    return (fd);
  }
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e T e m p o r a r y F i l e S t r e a m                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireTemporaryFileStream replaces the contents of the string buffer
%  pointed to by filename with a unique file name.  A pointer to an open
%  stdio FILE stream is returned on success, or a null pointer is returned
%  on failure. The temporary file is created on disk with read/write
%  permission by the user. The allocated temporary file should be deallocated
%  via the LiberateTemporaryFile() function once it is no longer required.
%
%  The format of the AcquireTemporaryFile method is:
%
%      FILE *AcquireTemporaryFileStream(char *filename,unsigned int text_mode)
%
%  A description of each parameter follows.
%
%   o status: The file descriptor for an open file on success, or -1
%             on failure.
%
%   o filename: Specifies a pointer to an array of characters.  The unique
%             file name is returned in this array.
%
%   o mode:  Set to TextFileIOMode to open the file in text mode, otherwise
%            the file is opened in binary mode.
%
*/
MagickExport FILE *AcquireTemporaryFileStream(char *filename,
  FileIOMode mode)
{
  int
    fd;

  if ((fd=AcquireTemporaryFileDescriptor(filename)) != -1)
    {
      if (mode == TextFileIOMode)
        return fdopen(fd,"w+");
      return fdopen(fd,"wb+");
    }

  return (FILE *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y T e m p o r a r y F i l e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyTemporaryFiles reclaims all currently allocated temporary files,
%  removing the files from the filesystem if they still exist.
%
%      void LiberateTemporaryFile(void)
%
*/
MagickExport void DestroyTemporaryFiles(void)
{
  TempfileInfo
    *member,
    *liberate;

  member=templist;
  while(member)
    {
      liberate=member;
      member=member->next;
      printf("DestroyTemporaryFiles: Removing \"%s\" ...\n",liberate->filename);
      if ((remove(liberate->filename)) != 0)
        printf("DestroyTemporaryFiles: Failure to remove \"%s\"!\n",liberate->filename);
      LiberateMemory((void **)&liberate);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   L i b e r a t e T e m p o r a r y F i l e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateTemporaryFile deallocates the allocated temporary file, removing
%  the temporary file from the filesystem if it still exists.
%
%      void LiberateTemporaryFile(const char *filename)
%
%  A description of each parameter follows.
%
%   o filename: Specifies a pointer to an array of characters representing
%               the temporary file to reclaim.
%
*/
MagickExport void LiberateTemporaryFile(const char *filename)
{
  if (RemoveTemporaryFileFromList(filename))
    {
      if ((remove(filename)) != 0)
        printf("LiberateTemporaryFile: Failure to remove \"%s\"!\n",filename);
    }
  else
    printf("LiberateTemporaryFile: \"%s\" not in list!\n",filename);
    
}
