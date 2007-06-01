//---------------------------------------------------------------------------------
// The AliKFVertex class
// .
// @author  S.Gorbunov, I.Kisel
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store primary and secondary vertices.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in AliKFParticleBase
// 
//  -= Copyright &copy ALICE HLT Group =-
//_________________________________________________________________________________

#ifndef ALIKFVERTEX_H
#define ALIKFVERTEX_H

#include "AliKFParticle.h"
#include "AliESDVertex.h"


class AliKFVertex :public AliKFParticle
{
  
 public:

  //*
  //*  INITIALIZATION
  //*

  //* Constructor (empty)

  AliKFVertex():AliKFParticle(){} 

  //* Destructor (empty)

  ~AliKFVertex(){}

  //* Initialisation from ESD vertex 

  AliKFVertex( const AliESDVertex &vertex );

  //* Copy vertex part to ESD vertex 

  void CopyToESDVertex( AliESDVertex &Vtx ) const ;


  //* 
  //* CONSTRUCTION OF THE VERTEX BY ITS DAUGHTERS 
  //* USING THE KALMAN FILTER METHOD
  //*


  //* Simple way to construct vertices ex. D0 = Pion + Kaon;   

  void operator +=( const AliKFParticle &Daughter );  

  //* Subtract particle from vertex

  AliKFVertex operator -( const AliKFParticle &Daughter ) const;

  void operator -=( const AliKFParticle &Daughter );  

  //* Construct vertex with selection of tracks (primary vertex)

  void ConstructPrimaryVertex( const AliKFParticle *vDaughters[], int NDaughters,
			       Bool_t vtxFlag[], Double_t ChiCut=3.5  );

  ClassDef( AliKFVertex, 1 );

};



//---------------------------------------------------------------------
//
//     Inline implementation of the AliKFVertex methods
//
//---------------------------------------------------------------------

inline void AliKFVertex::CopyToESDVertex( AliESDVertex &v ) const 
{
  AliKFVertex vTmp=*this;
  v = AliESDVertex( vTmp.fP, vTmp.fC, vTmp.fChi2, (vTmp.fNDF +3)/2 );
}

inline void AliKFVertex::operator +=( const AliKFParticle &Daughter )
{
  AliKFParticle::operator +=( Daughter );
}
  

inline void AliKFVertex::operator -=( const AliKFParticle &Daughter )
{
  Daughter.SubtractFromVertex( *this );
}
  
inline AliKFVertex AliKFVertex::operator -( const AliKFParticle &Daughter ) const 
{
  AliKFVertex tmp = *this;
  Daughter.SubtractFromVertex( tmp );
  return tmp;
}


#endif 
