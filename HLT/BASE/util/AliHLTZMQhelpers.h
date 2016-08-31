#ifndef __AliHLTZMQhelpers__
#define __AliHLTZMQhelpers__

// a helper library for using ZMQ with ALIROOT, focussed on multipart messaging
// this lib implements the HLT specific interface, for general use cases
// see AliZMQhelpers.h
// blame: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
// some of it might be inspired by czmq.h (http://czmq.zeromq.org)

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

#include "AliZMQhelpers.h"
#include "AliHLTDataTypes.h"
#include <inttypes.h>

namespace AliZMQhelpers
{

//this is just to implement the methods which depend on aliroot HLT definitions
//for ZMQ communication
struct AliHLTDataTopic : public DataTopic
{
  //ctor
  AliHLTDataTopic()
    : DataTopic()
  {
  }

  //copy ctor
  AliHLTDataTopic(const AliHLTComponentDataType& dataType)
    : DataTopic()
  {
    memcpy( fTopic, dataType.fID, kAliHLTComponentDataTypefIDsize );
    memcpy( fTopic+kAliHLTComponentDataTypefIDsize, dataType.fOrigin, kAliHLTComponentDataTypefOriginSize );
  }

  //copy ctor
  AliHLTDataTopic(const AliHLTComponentBlockData& blockData)
    : DataTopic()
  {
    fSpecification = blockData.fSpecification;
    memcpy( fTopic, blockData.fDataType.fID, kAliHLTComponentDataTypefIDsize );
    memcpy( fTopic+kAliHLTComponentDataTypefIDsize, blockData.fDataType.fOrigin, kAliHLTComponentDataTypefOriginSize );
  }

  //partial (no fSpecification) copy from AliHLTComponentDataType
  AliHLTDataTopic& operator=( const AliHLTComponentDataType& dataType )
  {
    memcpy( fTopic, dataType.fID, kAliHLTComponentDataTypefIDsize );
    memcpy( fTopic+kAliHLTComponentDataTypefIDsize, dataType.fOrigin, kAliHLTComponentDataTypefOriginSize );
    return *this;
  }

  //assignment from a AliHLTComponentBlockData
  AliHLTDataTopic& operator=( const AliHLTComponentBlockData& blockData )
  {
    memcpy( fTopic, blockData.fDataType.fID, kAliHLTComponentDataTypefIDsize );
    memcpy( fTopic+kAliHLTComponentDataTypefIDsize, blockData.fDataType.fOrigin, kAliHLTComponentDataTypefOriginSize );
    fSpecification = blockData.fSpecification;
    return *this;
  }

  bool operator==( const AliHLTDataTopic& dt )
  {
    bool topicMatch = Topicncmp(dt.fTopic, fTopic);
    return topicMatch;
  }

  void Fill(AliHLTComponentDataType& dt)
  {
    memcpy( dt.fID, &fTopic[0], kAliHLTComponentDataTypefIDsize );
    memcpy( dt.fOrigin, &fTopic[kAliHLTComponentDataTypefIDsize], kAliHLTComponentDataTypefOriginSize );
  }

};


int alizmq_msg_iter_check_id(aliZMQmsg::iterator it, const AliHLTDataTopic& topic);
int alizmq_msg_send(const AliHLTDataTopic& topic, TObject* object, void* socket, int flags, int compression=0, aliZMQrootStreamerInfo* streamers=NULL);
int alizmq_msg_send(const AliHLTDataTopic& topic, const std::string& data, void* socket, int flags);

}  //end namespace AliZMQhelpers

#endif

