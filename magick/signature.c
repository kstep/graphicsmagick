/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        SSSSS  IIIII   GGGG  N   N   AAA   TTTTT  U   U  RRRR   EEEEE        %
%        SS       I    G      NN  N  A   A    T    U   U  R   R  E            %
%         SSS     I    G  GG  N N N  AAAAA    T    U   U  RRRR   EEE          %
%           SS    I    G   G  N  NN  A   A    T    U   U  R R    E            %
%        SSSSS  IIIII   GGG   N   N  A   A    T     UUU   R  R   EEEEE        %
%                                                                             %
%                                                                             %
%             Methods to Compute a Digital Signature for an Image             %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              December 1992                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Define declarations.
*/
#define SignatureSize  64
#define Trunc32(x)  ((x) & 0xffffffffL)

/*
  Typedef declarations.
*/
typedef struct _SignatureInfo
{
  unsigned long
    digest[8],
    low_count,
    high_count;

  off_t
    offset;

  unsigned char
    data[SignatureSize];
} SignatureInfo;

/*
  Forward declarations.
*/
static void
  TransformSignature(SignatureInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   F i n a l i z e S i g n a t u r e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FinalizeSignature finalizes the SHA message digest computation.
%
%  The format of the FinalizeSignature method is:
%
%      FinalizeSignature(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
static void FinalizeSignature(SignatureInfo *signature_info)
{
  int
    count;

  unsigned long
    high_count,
    low_count;

  /*
    Finalize computing the SHA digest.
  */
  low_count=signature_info->low_count;
  high_count=signature_info->high_count;
  count=(int) ((low_count >> 3) & 0x3f);
  signature_info->data[count++]=0x80;
  if (count <= (SignatureSize-8))
    memset(signature_info->data+count,0,SignatureSize-8-count);
  else
    {
      memset(signature_info->data+count,0,SignatureSize-count);
      TransformSignature(signature_info);
      memset(signature_info->data,0,SignatureSize-8);
    }
  signature_info->data[56]=(high_count >> 24) & 0xff;
  signature_info->data[57]=(high_count >> 16) & 0xff;
  signature_info->data[58]=(high_count >> 8) & 0xff;
  signature_info->data[59]=(high_count >> 0) & 0xff;
  signature_info->data[60]=(low_count >> 24) & 0xff;
  signature_info->data[61]=(low_count >> 16) & 0xff;
  signature_info->data[62]=(low_count >> 8) & 0xff;
  signature_info->data[63]=(low_count >> 0) & 0xff;
  TransformSignature(signature_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t S i g n a t u r e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetSignatureInfo initializes the SHA message digest structure.
%
%  The format of the GetSignatureInfo method is:
%
%      GetSignatureInfo(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
static void GetSignatureInfo(SignatureInfo *signature_info)
{
  signature_info->digest[0]=0x6a09e667L;
  signature_info->digest[1]=0xbb67ae85L;
  signature_info->digest[2]=0x3c6ef372L;
  signature_info->digest[3]=0xa54ff53aL;
  signature_info->digest[4]=0x510e527fL;
  signature_info->digest[5]=0x9b05688cL;
  signature_info->digest[6]=0x1f83d9abL;
  signature_info->digest[7]=0x5be0cd19L;
  signature_info->low_count=0L;
  signature_info->high_count=0L;
  signature_info->offset=0;
  memset(signature_info->data,0,SignatureSize);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   T r a n s f o r m S i g n a t u r e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformSignature transforms the SHA message digest.
%
%  The format of the TransformSignature method is:
%
%      TransformSignature(SignatureInfo *signature_info)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%
*/
static void TransformSignature(SignatureInfo *signature_info)
{
#define Ch(x,y,z)  (((x) & (y))^(~(x) & (z)))
#define Maj(x,y,z)  (((x) & (y))^((x) & (z))^((y) & (z)))
#define Rot32(x,n)  Trunc32(((x >> n) | (x << (32-n))))
#define Sigma0(x)  (Rot32(x,7)^Rot32(x,18)^Trunc32(x >> 3))
#define Sigma1(x)  (Rot32(x,17)^Rot32(x,19)^Trunc32(x >> 10))
#define Suma0(x)  (Rot32(x,2)^Rot32(x,13)^Rot32(x,22))
#define Suma1(x)  (Rot32(x,6)^Rot32(x,11)^Rot32(x,25))

  static unsigned long
    K[64] =
    {
      0x428a2f98L, 0x71374491L, 0xb5c0fbcfL, 0xe9b5dba5L,
      0x3956c25bL, 0x59f111f1L, 0x923f82a4L, 0xab1c5ed5L,
      0xd807aa98L, 0x12835b01L, 0x243185beL, 0x550c7dc3L,
      0x72be5d74L, 0x80deb1feL, 0x9bdc06a7L, 0xc19bf174L,
      0xe49b69c1L, 0xefbe4786L, 0x0fc19dc6L, 0x240ca1ccL,
      0x2de92c6fL, 0x4a7484aaL, 0x5cb0a9dcL, 0x76f988daL,
      0x983e5152L, 0xa831c66dL, 0xb00327c8L, 0xbf597fc7L,
      0xc6e00bf3L, 0xd5a79147L, 0x06ca6351L, 0x14292967L,
      0x27b70a85L, 0x2e1b2138L, 0x4d2c6dfcL, 0x53380d13L,
      0x650a7354L, 0x766a0abbL, 0x81c2c92eL, 0x92722c85L,
      0xa2bfe8a1L, 0xa81a664bL, 0xc24b8b70L, 0xc76c51a3L,
      0xd192e819L, 0xd6990624L, 0xf40e3585L, 0x106aa070L,
      0x19a4c116L, 0x1e376c08L, 0x2748774cL, 0x34b0bcb5L,
      0x391c0cb3L, 0x4ed8aa4aL, 0x5b9cca4fL, 0x682e6ff3L,
      0x748f82eeL, 0x78a5636fL, 0x84c87814L, 0x8cc70208L,
      0x90befffaL, 0xa4506cebL, 0xbef9a3f7L, 0xc67178f2L
    };  /* 32-bit fractional part of the cube root of the first 64 primes */

  int
    j,
    shift;

  register int
    i;

  register unsigned char
    *p;

  unsigned long
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    lsb_first,
    T,
    T1,
    T2,
    W[64];

  shift=32;
  p=signature_info->data;
  lsb_first=1;
  if (*(char *) &lsb_first)
    {
      if (sizeof(lsb_first) > 4)
        for (i=0; i < 16; i+=2)
        {
          T=(*((unsigned long *) p));
          p+=8;
          W[i]=((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
            ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
          T>>=shift;
          W[i+1]=((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
            ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
        }
      else
        for (i=0; i < 16; i++)
        {
          T=(*((unsigned long *) p));
          p+=4;
          W[i]=((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
            ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
        }
    }
  else
    {
      if (sizeof(lsb_first) > 4)
        for (i=0; i < 16; i+=2)
        {
          T=(*((unsigned long *) p));
          p+=8;
          W[i]=Trunc32(T >> shift);
          W[i+1]=Trunc32(T);
        }
      else
        for (i=0; i < 16; i++)
        {
          T=(*((unsigned long *) p));
          p+=4;
          W[i]=Trunc32(T);
        }
    }
  A=signature_info->digest[0];
  B=signature_info->digest[1];
  C=signature_info->digest[2];
  D=signature_info->digest[3];
  E=signature_info->digest[4];
  F=signature_info->digest[5];
  G=signature_info->digest[6];
  H=signature_info->digest[7];
  for (i=16; i < 64; i++)
    W[i]=Trunc32(Sigma1(W[i-2])+W[i-7]+Sigma0(W[i-15])+W[i-16]);
  for (j=0; j < 64; j++)
  {
    T1=Trunc32(H+Suma1(E)+Ch(E,F,G)+K[j]+W[j]);
    T2=Trunc32(Suma0(A)+Maj(A,B,C));
    H=G;
    G=F;
    F=E;
    E=Trunc32(D+T1);
    D=C;
    C=B;
    B=A;
    A=Trunc32(T1+T2);
  }
  signature_info->digest[0]=Trunc32(signature_info->digest[0]+A);
  signature_info->digest[1]=Trunc32(signature_info->digest[1]+B);
  signature_info->digest[2]=Trunc32(signature_info->digest[2]+C);
  signature_info->digest[3]=Trunc32(signature_info->digest[3]+D);
  signature_info->digest[4]=Trunc32(signature_info->digest[4]+E);
  signature_info->digest[5]=Trunc32(signature_info->digest[5]+F);
  signature_info->digest[6]=Trunc32(signature_info->digest[6]+G);
  signature_info->digest[7]=Trunc32(signature_info->digest[7]+H);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   U p d a t e S i g n a t u r e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UpdateSignature updates the SHA message digest.
%
%  The format of the UpdateSignature method is:
%
%      UpdateSignature(SignatureInfo *signature_info,unsigned char *message,
%        size_t length)
%
%  A description of each parameter follows:
%
%    o signature_info: The address of a structure of type SignatureInfo.
%
%    o message: the message
%
%    o length: The length of the message.
%
%
*/
static void UpdateSignature(SignatureInfo *signature_info,
  unsigned char *message,size_t length)
{
  register int
    i;

  unsigned long
    count;

  /*
    Update the SHA digest.
  */
  count=Trunc32(signature_info->low_count+((unsigned char) length << 3));
  if (length < signature_info->low_count)
    signature_info->high_count++;
  signature_info->low_count=count;
  signature_info->high_count+=(unsigned char) length >> 29;
  if (signature_info->offset)
    {
      i=SignatureSize-signature_info->offset;
      if (i > length)
        i=length;
      memcpy(signature_info->data+signature_info->offset,message,i);
      length-=i;
      message+=i;
      signature_info->offset+=i;
      if (signature_info->offset != SignatureSize)
        return;
      TransformSignature(signature_info);
    }
  while (length >= SignatureSize)
  {
    memcpy(signature_info->data,message,SignatureSize);
    message+=SignatureSize;
    length-=SignatureSize;
    TransformSignature(signature_info);
  }
  memcpy(signature_info->data,message,length);
  signature_info->offset=length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S i g n a t u r e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SignatureImage computes a digital signature from an image with an
%  implementation of the NIST SHA-256 Message Digest algorithm.  This signature
%  uniquely identifies the image and is convenient for determining whether two
%  images are identical.
%
%  The format of the SignatureImage method is:
%
%      unsigned int SignatureImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
%
*/
MagickExport unsigned int SignatureImage(Image *image)
{
  char
    signature[MaxTextExtent];

  IndexPacket
    *indexes;

  int
    y;

  SignatureInfo
    signature_info;

  register int
    x;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *message;

  /*
    Allocate memory for digital signature.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  message=(unsigned char *) AcquireMemory(10*image->columns);
  if (message == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,
      "Unable to compute image signature","Memory allocation failed");
  /*
    Compute image digital signature.
  */
  GetSignatureInfo(&signature_info);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    q=message;
    for (x=0; x < (int) image->columns; x++)
    {
      *q++=XUpScale(p->red) >> 8;
      *q++=XUpScale(p->red);
      *q++=XUpScale(p->green) >> 8;
      *q++=XUpScale(p->green);
      *q++=XUpScale(p->blue) >> 8;
      *q++=XUpScale(p->blue);
      if (image->colorspace == CMYKColorspace)
        {
          *q++=XUpScale(p->opacity) >> 8;
          *q++=XUpScale(p->opacity);
          if (indexes != (IndexPacket *) NULL)
            {
              *q++=XUpScale(indexes[x]) >> 8;
              *q++=XUpScale(indexes[x]);
            }
        }
      else
        if (image->matte)
          {
            *q++=XUpScale(p->opacity) >> 8;
            *q++=XUpScale(p->opacity);
          }
        else
          {
            *q++=XUpScale(MaxRGB) >> 8;
            *q++=XUpScale(MaxRGB) & 0xff;
          }
      p++;
    }
    UpdateSignature(&signature_info,message,q-message);
  }
  FinalizeSignature(&signature_info);
  LiberateMemory((void **) &message);
  /*
    Convert digital signature to a 64 character hex string.
  */
  FormatString(signature,"%08lx%08lx%08lx%08lx%08lx%08lx%08lx%08lx",
    signature_info.digest[0],signature_info.digest[1],signature_info.digest[2],
    signature_info.digest[3],signature_info.digest[4],signature_info.digest[5],
    signature_info.digest[6],signature_info.digest[7]);
  while (SetImageAttribute(image,"Signature",(char *) NULL) != False);
  (void) SetImageAttribute(image,"Signature",signature);
  return(True);
}
