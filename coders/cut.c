/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                             CCC   U   U  TTTTT                              %
%                            C   C  U   U    T                                %
%                            C      U   U    T                                %
%                            C   C  U   U    T                                %
%                             CCC    UUU     T                                %
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

typedef struct
	{
	unsigned Width;
	unsigned Height;
	unsigned Reserved;
	}CUTHeader;			/*Dr Hallo*/
typedef struct
	{
	char FileId[2];
	unsigned Version;
	unsigned Size;
	char FileType;
	char SubType;
	unsigned BoardID;
	unsigned GraphicsMode;
	unsigned MaxIndex;
	unsigned MaxRed;
	unsigned MaxGreen;
	unsigned MaxBlue;
	char PaletteId[20];
	}CUTPalHeader;


static void InsertRow(unsigned char *p,long y,Image *image)
{
int bit; long x;
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
             for (x=0; x < ((long) image->columns-7); x+=8)
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
                 for (bit=0; bit < (long) (image->columns % 8); bit++)
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
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
            break;
            }
      case 2:  /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL)
                 break;
           indexes=GetIndexes(image);
           for (x=0; x < ((long) image->columns-1); x+=2)
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
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           break;
           }
	    
      case 4:  /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL)
                 break;
           indexes=GetIndexes(image);
           for (x=0; x < ((long) image->columns-1); x+=2)
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
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           break;
	   }
      case 8: /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL) break;
           indexes=GetIndexes(image);

	   for (x=0; x < (long) image->columns; x++)
                {
                index=ValidateColormapIndex(image,*p);
                indexes[x]=index;
                *q++=image->colormap[index];
                p++;
                }
           if (!SyncImagePixels(image))
                 break;
/*           if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           }
           break;

       }
}

static int GetCutColors(Image *image)
{
int MaxColor,x,y;
PixelPacket *q;
int UpScale16;

/*This procedure computes number of colors in Grayed R[i]=G[i]=B[i] image*/
UpScale16=UpScale(16);
MaxColor=0;
 for (y=0; y < (long)image->rows; y++)  
	{
	q=SetImagePixels(image,0,y,image->columns,1);  
	for (x=0; x < (long)image->columns; x++)  
             {  	   
	     if(MaxColor<q->red) MaxColor=q->red;
	     if(MaxColor>=UpScale16) return(255);	
	     q++;	
	     }	
	}
if(MaxColor<UpScale(2)) MaxColor=2;		
else if(MaxColor<UpScale(16)) MaxColor=16;		
return(MaxColor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C U T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCUTImage reads an CUT X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCUTImage method is:
%
%      Image *ReadCUTImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCUTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCUTImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image,*palette;
  ImageInfo *clone_info;
  unsigned int status;
  unsigned long EncodedByte;
  unsigned char RunCount,RunValue,RunCountMasked;
  CUTHeader  Header;
  CUTPalHeader PalHeader;
  long i,j;
  long ldblk;
  unsigned char *BImgBuff=NULL,*ptrB;
  PixelPacket *q;

  /*
    Open image file.
  */
  palette=NULL;
  clone_info=NULL;
  image=AllocateImage(image_info);
  
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read CUT image.
  */
   Header.Width=ReadBlobLSBShort(image);
   Header.Height=ReadBlobLSBShort(image);
   Header.Reserved=ReadBlobLSBShort(image);

   if (Header.Width==0 || Header.Height==0 || Header.Reserved!=0)
CUT_KO:  ThrowReaderException(CorruptImageWarning,"Not a CUT image file",image);

/*---This code checks first line of image---*/
  EncodedByte=ReadBlobLSBShort(image);
  RunCount=ReadBlobByte(image);
  RunCountMasked=RunCount & 0x7F;
  ldblk=0;
  while(RunCountMasked!=0)	/*end of line?*/
	{
	i=1;
	if(RunCount<0x80) i=RunCountMasked;
	(void) SeekBlob(image,TellBlob(image)+i,SEEK_SET);
	if(EOFBlob(image)) goto CUT_KO;	/*wrong data*/
	EncodedByte-=i+1;
	ldblk+=RunCountMasked;

	RunCount=ReadBlobByte(image);
	if(EOFBlob(image))  goto CUT_KO;	/*wrong data: unexpected eof in line*/
	RunCountMasked=RunCount & 0x7F;
	}
 if(EncodedByte!=1) goto CUT_KO;	/*wrong data: size incorrect*/
 i=0;				/*guess a number of bit planes*/
 if(ldblk==(int) Header.Width)   i=8;
 if(2*ldblk==(int) Header.Width) i=4;
 if(8*ldblk==(int) Header.Width) i=1;
 if(i==0) goto CUT_KO;		/*wrong data: incorrect bit planes*/

 image->columns=Header.Width;
 image->rows=Header.Height;
 image->depth=i;
 image->colors=1l >> i;

 
/* ----- Do something with palette ----- */
 if ((clone_info=CloneImageInfo(image_info)) == NULL) goto NoPalette;
 
 
 i=(long) strlen(clone_info->filename);
 j=i;
 while(--i>0)
        {
	if(clone_info->filename[i]=='.') 
		{
		break;
		}
	if(clone_info->filename[i]=='/' || clone_info->filename[i]=='\\' ||
	   clone_info->filename[i]==':' ) 
	       {
	       i=j;
	       break;
	       }
	}
	
 (void) strcpy(clone_info->filename+i,".PAL");
 if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
     {
     (void) strcpy(clone_info->filename+i,".pal");
     if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL)
         {
	 clone_info->filename[i]=0;
	 if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL) 
             {
	     DestroyImageInfo(clone_info);
	     clone_info=NULL;
	     goto NoPalette;
	     }
	 }
     }

 if( (palette=AllocateImage(clone_info))==NULL ) goto NoPalette;
 status=OpenBlob(clone_info,palette,ReadBinaryType);
 if (status == False)
     {
ErasePalette:     
     DestroyImage(palette);
     palette=NULL;
     goto NoPalette;
     }
 
     
 if(palette!=NULL)
   {
   (void) ReadBlob(palette,2,PalHeader.FileId);
   if(strncmp(PalHeader.FileId,"AH",2)) goto ErasePalette;
   PalHeader.Version=ReadBlobLSBShort(palette);
   PalHeader.Size=ReadBlobLSBShort(palette);
   PalHeader.FileType=ReadBlobByte(palette);
   PalHeader.SubType=ReadBlobByte(palette);
   PalHeader.BoardID=ReadBlobLSBShort(palette);
   PalHeader.GraphicsMode=ReadBlobLSBShort(palette);
   PalHeader.MaxIndex=ReadBlobLSBShort(palette);
   PalHeader.MaxRed=ReadBlobLSBShort(palette);
   PalHeader.MaxGreen=ReadBlobLSBShort(palette);
   PalHeader.MaxBlue=ReadBlobLSBShort(palette);
   (void) ReadBlob(palette,20,PalHeader.PaletteId);
   
   if(PalHeader.MaxIndex<1) goto ErasePalette;
   image->colors=PalHeader.MaxIndex+1;
   if (!AllocateImageColormap(image,image->colors)) goto NoMemory;
   
   if(PalHeader.MaxRed==0) PalHeader.MaxRed=MaxRGB;  /*avoid division by 0*/
   if(PalHeader.MaxGreen==0) PalHeader.MaxGreen=MaxRGB;
   if(PalHeader.MaxBlue==0) PalHeader.MaxBlue=MaxRGB;
   
   for(i=0;i<=(int) PalHeader.MaxIndex;i++)
           {      /*this may be wrong- I don't know why is palette such strange*/
	   j=(long) TellBlob(palette);
	   if((j % 512)>512-6)
	       {
	       j=((j / 512)+1)*512;
	       (void) SeekBlob(palette,j,SEEK_SET);
	       }
	   image->colormap[i].red=ReadBlobLSBShort(palette);
	   if(MaxRGB!=PalHeader.MaxRed) 
	       {
	       image->colormap[i].red=(Quantum)
	         (((unsigned long)image->colormap[i].red*MaxRGB+(PalHeader.MaxRed>>1))/PalHeader.MaxRed);
	       }
	   image->colormap[i].green=ReadBlobLSBShort(palette);
	   if(MaxRGB!=PalHeader.MaxGreen) 
	       {
	       image->colormap[i].green=(Quantum)
	         (((unsigned long)image->colormap[i].green*MaxRGB+(PalHeader.MaxGreen>>1))/PalHeader.MaxGreen);
	       }
	   image->colormap[i].blue=ReadBlobLSBShort(palette);       
	   if(MaxRGB!=PalHeader.MaxBlue)  
	       {
	       image->colormap[i].blue=(Quantum)
	         (((unsigned long)image->colormap[i].blue*MaxRGB+(PalHeader.MaxBlue>>1))/PalHeader.MaxBlue);
	       }
	       
	   }
   }

   

NoPalette:
 if(palette==NULL)
   { 
   
   image->colors=256;
   if (!AllocateImageColormap(image,image->colors))
	   {
NoMemory:  ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
				image)
           }	   
   
   for (i=0; i < (long)image->colors; i++)
	   {
	   image->colormap[i].red=UpScale(i);
	   image->colormap[i].green=UpScale(i);
	   image->colormap[i].blue=UpScale(i);
	   }
   }

           
/* ----- Load RLE compressed raster ----- */
 BImgBuff=(unsigned char *) malloc(ldblk);  /*Ldblk was set in the check phase*/
 if(BImgBuff==NULL) goto NoMemory;

 (void) SeekBlob(image,6 /*sizeof(Header)*/,SEEK_SET);
 for(i=0;i<(int) Header.Height;i++)
      {
      EncodedByte=ReadBlobLSBShort(image);

      ptrB=BImgBuff;
      j=ldblk;

      RunCount=ReadBlobByte(image);
      RunCountMasked=RunCount & 0x7F;

      while(RunCountMasked!=0)  	
    		{
    		if(RunCountMasked>j)
			{		/*Wrong Data*/
			RunCountMasked=(unsigned char) j;
			if(j==0) 
			    {
			    break;
			    }
			}

		if(RunCount>0x80)
			{
			RunValue=ReadBlobByte(image);
			(void) memset(ptrB,RunValue,RunCountMasked);
			}
		else {
		     (void) ReadBlob(image,RunCountMasked,ptrB);
		     }
		     
		ptrB+=RunCountMasked;
		j-=RunCountMasked;     
    
		if(EOFBlob(image)) goto Finish;	/* wrong data: unexpected eof in line */
		RunCount=ReadBlobByte(image);
		RunCountMasked=RunCount & 0x7F;
    		}

	InsertRow(BImgBuff,i,image);
   	}


/*detect monochrome image*/

if(palette==NULL)
    {		/*attempt to detect binary (black&white) images*/
    if(IsGrayImage(image))
      {
      if(GetCutColors(image)==2)
         {
	 for (i=0; i < (long)image->colors; i++)
	   {
	   j=UpScale(i);
	   if(image->colormap[i].red!=j) goto Finish;
	   if(image->colormap[i].green!=j) goto Finish;
	   if(image->colormap[i].blue!=j) goto Finish;
	   }
     
	 image->colormap[1].red=image->colormap[1].green=image->colormap[1].blue=MaxRGB;
	 for (i=0; i < (long)image->rows; i++)  
	   {
	   q=SetImagePixels(image,0,i,image->columns,1);  
	   for (j=0; j < (long)image->columns; j++)  
             {  	   
	     if(q->red==UpScale(1))
	        {
	        q->red=q->green=q->blue=MaxRGB;
	        }
	     q++;	
	     }	
	   if (!SyncImagePixels(image)) goto Finish;
	   }
	 }
       }     	 
    } 

Finish:
  if(BImgBuff!=NULL) free(BImgBuff);
  if(palette!=NULL) DestroyImage(palette);
  if(clone_info!=NULL) DestroyImageInfo(clone_info);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
  CloseBlob(image);
  return(image);       
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C U T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCUTImage adds attributes for the CUT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCUTImage method is:
%
%      RegisterCUTImage(void)
%
*/
ModuleExport void RegisterCUTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUT");
  entry->decoder=ReadCUTImage;
  entry->description=AllocateString("DR Hallo");
  entry->module=AllocateString("CUT");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C U T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCUTImage removes format registrations made by the
%  CUT module from the list of supported formats.
%
%  The format of the UnregisterCUTImage method is:
%
%      UnregisterCUTImage(void)
%
*/
ModuleExport void UnregisterCUTImage(void)
{
  (void) UnregisterMagickInfo("CUT");
}
