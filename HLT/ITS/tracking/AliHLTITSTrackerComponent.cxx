// @(#) $Id: AliHLTITSTrackerComponent.cxx 32659 2009-06-02 16:08:40Z sgorbuno $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************


/////////////////////////////////////////////////////
//                                                 //
// a ITS tracker processing component for the HLT  //
//                                                 //
/////////////////////////////////////////////////////

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTITSTrackerComponent.h"
#include "AliHLTArray.h"
#include "AliExternalTrackParam.h"
#include "TStopwatch.h"
#include "TMath.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "AliITStrackerHLT.h"
#include "AliHLTITSSpacePointData.h"
#include "AliHLTITSClusterDataFormat.h"
#include "AliHLTDataTypes.h"
#include "AliHLTExternalTrackParam.h"
#include "AliHLTKalmanTrack.h"



/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp( AliHLTITSTrackerComponent )
AliHLTITSTrackerComponent::AliHLTITSTrackerComponent()
    :
    fSolenoidBz( 0 ),
    fFullTime( 0 ),
    fRecoTime( 0 ),
    fNEvents( 0 ),
    fTracker(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTITSTrackerComponent::AliHLTITSTrackerComponent( const AliHLTITSTrackerComponent& )
    :
    AliHLTProcessor(),
    fSolenoidBz( 0 ),
    fFullTime( 0 ),
    fRecoTime( 0 ),
    fNEvents( 0 ),
    fTracker(0)
{
  // see header file for class documentation
  HLTFatal( "copy constructor untested" );
}

AliHLTITSTrackerComponent& AliHLTITSTrackerComponent::operator=( const AliHLTITSTrackerComponent& )
{
  // see header file for class documentation
  HLTFatal( "assignment operator untested" );
  return *this;
}

AliHLTITSTrackerComponent::~AliHLTITSTrackerComponent()
{
  // see header file for class documentation
  delete fTracker;
}

//
// Public functions to implement AliHLTComponent's interface.
// These functions are required for the registration process
//

const char* AliHLTITSTrackerComponent::GetComponentID()
{
  // see header file for class documentation
  return "ITSTracker";
}

void AliHLTITSTrackerComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list )
{
  // see header file for class documentation
  list.clear();
  list.push_back( kAliHLTDataTypeTrack|kAliHLTDataOriginTPC );
  list.push_back( kAliHLTDataTypeClusters|kAliHLTDataOriginITSSSD );
  list.push_back( kAliHLTDataTypeClusters|kAliHLTDataOriginITSSPD );
}

AliHLTComponentDataType AliHLTITSTrackerComponent::GetOutputDataType()
{
  // see header file for class documentation  
  return kAliHLTDataTypeTrack|kAliHLTDataOriginITS;
}

void AliHLTITSTrackerComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // define guess for the output data size
  constBase = 200;       // minimum size
  inputMultiplier = 0.5; // size relative to input
}

AliHLTComponent* AliHLTITSTrackerComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTITSTrackerComponent;
}

void AliHLTITSTrackerComponent::SetDefaultConfiguration()
{
  // Set default configuration for the CA tracker component
  // Some parameters can be later overwritten from the OCDB

  fSolenoidBz = 5.;
  fFullTime = 0;
  fRecoTime = 0;
  fNEvents = 0;
  fTracker = 0;
}

int AliHLTITSTrackerComponent::ReadConfigurationString(  const char* arguments )
{
  // Set configuration parameters for the CA tracker component from the string

  int iResult = 0;
  if ( !arguments ) return iResult;

  TString allArgs = arguments;
  TString argument;
  int bMissingParam = 0;

  TObjArray* pTokens = allArgs.Tokenize( " " );

  int nArgs =  pTokens ? pTokens->GetEntries() : 0;

  for ( int i = 0; i < nArgs; i++ ) {
    argument = ( ( TObjString* )pTokens->At( i ) )->GetString();
    if ( argument.IsNull() ) continue;

    if ( argument.CompareTo( "-solenoidBz" ) == 0 ) {
      if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
      fSolenoidBz = ( ( TObjString* )pTokens->At( i ) )->GetString().Atof();
      HLTInfo( "Magnetic Field set to: %f", fSolenoidBz );
      continue;
    }

    //if ( argument.CompareTo( "-minNClustersOnTrack" ) == 0 ) {
    //if ( ( bMissingParam = ( ++i >= pTokens->GetEntries() ) ) ) break;
    //fMinNTrackClusters = ( ( TObjString* )pTokens->At( i ) )->GetString().Atoi();
    //HLTInfo( "minNClustersOnTrack set to: %d", fMinNTrackClusters );
    //continue;
    //}

    HLTError( "Unknown option \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }
  delete pTokens;

  if ( bMissingParam ) {
    HLTError( "Specifier missed for parameter \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }

  return iResult;
}


int AliHLTITSTrackerComponent::ReadCDBEntry( const char* cdbEntry, const char* chainId )
{
  // see header file for class documentation

  const char* defaultNotify = "";

  if ( !cdbEntry ) {
    cdbEntry = "HLT/ConfigITS/ITSTracker";
    defaultNotify = " (default)";
    chainId = 0;
  }

  HLTInfo( "configure from entry \"%s\"%s, chain id %s", cdbEntry, defaultNotify, ( chainId != NULL && chainId[0] != 0 ) ? chainId : "<none>" );
  AliCDBEntry *pEntry = AliCDBManager::Instance()->Get( cdbEntry );//,GetRunNo());

  if ( !pEntry ) {
    HLTError( "cannot fetch object \"%s\" from CDB", cdbEntry );
    return -EINVAL;
  }

  TObjString* pString = dynamic_cast<TObjString*>( pEntry->GetObject() );

  if ( !pString ) {
    HLTError( "configuration object \"%s\" has wrong type, required TObjString", cdbEntry );
    return -EINVAL;
  }

  HLTInfo( "received configuration object string: \"%s\"", pString->GetString().Data() );

  return  ReadConfigurationString( pString->GetString().Data() );
}


int AliHLTITSTrackerComponent::Configure( const char* cdbEntry, const char* chainId, const char *commandLine )
{
  // Configure the component
  // There are few levels of configuration,
  // parameters which are set on one step can be overwritten on the next step

  //* read hard-coded values

  SetDefaultConfiguration();

  //* read the default CDB entry

  int iResult1 = ReadCDBEntry( NULL, chainId );

  //* read magnetic field

  int iResult2 = ReadCDBEntry( kAliHLTCDBSolenoidBz, chainId );

  //* read the actual CDB entry if required

  int iResult3 = ( cdbEntry ) ? ReadCDBEntry( cdbEntry, chainId ) : 0;

  //* read extra parameters from input (if they are)

  int iResult4 = 0;

  if ( commandLine && commandLine[0] != '\0' ) {
    HLTInfo( "received configuration string from HLT framework: \"%s\"", commandLine );
    iResult4 = ReadConfigurationString( commandLine );
  }

  // Initialise the tracker here

  return iResult1 ? iResult1 : ( iResult2 ? iResult2 : ( iResult3 ? iResult3 : iResult4 ) );
}



int AliHLTITSTrackerComponent::DoInit( int argc, const char** argv )
{
  // Configure the ITS tracker component

  if ( fTracker ) return EINPROGRESS;
  fTracker = new AliITStrackerHLT(0);

  TString arguments = "";
  for ( int i = 0; i < argc; i++ ) {
    if ( !arguments.IsNull() ) arguments += " ";
    arguments += argv[i];
  }

  return Configure( NULL, NULL, arguments.Data() );
}


int AliHLTITSTrackerComponent::DoDeinit()
{
  // see header file for class documentation
  delete fTracker;
  fTracker = 0;
  return 0;
}



int AliHLTITSTrackerComponent::Reconfigure( const char* cdbEntry, const char* chainId )
{
  // Reconfigure the component from OCDB .

  return Configure( cdbEntry, chainId, NULL );
}



int AliHLTITSTrackerComponent::DoEvent
(
  const AliHLTComponentEventData& evtData,
  const AliHLTComponentBlockData* blocks,
  AliHLTComponentTriggerData& /*trigData*/,
  AliHLTUInt8_t* outputPtr,
  AliHLTUInt32_t& size,
  vector<AliHLTComponentBlockData>& outputBlocks )
{
  //* process event

  AliHLTUInt32_t maxBufferSize = size;
  size = 0; // output size

  if ( GetFirstInputBlock( kAliHLTDataTypeSOR ) || GetFirstInputBlock( kAliHLTDataTypeEOR ) ) {
    return 0;
  }

  if ( evtData.fBlockCnt <= 0 ) {
    HLTWarning( "no blocks in event" );
    return 0;
  }


  TStopwatch timer;

  // Event reconstruction in ITS

  int iResult=0;

  int nBlocks = evtData.fBlockCnt;

  
  vector< AliExternalTrackParam > tracksTPC;
  vector< int > tracksTPCId;
  std::vector<AliITSRecPoint> clusters;

  int currentTrackID = 0;

  for (int ndx=0; ndx<nBlocks && iResult>=0; ndx++) {

    const AliHLTComponentBlockData* iter = blocks+ndx;
 
    // Read TPC tracks
    
    if( iter->fDataType == ( kAliHLTDataTypeTrack|kAliHLTDataOriginTPC ) ){	  
      AliHLTTracksData* dataPtr = ( AliHLTTracksData* ) iter->fPtr;
      int nTracks = dataPtr->fCount;
      AliHLTExternalTrackParam* currOutTrack = dataPtr->fTracklets;
      for( int itr=0; itr<nTracks; itr++ ){
	AliHLTKalmanTrack t(*currOutTrack);
	tracksTPC.push_back( t );
	tracksTPCId.push_back( currOutTrack->fTrackID );
	unsigned int dSize = sizeof( AliHLTExternalTrackParam ) + currOutTrack->fNPoints * sizeof( unsigned int );
	currOutTrack = ( AliHLTExternalTrackParam* )( (( Byte_t * )currOutTrack) + dSize );
      }
    }


    // Read ITS clusters

    if ( (iter->fDataType == (kAliHLTDataTypeClusters|kAliHLTDataOriginITSSSD) ) || 
	 (iter->fDataType == (kAliHLTDataTypeClusters|kAliHLTDataOriginITSSPD) ) 
	 ){

      AliHLTITSClusterData *inPtr=reinterpret_cast<AliHLTITSClusterData*>( iter->fPtr );
      int nClusters = inPtr->fSpacePointCnt;
      for( int icl=0; icl<nClusters; icl++ ){
	AliHLTITSSpacePointData &d = inPtr->fSpacePoints[icl];
	AliITSRecPoint p;
	p.SetY( d.fY );
	p.SetZ( d.fZ );
	p.SetSigmaY2( d.fSigmaY2 );
	p.SetSigmaZ2( d.fSigmaZ2 );
	p.SetSigmaYZ( d.fSigmaYZ );
	p.SetQ( d.fQ );
	p.SetNy( d.fNy );
	p.SetNz( d.fNz );
	p.SetLayer( d.fLayer );
	p.SetDetectorIndex( d.fIndex );
	p.SetLabel(0, d.fTracks[0] );
	p.SetLabel(1, d.fTracks[1] );
	p.SetLabel(2, d.fTracks[2] );
	clusters.push_back( p );
      }   
    }
    
  }// end read input blocks
  
  // set clusters to tracker

  fTracker->LoadClusters( clusters );

  // Reconstruct the event

  TStopwatch timerReco;
  
  fTracker->Reconstruct( tracksTPC );

  timerReco.Stop();
  
  // Fill output tracks

  {
    unsigned int mySize = 0;    
     
    AliHLTTracksData* outPtr = ( AliHLTTracksData* )( outputPtr );

    AliHLTExternalTrackParam* currOutTrack = outPtr->fTracklets;

    mySize =   ( ( AliHLTUInt8_t * )currOutTrack ) -  ( ( AliHLTUInt8_t * )outputPtr );

    outPtr->fCount = 0;
    
    int nTracks = fTracker->Tracks().size();

    for ( int itr = 0; itr < nTracks; itr++ ) {

      const AliExternalTrackParam &tp = fTracker->Tracks()[itr];
      int id =  tracksTPCId[fTracker->Tracks()[itr].TPCtrackId()];

      int nClusters = 0;

      unsigned int dSize = sizeof( AliHLTExternalTrackParam ) + nClusters * sizeof( unsigned int );

      if ( mySize + dSize > maxBufferSize ) {
        HLTWarning( "Output buffer size exceed (buffer size %d, current size %d), %d tracks are not stored", maxBufferSize, mySize, nTracks - itr + 1 );
        iResult = -ENOSPC;
        break;
      }

      currOutTrack->fAlpha = tp.GetAlpha();
      currOutTrack->fX = tp.GetX();
      currOutTrack->fY = tp.GetY();
      currOutTrack->fZ = tp.GetZ();            
      currOutTrack->fLastX = 0;
      currOutTrack->fLastY = 0;
      currOutTrack->fLastZ = 0;      
      currOutTrack->fq1Pt = tp.GetSigned1Pt();
      currOutTrack->fSinPsi = tp.GetSnp();
      currOutTrack->fTgl = tp.GetTgl();
      for( int i=0; i<15; i++ ) currOutTrack->fC[i] = tp.GetCovariance()[i];
      currOutTrack->fTrackID = id;
      currOutTrack->fFlags = 0;
      currOutTrack->fNPoints = nClusters;    
      currOutTrack = ( AliHLTExternalTrackParam* )( (( Byte_t * )currOutTrack) + dSize );
      mySize += dSize;
      outPtr->fCount++;
    }
  

    AliHLTComponentBlockData resultData;
    FillBlockData( resultData );
    resultData.fOffset = 0;
    resultData.fSize = mySize;
    resultData.fDataType = kAliHLTDataTypeTrack|kAliHLTDataOriginITS;
    outputBlocks.push_back( resultData );
    size = resultData.fSize;  
    
    HLTInfo( "ITS tracker:: output %d tracks",nTracks );
  }
  return iResult;
}
