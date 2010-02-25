/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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

/********************************** 
 * create an event and perform    *
 * flow analysis 'on the fly'     * 
 *                                * 
 * authors: Raimond Snellings     *
 *           (snelling@nikhef.nl) * 
 *          Ante Bilandzic        * 
 *           (anteb@nikhef.nl)    *
 *********************************/
  
#include "Riostream.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "AliFlowEventSimpleMakerOnTheFly.h"
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"

ClassImp(AliFlowEventSimpleMakerOnTheFly)


//========================================================================


AliFlowEventSimpleMakerOnTheFly::AliFlowEventSimpleMakerOnTheFly(UInt_t iseed):
  fMultDistrOfRPsIsGauss(kFALSE),
  fMultiplicityOfRP(0),
  fMultiplicitySpreadOfRP(0.),
  fMinMultOfRP(0),
  fMaxMultOfRP(0),  
  fTemperatureOfRP(0.),  
  fPtDependentHarmonics(kFALSE),
  fEtaDependentHarmonics(kFALSE),
  fV1RP(0.), 
  fV1SpreadRP(0.), 
  fConstantV2IsSampledFromGauss(kFALSE),
  fV2RP(0.), 
  fV2SpreadRP(0.), 
  fMinV2RP(0.),
  fMaxV2RP(0.),
  fV4RP(0.), 
  fV4SpreadRP(0.), 
  fV2vsPtEtaMax(0.),
  fPtCutOff(0.),
  fV2vsEtaSpread(0.),
  fPhiMin1(0.),              
  fPhiMax1(0.),             
  fProbability1(0.),       
  fPhiMin2(0.),   
  fPhiMax2(0.),            
  fProbability2(0.),          
  fPtSpectra(NULL),
  fPhiDistribution(NULL),
  fMyTRandom3(NULL),
  fCount(0),
  fNoOfLoops(1),
  fPhiRange(0.),
  fPtRange(0.),
  fEtaRange(0.),
  fNonflowSectorMin(0.),
  fNonflowSectorMax(TMath::TwoPi()),
  fEtaMinA(-1.0),
  fEtaMaxA(-0.01),
  fEtaMinB(0.01),
  fEtaMaxB(1.0)
{
  // constructor
  fMyTRandom3 = new TRandom3(iseed);   
  gRandom->SetSeed(fMyTRandom3->Integer(65539));
}


//========================================================================


AliFlowEventSimpleMakerOnTheFly::~AliFlowEventSimpleMakerOnTheFly()
{
 // destructor
 if (fPtSpectra) delete fPtSpectra;
 if (fPhiDistribution) delete fPhiDistribution;
 if (fMyTRandom3) delete fMyTRandom3;
}	


//========================================================================


void AliFlowEventSimpleMakerOnTheFly::Init()
{
 // define the pt spectra and phi distribution

 // pt spectra of pions (Boltzman):   
 Double_t dPtMin = 0.; // to be improved (move this to the body of contstructor?)
 Double_t dPtMax = 10.; // to be improved (move this to the body of contstructor?) 
  
 fPtSpectra = new TF1("fPtSpectra","[0]*x*TMath::Exp(-pow(0.13957*0.13957+x*x,0.5)/[1])",dPtMin,dPtMax);  
 fPtSpectra->SetParName(0,"Multiplicity of RPs");  
 fPtSpectra->SetParName(1,"Temperature of RPs");
 
 // phi distribution:
 Double_t dPhiMin = 0.; // to be improved (move this to the body of contstructor?)
 Double_t dPhiMax = TMath::TwoPi(); // to be improved (move this to the body of contstructor?)
  
 fPhiDistribution = new TF1("fPhiDistribution","1+2.*[0]*TMath::Cos(x-[2])+2.*[1]*TMath::Cos(2*(x-[2]))+2.*[3]*TMath::Cos(4*(x-[2]))",dPhiMin,dPhiMax);
 fPhiDistribution->SetParName(0,"directed flow");
 fPhiDistribution->SetParName(1,"elliptic flow"); 
 fPhiDistribution->SetParName(2,"Reaction Plane");
 fPhiDistribution->SetParName(3,"harmonic 4"); // to be improved (name)

}

//========================================================================

AliFlowEventSimple* AliFlowEventSimpleMakerOnTheFly::CreateEventOnTheFly(AliFlowTrackSimpleCuts *cutsRP, AliFlowTrackSimpleCuts *cutsPOI)
{
  // method to create event on the fly
  
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(fMultiplicityOfRP);
   
  // sampling the multiplicity:
  Int_t iNewMultiplicityOfRP = fMultiplicityOfRP;
  
  if(fMultDistrOfRPsIsGauss) {
    if (fMultiplicitySpreadOfRP>0.0) iNewMultiplicityOfRP = (Int_t)fMyTRandom3->Gaus(fMultiplicityOfRP,fMultiplicitySpreadOfRP);
    fPtSpectra->SetParameter(0,iNewMultiplicityOfRP);
  } else {
    if (fMinMultOfRP != fMaxMultOfRP) {
      iNewMultiplicityOfRP = (Int_t)fMyTRandom3->Uniform(fMinMultOfRP,fMaxMultOfRP);
      fPtSpectra->SetParameter(0,iNewMultiplicityOfRP);
    } else {
      fPtSpectra->SetParameter(0,fMinMultOfRP);
    }
  }
  
  // set the 'temperature' of RPs
  fPtSpectra->SetParameter(1,fTemperatureOfRP);  
  
  // sampling the reaction plane
  Double_t dMCReactionPlaneAngle = fMyTRandom3->Uniform(0.,TMath::TwoPi());
  fPhiDistribution->SetParameter(2,dMCReactionPlaneAngle);
  
  // sampling the V1:
  Double_t dNewV1RP=fV1RP;
  if(fV1SpreadRP>0.0) {dNewV1RP = fMyTRandom3->Gaus(fV1RP,fV1SpreadRP);}
  fPhiDistribution->SetParameter(0,dNewV1RP);
  
  // sampling the V2:
  if(!(fPtDependentHarmonics || fEtaDependentHarmonics)) 
  {
   Double_t dNewV2RP = fV2RP;
   if(fConstantV2IsSampledFromGauss) 
   {
    if(fV2SpreadRP>0.0) 
    {
     dNewV2RP = fMyTRandom3->Gaus(fV2RP,fV2SpreadRP);
    }
    fPhiDistribution->SetParameter(1,dNewV2RP);
   } else if(fMinV2RP < fMaxV2RP) 
     {
      dNewV2RP = fMyTRandom3->Uniform(fMinV2RP,fMaxV2RP);    
	   fPhiDistribution->SetParameter(1,dNewV2RP);  
     } else if(fMinV2RP == fMaxV2RP)
        {
	      dNewV2RP = fMinV2RP;
	      fPhiDistribution->SetParameter(1,dNewV2RP);          
        }
  } // end of if(!(bPtDependentHarmonics || bEtaDependentHarmonics))
  
  // sampling the V4:
  Double_t dNewV4RP = fV4RP;
  if(fV4SpreadRP>0.0) dNewV4RP = fMyTRandom3->Gaus(fV4RP,fV4SpreadRP);
  fPhiDistribution->SetParameter(3,dNewV4RP);
  
  // eta:
  Double_t dEtaMin = -1.; // to be improved 
  Double_t dEtaMax = 1.; // to be improved 
  
  Int_t iGoodTracks = 0;
  Int_t iSelParticlesRP = 0;
  Int_t iSelParticlesPOI = 0;
  // parameters of original tracks:
  Double_t dPhiOriginalTrack = 0.; 
  Double_t dPtOriginalTrack = 0.; 
  Double_t dEtaOriginalTrack = 0.; 
  // parameters of splitted tracks:  
  Double_t dPhiSplittedTrack = 0.; 
  Double_t dPtSplittedTrack = 0.; 
  Double_t dEtaSplittedTrack = 0.; 
  
  Double_t dTmpV2 = 0.;
  Bool_t bUniformAcceptance = kTRUE;
  Double_t Pi = TMath::Pi();

  if(!((fPhiMin1==0.) && (fPhiMax1==0.) && (fPhiMin2==0.) && (fPhiMax2==0.))) {
    bUniformAcceptance = kFALSE;
  }
  // loop over original tracks:
  for(Int_t i=0;i<iNewMultiplicityOfRP;i++) 
  {
   // sample the pt and eta for original track: 
   dPtOriginalTrack = fPtSpectra->GetRandom();
   dEtaOriginalTrack = fMyTRandom3->Uniform(dEtaMin,dEtaMax); 
   // generate flow harmonic which will determine the azimuthal distribution (to be improved - optimized):   
   if(fPtDependentHarmonics || fEtaDependentHarmonics) 
   {
    if(fEtaDependentHarmonics)
    {
     if(fV2vsEtaSpread>0.)
     { 
      dTmpV2 = TMath::Exp(-pow(dEtaOriginalTrack/fV2vsEtaSpread,2.));
     }
     if(!fPtDependentHarmonics)
     {
      dTmpV2*=fV2vsPtEtaMax;
     } 
    } // end of if(fEtaDependentHarmonics)
    if(fPtDependentHarmonics)
    {
     if(!fEtaDependentHarmonics)
     {
      if(dPtOriginalTrack >= fPtCutOff) {dTmpV2 = fV2vsPtEtaMax;} 
      else {dTmpV2 = fV2vsPtEtaMax*(dPtOriginalTrack/fPtCutOff);} 
     } else
       {
        if(dPtOriginalTrack >= fPtCutOff) {dTmpV2 *= fV2vsPtEtaMax;} 
        else {dTmpV2 *= fV2vsPtEtaMax*(dPtOriginalTrack/fPtCutOff);}         
       }
    } // end of if(fPtDependentHarmonics)
    // flow harmonic is determined and plugged in as a parameter in the predefined azimuthal distribution:
    fPhiDistribution->SetParameter(1,dTmpV2);         
   }
   // sample the phi angle for original track: 
   dPhiOriginalTrack = fPhiDistribution->GetRandom();
   // from each original track make fNoOfLoops splitted tracks if the particle is ongoing in 
   // detector's sector ranging from fNonflowSectorMin to fNonflowSectorMax
   // (simulating nonflow correlations between fNoOfLoops tracks in certain detector's sector):
   for(Int_t d=0;d<fNoOfLoops;d++) 
   {
    if(d>0 && (dPhiOriginalTrack>=fNonflowSectorMin && dPhiOriginalTrack<fNonflowSectorMax)) 
    {
     dPhiSplittedTrack = dPhiOriginalTrack;
     dPtSplittedTrack = dPtOriginalTrack;
     dEtaSplittedTrack = dEtaOriginalTrack;
     // phi:
     if(fPhiRange>0.)
     {
      dPhiSplittedTrack = fMyTRandom3->Uniform(dPhiOriginalTrack-fPhiRange,dPhiOriginalTrack+fPhiRange);
      if(dPhiSplittedTrack<0) 
      {
       dPhiSplittedTrack+=TMath::TwoPi(); // to ensure angle is in [0,2Pi>
      } 
      if(dPhiSplittedTrack>=TMath::TwoPi())
      {
       dPhiSplittedTrack-=TMath::TwoPi(); // to ensure angle is in [0,2Pi>
      }      
     } // end of if(fPhiRange>0.)    
     // pt:
     if(fPtRange>0.)
     {
      Double_t minPt = dPtOriginalTrack-fPtRange;
      Double_t maxPt = dPtOriginalTrack+fPtRange;
      if(minPt<0)
      {
       minPt = 0.; // protection against pt<0 for splitted track
      } 
      dPtSplittedTrack = fMyTRandom3->Uniform(minPt,maxPt);
     } // end of if(fPtRange>0.)    
     // eta:     
     if(fEtaRange>0.)
     {
      dEtaSplittedTrack = fMyTRandom3->Uniform(dEtaOriginalTrack-fEtaRange,dEtaOriginalTrack+fEtaRange);
     } // end of if(fEtaRange>0.)    
    } // end of if(d>0)  
    Double_t dTmpPhi = -44.;
    Double_t dTmpPt = -44.;
    Double_t dTmpEta = -44.;
    if(d>0)
    {
     if(dPhiOriginalTrack>=fNonflowSectorMin && dPhiOriginalTrack<fNonflowSectorMax)
     {
      dTmpPhi = dPhiSplittedTrack;
      dTmpPt = dPtSplittedTrack;
      dTmpEta = dEtaSplittedTrack;
     }
    } else
      {
       dTmpPhi = dPhiOriginalTrack;
       dTmpPt = dPtOriginalTrack;
       dTmpEta = dEtaOriginalTrack;      
      }   
    // make the new track:
    AliFlowTrackSimple *pTrack = new AliFlowTrackSimple();
    // uniform acceptance:
    if(bUniformAcceptance) 
    {
     pTrack->SetPt(dTmpPt); 
  	  pTrack->SetEta(dTmpEta); 
 	  pTrack->SetPhi(dTmpPhi);
 	  // checking RP cuts:  	 
     if(cutsRP->PassesCuts(pTrack))
     {
 	   pTrack->SetForRPSelection(kTRUE); 
      iSelParticlesRP++; 
	  }
	  // assign particles to subevents:
	  if(pTrack->Eta()>=fEtaMinA && pTrack->Eta()<=fEtaMaxA) 
	  {
	   pTrack->SetForSubevent(0);
	  }
     if(pTrack->Eta()>=fEtaMinB && pTrack->Eta()<=fEtaMaxB) 
     {
	   pTrack->SetForSubevent(1);
	  }
	  // checking POI cuts:
	  if(cutsPOI->PassesCuts(pTrack))
     {
	   pTrack->SetForPOISelection(kTRUE); 
	   iSelParticlesPOI++; 
	  }
	  pEvent->TrackCollection()->Add(pTrack); 
	  iGoodTracks++; 
    } // end of if(bUniformAcceptance) 
    // non-uniform acceptance, 1st sector:
    else if ((dTmpPhi > fPhiMin1*Pi/180) && (dTmpPhi < fPhiMax1*Pi/180)) 
    {
	  if(fMyTRandom3->Uniform(0,1) > 1 - fProbability1) 
	  {
	   pTrack->SetPt(dTmpPt);
	   pTrack->SetEta(dTmpEta);
	   pTrack->SetPhi(dTmpPhi);
	   // checking RP cuts:
	   if(cutsRP->PassesCuts(pTrack))
      {
	    pTrack->SetForRPSelection(kTRUE);
	    iSelParticlesRP++;
	   }
	   // assign particles to subevents
	   if (pTrack->Eta()>=fEtaMinA && pTrack->Eta()<=fEtaMaxA) 
	   {
	    pTrack->SetForSubevent(0);
	   }
	   if (pTrack->Eta()>=fEtaMinB && pTrack->Eta()<=fEtaMaxB) 
	   {
	    pTrack->SetForSubevent(1);
	   }
	   // checking POI cuts:
	   if(cutsPOI->PassesCuts(pTrack))
      {
	    pTrack->SetForPOISelection(kTRUE);
	    iSelParticlesPOI++;
	   }
	   pEvent->TrackCollection()->Add(pTrack);
	   iGoodTracks++;
	  } // end of if(fMyTRandom3->Uniform(0,1) > 1 - fProbability1) 
    } // end of else if ((dTmpPhi > fPhiMin1*Pi/180) && (dTmpPhi < fPhiMax1*Pi/180)) 
    // non-uniform acceptance, 2nd sector:
    else if ((dTmpPhi > fPhiMin2*Pi/180) && (dTmpPhi < fPhiMax2*Pi/180)) 
      {
	if(fMyTRandom3->Uniform(0,1) > 1 - fProbability2)
	  {
	    pTrack->SetPt(dTmpPt);
	    pTrack->SetEta(dTmpEta);
	    pTrack->SetPhi(dTmpPhi);
	    // checking RP cuts:
	    if(cutsRP->PassesCuts(pTrack))
	      {
		pTrack->SetForRPSelection(kTRUE);
		iSelParticlesRP++;
	      }
	    // assign particles to subevents
	    if (pTrack->Eta()>=fEtaMinA && pTrack->Eta()<=fEtaMaxA) 
	      {
		pTrack->SetForSubevent(0);
	      }
	    if (pTrack->Eta()>=fEtaMinB && pTrack->Eta()<=fEtaMaxB) 
	      {
		pTrack->SetForSubevent(1);
	      }
	    // checking POI cuts:
	    if(cutsPOI->PassesCuts(pTrack))
	      {
		pTrack->SetForPOISelection(kTRUE);
		iSelParticlesPOI++;
	      }
	    pEvent->TrackCollection()->Add(pTrack);
	    iGoodTracks++;
	  } // end of if(fMyTRandom3->Uniform(0,1) > 1 - fProbability2)
      } // end of else if ((dTmpPhi > fPhiMin2*Pi/180) && (dTmpPhi < fPhiMax2*Pi/180))
    else 
      {
	pTrack->SetPt(dTmpPt);
	pTrack->SetEta(dTmpEta);
	pTrack->SetPhi(dTmpPhi);
	// checking RP cuts:
	if(cutsRP->PassesCuts(pTrack))
	  {
	    pTrack->SetForRPSelection(kTRUE);
	    iSelParticlesRP++;
	  }
	// assign particles to subevents
	if (pTrack->Eta()>=fEtaMinA && pTrack->Eta()<=fEtaMaxA) 
	  {
	      pTrack->SetForSubevent(0);
	  }
	if (pTrack->Eta()>=fEtaMinB && pTrack->Eta()<=fEtaMaxB) 
	  {
	    pTrack->SetForSubevent(1);
	  }
	// checking POI cuts:
	if(cutsPOI->PassesCuts(pTrack))
	  {
	      pTrack->SetForPOISelection(kTRUE);
	      iSelParticlesPOI++;
	  }
	pEvent->TrackCollection()->Add(pTrack);
	iGoodTracks++;
      } // end of else
   } // end of for(Int_t d=0;d<fNoOfLoops;d++)
  } // end of for(Int_t i=0;i<iNewMultiplicityOfRP;i++)
  
  // update the event quantities
  pEvent->SetEventNSelTracksRP(iSelParticlesRP);  
  pEvent->SetNumberOfTracks(iGoodTracks); // total number of tracks (RPs + POIs + the ones that didn't pass neither RP nor POI cuts)
  pEvent->SetMCReactionPlaneAngle(dMCReactionPlaneAngle);
  
  Int_t cycle = 0;
  if(fPtDependentHarmonics || fEtaDependentHarmonics)
  { 
   cycle = 10;
  } else
    {
     cycle = 100;
    }
  
  if ( (++fCount % cycle) == 0) {
    if (!dMCReactionPlaneAngle == 0) cout<<" MC Reaction Plane Angle = "<<  dMCReactionPlaneAngle << endl;
    else cout<<" MC Reaction Plane Angle = unknown "<< endl;
    cout<<" iGoodTracks = "<< iGoodTracks << endl;
    cout<<" # of RP selected tracks = "<<iSelParticlesRP<<endl;
    cout<<" # of POI selected tracks = "<<iSelParticlesPOI<<endl;  
    cout << "# " << fCount << " events processed" << endl;
  }
  
  return pEvent;  
  
} // end of CreateEventOnTheFly()


 
