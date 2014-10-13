#include "AliStorageEventManager.h"
#include "AliStorageTypes.h"
#include "AliESDEvent.h"
#include "AliESDRun.h"

#include <iostream>

#include <TThread.h>

using namespace std;

/* parameters:

0 - connect directly to reconstruction socket
1 - connect to Storage Manager and ask for last event
2 - connect to alieventserver, receive in thread and use in main thread

*/

// global variables:
AliESDEvent *currentEvent[2];
TTree *currentTree[2];
TMutex myMutex;
int eventInUse=1;
int writingToEventIndex=0;
bool isNewEventAvaliable=false;
//-----------------

static void* GetNextEvent(void*);

int main(int argc, char **argv)
{
	if(argc<2)
	{
		cout<<"Usage: alifakedisplay <mode>"<<endl;
		cout<<"mode:"<<endl;
		cout<<"0 - connect directly to reconstruction socket"<<endl;
		cout<<"1 - connect to Storage Manager and ask for last event"<<endl;
		return 0;
	}

	storageSockets socket; 
	AliStorageEventManager *manager = AliStorageEventManager::GetEventManagerInstance();
	AliESDEvent *event;
	
	if(atoi(argv[1])==0)
	{
		socket = EVENTS_SERVER_SUB;
		manager->CreateSocket(socket);
		cout<<"Socket created"<<endl;
		while(1)
		{
		  cout<<"waiting for event..."<<flush;
			event = manager->GetEvent(socket);
			
			if(event)
			{
				cout<<"Received event. Run:"<<event->GetRunNumber()<<"\t event:"<<event->GetEventNumberInFile()<<endl;

				cout<<event->GetPeriodNumber()<<endl;
				cout<<event->GetOrbitNumber()<<endl;
				cout<<event->GetBunchCrossNumber()<<endl;
				for(int i=0;i<100;i++)
				{
					if(strcmp(event->GetESDRun()->GetTriggerClass(i),"")){
						cout<<event->GetESDRun()->GetTriggerClass(i)<<endl;}
				}
				delete event;
			}
			else
			{
				cout<<"NO EVENT"<<endl;
			}
		}
	}
	else if(atoi(argv[1])==1)
	{
		socket = SERVER_COMMUNICATION_REQ;
		manager->CreateSocket(socket);
		while(1)
		{
			struct serverRequestStruct *requestMessage = new struct serverRequestStruct;
			requestMessage->messageType = REQUEST_GET_LAST_EVENT;

			manager->Send(requestMessage,socket);			
			event = manager->GetEvent(socket);
			if(event)
			{
				cout<<"Last event - Run:"<<event->GetRunNumber()<<"\t event:"<<event->GetEventNumberInFile()<<endl;
		
				delete event;
			}
			else
			{
				cout<<"NO EVENT"<<endl;
			}
			sleep(1);
		}
	}
        else if(atoi(argv[1])==2)
	{
          TThread *getEventThread = new TThread("getEventThread",GetNextEvent,nullptr);
          getEventThread->Run();

          int counter=0;
          
          while(1)
          {
            counter++;
            if(isNewEventAvaliable)
            {
              cout<<"new event"<<endl;
              myMutex.Lock();
              if(writingToEventIndex == 0) eventInUse = 0;
              else if(writingToEventIndex == 1) eventInUse = 1;
              cout<<"Using:"<<eventInUse<<endl;

              if(currentEvent[eventInUse])
              {
                if(currentEvent[eventInUse]->GetRunNumber() >= 0)
                {
                  cout<<"CURRENT EVENT:"<<currentEvent[eventInUse]->GetEventNumberInFile()<<endl;
                }
              }
              isNewEventAvaliable = false;
              myMutex.UnLock();
            }
            else{cout<<"No new event is avaliable."<<endl;}

            sleep(2);
          }
	}

	return 0;
}

void* GetNextEvent(void*)
{
  AliStorageEventManager *eventManager = AliStorageEventManager::GetEventManagerInstance();
  eventManager->CreateSocket(EVENTS_SERVER_SUB);

  currentEvent[0]=0;
  currentEvent[1]=0;
  currentTree[0]=0;
  currentTree[1]=0;
  AliESDEvent *tmpEvent;
  
  while(1)
  {
    //if(tmpEvent){delete tmpEvent;tmpEvent=0;}
    tmpEvent = eventManager->GetEvent(EVENTS_SERVER_SUB);

    if(tmpEvent)
    {
      if(tmpEvent->GetRunNumber()>=0)
      {
        myMutex.Lock();
        if(eventInUse == 0){writingToEventIndex = 1;}
        else if(eventInUse == 1){writingToEventIndex = 0;}

        if(currentEvent[writingToEventIndex])
        {
          delete currentEvent[writingToEventIndex];
          currentEvent[writingToEventIndex]=0;
        }
        currentEvent[writingToEventIndex] = tmpEvent;
        isNewEventAvaliable = true;
        myMutex.UnLock();
      }
    }	
  }
}
