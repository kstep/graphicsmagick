/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   N   N   GGGG                              %
%                            P   P  NN  N  G                                  %
%                            PPPP   N N N  G  GG                              %
%                            P      N  NN  G   G                              %
%                            P      N   N   GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

#if defined(HasPNG)
#include "png.h"
#include "zlib.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNGImage reads a Portable Network Graphics (PNG) or
%  Multiple-image Network Graphics (MNG) image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image or set of images.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu
%
%  The format of the ReadPNGImage routine is:
%
%      image=ReadPNGImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%  To do, more or less in chronological order (as of version 4.2.6,
%    23 May 1999 -- glennrp):
%
%    (At this point, decode is supposed to be in full MNG-LC compliance)
%
%    Recognize and ignore certain MNG chunks (presently, all unrecognized
%    MNG chunks are ignored, but this can be unsafe).
%
%    Implement LOOP/ENDL [done, but could do discretionary loops more
%    efficiently by linking in the duplicate frames.].
%
%    Decode JNG datastreams.
%
%    Decode and act on the MHDR simplicity profile (offer option to reject
%    files or attempt to process them anyway when the profile isn't LC or VLC).
%
%    Decode global cHRM chunk.
%
%    Upgrade to full MNG without Delta-PNG.
%
%        o  BACK [done a while ago except for background image ID]
%        o  MOVE [done 15 May 1999]
%        o  CLIP [done 15 May 1999]
%        o  DISC [done 19 May 1999]
%        o  SAVE [partial 19 May 1999 (marks objects frozen)]
%        o  SEEK [partial 19 May 1999 (discard function only)]
%        o  SHOW
%        o  PAST
%        o  BASI
%
%    Use the scene signature to discover when an identical scene is
%    being reused, and just point to the original image->pixels instead
%    of storing another set of pixels.  This is not specific to MNG
%    but could be applied generally.
%
%    Upgrade to full MNG with Delta-PNG.
%
%    Decode and act on the iCCP chunk (wait for libpng implementation).
%
%    Decode and act on the iTXt chunk (wait for libpng implementation).
%
*/

/*
  For diagnosing PerlMagick problems
#define ALWAYS_VERBOSE
 */

/*
  This is temporary until I set up malloc'ed object attributes array.
  Recompile with MAX_MNG_OBJECTS = 65536 to avoid this limit but
  waste more memory.
 */
#define MAX_MNG_OBJECTS 256

/*
  Feature under construction.  Define these to work on them.
  #define MNG_OBJECT_BUFFERS
  #define MNG_GLOBAL_COLORSPACE_SUPPORTED
*/

/*
  Define this to 96 to get proposed MNG-0.96 capabilities (Draft 0.95b),
  or to 95 to get MNG-0.95 capabilities (in MNG-0.95a, object does not
  "exist" until the embedded image is received and the object attributes for
  object 0 are discarded immediately; in MNG-0.95b proposal, they "exist"
  when the DEFI chunk is found and the object attributes for object 0
  persist until redefined.)
*/

#define MNG_LEVEL 96

/*
  If this is not defined, spec is interpreted strictly.  If it is
  defined, an attempt will be made to recover from some errors,
  including
      o global PLTE too short
  #define MNG_LOOSE
 */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Constant strings for known chunk types.  If you need to add a chunk,
  add a string holding the name here.   To make the code more
  portable, we use ASCII numbers like this, not characters.
*/

png_byte FARDATA mng_MHDR[5] = { 77,  72,  68,  82, '\0'};
png_byte FARDATA mng_BACK[5] = { 66,  65,  67,  75, '\0'};
png_byte FARDATA mng_BASI[5] = { 66,  65,  83,  73, '\0'};
png_byte FARDATA mng_CLIP[5] = { 67,  76,  73,  80, '\0'};
png_byte FARDATA mng_CLON[5] = { 67,  76,  79,  78, '\0'};
png_byte FARDATA mng_DEFI[5] = { 68,  69,  70,  73, '\0'};
png_byte FARDATA mng_DHDR[5] = { 68,  72,  68,  82, '\0'};
png_byte FARDATA mng_DISC[5] = { 68,  73,  83,  67, '\0'};
png_byte FARDATA mng_ENDL[5] = { 69,  78,  68,  76, '\0'};
png_byte FARDATA mng_FRAM[5] = { 70,  82,  65,  77, '\0'};
png_byte FARDATA mng_IDAT[5] = { 73,  68,  65,  84, '\0'};
png_byte FARDATA mng_IEND[5] = { 73,  69,  78,  68, '\0'};
png_byte FARDATA mng_IHDR[5] = { 73,  72,  68,  82, '\0'};
png_byte FARDATA mng_JHDR[5] = { 74,  72,  68,  82, '\0'};
png_byte FARDATA mng_LOOP[5] = { 76,  79,  79,  80, '\0'};
png_byte FARDATA mng_MEND[5] = { 77,  69,  78,  68, '\0'};
png_byte FARDATA mng_MOVE[5] = { 77,  79,  86,  69, '\0'};
png_byte FARDATA mng_PAST[5] = { 80,  65,  83,  84, '\0'};
png_byte FARDATA mng_PLTE[5] = { 80,  76,  84,  69, '\0'};
png_byte FARDATA mng_SAVE[5] = { 83,  65,  86,  69, '\0'};
png_byte FARDATA mng_SEEK[5] = { 83,  69,  69,  75, '\0'};
png_byte FARDATA mng_SHOW[5] = { 83,  72,  79,  87, '\0'};
png_byte FARDATA mng_TERM[5] = { 84,  69,  82,  77, '\0'};
png_byte FARDATA mng_bKGD[5] = { 98,  75,  71,  68, '\0'};
png_byte FARDATA mng_cHRM[5] = { 99,  72,  82,  77, '\0'};
png_byte FARDATA mng_gAMA[5] = {103,  65,  77,  65, '\0'};
png_byte FARDATA mng_hIST[5] = {104,  73,  83,  84, '\0'};
png_byte FARDATA mng_iTXt[5] = {105,  84,  88, 116, '\0'};
png_byte FARDATA mng_oFFs[5] = {111,  70,  70, 115, '\0'};
png_byte FARDATA mng_pHYs[5] = {112,  72,  89, 115, '\0'};
png_byte FARDATA mng_sRGB[5] = {115,  82,  71,  66, '\0'};
png_byte FARDATA mng_tEXt[5] = {116,  69,  88, 116, '\0'};
png_byte FARDATA mng_tIME[5] = {116,  73,  77,  69, '\0'};
png_byte FARDATA mng_tRNS[5] = {116,  82,  78,  83, '\0'};
png_byte FARDATA mng_zTXt[5] = {122,  84,  88, 116, '\0'};

typedef struct _MngBox
{
  long
    left,
    right,
    top,
    bottom;
} MngBox;

typedef struct _MngPair
{
  long
    a,
    b;
} MngPair;

#ifdef MNG_OBJECT_BUFFERS
typedef struct _MngBuffer
{

  unsigned long
    height,
    width;

  Image
    *image;

  png_color
    plte[256];

  int
    reference_count;

  unsigned char
    alpha_sample_depth,
    compression_method,
    color_type,
    concrete,
    filter_method,
    frozen,
    image_type,
    interlace_method,
    pixel_sample_depth,
    plte_length,
    sample_depth,
    viewable;
} MngBuffer;
#endif

typedef struct _Mng
{

#ifdef MNG_OBJECT_BUFFERS
  MngBuffer
    *ob[MAX_MNG_OBJECTS];
#endif

  long
    x_off[MAX_MNG_OBJECTS],
    y_off[MAX_MNG_OBJECTS];

  MngBox
    object_clip[MAX_MNG_OBJECTS];

  unsigned char
    /* These flags could be combined into one byte */
    exists[MAX_MNG_OBJECTS],
    frozen[MAX_MNG_OBJECTS],
    visible[MAX_MNG_OBJECTS],
    viewable[MAX_MNG_OBJECTS];

  long
    loop_count[256],
    loop_iteration[256],
    loop_jump[256];

  unsigned char
    loop_active[256];

  png_color
    global_plte[256];

  png_byte
    global_trns[256];

  float
    global_gamma;

  ChromaticityInfo
    global_chromaticity;

  RenderingIntent
    global_srgb_intent;

  unsigned int
    verbose;
} Mng;

static void MngDiscardObject(Mng *m, int i)
{
  if(i && i<MAX_MNG_OBJECTS && m != (Mng *)NULL && m->exists[i] && !m->frozen[i])
    {
#ifdef MNG_OBJECT_BUFFERS
    if(m->ob[i] != (MngBuffer *)NULL)
      {
        if(m->ob[i]->reference_count > 0)
           m->ob[i]->reference_count--;
        if(m->ob[i]->reference_count == 0)
          {
            if(m->ob[i]->image != (Image *)NULL)
              DestroyImage(m->ob[i]->image);
            if(m->verbose)
              printf("Discarded object buffer[%d]\n",i);
            FreeMemory((char *)m->ob[i]);
          }
      }
    m->ob[i]=(MngBuffer *)NULL;
#endif
    m->exists[i]=False;
    m->visible[i]=True;
    m->viewable[i]=False;
    m->frozen[i]=False;
    m->x_off[i]=0;
    m->y_off[i]=0;
    m->object_clip[i].left=0;
    m->object_clip[i].right=0x3ffffff;
    m->object_clip[i].top=0;
    m->object_clip[i].bottom=0x3ffffff;
    if(m->verbose)
      printf("Discarded object %d\n",i);
  }
}

static void MngFreeStruct(Mng *m, int *have_mng_structure)
{
  if(*have_mng_structure && m != (Mng *)NULL)
    {
      int
        i;
      if(m->verbose)
        printf("Free mng structure.\n");
      for(i=1; i<MAX_MNG_OBJECTS; i++)
        MngDiscardObject(m,i);
      FreeMemory((char *)m);
      *have_mng_structure=False;
    }
}

MngBox mng_minimum_box(MngBox box1, MngBox box2)
{
  MngBox
    box;

  box = box1;
  if(box.left   < box2.left)   box.left   = box2.left  ;
  if(box.top    < box2.top )   box.top    = box2.top   ;
  if(box.right  > box2.right)  box.right  = box2.right ;
  if(box.bottom > box2.bottom) box.bottom = box2.bottom;
  return box;
}

MngBox mng_read_box(MngBox previous_box, char delta_type, unsigned char *p)
{
   /* read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk */
   MngBox
      box;
   box.left   = (long)((p[0]  << 24)| (p[1]  << 16)| (p[2]  << 8) |p[3]);
   box.right  = (long)((p[4]  << 24)| (p[5]  << 16)| (p[6]  << 8) |p[7]);
   box.top    = (long)((p[8]  << 24)| (p[9]  << 16)| (p[10] << 8) |p[11]);
   box.bottom = (long)((p[12] << 24)| (p[13] << 16)| (p[14] << 8) |p[15]);
   if(delta_type != 0)
     {
       box.left   += previous_box.left;
       box.right  += previous_box.right;
       box.top    += previous_box.top;
       box.bottom += previous_box.bottom;
     }
   return box;
}

MngPair mng_read_pair(MngPair previous_pair, char delta_type, unsigned char *p)
{
   /* read two longs from MHDR, CLON, MOVE or PAST chunk */
   MngPair
      pair;
   pair.a = (long)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
   pair.b = (long)((p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);

   if(delta_type != 0)
     {
       pair.a += previous_pair.a;
       pair.b += previous_pair.b;
     }
   return pair;
}

long mng_get_long(unsigned char *p)
{
   return (long)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static void MNGCoalesce(Image *image)
{
  long
    delay;

  register Image
    *p;

  if (image->previous == (Image *) NULL)
    return;
  p=image->previous;
  assert(p->next != (Image *) NULL);
  if (p->delay != 0)
    return;
  delay=(long)image->delay;
  CoalesceImages(p);
  p->file=(FILE *) NULL;
  p->orphan=False;
  DestroyImage(p);
  image->delay=delay;
}

static void PNGErrorHandler(png_struct *ping,png_const_charp message)
{
  MagickWarning(DelegateWarning,message,(char *) NULL);
  longjmp(ping->jmpbuf,1);
}

static void ReadTextChunk(png_info *ping_info,unsigned int i,char **value)
{
  unsigned int
    length;

  length=ping_info->text[i].text_length;
  if (*value != (char *) NULL)
    *value=(char *) ReallocateMemory((char *) *value,strlen(*value)+length+1);
  else
    {
      *value=(char *) AllocateMemory(length+1);
      if (*value != (char *) NULL)
        **value='\0';
    }
  if (*value == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"a. Memory allocation failed",
        (char *) NULL);
      return;
    }
  (void) strncat(*value,ping_info->text[i].text,length);
  (*value)[length]='\0';
}

static void PNGWarningHandler(png_struct *ping,png_const_charp message)
{
  MagickWarning(DelegateWarning,message,(char *) NULL);
  if(ping == (png_struct *)NULL)
     return;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

Export Image *ReadPNGImage(const ImageInfo *image_info)
{
  ColorPacket
    transparent_color;

  Image
    *image;

  unsigned long
     mng_width=0,
     mng_height=0;

  char
    page_geometry[MaxTextExtent];

  int
    y;

  register int
    i,
    x;

  register long
    packets;

  register unsigned char
    *p;

  register RunlengthPacket
    *q;

  png_info
    *end_info,
    *ping_info;

  png_struct
    *ping;

  Mng
    *m;

  unsigned char
    *png_pixels,
    **scanlines;

  unsigned int
    status;

  unsigned long
    length;

  long
    image_count=0;

  unsigned short
    index,
    value;

  unsigned long
    default_frame_delay, final_delay, frame_delay,
    frame_timeout, default_frame_timeout,
    image_width, image_height,
    subframe_width, subframe_height,
    ticks_per_second;

  int
    have_global_chromaticity,
    have_global_gamma,
    have_global_srgb,
    global_plte_length,
    global_trns_length;

  ColorPacket
    mng_background_color;

  unsigned int
    framing_mode=1,
    mandatory_back=0,
    mng_background_object=0,
    mng_level=MNG_LEVEL,
    mng_type=0,   /* 0: PNG; 1: MNG; 2: MNG-LC; 3: MNG-VLC */
    simplicity=0;

  int
    first_mng_object,
    image_found,
    have_mng_structure,
    object_id,
    term_chunk_found,
    skip_to_iend;

  short
    loop_level,
    loops_active,
    skipping_loop;

  MngBox
    clip,
    default_fb,
    fb,
    frame,
    image_box,
    previous_fb;
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to open file",image);


  first_mng_object=0;
  skipping_loop= -1;
  have_mng_structure=False;
#ifndef ALWAYS_VERBOSE
  if(image_info->verbose)
#endif
    printf("Reading %s\n",image_info->filename);
  if (Latin1Compare(image_info->magick,"MNG") == 0)
    {
      char
        magic_number[MaxTextExtent];

      /*
        Verify MNG signature.
      */
      (void) ReadData(magic_number,1,8,image->file);
      if (strncmp(magic_number,"\212MNG\r\n\032\n",8) != 0)
        {
          ReaderExit(CorruptImageWarning,"Not a MNG image file",image);
        }
#ifndef ALWAYS_VERBOSE
      if(image_info->verbose)
#endif
          printf("Reading MNG file.\n");
      first_mng_object=1;
      /*
        Allocate a Mng structure.
      */
      m=(Mng *)AllocateMemory(sizeof(Mng));
      if (m == (Mng *) NULL)
        ReaderExit(ResourceLimitWarning,"b. Memory allocation failed", image);
      have_mng_structure=True;
      m->verbose = image_info->verbose;
#ifdef ALWAYS_VERBOSE
      m->verbose = True;
#endif
      for (i=0; i<MAX_MNG_OBJECTS; i++)
        {
          m->exists[i]=False;
          m->visible[i]=True;
          m->viewable[i]=False;
          m->frozen[i]=False;
          m->x_off[i]=0;
          m->y_off[i]=0;
          m->object_clip[i].left=0;
          m->object_clip[i].right=0x3ffffff;
          m->object_clip[i].top=0;
          m->object_clip[i].bottom=0x3ffffff;
#ifdef MNG_OBJECT_BUFFERS
          m->ob[i]=(MngBuffer *)NULL;
#endif
    }
      if(mng_level > 95)
         m->exists[0]=True;
      for (i=0; i<256; i++)
        m->loop_active[i]=0;
    }

  mng_type=0;
  default_frame_delay=0;
  frame_delay=0;
  final_delay=100;
  ticks_per_second=100;
  object_id=0;
  skip_to_iend = False;
  term_chunk_found = False;
  image_found = 0;
  framing_mode=1;
  mandatory_back=0;
  have_global_gamma=0;
  have_global_chromaticity=0;
  have_global_srgb=0;
  global_plte_length=0;
  global_trns_length=0;
  mng_background_color=image->border_color;
  do
  {
    if (Latin1Compare(image_info->magick,"MNG") == 0)
      {
        char
          type[MaxTextExtent];

        unsigned char
          *chunk;

        /*
          Read a new chunk.
        */
        type[0]='\0';
        strcat(type,"errr");
        length=MSBFirstReadLong(image->file);
        status=ReadData(type,1,4,image->file);
        if(length > 0x3ffffff)
           status=False;
        if(m->verbose)
          {
            char
               chunk_name[6];

            if(skip_to_iend)
              {
                if (type[0] == 0x4a)   /* 'J' */
                  printf("Skipping JNG ");
                else
                  printf("Skipping PNG ");
              }
            else
              {
               if (!png_memcmp(type, mng_IHDR, 4))
                 printf("PNG ");
               else
                 {
                   if (type[0] == 0x4a)   /* 'J' */
                     printf("Skipping JNG ");
                   else
                     printf("MNG ");
                 }
              }
            for (i=0; i<4; i++)
              {
                const char
                   upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                   lower[] = "abcdefghijklmnopqrstuvwxyz";

                if (type[i] >= 0x41 && type[i] <= 0x5A)
                    chunk_name[i] = upper[type[i]-0x41];
                else if (type[i] >= 0x61 && type[i] <= 0x7A)
                    chunk_name[i] = lower[type[i]-0x61];
                else
                  {
                    printf("Invalid byte (0x%x) in chunk name\n",type[i]);
                    chunk_name[i]=0x3f;   /* '?' */
                  }
              }
            chunk_name[4]=0;
            printf("chunk %s, length=%lu\n",chunk_name,length);
          }
        if (status == False)
          {
            MagickWarning(CorruptImageWarning,"Corrupt MNG image",
              image->filename);
            break;
          }

        if (!png_memcmp(type, mng_JHDR, 4))
          {
            skip_to_iend = True;
            MagickWarning(DelegateWarning,"JNG is not implemented yet",
              image->filename);
          }

        if (!png_memcmp(type, mng_DHDR, 4))
          {
            skip_to_iend = True;
            MagickWarning(DelegateWarning,"Delta-PNG is not implemented yet",
              image->filename);
          }

        if (!png_memcmp(type, mng_BASI, 4))
          {
            skip_to_iend = True;
            MagickWarning(DelegateWarning,"BASI is not implemented yet",
              image->filename);
          }

        if(length != 0)
          {
            chunk=(unsigned char *) AllocateMemory(length*sizeof(unsigned char));
            if (chunk == (unsigned char *) NULL)
              {
                if(m->verbose)
                   printf("chunk length = %lu\n",length);
                ReaderExit(ResourceLimitWarning,
                  "Unable to allocate memory for chunk data", image);
              }

            for (i=0; i < (int) length; i++)
              chunk[i]=fgetc(image->file);
            p=chunk;
          }
        (void) MSBFirstReadLong(image->file);  /* read crc word */

        if (!png_memcmp(type, mng_MEND, 4))
          {
            break;
          }

        if(skip_to_iend)
          {
            if (!png_memcmp(type, mng_IEND, 4))
               skip_to_iend = False;
            continue;
          }

        if (!png_memcmp(type, mng_MHDR, 4))
          {
            MngPair
               pair;

            pair.a = pair.b = 0;
            pair=mng_read_pair(pair,0,p);
            mng_width=(unsigned long)pair.a;
            mng_height=(unsigned long)pair.b;
            p+=8;
            ticks_per_second=(unsigned long)mng_get_long(p);
            if(ticks_per_second == 0)default_frame_delay=0;
            else default_frame_delay=100/ticks_per_second;
            frame_delay=default_frame_delay;
            if(length > 16)
              {
            p+=16;
            simplicity=(unsigned long) mng_get_long(p);
              }
            else
              simplicity = 0;
            mng_type=1;    /* Full MNG */
            if(simplicity != 0 && (simplicity | 11) == 11)
               mng_type=2; /* LC */
            if(simplicity != 0 && (simplicity | 9) == 9)
               mng_type=3; /* VLC */
            if (image->pixels != (RunlengthPacket *) NULL)
              {
                /*
                  Allocate next image structure.
                */
                AllocateNextImage(image_info,image);
                if (image->next == (Image *) NULL)
                  {
                    DestroyImages(image);
                    MngFreeStruct(m,&have_mng_structure);
                    return((Image *) NULL);
                  }
                image=image->next;
              }

            if(mng_width > 65535 || mng_height > 65535)
              MagickWarning(DelegateWarning,
                "Image dimensions are too large.", (char *) NULL);

            FormatString(page_geometry,"%lux%lu%+0+0",mng_width,
               mng_height);

            frame.left = 0;
            frame.right = (long)mng_width;
            frame.top = 0;
            frame.bottom = (long)mng_height;
            clip = default_fb = previous_fb = frame;
            for (i=0; i<MAX_MNG_OBJECTS; i++)
                m->object_clip[i] = frame;

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_TERM, 4))
          {
            int
               repeat=0;
            png_uint_32
               iterations;

            if (length>0)
                  repeat=p[0];
            if (repeat == 3)
              {
                final_delay=(int)mng_get_long(&p[2]);
                iterations=(int)mng_get_long(&p[6]);
                if(iterations == 0x7ffffffL)
                   iterations = 0;
                if (image_info->iterations == (char *) NULL)
                    image->iterations=iterations;
                term_chunk_found = True;
                if (m->verbose)
                   printf("TERM: final_delay=%d, iterations=%d\n",
                      final_delay, iterations);
              }
            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_DEFI, 4))
          {
            if(mng_type == 3)
              MagickWarning(DelegateWarning,
                 "DEFI chunk found in MNG-VLC datastream",(char *) NULL);
            object_id = (p[0]<<8) | p[1];
            if(mng_type == 2 && object_id != 0)
              MagickWarning(DelegateWarning,
                 "Nonzero object_id in MNG-LC datastream",(char *) NULL);
            if(object_id > MAX_MNG_OBJECTS)
              {
              /* Instead of issuing a warning we should allocate a larger
                 Mng structure and continue. */
              MagickWarning(DelegateWarning,
                 "object_id too large",(char *) NULL);
              object_id = MAX_MNG_OBJECTS;
              }

            if(m->exists[object_id])
              {
              if(m->frozen[object_id])
                  {
                    MagickWarning(DelegateWarning,
                     "DEFI cannot redefine a frozen MNG object",(char *) NULL);
                    FreeMemory((char *) chunk);
                    continue;
              }
              }
            if (mng_level > 95)
            m->exists[object_id] = True;
            else
               m->exists[object_id] = False;  /* It doesn't exist until IHDR */

            if (length>2)
                  m->visible[object_id]=!p[2];
            else
               if (mng_level < 96)
                  m->visible[object_id]=True;
            /*
              Extract object offset info.
            */
            if (length>11)
              {
                MngPair
                  pair;

                pair.a = pair.b = 0;
                pair = mng_read_pair(pair, 0, &p[4]);
                m->x_off[object_id]=pair.a;
                m->y_off[object_id]=pair.b;
              }
            else
               if (mng_level < 96)
              {
                m->x_off[object_id]=0;
                m->y_off[object_id]=0;
              }

            /*
              Extract object clipping info.
            */
            if (length>27)
                m->object_clip[object_id]=mng_read_box(frame,0,&p[12]);
            else
               if (mng_level < 96)
                m->object_clip[object_id]=frame;

            if(m->verbose)
              {
                printf("  DEFI object_id=%d geometry=%ldx%ld%+ld%+ld\n",
                  object_id, mng_width, mng_height, m->x_off[object_id],
                  m->y_off[object_id]);
                printf("  clipping boundaries: L=%ld R=%ld T=%ld B=%ld\n",
                  m->object_clip[object_id].left,m->object_clip[object_id].right,
                  m->object_clip[object_id].top,m->object_clip[object_id].bottom);
              }

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_BACK, 4))
          {
            if (length>5)
              {
                mng_background_color.red   = 
                  (unsigned short)XDownScale((p[0]<<8) | p[1]);
                mng_background_color.green = 
                  (unsigned short)XDownScale((p[2]<<8) | p[3]);
                mng_background_color.blue  = 
                  (unsigned short)XDownScale((p[4]<<8) | p[5]);
              }

            if (length>6)
                mandatory_back = p[6];
            else
                mandatory_back = 0;

            if (length > 8)
               mng_background_object = p[7]<<8 | p[8];

            if (m->verbose)
              {
               printf(" Background color= %d.3 %d.3 %d.3, background object=%d",
                 mng_background_color.red, mng_background_color.green,
                 mng_background_color.blue, mng_background_object);
               printf(" (mandatory =%d)\n", mandatory_back);
              }

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_PLTE, 4))
          {
            int i;

            /* read global PLTE */

            if(length < 769)
              for (i=0; i<(int)length/3; i+=3)
                {
                  m->global_plte[i].red=chunk[3*i+4];
                  m->global_plte[i].green=chunk[3*i+5];
                  m->global_plte[i].blue=chunk[3*i+6];
                }
            global_plte_length = (int)length/3;
#ifdef MNG_LOOSE
            for (   ; i<256; i++)
              {
                m->global_plte[i].red=i;
                m->global_plte[i].green=i;
                m->global_plte[i].blue=i;
              }
            if(length > 0)
              global_plte_length = 256;
#endif
            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_tRNS, 4))
          {
            int i;

            /* read global tRNS */

            if(length < 257)
              for (i=0; i<length; i++)
                m->global_trns[i]=chunk[i+4];

#ifdef MNG_LOOSE
            for (   ; i<256; i++)
              m->global_trns[i]=255;
#endif
            global_trns_length=(int)length;

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_gAMA, 4))
          {
            int igamma = (int)mng_get_long(chunk);

            m->global_gamma = ((float)igamma+0.5)*0.00001;
            have_global_gamma = True;

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_cHRM, 4))
          {

            /* read global cHRM */

            /* TO DO */

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_sRGB, 4))
          {

            /* read global sRGB */

            m->global_srgb_intent = (RenderingIntent)p[0];
            have_global_srgb = 1;

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_FRAM, 4))
          {
            if(mng_type == 3)
              MagickWarning(DelegateWarning,
                 "FRAM chunk found in MNG-VLC datastream",(char *) NULL);

            if(framing_mode == 2 || framing_mode == 4)
               image->delay = frame_delay;

            frame_delay = default_frame_delay;
            frame_timeout = default_frame_timeout;

            fb = default_fb;

            if (length > 0)
               framing_mode=p[0];
            if (length > 6)
              {
                /*
                  Note the delay and frame clipping boundaries.
                */
                p++; /* framing mode */
                while (*p && ((p-chunk) < length))
                  p++;  /* frame name */
                p++;
                if ((p-chunk) < (length-4))
                  {
                    int change_delay    = *p++;
                    int change_timeout  = *p++;
                    int change_clipping = *p++;
                    p++; /* change_sync */

                    if (change_delay)
                      {
                         frame_delay=(unsigned int)(100*(mng_get_long(p))
                            /ticks_per_second);
                         if(change_delay == 2)
                           default_frame_delay = frame_delay;
                         p+=4;
                      }
                    if (change_timeout)
                      {
                         frame_timeout=(unsigned int)(100*(mng_get_long(p))
                            /ticks_per_second);
                         if(change_delay == 2)
                           default_frame_timeout = frame_timeout;
                         p+=4;
                      }
                    if (change_clipping)
                      {
                         fb = mng_read_box(previous_fb,p[0],&p[1]);
                         p+=17;
                         previous_fb   = fb;
                      }
                    if(change_clipping == 2)
                         default_fb   = fb;
                      }
                  }

          clip = fb;
          clip = mng_minimum_box(fb, frame);

          subframe_width = (unsigned int)(clip.right - clip.left);
          subframe_height = (unsigned int)(clip.bottom - clip.top);

          /* insert a background layer behind the frame if
             framing_mode is 4 */

          if(image_info->insert_backdrops && framing_mode == 4 &&
            subframe_width > 0 && subframe_height > 0)
            {
              char
                background_page_geometry[MaxTextExtent];

              /*
                Allocate next image structure.
              */
              AllocateNextImage(image_info,image);
              if (image->next == (Image *) NULL)
                {
                  DestroyImages(image);
                  MngFreeStruct(m,&have_mng_structure);
                  return((Image *) NULL);
                }
              image=image->next;
              if(term_chunk_found)
                {
                  image->restart_animation_here = True;
                  term_chunk_found=False;
                  if(m->verbose)
                    printf("Setting restart_animation_here\n");
                }

              image->columns=subframe_width;
              image->rows=subframe_height;
              FormatString(background_page_geometry,"%lux%lu%+ld%+ld",
                subframe_width, subframe_height, clip.left, clip.top);
              if (image->page != (char *) NULL)
                FreeMemory((char *) image->page);
              image->page=PostscriptGeometry(background_page_geometry);
              image->packets=image->columns*image->rows;
              image->pixels=(RunlengthPacket *)
                  AllocateMemory(image->packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  MagickWarning(ResourceLimitWarning,
                    "d. Memory allocation failed", (char *) NULL);
                  MngFreeStruct(m,&have_mng_structure);
                  return((Image *) NULL);
                }
              image->background_color = mng_background_color;
              image->matte = False;
              SetImage(image);
              image->delay = 0;
              if(image_info->coalesce_frames)
                MNGCoalesce(image);
            }

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_CLIP, 4))
          {
            /* read CLIP */
            unsigned int
               first_object,
               last_object;

            first_object = (p[0]<<8) | p[1];
            last_object  = (p[2]<<8) | p[3];

            if(m->verbose)
               printf("CLIP: first_object=%u, last_object=%u\n",
                  first_object, last_object);

            for(i=first_object; i<=last_object; i++)
              {
                if(m->exists[i] && !m->frozen[i])
                  {
                    MngBox
                      box;

                    box = m->object_clip[i];
                    m->object_clip[i]=mng_read_box(box,p[4],&p[5]);
                    if(m->verbose)
                      printf("  new clip[%d]: L=%ld, R=%ld, T=%ld, B=%ld\n",
                         i,m->object_clip[i].left,m->object_clip[i].right,
                         m->object_clip[i].top,m->object_clip[i].bottom);
                  }
              }
            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_SAVE, 4))
          {
            register int
              i;
            for (i=1; i<MAX_MNG_OBJECTS; i++)
              if(m->exists[i])
                {
                 m->frozen[i] = True;
#ifdef MNG_OBJECT_BUFFERS
                 if(m->ob[i] != (MngBuffer *)NULL)
                    m->ob[i]->frozen = True;
#endif
                }

            if(length > 0)
              FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_DISC, 4) || !png_memcmp(type, mng_SEEK, 4))
          {
            /* read DISC or SEEK */
            int
              i;

            if(length == 0 || !png_memcmp(type, mng_SEEK, 4))
              {
                for (i=1; i<MAX_MNG_OBJECTS; i++)
                  MngDiscardObject(m, i);
              }
            else
              {
                register int
                  j;

                for (j=0; j<length; j+=2)
                  {
                    i=p[j]<<8 | p[j+1];
                    MngDiscardObject(m, i);
                  }
              }

            if(length > 0)
              FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_MOVE, 4))
          {
            /* read MOVE */
            long
               first_object,
               last_object;

            first_object = (p[0]<<8) | p[1];
            last_object  = (p[2]<<8) | p[3];

            for(i=first_object; i<=last_object; i++)
              {
                if(m->exists[i] && !m->frozen[i])
                  {
                    MngPair
                      new_pair,
                      old_pair;
                    old_pair.a = m->x_off[i];
                    old_pair.b = m->y_off[i];
                    new_pair=mng_read_pair(old_pair,p[4],&p[5]);
                    m->x_off[i]=new_pair.a;
                    m->y_off[i]=new_pair.b;
                    if(m->verbose)
                      printf("  MOVE to x[%d]=%ld, y[%d]=%ld\n",
                         i,m->x_off[i],i,m->y_off[i]);
                  }
              }

            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_LOOP, 4))
          {
            long loop_iters=1;
            loop_level = chunk[0];
            loops_active++;
            m->loop_active[loop_level]=1;  /* mark loop active */
            /* record starting point */
            loop_iters = mng_get_long(&chunk[1]);
            if(loop_iters == 0)
               skipping_loop=loop_level;
            else
              {
                m->loop_jump[loop_level]=ftell(image->file);
                m->loop_count[loop_level]=loop_iters;
              }
            if(m->verbose)
              {
                printf("loop offset=%ld, ",
                m->loop_jump[loop_level]);
                printf("loop iterations=%ld\n",
                m->loop_count[loop_level]);
              }
            m->loop_iteration[loop_level]=0;
            FreeMemory((char *) chunk);
            continue;
          }

        if (!png_memcmp(type, mng_ENDL, 4))
          {
            loop_level = chunk[0];
            if(skipping_loop > 0)
              {
                if(skipping_loop == loop_level)
                  {
                    /* found end of zero-iteration loop */
                    skipping_loop = -1;
                    loops_active--;
                    m->loop_active[loop_level]=0;
                  }
              }   /* end of skipping_loop */
            else
              {
                if(m->loop_active[loop_level] == 1)
                  {
                    if(m->verbose)
                        printf("Processing ENDL chunk.\n");
                    m->loop_count[loop_level]--;
                    m->loop_iteration[loop_level]++;
                    if(m->loop_count[loop_level] == 0)
                      {
                        unsigned int last_level;
                        /* finished loop */
                        loops_active--;
                        m->loop_active[loop_level]=0;
                        /* restore old loop_level */
                        last_level = -1;
                        for(i=0; i<loop_level; i++)
                           if(m->loop_active[i] == 1) last_level=i;
                        loop_level=last_level;
                      }
                    else
                      {
                        /* jump back for another iteration of loop */
                        if(m->verbose != 0)
                            printf("LOOP back to %ld for another iteration\n",
                                m->loop_jump[loop_level]);
                        fseek(image->file, m->loop_jump[loop_level], SEEK_SET);
                      }
                  }
                else
                  {
                     /* no matching LOOP chunk */
                  }
               }   /* end of not skipping_loop */
        FreeMemory((char *) chunk);
             continue;
          }   /* end of ENDL chunk processing */

        if (!png_memcmp(type, mng_CLON, 4))
          {
            MagickWarning(DelegateWarning,"CLON is not implemented yet",
              image->filename);
          }

        if (!png_memcmp(type, mng_PAST, 4))
          {
            MagickWarning(DelegateWarning,"PAST is not implemented yet",
              image->filename);
          }

        if (!png_memcmp(type, mng_SHOW, 4))
          {
            MagickWarning(DelegateWarning,"SHOW is not implemented yet",
              image->filename);
          }

        if (png_memcmp(type, mng_IHDR, 4))
          {
            if(length > 0)
              FreeMemory((char *) chunk);
            continue;
          }

        m->exists[object_id]=True;
        m->viewable[object_id]=True;

        if(!m->visible[object_id])
          {
            /* DEFI "noshow" flag only persists for one embedded object
               when object_id is zero, in MNG-0.95.  For other object_id's,
               it remains in effect until reset by another DEFI chunk. */

            if (object_id == 0 && mng_level < 96)
              m->visible[object_id]=True;

            if(!image_info->decode_all_MNG_objects)
              {
                if(m->verbose)
                  printf("skipping invisible object %d...\n",object_id);
            skip_to_iend = True;
                FreeMemory((char *) chunk);
            continue;
          }
          }

            if(m->verbose)
              printf("  object_id=%d geometry=%ldx%ld%+ld%+ld\n",
                object_id, mng_width, mng_height, m->x_off[object_id],
                m->y_off[object_id]);

        /* insert a background layer behind the upcoming image if
           framing_mode is 3 and transparency might be present */

        image_width=(unsigned int) mng_get_long(p);
        image_height=(unsigned int) mng_get_long(&p[4]);

        FreeMemory((char *) chunk);

        if(image_info->insert_backdrops && framing_mode == 3 &&
             (first_mng_object == 0 || ( clip.left == 0 && clip.top == 0
              && image_width == mng_width && image_height == mng_height)))
          {
            if(simplicity == 0 || (simplicity & 8) == 8)
              {
                char
                  background_page_geometry[MaxTextExtent];

                int
                  delay;

                /*
                  Allocate next image structure.
                */
                AllocateNextImage(image_info,image);
                if (image->next == (Image *) NULL)
                  {
                    DestroyImages(image);
                    MngFreeStruct(m,&have_mng_structure);
                    return((Image *) NULL);
                  }
                image=image->next;
                if(term_chunk_found)
                  {
                    image->restart_animation_here = True;
                    term_chunk_found=False;
                  if(m->verbose)
                    printf("Setting restart_animation_here\n");
                  }

                delay = image->delay;
                image->delay=0;
                image->columns=subframe_width;
                image->rows=subframe_height;
                FormatString(background_page_geometry,"%lux%lu%+ld%+ld",
                  subframe_width, subframe_height, clip.left, clip.top);
                if (image->page != (char *) NULL)
                  FreeMemory((char *) image->page);
                image->page=PostscriptGeometry(background_page_geometry);
                image->packets=image->columns*image->rows;

                image->pixels=(RunlengthPacket *)
                  AllocateMemory(image->packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    MagickWarning(ResourceLimitWarning,
                      "e. Memory allocation failed", (char *) NULL);
                    MngFreeStruct(m,&have_mng_structure);
                    return((Image *) NULL);
                  }
                image->background_color = mng_background_color;
                image->matte = False;
                SetImage(image);
                image->delay=delay;
              }
          }

        /* insert a background layer behind the entire animation if it
           is not full screen or transparency might be present */

        if(image_info->insert_backdrops && first_mng_object)
          {
            if(simplicity == 0 || (simplicity & 8) == 8 ||
               (clip.left != 0 || clip.top != 0 ||
                image_width != mng_width || image_height != mng_height))
              {
                int
                  delay;

                char
                   background_page_geometry[MaxTextExtent];

                /* make a background rectangle */
                delay = image->delay;
                image->delay=0;
                image->columns=mng_width;
                image->rows=mng_height;
                FormatString(background_page_geometry,"%lux%lu%+ld%+ld",
                  mng_width, mng_height, 0, 0);

                if (image->page != (char *) NULL)
                  FreeMemory((char *) image->page);
                image->page=PostscriptGeometry(background_page_geometry);
                image->packets=image->columns*image->rows;
                image->pixels=(RunlengthPacket *)
                  AllocateMemory(image->packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    MagickWarning(ResourceLimitWarning,
                     "f. Memory allocation failed", (char *) NULL);
                    MngFreeStruct(m,&have_mng_structure);
                    return((Image *) NULL);
                  }
                image->background_color = mng_background_color;
                image->matte = False;
                image->delay=delay;
                SetImage(image);
            }
            first_mng_object=0;
            if (mng_level < 96)
              m->exists[0]=False;
          }

        /* Read the PNG image */

        /* Seek back to the beginning of the IHDR chunk's length field */
        (void) fseek(image->file,-((int) length+12),SEEK_CUR);
        if (image->pixels != (RunlengthPacket *) NULL)
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImages(image);
                MngFreeStruct(m,&have_mng_structure);
                return((Image *) NULL);
              }
            image=image->next;
            ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
              (unsigned int) image->filesize);
          }
              if(term_chunk_found)
                {
                  image->restart_animation_here = True;
                  term_chunk_found=False;
            if(m->verbose)
              printf("Setting restart_animation_here\n");
          }

          if(framing_mode == 1 || framing_mode == 3)
            {
              image->delay=frame_delay;
              frame_delay = default_frame_delay;
            }
          else
              image->delay=0;

        FormatString(page_geometry,"%lux%lu%+ld%+ld",mng_width,
          mng_height,m->x_off[object_id], m->y_off[object_id]);
            if (image->page != (char *) NULL)
              FreeMemory((char *) image->page);
          image->page=PostscriptGeometry(page_geometry);

      }


    /*
      Allocate the PNG structures
    */
    ping=png_create_read_struct(PNG_LIBPNG_VER_STRING,(void *) NULL,
      PNGErrorHandler,PNGWarningHandler);
    if (ping == (png_struct *) NULL)
      ReaderExit(ResourceLimitWarning,"g. Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,(png_info **) NULL,(png_info **) NULL);
        ReaderExit(ResourceLimitWarning,"h. Memory allocation failed",image);
      }
    end_info=png_create_info_struct(ping);
    if (end_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,&ping_info,(png_info **) NULL);
        ReaderExit(ResourceLimitWarning,"i. Memory allocation failed",image);
      }
    image->pixels=(RunlengthPacket *) NULL;
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
      {
        /*
          PNG image is corrupt.
        */
        png_destroy_read_struct(&ping,&ping_info,&end_info);
        if (scanlines != (unsigned char **) NULL)
          FreeMemory((char *) scanlines);
        if (png_pixels != (unsigned char *) NULL)
          FreeMemory((char *) png_pixels);
        CloseImage(image);
        if ((image->columns == 0) || (image->rows == 0))
          {
            DestroyImage(image);
            MngFreeStruct(m,&have_mng_structure);
            return((Image *) NULL);
          }
        MngFreeStruct(m,&have_mng_structure);
        return(image);
      }
    /*
      Prepare PNG for reading.
    */
    image_found++;
    png_init_io(ping,image->file);
    if (Latin1Compare(image_info->magick,"MNG") == 0)
      png_set_sig_bytes(ping,8);
    png_read_info(ping,ping_info);
    image->depth=ping_info->bit_depth;
    if (ping_info->bit_depth < 8)
      {
        if ((ping_info->color_type != PNG_COLOR_TYPE_PALETTE) &&
            (ping_info->color_type != PNG_COLOR_TYPE_GRAY))
          png_set_packing(ping);
        image->depth=8;
      }
#if defined(PNG_READ_sRGB_SUPPORTED)
    if(have_global_srgb)
      image->rendering_intent=(RenderingIntent) (m->global_srgb_intent+1);
    if (ping_info->valid & PNG_INFO_sRGB)
      image->rendering_intent=(RenderingIntent) (ping_info->srgb_intent+1);
#endif
    if(have_global_gamma)
      image->gamma=m->global_gamma;
    if (ping_info->valid & PNG_INFO_gAMA)
      image->gamma=ping_info->gamma;
    if(have_global_chromaticity)
        image->chromaticity=m->global_chromaticity;
    if (ping_info->valid & PNG_INFO_cHRM)
      {
        image->chromaticity.red_primary.x=ping_info->x_red;
        image->chromaticity.red_primary.y=ping_info->y_red;
        image->chromaticity.green_primary.x=ping_info->x_green;
        image->chromaticity.green_primary.y=ping_info->y_green;
        image->chromaticity.blue_primary.x=ping_info->x_blue;
        image->chromaticity.blue_primary.y=ping_info->y_blue;
        image->chromaticity.white_point.x=ping_info->x_white;
        image->chromaticity.white_point.y=ping_info->y_white;
      }
    if (ping_info->valid & PNG_INFO_pHYs)
      {
        /*
          Set image resolution.
        */
        image->x_resolution=(float)ping_info->x_pixels_per_unit;
        image->y_resolution=(float)ping_info->y_pixels_per_unit;
        if (ping_info->phys_unit_type == PNG_RESOLUTION_METER)
          {
            image->units=PixelsPerCentimeterResolution;
            image->x_resolution=(float)ping_info->x_pixels_per_unit/100.0;
            image->y_resolution=(float)ping_info->y_pixels_per_unit/100.0;
          }
      }
    if (ping_info->valid & PNG_INFO_bKGD)
      {
        /*
          Set image background color.
        */
        image->background_color.red=ping_info->background.red;
        image->background_color.green=ping_info->background.green;
        image->background_color.blue=ping_info->background.blue;
        if (ping_info->bit_depth > QuantumDepth)
          {
            image->background_color.red=
              (unsigned short)XDownScale(ping_info->background.red);
            image->background_color.green=
              (unsigned short)XDownScale(ping_info->background.green);
            image->background_color.blue=
              (unsigned short)XDownScale(ping_info->background.blue);
          }
      }
    if (ping_info->valid & PNG_INFO_PLTE)
      {
        int
          num_palette;
        png_colorp
          palette;


        png_get_PLTE(ping, ping_info, &palette, &num_palette);
        if(num_palette == 0 && ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          {
            if(global_plte_length)
              {
                png_set_PLTE(ping, ping_info, m->global_plte,
                  global_plte_length);
                if(m->verbose)
                  printf("setting global PLTE.\n");
                if (!(ping_info->valid & PNG_INFO_tRNS))
                  if(global_trns_length)
                  {
                    if(m->verbose)
                      printf("setting global tRNS.\n");
                    if(global_trns_length > global_plte_length)
                      MagickWarning(DelegateWarning,
                        "global tRNS has more entries than global PLTE in file",
                        image_info->filename);
                    png_set_tRNS(ping, ping_info, m->global_trns,
                       global_trns_length, NULL);
                  }
              }
            else
              MagickWarning(DelegateWarning,
                "No global PLTE in file",image_info->filename);
          }
      }

    if (ping_info->valid & PNG_INFO_tRNS)
      {
        /*
          Image has a transparent background.
        */
        transparent_color.red=ping_info->trans_values.red;
        transparent_color.green=ping_info->trans_values.green;
        transparent_color.blue=ping_info->trans_values.blue;
        transparent_color.index=ping_info->trans_values.gray;
        if (ping_info->bit_depth > QuantumDepth)
          {
            transparent_color.red=
              (unsigned short)XDownScale(ping_info->trans_values.red);
            transparent_color.green=
              (unsigned short)XDownScale(ping_info->trans_values.green);
            transparent_color.blue=
              (unsigned short)XDownScale(ping_info->trans_values.blue);
            transparent_color.index=
              (unsigned short)XDownScale(ping_info->trans_values.gray);
          }
      }
    png_read_update_info(ping,ping_info);
    /*
      Initialize image structure.
    */
    image_box.left = 0;
    image_box.right = (long)ping_info->width;
    image_box.top = 0;
    image_box.bottom = (long)ping_info->height;
    if(mng_type == 0)
      {
        mng_width=ping_info->width;
        mng_height=ping_info->height;
        frame = image_box;
        clip = image_box;
      }
#ifndef ALWAYS_VERBOSE
    if(image_info->verbose)
#endif
       printf("  Image width=%lu, height=%lu, delay=%d\n",
         ping_info->width, ping_info->height, image->delay);

    image->compression=ZipCompression;
    image->columns=(unsigned int) ping_info->width;
    image->rows=(unsigned int) ping_info->height;
    if ((ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
        (ping_info->color_type == PNG_COLOR_TYPE_GRAY))
      {
        image->class=PseudoClass;
        image->colors=1 << Min(ping_info->bit_depth,QuantumDepth);
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          image->colors=ping_info->num_palette;
      }
    if (image_info->ping)
      {
        png_destroy_read_struct(&ping,&ping_info,&end_info);
        CloseImage(image);
        MngFreeStruct(m,&have_mng_structure);
        return(image);
      }
    png_pixels=(unsigned char *)
      AllocateMemory(ping_info->rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AllocateMemory(image->rows*sizeof(unsigned char *));
    if ((png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (image->class == PseudoClass)
      {
        /*
          Initialize image colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(MaxRGB*i)/Max(image->colors-1,1);
          image->colormap[i].green=(MaxRGB*i)/Max(image->colors-1,1);
          image->colormap[i].blue=(MaxRGB*i)/Max(image->colors-1,1);
        }
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          for (i=0; i < (int) image->colors; i++)
          {
            image->colormap[i].red=
               (unsigned short)UpScale(ping_info->palette[i].red);
            image->colormap[i].green=
               (unsigned short)UpScale(ping_info->palette[i].green);
            image->colormap[i].blue=
               (unsigned short)UpScale(ping_info->palette[i].blue);
          }
      }
    packets=0;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->columns*image->rows*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      {
        FreeMemory((char *) png_pixels);
        FreeMemory((char *) scanlines);
        ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    /*
      Read image scanlines.
    */
    for (i=0; i < (int) image->rows; i++)
      scanlines[i]=png_pixels+(i*ping_info->rowbytes);
    png_read_image(ping,scanlines);
    png_read_end(ping,ping_info);
    /*
      Convert PNG pixels to runlength-encoded packets.
    */
    q=image->pixels;
    SetRunlengthEncoder(q);
    if (image->class == DirectClass)
      {
        Quantum
          blue,
          green,
          red;

        /*
          Convert image to DirectClass runlength-encoded packets.
        */
        if ((ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
            (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
            (ping_info->valid & PNG_INFO_tRNS))
          image->matte=True;
        for (y=0; y < (int) image->rows; y++)
        {
          p=scanlines[y];
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(red,p);
            green=red;
            blue=red;
            if (ping_info->color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
              {
                ReadQuantum(green,p);
                ReadQuantum(blue,p);
              }
            index=Opaque;
            if ((ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
                (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
              ReadQuantum(index,p);
            if (ping_info->valid & PNG_INFO_tRNS && ping_info->color_type
               != PNG_COLOR_TYPE_PALETTE)
              if ((red == transparent_color.red) &&
                  (green == transparent_color.green) &&
                  (blue == transparent_color.blue))
                index=Transparent;
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                (index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        SetRunlengthPackets(image,packets);
      }
    else
      {
        Quantum
          *quantum_scanline;

        register Quantum
          *r;

        /*
          Convert image to PseudoClass runlength-encoded packets.
        */
        quantum_scanline=(Quantum *)
          AllocateMemory(image->columns*sizeof(Quantum));
        if (quantum_scanline == (Quantum *) NULL)
          ReaderExit(ResourceLimitWarning,"m. Memory allocation failed",image);
        for (y=0; y < (int) image->rows; y++)
        {
          p=scanlines[y];
          r=quantum_scanline;
          switch (ping_info->bit_depth)
          {
            case 1:
            {
              register int
                bit;

              for (x=0; x < ((int) image->columns-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=7; bit >= (int) (8-(image->columns % 8)); bit--)
                    *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((int) image->columns-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((image->columns % 4) != 0)
                {
                  for (i=3; i >= (int) (4-(image->columns % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((int) image->columns-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((image->columns % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              for (x=0; x < (int) image->columns; x++)
                *r++=(*p++);
              break;
            }
            case 16:
            {
              for (x=0; x < (int) image->columns; x++)
              {
                ReadQuantum(*r,p);
                r++;
              }
              break;
            }
            default:
              break;
          }
          /*
            Transfer image scanline.
          */
          r=quantum_scanline;
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*r++);
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        FreeMemory((char *) quantum_scanline);
        SetRunlengthPackets(image,packets);
        if (image->class == PseudoClass)
          SyncImage(image);
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            /*
              Image has a transparent background.
            */
            image->class=DirectClass;
            image->matte=True;
            q=image->pixels;
            for (i=0; i < packets; i++)
            {
              index=q->index;
              q->index=Opaque;
              if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
                {
                  if (index < ping_info->num_trans)
                    q->index=(unsigned short)UpScale(ping_info->trans[index]);
                }
              else
                if (index == transparent_color.index)
                  q->index=Transparent;
              q++;
            }
          }
      }
    if (ping_info->num_text > 0)
      for (i=0; i < ping_info->num_text; i++)
      {
        if (Latin1Compare(ping_info->text[i].key,"Comment") == 0)
          ReadTextChunk(ping_info,i,&image->comments);
        if (Latin1Compare(ping_info->text[i].key,"Delay") == 0)
          if (image_info->delay == (char *) NULL)
            {
              char
                *delay;

              delay=(char *) NULL;
              ReadTextChunk(ping_info,i,&delay);
              image->delay=atoi(delay);
              FreeMemory(delay);
            }
        if (Latin1Compare(ping_info->text[i].key,"Description") == 0)
          ReadTextChunk(ping_info,i,&image->comments);
        if (Latin1Compare(ping_info->text[i].key,"Directory") == 0)
          ReadTextChunk(ping_info,i,&image->directory);
        if (Latin1Compare(ping_info->text[i].key,"Label") == 0)
          ReadTextChunk(ping_info,i,&image->label);
        if (Latin1Compare(ping_info->text[i].key,"Montage") == 0)
          ReadTextChunk(ping_info,i,&image->montage);
        if (Latin1Compare(ping_info->text[i].key,"Page") == 0)
          {
          if (image_info->page == (char *) NULL)
            ReadTextChunk(ping_info,i,&image->page);
          }
        if (Latin1Compare(ping_info->text[i].key,"Scene") == 0)
          {
            char
              *scene;

            scene=(char *) NULL;
            ReadTextChunk(ping_info,i,&scene);
            image->scene=atoi(scene);
            FreeMemory(scene);
          }
        if (Latin1Compare(ping_info->text[i].key,"Signature") == 0)
          ReadTextChunk(ping_info,i,&image->signature);
        if (Latin1Compare(ping_info->text[i].key,"Title") == 0)
          ReadTextChunk(ping_info,i,&image->label);
      }
#ifdef MNG_OBJECT_BUFFERS
    /*
      Store the object if necessary.
    */
    if(object_id && !m->frozen[object_id])
      {
        if(m->ob[object_id] == (MngBuffer *)NULL)
          {
            /* create a new object buffer */
            if(m->verbose)
              printf("Allocating memory for object buffer[%d]\n",object_id);
            m->ob[object_id]
              =(MngBuffer *)AllocateMemory(sizeof(MngBuffer));
            if(m->ob[object_id] != (MngBuffer *)NULL)
              {
                m->ob[object_id]->image = (Image *)NULL;
                m->ob[object_id]->reference_count=1;
              }
          }
        if(m->ob[object_id] == (MngBuffer *)NULL || m->ob[object_id]->frozen)
          {
            if(m->ob[object_id] == (MngBuffer *)NULL)
              MagickWarning(ResourceLimitWarning,
                "Memory allocation of MNG object buffer failed",image->filename);
            if(m->ob[object_id]->frozen)
              MagickWarning(ResourceLimitWarning,
                "Cannot overwrite frozen MNG object buffer",image->filename);
          }
        else
          {
            png_uint_32
              width,
              height;

            int
              bit_depth,
              color_type,
              interlace_method,
              compression_method,
              filter_method;

            if(m->verbose)
              {
                if(m->ob[object_id]->image != (Image *)NULL)
                  printf("Creating object buffer[%d]\n",object_id);
                else
                  printf("Updating object buffer[%d]\n",object_id);
              }
            if(m->ob[object_id]->image != (Image *)NULL)
              DestroyImage(m->ob[object_id]->image);
            image->orphan=True;
            m->ob[object_id]->image=
              CloneImage(image, image->columns, image->rows, True);
            image->orphan=False;
            if(m->ob[object_id]->image == (Image *)NULL)
               MagickWarning(ResourceLimitWarning,
                 "Cloning image for object buffer failed",image->filename);
            else
              {
                if(m->verbose)
                  printf("Copied image to object buffer[%d]\n",object_id);
                m->ob[object_id]->image->file=(FILE *)NULL;
              }
            png_get_IHDR(ping,ping_info,&width,&height,&bit_depth,
                &color_type,&interlace_method,&compression_method,
                &filter_method);
            m->ob[object_id]->width=width;
            m->ob[object_id]->height=height;
            m->ob[object_id]->color_type=color_type;
            m->ob[object_id]->interlace_method=interlace_method;
            m->ob[object_id]->compression_method
              =compression_method;
            m->ob[object_id]->filter_method=filter_method;
            if (ping_info->valid & PNG_INFO_PLTE)
              {
                int
                   num_palette;
                png_colorp
                   plte;
                /* copy the PLTE to the object buffer */
                png_get_PLTE(ping, ping_info, &plte, &num_palette);
                m->ob[object_id]->plte_length = num_palette;
                for (i=0; i<num_palette; i++)
                  {
                    m->ob[object_id]->plte[i] = plte[i];
                  }
              }
            else
                m->ob[object_id]->plte_length=0;
          }
      }
#endif

    /*
      Free memory.
    */
    png_destroy_read_struct(&ping,&ping_info,&end_info);
    FreeMemory((char *) png_pixels);
    FreeMemory((char *) scanlines);

    if(mng_type)
      {
        /* MngBox: unsigned long left, right, top, bottom. */
        MngBox
          crop_box;

        /* crop_box is with respect to the upper left corner of the MNG */
        crop_box.left   = image_box.left    + m->x_off[object_id];
        crop_box.right  = image_box.right   + m->x_off[object_id];
        crop_box.top    = image_box.top     + m->y_off[object_id];
        crop_box.bottom = image_box.bottom  + m->y_off[object_id];
        crop_box = mng_minimum_box(crop_box, clip);
        crop_box = mng_minimum_box(crop_box, frame);
        crop_box = mng_minimum_box(crop_box, m->object_clip[object_id]);

        if(crop_box.left   != image_box.left   + m->x_off[object_id] ||
           crop_box.right  != image_box.right  + m->x_off[object_id] ||
           crop_box.top    != image_box.top    + m->y_off[object_id] ||
           crop_box.bottom != image_box.bottom + m->y_off[object_id] )
      {
            if(crop_box.left < crop_box.right && crop_box.top < crop_box.bottom)
              {
                Image
                  *p;

                /* RectangleInfo: unsigned int width, height; int x, y. */
        RectangleInfo
          crop_info;

                char
                  page_geometry[MaxTextExtent];

                char
                  *page;

                /* crop_info is with respect to the upper left corner of
                   the image */
                crop_info.x = crop_box.left - m->x_off[object_id];
                crop_info.y = crop_box.top  - m->y_off[object_id];
                crop_info.width  = (unsigned int)(crop_box.right
                   - crop_box.left);
                crop_info.height = (unsigned int)(crop_box.bottom
                   - crop_box.top);

                page=image->page;
                image->page = (char *) NULL;
                FormatString(page_geometry,"%lux%lu+0+0",
                  image->columns, image->rows);
                image->page=PostscriptGeometry(page_geometry);

                image->orphan=True;
                p=CropImage(image, &crop_info);
                image->orphan=False;

                if (p == (Image *) NULL)
                  {
                    if(m->verbose)
                      MagickWarning(ResourceLimitWarning,"Unable to crop image",
                       "Who knows why?");
                    assert(image->page != (char *) NULL);
                    FreeMemory((char *) image->page);
                    image->page=page;
                  }
                else
                  {
                    image->columns=p->columns;
                    image->rows=p->rows;
                    image->packets=p->columns*p->rows;

                    if (image->pixels != (RunlengthPacket *) NULL)
                       FreeMemory((char *) image->pixels);
                    image->pixels=p->pixels;

                    if (image->pixels == (RunlengthPacket *) NULL)
                      MagickWarning(ResourceLimitWarning,"Unable to crop image",
                       "image->pixels == NULL");

                    p->pixels=(RunlengthPacket *)NULL;

                    if (image->packed_pixels != (unsigned char *) NULL)
                       FreeMemory((char *) image->packed_pixels);
                    image->packed_pixels = p->packed_pixels;
                    p->packed_pixels=(unsigned char *)NULL;

                    p->orphan=True;  /* don't mess up links or close FILE */
                    p->file=(FILE *)NULL;
                    DestroyImage(p);
                    FormatString(page_geometry,"%lux%lu%+ld%+ld",
                      image->columns, image->rows, crop_box.left,
                      crop_box.top);
                    if (image->page != (char *) NULL)
                      FreeMemory((char *) image->page);
                    image->page=PostscriptGeometry(page_geometry);
                    CondenseImage(image);
                    if(m->verbose)
                      printf("  object_id %d cropped to geometry=%s\n",
                        object_id, page_geometry);
                  }
              }
            else
              {
                /* No pixels in crop area.  The MNG spec still requires
                   a layer, though, so make a single transparent pixel in
                   the top left corner */
                image->columns=(unsigned int)1;
                image->rows=(unsigned int)1;
                image->packets=(unsigned long)1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    MagickWarning(ResourceLimitWarning,
                      "Unable to reallocate 1x1 image",
                      "p. Memory allocation failed");
                    return(False);
                  }
                image->matte=True;

                FormatString(page_geometry,"1x1%+ld%+ld",
                  crop_box.left, crop_box.top);
                if (image->page != (char *) NULL)
                  FreeMemory((char *) image->page);
                image->page=PostscriptGeometry(page_geometry);
                CondenseImage(image);
                if(m->verbose)
                  printf("  object_id %d cropped to geometry=%s\n",
                        object_id, page_geometry);
              }
          }
        if(image_info->coalesce_frames)
          {
        MNGCoalesce(image);
            if(m->verbose)
              printf("frames coalesced.\n");
      }
      }
  } while (Latin1Compare(image_info->magick,"MNG") == 0);

#ifndef ALWAYS_VERBOSE
  if(image_info->verbose)
#endif
    printf("Supposedly there are %d visible images.\n",image_found);

  if(image_info->insert_backdrops && !image_found && mng_width > 0 &&
      mng_height > 0)
    {
      /* insert a background layer if nothing else was found. */

        char
           background_page_geometry[MaxTextExtent];

        if (image->pixels != (RunlengthPacket *) NULL)
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImages(image);
                MngFreeStruct(m,&have_mng_structure);
                return((Image *) NULL);
              }
            image=image->next;
          }
        image->columns=mng_width;
        image->rows=mng_height;
        FormatString(background_page_geometry,"%lux%lu%+ld%+ld",
          mng_width, mng_height, 0, 0);
        if (image->page != (char *) NULL)
          FreeMemory((char *) image->page);
        image->page=PostscriptGeometry(background_page_geometry);
        image->packets=image->columns*image->rows;
        image->pixels=(RunlengthPacket *)
          AllocateMemory(image->packets*sizeof(RunlengthPacket));
        if (image->pixels == (RunlengthPacket *) NULL)
  {
            MagickWarning(ResourceLimitWarning,
             "Memory allocation failed for background layer", (char *) NULL);
            MngFreeStruct(m,&have_mng_structure);
            return((Image *) NULL);
          }
        image->background_color = mng_background_color;
        if(mandatory_back)
           image->matte = False;
        else
           image->matte = True;
        SetImage(image);
        CondenseImage(image);
        image_found++;
#ifndef ALWAYS_VERBOSE
        if(image_info->verbose)
#endif
          printf("Inserted an empty %s frame behind empty MNG.\n",
             background_page_geometry);
      }
  if(ticks_per_second)
     image->delay=100*final_delay/ticks_per_second;
  else
     image->delay=final_delay;
  if(image_info->verbose)
     printf("terminal delay=%d (final_delay=%d, tps=%d\n",image->delay,
       final_delay, ticks_per_second);
  while (image->previous != (Image *) NULL)
    {
      image_count++;
      if(image_count>10*image_found)
        {
          MagickWarning(DelegateWarning,
            "Linked list is corrupted, beginning of list not found",
             image_info->filename);
          return(Image *)NULL;
        }
    image=image->previous;
      if(image->next == (Image *)NULL)
        MagickWarning(DelegateWarning,
         "Linked list is corrupted; next_image is NULL",image_info->filename);
    }
  if(ticks_per_second && image_found > 1 && image->next == (Image *)NULL)
    MagickWarning(DelegateWarning,
     "image->next for first image is NULL but shouldn't be.",
      image_info->filename);
  if (!image_found)
    {
      MagickWarning(DelegateWarning,
        "No visible images in file",image_info->filename);
      if (image != (Image *) NULL)
        DestroyImages(image);
      MngFreeStruct(m,&have_mng_structure);
      return((Image *) NULL);
    }
#ifndef ALWAYS_VERBOSE
  if(image_info->verbose)
#endif
    printf("Finished loading %d visible images.\n",image_found);
  CloseImage(image);
  MngFreeStruct(m,&have_mng_structure);
  have_mng_structure=False;
  return(image);
}
#else
Export Image *ReadPNGImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"PNG library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

#if defined(HasPNG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePNGImage writes an image in the Portable Network Graphics
%  encoded image format.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu
%
%  The format of the WritePNGImage routine is:
%
%      status=WritePNGImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
%  To do (as of version 4.2.6, 22 May 1999 -- glennrp):
%
%    Figure out what to do with "dispose=<restore-to-previous>"
%    This will be complicated if we limit ourselves to generating
%    MNG-LC files.  For now we ignore the disposal method and
%    simply overlay the next image on it.
%
%    Check for identical PLTE's or PLTE/tRNS combinations and use a
%    global MNG PLTE or PLTE/tRNS combination when appropriate.
%
%    Improve selection of color type (use indexed-colour or indexed-colour
%    with tRNS when 256 or fewer colors or 256 color-alpha combinations
%    are present).
%
%    Check for identical sRGB and replace with a global sRGB (and remove
%    gAMA/cHRM if sRGB is found; check for identical gAMA/cHRM and
%    replace with global gAMA/cHRM.
%
%    Provide option to skip writing the signature tEXt chunks.
%
%    Encode JNG datastreams.
%
%    Provide an option to force LC files (to ensure exact framing rate)
%    instead of VLC (i.e., specify file type MNG-LC on the command line).
%
%    Provide an option to force VLC files instead of LC (i.e., specify
%    file type MNG-VLC on the command line), even when offsets are present.
%    This will involve expanding the embedded images with a transparent
%    region at the top and/or left.
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

void PNGLong(png_bytep p,png_uint_32 value)
{
  *p++=(png_byte) ((value >> 24) & 0xff);
  *p++=(png_byte) ((value >> 16) & 0xff);
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

void PNGType(png_bytep p,png_bytep type)
{
  *p++ = *type++;
  *p++ = *type++;
  *p++ = *type++;
  *p++ = *type++;
}

static void WriteTextChunk(const ImageInfo *image_info,png_info *ping_info,
  char *keyword,char *value)
{
  register int
    i;

  i=ping_info->num_text++;
  ping_info->text[i].key=keyword;
  ping_info->text[i].text=value;
  ping_info->text[i].text_length=Extent(value);
  ping_info->text[i].compression=
    image_info->compression == NoCompression ||
    (image_info->compression == UndefinedCompression &&
    ping_info->text[i].text_length < 128) ? -1 : 0;
  if(image_info->verbose)
    printf("image_info->compression = %d for text chunk %d\n",
      image_info->compression,i);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

Export unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  int
    framing_mode,
    old_framing_mode,
    x,
    y,
    have_global_plte,
#ifdef MNG_GLOBAL_COLOR_SPACE_SUPPORTED
    have_global_srgb,
    have_global_gamma,
    have_global_chrm,
#endif
    need_defi,
    need_fram,
    need_iterations,
    need_matte;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  RectangleInfo
    page_info;

  png_info
    *ping_info;

  png_struct
    *ping;

  unsigned char
    chunk[32],
    *png_pixels,
    **scanlines;

  unsigned int
    delay,
    final_delay,
    initial_delay,
    matte,
    scene,
    ticks_per_second;

  unsigned short
    value;

  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    WriterExit(FileOpenWarning,"Unable to open file",image);
  if (image_info->verbose)
    printf("Writing PNG image\n");
  if (image_info->adjoin)
    {
      Image
        *next_image;

      int
        need_geom;

      unsigned int
        height,
        width;

      page_info.width=0;
      page_info.height=0;
      page_info.x=0;
      page_info.y=0;
      have_global_plte=False;
#ifdef MNG_GLOBAL_COLOR_SPACE_SUPPORTED
      have_global_srgb=False;
      have_global_gamma= False;
      have_global_chrm=False;
#endif
      need_geom=True;
      need_defi=False;
      need_fram=False;
      need_matte=False;
      framing_mode=1;
      old_framing_mode=1;

      /* Determine image bounding box.  */

      if (image_info->page != (char *) NULL)
        {
         /* Get specified global "page" geometry.  */
          (void) ParseGeometry(image_info->page,&page_info.x,&page_info.y,
            &page_info.width,&page_info.height);
          need_geom=False;
        }

      /* check all the scenes */

      initial_delay = image->delay;
      need_iterations=False;

      for (next_image=image; next_image != (Image *) NULL; )
        {
          if (next_image->page != (char *) NULL)
            {
              /* Get "page" geometry of scene. */
              (void) ParseGeometry(next_image->page,&page_info.x,
                &page_info.y, &width, &height);
            }
          else
            {
              page_info.x=0;
              page_info.y=0;
            }
          if(need_geom)
            {
              if ((next_image->columns+page_info.x) > page_info.width)
                page_info.width=next_image->columns+page_info.x;
              if ((next_image->rows+page_info.y) > page_info.height)
                page_info.height=next_image->rows+page_info.y;
            }
          if (page_info.x || page_info.y)
               need_defi=True;
          if (next_image->matte)
               need_matte=True;
          if (next_image->dispose >= 2)
             if(next_image->matte || page_info.x || page_info.y ||
             (next_image->columns < page_info.width &&
              next_image->rows < page_info.height))
               need_fram=True;
          if (next_image->iterations)
               need_iterations = True;
          final_delay = next_image->delay;
          if(final_delay != initial_delay)
             need_fram=1;
          next_image=next_image->next;
       }

       if(image_info->verbose)
         {
           printf("initial delay=%d, final delay=%d\n",
             initial_delay, final_delay);
           printf("need_defi=%d, need_fram=%d\n",need_defi, need_fram);
         }

       if(!need_fram)
        {
          /*
             Only certain framing rates 100/n are exactly representable without
             the FRAM chunk but we'll allow some slop in VLC files
           */

          if(final_delay == 0)
            {
              if(need_iterations)
                {
                  /* It's probably a GIF with loop; don't run it *too* fast */
                final_delay=10;
                  MagickWarning(DelegateWarning,
                    "input has zero delay between all frames; assuming 10 cs",
                    image->filename);
                }
              else
                ticks_per_second=0;
            }
          if(final_delay > 0)ticks_per_second = 100/final_delay;
          if(final_delay > 50) ticks_per_second = 2;
          if(final_delay > 75) ticks_per_second = 1;
          if(final_delay > 125) need_fram=True;

          if(image_info->verbose)
            {
              printf("final delay=%d, need_fram=%d\n",final_delay,need_fram);
            }
          if(need_defi && final_delay > 2 && final_delay != 4 &&
             final_delay != 5 && final_delay != 10 && final_delay != 20 &&
             final_delay != 25 && final_delay != 50 && final_delay != 100)
                need_fram=True;  /* make it exact; we cannot have VLC anyway */
        }
       if(need_fram)
          ticks_per_second = 100;

      /* If pseudocolor, we should also check to see if all the
         palettes are identical and write a global PLTE if they are.
         ../glennrp Feb 99
      */

      /*
        Write the MNG version 0.95 signature and MHDR chunk.
      */
      (void) fwrite("\212MNG\r\n\032\n",1,8,image->file);
      MSBFirstWriteLong(28L,image->file);  /* chunk data length = 28 */
      PNGType(chunk,mng_MHDR);
      PNGLong(chunk+4,page_info.width);
      PNGLong(chunk+8,page_info.height);
      PNGLong(chunk+12,ticks_per_second);
      PNGLong(chunk+16,0L);    /* layer count = unknown */
      PNGLong(chunk+20,0L);    /* frame count = unknown */
      PNGLong(chunk+24,0L);    /* play time = unknown   */
      if(need_matte)
        {
          if(need_defi || need_fram || have_global_plte)
             PNGLong(chunk+28,11L);    /* simplicity =  LC */
          else
             PNGLong(chunk+28,9L);    /* simplicity = VLC */
        }
      else
        {
          if(need_defi || need_fram || have_global_plte)
             PNGLong(chunk+28,3L);    /* simplicity =  LC, no transparency */
          else
             PNGLong(chunk+28,1L);    /* simplicity = VLC, no transparency */
        }

      (void) fwrite(chunk,1,32,image->file);
      MSBFirstWriteLong(crc32(0,chunk,32),image->file);

        if ((image->previous == (Image *) NULL) &&
            (image->next != (Image *) NULL) && (image->iterations != 1))
          {
            /*
              Write MNG TERM chunk
            */
          MSBFirstWriteLong(10L,image->file);  /* data length = 10 */
          PNGType(chunk,mng_TERM);
          chunk[4]=3;  /* repeat animation */
          chunk[5]=0;  /* show last frame when done */
          PNGLong(chunk+6, (png_uint_32)(ticks_per_second*final_delay/100));
          if(image_info->verbose)
             printf(
              "Wrote MNG TERM chunk with delay=%d (tps=%d, final_delay=%d)\n",
               ticks_per_second*final_delay/100,ticks_per_second,final_delay);
          if(image->iterations == 0)
             PNGLong(chunk+10, 0x7fffffffL);
          else
          PNGLong(chunk+10, (png_uint_32)image->iterations);
          (void) fwrite(chunk,1,14,image->file);
          MSBFirstWriteLong(crc32(0,chunk,14),image->file);
          }
    }
  scene=0;
  delay=0;
  do
  {
    if(need_defi)
      {
        int
          previous_x,
          previous_y;

        unsigned int
          width,
          height;

        if(scene)
          {
            previous_x=page_info.x;
            previous_y=page_info.y;
          }
        else
          {
            previous_x=0;
            previous_y=0;
          }

        if (image->page != (char *) NULL)
          {
            (void) ParseGeometry(image->page,&page_info.x,&page_info.y,
               &width,&height);
          }
        else
          {
             page_info.x=0;
             page_info.y=0;
          }
        if(page_info.x || page_info.y || previous_x || previous_y)
          {
            MSBFirstWriteLong(12L,image->file);  /* data length = 12 */
            PNGType(chunk,mng_DEFI);
            chunk[4]=0; /* object 0 MSB */
            chunk[5]=0; /* object 0 LSB */
            chunk[6]=0; /* visible  */
            chunk[7]=0; /* abstract */
            PNGLong(chunk+8,page_info.x);
            PNGLong(chunk+12,page_info.y);
            (void) fwrite(chunk,1,16,image->file);
            MSBFirstWriteLong(crc32(0,chunk,16),image->file);
          }
      }

    /*
      Allocate the PNG structures
    */
    TransformRGBImage(image,RGBColorspace);
    ping=png_create_write_struct(PNG_LIBPNG_VER_STRING,(void *) NULL,
      PNGErrorHandler,PNGWarningHandler);
    if (ping == (png_struct *) NULL)
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_write_struct(&ping,(png_info **) NULL);
        WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    png_init_io(ping,image->file);
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
     {
        /*
          PNG write failed.
        */
        png_destroy_write_struct(&ping,&ping_info);
        if (scanlines != (unsigned char **) NULL)
          FreeMemory((char *) scanlines);
        if (png_pixels != (unsigned char *) NULL)
          FreeMemory((char *) png_pixels);
        CloseImage(image);
        return(False);
      }
    /*
      Prepare PNG for writing.
    */
    ping_info->width=image->columns;
    ping_info->height=image->rows;
    ping_info->bit_depth=image->depth;
    if (IsMonochromeImage(image))
      ping_info->bit_depth=1;
    ping_info->color_type=PNG_COLOR_TYPE_RGB;
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        ping_info->valid|=PNG_INFO_pHYs;
        ping_info->phys_unit_type=PNG_RESOLUTION_UNKNOWN;
        ping_info->x_pixels_per_unit=(png_uint_32) image->x_resolution;
        ping_info->y_pixels_per_unit=(png_uint_32) image->y_resolution;
        if (image->units == PixelsPerInchResolution)
          {
            ping_info->phys_unit_type=PNG_RESOLUTION_METER;
            ping_info->x_pixels_per_unit=(png_uint_32)
              (100.0*image->x_resolution/2.54);
            ping_info->y_pixels_per_unit=(png_uint_32)
              (100.0*image->y_resolution/2.54);
          }
        if (image->units == PixelsPerCentimeterResolution)
          {
            ping_info->phys_unit_type=PNG_RESOLUTION_METER;
            ping_info->x_pixels_per_unit=(png_uint_32)
              (100.0*image->x_resolution);
            ping_info->y_pixels_per_unit=(png_uint_32)
              (100.0*image->y_resolution);
          }
      }
    ping_info->valid|=PNG_INFO_bKGD;
    ping_info->background.red=
      (unsigned short)DownScale(image->background_color.red);
    ping_info->background.green=
      (unsigned short)DownScale(image->background_color.green);
    ping_info->background.blue=
      (unsigned short)DownScale(image->background_color.blue);
    ping_info->background.gray=
      (unsigned short)DownScale(Intensity(image->background_color));
    ping_info->background.index=ping_info->background.gray;
    matte=image->matte;
    if (matte)
      {
        /*
          Determine if there is one and only one transparent color.
          We should have done this before writing MHDR ../glennrp Feb 99
        */
        ping_info->color_type=PNG_COLOR_TYPE_GRAY_ALPHA;
        p=image->pixels;
        for (i=0; (p->index == Opaque) && (i < (int) (image->packets-1)); i++)
        {
          if (!IsGray(*p))
            ping_info->color_type=PNG_COLOR_TYPE_RGB_ALPHA;
          p++;
        }
        if (p->index != Opaque)
          ping_info->valid|=PNG_INFO_tRNS;
        ping_info->trans_values.red=p->red;
        ping_info->trans_values.green=p->green;
        ping_info->trans_values.blue=p->blue;
        ping_info->trans_values.gray=(unsigned short)Intensity(*p);
        ping_info->trans_values.index=(unsigned short)DownScale(p->index);
        for ( ; i < (int) image->packets; i++)
        {
          if (!IsGray(*p))
            ping_info->color_type=PNG_COLOR_TYPE_RGB_ALPHA;
          if (p->index != ping_info->trans_values.index)
            if (ColorMatch(ping_info->trans_values,*p,0))
              ping_info->valid&=(~PNG_INFO_tRNS);
          p++;
        }
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            matte=False;
            ping_info->color_type=PNG_COLOR_TYPE_RGB;
          }
      }
    if (IsGrayImage(image))
      ping_info->color_type=PNG_COLOR_TYPE_GRAY;
    else
      if (IsPseudoClass(image))
        {
          /*
            Set image palette.
          */
          ping_info->color_type=PNG_COLOR_TYPE_PALETTE;
          ping_info->num_palette=image->colors;
          ping_info->valid|=PNG_INFO_PLTE;
          ping_info->palette=(png_color *)
            AllocateMemory(image->colors*sizeof(png_color));
          if (ping_info->palette == (png_color *) NULL)
            WriterExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          for (i=0; i < (int) image->colors; i++)
          {
            ping_info->palette[i].red=
              (unsigned short)DownScale(image->colormap[i].red);
            ping_info->palette[i].green=
              (unsigned short)DownScale(image->colormap[i].green);
            ping_info->palette[i].blue=
              (unsigned short)DownScale(image->colormap[i].blue);
          }
          ping_info->bit_depth=1;
          while ((1 << ping_info->bit_depth) < (int) image->colors)
            ping_info->bit_depth<<=1;
          if (ping_info->valid & PNG_INFO_tRNS)
            {
              /*
                Identify which colormap entry is transparent.
              */
              ping_info->trans=(unsigned char *)
                AllocateMemory(image->colors*sizeof(unsigned char));
              if (ping_info->trans == (unsigned char *) NULL)
                WriterExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              for (i=0; i < (int) image->colors; i++)
              {
                ping_info->trans[i]=DownScale(Opaque);
                if (ColorMatch(ping_info->trans_values,image->colormap[i],0))
                  {
                    ping_info->trans[i]=ping_info->trans_values.index;
                    break;
                  }
              }
              ping_info->num_trans=i+1;
            }
          /*
            Identify which colormap entry is the background color.
          */
          for (i=0; i < (int) (image->colors-1); i++)
            if (ColorMatch(ping_info->background,image->colormap[i],0))
              break;
          ping_info->background.index=(unsigned short) i;
        }
#if defined(PNG_WRITE_sRGB_SUPPORTED)
    if (image->rendering_intent != UndefinedIntent)
      {
        /*
          Note image rendering intent.
        */
        ping_info->valid|=PNG_INFO_sRGB;
        ping_info->srgb_intent=(int) image->rendering_intent-1;
      }
    if (!image_info->adjoin || image->rendering_intent != UndefinedIntent)
#endif
    {
       if (image->gamma != 0.0)
         {
           /*
             Note image gamma.
           */
           ping_info->valid|=PNG_INFO_gAMA;
           ping_info->gamma=image->gamma;
         }
       if (image->chromaticity.white_point.x != 0.0)
         {
           /*
             Note image chromaticity.
           */
           ping_info->valid|=PNG_INFO_cHRM;
           ping_info->x_red=image->chromaticity.red_primary.x;
           ping_info->y_red=image->chromaticity.red_primary.y;
           ping_info->x_green=image->chromaticity.green_primary.x;
           ping_info->y_green=image->chromaticity.green_primary.y;
           ping_info->x_blue=image->chromaticity.blue_primary.x;
           ping_info->y_blue=image->chromaticity.blue_primary.y;
           ping_info->x_white=image->chromaticity.white_point.x;
           ping_info->y_white=image->chromaticity.white_point.y;
         }
    }
    ping_info->interlace_type=image_info->interlace != NoInterlace;
    /*
      Initialize compression level and filtering.
    */
    png_set_compression_level(ping,Min(image_info->quality/10,9));
    if ((image_info->quality % 10) > 5)
      png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
    else
      if ((image_info->quality % 10) != 5)
        png_set_filter(ping,PNG_FILTER_TYPE_BASE,image_info->quality % 10);
      else
        if ((ping_info->valid & PNG_INFO_tRNS) ||
            (ping_info->color_type == PNG_COLOR_TYPE_GRAY) ||
            (ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
            (image_info->quality < 50))
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);
        else
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
    if (need_fram && image_info->adjoin && ((image->delay != delay) ||
        framing_mode != old_framing_mode))
      {
        if(image->delay == delay)
        {
           /*
             Write a MNG FRAM chunk with the new framing mode.
           */
           MSBFirstWriteLong(1L,image->file);  /* data length = 1 */
           PNGType(chunk,mng_FRAM);
           chunk[4]=framing_mode;
           (void) fwrite(chunk,1,5,image->file);
           MSBFirstWriteLong(crc32(0,chunk,5),image->file);
        }
        else
        {
           /*
             Write a MNG FRAM chunk with the delay.
           */
           MSBFirstWriteLong(10L,image->file);  /* data length = 10 */
           PNGType(chunk,mng_FRAM);
           chunk[4]=framing_mode;
           chunk[5]=0;  /* frame name separator (no name) */
           chunk[6]=2;  /* flag for changing default delay */
           chunk[7]=0;  /* flag for changing frame timeout */
           chunk[8]=0;  /* flag for changing frame clipping */
           chunk[9]=0;  /* flag for changing frame sync_id */
           PNGLong(chunk+10,(png_uint_32)((ticks_per_second*image->delay)/100));
           (void) fwrite(chunk,1,14,image->file);
           MSBFirstWriteLong(crc32(0,chunk,14),image->file);
           delay=image->delay;
        }
        old_framing_mode = framing_mode;
      }


    if (image_info->adjoin)
      png_set_sig_bytes(ping,8);
    png_write_info(ping,ping_info);
    png_set_packing(ping);
    /*
      Allocate memory.
    */
    ping_info->rowbytes=image->columns;
    if (!IsPseudoClass(image))
      ping_info->rowbytes*=(matte ? 4 : 3)*(image->depth >> 3);
    png_pixels=(unsigned char *)
      AllocateMemory(ping_info->rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AllocateMemory(image->rows*sizeof(unsigned char *));
    if ((png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Initialize image scanlines.
    */
    for (i=0; i < (int) image->rows; i++)
      scanlines[i]=png_pixels+(ping_info->rowbytes*i);
    x=0;
    y=0;
    p=image->pixels;
    q=scanlines[y];
    if (IsMonochromeImage(image))
      {
        register unsigned char
          polarity;

        /*
          Convert PseudoClass image to a PNG monochrome image.
        */
        polarity=Intensity(image->colormap[0]) < (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(image->colormap[1]) > Intensity(image->colormap[0]);
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(p->index == polarity ? 1 : 0,q);
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                x=0;
                y++;
                q=scanlines[y];
              }
          }
          p++;
        }
      }
    else
      if (IsGrayImage(image))
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(Intensity(*p),q);
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                x=0;
                y++;
                q=scanlines[y];
              }
          }
          p++;
        }
      else
        if (!IsPseudoClass(image))
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              WriteQuantum(p->red,q);
              if (ping_info->color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
                {
                  WriteQuantum(p->green,q);
                  WriteQuantum(p->blue,q);
                }
              if (matte)
                WriteQuantum(p->index,q);
              x++;
              if (x == (int) image->columns)
                {
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  x=0;
                  y++;
                  if (y < (int) image->rows)
                    q=scanlines[y];
                }
            }
            p++;
          }
        else
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=p->index;
              x++;
              if (x == (int) image->columns)
                {
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  x=0;
                  y++;
                  if (y < (int) image->rows)
                    q=scanlines[y];
                }
            }
            p++;
          }
    png_write_image(ping,scanlines);
    /*
      Generate text chunks.
    */
    ping_info->num_text=0;
    ping_info->text=(png_text *) AllocateMemory(256*sizeof(png_text));
    if (ping_info->text == (png_text *) NULL)
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
    /* Write a Software tEXt chunk only in the first PNG datastream */
    if (image->scene == 0)
    WriteTextChunk(image_info,ping_info,"Software",MagickVersion);
    if (!image_info->adjoin)
      {
    SignatureImage(image);
    if (image->signature != (char *) NULL)
      WriteTextChunk(image_info,ping_info,"Signature",image->signature);
        if (image->delay != 0)
          {
            char
              delay[MaxTextExtent];

            FormatString(delay,"%u",image->delay);
            WriteTextChunk(image_info,ping_info,"Delay",delay);
          }
    if (image->scene != 0)
      {
        char
          scene[MaxTextExtent];

        FormatString(scene,"%u",image->scene);
        WriteTextChunk(image_info,ping_info,"Scene",scene);
      }
        if (image->page != (char *) NULL)
          WriteTextChunk(image_info,ping_info,"Page",image->page);
      }
    if (image->label != (char *) NULL)
      WriteTextChunk(image_info,ping_info,"Label",image->label);
    if (image->montage != (char *) NULL)
      WriteTextChunk(image_info,ping_info,"Montage",image->montage);
    if (image->directory != (char *) NULL)
      WriteTextChunk(image_info,ping_info,"Directory",image->directory);
    if (image->comments != (char *) NULL)
      WriteTextChunk(image_info,ping_info,"Comment",image->comments);
    png_write_end(ping,ping_info);

    if(need_fram && image->dispose == 2)
      {
        if(page_info.x || page_info.y || ping_info->width != page_info.width ||
           ping_info->height != page_info.height)
          {
             /* Write FRAM 4 with clipping boundaries followed by FRAM 1. */
             MSBFirstWriteLong(27L,image->file);  /* data length = 27 */
             PNGType(chunk,mng_FRAM);
             chunk[4]=4;
             chunk[5]=0;  /* frame name separator (no name) */
             chunk[6]=1;  /* flag for changing delay, for next frame only */
             chunk[7]=0;  /* flag for changing frame timeout */
             chunk[8]=1;  /* flag for changing frame clipping for next frame */
             chunk[9]=0;  /* flag for changing frame sync_id */
             PNGLong(chunk+10,(png_uint_32)(0L)); /* temporary 0 delay */
             chunk[14]=0; /* clipping boundaries delta type */
             PNGLong(chunk+15,(png_uint_32)(page_info.x)); /* left cb */
             PNGLong(chunk+19,(png_uint_32)(page_info.x + ping_info->width));
             PNGLong(chunk+23,(png_uint_32)(page_info.y)); /* top cb */
             PNGLong(chunk+27,(png_uint_32)(page_info.y + ping_info->height));
             (void) fwrite(chunk,1,31,image->file);
             MSBFirstWriteLong(crc32(0,chunk,31),image->file);

             old_framing_mode = 4;
             framing_mode = 1;
          }
        else
            framing_mode = 3;
      }
    if(need_fram && image->dispose == 3)
      {
        MagickWarning(DelegateWarning,
          "Cannot convert GIF with disposal method 3 to MNG-LC",(char *) NULL);
      }

    /*
      Free PNG resources.
    */
    if (ping_info->valid & PNG_INFO_PLTE)
      FreeMemory((char *) ping_info->palette);
    png_destroy_write_struct(&ping,&ping_info);
    FreeMemory((char *) scanlines);
    FreeMemory((char *) png_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  if (image_info->adjoin)
    {
      /*
        Write the MEND chunk.
      */
      MSBFirstWriteLong(0x00000000L,image->file);
      PNGType(chunk,mng_MEND);
      (void) fwrite(chunk,1,4,image->file);
      MSBFirstWriteLong(crc32(0,chunk,4),image->file);
    }
  /*
    Free memory.
  */
  CloseImage(image);
  return(True);
}
#else
Export unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"PNG library is not available",
    image->filename);
  return(False);
}
#endif
