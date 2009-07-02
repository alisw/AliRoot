#ifndef ALI_DCS_ARRAY_H
#define ALI_DCS_ARRAY_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This class represents the value(s) of a the LHC DPs at a given timestamp   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TString.h>
#include <TTimeStamp.h>

class AliDCSArray : public TObject {
 public:
	enum Type {
		kInvalid = 0,
		kBool = 1,
		kChar = 2,
		kInt = 3,
		kUInt = 4,
		kFloat = 5,
		kString = 6
	};
	
	AliDCSArray();
	AliDCSArray(const AliDCSArray& c);
	
	virtual ~AliDCSArray();
	
	AliDCSArray& operator=(const AliDCSArray& c);
	
	AliDCSArray(Int_t nentries, Bool_t* value, TTimeStamp* timeStamp);
	AliDCSArray(Int_t nentries, Char_t* value, TTimeStamp* timeStamp);
	AliDCSArray(Int_t nentries, Int_t* value, TTimeStamp* timeStamp);
	AliDCSArray(Int_t nentries, UInt_t* value, TTimeStamp* timeStamp);
	AliDCSArray(Int_t nentries, Float_t* value, TTimeStamp* timeStamp);
	AliDCSArray(Int_t nentries, TString* value, TTimeStamp* timeStamp);
	
	Int_t GetNEntries() const { return fnentries;}
	Bool_t* GetBool() const { return fBool; }
	Char_t* GetChar() const { return fChar; }
	Int_t* GetInt() const { return fInt; }
	UInt_t* GetUInt() const { return fUInt; }
	Float_t* GetFloat() const { return fFloat; }
	TString* GetString() const { return fString; }
	
	Type GetType() const { return fType; }
	
	TTimeStamp* GetTimeStamp() const { return fTimeStamp; }
	void SetTimeStamp(TTimeStamp* timeStamp) { fTimeStamp = timeStamp; }
	
 protected:
	
	void Init();
	
	Type fType;            // type of the value stored
	
	Int_t fnentries;       // n. of entries at the same timestamp
	Bool_t* fBool;         //[fnentries] bool value
	Char_t* fChar;         //[fnentries] char value
	Int_t* fInt;           //[fnentries] int value
	UInt_t* fUInt;         //[fnentries] uint value
	Float_t* fFloat;       //[fnentries] float value
	TString* fString;      //[fnentries] string value
	
	TTimeStamp* fTimeStamp;    // timestamp of this value
	
	ClassDef(AliDCSArray, 1);
};

#endif
