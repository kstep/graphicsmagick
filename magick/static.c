/* warning - this source code file is auto generated -- do not edit */
#include "magick.h"
#include "define.h"

void OpenStaticModules(void)
{
#if !defined(HasMODULES)
  RegisterARTImage();
  RegisterAVIImage();
  RegisterAVSImage();
  RegisterBMPImage();
  RegisterCMYKImage();
  RegisterCUTImage();
  RegisterDCMImage();
  RegisterDIBImage();
  RegisterDPXImage();
  RegisterEPTImage();
  RegisterFAXImage();
  RegisterFITSImage();
  RegisterFPXImage();
  RegisterGIFImage();
  RegisterGRAYImage();
  RegisterGRADIENTImage();
  RegisterHDFImage();
  RegisterHISTOGRAMImage();
  RegisterHTMLImage();
  RegisterICONImage();
  RegisterJBIGImage();
  RegisterJPEGImage();
  RegisterJP2Image();
  RegisterLABELImage();
  RegisterLOGOImage();
  RegisterMAPImage();
  RegisterMATImage();
  RegisterMATTEImage();
  RegisterMETAImage();
  RegisterMIFFImage();
  RegisterMONOImage();
  RegisterMPCImage();
  RegisterMPEGImage();
  RegisterMPRImage();
  RegisterMTVImage();
  RegisterMVGImage();
  RegisterNULLImage();
  RegisterOTBImage();
  RegisterPCDImage();
  RegisterPCLImage();
  RegisterPCXImage();
  RegisterPDBImage();
  RegisterPDFImage();
  RegisterPICTImage();
  RegisterPIXImage();
  RegisterPLASMAImage();
  RegisterPNGImage();
  RegisterPNMImage();
  RegisterPREVIEWImage();
  RegisterPSImage();
  RegisterPS2Image();
  RegisterPS3Image();
  RegisterPSDImage();
  RegisterPWPImage();
  RegisterRGBImage();
  RegisterRLAImage();
  RegisterRLEImage();
  RegisterSCTImage();
  RegisterSGIImage();
  RegisterSTEGANOImage();
  RegisterSUNImage();
  RegisterSVGImage();
  RegisterTGAImage();
  RegisterTIFFImage();
  RegisterTILEImage();
  RegisterTIMImage();
  RegisterTTFImage();
  RegisterTXTImage();
  RegisterUILImage();
  RegisterURLImage();
  RegisterUYVYImage();
  RegisterVICARImage();
  RegisterVIDImage();
  RegisterVIFFImage();
  RegisterWBMPImage();
  RegisterWMFImage();
  RegisterWPGImage();
  RegisterXImage();
  RegisterXBMImage();
  RegisterXCImage();
  RegisterXCFImage();
  RegisterXPMImage();
  RegisterXWDImage();
  RegisterYUVImage();
#endif
}

#if !defined(HasMODULES)
extern unsigned int mslImage(Image *image,
  const int argc,char **argv);
#endif

unsigned int ExecuteStaticModuleProcess(const char *tag,
    Image *image,const int argc,char **argv)
{
#if !defined(HasMODULES)
  if (LocaleCompare("msl",tag) == 0)
    return mslImage(image,argc,argv);
#endif
  return(False);
}

