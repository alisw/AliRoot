#ifndef ALIITSNEURALTRACK_H
#define ALIITSNEURALTRACK_H

#include <TMatrixD.h>

class TObjArray;
class AliITSNeuralPoint;
//class AliITSVertex;
class AliITSIOTrack;

class AliITSNeuralTrack : public TObject {

public:
	         AliITSNeuralTrack();
	virtual ~AliITSNeuralTrack();
		
	// Points insertion and goodness evaluation

	void     AssignLabel();
	void     CleanSlot(Int_t i, Bool_t del = kFALSE);
	void     CleanAllSlots(Bool_t del = kFALSE)        {Int_t i; for(i=0;i<6;i++) CleanSlot(i,del);}
	void     GetModuleData(Int_t i, Int_t &mod, Int_t &pos);
	void     Insert(AliITSNeuralPoint *point);
	Bool_t   IsGood(Int_t min)                         {return (fCount >= min);}
	Int_t    OccupationMask();
	void     PrintLabels();
		
	// Fit procedures
		
	Bool_t   AddEL(Int_t layer, Double_t sign);
	Bool_t   AddMS(Int_t layer);
	void     ForceSign(Double_t sign)            { fC *= sign; }  // externally imposed trach charge
	void     ResetChi2()                         { fChi2 = fNSteps = 0.0; }
	Bool_t   SeedCovariance();
	Int_t    PropagateTo(Double_t r);
	Bool_t   Filter(AliITSNeuralPoint *test);
	Bool_t   KalmanFit();
	Bool_t   RiemannFit();
	void     PrintState(Bool_t matrix);

	// Getters

	Int_t    GetLabel()        {return fLabel;}
	Int_t    GetCount()        {return fCount;}
	Double_t GetDt()           {return fDt;}
	Double_t GetDz();          
	Double_t GetC()            {return fC;}
	Double_t GetR()            {return fR;}
	Double_t GetXC()           {return fXC;}
	Double_t GetYC()           {return fYC;}
	Double_t GetTanL()         {return fTanL;}
	Double_t GetGamma();       
	Double_t GetChi2()         {return fChi2;}
	Double_t GetStateR()       {return fStateR;}
	Double_t GetStatePhi()     {return fStatePhi;}
	Double_t GetStateZ()       {return fStateZ;}
	Double_t GetCovElement(Int_t i, Int_t j) {return fMatrix(i,j);}
	Double_t GetPhi(Double_t r);        // phi = gamma0 + asin(argphi(rho))
	Double_t GetZ(Double_t r);          // z = dz + (tanl / C) * asin(argz(rho))
	

	Double_t GetP()       {return GetPt() * (1.0 + fTanL * fTanL);}
	Double_t GetPt()      {return 0.299792658 * 0.2 * fField * fabs(1./fC/100.);}
	Double_t GetPz()      {return GetPt() * fTanL;}
	Double_t GetE()       {return sqrt(fMass*fMass + GetPt()*GetPt());}
	Double_t GetLambda()  {return atan(fTanL);}
	Int_t    GetPDGcode() {return fPDG;}
	Double_t GetdEdX();

	// Setters

	void     SetFieldFactor(Double_t fact) {fField=fact;}
	void     SetMass(Double_t mass)        {fMass=mass;}
	void     SetPDGcode(Int_t code)        {fPDG=code;}
	void     SetVertex(Double_t *pos, Double_t *err);
	/*
	void     SetRho(Double_t a)  {fStateR=a;}
	void     SetPhi(Double_t a)  {fStatePhi=a;}
	void     SetZ(Double_t a)    {fStateZ=a;}
	void     SetDt(Double_t a)   {fDt=a;}
	void     SetTanL(Double_t a) {fTanL=a;}
	void     SetC(Double_t a)    {fC=a;}
	void     SetChi2(Double_t a) {fChi2=a;}
	void     SetGamma(Double_t a){fG0=a;}
	void     SetDz(Double_t a)   {fDz=a;}
	void     SetCovElement(Int_t i, Int_t j, Double_t a) {fMatrix(i,j)=a; if(i!=j) fMatrix(j,i)=a;}
	*/
	AliITSIOTrack* ExportIOtrack(Int_t min);

private:
	
	Double_t ArgPhi(Double_t r) const;
	Double_t ArgZ  (Double_t r) const;
	Double_t ArgB  (Double_t r) const;
	Double_t ArgC  (Double_t r) const;

	Double_t fXC;       // X ofcurvature center
	Double_t fYC;       // Y of curvature center
	Double_t fR;        // curvature radius
	Double_t fC;        // semi-curvature of the projected circle (signed)
	Double_t fTanL;     // tangent of dip angle
	Double_t fG0;       // phase coefficient
	Double_t fDt;       // transverse impact parameter
	Double_t fDz;       // longitudinal impact parameter

	Double_t fStateR;   //
	Double_t fStatePhi; // state vector coordinates
	Double_t fStateZ;   //
	TMatrixD fMatrix;   // covariance matrix
	Double_t fChi2;     // square chi (calculated by Kalman filter)
	Double_t fNSteps;   // number of Kalman steps

	Double_t fMass;     // the particle mass
	Double_t fField;    // B field = 0.2 * fField (Tesla)
		
	Int_t    fPDG;      // PDG code of the recognized particle
	Int_t    fLabel;    // the GEANT label most appearing among track recpoints
	Int_t    fCount;    // number of counts of above label
		
	AliITSNeuralPoint  fVertex;   // vertex position data
	AliITSNeuralPoint *fPoint[6]; // track points

	ClassDef(AliITSNeuralTrack, 1)
};

#endif
