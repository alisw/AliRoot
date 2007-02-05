#!/bin/sh
# $Id$

CURDIR=`pwd`
OUTDIR=test_out

rm -fr $OUTDIR
mkdir $OUTDIR
cp $ALICE_ROOT/MUON/.rootrc $ALICE_ROOT/MUON/rootlogon.C $OUTDIR
cd $OUTDIR

FULLPATH="$CURDIR/$OUTDIR"
NEVENTS=100
SEED=1234567

echo "Running simulation  ..."

aliroot -b  >& testSim.out << EOF 
// Uncoment following lines to run simulation with local residual mis-alignment
// (generated via MUONGenerateGeometryData.C macro)
// AliCDBManager* man = AliCDBManager::Instance();
// man->SetDefaultStorage("local://$ALICE_ROOT");
// man->SetSpecificStorage("MUON","local://$ALICE_ROOT/MUON/ResMisAlignCDB");
gRandom->SetSeed($SEED);
AliSimulation MuonSim("$ALICE_ROOT/MUON/Config.C");
MuonSim.SetMakeTrigger("MUON");
MuonSim.SetWriteRawData("MUON");
MuonSim.Run($NEVENTS);
.q
EOF

echo "Removing Digits files ..."
mkdir MUON.Digits
mv MUON.Digits*.root MUON.Digits/ 

echo "Running reconstruction  ..."

aliroot -b >& testReco.out << EOF
gRandom->SetSeed($SEED);
AliMagFMaps* field = new AliMagFMaps("Maps","Maps", 1, 1., 10., AliMagFMaps::k5kG);
AliTracker::SetFieldMap(field, kFALSE);
AliReconstruction MuonRec("galice.root");
MuonRec.SetInput("$FULLPATH/");
MuonRec.SetRunTracking("");
MuonRec.SetRunVertexFinder(kFALSE);
MuonRec.SetRunLocalReconstruction("MUON");
MuonRec.SetFillESD("MUON");
MuonRec.SetLoadAlignData("MUON");
// Uncoment following line to run reconstruction with the orginal tracking method
// instead of the kalman one (default)
// MuonRec.SetOption("MUON","Original");
// Use the following to change clustering method
//MuonRec.SetOption("MUON","MLEM"); // new scheme AZ's clustering
//MuonRec.SetOption("MUON","SIMPLEFIT"); // new scheme simple fitting
//MuonRec.SetOption("MUON","COG"); // new scheme basic center-of-gravity only
MuonRec.Run();
.q
EOF

echo "Running Trigger efficiency  ..."
aliroot -b >& testTriggerResults.out << EOF
.L $ALICE_ROOT/MUON/MUONTriggerEfficiency.C+
MUONTriggerEfficiency("galice.root",1);
.q
EOF

echo "Running efficiency  ..."

aliroot -b >& testResults.out << EOF
.L $ALICE_ROOT/MUON/MUONefficiency.C+
// no argument assumes Upsilon but MUONefficiency(443) works on Jpsi
MUONefficiency();
.q
EOF

if [ "$NEVENTS" -le 20 ]; then

echo "Running dumps ..."

aliroot -b << EOF
.L $ALICE_ROOT/MUON/MUONCheck.C+
MUONdigits(); > check.digits
MUONrecpoints(); > check.recpoints
MUONrectracks(); > check.rectracks
MUONrectrigger(); > check.rectrigger
.q
EOF

fi

echo "Finished"  
echo "... see results in test_out"

cd $CURDIR
