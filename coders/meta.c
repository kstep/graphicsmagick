/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M  EEEEE  TTTTT   AAA                            %
%                        MM MM  E        T    A   A                           %
%                        M M M  EEE      T    AAAAA                           %
%                        M   M  E        T    A   A                           %
%                        M   M  EEEEE    T    A   A                           %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             William Radcliffe                               %
%                                 July 2001                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,            %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "magick.h"
#include "utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMETAImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M E T A                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMETA returns True if the image format type, identified by the
%  magick string, is META.
%
%  The format of the IsMETA method is:
%
%      unsigned int IsMETA(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMETA returns True if the image format type is META.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
#ifdef IMPLEMENT_IS_FUNCTION
static unsigned int IsMETA(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"8BIM",4) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"APP1",4) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"\034\002",2) == 0)
    return(True);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M E T A I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMETAImage reads a META image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMETAImage method is:
%
%      Image *ReadMETAImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  Decompression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o image: Method ReadMETAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define BUFFER_SZ 4096

typedef struct _html_code
{
  short
    len;
  const char
    *code,
    val;
} html_code;

static html_code html_codes[] = {
#ifdef HANDLE_GT_LT
  { 4,"&lt;",'<' },
  { 4,"&gt;",'>' },
#endif
  { 5,"&amp;",'&' },
  { 6,"&quot;",'"' },
  { 6,"&apos;",'\''}
};

static int stringnicmp(const char *p,const char *q,size_t n)
{
  register long
    i,
    j;

  if (p == q)
    return(0);
  if (p == (char *) NULL)
    return(-1);
  if (q == (char *) NULL)
    return(1);
  while ((*p != '\0') && (*q != '\0'))
  {
    if ((*p == '\0') || (*q == '\0'))
      break;
    i=(*p);
    if (islower(i))
      i=toupper(i);
    j=(*q);
    if (islower(j))
      j=toupper(j);
    if (i != j)
      break;
    n--;
    if (n == 0)
      break;
    p++;
    q++;
  }
  return(toupper(*p)-toupper(*q));
}

static int convertHTMLcodes(char *s, int len)
{
  if (len <=0 || s==(char*)NULL || *s=='\0')
    return 0;

  if (s[1] == '#')
    {
      int val, o;

      if (sscanf(s,"&#%d;",&val) == 1)
      {
        o = 3;
        while (s[o] != ';')
        {
          o++;
          if (o > 5)
            break;
        }
        if (o < 6)
          strcpy(s+1, s+1+o);
        *s = val;
        return o;
      }
    }
  else
    {
      int
        i,
        codes = sizeof(html_codes) / sizeof(html_code);

      for (i=0; i < codes; i++)
      {
        if (html_codes[i].len <= len)
          if (stringnicmp(s, html_codes[i].code, html_codes[i].len) == 0)
            {
              strcpy(s+1, s+html_codes[i].len);
              *s = html_codes[i].val;
              return html_codes[i].len-1;
            }
      }
    }
  return 0;
}

static char *super_fgets(char **b, int *blen, Image *file)
{
  int
    c,
    len;

  unsigned char
    *q;

  len=*blen;
  for (q=(unsigned char *) (*b); ; q++)
  {
    c=ReadBlobByte(file);
    if (c == EOF || c == '\n')
      break;
    if ((q-(unsigned char *) (*b)+1) >= (int) len)
      {
        int
          tlen;

        tlen=q-(unsigned char *) (*b);
        len<<=1;
        ReacquireMemory((void **) b,(len+2));
        if ((*b) == (char *) NULL)
          break;
        q=(unsigned char*) (*b)+tlen;
      }
    *q=(unsigned char) c;
  }
  *blen=0;
  if ((*b) != (char *) NULL)
    {
      int
        tlen;

      tlen=q-(unsigned char *) (*b);
      if (tlen == 0)
        return (char *) NULL;
      (*b)[tlen] = '\0';
      *blen=++tlen;
    }
  return (*b);
}

#define BUFFER_SZ 4096
#define IPTC_ID 1028
#define THUMBNAIL_ID 1033

static long parse8BIM(Image *ifile, Image *ofile)
{
  char
    brkused,
    quoted,
    *line,
    *token,
    *newstr,
    *name;

  int
    state,
    next;

  unsigned char
    dataset;

  unsigned int
    recnum;

  int
    inputlen = BUFFER_SZ;

  long
    savedolen = 0L,
    outputlen = 0L;

  ExtendedSignedIntegralType
    savedpos,
    currentpos;

  TokenInfo
    token_info;

  dataset = 0;
  recnum = 0;
  line = (char *) AcquireMemory(inputlen);
  name = token = (char *)NULL;
  savedpos = 0;
  while(super_fgets(&line,&inputlen,ifile)!=NULL)
  {
    state=0;
    next=0;

    token = (char *) AcquireMemory(inputlen);
    newstr = (char *) AcquireMemory(inputlen);
    while (Tokenizer(&token_info, 0, token, inputlen, line,
          (char *) "", (char *) "=",
      (char *) "\"", 0, &brkused,&next,&quoted)==0)
    {
      if (state == 0)
        {
          int
            state,
            next;

          char
            brkused,
            quoted;

          state=0;
          next=0;
          while(Tokenizer(&token_info, 0, newstr, inputlen, token, (char *) "",
            (char *) "#", (char *) "", 0, &brkused, &next, &quoted)==0)
          {
            switch (state)
            {
              case 0:
                if (strcmp(newstr,"8BIM")==0)
                  dataset = 255;
                else
                  dataset = (unsigned char) atoi(newstr);
                break;
              case 1:
                recnum = atoi(newstr);
                break;
              case 2:
                name = (char *) AcquireMemory(strlen(newstr)+1);
                if (name)
                  strcpy(name,newstr);
                break;
            }
            state++;
          }
        }
      else
        if (state == 1)
          {
            int
              next;

            unsigned long
              len;

            char
              brkused,
              quoted;

            next=0;
            len = strlen(token);
            while (Tokenizer(&token_info,0, newstr, inputlen, token, (char *) "",
              (char *) "&", (char *) "", 0, &brkused, &next, &quoted)==0)
            {
              if (brkused && next > 0)
                {
                  char
                    *s = &token[next-1];

                  len -= convertHTMLcodes(s, strlen(s));
                }
            }

            if (dataset == 255)
              {
                unsigned char
                  nlen = 0;

                int
                  i;

                if (savedolen > 0)
                  {
                    long diff = outputlen - savedolen;
                    currentpos = TellBlob(ofile);
                    SeekBlob(ofile,savedpos,SEEK_SET);
                    WriteBlobMSBLong(ofile,diff);
                    SeekBlob(ofile,currentpos,SEEK_SET);
                    savedolen = 0L;
                  }
                if (outputlen & 1)
                  {
                    WriteBlobByte(ofile,0x00);
                    outputlen++;
                  }
                WriteBlobString(ofile,"8BIM");
                WriteBlobMSBShort(ofile,recnum);
                outputlen += 6;
                if (name)
                  nlen = strlen(name);
                WriteBlobByte(ofile,nlen);
                outputlen++;
                for (i=0; i<nlen; i++)
                  WriteBlobByte(ofile,name[i]);
                outputlen += nlen;
                if (!(nlen&1))
                  {
                    WriteBlobByte(ofile,0x00);
                    outputlen++;
                  }
                if (recnum != IPTC_ID)
                  {
                    WriteBlobMSBLong(ofile, len);
                    outputlen += 4;

                    next=0;
                    outputlen += len;
                    while (len--)
                      WriteBlobByte(ofile,token[next++]);

                    if (outputlen & 1)
                      {
                        WriteBlobByte(ofile,0x00);
                        outputlen++;
                      }
                  }
                else
                  {
                    /* patch in a fake length for now and fix it later */
                    savedpos = TellBlob(ofile);
                    WriteBlobMSBLong(ofile,0xFFFFFFFFUL);
                    outputlen += 4;
                    savedolen = outputlen;
                  }
              }
            else
              {
                if (len <= 0x7FFF)
                  {
                    WriteBlobByte(ofile,0x1c);
                    WriteBlobByte(ofile,dataset);
                    WriteBlobByte(ofile,recnum & 255);
                    WriteBlobMSBShort(ofile,len);
                    outputlen += 5;
                    next=0;
                    outputlen += len;
                    while (len--)
                      WriteBlobByte(ofile,token[next++]);
                  }
              }
          }
      state++;
    }
    LiberateMemory((void **) &token);
    LiberateMemory((void **) &newstr);
    LiberateMemory((void **) &name);
  }
  LiberateMemory((void **) &line);
  if (savedolen > 0)
    {
      long diff = outputlen - savedolen;

      currentpos = TellBlob(ofile);
      SeekBlob(ofile,savedpos,SEEK_SET);
      WriteBlobMSBLong(ofile,diff);
      SeekBlob(ofile,currentpos,SEEK_SET);
      savedolen = 0L;
    }
  return outputlen;
}

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0            /* Start Of Frame N */
#define M_SOF1  0xC1            /* N indicates which compression process */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8
#define M_EOI   0xD9            /* End Of Image (end of datastream) */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data) */
#define M_APP0  0xe0
#define M_APP1  0xe1
#define M_APP2  0xe2
#define M_APP3  0xe3
#define M_APP4  0xe4
#define M_APP5  0xe5
#define M_APP6  0xe6
#define M_APP7  0xe7
#define M_APP8  0xe8
#define M_APP9  0xe9
#define M_APP10 0xea
#define M_APP11 0xeb
#define M_APP12 0xec
#define M_APP13 0xed
#define M_APP14 0xee
#define M_APP15 0xef

static int jpeg_transfer_1(Image *ifile, Image *ofile)
{
  int c;

  c = ReadBlobByte(ifile);
  if (c == EOF)
    return EOF;
  WriteBlobByte(ofile,c);
  return c;
}

static int jpeg_skip_1(Image *ifile)
{
  int c;

  c = ReadBlobByte(ifile);
  if (c == EOF)
    return EOF;
  return c;
}

static int jpeg_read_remaining(Image *ifile, Image *ofile)
{
   int c;

  while ((c = jpeg_transfer_1(ifile, ofile)) != EOF)
    continue;
  return M_EOI;
}

static int jpeg_skip_variable(Image *ifile, Image *ofile)
{
  unsigned int  length;
  int c1,c2;

  if ((c1 = jpeg_transfer_1(ifile, ofile)) == EOF)
    return M_EOI;
  if ((c2 = jpeg_transfer_1(ifile, ofile)) == EOF)
    return M_EOI;

  length = (((unsigned char) c1) << 8) + ((unsigned char) c2);
  length -= 2;

  while (length--)
    if (jpeg_transfer_1(ifile, ofile) == EOF)
      return M_EOI;

  return 0;
}

static int jpeg_skip_variable2(Image *ifile, Image *ofile)
{
  unsigned int  length;
  int c1,c2;

  if ((c1 = ReadBlobByte(ifile)) == EOF) return M_EOI;
  if ((c2 = ReadBlobByte(ifile)) == EOF) return M_EOI;

  length = (((unsigned char) c1) << 8) + ((unsigned char) c2);
  length -= 2;

  while (length--)
    if (ReadBlobByte(ifile) == EOF)
      return M_EOI;

  return 0;
}

static int jpeg_nextmarker(Image *ifile, Image *ofile)
{
  int c;

  /* transfer anything until we hit 0xff */
  do
  {
    c = ReadBlobByte(ifile);
    if (c == EOF)
      return M_EOI; /* we hit EOF */
    else
      if (c != 0xff)
        WriteBlobByte(ofile,c);
  } while (c != 0xff);

  /* get marker byte, swallowing possible padding */
  do
  {
    c = ReadBlobByte(ifile);
    if (c == EOF)
      return M_EOI; /* we hit EOF */
  } while (c == 0xff);

  return c;
}

static int jpeg_skip_till_marker(Image *ifile, int marker)
{
  int c, i;

  do
  {
    /* skip anything until we hit 0xff */
    i = 0;
    do
    {
      c = ReadBlobByte(ifile);
      i++;
      if (c == EOF)
        return M_EOI; /* we hit EOF */
    } while (c != 0xff);

    /* get marker byte, swallowing possible padding */
    do
    {
      c = ReadBlobByte(ifile);
      if (c == EOF)
        return M_EOI; /* we hit EOF */
    } while (c == 0xff);
  } while (c != marker);
  return c;
}

static char psheader[] = "\xFF\xED\0\0Photoshop 3.0\08BIM\x04\x04\0\0\0\0";

/* Embed binary IPTC data into a JPEG image. */
static int jpeg_embed(Image *ifile, Image *ofile, Image *iptc)
{
  unsigned int marker;
  unsigned int done = 0;
  unsigned int len;
  int inx;

  if (jpeg_transfer_1(ifile, ofile) != 0xFF)
    return 0;
  if (jpeg_transfer_1(ifile, ofile) != M_SOI)
    return 0;

  while (!done)
  {
    marker = jpeg_nextmarker(ifile, ofile);
    if (marker == M_EOI)
      { /* EOF */
        break;
      }
    else
      {
        if (marker != M_APP13)
          {
            WriteBlobByte(ofile,0xff);
            WriteBlobByte(ofile,marker);
          }
      }

    switch (marker)
    {
      case M_APP13:
        /* we are going to write a new APP13 marker, so don't output the old one */
        jpeg_skip_variable2(ifile, ofile);
        break;

      case M_APP0:
        /* APP0 is in each and every JPEG, so when we hit APP0 we insert our new APP13! */
        jpeg_skip_variable(ifile, ofile);

        if (iptc != (Image *)NULL)
          {
            len=GetBlobSize(iptc);
            if (len & 1)
              len++; /* make the length even */
            psheader[ 2 ] = (len+16)>>8;
            psheader[ 3 ] = (len+16)&0xff;
            for (inx = 0; inx < 18; inx++)
              WriteBlobByte(ofile,psheader[inx]);
            jpeg_read_remaining(iptc, ofile);
            len=GetBlobSize(iptc);
            if (len & 1)
              WriteBlobByte(ofile,0);
          }
        break;

      case M_SOS:
        /* we hit data, no more marker-inserting can be done! */
        jpeg_read_remaining(ifile, ofile);
        done = 1;
        break;

      default:
        jpeg_skip_variable(ifile, ofile);
        break;
    }
  }
  return 1;
}

/* handle stripping the APP13 data out of a JPEG */
static void jpeg_strip(Image *ifile, Image *ofile)
{
  unsigned int marker;

  marker = jpeg_skip_till_marker(ifile, M_SOI);
  if (marker == M_SOI)
  {
    WriteBlobByte(ofile,0xff);
    WriteBlobByte(ofile,M_SOI);
    jpeg_read_remaining(ifile, ofile);
  }
}

/* Extract any APP13 binary data into a file. */
static int jpeg_extract(Image *ifile, Image *ofile)
{
  unsigned int marker;
  unsigned int done = 0;

  if (jpeg_skip_1(ifile) != 0xff)
    return 0;
  if (jpeg_skip_1(ifile) != M_SOI)
    return 0;

  while (!done)
  {
    marker = jpeg_skip_till_marker(ifile, M_APP13);
    if (marker == M_APP13)
      {
        marker = jpeg_nextmarker(ifile, ofile);
        break;
      }
  }
  return 1;
}

static Image *ReadMETAImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *buff,
    *image;

  int
    c;

  size_t
    length;

  unsigned int
    status;

  void
    *blob;

  /*
    Open file containing binary metadata
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"UnableToOpenFile",image);
  image->columns=1;
  image->rows=1;
  SetImage(image,OpaqueOpacity);
  length=1;
  if (LocaleNCompare(image_info->magick,"8BIM",4) == 0)
    {
      /*
        Read 8BIM binary metadata.
      */
      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
      blob=(unsigned char *) AcquireMemory(length);
      if (blob == (unsigned char *) NULL)
        {
          DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
            image)
        }
      AttachBlob(buff->blob,blob,length);
      if (LocaleCompare(image_info->magick,"8BIMTEXT") == 0)
        {
          length=parse8BIM(image, buff);
          if (length & 1)
            WriteBlobByte(buff,0x0);
        }
      else
        {
          for ( ; ; )
          {
            c=ReadBlobByte(image);
            if (c == EOF)
              break;
            WriteBlobByte(buff,c);
          }
        }
      image->iptc_profile.info = buff->blob->data;
      image->iptc_profile.length=GetBlobSize(buff);
      DetachBlob(buff->blob);
      DestroyImage(buff);
    }
  if (LocaleNCompare(image_info->magick,"APP1",4) == 0)
    {
      int
        i;
      /*
        Read APP1 binary metadata.
      */
      i=(long) image->generic_profiles;
      if (image->generic_profile == (ProfileInfo *) NULL)
        image->generic_profile=(ProfileInfo *)
          AcquireMemory(sizeof(ProfileInfo));
      else
        ReacquireMemory((void **) &image->generic_profile,
          (i+1)*sizeof(ProfileInfo));
      if (image->generic_profile == (ProfileInfo *) NULL)
        {
          image->generic_profiles=0;
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
            image)
        }
      image->generic_profiles++;
      image->generic_profile[i].name=AllocateString((char *) NULL);
      image->generic_profile[i].length=0;
      image->generic_profile[i].info=(unsigned char *) NULL;
      FormatString(image->generic_profile[i].name,"APP%d",1);

      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
      blob=(unsigned char *) AcquireMemory(length);
      if (blob == (unsigned char *) NULL)
        {
          DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
            image)
        }
      AttachBlob(buff->blob,blob,length);
      if (LocaleCompare(image_info->magick,"APP1JPEG") == 0)
        {
          Image
            *iptc;

          int
            result;

          ProfileInfo
            *pinfo;

          pinfo = (ProfileInfo *) image_info->client_data;
          if ((pinfo == (ProfileInfo *) NULL) ||
              (pinfo->info == (unsigned char *) NULL) || (pinfo->length == 0))
            {
              DetachBlob(buff->blob);
              LiberateMemory((void **) &blob);
              DestroyImage(buff);
              ThrowReaderException(CoderError,"NoIPTCProfileAvailable",image)
            }
          iptc=AllocateImage((ImageInfo *) NULL);
          if (iptc == (Image *) NULL)
            {
              DetachBlob(buff->blob);
              LiberateMemory((void **) &blob);
              DestroyImage(buff);
              ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
                image)
            }
          AttachBlob(iptc->blob,pinfo->info,pinfo->length);
          result=jpeg_embed(image,buff,iptc);
          DetachBlob(iptc->blob);
          DestroyImage(iptc);
          if (result == 0)
            {
              DetachBlob(buff->blob);
              LiberateMemory((void **) &blob);
              DestroyImage(buff);
              ThrowReaderException(CoderError,"JPEGEmbeddingFailed",image)
            }
        }
      else
        {
#ifdef SLOW_METHOD
          for ( ; ; )
          {
            /* Really - really slow - FIX ME PLEASE!!!! */
            c=ReadBlobByte(image);
            if (c == EOF)
              break;
            WriteBlobByte(buff,c);
          }
#else
#define MaxBufferSize  65541
          char
            *buffer;

          ExtendedSignedIntegralType
            count;

          size_t
            i,
            length;

          buffer=(char *) AcquireMemory(MaxBufferSize);
          if (buffer != (char *) NULL)
            {
              i=0;
              while ((length=ReadBlob(image,MaxBufferSize,buffer)) != 0)
              {
                count=0;
                for (i=0; i < length; i+=count)
                {
                  count=WriteBlob(buff,length-i,buffer+i);
                  if (count <= 0)
                    break;
                }
                if (i < length)
                  break;
              }
              LiberateMemory((void **) &buffer);
            }
#endif
        }
      image->generic_profile[i].info = buff->blob->data;
      image->generic_profile[i].length=GetBlobSize(buff);
      DetachBlob(buff->blob);
      DestroyImage(buff);
    }
  if ((LocaleCompare(image_info->magick,"ICC") == 0) ||
      (LocaleCompare(image_info->magick,"ICM") == 0))
    {
      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
      blob=(unsigned char *) AcquireMemory(length);
      if (blob == (unsigned char *) NULL)
        {
          DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
            image)
        }
      AttachBlob(buff->blob,blob,length);
      for ( ; ; )
      {
        c=ReadBlobByte(image);
        if (c == EOF)
          break;
        WriteBlobByte(buff,c);
      }
      image->color_profile.info = buff->blob->data;
      image->color_profile.length=GetBlobSize(buff);
      DetachBlob(buff->blob);
      DestroyImage(buff);
    }
  if (LocaleCompare(image_info->magick,"IPTC") == 0)
    {
      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
      blob=(unsigned char *) AcquireMemory(length);
      if (blob == (unsigned char *) NULL)
        {
          DestroyImage(buff);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
            image)
        }
      AttachBlob(buff->blob,blob,length);
      /* write out the header - length field patched below */
      WriteBlob(buff,12,"8BIM\04\04\0\0\0\0\0\0");
      if (LocaleCompare(image_info->magick,"IPTCTEXT") == 0)
        {
          length=parse8BIM(image, buff);
          if (length & 1)
            WriteBlobByte(buff,0x0);
        }
      else
        {
          for ( ; ; )
          {
            c=ReadBlobByte(image);
            if (c == EOF)
              break;
            WriteBlobByte(buff,c);
          }
        }
      image->iptc_profile.info = buff->blob->data;
      image->iptc_profile.length=GetBlobSize(buff);
      DetachBlob(buff->blob);
      DestroyImage(buff);
      if (image->iptc_profile.info != (unsigned char *) NULL)
        {
          /* subtract off the length of the 8BIM stuff */
          length=image->iptc_profile.length-12;
          image->iptc_profile.info[10]=length >> 8;
          image->iptc_profile.info[11]=length & 0xff;
        }
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M E T A I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMETAImage adds attributes for the META image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMETAImage method is:
%
%      RegisterMETAImage(void)
%
*/
ModuleExport void RegisterMETAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("8BIM");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Photoshop resource format");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("8BIMTEXT");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Photoshop resource text format");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("APP1");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Raw application information");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("APP1JPEG");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Raw JPEG binary data");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("EXIF");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Exif digital camera binary data");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("XMP");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("Adobe XML metadata");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICM");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("ICC Color Profile");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICC");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("ICC Color Profile");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("IPTC");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("IPTC Newsphoto");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("IPTCTEXT");
  entry->decoder=(DecoderHandler) ReadMETAImage;
  entry->encoder=(EncoderHandler) WriteMETAImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description=AllocateString("IPTC Newsphoto text format");
  entry->module=AllocateString("META");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M E T A I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMETAImage removes format registrations made by the
%  META module from the list of supported formats.
%
%  The format of the UnregisterMETAImage method is:
%
%      UnregisterMETAImage(void)
%
*/
ModuleExport void UnregisterMETAImage(void)
{
  (void) UnregisterMagickInfo("8BIM");
  (void) UnregisterMagickInfo("8BIMTEXT");
  (void) UnregisterMagickInfo("ICC");
  (void) UnregisterMagickInfo("ICCTEXT");
  (void) UnregisterMagickInfo("APP1");
  (void) UnregisterMagickInfo("ICM");
  (void) UnregisterMagickInfo("ICC");
  (void) UnregisterMagickInfo("EXIF");
  (void) UnregisterMagickInfo("XMP");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M E T A I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMETAImage writes a META image to a file.
%
%  The format of the WriteMETAImage method is:
%
%      unsigned int WriteMETAImage(const ImageInfo *image_info,Image *image)
%
%  Compression code contributed by Kyle Shorter.
%
%  A description of each parameter follows:
%
%    o status: Method WriteMETAImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/

static long GetIPTCStream(unsigned char **info,unsigned long length)
{
  long
    info_length;

  register long
    i;

  register unsigned char
    *p;

  unsigned char
    buffer[4],
    c;

  unsigned int
    marker;

  unsigned long
    tag_length;

  /*
    Find the beginning of the IPTC info.
  */
  p=(*info);
  tag_length=0;
iptc_find:
  info_length=0;
  marker=False;
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break;
    if (c == 0x1c)
      {
        p--;
        *info=p; /* let the caller know were it is */
        break;
      }
  }
  /*
    Determine the length of the IPTC info.
  */
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break;
    if (c == 0x1c)
      marker=True;
    else
      if (marker)
        break;
      else
        continue;
    info_length++;
    /*
      Found the 0x1c tag; skip the dataset and record number tags.
    */
    c=(*p++); /* should be 2 */
    length--;
    if (length == 0)
      break;
    if ((info_length == 1) && (c != 2))
      goto iptc_find;
    info_length++;
    c=(*p++); /* should be 0 */
    length--;
    if (length == 0)
      break;
    if ((info_length == 2) && (c != 0))
      goto iptc_find;
    info_length++;
    /*
      Decode the length of the block that follows - long or short format.
    */
    c=(*p++);
    length--;
    if (length == 0)
      break;
    info_length++;
    if (c & (unsigned char) 0x80)
      {
        for (i=0; i < 4; i++)
        {
          buffer[i]=(*p++);
          length--;
          if (length == 0)
            break;
          info_length++;
        }
        tag_length=(((long) buffer[0]) << 24) | (((long) buffer[1]) << 16) |
          (((long) buffer[2]) << 8) | (((long) buffer[3]));
      }
    else
      {
        tag_length=((long) c) << 8;
        c=(*p++);
        length--;
        if (length == 0)
          break;
        info_length++;
        tag_length|=(long) c;
      }
    if (tag_length > length)
      break;
    p+=tag_length;
    length-=tag_length;
    if (length == 0)
      break;
    info_length+=tag_length;
  }
  return(info_length);
}

static void formatString(Image *ofile, const char *s, int len)
{
  char
    temp[MaxTextExtent];

  WriteBlobByte(ofile,'"');
  for (; len > 0; --len, ++s) {
    int c = (*s) & 255;
    switch (c) {
    case '&':
      WriteBlobString(ofile,"&amp;");
      break;
#ifdef HANDLE_GT_LT
    case '<':
      WriteBlobString(ofile,"&lt;");
      break;
    case '>':
      WriteBlobString(ofile,"&gt;");
      break;
#endif
    case '"':
      WriteBlobString(ofile,"&quot;");
      break;
    default:
      if (isprint(c))
        WriteBlobByte(ofile,*s);
      else
        {
          FormatString(temp, "&#%d;", c & 255);
          WriteBlobString(ofile,temp);
        }
      break;
    }
  }
#if defined(WIN32)
  WriteBlobString(ofile,"\"\r\n");
#else
#if defined(macintosh)
  WriteBlobString(ofile,"\"\r");
#else
  WriteBlobString(ofile,"\"\n");
#endif
#endif
}

typedef struct _tag_spec
{
  short
    id;

  char
    *name;
} tag_spec;

static tag_spec tags[] = {
  { 5, (char *) "Image Name" },
  { 7, (char *) "Edit Status" },
  { 10, (char *) "Priority" },
  { 15, (char *) "Category" },
  { 20, (char *) "Supplemental Category" },
  { 22, (char *) "Fixture Identifier" },
  { 25, (char *) "Keyword" },
  { 30, (char *) "Release Date" },
  { 35, (char *) "Release Time" },
  { 40, (char *) "Special Instructions" },
  { 45, (char *) "Reference Service" },
  { 47, (char *) "Reference Date" },
  { 50, (char *) "Reference Number" },
  { 55, (char *) "Created Date" },
  { 60, (char *) "Created Time" },
  { 65, (char *) "Originating Program" },
  { 70, (char *) "Program Version" },
  { 75, (char *) "Object Cycle" },
  { 80, (char *) "Byline" },
  { 85, (char *) "Byline Title" },
  { 90, (char *) "City" },
  { 95, (char *) "Province State" },
  { 100, (char *) "Country Code" },
  { 101, (char *) "Country" },
  { 103, (char *) "Original Transmission Reference" },
  { 105, (char *) "Headline" },
  { 110, (char *) "Credit" },
  { 115, (char *) "Source" },
  { 116, (char *) "Copyright String" },
  { 120, (char *) "Caption" },
  { 121, (char *) "Image Orientation" },
  { 122, (char *) "Caption Writer" },
  { 131, (char *) "Local Caption" },
  { 200, (char *) "Custom Field 1" },
  { 201, (char *) "Custom Field 2" },
  { 202, (char *) "Custom Field 3" },
  { 203, (char *) "Custom Field 4" },
  { 204, (char *) "Custom Field 5" },
  { 205, (char *) "Custom Field 6" },
  { 206, (char *) "Custom Field 7" },
  { 207, (char *) "Custom Field 8" },
  { 208, (char *) "Custom Field 9" },
  { 209, (char *) "Custom Field 10" },
  { 210, (char *) "Custom Field 11" },
  { 211, (char *) "Custom Field 12" },
  { 212, (char *) "Custom Field 13" },
  { 213, (char *) "Custom Field 14" },
  { 214, (char *) "Custom Field 15" },
  { 215, (char *) "Custom Field 16" },
  { 216, (char *) "Custom Field 17" },
  { 217, (char *) "Custom Field 18" },
  { 218, (char *) "Custom Field 19" },
  { 219, (char *) "Custom Field 20" }
};

static int formatIPTC(Image *ifile, Image *ofile)
{
  char
    temp[MaxTextExtent];

  unsigned int
    foundiptc,
    tagsfound;

  unsigned char
    recnum,
    dataset;

  unsigned char
    *readable,
    *str;

  long
    tagindx,
    taglen;

  int
    i,
    tagcount = sizeof(tags) / sizeof(tag_spec);

  int
    c;

  foundiptc = 0; /* found the IPTC-Header */
  tagsfound = 0; /* number of tags found */

  c = ReadBlobByte(ifile);
  while (c != EOF)
  {
    if (c == 0x1c)
      foundiptc = 1;
    else
      {
        if (foundiptc)
          return -1;
        else
          continue;
      }

    /* we found the 0x1c tag and now grab the dataset and record number tags */
    c = ReadBlobByte(ifile);
    if (c == EOF) return -1;
    dataset = (unsigned char) c;
    c = ReadBlobByte(ifile);
    if (c == EOF) return -1;
    recnum = (unsigned char) c;
    /* try to match this record to one of the ones in our named table */
    for (i=0; i< tagcount; i++)
    {
      if (tags[i].id == recnum)
          break;
    }
    if (i < tagcount)
      readable = (unsigned char *) tags[i].name;
    else
      readable = (unsigned char *) "";

    /* then we decode the length of the block that follows - long or short fmt */
    c=ReadBlobByte(ifile);
    if (c == EOF) return -1;
    if (c & (unsigned char) 0x80)
      return 0;
    else
      {
        int
          c0;

        c0=ReadBlobByte(ifile);
        if (c0 == EOF) return -1;
        taglen = (c << 8) | c0;
      }
    if (taglen < 0) return -1;
    /* make a buffer to hold the tag data and snag it from the input stream */
    str=(unsigned char *) AcquireMemory((unsigned int) (taglen+1));
    if (str == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
    for (tagindx=0; tagindx<taglen; tagindx++)
    {
      c=ReadBlobByte(ifile);
      if (c == EOF) return -1;
      str[tagindx] = (unsigned char) c;
    }
    str[taglen] = 0;

    /* now finish up by formatting this binary data into ASCII equivalent */
    if (strlen((char *)readable) > 0)
      FormatString(temp, "%d#%d#%s=",(unsigned int)dataset, (unsigned int) recnum, readable);
    else
      FormatString(temp, "%d#%d=",(unsigned int)dataset, (unsigned int) recnum);
    WriteBlobString(ofile,temp);
    formatString( ofile, (char *)str, taglen );
    LiberateMemory((void **) &str);

    tagsfound++;

    c=ReadBlobByte(ifile);
  }
  return tagsfound;
}

static int readWordFromBuffer(char **s, long *len)
{
  unsigned char
    buffer[2];

  int
    i,
    c;

  for (i=0; i<2; i++)
  {
    c = *(*s)++; (*len)--;
    if (*len < 0) return -1;
    buffer[i] = (unsigned char) c;
  }
  return (((int) buffer[ 0 ]) <<  8) |
         (((int) buffer[ 1 ]));
}

static int formatIPTCfromBuffer(Image *ofile, char *s, long len)
{
  char
    temp[MaxTextExtent];

  unsigned int
    foundiptc,
    tagsfound;

  unsigned char
    recnum,
    dataset;

  unsigned char
    *readable,
    *str;

  long
    tagindx,
    taglen;

  int
    i,
    tagcount = sizeof(tags) / sizeof(tag_spec);

  int
    c;

  foundiptc = 0; /* found the IPTC-Header */
  tagsfound = 0; /* number of tags found */

  while (len > 0)
  {
    c = *s++; len--;
    if (c == 0x1c)
      foundiptc = 1;
    else
      {
        if (foundiptc)
          return -1;
        else
          continue;
      }

    /* we found the 0x1c tag and now grab the dataset and record number tags */
    c = *s++; len--;
    if (len < 0) return -1;
    dataset = (unsigned char) c;
    c = *s++; len--;
    if (len < 0) return -1;
    recnum = (unsigned char) c;
    /* try to match this record to one of the ones in our named table */
    for (i=0; i< tagcount; i++)
    {
      if (tags[i].id == recnum)
          break;
    }
    if (i < tagcount)
      readable = (unsigned char *) tags[i].name;
    else
      readable = (unsigned char *) "";

    /* then we decode the length of the block that follows - long or short fmt */
    c = *s++; len--;
    if (len < 0) return -1;
    if (c & (unsigned char) 0x80)
      return 0;
    else
      {
        s--; len++;
        taglen = readWordFromBuffer(&s, &len);
      }
    if (taglen < 0) return -1;
    /* make a buffer to hold the tag data and snag it from the input stream */
    str=(unsigned char *) AcquireMemory((unsigned int) (taglen+1));
    if (str == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
    for (tagindx=0; tagindx<taglen; tagindx++)
    {
      c = *s++; len--;
      if (len < 0) return -1;
      str[tagindx] = (unsigned char) c;
    }
    str[ taglen ] = 0;

    /* now finish up by formatting this binary data into ASCII equivalent */
    if (strlen((char *)readable) > 0)
      FormatString(temp, "%d#%d#%s=",(unsigned int)dataset, (unsigned int) recnum, readable);
    else
      FormatString(temp, "%d#%d=",(unsigned int)dataset, (unsigned int) recnum);
    WriteBlobString(ofile,temp);
    formatString( ofile, (char *)str, taglen );
    LiberateMemory((void **) &str);

    tagsfound++;
  }
  return tagsfound;
}

static int format8BIM(Image *ifile, Image *ofile)
{
  char
    temp[MaxTextExtent];

  unsigned int
    foundOSType;

  ExtendedSignedIntegralType
    Size;

  int
    ID,
    resCount,
    i,
    c;

  unsigned char
    *PString,
    *str;

  resCount = foundOSType = 0; /* found the OSType */

  c =ReadBlobByte(ifile);
  while (c != EOF)
  {
    if (c == '8')
      {
        unsigned char
          buffer[5];

        buffer[0] = (unsigned char) c;
        for (i=1; i<4; i++)
        {
          c=ReadBlobByte(ifile);
          if (c == EOF) return -1;
          buffer[i] = (unsigned char) c;
        }
        buffer[4] = 0;
        if (strcmp((const char *)buffer, "8BIM") == 0)
          foundOSType = 1;
        else
          continue;
      }
    else
      {
        c=ReadBlobByte(ifile);
        continue;
      }

    /* we found the OSType (8BIM) and now grab the ID, PString, and Size fields */
    ID = ReadBlobMSBShort(ifile);
    if (ID < 0) return -1;
    {
      unsigned char
        plen;

      c=ReadBlobByte(ifile);
      if (c == EOF) return -1;
      plen = (unsigned char) c;
      PString=(unsigned char *) AcquireMemory((unsigned int) (plen+1));
      if (PString == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
      for (i=0; i<plen; i++)
      {
        c=ReadBlobByte(ifile);
        if (c == EOF) return -1;
        PString[i] = (unsigned char) c;
      }
      PString[ plen ] = 0;
      if (!(plen&1))
      {
        c=ReadBlobByte(ifile);
        if (c == EOF) return -1;
      }
    }
    Size = ReadBlobMSBLong(ifile);
    if (Size < 0) return -1;
    /* make a buffer to hold the data and snag it from the input stream */
    str=(unsigned char *) AcquireMemory((size_t) Size);
    if (str == (unsigned char *) NULL)
      {
        printf("MemoryAllocationFailed");
        return 0;
      }
    for (i=0; i<Size; i++)
    {
      c=ReadBlobByte(ifile);
      if (c == EOF) return -1;
      str[i] = (unsigned char) c;
    }

    /* we currently skip thumbnails, since it does not make
     * any sense preserving them in a real world application
     */
    if (ID != THUMBNAIL_ID)
      {
        /* now finish up by formatting this binary data into
         * ASCII equivalent
         */
        if (strlen((const char *)PString) > 0)
          FormatString(temp, "8BIM#%d#%s=", ID, PString);
        else
          FormatString(temp, "8BIM#%d=", ID);
        WriteBlobString(ofile,temp);
        if (ID == IPTC_ID)
          {
            formatString(ofile, "IPTC", 4);
            formatIPTCfromBuffer(ofile, (char *)str, (long) Size);
          }
        else
          formatString(ofile, (char *)str, (long) Size);
      }
    LiberateMemory((void **) &str);
    LiberateMemory((void **) &PString);

    resCount++;

    c=ReadBlobByte(ifile);
  }
  return resCount;
}

static unsigned int WriteMETAImage(const ImageInfo *image_info,Image *image)
{
  register long
    i;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (LocaleCompare(image_info->magick,"8BIM") == 0)
    {
      /*
        Write 8BIM image.
      */
      if (image->iptc_profile.length == 0)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable",image);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
      (void) WriteBlob(image,image->iptc_profile.length,
        (char *) image->iptc_profile.info);
      CloseBlob(image);
      return(True);
    }
  if (LocaleCompare(image_info->magick,"IPTC") == 0)
    {
      size_t
        length;

      unsigned char
        *info;

      if (image->iptc_profile.length == 0)
        ThrowWriterException(CoderError,"NoIPTCProfileAvailable",image);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      info=image->iptc_profile.info;
      length=image->iptc_profile.length;
      length=GetIPTCStream(&info,length);
      if (length == 0)
        {
          ThrowWriterException(CoderError,"NoIPTCInfoWasFound",image);
        }
      (void) WriteBlob(image,length,info);
      CloseBlob(image);
      return(True);
    }
  if (LocaleCompare(image_info->magick,"8BIMTEXT") == 0)
    {
      Image
        *buff;

      if (image->iptc_profile.length == 0)
        ThrowWriterException(CoderError,"No8BIMDataIsAvailable",image);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        {
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image);
        }
      AttachBlob(buff->blob,image->iptc_profile.info,image->iptc_profile.length);
      format8BIM(buff,image);
      DetachBlob(buff->blob);
      DestroyImage(buff);
      CloseBlob(image);
      return(True);
    }
  if (LocaleCompare(image_info->magick,"IPTCTEXT") == 0)
    {
      Image
        *buff;

      size_t
        length;

      unsigned char
        *info;

      if (image->iptc_profile.length == 0)
        ThrowWriterException(CoderError,"NoIPTCDataIsAvailable",image);
      info=image->iptc_profile.info;
      length=image->iptc_profile.length;
      length=GetIPTCStream(&info,length);
      if (length == 0)
        ThrowWriterException(CoderError,"NoIPTCInfoWasFound",image);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
      buff=AllocateImage((ImageInfo *) NULL);
      if (buff == (Image *) NULL)
        {
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image);
        }
      AttachBlob(buff->blob,info,length);
      formatIPTC(buff,image);
      DetachBlob(buff->blob);
      DestroyImage(buff);
      CloseBlob(image);
      return(True);
    }
  if ((LocaleCompare(image_info->magick,"APP1") == 0) ||
      (LocaleCompare(image_info->magick,"EXIF") == 0) ||
      (LocaleCompare(image_info->magick,"XMP") == 0))
    {
      /*
        Write APP1 image.
      */
      for (i=0; i < (int) image->generic_profiles; i++)
      {
        char
          *name;

        long
          length;

        length=image->generic_profile[i].length;
        if (length == 0)
          ThrowWriterException(CoderError,"NoAPP1DataIsAvailable",image);
        name=image->generic_profile[i].name;
        if (LocaleCompare(name,image_info->magick) == 0)
          {
            status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
            (void) WriteBlob(image,(int) length,
              (char *) image->generic_profile[i].info);
            CloseBlob(image);
            return(True);
          }
      }
      ThrowWriterException(CoderError,"NoAPP1DataIsAvailable",image)
    }
  if ((LocaleCompare(image_info->magick,"ICC") == 0) ||
      (LocaleCompare(image_info->magick,"ICM") == 0))
    {
      /*
        Write ICM image.
      */
      if (image->color_profile.length == 0)
        ThrowWriterException(CoderError,"NoColorProfileAvailable",image);
      status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
      (void) WriteBlob(image,image->color_profile.length,
        (char *) image->color_profile.info);
      CloseBlob(image);
      return(True);
    }
  return(False);
}
