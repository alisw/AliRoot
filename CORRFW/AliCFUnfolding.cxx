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

//---------------------------------------------------------------------//
//                                                                     //
// AliCFUnfolding Class                                                //
// Class to handle general unfolding procedure                         // 
// For the moment only bayesian unfolding is supported                 //
// The next steps are to add chi2 minimisation and weighting methods   //
//                                                                     //
//                                                                     //
//                                                                     //
// Use :                                                               //
// -------                                                             //
// The Bayesian unfolding consists of several iterations.              //
// At each iteration, an inverse response matrix is calculated, given  //
// the measured spectrum, the a priori (guessed) spectrum,             //
// the efficiency spectrum and the response matrix.                    //
//                                                                     //
// Then at each iteration, the unfolded spectrum is calculated using   //
// the inverse response : the goal is to get an unfolded spectrum      //
// similar (according to some criterion) to the a priori one.          //
// If the difference is too big, another iteration is performed :      //
// the a priori spectrum is updated to the unfolded one from the       //
// previous iteration, and so on so forth, until the maximum number    //
// of iterations or the similarity criterion is reached.               //
//                                                                     //
// Currently the similarity criterion is the Chi2 between the a priori //
// and the unfolded spectrum.                                          //
//                                                                     //
// Currently the user has to define the max. number of iterations      //
// (::SetMaxNumberOfIterations)                                        //
// and the chi2 below which the procedure will stop                    //
// (::SetMaxChi2 or ::SetMaxChi2PerDOF)                                //
//                                                                     //
// An optional possibility is to smooth the unfolded spectrum at the   //
// end of each iteration, either using a fit function                  //
// (only if #dimensions <=3)                                           //
// or a simple averaging using the neighbouring bins values.           //
// This is possible calling the function ::UseSmoothing                //
// If no argument is passed to this function, then the second option   //
// is used.                                                            //
//                                                                     //
// IMPORTANT:                                                          //
//-----------                                                          //
// With this approach, the efficiency map must be calculated           //
// with *simulated* values only, otherwise the method won't work.      //
//                                                                     //
// ex: efficiency(bin_pt) = number_rec(bin_pt) / number_sim(bin_pt)    //
//                                                                     //
// the pt bin "bin_pt" must always be the same in both the efficiency  //
// numerator and denominator.                                          //
// This is why the efficiency map has to be created by a method        //
// from which both reconstructed and simulated values are accessible   //
// simultaneously.                                                     //
//                                                                     //
//                                                                     //
//---------------------------------------------------------------------//
// Author : renaud.vernet@cern.ch                                      //
//---------------------------------------------------------------------//


#include "AliCFUnfolding.h"
#include "TMath.h"
#include "TAxis.h"
#include "AliLog.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

ClassImp(AliCFUnfolding)

//______________________________________________________________

AliCFUnfolding::AliCFUnfolding() :
  TNamed(),
  fResponse(0x0),
  fPrior(0x0),
  fEfficiency(0x0),
  fMeasured(0x0),
  fMaxNumIterations(0),
  fNVariables(0),
  fMaxChi2(0),
  fUseSmoothing(kFALSE),
  fSmoothFunction(0x0),
  fSmoothOption(""),
  fOriginalPrior(0x0),
  fInverseResponse(0x0),
  fMeasuredEstimate(0x0),
  fConditional(0x0),
  fProjResponseInT(0x0),
  fUnfolded(0x0),
  fCoordinates2N(0x0),
  fCoordinatesN_M(0x0),
  fCoordinatesN_T(0x0)
{
  //
  // default constructor
  //
}

//______________________________________________________________

AliCFUnfolding::AliCFUnfolding(const Char_t* name, const Char_t* title, const Int_t nVar, 
			       const THnSparse* response, const THnSparse* efficiency, const THnSparse* measured, const THnSparse* prior) :
  TNamed(name,title),
  fResponse((THnSparse*)response->Clone()),
  fPrior(0x0),
  fEfficiency((THnSparse*)efficiency->Clone()),
  fMeasured((THnSparse*)measured->Clone()),
  fMaxNumIterations(0),
  fNVariables(nVar),
  fMaxChi2(0),
  fUseSmoothing(kFALSE),
  fSmoothFunction(0x0),
  fSmoothOption(""),
  fOriginalPrior(0x0),
  fInverseResponse(0x0),
  fMeasuredEstimate(0x0),
  fConditional(0x0),
  fProjResponseInT(0x0),
  fUnfolded(0x0),
  fCoordinates2N(0x0),
  fCoordinatesN_M(0x0),
  fCoordinatesN_T(0x0)
{
  //
  // named constructor
  //

  AliInfo(Form("\n\n--------------------------\nCreating an unfolder :\n--------------------------\nresponse matrix has %d dimension(s)",fResponse->GetNdimensions()));
  
  if (!prior) CreateFlatPrior(); // if no prior distribution declared, simply use a flat distribution
  else {
    fPrior = (THnSparse*) prior->Clone();
    fOriginalPrior = (THnSparse*)fPrior->Clone();
    if (fPrior->GetNdimensions() != fNVariables) 
      AliFatal(Form("The prior matrix should have %d dimensions, and it has actually %d",fNVariables,fPrior->GetNdimensions()));
  }

  if (fEfficiency->GetNdimensions() != fNVariables) 
    AliFatal(Form("The efficiency matrix should have %d dimensions, and it has actually %d",fNVariables,fEfficiency->GetNdimensions()));
  if (fMeasured->GetNdimensions() != fNVariables) 
    AliFatal(Form("The measured matrix should have %d dimensions, and it has actually %d",fNVariables,fMeasured->GetNdimensions()));
  if (fResponse->GetNdimensions() != 2*fNVariables) 
    AliFatal(Form("The response matrix should have %d dimensions, and it has actually %d",2*fNVariables,fResponse->GetNdimensions()));
  

  for (Int_t iVar=0; iVar<fNVariables; iVar++) {
    AliInfo(Form("prior      matrix has %d bins in dimension %d",fPrior     ->GetAxis(iVar)->GetNbins(),iVar));
    AliInfo(Form("efficiency matrix has %d bins in dimension %d",fEfficiency->GetAxis(iVar)->GetNbins(),iVar));
    AliInfo(Form("measured   matrix has %d bins in dimension %d",fMeasured  ->GetAxis(iVar)->GetNbins(),iVar));
  }
  Init();
}


//______________________________________________________________

AliCFUnfolding::AliCFUnfolding(const AliCFUnfolding& c) :
  TNamed(c),
  fResponse((THnSparse*)c.fResponse->Clone()),
  fPrior((THnSparse*)c.fPrior->Clone()),
  fEfficiency((THnSparse*)c.fEfficiency->Clone()),
  fMeasured((THnSparse*)c.fMeasured->Clone()),
  fMaxNumIterations(c.fMaxNumIterations),
  fNVariables(c.fNVariables),
  fMaxChi2(c.fMaxChi2),
  fUseSmoothing(c.fUseSmoothing),
  fSmoothFunction((TF1*)c.fSmoothFunction->Clone()),
  fSmoothOption(fSmoothOption),
  fOriginalPrior((THnSparse*)c.fOriginalPrior->Clone()),
  fInverseResponse((THnSparse*)c.fInverseResponse->Clone()),
  fMeasuredEstimate((THnSparse*)fMeasuredEstimate->Clone()),
  fConditional((THnSparse*)c.fConditional->Clone()),
  fProjResponseInT((THnSparse*)c.fProjResponseInT->Clone()),
  fUnfolded((THnSparse*)c.fUnfolded->Clone()),
  fCoordinates2N(new Int_t(*c.fCoordinates2N)),
  fCoordinatesN_M(new Int_t(*c.fCoordinatesN_M)),
  fCoordinatesN_T(new Int_t(*c.fCoordinatesN_T))
{
  //
  // copy constructor
  //
}

//______________________________________________________________

AliCFUnfolding& AliCFUnfolding::operator=(const AliCFUnfolding& c) {
  //
  // assignment operator
  //
  
  if (this!=&c) {
    TNamed::operator=(c);
    fResponse = (THnSparse*)c.fResponse->Clone() ;
    fPrior = (THnSparse*)c.fPrior->Clone() ;
    fEfficiency = (THnSparse*)c.fEfficiency->Clone() ;
    fMeasured = (THnSparse*)c.fMeasured->Clone() ;
    fMaxNumIterations = c.fMaxNumIterations ;
    fNVariables = c.fNVariables ;
    fMaxChi2 = c.fMaxChi2 ;
    fUseSmoothing = c.fUseSmoothing ;
    fSmoothFunction = (TF1*)c.fSmoothFunction->Clone();
    fSmoothOption = c.fSmoothOption ;
    fOriginalPrior = (THnSparse*)c.fOriginalPrior->Clone() ;
    fInverseResponse = (THnSparse*)c.fInverseResponse->Clone() ;
    fMeasuredEstimate = (THnSparse*)fMeasuredEstimate->Clone() ;
    fConditional = (THnSparse*)c.fConditional->Clone() ;
    fProjResponseInT = (THnSparse*)c.fProjResponseInT->Clone() ;
    fUnfolded = (THnSparse*)c.fUnfolded->Clone() ;
    fCoordinates2N  = new Int_t(*c.fCoordinates2N)  ;
    fCoordinatesN_M = new Int_t(*c.fCoordinatesN_M) ;
    fCoordinatesN_T = new Int_t(*c.fCoordinatesN_T) ;
  }
  return *this;
}

//______________________________________________________________

AliCFUnfolding::~AliCFUnfolding() {
  //
  // destructor
  //
  if (fResponse)           delete fResponse;
  if (fPrior)              delete fPrior;
  if (fEfficiency)         delete fEfficiency;
  if (fMeasured)           delete fMeasured;
  if (fSmoothFunction)     delete fSmoothFunction;
  if (fOriginalPrior)      delete fOriginalPrior;
  if (fInverseResponse)    delete fInverseResponse;
  if (fMeasuredEstimate)   delete fMeasuredEstimate;
  if (fConditional)        delete fConditional;
  if (fProjResponseInT)    delete fProjResponseInT;
  if (fCoordinates2N)      delete [] fCoordinates2N; 
  if (fCoordinatesN_M)     delete [] fCoordinatesN_M; 
  if (fCoordinatesN_T)     delete [] fCoordinatesN_T; 
}

//______________________________________________________________

void AliCFUnfolding::Init() {
  //
  // initialisation function : creates internal settings
  //

  fCoordinates2N  = new Int_t[2*fNVariables];
  fCoordinatesN_M = new Int_t[fNVariables];
  fCoordinatesN_T = new Int_t[fNVariables];

  // create the matrix of conditional probabilities P(M|T)
  CreateConditional();
  
  // create the frame of the inverse response matrix
  fInverseResponse  = (THnSparse*) fResponse->Clone();
  // create the frame of the unfolded spectrum
  fUnfolded = (THnSparse*) fPrior->Clone();
  // create the frame of the measurement estimate spectrum
  fMeasuredEstimate = (THnSparse*) fMeasured->Clone();
}

//______________________________________________________________

void AliCFUnfolding::CreateEstMeasured() {
  //
  // This function creates a estimate (M) of the reconstructed spectrum 
  // given the a priori distribution (T), the efficiency (E) and the conditional matrix (COND)
  //
  // --> P(M) = SUM   { P(M|T)    * P(T) }
  // --> M(i) = SUM_k { COND(i,k) * T(k) * E (k)}
  //
  // This is needed to calculate the inverse response matrix
  //


  // clean the measured estimate spectrum
  fMeasuredEstimate->Reset();

  THnSparse* priorTimesEff = (THnSparse*) fPrior->Clone();
  priorTimesEff->Multiply(fEfficiency);

  // fill it
  for (Long_t iBin=0; iBin<fConditional->GetNbins(); iBin++) {
    Double_t conditionalValue = fConditional->GetBinContent(iBin,fCoordinates2N);
    Double_t conditionalError = fConditional->GetBinError  (iBin);
    GetCoordinates();
    Double_t priorTimesEffValue = priorTimesEff->GetBinContent(fCoordinatesN_T);
    Double_t priorTimesEffError = priorTimesEff->GetBinError  (fCoordinatesN_T);
    Double_t fill = conditionalValue * priorTimesEffValue ;
    
    if (fill>0.) {
      // error calculation : gaussian error propagation (may be overestimated...)
      Double_t err2  = TMath::Power(fMeasuredEstimate->GetBinError(fCoordinatesN_M),2) ;
      err2 += TMath::Power(conditionalValue*priorTimesEffError,2) + TMath::Power(conditionalError*priorTimesEffValue,2) ;
      Double_t err = TMath::Sqrt(err2);
      fMeasuredEstimate->SetBinError(fCoordinatesN_M,err);

      fMeasuredEstimate->AddBinContent(fCoordinatesN_M,fill);
    }
  }
  delete priorTimesEff ;
}

//______________________________________________________________

void AliCFUnfolding::CreateInvResponse() {
  //
  // Creates the inverse response matrix (INV) with Bayesian method
  //  : uses the conditional matrix (COND), the prior probabilities (T) and the efficiency map (E)
  //
  // --> P(T|M)   = P(M|T)    * P(T) * eff(T) / SUM   { P(M|T)    * P(T) }
  // --> INV(i,j) = COND(i,j) * T(j) * E(j)   / SUM_k { COND(i,k) * T(k) }
  //

  THnSparse* priorTimesEff = (THnSparse*) fPrior->Clone();
  priorTimesEff->Multiply(fEfficiency);

  for (Long_t iBin=0; iBin<fConditional->GetNbins(); iBin++) {
    Double_t conditionalValue = fConditional->GetBinContent(iBin,fCoordinates2N);
    Double_t conditionalError = fConditional->GetBinError  (iBin);
    GetCoordinates();
    Double_t estMeasuredValue   = fMeasuredEstimate->GetBinContent(fCoordinatesN_M);
    Double_t estMeasuredError   = fMeasuredEstimate->GetBinError  (fCoordinatesN_M);
    Double_t priorTimesEffValue = priorTimesEff    ->GetBinContent(fCoordinatesN_T);
    Double_t priorTimesEffError = priorTimesEff    ->GetBinError  (fCoordinatesN_T);
    Double_t fill = (estMeasuredValue>0. ? conditionalValue * priorTimesEffValue / estMeasuredValue : 0. ) ;
    // error calculation : gaussian error propagation (may be overestimated...)
    Double_t err  = 0. ;
    if (estMeasuredValue>0.) {
      err = TMath::Sqrt( TMath::Power(conditionalError * priorTimesEffValue * estMeasuredValue ,2) +
			 TMath::Power(conditionalValue * priorTimesEffError * estMeasuredValue ,2) + 
			 TMath::Power(conditionalValue * priorTimesEffValue * estMeasuredError ,2) )
	/ TMath::Power(estMeasuredValue,2) ;
    }
    if (fill>0. || fInverseResponse->GetBinContent(fCoordinates2N)>0.) {
      fInverseResponse->SetBinContent(fCoordinates2N,fill);
      fInverseResponse->SetBinError  (fCoordinates2N,err );
    }
  } 
  delete priorTimesEff ;
}

//______________________________________________________________

void AliCFUnfolding::Unfold() {
  //
  // Main routine called by the user : 
  // it calculates the unfolded spectrum from the response matrix and the measured spectrum
  // several iterations are performed until a reasonable chi2 is reached
  //

  Int_t iIterBayes=0 ;
  Double_t chi2=0 ;

  for (iIterBayes=0; iIterBayes<fMaxNumIterations; iIterBayes++) { // bayes iterations
    CreateEstMeasured();
    CreateInvResponse();
    CreateUnfolded();
    chi2 = GetChi2();
    AliDebug(0,Form("Chi2 at iteration %d is %e",iIterBayes,chi2));
    if (fMaxChi2>0. && chi2<fMaxChi2) {
      break;
    }
    // update the prior distribution
    if (fUseSmoothing) {
      if (Smooth()) {
	AliError("Couldn't smooth the unfolded spectrum!!");
	AliInfo(Form("\n\n=======================\nFinished at iteration %d : Chi2 is %e and you required it to be < %e\n=======================\n\n",iIterBayes,chi2,fMaxChi2));
	return;
      }
    }
    fPrior = (THnSparse*)fUnfolded->Clone() ; // this should be changed (memory)
  }
  AliInfo(Form("\n\n=======================\nFinished at iteration %d : Chi2 is %e and you required it to be < %e\n=======================\n\n",iIterBayes,chi2,fMaxChi2));
}

//______________________________________________________________

void AliCFUnfolding::CreateUnfolded() {
  //
  // Creates the unfolded (T) spectrum from the measured spectrum (M) and the inverse response matrix (INV)
  // We have P(T) = SUM   { P(T|M)   * P(M) } 
  //   -->   T(i) = SUM_k { INV(i,k) * M(k) }
  //


  // clear the unfolded spectrum
  fUnfolded->Reset();
  
  for (Long_t iBin=0; iBin<fInverseResponse->GetNbins(); iBin++) {
    Double_t invResponseValue = fInverseResponse->GetBinContent(iBin,fCoordinates2N);
    Double_t invResponseError = fInverseResponse->GetBinError  (iBin);
    GetCoordinates();
    Double_t effValue      = fEfficiency->GetBinContent(fCoordinatesN_T);
    Double_t effError      = fEfficiency->GetBinError  (fCoordinatesN_T);
    Double_t measuredValue = fMeasured  ->GetBinContent(fCoordinatesN_M);
    Double_t measuredError = fMeasured  ->GetBinError  (fCoordinatesN_M);
    Double_t fill = (effValue>0. ? invResponseValue * measuredValue / effValue : 0.) ;
    
    if (fill>0.) {
      // error calculation : gaussian error propagation (may be overestimated...)
      Double_t err2 = TMath::Power(fUnfolded->GetBinError(fCoordinatesN_T),2) ;
      err2 += TMath::Power(invResponseError * measuredValue * effValue,2) / TMath::Power(effValue,4) ;
      err2 += TMath::Power(invResponseValue * measuredError * effValue,2) / TMath::Power(effValue,4) ;
      err2 += TMath::Power(invResponseValue * measuredValue * effError,2) / TMath::Power(effValue,4) ;
      Double_t err = TMath::Sqrt(err2);
      fUnfolded->SetBinError(fCoordinatesN_T,err);

      fUnfolded->AddBinContent(fCoordinatesN_T,fill);
    }
  }
}
    
//______________________________________________________________

void AliCFUnfolding::GetCoordinates() {
  //
  // assign coordinates in Measured and True spaces (dim=N) from coordinates in global space (dim=2N)
  //
  for (Int_t i = 0; i<fNVariables ; i++) {
    fCoordinatesN_M[i] = fCoordinates2N[i];
    fCoordinatesN_T[i] = fCoordinates2N[i+fNVariables];
  }
}

//______________________________________________________________

void AliCFUnfolding::CreateConditional() {
  //
  // creates the conditional probability matrix (R*) holding the P(M|T), given the reponse matrix R
  //
  //  --> R*(i,j) = R(i,j) / SUM_k{ R(k,j) }
  //

  fConditional     = (THnSparse*) fResponse->Clone(); // output of this function
  fProjResponseInT = (THnSparse*) fPrior->Clone();    // output denominator : 
                                                      // projection of the response matrix on the TRUE axis
  Int_t* dim = new Int_t [fNVariables];
  for (Int_t iDim=0; iDim<fNVariables; iDim++) dim[iDim] = fNVariables+iDim ; //dimensions corresponding to TRUE values (i.e. from N to 2N-1)
  fProjResponseInT = fConditional->Projection(fNVariables,dim,"E"); //project
  delete [] dim; 
  
  // fill the conditional probability matrix
  for (Long_t iBin=0; iBin<fResponse->GetNbins(); iBin++) {
    Double_t responseValue = fResponse->GetBinContent(iBin,fCoordinates2N);
    Double_t responseError = fResponse->GetBinError  (iBin);
    GetCoordinates();
    Double_t projValue = fProjResponseInT->GetBinContent(fCoordinatesN_T);
    Double_t projError = fProjResponseInT->GetBinError  (fCoordinatesN_T);
    
    Double_t fill = responseValue / projValue ;
    if (fill>0. || fConditional->GetBinContent(fCoordinates2N)>0.) {
      fConditional->SetBinContent(fCoordinates2N,fill);
      // gaussian error for the moment
      Double_t err2 = TMath::Power(responseError*projValue,2) + TMath::Power(responseValue*projError,2) ;
      Double_t err = TMath::Sqrt(err2);
      err /= TMath::Power(projValue,2) ;
      fConditional->SetBinError  (fCoordinates2N,err);
    }
  }
}

//______________________________________________________________

Double_t AliCFUnfolding::GetChi2() {
  //
  // Returns the chi2 between unfolded and a priori spectrum
  //

  Double_t chi2 = 0. ;
  for (Long_t iBin=0; iBin<fPrior->GetNbins(); iBin++) {
    Double_t priorValue = fPrior->GetBinContent(iBin,fCoordinatesN_T);
    Double_t error_unf  = fUnfolded->GetBinError(fCoordinatesN_T);
    chi2 += (error_unf > 0. ? TMath::Power((fUnfolded->GetBinContent(fCoordinatesN_T) - priorValue)/error_unf,2) / priorValue : 0.) ;
  }
  return chi2;
}

//______________________________________________________________

void AliCFUnfolding::SetMaxChi2PerDOF(Double_t val) {
  //
  // Max. chi2 per degree of freedom : user setting
  //

  Int_t nDOF = 1 ;
  for (Int_t iDim=0; iDim<fNVariables; iDim++) {
    nDOF *= fPrior->GetAxis(iDim)->GetNbins();
  }
  AliInfo(Form("Number of degrees of freedom = %d",nDOF));
  fMaxChi2 = val * nDOF ;
}

//______________________________________________________________

Short_t AliCFUnfolding::Smooth() {
  //
  // Smoothes the unfolded spectrum
  //
  // By default each cell content is replaced by the average with the neighbouring bins (but not diagonally-neighbouring bins)
  // However, if a specific function fcn has been defined in UseSmoothing(fcn), the unfolded will be fit and updated using fcn 
  //
  
  if (fSmoothFunction) {
    AliDebug(2,Form("Smoothing spectrum with fit function %p",fSmoothFunction));
    return SmoothUsingFunction();
  }
  else return SmoothUsingNeighbours();
}

//______________________________________________________________

Short_t AliCFUnfolding::SmoothUsingNeighbours() {
  //
  // Smoothes the unfolded spectrum using neighouring bins
  //

  Int_t* numBins = new Int_t[fNVariables];
  for (Int_t iVar=0; iVar<fNVariables; iVar++) numBins[iVar]=fUnfolded->GetAxis(iVar)->GetNbins();
  
  //need a copy because fUnfolded will be updated during the loop, and this creates problems
  THnSparse* copy = (THnSparse*)fUnfolded->Clone();

  for (Long_t iBin=0; iBin<copy->GetNbins(); iBin++) { //loop on non-empty bins
    Double_t content = copy->GetBinContent(iBin,fCoordinatesN_T);
    Double_t error2  = TMath::Power(copy->GetBinError(iBin),2);

    // skip the under/overflow bins...
    Bool_t isOutside = kFALSE ;
    for (Int_t iVar=0; iVar<fNVariables; iVar++) {
      if (fCoordinatesN_T[iVar]<1 || fCoordinatesN_T[iVar]>numBins[iVar]) {
	isOutside=kTRUE;
	break;
      }
    }
    if (isOutside) continue;
    
    Int_t neighbours = 0; // number of neighbours to average with

    for (Int_t iVar=0; iVar<fNVariables; iVar++) {
      if (fCoordinatesN_T[iVar] > 1) { // must not be on low edge border
	fCoordinatesN_T[iVar]-- ; //get lower neighbouring bin 
	content += copy->GetBinContent(fCoordinatesN_T);
	error2  += TMath::Power(copy->GetBinError(fCoordinatesN_T),2);
	neighbours++;
	fCoordinatesN_T[iVar]++ ; //back to initial coordinate
      }
      if (fCoordinatesN_T[iVar] < numBins[iVar]) { // must not be on up edge border
	fCoordinatesN_T[iVar]++ ; //get upper neighbouring bin
	content += copy->GetBinContent(fCoordinatesN_T);
	error2  += TMath::Power(copy->GetBinError(fCoordinatesN_T),2);
	neighbours++;
	fCoordinatesN_T[iVar]-- ; //back to initial coordinate
      }
    }
    // make an average
    fUnfolded->SetBinContent(fCoordinatesN_T,content/(1.+neighbours));
    fUnfolded->SetBinError  (fCoordinatesN_T,TMath::Sqrt(error2)/(1.+neighbours));
  }
  delete [] numBins;
  delete copy;
  return 0;
}

//______________________________________________________________

Short_t AliCFUnfolding::SmoothUsingFunction() {
  //
  // Fits the unfolded spectrum using the function fSmoothFunction
  //

  AliDebug(0,Form("Smooth function is a %s with option \"%s\" and has %d parameters : ",fSmoothFunction->ClassName(),fSmoothOption,fSmoothFunction->GetNpar()));

  for (Int_t iPar=0; iPar<fSmoothFunction->GetNpar(); iPar++) AliDebug(0,Form("par[%d]=%e",iPar,fSmoothFunction->GetParameter(iPar)));

  Int_t fitResult = 0;

  switch (fNVariables) {
  case 1 : fitResult = fUnfolded->Projection(0)    ->Fit(fSmoothFunction,fSmoothOption); break;
  case 2 : fitResult = fUnfolded->Projection(1,0)  ->Fit(fSmoothFunction,fSmoothOption); break; // (1,0) instead of (0,1) -> TAxis issue
  case 3 : fitResult = fUnfolded->Projection(0,1,2)->Fit(fSmoothFunction,fSmoothOption); break;
  default: AliFatal(Form("Cannot handle such fit in %d dimensions",fNVariables)) ; return 1;
  }

  if (fitResult != 0) {
    AliWarning(Form("Fit failed with status %d, stopping the loop",fitResult));
    return 1;
  }

  Int_t nDim = fNVariables;
  Int_t* bins = new Int_t[nDim]; // number of bins for each variable
  Long_t nBins = 1; // used to calculate the total number of bins in the THnSparse

  for (Int_t iVar=0; iVar<nDim; iVar++) {
    bins[iVar] = fUnfolded->GetAxis(iVar)->GetNbins();
    nBins *= bins[iVar];
  }

  Int_t *bin  = new Int_t[nDim];    // bin to fill the THnSparse (holding the bin coordinates)
  Double_t x[3] = {0,0,0} ;         // value in bin center (max dimension is 3 (TF3))

  // loop on the bins and update of fUnfolded
  // THnSparse::Multiply(TF1*) doesn't exist, so let's do it bin by bin
  for (Long_t iBin=0; iBin<nBins; iBin++) {
    Long_t bin_tmp = iBin ;
    for (Int_t iVar=0; iVar<nDim; iVar++) {
      bin[iVar] = 1 + bin_tmp % bins[iVar] ;
      bin_tmp /= bins[iVar] ;
      x[iVar] = fUnfolded->GetAxis(iVar)->GetBinCenter(bin[iVar]);
    }
    Double_t functionValue = fSmoothFunction->Eval(x[0],x[1],x[2]) ;
    fUnfolded->SetBinError  (bin,fUnfolded->GetBinError(bin)*functionValue/fUnfolded->GetBinContent(bin));
    fUnfolded->SetBinContent(bin,functionValue);
  }
  return 0;
}

//______________________________________________________________

void AliCFUnfolding::CreateFlatPrior() {
  //
  // Creates a flat prior distribution
  // 

  AliInfo("Creating a flat a priori distribution");
  
  // create the frame of the THnSparse given (for example) the one from the efficiency map
  fPrior = (THnSparse*) fEfficiency->Clone();

  if (fNVariables != fPrior->GetNdimensions()) 
    AliFatal(Form("The prior matrix should have %d dimensions, and it has actually %d",fNVariables,fPrior->GetNdimensions()));

  Int_t nDim = fNVariables;
  Int_t* bins = new Int_t[nDim]; // number of bins for each variable
  Long_t nBins = 1; // used to calculate the total number of bins in the THnSparse

  for (Int_t iVar=0; iVar<nDim; iVar++) {
    bins[iVar] = fPrior->GetAxis(iVar)->GetNbins();
    nBins *= bins[iVar];
  }

  Int_t *bin = new Int_t[nDim]; // bin to fill the THnSparse (holding the bin coordinates)

  // loop that sets 1 in each bin
  for (Long_t iBin=0; iBin<nBins; iBin++) {
    Long_t bin_tmp = iBin ;
    for (Int_t iVar=0; iVar<nDim; iVar++) {
      bin[iVar] = 1 + bin_tmp % bins[iVar] ;
      bin_tmp /= bins[iVar] ;
    }
    fPrior->SetBinContent(bin,1.); // put 1 everywhere
    fPrior->SetBinError  (bin,0.); // put 0 everywhere
  }
  
  fOriginalPrior = (THnSparse*)fPrior->Clone();

  delete [] bin;
  delete [] bins;
}
