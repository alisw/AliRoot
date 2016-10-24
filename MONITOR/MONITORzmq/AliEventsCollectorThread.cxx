#include "AliEventsCollectorThread.h"
#include "AliZMQManager.h"

#include <TSystemDirectory.h>

#include <iostream>
#include <fstream>

using namespace std;

AliEventsCollectorThread::AliEventsCollectorThread(AliStorageClientThread *onlineReconstructionManager) :
fManager(onlineReconstructionManager),
fCurrentRunNumber(-1),
fCollectorThread(0),
fDatabase(0)
{
    fDatabase = new AliStorageDatabase();
    
    if(!fDatabase){
        cout<<"AliEventsCollectorThread -- FATAL\n\nCould not connect to database!\n\n"<<endl;
        exit(0);
    }
    else{
        cout<<"AliEventsCollectorThread -- connected to database"<<endl;
    }
    
    CheckCurrentStorageSize();
    
    // start collecting events in a thread
    fCollectorThread = new TThread("fCollectorThread",Dispatch,(void*)this);
    fCollectorThread->Run();
}


AliEventsCollectorThread::~AliEventsCollectorThread()
{
    if(fCollectorThread){delete fCollectorThread;}
    if(fDatabase){delete fDatabase;}
    if(fManager){delete fManager;}
}

void AliEventsCollectorThread::Kill()
{
    if(fCollectorThread){
        fCollectorThread->Join();
        fCollectorThread->Kill();
    }
}

void AliEventsCollectorThread::CollectorHandle()
{
    AliZMQManager *eventManager = AliZMQManager::GetInstance();
    eventManager->CreateSocket(EVENTS_SERVER_SUB);
    
    bool receiveStatus = false;
    
    AliESDEvent *event = NULL;
    
    TFile *esdFile = NULL;
    TTree *esdTree = NULL;
    
    while(1)
    {
        if(event){ delete event; event = 0; }
        
        cout<<"AliEventsCollectorThread -- waiting for event..."<<endl;
        receiveStatus = eventManager->Get(event,EVENTS_SERVER_SUB);
        
        if (receiveStatus == 0){ // timeout
            continue;
        }
        else if(receiveStatus == -1){ // error, socket closed
            break;
        }
        else if(event && receiveStatus)
        {
            cout<<"AliEventsCollectorThread -- Received event:"<<event->GetEventNumberInFile()<<"\trun:"<<event->GetRunNumber()<<endl;
            
            CheckCurrentStorageSize();

            TThread::Lock();
            
            fDatabase->InsertEvent(event->GetRunNumber(),
                                   event->GetEventNumberInFile(),
                                   (char*)event->GetBeamType(),
                                   event->GetNumberOfTracks(),
                                   Form("%s/run%d/AliESDs.root",fManager->fStoragePath.c_str(),
                                        event->GetRunNumber()),
                                   event->GetTriggerMask(),
                                   event->GetTriggerMaskNext50()
                                   );
            
            cout<<"AliEventsCollectorThread -- event inserted to the database"<<endl;
            
            TThread::UnLock();
        }
        else
        {
            cout<<"AliEventsCollectorThread -- ERROR -- NO DATA!"<<endl;
            if(fManager->fReceivingStatus!=STATUS_ERROR){fManager->fReceivingStatus=STATUS_ERROR;}
        }
    }
    if(event){delete event;}
}


Long64_t AliEventsCollectorThread::GetSizeOfAllChunks()
{
    Long64_t totalStorageSize = 0;
    
    TSystemDirectory dir(fManager->fStoragePath.c_str(),fManager->fStoragePath.c_str());
    TList *listOfDirectories = dir.GetListOfFiles();
    
    if (!listOfDirectories){
        cout<<"AliEventsCollectorThread -- Storage directory is empty"<<endl;
        return 0;
    }
    
    TIter nextDirectory(listOfDirectories);
    TSystemFile *runDirectory;
    string directoryName;
    const char *size;
    
    while ((runDirectory=(TSystemFile*)nextDirectory()))
    {
        directoryName=runDirectory->GetName();
        if (runDirectory->IsDirectory() && directoryName.find("run")==0)
        {
            
            TSystemDirectory dirRun(Form("%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str()),
                                    Form("%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str()));
            TList *listOfFiles = dirRun.GetListOfFiles();
            
            if(listOfFiles)
            {
                TIter nextFile(listOfFiles);
                TSystemFile *file;
                string fileName;
                
                while((file=(TSystemFile*)nextFile()))
                {
                    fileName = file->GetName();
                    if(!file->IsDirectory() && fileName.find("AliESDs.root")==0)
                    {
                        size = gSystem->GetFromPipe(Form("wc -c < %s/%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str(), fileName.c_str())).Data();
                        
                        totalStorageSize += atoi(size);
                    }
                }
                if(file){delete file;}
            }
            if(listOfFiles){delete listOfFiles;}
        }
    }
    
    if(listOfDirectories){delete listOfDirectories;}
    if(runDirectory){delete runDirectory;}
    
    printf("AliEventsCollectorThread -- Total storage size:%lld\t(%.2f MB)\n",totalStorageSize,(float)totalStorageSize/(1000.*1000.));
    
    return totalStorageSize;
}

void AliEventsCollectorThread::CheckCurrentStorageSize()
{
    fManager->fCurrentStorageSize=GetSizeOfAllChunks();
    
    if(fManager->fCurrentStorageSize >  (float)fManager->fStorageOccupationLevel/100. * fManager->fMaximumStorageSize)
    {
        while(GetSizeOfAllChunks() > (float)fManager->fRemoveEventsPercentage/100. * fManager->fMaximumStorageSize)
        {
            TThread::Lock();
            struct eventStruct oldestEvent = fDatabase->GetOldestEvent();
            string oldestEventPath = fDatabase->GetFilePath(oldestEvent);
            TThread::UnLock();
            
            if(oldestEvent.runNumber == fCurrentRunNumber){
                cout<<"AliEventsCollectorThread -- Can't remove file for current run"<<endl;
                return;
            }
            
            //remove oldest event
            cout<<"AliEventsCollectorThread -- Removing old events:"<<oldestEventPath<<endl;
            gSystem->Exec(Form("rm -fr dirname %s",oldestEventPath.c_str()));
            TThread::Lock();
            fDatabase->RemoveEventsWithPath(oldestEventPath);
            TThread::UnLock();
        }
    }
}



