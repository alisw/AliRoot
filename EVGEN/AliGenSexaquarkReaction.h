#ifndef ALIGENSEXAQUARKREACTION_H
#define ALIGENSEXAQUARKREACTION_H

/* Copyright(c) 1998-2023, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliGenerator.h"

/*
 Injector of the products of AntiSexaquark-Nucleon interactions
 - anti-sexaquark kinematics under a flat distribution
 - radius of secondary vertex chosen from a flat distribution
 - struck nucleon momentum assigned with simple gaussian
 -- based on /EVGEN/AliGenBox[.cxx/.h]
 -- author: aborquez@cern.ch
 */
class AliGenSexaquarkReaction : public AliGenerator {

    /* Copied from AliGenBox */

   public:
    AliGenSexaquarkReaction();
    AliGenSexaquarkReaction(Int_t n_reactions, Float_t sexaquark_mass, Char_t reaction_channel);
    virtual ~AliGenSexaquarkReaction();
    virtual void GenerateN(Int_t n_triggers);
    virtual void Generate();
    virtual void Init();
    virtual void SetSeed(UInt_t /*seed*/) { ; }

    /* Sexaquark */

   public:
    void SetDefaultRanges();
    void SetDefaultValues();
    void SetDefaultNameTitle();
    /*
     Set minimum and maximum radius for uniform distribution to locate interaction (or secondary) vertex
     */
    void SetRadiusRange(Float_t radiusMin, Float_t radiusMax) {
        fRadiusMin = radiusMin;
        fRadiusMax = radiusMax;
    };
    void InitReactionsInfo();
    void PrintParameters();

   private:
    // PDG codes of the struck nucleon and the products of the current reaction
    std::map<Char_t, std::vector<std::vector<Int_t>>> fReactionChannelsMap;
    Float_t fRadiusMin;                       // minimum radius
    Float_t fRadiusMax;                       // maximum radius
    Int_t fStruckNucleonPDG;                  // PDG code of struck nucleon
    std::vector<Int_t> fReactionProductsPDG;  // vector of PDG codes of reaction products
    Int_t fNReactionProducts;                 // number of product particles per reaction

    /* Fermi Momentum */

   public:
    void InitFermiMomentumInfo();
    void GetFermiMomentum(Float_t &px, Float_t &py, Float_t &pz);

   private:
    Float_t fFermiMomentum;       // central value of Fermi Momentum
    Float_t fFermiMomentumError;  // sigma value of Fermi Momentum
    TF1 *fFermiMomentumModel;

    /* Input Options */

   private:
    Int_t fNReactions;        // input, number of reactions
    Float_t fSexaquarkMass;   // hypothesized sexaquark mass
    Char_t fReactionChannel;  // single letter to determine reaction channel, see InitReactionsInfo()

    ClassDef(AliGenSexaquarkReaction, 3);  // integrate class for interactive use in ROOT
};

#endif
