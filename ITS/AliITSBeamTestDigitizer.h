#ifndef ALIITSBEAMTESTDIGITIZER_H
#define ALIITSBEAMTESTDIGITIZER_H

////////////////////////////////////////////////////
//  Class to manage the                           //
//  ITS beam test conversion from rawdata         //
//  to digits. It executes the digitization for   //
//  SPD, SDD and SSD.                             //
//  Origin:  E. Crescio crescio@to.infn.it        //
//           J. Conrad  Jan.Conrad@cern.ch        //
//                                                //
//  The choice of the beam test period is by      //
//  deafult Nov04 (Integrated ITS beam test of    //
//  November 2004). To choose the SDD beam test   //
//  of August 2004 call the constructor:          //
//  AliITSBeamTestDigitizer("name","title","Aug04"//
//                                                //
////////////////////////////////////////////////////
#include <TTask.h>


class AliITSBeamTestDigSDD;
class AliRawReaderDate;
class AliRunLoader;
class AliITSLoader;
class AliITS;
class AliITSEventHeader;
class AliRawDataHeader;


class AliITSBeamTestDigitizer : public TTask {
 
 public:
 
  AliITSBeamTestDigitizer(); 
  AliITSBeamTestDigitizer(const Text_t* name,const Text_t* title, 
			  Char_t* opt="Nov04");
  AliITSBeamTestDigitizer(const Text_t* name,const Text_t* title, 
			  Int_t run,Char_t* opt="Nov04");
  AliITSBeamTestDigitizer(const char* filename);
  AliITSBeamTestDigitizer(const AliITSBeamTestDigitizer& bt);
  AliITSBeamTestDigitizer& operator=(const AliITSBeamTestDigitizer &source);

  virtual ~AliITSBeamTestDigitizer();

  void SetDigitsFileName(const TString& name)     {fDigitsFileName=name;}
  void SetRawdataFileName(const TString& name) {fRawdataFileName=name;}
  void SetNumberOfEventsPerFile(Int_t nev); 

  void SetStartEventNumber(Int_t evin) {fEvIn=evin;}
  void SetStopEventNumber(Int_t evfin) {fEvFin=evfin;}
  void SetFlagHeader(Bool_t flag=kFALSE){fFlagHeader=flag;}
  void SetFlagInit(Bool_t flag=kFALSE){fFlagInit=flag;}
  void SelectEvents(Int_t eventtype) {fDATEEvType=eventtype;}
  void SetBeamTestPeriod(BeamtestPeriod_t per=kNov04) {fPeriod=per;}
  void SetRunNumber(Int_t run) {fRunNumber=run;}

  void SetBeamTestGeometry(AliITS* bt){fBt=bt;}

  void SetActive(const TString& subdet,Bool_t value);

  void ExecDigitization();
  void Init();
 
  Int_t GetRunNumber()  const {return fRunNumber;}
  Bool_t GetFlagInit()  const {return fFlagInit;}
  BeamtestPeriod_t GetBeamTestPeriod() const {return fPeriod;}
  AliITS* GetBeamTestGeometry() const {return fBt;}

 protected:


  Int_t    fEvIn;                   //starting event to be read
  Int_t    fEvFin;                  //last event to be read
  Int_t    fRunNumber;              //run number 
  Int_t    fDATEEvType;             //date eventtype (JC) 

  Bool_t   fFlagHeader;             //flag for the hader 
  Bool_t   fFlagInit;               //flag for initialization

  TString  fDigitsFileName;         //Digits file name
  TString  fRawdataFileName;        //Raw data file name
  BeamtestPeriod_t  fPeriod;                 //Beam test period

  AliITS*     fBt;                  //! Local pointer to ITS geometry.
  
  AliRunLoader* fRunLoader;         // Local pointer to run loader
  AliITSLoader* fLoader;            // Pointer to ITS loader

  const AliRawDataHeader* fHeader;  //!current data header

  static const TString fgkDefaultDigitsFileName; // default name for dig. file  

  ClassDef(AliITSBeamTestDigitizer,1)  // An Alice SDD beam test digitization class 

  };


#endif

    
