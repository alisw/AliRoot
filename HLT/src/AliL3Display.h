#ifndef ALIL3_Display
#define ALIL3_Display

#include <TObject.h>
#include <TGeometry.h>

#include "AliL3RootTypes.h"

class AliL3SpacePointData;
class AliL3TrackArray;

class AliL3Display : public TObject {

 private:
  TGeometry *fGeom;
  AliL3SpacePointData *fClusters[36][6]; //!
  AliL3TrackArray *fTracks; //!
  UInt_t fNcl[36][6];

  Int_t fMinSlice;
  Int_t fMaxSlice;
  
 public:
  AliL3Display();
  AliL3Display(Int_t *slice, Char_t *gfile="$(LEVEL3)/GEO/alice.geom");
  virtual ~AliL3Display();

  void Setup(Char_t *trackfile,Char_t *path);
  void DisplayTracks(Int_t min_hits=10,Bool_t x3don=kTRUE);
  void DisplayAll(Int_t min_hits=10,Bool_t x3don=kTRUE);
  void DisplayClusters(Bool_t x3don=kTRUE);

  void DisplayClusterRow(Int_t slice,Int_t padrow,Char_t *digitsFile,Char_t *type="hist");
  void SetTracks(AliL3TrackArray *tracks) {fTracks=tracks;}
  
  ClassDef(AliL3Display,1) //Display class
};

#endif
