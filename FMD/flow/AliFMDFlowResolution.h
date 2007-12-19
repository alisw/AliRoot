// -*- mode: C++ -*-
/* Copyright (C) 2007 Christian Holm Christensen <cholm@nbi.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
/** @file 
    @brief Declaration of an Resolution class */
//____________________________________________________________________
//
// Calculate the event plane resolution. 
// Input is the observed phis. 
// There's a number of implementations of this. 
#ifndef ALIFMDFLOWRESOLUTION_H
#define ALIFMDFLOWRESOLUTION_H
#include <flow/AliFMDFlowStat.h>
#include <TH1D.h>
#include <TH2D.h>
class TBrowser;

//______________________________________________________
/** @class AliFMDFlowResolution flow/AliFMDFlowResolution.h <flow/AliFMDFlowResolution.h>
    @brief Class to calculate the event plane resolution based on two
    sub-events 
    @ingroup a_basic
    
    This class calculates the event plane resolution based on the
    basic formulas given in Phys. Rev. @b C58, 1671.   That is, the
    resolution is given by 
    @f[ 
    R_{k} = \langle\cos(km(\Psi_m-\Psi_R))\rangle 
    @f] 
    where @f$ \Psi_R@f$ is the unknown @e true event plane angle, and
    @f$ n = km@f$

    Using two random sub-events, @f$ A, B@f$ we get that 
    @f[
    \langle\cos(n(\Psi^A_m-\Psi^B_m))\rangle = 
    \langle\cos(n(\Psi^A_m-\Psi_R))\rangle 
    \langle\cos(n(\Psi^B_m-\Psi_R))\rangle 
    @f]
    If the sub-events are of equal size, and randomly chosen, then we
    get that 
    @f[ 
    \langle\cos(n(\Psi^A_m-\Psi_R))\rangle = 
    \sqrt{\langle\cos(n(\Psi^A_m-\Psi^B_m))}
    @f]
    and it follows that 
    @f{eqnarray*}
    \langle\cos(km(\Psi_m-\Psi_R))\rangle & = & 
    \sqrt{2}\langle\cos(n(\Psi^A_m-\Psi_R))\rangle\\
    \sqrt{2}\sqrt{\langle\cos(n(\Psi^A_m-\Psi^B_m))}
    @f}

    Hence, the event-plane resolution is simply the square root of the
    scaled average distance between the two sub-events, multiplied by
    @f$ \sqrt{s}@f$ 

    The error is therefor @f$ \sqrt{s}@f$ times the variance of the
    cosine of the distance between the two sub-events. 
*/
class AliFMDFlowResolution : public AliFMDFlowStat
{
public:
  /** Constructor
      @param n Harmonic order */
  AliFMDFlowResolution(UShort_t n=0);
  /** Destructor */
  virtual ~AliFMDFlowResolution() {}
  /** Copy constructor 
      @param o Object to copy from */ 
  AliFMDFlowResolution(const AliFMDFlowResolution& o);
  /** Assignment operator
      @param o Object to copy from */ 
  AliFMDFlowResolution& operator=(const AliFMDFlowResolution& o);

  /** @{ 
      @name Processing */
  /** add data point 
      @param psiA A sub-event plane angle @f$ \Psi_A \in[0,2\pi]@f$
      @param psiB B sub-event plane angle @f$ \Psi_B \in[0,2\pi]@f$ */
  virtual void Add(Double_t psiA, Double_t psiB);
  /** @} */

  /** @{ 
      @name Information */
  /** Get the correction for harmonic strength of order @a k 
      @param k  The harminic strenght order to get the correction for
      @param e2 The square error on the correction 
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k, Double_t& e2) const;
  /** Get the correction for harmonic strength of order @a k 
      @param k The harminic strenght order to get the correction for
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k) const;
  /** Get the harmnic order */
  UShort_t Order() const { return fOrder; }
  /** @} */
  
  /** @{ 
      @name Utility */
  /** Draw this corrrection function 
      @param option Options passed to drawing */
  virtual void Draw(Option_t* option=""); //*MENU*
  /** This is a folder */ 
  Bool_t IsFolder() const { return kTRUE; }
  /** Look through this object */ 
  virtual void Browse(TBrowser* b); 
  /** @} */

  /** @{ 
      @name histograms */ 
  /** @return Contrib histogram */
  const TH1& ContribHistogram() const { return fContrib; }
  /** @} */
protected:
  /** Order */
  UShort_t fOrder; // Order 
  /** Contributions */
  TH1D fContrib;
  /** Define for ROOT I/O */
  ClassDef(AliFMDFlowResolution,1);
};

//______________________________________________________
/** 
    @ingroup a_basic
    The event plane angle resolution function is 
    @f[
    R_k(\chi) = \frac{\pi} \chi e^{-\chi^2/4}\left( 
    I_{\frac{k-1}2}(\chi^2/4) + I_{\frac{k+1}2}(\chi^2/4)\right)
    @f]
    Where @f$ I_n(x)@f$ is the modified Bessel function of the first
    kind.   Identifying 
    @f[
    y = \chi^2/4\quad C=\frac{\sqrt{\pi}e^{-y}}{2\sqrt{2}}
    @f]
    and the short hands @f$ f2(y) = I_{\frac{k-1}2}@f$ and @f$ f3(y) =
    I_{\frac{k+1}2}(y)@f$ we can write this more compact as 
    @f[ 
    R_k(y) = C y (f2(y) + f3(y))
    @f] 
    The derivative of the resolution function is 
    @f[ 
    R_k'(y) = \frac{C}{2}\left[4\sqrt{y}\left(f2'(y)-f3'(y)\right) 
    - (4 y - 2)\left(f2(y) + f3(y)\right)\right]
    @f]
    Since 
    @f[
    I_\nu'(x) = I_{\nu-1}(x) - \frac{\nu}{x} I_\nu(x)\quad,
    @f]
    and setting @f$ f1(y) = I_{\frac{k-3}2}(y)@f$, we get 
    @f[ 
    R_k'(y) = \frac{C}{2}\left[4y f1(y) + (4-2k) f2(y) - (4y+2k)
    f3(y)\right]
    @f]

    In this class, the argument @f$ \chi@f$ is estimated by finding
    the minima of @f$ R_k(\chi)@f$ near the average of
    @f$\cos(n(\Psi_A-\Psi_B))@f$.  The error @f$ \delta\chi@f$ is
    estimated as the largest step size in the minimisation. 

    The total error on the correction is then 
    @f[ 
    \delta R_k = R_k'(\chi) \delta\chi
    @f] 
*/

class AliFMDFlowResolutionStar : public AliFMDFlowResolution
{
public:
  /** Constructor
      @param n Harmonic order */
  AliFMDFlowResolutionStar(UShort_t n=0) : AliFMDFlowResolution(n) {}
  /** Destructor */
  virtual ~AliFMDFlowResolutionStar() {}

  /** @{ 
      @name Information */
  /** Get the correction for harmonic strength of order @a k 
      @param k The harminic strenght order to get the correction for
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k) const;
  /** Get the correction for harmonic strength of order @a k 
      @param k  The harminic strenght order to get the correction for
      @param e2 The square error on the correction 
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k, Double_t& e2) const;
  /** Get @f$ \chi@f$ 
      @param res  First shot at the resolution. 
      @param k    Order 
      @param delta On return, the last step size in @f$ \chi@f$ -
      which is taken to be @f$ \delta\chi@f$  
      @return @f$\chi@f$ */
  virtual Double_t Chi(Double_t res, UShort_t k, Double_t& delta) const;
  /** @} */

  /** @{ 
      @name Utilities */
  /** Draw this corrrection function 
      @param option Options passed to drawing */
  virtual void Draw(Option_t* option=""); //*MENU*
  /** @} */
protected:
  /** Calculate resolution 
      @param chi @f$ \chi@f$
      @param k   Order factor 
      @param dr  On return, the derivative of @f$ R(\chi)@f$
      @return 
      @f[ 
      \frac{\sqrt{\pi/2}}{2}\chi e^{-\chi^2/4}
      (I_{\frac{(k-1)}{2}}(\chi^2/4)+ I_{\frac{(k+1)}{2}}(\chi^2/4))
      @f] */
  Double_t Res(Double_t chi, UShort_t k, Double_t& dr) const;
  /** Define for ROOT I/O */
  ClassDef(AliFMDFlowResolutionStar,1);
};

//______________________________________________________
/** 
    @ingroup a_basic

    For more on the event plane angle resolution function, please
    refer to the class description of ResolutionStar. 

    In this class @f$ \chi@f$ is calculated from the ratio @f$ k/N@f$
    of events with @f$ |\Psi_A - \Psi_B| > \pi/2@f$ to the total
    number of events. 

    The pre-print @c nucl-ex/9711003v2 gives the formula 
    @f[ 
    \frac{k}{N} = \frac{e^{-\chi^2/2}}{2}
    @f] 
    for @f$ \chi@f$.  Note, that this differs from the @f$ \chi@f$
    used in ResolutionStar by a factor of @f$ 1/sqrt{2}@f$. 
    
    We can isolate @f$ \chi = \mp\sqrt{-2\log(2k/N)}@f$ from the
    equation above.  

    Since @f$ r=k/N@f$ is obviously a efficiency-like ratio, we get
    that error @f$ \delta r@f$ is given by Binomial errors 
    @f[ 
    \delta r =  \sqrt{r\frac{1 - r}{N}}\quad.
    @f] 
    The total error @f$ \delta\chi@f$ then becomes 
    @f[ 
    \delta^2\chi = \left(\frac{d\chi}{dr}\right)^2 \delta^2r 
    = \frac{r - 1}{4 k log(2 r)}
    @f]

    The total error on the correction is, as in
    ResolutionStar, then given by 
    @f[ 
    \delta R_k = R_k'(\chi) \delta\chi
    @f]
*/
class AliFMDFlowResolutionTDR : public AliFMDFlowResolution
{
public:
  /** Constructor
      @param n Harmonic order */
  AliFMDFlowResolutionTDR(UShort_t n=0); 
  /** DTOR */
  ~AliFMDFlowResolutionTDR() {}
  /** Copy constructor 
      @param o  Object to copy from */ 
  AliFMDFlowResolutionTDR(const AliFMDFlowResolutionTDR& o);
  /** Assignment operator 
      @param o  Object to assign from 
      @return reference to this */ 
  AliFMDFlowResolutionTDR& operator=(const AliFMDFlowResolutionTDR& o);

  /** @{ 
      @name Processing */
  virtual void Clear(Option_t* option="");
  /** add a data  point */
  virtual void Add(Double_t psiA, Double_t psiB);
  /** @} */

  /** @{ 
      @name Information */
  /** Get the correction for harmonic strength of order @a k 
      @param k The harminic strenght order to get the correction for
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k) const;
  /** Get the correction for harmonic strength of order @a k 
      @param e2 The square error on the correction 
      @param k  The harminic strenght order to get the correction for
      @return @f$ \langle\cos(n(\psi_n - \psi_R))\rangle@f$ */ 
  virtual Double_t Correction(UShort_t k, Double_t& e2) const;
  /** Get @f$ \chi^2/2@f$ 
      @param e2 The square error on the correction 
      @return @f$ \chi^2/2@f$ */
  virtual Double_t Chi2Over2(Double_t r, Double_t& e2) const;
  /** @} */

  /** @{ 
      @name Utilities */
  /** Draw this corrrection function 
      @param option Options passed to drawing */
  virtual void Draw(Option_t* option=""); //*MENU*
  /** Look through this object */ 
  virtual void Browse(TBrowser* b); 
  /** @} */

  /** @{ 
      @name histograms */ 
  /** @return NK histogram */
  const TH1& NKHistogram() const { return fNK; }
  /** @} */
protected:
  /** Calculate resolution 
      @param k    Order factor 
      @param y    @f$ \chi^2/2@f$
      @param echi @f$\delta\chi@f$ 
      @param dr   On return, the derivative of @f$ R(\chi)@f$
      @return 
      @f[ 
      \frac{\sqrt{\pi/2}}{2}\chi e^{-\chi^2/2}
      (I_{\frac{(k-1)}{2}}(\chi^2/2)+ I_{\frac{(k+1)}{2}}(\chi^2/2))
      @f] */
  Double_t Res(UShort_t k, Double_t y, Double_t echi2, Double_t& e2) const;
  /** Number of events with large diviation */
  ULong_t fLarge; // Number of events with large angle 
  /** Histogram of large and all angles */ 
  TH1D fNK;
  /** Define for ROOT I/O */
  ClassDef(AliFMDFlowResolutionTDR,1);
};


#endif
//
// EOF
//
