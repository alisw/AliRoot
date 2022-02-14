///////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010
//
//    This file is part of starlight.
//
//    starlight is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    starlight is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with starlight. If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////
//
// File and Version Information:
// $Rev:: 308                         $: revision of last commit
// $Author:: aaronstanek              $: author of last commit
// $Date:: 2019-06-24 19:30:51 +0200 #$: date of last commit
//
// Description:
//
//
//
///////////////////////////////////////////////////////////////////////////


#ifndef EVENTFILEWRITER_H
#define EVENTFILEWRITER_H


#include <string>

#include "filewriter.h"
#include "inputParameters.h"


class eventFileWriter : public fileWriter
{
   public:
      
      /** Default constructor */
      eventFileWriter();
      
      /** Constructor with name */
      eventFileWriter(std::string filename);

      /** Write out simulation set up */
      int writeInit(inputParameters &param );

      /** Write an UPC event to file */
      int writeEvent(upcEvent &event, int eventnumber);
      
      /** Set if we want to write full pythia information */
      void writeFullPythiaInfo(bool v) { _writeFullPythia = v; }
      
private:
  
  bool _writeFullPythia;
      
};


#endif  // EVENTFILEWRITER_H
