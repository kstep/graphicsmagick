//
//  Little cms 
//  Copyright (C) 1998-2002 Marti Maria
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



static
double DecodeAbTIFF(double ab)
{
	if (ab <= 128.)
		ab += 127.;
	else
		ab -= 127.;

	return ab;
}


static
LPGAMMATABLE CreateStep(void)
{
	LPGAMMATABLE Gamma = cmsAllocGamma(4096);
	LPWORD Table = Gamma ->GammaTable;
	int i;
	double a;

	for (i=0; i < 4096; i++) {

		a = (double) i * 255. / 4095.;

		a = DecodeAbTIFF(a);

		Table[i] = (WORD) floor(a * 257. + 0.5);
	}

	return Gamma;
}


static
LPGAMMATABLE CreateLinear()
{
	LPGAMMATABLE Gamma = cmsAllocGamma(4096);
	LPWORD Table = Gamma ->GammaTable;
	int i;

       for (i=0; i < 4096; i++) {

		   Table[i] = _cmsQuantizeVal(i, 4096);              		   

       }
       return Gamma;
}


static
int Identity(register WORD In[], register WORD Out[], register LPVOID Cargo)
{
	Out[0] = In[0];
	Out[1] = In[1];
	Out[2] = In[2];
	return TRUE;
}




int main(int argc, char *argv[])
{
	cmsHPROFILE hProfile;
	LPLUT AToB0, BToA0;
	LPGAMMATABLE PreLinear[3];
	LPGAMMATABLE Lin, Step;

	fprintf(stderr, "Creating lcmstiff8.icm...");

    
    unlink("lcmstiff8.icm");
	hProfile = cmsOpenProfileFromFile("lcmstiff8.icm", "w");

	// Create linearization
	Lin  = CreateLinear();
	Step = CreateStep();

	PreLinear[0] = Lin;
	PreLinear[1] = Step;
	PreLinear[2] = Step;

    AToB0 = cmsAllocLUT();
	BToA0 = cmsAllocLUT();

	cmsAlloc3DGrid(AToB0, 2, 3, 3);
	cmsAlloc3DGrid(BToA0, 2, 3, 3);

	cmsSample3DGrid(AToB0, Identity, NULL, 0);
	cmsSample3DGrid(BToA0, Identity, NULL, 0);
	
	cmsAllocLinearTable(AToB0, PreLinear,  1);   
	cmsAllocLinearTable(BToA0, PreLinear,  2);   

    cmsAddTag(hProfile, icSigAToB0Tag, AToB0);
	cmsAddTag(hProfile, icSigBToA0Tag, BToA0);

	cmsAddTag(hProfile, icSigProfileDescriptionTag, "Little cms Tiff8 CIELab");
    cmsAddTag(hProfile, icSigCopyrightTag,          "Copyright (c) Marti Maria, 2002. All rights reserved.");
    cmsAddTag(hProfile, icSigDeviceMfgDescTag,      "Little cms");    
    cmsAddTag(hProfile, icSigDeviceModelDescTag,    "TIFF Lab8");
	

	cmsCloseProfile(hProfile);

	cmsFreeGamma(Lin);
	cmsFreeGamma(Step);
	cmsFreeLUT(AToB0);
	cmsFreeLUT(BToA0);

	
	fprintf(stderr, "Done.\n");

	return 0;
}
