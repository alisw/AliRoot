#include "AliEventsCollectorThread.h"
#include "AliZMQManager.h"

#include <TSystemDirectory.h>

#include <iostream>
#include <fstream>

using namespace std;

AliEventsCollectorThread::AliEventsCollectorThread(AliStorageClientThread *onlineReconstructionManager) :
fManager(onlineReconstructionManager),
fCollectorThread(0),
fCurrentFile(0),
fDatabase(0)
{
    fDatabase = new AliStorageDatabase();
    
    if(!fDatabase){
        cout<<"\n\nCould not connect to database!\n\n"<<endl;
        exit(0);
    }
    else{
        cout<<"\n\nConnected to database\n\n"<<endl;
    }
    
//    CheckCurrentStorageSize();
    
    // start collecting events in a thread
    fCollectorThread = new TThread("fCollectorThread",Dispatch,(void*)this);
    fCollectorThread->Run();
}


AliEventsCollectorThread::~AliEventsCollectorThread()
{
    if(fCollectorThread){delete fCollectorThread;}
    
    if(fCurrentFile){
        fCurrentFile->Close();
        delete fCurrentFile;
    }
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
    
    int runNumber=-1;
    int receiveStatus = false;
    
    AliESDEvent *event = NULL;
    
    TFile *esdFile = NULL;
    TTree *esdTree = NULL;
    
    while(1)
    {
        cout<<"CLIENT -- waiting for event..."<<endl;
        receiveStatus = eventManager->Get(event,EVENTS_SERVER_SUB);
        
        if (receiveStatus == 0){ // timeout
            continue;
        }
        else if(receiveStatus == -1){ // error, socket closed
            break;
        }
        else if(event && receiveStatus)
        {
            cout<<"CLIENT -- Received event:"<<event->GetEventNumberInFile()<<"\trun:"<<event->GetRunNumber()<<endl;
            
            if(event->GetRunNumber() != runNumber)// first event in a new run
            {
                cout<<"CLIENT -- new run started"<<endl;
                runNumber = event->GetRunNumber();
                gSystem->Exec(Form("mkdir -p %s/run%d",fManager->fStoragePath.c_str(),runNumber));
                
                if(esdFile){
                    esdFile->Write();
                    esdFile->Close();
                    delete esdFile;
                    esdFile = 0;
                }
            
                // create new empty file for ESD events
                esdFile = TFile::Open(Form("%s/run%d/AliESDs.root",fManager->fStoragePath.c_str(),runNumber), "RECREATE");
                esdTree = new TTree("esdTree", "Tree with ESD objects");
                event->WriteToTree(esdTree);
                esdTree->Fill();
                esdTree->Write();
                esdFile->Close();
                cout<<"file created"<<endl;
            }
            else
            {
                esdFile = TFile::Open(Form("%s/run%d/AliESDs.root",fManager->fStoragePath.c_str(),runNumber), "UPDATE");
    //
                esdTree = (TTree*)esdFile->Get("esdTree");
                
                event->AddToTree(esdTree);
                esdTree->Fill();
                esdTree->Write(0,TObject::kWriteDelete,0);
                
                delete esdTree;
                esdFile->Close();
            }
            
            /*
//          CheckCurrentStorageSize();

            TThread::Lock();
            
            fDatabase->InsertEvent(event->GetRunNumber(),
                                   event->GetEventNumberInFile(),
                                   (char*)event->GetBeamType(),
                                   event->GetMultiplicity()->GetNumberOfTracklets(),
                                   Form("%s/run%d/AliESDs.root",fManager->fStoragePath.c_str(),
                                        event->GetRunNumber()),
                                   event->GetTriggerMask(),
                                   event->GetTriggerMaskNext50()
                                   );
            
            cout<<"AliEventsCollectorThread -- event inserted to the database"<<endl;
            
            TThread::UnLock();
            */
//            delete event;event=0;
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
        cout<<"CLIENT -- Storage directory is empty"<<endl;
        return 0;
    }
    TIter nextDirectory(listOfDirectories);
    TSystemFile *runDirectory;
    string directoryName;
    
    while ((runDirectory=(TSystemFile*)nextDirectory()))
    {
        directoryName=runDirectory->GetName();
        if (runDirectory->IsDirectory() && directoryName.find("run")==0)
        {
            TSystemDirectory dirChunks(Form("%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str()),Form("%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str()));
            TList *listOfChunks = dirChunks.GetListOfFiles();
            
            if(listOfChunks)
            {
                TIter nextChunk(listOfChunks);
                TSystemFile *chunk;
                string chunkFileName;
                
                while((chunk=(TSystemFile*)nextChunk()))
                {
                    chunkFileName = chunk->GetName();
                    if(!chunk->IsDirectory() && chunkFileName.find("chunk")==0)
                    {
                        TFile *tmpFile = new TFile(Form("%s/%s/%s",fManager->fStoragePath.c_str(),directoryName.c_str(),chunkFileName.c_str()),"read");
                        if(tmpFile)
                        {
                            totalStorageSize+=tmpFile->GetSize();
                            tmpFile->Close();
                            delete tmpFile;
                        }
                    }
                }
                if(chunk){delete chunk;}
            }
            if(listOfChunks){delete listOfChunks;}
        }
    }
    
    if(listOfDirectories){delete listOfDirectories;}
    if(runDirectory){delete runDirectory;}
    
    printf("CLIENT -- Total storage size:%lld\t(%.2f MB)\n",totalStorageSize,(float)totalStorageSize/(1000.*1000.));
    
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
            //remove oldest event
            cout<<"CLIENT -- Removing old events:"<<oldestEventPath<<endl;
            gSystem->Exec(Form("rm -f %s",oldestEventPath.c_str()));
            TThread::Lock();
            fDatabase->RemoveEventsWithPath(oldestEventPath);
            TThread::UnLock();
        }
    }
}



