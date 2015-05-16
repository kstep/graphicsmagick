/*
% Copyright (C) 2003 - 2015 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Temporary file allocation manager.
%
*/

#include "magick/studio.h"
#include "magick/error.h"
#include "magick/log.h"
#include "magick/random.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#if defined(S_IRUSR) && defined(S_IWUSR)
#  define S_MODE     (S_IRUSR | S_IWUSR)
#elif defined (MSWINDOWS)
#  define S_MODE     (_S_IREAD | _S_IWRITE)
#else
# define S_MODE      0600
#endif

#if defined(MSWINDOWS)
# if defined(_P_tmpdir) && !defined(P_tmpdir)
#  define P_tmpdir _P_tmpdir
# endif
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

static TempfileInfo
  *templist = 0;

static SemaphoreInfo
  *templist_semaphore = 0;

/*
  Add a temporary file to list
*/
static void AddTemporaryFileToList(const char *filename)
{
  (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
    "Allocating temporary file \"%s\"",filename);
  LockSemaphoreInfo(templist_semaphore);
  {
    TempfileInfo
      *info;

    info=MagickAllocateMemory(TempfileInfo *,sizeof(TempfileInfo));
    if (info)
      {
        info->next=0;
        (void) strlcpy(info->filename,filename,sizeof(info->filename));
        if (!templist)
          templist=info;
        else
          {
            info->next=templist;
            templist=info;
          }
      }
  }
  UnlockSemaphoreInfo(templist_semaphore);
}

/*
  Remove a temporary file from the list.
  Return MagickPass if removed or MagickFail if not found
*/
static MagickPassFail RemoveTemporaryFileFromList(const char *filename)
{
  MagickPassFail
    status=MagickFail;

  (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
    "Deallocating temporary file \"%s\"",filename);

  LockSemaphoreInfo(templist_semaphore);
  {
    TempfileInfo
      *current,
      *previous=0;

    for (current=templist; current; current=current->next)
      {
        if (strcmp(current->filename,filename) == 0)
          {
            if (previous)
              previous->next=current->next;
            else
              templist=current->next;
            MagickFreeMemory(current);
            status=MagickPass;
            break;
          }
        previous=current;
      }
  }
  UnlockSemaphoreInfo(templist_semaphore);
  return status;
}
/*
  Compose a temporary file name based a template string provided by
  the user.  Any 'X' characters are replaced with characters randomly
  selected from a "safe" set of characters which are unlikely to
  be interpreted by a shell or program and cause confusion.
*/
static void ComposeTemporaryFileName(char *name)
{
  static const char SafeChars[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  char
    *c;

  assert(name != (char *) NULL);

  for (c=name; *c; c++)
    {
      if (*c == 'X')
	*c=SafeChars[MagickRandomInteger() % (sizeof(SafeChars)-1)];
    }
}

/*
  Validate a temporary directory path
*/
static MagickPassFail ValidateTemporaryFileDirectory(const char *tempdir)
{
  assert(tempdir != (char *) NULL);

  if (tempdir[0] == '\0')
    return MagickFail;
  if (access(tempdir,W_OK) != 0)
    return MagickFail;
  return MagickPass;
}


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
%  to by filename with a unique file name.  MagickPass is returned if a file
%  name is allocated, or MagickFail is returned if there is a failure.
%  When the filename is allocated, a temporary file is created on disk with
%  zero length, and read/write permission by the user.
%  The allocated filename should be recovered via the LiberateTemporaryFile()
%  function once it is no longer required.
%
%  The format of the AcquireTemporaryFileName method is:
%
%      MagickPassFail AcquireTemporaryFileName(char *filename,
%                                              ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%   o status: MagickPass if a temporary file name is allocated and successfully
%             created on disk.  MagickFail if the temporary file name or file
%             creation fails.
%
%   o filename: Specifies a pointer to an array of characters.  The unique
%             file name is returned in this array.
%
*/
MagickExport MagickPassFail AcquireTemporaryFileName(char *filename)
{
  int
    fd;

  assert(filename != (char *) NULL);
  if ((fd=AcquireTemporaryFileDescriptor(filename)) != -1)
    {
      (void) close(fd);
      return (MagickPass);
    }
  return (MagickFail);
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
%      int AcquireTemporaryFileDescriptor(char *filename)
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
  static const char *env_strings[] =
    {
      "MAGICK_TMPDIR",
#if defined(POSIX)
      "TMPDIR",
#endif /* defined(POSIX) */
#if defined(MSWINDOWS) || defined(__CYGWIN__)
      "TMP",
      "TEMP",
#endif
      NULL
    };

  static const char *fixed_strings[] =
    {
#if defined(P_tmpdir)
      P_tmpdir,
#endif
      NULL
    };

  char
    tempdir[MaxTextExtent];

  unsigned int
    i;

  int
    fd=-1;

  assert(filename != (char *) NULL);
  filename[0]='\0';
  tempdir[0]='\0';

  for (i=0; i < sizeof(env_strings)/sizeof(env_strings[0]); i++)
    {
      const char
        *env;

      if (env_strings[i] == NULL)
        break;
      if ((env=getenv(env_strings[i])) != NULL)
        {
          const size_t copy_len = sizeof(tempdir)-16;
          if (strlcpy(tempdir,env,copy_len) >= copy_len)
            tempdir[0]='\0';
          if ((tempdir[0] != '\0') &&
              !ValidateTemporaryFileDirectory(tempdir))
            tempdir[0]='\0';
          if (tempdir[0] != '\0')
            break;
        }
    }

  if (tempdir[0] == '\0')
    for (i=0; i < sizeof(fixed_strings)/sizeof(fixed_strings[0]); i++)
      {
        const size_t copy_len = sizeof(tempdir)-16;

        if (fixed_strings[i] == NULL)
          break;
        if (strlcpy(tempdir,fixed_strings[i],copy_len) >= copy_len)
          tempdir[0]='\0';
        if ((tempdir[0] != '\0') &&
            !ValidateTemporaryFileDirectory(tempdir))
          tempdir[0]='\0';
        if (tempdir[0] != '\0')
          break;
      }

  if (tempdir[0] != '\0')
    {
      char
        tempname[16];
      
      int
        tries=0;
      
      for (tries=0; tries < 256; tries++)
        {
          (void) strlcpy(tempname,"gmXXXXXX",sizeof(tempname));
          ComposeTemporaryFileName(tempname);
          if (strlcpy(filename,tempdir,MaxTextExtent) >= MaxTextExtent)
            break;
          if (filename[strlen(filename)-1] != DirectorySeparator[0])
            if (strlcat(filename,DirectorySeparator,MaxTextExtent) >=
                MaxTextExtent)
              break;
          if (strlcat(filename,tempname,MaxTextExtent) >= MaxTextExtent)
            break;
          fd=open(filename,O_RDWR | O_CREAT | O_BINARY | O_EXCL, S_MODE);
          if (fd != -1)
            {
              AddTemporaryFileToList(filename);
              return (fd);
            }
        }
    }

  return fd;
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
%      FILE *AcquireTemporaryFileStream(char *filename,FileIOMode mode)
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
%  removing the files from the filesystem if they still exist.  Also destroys
%  the associated semaphore so that temporary file system can not be used
%  again without invoking InitializeTemporaryFiles() to re-initialize it.
%
%      void DestroyTemporaryFiles(void)
%
*/
MagickExport void DestroyTemporaryFiles(void)
{
  PurgeTemporaryFiles();
  DestroySemaphoreInfo(&templist_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e T e m p o r a r y F i l e s                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeTemporaryFiles initializes the temporary file facility
%
%  The format of the InitializeTemporaryFiles method is:
%
%      MagickPassFail InitializeTemporaryFiles(void)
%
%
*/
MagickPassFail
InitializeTemporaryFiles(void)
{
  assert(templist_semaphore == (SemaphoreInfo *) NULL);
  templist_semaphore=AllocateSemaphoreInfo();
  return MagickPass;
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
%  the temporary file from the filesystem if it still exists. If the name
%  provided is a valid temporary file name, then the first position in the
%  string buffer is set to null in order to avoid accidental continued use.
%  If the name is not contained in the temporary file list, then it is left
%  unmodified.
%
%      MagickPassFail LiberateTemporaryFile(char *filename)
%
%  A description of each parameter follows.
%
%   o filename: Specifies a pointer to an array of characters representing
%               the temporary file to reclaim.
%
*/
MagickExport MagickPassFail LiberateTemporaryFile(char *filename)
{
  MagickPassFail
    status = MagickFail;

  if (RemoveTemporaryFileFromList(filename))
    {
      if ((remove(filename)) == 0)
        status=MagickPass;
      else
        (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
           "Temporary file removal failed \"%s\"",filename);
      /* Erase name so it is not accidentally used again */
      filename[0]='\0';
    }
  else
    (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
      "Temporary file \"%s\" to be removed not allocated!",filename);
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P u r g e T e m p o r a r y F i l e s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PurgeTemporaryFiles reclaims all currently allocated temporary files,
%  removing the files from the filesystem if they still exist.  The temporary
%  file allocation system remains usable after this function is called.
%
%      void PurgeTemporaryFiles(void)
%
*/
MagickExport void PurgeTemporaryFiles(void)
{
  TempfileInfo
    *member,
    *liberate;

  member=templist;
  templist=0;
  while(member)
    {
      liberate=member;
      member=member->next;
      (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
        "Removing leaked temporary file \"%s\"",liberate->filename);
      if ((remove(liberate->filename)) != 0)
        (void) LogMagickEvent(TemporaryFileEvent,GetMagickModule(),
          "Temporary file removal failed \"%s\"",liberate->filename);
      liberate->next=0;
      MagickFreeMemory(liberate);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P u r g e T e m p o r a r y F i l e s A s y n c S a f e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PurgeTemporaryFiles reclaims all currently allocated temporary files,
%  removing the files from the filesystem if they still exist.  This version
%  is similar to PurgeTemporaryFiles() except that it intentionally does
%  not release any memory (might use a mutex/semaphore) or invoke any
%  functions which are not async-safe.  The only reason to call this function
%  is something has gone wrong and the program needs to quit immediately.
%
%      void PurgeTemporaryFilesAsyncSafe(void)
%
*/
MagickExport void PurgeTemporaryFilesAsyncSafe(void)
{
  const TempfileInfo
    *member;

  member=templist;
  templist=0;
  while(member)
    {
      (void) unlink(member->filename);
      member=member->next;
    }
}
