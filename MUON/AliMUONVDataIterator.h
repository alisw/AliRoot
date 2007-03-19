/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup base
/// \class AliMUONVDataIterator
/// \brief Defines an interface of an iterator over muon data structure(s)
/// 
//  Author Laurent Aphecetche

#ifndef ALIMUONVDATAITERATOR_H
#define ALIMUONVDATAITERATOR_H

#ifndef ROOT_TObject
#  include "TObject.h"
#endif

class AliMUONVDataIterator : public TObject
{
public:
  AliMUONVDataIterator();
  virtual ~AliMUONVDataIterator();
  
  /// \todo add comment
  virtual TObject* Next() = 0;
  
  /// \todo add comment
  virtual void Reset() = 0; 
  
  /// \todo add comment
  virtual Bool_t Remove() = 0;
  
  ClassDef(AliMUONVDataIterator,0) // Interface for an iterator on AliMUONData.
};

#endif
