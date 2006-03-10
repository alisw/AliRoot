/*******************************************************************************
 * Copyright(c) 2003, IceCube Experiment at the South Pole. All rights reserved.
 *
 * Author: The IceCube RALICE-based Offline Project.
 * Contributors are mentioned in the code where appropriate.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation strictly for non-commercial purposes is hereby granted
 * without fee, provided that the above copyright notice appears in all
 * copies and that both the copyright notice and this permission notice
 * appear in the supporting documentation.
 * The authors make no claims about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *******************************************************************************/

// $Id$

///////////////////////////////////////////////////////////////////////////
// Class IceDwalk
// TTask derived class to perform direct walk track reconstruction.
// The procedure is based on the method described in the Amanda publication
// in Nuclear Instruments and Methods A524 (2004) 179-180.
// However, the Amanda method has been extended with the intention to
// take also multiple (muon) tracks within 1 event into account.
// This will not only provide a means to reconstruct muon bundles and
// multiple track events in IceCube, but will also allow to reduce the
// background of faked upgoing muons as a result of multiple downgoing
// muons hitting the top and bottom parts of the detector.
// To prevent waisting CPU time in trying to reconstruct (high-energy) cascade
// events, or to select specifically reconstruction of low multiplicity events,
// the user may invoke the memberfunctions SetMaxModA() and SetMinModA().
// This allows selection of events for processing with a certain maximum and/or
// minimum number of good Amanda OMs firing.
// By default the minimum and maximum are set to 0 and 999, respectively,
// in the constructor, which implies no multiplicity selection. 
// The various reconstruction steps are summarised as follows :
//
// 1) Construction of track elements (TE's).
//    A track element is a straight line connecting two hits that
//    appeared at some minimum distance d and within some maximum
//    time difference dt.
//    The default values for d and dt are given in eq. (20) of the
//    NIM article, but can be modified by the appropriate Set functions.
//    For dt a default margin of 30 ns is used (according to eq. (20)),
//    but also this margin may be modified via the appropriate Set function.    
//    The reference point r0 of the TE is taken as the center between
//    the two hit positions and the TE timestamp t0 at the position r0
//    is taken as the IceEvent timestamp increased by the average of the
//    two hit times. So, all timestamps contain the overall IceEvent
//    timestamp as a basis. This means that time differences can be
//    obtained via the AliTimestamp facilities (supporting upto picosecond
//    precision when available).
//    The TE direction is given by the relative position of the two hits.
//
// 2) Each TE will obtain so called associated hits.
//    A hit is associated to a TE when it fulfills both the conditions
//
//      -30 < tres < 300 ns
//      dhit < 25*(tres+30)^(1/4) meter
//
//    tres : time residual
//           Difference between the observed hit time and the time expected
//           for a direct photon hit.     
//    dhit : Distance between the hit and the TE
//
// 3) Construction of track candidates (TC's).
//    These are TE's that fulfill the condition (see eq. (21) in the NIM article)
//
//     qtc >= 0.7*qtcmax
//
//     qtc=min(nah,0.3*sigmal+7)
//     qtcmax=max(qtc)
//
//    where we have used the observables :
//
//    nah    : Number of associated hits.
//    sigmal : rms variance of the distances between r0 and the point on
//             the track which is closest to the various associated hits. 
//
//    Note : The following additional quality selection as indicated
//           in the NIM article is not used anymore. 
//
//      nah >= 10
//      sigmal >= 20 meter
// 
//
// 4) The remaining track candidates are clustered into jets when their directions
//    are within a certain maximum opening angle.
//    In addition the distance between their r0's must be below a certain maximum
//    or the relative r0 direction must fall within a certain maximum opening angle
//    w.r.t. the jet-starting track candidate.
//    The latter criterion prevents clustering of (nearly) parallel track candidates
//    crossing the detector a very different locations (e.g. muon bundles).
//    The default maximum track opening angle is 15 degrees, but can be modified
//    via the SetTangmax memberfunction.
//    The remaining parameters related to the r0 criteria can be modified via
//    the SetRtdmax and SetRtangmax memberfunctions.
//    See the corresponding docs for defaults etc...
//
//    The average of all the r0 and t0 values of the constituent TC's
//    of the jet will provide the r0 and t0 (i.e. reference point) of the jet.
//
// 5) The jets are merged when their directions are within a certain maximum
//    opening angle. 
//    In addition the distance between their r0's must be below a certain maximum
//    or the relative r0 direction must fall within a certain maximum opening angle
//    w.r.t. the starting jet.
//    The latter criterion prevents merging of (nearly) parallel tracks/jets
//    crossing the detector a very different locations (e.g. muon bundles).
//    The default maximum opening angle is half the TC maximum opening angle,
//    but can be modified via the SetJangmax memberfunction.
//    The remaining parameters related to the r0 criteria can be modified via
//    the SetRjdmax and SetRjangmax memberfunctions.
//    See the corresponding docs for defaults etc...
//
//    Note : Setting the maximum jet opening angle to <=0 will prevent
//           the merging of jets.
//
//    The average of all the r0 and t0 values of the merged jets will provide
//    the r0 and t0 (i.e. reference point) of the final jet.
//
// 6) The remaining jets are ordered w.r.t. decreasing number of tracks.
//    Each remaining jet will provide the parameters (e.g. direction)
//    for a reconstructed track.
//    The track 3-momentum is set to the total jet 3-momentum, normalised
//    to 1 GeV. The mass and charge of the track are left 0.
//    The reference point data of the jet will provide the r0 and t0
//    (i.e. reference point) of the track.
//
//    All these tracks will be stored in the IceEvent structure with as
//    default "IceDwalk" as the name of the track.
//    This track name identifier can be modified by the user via the
//    SetTrackName() memberfunction. This will allow unique identification
//    of tracks which are produced when re-processing existing data with
//    different criteria.
//
//    Note : In case the maximum jet opening angle was specified <0,
//           only the jet with the maximum number of tracks will appear
//           as a reconstructed track in the IceEvent structure.
//           This will allow comparison with the standard Sieglinde
//           single track direct walk reconstruction results. 
//    
// For further details the user is referred to NIM A524 (2004) 169.
//
// Note : This algorithm works best on data which has been calibrated
//        (IceCalibrate), cross talk corrected (IceXtalk) and cleaned
//        from noise hits etc. (IceCleanHits).
//
//--- Author: Nick van Eijndhoven 07-oct-2005 Utrecht University
//- Modified: NvE $Date$ Utrecht University
///////////////////////////////////////////////////////////////////////////
 
#include "IceDwalk.h"
#include "Riostream.h"

ClassImp(IceDwalk) // Class implementation to enable ROOT I/O

IceDwalk::IceDwalk(const char* name,const char* title) : TTask(name,title)
{
// Default constructor.
// The various reconstruction parameters are initialised to the values
// as mentioned in NIM A524 (2004) 179-180.
// The newly introduced angular separation parameter for jet merging
// is initialised as half the value of the angular separation parameter
// for track candidate clustering.    
 fDmin=50;
 fDtmarg=30;
 fTangmax=15;
 fRtangmax=fTangmax;
 fRtdmax=0;
 fJangmax=fTangmax/2.;
 fRjangmax=fRtangmax;
 fRjdmax=fDmin;
 fMaxmodA=999;
 fMinmodA=0;
 fTrackname="IceDwalk";
}
///////////////////////////////////////////////////////////////////////////
IceDwalk::~IceDwalk()
{
// Default destructor.
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetDmin(Float_t d)
{
// Set minimum hit distance (in m) to form a track element.
// In the constructor the default has been set to 50 meter, in accordance
// to eq.(20) of NIM A524 (2004) 179.
 fDmin=d;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetDtmarg(Int_t dt)
{
// Set maximum hit time difference margin (in ns) for track elements.
// In the constructor the default has been set to 30 ns, in accordance
// to eq.(20) of NIM A524 (2004) 179.
 fDtmarg=dt;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetTangmax(Float_t ang)
{
// Set maximum angular separation (in deg) for track candidate clustering
// into jets.
// In the constructor the default has been set to 15 deg, in accordance
// to NIM A524 (2004) 180.
//
// Note : This function also sets automatically the value of the maximum
//        angular separation for jet merging into 1 single track to ang/2.
//        In order to specify a different max. jet merging separation angle,
//        one has to invoke the memberfunction SetJangmax afterwards.
 
 fTangmax=ang;
 fJangmax=ang/2.;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetRtangmax(Float_t ang)
{
// Set maximum angular separation (in deg) for the relative direction of the
// two r0's of two track candidates (w.r.t. the direction of the starting
// track candidate) in the track clustering process.
// In the constructor the default has been set to 15 deg, corresponding
// to the default max. angular separation for track candidate clustering.
//
// Note : This function also sets automatically the value of the maximum
//        angular separation for the relative direction of the two r0's
//        of two jets (w.r.t. the direction of the starting jet)
//        in the jet merging process.
//        In order to specify a different value related to jet merging,
//        one has to invoke the memberfunction SetRjangmax afterwards.
 
 fRtangmax=ang;
 fRjangmax=ang;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetRtdmax(Float_t d)
{
// Set maximum distance (in m) of the two r0's of two track candidates
// in the track clustering process.
// This will allow clustering of tracks with very close r0's, of which
// their relative direction may point in any direction.
// In the constructor the default has been set 0 until further tuning
// of this parameter has been achieved.
//
// Note : In case the distance between the two r0's exceeds the maximum,
//        the track candidates will still be clustered if the relative
//        direction of the two r0's falls within the maximum separation
//        angle w.r.t. the starting track direction.
 
 fRtdmax=d;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetJangmax(Float_t ang)
{
// Set angular separation (in deg) within which jets are merged into 1
// single track.
// In the constructor the default has been set 7.5 deg, being half of the
// value of the default track candidate clustering separation angle. 
//
// Notes :
// -------
// 1)  Setting ang=0 will prevent jet merging.
//     Consequently, every jet will appear as a separate track in the
//     reconstruction result.  
// 2)  Setting ang<0 will prevent jet merging.
//     In addition, only the jet with the maximum number of tracks will
//     appear as a track in the reconstruction result.
//     This situation resembles the standard Sieglinde direct walk processing
//     and as such can be used to perform comparison studies.

 fJangmax=ang;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetRjangmax(Float_t ang)
{
// Set maximum angular separation (in deg) for the relative direction of the
// two r0's of two jets (w.r.t. the direction of the starting jet)
// in the jet merging process.
// In the constructor the default has been set to the corresponding value
// of the same parameter related to the track clustering.
 
 fRjangmax=ang;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetRjdmax(Float_t d)
{
// Set maximum distance (in m) of the two r0's of two jets in the
// jet merging process.
// This will allow merging of jets with rather close r0's, of which
// their relative direction may point in any direction.
// In the constructor the default has been set to 50 meter, corresponding
// to the value of the minimum hit distance to form a track element.
//
// Note : In case the distance between the two r0's exceeds the maximum,
//        the jets will still be merged if the relative direction of the
//        two r0's falls within the maximum separation angle w.r.t. the
//        starting jet direction.
 
 fRjdmax=d;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetMaxModA(Int_t nmax)
{
// Set the maximum number of good Amanda modules that may have fired
// in order to process this event.
// This allows suppression of processing (high-energy) cascade events
// with this direct walk tracking to prevent waisting cpu time for cases
// in which tracking doesn't make sense anyhow.
// Furthermore it allows selection of low multiplicity events for processing.
// By default the maximum number of Amanda modules is set to 999 in the ctor,
// which implies no selection on maximum module multiplicity.
// See also the memberfunction SetMinModA().
 fMaxmodA=nmax;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetMinModA(Int_t nmin)
{
// Set the minimum number of good Amanda modules that must have fired
// in order to process this event.
// This allows selection of a minimal multiplicity for events to be processed.
// By default the minimum number of Amanda modules is set to 0 in the ctor,
// which implies no selection on minimum module multiplicity.
// See also the memberfunction SetMaxModA().
 fMinmodA=nmin;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::SetTrackName(TString s)
{
// Set (alternative) name identifier for the produced first guess tracks.
// This allows unique identification of (newly) produced direct walk tracks
// in case of re-processing of existing data with different criteria.
// By default the produced first guess tracks have the name "IceDwalk"
// which is set in the constructor of this class.
 fTrackname=s;
}
///////////////////////////////////////////////////////////////////////////
void IceDwalk::Exec(Option_t* opt)
{
// Implementation of the direct walk track reconstruction.

 TString name=opt;
 AliJob* parent=(AliJob*)(gROOT->GetListOfTasks()->FindObject(name.Data()));

 if (!parent) return;

 IceEvent* evt=(IceEvent*)parent->GetObject("IceEvent");
 if (!evt) return;

 // Fetch all fired Amanda OMs for this event
 TObjArray* aoms=evt->GetDevices("IceAOM");
 Int_t naoms=aoms->GetEntries();
 if (!naoms) return;

 // Check for the minimum and/or maximum number of good fired Amanda OMs
 Int_t ngood=0;
 for (Int_t iom=0; iom<naoms; iom++)
 {
  IceGOM* omx=(IceGOM*)aoms->At(iom);
  if (!omx) continue;
  if (omx->GetDeadValue("ADC") || omx->GetDeadValue("LE") || omx->GetDeadValue("TOT")) continue;
  ngood++;
 } 
 if (ngood<fMinmodA || ngood>fMaxmodA) return;

 const Float_t c=0.3;                // Light speed in vacuum in meters per ns
 const Float_t nice=1.33;            // Refractive index of ice
 const Float_t thetac=acos(1./nice); // Cherenkov angle (in radians)

 // Storage of track elements with various time difference margins.
 // temap(i,j) holds the i-th track element (TE) with a time difference margin
 // of less than j*3 nanoseconds. Currently we use a maximum margin of 30 ns.
 TObjArray tes;
 tes.SetOwner();
 AliObjMatrix temap;

 // Counter of TEs for each 3 ns margin slot
 TArrayI ntes(fDtmarg/3);
 if (ntes.GetSize()==0) ntes.Set(1);

 AliPosition r1;
 AliPosition r2;
 Ali3Vector r12;
 Ali3Vector rsum;
 AliPosition r0;
 TObjArray hits1;
 TObjArray hits2;
 Int_t nh1,nh2;
 AliSignal* sx1=0;
 AliSignal* sx2=0;
 Float_t dist=0;
 Float_t t1,t2,dt,t0;
 Float_t dtmax,dttest;
 TObjArray hits;

 // Check the hits of Amanda OM pairs for posible track elements.
 // Also all the good hits are stored in the meantime (to save CPU time)
 // for hit association with the various track elements lateron.
 AliTrack* te=0;
 Int_t ite=0;
 for (Int_t i1=0; i1<naoms; i1++) // First OM of the pair
 {
  IceGOM* omx1=(IceGOM*)aoms->At(i1);
  if (!omx1) continue;
  if (omx1->GetDeadValue("LE")) continue;
  r1=omx1->GetPosition();
  // Select all the good hits of this first OM
  hits1.Clear();
  for (Int_t j1=1; j1<=omx1->GetNhits(); j1++)
  {
   sx1=omx1->GetHit(j1);
   if (!sx1) continue;
   if (sx1->GetDeadValue("ADC") || sx1->GetDeadValue("LE") || sx1->GetDeadValue("TOT")) continue;
   hits1.Add(sx1);
   // Also store all good hits in the total hit array
   hits.Add(sx1);
  }

  // No further pair to be formed with the last OM in the list 
  if (i1==(naoms-1)) break;

  nh1=hits1.GetEntries();
  if (!nh1) continue;

  for (Int_t i2=i1+1; i2<naoms; i2++) // Second OM of the pair
  {
   IceGOM* omx2=(IceGOM*)aoms->At(i2);
   if (!omx2) continue;
   if (omx2->GetDeadValue("LE")) continue;
   r2=omx2->GetPosition();
   r12=r2-r1;
   dist=r12.GetNorm();

   if (dist<=fDmin) continue;

   // Select all the good hits of this second OM
   hits2.Clear();
   for (Int_t j2=1; j2<=omx2->GetNhits(); j2++)
   {
    sx2=omx2->GetHit(j2);
    if (!sx2) continue;
    if (sx2->GetDeadValue("ADC") || sx2->GetDeadValue("LE") || sx2->GetDeadValue("TOT")) continue;
    hits2.Add(sx2);
   }
 
   nh2=hits2.GetEntries();
   if (!nh2) continue;

   // Position r0 in between the two OMs and normalised relative direction r12
   rsum=(r1+r2)/2.;
   r0.SetPosition((Ali3Vector&)rsum);
   r12/=dist;

   // Check all hit pair combinations of these two OMs for possible track elements  
   dtmax=dist/c+float(fDtmarg);
   for (Int_t ih1=0; ih1<nh1; ih1++) // Hits of first OM
   {
    sx1=(AliSignal*)hits1.At(ih1);
    if (!sx1) continue;
    for (Int_t ih2=0; ih2<nh2; ih2++) // Hits of second OM
    {
     sx2=(AliSignal*)hits2.At(ih2);
     if (!sx2) continue;
     t1=sx1->GetSignal("LE",7);
     t2=sx2->GetSignal("LE",7);
     dt=t2-t1;
     t0=(t1+t2)/2.;

     if (fabs(dt)>=dtmax) continue;

     te=new AliTrack();
     tes.Add(te);
     ite++;
     if (dt<0) r12*=-1.;
     r0.SetTimestamp((AliTimestamp&)*evt);
     AliTimestamp* tsx=r0.GetTimestamp();
     tsx->Add(0,0,(int)t0);
     te->SetReferencePoint(r0);
     te->Set3Momentum(r12);
     dttest=dtmax;
     for (Int_t jt=ntes.GetSize(); jt>0; jt--)
     {
      if (fabs(dt)>=dttest) break;
      temap.EnterObject(ite,jt,te);
      ntes.AddAt(ntes.At(jt-1)+1,jt-1);
      dttest-=3.;
     }
    }
   }
  } // end of loop over the second OM of the pair
 } // end of loop over first OM of the pair

 // Association of hits to the various track elements
 // For the time being all track elements will be treated,
 // but in a later stage one could select only the TE's of a certain
 // 3 ns margin slot in the TE map to save CPU time.
 Int_t nte=tes.GetEntries();
 Int_t nh=hits.GetEntries();
 Float_t d=0;
 Ali3Vector p;
 Float_t tgeo,tres;
 AliSample levers;  // Statistics of the assoc. hit lever arms
 AliSignal fit;     // Storage of Q value etc... for each track candidate
 fit.SetSlotName("QTC",1);
 fit.SetSlotName("SIGMAL",2);
 Float_t qtc=0,qmax=0;
 Int_t nah;      // Number of associated hits for a certain TE
 Float_t sigmal; // The mean lever arm of the various associated hits
 for (Int_t jte=0; jte<nte; jte++)
 {
  te=(AliTrack*)tes.At(jte);
  if (!te) continue;
  AliPosition* tr0=te->GetReferencePoint();
  AliTimestamp* tt0=tr0->GetTimestamp();
  t0=evt->GetDifference(tt0,"ns");
  p=te->Get3Momentum();
  levers.Reset();
  for (Int_t jh=0; jh<nh; jh++)
  {
   sx1=(AliSignal*)hits.At(jh);
   if (!sx1) continue;
   IceGOM* omx=(IceGOM*)sx1->GetDevice();
   if (!omx) continue;
   r1=omx->GetPosition();
   d=tr0->GetDistance(r1);
   d*=sin(thetac);
   r12=r1-(*tr0);
   dist=p.Dot(r12)+d*tan(thetac);
   tgeo=t0+dist/c;
   t1=sx1->GetSignal("LE",7);
   tres=t1-tgeo;

   if (tres<-30 || tres>300 || d>25.*pow(tres+30.,0.25)) continue;

   // Associate this hit to the TE
   te->AddSignal(*sx1);
   levers.Enter(d/tan(thetac));
  }

  // Determine the Q quality of the various TE's.
  // Good quality TE's will be called track candidates (TC's)
  nah=te->GetNsignals();
  sigmal=levers.GetSigma(1);
  qtc=0.3*sigmal+7.;
  if (qtc>nah) qtc=nah;
  fit.SetSignal(qtc,"QTC");
  fit.SetSignal(sigmal,"SIGMAL");
  te->SetFitDetails(fit);
  if (qtc>qmax) qmax=qtc;
 }

 // Perform selection on Q value in case of multiple track candidates
 for (Int_t jtc=0; jtc<nte; jtc++)
 {
  te=(AliTrack*)tes.At(jtc);
  if (!te) continue;
  sx1=(AliSignal*)te->GetFitDetails();
  qtc=-1;
  if (sx1) qtc=sx1->GetSignal("QTC");
  if (qtc<0.7*qmax)
  {
   temap.RemoveObjects(te);
   tes.RemoveAt(jtc);
   delete te;
  }
 } 
 tes.Compress();
 nte=tes.GetEntries();

 if (!nte) return;

 // Order the track candidates w.r.t. decreasing number of associated hits
 TObjArray* ordered=0;
 ordered=evt->SortTracks(-1,&tes);
 TObjArray tcs(*ordered);

 // Cluster track candidates within a certain opening angle into jets.
 // Also the relative direction of the both r0's of the track candidates
 // should be within a certain opening angle w.r.t. the starting track direction,
 // unless the distance between the two r0's is below a certain maximum.
 // The latter prevents clustering of (nearly) parallel track candidates
 // crossing the detector a very different locations (e.g. muon bundles).
 // The average r0 and t0 of the constituent tracks will be taken as the
 // jet reference point. 
 TObjArray jets;
 jets.SetOwner();
 AliTrack* te2=0;
 Float_t ang=0;
 AliSample pos;
 AliSample time;
 Float_t vec[3],err[3];
 Float_t edist=0;
 for (Int_t jtc1=0; jtc1<nte; jtc1++)
 {
  te=(AliTrack*)tcs.At(jtc1);
  if (!te) continue;
  AliPosition* x1=te->GetReferencePoint();
  if (!x1) continue;
  AliTimestamp* ts1=x1->GetTimestamp();
  if (!ts1) continue;
  AliJet* jx=new AliJet();
  jx->AddTrack(te);
  pos.Reset();
  time.Reset();
  x1->GetPosition(vec,"car");
  pos.Enter(vec[0],vec[1],vec[2]);
  t0=evt->GetDifference(ts1,"ns");
  time.Enter(t0);
  for (Int_t jtc2=0; jtc2<nte; jtc2++)
  {
   if (jtc2==jtc1) continue;
   te2=(AliTrack*)tcs.At(jtc2);
   if (!te2) continue;
   ang=te->GetOpeningAngle(*te2,"deg");
   if (ang<fTangmax)
   {
    AliPosition* x2=te2->GetReferencePoint();
    if (!x2) continue;
    AliTimestamp* ts2=x2->GetTimestamp();
    if (!ts2) continue;
    dist=x1->GetDistance(x2);
    edist=x1->GetResultError();
    dt=ts1->GetDifference(ts2,"ns");
    if (dist>0)
    {
     r12=(*x2)-(*x1);
     if (dt<0) r12*=-1.;
     ang=te->GetOpeningAngle(r12,"deg");
     if (ang<fRtangmax || dist<(fRtdmax+edist))
     {
      x2->GetPosition(vec,"car");
      pos.Enter(vec[0],vec[1],vec[2]);
      t0=evt->GetDifference(ts2,"ns");
      time.Enter(t0);
      jx->AddTrack(te2);
     }
    }
   }
  }

  // Set the reference point data for this jet
  for (Int_t j=1; j<=3; j++)
  {
   vec[j-1]=pos.GetMean(j);
   err[j-1]=pos.GetSigma(j);
  }
  r0.SetPosition(vec,"car");
  r0.SetPositionErrors(err,"car");
  r0.SetTimestamp((AliTimestamp&)*evt);
  AliTimestamp* jt0=r0.GetTimestamp();
  t0=time.GetMean(1);
  jt0->Add(0,0,(int)t0);
  jx->SetReferencePoint(r0);

  // Store this jet for further processing if ntracks>1
  if (jx->GetNtracks() > 1 || fTangmax<=0 || fRtangmax<=0)
  {
   jets.Add(jx);
  }
  else // Only keep single-track jets which have qtc=qmax 
  {
   sx1=(AliSignal*)te->GetFitDetails();
   qtc=-1;
   if (sx1) qtc=sx1->GetSignal("QTC");
   if (qtc>=(qmax-1.e-10))
   {
    jets.Add(jx);
   }
   else
   {
    delete jx;
   }
  }
 }
 Int_t njets=jets.GetEntries();

 if (!njets) return;

 // Order the jets w.r.t. decreasing number of tracks
 ordered=evt->SortJets(-1,&jets);
 TObjArray jets2(*ordered);

 // Merge jets within a certain opening to provide the final track(s).
 AliJet* jx1=0;
 AliJet* jx2=0;
 if (fJangmax>0)
 {
  for (Int_t jet1=0; jet1<njets; jet1++)
  {
   jx1=(AliJet*)jets2.At(jet1);
   if (!jx1) continue;
   AliPosition* x1=jx1->GetReferencePoint();
   if (!x1) continue;
   AliTimestamp* ts1=x1->GetTimestamp();
   if (!ts1) continue;
   pos.Reset();
   time.Reset();
   x1->GetPosition(vec,"car");
   pos.Enter(vec[0],vec[1],vec[2]);
   t0=evt->GetDifference(ts1,"ns");
   time.Enter(t0);
   for (Int_t jet2=jet1+1; jet2<njets; jet2++)
   {
    jx2=(AliJet*)jets2.At(jet2);
    if (!jx2) continue;
    AliPosition* x2=jx2->GetReferencePoint();
    if (!x2) continue;
    AliTimestamp* ts2=x2->GetTimestamp();
    if (!ts2) continue;
    ang=jx1->GetOpeningAngle(*jx2,"deg");
    if (ang<fJangmax)
    {
     dist=x1->GetDistance(x2);
     edist=x1->GetResultError();
     dt=ts1->GetDifference(ts2,"ns");
     r12=(*x2)-(*x1);
     if (dt<0) r12*=-1.;
     ang=jx1->GetOpeningAngle(r12,"deg");
     if (ang<fRjangmax || dist<(fRjdmax+edist))
     {
      x2->GetPosition(vec,"car");
      pos.Enter(vec[0],vec[1],vec[2]);
      t0=evt->GetDifference(ts2,"ns");
      time.Enter(t0);
      for (Int_t jtk=1; jtk<=jx2->GetNtracks(); jtk++)
      {
       te=jx2->GetTrack(jtk);
       if (!te) continue;
       jx1->AddTrack(te);
      }
      jets2.RemoveAt(jet2);
     }
    }
   }
   // Set the reference point data for this jet
   for (Int_t k=1; k<=3; k++)
   {
    vec[k-1]=pos.GetMean(k);
    err[k-1]=pos.GetSigma(k);
   }
   r0.SetPosition(vec,"car");
   r0.SetPositionErrors(err,"car");
   r0.SetTimestamp((AliTimestamp&)*evt);
   AliTimestamp* jt0=r0.GetTimestamp();
   t0=time.GetMean(1);
   jt0->Add(0,0,(int)t0);
   jx1->SetReferencePoint(r0);
  }
  jets2.Compress();
  njets=jets2.GetEntries();
 }

 // Store every jet as a reconstructed track in the event structure.
 // The jet 3-momentum (normalised to 1) and reference point
 // (i.e.the average r0 and t0 of the constituent tracks) will make up
 // the final track parameters.
 // All the associated hits of all the constituent tracks of the jet
 // will be associated to the final track.
 // In case the jet angular separation was set <0, only the jet with
 // the maximum number of tracks (i.e. the first one in the array)
 // will be used to form a track. This will allow comparison with
 // the standard Sieglinde processing.
 AliTrack t; 
 t.SetNameTitle(fTrackname.Data(),"IceDwalk direct walk track");
 for (Int_t jet=0; jet<njets; jet++)
 {
  AliJet* jx=(AliJet*)jets2.At(jet);
  if (!jx) continue;
  AliPosition* ref=jx->GetReferencePoint();
  if (!ref) continue;
  evt->AddTrack(t);
  AliTrack* trk=evt->GetTrack(evt->GetNtracks());
  if (!trk) continue;
  trk->SetId(evt->GetNtracks(1)+1);
  p=jx->Get3Momentum();
  p/=p.GetNorm();
  trk->Set3Momentum(p);
  trk->SetReferencePoint(*ref);
  AliTimestamp* tt0=ref->GetTimestamp();
  if (tt0) trk->SetTimestamp(*tt0);
  for (Int_t jt=1; jt<=jx->GetNtracks(); jt++)
  {
   AliTrack* tx=jx->GetTrack(jt);
   if (!tx) continue;
   for (Int_t is=1; is<=tx->GetNsignals(); is++)
   {
    sx1=tx->GetSignal(is);
    if (sx1) sx1->AddLink(trk);
   }
  }

  // Only take the jet with the maximum number of tracks
  // (i.e. the first jet in the list) when the user had selected
  // this reconstruction mode.
  if (fJangmax<0) break;
 }
}
///////////////////////////////////////////////////////////////////////////
