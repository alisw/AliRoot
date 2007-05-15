/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTPHOSClusterizer.h
    @author Øystein Djuvsland
    @date   
    @brief  A clusterizer component for PHOS HLT
*/


#ifndef ALIHLTPHOSCLUSTERIZERCOMPONENT_H
#define ALIHLTPHOSCLUSTERIZERCOMPONENT_H

#include "AliHLTProcessor.h"


class AliHLTPHOSClusterizer;
class Rtypes;
struct AliHLTPHOSRcuCellEnergyDataStruct;
struct AliHLTPHOSClusterDataStruct;
struct AliHLTPHOSRecPointDataStruct;
struct AliHLTPHOSRecPointListDataStruct;


class AliHLTPHOSClusterizerComponent: public AliHLTProcessor
{
 public:

  AliHLTPHOSClusterizerComponent();
  ~AliHLTPHOSClusterizerComponent();
  AliHLTPHOSClusterizerComponent(const AliHLTPHOSClusterizerComponent &);
  AliHLTPHOSClusterizerComponent & operator = (const AliHLTPHOSClusterizerComponent &)
    {
      return *this;
    }
  const char* GetComponentID();
  void GetInputDataTypes(std::vector<AliHLTComponentDataType, std::allocator<AliHLTComponentDataType> >&);

  AliHLTComponentDataType GetOutputDataType();

  void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);

  Int_t DoEvent(const AliHLTComponentEventData&, const AliHLTComponentBlockData*,
		AliHLTComponentTriggerData&, AliHLTUInt8_t*, AliHLTUInt32_t&,
		std::vector<AliHLTComponentBlockData>&);

  AliHLTComponent* Spawn();

 protected:

  Int_t DoInit(int argc, const char** argv);
  Int_t Deinit();
  Int_t DoDeinit();

 private:
  AliHLTPHOSClusterizer* fClusterizerPtr;                       //Pointer to the clusterizer
  AliHLTPHOSClusterDataStruct* fOutPtr;                         //Pointer to the struct of output clusters
  AliHLTPHOSRecPointDataStruct* fRecPointStructArrayPtr;        //Pointer to the struct of output recpoints
  AliHLTPHOSRecPointListDataStruct* fRecPointListPtr;           //Pointer to the struct of list of output recpoints
  static const AliHLTComponentDataType fgkInputDataTypes[];     //HLT input data type

};

#endif
