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


#include "lcms.h"

// This is a sample on how to build a profile for decoding ITU T.42/Fax JPEG
// streams. The profile has an additional ability in the input direction of
// gamut compress values between 85 < a < -85 and -75 < b < 125. This conforms
// the default range for ITU/T.42 -- See RFC 2301, section 6.2.3 for details


//  L*	=	[0, 100]
//	a*	=	[–85, 85]
//	b*	=	[–75, 125]


// These functions does convert the encoding of ITUFAX to floating point

static
void ITU2Lab(WORD In[3], LPcmsCIELab Lab)
{
   Lab -> L = (double) In[0] / 655.35;
   Lab -> a = (double) 170.* (In[1] - 32768.) / 65535.;
   Lab -> b = (double) 200.* (In[2] - 24576.) / 65535.;
}


static
void Lab2ITU(LPcmsCIELab Lab, WORD Out[3])
{
	Out[0] = (WORD) floor((double) (Lab -> L / 100.)* 65535. + 0.5);
    Out[1] = (WORD) floor((double) (Lab -> a / 170.)* 65535. + 32768. + 0.5);
    Out[2] = (WORD) floor((double) (Lab -> b / 200.)* 65535. + 24576. + 0.5);
}


// These are the samplers-- They are passed as callbacks to cmsSample3DGrid()
// then, cmsSample3DGrid() will sweel whole Lab gamut calling these functions
// once for each node. In[] will contain the Lab PCS value to convert to ITUFAX
// on InputDirection, or the ITUFAX value to convert to Lab in OutputDirection
// You can change the number of sample points if desired, the algorithm will
// remain same. 33 points gives good accurancy, but you can reduce to 22 or less
// is space is critical

#define GRID_POINTS 33

static
int InputDirection(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	   
    cmsCIELab Lab;

    cmsLabEncoded2Float(&Lab, In);    
    cmsClampLab(&Lab, 85, -85, 125, -75);    // This function does the necessary gamut remapping  
    Lab2ITU(&Lab, Out);

	return TRUE;
}


static
int OutputDirection(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	

	cmsCIELab Lab;

    ITU2Lab(In, &Lab);
    cmsFloat2LabEncoded(Out, &Lab);    

	return TRUE;
}


// The main entry point. Just create a profile an populate it with required tags.
// note that cmsOpenProfileFromFile("itufax.icm", "w") will NOT delete the file
// if already exists. This is for obvious safety reasons.

	
int main(int argc, char *argv[])
{
	LPLUT AToB0, BToA0;
	cmsHPROFILE hProfile;

	fprintf(stderr, "Creating itufax.icm...");

	unlink("itufax.icm");
	hProfile = cmsOpenProfileFromFile("itufax.icm", "w");
	
    AToB0 = cmsAllocLUT();
	BToA0 = cmsAllocLUT(); 

	cmsAlloc3DGrid(AToB0, GRID_POINTS, 3, 3);
	cmsAlloc3DGrid(BToA0, GRID_POINTS, 3, 3);
    
	cmsSample3DGrid(AToB0, InputDirection, NULL, 0);
	cmsSample3DGrid(BToA0, OutputDirection, NULL, 0);
		
    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	cmsAddTag(hProfile, icSigBToA0Tag, BToA0);

                                
	cmsSetColorSpace(hProfile, icSigLabData);
    cmsSetPCS(hProfile, icSigLabData);
    cmsSetDeviceClass(hProfile, icSigColorSpaceClass);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "ITU T.42/Fax JPEG CIEL*a*b*");
    cmsAddTag(hProfile, icSigCopyrightTag,          "No Copyright, use freely.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "ITU T.42/Fax JPEG CIEL*a*b*");
	
	cmsCloseProfile(hProfile);
    
	cmsFreeLUT(AToB0);
	cmsFreeLUT(BToA0);

	fprintf(stderr, "Done.\n");

	return 0;
}
