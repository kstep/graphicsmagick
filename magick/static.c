/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  SSSSS  TTTTT   AAA   TTTTT  IIIII   CCCC                   %
%                  SS       T    A   A    T      I    C                       %
%                   SSS     T    AAAAA    T      I    C                       %
%                     SS    T    A   A    T      I    C                       %
%                  SSSSS    T    A   A    T    IIIII   CCCC                   %
%                                                                             %
%                                                                             %
%                         ImageMagick Static Methods                          %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated	      %
%  to making software imaging solutions freely available.		      %
%									      %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("Open	      %
%  ImageMagick"), to deal in GraphicsMagick without restriction,	      %
%  including without limitation the rights to use, copy, modify, merge,	      %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,	      %
%  subject to the following conditions:					      %
%									      %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.		      %
%									      %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of		      %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,	      %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick	      %
%  or the use or other dealings in GraphicsMagick.			      %
%									      %
%  Except as contained in this notice, the name of the GraphicsMagick	      %
%  Group shall not be used in advertising or otherwise to promote the	      %
%  sale, use or other dealings in GraphicsMagick without prior written	      %
%  authorization from the GraphicsMagick Group.				      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "module.h"
#include "static.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x e c u t e S t a t i c M o d u l e P r o c e s s                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ExecuteStaticModuleProcess() is just a template method.
%
%  The format of the ExecuteStaticModuleProcess method is:
%
%      unsigned int ExecuteStaticModuleProcess(const char *tag,
%        Image **image,const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o tag: The module tag.
%
%    o image: The image.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
*/
MagickExport unsigned int ExecuteStaticModuleProcess(const char *tag,
  Image **image,const int argc,char **argv)
{
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S t a t i c M o d u l e s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterStaticModules() statically registers all the available module
%  handlers.
%
%  The format of the RegisterStaticModules method is:
%
%      RegisterStaticModules(void)
%
%
*/
MagickExport void RegisterStaticModules(void)
{
#if !defined(BuildMagickModules)
  RegisterARTImage();
  RegisterAVIImage();
  RegisterAVSImage();
  RegisterBMPImage();
  RegisterCAPTIONImage();
  RegisterCLIPBOARDImage();
  RegisterCMYKImage();
  RegisterCUTImage();
  RegisterDCMImage();
  RegisterDIBImage();
  RegisterDPSImage();
  RegisterDPXImage();
  RegisterEMFImage();
  RegisterEPTImage();
  RegisterFAXImage();
  RegisterFITSImage();
  RegisterFPXImage();
  RegisterGIFImage();
  RegisterGRAYImage();
  RegisterGRADIENTImage();
  RegisterHISTOGRAMImage();
  RegisterHTMLImage();
  RegisterICONImage();
  RegisterJBIGImage();
  RegisterJPEGImage();
  RegisterJP2Image();
  RegisterLABELImage();
  RegisterLOCALEImage();
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
  RegisterMSLImage();
  RegisterMTVImage();
  RegisterMVGImage();
  RegisterNULLImage();
  RegisterOTBImage();
  RegisterPALMImage();
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
  RegisterSFWImage();
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
#endif /* !defined(BuildMagickModules) */
}
