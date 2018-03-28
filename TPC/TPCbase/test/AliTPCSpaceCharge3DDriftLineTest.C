/*
  Unit test for the  AliTPCSpaceCharge3DDriftLine class:

  Tests:
  1.) Check consistency for distortion and correction

  usage:

  .x AliTPCSpaceCharge3DDriftLineTest.C+

*/

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "TLegend.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TString.h"
#include "TH3.h"
#include "TH2.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TMap.h"
#include "TFile.h"
#include "TGraph.h"
#include "AliTPCSpaceCharge3DDriftLine.h"
#include "AliTPCPoissonSolver.h"
#include "TClonesArray.h"
#include "TTreeStream.h"
#include "TGrid.h"
#include "TStatToolkit.h"

#endif

void UnitTestCorrectnessDistortion();
void UnitTestConsistencyDistortion();


Double_t dFunctionVZ(Double_t *x, Double_t *par); // function test for boundary
const Double_t gkVCE  = -50000;
const Double_t gkVROC = -70;
const Int_t gkMaxIter = 200;
const Float_t gkConvError = 1e-8;
const Double_t gkMaxEpsilon = 1e-2;


// TODO: Move the helper classes to AliTPCSpaceCharge3DDriftLine
// helper class
void
LocalDistCorrDzExact(TFormula *intDrDzF, TFormula *intDPhiDzF, TFormula *intDzDzF, Double_t *rList, Double_t *phiList,
                     Double_t *zList, TMatrixD **matricesDistDrDz, TMatrixD **matricesDistDPhiRDz,
                     TMatrixD **matricesDistDz, TMatrixD **matricesCorrDrDz, TMatrixD **matricesCorrDPhiRDz,
                     TMatrixD **matricesCorrDz, const Int_t rRow, const Int_t zColumn, const Int_t phiSlice,
                     const Double_t c0, const Double_t c1, const Double_t ezField, const Double_t dvdE,
                     const Int_t side);

void InitPotentialAndCharge3D(TFormula *vTestFunction, TFormula *rhoTestFunction, TFormula *erTestFunction,
                         TFormula *ePhiTestFunction, TFormula *ezTestFunction, TFormula *intErDzTestFunction,
                         TFormula *intEPhiRDzTestFunction, TFormula *intDzTestFunction, TMatrixD **matricesV,
                         TMatrixD **matricesCharge, TMatrixD **matricesEr, TMatrixD **matricesEPhi,
                         TMatrixD **matricesEz, TMatrixD **matricesDistDrDz, TMatrixD **matricesDistDPhiRDz,
                         TMatrixD **matricesDistDz, TMatrixD **matricesCorrDrDz, TMatrixD **matricesCorrDPhiRDz,
                         TMatrixD **matricesCorrDz, const Int_t rRow, const Int_t zColumn, const Int_t phiSlice,
                         const Int_t side, const Double_t c0, const Double_t c1, const Double_t ezField,
                         const Double_t dvdE);


/// main macro function, call unit test
void AliTPCSpaceCharge3DDriftLineTest() {
  UnitTestCorrectnessDistortion();
  UnitTestConsistencyDistortion();
}






/// unit test for Correctness
/// - a set of known functions and stored it in scExact
/// - generate numerical calculation in scNumeric
/// - calculate relative error (accepted if less than 10^-2)
void UnitTestCorrectnessDistortion() {

  const Int_t phiSlice = 18;
  const Int_t rRow = 9;
  const Int_t zColumn = 9;
  const Int_t maxIter = 100;
  const Float_t convergenceError = 1e-8;

  const Int_t nPhiSliceTest = 18;
  const Int_t nRRowTest = 9;
  const Int_t nZColumnTest = 9;
  const Int_t nPoints = nPhiSliceTest * nRRowTest * nZColumnTest;
  const Double_t maxEpsilon = 1e-2;

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion", "Begin");
  AliTPCSpaceCharge3DDriftLine *sc = new AliTPCSpaceCharge3DDriftLine("unitTestNumeric", "unitTestNumeric", rRow, zColumn, phiSlice);
  AliTPCSpaceCharge3DDriftLine *scExact = new AliTPCSpaceCharge3DDriftLine("unitTestExact", "unitTestExact", rRow, zColumn, phiSlice);

  
  TFormula vTestFunction1("f1", "[0]*(x^4 - 338.0 *x^3 + 21250.75 * x^2)*cos([1]* y)^2*exp(-1* [2] * z^2)");
  TFormula rhoTestFunction1("ff1", "[0]*(((16.0 * x^2 - 9.0 * 338.0 * x + 4.0*21250.75) *cos([1] * y)^2 * exp(-1 *[2]*z^2)) - ((x^2 -  338.0 * x + 21250.75) * 2 * [1]^2 * cos(2 * [1] * y) * exp(-1 *[2]*z^2)) + ((x^4 -  338.0 * x^3 + 21250.75 * x^2) * cos([1] * y)^2 * (4*[2]^2*z^2 - 2 * [2]) * exp(-1 *[2]*z^2)))");

  TFormula erTestFunction1("er", " [0]*(4*x^3 - 3 * 338.0 *x^2 + 2 * 21250.75 * x)*cos([1]* y)^2*exp(-1* [2] * z^2)");
  TFormula ePhiTestFunction1("ePhi",
                            "  [0]*(x^3 - 338.0 *x^2 +  21250.75 * x)* -1  * [1] * sin(2 * [1]* y)*exp(-1* [2] * z^2)");
  TFormula ezTestFunction1("ez",
                          " [0]*(x^4 - 338.0 *x^3 + 21250.75 * x^2)*cos([1]* y)^2*-1*2*[2]*z*exp(-1* [2] * z^2)");

  TFormula intErDzTestFunction1("intErDz",
                               " [0]*(4*x^3 - 3 * 338.0 *x^2 + 2 * 21250.75 * x)*cos([1]* y)^2*((sqrt(pi)*TMath::Erf(sqrt([2]) * z))/(2 * sqrt([2]))) ");
  TFormula intEPhiRDzTestFunction1("intEPhiDz",
                                  "[0]* (x^3 - 338.0 *x^2 +  21250.75 * x)* -1  * [1] * sin(2 * [1]* y)*((sqrt(pi)*TMath::Erf(sqrt([2]) * z))/(2 * sqrt([2])))");
  TFormula intDzTestFunction1("intEzDz",
                             "[0]* (x^4 - 338.0 *x^3 + 21250.75 * x^2)*cos([1]* y)^2*exp(-1* [2] * z^2)");


  // set parameters for function
  Double_t a = AliTPCPoissonSolver::fgkOFCRadius * AliTPCPoissonSolver::fgkOFCRadius;
  a *= (AliTPCPoissonSolver::fgkOFCRadius - AliTPCPoissonSolver::fgkIFCRadius);
  a *= (AliTPCPoissonSolver::fgkOFCRadius - AliTPCPoissonSolver::fgkIFCRadius);
  a =  (1000.0 / a);
  a = 1e-7;
  Double_t b = 0.5;
  Double_t c = 1.0 / (((AliTPCPoissonSolver::fgkTPCZ0) / 2.0) * ((AliTPCPoissonSolver::fgkTPCZ0) / 2.0));
  c = 1e-4;

  vTestFunction1.SetParameters(a, b, c);
  rhoTestFunction1.SetParameters(a, b, c);

  erTestFunction1.SetParameters(-a, b, c);
  ePhiTestFunction1.SetParameters(-a, b, c);
  ezTestFunction1.SetParameters(-a, b, c);
  intErDzTestFunction1.SetParameters(-a, b, c);
  intEPhiRDzTestFunction1.SetParameters(-a, b, c);
  intDzTestFunction1.SetParameters(-a, b, c);

  TFormula *vTestFunction = &vTestFunction1;
  TFormula *rhoTestFunction = &rhoTestFunction1;


  TFormula *erTestFunction = &erTestFunction1;
  TFormula *ePhiTestFunction = &ePhiTestFunction1;
  TFormula *ezTestFunction = &ezTestFunction1;

  TFormula *intErDzTestFunction = &intErDzTestFunction1;
  TFormula *intEPhiRDzTestFunction = &intEPhiRDzTestFunction1;
  TFormula *intDzTestFunction = &intDzTestFunction1;



  Float_t ofcRadius = AliTPCPoissonSolver::fgkOFCRadius;
  Float_t ifcRadius = AliTPCPoissonSolver::fgkIFCRadius;
  Float_t tpcZ0 = AliTPCPoissonSolver::fgkTPCZ0;

  Double_t ezField = (AliTPCPoissonSolver::fgkCathodeV - AliTPCPoissonSolver::fgkGG) /
                     AliTPCPoissonSolver::fgkTPCZ0; // = ALICE Electric Field (V/cm) Magnitude ~ -400 V/cm;
  Double_t dvdE = AliTPCPoissonSolver::fgkdvdE;

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion","Setting up Poisson Solver and Boundary Charge");
  // create Poisson Solver

  sc->SetPotentialBoundaryAndCharge(vTestFunction, rhoTestFunction);
  sc->SetCorrectionType(0);
  sc->SetOmegaTauT1T2(0.35, 1., 1.);

  scExact->SetPotentialBoundaryAndCharge(vTestFunction, rhoTestFunction);
  scExact->SetCorrectionType(0);
  scExact->SetOmegaTauT1T2(0.35, 1., 1.);

  Float_t c0 = scExact->GetC0();
  Float_t c1 = scExact->GetC1();

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion", "Create Distortion/Correction maps");
  // for numeric
  sc->InitSpaceCharge3DPoissonIntegralDz(rRow, zColumn, phiSlice, maxIter, convergenceError);

  // for exact equation




  TMatrixD *matricesDistDrDzExactA[phiSlice];
  TMatrixD *matricesDistDrDzExactC[phiSlice];
  TMatrixD *matricesDistDPhiRDzExactA[phiSlice];
  TMatrixD *matricesDistDPhiRDzExactC[phiSlice];
  TMatrixD *matricesDistDzExactA[phiSlice];
  TMatrixD *matricesDistDzExactC[phiSlice];
  TMatrixD *matricesCorrDrDzExactA[phiSlice];
  TMatrixD *matricesCorrDrDzExactC[phiSlice];
  TMatrixD *matricesCorrDPhiRDzExactA[phiSlice];
  TMatrixD *matricesCorrDPhiRDzExactC[phiSlice];
  TMatrixD *matricesCorrDzExactA[phiSlice];
  TMatrixD *matricesCorrDzExactC[phiSlice];

  TMatrixD *matricesChargeA[phiSlice];
  TMatrixD *matricesChargeC[phiSlice];
  TMatrixD *matricesVExactA[phiSlice];
  TMatrixD *matricesVExactC[phiSlice];
  TMatrixD *matricesErExactA[phiSlice];
  TMatrixD *matricesErExactC[phiSlice];
  TMatrixD *matricesEPhiExactA[phiSlice];
  TMatrixD *matricesEPhiExactC[phiSlice];
  TMatrixD *matricesEzExactA[phiSlice];
  TMatrixD *matricesEzExactC[phiSlice];

  for (Int_t m = 0; m < phiSlice; m++) {
    matricesDistDrDzExactA[m] = new TMatrixD(rRow, zColumn);
    matricesDistDPhiRDzExactA[m] = new TMatrixD(rRow, zColumn);
    matricesDistDzExactA[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDrDzExactA[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDPhiRDzExactA[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDzExactA[m] = new TMatrixD(rRow, zColumn);

    matricesDistDrDzExactC[m] = new TMatrixD(rRow, zColumn);
    matricesDistDPhiRDzExactC[m] = new TMatrixD(rRow, zColumn);
    matricesDistDzExactC[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDrDzExactC[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDPhiRDzExactC[m] = new TMatrixD(rRow, zColumn);
    matricesCorrDzExactC[m] = new TMatrixD(rRow, zColumn);

    matricesVExactA[m] = new TMatrixD(rRow, zColumn);
    matricesChargeA[m] = new TMatrixD(rRow, zColumn);
    matricesErExactA[m] = new TMatrixD(rRow, zColumn);
    matricesEPhiExactA[m] = new TMatrixD(rRow, zColumn);
    matricesEzExactA[m] = new TMatrixD(rRow, zColumn);

    matricesVExactC[m] = new TMatrixD(rRow, zColumn);
    matricesChargeC[m] = new TMatrixD(rRow, zColumn);
    matricesErExactC[m] = new TMatrixD(rRow, zColumn);
    matricesEPhiExactC[m] = new TMatrixD(rRow, zColumn);
    matricesEzExactC[m] = new TMatrixD(rRow, zColumn);
  }

  InitPotentialAndCharge3D(vTestFunction, rhoTestFunction, erTestFunction, ePhiTestFunction, ezTestFunction,
                      intErDzTestFunction, intEPhiRDzTestFunction, intDzTestFunction, matricesVExactA,
                      matricesChargeA, matricesErExactA, matricesEPhiExactA, matricesEzExactA, matricesDistDrDzExactA,
                      matricesDistDPhiRDzExactA, matricesDistDzExactA, matricesCorrDrDzExactA,
                      matricesCorrDPhiRDzExactA, matricesCorrDzExactA, rRow, zColumn, phiSlice, 0, c0, c1, ezField,
                      dvdE);

  InitPotentialAndCharge3D(vTestFunction, rhoTestFunction, erTestFunction, ePhiTestFunction, ezTestFunction,
                      intErDzTestFunction, intEPhiRDzTestFunction, intDzTestFunction, matricesVExactC,
                      matricesChargeC, matricesErExactC, matricesEPhiExactC, matricesEzExactC, matricesDistDrDzExactC,
                      matricesDistDPhiRDzExactC, matricesDistDzExactC, matricesCorrDrDzExactC,
                      matricesCorrDPhiRDzExactC, matricesCorrDzExactC, rRow, zColumn, phiSlice, 1, c0, c1, ezField,
                      dvdE);

  scExact->InitSpaceCharge3DPoissonIntegralDz(rRow, zColumn, phiSlice, maxIter, convergenceError,
                                              matricesDistDrDzExactA, matricesDistDPhiRDzExactA, matricesDistDzExactA,
                                              matricesCorrDrDzExactA, matricesCorrDPhiRDzExactA, matricesCorrDzExactA,
                                              matricesDistDrDzExactC, matricesDistDPhiRDzExactC, matricesDistDzExactC,
                                              matricesCorrDrDzExactC, matricesCorrDPhiRDzExactC, matricesCorrDzExactC,
                                              intErDzTestFunction, intEPhiRDzTestFunction, intDzTestFunction);

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyCorrectnessDistortion", "Creating test points");
  sc->CreateDistortionTree(nRRowTest,nZColumnTest,nPhiSliceTest);
  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyCorrectnessDistortion", "Finish for numeric");
  scExact->CreateDistortionTree(nRRowTest,nZColumnTest,nPhiSliceTest);
  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyCorrectnessDistortion", "Finish for exact");

  // calculate error for
  TFile fileNumeric(TString::Format("distortion%s.root",sc->GetName()));
  TTree *treeNumeric = (TTree *)fileNumeric.Get("distortion");
  treeNumeric->AddFriend("distortionExact = distortion",TString::Format("distortion%s.root",scExact->GetName()));

  Int_t nPoint = treeNumeric->GetEntries();


  treeNumeric->Draw("abs(distortionExact.drDist)", "", "goff");
  Double_t *numericList = treeNumeric->GetV1();
  Double_t maxVar = TMath::MaxElement(nPoint, numericList);
  treeNumeric->Draw(TString::Format("abs(distortionExact.drDist-drDist)/%f",maxVar), "", "goff");
  numericList = treeNumeric->GetV1();

  Double_t residueMean = TMath::Mean(nPoint, numericList);
  Double_t residueRMS = TMath::RMS(nPoint, numericList);
  Double_t residueMax = TMath::MaxElement(nPoint, numericList);

  printf("====== Correctness Testing (numeric vs exact solution) =====\n");
  printf("V(r,phi,z) = %s\n", (vTestFunction->GetExpFormula()).Data());
  printf("rho(r,phi,z) = %s\n", (rhoTestFunction->GetExpFormula()).Data());
  printf("drDist\n");
  printf("Mean Residue drDist \t\t= %.5E\n",residueMean);
  printf("RMS  Residue drDist \t\t= %.5E\n",residueRMS);
  printf("Max  Residue drDist \t\t= %.5E\n",residueMax);

  treeNumeric->Draw("abs(distortionExact.drPhiDist)", "", "goff");
  numericList = treeNumeric->GetV1();
  maxVar = TMath::MaxElement(nPoint, numericList);
  treeNumeric->Draw(TString::Format("abs(distortionExact.drPhiDist-drPhiDist)/%f",maxVar), "", "goff");
  numericList = treeNumeric->GetV1();
  Double_t residueMean2 = TMath::Mean(nPoint, numericList);
  Double_t residueRMS2 = TMath::RMS(nPoint, numericList);
  Double_t residueMax2 = TMath::MaxElement(nPoint, numericList);

  printf("drPhiDist\n");
  printf("Mean Residue drPhiDist \t= %.5E\n",residueMean2);
  printf("RMS  Residue drPhiDist \t= %.5E\n",residueRMS2);
  printf("Max  Residue drPhiDist \t= %.5E\n",residueMax2);

  treeNumeric->Draw("abs(distortionExact.dzDist)", "", "goff");
  numericList = treeNumeric->GetV1();
  maxVar = TMath::MaxElement(nPoint, numericList);
  treeNumeric->Draw(TString::Format("abs(distortionExact.dzDist-dzDist)/%f",maxVar), "", "goff");
  numericList = treeNumeric->GetV1();
  Double_t residueMean3 = TMath::Mean(nPoint, numericList);
  Double_t residueRMS3 = TMath::RMS(nPoint, numericList);
  Double_t residueMax3 = TMath::MaxElement(nPoint, numericList);

  printf("dzDist\n");
  printf("Mean Residue dzDist \t\t= %.5E\n",residueMean3);
  printf("RMS  Residue dzDist \t\t= %.5E\n",residueRMS3);
  printf("Max  Residue dzDist \t\t= %.5E\n",residueMax3);

  if (residueMean < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) drDist",
           "Test OK: Mean Residue=%.2E < %.2E",
           residueMean, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) drDist",
            "Test FAILED: Mean Residue=%.2E > %.2E",
            residueMean, maxEpsilon);
  if (residueMean2 < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) drPhiDist",
           "Test OK: Mean Residue=%.2E < %.2E", residueMean2, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) drPhiDist",
            "Test FAILED: Mean Residue=%.2E > %.2E", residueMean2, maxEpsilon);
  if (residueMean3 < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) dzDist",
           "Test OK: Mean Residue=%.2E < %.2E", residueMean3, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion (Correctness) dzDist",
            "Test FAILED: Mean Residue=%.2E > %.2E", residueMean3, maxEpsilon);



  delete sc;
  delete scExact;


  for (Int_t m = 0;m <phiSlice;m++) {
    delete matricesVExactA[m];
    delete matricesChargeA[m];
    delete matricesVExactC[m];
    delete matricesChargeC[m];
    delete matricesErExactA[m];
    delete matricesEPhiExactA[m];
    delete matricesEzExactA[m];
    delete matricesErExactC[m];
    delete matricesEPhiExactC[m];
    delete matricesEzExactC[m];
    delete matricesDistDrDzExactA[m];
    delete matricesDistDPhiRDzExactA[m];
    delete matricesDistDzExactA[m];
    delete matricesCorrDrDzExactA[m];
    delete matricesCorrDPhiRDzExactA[m];
    delete matricesCorrDzExactA[m];
    delete matricesDistDrDzExactC[m];
    delete matricesDistDPhiRDzExactC[m];
    delete matricesDistDzExactC[m];
    delete matricesCorrDrDzExactC[m];
    delete matricesCorrDPhiRDzExactC[m];
    delete matricesCorrDzExactC[m];
  }
  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestCorrectnessDistortion", "End");
}



/// unit test for Consistency
/// - a known potential function at boundary, charge distribution
/// - generate numerical calculation of distortions and corrections in sc
/// - calc
void UnitTestConsistencyDistortion() {
  const Int_t phiSlice = 18;
  const Int_t rRow = 9;
  const Int_t zColumn = 9;
  const Int_t maxIter = 100;
  const Float_t convergenceError = 1e-8;

  const Int_t nPhiSliceTest = 18;
  const Int_t nRRowTest = 9;
  const Int_t nZColumnTest = 9;
  const Int_t nPoints = nPhiSliceTest * nRRowTest * nZColumnTest;
  const Double_t maxEpsilon = 1e-2;

  const Double_t zShort = 40;
  const Double_t amplitude = -1000;

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion", "Begin");


  // set potential boundary in V
  TF1 * potentialBoundaryFunctionInZ = new TF1("dFunctionVZ", dFunctionVZ, -250.0, 250.0,2);
  potentialBoundaryFunctionInZ->SetParName(0,"zShort");
  potentialBoundaryFunctionInZ->SetParName(1,"amplitude");
  potentialBoundaryFunctionInZ->SetParameter(0,zShort);
  potentialBoundaryFunctionInZ->SetParameter(1,amplitude);
  potentialBoundaryFunctionInZ->Draw();


  // charge in histogram
  TH3 * chargeA = new TH3F ("charge A","charge A",250, 0.0, TMath::TwoPi(), 250, 85.0, 250.0, 250,0.0,250.0);
  TH3 * chargeC = new TH3F ("charge C","charge C",250, 0.0, TMath::TwoPi(), 250, 85.0, 250.0, 250,0.0,250.0);

  ::Info( "distortionIFCDeltaPotential","Begin");
  AliTPCSpaceCharge3DDriftLine * sc = new AliTPCSpaceCharge3DDriftLine(TString::Format("distortionIFCDeltaPotentialRRow%dZCol%dPhiSlice%d",rRow,zColumn,phiSlice).Data(),TString::Format("distortionIFCDeltaPotentialRRow%dZCol%dPhiSlice%d",rRow,zColumn,phiSlice).Data(), rRow,zColumn,phiSlice);

  // set the input boundary potential and charge densities
  sc->SetInputSpaceCharge(chargeA,0);
  sc->SetInputSpaceCharge(chargeC,1);
  sc->SetBoundaryIFCA(potentialBoundaryFunctionInZ);
  sc->SetOmegaTauT1T2(0.35,1.,1.);
  sc->InitSpaceCharge3DPoissonIntegralDz(rRow,zColumn,phiSlice,gkMaxIter,gkConvError);

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion", "Creating test points");
  sc->CreateDistortionTree(nRRowTest,nZColumnTest,nPhiSliceTest);
  sc->CreateDistortionTree(5.0);

  ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion", "Finish for numeric");
  printf("====== Test 2:  Consistency Testing relative error (distortion- correction) =====\n");


  printf("zShort = %.3f cm, V (max)=%.f V\n",zShort,amplitude);

  TFile fileNumeric(TString::Format("distortion%s.root",sc->GetName()));
  TTree *treeNumeric = (TTree *)fileNumeric.Get("distortion");
  Int_t nPoint = treeNumeric->GetEntries();

  treeNumeric->Draw("abs(drDist)", "", "goff");
  Double_t * numericList = treeNumeric->GetV1();
  Double_t maxVar = TMath::MaxElement(nPoint, numericList);
  Double_t meanVar = TMath::Mean(nPoint, numericList);
  Double_t rmsVar = TMath::RMS(nPoint, numericList);
  printf("drDist (distortion r)\n");
  printf("mean drDist \t\t\t= %.5E cm\nrms drDist\t\t\t= %.5E cm\nmax drDist\t\t\t= %.5E cm\n",meanVar,rmsVar,maxVar);


  treeNumeric->Draw("abs(drDist+drCorr)", "", "goff");
  numericList = treeNumeric->GetV1();

  Double_t residueMean4 = TMath::Mean(nPoint, numericList);
  Double_t residueRMS4 = TMath::RMS(nPoint, numericList);
  Double_t residueMax4 = TMath::MaxElement(nPoint, numericList);

  printf("Mean Residue drDist \t\t= %.5E cm\n", residueMean4);
  printf("RMS  Residue drDist \t\t= %.5E cm\n", residueRMS4);
  printf("Max  Residue drDist \t\t= %.5E cm\n", residueMax4);

  treeNumeric->Draw("abs(drPhiDist)", "", "goff");
  numericList = treeNumeric->GetV1();
  maxVar = TMath::MaxElement(nPoint, numericList);
  meanVar = TMath::Mean(nPoint, numericList);
  rmsVar = TMath::RMS(nPoint, numericList);
  printf("drPhiDist (r * distortion Phi)\n");
  printf("mean drPhiDist \t\t= %.5E cm\nrms drPhiDist \t\t= %.5E cm\nmax drPhiDist \t\t= %.5E cm\n",meanVar,rmsVar,maxVar);

  treeNumeric->Draw("abs(drPhiDist+drPhiCorr)", "", "goff");
  numericList = treeNumeric->GetV1();

  Double_t residueMean5 = TMath::Mean(nPoint, numericList);
  Double_t residueRMS5 = TMath::RMS(nPoint, numericList);
  Double_t residueMax5 = TMath::MaxElement(nPoint, numericList);

  printf("Mean Residue drPhiDist \t= %.5E cm\n", residueMean5);
  printf("RMS  Residue drPhiDist \t= %.5E cm\n", residueRMS5);
  printf("Max  Residue drPhiDist \t= %.5E cm\n", residueMax5);

  treeNumeric->Draw("abs(dzDist)", "", "goff");
  numericList = treeNumeric->GetV1();
  maxVar = TMath::MaxElement(nPoint, numericList);
  meanVar = TMath::Mean(nPoint, numericList);
  rmsVar = TMath::RMS(nPoint, numericList);

  printf("dzDist (distortion z)\n");
  printf("mean dzDist \t\t\t= %.5E cm\nrms dzDist\t\t\t= %.5E cm\nmax dzDist\t\t\t= %.5E cm\n",meanVar,rmsVar,maxVar);


  treeNumeric->Draw("abs(dzDist+dzCorr)", "", "goff");
  numericList = treeNumeric->GetV1();

  Double_t residueMean6 = TMath::Mean(nPoint, numericList);
  Double_t residueRMS6 = TMath::RMS(nPoint, numericList);
  Double_t residueMax6 = TMath::MaxElement(nPoint, numericList);

  printf("Mean Residue dzDist \t\t= %.5E cm\n", residueMean6);
  printf("RMS  Residue dzDist \t\t= %.5E cm\n", residueRMS6);
  printf("Max  Residue dzDist \t\t= %.5E cm\n", residueMax6);


  if (residueMean4 < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion (Consistency) drDist",
           "Test OK: Mean Residue=%.2E < %.2E", residueMean4, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion(Consistency) drDist",
            "Test FAILED: Mean Residue=%.2E > %.2E", residueMean4, maxEpsilon);

  if (residueMean5 < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion (Consistency) drPhiDist",
           "Test OK: Mean Residue=%.2E < %.2E", residueMean5, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion(Consistency) drPhiDist",
            "Test FAILED: Mean Residue=%.2E > %.2E", residueMean5, maxEpsilon);

  if (residueMean6 < maxEpsilon)
    ::Info("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion (Consistency) dzDist",
           "Test OK: Mean Residue=%.2E < %.2E", residueMean6, maxEpsilon);
  else
    ::Error("AliTPCSpaceCharge3DDriftLineTest::UnitTestConsistencyDistortion(Consistency) dzDist",
            "Test FAILED: Mean Residue=%.2E > %.2E", residueMean6, maxEpsilon);

}


/// for exact functions
///
/// \param vTestFunction
/// \param rhoTestFunction
/// \param erTestFunction
/// \param ePhiTestFunction
/// \param ezTestFunction
/// \param intErDzTestFunction
/// \param intEPhiRDzTestFunction
/// \param intDzTestFunction
/// \param matricesV
/// \param matricesCharge
/// \param matricesEr
/// \param matricesEPhi
/// \param matricesEz
/// \param matricesDistDrDz
/// \param matricesDistDPhiRDz
/// \param matricesDistDz
/// \param matricesCorrDrDz
/// \param matricesCorrDPhiRDz
/// \param matricesCorrDz
/// \param rRow
/// \param zColumn
/// \param phiSlice
/// \param side
/// \param c0
/// \param c1
/// \param ezField
/// \param dvdE
void InitPotentialAndCharge3D(TFormula *vTestFunction, TFormula *rhoTestFunction, TFormula *erTestFunction,
                         TFormula *ePhiTestFunction, TFormula *ezTestFunction, TFormula *intErDzTestFunction,
                         TFormula *intEPhiRDzTestFunction, TFormula *intDzTestFunction, TMatrixD **matricesV,
                         TMatrixD **matricesCharge, TMatrixD **matricesEr, TMatrixD **matricesEPhi,
                         TMatrixD **matricesEz, TMatrixD **matricesDistDrDz, TMatrixD **matricesDistDPhiRDz,
                         TMatrixD **matricesDistDz, TMatrixD **matricesCorrDrDz, TMatrixD **matricesCorrDPhiRDz,
                         TMatrixD **matricesCorrDz, const Int_t rRow, const Int_t zColumn, const Int_t phiSlice,
                         const Int_t side, const Double_t c0, const Double_t c1, const Double_t ezField,
                         const Double_t dvdE) {
  Double_t rList[rRow], zedList[zColumn], phiList[phiSlice];
  Float_t ofcRadius = AliTPCPoissonSolver::fgkOFCRadius;
  Float_t ifcRadius = AliTPCPoissonSolver::fgkIFCRadius;
  Float_t tpcZ0 = AliTPCPoissonSolver::fgkTPCZ0;

  Float_t dRadius = (AliTPCPoissonSolver::fgkOFCRadius - AliTPCPoissonSolver::fgkIFCRadius) / (rRow - 1);
  Float_t dZ = AliTPCPoissonSolver::fgkTPCZ0 / (zColumn - 1);
  Float_t dPhi = TMath::TwoPi() / phiSlice;



  // list points on grid in cm
  for (Int_t k = 0; k < phiSlice; k++)
    phiList[k] = dPhi * k;
  for (Int_t i = 0; i < rRow; i++)
    rList[i] = AliTPCPoissonSolver::fgkIFCRadius + i * dRadius;
  for (Int_t j = 0; j < zColumn; j++)
    zedList[j] = j * dZ;

  TMatrixD *arrayV;
  TMatrixD *charge;
  TMatrixD *er, *ePhi, *ez;

  Double_t radius0, z0, phi0;

  for (Int_t k = 0; k < phiSlice; k++) {
    arrayV = matricesV[k];
    charge = matricesCharge[k];
    er = matricesEr[k];
    ePhi = matricesEPhi[k];
    ez = matricesEz[k];
    phi0 = phiList[k];

    /// Fill the non-boundary values
    for (Int_t i = 0; i < rRow; i++) {
      radius0 = rList[i];
      for (Int_t j = 0; j < zColumn; j++) {
        z0 = zedList[j];

        // Exact solution
        (*arrayV)(i, j) = vTestFunction->Eval(radius0, phi0, z0);
        (*charge)(i, j) = -1.0 * rhoTestFunction->Eval(radius0, phi0, z0);


        (*er)(i, j) = -1.0 * erTestFunction->Eval(radius0, phi0, z0);
        (*ePhi)(i, j) = -1.0 * ePhiTestFunction->Eval(radius0, phi0, z0);
        (*ez)(i, j) = -1.0 * ezTestFunction->Eval(radius0, phi0, z0);
        //if (side == 1) (*ez)(i, j) = -(*ez)(i, j);
      } // end j
    } // end i
  } // end phi


  LocalDistCorrDzExact(intErDzTestFunction, intEPhiRDzTestFunction, intDzTestFunction, rList, phiList, zedList,
                       matricesDistDrDz, matricesDistDPhiRDz, matricesDistDz, matricesCorrDrDz, matricesCorrDPhiRDz,
                       matricesCorrDz, rRow, zColumn, phiSlice, c0, c1, ezField, dvdE, side);
}

/// for correctness analysis
///
/// \param intDrDzF
/// \param intDPhiDzF
/// \param intDzDzF
/// \param rList
/// \param phiList
/// \param zList
/// \param matricesDistDrDz
/// \param matricesDistDPhiRDz
/// \param matricesDistDz
/// \param matricesCorrDrDz
/// \param matricesCorrDPhiRDz
/// \param matricesCorrDz
/// \param rRow
/// \param zColumn
/// \param phiSlice
/// \param c0
/// \param c1
/// \param ezField
/// \param dvdE
/// \param side
void
LocalDistCorrDzExact(TFormula *intDrDzF, TFormula *intDPhiDzF, TFormula *intDzDzF, Double_t *rList, Double_t *phiList,
                     Double_t *zList, TMatrixD **matricesDistDrDz, TMatrixD **matricesDistDPhiRDz,
                     TMatrixD **matricesDistDz, TMatrixD **matricesCorrDrDz, TMatrixD **matricesCorrDPhiRDz,
                     TMatrixD **matricesCorrDz, const Int_t rRow, const Int_t zColumn, const Int_t phiSlice,
                     const Double_t c0, const Double_t c1, const Double_t ezField, const Double_t dvdE,
                     const Int_t side) {
  Double_t r0, z0, phi0, z1;
  Float_t localIntErOverEz = 0.0;
  Float_t localIntEPhiOverEz = 0.0;
  Float_t localIntDeltaEz = 0.0;

  // pointer declaration
  TMatrixD *distDrDz;
  TMatrixD *distDPhiRDz;
  TMatrixD *distDz;
  TMatrixD *corrDrDz;
  TMatrixD *corrDPhiRDz;
  TMatrixD *corrDz;


  for (Int_t m = 0; m < phiSlice; m++) {
    distDrDz = matricesDistDrDz[m];
    distDPhiRDz = matricesDistDPhiRDz[m];
    distDz = matricesDistDz[m];

    corrDrDz = matricesCorrDrDz[m];
    corrDPhiRDz = matricesCorrDPhiRDz[m];
    corrDz = matricesCorrDz[m];

    for (Int_t i = 0; i < rRow; i++) {
      (*distDrDz)(i, zColumn - 1) = 0.0;
      (*distDPhiRDz)(i, zColumn - 1) = 0.0;
      (*distDz)(i, zColumn - 1) = 0.0;

      (*corrDrDz)(i, 0) = 0.0;
      (*corrDPhiRDz)(i, 0) = 0.0;
      (*corrDz)(i, 0) = 0.0;
    }

  }

  // for this case
  // use trapezoidal rule assume no ROC displacement
  for (Int_t m = 0; m < phiSlice; m++) {
    phi0 = phiList[m];

    distDrDz = matricesDistDrDz[m];
    distDPhiRDz = matricesDistDPhiRDz[m];
    distDz = matricesDistDz[m];

    corrDrDz = matricesCorrDrDz[m];
    corrDPhiRDz = matricesCorrDPhiRDz[m];
    corrDz = matricesCorrDz[m];

    for (Int_t j = 0; j < zColumn - 1; j++) {
      z0 = zList[j];
      z1 = zList[j + 1];
      for (Int_t i = 0; i < rRow; i++) {
        r0 = rList[i];

        if (side == 0) {
          localIntErOverEz = (intDrDzF->Eval(r0, phi0, z1) - intDrDzF->Eval(r0, phi0, z0)) / (-1 * ezField);
          localIntEPhiOverEz = (intDPhiDzF->Eval(r0, phi0, z1) - intDPhiDzF->Eval(r0, phi0, z0)) / (-1 * ezField);
          localIntDeltaEz = intDzDzF->Eval(r0, phi0, z1) - intDzDzF->Eval(r0, phi0, z0);
        } else {
          localIntErOverEz = (intDrDzF->Eval(r0, phi0, z1) - intDrDzF->Eval(r0, phi0, z0)) / (-1 * ezField);
          localIntEPhiOverEz = (intDPhiDzF->Eval(r0, phi0, z1) - intDPhiDzF->Eval(r0, phi0, z0)) / (-1 * ezField);
          localIntDeltaEz = intDzDzF->Eval(r0, phi0, z1) - intDzDzF->Eval(r0, phi0, z0);
        }

        (*distDrDz)(i, j) = c0 * localIntErOverEz + c1 * localIntEPhiOverEz;
        (*distDPhiRDz)(i, j) = c0 * localIntEPhiOverEz - c1 * localIntErOverEz;
        (*distDz)(i, j) = localIntDeltaEz * dvdE * dvdE; // two times?

        //(*distDrDz)(i,j) 		= localIntErOverEz;
        //(*distDPhiRDz)(i,j) = localIntEPhiOverEz;
        //(*distDz)(i,j)      = localIntDeltaEz; // two times?

        (*corrDrDz)(i, j + 1) = -1 * (*distDrDz)(i, j);
        (*corrDPhiRDz)(i, j + 1) = -1 * (*distDPhiRDz)(i, j);
        (*corrDz)(i, j + 1) = -1 * (*distDz)(i, j);
      }
    }
  }
}




// function for dV(z)
Double_t dFunctionVZ(Double_t *x, Double_t *par)
{
  /// dU=0 at (0 and at 250)
  /// -k0*z     at z(0,z0)
  /// -k0*z0+k1*z   at z(z0,250)
  /// -k0*z0+k1*250=0
  ///  k1=k0*z0/250.

  Float_t xx =x[0];
  const Float_t z0 =par[0]; // U at central electrode
  const Float_t k0 =par[1]; // z0
  //const Float_t k1 = k0 * z0 / AliTPCPoissonSolver::fgkTPCZ0; // k1

  const Float_t k1 = k0  / (AliTPCPoissonSolver::fgkTPCZ0 - z0);
  Double_t f = 0.0;

  // z location near
  if (TMath::Abs(xx) < TMath::Abs(z0)) {
    // ideal case
    f = -k0 * (TMath::Abs(xx) / TMath::Abs(z0)) ;
  } else {
    f = -k0 + k1 * (TMath::Abs(xx) - z0);
  }

  return f;
}
