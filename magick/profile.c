/*
% Copyright (C) 2003 GraphicsMagick Group
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
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/composite.h"
#include "magick/monitor.h"
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
%  The format of the GetImageProfile method is:
%
%      const unsigned char *GetImageProfile(const Image* image,
%                             const char *name, size_t *length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", & "IPTC" or an
%                          existing generic profile name.
%
%    o length: Updated with profile length if profile is present.
%
*/
MagickExport const unsigned char *
GetImageProfile(const Image* image, const char *name, size_t *length)
{
  long
    i;
  
  const ProfileInfo
    *profile=0;
  
  *length=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);
  assert(length != (size_t *) NULL);

  /* ICC color profile ("ICM") */
  if (((LocaleCompare("ICC",name) == 0) ||
       (LocaleCompare("ICM",name) == 0))
      &&
      (image->color_profile.info != 0))
    {
      profile=&image->color_profile;
    }

  /* IPTC profile ("8BIM" or "IPTC") */
  if (((LocaleCompare("8BIM",name) == 0) ||
      (LocaleCompare("IPTC",name) == 0)) &&
       (image->iptc_profile.info != 0))
    {
      profile=&image->iptc_profile;
    }

  /* Generic profiles */
  if ((image->generic_profiles != 0) && (image->generic_profile))
    {
      for (i=0; i < (long) image->generic_profiles; i++)
        {
          if (LocaleCompare(image->generic_profile[i].name,name) != 0)
            continue;
          if(image->generic_profile[i].info)
            profile=&image->generic_profile[i];
          break;
        }
    }
  if (profile)
    {
      *length=profile->length;
      return (profile->info);
    }

  return (0);
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
%  ProfileImage() adds or removes a ICM, IPTC, or generic profile from an
%  image.  If the profile is NULL, it is removed from the image otherwise
%  added.  Use a name of '*' and a profile of NULL to remove all profiles
%  from the image. Ownership of the profile is transferred to GraphicsMagick
%  (it should not be altered or deallocated) unless the clone option is set
%  to True.
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
ProfileImage(Image *image,const char *name,const unsigned char *profile,
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
      if (GlobExpression("8bim",name) || GlobExpression("iptc",name))
        {
          if (image->iptc_profile.length != 0)
            MagickFreeMemory(image->iptc_profile.info);
          image->iptc_profile.length=0;
          image->iptc_profile.info=(unsigned char *) NULL;
        }
      if (GlobExpression("icm",name))
        {
          if (image->color_profile.length != 0)
            MagickFreeMemory(image->color_profile.info);
          image->color_profile.length=0;
          image->color_profile.info=(unsigned char *) NULL;
        }
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        if (!GlobExpression(image->generic_profile[i].name,name))
          continue;
        if (image->generic_profile[i].name != (char *) NULL)
          MagickFreeMemory(image->generic_profile[i].name);
        if (image->generic_profile[i].length != 0)
          MagickFreeMemory(image->generic_profile[i].info);
        image->generic_profiles--;
        for (j=i; j < (long) image->generic_profiles; j++)
          image->generic_profile[j]=image->generic_profile[j+1];
        i--;
      }
      return(MagickPass);
    }
  /*
    Add a ICM, IPTC, or generic profile to the image.
  */
  if ((LocaleCompare("8bim",name) == 0) || (LocaleCompare("iptc",name) == 0))
    {
      if (image->iptc_profile.length != 0)
        MagickFreeMemory(image->iptc_profile.info);
      if (clone)
        {
          image->iptc_profile.info=MagickAllocateMemory(unsigned char *,length);
          if (image->iptc_profile.info == (unsigned char *) NULL)
            ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
              UnableToAddIPTCProfile);
          image->iptc_profile.length=length;
          (void) memcpy(image->iptc_profile.info,profile,length);
        }
      else
        {
          image->iptc_profile.length=length;
          image->iptc_profile.info=(unsigned char *) profile;
        }
      return(MagickPass);
    }
  if (LocaleCompare("icm",name) == 0)
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                            "Profile1: %lu bytes, Profile2: %lu bytes",
                            (unsigned long) length,
                            (unsigned long) image->color_profile.length);

      /* Check for identical input and output profiles. Return on identity. */
      if ((length != 0) && (length == image->color_profile.length) &&
          (memcmp(image->color_profile.info,profile,length) == 0))
        {
          return(MagickPass);
        }

      /* Convert to new colors if we have both an old and a new profile. */
      if ((image->color_profile.length != 0) && (length != 0))
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
          source_profile=cmsOpenProfileFromMem(image->color_profile.info,
            image->color_profile.length);
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

          if ((source_colorspace == UndefinedColorspace) ||
              (target_colorspace == UndefinedColorspace))
            {
              cmsCloseProfile(source_profile);
              cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          if ((IsGrayColorspace(source_colorspace)) &&
              (!IsGrayImage(image,&image->exception)))
            {
              cmsCloseProfile(source_profile);
              cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          if ((source_colorspace == CMYKColorspace) &&
              (image->colorspace != CMYKColorspace))
            {
              cmsCloseProfile(source_profile);
              cmsCloseProfile(target_profile);
              ThrowBinaryException3(ImageError,UnableToAssignProfile,
                ColorspaceColorProfileMismatch);
            }
          if ((source_colorspace != GRAYColorspace) &&
              (source_colorspace != CMYKColorspace) &&
              (image->colorspace != RGBColorspace))
            {
              cmsCloseProfile(source_profile);
              cmsCloseProfile(target_profile);
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
          cmsCloseProfile(source_profile);
          cmsCloseProfile(target_profile);
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
          MagickFreeMemory(image->color_profile.info);
          image->color_profile.info=(unsigned char *) NULL;
          image->color_profile.length=0;
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
      if (clone)
        {
          image->color_profile.info=MagickAllocateMemory(unsigned char *,length);
          if (image->color_profile.info == (unsigned char *) NULL)
            ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
              UnableToAddColorProfile);
          image->color_profile.length=length;
          (void) memcpy(image->color_profile.info,profile,length);
        }
      else
        {
          image->color_profile.length=length;
          image->color_profile.info=(unsigned char *) profile;
        }
      return(status);
    }
  for (i=0; i < (long) image->generic_profiles; i++)
    if (LocaleCompare(image->generic_profile[i].name,name) == 0)
      break;
  if (i == (long) image->generic_profiles)
    {
      if (image->generic_profile == (ProfileInfo *) NULL)
        image->generic_profile=MagickAllocateMemory(ProfileInfo *,
          (i+1)*sizeof(ProfileInfo));
      else
        MagickReallocMemory(image->generic_profile,
          (i+1)*sizeof(ProfileInfo));
      if (image->generic_profile == (ProfileInfo *) NULL)
        ThrowBinaryException(ResourceLimitWarning,MemoryAllocationFailed,
          (char *) NULL)
      image->generic_profiles++;
      image->generic_profile[i].length=0;
      image->generic_profile[i].info=(unsigned char *) NULL;
      image->generic_profile[i].name=AllocateString(name);
    }
  if (image->generic_profile[i].length != 0)
    MagickFreeMemory(image->generic_profile[i].info);
  if (clone)
    {
      image->generic_profile[i].info=MagickAllocateMemory(unsigned char *,length);
      if (image->generic_profile[i].info == (unsigned char *) NULL)
        ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToAddGenericProfile);
      image->generic_profile[i].length=length;
      (void) memcpy(image->generic_profile[i].info,profile,length);
    }
  else
    {
      image->generic_profile[i].length=length;
      image->generic_profile[i].info=(unsigned char *) profile;
    }
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
%  The format of the SetImageProfile method is:
%
%      unsigned int SetImageProfile(Image *image,const char *name,
%                            const unsigned char *profile,const size_t length)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o name: Profile name. Valid names are "8BIM", "ICM", & "IPTC" or an
%                          generic profile name.
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
  long
    i;
  
  ProfileInfo
    *image_profile=0;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(name != NULL);

  /* ICC color profile ("ICC" or "ICM") */
  if ((LocaleCompare("ICC",name) == 0) ||
      (LocaleCompare("ICM",name) == 0))
    image_profile=&image->color_profile;

  /* IPTC profile ("8BIM" or "IPTC") */
  if ((LocaleCompare("8BIM",name) == 0) ||
      (LocaleCompare("IPTC",name) == 0))
    image_profile=&image->iptc_profile;

  /* Generic profiles */
  if (!image_profile)
    {
      if ((image->generic_profiles != 0) && (image->generic_profile))
        {
          /* Search for an existing profile entry using name */
          for (i=0; i < (long) image->generic_profiles; i++)
            {
              if (LocaleCompare(image->generic_profile[i].name,name) == 0)
                {
                  image_profile=&image->generic_profile[i];
                  break;
                }
            }
        }
      if (!image_profile && profile)
        {
          /* Need to add a new generic profile */
          if (!image->generic_profile)
            {
              /* Initial generic profile */
              image->generic_profile=MagickAllocateMemory(ProfileInfo *,
                sizeof(ProfileInfo));
              image->generic_profiles=1;
            }
          else
            {
              /* Extend generic profiles */
              image->generic_profiles++;
              MagickReallocMemory(image->generic_profile,
                image->generic_profiles*sizeof(ProfileInfo));
            }
          if (!image->generic_profile)
            ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
              UnableToAddColorProfile);
          image_profile=&image->generic_profile[image->generic_profiles-1];
          image_profile->info=0;
          image_profile->length=0;
	  image_profile->name=NULL;
        }
    }
  if (image_profile)
    {
      /* Clear existing profile */
      MagickFreeMemory(image_profile->info);
      image_profile->length=0;

      if (profile)
        {
          /* Clone user-supplied profile */
          (void) CloneString(&image_profile->name, name);
	  if (!image_profile->name)
            ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
              UnableToAddColorProfile);
          image_profile->info=MagickAllocateMemory(unsigned char *,length);
          if (!image_profile->info)
            ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
              UnableToAddColorProfile);
          image_profile->length=length;
          (void) memcpy(image_profile->info,profile,length);
        }
    }
  return (status);
}
