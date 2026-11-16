#! /bin/sh
#
# Build a PAR archive for local or grid analysis.
#

EDIR=TPCcalibPar

mkdir $EDIR


SRC=$ALICE_ROOT/TPC
echo Source $SRC  
echo EDIR $EDIR
cp $SRC/AliTPCFitPad*               $EDIR
cp $SRC/AliTPCCal*.h                $EDIR
cp $SRC/AliTPCCal*.cxx              $EDIR
cp $SRC/AliTPCcal*.h                $EDIR
cp $SRC/AliTPCcal*.cxx              $EDIR
cp $SRC/AliTPCSel*.cxx              $EDIR
cp $SRC/AliTPCSel*.h                $EDIR
cp $SRC/AliTPC*Ana*.*               $EDIR
cp $SRC/TPCcalibLinkDef.h           $EDIR
cp $SRC/Makefile.Calib              $EDIR/Makefile
cp $SRC/Makefile.arch.Calib         $EDIR/Makefile.arch
cp $SRC/libTPCcalib.pkg             $EDIR  


mkdir $EDIR/PROOF-INF
cd $EDIR/PROOF-INF


cp $SRC/BUILDcalib.sh BUILD.sh
cp $SRC/SETUPcalib.C  SETUP.C 


chmod 755 BUILD.sh

cd ../..

tar zcvf TPCcalibPar.par $EDIR


exit 0
