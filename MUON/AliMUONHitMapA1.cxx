/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log$
Revision 1.10  2001/11/02 12:43:21  jchudoba
remove fNdigits data member, it could have wrong value if fDigits is updated. Make this class non-persistent

Revision 1.9  2001/03/20 13:34:05  egangler
Obvious small bug

Revision 1.8  2001/01/26 21:47:21  morsch
Use access functions to AliMUONDigit member data.

Revision 1.7  2001/01/25 11:43:48  morsch
Add a "real" default constructor.

Revision 1.6  2000/10/06 09:05:42  morsch
Correction on upper limits for x- and y- coordinates to make code work with slat chambers.

Revision 1.5  2000/10/02 21:28:09  fca
Removal of useless dependecies via forward declarations

Revision 1.4  2000/07/13 16:19:44  fca
Mainly coding conventions + some small bug fixes

Revision 1.3  2000/07/03 11:54:57  morsch
AliMUONSegmentation and AliMUONHitMap have been replaced by AliSegmentation and AliHitMap in STEER
The methods GetPadIxy and GetPadXxy of AliMUONSegmentation have changed name to GetPadI and GetPadC.

Revision 1.2  2000/06/15 07:58:48  morsch
Code from MUON-dev joined

Revision 1.1.2.2  2000/06/12 07:58:06  morsch
include TMath.h

Revision 1.1.2.1  2000/06/09 22:01:09  morsch
Code from AliMUONHitMap.h
Most coding rule violations corrected.

*/

#include "AliMUONHitMapA1.h"
#include "AliSegmentation.h"
#include "AliMUONResponse.h"
#include "AliMUONDigit.h"

#include <TObjArray.h>
#include <TMath.h>

ClassImp(AliMUONHitMapA1)

    AliMUONHitMapA1::AliMUONHitMapA1()
{
    // Default constructor
    fNpx          = 0;
    fNpy          = 0;
    fMaxIndex     = 0;
    
    fHitMap       = 0;
    fDigits       = 0;
}

AliMUONHitMapA1::AliMUONHitMapA1(AliSegmentation *seg, TObjArray *dig)
{
// Constructor
    fNpx  = seg->Npx()+1;
    fNpy  = seg->Npy()+1;
    fMaxIndex=2*(fNpx+1)*2*(fNpy+1)+2*fNpy;
    
    fHitMap = new Int_t[fMaxIndex];
    fDigits =  dig;
    Clear();
}

AliMUONHitMapA1::AliMUONHitMapA1(const AliMUONHitMapA1 & hitMap)
{
// Dummy copy constructor
    ;
}

 
AliMUONHitMapA1::~AliMUONHitMapA1()
{
// Destructor
    if (fHitMap) delete[] fHitMap;
}

void AliMUONHitMapA1::Clear(const char *)
{
// Clear hitmap
    memset(fHitMap,0,sizeof(int)*fMaxIndex);
}

Bool_t AliMUONHitMapA1::ValidateHit(Int_t ix, Int_t iy)
{
    //
    // Check if pad coordinates are within boundaries
    //
//    printf("\n Validate %d %d %d %d", ix, iy, fNpx, fNpy);
    
    return (TMath::Abs(ix) <= fNpx && TMath::Abs(iy) <= fNpy); 
}

Int_t AliMUONHitMapA1::CheckedIndex(Int_t ix, Int_t iy) const
{
// Return checked indices ix, iy
    Int_t index=2*fNpy*(ix+fNpx)+(iy+fNpy);
    if (index >= fMaxIndex) {
	printf("\n \n \n Try to read/write outside array !!!! \n \n %d %d %d %d %d %d",
	       ix,iy, fMaxIndex, index, fNpx, fNpy);
	return  fMaxIndex-1;
    } else {
	return index;
    }
}

	
void  AliMUONHitMapA1::FillHits()
{
// Fill hits from digits list  
    Int_t ndigits = fDigits->GetEntriesFast();
    //printf("\n Filling hits into HitMap\n");
    //printf("FindRawClusters -- ndigits %d \n",ndigits);
    if (!ndigits) return;
    AliMUONDigit *dig;
    for (Int_t ndig=0; ndig<ndigits; ndig++) {
	dig = (AliMUONDigit*)fDigits->UncheckedAt(ndig);
	SetHit(dig->PadX(),dig->PadY(),ndig);
    }
}


void  AliMUONHitMapA1::SetHit(Int_t ix, Int_t iy, Int_t idigit)
{
// Assign digit to hit cell ix,iy
//    fHitMap[kMaxNpady*(ix+fNpx)+(iy+fNpy)]=idigit+1;
    fHitMap[CheckedIndex(ix, iy)]=idigit+1;
}

void AliMUONHitMapA1::DeleteHit(Int_t ix, Int_t iy)
{
// Delete hit at cell ix,iy
//    fHitMap[kMaxNpady*(ix+fNpx)+(iy+fNpy)]=0;
    fHitMap[CheckedIndex(ix, iy)]=0;
}

void AliMUONHitMapA1::FlagHit(Int_t ix, Int_t iy)
{
// Flag hit as used
    fHitMap[CheckedIndex(ix, iy)]=
	-TMath::Abs(fHitMap[CheckedIndex(ix, iy)]);
}

Int_t AliMUONHitMapA1::GetHitIndex(Int_t ix, Int_t iy) const
{
// Get absolute value of contents of hit cell ix,iy
    return TMath::Abs(fHitMap[CheckedIndex(ix, iy)])-1;
}

TObject* AliMUONHitMapA1::GetHit(Int_t ix, Int_t iy) const
{
    // Get pointer to object at hit cell ix, iy
    // Force crash if index does not exist ! (Manu)
    Int_t index=GetHitIndex(ix,iy);
    return (index <0) ? 0 : fDigits->UncheckedAt(GetHitIndex(ix,iy));
}

FlagType AliMUONHitMapA1::TestHit(Int_t ix, Int_t iy)
{
// Check if hit cell is empty, used or unused
//
    Int_t inf=fHitMap[CheckedIndex(ix, iy)];
    if (inf < 0) {
	return kUsed;
    } else if (inf == 0) {
	return kEmpty;
    } else {
	return kUnused;
    }
}

AliMUONHitMapA1 & AliMUONHitMapA1::operator = (const AliMUONHitMapA1 & rhs) 
{
// Dummy assignment operator
    return *this;
}





