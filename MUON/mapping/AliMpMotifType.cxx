// $Id$
// Category: motif
//
// Class AliMpMotifType
// --------------------
// Class that defines the motif properties.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#include <stdlib.h>
#include <Riostream.h>

#include "AliMpMotifType.h"
#include "AliMpMotifTypePadIterator.h"
#include "AliMpConnection.h"

ClassImp(AliMpMotifType)

const Int_t AliMpMotifType::fgkPadNumForA = 65;
#ifdef WITH_ROOT
const Int_t AliMpMotifType::fgkSeparator = 10000;
#endif


//______________________________________________________________________________
AliMpMotifType::AliMpMotifType(const TString &id) 
  : TObject(),
    fID(id),
    fNofPadsX(0),   
    fNofPadsY(0),
    fVerboseLevel(0),
    fConnections()
{
  // Constructor
}

//______________________________________________________________________________
AliMpMotifType::AliMpMotifType() 
  : TObject(),
    fID(""),
    fNofPadsX(0),   
    fNofPadsY(0),
    fVerboseLevel(0),
    fConnections()
{
  // Default constructor (dummy)
}

//______________________________________________________________________________
AliMpMotifType::~AliMpMotifType() {
// Destructor

#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   delete i->second;

  fConnections.erase(fConnections.begin(),fConnections.end());
#endif  

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) delete (AliMpConnection*)value;
#endif  
}

#ifdef WITH_ROOT
//______________________________________________________________________________
Int_t  AliMpMotifType::GetIndex(const AliMpIntPair& pair) const
{
// Converts the pair of integers to integer.
// ---

  if (pair.GetFirst() >= fgkSeparator || pair.GetSecond() >= fgkSeparator)
    Fatal("GetIndex", "Index out of limit.");
      
  return pair.GetFirst()*fgkSeparator + pair.GetSecond() + 1;
}  

//______________________________________________________________________________
AliMpIntPair  AliMpMotifType::GetPair(Int_t index) const
{
// Converts the integer index to the pair of integers.
// ---

  return AliMpIntPair((index-1)/fgkSeparator,(index-1)%fgkSeparator);
}  
#endif

//______________________________________________________________________________
AliMpVPadIterator* AliMpMotifType::CreateIterator() const
{
  return new AliMpMotifTypePadIterator(this);
}

//______________________________________________________________________________
void AliMpMotifType::SetNofPads(Int_t nofPadsX, Int_t nofPadsY)
{
  // Change the number of pads in this motif

  fNofPadsX = nofPadsX;
  fNofPadsY = nofPadsY;
}


//______________________________________________________________________________
Int_t AliMpMotifType::PadNum(const TString &padName) const
{
  // Transform a pad name into the equivalent pad number
  if ( (padName[0]>='A') && (padName[0]<='Z') )
    return fgkPadNumForA+padName[0]-'A';
  else
    return atoi(padName.Data());
}

//______________________________________________________________________________
TString AliMpMotifType::PadName(Int_t padNum) const
{
  // Transform a pad number into its equivalent pad name
  if (padNum<fgkPadNumForA)
    return Form("%d",padNum);
  else
    return char('A'+padNum-fgkPadNumForA);
}

//______________________________________________________________________________
void AliMpMotifType::AddConnection(const AliMpIntPair &localIndices, 
                               AliMpConnection* connection)
{
  // Add the connection to the map
  
#ifdef WITH_STL
  fConnections[localIndices]=connection;
#endif

#ifdef WITH_ROOT
  fConnections.Add(GetIndex(localIndices), (Long_t)connection);
#endif   

  connection->SetOwner(this);
}  

//______________________________________________________________________________
AliMpConnection *AliMpMotifType::FindConnectionByPadNum(Int_t padNum) const
{
  // Retrieve the AliMpConnection pointer from its pad num
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetPadNum()==padNum) return i->second;
 return 0;
#endif

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetPadNum()==padNum) return connection;
  }  
 return 0;
#endif
}

//______________________________________________________________________________
AliMpConnection *AliMpMotifType::FindConnectionByLocalIndices(
                                       const AliMpIntPair& localIndices) const
{
  if (!localIndices.IsValid()) return 0;

  // Retrieve the AliMpConnection pointer from its position (in pad unit)
#ifdef WITH_STL
  ConnectionMapCIterator i = fConnections.find(localIndices);
 if (i != fConnections.end())
   return i->second;
 else return 0;
#endif

#ifdef WITH_ROOT
  Long_t value = fConnections.GetValue(GetIndex(localIndices));
  if (value) 
    return (AliMpConnection*)value;
  else
    return 0;  
#endif
}

//______________________________________________________________________________
AliMpConnection *AliMpMotifType::FindConnectionByGassiNum(Int_t gassiNum) const
{
  // return the connection for the given gassiplex number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetGassiNum()==gassiNum) return i->second;
 return 0;
#endif

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetGassiNum()==gassiNum) return connection;
  }  
 return 0;
#endif
}

//______________________________________________________________________________
AliMpConnection *AliMpMotifType::FindConnectionByKaptonNum(Int_t kaptonNum) const
{
  // Gives the connection related to the given kapton number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetKaptonNum()==kaptonNum) return i->second;
 return 0;
#endif

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetKaptonNum()==kaptonNum) return connection;
  }  
 return 0;
#endif
}
//______________________________________________________________________________
AliMpConnection *AliMpMotifType::FindConnectionByBergNum(Int_t bergNum) const
{
  // Retrieve the connection from a Berg connector number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetBergNum()==bergNum) return i->second;
 return 0;
#endif

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetBergNum()==bergNum) return connection;
  }  
  return 0;
#endif
}


//______________________________________________________________________________
AliMpIntPair AliMpMotifType::FindLocalIndicesByConnection(
                                 const AliMpConnection* connection) const
{
  // Retrieve the pad position from the connection pointer.
  // Not to be used widely, since it use a search in the
  // connection list...

#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second==connection) return i->first;
#endif

#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* aConnection = (AliMpConnection*)value;
    if (aConnection == connection) return GetPair(key);
  }  
#endif

  return AliMpIntPair::Invalid();
}

//______________________________________________________________________________
AliMpIntPair AliMpMotifType::FindLocalIndicesByPadNum(Int_t padNum) const
{
  // Retrieve the AliMpConnection pointer from its pad num
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetPadNum()==padNum) return i->first;
#endif
   
#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetPadNum() == padNum) return GetPair(key);
  }  
#endif
 return AliMpIntPair::Invalid();
}

//______________________________________________________________________________
AliMpIntPair AliMpMotifType::FindLocalIndicesByGassiNum(Int_t gassiNum) const
{
  // return the connection for the given gassiplex number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetGassiNum()==gassiNum) return i->first;
#endif
   
#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetGassiNum()==gassiNum) return GetPair(key);
  }  
#endif
   
 return AliMpIntPair::Invalid();
}

//______________________________________________________________________________
AliMpIntPair AliMpMotifType::FindLocalIndicesByKaptonNum(Int_t kaptonNum) const
{
  // Gives the connection related to the given kapton number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetKaptonNum()==kaptonNum) return i->first;
#endif
   
#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetKaptonNum()==kaptonNum) return GetPair(key);
  }  
#endif
   
 return AliMpIntPair::Invalid();
}

//______________________________________________________________________________
AliMpIntPair AliMpMotifType::FindLocalIndicesByBergNum(Int_t bergNum) const
{
  // Retrieve the connection from a Berg connector number
#ifdef WITH_STL
 for(ConnectionMapCIterator i = fConnections.begin();
  i!=fConnections.end();++i)
   if (i->second->GetBergNum()==bergNum) return i->first;
#endif
   
#ifdef WITH_ROOT
  ConnectionMapCIterator i(&fConnections);
  Long_t key, value;
  while ( i.Next(key, value) ) {
    AliMpConnection* connection = (AliMpConnection*)value;
    if (connection->GetBergNum()==bergNum) return GetPair(key);
  }  
#endif
   
 return AliMpIntPair::Invalid();
}

//______________________________________________________________________________
Int_t  AliMpMotifType::GetNofPads() const   
{
// Returns the number of pads

#ifdef WITH_STL
  return fConnections.size();
#endif
   
#ifdef WITH_ROOT
  return fConnections.GetSize();
#endif
}

//______________________________________________________________________________
Bool_t AliMpMotifType::HasPad(const AliMpIntPair& localIndices) const
{
  if (!localIndices.IsValid()) return false;

#ifdef WITH_STL
  // return true if the pad indexed by <localIndices> has a connection
  return fConnections.find(localIndices)!=fConnections.end();
#endif

#ifdef WITH_ROOT
  Long_t value = fConnections.GetValue(GetIndex(localIndices));
  return value!=0;
#endif
}

//______________________________________________________________________________
void AliMpMotifType::Print(Option_t *option) const
{
  // Print the map of the motif. In each cel, the value
  // printed depends of option, as the following:
  // option="N" the "name" of the pad is written
  // option="K" the Kapton connect. number attached to the pad is written
  // option="B" the Berg connect. number attached to the pad is written
  // option="G" the Gassiplex channel number attached to the pad is written
  // otherwise the number of the pad is written

  // NOTE : this method is really not optimized, in case 'N' or '',
  // but the Print() this should not be very important in a Print() method

  switch (option[0]){
  case 'N':cout<<"Name mapping";
    break;
  case 'K':cout<<"Kapton mapping";
    break;
  case 'B':cout<<"Berg mapping";
    break;
  case 'G':cout<<"Gassiplex number mapping";
    break;
  default:cout<<"Pad mapping";
  }
  cout<<" in the motif "<<fID<<endl;
  cout<<"-----------------------------------"<<endl;

  for (Int_t j=fNofPadsY-1;j>=0;j--){
    for (Int_t i=0;i<fNofPadsX;i++){
      AliMpConnection *connexion = FindConnectionByLocalIndices(AliMpIntPair(i,j));
      TString str;
      if (connexion){
	switch (option[0]){
	case 'N':str=PadName(connexion->GetPadNum());
	  break;
	case 'K':str=Form("%d",connexion->GetKaptonNum());
	  break;
	case 'B':str=Form("%d",connexion->GetBergNum());
	  break;
        case 'G':str=Form("%d",connexion->GetGassiNum());
          break;
	default:str= Form("%d",connexion->GetPadNum());
	}
	cout<<setw(2)<<str;
      } else cout<<setw(2)<<"--";
      cout<<" ";
    }
    cout<<endl;
  }
}
