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

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
///
/// This class provides access to ITS SDD digits in raw data.
///
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStreamSDD.h"
#include "AliRawReader.h"
#include "AliLog.h"

ClassImp(AliITSRawStreamSDD)

const Int_t AliITSRawStreamSDD::fgkDDLModuleMap[kDDLsNumber][kModulesPerDDL] = {
 
  {240,241,242,246,247,248,252,253,254,258,259,260},
  {264,265,266,270,271,272,276,277,278,282,283,284},
  {288,289,290,294,295,296,300,301,302,306,307,308},
  {312,313,314,318,319,320,-1,-1,-1,-1,-1,-1},
  {243,244,245,249,250,251,255,256,257,261,262,263},
  {267,268,269,273,274,275,279,280,281,285,286,287},
  {291,292,293,297,298,299,303,304,305,309,310,311},
  {315,316,317,321,322,323,-1,-1,-1,-1,-1,-1},
  {324,325,326,327,332,333,334,335,340,341,342,343},
  {348,349,350,351,356,357,358,359,364,365,366,367},
  {372,373,374,375,380,381,382,383,388,389,390,391},
  {396,397,398,399,404,405,406,407,412,413,414,415},
  {420,421,422,423,428,429,430,431,436,437,438,439},
  {444,445,446,447,452,453,454,455,460,461,462,463},
  {468,469,470,471,476,477,478,479,484,485,486,487},
  {492,493,494,495,-1,-1,-1,-1,-1,-1,-1,-1},
  {328,329,330,331,336,337,338,339,344,345,346,347},
  {352,353,354,355,360,361,362,363,368,369,370,371},
  {376,377,378,379,384,385,386,387,392,393,394,395},
  {400,401,402,403,408,409,410,411,416,417,418,419},
  {424,425,426,427,432,433,434,435,440,441,442,443},
  {448,449,450,451,456,457,458,459,464,465,466,467},
  {472,473,474,475,480,481,482,483,488,489,490,491},
  {496,497,498,499,-1,-1,-1,-1,-1,-1,-1,-1}};
  
const UInt_t AliITSRawStreamSDD::fgkCodeLength[8] =  {8, 18, 2, 3, 4, 5, 6, 7};

AliITSRawStreamSDD::AliITSRawStreamSDD(AliRawReader* rawReader) :
  AliITSRawStream(rawReader),
fData(0),
fCarlosId(0),
fEventId(0),
fChannel(0),
fJitter(0),
fNCarlos(kModulesPerDDL),
fNfifo0(0),
fNfifo1(0),
fNfifo2(0),
fNfifo3(0),
fDDL(0){
// create an object to read ITS SDD raw digits
  
  for(Int_t i=0;i<2;i++){
    for(Int_t ic=0;ic<kModulesPerDDL;ic++){
      fChannelData[ic][i]=0;
      fLastBit[ic][i]=0;
      fChannelCode[ic][i]=0;
      fReadCode[ic][i]=kFALSE;
      fReadBits[ic][i]=0;
      fTimeBin[ic][i]=0;
      fAnode[ic][i]=0;     
    }
    fLowThreshold[i]=0;
  }
  for(Int_t i=0;i<kDDLsNumber;i++) fSkip[i]=0;
  fRawReader->Reset();
  fRawReader->Select("ITSSDD");

  //fRawReader->SelectEquipment(17, 101, 101);//select this for test data
  //fNCarlos = 8; //select this for test data
}

UInt_t AliITSRawStreamSDD::ReadBits()
{
// read bits from the given channel

  UInt_t result = (fChannelData[fCarlosId][fChannel] & ((1<<fReadBits[fCarlosId][fChannel]) - 1));
  fChannelData[fCarlosId][fChannel] >>= fReadBits[fCarlosId][fChannel]; 
  fLastBit[fCarlosId][fChannel] -= fReadBits[fCarlosId][fChannel];
  return result;
}

Int_t AliITSRawStreamSDD::DecompAmbra(Int_t value) const
{
  // AMBRA decompression (from 8 to 10 bit)
  
  if ((value & 0x80) == 0) {
    return value & 0x7f;
  } else if ((value & 0x40) == 0) {
    return 0x081 + ((value & 0x3f) << 1);
  } else if ((value & 0x20) == 0) {
    return 0x104 + ((value & 0x1f) << 3);
  } else {
    return 0x208 + ((value & 0x1f) << 4);
  }
  
}

Bool_t AliITSRawStreamSDD::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left

  fPrevModuleID = fModuleID;
  fDDL=fRawReader->GetDDLID();
  Int_t ddln = fRawReader->GetDDLID();
  if(ddln <0) ddln=0;
  while (fSkip[ddln] < 9) {
    if (!fRawReader->ReadNextInt(fData)) return kFALSE;
    if ((fData >> 30) == 0x01) continue;  // JTAG word
    fSkip[ddln]++;
  }
  

  Int_t countFoot[kModulesPerDDL];
  for(Int_t i=0;i<kModulesPerDDL;i++) countFoot[i]=0;
  while (kTRUE) {
    if ((fChannel < 0) || (fLastBit[fCarlosId][fChannel] < fReadBits[fCarlosId][fChannel])) {

      if (!fRawReader->ReadNextInt(fData)) return kFALSE;  // read next word 
      ddln = fRawReader->GetDDLID();
      if(ddln!=fDDL){
	Reset();
	for(Int_t icr=0;icr<kModulesPerDDL;icr++) countFoot[icr]=0;
      }
      if(ddln < 0 || ddln > (kDDLsNumber-1)) ddln  = 0;
      while (fSkip[ddln] < 9) {
	if (!fRawReader->ReadNextInt(fData)) return kFALSE;
	if ((fData >> 30) == 0x01) continue;  // JTAG word
	fSkip[ddln]++;
      }

      fChannel = -1;
      
      
      if( ((fData << 4) >> 4) == 0){
	fCarlosId = 0;fNfifo0 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 1){
	fCarlosId = 1;fNfifo0 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 2){
	fCarlosId = 2;
	if(fNCarlos == 8) fNfifo1 = fCarlosId;
	else fNfifo0 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 3){
	fCarlosId = 3;fNfifo1 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 4){
	fCarlosId = 4;
	if(fNCarlos == 8) fNfifo2 = fCarlosId;
	else fNfifo1 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 5){
	fCarlosId = 5;
	if(fNCarlos == 8) fNfifo2 = fCarlosId;
	else fNfifo1 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 6){
	fCarlosId = 6;
	if(fNCarlos == 8) fNfifo3 = fCarlosId;
	else fNfifo2 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 7){
	fCarlosId = 7;
	if(fNCarlos == 8) fNfifo3 = fCarlosId;
	else fNfifo2 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 8){
	fCarlosId = 8;fNfifo2 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 9){
	fCarlosId = 9;fNfifo3 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 10){
	fCarlosId = 10;fNfifo3 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 11){
	fCarlosId = 11;fNfifo3 = fCarlosId;
      }
      else if (((fData << 4) >> 4) == 16){
	fCarlosId = fNfifo0;
      }
      else if (((fData << 4) >> 4) == 17){
	fCarlosId = fNfifo1;
      }
      else if (((fData << 4) >> 4) == 18){
	fCarlosId = fNfifo2;
      }
      else if (((fData << 4) >> 4) == 19){
	fCarlosId = fNfifo3;
      }
      if(fData==1059004191) continue;
      if (fNCarlos == 8 && (fCarlosId == 8 || fCarlosId == 9 || 
			    fCarlosId ==10 || fCarlosId == 11))continue;
      
      fModuleID = fgkDDLModuleMap[fRawReader->GetDDLID()][fCarlosId];
      
      if ((fData >> 28) == 0x02) {           // header
	fEventId = (fData >> 3) & 0x07FF;
      } else if ((fData >> 28) == 0x03) {    // footer
        countFoot[fCarlosId]++; // stop before the last word (last word=jitter)
        if(countFoot[fCarlosId]==3){
	  return kFALSE;
	}	 
      } else if ((fData >> 29) == 0x00) {    // error

	if ((fData & 0x00000163) != 0) {
	  fRawReader->AddMajorErrorLog(kDataError,Form("Error code = %8.8x",fData));	 
	  AliWarning(Form("error codes = %8.8x",fData));
	  return kFALSE; 
	  
	}
      } else if ((fData >> 30) == 0x01) {    // JTAG word
	// ignored
      } else if ((fData >> 30) == 0x02) {    // channel 0 data
	
	fChannel = 0;
      } else if ((fData >> 30) == 0x03) {    // channel 1 data
	fChannel = 1;
      } else {                               // unknown data format
	fRawReader->AddMajorErrorLog(kDataFormatErr,Form("Invalid data %8.8x",fData));
	AliWarning(Form("invalid data: %8.8x\n", fData));
	return kFALSE;
      }
      
      if (fChannel >= 0) {   // add read word to the data
	fChannelData[fCarlosId][fChannel] += 
	  (ULong64_t(fData & 0x3FFFFFFF) << fLastBit[fCarlosId][fChannel]);
	fLastBit[fCarlosId][fChannel] += 30;
      }

    } else {  // decode data

      if (fReadCode[fCarlosId][fChannel]) {// read the next code word
	fChannelCode[fCarlosId][fChannel] = ReadBits();
	fReadCode[fCarlosId][fChannel] = kFALSE;
	fReadBits[fCarlosId][fChannel] = fgkCodeLength[fChannelCode[fCarlosId][fChannel]];

      } else {                      // read the next data word
	UInt_t data = ReadBits();
	fReadCode[fCarlosId][fChannel] = kTRUE;
	fReadBits[fCarlosId][fChannel] = 3;
	if (fChannelCode[fCarlosId][fChannel] == 0) {         // set the time bin	  
	  fTimeBin[fCarlosId][fChannel] = data;
	} else if (fChannelCode[fCarlosId][fChannel] == 1) {  // next anode
	  fTimeBin[fCarlosId][fChannel] = 0;
	  fAnode[fCarlosId][fChannel]++;

	}else {                                   // ADC signal data
	  fSignal = DecompAmbra(data + (1 << fChannelCode[fCarlosId][fChannel]) + fLowThreshold[fChannel]);
	  fCoord1 = fAnode[fCarlosId][fChannel];
	  fCoord2 = fTimeBin[fCarlosId][fChannel];
	  fTimeBin[fCarlosId][fChannel]++;
	  return kTRUE;
	}
      }
    }
  }
  return kFALSE;  

}

void AliITSRawStreamSDD::Reset(){
  //reset data member for a new ddl
  
  for(Int_t i=0;i<2;i++){
    for(Int_t ic=0;ic<kModulesPerDDL;ic++){
      fChannelData[ic][i]=0;
      fLastBit[ic][i]=0;
      fChannelCode[ic][i]=0;
      fReadCode[ic][i]=kFALSE;
      fReadBits[ic][i]=0;
      fTimeBin[ic][i]=0;
      fAnode[ic][i]=0;     
    }
    fLowThreshold[i]=0;
  }
  fChannel=-1;
  fDDL=fRawReader->GetDDLID();
}
