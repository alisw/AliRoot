/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

////////////////////////////////////////////////////////////////////////
//
//  Base Class for Detector specific Merging/Digitization   
//                  
//  Author: Jiri Chudoba (CERN)
//
////////////////////////////////////////////////////////////////////////

/*
$Log$
Revision 1.2  2001/10/04 15:56:34  jchudoba
TTask inheritance

Revision 1.1  2001/07/27 13:02:06  jchudoba
ABC for detector digits merging/digitization

*/

// system includes
#include <iostream.h>

// ROOT includes

// AliROOT includes
#include "AliDigitizer.h"
#include "AliRunDigitizer.h"

ClassImp(AliDigitizer)

AliDigitizer::AliDigitizer(const Text_t* name, const Text_t* title)
  :TTask(name,title) 
{
//
// dummy default ctor with name and title
//
  fManager = 0;
}

AliDigitizer::AliDigitizer(AliRunDigitizer *manager, 
			   const Text_t* name, const Text_t* title)
  :TTask(name,title)
{
//
// ctor with name and title
//
  fManager = manager;
  manager->AddDigitizer(this);
}



AliDigitizer::~AliDigitizer() {;}
