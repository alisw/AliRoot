#ifndef AliTPCCALIBV0_H
#define AliTPCCALIBV0_H


#include <TNamed.h>


class TTreeSRedirector;
class AliTPCROC;
class AliTPCseed;
class AliESDtrack;
class AliESD;
class TH3F;
class TH1F;
class TH2F;
class TH1I;
class TDatabasePDG;
class AliKFParticle;
class AliKFVertex;
class AliESDv0;
class TArrayI;


class AliTPCcalibV0 : public TNamed {
public :

   // List of branches

   AliTPCcalibV0();
  virtual ~AliTPCcalibV0();
   virtual void    ProofSlaveBegin(TList * output);
  void ProcessESD(AliESD *esd);
  void MakeV0s();
  void ProcessV0(Int_t ftype);
  void ProcessPI0();
  //
  //
  //  
protected:
  AliKFParticle * Fit(AliKFVertex & primVtx, AliESDv0 *v0, Int_t PDG1, Int_t PDG2);
private:
   TTreeSRedirector   *fDebugStream;  //debug stream for 
   TList          *fOutput;           //output list 	
   AliESD         *fESD;              //! current ED to proccess - NOT OWNER
   TDatabasePDG   *fPdg;              // particle database
   TObjArray      *fV0s;               // array of V0s
   TObjArray      *fGammas;           // gamma conversion candidates
   //
   TArrayI        *fV0type;            // array of types for V0s       
   TH2F           *fTPCdEdx;              // dEdx spectra
   TH2F           *fTPCdEdxPi;            // dEdx spectra - pion anti-pion
   TH2F           *fTPCdEdxEl;            // dEdx spectra - electroen -positrons from gamma
   TH2F           *fTPCdEdxP;             // dEdx spectra - proton antiproton - lambda -  antilambda
   //       
   ClassDef(AliTPCcalibV0,1);
};


#endif
