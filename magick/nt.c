/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                 N   N  TTTTT                                %
%                                 NN  N    T                                  %
%                                 N N N    T                                  %
%                                 N  NN    T                                  %
%                                 N   N    T                                  %
%                                                                             %
%                 Windows NT Utility Routines for ImageMagick.                %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1996                                %
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
%
*/

#if defined(WIN32)
/*
  Include declarations.
*/
#include "magick.h"
#include "nt.h"

/*
  Static declarations.
*/
static HINSTANCE
  nt_instance;

static Image
  *nt_image;

/*
  External declarations.
*/
#if !defined(_VISUALC_)
extern "C" BOOL WINAPI
  DllMain(HINSTANCE instance,DWORD data_segment,LPVOID reserved);
#endif

extern char
  *SetClientName(const char *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   c l o s e d i r                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method closedir closes the named directory stream and frees the DIR
%  structure.
%
%  The format of the closedir routine is:
%
%      closedir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
Export void closedir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  FindClose(entry->hSearch);
  FreeMemory((char *) entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x i t                                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Exit calls TerminateProcess for Win95.
%
%  The format of the Exit routine is:
%
%      Exit(status)
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
Export int Exit(int status)
{
  if (IsWindows95())
    TerminateProcess(GetCurrentProcess(),(unsigned int) status);
  exit(status);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e F o r m a t C o n f l i c t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageFormatConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the ImageFormatConflict routine is:
%
%      status=ImageFormatConflict(magick)
%
%  A description of each parameter follows:
%
%    o status: Method ImageFormatConflict returns true if the image format
%              conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
Export int ImageFormatConflict(char *magick)
{
  assert(magick != (char *) NULL);
  if (strlen(magick) > 1)
    return(0);
  return((GetLogicalDrives()) & (1 << ((toupper((int) (*magick)))-'A')));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W i n d o w s 9 5                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsWindows95 returns true if the system is Windows 95.
%
%  The format of the IsWindows95 routine is:
%
%      IsWindows95()
%
%  A description of each parameter follows:
%
%    o status: an integer value representing the status of the terminating
%      process.
%
%
*/
Export int IsWindows95()
{
  OSVERSIONINFO
    version_info;

  version_info.dwOSVersionInfoSize=sizeof(version_info);
  if (GetVersionEx(&version_info) &&
      (version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS))
    return(1);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   N T E r r o r H a n d l e r                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTErrorHandler displays an error message and then terminates
%  the program.
%
%  The format of the NTErrorHandler routine is:
%
%      NTErrorHandler(message,qualifier)
%
%  A description of each parameter follows:
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
Export void NTErrorHandler(const char *message,const char *qualifier)
{
  char
    buffer[2048];

  if (message == (char *) NULL)
    Exit(0);
  (void) sprintf(buffer,"%s: %s",SetClientName((char *) NULL),message);
  if (qualifier != (char *) NULL)
    (void) sprintf(buffer,"%s (%s)",buffer,qualifier);
  if (errno)
    (void) sprintf(buffer,"%s [%s]",buffer,strerror(errno));
  (void) sprintf(buffer,"%s.\n",buffer);
  (void) MessageBox(NULL,buffer,"ImageMagick Error",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONEXCLAMATION);
  Exit(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T S y s t e m C o m m a n d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTSystemCommand executes the specified command and waits until it
%   terminates.  The returned value is the exit status of the command.
%
%  The format of the NTSystemCommand routine is:
%
%      NTSystemCommand(command)
%
%  A description of each parameter follows:
%
%    o command: This string is the command to execute.
%
%
*/
Export int NTSystemCommand(char *command)
{
  char
    local_command[2048];

  DWORD
    child_status;

  int
    status;

  PROCESS_INFORMATION
    process_info;

  STARTUPINFO
    startup_info;

  unsigned int
    background_process;

  if (command == (char *) NULL)
    return(-1);
  GetStartupInfo(&startup_info);
  startup_info.dwFlags=STARTF_USESHOWWINDOW;
  startup_info.wShowWindow=SW_SHOWMINNOACTIVE;
  (void) strcpy(local_command,command);
  background_process=command[strlen(command)-1] == '&';
  if (background_process)
    local_command[strlen(command)-1]='\0';
  if (command[strlen(command)-1] == '|')
     local_command[strlen(command)-1]='\0';
   else
     startup_info.wShowWindow=SW_SHOWDEFAULT;
  status=CreateProcess((LPCTSTR) NULL,local_command,
    (LPSECURITY_ATTRIBUTES) NULL,(LPSECURITY_ATTRIBUTES) NULL,(BOOL) FALSE,
    (DWORD) NORMAL_PRIORITY_CLASS,(LPVOID) NULL,(LPCSTR) NULL,&startup_info,
    &process_info);
  if (status == 0)
    return(-1);
  if (background_process)
    return(status == 0);
  status=WaitForSingleObject(process_info.hProcess,INFINITE);
  if (status != WAIT_OBJECT_0)
    return(status);
  status=GetExitCodeProcess(process_info.hProcess,&child_status);
  if (status == 0)
    return(-1);
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);
  return((int) child_status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T T e m p o r a r y F i l e n a m e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method NTTemporaryFilename creates a name for a temporary file.  It
%   returns zero if an error occurs.
%
%  The format of the TemporaryFilename routine is:
%
%      NTTemporaryFilename(filename)
%
%  A description of each parameter follows:
%
%    o filename: Specifies a pointer to a string to place the filename.
%
%
*/
Export int NTTemporaryFilename(char *filename)
{
  char
    path[1024];

  int
    status;

  assert(filename != (char *) NULL);
  status=GetTempPath(sizeof(path),path);
  if (status != 0)
    status=GetTempFileName(path,"magick",0,filename);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N T W a r n i n g H a n d l e r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method NTWarningHandler displays a warning message.
%
%  The format of the NTWarningHandler routine is:
%
+      NTWarningHandler(message,qualifier)
%
%  A description of each parameter follows:
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
Export void NTWarningHandler(const char *message,const char *qualifier)
{
  char
    buffer[2048];

  if (message == (char *) NULL)
    return;
  (void) sprintf(buffer,"%s: %s",SetClientName((char *) NULL),message);
  if (qualifier != (char *) NULL)
    (void) sprintf(buffer,"%s (%s)",buffer,qualifier);
  (void) sprintf(buffer,"%s.\n",buffer);
  (void) MessageBox(NULL,buffer,"ImageMagick Warning",MB_OK | MB_TASKMODAL |
    MB_SETFOREGROUND | MB_ICONINFORMATION);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   o p e n d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method opendir opens the directory named by filename and associates
%  a directory stream with it.
%
%  The format of the opendir routine is:
%
%      opendir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
Export DIR *opendir(char *path)
{
  char
    file_specification[2048];

  DIR
    *entry;

  assert(path != (char *) NULL);
  (void) strcpy(file_specification,path);
  (void) strcat(file_specification,"/*.*");
  entry=(DIR *) AllocateMemory(sizeof(DIR));
  if (entry != (DIR *) NULL)
    entry->hSearch=
      FindFirstFile(file_specification,&entry->Win32FindData);
  return(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   r e a d d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method readdir returns a pointer to a structure representing the
%  directory entry at the current position in the directory stream to
%  which entry refers.
%
%  The format of the readdir
%
%      readdir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
Export struct dirent *readdir(DIR *entry)
{
  int
    status;

  static struct dirent
    file_info;

  status=FindNextFile(entry->hSearch,&entry->Win32FindData);
  if (status == 0)
    return((struct dirent *) NULL);
  (void) strcpy(file_info.d_name,entry->Win32FindData.cFileName);
  file_info.d_namlen=strlen(file_info.d_name);
  return(&file_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   s e e k d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method seekdir sets the position of the next readdir() operation
%   on the directory stream.
%
%  The format of the seekdir routine is:
%
%      seekdir(entry,position)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%    o position: specifies the position associated with the directory
%      stream.
%
%
%
*/
Export void seekdir(DIR *entry,long position)
{
  assert(entry != (DIR *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   t e l l d i r                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Method telldir returns the current location associated  with  the
%   named directory stream.
%
%  The format of the telldir routine is:
%
%      telldir(entry)
%
%  A description of each parameter follows:
%
%    o entry: Specifies a pointer to a DIR structure.
%
%
*/
Export long telldir(DIR *entry)
{
  assert(entry != (DIR *) NULL);
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e N T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteNTImage writes an image in the NT encoded image format.
%
%  The format of the WriteNTImage routine is:
%
%      status=WriteNTImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteNTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static LRESULT CALLBACK DisplayWindow(HWND window,UINT message,WPARAM wparam,
  LPARAM lparam)
{
  switch (message)
  {
    case WM_PAINT:
    {
      BITMAPINFO
        *bitmap_info;

      char buffer
        [sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)];

      HDC
        device_context;

      int
        x,
        y;

      PAINTSTRUCT
        paint;

      register int
        i,
        j;

      register RunlengthPacket
        *p;

      register unsigned char
        *q;

      unsigned char
        *pixels;

      unsigned int
        bytes_per_line;

      device_context=BeginPaint(window,&paint);
      bitmap_info=(BITMAPINFO *) buffer;
      bitmap_info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
      bitmap_info->bmiHeader.biWidth=nt_image->columns;
      bitmap_info->bmiHeader.biHeight=nt_image->rows;
      bitmap_info->bmiHeader.biPlanes=1;
      bitmap_info->bmiHeader.biBitCount=(WORD) nt_image->matte ? 32 : 24;
      bitmap_info->bmiHeader.biCompression=BI_RGB;
      bitmap_info->bmiHeader.biSizeImage=0;
      bitmap_info->bmiHeader.biXPelsPerMeter=0;
      bitmap_info->bmiHeader.biYPelsPerMeter=0;
      bitmap_info->bmiHeader.biClrUsed=0;
      bitmap_info->bmiHeader.biClrImportant=0;
      bytes_per_line=
        ((nt_image->columns*bitmap_info->bmiHeader.biBitCount+31)/32)*4;
      if (IsPseudoClass(nt_image)|| IsGrayImage(nt_image))
        {
          bitmap_info->bmiHeader.biBitCount=(WORD) 8;
          bytes_per_line=
            ((nt_image->columns*bitmap_info->bmiHeader.biBitCount+31)/32)*4;
          if (IsMonochromeImage(nt_image))
            {
              bitmap_info->bmiHeader.biBitCount=1;
              bytes_per_line=((nt_image->columns*
                bitmap_info->bmiHeader.biBitCount+31)/32)*4;
            }
          bitmap_info->bmiHeader.biClrUsed=nt_image->colors;
          bitmap_info->bmiHeader.biClrImportant=nt_image->colors;
          for (i=0; i < nt_image->colors; i++)
          {
            bitmap_info->bmiColors[i].rgbRed=nt_image->colormap[i].red;
            bitmap_info->bmiColors[i].rgbGreen=nt_image->colormap[i].green;
            bitmap_info->bmiColors[i].rgbBlue=nt_image->colormap[i].blue;
            bitmap_info->bmiColors[i].rgbReserved=0;
          }
        }
      pixels=(unsigned char *) AllocateMemory(bytes_per_line*nt_image->rows);
      if (pixels == (unsigned char *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Memory allocation failed",
            nt_image->filename);
          return(0);
        }
      x=0;
      y=nt_image->rows-1;
      switch (bitmap_info->bmiHeader.biBitCount)
      {
        case 1:
        {
          register unsigned char
            bit,
            byte,
            polarity;

          /*
            Convert PseudoClass image to a BMP monochrome image.
          */
          p=nt_image->pixels;
          polarity=0;
          if (nt_image->colors == 2)
            polarity=Intensity(nt_image->colormap[1]) >
              Intensity(nt_image->colormap[0]);
          bit=0;
          byte=0;
          q=pixels+y*bytes_per_line;
          for (i=0; i < nt_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              byte<<=1;
              if (p->index == polarity)
                byte|=0x01;
              bit++;
              if (bit == 8)
                {
                  *q++=byte;
                  bit=0;
                  byte=0;
                }
              x++;
              if (x == nt_image->columns)
                {
                  /*
                    Advance to the next scanline.
                  */
                  if (bit != 0)
                    *q++=byte << (8-bit);
                  bit=0;
                  byte=0;
                  x=0;
                  y--;
                  q=pixels+y*bytes_per_line;
               }
            }
            p++;
          }
          break;
        }
        case 8:
        {
          /*
            Convert PseudoClass packet to BMP pixel.
          */
          p=nt_image->pixels;
          q=pixels+y*bytes_per_line;
          for (i=0; i < nt_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=p->index;
              x++;
              if (x == nt_image->columns)
                {
                  x=0;
                  y--;
                  q=pixels+y*bytes_per_line;
                }
            }
            p++;
          }
          break;
        }
        case 24:
        case 32:
        {
          /*
            Convert DirectClass packet to BMP RGB pixel.
          */
          p=nt_image->pixels;
          q=pixels+y*bytes_per_line;
          for (i=0; i < nt_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=DownScale(p->blue);
              *q++=DownScale(p->green);
              *q++=DownScale(p->red);
              if (nt_image->matte)
                *q++=Opaque-DownScale(p->index);
              x++;
              if (x == nt_image->columns)
                {
                  x=0;
                  y--;
                  q=pixels+y*bytes_per_line;
                }
            }
            p++;
          }
          break;
        }
      }
      SetDIBitsToDevice(device_context,10,10,nt_image->columns,nt_image->rows,0,
        0,0,bitmap_info->bmiHeader.biHeight,pixels,bitmap_info,DIB_RGB_COLORS);
      FreeMemory((char *) pixels);
      EndPaint(window,&paint);
      return(0);
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return(0);
    }
  }
  return(DefWindowProc(window,message,wparam,lparam));
}

Export unsigned int WriteNTImage(const ImageInfo *image_info,Image *image)
{
  HWND
    window;

  MSG
    message;

  WNDCLASS
    class_info;

  nt_image=image;
  class_info.style=CS_HREDRAW | CS_VREDRAW;
  class_info.lpfnWndProc=DisplayWindow;
  class_info.cbClsExtra=0;
  class_info.cbWndExtra=0;
  class_info.hInstance=nt_instance;
  class_info.hIcon=LoadIcon(NULL,IDI_APPLICATION);
  class_info.hCursor=LoadCursor(NULL,IDC_CROSS);
  class_info.lpszMenuName=NULL;
  class_info.lpszClassName="ImageMagick";
  RegisterClass(&class_info);
  window=CreateWindow(class_info.lpszClassName,"ImageMagick",
    WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
    CW_USEDEFAULT,NULL,NULL,nt_instance,NULL);
  ShowWindow(window,1);
  while (GetMessage(&message,NULL,0,0))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
  return(message.wParam);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D l l M a i n                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Dllmain is the entry point for the ImageMagick DLL library.
%
%
*/
BOOL WINAPI DllMain(HINSTANCE instance,DWORD data_segment,LPVOID reserved)
{
  nt_instance=instance;
  switch(data_segment)
  {
    case DLL_PROCESS_ATTACH:
    {
      return(1);
    }
    case DLL_PROCESS_DETACH:
    {
      break;
    }
    default:
    {
      return(1);
    }
  }
  return(0);
}
#endif
