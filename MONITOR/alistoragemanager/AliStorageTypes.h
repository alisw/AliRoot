#ifndef AliStorageTypes_H
#define AliStorageTypes_H

#include <TSystem.h>
#include <TFile.h>

inline const char* GetConfigFilePath(){
    return Form("%s/setupStorageDatabase.sh",gSystem->Getenv("HOME"));
}

enum storageSockets{
	SERVER_COMMUNICATION_REQ=0,
	SERVER_COMMUNICATION_REP,
	CLIENT_COMMUNICATION_REQ,
	CLIENT_COMMUNICATION_REP,
	EVENTS_SERVER_PUB,
	EVENTS_SERVER_SUB,
	XML_PUB,
	ITS_POINTS_PUB,
	ITS_POINTS_SUB,
	NUMBER_OF_SOCKETS
};
	
enum statusType{
	STATUS_WAITING=1,
	STATUS_OK,
	STATUS_ERROR,
	STATUS_DOWN
};

enum requestType{
	REQUEST_CONNECTION=1,
	REQUEST_RECEIVING,
	REQUEST_SAVING,
	REQUEST_CURRENT_SIZE,
	REQUEST_LIST_EVENTS,
	REQUEST_GET_EVENT,
	REQUEST_GET_NEXT_EVENT,
	REQUEST_GET_PREV_EVENT,
	REQUEST_GET_LAST_EVENT,
	REQUEST_GET_FIRST_EVENT,
	REQUEST_MARK_EVENT,
	REQUEST_SET_PARAMS,
	REQUEST_GET_PARAMS
};

struct clientRequestStruct
{
    clientRequestStruct(){};
    clientRequestStruct(const clientRequestStruct& crs){
        messageType = crs.messageType;
        maxStorageSize =crs.maxStorageSize;
        maxOccupation = crs.maxOccupation;
        removeEvents = crs.removeEvents;
        eventsInChunk = crs.eventsInChunk;
    }
    
	int messageType = -1;
	int maxStorageSize = -1;
	int maxOccupation = -1;
	int removeEvents=-1;
	int eventsInChunk=-1;
};

struct eventStruct{
	int runNumber;
	int eventNumber;
};

struct listRequestStruct{
	int runNumber[2];
	int eventNumber[2];
	int marked[2];
	int multiplicity[2];
	char system[2][20];
};

struct serverRequestStruct
{
    serverRequestStruct(){};
    serverRequestStruct(const serverRequestStruct& src){
        messageType = src.messageType;
        event = src.event;
        list = src.list;
    }
	int messageType;
	struct eventStruct event;
	struct listRequestStruct list;
};

struct recPointsStruct{
  TFile *files[10];
};

typedef struct serverListStruct{
	int runNumber;
	int eventNumber;
	char system[20];
	int multiplicity;
	int marked;
}serverListStruct;

#endif
