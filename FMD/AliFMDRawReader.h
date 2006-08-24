#ifndef ALIFMDRAWREADER_H
#define ALIFMDRAWREADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * Latest changes by Christian Holm Christensen <cholm@nbi.dk>
 *
 * See cxx source for full Copyright notice                               
 */
//____________________________________________________________________
// 
// Class to read ADC values from a AliRawReader object. 
// Note, that it uses an ALTRO reader, which is wrong. 
// Perhaps we need to implement it our selves
// 
/* $Id$ */
/** @file    AliFMDRawReader.h
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 12:45:23 2006
    @brief   Class to read raw data 
    @ingroup FMD_rec
*/
#ifndef ROOT_TTask
# include <TTask.h>
#endif

//____________________________________________________________________
class AliRawReader;
class TTree;
class TClonesArray;


//____________________________________________________________________
/** @brief Class to read ALTRO formated raw data from an AliRawReader
    object. 
    @code 
    AliRawReader*    reader    = new AliRawReaderFile(0);
    AliFMDRawReader* fmdReader = new AliFMDRawReader(reader);
    TClonesArray*    array     = new TClonesArray("AliFMDDigit");
    fmdReader->ReadAdcs(array);
    @endcode 
    @ingroup FMD_rec
*/
class AliFMDRawReader : public TTask 
{
public:
  /** CTOR 
      @param reader Raw reader
      @param array  Output tree */
  AliFMDRawReader(AliRawReader* reader, TTree* array);
  /** DTOR */
  virtual ~AliFMDRawReader() {}
  /** Read in, and store in output tree 
      @param option Not used */
  virtual void   Exec(Option_t* option="");
  /** Read ADC's into a TClonesArray of AliFMDDigit objects. 
      @param array Array to read into 
      @return @c true on success */
  virtual Bool_t ReadAdcs(TClonesArray* array);
protected:
  AliFMDRawReader(const AliFMDRawReader& o) 
    : TTask(o), 
      fTree(0), 
      fReader(0), 
      fSampleRate(0)
  {}
  AliFMDRawReader& operator=(const AliFMDRawReader&) { return *this; }
  TTree*        fTree;       //! Pointer to tree to read into 
  AliRawReader* fReader;     //! Pointer to raw reader 
  UShort_t      fSampleRate; // The sample rate (if 0, inferred from data)
  ClassDef(AliFMDRawReader, 0) // Read FMD raw data into a cache 
};

#endif
//____________________________________________________________________
//
// Local Variables:
//   mode: C++
// End:
//
// EOF
//
