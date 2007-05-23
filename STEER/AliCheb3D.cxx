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

// Author: ruben.shahoyan@cern.ch   09/09/2006
//
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliCheb3D produces the interpolation of the user 3D->NDimOut arbitrary     //
// function supplied in "void (*fcn)(float* inp,float* out)" format           //
// either in a separate macro file or as a function pointer.                  //
// Only coefficients needed to guarantee the requested precision are kept.    //
//                                                                            //
// The user-callable methods are:                                             //
// To create the interpolation use:                                           //
// AliCheb3D(const char* funName,  // name of the file with user function     //
//          or                                                                //
// AliCheb3D(void (*ptr)(float*,float*),// pointer on the  user function      //
//        Int_t     DimOut,     // dimensionality of the function's output    // 
//        Float_t  *bmin,       // lower 3D bounds of interpolation domain    // 
//        Float_t  *bmax,       // upper 3D bounds of interpolation domain    // 
//        Int_t    *npoints,    // number of points in each of 3 input        //
//                              // dimension, defining the interpolation grid //
//        Float_t   prec=1E-6); // requested max.absolute difference between  //
//                              // the interpolation and any point on grid    //
//                                                                            //
// To test obtained parameterization use the method                           //
// TH1* TestRMS(int idim,int npoints = 1000,TH1* histo=0);                    // 
// it will compare the user output of the user function and interpolation     //
// for idim-th output dimension and fill the difference in the supplied       //
// histogram. If no histogram is supplied, it will be created.                //
//                                                                            //
// To save the interpolation data:                                            //
// SaveData(const char* filename, Bool_t append )                             //
// write text file with data. If append is kTRUE and the output file already  //
// exists, data will be added in the end of the file.                         //
// Alternatively, SaveData(FILE* stream) will write the data to               //
// already existing stream.                                                   //
//                                                                            //
// To read back already stored interpolation use either the constructor       // 
// AliCheb3D(const char* inpFile);                                            //
// or the default constructor AliCheb3D() followed by                         //
// AliCheb3D::LoadData(const char* inpFile);                                  //
//                                                                            //
// To compute the interpolation use Eval(float* par,float *res) method, with  //
// par being 3D vector of arguments (inside the validity region) and res is   //
// the array of DimOut elements for the output.                               //
//                                                                            //
// If only one component (say, idim-th) of the output is needed, use faster   //
// Float_t Eval(Float_t *par,int idim) method.                                //
//                                                                            //
// void Print(option="") will print the name, the ranges of validity and      //
// the absolute precision of the parameterization. Option "l" will also print //
// the information about the number of coefficients for each output           //
// dimension.                                                                 //
//                                                                            //
// NOTE: during the evaluation no check is done for parameter vector being    //
// outside the interpolation region. If there is such a risk, use             //
// Bool_t IsInside(float *par) method. Chebyshev parameterization is not      //
// good for extrapolation!                                                    //
//                                                                            //
// For the properties of Chebyshev parameterization see:                      //
// H.Wind, CERN EP Internal Report, 81-12/Rev.                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <TString.h>
#include <TSystem.h>
#include <TRandom.h>
#include <TROOT.h>
#include "AliCheb3D.h"
#include "AliLog.h"



ClassImp(AliCheb3DCalc)


AliCheb3DCalc::AliCheb3DCalc():
    TNamed("", ""),
    fNCoefs(0),  
    fNRows(0),
    fNCols(0),
    fNElemBound2D(0),
    fNColsAtRow(0),
    fColAtRowBg(0),
    fCoefBound2D0(0),
    fCoefBound2D1(0),
    fCoefs(0),
    fTmpCf1(0),
    fTmpCf0(0)
{
    // Default constructor
    Init0();
}

AliCheb3DCalc::AliCheb3DCalc(FILE* stream):
    TNamed("", ""),
    fNCoefs(0),  
    fNRows(0),
    fNCols(0),
    fNElemBound2D(0),
    fNColsAtRow(0),
    fColAtRowBg(0),
    fCoefBound2D0(0),
    fCoefBound2D1(0),
    fCoefs(0),
    fTmpCf1(0),
    fTmpCf0(0)
{
    // Default constructor
    Init0();
    LoadData(stream);
}


AliCheb3DCalc::AliCheb3DCalc(const AliCheb3DCalc& src) :
    TNamed(src), 
    fNCoefs(src.fNCoefs), 
    fNRows(src.fNRows), 
    fNCols(src.fNCols),
    fNElemBound2D(src.fNElemBound2D), 
    fNColsAtRow(0), 
    fColAtRowBg(0), 
    fCoefBound2D0(0), 
    fCoefBound2D1(0), 
    fCoefs(0), 
    fTmpCf1(0), 
    fTmpCf0(0)
{
    // Copy constructor
    if (src.fNColsAtRow) {
	fNColsAtRow = new Int_t[fNRows]; 
	for (int i=fNRows;i--;) fNColsAtRow[i] = src.fNColsAtRow[i];
    }
    if (src.fColAtRowBg) {
	fColAtRowBg = new Int_t[fNRows]; 
	for (int i=fNRows;i--;) fColAtRowBg[i] = src.fColAtRowBg[i];
    }
    if (src.fCoefBound2D0) {
	fCoefBound2D0 = new Int_t[fNElemBound2D];
	for (int i=fNElemBound2D;i--;) fCoefBound2D0[i] = src.fCoefBound2D0[i];
    }
    if (src.fCoefBound2D1) {
	fCoefBound2D1 = new Int_t[fNElemBound2D];
	for (int i=fNElemBound2D;i--;) fCoefBound2D1[i] = src.fCoefBound2D1[i];
    }
    if (src.fCoefs) {
	fCoefs = new Float_t[fNCoefs];
	for (int i=fNCoefs;i--;) fCoefs[i] = src.fCoefs[i];
    }
    if (src.fTmpCf1) fTmpCf1 = new Float_t[fNCols];
    if (src.fTmpCf0) fTmpCf0 = new Float_t[fNRows];
}

AliCheb3DCalc& AliCheb3DCalc::operator=(const AliCheb3DCalc& rhs)
{
    // Assignment operator
    if (this != &rhs) {
	Clear();
	SetName(rhs.GetName());
	SetTitle(rhs.GetTitle());
	fNCoefs = rhs.fNCoefs;
	fNRows  = rhs.fNRows;
	fNCols  = rhs.fNCols;    
	if (rhs.fNColsAtRow) {
	    fNColsAtRow = new Int_t[fNRows]; 
	    for (int i=fNRows;i--;) fNColsAtRow[i] = rhs.fNColsAtRow[i];
	}
	if (rhs.fColAtRowBg) {
	    fColAtRowBg = new Int_t[fNRows]; 
	    for (int i=fNRows;i--;) fColAtRowBg[i] = rhs.fColAtRowBg[i];
	}
	if (rhs.fCoefBound2D0) {
	    fCoefBound2D0 = new Int_t[fNElemBound2D];
	    for (int i=fNElemBound2D;i--;) fCoefBound2D0[i] = rhs.fCoefBound2D0[i];
	}
	if (rhs.fCoefBound2D1) {
	    fCoefBound2D1 = new Int_t[fNElemBound2D];
	    for (int i=fNElemBound2D;i--;) fCoefBound2D1[i] = rhs.fCoefBound2D1[i];
	}
	if (rhs.fCoefs) {
	    fCoefs = new Float_t[fNCoefs];
	    for (int i=fNCoefs;i--;) fCoefs[i] = rhs.fCoefs[i];
	}
	if (rhs.fTmpCf1) fTmpCf1 = new Float_t[fNCols];
	if (rhs.fTmpCf0) fTmpCf0 = new Float_t[fNRows];    
    }
    return *this;
}

//__________________________________________________________________________________________
void AliCheb3DCalc::Clear(Option_t*)
{
  // delete all dynamycally allocated structures
  if (fTmpCf1)       { delete[] fTmpCf1;  fTmpCf1 = 0;}
  if (fTmpCf0)       { delete[] fTmpCf0;  fTmpCf0 = 0;}
  if (fCoefs)        { delete[] fCoefs;   fCoefs  = 0;}
  if (fCoefBound2D0) { delete[] fCoefBound2D0; fCoefBound2D0 = 0; }
  if (fCoefBound2D1) { delete[] fCoefBound2D1; fCoefBound2D1 = 0; }
  if (fNColsAtRow)   { delete[] fNColsAtRow;   fNColsAtRow = 0; }
  if (fColAtRowBg)   { delete[] fColAtRowBg;   fColAtRowBg = 0; }
  //
}

//__________________________________________________________________________________________
void AliCheb3DCalc::Init0()
{
  // reset everything to 0
  fNCoefs = fNRows = fNCols = fNElemBound2D = 0;
  fCoefs = 0;
  fCoefBound2D0 = fCoefBound2D1 = 0;
  fNColsAtRow = fColAtRowBg = 0;
  fTmpCf0 = fTmpCf1 = 0;
}

//__________________________________________________________________________________________
void AliCheb3DCalc::Print(Option_t* ) const
{
  printf("Chebyshev parameterization data %s for 3D->1 function.\n",GetName());
  int nmax3d = 0; 
  for (int i=fNElemBound2D;i--;) if (fCoefBound2D0[i]>nmax3d) nmax3d = fCoefBound2D0[i];
  printf("%d coefficients in %dx%dx%d matrix\n",fNCoefs,fNRows,fNCols,nmax3d);
  //
}

//__________________________________________________________________________________________
Float_t  AliCheb3DCalc::Eval(Float_t  *par) const
{
  // evaluate Chebyshev parameterization for 3D function.
  // VERY IMPORTANT: par must contain the function arguments ALREADY MAPPED to [-1:1] interval
  Float_t  &z = par[2];
  Float_t  &y = par[1];
  Float_t  &x = par[0];
  //
  int ncfRC;
  for (int id0=fNRows;id0--;) {
    int nCLoc = fNColsAtRow[id0];                   // number of significant coefs on this row
    int Col0  = fColAtRowBg[id0];                   // beginning of local column in the 2D boundary matrix
    for (int id1=nCLoc;id1--;) {
      int id = id1+Col0;
      fTmpCf1[id1] = (ncfRC=fCoefBound2D0[id]) ? ChebEval1D(z,fCoefs + fCoefBound2D1[id], ncfRC) : 0.0;
    }
    fTmpCf0[id0] = nCLoc>0 ? ChebEval1D(y,fTmpCf1,nCLoc):0.0;
  }
  return ChebEval1D(x,fTmpCf0,fNRows);
  //
}

//_______________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3DCalc::SaveData(const char* outfile,Bool_t append) const
{
  // writes coefficients data to output text file, optionallt appending on the end of existing file
  TString strf = outfile;
  gSystem->ExpandPathName(strf);
  FILE* stream = fopen(strf,append ? "a":"w");
  SaveData(stream);
  fclose(stream);
  //
}
#endif

//_______________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3DCalc::SaveData(FILE* stream) const
{
  // writes coefficients data to existing output stream
  // Note: fNCols, fNElemBound2D and fColAtRowBg is not stored, will be computed on fly during the loading of this file
  fprintf(stream,"#\nSTART %s\n",GetName());
  fprintf(stream,"# Number of rows\n%d\n",fNRows);
  //
  fprintf(stream,"# Number of columns per row\n");
  for (int i=0;i<fNRows;i++) fprintf(stream,"%d\n",fNColsAtRow[i]);
  //
  fprintf(stream,"# Number of Coefs in each significant block of third dimension\n");
  for (int i=0;i<fNElemBound2D;i++) fprintf(stream,"%d\n",fCoefBound2D0[i]);
  //
  fprintf(stream,"# Coefficients\n");
  for (int i=0;i<fNCoefs;i++) fprintf(stream,"%+.8e\n",fCoefs[i]);
  fprintf(stream,"END %s\n",GetName());
  //
}
#endif

//_______________________________________________
void AliCheb3DCalc::LoadData(FILE* stream)
{
  // Load coefs. from the stream 
  if (!stream) {Error("LoadData","No stream provided.\nStop"); exit(1);}
  TString buffs;
  Clear();
  ReadLine(buffs,stream);
  if (!buffs.BeginsWith("START")) {Error("LoadData","Expected: \"START <fit_name>\", found \"%s\"\nStop\n",buffs.Data());exit(1);}
  if (buffs.First(' ')>0) SetName(buffs.Data()+buffs.First(' ')+1);
  //
  ReadLine(buffs,stream); // NRows
  fNRows = buffs.Atoi(); 
  if (fNRows<1) {Error("LoadData","Expected: '<number_of_rows>', found \"%s\"\nStop\n",buffs.Data());exit(1);}
  //
  fNCols = 0;
  fNElemBound2D = 0;
  InitRows(fNRows);
  //
  for (int id0=0;id0<fNRows;id0++) {
    ReadLine(buffs,stream);               // n.cols at this row
    fNColsAtRow[id0] = buffs.Atoi();
    fColAtRowBg[id0] = fNElemBound2D;     // begining of this row in 2D boundary surface
    fNElemBound2D   += fNColsAtRow[id0];
    if (fNCols<fNColsAtRow[id0]) fNCols = fNColsAtRow[id0];
  }
  InitCols(fNCols);
  //  
  fNCoefs = 0; 
  InitElemBound2D(fNElemBound2D);
  //
  for (int i=0;i<fNElemBound2D;i++) {
    ReadLine(buffs,stream);               // n.coeffs at 3-d dimension for the given column/row
    fCoefBound2D0[i] = buffs.Atoi();
    fCoefBound2D1[i] = fNCoefs;
    fNCoefs += fCoefBound2D0[i];
  }
  //
  if (fNCoefs<=0) {Error("LoadData","Negtive (%d) number of Chebychef coeffs. is obtained.\nStop\n",fNCoefs);exit(1);}
  //
  InitCoefs(fNCoefs);
  for (int i=0;i<fNCoefs;i++) {
    ReadLine(buffs,stream);
    fCoefs[i] = buffs.Atof();
  }
  // check end_of_data record
  ReadLine(buffs,stream);
  if (!buffs.BeginsWith("END") || !buffs.Contains(GetName())) {
    Error("LoadData","Expected \"END %s\", found \"%s\".\nStop\n",GetName(),buffs.Data());
    exit(1);
  }
  //
}

//_______________________________________________
void AliCheb3DCalc::ReadLine(TString& str,FILE* stream) 
{
  // read single line from the stream, skipping empty and commented lines. EOF is not expected
  while (str.Gets(stream)) {
    str = str.Strip(TString::kBoth,' ');
    if (str.IsNull()||str.BeginsWith("#")) continue;
    return;
  }
  fprintf(stderr,"AliCheb3D::ReadLine: Failed to read from stream.\nStop");exit(1); // normally, should not reach here
}

//_______________________________________________
void AliCheb3DCalc::InitCols(int nc)
{
  // Set max.number of significant columns in the coefs matrix
  fNCols = nc;
  if (fTmpCf1) delete[] fTmpCf1;
  fTmpCf1 = new Float_t [fNCols];
}

//_______________________________________________
void AliCheb3DCalc::InitRows(int nr)
{
  // Set max.number of significant rows in the coefs matrix
  if (fNColsAtRow) delete[] fNColsAtRow;
  if (fColAtRowBg) delete[] fColAtRowBg;
  if (fTmpCf0)     delete[] fTmpCf0;
  fNRows = nr;
  fNColsAtRow = new Int_t[fNRows];
  fTmpCf0     = new Float_t [fNRows];
  fColAtRowBg = new Int_t[fNRows];
  for (int i=fNRows;i--;) fNColsAtRow[i] = fColAtRowBg[i] = 0;
}

//_______________________________________________
void AliCheb3DCalc::InitElemBound2D(int ne)
{
  // Set max number of significant coefs for given row/column of coefs 3D matrix
  if (fCoefBound2D0) delete[] fCoefBound2D0; 
  if (fCoefBound2D1) delete[] fCoefBound2D1; 
  fNElemBound2D = ne;
  fCoefBound2D0 = new Int_t[fNElemBound2D];
  fCoefBound2D1 = new Int_t[fNElemBound2D];
  for (int i=fNElemBound2D;i--;) fCoefBound2D0[i] = fCoefBound2D1[i] = 0;
}

//_______________________________________________
void AliCheb3DCalc::InitCoefs(int nc)
{
  // Set total number of significant coefs
  if (fCoefs) delete[] fCoefs; 
  fNCoefs = nc;
  fCoefs = new Float_t [fNCoefs];
  for (int i=fNCoefs;i--;) fCoefs[i] = 0.0;
}




ClassImp(AliCheb3D)

AliCheb3D::AliCheb3D():
    TNamed("", ""),
    fDimOut(0),
    fPrec(0.),
    fChebCalc(),
    fMaxCoefs(0),
    fResTmp(0),
    fGrid(0),
    fUsrFunName(),
    fUsrMacro(0)	     
{
    // Default constructor
    Init0();
}

AliCheb3D::AliCheb3D(const char* inputFile):
    TNamed("", ""),
    fDimOut(0),
    fPrec(0.),
    fChebCalc(),
    fMaxCoefs(0),
    fResTmp(0),
    fGrid(0),
    fUsrFunName(),
    fUsrMacro(0)	     
{
    // Default constructor
    Init0();
    LoadData(inputFile);
}



AliCheb3D::AliCheb3D(FILE* stream):
    TNamed("", ""),
    fDimOut(0),
    fPrec(0.),
    fChebCalc(),
    fMaxCoefs(0),
    fResTmp(0),
    fGrid(0),
    fUsrFunName(),
    fUsrMacro(0)	     
{
    // Default constructor
    Init0();
    LoadData(stream);
}

AliCheb3D::AliCheb3D(const AliCheb3D& src) : 
    TNamed(src),
    fDimOut(src.fDimOut), 
    fPrec(src.fPrec), 
    fChebCalc(1), 
    fMaxCoefs(src.fMaxCoefs), 
					   fResTmp(0),
    fGrid(0), 
    fUsrFunName(src.fUsrFunName), 
    fUsrMacro(0)
{
    // Copy constructor
    // read coefs from text file
    for (int i=3;i--;) {
	fBMin[i]    = src.fBMin[i];
	fBMax[i]    = src.fBMax[i];
	fBScale[i]  = src.fBScale[i];
	fBOffset[i] = src.fBOffset[i];
	fNPoints[i] = src.fNPoints[i];
    }
    for (int i=0;i<fDimOut;i++) {
	AliCheb3DCalc* cbc = src.GetChebCalc(i);
	if (cbc) fChebCalc.AddAtAndExpand(new AliCheb3DCalc(*cbc),i);
    }
}

AliCheb3D& AliCheb3D::operator=(const AliCheb3D& rhs)
{
    // Assignment operator
    if (this != &rhs) {
	Clear();
	fDimOut   = rhs.fDimOut;
	fPrec     = rhs.fPrec;
	fMaxCoefs = rhs.fMaxCoefs;
	fUsrFunName = rhs.fUsrFunName;
	fUsrMacro   = 0;
	for (int i=3;i--;) {
	    fBMin[i]    = rhs.fBMin[i];
	    fBMax[i]    = rhs.fBMax[i];
	    fBScale[i]  = rhs.fBScale[i];
	    fBOffset[i] = rhs.fBOffset[i];
	    fNPoints[i] = rhs.fNPoints[i];
	} 
	for (int i=0;i<fDimOut;i++) {
	    AliCheb3DCalc* cbc = rhs.GetChebCalc(i);
	    if (cbc) fChebCalc.AddAtAndExpand(new AliCheb3DCalc(*cbc),i);
	}    
    }
    return *this;
    //
}


//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
AliCheb3D::AliCheb3D(const char* funName, int DimOut, Float_t  *bmin,Float_t  *bmax, Int_t *npoints, Float_t prec) : TNamed(funName,funName)
{
  // Construct the parameterization for the function
  // funName : name of the file containing the function: void funName(Float_t * inp,Float_t * out)
  // DimOut  : dimension of the vector computed by the user function
  // bmin    : array of 3 elements with the lower boundaries of the region where the function is defined
  // bmax    : array of 3 elements with the upper boundaries of the region where the function is defined
  // npoints : array of 3 elements with the number of points to compute in each of 3 dimension
  // prec    : max allowed absolute difference between the user function and computed parameterization on the requested grid
  //
  Init0();
  fPrec = TMath::Max(1.E-12f,prec);
  if (DimOut<1) {Error("AliCheb3D","Requested output dimension is %d\nStop\n",fDimOut); exit(1);}
  SetDimOut(DimOut);
  PrepareBoundaries(bmin,bmax);
  DefineGrid(npoints);
  SetUsrFunction(funName);
  ChebFit();
  //
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
AliCheb3D::AliCheb3D(void (*ptr)(float*,float*), int DimOut, Float_t  *bmin,Float_t  *bmax, Int_t *npoints, Float_t prec) : TNamed("AliCheb3D","AliCheb3D")
{
  // Construct the parameterization for the function
  // ptr     : pointer on the function: void fun(Float_t * inp,Float_t * out)
  // DimOut  : dimension of the vector computed by the user function
  // bmin    : array of 3 elements with the lower boundaries of the region where the function is defined
  // bmax    : array of 3 elements with the upper boundaries of the region where the function is defined
  // npoints : array of 3 elements with the number of points to compute in each of 3 dimension
  // prec    : max allowed absolute difference between the user function and computed parameterization on the requested grid
  //
  Init0();
  fPrec = TMath::Max(1.E-12f,prec);
  if (DimOut<1) {Error("AliCheb3D","Requested output dimension is %d\nStop\n",fDimOut); exit(1);}
  SetDimOut(DimOut);
  PrepareBoundaries(bmin,bmax);
  DefineGrid(npoints);
  SetUsrFunction(ptr);
  ChebFit();
  //
}
#endif


//__________________________________________________________________________________________
void AliCheb3D::Clear(Option_t*)
{
  if (fResTmp)        { delete[] fResTmp; fResTmp = 0; }
  if (fGrid)          { delete[] fGrid;   fGrid   = 0; }
  if (fUsrMacro)      { delete fUsrMacro; fUsrMacro = 0;}
  fChebCalc.Delete();
  //
}

//__________________________________________________________________________________________
void AliCheb3D::Print(Option_t* opt) const
{
  printf("%s: Chebyshev parameterization for 3D->%dD function. Precision: %e\n",GetName(),fDimOut,fPrec);
  printf("Region of validity: [%+.5e:%+.5e] [%+.5e:%+.5e] [%+.5e:%+.5e]\n",fBMin[0],fBMax[0],fBMin[1],fBMax[1],fBMin[2],fBMax[2]);
  TString opts = opt; opts.ToLower();
  if (opts.Contains("l")) for (int i=0;i<fDimOut;i++) {printf("Output dimension %d:\n",i+1); GetChebCalc(i)->Print();}
  //
}

//__________________________________________________________________________________________
void AliCheb3D::Init0()
{
  for (int i=3;i--;) fBMin[i] = fBMax[i] = fBScale[i] = fBOffset[i] = 0;
  fMaxCoefs = 0;
  fGrid = 0;
  fResTmp = 0;
  fUsrFunName = "";
  fUsrMacro = 0;
#ifdef _INC_CREATION_ALICHEB3D_
  gUsrFunAliCheb3D = 0;
#endif
}

//__________________________________________________________________________________________
void AliCheb3D::PrepareBoundaries(Float_t  *bmin,Float_t  *bmax)
{
  // Set and check boundaries defined by user, prepare coefficients for their conversion to [-1:1] interval
  //
  for (int i=3;i--;) {
    fBMin[i]   = bmin[i];
    fBMax[i]   = bmax[i];
    fBScale[i] = bmax[i]-bmin[i];
    if (fBScale[i]<=0) { 
      Error("PrepareBoundaries","Boundaries for %d-th dimension are not increasing: %+.4e %+.4e\nStop\n",i,fBMin[i],fBMax[i]);
      exit(1);
    }
    fBOffset[i] = bmin[i] + fBScale[i]/2.0;
    fBScale[i] = 2./fBScale[i];
  }
  //
}

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::SetUsrFunction(const char* name)
{
  // load user macro with function definition and compile it
  gUsrFunAliCheb3D = 0; 
  fUsrFunName = name;
  gSystem->ExpandPathName(fUsrFunName);
  if (fUsrMacro) delete fUsrMacro;
  TString tmpst = fUsrFunName;
  tmpst += "+"; // prepare filename to compile
  if (gROOT->LoadMacro(tmpst.Data())) {Error("SetUsrFunction","Failed to load user function from %s\nStop\n",name); exit(1);}
  fUsrMacro = new TMethodCall();        
  tmpst = tmpst.Data() + tmpst.Last('/')+1; //Strip away any path preceding the macro file name
  int dot = tmpst.Last('.');
  if (dot>0) tmpst.Resize(dot);
  fUsrMacro->InitWithPrototype(tmpst.Data(),"Float_t *,Float_t *");
  long args[2];
  args[0] = (long)fArgsTmp;
  args[1] = (long)fResTmp;
  fUsrMacro->SetParamPtrs(args); 
  //
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::SetUsrFunction(void (*ptr)(float*,float*))
{
  if (fUsrMacro) delete fUsrMacro;
  fUsrMacro = 0;
  fUsrFunName = "";
  gUsrFunAliCheb3D = ptr;
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::EvalUsrFunction(Float_t  *x, Float_t  *res) {
  for (int i=3;i--;) fArgsTmp[i] = x[i];
  if   (gUsrFunAliCheb3D) gUsrFunAliCheb3D(fArgsTmp,fResTmp);
  else fUsrMacro->Execute(); 
  for (int i=fDimOut;i--;) res[i] = fResTmp[i];
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
Int_t AliCheb3D::CalcChebCoefs(Float_t  *funval,int np, Float_t  *outCoefs, Float_t  prec)
{
  // Calculate Chebyshev coeffs using precomputed function values at np roots.
  // If prec>0, estimate the highest coeff number providing the needed precision
  //
  double sm;                 // do summations in double to minimize the roundoff error
  for (int ic=0;ic<np;ic++) { // compute coeffs
    sm = 0;          
    for (int ir=0;ir<np;ir++) {
      float  rt = TMath::Cos( ic*(ir+0.5)*TMath::Pi()/np);
      sm += funval[ir]*rt;
    }
    outCoefs[ic] = Float_t( sm * ((ic==0) ? 1./np : 2./np) );
  }
  //
  if (prec<=0) return np;
  //
  sm = 0;
  int cfMax = 0;
  for (cfMax=np;cfMax--;) {
    sm += TMath::Abs(outCoefs[cfMax]);
    if (sm>=prec) break;
  }
  if (++cfMax==0) cfMax=1;
  return cfMax;
  //
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::DefineGrid(Int_t* npoints)
{
  // prepare the grid of Chebyshev roots in each dimension
  const int kMinPoints = 1;
  int ntot = 0;
  fMaxCoefs = 1;
  for (int id=3;id--;) { 
    fNPoints[id] = npoints[id];
    if (fNPoints[id]<kMinPoints) {
      Error("DefineGrid","at %d-th dimension %d point is requested, at least %d is needed\nStop\n",fNPoints[id],kMinPoints);
      exit(1);
    }
    ntot += fNPoints[id];
    fMaxCoefs *= fNPoints[id];
  }
  fGrid = new Float_t [ntot];
  //
  int curp = 0;
  for (int id=3;id--;) { 
    int np = fNPoints[id];
    fGridOffs[id] = curp;
    for (int ip=0;ip<np;ip++) {
      Float_t x = TMath::Cos( TMath::Pi()*(ip+0.5)/np );
      fGrid[curp++] = MapToExternal(x,id);
    }
  }
  //
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
Int_t AliCheb3D::ChebFit()
{
  // prepare parameterization for all output dimensions
  int ir=0; 
  for (int i=fDimOut;i--;) ir+=ChebFit(i); 
  return ir;
}
#endif

//__________________________________________________________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
Int_t AliCheb3D::ChebFit(int dmOut)
{
  // prepare paramaterization of 3D function for dmOut-th dimension 
  int maxDim = 0;
  for (int i=0;i<3;i++) if (maxDim<fNPoints[i]) maxDim = fNPoints[i];
  Float_t  *fvals      = new Float_t [ fNPoints[0] ];
  Float_t  *tmpCoef3D  = new Float_t [ fNPoints[0]*fNPoints[1]*fNPoints[2] ]; 
  Float_t  *tmpCoef2D  = new Float_t [ fNPoints[0]*fNPoints[1] ]; 
  Float_t  *tmpCoef1D  = new Float_t [ maxDim ];
  //
  Float_t RTiny = fPrec/Float_t(maxDim); // neglect coefficient below this threshold
  //
  // 1D Cheb.fit for 0-th dimension at current steps of remaining dimensions
  int ncmax = 0;
  //
  AliCheb3DCalc* cheb =  GetChebCalc(dmOut);
  //
  for (int id2=fNPoints[2];id2--;) {
    fArgsTmp[2] = fGrid[ fGridOffs[2]+id2 ];
    //
    for (int id1=fNPoints[1];id1--;) {
      fArgsTmp[1] = fGrid[ fGridOffs[1]+id1 ];
      //
      for (int id0=fNPoints[0];id0--;) {
	fArgsTmp[0] = fGrid[ fGridOffs[0]+id0 ];
	EvalUsrFunction();     // compute function values at Chebyshev roots of 0-th dimension
	fvals[id0] =  fResTmp[dmOut];
      }
      int nc = CalcChebCoefs(fvals,fNPoints[0], tmpCoef1D, fPrec);
      for (int id0=fNPoints[0];id0--;) tmpCoef2D[id1 + id0*fNPoints[1]] = tmpCoef1D[id0];
      if (ncmax<nc) ncmax = nc;              // max coefs to be kept in dim0 to guarantee needed precision
    }
    //
    // once each 1d slice of given 2d slice is parametrized, parametrize the Cheb.coeffs
    for (int id0=fNPoints[0];id0--;) {
      CalcChebCoefs( tmpCoef2D+id0*fNPoints[1], fNPoints[1], tmpCoef1D, -1);
      for (int id1=fNPoints[1];id1--;) tmpCoef3D[id2 + fNPoints[2]*(id1+id0*fNPoints[1])] = tmpCoef1D[id1];
    }
  }
  //
  // now fit the last dimensions Cheb.coefs
  for (int id0=fNPoints[0];id0--;) {
    for (int id1=fNPoints[1];id1--;) {
      CalcChebCoefs( tmpCoef3D+ fNPoints[2]*(id1+id0*fNPoints[1]), fNPoints[2], tmpCoef1D, -1);
      for (int id2=fNPoints[2];id2--;) tmpCoef3D[id2+ fNPoints[2]*(id1+id0*fNPoints[1])] = tmpCoef1D[id2]; // store on place
    }
  }
  //
  // now find 2D surface which separates significant coefficients of 3D matrix from nonsignificant ones (up to fPrec)
  int *tmpCoefSurf = new Int_t[ fNPoints[0]*fNPoints[1] ];
  for (int id0=fNPoints[0];id0--;) for (int id1=fNPoints[1];id1--;) tmpCoefSurf[id1+id0*fNPoints[1]]=0;  
  Double_t resid = 0;
  for (int id0=fNPoints[0];id0--;) {
    for (int id1=fNPoints[1];id1--;) {
      for (int id2=fNPoints[2];id2--;) {
	int id = id2 + fNPoints[2]*(id1+id0*fNPoints[1]);
	Float_t  cfa = TMath::Abs(tmpCoef3D[id]);
	if (cfa < RTiny) {tmpCoef3D[id] = 0; continue;} // neglect coeefs below the threshold

	resid += cfa;
	if (resid<fPrec) continue; // this coeff is negligible
	// otherwise go back 1 step
	resid -= cfa;
	tmpCoefSurf[id1+id0*fNPoints[1]] = id2+1; // how many coefs to keep
	break;
      }
    }
  }
  /*
  printf("\n\nCoeffs\n");  
  int cnt = 0;
  for (int id0=0;id0<fNPoints[0];id0++) {
    for (int id1=0;id1<fNPoints[1];id1++) {
      for (int id2=0;id2<fNPoints[2];id2++) {
	printf("%2d%2d%2d %+.4e |",id0,id1,id2,tmpCoef3D[cnt++]);
      }
      printf("\n");
    }
    printf("\n");
  }
  */
  // see if there are rows to reject, find max.significant column at each row
  int NRows = fNPoints[0];
  int *tmpCols = new int[NRows]; 
  for (int id0=fNPoints[0];id0--;) {
    int id1 = fNPoints[1];
    while (id1>0 && tmpCoefSurf[(id1-1)+id0*fNPoints[1]]==0) id1--;
    tmpCols[id0] = id1;
  }
  // find max significant row
  for (int id0=NRows;id0--;) {if (tmpCols[id0]>0) break; NRows--;}
  // find max significant column and fill the permanent storage for the max sigificant column of each row
  cheb->InitRows(NRows);                  // create needed arrays;
  int *NColsAtRow = cheb->GetNColsAtRow();
  int *ColAtRowBg = cheb->GetColAtRowBg();
  int NCols = 0;
  int NElemBound2D = 0;
  for (int id0=0;id0<NRows;id0++) {
    NColsAtRow[id0] = tmpCols[id0];     // number of columns to store for this row
    ColAtRowBg[id0] = NElemBound2D;     // begining of this row in 2D boundary surface
    NElemBound2D += tmpCols[id0];
    if (NCols<NColsAtRow[id0]) NCols = NColsAtRow[id0];
  }
  cheb->InitCols(NCols);
  delete[] tmpCols;
  //  
  // create the 2D matrix defining the boundary of significance for 3D coeffs.matrix 
  // and count the number of siginifacnt coefficients
  //
  cheb->InitElemBound2D(NElemBound2D);
  int *CoefBound2D0 = cheb->GetCoefBound2D0();
  int *CoefBound2D1 = cheb->GetCoefBound2D1();
  fMaxCoefs = 0; // redefine number of coeffs
  for (int id0=0;id0<NRows;id0++) {
    int nCLoc = NColsAtRow[id0];
    int Col0  = ColAtRowBg[id0];
    for (int id1=0;id1<nCLoc;id1++) {
      CoefBound2D0[Col0 + id1] = tmpCoefSurf[id1+id0*fNPoints[1]];  // number of coefs to store for 3-d dimension
      CoefBound2D1[Col0 + id1] = fMaxCoefs;
      fMaxCoefs += CoefBound2D0[Col0 + id1];
    }
  }
  //
  // create final compressed 3D matrix for significant coeffs
  cheb->InitCoefs(fMaxCoefs);
  Float_t  *Coefs = cheb->GetCoefs();
  int count = 0;
  for (int id0=0;id0<NRows;id0++) {
    int ncLoc = NColsAtRow[id0];
    int Col0  = ColAtRowBg[id0];
    for (int id1=0;id1<ncLoc;id1++) {
      int ncf2 = CoefBound2D0[Col0 + id1];
      for (int id2=0;id2<ncf2;id2++) {
	Coefs[count++] = tmpCoef3D[id2 + fNPoints[2]*(id1+id0*fNPoints[1])];
      }
    }
  }
  /*
  printf("\n\nNewSurf\n");
  for (int id0=0;id0<fNPoints[0];id0++) {
    for (int id1=0;id1<fNPoints[1];id1++) {
      printf("(%2d %2d) %2d |",id0,id1,tmpCoefSurf[id1+id0*fNPoints[1]]);  
    }
    printf("\n");
  }
  */
  //
  delete[] tmpCoefSurf;
  delete[] tmpCoef1D;
  delete[] tmpCoef2D;
  delete[] tmpCoef3D;
  delete[] fvals;
  //
  return 1;
}
#endif

//_______________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::SaveData(const char* outfile,Bool_t append) const
{
  // writes coefficients data to output text file, optionallt appending on the end of existing file
  TString strf = outfile;
  gSystem->ExpandPathName(strf);
  FILE* stream = fopen(strf,append ? "a":"w");
  SaveData(stream);
  fclose(stream);
  //
}
#endif

//_______________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
void AliCheb3D::SaveData(FILE* stream) const
{
  // writes coefficients data to existing output stream
  //
  fprintf(stream,"\n# These are automatically generated data for the Chebyshev interpolation of 3D->%dD function\n",fDimOut); 
  fprintf(stream,"#\nSTART %s\n",GetName());
  fprintf(stream,"# Dimensionality of the output\n%d\n",fDimOut);
  fprintf(stream,"# Interpolation abs. precision\n%+.8e\n",fPrec);
  //
  fprintf(stream,"# Lower boundaries of interpolation region\n");
  for (int i=0;i<3;i++) fprintf(stream,"%+.8e\n",fBMin[i]);
  fprintf(stream,"# Upper boundaries of interpolation region\n");
  for (int i=0;i<3;i++) fprintf(stream,"%+.8e\n",fBMax[i]);
  fprintf(stream,"# Parameterization for each output dimension follows:\n",GetName());
  //
  for (int i=0;i<fDimOut;i++) GetChebCalc(i)->SaveData(stream);
  fprintf(stream,"#\nEND %s\n#\n",GetName());
  //
}
#endif

//_______________________________________________
void AliCheb3D::LoadData(const char* inpFile)
{
  TString strf = inpFile;
  gSystem->ExpandPathName(strf);
  FILE* stream = fopen(strf.Data(),"r");
  LoadData(stream);
  fclose(stream);
  //
}

//_______________________________________________
void AliCheb3D::LoadData(FILE* stream)
{
  if (!stream) {Error("LoadData","No stream provided.\nStop"); exit(1);}
  TString buffs;
  Clear();
  AliCheb3DCalc::ReadLine(buffs,stream);
  if (!buffs.BeginsWith("START")) {Error("LoadData","Expected: \"START <fit_name>\", found \"%s\"\nStop\n",buffs.Data());exit(1);}
  SetName(buffs.Data()+buffs.First(' ')+1);
  //
  AliCheb3DCalc::ReadLine(buffs,stream); // N output dimensions
  fDimOut = buffs.Atoi(); 
  if (fDimOut<1) {Error("LoadData","Expected: '<number_of_output_dimensions>', found \"%s\"\nStop\n",buffs.Data());exit(1);}
  //
  SetDimOut(fDimOut);
  //
  AliCheb3DCalc::ReadLine(buffs,stream); // Interpolation abs. precision
  fPrec = buffs.Atof();
  if (fPrec<=0) {Error("LoadData","Expected: '<abs.precision>', found \"%s\"\nStop\n",buffs.Data());exit(1);}
  //
  for (int i=0;i<3;i++) { // Lower boundaries of interpolation region
    AliCheb3DCalc::ReadLine(buffs,stream);
    fBMin[i] = buffs.Atof(); 
  }
  for (int i=0;i<3;i++) { // Upper boundaries of interpolation region
    AliCheb3DCalc::ReadLine(buffs,stream);
    fBMax[i] = buffs.Atof(); 
  }
  PrepareBoundaries(fBMin,fBMax);
  //
  // data for each output dimension
  for (int i=0;i<fDimOut;i++) GetChebCalc(i)->LoadData(stream);
  //
  // check end_of_data record
  AliCheb3DCalc::ReadLine(buffs,stream);
  if (!buffs.BeginsWith("END") || !buffs.Contains(GetName())) {
    Error("LoadData","Expected \"END %s\", found \"%s\".\nStop\n",GetName(),buffs.Data());
    exit(1);
  }
  //
}

//_______________________________________________
void AliCheb3D::SetDimOut(int d)
{
  fDimOut = d;
  if (fResTmp) delete fResTmp;
  fResTmp = new Float_t[fDimOut]; // RRR
  fChebCalc.Delete();
  for (int i=0;i<d;i++) fChebCalc.AddAtAndExpand(new AliCheb3DCalc(),i);
}

//_______________________________________________
void AliCheb3D::ShiftBound(int id,float dif)
{
  if (id<0||id>2) {printf("Maximum 3 dimensions are supported\n"); return;}
  fBMin[id] += dif;
  fBMax[id] += dif;
  fBOffset[id] += dif;
}

//_______________________________________________
#ifdef _INC_CREATION_ALICHEB3D_
TH1* AliCheb3D::TestRMS(int idim,int npoints,TH1* histo)
{
  // fills the difference between the original function and parameterization (for idim-th component of the output)
  // to supplied histogram. Calculations are done in npoints random points. 
  // If the hostgram was not supplied, it will be created. It is up to the user to delete it! 
  if (!fUsrMacro) {
    printf("No user function is set\n");
    return 0;
  }
  if (!histo) histo = new TH1D(GetName(),"Control: Function - Parametrization",100,-2*fPrec,2*fPrec);
  for (int ip=npoints;ip--;) {
    gRandom->RndmArray(3,(Float_t *)fArgsTmp);
    for (int i=3;i--;) fArgsTmp[i] = fBMin[i] + fArgsTmp[i]*(fBMax[i]-fBMin[i]);
    EvalUsrFunction();
    Float_t valFun = fResTmp[idim];
    Eval(fArgsTmp,fResTmp);
    Float_t valPar = fResTmp[idim];
    histo->Fill(valFun - valPar);
  }
  return histo;
  //
}
#endif
