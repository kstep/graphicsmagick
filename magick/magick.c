/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%               Methods to Read or List ImageMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
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

/*
  Image format declarations.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static const MagickInfo
  ImageFormats[] =
  {
    { "AVS", ReadAVSImage, WriteAVSImage, True, False, "AVS X image file" },
#if defined(HasJBIG)
    { "BIE", ReadJBIGImage, WriteJBIGImage, False, False,
      "Joint Bi-level Image experts Group file interchange format" },
#endif
    { "BMP", ReadBMPImage, WriteBMPImage, True, False,
      "Microsoft Windows bitmap image file" },
    { "BMP24", ReadBMPImage, WriteBMPImage, True, False,
      "Microsoft Windows 24-bit bitmap image file" },
    { "CMYK", ReadCMYKImage, WriteCMYKImage, False, False,
      "Raw cyan, magenta, yellow, and black bytes" },
    { "DCM", ReadDCMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Digital Imaging and Communications in Medicine image file" },
    { "DCX", ReadPCXImage, WritePCXImage, True, False,
      "ZSoft IBM PC multi-page Paintbrush file" },
    { "DIB", ReadBMPImage, WriteBMPImage, True, False,
      "Microsoft Windows bitmap image file" },
    { "EPDF", ReadPDFImage, WritePDFImage, False, False,
      "Encapsulated Portable Document Format" },
    { "EPI", ReadPSImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Adobe Encapsulated PostScript Interchange format" },
    { "EPS", ReadPSImage, WritePSImage, False, False,
      "Adobe Encapsulated PostScript file" },
    { "EPS2", ReadPSImage, WritePS2Image, False, False,
      "Adobe Level II Encapsulated PostScript file" },
    { "EPSF", ReadPSImage, WritePSImage, False, False,
      "Adobe Encapsulated PostScript file" },
    { "EPSI", ReadPSImage, WritePSImage, False, False,
      "Adobe Encapsulated PostScript Interchange format" },
    { "EPT", ReadPSImage, WriteEPTImage, False, False,
      "Adobe Encapsulated PostScript with TIFF preview" },
    { "FAX", ReadFAXImage, WriteFAXImage, True, False, "Group 3 FAX" },
    { "FITS", ReadFITSImage, WriteFITSImage, False, False,
      "Flexible Image Transport System" },
#if defined(HasFPX)
    { "FPX", ReadFPXImage, WriteFPXImage, False, False, "FlashPix Format" },
#endif
    { "G3", ReadFAXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Group 3 FAX" },
    { "GIF", ReadGIFImage, WriteGIFImage, True, False,
      "CompuServe graphics interchange format" },
    { "GIF87", ReadGIFImage, WriteGIFImage, False, False,
      "CompuServe graphics interchange format (version 87a)" },
    { "GRADATION", ReadGRADATIONImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, False,
      "Gradual passing from one shade to another" },
    { "GRANITE", ReadLOGOImage, WriteLOGOImage, False, False, "Granite texture" },
    { "GRAY", ReadGRAYImage, WriteGRAYImage, True, False, "Raw gray bytes" },
    { "H", ReadLOGOImage, WriteLOGOImage, False, False, "Internal format" },
    { "HDF", ReadHDFImage, WriteHDFImage, True, False, "Hierarchical Data Format" },
    { "HISTOGRAM", ReadHISTOGRAMImage, WriteHISTOGRAMImage, False, False,
      "Histogram of the image" },
    { "HTM", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False, False,
      "Hypertext Markup Language with a client-side image map" },
    { "HTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False, False,
      "Hypertext Markup Language with a client-side image map" },
    { "ICB", ReadTGAImage, WriteTGAImage, True, False, "Truevision Targa image file" },
    { "ICC", ReadICCImage, WriteICCImage, False, False, "ICC Color Profile" },
    { "ICO", ReadICONImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Microsoft icon" },
    { "IMPLICIT", (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True, False,
      "Internal format" },
    { "IPTC", ReadIPTCImage, WriteIPTCImage, False, False, "IPTC Newsphoto" },
#if defined(HasJBIG)
    { "JBG", ReadJBIGImage, WriteJBIGImage, True, False,
      "Joint Bi-level Image experts Group file interchange format" },
    { "JBIG", ReadJBIGImage, WriteJBIGImage, True, False,
      "Joint Bi-level Image experts Group file interchange format" },
#endif
#if defined(HasJPEG)
    { "JPG", ReadJPEGImage, WriteJPEGImage, False, False,
      "Joint Photographic Experts Group JFIF format" },
    { "JPEG", ReadJPEGImage, WriteJPEGImage, False, False,
      "Joint Photographic Experts Group JFIF format" },
    { "JPEG24", ReadJPEGImage, WriteJPEGImage, False, False,
      "Joint Photographic Experts Group JFIF format" },
#endif
    { "LABEL", ReadLABELImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, False,
      "Text image format" },
    { "LOGO", ReadLOGOImage, WriteLOGOImage, False, False, "ImageMagick Logo" },
    { "MAP", ReadMAPImage, WriteMAPImage, False, False,
      "Colormap intensities and indices" },
    { "MATTE", ReadMIFFImage, WriteMATTEImage, True, False, "Matte format" },
    { "MIFF", ReadMIFFImage, WriteMIFFImage, True, False, "Magick image file format" },
#if defined(HasPNG)
    { "MNG", ReadPNGImage, WritePNGImage, True, False,
      "Multiple-image Network Graphics" },
#endif
    { "MONO", ReadMONOImage, WriteMONOImage, False, False,
      "Bi-level bitmap in least-significant-byte (LSB) first order" },
    { "MTV", ReadMTVImage, WriteMTVImage, True, False, "MTV Raytracing image format" },
    { "NETSCAPE", ReadLOGOImage, WriteLOGOImage, False, False,
      "Netscape 216 color cube" },
    { "NULL", ReadNULLImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "NULL image" },
    { "P7", ReadPNMImage, WritePNMImage, True, False, "Xv thumbnail format" },
    { "PBM", ReadPNMImage, WritePNMImage, True, False,
      "Portable bitmap format (black and white)" },
    { "PCD", ReadPCDImage, WritePCDImage, False, False, "Photo CD" },
    { "PCDS", ReadPCDImage, WritePCDImage, False, False, "Photo CD" },
    { "PCL", ReadPCLImage, WritePCLImage, False, False, "Page Control Language" },
    { "PCT", (Image *(*)(const ImageInfo *)) ReadPICTImage, WritePICTImage,
      False, False, "Apple Macintosh QuickDraw/PICT file" },
    { "PCX", ReadPCXImage, WritePCXImage, False, False,
      "ZSoft IBM PC Paintbrush file" },
    { "PDF", ReadPDFImage, WritePDFImage, True, False, "Portable Document Format" },
    { "PIC", ReadPICTImage, WritePICTImage, False, False,
      "Apple Macintosh QuickDraw/PICT file" },
    { "PICT", ReadPICTImage, WritePICTImage, False, False,
      "Apple Macintosh QuickDraw/PICT file" },
    { "PIX", ReadPIXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, False, "Alias/Wavefront RLE image format" },
    { "PLASMA", ReadPLASMAImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, False,
      "Plasma fractal image" },
    { "PGM", ReadPNMImage, WritePNMImage, True, False,
      "Portable graymap format (gray scale)" },
    { "PM", ReadXPMImage, WriteXPMImage, False, False,
      "X Windows system pixmap file (color)" },
    { "PNM", ReadPNMImage, WritePNMImage, True, False, "Portable anymap" },
    { "PPM", ReadPNMImage, WritePNMImage, True, False,
      "Portable pixmap format (color)" },
#if defined(HasPNG)
    { "PNG", ReadPNGImage, WritePNGImage, False, False, "Portable Network Graphics" },
#endif
    { "PREVIEW", (Image *(*)(const ImageInfo *)) NULL, WritePREVIEWImage, False, False,
      "Show a preview an image enhancement effect, or f/x" },
    { "PS", ReadPSImage, WritePSImage, True, False, "Adobe PostScript file" },
    { "PS2", (Image *(*)(const ImageInfo *)) NULL, WritePS2Image, True, False,
      "Adobe Level II PostScript file" },
    { "PS3", (Image *(*)(const ImageInfo *)) NULL, WritePS3Image, True, False,
      "Adobe Level III PostScript file" },
    { "PSD", ReadPSDImage, WritePSDImage, False, False,
      "Adobe Photoshop bitmap file" },
#if defined(HasTIFF)
    { "PTIF", ReadTIFFImage, WriteTIFFImage, True, False, "Pyramid encoded TIFF" },
#endif
    { "PWP", ReadPWPImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, False, "Seattle Film Works" },
    { "RAS", ReadSUNImage, WriteSUNImage, True, False, "SUN Rasterfile" },
    { "RGB", ReadRGBImage, WriteRGBImage, True, False,
      "Raw red, green, and blue bytes" },
    { "RGBA", ReadRGBImage, WriteRGBImage, True, False,
      "Raw red, green, blue, and matte bytes" },
    { "RLA", ReadRLAImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Alias/Wavefront image file" },
    { "RLE", ReadRLEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Utah Run length encoded image file" },
    { "SCT", ReadSCTImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Scitex HandShake" },
    { "SFW", ReadSFWImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Seattle Film Works" },
    { "SGI", ReadSGIImage, WriteSGIImage, True, False, "Irix RGB image file" },
    { "SHTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False, False,
      "Hypertext Markup Language with a client-side image map" },
    { "STEGANO", ReadSTEGANOImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True, False,
      "Steganographic image" },
    { "SUN", ReadSUNImage, WriteSUNImage, True, False, "SUN Rasterfile" },
    { "TEXT", ReadTXTImage, WriteTXTImage, True, False, "Raw text file" },
    { "TGA", ReadTGAImage, WriteTGAImage, True, False, "Truevision Targa image file" },
#if defined(HasTIFF)
    { "TIF", ReadTIFFImage, WriteTIFFImage, True, False, "Tagged Image File Format" },
    { "TIFF", ReadTIFFImage, WriteTIFFImage, True, False, "Tagged Image File Format" },
    { "TIFF24", ReadTIFFImage, WriteTIFFImage, True, False,
      "24-bit Tagged Image File Format" },
#endif
    { "TILE", ReadTILEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, False, "Tile image with a texture" },
    { "TIM", ReadTIMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, False, "PSX TIM file" },
#if defined(HasTTF)
    { "TTF", ReadTTFImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "True, FalseType font file" },
#endif
    { "TXT", ReadTXTImage, WriteTXTImage, True, False, "Raw text file" },
    { "UIL", ReadUILImage, WriteUILImage, False, False, "X-Motif UIL table" },
    { "UYVY", ReadUYVYImage, WriteUYVYImage, False, False,
      "16bit/pixel interleaved YUV" },
    { "VDA", ReadTGAImage, WriteTGAImage, True, False, "Truevision Targa image file" },
    { "VICAR", ReadVICARImage, WriteVICARImage, False, False,
      "VICAR rasterfile format" },
    { "VID", ReadVIDImage, WriteMIFFImage, True, False, "Visual Image Directory" },
    { "VIFF", ReadVIFFImage, WriteVIFFImage, True, False,
      "Khoros Visualization image file" },
    { "VST", ReadTGAImage, WriteTGAImage, True, False, "Truevision Targa image file" },
#if defined(HasX11)
    { "X", ReadXImage, WriteXImage, False, False, "X Image" },
#endif
    { "XBM", ReadXBMImage, WriteXBMImage, False, False,
      "X Windows system bitmap (black and white)" },
    { "XC", ReadXCImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, False, "Constant image of X server color" },
    { "XPM", ReadXPMImage, WriteXPMImage, False, False,
      "X Windows system pixmap file (color)" },
    { "XV", ReadVIFFImage, WriteVIFFImage, True, False,
      "Khoros Visualization image file" },
#if defined(HasX11)
    { "XWD", ReadXWDImage, WriteXWDImage, False, False,
      "X Windows system window dump file (color)" },
#endif
    { "YUV", ReadYUVImage, WriteYUVImage, False, False, "CCIR 601 4:1:1 file" },
    { (char *) NULL, (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, False,
      (char *) NULL },
  };

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagickInfo returns a pointer MagickInfo structure that matches
%  the specified tag.
%
%  The format of the GetMagickInfo routine is:
%
%      magick_info=GetMagickInfo(tag)
%
%  A description of each parameter follows:
%
%    o magick_info: Method GetMagickInfo returns a pointer MagickInfo
%      structure that matches the specified tag.
%
%    o tag: a character string that represents the image format we are
%      looking for.
%
%
*/
Export const MagickInfo *GetMagickInfo(const char *tag)
{
  register int
    i;

  for (i=0; ImageFormats[i].magick != (char *) NULL; i++)
    if (Latin1Compare(tag,ImageFormats[i].magick) == 0)
      return(&ImageFormats[i]);
  return((const MagickInfo *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagickInfo lists the image formats to a file.
%
%  The format of the ListMagickInfo routine is:
%
%      ListMagickInfo(file)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%
*/
Export void ListMagickInfo(FILE *file)
{
  register int
    i;

  if (file == (FILE *) NULL)
    file=stdout;
  (void) fprintf(file,"\nHere is a list of image formats recognized by "
    "ImageMagick.  Mode 'rw+'\nmeans ImageMagick can read, write, and "
    "save more than one image of a\nsequence to the same file.\n\n");
  (void) fprintf(file,"    Format  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "-----------------\n");
  for (i=0; ImageFormats[i].magick != (char *) NULL; i++)
    (void) fprintf(file,"%10s%c  %c%c%c   %s\n",ImageFormats[i].magick,
      ImageFormats[i].blob_support ? '*' : ' ',
      ImageFormats[i].decoder ? 'r' : '-',ImageFormats[i].encoder ? 'w' : '-',
      ImageFormats[i].encoder && ImageFormats[i].adjoin ? '+' : '-',
      ImageFormats[i].description);
  (void) fprintf(file,"\n* native blob support\n\n");
  (void) fflush(file);
}
