// Author: Dariusz Miskowiec <mailto:d.miskowiec@gsi.de> 2007

///////////////////////////////////////////////////////////////////////////////
// Parent analysis class
///////////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TFile.h>
#include "AliDAnal.h"

ClassImp(AliDAnal)
  
TDatabasePDG *AliDAnal::fPDG = TDatabasePDG::Instance();

/*****************************************************************************/
AliDAnal::AliDAnal(char *nam) : TNamed(nam,nam), fHistos()  
{
  // constructor

  fHistos.SetOwner(1);
  TDirectory *dir = gROOT->mkdir(GetName());
  dir->cd();

  printf("%s object named %s created\n",ClassName(),GetName());
}
/*****************************************************************************/
void AliDAnal::Save(const char *outfil, const char *mode) 
{
  // store histograms on file in a directory named after the object
  // mode should be "update" (default) or "new"

  printf("%s saving  histograms on %s (%s)\n",GetName(),outfil,mode);  
  TFile * f = TFile::Open(outfil, mode);
  TDirectory *dest = f->mkdir(GetName());
  dest->cd();
  fHistos.Write();
  gROOT->cd();
  f->Close();
}
/******************************************************************************/
