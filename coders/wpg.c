/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                            W   W  PPPP    GGGG                              %
%                            W   W  P   P  G                                  %
%                            W W W  PPPP   G GGG                              %
%                            WW WW  P      G   G                              %
%                            W   W  P       GGG                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                 June 2000                                   %
%                                                                             %
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
#include "define.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W P G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsWPG returns True if the image format type, identified by the
%  magick string, is WPG.
%
%  The format of the IsWPG method is:
%
%      unsigned int IsWPG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsWPG returns True if the image format type is WPG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsWPG(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\377WPC",4) == 0)
    return(True);
  return(False);
}


static void Rd_WP_DWORD(Image *image,unsigned long *d)
{
  unsigned char
    b;

  b=ReadBlobByte(image);
  *d=b;
  if (b < 0xFF)
    return;
  b=ReadBlobByte(image);
  *d=(unsigned long) b;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b*256l;
  if (*d < 0x8000)
    return;
  *d=(*d & 0x7FFF) << 16;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b;
  b=ReadBlobByte(image);
  *d+=(unsigned long) b*256l;
  return;
}

static void InsertRow(unsigned char *p,int y,Image *image)
{
int bit,x;
register PixelPacket *q;
IndexPacket index;
register IndexPacket *indexes;


 switch (image->depth)
      {
      case 1:  /* Convert bitmap scanline. */
             {
             q=SetImagePixels(image,0,y,image->columns,1);
             if (q == (PixelPacket *) NULL)
                   break;
             indexes=GetIndexes(image);
             for (x=0; x < ((int) image->columns-7); x+=8)
                {
                for (bit=0; bit < 8; bit++)
                   {
                   index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                   indexes[x+bit]=index;
                   *q++=image->colormap[index];
                   }
                p++;
                }
             if ((image->columns % 8) != 0)
                 {
                 for (bit=0; bit < (int) (image->columns % 8); bit++)
                     {
                     index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                     indexes[x+bit]=index;
                     *q++=image->colormap[index];
                     }
                 p++;
                 }
              if (!SyncImagePixels(image))
                 break;
/*            if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
            break;
            }
      case 2:  /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL)
                 break;
           indexes=GetIndexes(image);
           for (x=0; x < ((int) image->columns-1); x+=2)
                 {
                 index=ValidateColormapIndex(image,(*p >> 6) & 0x3);
                 indexes[x]=index;
                 *q++=image->colormap[index];
		 index=ValidateColormapIndex(image,(*p >> 4) & 0x3);
                 indexes[x]=index;
                 *q++=image->colormap[index];
		 index=ValidateColormapIndex(image,(*p >> 2) & 0x3);
                 indexes[x]=index;
                 *q++=image->colormap[index];
                 index=ValidateColormapIndex(image,(*p) & 0x3);
                 indexes[x+1]=index;
                 *q++=image->colormap[index];
                 p++;
                 }
           if ((image->columns % 4) != 0)
                 {
                   index=ValidateColormapIndex(image,(*p >> 6) & 0x3);
                   indexes[x]=index;
                   *q++=image->colormap[index];
		   if ((image->columns % 4) >= 1)

		      {
		      index=ValidateColormapIndex(image,(*p >> 4) & 0x3);
                      indexes[x]=index;
                      *q++=image->colormap[index];
		      if ((image->columns % 4) >= 2)

		        {
		        index=ValidateColormapIndex(image,(*p >> 2) & 0x3);
                        indexes[x]=index;
                        *q++=image->colormap[index];
		        }
		      }
                   p++;
                 }
           if (!SyncImagePixels(image))
                 break;
/*         if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           break;
           }
	    
      case 4:  /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL)
                 break;
           indexes=GetIndexes(image);
           for (x=0; x < ((int) image->columns-1); x+=2)
                 {
                 index=ValidateColormapIndex(image,(*p >> 4) & 0xf);
                 indexes[x]=index;
                 *q++=image->colormap[index];
                 index=ValidateColormapIndex(image,(*p) & 0xf);
                 indexes[x+1]=index;
                 *q++=image->colormap[index];
                 p++;
                 }
           if ((image->columns % 2) != 0)
                 {
                   index=ValidateColormapIndex(image,(*p >> 4) & 0xf);
                   indexes[x]=index;
                   *q++=image->colormap[index];
                   p++;
                 }
           if (!SyncImagePixels(image))
                 break;
/*         if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           break;
           }
      case 8: /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL) break;
           indexes=GetIndexes(image);

	   for (x=0; x < (int) image->columns; x++)
                {
                index=ValidateColormapIndex(image,*p++);
                indexes[x]=index;
                *q++=image->colormap[index];
                }
           if (!SyncImagePixels(image))
                 break;
/*           if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           }
           break;
	   
       case 24:     /*  Convert DirectColor scanline.  */
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
	    break;
          for (x=0; x < (int) image->columns; x++)
             {
             q->red=UpScale(*p++);
             q->green=UpScale(*p++);
             q->blue=UpScale(*p++);
             q++;
             }
          if (!SyncImagePixels(image))
            break;
/*          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
          break;	   
       }
}

#define InsertByte(b) {BImgBuff[x]=b;x++;if((int)x>=ldblk) {InsertRow(BImgBuff,y,image);x=0;y++;}}
static int UnpackWPGRaster(Image *image)
{
int x,y,i;
unsigned char bbuf,RunCount;
unsigned char *BImgBuff;

long ldblk;

 x=0;
 y=0;

 ldblk=((long)image->depth*image->columns+7)/8;
 BImgBuff=(unsigned char *) malloc(ldblk);
 if(BImgBuff==NULL) return(-2);

 while(y<(int) image->rows)
     {
     bbuf=ReadBlobByte(image);

/*     if not readed byte ??????	{
	delete Raster;
	Raster=NULL;
	return(-2);
	}*/

     RunCount=bbuf & 0x7F;
     if(bbuf & 0x80)
	{
	if(RunCount)	/* repeat next byte runcount * */
		{
		bbuf=ReadBlobByte(image);
		for(i=0;i<(int) RunCount;i++) InsertByte(bbuf);
		}
	   else {	/* read next byte as RunCount; repeat 0xFF runcount* */
		RunCount=ReadBlobByte(image);
		for(i=0;i<(int) RunCount;i++) InsertByte(0xFF);
		}
	}
   else {
	if(RunCount)   /* next runcount byte are readed directly */
		{
		for(i=0;i < (int) RunCount;i++)
			{
			bbuf=ReadBlobByte(image);
			InsertByte(bbuf);
			}
		}
	   else {	/* repeat previous line runcount* */
		RunCount=ReadBlobByte(image);
		if(x) {              /* attempt to duplicate row from x position: */
		      free(BImgBuff);/* I do not know what to do here */
		      return(-3);
		      }
		for(i=0;i < (int) RunCount;i++)
			{
			x=0;
			y++;    /* Here I need to duplicate previous row RUNCOUNT* */
			if(y<2) continue;
			if(y>(int) image->rows)
				 {
				 free(BImgBuff);
				 return(-4);
				 }
			InsertRow(BImgBuff,y-1,image);
			}
		}
	}
   }
 free(BImgBuff);
 return(0);
}

#define InsertRByte(b) {BImgBuff[x]=b;x++;if(x>=ldblk) {InsertRow(BImgBuff,image->rows-y-1,image);x=0;y++;}}
static int UnpackWPG2Raster(Image *image)
{
int x,y,i;
unsigned char bbuf,RunCount;
unsigned char *BImgBuff;
long ldblk;
unsigned char SampleBuffer[8];
char SampleSize=1;


 x=0;
 y=0;
 ldblk=((long)image->depth*image->columns+7)/8;
 BImgBuff=(unsigned char *) malloc(ldblk);
 if(BImgBuff==NULL) return(-2);

 while(y<(int) image->rows)
     {
     bbuf=ReadBlobByte(image);

     switch(bbuf)
       {
       case 0x7D:SampleSize=ReadBlobByte(image);	
		 if(SampleSize>8) return(-2);
		 if(SampleSize<1) return(-2);
		 break;
       case 0x7E:
		 break;
       case 0x7F:RunCount=ReadBlobByte(image);	
		 for(i=0;i<(int) SampleSize*((int)RunCount+1);i++)
			{
			InsertRByte(0);
			}
		 break;
       case 0xFD:
		 break;
       case 0xFE:
		 break;
       case 0xFF:RunCount=ReadBlobByte(image);
		 for(i=0;i<(int) SampleSize*((int)RunCount+1);i++)
			{
			InsertRByte(0xFF);
			}
		 break;
       default:
	  RunCount=bbuf & 0x7F;

	  if(bbuf & 0x80)
		{	
		for(i=0;i<(int) SampleSize;i++)
			SampleBuffer[i]=ReadBlobByte(image);
		for(i=0;i<=(int)RunCount;i++)
		    for(bbuf=0;bbuf<SampleSize;bbuf++)
			InsertRByte(SampleBuffer[bbuf]);
		}
	   else {
		for(i=0;i<(int) SampleSize*((int)RunCount+1);i++)
			{
			bbuf=ReadBlobByte(image);
			InsertRByte(bbuf);
			}
		}
	  }

     }
 free(BImgBuff);
 return(0);
}


static Image *ExtractPostscript(Image *image,const ImageInfo *image_info,
  long PS_Offset,long PS_Size)
{
/*char Filename[MaxTextExtent];*/
FILE *f;
ImageInfo *clone_info;
ExceptionInfo exception;
Image *image2;

if ((clone_info=CloneImageInfo(image_info)) == NULL) return(image);
RewindBlob(clone_info->blob);
TemporaryFilename((char *) clone_info->filename);

if( (f=fopen(clone_info->filename,"wb"))==NULL) goto FINISH;
SeekBlob(image,PS_Offset,SEEK_SET);
while(PS_Size-->0)
    {
    fputc(ReadBlobByte(image),f);
    }
fclose(f);    

if((clone_info->file=fopen(clone_info->filename,"r"))==NULL) goto FINISH_UNL;
image2=ReadImage(clone_info,&exception);
/*if(clone_info->file!=NULL) {fclose(clone_info->file);clone_info->file=NULL;}*/    

if(image2!=NULL) 	/* Allocate next image structure. */
    {
    while(image->previous!=NULL)
       image=image->previous;
       
    image->previous=image2;
    image2->next=image;
    
    while(image->next!=NULL)
       image=image->next;
    }
    
FINISH_UNL:    
remove(clone_info->filename);/* */
FINISH:
DestroyImageInfo(clone_info);
return(image);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W P G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadWPGImage reads an WPG X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadWPGImage method is:
%
%      Image *ReadWPGImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadWPGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadWPGImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
typedef struct
	{
	unsigned long FileId;
	unsigned long DataOffset;
	unsigned int ProductType;
	unsigned int FileType;
	unsigned char MajorVersion;
	unsigned char MinorVersion;
	unsigned int EncryptKey;
	unsigned int Reserved;
	}WPGHeader;
typedef struct
	{
	unsigned char	RecType;
	unsigned long   RecordLength;
	}WPGRecord;
typedef struct
	{
	unsigned char	Class;
	unsigned char    RecType;
	unsigned long   Extension;
	unsigned long   RecordLength;
	}WPG2Record;
typedef struct
	{
	unsigned int Width;
	unsigned int Heigth;
	unsigned int Depth;
	unsigned int HorzRes;
	unsigned int VertRes;
	}WPGBitmapType1;
typedef struct
	{
	unsigned int Width;
	unsigned int Heigth;
	unsigned char Depth;
	unsigned char Compression;
	}WPG2BitmapType1;
typedef struct
	{
	unsigned int RotAngle;
	unsigned int LowLeftX;
	unsigned int LowLeftY;
	unsigned int UpRightX;
	unsigned int UpRightY;
	unsigned int Width;
	unsigned int Heigth;
	unsigned int Depth;
	unsigned int HorzRes;
	unsigned int VertRes;
	}WPGBitmapType2;
typedef struct
	{
	unsigned int StartIndex;
	unsigned int NumOfEntries;
	}WPGColorMapRec;
typedef struct {
	unsigned long PS_unknown1;
	unsigned int PS_unknown2;
	unsigned int PS_unknown3;
       } WPGPSl1Record;	
  Image *image;
  unsigned int status;
  WPGHeader  Header;
  WPGRecord  Rec;
  WPG2Record Rec2;
  WPGBitmapType1 BitmapHeader1;
  WPG2BitmapType1 Bitmap2Header1;
  WPGBitmapType2 BitmapHeader2;
  WPGColorMapRec WPG_Palette;
  int i;
  long ldblk;
  unsigned char *BImgBuff;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read WPG image.
  */
   Header.FileId=ReadBlobLSBLong(image);
   Header.DataOffset=ReadBlobLSBLong(image);
   Header.ProductType=ReadBlobLSBShort(image);
   Header.FileType=ReadBlobLSBShort(image);
   Header.MajorVersion=ReadBlobByte(image);
   Header.MinorVersion=ReadBlobByte(image);
   Header.EncryptKey=ReadBlobLSBShort(image);
   Header.Reserved=ReadBlobLSBShort(image);

  if (Header.FileId!=0x435057FF || (Header.ProductType>>8)!=0x16 )
      ThrowReaderException(CorruptImageWarning,"Not a WPG image file",image);
  if(Header.EncryptKey!=0 )
      ThrowReaderException(CorruptImageWarning,"Encrypted WPG image file",image);

  image->colors = 0;

  switch(Header.FileType)
    {
    case 1:     /*WPG level 1*/
      while(!EOFBlob(image)) /* object parser loop */
       {
       SeekBlob(image,Header.DataOffset,SEEK_SET);
       if(EOFBlob(image)) break;

       Rec.RecType=(i=ReadBlobByte(image));
       if(i==EOF) break;
       Rd_WP_DWORD(image,&Rec.RecordLength);
       if(EOFBlob(image)) break;


       Header.DataOffset=TellBlob(image)+Rec.RecordLength;

       switch(Rec.RecType)
	 {
	 case 0x0B: /* bitmap type 1 */
		 BitmapHeader1.Width=ReadBlobLSBShort(image);
		 BitmapHeader1.Heigth=ReadBlobLSBShort(image);
		 BitmapHeader1.Depth=ReadBlobLSBShort(image);
		 BitmapHeader1.HorzRes=ReadBlobLSBShort(image);
		 BitmapHeader1.VertRes=ReadBlobLSBShort(image);

		 if(BitmapHeader1.HorzRes && BitmapHeader1.VertRes)
		   {
		   image->units=PixelsPerCentimeterResolution;
		   image->x_resolution=BitmapHeader1.HorzRes/470.0;
		   image->y_resolution=BitmapHeader1.VertRes/470.0;
		   }
		 image->columns=BitmapHeader1.Width;
		 image->rows=BitmapHeader1.Heigth;
		 image->depth=BitmapHeader1.Depth;

		 goto UnpackRaster;

	 case 0x0E:	/*Color palette */
		 WPG_Palette.StartIndex=ReadBlobLSBShort(image);
		 WPG_Palette.NumOfEntries=ReadBlobLSBShort(image);

		 image->colors=WPG_Palette.NumOfEntries;
		 if (!AllocateImageColormap(image,image->colors))
			goto NoMemory;
		 for (i=WPG_Palette.StartIndex; i < (int)WPG_Palette.NumOfEntries; i++)
		   {
		   image->colormap[i].red=UpScale(ReadBlobByte(image));
		   image->colormap[i].green=UpScale(ReadBlobByte(image));
		   image->colormap[i].blue=UpScale(ReadBlobByte(image));
		   }
		 break;
		 
         case 0x11:  /* Start PS l1 */
		   if(Rec.RecordLength>8)
		      image=ExtractPostscript(image,image_info,
		               TellBlob(image)+8,   /*skip PS header in the wpg*/
			       Rec.RecordLength-8);
		   break;		 

	 case 0x14:  /* bitmap type 2 */
		 BitmapHeader2.RotAngle=ReadBlobLSBShort(image);
		 BitmapHeader2.LowLeftX=ReadBlobLSBShort(image);
		 BitmapHeader2.LowLeftY=ReadBlobLSBShort(image);
		 BitmapHeader2.UpRightX=ReadBlobLSBShort(image);
		 BitmapHeader2.UpRightY=ReadBlobLSBShort(image);
		 BitmapHeader2.Width=ReadBlobLSBShort(image);
		 BitmapHeader2.Heigth=ReadBlobLSBShort(image);
		 BitmapHeader2.Depth=ReadBlobLSBShort(image);
		 BitmapHeader2.HorzRes=ReadBlobLSBShort(image);
		 BitmapHeader2.VertRes=ReadBlobLSBShort(image);

		 image->units=PixelsPerCentimeterResolution;
		 image->page.width=(unsigned int)
                  ((BitmapHeader2.LowLeftX-BitmapHeader2.UpRightX)/470.0);
		 image->page.height=(unsigned int)
                   ((BitmapHeader2.LowLeftX-BitmapHeader2.UpRightY)/470.0);
		 image->page.x=(int) (BitmapHeader2.LowLeftX/470.0);
		 image->page.y=(int) (BitmapHeader2.LowLeftX/470.0);
		 if(BitmapHeader2.HorzRes && BitmapHeader2.VertRes)
		   {
		   image->x_resolution=BitmapHeader2.HorzRes/470.0;
		   image->y_resolution=BitmapHeader2.VertRes/470.0;
		   }
		 image->columns=BitmapHeader2.Width;
		 image->rows=BitmapHeader2.Heigth;
		 image->depth=BitmapHeader2.Depth;

UnpackRaster:
		 if (image->colors == 0 && image->depth!=24)
		     {
		     image->colors=1 << image->depth;
		     if (!AllocateImageColormap(image,image->colors))
			{
NoMemory:		ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
					 image);
			}
		     }
		 else {
		      if(image->depth<24)
			if( (int) image->colors<(1<<image->depth) && image->depth!=24 )
			    ReacquireMemory((void **) &image->colormap,(1<<image->depth)*sizeof(PixelPacket));
		      }
		      
		 if(image->depth==1)
		      {
		      if(image->colormap[0].red==0 && image->colormap[0].green==0 && image->colormap[0].blue==0 &&
			 image->colormap[1].red==0 && image->colormap[1].green==0 && image->colormap[1].blue==0)
				{  /*fix crippled monochrome palette*/
				image->colormap[1].red =
				image->colormap[1].green =
				image->colormap[1].blue = UpScale(255);
				}
		       }      

		 if(UnpackWPGRaster(image)<0) /* The raster cannot be unpacked */
		     {
DecompressionFailed: ThrowReaderException(ResourceLimitWarning,"Cannot decompress WPG raster",image);
		     }

		 /* Allocate next image structure. */
		 AllocateNextImage(image_info,image);
		 if (image->next == (Image *) NULL) goto Finish;
		 image=image->next;
		 image->colors=image->columns=image->rows=image->depth=0;
		 break;

	 case 0x1B:  /*Postscript l2*/
		 if(Rec.RecordLength>0x3C)
		      image=ExtractPostscript(image,image_info,
			       TellBlob(image)+0x3C,   /*skip PS l2 header in the wpg*/
			       Rec.RecordLength-0x3C);
		 break;
	 }
      }
      break;

   case 2:  /*WPG level 2*/
     while(!EOFBlob(image)) /* object parser loop */

       {
       SeekBlob(image,Header.DataOffset,SEEK_SET);
       if(EOFBlob(image)) break;

       Rec2.Class=(i=ReadBlobByte(image)); if(i==EOF) break;
       Rec2.RecType=(i=ReadBlobByte(image)); if(i==EOF) break;
       Rd_WP_DWORD(image,&Rec2.Extension);
       Rd_WP_DWORD(image,&Rec2.RecordLength);
       if(EOFBlob(image)) break;

       Header.DataOffset=TellBlob(image)+Rec2.RecordLength;

       switch(Rec2.RecType)
	 {
	 case 0x0C:  	/*Color palette */
	     WPG_Palette.StartIndex=ReadBlobLSBShort(image);
	     WPG_Palette.NumOfEntries=ReadBlobLSBShort(image);

	     image->colors=WPG_Palette.NumOfEntries;
	     if (!AllocateImageColormap(image,image->colors))
			ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
					 image);
	     for (i=WPG_Palette.StartIndex; i < (int)WPG_Palette.NumOfEntries; i++)
		   {
		   image->colormap[i].red=UpScale(ReadBlobByte(image));
		   image->colormap[i].green=UpScale(ReadBlobByte(image));
		   image->colormap[i].blue=UpScale(ReadBlobByte(image));
		   ReadBlobByte(image);   /*Opacity??*/
		   }
	     break;
	 case 0x0E:
	     Bitmap2Header1.Width=ReadBlobLSBShort(image);
	     Bitmap2Header1.Heigth=ReadBlobLSBShort(image);
	     Bitmap2Header1.Depth=ReadBlobByte(image);
	     Bitmap2Header1.Compression=ReadBlobByte(image);

	     if(Bitmap2Header1.Compression>1) continue; /*Unknown compression method */
	     switch(Bitmap2Header1.Depth)
	       {
	       case 1:image->depth=1;break;
	       case 2:image->depth=2;break;
	       case 3:image->depth=4;break;
	       case 4:image->depth=8;break;
	       case 8:image->depth=24;break;
	       default:continue;  /*Ignore raster with unknown depth*/
	       }
	     image->columns=Bitmap2Header1.Width;
	     image->rows=Bitmap2Header1.Heigth;  

	     if (image->colors == 0 && image->depth!=24)
		     {
		     image->colors=1 << image->depth;
		     if (!AllocateImageColormap(image,image->colors))
			goto NoMemory;
		     }
	     else {
		  if(image->depth<24)
		    if( (int) image->colors<(1<<image->depth) && image->depth!=24 )
			 ReacquireMemory((void **) &image->colormap,(1<<image->depth)*sizeof(PixelPacket));
		  }


	     switch(Bitmap2Header1.Compression)
		{
		case 0:		/*Uncompressed raster*/
		   {
		   ldblk=((long)image->depth*image->columns+7)/8;
		   if( (BImgBuff=(unsigned char *) malloc(ldblk))==NULL) goto NoMemory;

		   for(i=0;i<(int) image->rows;i++)
		      {
		      ReadBlob(image,ldblk,(char *)BImgBuff);
		      InsertRow(BImgBuff,i,image);
		      }
		   if(BImgBuff) free(BImgBuff);
		   break;
		   }
		case 1:		/*RLE for WPG2 */
		   {
		   if(UnpackWPG2Raster(image)<0)
			goto DecompressionFailed;
		   break;
		   }   
		}
		  /* Allocate next image structure. */
	     AllocateNextImage(image_info,image);
	     if (image->next == (Image *) NULL) goto Finish;
	     image=image->next;
	     image->colors=image->columns=image->rows=image->depth=0;
	     break;

	 case 0x12:	/* Postscript WPG2*/
	     if(Rec2.RecordLength>0x12)
		      image=ExtractPostscript(image,image_info,
		               TellBlob(image)+0x12,   /*skip PS header in the wpg2*/
			       Rec2.RecordLength-0x12);
	     break;
	     
/*	 default:printf("Record type %d; size %ld; offset %lX\n",Rec2.RecType,Rec2.RecordLength,Header.DataOffset-Rec2.RecordLength); */     
	 }
       }

      break;

   default:
     ThrowReaderException(CorruptImageWarning,"Unsupported level of WPG image",image);
   }


Finish:
 CloseBlob(image);
 
 while (image->previous != (Image *) NULL)
    {
    image=image->previous;
    if(image->next!=NULL)
      if(image->next->rows==0 && image->next->columns==0)
	    DestroyImage(image->next);
    }
 return(image);       
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r W P G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterWPGImage adds attributes for the WPG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWPGImage method is:
%
%      RegisterWPGImage(void)
%
*/
ModuleExport void RegisterWPGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("WPG");
  entry->decoder=ReadWPGImage;
  entry->magick=IsWPG;
  entry->description=AllocateString("Word Perfect Graphics");
  entry->module=AllocateString("WPG");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W P G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterWPGImage removes format registrations made by the
%  WPG module from the list of supported formats.
%
%  The format of the UnregisterWPGImage method is:
%
%      UnregisterWPGImage(void)
%
*/
ModuleExport void UnregisterWPGImage(void)
{
  UnregisterMagickInfo("WPG");
}
