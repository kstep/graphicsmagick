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
%  copy of this software and associated documentations ("ImageMagick"),  %
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
    { "AVS", ReadAVSImage, WriteAVSImage, True, True, "AVS X image" },
#if defined(HasJBIG)
    { "BIE", ReadJBIGImage, WriteJBIGImage, False, False,
      "Joint Bi-level Image experts Group interchange format" },
#endif
    { "BMP", ReadBMPImage, WriteBMPImage, True, True,
      "Microsoft Windows bitmap image" },
    { "BMP24", ReadBMPImage, WriteBMPImage, True, True,
      "Microsoft Windows 24-bit bitmap image" },
    { "CMYK", ReadCMYKImage, WriteCMYKImage, False, True,
      "Raw cyan, magenta, yellow, and black bytes" },
    { "DCM", ReadDCMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Digital Imaging and Communications in Medicine image" },
    { "DCX", ReadPCXImage, WritePCXImage, True, True,
      "ZSoft IBM PC multi-page Paintbrush" },
    { "DIB", ReadBMPImage, WriteBMPImage, True, True,
      "Microsoft Windows bitmap image" },
    { "EPDF", ReadPDFImage, WritePDFImage, False, True,
      "Encapsulated Portable Document Format" },
    { "EPI", ReadPSImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Adobe Encapsulated PostScript Interchange format" },
    { "EPS", ReadPSImage, WritePSImage, False, True,
      "Adobe Encapsulated PostScript" },
    { "EPS2", ReadPSImage, WritePS2Image, False, True,
      "Adobe Level II Encapsulated PostScript" },
    { "EPSF", ReadPSImage, WritePSImage, False, True,
      "Adobe Encapsulated PostScript" },
    { "EPSI", ReadPSImage, WritePSImage, False, True,
      "Adobe Encapsulated PostScript Interchange format" },
    { "EPT", ReadPSImage, WriteEPTImage, False, True,
      "Adobe Encapsulated PostScript with TIFF preview" },
    { "FAX", ReadFAXImage, WriteFAXImage, True, True, "Group 3 FAX" },
    { "FITS", ReadFITSImage, WriteFITSImage, False, True,
      "Flexible Image Transport System" },
#if defined(HasFPX)
    { "FPX", ReadFPXImage, WriteFPXImage, False, True, "FlashPix Format" },
#endif
    { "G3", ReadFAXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Group 3 FAX" },
    { "GIF", ReadGIFImage, WriteGIFImage, True, True,
      "CompuServe graphics interchange format" },
    { "GIF87", ReadGIFImage, WriteGIFImage, False, True,
      "CompuServe graphics interchange format (version 87a)" },
    { "GRADATION", ReadGRADATIONImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, True,
      "Gradual passing from one shade to another" },
    { "GRANITE", ReadLOGOImage, WriteLOGOImage, False, True,
      "Granite texture" },
    { "GRAY", ReadGRAYImage, WriteGRAYImage, True, True, "Raw gray bytes" },
    { "H", ReadLOGOImage, WriteLOGOImage, False, True, "Internal format" },
    { "HDF", ReadHDFImage, WriteHDFImage, True, True,
      "Hierarchical Data Format" },
    { "HISTOGRAM", ReadHISTOGRAMImage, WriteHISTOGRAMImage, False, True,
      "Histogram of the image" },
    { "HTM", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False, True,
      "Hypertext Markup Language with a client-side image map" },
    { "HTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False,
      False, "Hypertext Markup Language with a client-side image map" },
    { "ICB", ReadTGAImage, WriteTGAImage, True, True,
      "Truevision Targa image" },
    { "ICC", ReadICCImage, WriteICCImage, False, True, "ICC Color Profile" },
    { "ICO", ReadICONImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Microsoft icon" },
    { "IMPLICIT", (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True, True,
      "Internal format" },
    { "IPTC", ReadIPTCImage, WriteIPTCImage, False, True, "IPTC Newsphoto" },
#if defined(HasJBIG)
    { "JBG", ReadJBIGImage, WriteJBIGImage, True, False,
      "Joint Bi-level Image experts Group interchange format" },
    { "JBIG", ReadJBIGImage, WriteJBIGImage, True, False,
      "Joint Bi-level Image experts Group interchange format" },
#endif
#if defined(HasJPEG)
    { "JPG", ReadJPEGImage, WriteJPEGImage, False, True,
      "Joint Photographic Experts Group JFIF format" },
    { "JPEG", ReadJPEGImage, WriteJPEGImage, False, True,
      "Joint Photographic Experts Group JFIF format" },
    { "JPEG24", ReadJPEGImage, WriteJPEGImage, False, True,
      "Joint Photographic Experts Group JFIF format" },
#endif
    { "LABEL", ReadLABELImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, True,
      "Text image format" },
    { "LOGO", ReadLOGOImage, WriteLOGOImage, False, True, "ImageMagick Logo" },
    { "MAP", ReadMAPImage, WriteMAPImage, False, True,
      "Colormap intensities and indices" },
    { "MATTE", ReadMIFFImage, WriteMATTEImage, True, True, "Matte format" },
    { "MIFF", ReadMIFFImage, WriteMIFFImage, True, True,
      "Magick image format" },
#if defined(HasPNG)
    { "MNG", ReadPNGImage, WritePNGImage, True, True,
      "Multiple-image Network Graphics" },
#endif
    { "MONO", ReadMONOImage, WriteMONOImage, False, True,
      "Bi-level bitmap in least-significant-byte (LSB) first order" },
    { "MTV", ReadMTVImage, WriteMTVImage, True, True,
      "MTV Raytracing image format" },
    { "NETSCAPE", ReadLOGOImage, WriteLOGOImage, False, True,
      "Netscape 216 color cube" },
    { "NULL", ReadNULLImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "NULL image" },
    { "P7", ReadPNMImage, WritePNMImage, True, True, "Xv thumbnail format" },
    { "PBM", ReadPNMImage, WritePNMImage, True, True,
      "Portable bitmap format (black and white)" },
    { "PCD", ReadPCDImage, WritePCDImage, False, True, "Photo CD" },
    { "PCDS", ReadPCDImage, WritePCDImage, False, True, "Photo CD" },
    { "PCL", ReadPCLImage, WritePCLImage, False, True,
      "Page Control Language" },
    { "PCT", (Image *(*)(const ImageInfo *)) ReadPICTImage, WritePICTImage,
      False, True, "Apple Macintosh QuickDraw/PICT" },
    { "PCX", ReadPCXImage, WritePCXImage, False, True,
      "ZSoft IBM PC Paintbrush" },
    { "PDF", ReadPDFImage, WritePDFImage, True, True,
      "Portable Document Format" },
    { "PIC", ReadPICTImage, WritePICTImage, False, True,
      "Apple Macintosh QuickDraw/PICT" },
    { "PICT", ReadPICTImage, WritePICTImage, False, True,
      "Apple Macintosh QuickDraw/PICT" },
    { "PIX", ReadPIXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, True, "Alias/Wavefront RLE image format" },
    { "PLASMA", ReadPLASMAImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, True,
      "Plasma fractal image" },
    { "PGM", ReadPNMImage, WritePNMImage, True, True,
      "Portable graymap format (gray scale)" },
    { "PM", ReadXPMImage, WriteXPMImage, False, True,
      "X Windows system pixmap (color)" },
#if defined(HasPNG)
    { "PNG", ReadPNGImage, WritePNGImage, False, True,
      "Portable Network Graphics" },
#endif
    { "PNM", ReadPNMImage, WritePNMImage, True, True, "Portable anymap" },
    { "PPM", ReadPNMImage, WritePNMImage, True, True,
      "Portable pixmap format (color)" },
    { "PREVIEW", (Image *(*)(const ImageInfo *)) NULL, WritePREVIEWImage,
      False, True, "Show a preview an image enhancement effect, or f/x" },
    { "PS", ReadPSImage, WritePSImage, True, True, "Adobe PostScript" },
    { "PS2", (Image *(*)(const ImageInfo *)) NULL, WritePS2Image, True, True,
      "Adobe Level II PostScript" },
    { "PS3", (Image *(*)(const ImageInfo *)) NULL, WritePS3Image, True, True,
      "Adobe Level III PostScript" },
    { "PSD", ReadPSDImage, WritePSDImage, False, True,
      "Adobe Photoshop bitmap" },
#if defined(HasTIFF)
    { "PTIF", ReadTIFFImage, WriteTIFFImage, True, False,
      "Pyramid encoded TIFF" },
#endif
    { "PWP", ReadPWPImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, True, "Seattle Film Works" },
    { "RAS", ReadSUNImage, WriteSUNImage, True, True, "SUN Rasterfile" },
    { "RGB", ReadRGBImage, WriteRGBImage, True, True,
      "Raw red, green, and blue bytes" },
    { "RGBA", ReadRGBImage, WriteRGBImage, True, True,
      "Raw red, green, blue, and matte bytes" },
    { "RLA", ReadRLAImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Alias/Wavefront image" },
    { "RLE", ReadRLEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Utah Run length encoded image" },
    { "SCT", ReadSCTImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Scitex HandShake" },
    { "SFW", ReadSFWImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Seattle Film Works" },
    { "SGI", ReadSGIImage, WriteSGIImage, True, True, "Irix RGB image" },
    { "SHTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False,
      False, "Hypertext Markup Language with a client-side image map" },
    { "STEGANO", ReadSTEGANOImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True, True,
      "Steganographic image" },
    { "SUN", ReadSUNImage, WriteSUNImage, True, True, "SUN Rasterfile" },
    { "TEXT", ReadTXTImage, WriteTXTImage, True, True, "Raw text" },
    { "TGA", ReadTGAImage, WriteTGAImage, True, True,
      "Truevision Targa image" },
#if defined(HasTIFF)
    { "TIF", ReadTIFFImage, WriteTIFFImage, True, True,
      "Tagged Image File Format" },
    { "TIFF", ReadTIFFImage, WriteTIFFImage, True, True,
      "Tagged Image File Format" },
    { "TIFF24", ReadTIFFImage, WriteTIFFImage, True, True,
      "24-bit Tagged Image File Format" },
#endif
    { "TILE", ReadTILEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, True, "Tile image with a texture" },
    { "TIM", ReadTIMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, True, "PSX TIM" },
#if defined(HasTTF)
    { "TTF", ReadTTFImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "True, TrueType font" },
#endif
    { "TXT", ReadTXTImage, WriteTXTImage, True, True, "Raw text" },
    { "UIL", ReadUILImage, WriteUILImage, False, True, "X-Motif UIL table" },
    { "UYVY", ReadUYVYImage, WriteUYVYImage, False, True,
      "16bit/pixel interleaved YUV" },
    { "VDA", ReadTGAImage, WriteTGAImage, True, True,
      "Truevision Targa image" },
    { "VICAR", ReadVICARImage, WriteVICARImage, False, True,
      "VICAR rasterfile format" },
    { "VID", ReadVIDImage, WriteMIFFImage, True, True,
      "Visual Image Directory" },
    { "VIFF", ReadVIFFImage, WriteVIFFImage, True, True,
      "Khoros Visualization image" },
    { "VST", ReadTGAImage, WriteTGAImage, True, True,
      "Truevision Targa image" },
#if defined(HasX11)
    { "X", ReadXImage, WriteXImage, False, True, "X Image" },
#endif
    { "XBM", ReadXBMImage, WriteXBMImage, False, True,
      "X Windows system bitmap (black and white)" },
    { "XC", ReadXCImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, True, "Constant image of X server color" },
    { "XPM", ReadXPMImage, WriteXPMImage, False, True,
      "X Windows system pixmap (color)" },
    { "XV", ReadVIFFImage, WriteVIFFImage, True, True,
      "Khoros Visualization image" },
#if defined(HasX11)
    { "XWD", ReadXWDImage, WriteXWDImage, False, True,
      "X Windows system window dump (color)" },
#endif
    { "YUV", ReadYUVImage, WriteYUVImage, False, True, "CCIR 601 4:1:1" },
    { (char *) NULL, (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False, True,
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
%  Method ListMagickInfo lists the image formats to a.
%
%  The format of the ListMagickInfo routine is:
%
%      ListMagickInfo(file)
%
%  A description of each parameter follows.
%
%    o:  An pointer to a FILE.
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
    "save more than one image of a\nsequence to the same blob or file.\n\n");
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
