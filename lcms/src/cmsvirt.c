//
//  Little cms
//  Copyright (C) 1998-2003 Marti Maria
//
// THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//
// IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
// OF THIS SOFTWARE.
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 
// 1.11a

#include "lcms.h"


// Virtual (built-in) profiles
// -----------------------------------------------------------------------------------


// This function creates a profile based on White point, primaries and
// transfer functions.


cmsHPROFILE LCMSEXPORT cmsCreateRGBProfile(LPcmsCIExyY WhitePoint,
                                LPcmsCIExyYTRIPLE Primaries,
                                LPGAMMATABLE TransferFunction[3])
{
       cmsHPROFILE hICC;
       cmsCIEXYZ tmp;
       MAT3 MColorants;
       cmsCIEXYZTRIPLE Colorants;
       cmsCIExyY MaxWhite;
      

       hICC = _cmsCreateProfilePlaceholder();
       if (!hICC)                          // can't allocate
            return NULL;
              

       cmsSetDeviceClass(hICC,      icSigDisplayClass);
       cmsSetColorSpace(hICC,       icSigRgbData);
       cmsSetPCS(hICC,              icSigXYZData);
       cmsSetRenderingIntent(hICC,  INTENT_PERCEPTUAL); 
        
      
       // Implement profile using following tags:
       //
       //  1 icSigProfileDescriptionTag
       //  2 icSigMediaWhitePointTag
       //  3 icSigRedColorantTag
       //  4 icSigGreenColorantTag
       //  5 icSigBlueColorantTag
       //  6 icSigRedTRCTag
       //  7 icSigGreenTRCTag
       //  8 icSigBlueTRCTag

       // This conforms a standard RGB DisplayProfile as says ICC, and then I add

       // 9 icSigChromaticityTag

       // As addendum II
      

       // Fill-in the tags

       cmsAddTag(hICC, icSigDeviceMfgDescTag,       "(lcms internal)");
       cmsAddTag(hICC, icSigProfileDescriptionTag,  "lcms RGB virtual profile");
       cmsAddTag(hICC, icSigDeviceModelDescTag,     "rgb built-in");      
          

       if (WhitePoint) {

       cmsxyY2XYZ(&tmp, WhitePoint);
       cmsAddTag(hICC, icSigMediaWhitePointTag, &tmp);
       }

       if (WhitePoint && Primaries) {

        MaxWhite.x =  WhitePoint -> x;
        MaxWhite.y =  WhitePoint -> y;
        MaxWhite.Y =  1.0;

       if (!cmsBuildRGB2XYZtransferMatrix(&MColorants, &MaxWhite, Primaries))
       {
              cmsCloseProfile(hICC);
              return NULL;
       }  
     
       cmsAdaptMatrixToD50(&MColorants, &MaxWhite);

       Colorants.Red.X = MColorants.v[0].n[0];
       Colorants.Red.Y = MColorants.v[1].n[0];
       Colorants.Red.Z = MColorants.v[2].n[0];

       Colorants.Green.X = MColorants.v[0].n[1];
       Colorants.Green.Y = MColorants.v[1].n[1];
       Colorants.Green.Z = MColorants.v[2].n[1];

       Colorants.Blue.X = MColorants.v[0].n[2];
       Colorants.Blue.Y = MColorants.v[1].n[2];
       Colorants.Blue.Z = MColorants.v[2].n[2];

       cmsAddTag(hICC, icSigRedColorantTag,   &Colorants.Red);
       cmsAddTag(hICC, icSigBlueColorantTag,  &Colorants.Blue);
       cmsAddTag(hICC, icSigGreenColorantTag, &Colorants.Green);
       }


       if (TransferFunction) {

       // In case of gamma, we must dup' the table pointer

        cmsAddTag(hICC, icSigRedTRCTag,   TransferFunction[0]);
        cmsAddTag(hICC, icSigGreenTRCTag, TransferFunction[1]);
        cmsAddTag(hICC, icSigBlueTRCTag,  TransferFunction[2]);
       }

       if (Primaries) {
            cmsAddTag(hICC, icSigChromaticityTag, Primaries);
       }

       return hICC;
}



// This function creates a profile based on White point and transfer function.

cmsHPROFILE   LCMSEXPORT cmsCreateGrayProfile(LPcmsCIExyY WhitePoint,
                                              LPGAMMATABLE TransferFunction)
{
       cmsHPROFILE hICC;
       cmsCIEXYZ tmp;              
      
       
       hICC = _cmsCreateProfilePlaceholder();
       if (!hICC)                          // can't allocate
            return NULL;
              

       cmsSetDeviceClass(hICC,      icSigDisplayClass);
       cmsSetColorSpace(hICC,       icSigGrayData);
       cmsSetPCS(hICC,              icSigXYZData);
       cmsSetRenderingIntent(hICC,  INTENT_PERCEPTUAL); 



       // Implement profile using following tags:
       //
       //  1 icSigProfileDescriptionTag
       //  2 icSigMediaWhitePointTag
       //  6 icSigGrayTRCTag
       
       // This conforms a standard Gray DisplayProfile 
            
       // Fill-in the tags

       
       cmsAddTag(hICC, icSigDeviceMfgDescTag,       "(lcms internal)");
       cmsAddTag(hICC, icSigProfileDescriptionTag,  "lcms gray virtual profile");
       cmsAddTag(hICC, icSigDeviceModelDescTag,     "gray built-in");      
    

       if (WhitePoint) {

       cmsxyY2XYZ(&tmp, WhitePoint);
       cmsAddTag(hICC, icSigMediaWhitePointTag, &tmp);
       }


       if (TransferFunction) {

       // In case of gamma, we must dup' the table pointer

       cmsAddTag(hICC, icSigGrayTRCTag, TransferFunction);
       }

       return hICC;

}


static
int IsPCS(icColorSpaceSignature ColorSpace)
{
    return (ColorSpace == icSigXYZData ||
            ColorSpace == icSigLabData);
}

static
void FixColorSpaces(cmsHPROFILE hProfile, 
                              icColorSpaceSignature ColorSpace, 
                              icColorSpaceSignature PCS)
{

    if (IsPCS(ColorSpace) && IsPCS(PCS)) { 

            cmsSetDeviceClass(hProfile,      icSigAbstractClass);
            cmsSetColorSpace(hProfile,       ColorSpace);
            cmsSetPCS(hProfile,              PCS);
            return;
    }

    if (IsPCS(ColorSpace) && !IsPCS(PCS)) {

            cmsSetDeviceClass(hProfile, icSigOutputClass);
            cmsSetPCS(hProfile,         ColorSpace);
            cmsSetColorSpace(hProfile,  PCS);        
            return;
    }

    if (IsPCS(PCS) && !IsPCS(ColorSpace)) {

           cmsSetDeviceClass(hProfile,  icSigInputClass);
           cmsSetColorSpace(hProfile,   ColorSpace);
           cmsSetPCS(hProfile,          PCS);
           return;
    }

     cmsSetDeviceClass(hProfile,      icSigLinkClass);
     cmsSetColorSpace(hProfile,       ColorSpace);
     cmsSetPCS(hProfile,              PCS);

}


// Does convert a transform into a device link profile

cmsHPROFILE LCMSEXPORT cmsTransform2DeviceLink(cmsHTRANSFORM hTransform, DWORD dwFlags)
{
         cmsHPROFILE hICC;
        _LPcmsTRANSFORM v = (_LPcmsTRANSFORM) hTransform;
        LPLUT Lut;
        BOOL MustFreeLUT;
      
        if (v ->DeviceLink) {

                Lut = v -> DeviceLink;
                MustFreeLUT = FALSE;
        }
        else {

            Lut = _cmsPrecalculateDeviceLink(hTransform, dwFlags);
            if (!Lut) return NULL;
            MustFreeLUT = TRUE;
        }

       hICC = _cmsCreateProfilePlaceholder();
       if (!hICC) {                          // can't allocate
      
            if (MustFreeLUT) cmsFreeLUT(Lut);
            return NULL;
       }


       FixColorSpaces(hICC, v -> EntryColorSpace, v -> ExitColorSpace);             

       cmsSetRenderingIntent(hICC,  v -> Intent); 
              
       // Implement devicelink profile using following tags:
       //
       //  1 icSigProfileDescriptionTag
       //  2 icSigMediaWhitePointTag
       //  3 icSigAToB0Tag
       //  and should store 4 icSigProfileSequenceDescTag, but currently it doesn't
       
                 
       cmsAddTag(hICC, icSigDeviceMfgDescTag,       "LittleCMS");       
       cmsAddTag(hICC, icSigProfileDescriptionTag,  "Device link");
       cmsAddTag(hICC, icSigDeviceModelDescTag,     "Device link");      
    

       cmsAddTag(hICC, icSigMediaWhitePointTag,  cmsD50_XYZ());

       if (cmsGetDeviceClass(hICC) == icSigOutputClass) {
           
                icSignature Tags[] = { icSigBToA0Tag, 
                                       icSigBToA1Tag, 
                                       icSigBToA2Tag, 
                                       icSigBToA1Tag };

                cmsAddTag(hICC, Tags[v ->Intent], Lut);
       }

       else
                cmsAddTag(hICC, icSigAToB0Tag, Lut);

       
       if (MustFreeLUT) cmsFreeLUT(Lut);

       return hICC;

}


// This is a devicelink operating in the target colorspace with as many transfer
// functions as components

cmsHPROFILE LCMSEXPORT cmsCreateLinearizationDeviceLink(icColorSpaceSignature ColorSpace,
                                                        LPGAMMATABLE TransferFunctions[])
{
       cmsHPROFILE hICC;
       LPLUT Lut;
           
        
       hICC = _cmsCreateProfilePlaceholder();
       if (!hICC)                          // can't allocate
            return NULL;


       cmsSetDeviceClass(hICC,      icSigLinkClass);
       cmsSetColorSpace(hICC,       ColorSpace);
       cmsSetPCS(hICC,              ColorSpace);
       cmsSetRenderingIntent(hICC,  INTENT_PERCEPTUAL); 

     
       // Creates a LUT with prelinearization step only
       Lut = cmsAllocLUT();

       // Set up channels
       Lut ->InputChan = Lut ->OutputChan = _cmsChannelsOf(ColorSpace);

       // Copy tables to LUT
       cmsAllocLinearTable(Lut, TransferFunctions, 1);

       // Create tags       
       cmsAddTag(hICC, icSigDeviceMfgDescTag,       "(lcms internal)");
       cmsAddTag(hICC, icSigProfileDescriptionTag,  "lcms linearization device link");
       cmsAddTag(hICC, icSigDeviceModelDescTag,     "linearization built-in");      
    
       cmsAddTag(hICC, icSigMediaWhitePointTag, cmsD50_XYZ());
       cmsAddTag(hICC, icSigAToB0Tag, Lut);
       
       // LUT is already on virtual profile
       cmsFreeLUT(Lut);

       // Ok, done
       return hICC;
}


// Ink-limiting algorithm
//
//  Sum = C + M + Y + K 
//  If Sum > InkLimit 
//        Ratio= 1 - (Sum - InkLimit) / (C + M + Y)
//        if Ratio <0 
//              Ratio=0
//        endif     
//     Else 
//         Ratio=1
//     endif
//
//     C = Ratio * C
//     M = Ratio * M
//     Y = Ratio * Y
//     K: Does not change
   
static
int InkLimitingSampler(register WORD In[], register WORD Out[], register LPVOID Cargo)
{
        double InkLimit = *(double *) Cargo;
        double SumCMY, SumCMYK, Ratio;
    
        InkLimit = (InkLimit * 655.35);

        SumCMY   = In[0]  + In[1] + In[2];
        SumCMYK  = SumCMY + In[3];      

        if (SumCMYK > InkLimit) {

                Ratio = 1 - ((SumCMYK - InkLimit) / SumCMY);
                if (Ratio < 0)
                        Ratio = 0;
        }
        else Ratio = 1;
                
        Out[0] = (WORD) floor(In[0] * Ratio + 0.5);     // C
        Out[1] = (WORD) floor(In[1] * Ratio + 0.5);     // M
        Out[2] = (WORD) floor(In[2] * Ratio + 0.5);     // Y

        Out[3] = In[3];                                 // K (untouched)

        return TRUE;
}

// This is a devicelink operating in CMYK for ink-limiting

cmsHPROFILE LCMSEXPORT cmsCreateInkLimitingDeviceLink(icColorSpaceSignature ColorSpace,
                                                        double Limit)
{
       cmsHPROFILE hICC;
       LPLUT Lut;
           
       if (ColorSpace != icSigCmykData) {
            cmsSignalError(LCMS_ERRC_ABORTED, "InkLimiting: Only CMYK currently supported");
            return NULL;
       }

       if (Limit < 0.0 || Limit > 400) {

           cmsSignalError(LCMS_ERRC_WARNING, "InkLimiting: Limit should be between 0..400");        
           if (Limit < 0) Limit = 0;
           if (Limit > 400) Limit = 400;
       
       }

      hICC = _cmsCreateProfilePlaceholder();
       if (!hICC)                          // can't allocate
            return NULL;
              

       cmsSetDeviceClass(hICC,      icSigLinkClass);
       cmsSetColorSpace(hICC,       ColorSpace);
       cmsSetPCS(hICC,              ColorSpace);
       cmsSetRenderingIntent(hICC,  INTENT_PERCEPTUAL); 

      
       // Creates a LUT with 3D grid only
       Lut = cmsAllocLUT();


       cmsAlloc3DGrid(Lut, 17, _cmsChannelsOf(ColorSpace), 
                               _cmsChannelsOf(ColorSpace));

       if (!cmsSample3DGrid(Lut, InkLimitingSampler, (LPVOID) &Limit, 0)) {

                // Shouldn't reach here
                cmsFreeLUT(Lut);
                cmsCloseProfile(hICC);
                return NULL;
       }    
       
       // Create tags
        
       cmsAddTag(hICC, icSigDeviceMfgDescTag,     "(lcms internal)"); 
       cmsAddTag(hICC, icSigProfileDescriptionTag, "lcms ink limiting device link");  
       cmsAddTag(hICC, icSigDeviceModelDescTag,    "ink limiting built-in");      
       
       cmsAddTag(hICC, icSigMediaWhitePointTag, cmsD50_XYZ());

       cmsAddTag(hICC, icSigAToB0Tag, Lut);
       
       // LUT is already on virtual profile
       cmsFreeLUT(Lut);

       // Ok, done
       return hICC;
}



static
LPLUT Create3x3EmptyLUT(void)
{
        LPLUT AToB0 = cmsAllocLUT();
        AToB0 -> InputChan = AToB0 -> OutputChan = 3;

        return AToB0;
}



// Creates a fake Lab identity.
cmsHPROFILE LCMSEXPORT cmsCreateLabProfile(LPcmsCIExyY WhitePoint)
{
        cmsHPROFILE hProfile;        
        LPLUT Lut;


        hProfile = cmsCreateRGBProfile(WhitePoint == NULL ? cmsD50_xyY() : WhitePoint, NULL, NULL);

        cmsSetDeviceClass(hProfile, icSigAbstractClass);
        cmsSetColorSpace(hProfile,  icSigLabData);
        cmsSetPCS(hProfile,         icSigLabData);

        cmsAddTag(hProfile, icSigDeviceMfgDescTag,     "(lcms internal)"); 
        cmsAddTag(hProfile, icSigProfileDescriptionTag, "lcms Lab identity");      
        cmsAddTag(hProfile, icSigDeviceModelDescTag,    "Lab built-in");      


       // An empty LUTs is all we need
       Lut = Create3x3EmptyLUT();
       if (Lut == NULL) return NULL;

       cmsAddTag(hProfile, icSigAToB0Tag,    Lut);
       cmsAddTag(hProfile, icSigBToA0Tag,    Lut);
    
       cmsFreeLUT(Lut);

       return hProfile;
}


// Creates a fake XYZ identity
cmsHPROFILE LCMSEXPORT cmsCreateXYZProfile(void)
{
        cmsHPROFILE hProfile;       
        LPLUT Lut;

        hProfile = cmsCreateRGBProfile(cmsD50_xyY(), NULL, NULL);

        cmsSetDeviceClass(hProfile, icSigAbstractClass);
        cmsSetColorSpace(hProfile, icSigXYZData);
        cmsSetPCS(hProfile,  icSigXYZData);

        cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "(lcms internal)");    
        cmsAddTag(hProfile, icSigProfileDescriptionTag, "lcms XYZ identity");     
        cmsAddTag(hProfile, icSigDeviceModelDescTag,     "XYZ built-in");

       // An empty LUTs is all we need
       Lut = Create3x3EmptyLUT();
       if (Lut == NULL) return NULL;

       cmsAddTag(hProfile, icSigAToB0Tag,    Lut);
       cmsAddTag(hProfile, icSigBToA0Tag,    Lut);
       cmsAddTag(hProfile, icSigPreview0Tag, Lut);

       cmsFreeLUT(Lut);

    
       return hProfile;
}



/*

If  R’sRGB,G’sRGB, B’sRGB < 0.04045

    R =  R’sRGB / 12.92
    G =  G’sRGB / 12.92
    B =  B’sRGB / 12.92

 

else if  R’sRGB,G’sRGB, B’sRGB >= 0.04045

    R = ((R’sRGB + 0.055) / 1.055)^2.4
    G = ((G’sRGB + 0.055) / 1.055)^2.4
    B = ((B’sRGB + 0.055) / 1.055)^2.4

  */

static
LPGAMMATABLE Build_sRGBGamma(void)
{
    double Parameters[5];

    Parameters[0] = 2.4;
    Parameters[1] = 1. / 1.055;
    Parameters[2] = 0.055 / 1.055;
    Parameters[3] = 1. / 12.92;
    Parameters[4] = 0.04045;    // d

    return cmsBuildParametricGamma(1024, 4, Parameters);
}

cmsHPROFILE LCMSEXPORT cmsCreate_sRGBProfile(void)
{
       cmsCIExyY       D65;
       cmsCIExyYTRIPLE Rec709Primaries = {
                                   {0.6400, 0.3300, 1.0},
                                   {0.3000, 0.6000, 1.0},
                                   {0.1500, 0.0600, 1.0}
                                   };
       LPGAMMATABLE Gamma22[3];
       cmsHPROFILE  hsRGB;
 
       cmsWhitePointFromTemp(6504, &D65);
       Gamma22[0] = Gamma22[1] = Gamma22[2] = Build_sRGBGamma();
           
       hsRGB = cmsCreateRGBProfile(&D65, &Rec709Primaries, Gamma22);
       cmsFreeGamma(Gamma22[0]);

      
       cmsAddTag(hsRGB, icSigDeviceMfgDescTag,      "(lcms internal)");
       cmsAddTag(hsRGB, icSigDeviceModelDescTag,    "sRGB built-in");
       cmsAddTag(hsRGB, icSigProfileDescriptionTag, "sRGB built-in");
        
       return hsRGB;
}


