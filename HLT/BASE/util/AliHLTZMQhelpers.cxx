// a helper library for using ZMQ with ALIROOT, focussed on multipart messaging
// this lib implements the HLT specific interface, for general use cases
// see AliZMQhelpers.h
// blame: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
// some of it might be inspired by czmq.h (http://czmq.zeromq.org)
//
// Copyright (C) 2015 Goethe University Frankfurt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "AliHLTZMQhelpers.h"
#include "AliHLTMessage.h"
#include "zmq.h"

using namespace AliZMQhelpers;

//_______________________________________________________________________________________
int AliZMQhelpers::alizmq_msg_iter_check_id(aliZMQmsg::iterator it, const AliHLTDataTopic& topic)
{
  AliHLTDataTopic actualTopic;
  alizmq_msg_iter_topic(it, actualTopic);
  if (actualTopic.GetID() == topic.GetID()) return 0;
  return 1;
}


//_______________________________________________________________________________________
int AliZMQhelpers::alizmq_msg_send(const AliHLTDataTopic& topic, TObject* object, void* socket, int flags, 
                    int compression, aliZMQrootStreamerInfo* streamers)
{
  int rc = 0;

  AliHLTMessage* tmessage = AliHLTMessage::Stream(object, compression);
  zmq_msg_t dataMsg;
  rc = zmq_msg_init_data( &dataMsg, tmessage->Buffer(), tmessage->Length(),
      alizmq_deleteTObject, tmessage);
  
  if (streamers) {
    alizmq_update_streamerlist(streamers, tmessage->GetStreamerInfos());
  }

  //then send the object topic
  rc = zmq_send( socket, &topic, sizeof(topic), ZMQ_SNDMORE );
  if (rc<0) 
  {
    zmq_msg_close(&dataMsg);
    //printf("unable to send topic: %s %s\n", topic.Description().c_str(), zmq_strerror(errno));
    return rc;
  }

  //send the object itself
  rc = zmq_msg_send(&dataMsg, socket, flags);
  if (rc<0) 
  {
    //printf("unable to send data: %s %s\n", tmessage->GetName(), zmq_strerror(errno));
    zmq_msg_close(&dataMsg);
    return rc;
  }
  return rc;
}

//______________________________________________________________________________
int AliZMQhelpers::alizmq_msg_send(const AliHLTDataTopic& topic, const std::string& data, void* socket, int flags)
{
  int rc = 0;
  rc = zmq_send( socket, &topic, sizeof(topic), ZMQ_SNDMORE );
  if (rc<0) 
  {
    //printf("unable to send topic: %s %s\n", topic.Description().c_str(), zmq_strerror(errno));
    return rc;
  }

  zmq_msg_t dataMsg;
  zmq_msg_init_size(&dataMsg, data.size());
  memcpy(zmq_msg_data(&dataMsg), data.data(), zmq_msg_size(&dataMsg));
  rc = zmq_msg_send(&dataMsg, socket, flags);
  if (rc<0) 
  {
    //printf("unable to send data: %s\n", data.c_str());
    zmq_msg_close(&dataMsg);
    return rc;
  }
  return rc;
}

