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

/*
  Include declarations.
*/
#include "magick.h"

/*
  Image format declarations.
*/
static MagickInfo
  ImageFormats[] =
  {
    { "JPG", ReadJPEGImage, WriteJPEGImage, True,
      "Joint Photographic Experts Group JFIF format" },
    { "JPEG", ReadJPEGImage, WriteJPEGImage, True,
      "Joint Photographic Experts Group JFIF format" },
    { "GIF", ReadGIFImage, WriteGIFImage, True,
      "CompuServe graphics interchange format" },
    { "PNG", ReadPNGImage, WritePNGImage, False, "Portable Network Graphics" },
    { "PS", ReadPSImage, WritePSImage, True, "Adobe PostScript file" },
    { "TIF", ReadTIFFImage, WriteTIFFImage, True, "Tagged Image File Format" },
    { "BMP", ReadBMPImage, WriteBMPImage, True,
      "Microsoft Windows bitmap image file" },
    { "PBM", ReadPNMImage, WritePNMImage, True,
      "Portable bitmap format (black and white)" },
    { "PGM", ReadPNMImage, WritePNMImage, True,
      "Portable graymap format (gray scale)" },
    { "PNM", ReadPNMImage, WritePNMImage, True, "Portable anymap" },
    { "PPM", ReadPNMImage, WritePNMImage, True,
      "Portable pixmap format (color)" },
    { "EPS", ReadPSImage, WritePSImage, False,
      "Adobe Encapsulated PostScript file" },
    { "TGA", ReadTGAImage, WriteTGAImage, True, "Truevision Targa image file" },
    { "XC", ReadXCImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Constant image of X server color" },
    { "TXT", ReadTEXTImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, "Raw text file" },
    { "XPM", ReadXPMImage, WriteXPMImage, False,
      "X Windows system pixmap file (color)" },
    { "PCD", ReadPCDImage, WritePCDImage, False, "Photo CD" },
    { "XBM", ReadXBMImage, WriteXBMImage, False,
      "X Windows system bitmap (black and white)" },
    { "MIFF", ReadMIFFImage, WriteMIFFImage, True, "Magick image file format" },
    { "AVS", ReadAVSImage, WriteAVSImage, True, "AVS X image file" },
    { "BIE", ReadJBIGImage, WriteJBIGImage, False,
      "Joint Bi-level Image experts Group file interchange format" },
    { "BMP24", ReadBMPImage, WriteBMPImage, True,
      "Microsoft Windows 24-bit bitmap image file" },
    { "CMYK", ReadCMYKImage, WriteCMYKImage, False,
      "Raw cyan, magenta, yellow, and black bytes" },
    { "DCM", ReadDCMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "DICOM Medical image file" },
    { "DCX", ReadPCXImage, WritePCXImage, True,
      "ZSoft IBM PC multi-page Paintbrush file" },
    { "DIB", ReadBMPImage, WriteBMPImage, True,
      "Microsoft Windows bitmap image file" },
    { "EPDF", ReadPDFImage, WritePDFImage, False,
      "Encapsulated Portable Document Format" },
    { "EPI", ReadPSImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Adobe Encapsulated PostScript Interchange format" },
    { "EPS2", ReadPSImage, WritePS2Image, False,
      "Adobe Level II Encapsulated PostScript file" },
    { "EPSF", ReadPSImage, WritePSImage, False,
      "Adobe Encapsulated PostScript file" },
    { "EPSI", ReadPSImage, WritePSImage, False,
      "Adobe Encapsulated PostScript Interchange format" },
    { "EPT", ReadPSImage, WriteEPTImage, False,
      "Adobe Encapsulated PostScript with TIFF preview" },
    { "FAX", ReadFAXImage, WriteFAXImage, True, "Group 3 FAX" },
    { "FITS", ReadFITSImage, WriteFITSImage, False,
      "Flexible Image Transport System" },
    { "FPX", ReadFPXImage, WriteFPXImage, False, "FlashPix Format" },
    { "G3", ReadFAXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Group 3 FAX" },
    { "GIF87", ReadGIFImage, WriteGIFImage, False,
      "CompuServe graphics interchange format (version 87a)" },
    { "GRADATION", ReadGRADATIONImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False,
      "Gradual passing from one shade to another" },
    { "GRANITE", ReadLOGOImage, WriteLOGOImage, False, "Granite texture" },
    { "GRAY", ReadGRAYImage, WriteGRAYImage, True, "Raw gray bytes" },
    { "H", ReadLOGOImage, WriteLOGOImage, False, "Internal format" },
    { "HDF", ReadHDFImage, WriteHDFImage, True, "Hierarchical Data Format" },
    { "HISTOGRAM", ReadHISTOGRAMImage, WriteHISTOGRAMImage, False,
      "Histogram of the image" },
    { "HTM", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False,
      "Hypertext Markup Language with a client-side image map" },
    { "HTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False,
      "Hypertext Markup Language with a client-side image map" },
    { "ICB", ReadTGAImage, WriteTGAImage, True, "Truevision Targa image file" },
    { "ICC", (Image *(*)(const ImageInfo *)) NULL, WriteICCImage, False,
      "ICC Color Profile" },
    { "ICO", ReadICONImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Microsoft icon" },
    { "IMPLICIT", (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True,
      "Internal format" },
    { "JBG", ReadJBIGImage, WriteJBIGImage, True,
      "Joint Bi-level Image experts Group file interchange format" },
    { "JBIG", ReadJBIGImage, WriteJBIGImage, True,
      "Joint Bi-level Image experts Group file interchange format" },
    { "JPEG24", ReadJPEGImage, WriteJPEGImage, True,
      "Joint Photographic Experts Group JFIF format" },
    { "LABEL", ReadLABELImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False,
      "Text image format" },
    { "LOGO", ReadLOGOImage, WriteLOGOImage, False, "ImageMagick Logo" },
    { "MAP", ReadMAPImage, WriteMAPImage, False,
      "Colormap intensities and indices" },
    { "MATTE", ReadMIFFImage, WriteMATTEImage, True, "Matte format" },
    { "MNG", ReadPNGImage, WritePNGImage, True,
      "Multiple-image Network Graphics" },
    { "MONO", ReadMONOImage, WriteMONOImage, False,
      "Bi-level bitmap in least-significant-byte (LSB) first order" },
    { "MTV", ReadMTVImage, WriteMTVImage, True, "MTV Raytracing image format" },
    { "NETSCAPE", ReadLOGOImage, WriteLOGOImage, False,
      "Netscape 216 color cube" },
    { "NULL", ReadNULLImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "NULL image" },
    { "P7", ReadPNMImage, WritePNMImage, True, "Xv thumbnail format" },
    { "PCDS", ReadPCDImage, WritePCDImage, False, "Photo CD" },
    { "PCL", ReadPCLImage, WritePCLImage, False, "Page Control Language" },
    { "PCT", (Image *(*)(const ImageInfo *)) ReadPICTImage, WritePICTImage,
      False, "Apple Macintosh QuickDraw/PICT file" },
    { "PCX", ReadPCXImage, WritePCXImage, False,
      "ZSoft IBM PC Paintbrush file" },
    { "PDF", ReadPDFImage, WritePDFImage, True, "Portable Document Format" },
    { "PIC", ReadPICTImage, WritePICTImage, False,
      "Apple Macintosh QuickDraw/PICT file" },
    { "PICT", ReadPICTImage, WritePICTImage, False,
      "Apple Macintosh QuickDraw/PICT file" },
    { "PIX", ReadPIXImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, "Alias/Wavefront RLE image format" },
    { "PLASMA", ReadPLASMAImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, False,
      "Plasma fractal image" },
    { "PM", ReadXPMImage, WriteXPMImage, False,
      "X Windows system pixmap file (color)" },
    { "PREVIEW", (Image *(*)(const ImageInfo *)) NULL, WritePREVIEWImage, False,
      "Show a preview an image enhancement effect, or f/x" },
    { "PS2", (Image *(*)(const ImageInfo *)) NULL, WritePS2Image, True,
      "Adobe Level II PostScript file" },
    { "PSD", ReadPSDImage, WritePSDImage, False,
      "Adobe Photoshop bitmap file" },
    { "RAS", ReadSUNImage, WriteSUNImage, True, "SUN Rasterfile" },
    { "RGB", ReadRGBImage, WriteRGBImage, True,
      "Raw red, green, and blue bytes" },
    { "RGBA", ReadRGBImage, WriteRGBImage, True,
      "Raw red, green, blue, and matte bytes" },
    { "RLA", ReadRLAImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Alias/Wavefront image file" },
    { "RLE", ReadRLEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "Utah Run length encoded image file" },
    { "SGI", ReadSGIImage, WriteSGIImage, True, "Irix RGB image file" },
    { "SHTML", (Image *(*)(const ImageInfo *)) NULL, WriteHTMLImage, False,
      "Hypertext Markup Language with a client-side image map" },
    { "STEGANO", ReadSTEGANOImage,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, True,
      "Steganographic image" },
    { "SUN", ReadSUNImage, WriteSUNImage, True, "SUN Rasterfile" },
    { "TEXT", ReadTEXTImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, "Raw text file" },
    { "TIFF", ReadTIFFImage, WriteTIFFImage, True, "Tagged Image File Format" },
    { "TIFF24", ReadTIFFImage, WriteTIFFImage, True,
      "24-bit Tagged Image File Format" },
    { "TILE", ReadTILEImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, "Tile image with a texture" },
    { "TIM", ReadTIMImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      True, "PSX TIM file" },
    { "TTF", ReadTTFImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,
      False, "TrueType font file" },
    { "UIL", ReadUILImage, WriteUILImage, False, "X-Motif UIL table" },
    { "UYVY", ReadUYVYImage, WriteUYVYImage, False,
      "16bit/pixel interleaved YUV" },
    { "VDA", ReadTGAImage, WriteTGAImage, True, "Truevision Targa image file" },
    { "VICAR", ReadVICARImage, WriteVICARImage, False,
      "VICAR rasterfile format" },
    { "VID", ReadVIDImage, WriteMIFFImage, False, "Visual Image Directory" },
    { "VIFF", ReadVIFFImage, WriteVIFFImage, True,
      "Khoros Visualization image file" },
    { "VST", ReadTGAImage, WriteTGAImage, True, "Truevision Targa image file" },
    { "X", ReadXImage, WriteXImage, False, "X Image" },
    { "XV", ReadVIFFImage, WriteVIFFImage, True,
      "Khoros Visualization image file" },
    { "XWD", ReadXWDImage, WriteXWDImage, False,
      "X Windows system window dump file (color)" },
    { "YUV", ReadYUVImage, WriteYUVImage, True, "CCIR 601 4:1:1 file" },
    { (char *) NULL, (Image *(*)(const ImageInfo *)) NULL,
      (unsigned int (*)(const ImageInfo *,Image *)) NULL, 0,
      (char *) NULL },
  };

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
Export MagickInfo *GetMagickInfo(const char *tag)
{
  register int
    i;

  for (i=0; ImageFormats[i].magick != (char *) NULL; i++)
    if (Latin1Compare(tag,ImageFormats[i].magick) == 0)
      return(&ImageFormats[i]);
  return((MagickInfo *) NULL);
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
    (void) fprintf(file,"%10s  %c%c%c   %s\n",ImageFormats[i].magick,
      ImageFormats[i].decoder ? 'r' : '-',ImageFormats[i].encoder ? 'w' : '-',
      ImageFormats[i].encoder && ImageFormats[i].adjoin ? '+' : '-',
      ImageFormats[i].description);
}
