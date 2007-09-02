/*
% Copyright (C) 2003, 2004, 2005 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              PPPP   RRRR    OOO   FFFFF  IIIII  L      EEEEE                %
%              P   P  R   R  O   O  F        I    L      E                    %
%              PPPP   RRRR   O   O  FFF      I    L      EEE                  %
%              P      R R    O   O  F        I    L      E                    %
%              P      R  R    OOO   F      IIIII  LLLLL  EEEEE                %
%                                                                             %
%                   GraphicsMagick Image Profile Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               Bill Radcliffe                                %
%                              Bob Friesenhahn                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/map.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/resize.h"
#include "magick/transform.h"
#include "magick/quantize.h"
#include "magick/utility.h"
#include "magick/log.h"
#include "magick/profile.h"
#if defined(HasLCMS)
#if defined(HAVE_LCMS_LCMS_H)
#include <lcms/lcms.h>
#else
#include "lcms.h"
#endif
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     A l l o c a t e I m a g e P r o f i l e I t e r a t o r                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageProfileIterator allocates an iterator to traverse the
%  image profile list.
%
%  The format of the AllocateImageProfileIterator method is:
%
%      ImageProfileIterator AllocateImageProfileIterator(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
*/
MagickExport ImageProfileIterator
AllocateImageProfileIterator(const Image *image)
{
  if (!image->profiles)
    return 0;

  return (ImageProfileIterator) MagickMapAllocateIterator(image->profiles);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     D e a l l o c a t e I m a g e P r o f i l e I t e r a t o r             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeallocateImageProfileIterator deallocates an image profile iterator.
%
%  The format of the DeallocateImageProfileIterator method is:
%
%      void DeallocateImageProfileIterator(ImageProfileIterator profile_iterator)
%
%  A description of each parameter follows:
%
%    o profile_iterator: Profile iterator to deallocate.
%
*/
MagickExport void
DeallocateImageProfileIterator(ImageProfileIterator profile_iterator)
{
  if (profile_iterator != 0)
    MagickMapDeallocateIterator((MagickMapIterator) profile_iterator);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     D e l e t e I m a g e P r o f i l e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageProfile removes a named profile from the image.
%
%  The format of the DeleteImageProfile method is:
%
%      unsigned int DeleteImageProfile(Image *image,const char *name)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", & "IPTC" or a
%                          generic profile name.
%
*/
MagickExport MagickPassFail
DeleteImageProfile(Image *image,const char *name)
{
  return(SetImageProfile(image,name,0,0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     G e t I m a g e P r o f i l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageProfile returns a pointer to the named image profile if it is
%  present. A null pointer is returned if the named profile is not present.
%
%  Older versions of this function stored profiles named "8BIM" and "IPTC"
%  in the same storage location.  This is no longer the case.  However,
%  GetImageProfile() will try the alternate name if the specifically
%  requested profile name is not available.
%
%  The format of the GetImageProfile method is:
%
%      const unsigned char *GetImageProfile(const Image* image,
%                             const char *name, size_t *length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", "IPTC", "XMP" or any
%                          unique text string.
%
%    o length: Updated with profile length if profile is present.  Set to NULL
%              if length is not needed.
%
*/
MagickExport const unsigned char *
GetImageProfile(const Image* image, const char *name, size_t *length)
{
  const unsigned char
    *profile = 0;

  size_t
    profile_length=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);

  if (length)
    *length=0;

  if (!image->profiles)
    return 0;

  profile=MagickMapAccessEntry(image->profiles,name,&profile_length);

  if (!profile)
    {
      /*
        Support common alias names and work-alikes.
      */
      if (LocaleCompare("ICC",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"ICM",&profile_length);
      else if (LocaleCompare("ICM",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"ICC",&profile_length);
      else if (LocaleCompare("IPTC",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"8BIM",&profile_length);
      else if (LocaleCompare("8BIM",name) == 0)
        profile=MagickMapAccessEntry(image->profiles,"IPTC",&profile_length);
    }

  if (length)
    *length=profile_length;

  return profile;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     N e x t I m a g e P r o f i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NextImageProfile iterates forward to the next image profile. The profile
%  name is returned along with the profile data, and length.  If there are
%  no more entries in the list, then MagickFail is returned.
%
%  The format of the AllocateImageProfileIterator method is:
%
%      MagickPassFail NextImageProfile(ImageProfileIterator profile_iterator,
%                             const char **name, const unsigned char **profile,
%                             size_t *length)
%
%  A description of each parameter follows:
%
%    o profile_iterator: Profile iterator.
%
%    o name: Address of pointer to update with address of name.
%
%    o profile: Address of pointer to update with location of profile data.
%
%    o length: Address of parameter to update with profile length.
%
*/
MagickExport MagickPassFail
NextImageProfile(ImageProfileIterator profile_iterator,
                 const char **name,
                 const unsigned char **profile,
                 size_t *length)
{
  MagickMapIterator
    map_iterator;

  assert(name != (const char **) NULL);
  assert(length != (size_t *) NULL);

  if (profile_iterator == 0)
    return (MagickFail);

  MagickPassFail
    status;

  map_iterator=(MagickMapIterator) profile_iterator;
  status=MagickMapIterateNext(map_iterator,name);
  if (status != MagickFail)
    *profile=MagickMapDereferenceIterator(map_iterator,length);
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r o f i l e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ProfileImage() adds, applies, or removes a ICM, IPTC, or generic profile
%  from an image.  If the profile is NULL, it is removed from the image
%  otherwise added (or applied).  Use a name of '*' and a profile of NULL to
%  remove all profiles from the image. Ownership of the profile is
%  transferred to GraphicsMagick (it should not be altered or deallocated)
%  unless the clone option is set to True.
%
%  ICC ICM profiles are a special case and are handled as follows:
%
%  If there is no ICM profile currently associated with the image, then
%  the profile is simply associated with the image and the image pixels
%  are not altered.
%  
%  If there is already a ICM profile associated with the image, then
%  the colorspace transform described by the existing and new profiles
%  is applied to the image pixels, and the new profile is associated
%  with the image.
%
%  The format of the ProfileImage method is:
%
%      unsigned int ProfileImage(Image *image,const char *name,
%        const unsigned char *profile,const size_t length,unsigned int clone)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Name of profile to add or remove: ICM, IPTC, or generic profile.
%
%    o profile: The profile.
%
%    o length: The length of the profile.
%
%    o clone: If set True, then copy the profile rather than taking
%             ownership of it.
%
%
*/
#if defined(HasLCMS)
#if defined(LCMS_VERSION) && (LCMS_VERSION > 1010)
static int
lcmsReplacementErrorHandler(int ErrorCode, const char *ErrorText)
{
  ExceptionType
    type;

  switch(ErrorCode)
  {
    case LCMS_ERRC_ABORTED:
      type=TransformError;
      break;
    default:
    case LCMS_ERRC_RECOVERABLE:
    case LCMS_ERRC_WARNING:
      type=TransformWarning;
      break;
  }
  (void) LogMagickEvent(type,GetMagickModule(),"lcms: #%d, %s",
    ErrorCode,(ErrorText != (char *) NULL) ? ErrorText : "No error text");
  return 1; /* tells lcms that we handled the problem */
}
#endif
#endif

MagickExport MagickPassFail
ProfileImage(Image *image,const char *name,unsigned char *profile,
             const size_t length,unsigned int clone)
{
  register long
    i,
    j;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (name == (const char *) NULL)
    ThrowBinaryException3(OptionError,NoProfileNameWasGiven,
      UnableToAddOrRemoveProfile);
  if ((profile == (const unsigned char *) NULL) || (length == 0))
    {
      /*
        Remove an ICM, IPTC, or generic profile from the image.
      */
      const char
        *profile_name;
      
      size_t
        profile_length;
      
      const unsigned char *
        profile_info;
      
      ImageProfileIterator
        profile_iterator;

      profile_iterator=AllocateImageProfileIterator(image);
      while(NextImageProfile(profile_iterator,&profile_name,&profile_info,
                             &profile_length) != MagickFail)
        {
          /* FIXME, this might just crash */
          if (GlobExpression(profile_name,name))
            {
              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                    "Removing %s profile",profile_name);
              DeleteImageProfile(image,profile_name);
            }
        }
      DeallocateImageProfileIterator(profile_iterator);
      return(MagickPass);
    }
  /*
    Add a ICM, IPTC, or generic profile to the image.
  */
  if ((LocaleCompare("8bim",name) == 0) || (LocaleCompare("iptc",name) == 0))
    {
      if (clone)
        {
          SetImageProfile(image,name,profile,length);
        }
      else
        {
          SetImageProfile(image,name,profile,length);
          MagickFreeMemory(profile);
        }
      return(MagickPass);
    }
  if (LocaleCompare("icm",name) == 0)
    {
      const unsigned char
        *existing_profile;

      size_t
        existing_profile_length=0;

      /* Check for identical input and output profiles. Return on identity. */
      existing_profile=GetImageProfile(image,"ICM",&existing_profile_length);

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Profile1: %lu bytes, Profile2: %lu bytes",
                            (unsigned long) length,
                            (unsigned long) existing_profile_length);

      if ((length != 0) && (length == existing_profile_length) &&
          (memcmp(existing_profile,profile,length) == 0))
        {
          return(MagickPass);
        }

      /* Convert to new colors if we have both an old and a new profile. */
      if ((existing_profile_length != 0) && (length != 0))
        {
#if defined(HasLCMS)

          typedef struct _ProfilePacket
          {
            unsigned short
              red,
              green,
              blue,
              black;
          } ProfilePacket;

          ColorspaceType
            source_colorspace,
            target_colorspace;

          cmsHPROFILE
            source_profile,
            target_profile;

          DWORD
            source_type,
            target_type;

          cmsHTRANSFORM
            transform;

          IndexPacket
            *indexes;

          int
            intent;

          unsigned int
            transform_colormap;

          long
            y;

          ProfilePacket
            alpha,
            beta;

          register long
            x;

          register PixelPacket
            *q;

          /*
            Transform pixel colors as defined by the color profiles.
          */
#if defined(LCMS_VERSION) && (LCMS_VERSION > 1010)
          cmsSetErrorHandler(lcmsReplacementErrorHandler);
#else
          (void) cmsErrorAction(LCMS_ERROR_SHOW);
#endif
          source_profile=cmsOpenProfileFromMem((unsigned char *) existing_profile,
            existing_profile_length);
          target_profile=cmsOpenProfileFromMem((unsigned char *) profile,
            length);
          if ((source_profile == (cmsHPROFILE) NULL) ||
              (target_profile == (cmsHPROFILE) NULL))
            ThrowBinaryException3(ResourceLimitError,UnableToManageColor,
              UnableToOpenColorProfile);

          /*
            TODO: This may be optimized to use PixelPackets instead
            of moving PixelPacket components to and from ProfilePackets.
            The transform types below, then, must match #ifdef's in the
            PixelPacket struct definition and should be optimized
            based on Quantum size. Some (if not all?) YCbCr and LUV
            profiles are (TIFF) scanline oriented, so transforming
            one pixel at a time does not work for those profiles.

            Notice that the performance penalty of transforming only
            one pixel at a time is very small and probably not worth
            optimizing.
          */
          switch (cmsGetColorSpace(source_profile))
          {
            case icSigCmykData:
            {
              source_colorspace=CMYKColorspace;
              source_type=TYPE_CMYK_16;
              break;
            }
            case icSigYCbCrData:
            {
              source_colorspace=YCbCrColorspace;
              source_type=TYPE_YCbCr_16;
              break;
            }
            case icSigLuvData:
            {
              source_colorspace=YUVColorspace;
              source_type=TYPE_YUV_16;
              break;
            }
            case icSigGrayData:
            {
              source_colorspace=GRAYColorspace;
              source_type=TYPE_GRAY_16;
              break;
            }
            case icSigRgbData:
            {
              source_colorspace=RGBColorspace;
              source_type=TYPE_RGB_16;
              break;
            }
            default:
            {
              source_colorspace=UndefinedColorspace;
              source_type=TYPE_RGB_16;
              break;
            }
          }
          switch (cmsGetColorSpace(target_profile))
          {
            case icSigCmykData:
            {
              target_colorspace=CMYKColorspace;
              target_type=TYPE_CMYK_16;
              break;
            }
            case icSigYCbCrData:
            {
              target_colorspace=YCbCrColorspace;
              target_type=TYPE_YCbCr_16;
              break;
            }
            case icSigLuvData:
            {
              target_colorspace=YUVColorspace;
              target_type=TYPE_YUV_16;
              break;
            }
            case icSigGrayData:
            {
              target_colorspace=GRAYColorspace;
              target_type=TYPE_GRAY_16;
              break;
            }
            case icSigRgbData:
            {
              target_colorspace=RGBColorspace;
              target_type=TYPE_RGB_16;
              break;
            }
            default:
            {
              target_colorspace=UndefinedColorspace;
              target_type=TYPE_RGB_16;
              break;
            }
          }

          /* Colorspace undefined */
          if ((source_colorspace == UndefinedColorspace) ||
              (target_colorspace == UndefinedColorspace))
            {
              (void) cmsCloseProfile(source_profile);
              (void) cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          /* Gray colorspace */
          if (IsGrayColorspace(source_colorspace) &&
              !IsGrayImage(image,&image->exception))
            {
              (void) cmsCloseProfile(source_profile);
              (void) cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          /* CMYK colorspace */
          if (IsCMYKColorspace(source_colorspace) &&
              !IsCMYKColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(source_profile);
              (void) cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          /* YCbCr colorspace */
          if (IsYCbCrColorspace(source_colorspace) &&
              !IsYCbCrColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(source_profile);
              (void) cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                                    ColorspaceColorProfileMismatch);
            }
          /* Verify that source colorspace type is supported */
          if (!IsGrayColorspace(source_colorspace) &&
              !IsCMYKColorspace(source_colorspace) &&
              !IsYCbCrColorspace(source_colorspace) &&
              !IsRGBColorspace(image->colorspace))
            {
              (void) cmsCloseProfile(source_profile);
              (void) cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }

          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Source pixel format: COLORSPACE=%d SWAPFIRST=%d FLAVOR=%d PLANAR=%d ENDIAN16=%d DOSWAP=%d EXTRA=%d CHANNELS=%d BYTES=%d",
                                (int) T_COLORSPACE(source_type),
                                (int) T_SWAPFIRST(source_type),
                                (int) T_FLAVOR(source_type),
                                (int) T_PLANAR(source_type),
                                (int) T_ENDIAN16(source_type),
                                (int) T_DOSWAP(source_type),
                                (int) T_EXTRA(source_type),
                                (int) T_CHANNELS(source_type),
                                (int) T_BYTES(source_type));

          (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                "Target pixel format: COLORSPACE=%d SWAPFIRST=%d FLAVOR=%d PLANAR=%d ENDIAN16=%d DOSWAP=%d EXTRA=%d CHANNELS=%d BYTES=%d",
                                (int) T_COLORSPACE(target_type),
                                (int) T_SWAPFIRST(target_type),
                                (int) T_FLAVOR(target_type),
                                (int) T_PLANAR(target_type),
                                (int) T_ENDIAN16(target_type),
                                (int) T_DOSWAP(target_type),
                                (int) T_EXTRA(target_type),
                                (int) T_CHANNELS(target_type),
                                (int) T_BYTES(target_type));

          switch (image->rendering_intent)
          {
            case AbsoluteIntent:
              intent=INTENT_ABSOLUTE_COLORIMETRIC;
              break;
            case PerceptualIntent: 
              intent=INTENT_PERCEPTUAL; 
              break;
            case RelativeIntent: 
              intent=INTENT_RELATIVE_COLORIMETRIC;
              break;
            case SaturationIntent: 
              intent=INTENT_SATURATION; 
              break;
            default: 
              intent=INTENT_PERCEPTUAL; 
              break;
          }

          /*
            Transform just the colormap if the image is colormapped and we're
            not transforming from gray to RGB/CMYK. A gray to RGB/CMYK
            transformation must create a direct class image for the new image
            to match the color profile even if the new image only has gray
            colors. CMYK images are never color mapped.
          */
          transform_colormap=(image->storage_class == PseudoClass) &&
                             (target_colorspace != CMYKColorspace) &&
                             ((source_colorspace != GRAYColorspace) ||
                              (source_colorspace == target_colorspace));

          transform=cmsCreateTransform(source_profile, /* input profile */
                                       source_type,    /* input pixel format */
                                       target_profile, /* output profile */
                                       target_type,    /* output pixel format */
                                       intent,         /* rendering intent */
                                                       /* build pre-computed transforms? */
                                       (transform_colormap ? cmsFLAGS_NOTPRECALC : 0));
          (void) cmsCloseProfile(source_profile);
          (void) cmsCloseProfile(target_profile);
          if (transform == (cmsHTRANSFORM) NULL)
            {
              ThrowBinaryException3(ResourceLimitError,UnableToManageColor,
                UnableToCreateColorTransform);
            }

          if (transform_colormap)
            {
              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                "Performing pseudo class color conversion");
              q=image->colormap;
              for (x=0; x < (long) image->colors; x++)
              {
                alpha.red=ScaleQuantumToShort(q->red);
                if (source_colorspace != GRAYColorspace)
                  {
                    alpha.green=ScaleQuantumToShort(q->green);
                    alpha.blue=ScaleQuantumToShort(q->blue);
                  }
                cmsDoTransform(transform,&alpha,&beta,1);
                q->red=ScaleShortToQuantum(beta.red);
                if (IsGrayColorspace(target_colorspace))
                  {
                    q->green=q->red;
                    q->blue=q->red;
                  }
                else
                  {
                    q->green=ScaleShortToQuantum(beta.green);
                    q->blue=ScaleShortToQuantum(beta.blue);
                  }
                q++;
              }
              status &= SyncImage(image);
            }
          else
            {
              (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                "Performing direct class color conversion");
              if (image->storage_class == PseudoClass)
                {
                  status &= SyncImage(image);
                  image->storage_class=DirectClass;
                }
              if (target_colorspace == CMYKColorspace)
                image->colorspace=target_colorspace;
              for (y=0; y < (long) image->rows; y++)
              {
                q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                indexes=GetIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                {
                  alpha.red=ScaleQuantumToShort(q->red);
                  if (source_colorspace != GRAYColorspace)
                    {
                      alpha.green=ScaleQuantumToShort(q->green);
                      alpha.blue=ScaleQuantumToShort(q->blue);
                      if (source_colorspace == CMYKColorspace)
                        alpha.black=ScaleQuantumToShort(q->opacity);
                    }
                  cmsDoTransform(transform,&alpha,&beta,1);
                  q->red=ScaleShortToQuantum(beta.red);
                  if (IsGrayColorspace(target_colorspace))
                    {
                      q->green=q->red;
                      q->blue=q->red;
                    }
                  else
                    {
                      q->green=ScaleShortToQuantum(beta.green);
                      q->blue=ScaleShortToQuantum(beta.blue);
                    }
                  if (image->matte)
                    {
                      if ((source_colorspace == CMYKColorspace) &&
                          (target_colorspace != CMYKColorspace))
                        q->opacity=indexes[x];
                      else
                        if ((source_colorspace != CMYKColorspace) &&
                            (target_colorspace == CMYKColorspace))
                          indexes[x]=q->opacity;
                    }
                  if (target_colorspace == CMYKColorspace)
                    q->opacity=ScaleShortToQuantum(beta.black);
                  q++;
                }
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
              }
            }
          image->colorspace=target_colorspace;
          /*
            We can't be sure black and white stays exactly black and white
            and that gray colors transform to gray colors.
          */
          image->is_grayscale=IsGrayColorspace(target_colorspace);
          image->is_monochrome=False; 
          cmsDeleteTransform(transform);

          /*
            Throw away the old profile after conversion before we
            assign a new one.
          */
          DeleteImageProfile(image,"ICM");
#else
          ThrowBinaryException(MissingDelegateError,LCMSLibraryIsNotAvailable,
            image->filename);
#endif
        }

      /*
        TODO: If the image *did not* already have a color profile,
        verify that the colorspace of the new profile is valid for the
        colorspace of the image. If LCMS is not available we should
        refuse to assign a new profile (just like we're refusing a
        conversion above) as we can't be sure the assigment is valid.
        We might be trying to assign a CMYK profile to an RGB image,
        for instance.
      */
      
      SetImageProfile(image,"ICM",profile,length);
      if (!clone)
        MagickFreeMemory(profile);
      return(status);
    }

  status &= SetImageProfile(image,name,profile,length);
  if (!clone)
    MagickFreeMemory(profile);

  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     S e t I m a g e P r o f i l e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageProfile adds a named profile to the image. If a profile with the
%  same name already exists, then it is replaced. If a null profile address
%  is supplied, then an existing profile is removed. The profile is copied
%  into the image. Note that this function does not execute CMS color
%  profiles. Any existing CMS color profile is simply replaced. Use the
%  ProfileImage() function in order to execute a CMS color profile.
%
%  Older versions of this function stored profiles named "8BIM" and "IPTC"
%  in the same storage location.  This is no longer the case.  However,
%  GetImageProfile() will try the alternate name if the specifically
%  requested profile name is not available.  Note that when trying to remove
%  a profile, it may be necessary to remove both names in order for an
%  "IPTC" profile to no longer be included in output file formats.
%
%  The format of the SetImageProfile method is:
%
%      unsigned int SetImageProfile(Image *image,const char *name,
%                            const unsigned char *profile,const size_t length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", "IPTC", XMP, or any
%                          unique text string.
%
%    o profile: Address of profile to add. Pass zero to remove an existing
%               profile.
%
%    o length: The length of the profile.
%
*/
MagickExport MagickPassFail
SetImageProfile(Image *image,const char *name, const unsigned char *profile,
                const size_t length)
{
  unsigned int
    status = MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);

  if ((profile == 0) && (image->profiles != 0))
    {
      /*
        Remove existing entry.
      */
      status &= MagickMapRemoveEntry(image->profiles,name);
    }
  else
    {
      /*
        Add or replace entry.
      */
      if (image->profiles == 0)
        image->profiles=MagickMapAllocateMap(MagickMapCopyBlob,
                                             MagickMapDeallocateBlob);
      
      status &= MagickMapAddEntry(image->profiles,name,profile,length,
                                  &image->exception);
    }
  return (status);
}
