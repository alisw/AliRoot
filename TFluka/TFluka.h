#ifndef TFLUKA_H
#define TFLUKA_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
// FLUKA implementation of the VirtualMC Interface                           //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "TVirtualMC.h"

//Forward declaration
class TGeoMCGeometry;
class TFlukaMCGeometry;
class TGeoMaterial;

class TFluka : public TVirtualMC {
  
 public:
  TFluka(const char *title, Int_t verbosity = 0,  Bool_t isRootGeometrySupported = 0);
  TFluka();
  virtual ~TFluka();
  
  //
  // Methods for building/management of geometry
  // ------------------------------------------------
  //
  
  // Functions from GCONS 
  virtual void   Gfmate(Int_t imat, char *name, Float_t &a, Float_t &z,  
			Float_t &dens, Float_t &radl, Float_t &absl,
			Float_t* ubuf, Int_t& nbuf);
  virtual void   Gfmate(Int_t imat, char *name, Double_t &a, Double_t &z,
			Double_t &dens, Double_t &radl, Double_t &absl,
			Double_t* ubuf, Int_t& nbuf);
  
  // Detector composition
  virtual void   Material(Int_t& kmat, const char* name, Double_t a, 
			  Double_t z, Double_t dens, Double_t radl, Double_t absl,
			  Float_t* buf, Int_t nwbuf);
  virtual void   Material(Int_t& kmat, const char* name, Double_t a, 
			  Double_t z, Double_t dens, Double_t radl, Double_t absl,
			  Double_t* buf, Int_t nwbuf);
  virtual void   Mixture(Int_t& kmat, const char *name, Float_t *a, 
			 Float_t *z, Double_t dens, Int_t nlmat, Float_t *wmat);
  virtual void   Mixture(Int_t& kmat, const char *name, Double_t *a, 
			 Double_t *z, Double_t dens, Int_t nlmat, Double_t *wmat);
  virtual void   Medium(Int_t& kmed, const char *name, Int_t nmat, 
			Int_t isvol, Int_t ifield, Double_t fieldm, Double_t tmaxfd, 
			Double_t stemax, Double_t deemax, Double_t epsil, 
			Double_t stmin, Float_t* ubuf, Int_t nbuf);
  virtual void   Medium(Int_t& kmed, const char *name, Int_t nmat, 
			Int_t isvol, Int_t ifield, Double_t fieldm, Double_t tmaxfd, 
			Double_t stemax, Double_t deemax, Double_t epsil, 
			Double_t stmin, Double_t* ubuf, Int_t nbuf);
  virtual void   Matrix(Int_t& krot, Double_t thetaX, Double_t phiX, 
			Double_t thetaY, Double_t phiY, Double_t thetaZ, 
			Double_t phiZ);
  virtual void   Gstpar(Int_t itmed, const char *param, Double_t parval);
  
  // Functions from GGEOM 
  virtual Int_t  Gsvolu(const char *name, const char *shape, Int_t nmed,  
			Float_t *upar, Int_t np);
  virtual Int_t  Gsvolu(const char *name, const char *shape, Int_t nmed,  
			Double_t *upar, Int_t np);
  virtual void   Gsdvn(const char *name, const char *mother, Int_t ndiv, 
		       Int_t iaxis);
  virtual void   Gsdvn2(const char *name, const char *mother, Int_t ndiv, 
			Int_t iaxis, Double_t c0i, Int_t numed);
  virtual void   Gsdvt(const char *name, const char *mother, Double_t step, 
		       Int_t iaxis, Int_t numed, Int_t ndvmx);
  virtual void   Gsdvt2(const char *name, const char *mother, Double_t step, 
		       Int_t iaxis, Double_t c0, Int_t numed, Int_t ndvmx);
  virtual void   Gsord(const char *name, Int_t iax);
  virtual void   Gspos(const char *name, Int_t nr, const char *mother,  
		       Double_t x, Double_t y, Double_t z, Int_t irot, 
		      const char *konly="ONLY");
  virtual void   Gsposp(const char *name, Int_t nr, const char *mother,  
			Double_t x, Double_t y, Double_t z, Int_t irot,
			const char *konly, Float_t *upar, Int_t np);
  virtual void   Gsposp(const char *name, Int_t nr, const char *mother,  
			Double_t x, Double_t y, Double_t z, Int_t irot,
			const char *konly, Double_t *upar, Int_t np);
  virtual void   Gsbool(const char* onlyVolName, const char* manyVolName);
  
  virtual void   SetCerenkov(Int_t itmed, Int_t npckov, Float_t *ppckov,
			     Float_t *absco, Float_t *effic, Float_t *rindex);
  virtual void   SetCerenkov(Int_t itmed, Int_t npckov, Double_t *ppckov,
			     Double_t *absco, Double_t *effic, Double_t *rindex);
  virtual void   SetCerenkov(Int_t itmed, Int_t npckov, Float_t *ppckov,
			     Float_t *absco, Float_t *effic, Float_t *rindex, Float_t *rfl);
  virtual void   SetCerenkov(Int_t itmed, Int_t npckov, Double_t *ppckov,
			     Double_t *absco, Double_t *effic, Double_t *rindex, Double_t *rfl);
  
  
  // Functions for drawing
  virtual void   DrawOneSpec(const char* /*name*/)
      {printf("WARNING: DrawOneSpec not yet implemented !\n");}
  virtual void   Gsatt(const char* name, const char* att, Int_t val);
  virtual void   Gdraw(const char*,Double_t /*theta = 30*/, Double_t /*phi = 30*/,
		       Double_t /*psi = 0*/, Double_t /*u0 = 10*/, Double_t /*v0 = 10*/,
		       Double_t /*ul = 0.01*/, Double_t /*vl = 0.01*/)
      {printf("WARNING: Gdraw not yet implemented !\n");}
  
  // Euclid
  virtual void   WriteEuclid(const char*, const char*, Int_t, Int_t);
  
  // Getters
  virtual Int_t  VolId(const Text_t* volName) const;
  virtual const  char* VolName(Int_t id) const;
  virtual Int_t  NofVolumes() const {return fNVolumes;}
  virtual Int_t  VolId2Mate(Int_t id) const;
  //
  // Methods for physics management
  // ------------------------------------------------
  //
  
  // User configuration
  virtual Bool_t     SetProcess(const char* flagName, Int_t flagValue);
  virtual void       SetProcess(const char* flagName, Int_t flagValue, Int_t imed);
  virtual Bool_t     SetCut(const char* cutName, Double_t cutValue);
  virtual void       SetCut(const char* cutName, Double_t cutValue, Int_t imed);
  virtual TObjArray* GetListOfUserConfigs() {return fUserConfig;}
  virtual Double_t   Xsec(char*, Double_t, Int_t, Int_t);

  
  // Particle table usage         
  virtual Int_t    IdFromPDG(Int_t id) const;
  virtual Int_t    PDGFromId(Int_t pdg) const;
  virtual void     DefineParticles()
  {printf("WARNING: DefineParticles not yet implemented !\n");}     
  
  //
  // methods for step management
  // ------------------------------------------------
  //
  
  // Action methods
  virtual void   StopTrack();
  virtual void   ResetStoppingCondition() {fStopped = kFALSE;}
  virtual Bool_t GetStoppingCondition() const   {return fStopped;}

  virtual void   StopEvent() {fStopEvent = kTRUE;}   
  virtual void   StopRun()   {fStopEvent = kTRUE; fStopRun   = kTRUE;}
  virtual Bool_t EventIsStopped() const {return fStopEvent;}
  virtual void   SetStopEvent(Bool_t flag) {fStopEvent = flag;}
  
  // Set methods
  virtual void SetMaxStep (Double_t step);
  virtual void SetMaxNStep(Int_t);
  virtual void SetUserDecay(Int_t);
  
  // Get methods
  // Tracking volume(s) 
  virtual Int_t    CurrentVolID(Int_t& copyNo) const;
  virtual Int_t    CurrentVolOffID(Int_t off, Int_t& copyNo) const;
  virtual const char* CurrentVolName() const;
  virtual const char* CurrentVolOffName(Int_t off) const;
  virtual Int_t    CurrentMaterial(Float_t &a, Float_t &z, 
				   Float_t &dens, Float_t &radl, Float_t &absl) const;
  virtual Int_t    CurrentEvent() const {return fNEvent;}

  virtual void     Gmtod(Float_t* xm, Float_t* xd, Int_t iflag);
  
  virtual void     Gmtod(Double_t* xm, Double_t* xd, Int_t iflag);
  
  virtual void     Gdtom(Float_t* xd, Float_t* xm, Int_t iflag);
  
  virtual void     Gdtom(Double_t* xd, Double_t* xm, Int_t iflag);
  
  virtual Double_t MaxStep() const;
  virtual Int_t    GetMaxNStep() const
      {printf("WARNING: GetMaxNStep not yet implemented !\n"); return -1;}
  virtual Int_t    GetMedium() const;
  
  // Tracking particle 
  // dynamic properties
  virtual void     TrackPosition(TLorentzVector& position) const;
  virtual void     TrackPosition(Double_t& x, Double_t& y, Double_t& z) const;
  virtual void     TrackMomentum(TLorentzVector& momentum) const;
  virtual void     TrackMomentum(Double_t& px, Double_t& py, Double_t& pz, Double_t& e) const;
  virtual Double_t TrackStep() const;
  virtual Double_t TrackLength() const;
  virtual Double_t TrackTime() const;
  virtual Double_t Edep() const;
  // Static properties
  virtual Int_t    TrackPid() const;
  virtual Double_t TrackCharge() const;
  virtual Double_t TrackMass() const;
  virtual Double_t Etot() const;
  // Track status
  virtual Bool_t   IsNewTrack() const;
  virtual Bool_t   IsTrackInside() const;
  virtual Bool_t   IsTrackEntering() const;
  virtual Bool_t   IsTrackExiting() const;
  virtual Bool_t   IsTrackOut() const;
  virtual Bool_t   IsTrackDisappeared() const;
  virtual Bool_t   IsTrackStop() const;
  virtual Bool_t   IsTrackAlive() const;
 
  // Secondaries
  virtual Int_t    NSecondaries() const;
  virtual void     SetNCerenkov(Int_t nc) {fNCerenkov = nc;}
	  
  virtual void     GetSecondary(Int_t isec, Int_t& particleId, 
			TLorentzVector& position, TLorentzVector& momentum);
  virtual Bool_t   SecondariesAreOrdered() const {return kFALSE;}
  virtual TMCProcess ProdProcess(Int_t iproc) const ;
  virtual Int_t    StepProcesses(TArrayI & proc) const;
  //
  // Geant3 specific methods
  // !!! need to be transformed to common interface
  //
  virtual void Gdopt(const char*,const char*)
    {printf("WARNING: Gdopt not yet implemented !\n");}
  virtual void SetClipBox(const char*,Double_t=-9999,Double_t=0, Double_t=-9999,
			  Double_t=0,Double_t=-9999,Double_t=0)
    {printf("WARNING: SetClipBox not yet implemented !\n");}
  virtual void DefaultRange()
    {printf("WARNING: DefaultRange not yet implemented !\n");}
  virtual void Gdhead(Int_t, const char*, Double_t=0)
    {printf("WARNING: Gdhead not yet implemented !\n");}  
  virtual void Gdman(Double_t, Double_t, const char*)
    {printf("WARNING: Gdman not yet implemented !\n");}
  virtual void SetColors()
    {printf("WARNING: SetColors not yet implemented !\n");}
  virtual void Gtreve()
    {printf("WARNING: Gtreve not yet implemented !\n");}
  virtual void GtreveRoot()
    {printf("WARNING: GtreveRoot not yet implemented !\n");}
  virtual void Gckmat(Int_t, char*)
    {printf("WARNING: Gckmat not yet implemented !\n");}
  virtual void InitLego()
    {printf("WARNING: InitLego not yet implemented !\n");}
//
  virtual void Gfpart(Int_t pdg, char* name, Int_t& type, Float_t& mass, Float_t& charge, Float_t& tlife);
  virtual void Gspart(Int_t, const char*, Int_t, Double_t, Double_t, Double_t)
      {printf("WARNING: Gspart not yet implemented !\n");}

  //
  // Particle Properties
  // -------------------
  //
  virtual Bool_t DefineParticle(int, const char*, TMCParticleType, double, double, double) {return kFALSE;}
  virtual Bool_t DefineIon(const char*, int, int, int, double, double) {return kFALSE;}
  virtual TString  ParticleName(int pdg)      const;
  virtual Double_t ParticleMass(int pdg)      const;
  virtual Double_t ParticleCharge(int pdg)    const;
  virtual Double_t ParticleLifeTime(int pdg)  const;
  virtual TMCParticleType ParticleMCType(int) const {return (TMCParticleType) 9;}
  //
  // Control methods
  // ------------------------------------------------
  //
  
  virtual void Init();
  virtual void InitPhysics();
  virtual void FinishGeometry();
  virtual void BuildPhysics();
  virtual void ProcessEvent();
  virtual Bool_t ProcessRun(Int_t nevent);


  //
  // FLUKA Scoring specific methods
  // ------------------------------
  //
  virtual void SetUserScoring(const char* option, Int_t npr,char* outfile, Float_t* what);
  virtual void SetUserScoring(const char* option, Int_t npr,char* outfile, Float_t* what,
			      const char* det1, const char* det2, const char* det3);
  //
  // New Getter and Setters
  // ------------------------------------------------
  //
  // - Core input file name
  TString GetCoreInputFileName() const {return fCoreInputFileName;}
  void SetCoreInputFileName(const char* file = "coreFlukaVmc.inp") {fCoreInputFileName = file;}

  // - Input file name
  TString GetInputFileName() const {return fInputFileName;}
  void SetInputFileName(const char* file = "FlukaVmc.inp") {fInputFileName = file;}

  // - Verbosity level
  Int_t GetVerbosityLevel() const {return fVerbosityLevel;}
  void SetVerbosityLevel(Int_t l) {fVerbosityLevel = l;}

  // - Fluka Draw procedures identifiers
  // bxdraw = 1  inside
  // bxdraw = 11 entering
  // bxdraw = 12 exiting
  // eedraw = 2
  // endraw = 3
  // mgdraw = 4
  // sodraw = 5
  // usdraw = 6
  Int_t GetCaller() const {return fCaller;}
  void SetCaller(Int_t l) {fCaller = l;}
  
  // - Fluka Draw procedures formal parameters
  Int_t GetIcode() const {return fIcode;}
  void SetIcode(Int_t l) {fIcode = l;}
  // in the case of sodraw fIcode=0

  Int_t GetMreg() const {return fCurrentFlukaRegion;}
  void SetMreg(Int_t l);

  Int_t GetNewreg() const {return fNewReg;}
  void SetNewreg(Int_t l) {fNewReg = l;}

  Double_t GetRull() const {return fRull;}
  void SetRull(Double_t r) {fRull = r;}

  Double_t GetXsco() const {return fXsco;}
  void SetXsco(Double_t x) {fXsco = x;}

  Double_t GetYsco() const {return fYsco;}
  void SetYsco(Double_t y) {fYsco = y;}

  Double_t GetZsco() const {return fZsco;}
  void SetZsco(Double_t z) {fZsco = z;}

  void SetCurrentFlukaRegion(Int_t reg) {fCurrentFlukaRegion=reg;}
  Int_t GetCurrentFlukaRegion() const {return fCurrentFlukaRegion;}

  void   SetDummyBoundary(Int_t mode) {fDummyBoundary = mode;}
  Int_t  GetDummyBoundary() const {return fDummyBoundary;}
  Bool_t IsDummyBoundary() const {return (fDummyBoundary==0)?kFALSE:kTRUE;}
  
  void   SetGeneratePemf(Bool_t flag = kTRUE) {fGeneratePemf = flag;}
  Bool_t IsGeneratePemf() const {return fGeneratePemf;}
  
  void   EnableField(Bool_t flag=kTRUE) {fFieldFlag = flag;}
  Bool_t IsFieldEnabled() const {return fFieldFlag;}
  void SetTrackIsEntering(){fTrackIsEntering = kTRUE; fTrackIsExiting = kFALSE;}
  void SetTrackIsExiting() {fTrackIsExiting  = kTRUE; fTrackIsEntering = kFALSE;}
  void SetTrackIsInside()  {fTrackIsExiting  = kFALSE; fTrackIsEntering = kFALSE;}
  void SetTrackIsNew(Bool_t flag = kTRUE);
  
  Int_t GetMaterialIndex(Int_t idmat) const {return fMaterials[idmat];}
  TObjArray *GetFlukaMaterials();
  private:
  TFluka(const TFluka &mc): TVirtualMC(mc) {;}
  TFluka & operator=(const TFluka &) {return (*this);}

  
  Int_t   fVerbosityLevel; //Verbosity level (0 lowest - 3 highest)
  Int_t   fNEvent;         //Current event number
  TString fInputFileName;     //Name of the real input file 
  TString fCoreInputFileName; //Name of the input file 

  Int_t    fCaller; //Parameter to indicate who is the caller of the Fluka Draw
  Int_t    fIcode;  //Fluka Draw procedures formal parameter 
  Int_t    fNewReg; //Fluka Draw procedures formal parameter
  Double_t fRull;   //Fluka Draw procedures formal parameter
  Double_t fXsco;   //Fluka Draw procedures formal parameter
  Double_t fYsco;   //Fluka Draw procedures formal parameter
  Double_t fZsco;   //Fluka Draw procedures formal parameter
  Bool_t   fTrackIsEntering;  // Flag for track entering
  Bool_t   fTrackIsExiting;   // Flag for track exiting  
  Bool_t   fTrackIsNew;       // Flag for new track
  Bool_t   fFieldFlag;        // Flag for magnetic field
  Bool_t   fGeneratePemf;     // Flag for automatic .pemf generation
  Int_t    fDummyBoundary;    // Flag for crossing dummy boundaries
  Bool_t   fStopped;          // Flag for stopping 
  Bool_t   fStopEvent;        // Flag for stopped event
  Bool_t   fStopRun;          // Flag for stopped run 
  
  //
  //Geometry through TGeo
  //
  Int_t*               fMaterials;          //!Array of indices
  Int_t                fNVolumes;           //!Current number of volumes
  Int_t                fCurrentFlukaRegion; // Index of fluka region at each step
  Int_t                fNCerenkov;          // Number of cerekov photons 
  TFlukaMCGeometry    *fGeom;               // TGeo-FLUKA interface
  TGeoMCGeometry      *fMCGeo;              // Interface to TGeo builder

  // SetProcess, SetCut and user Scoring dynamic storage
  TObjArray* fUserConfig;            // List of user physics configuration 
  TObjArray* fUserScore;             // List of user scoring options
  
  ClassDef(TFluka,1)  //C++ interface to Fluka montecarlo
};

#endif //TFLUKA

