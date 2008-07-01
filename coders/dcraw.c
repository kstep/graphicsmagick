/*
% Copyright (C) 2008 GraphicsMagick Group
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      DDDD    CCCC  RRRR    AAA   W   W                      %
%                      D   D  C      R   R  A   A  W   W                      %
%                      D   D  C      RRRR   AAAAA  W   W                      %
%                      D   D  C      R R    A   A  W W W                      %
%                      DDDD    CCCC  R  R   A   A   W W                       %
%                                                                             %
%                                                                             %
%                          Read DCRAW Image Formats                           %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D C R A W I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDCRAWImage reads a raw digital camera image using the dcraw
%  delegate definition.  This is just a simple proxy to redirect requests
%  for camera raw formats to use dcraw.
%
%  The format of the ReadDCRAWImage method is:
%
%      Image *ReadDCRAWImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDCRAWImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDCRAWImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  clone_info=CloneImageInfo(image_info);
  if (clone_info == (ImageInfo *) NULL)
    return NULL;

  (void) strlcpy(clone_info->filename,"DCRAW:",MaxTextExtent);
  (void) strlcat(clone_info->filename,image_info->filename,MaxTextExtent);
  (void) strlcpy(clone_info->magick,"DCRAW",MaxTextExtent);

  image=ReadImage(clone_info,exception);

  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D C R A W I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDCRAWImage adds attributes for formats supported by 'dcraw'
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  See http://en.wikipedia.org/wiki/RAW_image_format for a description of
%  RAW camera formats.
%
%  The format of the RegisterDCRAWImage method is:
%
%      RegisterDCRAWImage(void)
%
*/
static const struct
{
  const char *id;
  const char *description;
} dcraw_formats[] =
  {
    { "ARW", "Sony Alpha DSLR RAW" }, /* Sony DSLR-A100 */
    { "DNG", "Adobe Digital Negative" }, /* http://www.adobe.com/products/dng/ */
    { "CR2", "Canon Photo RAW" }, /* Canon EOS 400D, etc. */
    { "CRW", "Canon Photo RAW" }, /* Canon EOS-10, etc. */
    { "DCR", "Kodak Photo RAW" }, /* Kodak DSC Pro *, DSC 660C */
    { "KDC", "Kodak Photo RAW" }, /* Kodak DC40/50/120, P850 */
    { "K25", "Kodak Photo RAW" }, /* Kodak DC25 */
    { "MRW", "Minolta Photo Raw" }, /* Minolta DiMAGE*, Dynax *, Maxxum * */
    { "NEF", "Nikon Electronic Format" }, /* Nikon D1, etc. */
    { "ORF", "Olympus Photo RAW" }, /* Olympus C5050Z, etc. */
    { "PEF", "Pentax Electronic File" }, /* Pentax istDS, etc. */
    { "RAF", "Fuji Photo RAW" }, /* Fuji FinePix * */
    { "SRF", "Sony Photo RAW" }, /* Sony DSC-F828, Sony DSC-R1 */
    { "SR2", "Sony Photo RAW" }, /* Sony R1 */
    { "X3F", "Sigma Foveon Photo RAW" }, /* Sigma SD10 */
    { NULL, NULL }
  };
ModuleExport void RegisterDCRAWImage(void)
{
  unsigned int
    i;
  
  MagickInfo
    *entry;
  
  for (i=0; dcraw_formats[i].id != NULL; i++)
    {
      entry=SetMagickInfo(dcraw_formats[i].id);
      entry->decoder=(DecoderHandler) ReadDCRAWImage;
      entry->description=dcraw_formats[i].description;
      entry->extension_treatment=ObeyExtensionTreatment;
      (void) RegisterMagickInfo(entry);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r N U L L I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDCRAWImage removes format registrations made by the
%  DCRAW module from the list of supported formats.
%
%  The format of the UnregisterDCRAWImage method is:
%
%      UnregisterDCRAWImage(void)
%
*/
ModuleExport void UnregisterDCRAWImage(void)
{
  unsigned int
    i;
  
  for (i=0; dcraw_formats[i].id != NULL; i++)
    (void) UnregisterMagickInfo(dcraw_formats[i].id);
}
