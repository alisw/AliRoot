// $Id$

//____________________________________________________________________
//////////////////////////////////////////////////////////////////////
//                                                                  //
// class AliHBTReaderESD                                            //
//                                                                  //
// Reader for ALICE Event Summary Data (ESD).                       //
//                                                                  //
// Piotr.Skowronski@cern.ch                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TPDGCode.h>
#include <TString.h>
#include <TObjString.h>
#include <TTree.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <AliESD.h>
#include <AliESDtrack.h>
#include <AliJetEventParticles.h>
#include "AliJetParticlesReaderESD.h"

ClassImp(AliJetParticlesReaderESD)

AliJetParticlesReaderESD::AliJetParticlesReaderESD(const Char_t* esdfilename) :
  AliJetParticlesReader(),
  fESDFileName(esdfilename),
  fFile(0),
  fKeyIterator(0),
  fPassFlag(AliESDtrack::kTPCrefit)
{
  //constructor
}

/********************************************************************/
  
AliJetParticlesReaderESD::AliJetParticlesReaderESD(
                                      TObjArray* dirs,
                                      const Char_t* esdfilename) :
  AliJetParticlesReader(dirs),
  fESDFileName(esdfilename),
  fFile(0),
  fKeyIterator(0),
  fPassFlag(AliESDtrack::kTPCrefit)
{
  //constructor
}

/********************************************************************/

AliJetParticlesReaderESD::~AliJetParticlesReaderESD()
{
  //desctructor
  if(fFile) delete fFile;
  if(fKeyIterator) delete fKeyIterator;
}


Int_t AliJetParticlesReaderESD::ReadNext()
{
  //reads next event from fFile

  do   // is OK even if 0 dirs specified, 
    {  // in that case we try to read from "./"
      if (fFile == 0)
	{
	  fFile = OpenFile(fCurrentDir);
	  if (fFile == 0)
	    {
	      Error("ReadNext","Cannot get fFile for dir no. %d",fCurrentDir);
	      fCurrentDir++;
	      continue;
	    }
     
	  fKeyIterator = new TIter(fFile->GetListOfKeys());  
	  fCurrentEvent = 0;
	  //fFile->Dump();
	  //fFile->GetListOfKeys()->Print();
	} 

      TKey* key = (TKey*)fKeyIterator->Next();
      if (key == 0)
	{
	  fCurrentDir++;
	  delete fKeyIterator;
	  fKeyIterator = 0;
	  delete fFile; //we have to assume there are no more ESD objects in the fFile
	  fFile = 0;
	  continue;
	}

      TString esdname = "ESD";
      esdname+=fCurrentEvent;
      AliESD* esd = dynamic_cast<AliESD*>(fFile->Get(esdname));
      if (esd == 0)
      {
	//Info("ReadNext","This key is not an AliESD object %s",key->GetName());
	Info("ReadNext","Can not find AliESD object named %s",esdname.Data());
	
	fCurrentDir++;
	delete fKeyIterator;
	fKeyIterator = 0;
	delete fFile;//we have to assume there is no more ESD objects in the fFile
	fFile = 0;
	continue;
      }
     
      ReadESD(esd);
      
      fCurrentEvent++;
      fNEventsRead++;
      delete esd;
      return kTRUE;//success -> read one event
    }  while(fCurrentDir < GetNumberOfDirs());
       //end of loop over directories specified in fDirs Obj Array  
  return kFALSE; //no more directories to read
}

/**********************************************************/

Int_t AliJetParticlesReaderESD::ReadESD(AliESD* esd)
{
  //Reads one ESD

  if (esd == 0)
   {
     Error("ReadESD","ESD is NULL");
     return kFALSE;
   }

  //TDatabasePDG* pdgdb = TDatabasePDG::Instance();
  //if (pdgdb == 0)
  //{
  //   Error("ReadESD","Can not get PDG Database Instance.");
  //   return kFALSE;
  // }
   
  //Float_t mf = esd->GetMagneticField(); 
  //if ( (mf == 0.0) && (fNTrackPoints > 0) )
  //{
  //   Error("ReadESD","Magnetic Field is 0 and Track Points demanded. Skipping to next event.");
  //   return kFALSE;
  //}

  Info("ReadESD","Reading Event %d",fCurrentEvent);
  if((!fOwner) || (fEventParticles==0)) 
    fEventParticles = new AliJetEventParticles();

  Double_t vertexpos[3];//vertex position
  const AliESDVertex* kvertex = esd->GetVertex();
  if (kvertex == 0)
   {
     Info("ReadESD","ESD returned NULL pointer to vertex - assuming (0.0,0.0,0.0)");
     vertexpos[0] = 0.0;
     vertexpos[1] = 0.0;
     vertexpos[2] = 0.0;
   }
  else
   {
     kvertex->GetXYZ(vertexpos);
   }
  fEventParticles->SetVertex(vertexpos[0],vertexpos[1],vertexpos[2]);

  const Int_t kntr = esd->GetNumberOfTracks();
  Info("ReadESD","Found %d tracks.",kntr);
  for (Int_t i = 0;i<kntr; i++)
   {
     const AliESDtrack *kesdtrack = esd->GetTrack(i);
     if (kesdtrack == 0)
      {
        Error("ReadESD","Can not get track %d", i);
        continue;
      }

     if ((kesdtrack->GetStatus() & fPassFlag) == kFALSE)
      {
	Info("ReadNext","Particle skipped.");
        continue;
      }

     Double_t mom[3];  //momentum
     kesdtrack->GetPxPyPz(mom);
     //kesdtrack->GetConstrainedPxPyPz(mom);
     //Double_t pos[3];//position
     //kesdtrack->GetXYZ(pos);
     //kesdtrack->GetConstrainedXYZ(pos);
     const Float_t kmass=kesdtrack->GetMass();
     const Float_t kp2=mom[0]*mom[0]+mom[1]*mom[1]+mom[2]*mom[2];
     const Float_t ketot=TMath::Sqrt(kmass*kmass+kp2);
     const Float_t kpt=TMath::Sqrt(mom[0]*mom[0]+mom[1]*mom[1]);
     const Float_t kp=TMath::Sqrt(kp2);
     const Float_t keta=0.5*TMath::Log((kp+mom[2]+1e-30)/(kp-mom[2]+1e-30)); 
     const Float_t kphi=TMath::Pi()+TMath::ATan2(-mom[1],-mom[0]);
     if(IsAcceptedParticle(kpt,kphi,keta))
       fEventParticles->AddParticle(mom[0],mom[1],mom[2],ketot,i,kesdtrack->GetLabel(),kpt,kphi,keta);

   } // loop over tracks

  return kTRUE;
}

/**********************************************************/

void AliJetParticlesReaderESD::Rewind()
{
  //rewinds reading 

  if(fFile) delete fFile;
  if(fKeyIterator) delete fKeyIterator;
  fKeyIterator = 0;
  fFile = 0;
  fCurrentDir = 0;
  fNEventsRead = 0;
}

/**********************************************************/

TFile* AliJetParticlesReaderESD::OpenFile(Int_t n)
{
  //opens fFile with kine tree

 const TString& dirname = GetDirName(n);
 if (dirname == "")
  {
   Error("OpenFiles","Can not get directory name");
   return 0;
  }
 TString filename = dirname +"/"+ fESDFileName;
 TFile *ret = TFile::Open(filename.Data()); 
 if (ret == 0)
  {
    Error("OpenFiles","Can't open fFile %s",filename.Data());
    return 0;
  }
 if (!ret->IsOpen())
  {
    Error("OpenFiles","Can't open fFile  %s",filename.Data());
    return 0;
  }
   
 return ret;
}
