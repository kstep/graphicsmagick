/* 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%                  L       OOO    CCCC   AAA   L      EEEEE                   % 
%                  L      O   O  C      A   A  L      E                       % 
%                  L      O   O  C      AAAAA  L      EEE                     % 
%                  L      O   O  C      A   A  L      E                       % 
%                  LLLLL   OOO    CCCC  A   A  LLLLL  EEEEE                   % 
%                                                                             % 
%                                                                             % 
%                    ImageMagick Locale Message Methods                       % 
%                                                                             % 
%                                                                             % 
%                              Software Design                                % 
%                                John Cristy                                  % 
%                                Kyle Shorter                                 % 
%                               September 2002                                % 
%                                                                             % 
%                                                                             % 
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated % 
%  to making software imaging solutions freely available.                     % 
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
%  Licensor ("ImageMagick Studio LLC") warrants that the copyright in and to  % 
%  the Software ("ImageMagick") is owned by ImageMagick Studio LLC or that    % 
%  ImageMagick is distributed by ImageMagick Studio LLC under a valid current % 
%  license. Except as expressly stated in the immediately preceding           % 
%  sentence, ImageMagick is provided by ImageMagick Studio LLC, contributors, % 
%  and copyright owners "AS IS", without warranty of any kind, express or     % 
%  implied, including but not limited to the warranties of merchantability,   % 
%  fitness for a particular purpose and non-infringement.  In no event shall  % 
%  ImageMagick Studio LLC, contributors or copyright owners be liable for     % 
%  any claim, damages, or other liability, whether in an action of contract,  % 
%  tort or otherwise, arising from, out of or in connection with              % 
%  ImageMagick.                                                               % 
%                                                                             % 
%  Except as contained in this notice, the name of the ImageMagick Studio     % 
%  shall not be used in advertising or otherwise to promote the sale, use or  % 
%  other dealings in ImageMagick without prior written authorization from the % 
%  ImageMagick Studio.                                                        % 
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
#include "utility.h" 
 
/* 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%   G e t L o c a l e M e s s a g e                                           % 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% 
%  GetLocaleMessage() returns a message in the current locale that matches the 
%  supplied tag. 
% 
%  The format of the GetLocaleMessage method is: 
% 
%      const char *GetLocaleMessage(const char *tag) 
% 
%  A description of each parameter follows: 
% 
%    o tag: Return a message that matches this tag in the current locale. 
% 
% 
*/ 
/* This method is autogenerated-- do not edit */
const char *GetLocaleMessage(const char *tag)
{
#define NEXT_FIELD ((p = (np = strchr((tp = np), '/')) ? np++ : (np = tp + strlen(tp))), tp)


   static const char *locale = 0;
   register const char *p, *tp, *np;
   if (!tag || *tag == '\0')
      return "";

   if ( (!locale &&
         ( (!(locale = setlocale(LC_CTYPE, 0)) || *locale == '\0') &&
           (!(locale = getenv("LC_ALL"))       || *locale == '\0') &&
           (!(locale = getenv("LC_CTYPE"))     || *locale == '\0') &&
           (!(locale = getenv("LANG"))         || *locale == '\0') ) )
         || !LocaleCompare(locale, "C"))
      locale = "C";

   tp = locale;
   p = locale + strlen(locale);
   np = tag;
  switch (*locale)
  {
  default:
    return tag;

  case 'c':  case 'C':
    if (p - tp == 1 && !LocaleNCompare(tp, "C", 1))
      switch (*NEXT_FIELD)
      {
      default:
        return tag;

      case 'b':  case 'B':
        if (p - tp == 4 && !LocaleNCompare(tp, "Blob", 4))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCreateBlob", 18))
                return *np ? tag : "Unable to create blob";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToOpenFile", 16))
                return *np ? tag : "Unable to open file";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadBlob", 16))
                return *np ? tag : "Unable to read blob";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadFile", 16))
                return *np ? tag : "Unable to read file";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToWriteBlob", 17))
                return *np ? tag : "Unable to write blob";
              else
              if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedImageFormat", 23))
                return *np ? tag : "Unrecognized image format";
              else
                return tag;

            case 'z':  case 'Z':
              if (p - tp == 26 && !LocaleNCompare(tp, "ZeroLengthBlobNotPermitted", 26))
                return *np ? tag : "Zero-length blob not permitted";
              else
                return tag;
            }
        else
          return tag;

      case 'c':  case 'C':
        if (p - tp == 5 && !LocaleNCompare(tp, "Cache", 5))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'p':  case 'P':
              if (p - tp == 19 && !LocaleNCompare(tp, "PixelCacheIsNotOpen", 19))
                return *np ? tag : "Pixel cache is not open";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCloneCache", 18))
                return *np ? tag : "Unable to clone cache";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToExtendCache", 19))
                return *np ? tag : "Unable to extend cache";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToGetCacheNexus", 21))
                return *np ? tag : "Unable to get cache nexus";
              else
              if (p - tp == 26 && !LocaleNCompare(tp, "UnableToGetPixelsFromCache", 26))
                return *np ? tag : "Unable to get pixels from cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToOpenCache", 17))
                return *np ? tag : "Unable to open cache";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "UnableToReadPixelCache", 22))
                return *np ? tag : "Unable to read pixel cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToSyncCache", 17))
                return *np ? tag : "Unable to sync cache";
              else
                return tag;
            }
        else
        if (p - tp == 9 && !LocaleNCompare(tp, "Configure", 9))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
          if (LocaleNCompare(NEXT_FIELD, "IncludeElementNestedTooDeeply", 29) || p - tp != 29)
            return tag;
          else
            return *np ? tag : "include element nested too deeply";
        else
          return tag;

      case 'i':  case 'I':
        if (p - tp == 5 && !LocaleNCompare(tp, "Image", 5))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'i':  case 'I':
              if (p - tp == 23 && !LocaleNCompare(tp, "ImageSequenceIsRequired", 23))
                return *np ? tag : "Image sequence is required";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCompareImages", 21))
                return *np ? tag : "Unable to compare images";
              else
                return tag;
            }
        else
          return tag;

      case 'o':  case 'O':
        if (p - tp == 6 && !LocaleNCompare(tp, "Option", 6))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'g':  case 'G':
              if (p - tp == 25 && !LocaleNCompare(tp, "GeometryDimensionsAreZero", 25))
                return *np ? tag : "Geometry dimensions are zero";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "GeometryDoesNotContainImage", 27))
                return *np ? tag : "Geometry does not contain image";
              else
                return tag;

            case 'i':  case 'I':
              if (p - tp == 23 && !LocaleNCompare(tp, "ImagesAreNotTheSameSize", 23))
                return *np ? tag : "Images are not the same size";
              else
                return tag;

            case 'n':  case 'N':
              if (p - tp == 21 && !LocaleNCompare(tp, "NoProfileNameWasGiven", 21))
                return *np ? tag : "No profile name was given";
              else
                return tag;
            }
        else
          return tag;

      case 'r':  case 'R':
        if (p - tp == 8 && !LocaleNCompare(tp, "Resource", 8))
          if (LocaleNCompare(NEXT_FIELD, "Limit", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'e':  case 'E':
              if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
                switch (*NEXT_FIELD)
                {
                default:
                  return tag;

                case 'c':  case 'C':
                  if (p - tp == 23 && !LocaleNCompare(tp, "CacheResourcesExhausted", 23))
                    return *np ? tag : "Cache resources exhausted";
                  else
                    return tag;

                case 'm':  case 'M':
                  if (p - tp == 22 && !LocaleNCompare(tp, "MemoryAllocationFailed", 22))
                    return *np ? tag : "Memory allocation failed";
                  else
                    return tag;

                case 'n':  case 'N':
                  if (p - tp == 22 && !LocaleNCompare(tp, "NoPixelsDefinedInCache", 22))
                    return *np ? tag : "No pixels defined in cache";
                  else
                    return tag;

                case 'p':  case 'P':
                  if (p - tp == 26 && !LocaleNCompare(tp, "PixelCacheAllocationFailed", 26))
                    return *np ? tag : "Pixel cache allocation failed";
                  else
                    return tag;

                case 'u':  case 'U':
                  if (p - tp == 19 && !LocaleNCompare(tp, "UnableToManageColor", 19))
                    return *np ? tag : "Unable to manage color";
                  else
                    return tag;
                }
              else
                return tag;

            case 'w':  case 'W':
              if (p - tp == 7 && !LocaleNCompare(tp, "Warning", 7))
                if (LocaleNCompare(NEXT_FIELD, "MemoryAllocationFailed", 22) || p - tp != 22)
                  return tag;
                else
                  return *np ? tag : "Memory allocation failed";
              else
                return tag;
            }
        else
          return tag;

      case 'u':  case 'U':
        if (p - tp == 27 && !LocaleNCompare(tp, "UnableToAccessConfigureFile", 27))
          return *np ? tag : "Unable to access configure file";
        else
        if (p - tp == 22 && !LocaleNCompare(tp, "UnableToAccessFontFile", 22))
          return *np ? tag : "Unable to access font file";
        else
        if (p - tp == 24 && !LocaleNCompare(tp, "UnableToAccessModuleFile", 24))
          return *np ? tag : "Unable to access module file";
        else
          return tag;
      }
    else
      return tag;

  case 'e':  case 'E':
    if (p - tp == 5 && !LocaleNCompare(tp, "en_US", 5))
      switch (*NEXT_FIELD)
      {
      default:
        return tag;

      case 'b':  case 'B':
        if (p - tp == 4 && !LocaleNCompare(tp, "Blob", 4))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCreateBlob", 18))
                return *np ? tag : "Unable to create blob";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToOpenFile", 16))
                return *np ? tag : "Unable to open file";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadBlob", 16))
                return *np ? tag : "Unable to read blob";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadFile", 16))
                return *np ? tag : "Unable to read file";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToWriteBlob", 17))
                return *np ? tag : "Unable to write blob";
              else
              if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedImageFormat", 23))
                return *np ? tag : "Unrecognized image format";
              else
                return tag;

            case 'z':  case 'Z':
              if (p - tp == 26 && !LocaleNCompare(tp, "ZeroLengthBlobNotPermitted", 26))
                return *np ? tag : "Zero-length blob not permitted";
              else
                return tag;
            }
        else
          return tag;

      case 'c':  case 'C':
        if (p - tp == 5 && !LocaleNCompare(tp, "Cache", 5))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'p':  case 'P':
              if (p - tp == 19 && !LocaleNCompare(tp, "PixelCacheIsNotOpen", 19))
                return *np ? tag : "Pixel cache is not open";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCloneCache", 18))
                return *np ? tag : "Unable to clone cache";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToExtendCache", 19))
                return *np ? tag : "Unable to extend cache";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToGetCacheNexus", 21))
                return *np ? tag : "Unable to get cache nexus";
              else
              if (p - tp == 26 && !LocaleNCompare(tp, "UnableToGetPixelsFromCache", 26))
                return *np ? tag : "Unable to get pixels from cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToOpenCache", 17))
                return *np ? tag : "Unable to open cache";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "UnableToReadPixelCache", 22))
                return *np ? tag : "Unable to read pixel cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToSyncCache", 17))
                return *np ? tag : "Unable to sync cache";
              else
                return tag;
            }
        else
        if (p - tp == 9 && !LocaleNCompare(tp, "Configure", 9))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
          if (LocaleNCompare(NEXT_FIELD, "IncludeElementNestedTooDeeply", 29) || p - tp != 29)
            return tag;
          else
            return *np ? tag : "include element nested too deeply";
        else
          return tag;

      case 'i':  case 'I':
        if (p - tp == 5 && !LocaleNCompare(tp, "Image", 5))
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
          if (LocaleNCompare(NEXT_FIELD, "ImageSequenceIsRequired", 23) || p - tp != 23)
            return tag;
          else
            return *np ? tag : "Image sequence is required";
        else
          return tag;

      case 'r':  case 'R':
        if (p - tp == 8 && !LocaleNCompare(tp, "Resource", 8))
          if (LocaleNCompare(NEXT_FIELD, "Limit", 5) || p - tp != 5)
            return tag;
          else
          if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
            return tag;
          else
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'c':  case 'C':
              if (p - tp == 23 && !LocaleNCompare(tp, "CacheResourcesExhausted", 23))
                return *np ? tag : "Cache resources exhausted";
              else
                return tag;

            case 'm':  case 'M':
              if (p - tp == 22 && !LocaleNCompare(tp, "MemoryAllocationFailed", 22))
                return *np ? tag : "Memory allocation failed";
              else
                return tag;

            case 'n':  case 'N':
              if (p - tp == 22 && !LocaleNCompare(tp, "NoPixelsDefinedInCache", 22))
                return *np ? tag : "No pixels defined in cache";
              else
                return tag;

            case 'p':  case 'P':
              if (p - tp == 26 && !LocaleNCompare(tp, "PixelCacheAllocationFailed", 26))
                return *np ? tag : "Pixel cache allocation failed";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToManageColor", 19))
                return *np ? tag : "Unable to manage color";
              else
                return tag;
            }
        else
          return tag;

      case 'u':  case 'U':
        if (p - tp == 27 && !LocaleNCompare(tp, "UnableToAccessConfigureFile", 27))
          return *np ? tag : "Unable to access configure file";
        else
        if (p - tp == 22 && !LocaleNCompare(tp, "UnableToAccessFontFile", 22))
          return *np ? tag : "Unable to access font file";
        else
        if (p - tp == 24 && !LocaleNCompare(tp, "UnableToAccessModuleFile", 24))
          return *np ? tag : "Unable to access module file";
        else
          return tag;
      }
    else
      return tag;
  }

   return tag;
}
