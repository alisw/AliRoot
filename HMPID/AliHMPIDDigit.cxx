//  **************************************************************************
//  * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
//  *                                                                        *
//  * Author: The ALICE Off-line Project.                                    *
//  * Contributors are mentioned in the code where appropriate.              *
//  *                                                                        *
//  * Permission to use, copy, modify and distribute this software and its   *
//  * documentation strictly for non-commercial purposes is hereby granted   *
//  * without fee, provided that the above copyright notice appears in all   *
//  * copies and that both the copyright notice and this permission notice   *
//  * appear in the supporting documentation. The authors make no claims     *
//  * about the suitability of this software for any purpose. It is          *
//  * provided "as is" without express or implied warranty.                  *
//  **************************************************************************

#include "AliHMPIDDigit.h"    //class header
#include <TClonesArray.h>     //WriteRaw() 
#include <TBox.h>             //Draw() 
#include <AliRawDataHeader.h> //WriteRaw()
#include <AliDAQ.h>           //WriteRaw()
#include <Riostream.h>        //WriteRaw()
ClassImp(AliHMPIDDigit)

const Float_t AliHMPIDDigit::fMinPcX[]={ 0.00 ,  66.60 ,   0.00 ,  66.60 ,  0.00 ,  66.60};
const Float_t AliHMPIDDigit::fMaxPcX[]={64.00 , 130.60 ,  64.00 , 130.60 , 64.00 , 130.60};

const Float_t AliHMPIDDigit::fMinPcY[]={ 0.00 , 0.00 , 42.92 ,  42.92,  85.84,  85.84};
const Float_t AliHMPIDDigit::fMaxPcY[]={40.32 , 40.32, 83.24 ,  83.24, 126.16, 126.16};

const Float_t  AliHMPIDDigit::k1      =0.28278796;
const Float_t  AliHMPIDDigit::k2      =0.96242952;
const Float_t  AliHMPIDDigit::kSqrtK3 =0.77459667;
const Float_t  AliHMPIDDigit::k4      =0.37932926;

Int_t AliHMPIDDigit::fSigmas=4;


/*
 Preface: all geometrical information (like left-right sides) is reported as seen from electronic side.
 
     d10          d1    d10          d1 
      _______________    _______________          
r24  |               |  |               |126.16   r1
     |       4       |  |       5       | 
     |               |  |               | 
     |_______________|  |_______________|85.84 
      _______________    _______________
     |               |  |               |83.24  
     |       2       |  |       3       |
     |               |  |               |             
     |_______________|  |_______________|42.92
      _______________    _______________
     |               |  |               |40.32
     |       0       |  |       1       |
     |               |  |               |
r1   |_______________|  |_______________|0        r24
      0           64.0  66.6        130.6 



The DDL file starts with common header which size and structure is standartized and mandatory for all detectors. 
The header contains among other words, so called Equipment ID word. This unique value for each D-RORC is calculated as detector ID << 8 + DDL index. 
For HMPID the detector ID is 6 (reffered in the code as kRichRawId) while DDL indexes are from 0 to 13.

Common header might be followed by the private one although  HMPID has no any private header, just uses the common one.

Single HMPID D-RORC (with 2 channels) serves a single chamber so that channel 0 serves left half (PCs 0-2-4) 
                                                                              1 serves right half(PCs 1-3-5) 

So the LDC -chamber-ddl map is:
DDL index  0 -> ch 0 left -> DDL ID 0x600          DDL index  1 -> ch 1 right -> DDL ID 0x601 
DDL index  2 -> ch 1 left -> DDL ID 0x602          DDL index  3 -> ch 2 right -> DDL ID 0x603 
DDL index  4 -> ch 2 left -> DDL ID 0x604          DDL index  5 -> ch 3 right -> DDL ID 0x605 
DDL index  6 -> ch 3 left -> DDL ID 0x606          DDL index  7 -> ch 4 right -> DDL ID 0x607 
DDL index  8 -> ch 4 left -> DDL ID 0x608          DDL index  9 -> ch 5 right -> DDL ID 0x609 
DDL index 10 -> ch 5 left -> DDL ID 0x60a          DDL index 11 -> ch 6 right -> DDL ID 0x60b 
DDL index 12 -> ch 6 left -> DDL ID 0x60c          DDL index 13 -> ch 7 right -> DDL ID 0x60d 

HMPID FEE as seen by single D-RORC is composed from a number of DILOGIC chips organized in vertical stack of rows. 
Each DILOGIC chip serves 48 channels for the 8x6 pads Channels counted from 0 to 47.

The mapping inside DILOGIC chip has the following structure (see from electronics side):
pady

5  04 10 16 22 28 34 40 46                   due to repetition in column structure we may introduce per column map:   
4  02 08 14 20 26 32 38 44                   pady= 0 1 2 3 4 5          
3  00 06 12 18 24 30 36 42                   addr= 5 3 1 0 2 4
2  01 07 13 19 25 31 37 43                   or vice versa 
1  03 09 15 21 27 33 39 45                   addr= 0 1 2 3 4 5
0  05 11 17 23 29 35 41 47                   pady= 3 2 4 1 5 0  
    
    0  1  2  3  4  5  6  7  padx

10 DILOGIC chips composes so called "row" in horizontal direction (reffered in the code as kNdil), so the row is 80x6 pads structure. 
DILOGIC chips in the row are counted from right to left as seen from electronics side, from 1 to 10.
24 rows are piled up forming the whole FEE served by single D-RORC, so one DDL sees 80x144 pads separated in 3 photocathodes.
Rows are counted from 1 to 24 from top    to bottom for right  half of the chamber (PCs 1-3-5) as seen from electronics side, meaning even LDC number
                          and from bottom to top    for left   half of the chamber (PCs 0-2-4) as seen from electronics side, meaning odd LDC number.

HMPID raw word is 32 bits with the structure:   
   00000             rrrrr                      dddd                               aaaaaa                          qqqqqqqqqqqq 
 5 bits zero  5 bits row number (1..24)  4 bits DILOGIC chip number (1..10) 6 bits DILOGIC address (0..47)  12 bits QDC value (0..4095)
*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDDigit::Draw(Option_t*)
{
//  TMarker *pMark=new TMarker(LorsX(),LorsY(),25); pMark->SetMarkerColor(kGreen);pMark->Draw();
  TBox *pad = new TBox(LorsX()-0.5*SizePadX(),LorsY()-0.5*SizePadY(),LorsX()+0.5*SizePadX(),LorsY()+0.5*SizePadY());
  Int_t slice=(Int_t)fQ/20;
  switch(slice){
    case 0: pad->SetFillColor(kBlue); break;
    case 1: pad->SetFillColor(kBlue+4); break;
    case 2: pad->SetFillColor(kCyan); break;
    case 3: pad->SetFillColor(kCyan+4); break;
    case 4: pad->SetFillColor(kGreen); break;
    case 5: pad->SetFillColor(kGreen+4); break;
    case 6: pad->SetFillColor(kYellow); break;
    case 7: pad->SetFillColor(kYellow+4); break;
    default: pad->SetFillColor(kRed);      break;
  }
   pad->SetUniqueID((Int_t)fQ);
  pad->Draw();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDDigit::Print(Option_t *opt)const
{
// Print current digit  
// Arguments: option string not used
//   Returns: none    
  UInt_t w32; Int_t ddl,r,d,a;
  Raw(w32,ddl,r,d,a);
  Printf("%sDIG:(ch=%1i,pc=%1i,x=%2i,y=%2i) (%7.3f,%7.3f) Q=%8.3f TID=(%5i,%5i,%5i) raw=0x%x (ddl=%2i,r=%2i,d=%2i,a=%2i) %s",
          opt,  A2C(fPad),A2P(fPad),A2X(fPad),A2Y(fPad),LorsX(),LorsY(), Q(),  fTracks[0],fTracks[1],fTracks[2],w32,ddl,r,d,a, (IsOverTh(Q()))?"":"below thr");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDDigit::WriteRaw(TObjArray *pDigAll)
{
// Write a list of digits for a given chamber in raw data stream
// Arguments: pDigAll- list of digits 
//   Returns: none      
  ofstream ddlL,ddlR;                               //output streams, 2 per chamber
  Int_t    cntL=0,cntR=0;                           //data words counters for DDLs
  AliRawDataHeader header; header.SetAttribute(0);  //empty DDL header

  UInt_t w32=0; Int_t ddl,r,d,a;            //32 bits data word 
  for(Int_t iCh=kMinCh;iCh<=kMaxCh;iCh++){//chambers loop
    ddlL.open(AliDAQ::DdlFileName("HMPID",2*iCh)); 
    ddlR.open(AliDAQ::DdlFileName("HMPID",2*iCh+1));      //open both DDL of this chamber in parallel
    ddlL.write((char*)&header,sizeof(header));            //write dummy header as place holder, actual 
    ddlR.write((char*)&header,sizeof(header));            //will be rewritten later when total size of DDL is known
  
    TClonesArray *pDigCh=(TClonesArray *)pDigAll->At(iCh); //list of digits for current chamber 
    for(Int_t iDig=0;iDig<pDigCh->GetEntriesFast();iDig++){//digits loop
      AliHMPIDDigit *pDig=(AliHMPIDDigit*)pDigCh->At(iDig);
      pDig->Raw(w32,ddl,r,d,a);                             
      if(ddl%2){
        ddlL.write((char*)&w32,sizeof(w32));  cntL++;
      }else{
        ddlR.write((char*)&w32,sizeof(w32));  cntR++;
      }
    }//digits  loop

    header.fSize=sizeof(header)+cntL*sizeof(w32); ddlL.seekp(0); ddlL.write((char*)&header,sizeof(header)); ddlL.close(); //rewrite header with size set to
    header.fSize=sizeof(header)+cntR*sizeof(w32); ddlR.seekp(0); ddlR.write((char*)&header,sizeof(header)); ddlR.close(); //number of bytes and close file
  }//chambers loop
}//WriteRaw()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
