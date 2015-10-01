#ifndef ALIHLTZMQSINK_H
#define ALIHLTZMQSINK_H

/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/** @file    AliHLTZMQsink.h
    @author  Mikolaj Krzewicki
    @brief   ZeroMQ sink
*/

#include "AliHLTDataSink.h"
#include <map>
#include <string>

class AliHLTZMQsink : public AliHLTDataSink {
public:
  
  typedef map<std::string,std::string> stringMap;

  AliHLTZMQsink();
  virtual ~AliHLTZMQsink();

  //interface functions
  const Char_t* GetComponentID();
  void GetInputDataTypes( vector<AliHLTComponentDataType>& list);
  AliHLTComponent* Spawn();

  //new option parser
  static stringMap* TokenizeOptionString(const TString str);
  int ProcessOptionString(TString arguments);
  int ProcessOption(TString option, TString value);

protected:
  //interface functions
  Int_t DoInit( Int_t /*argc*/, const Char_t** /*argv*/ );
  Int_t DoDeinit();
  virtual int DumpEvent( const AliHLTComponentEventData& evtData,
                         const AliHLTComponentBlockData* blocks, 
                         AliHLTComponentTriggerData& trigData );
  
private:

  AliHLTZMQsink(const AliHLTZMQsink&);
  AliHLTZMQsink& operator=(const AliHLTZMQsink&);

  void* fZMQcontext;       //ZMQ context pointer
  void* fZMQout;           //the output socket
  int fZMQsocketType;      //REP
  TString fZMQconnectMode; //"connect" or "bind"
  TString fZMQendpoint;    //e.g. "tcp://*:60100" "tcp://ecs0:60100"
  Bool_t fZMQpollIn;       //do we poll fo incoming requests?
  Bool_t fZMQsendAllInOne; //send all blocks in one multi part message
  
  
  ClassDef(AliHLTZMQsink, 1)
};
#endif
