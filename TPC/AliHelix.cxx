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

//-------------------------------------------------------------------------
//                Implementation of the AliHelix class
//        Origin: Marian Ivanov, CERN, marian.ivanov@cern.ch
//-------------------------------------------------------------------------


#include "AliHelix.h"
#include "AliKalmanTrack.h"
#include "TMath.h"
ClassImp(AliHelix)


//_______________________________________________________________________
AliHelix::AliHelix()
{
  //
  // Default constructor
  //
  for (Int_t i =0;i<9;i++) fHelix[i]=0;
}

//_______________________________________________________________________
AliHelix::AliHelix(const AliHelix &t){
  //
  //
  for (Int_t i=0;i<9;i++) 
    fHelix[i]=t.fHelix[i];
}

AliHelix::AliHelix(const AliKalmanTrack &t)
{
  //
  // 
  Double_t alpha,x,cs,sn;
  t.GetExternalParameters(x,fHelix); 
  alpha=t.GetAlpha();
  //
  //circle parameters
  fHelix[4]=fHelix[4]/t.GetConvConst();    // C
  cs=TMath::Cos(alpha); sn=TMath::Sin(alpha);

  Double_t xc, yc, rc;
  rc  =  1/fHelix[4];
  xc  =  x-fHelix[2]*rc;
  yc  =  fHelix[0]+TMath::Sqrt(1-(x-xc)*(x-xc)*fHelix[4]*fHelix[4])/fHelix[4];
  
  fHelix[6] = xc*cs - yc*sn;
  fHelix[7] = xc*sn + yc*cs;
  fHelix[8] =  TMath::Abs(rc);
  //
  //
  fHelix[5]=x*cs - fHelix[0]*sn;            // x0
  fHelix[0]=x*sn + fHelix[0]*cs;            // y0
  //fHelix[1]=                               // z0
  fHelix[2]=TMath::ASin(fHelix[2]) + alpha; // phi0
  //fHelix[3]=                               // tgl
  //
  //
  fHelix[5]   = fHelix[6];
  fHelix[0]   = fHelix[7];
  //fHelix[5]-=TMath::Sin(fHelix[2])/fHelix[4]; 
  //fHelix[0]+=TMath::Cos(fHelix[2])/fHelix[4];  
}

AliHelix::AliHelix(Double_t x[3], Double_t p[3], Double_t charge, Double_t conversion)
{
  //
  //
  //
  Double_t pt = TMath::Sqrt(p[0]*p[0]+p[1]*p[1]);
  if (TMath::Abs(conversion)<0.00000001) 
    conversion = AliKalmanTrack::GetConvConst();
  //
  //  
  fHelix[4] = charge/(conversion*pt); // C
  fHelix[3] = p[2]/pt;    // tgl
  //  
  Double_t xc, yc, rc;
  rc  =  1/fHelix[4];
  xc  =  x[0]  -rc*p[1]/pt;
  yc  =  x[1]  +rc*p[0]/pt; 
  //
  fHelix[5] = x[0];   // x0
  fHelix[0] = x[1];   // y0
  fHelix[1] = x[2];   // z0
  //
  fHelix[6] = xc;
  fHelix[7] = yc;
  fHelix[8] = TMath::Abs(rc);
  //
  fHelix[5]=xc; 
  fHelix[0]=yc; 
  //
  if (TMath::Abs(p[1])<TMath::Abs(p[0])){     
    fHelix[2]=TMath::ASin(p[1]/pt);
    if (charge*yc<charge*x[1])  fHelix[2] = TMath::Pi()-fHelix[2];
  }
  else{
    fHelix[2]=TMath::ACos(p[0]/pt);
    if (charge*xc>charge*x[0])  fHelix[2] = -fHelix[2];
  }

}

void  AliHelix::GetMomentum(Double_t phase, Double_t p[4],Double_t conversion)
{
  // return  momentum at given phase
  Double_t x[3],g[3],gg[3];
  Evaluate(phase,x,g,gg);
  if (TMath::Abs(conversion)<0.0001) conversion = AliKalmanTrack::GetConvConst();
  Double_t mt = TMath::Sqrt(g[0]*g[0]+g[1]*g[1]);
  p[0] = fHelix[8]*g[0]/(mt*conversion);
  p[1] = fHelix[8]*g[1]/(mt*conversion);
  p[2] = fHelix[8]*g[2]/(mt*conversion);
}

void   AliHelix::GetAngle(Double_t t1, AliHelix &h, Double_t t2, Double_t angle[3])
{
  //
  //
  //
  Double_t x1[3],g1[3],gg1[3];
  Double_t x2[3],g2[3],gg2[3];
  Evaluate(t1,x1,g1,gg1);
  h.Evaluate(t2,x2,g2,gg2);

  //
  Double_t norm1r = g1[0]*g1[0]+g1[1]*g1[1];
  Double_t norm1  = TMath::Sqrt(norm1r+g1[2]*g1[2]);
  norm1r         = TMath::Sqrt(norm1r);
  //
  Double_t norm2r = g2[0]*g2[0]+g2[1]*g2[1];
  Double_t norm2  = TMath::Sqrt(norm2r+g2[2]*g2[2]);
  norm2r         = TMath::Sqrt(norm2r);
  //
  angle[0]  = TMath::ACos((g1[0]*g2[0]+g1[1]*g2[1])/(norm1r*norm2r));   // angle in phi projection
  angle[1]  = TMath::ACos(((norm1r*norm2r)+g1[2]*g2[2])/(norm1*norm2)); // angle in rz  projection
  angle[2]  = TMath::ACos((g1[0]*g2[0]+g1[1]*g2[1]+g1[2]*g2[2])/(norm1*norm2)); //3D angle

    
  

}


void AliHelix::Evaluate(Double_t t,
                     Double_t r[3],  //radius vector
                     Double_t g[3],  //first defivatives
                     Double_t gg[3]) //second derivatives
{
  //--------------------------------------------------------------------
  // Calculate position of a point on a track and some derivatives at given phase
  //--------------------------------------------------------------------
  Double_t phase=fHelix[4]*t+fHelix[2];
  Double_t sn=TMath::Sin(phase), cs=TMath::Cos(phase);

  //r[0] = fHelix[5] + (sn - fHelix[6])/fHelix[4];
  //r[1] = fHelix[0] - (cs - fHelix[7])/fHelix[4];  
  r[0] = fHelix[5] + sn/fHelix[4];
  r[1] = fHelix[0] - cs/fHelix[4];  
  r[2] = fHelix[1] + fHelix[3]*t;

  g[0] = cs; g[1]=sn; g[2]=fHelix[3];
  
  gg[0]=-fHelix[4]*sn; gg[1]=fHelix[4]*cs; gg[2]=0.;
}

Double_t  AliHelix::GetPhase(Double_t x, Double_t y )
			
{
  //
  //calculate helix param at given x,y  point
  //
  Double_t phase  =  (x-fHelix[5])*fHelix[4];
  if (TMath::Abs(phase)>=1)
    phase = TMath::Sign(0.99999999999,phase);
  phase = TMath::ASin(phase);

  if ( ( ( fHelix[0]-y)*fHelix[4]) < 0.) {
    if (phase>0) 
      phase = TMath::Pi()-phase;
    else
      phase = -(TMath::Pi()+phase);
  }
  if ( (phase-fHelix[2])>TMath::Pi())  phase = phase-2.*TMath::Pi();
  if ( (phase-fHelix[2])<-TMath::Pi()) phase = phase+2.*TMath::Pi();

  Double_t t     = (phase-fHelix[2])/fHelix[4];
  
  //  Double_t r[3];
  //Evaluate(t,r);
  //if  ( (TMath::Abs(r[0]-x)>0.01) || (TMath::Abs(r[1]-y)>0.01)){
  //     Double_t phase  =  (x-fHelix[5])*fHelix[4];
  //  printf("problem\n");    
  //}
  return t;
}

Int_t AliHelix::GetPhase(Double_t r0, Double_t t[2]) 
{
  //
  //calculate helix param at given r  point - return nearest point ()
  //
  // not implemented yet
  

  return 0;
}


Double_t  AliHelix::GetPhaseZ(Double_t z0)
{
  //
  //
  return (z0-fHelix[1])/fHelix[3];
}


Int_t    AliHelix::GetRPHIintersections(AliHelix &h, Double_t phase[2][2], Double_t ri[2], Double_t cut)
{
  //--------------------------------------------------------------------
  // This function returns  phase vectors with intesection between helix (0, 1 or 2)
  // in x-y plane projection  
  //--------------------------------------------------------------------
  //    
  //  Double_t * c1 = &fHelix[6];
  //Double_t * c2 = &(h.fHelix[6]);
  //  Double_t  c1[3] = {fHelix[5],fHelix[0],fHelix[8]};
  Double_t  c1[3] = {0,0,fHelix[8]};
  Double_t  c2[3] = {h.fHelix[5]-fHelix[5],h.fHelix[0]-fHelix[0],h.fHelix[8]};

  Double_t d  = TMath::Sqrt(c2[0]*c2[0]+c2[1]*c2[1]); 
  //
  Double_t x0[2];
  Double_t y0[2];
  //  
  if ( d>=(c1[2]+c2[2])){
    if (d>=(c1[2]+c2[2]+cut)) return 0;
    x0[0] = (d+c1[2]-c2[2])*c2[0]/(2*d)+ fHelix[5];
    y0[0] = (d+c1[2]-c2[2])*c2[1]/(2*d)+ fHelix[0];
    return 0;
    phase[0][0] = GetPhase(x0[0],y0[0]);
    phase[0][1] = h.GetPhase(x0[0],y0[0]);
    ri[0] = x0[0]*x0[0]+y0[0]*y0[0];
    return 1;
  }
  if ( (d+c2[2])<c1[2]){
    if ( (d+c2[2])+cut<c1[2]) return 0;
    //
    Double_t xx = c2[0]+ c2[0]*c2[2]/d+ fHelix[5];
    Double_t yy = c2[1]+ c2[1]*c2[2]/d+ fHelix[0]; 
    phase[0][1] = h.GetPhase(xx,yy);
    //
    Double_t xx2 = c2[0]*c1[2]/d+ fHelix[5];
    Double_t yy2 = c2[1]*c1[2]/d+ fHelix[0]; 
    phase[0][0] = GetPhase(xx2,yy2);
    ri[0] = xx*xx+yy*yy;
    return 1;
  }

  if ( (d+c1[2])<c2[2]){
    if ( (d+c1[2])+cut<c2[2]) return 0;
    //
    Double_t xx = -c2[0]*c1[2]/d+ fHelix[5];
    Double_t yy = -c2[1]*c1[2]/d+ fHelix[0]; 
    phase[0][1] = GetPhase(xx,yy);
    //
    Double_t xx2 = c2[0]- c2[0]*c2[2]/d+ fHelix[5];
    Double_t yy2 = c2[1]- c2[1]*c2[2]/d+ fHelix[0]; 
    phase[0][0] = h.GetPhase(xx2,yy2);
    ri[0] = xx*xx+yy*yy;
    return 1;
  }

  Double_t d1 = (d*d+c1[2]*c1[2]-c2[2]*c2[2])/(2.*d);
  Double_t v1 = c1[2]*c1[2]-d1*d1;
  if (v1<0) return 0;
  v1 = TMath::Sqrt(v1);
  //
  x0[0] = (c2[0]*d1+c2[1]*v1)/d + fHelix[5];
  y0[0] = (c2[1]*d1-c2[0]*v1)/d + fHelix[0];            
  //
  x0[1] = (c2[0]*d1-c2[1]*v1)/d + fHelix[5];
  y0[1] = (c2[1]*d1+c2[0]*v1)/d + fHelix[0];      
  //
  for (Int_t i=0;i<2;i++){
    phase[i][0] = GetPhase(x0[i],y0[i]);
    phase[i][1] = h.GetPhase(x0[i],y0[i]);
    ri[i] = x0[i]*x0[i]+y0[i]*y0[i];    
  }      
  return 2;
} 

/*

Int_t    AliHelix::GetRPHIintersections(AliHelix &h, Double_t phase[2][2], Double_t ri[2], Double_t cut)
{
  //--------------------------------------------------------------------
  // This function returns  phase vectors with intesection between helix (0, 1 or 2)
  // in x-y plane projection  
  //--------------------------------------------------------------------
  //    
  Double_t * c1 = &fHelix[6];
  Double_t * c2 = &(h.fHelix[6]);
  Double_t d  = TMath::Sqrt((c1[0]-c2[0])*(c1[0]-c2[0])+(c1[1]-c2[1])*(c1[1]-c2[1])); 
  //
  Double_t x0[2];
  Double_t y0[2];
  //  
  if ( d>=(c1[2]+c2[2])){
    if (d>=(c1[2]+c2[2]+cut)) return 0;
    x0[0] = c1[0]+ (d+c1[2]-c2[2])*(c2[0]-c1[0])/(2*d);
    y0[0] = c1[1]+ (d+c1[2]-c2[2])*(c2[1]-c1[1])/(2*d);
    return 0;
    phase[0][0] = GetPhase(x0[0],y0[0]);
    phase[0][1] = h.GetPhase(x0[0],y0[0]);
    ri[0] = x0[0]*x0[0]+y0[0]*y0[0];
    return 1;
  }
  if ( (d+c2[2])<c1[2]){
    if ( (d+c2[2])+cut<c1[2]) return 0;
    //
    Double_t xx = c2[0]+ (c2[0]-c1[0])*c2[2]/d;
    Double_t yy = c2[1]+ (c2[1]-c1[1])*c2[2]/d; 
    phase[0][1] = h.GetPhase(xx,yy);
    //
    Double_t xx2 = c1[0]- (c1[0]-c2[0])*c1[2]/d;
    Double_t yy2 = c1[1]- (c1[1]-c2[1])*c1[2]/d; 
    phase[0][0] = GetPhase(xx2,yy2);
    //if ( (TMath::Abs(xx2-xx)>cut)||(TMath::Abs(yy2-yy)>cut)){
    //  printf("problem\n");
    //}
    ri[0] = xx*xx+yy*yy;
    return 1;
  }

  if ( (d+c1[2])<c2[2]){
    if ( (d+c1[2])+cut<c2[2]) return 0;
    //
    Double_t xx = c1[0]+ (c1[0]-c2[0])*c1[2]/d;
    Double_t yy = c1[1]+ (c1[1]-c2[1])*c1[2]/d; 
    phase[0][1] = GetPhase(xx,yy);
    //
    Double_t xx2 = c2[0]- (c2[0]-c1[0])*c2[2]/d;
    Double_t yy2 = c2[1]- (c2[1]-c1[1])*c2[2]/d; 
    phase[0][0] = h.GetPhase(xx2,yy2);
    //if ( (TMath::Abs(xx2-xx)>cut)||(TMath::Abs(yy2-yy)>cut)){
    //  printf("problem\n");
    //}
    ri[0] = xx*xx+yy*yy;
    return 1;
  }

  Double_t d1 = (d*d+c1[2]*c1[2]-c2[2]*c2[2])/(2.*d);
  Double_t v1 = c1[2]*c1[2]-d1*d1;
  if (v1<0) return 0;
  v1 = TMath::Sqrt(v1);
  //
  x0[0] = c1[0]+ ((c2[0]-c1[0])*d1-(c1[1]-c2[1])*v1)/d;
  y0[0] = c1[1]+ ((c2[1]-c1[1])*d1+(c1[0]-c2[0])*v1)/d;            
  //
  x0[1] = c1[0]+ ((c2[0]-c1[0])*d1+(c1[1]-c2[1])*v1)/d;
  y0[1] = c1[1]+ ((c2[1]-c1[1])*d1-(c1[0]-c2[0])*v1)/d;      
  //
  for (Int_t i=0;i<2;i++){
    phase[i][0] = GetPhase(x0[i],y0[i]);
    phase[i][1] = h.GetPhase(x0[i],y0[i]);
    ri[i] = x0[i]*x0[i]+y0[i]*y0[i];    
  }      
  return 2;
} 
*/


Int_t   AliHelix::LinearDCA(AliHelix &h, Double_t &t1, Double_t &t2, 
		      Double_t &R, Double_t &dist)
{
  //
  //
  // find intersection using linear approximation
  Double_t r1[3],g1[3],gg1[3];
  Double_t r2[3],g2[3],gg2[3];
  //
  Evaluate(t1,r1,g1,gg1);
  h.Evaluate(t2,r2,g2,gg2);
  // 
  Double_t g1_2 = g1[0]*g1[0] +g1[1]*g1[1] +g1[2]*g1[2];
  Double_t g2_2 = g2[0]*g2[0] +g2[1]*g2[1] +g2[2]*g2[2];
  Double_t g1x2 = g1[0]*g2[0] +g1[1]*g2[1] +g1[2]*g2[2];  
  Double_t det  = g1_2*g2_2   - g1x2*g1x2;
  //  
  if (TMath::Abs(det)>0){
    //
    Double_t r1g1 = r1[0]*g1[0] +r1[1]*g1[1] +r1[2]*g1[2];    
    Double_t r2g1 = r2[0]*g1[0] +r2[1]*g1[1] +r2[2]*g1[2];      
    Double_t r1g2 = r1[0]*g2[0] +r1[1]*g2[1] +r1[2]*g2[2];
    Double_t r2g2 = r2[0]*g2[0] +r2[1]*g2[1] +r2[2]*g2[2];
    //    
    Double_t dt    = - ( g2_2*(r1g1-r2g1) - g1x2*(r1g2-r2g2)) / det;      
    Double_t dp    = - ( g1_2*(r2g2-r1g2) - g1x2*(r2g1-r1g1)) / det;
    //
    t1+=dt;
    t2+=dp;
    Evaluate(t1,r1);
    h.Evaluate(t2,r2);
    //
    dist = (r1[0]-r2[0])*(r1[0]-r2[0])+
				  (r1[1]-r2[1])*(r1[1]-r2[1])+
				  (r1[2]-r2[2])*(r1[2]-r2[2]);    
    R = ((r1[0]+r2[0])*(r1[0]+r2[0])+(r1[1]+r2[1])*(r1[1]+r2[1]))/4.;
  }     
  return 0;
}




/*
Int_t  AliHelix::ParabolicDCA(AliHelix&h,  //helixes
			       Double_t &t1, Double_t &t2, 
			       Double_t &R, Double_t &dist, Int_t iter)
{
  //
  //
  // find intersection using linear fit
  Double_t r1[3],g1[3],gg1[3];
  Double_t r2[3],g2[3],gg2[3];
  //
  Evaluate(t1,r1,g1,gg1);
  h.Evaluate(t2,r2,g2,gg2);

  //
  Double_t dx2=1.;
  Double_t dy2=1.;
  Double_t dz2=1.;
  //
  Double_t dx=r2[0]-r1[0], dy=r2[1]-r1[1], dz=r2[2]-r1[2];
  Double_t dm=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;
  //

 iter++;
 while (iter--) {

     Double_t gt1=-(dx*g1[0]/dx2 + dy*g1[1]/dy2 + dz*g1[2]/dz2);
     Double_t gt2=+(dx*g2[0]/dx2 + dy*g2[1]/dy2 + dz*g2[2]/dz2);
     Double_t h11=(g1[0]*g1[0] - dx*gg1[0])/dx2 + 
                  (g1[1]*g1[1] - dy*gg1[1])/dy2 +
                  (g1[2]*g1[2] - dz*gg1[2])/dz2;
     Double_t h22=(g2[0]*g2[0] + dx*gg2[0])/dx2 + 
                  (g2[1]*g2[1] + dy*gg2[1])/dy2 +
                  (g2[2]*g2[2] + dz*gg2[2])/dz2;
     Double_t h12=-(g1[0]*g2[0]/dx2 + g1[1]*g2[1]/dy2 + g1[2]*g2[2]/dz2);

     Double_t det=h11*h22-h12*h12;

     Double_t dt1,dt2;
     if (TMath::Abs(det)<1.e-33) {
        //(quasi)singular Hessian
        dt1=-gt1; dt2=-gt2;
     } else {
        dt1=-(gt1*h22 - gt2*h12)/det; 
        dt2=-(h11*gt2 - h12*gt1)/det;
     }

     if ((dt1*gt1+dt2*gt2)>0) {dt1=-dt1; dt2=-dt2;}

     //check delta(phase1) ?
     //check delta(phase2) ?

     if (TMath::Abs(dt1)/(TMath::Abs(t1)+1.e-3) < 1.e-4)
     if (TMath::Abs(dt2)/(TMath::Abs(t2)+1.e-3) < 1.e-4) {
       //if ((gt1*gt1+gt2*gt2) > 1.e-4/dy2/dy2) 
       //  Warning("GetDCA"," stopped at not a stationary point !\n");
        Double_t lmb=h11+h22; lmb=lmb-TMath::Sqrt(lmb*lmb-4*det);
        if (lmb < 0.) 
	  //Warning("GetDCA"," stopped at not a minimum !\n");
        break;
     }

     Double_t dd=dm;
     for (Int_t div=1 ; ; div*=2) {
        Evaluate(t1+dt1,r1,g1,gg1);
        h.Evaluate(t2+dt2,r2,g2,gg2);
        dx=r2[0]-r1[0]; dy=r2[1]-r1[1]; dz=r2[2]-r1[2];
        dd=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;
	if (dd<dm) break;
        dt1*=0.5; dt2*=0.5;
        if (div>512) {
	  //Warning("GetDCA"," overshoot !\n"); 
	  break;
        }   
     }
     dm=dd;

     t1+=dt1;
     t2+=dt2;

 }

 Evaluate(t1,r1,g1,gg1);
 h.Evaluate(t2,r2,g2,gg2);
 //
 dist = (r1[0]-r2[0])*(r1[0]-r2[0])+
   (r1[1]-r2[1])*(r1[1]-r2[1])+
   (r1[2]-r2[2])*(r1[2]-r2[2]);    
 
 R = ((r1[0]+r2[0])*(r1[0]+r2[0])+(r1[1]+r2[1])*(r1[1]+r2[1]))/4;
 
}
*/






Int_t  AliHelix::ParabolicDCA(AliHelix&h,  //helixes
			       Double_t &t1, Double_t &t2, 
			       Double_t &R, Double_t &dist, Int_t iter)
{
  //
  //
  // find intersection using linear fit
  Double_t r1[3],g1[3],gg1[3];
  Double_t r2[3],g2[3],gg2[3];
  //
  Evaluate(t1,r1,g1,gg1);
  h.Evaluate(t2,r2,g2,gg2);

  //
  Double_t dx2=1.;
  Double_t dy2=1.;
  Double_t dz2=1.;
  //
  Double_t dx=r2[0]-r1[0], dy=r2[1]-r1[1], dz=r2[2]-r1[2];
  Double_t dm=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;
  //

 iter++;
 while (iter--) {
    Double_t gt1=-(dx*g1[0]/dx2 + dy*g1[1]/dy2 + dz*g1[2]/dz2);
    Double_t gt2=+(dx*g2[0]/dx2 + dy*g2[1]/dy2 + dz*g2[2]/dz2);
    
    Double_t h11=(g1[0]*g1[0] - dx*gg1[0])/dx2 + 
      (g1[1]*g1[1] - dy*gg1[1])/dy2 +
      (g1[2]*g1[2] - dz*gg1[2])/dz2;
    Double_t h22=(g2[0]*g2[0] + dx*gg2[0])/dx2 + 
      (g2[1]*g2[1] + dy*gg2[1])/dy2 +
      (g2[2]*g2[2] + dz*gg2[2])/dz2;
    Double_t h12=-(g1[0]*g2[0]/dx2 + g1[1]*g2[1]/dy2 + g1[2]*g2[2]/dz2);
    
    Double_t det=h11*h22-h12*h12;
    
    Double_t dt1,dt2;
    if (TMath::Abs(det)<1.e-33) {
      //(quasi)singular Hessian
      dt1=-gt1; dt2=-gt2;
    } else {
      dt1=-(gt1*h22 - gt2*h12)/det; 
      dt2=-(h11*gt2 - h12*gt1)/det;
    }
    
    if ((dt1*gt1+dt2*gt2)>0) {dt1=-dt1; dt2=-dt2;}
    
    //if (TMath::Abs(dt1)/(TMath::Abs(t1)+1.e-3) < 1.e-4)
    //  if (TMath::Abs(dt2)/(TMath::Abs(t2)+1.e-3) < 1.e-4) {
    //	break;
    //  }
    
    Double_t dd=dm;
    for (Int_t div=1 ; div<512 ; div*=2) {
      Evaluate(t1+dt1,r1,g1,gg1);
      h.Evaluate(t2+dt2,r2,g2,gg2);
      dx=r2[0]-r1[0]; dy=r2[1]-r1[1]; dz=r2[2]-r1[2];
      dd=dx*dx/dx2 + dy*dy/dy2 + dz*dz/dz2;
      if (dd<dm) break;
      dt1*=0.5; dt2*=0.5;
      if (div==0){
	div =1;
      }
      if (div>512) {	  
	break;
      }   
    }
    dm=dd;
    t1+=dt1;
    t2+=dt2;
 }
 Evaluate(t1,r1,g1,gg1);
 h.Evaluate(t2,r2,g2,gg2);
 //
 dist = (r1[0]-r2[0])*(r1[0]-r2[0])+
   (r1[1]-r2[1])*(r1[1]-r2[1])+
   (r1[2]-r2[2])*(r1[2]-r2[2]);    
 
 R = ((r1[0]+r2[0])*(r1[0]+r2[0])+(r1[1]+r2[1])*(r1[1]+r2[1]))/4;
 return 0;
 
}

