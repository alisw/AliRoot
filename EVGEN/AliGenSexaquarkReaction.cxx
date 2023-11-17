#include <fstream>
#include <set>
#include <vector>

#include "TDatabasePDG.h"
#include "TF1.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TPDGCode.h"

#include "AliConst.h"
#include "AliGenEventHeader.h"
#include "AliGenSexaquarkReaction.h"
#include "AliLog.h"
#include "AliPDG.h"
#include "AliRun.h"

#define VERBOSE_MODE 0

ClassImp(AliGenSexaquarkReaction);  // integrate class for interactive use in ROOT

/*
 Default constructor
 - Set default kinematic ranges for injected anti-sexaquark
 - Set default radius range for secondary vertex
 - Struck nucleon momentum assigned with simple gaussian
 - Sexaquark mass, reaction channel and number of reactions will be provided in SetDefaultValues()
 */
AliGenSexaquarkReaction::AliGenSexaquarkReaction()
    : AliGenerator(),
      fReactionChannelsMap(),
      fRadiusMin(0),
      fRadiusMax(0),
      fStruckNucleonPDG(0),
      fReactionProductsPDG(0),
      fFermiMomentum(0),
      fFermiMomentumError(0),
      fFermiMomentumModel(0),
      fNReactions(0),
      fNReactionProducts(0),
      fSexaquarkMass(0),
      fReactionChannel(0) {

    SetDefaultNameTitle();

    InitReactionsInfo();
    InitFermiMomentumInfo();

    SetDefaultRanges();
    SetDefaultValues();

    fStruckNucleonPDG = fReactionChannelsMap[fReactionChannel][0][0];
    fReactionProductsPDG = fReactionChannelsMap[fReactionChannel][1];
    fNReactionProducts = (Int_t)fReactionProductsPDG.size();

    SetNumberParticles(fNReactions * fNReactionProducts);
    AliConfig::Instance()->Add(this);
}

/*
 Default constructor
 - Set default kinematic ranges for injected anti-sexaquark
 - Set default radius range for secondary vertex
 - Struck nucleon momentum assigned with simple gaussian
 */
AliGenSexaquarkReaction::AliGenSexaquarkReaction(Int_t n_reactions, Float_t sexaquark_mass, Char_t reaction_channel)
    : AliGenerator(),
      fReactionChannelsMap(),
      fRadiusMin(0),
      fRadiusMax(0),
      fStruckNucleonPDG(0),
      fReactionProductsPDG(0),
      fFermiMomentum(0),
      fFermiMomentumError(0),
      fFermiMomentumModel(0),
      fNReactions(n_reactions),
      fNReactionProducts(0),
      fSexaquarkMass(sexaquark_mass),
      fReactionChannel(reaction_channel) {

    SetDefaultNameTitle();

    InitReactionsInfo();
    InitFermiMomentumInfo();

    SetDefaultRanges();

    fStruckNucleonPDG = fReactionChannelsMap[fReactionChannel][0][0];
    fReactionProductsPDG = fReactionChannelsMap[fReactionChannel][1];
    fNReactionProducts = (Int_t)fReactionProductsPDG.size();

    SetNumberParticles(fNReactions * fNReactionProducts);
    AliConfig::Instance()->Add(this);
}

/*
 Default destructor
*/
AliGenSexaquarkReaction::~AliGenSexaquarkReaction() { delete fFermiMomentumModel; }

/*
 Set default name and title
 */
void AliGenSexaquarkReaction::SetDefaultNameTitle() {
    fName = "AliGenSexaquarkReaction";
    fTitle = "Generator of AntiSexaquark-Nucleon Reactions";
}

/*
 Initialization of the generator
*/
void AliGenSexaquarkReaction::Init() {

    // check consistency of selected ranges
    if (TestBit(kPtRange) && TestBit(kMomentumRange)) {
        Fatal("Init", "You should not set the momentum range and the pt range!\n");
    }
    if ((!TestBit(kPtRange)) && (!TestBit(kMomentumRange))) {
        Fatal("Init", "You should set either the momentum or the pt range!\n");
    }
    if ((TestBit(kYRange) && TestBit(kThetaRange)) || (TestBit(kYRange) && TestBit(kEtaRange)) ||
        (TestBit(kEtaRange) && TestBit(kThetaRange))) {
        Fatal("Init", "You should only set the range of one of these variables: y, eta or theta\n");
    }
    if ((!TestBit(kYRange)) && (!TestBit(kEtaRange)) && (!TestBit(kThetaRange))) {
        Fatal("Init", "You should set the range of one of these variables: y, eta or theta\n");
    }

    PrintParameters();
}

/*
 Set default sexaquark mass, reaction channel and number of reactions
 */
void AliGenSexaquarkReaction::SetDefaultValues() {
    fSexaquarkMass = 1.8;  // GeV/c^2
    fReactionChannel = 'A';
    fNReactions = 1;
}

/*
 Set default kinematic ranges for the anti-sexaquark, and radius range for the interaction vertex
 */
void AliGenSexaquarkReaction::SetDefaultRanges() {
    // anti-sexaquark
    SetPtRange(0., 5.);                // GeV/c
    SetPhiRange(0., 2 * TMath::Pi());  // radians
    SetYRange(-0.8, 0.8);
    // interaction vertex
    SetRadiusRange(5., 180.);  // cm
}

/*
 Initialize reaction channels map of vectors
 */
void AliGenSexaquarkReaction::InitReactionsInfo() {
    fReactionChannelsMap['A'] = {{2112}, {-3122, 310}};             // AntiS + Neutron -> AntiLambda, K0
    fReactionChannelsMap['B'] = {{2112}, {-3122, 310, -211, 211}};  // AntiS + Neutron -> AntiLambda, K0, Pi-, Pi+
    fReactionChannelsMap['C'] = {{2112}, {-2212, 310, 310, 211}};   // AntiS + Neutron -> AntiProton, K0, K0, Pi+
    fReactionChannelsMap['D'] = {{2212}, {-3122, 321}};             // AntiS + Proton -> AntiLambda, K+
    fReactionChannelsMap['E'] = {{2212}, {-3122, 321, -211, 211}};  // AntiS + Proton -> AntiLambda, K+, Pi-, Pi+
    fReactionChannelsMap['F'] = {{2212}, {-2212, 321, 310, 211}};   // AntiS + Proton -> AntiProton, K+, K0, Pi+
    fReactionChannelsMap['G'] = {{2112}, {-3312, -211}};            // AntiS + Neutron -> Xi+, Pi-
    fReactionChannelsMap['H'] = {{2212}, {-2212, 321, 321, 111}};   // AntiS + Proton -> AntiProton, K+, K+, Pi0
}

/*
 Initialise the Fermi momentum information
 */
void AliGenSexaquarkReaction::InitFermiMomentumInfo() {
    // define central value and error, obtained from:
    // -- Povh, Rith, Scholz, Zetsche, Rodejohann.
    //    "Particles and Nuclei: An Introduction to the Physical Concepts".
    //    (Springer, 2015, 7th edition)
    fFermiMomentum = 0.250;       // GeV
    fFermiMomentumError = 0.005;  // GeV

    // define function
    fFermiMomentumModel = new TF1("Fermi Momentum Model", "[0]*exp(-0.5*((x-[1])/[2])**2) ",  //
                                  fFermiMomentum - 5 * fFermiMomentumError, fFermiMomentum + 5 * fFermiMomentumError);
    fFermiMomentumModel->SetParameter(0, 1.);
    fFermiMomentumModel->SetParameter(1, fFermiMomentum);
    fFermiMomentumModel->SetParameter(2, fFermiMomentumError);
}

/*
 Generate n_triggers
 */
void AliGenSexaquarkReaction::GenerateN(Int_t n_triggers) {

    // prepare random numbers
    GetRandom()->SetSeed(0);
    Float_t fRandom[4];

    /* Declaration of variables */

    // anti-sexaquark
    Float_t Pt_S, Px_S, Py_S, Pl_S;
    Float_t Y_S, Phi_S;
    Float_t Mt_S, E_S;
    TLorentzVector Sexaquark;

    // struck nucleon
    Float_t Px_N, Py_N, Pz_N;
    Float_t M_N, E_N;
    TLorentzVector Nucleon;

    // reaction
    Double_t ReactionProductsMasses[fNReactionProducts];
    TGenPhaseSpace ThisPhaseSpace;  // generator of a single n-body reaction with constant cross-section
    TLorentzVector W;
    Float_t weight;

    // secondary vertex
    Float_t Radius_S, Theta_S, Radius3D_S;
    Float_t Vx_S, Vy_S, Vz_S;
    Float_t Beta_S, SecondaryTime;
    TVector3 SecondaryVertex;

    // bookkeeping
    Int_t TrackCounter;
    Int_t ReactionID;

    /* Definitions */

    // primary vertex (taken from AliGenCocktail)
    TVector3 PrimaryVertex(fVertex[0], fVertex[1], fVertex[2]);  // (in cm)
    Float_t PrimaryTime = fTime;                                 // (in ns)

    Int_t mult = n_triggers * fNReactions;

    for (Int_t i = 0; i < mult; i++) {

        // update reaction ID
        ReactionID = 600 + i;

#if VERBOSE_MODE
        AliInfoF("\tSummary of Reaction #%i", i);
        AliInfo("");
#endif

        // get new random numbers
        Rndm(fRandom, 4);

        // 1) Prepare anti-sexaquark
        Pt_S = fPtMin + fRandom[0] * (fPtMax - fPtMin);
        Mt_S = TMath::Sqrt(fSexaquarkMass * fSexaquarkMass + Pt_S * Pt_S);  // transverse mass (derived from inv. mass and Pt)
        Y_S = fYMin + fRandom[1] * (fYMax - fYMin);                         // rapidity (uniform distribution)
        Phi_S = fPhiMin + fRandom[2] * (fPhiMax - fPhiMin);                 // azimuthal angle (uniform distribution) (in radians)
        Px_S = Pt_S * TMath::Cos(Phi_S);                                    // Px
        Py_S = Pt_S * TMath::Sin(Phi_S);                                    // Py
        Pl_S = Mt_S * TMath::SinH(Y_S);                                     // longitudinal momentum = Pz (derived from trans. mass and Y)
        E_S = Mt_S * TMath::CosH(Y_S);                                      // energy (derived from trans. mass and Y)
        Sexaquark.SetPxPyPzE(Px_S, Py_S, Pl_S, E_S);

#if VERBOSE_MODE
        AliInfo("Generated AntiSexaquark:");
        AliInfoF(">> Px,Py,Pz = (%.4f, %.4f, %.4f)", Sexaquark.Px(), Sexaquark.Py(), Sexaquark.Pz());
        AliInfoF(">> M        = %.4f", Sexaquark.M());
        AliInfoF(">> Y        = %.4f", Y_S);
        AliInfoF(">> Theta    = %.4f", Sexaquark.Theta());
        AliInfoF(">> Phi      = %.4f", Phi_S);
#endif

        // 2) Prepare struck nucleon
        Px_N = 0.;
        Py_N = 0.;
        Pz_N = 0.;
        GetFermiMomentum(Px_N, Py_N, Pz_N);
        M_N = TDatabasePDG::Instance()->GetParticle(fStruckNucleonPDG)->Mass();
        E_N = TMath::Sqrt(TMath::Power(Px_N, 2) + TMath::Power(Py_N, 2) + TMath::Power(Pz_N, 2) + TMath::Power(M_N, 2));
        Nucleon.SetPxPyPzE(Px_N, Py_N, Pz_N, E_N);

        // 3) Generate reaction
        // fill masses of reaction products
        for (Int_t j = 0; j < fNReactionProducts; j++) {
            ReactionProductsMasses[j] = TDatabasePDG::Instance()->GetParticle(fReactionProductsPDG[j])->Mass();
        }
        // determine momenta of reaction products
        W = Sexaquark + Nucleon;
        ThisPhaseSpace.SetDecay(W, fNReactionProducts, &ReactionProductsMasses[0]);
        weight = (Float_t)ThisPhaseSpace.Generate();

        // 4) Set secondary vertex
        Radius_S = fRadiusMin + fRandom[3] * (fRadiusMax - fRadiusMin);  // 2D radius (uniform distribution) (in cm)
#if VERBOSE_MODE
        AliInfoF(">> Radius   = %.4f", Radius_S);
#endif
        Theta_S = Sexaquark.Theta();                                                  // polar angle (in radians)
        Vz_S = Radius_S / TMath::Tan(Theta_S);                                        // z, in both cartesian and cylindrical coordinates
        Radius3D_S = TMath::Sqrt(TMath::Power(Radius_S, 2) + TMath::Power(Vz_S, 2));  // 3D Radius -- radius in spherical coordinates
        Vx_S = Radius3D_S * TMath::Sin(Theta_S) * TMath::Cos(Phi_S);                  // x, in cartesian coordinates
        Vy_S = Radius3D_S * TMath::Sin(Theta_S) * TMath::Sin(Phi_S);                  // y, in cartesian coordinates
        SecondaryVertex.SetXYZ(Vx_S, Vy_S, Vz_S);

        // add displacement from PV
        SecondaryVertex = SecondaryVertex + PrimaryVertex;

        Beta_S = Sexaquark.P() / Sexaquark.E();
        SecondaryTime = (SecondaryVertex.Mag() * TMath::Power(10, -7)) / (Beta_S * TMath::Ccgs());  // (in ns)

        /* Print anti-sexaquark and nucleon information */

        AliInfoF("%i,%.8e,%.8e,%.8e,%.8e,%.8e,%.8e",              //
                 ReactionID,                                      //
                 Sexaquark.Px(), Sexaquark.Py(), Sexaquark.Pz(),  //
                 Nucleon.Px(), Nucleon.Py(), Nucleon.Pz());

#if VERBOSE_MODE
        AliInfo("   Vertex            R        Theta          Phi");
        AliInfoF("  Primary %12.7f %12.7f %12.7f", PrimaryVertex.Perp(), PrimaryVertex.Theta(), PrimaryVertex.Phi());
        AliInfoF("Secondary %12.7f %12.7f %12.7f", SecondaryVertex.Perp(), SecondaryVertex.Theta(), SecondaryVertex.Phi());
        AliInfo("");
        AliInfo("  I          PDG           Px           Py           Pz            E            M");
#endif

        // get 4-momentum vectors of the reaction products and put them on the Geant stack
        for (Int_t j = 0; j < fNReactionProducts; j++) {

            // add particle to the stack
            PushTrack(fTrackIt,                          // done: track final state particles
                      -1,                                // parent set to -1 (= primary)
                      fReactionProductsPDG[j],           // PDG code of the product particle
                      ThisPhaseSpace.GetDecay(j)->Px(),  // Px of the product particle (in GeV/c)
                      ThisPhaseSpace.GetDecay(j)->Py(),  // Py of the product particle (in GeV/c)
                      ThisPhaseSpace.GetDecay(j)->Pz(),  // Pz of the product particle (in GeV/c)
                      ThisPhaseSpace.GetDecay(j)->E(),   // energy of the product particle (in GeV)
                      SecondaryVertex.X(),               // vx: x-component of position of interaction vertex (in cm)
                      SecondaryVertex.Y(),               // vy: y-component of position of interaction vertex (in cm)
                      SecondaryVertex.Z(),               // vz: z-component of position of interaction vertex (in cm)
                      SecondaryTime + PrimaryTime,       // vt: time of interaction vertex (in ns)
                      0.,                                // polx: x-component of polarisation vector, 0 by default
                      0.,                                // poly: y-component of polarisation vector, 0 by default
                      0.,                                // polz: z-component of polarisation vector, 0 by default
                      kPHInhelastic,                     // mech: production mechanism (Hadronic Inelastic Scattering)
                      TrackCounter,                      // counter of tracks (needed and incremented internally by AliStack)
                      weight,                            // weight of the event
                      ReactionID);                       // generation status code

#if VERBOSE_MODE
            AliInfoF("%3i %12i %12.7f %12.7f %12.7f %12.7f %12.7f", TrackCounter, fReactionProductsPDG[j], ThisPhaseSpace.GetDecay(j)->Px(),
                     ThisPhaseSpace.GetDecay(j)->Py(), ThisPhaseSpace.GetDecay(j)->Pz(), ThisPhaseSpace.GetDecay(j)->E(),
                     ThisPhaseSpace.GetDecay(j)->M());
#endif
        }

#if VERBOSE_MODE
        AliInfo("");
#endif
    }  // end of loop

    /* Copied from AliGenBox */

    AliGenEventHeader* header = new AliGenEventHeader("SEXAQUARK");
    header->SetPrimaryVertex(fVertex);
    header->SetNProduced(n_triggers * fNReactions * fNReactionProducts);
    header->SetInteractionTime(fTime);

    // passes header either to the container or to gAlice
    if (fContainer) {
        header->SetName(fName);
        fContainer->AddHeader(header);
    } else {
        gAlice->SetGenEventHeader(header);
    }
}

/*
 Generate a single trigger
 */
void AliGenSexaquarkReaction::Generate() { GenerateN(1); }

/*
 Print the parameters of the current instance
*/
void AliGenSexaquarkReaction::PrintParameters() {

    AliInfo("> Initializing AliGenSexaquarkReaction");
    AliInfo("");
    AliInfoF("  Number of Reactions = %i", fNReactions);
    AliInfoF("  Chosen Reaction Channel : %c", fReactionChannel);
    AliInfoF("  * Nucleon : %i", fStruckNucleonPDG);
    TString ReactionProducts_str = "{";
    for (Int_t p = 0; p < fNReactionProducts; p++) {
        ReactionProducts_str += Form("%i", fReactionProductsPDG[p]);
        if (p + 1 != fNReactionProducts) ReactionProducts_str += ", ";
    }
    ReactionProducts_str += "}";
    AliInfoF("  * Reaction Products : %s", ReactionProducts_str.Data());
    AliInfo("");
    AliInfo("  Injected AntiSexaquark:");
    AliInfoF("  * M = %f (GeV/c^2)", fSexaquarkMass);
    AliInfoF("  * %f < Pt < %f (GeV/c)", fPtMin, fPtMax);
    AliInfoF("  * %f < Phi < %f (rad)", fPhiMin, fPhiMax);
    AliInfoF("  * %f < Y < %f", fYMin, fYMax);
    AliInfo("");
    AliInfo("  Secondary Vertex:");
    AliInfoF("  * %f < Radius < %f (cm)", fRadiusMin, fRadiusMax);
    AliInfo("");
}

/*
 Based on the current model for the Fermi momentum, get a random value
 - uses: fFermiMomentum, fFermiMomentumError, fFermiMomentumModel
 - output: Px, Py, Pz
 */
void AliGenSexaquarkReaction::GetFermiMomentum(Float_t& Px, Float_t& Py, Float_t& Pz) {

    Float_t P_N = fFermiMomentumModel->GetRandom();
    // given the total momentum, set phi,theta as uniform and uncorrelated variables
    Float_t random_N[2];
    Rndm(random_N, 2);
    Float_t Phi_N = 2 * TMath::Pi() * random_N[0];  // azimuthal angle (uniform distribution) (in radians)
    Float_t Theta_N = TMath::Pi() * random_N[1];    // polar angle (uniform distribution) (in radians)
    // then, assign px,py,pz
    Px = P_N * TMath::Cos(Phi_N) * TMath::Sin(Theta_N);
    Py = P_N * TMath::Sin(Phi_N) * TMath::Sin(Theta_N);
    Pz = P_N * TMath::Cos(Theta_N);
}
