//$Id$

// Author: Anders Vestbo <mailto:vestbo$fi.uib.no>
//*-- Copyright &copy ASV

#include <stream.h>
#include <string.h>
#include <stdlib.h>
#include "AliL3RootTypes.h"
#include "AliLevel3.h"

//Standalone program to run the track follower for benchmark tests.

#if GCCVERSION == 3
using namespace std;
#endif

int main(int argc,char **argv)
{
  
  if(argc != 4)
    {
      cout<<"Usage : runtracker <path> minslice maxslice "<<endl;
      return -1;
    }
  Char_t path[1024];
  Int_t sl1,sl2;
  
  strcpy(path,argv[1]);
  sl1 = atoi(argv[2]);
  sl2 = atoi(argv[3]);
  
  AliLevel3 level3;
  level3.Init(path,kTRUE);
  level3.SetClusterFinderParam(0.2,0.3,kFALSE);
  
  Int_t phi_segments,eta_segments,trackletlength,tracklength;
  Int_t rowscopetracklet,rowscopetrack;
  Double_t min_pt_fit,maxangle,goodDist,hitChi2Cut;
  Double_t goodHitChi2,trackChi2Cut,maxphi,maxeta;
  
  phi_segments = 50;//50;
  eta_segments = 100;//100;
  trackletlength = 3;
  tracklength = 5;
  rowscopetracklet = 2;
  rowscopetrack = 2;
  min_pt_fit = 0;
  maxangle = 1.31;
  goodDist = 5;
  maxphi=100;
  maxeta=100;
  hitChi2Cut = 20;//100
  goodHitChi2 = 10;//20;
  trackChi2Cut = 50;
  
  //main vertex tracking parameters:
  level3.SetTrackerParam(phi_segments,eta_segments,trackletlength,tracklength,
		     rowscopetracklet,rowscopetrack,
		     min_pt_fit,maxangle,goodDist,hitChi2Cut,
		     goodHitChi2,trackChi2Cut,50,maxphi,maxeta,kTRUE);
  
  //level3.WriteFiles("data/");
  level3.ProcessEvent(sl1,sl2);
  level3.DoBench("benchmark_0");

  return 0;
}
