/* C code produced by gperf version 2.5 (GNU C++ version) */
/* Command-line: gperf -C -p -a -n -t -N GetMagickInfo -K magick -k1-4,$ magick.ph  */
/*
  Use to generate magick.c via:

   gperf -C -p -a -n -t -N GetMagickInfo -K magick -k'1-4,$' \
     magick.ph > magick.c
*/
#include "magick.h"
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

#define TOTAL_KEYWORDS 99
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 9
#define MIN_HASH_VALUE 18
#define MAX_HASH_VALUE 436
/* maximum key range = 419, duplicates = 0 */

static unsigned int
hash (register const char *str, register int len)
{
  static const unsigned short asso_values[] =
    {
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
      45,  35, 105, 437, 437,  15,  30, 437, 437, 437,
     437, 437, 437, 437, 437,  10,  15, 110,  42,   5,
      35,   7,   9,  55,  32,  10, 107,  37,  35,  75,
      84, 437,  54,  30,   0,  80, 115,  10,  90,  15,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437, 437, 437,
     437, 437, 437, 437, 437, 437, 437, 437,
    };
  register int hval = 0;

  switch (len)
    {
      default:
      case 4:
        hval += asso_values[str[3]];
      case 3:
        hval += asso_values[str[2]];
      case 2:
        hval += asso_values[str[1]];
      case 1:
        hval += asso_values[str[0]];
        break;
    }
  return hval + asso_values[str[len - 1]];
}

const struct MagickInfo *
GetMagickInfo (register const char *str, register int len)
{
  static const struct MagickInfo wordlist[] =
    {
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      
      {"H", 		ReadLOGOImage,	WriteLOGOImage,	False, ""},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"TGA", 		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"JBG", 		ReadJBIGImage,	WriteJBIGImage,	True, "Joint Bi-level Image experts Group file interchange format"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"TTF", 		ReadTTFImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "TrueType font file"},
      {"",}, {"",}, {"",}, {"",}, 
      {"NETSCAPE", 	ReadLOGOImage,	WriteLOGOImage,	False, "Netscape 216 color cube"},
      {"",}, 
      {"G3", 		ReadFAXImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Group 3 FAX"},
      {"",}, {"",}, 
      {"BIE", 		ReadJBIGImage,	WriteJBIGImage,	False, "JBIG image"},
      {"",}, {"",}, 
      {"HTM", 		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"},
      {"",}, {"",}, 
      {"MNG", 		ReadPNGImage,	WritePNGImage,	True, "Multiple-image Network Graphics"},
      {"",}, {"",}, 
      {"EPT", 		ReadPSImage,	WriteEPTImage,	False, "Adobe Encapsulated PostScript Interchange format with TIFF preview"},
      {"TXT", 		ReadTEXTImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Raw text file"},
      {"RGB", 		ReadRGBImage,	WriteRGBImage,	True, "Raw red, green, and blue bytes"},
      {"",}, {"",}, {"",}, 
      {"TEXT", 		ReadTEXTImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Raw text file"},
      {"RGBA", 		ReadRGBImage,	WriteRGBImage,	True, "Raw red, green, blue, and matte bytes"},
      {"",}, {"",}, {"",}, {"",}, 
      {"GRAY", 		ReadGRAYImage,	WriteGRAYImage,	True, "Raw gray bytes"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"GRANITE", 	ReadLOGOImage,	WriteLOGOImage,	False, "Granite texture"},
      {"",}, {"",}, 
      {"P7", 		ReadPNMImage,	WritePNMImage,	True, "Xv thumbnail format"},
      {"",}, 
      {"JBIG", 		ReadJBIGImage,	WriteJBIGImage,	True, "Joint Bi-level Image experts Group file interchange format"},
      {"STEGANO", 	ReadSTEGANOImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Steganographic image"},
      {"",}, {"",}, {"",}, 
      {"HDF", 		ReadHDFImage,	WriteHDFImage,	True, "Hierarchical Data Format"},
      {"",}, {"",}, 
      {"RAS", 		ReadSUNImage,	WriteSUNImage,	True, "SUN Rasterfile"},
      {"TIF", 		ReadTIFFImage,	WriteTIFFImage,	True, "Tagged Image File Format"},
      {"",}, 
      {"DIB", 		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows bitmap image file"},
      {"",}, 
      {"TIM", 		ReadTIMImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "PSX TIM file"},
      {"JPG", 		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"},
      {"HISTOGRAM", 	ReadHISTOGRAMImage, WriteHISTOGRAMImage, False, "Histogram of the image"},
      {"GIF", 		ReadGIFImage,	WriteGIFImage,	True, "CompuServe graphics interchange format"},
      {"PNG", 		ReadPNGImage,	WritePNGImage,	False, "Portable Network Graphics"},
      {"",}, 
      {"JPEG", 		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"GIF87", 		ReadGIFImage,   WriteGIFImage,	False, "CompuServe graphics interchange format"},
      {"",}, 
      {"PS", 		ReadPSImage,	WritePSImage,	True, "Adobe PostScript file"},
      {"VST", 		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"},
      {"",}, 
      {"SGI", 		ReadSGIImage,	WriteSGIImage,	True, "Irix RGB image file"},
      {"GRADATION", 	ReadGRADATIONImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,	False, "Gradual passing from one shade to another"},
      {"EPS", 		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript file"},
      {"FITS", 		ReadFITSImage,	WriteFITSImage,	False, "Flexible Image Transport System"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"PM", 		ReadXPMImage,	WriteXPMImage,	False, "X Windows system pixmap file (color)"},
      {"",}, 
      {"TIFF", 		ReadTIFFImage,	WriteTIFFImage,	True, "Tagged Image File Format"},
      {"",}, {"",}, {"",}, {"",}, 
      {"PGM", 		ReadPNMImage,	WritePNMImage,	True, "Portable graymap format (gray scale)"},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"RLE", 		ReadRLEImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Utah Run length encoded image file"},
      {"TILE", 		ReadTILEImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Tile image with a texture"},
      {"PBM", 		ReadPNMImage,	WritePNMImage,	True, "Portable bitmap format (black and white)"},
      {"",}, {"",}, {"",}, 
      {"VDA", 		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"},
      {"",}, 
      {"XBM", 		ReadXBMImage,	WriteXBMImage,	False, "X Windows system bitmap (black and white)"},
      {"SUN", 		ReadSUNImage,	WriteSUNImage,	True, "SUN Rasterfile"},
      {"RLA", 		ReadRLAImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Alias/Wavefront image file"},
      {"CMYK", 		ReadCMYKImage,	WriteCMYKImage,	False, "Raw cyan, magenta, yellow, and black bytes"},
      {"SHTML", 		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"},
      {"XWD", 		ReadXWDImage,	WriteXWDImage,	False, "X Windows system window dump file (color)"},
      {"AVS", 		ReadAVSImage,	WriteAVSImage,	True, "AVS X image file"},
      {"",}, {"",}, {"",}, 
      {"EPSF", 		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript file"},
      {"",}, {"",}, {"",}, 
      {"PNM", 		ReadPNMImage,	WritePNMImage,	True, "Portable anymap"},
      {"PCT", 		(Image *(*)(const ImageInfo *)) ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"},
      {"ICB", 		ReadTGAImage,	WriteTGAImage,	True, "Truevision Targa image file"},
      {"PDF", 		ReadPDFImage,	WritePDFImage,	True, "Portable Document Format"},
      {"MIFF", 		ReadMIFFImage,	WriteMIFFImage,	True, "Magick image file format"},
      {"PSD", 		ReadPSDImage,	WritePSDImage,	False, "Adobe Photoshop bitmap file"},
      {"EPI", 		ReadPSImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Adobe Encapsulated PostScript Interchange format"},
      {"",}, 
      {"EPDF", 		ReadPDFImage,	WritePDFImage,	False, "Encapsulated Portable Document Format"},
      {"",}, {"",}, 
      {"PS2", 		(Image *(*)(const ImageInfo *)) NULL,		WritePS2Image,	True, "Adobe Level II PostScript file"},
      {"",}, {"",}, {"",}, {"",}, 
      {"EPS2", 		ReadPSImage,	WritePS2Image,	False, "Adobe Level II Encapsulated PostScript file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"MAP", 		ReadMAPImage,	WriteMAPImage,	False, "Colormap intensities and indices"},
      {"",}, {"",}, {"",}, {"",}, 
      {"BMP", 		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows bitmap image file"},
      {"",}, {"",}, {"",}, {"",}, 
      {"FAX", 		ReadFAXImage,	WriteFAXImage,	True, "Group 3 FAX"},
      {"DCM", 		ReadDCMImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "DICOM Medical image file"},
      {"",}, {"",}, 
      {"EPSI", 		ReadPSImage,	WritePSImage,	False, "Adobe Encapsulated PostScript Interchange format"},
      {"TIFF24", 		ReadTIFFImage,	WriteTIFFImage,	True, "24-bit Tagged Image File Format"},
      {"",}, {"",}, 
      {"JPEG24", 		ReadJPEGImage,	WriteJPEGImage,	True, "Joint Photographic Experts Group JFIF format"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"UYVY", 		ReadUYVYImage,	WriteUYVYImage,	False, "16bit/pixel interleaved YUV"},
      {"PLASMA", 		ReadPLASMAImage, (unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Plasma fractal image"},
      {"PPM", 		ReadPNMImage,	WritePNMImage,	True, "Portable pixmap format (color)"},
      {"",}, 
      {"LABEL", 		ReadLABELImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Text image format"},
      {"",}, {"",}, {"",}, 
      {"XPM", 		ReadXPMImage,	WriteXPMImage,	False, "X Windows system pixmap file (color)"},
      {"PICT", 		ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"},
      {"",}, {"",}, {"",}, {"",}, 
      {"VID", 		ReadVIDImage,	WriteMIFFImage,	False, "Visual Image Directory"},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"HTML", 		(Image *(*)(const ImageInfo *)) NULL,		WriteHTMLImage,	False, "Hypertext Markup Language with a client-side image map"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"MTV", 		ReadMTVImage,	WriteMTVImage,	True, "MTV Raytracing image format"},
      {"PREVIEW", 	(Image *(*)(const ImageInfo *)) NULL,		WritePREVIEWImage, False, "Show a preview an image enhancement effect, or f/x"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"VIFF", 		ReadVIFFImage,	WriteVIFFImage,	True, "Khoros Visualization image file"},
      {"",}, {"",}, 
      {"PCD", 		ReadPCDImage,	WritePCDImage,	False, "Photo CD"},
      {"",}, {"",}, {"",}, {"",}, 
      {"IMPLICIT", 		(Image *(*)(const ImageInfo *)) NULL,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "internal format"},
      {"",}, {"",}, 
      {"BMP24", 		ReadBMPImage,	WriteBMPImage,	True, "Microsoft Windows 24-bit bitmap image file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"PCDS", 		ReadPCDImage,	WritePCDImage,	False, "Photo CD"},
      {"MONO", 		ReadMONOImage,	WriteMONOImage,	False, "Bi-level bitmap in least-significant-byte (LSB) first order"},
      {"",}, 
      {"FPX", 		ReadFPXImage,	WriteFPXImage,	False, "FlashPix Format"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, 
      {"XC", 		ReadXCImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Constant image of X server color"},
      {"",}, {"",}, {"",}, {"",}, 
      {"ICO", 		ReadICONImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "Microsoft icon"},
      {"",}, {"",}, {"",}, 
      {"PIX", 		ReadPIXImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		True, "Alias/Wavefront RLE image format"},
      {"XV", 		ReadVIFFImage,	WriteVIFFImage,	True, "Khoros Visualization image file"},
      {"",}, {"",}, {"",}, {"",}, 
      {"YUV", 		ReadYUVImage,	WriteYUVImage,	True, "CCIR 601 4:1:1 file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"DCX", 		ReadPCXImage,	WritePCXImage,	True, "ZSoft IBM PC multi-page Paintbrush file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"LOGO", 		ReadLOGOImage,	WriteLOGOImage,	False, ""},
      {"",}, {"",}, {"",}, {"",}, 
      {"VICAR", 		ReadVICARImage,	WriteVICARImage, False, "VICAR rasterfile format"},
      {"",}, {"",}, {"",}, {"",}, 
      {"UIL", 		ReadUILImage,	WriteUILImage,	False, "X-Motif UIL table"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"PIC", 		ReadPICTImage,	WritePICTImage,	False, "Apple Macintosh QuickDraw/PICT file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"PCX", 		ReadPCXImage,	WritePCXImage,	False, "ZSoft IBM PC Paintbrush file"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, 
      {"ICC", 		(Image *(*)(const ImageInfo *)) NULL,		WriteICCImage,	False, "ICC Color Profile"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, 
      {"PCL", 		ReadPCLImage,	WritePCLImage,	False, "Page Control Language"},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      
      {"NULL", 		ReadNULLImage,	(unsigned int (*)(const ImageInfo *,Image *)) NULL,		False, "NULL image"},
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].magick;

          if (*s == *str && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
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
