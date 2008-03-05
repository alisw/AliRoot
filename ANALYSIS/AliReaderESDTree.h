#ifndef AliReaderESDTree_H
#define AliReaderESDTree_H

//***********************************************************************
// class AliReaderESDTree
// Reader for ESD Tree 
// Ch. Finck
//***********************************************************************

#include "AliReaderESD.h"

class TFile;
class TTree;

class AliReaderESDTree: public AliReaderESD
 {
   public:

    AliReaderESDTree(const Char_t* esdfilename = "AliESDs.root", 
                     const Char_t* galfilename = "galice.root");

    virtual ~AliReaderESDTree();


   protected:
    Int_t         ReadNext();//reads tracks and particles and puts them in runs
    TFile*        OpenFile(Int_t evno);//opens files to be read for given event
   
    TTree*        fTree;// tree pointer
    
   private:
    AliReaderESDTree(const AliReaderESDTree&); // Not implemented
    AliReaderESDTree& operator=(const AliReaderESDTree&); // Not implemented

    ClassDef(AliReaderESDTree,1)
 };

#endif
