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


#define GRID_POINTS 33

static
int Forward(register WORD In[], register WORD Out[], register LPVOID Cargo)
{	
    cmsCIELab Lab;

   

    cmsLabEncoded2Float(&Lab, In);
    Lab.a = Lab.b = 0;
    cmsFloat2LabEncoded(Out, &Lab);
	
	return TRUE;
}




	
int main(int argc, char *argv[])
{
	LPLUT AToB0;
	cmsHPROFILE hProfile;

	fprintf(stderr, "Creating grayer.icm...");

	unlink("grayer.icm");
	hProfile = cmsOpenProfileFromFile("grayer.icm", "w");


    AToB0 = cmsAllocLUT();
	 

	cmsAlloc3DGrid(AToB0, GRID_POINTS, 3, 3);
	    
	cmsSample3DGrid(AToB0, Forward, NULL, 0);
			
    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	                                
	cmsSetColorSpace(hProfile, icSigLabData);
    cmsSetPCS(hProfile, icSigLabData);
    cmsSetDeviceClass(hProfile, icSigAbstractClass);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "Little cms Grayifier");
    cmsAddTag(hProfile, icSigCopyrightTag,          "Copyright (c) Marti Maria 2003. All rights reserved.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "Grayifier abstract profile");

	
	cmsCloseProfile(hProfile);
    
	cmsFreeLUT(AToB0);
	
	fprintf(stderr, "Done.\n");

	return 0;
}
