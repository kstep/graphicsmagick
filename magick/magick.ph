%{
/*
  Use to generate magick.c via:

   gperf -C -p -a -n -t -N GetMagickInfo -K magick -k'1-4,$' \
     magick.ph > magick.c
*/
#include "magick.h"
%}
struct MagickInfo {
  char
    *magick;

  Image
    *(*decoder) (const ImageInfo *);

  unsigned int
    (*encoder) (const ImageInfo *,Image *),
    adjoin;

  const char
    *description;
};
%%
AVS,		ReadAVSImage,	WriteAVSImage,	True, "AVS X image file"
BIE,		ReadJBIGImage,	WriteJBIGImage,	False, "JBIG image"
BMP,		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows bitmap image file"
BMP24,		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows 24-bit bitmap image file"
CMYK,		ReadCMYKImage,	WriteCMYKImage,	False, "Raw cyan, magenta, yellow, and black bytes"
DCM,		ReadDCMImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "DICOM Medical image file"
DCX,		ReadPCXImage,	WritePCXImage,	True, "ZSoft IBM PC multi-page Paintbrush file"
DIB,		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows bitmap image file"
EPDF,		ReadPDFImage,	WritePDFImage,	False, "Encapsulated Portable Document Format"
EPI,		ReadPSImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Adobe Encapsulated PostScript Interchange format"
EPS,		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript file"
EPS2,		ReadPSImage,	WritePS2Image,	False, "Adobe Level II Encapsulated PostScript file"
EPSF,		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript file"
EPSI,		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript Interchange format"
EPT,		ReadPSImage,	WriteEPTImage,	False, "Adobe Encapsulated PostScript Interchange format with TIFF preview"
FAX,		ReadFAXImage,	WriteFAXImage,	True, "Group 3 FAX"
FITS,		ReadFITSImage,	WriteFITSImage,	False, "Flexible Image Transport System"
FPX,		ReadFPXImage,	WriteFPXImage,	False, "FlashPix Format"
G3,		ReadFAXImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Group 3 FAX"
GIF,		ReadGIFImage,	WriteGIFImage,	True, "CompuServe graphics interchange format"
GIF87,		ReadGIFImage,   WriteGIFImage,	False, "CompuServe graphics interchange format"
GRADATION,	ReadGRADATIONImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,	False, "Gradual passing from one shade to another"
GRANITE,	ReadLOGOImage,	WriteLOGOImage,	False, "Granite texture"
GRAY,		ReadGRAYImage,	WriteGRAYImage,	True, "Raw gray bytes"
H,		ReadLOGOImage,	WriteLOGOImage,	False, ""
HDF,		ReadHDFImage,	WriteHDFImage,	True, "Hierarchical Data Format"
HISTOGRAM,	ReadHISTOGRAMImage, WriteHISTOGRAMImage, False, "Histogram of the image"
HTM,		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"
HTML,		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"
ICB,		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"
ICC,		(Image *(*)(const ImageInfo *)) NULL,		WriteICCImage,	False, "ICC Color Profile"
ICO,		ReadICONImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Microsoft icon"
IMPLICIT,		(Image *(*)(const ImageInfo *)) NULL,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "internal format"
JBG,		ReadJBIGImage,	WriteJBIGImage,	True, "Joint Bi-level Image experts Group file interchange format"
JBIG,		ReadJBIGImage,	WriteJBIGImage,	True, "Joint Bi-level Image experts Group file interchange format"
JPEG,		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"
JPG,		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"
JPEG24,		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"
LABEL,		ReadLABELImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Text image format"
LOGO,		ReadLOGOImage,	WriteLOGOImage,	False, ""
MAP,		ReadMAPImage,	WriteMAPImage,	False, "Colormap intensities and indices"
MIFF,		ReadMIFFImage,	WriteMIFFImage,	True, "Magick image file format"
MNG,		ReadPNGImage,	WritePNGImage,	True, "Multiple-image Network Graphics"
MONO,		ReadMONOImage,	WriteMONOImage,	False, "Bi-level bitmap in least-significant-byte (LSB) first order"
MTV,		ReadMTVImage,	WriteMTVImage,	True, "MTV Raytracing image format"
NETSCAPE,	ReadLOGOImage,	WriteLOGOImage,	False, "Netscape 216 color cube"
NULL,		ReadNULLImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "NULL image"
P7,		ReadPNMImage,	WritePNMImage,	True, "Xv thumbnail format"
PBM,		ReadPNMImage,	WritePNMImage,	True, "Portable bitmap format (black and white)"
PCD,		ReadPCDImage,	WritePCDImage,	False, "Photo CD"
PCDS,		ReadPCDImage,	WritePCDImage,	False, "Photo CD"
PCL,		ReadPCLImage,	WritePCLImage,	False, "Page Control Language"
PCT,		(Image *(*)(const ImageInfo *)) ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"
PCX,		ReadPCXImage,	WritePCXImage,	False, "ZSoft IBM PC Paintbrush file"
PDF,		ReadPDFImage,	WritePDFImage,	True, "Portable Document Format"
PGM,		ReadPNMImage,	WritePNMImage,	True, "Portable graymap format (gray scale)"
PIC,		ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"
PICT,		ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"
PIX,		ReadPIXImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Alias/Wavefront RLE image format"
PLASMA,		ReadPLASMAImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Plasma fractal image"
PM,		ReadXPMImage,	WriteXPMImage,	False, "X Windows system pixmap file (color)"
PNG,		ReadPNGImage,	WritePNGImage,	False, "Portable Network Graphics"
PNM,		ReadPNMImage,	WritePNMImage,	True, "Portable anymap"
PPM,		ReadPNMImage,	WritePNMImage,	True, "Portable pixmap format (color)"
PREVIEW,	(Image *(*)(const ImageInfo *)) NULL,		WritePREVIEWImage, False, "Show a preview an image enhancement effect, or f/x"
PS,		ReadPSImage,	WritePSImage,	True, "Adobe PostScript file"
PS2,		(Image *(*)(const ImageInfo *)) NULL,		WritePS2Image,	True, "Adobe Level II PostScript file"
PSD,		ReadPSDImage,	WritePSDImage,	False, "Adobe Photoshop bitmap file"
RAS,		ReadSUNImage,	WriteSUNImage,	True, "SUN Rasterfile"
RGB,		ReadRGBImage,	WriteRGBImage,	True, "Raw red, green, and blue bytes"
RGBA,		ReadRGBImage,	WriteRGBImage,	True, "Raw red, green, blue, and matte bytes"
RLA,		ReadRLAImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Alias/Wavefront image file"
RLE,		ReadRLEImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Utah Run length encoded image file"
SGI,		ReadSGIImage,	WriteSGIImage,	True, "Irix RGB image file"
SHTML,		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"
STEGANO,	ReadSTEGANOImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Steganographic image"
SUN,		ReadSUNImage,	WriteSUNImage,	True, "SUN Rasterfile"
TEXT,		ReadTEXTImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Raw text file"
TGA,		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"
TIF,		ReadTIFFImage,	WriteTIFFImage,	True, "Tagged Image File Format"
TIFF,		ReadTIFFImage,	WriteTIFFImage,	True, "Tagged Image File Format"
TIFF24,		ReadTIFFImage,	WriteTIFFImage,	True, "24-bit Tagged Image File Format"
TILE,		ReadTILEImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Tile image with a texture"
TIM,		ReadTIMImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "PSX TIM file"
TTF,		ReadTTFImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "TrueType font file"
TXT,		ReadTEXTImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Raw text file"
UIL,		ReadUILImage,	WriteUILImage,	False, "X-Motif UIL table"
UYVY,		ReadUYVYImage,	WriteUYVYImage,	False, "16bit/pixel interleaved YUV"
VDA,		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"
VICAR,		ReadVICARImage,	WriteVICARImage, False, "VICAR rasterfile format"
VID,		ReadVIDImage,	WriteMIFFImage,	False, "Visual Image Directory"
VIFF,		ReadVIFFImage,	WriteVIFFImage,	True, "Khoros Visualization image file"
VST,		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"
XBM,		ReadXBMImage,	WriteXBMImage,	False, "X Windows system bitmap (black and white)"
XC,		ReadXCImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Constant image of X server color"
XPM,		ReadXPMImage,	WriteXPMImage,	False, "X Windows system pixmap file (color)"
XV,		ReadVIFFImage,	WriteVIFFImage,	True, "Khoros Visualization image file"
XWD,		ReadXWDImage,	WriteXWDImage,	False, "X Windows system window dump file (color)"
YUV,		ReadYUVImage,	WriteYUVImage,	True, "CCIR 601 4:1:1 file"
%%
/* Auxillary code */
#ifdef DEBUG
const char
  buffer[] = "XWD";

int main()
{
  struct MagickInfo
    *magick_info;

  magick_info=GetMagickInfo(buffer,strlen(buffer));
  if (magick_info == (const MagickInfo *) NULL)
    printf("Entry %s: not found\n",buffer);
  else
    printf("Entry %s: read=%lx write=%lx description:%s\n",
      magick_info->magick,magick_info->decoder,magick_info->encoder,
      magick_info->description);
  return(0);
}
#endif
