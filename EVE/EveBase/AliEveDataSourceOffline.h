//
//  AliEveDataSourceOffline.h
//  xAliRoot
//
//  Created by Jeremi Niedziela on 01/06/15.
//
//

#ifndef __AliEveDataSourceOffline__
#define __AliEveDataSourceOffline__

#include "AliEveEventManager.h"
#include "AliEveDataSource.h"
#include "AliEventInfo.h"

class AliEveDataSourceOffline : public AliEveDataSource
{
public:
    AliEveDataSourceOffline();
    ~AliEveDataSourceOffline();
    
    void GotoEvent(Int_t event);
    void NextEvent();
    
    void SetFilesPath(const TString& path);
    
    Int_t GetMaxEventId();
private:
    void Init(){};// to implement
    void Open();
    void Close();
    TTree* readESDTree(const char* treeName, int &run);
    
    bool fIsOpen;           // Are event-files opened.
    bool fESDfriendExists;	// Flag specifying if ESDfriend was found during opening of the event-data.
    
    AliEveEventManager *fEventManager;
    
    // names of files:
    TString  fgESDFileName;           // Name by which to open ESD
    TString  fgESDfriendsFileName;    // Name by which to open ESD friend
    TString  fgAODFileName;           // Name by which to open AOD
    TString  fgRawFileName;           // Name by which to open raw-data file
    TString  fgGAliceFileName;        // galice.root file
    
    TList *fgAODfriends; // Global list of AOD friend names to be attached during opening of the event-data (empty by default).
    
    ClassDef(AliEveDataSourceOffline, 0);
};


#endif
