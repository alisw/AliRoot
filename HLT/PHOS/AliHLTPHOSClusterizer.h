/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors: Oystein Djuvsland                                     *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          * 
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#ifndef ALIHLTPHOSCLUSTERIZER_H
#define ALIHLTPHOSCLUSTERIZER_H


/**
 * Class does clusterization in for PHOS on an event basis. It is intended 
 * for use in HLT, but can also be used offline
 *
 * @file   AliHLTPHOSClusterizer.h
 * @author Oystein Djuvsland
 * @date
 * @brief  Clusterizer for PHOS HLT
 */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliHLTPHOSBase.h"
#include "AliPHOSLoader.h"

#include "AliHLTPHOSRecPointContainerStruct.h"
#include "AliHLTPHOSRecPointDataStruct.h"
#include "AliHLTPHOSDigitContainerDataStruct.h"
#include "AliHLTPHOSDigitDataStruct.h"

#include "AliPHOSGeometry.h"

class TClonesArray;
class AliPHOSDigit;
class AliPHOSRecoParamEmc;
class AliPHOSRecoParam;

/** 
 * @class AliHLTPHOSClusterizer
 * Clusterizer for PHOS HLT. The clusterizer takes digits as input, either
 * in the form of a container of AliHLTPHOSDigitDataStruct or a
 * TClonesArray of AliPHOSDigit through an instance of a AliPHOSLoader
 *
 * @ingroup alihlt_phos
 */
class AliHLTPHOSClusterizer : public AliHLTPHOSBase
{
  
public:
  
  /** Constructor */
  AliHLTPHOSClusterizer();    
  
  /** Destructor */
  virtual ~AliHLTPHOSClusterizer();

  /** Copy constructor */  
 AliHLTPHOSClusterizer(const AliHLTPHOSClusterizer &) :  AliHLTPHOSBase(),
    fEmcClusteringThreshold(0),
    fEmcMinEnergyThreshold(0),
    fEmcTimeGate(0),
    fLogWeight(0),
    fDigitsInCluster(0),
    fOnlineMode(true),
    fDigitArrayPtr(0),
    fEmcRecPointsPtr(0),
    fDigitPtr(0),
    fDigitContainerPtr(0),
    fRecPointContainerPtr(0),
    fPHOSGeometry(0),
    fGetterPtr(0),
      fMaxDigitIndexDiff(0)
  {
    //Copy constructor not implemented
  }
  
  /** Assignment */
  AliHLTPHOSClusterizer & operator = (const AliHLTPHOSClusterizer)
  {
    //Assignment
    return *this; 
  }
  
  /** Set digit container */
  void SetDigitContainer(AliHLTPHOSDigitContainerDataStruct* digitContainerPtr)
  { fDigitContainerPtr = digitContainerPtr; }

  /** Set rec point container */
  void SetRecPointContainer(AliHLTPHOSRecPointContainerStruct *recPointContainerPtr);


  /** Set reco parameters */
  void SetRecoParameters(AliPHOSRecoParam* recoPars);

  /** Set emc clustering threshold */
  void SetEmcClusteringThreshold(Float_t threshold) { fEmcClusteringThreshold = threshold; }

  /** Set emc min energy threshold */
  void SetEmcMinEnergyThreshold(Float_t threshold) { fEmcMinEnergyThreshold = threshold; }

  /** Set emc time gate */
  void SetEmcTimeGate(Float_t gate) { fEmcTimeGate = gate; }

  /** Set log weight */
  void SetLogWeight(Float_t weight) { fLogWeight = weight; }  
    
  /** 
   * Set offline mode
   * @param getter pointer to an instance of AliPHOSGetter
   */
  //  void SetOfflineMode(AliPHOSLoader* getter); 
  
  /** Starts clusterization of the event */ 
  virtual Int_t ClusterizeEvent();

  /** 
   * Gets an event, for offline mode
   * @param evtNr event number to get
   */
  //virtual Int_t GetEvent(Int_t evtNr);
  
  /** Get number of events */
  //Int_t GetNEvents();

  /**
   * For a given digit this digit scans for neighbouring digits which 
   * passes the threshold for inclusion in a rec point. If one is found 
   * it is added to the current rec point
   * @param digIndex index of the digit in the digit container
   * @param recPoint pointer to the current rec point
   */
  virtual void ScanForNeighbourDigits(Int_t digIndex, AliHLTPHOSRecPointDataStruct* recPoint);

  /**
   * Checks if two digits are neighbours
   * @param d1 first digit
   * @param d2 second digit
   */
  virtual Int_t AreNeighbours(AliHLTPHOSDigitDataStruct* d1, AliHLTPHOSDigitDataStruct* d2);


protected:
  /** Energy threshold for starting a cluster for the calorimeter */
  Float_t fEmcClusteringThreshold;                             //COMMENT

  /** Energy threshold for including a crystal in a cluster */
  Float_t fEmcMinEnergyThreshold;                              //COMMENT

  /** Maximum time difference for inclusion in a rec point */
  Float_t fEmcTimeGate;                                        //COMMENT

  /** Variable used in calculation of the center of gravity for a rec point */
  Float_t fLogWeight;                                          //COMMENT

  /** Counts the digits in a rec point */
  Int_t fDigitsInCluster;                                      //COMMENT

  /** Online mode flag */
  Bool_t fOnlineMode;                                          //COMMENT
  
  /** Array of digits from one event, used in offline mode */
  TClonesArray *fDigitArrayPtr;                                //! transient

  /** Array of reconstruction points from one event, used in offline mode */ 
  TObjArray *fEmcRecPointsPtr;                                 //! transient

  /** Pointer to digit */
  AliPHOSDigit *fDigitPtr;                                     //! transient

  /** Contains the digits from one event */
  AliHLTPHOSDigitContainerDataStruct *fDigitContainerPtr;      //! transient

  /** Contains the reconstruction points from one event */
  AliHLTPHOSRecPointContainerStruct *fRecPointContainerPtr;    //! transient

  /** Instance of the PHOS geometry class */
  AliPHOSGeometry *fPHOSGeometry;                              //! transient
  
  /** Instance of the PHOS getter, used in offline mode */    
  AliPHOSLoader *fGetterPtr;                                   //! transient

  /** Maximum difference in index to be a neighbour */
  Int_t fMaxDigitIndexDiff;                                   //COMMENT

  ClassDef(AliHLTPHOSClusterizer, 1);
};

#endif
