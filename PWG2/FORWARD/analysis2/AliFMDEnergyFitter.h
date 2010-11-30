#ifndef ALIROOT_PWG2_FORWARD_ALIFMDENERGYFITTER_H
#define ALIROOT_PWG2_FORWARD_ALIFMDENERGYFITTER_H
#include <TNamed.h>
#include <TH1D.h>
#include <TAxis.h>
#include <TList.h>
#include <TObjArray.h>
#include "AliForwardUtil.h"
class AliESDFMD;
class TFitResult;
class TF1;
class TArrayD;

/**
 * Class to fit the energy distribution.  
 *
 * @par Input: 
 *    - AliESDFMD object  - from reconstruction
 *
 * @par Output: 
 *    - Lists of histogram - one per ring.  Each list has a number of 
 *      histograms corresponding to the number of eta bins defined.  
 *
 * @par Corrections used: 
 *    - None
 *
 *
 * @ingroup pwg2_forward_analysis 
 */
class AliFMDEnergyFitter : public TNamed
{
public: 
    enum { 
      kC	= AliForwardUtil::ELossFitter::kC,
      kDelta	= AliForwardUtil::ELossFitter::kDelta, 
      kXi	= AliForwardUtil::ELossFitter::kXi, 
      kSigma	= AliForwardUtil::ELossFitter::kSigma, 
      kSigmaN	= AliForwardUtil::ELossFitter::kSigmaN,
      kN	= AliForwardUtil::ELossFitter::kN, 
      kA	= AliForwardUtil::ELossFitter::kA
    };

  /** 
   * Destructor
   */
  virtual ~AliFMDEnergyFitter();
  /** 
   * Default Constructor - do not use 
   */
  AliFMDEnergyFitter();
  /** 
   * Constructor 
   * 
   * @param title Title of object  - not significant 
   */
  AliFMDEnergyFitter(const char* title);
  /** 
   * Copy constructor 
   * 
   * @param o Object to copy from 
   */
  AliFMDEnergyFitter(const AliFMDEnergyFitter& o);
  /** 
   * Assignment operator 
   * 
   * @param o Object to assign from 
   * 
   * @return Reference to this 
   */
  AliFMDEnergyFitter& operator=(const AliFMDEnergyFitter& o);

  /** 
   * Initialise the task
   * 
   * @param etaAxis The eta axis to use.  Note, that if the eta axis
   * has already been set (using SetEtaAxis), then this parameter will be 
   * ignored
   */
  void Init(const TAxis& etaAxis);
  /** 
   * Set the eta axis to use.  This will force the code to use this
   * eta axis definition - irrespective of whatever axis is passed to
   * the Init member function.  Therefore, this member function can be
   * used to force another eta axis than one found in the correction
   * objects. 
   * 
   * @param nBins  Number of bins 
   * @param etaMin Minimum of the eta axis 
   * @param etaMax Maximum of the eta axis 
   */
  void SetEtaAxis(Int_t nBins, Double_t etaMin, Double_t etaMax);
  /** 
   * Set the eta axis to use.  This will force the code to use this
   * eta axis definition - irrespective of whatever axis is passed to
   * the Init member function.  Therefore, this member function can be
   * used to force another eta axis than one found in the correction
   * objects. 
   * 
   * @param etaAxis Eta axis to use 
   */
  void SetEtaAxis(const TAxis& etaAxis);
  /** 
   * Set the low cut used for energy 
   * 
   * @param lowCut Low cut
   */
  void SetLowCut(Double_t lowCut=0.3) { fLowCut = lowCut; }
  /** 
   * Set the number of bins to subtract 
   * 
   * @param n 
   */
  void SetFitRangeBinWidth(UShort_t n=4) { fFitRangeBinWidth = n; }
  /** 
   * Whether or not to enable fitting of the final merged result.  
   * Note, fitting takes quite a while and one should be careful not to do 
   * this needlessly 
   * 
   * @param doFit Whether to do the fits or not 
   */
  void SetDoFits(Bool_t doFit=kTRUE) { fDoFits = doFit; }
  /** 
   * Set how many particles we will try to fit at most to the data
   * 
   * @param n Max number of particle to try to fit 
   */
  void SetNParticles(UShort_t n) { fNParticles = (n < 1 ? 1 : (n > 5 ? 5 : n)); }
  /** 
   * Set the minimum number of entries each histogram must have 
   * before we try to fit our response function to it
   * 
   * @param n Minimum number of entries
   */
  void SetMinEntries(UShort_t n) { fMinEntries = (n < 1 ? 1 : n); }
  /**
   * Set maximum energy loss to consider 
   *
   * @param x Maximum energy loss to consider 
   */
  void SetMaxE(Double_t x) { fMaxE = x; }
  /**
   * Set number of energy loss bins 
   *
   * @param x Number of energy loss bins 
   */
  void SetNEbins(Int_t x) { fNEbins = x; }
  void SetMaxRelativeParameterError(Double_t e) { fMaxRelParError = e; }
  void SetMaxChi2PerNDF(Double_t c) { fMaxChi2PerNDF = c; }
  /**
   * Set wheter to use increasing bin sizes 
   *
   * @param x Wheter to use increasing bin sizes 
   */
  void SetUseIncreasingBins(Bool_t x) { fUseIncreasingBins = x; }
  /** 
   * Fitter the input AliESDFMD object
   * 
   * @param input     Input 
   * @param empty     Whether the event is 'empty'
   * 
   * @return True on success, false otherwise 
   */
  Bool_t Accumulate(const AliESDFMD& input, 
		    Bool_t           empty);
  /** 
   * Scale the histograms to the total number of events 
   * 
   * @param dir Where the histograms are  
   */
  void Fit(TList* dir);
  
  /** 
   * Define the output histograms.  These are put in a sub list of the
   * passed list.   The histograms are merged before the parent task calls 
   * AliAnalysisTaskSE::Terminate 
   * 
   * @param dir Directory to add to 
   */
  void DefineOutput(TList* dir);
  /** 
   * Set the debug level.  The higher the value the more output 
   * 
   * @param dbg Debug level 
   */
  void SetDebug(Int_t dbg=1);
protected:
  /** 
   * Internal data structure to keep track of the histograms
   */
  struct RingHistos : public AliForwardUtil::RingHistos
  { 
    /** 
     * Default CTOR
     */
    RingHistos();
    /** 
     * Constructor
     * 
     * @param d detector
     * @param r ring 
     */
    RingHistos(UShort_t d, Char_t r);
    /** 
     * Copy constructor 
     * 
     * @param o Object to copy from 
     */
    RingHistos(const RingHistos& o);
    /** 
     * Assignment operator 
     * 
     * @param o Object to assign from 
     * 
     * @return Reference to this 
     */
    RingHistos& operator=(const RingHistos& o);
    /** 
     * Destructor 
     */
    ~RingHistos();
    /** 
     * Define outputs
     * 
     * @param dir 
     */
    void Output(TList* dir);
    /** 
     * Initialise object 
     * 
     * @param eAxis      Eta axis
     * @param maxDE      Max energy loss to consider 
     * @param nDEbins    Number of bins 
     * @param useIncrBin Whether to use an increasing bin size 
     */
    void Init(const TAxis& eAxis, 
	      Double_t     maxDE=10, 
	      Int_t        nDEbins=300, 
	      Bool_t       useIncrBin=true);
    /** 
     * Fill histogram 
     * 
     * @param empty  True if event is empty
     * @param ieta   Eta bin
     * @param mult   Signal 
     */
    void Fill(Bool_t empty, Int_t ieta, Double_t mult);
    /** 
     * Fit each histogram to up to @a nParticles particle responses.
     * 
     * @param dir         Output list 
     * @param eta         Eta axis 
     * @param lowCut      Lower cut 
     * @param nParticles  Max number of convolved landaus to fit
     * @param minEntries  Minimum number of entries 
     * @param minusBins   Number of bins from peak to subtract to 
     *                    get the fit range 
     * @param relErrorCut Cut applied to relative error of parameter. 
     *                    Note, for multi-particle weights, the cut 
     *                    is loosend by a factor of 2 
     * @param chi2nuCut   Cut on @f$ \chi^2/\nu@f$ - 
     *                    the reduced @f$\chi^2@f$ 
     */
    TObjArray* Fit(TList* dir, 
		   const TAxis& eta,
		   Double_t     lowCut, 
		   UShort_t     nParticles,
		   UShort_t     minEntries,
		   UShort_t     minusBins,
		   Double_t     relErrorCut, 
		   Double_t     chi2nuCut) const;
    /** 
     * Fit a signal histogram.  First, the bin @f% b_{min}@f$ with
     * maximum bin content in the range @f$ [E_{min},\infty]@f$ is
     * found.  Then the fit range is set to the bin range 
     * @f$ [b_{min}-\Delta b,b_{min}+2\Delta b]@f$, and a 1 
     * particle signal is fitted to that.  The parameters of that fit 
     * is then used as seeds for a fit of the @f$ N@f$ particle response 
     * to the data in the range 
     * @f$ [b_{min}-\Delta b,N(\Delta_1+\xi_1\log(N))+2N\xi@f$
     * 
     * @param dist        Histogram to fit 
     * @param lowCut      Lower cut @f$ E_{min}@f$ on signal 
     * @param nParticles  Max number @f$ N@f$ of convolved landaus to fit
     * @param minusBins   Number of bins @f$ \Delta b@f$ from peak to 
     *                    subtract to get the fit range 
     * @param relErrorCut Cut applied to relative error of parameter. 
     *                    Note, for multi-particle weights, the cut 
     *                    is loosend by a factor of 2 
     * @param chi2nuCut   Cut on @f$ \chi^2/\nu@f$ - 
     *                    the reduced @f$\chi^2@f$ 
     * 
     * @return The best fit function 
     */
    TF1* FitHist(TH1*     dist,
		 Double_t lowCut, 
		 UShort_t nParticles,
		 UShort_t minusBins,
		 Double_t relErrorCut, 
		 Double_t chi2nuCut) const;
    /** 
     * Check the result of the fit. Returns true if 
     * - @f$ \chi^2/\nu < \max{\chi^2/\nu}@f$
     * - @f$ \Delta p_i/p_i < \delta_e@f$ for all parameters.  Note, 
     *   for multi-particle fits, this requirement is relaxed by a 
     *   factor of 2
     * - @f$ a_{n} > 10^{-7}@f$ when fitting to an @f$ n@f$ 
     *   particle response 
     * 
     * @param r           Result to check
     * @param relErrorCut Cut @f$ \delta_e@f$ applied to relative error 
     *                    of parameter.  
     * @param chi2nuCut   Cut @f$ \max{\chi^2/\nu}@f$ 
     * 
     * @return true if fit is good. 
     */
    Bool_t CheckResult(TFitResult* r,
		       Double_t    relErrorCut, 
		       Double_t    chi2nuCut) const;
    /** 
     * Make an axis with increasing bins 
     * 
     * @param n    Number of bins 
     * @param min  Minimum 
     * @param max  Maximum
     * 
     * @return An axis with quadratically increasing bin size 
     */
    TArrayD MakeIncreasingAxis(Int_t n, Double_t min, Double_t max) const;
    /** 
     * Make E/E_mip histogram 
     * 
     * @param ieta   Eta bin
     * @param eMin   Least signal
     * @param eMax   Largest signal 
     */
    void Make(Int_t ieta, Double_t eMin, Double_t eMax, 
	      Double_t deMax=12, Int_t nDeBins=300, Bool_t incr=true);
    /** 
     * Make a parameter histogram
     * 
     * @param name   Name of histogram.
     * @param title  Title of histogram. 
     * @param eta    Eta axis 
     * 
     * @return 
     */
    TH1D* MakePar(const char* name, const char* title, const TAxis& eta) const;
    /** 
     * Make a histogram that contains the results of the fit over the full ring 
     * 
     * @param name  Name 
     * @param title Title
     * @param eta   Eta axis 
     * @param low   Least bin
     * @param high  Largest bin
     * @param val   Value of parameter 
     * @param err   Error on parameter 
     * 
     * @return The newly allocated histogram 
     */
    TH1D* MakeTotal(const char* name, 
		    const char* title, 
		    const TAxis& eta, 
		    Int_t low, 
		    Int_t high, 
		    Double_t val, 
		    Double_t err) const;
    TH1D*     fEDist;        // Ring energy distribution 
    TH1D*     fEmpty;        // Ring energy distribution for empty events
    TList     fEtaEDists;    // Energy distributions per eta bin. 
    TList*    fList;
    Int_t     fDebug;
    ClassDef(RingHistos,1);
  };
  /** 
   * Get the ring histogram container 
   * 
   * @param d Detector
   * @param r Ring 
   * 
   * @return Ring histogram container 
   */
  RingHistos* GetRingHistos(UShort_t d, Char_t r) const;

  TList    fRingHistos;    // List of histogram containers
  Double_t fLowCut;        // Low cut on energy
  UShort_t fNParticles;    // Number of landaus to try to fit 
  UShort_t fMinEntries;    // Minimum number of entries
  UShort_t fFitRangeBinWidth;// Number of bins to subtract from found max
  Bool_t   fDoFits;        // Wheter to actually do the fits 
  TAxis    fEtaAxis;       // Eta axis 
  Double_t fMaxE;          // Maximum energy loss to consider 
  Int_t    fNEbins;        // Number of energy loss bins 
  Bool_t   fUseIncreasingBins; // Wheter to use increasing bin sizes 
  Double_t fMaxRelParError;// Relative error cut
  Double_t fMaxChi2PerNDF; // chi^2/nu cit
  Int_t    fDebug;         // Debug level 

  ClassDef(AliFMDEnergyFitter,1); //
};

#endif
// Local Variables:
//  mode: C++ 
// End:
