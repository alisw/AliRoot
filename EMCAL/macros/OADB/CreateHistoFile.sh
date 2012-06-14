#! bash
# How to create the badchannels root files with histogramas SM by SM.
# First argument of the macro = run number(normally, one can use any number within the specific period)
# Second argument=Filename.root (this will be used in the CreateEMCAL_OADB_BadChannels.C)
# Carefull, one needs the current OCDB downloaded in $ALICE_ROOT/OCDB/EMCAL/Calib/Pedestals(Default) or any ./OCDB/EMCAL/Calib/Pedestals specified (see AliEMCALOCDBTenderConverter.cxx)

# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(146860,"BadChannels2011_11a.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(150629,"BadChannels2011_11b.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(152700,"BadChannels2011_11c1.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(153570,"BadChannels2011_11c2.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(155384,"BadChannels2011_11c3.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(159635,"BadChannels2011_11d.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(162740,"BadChannels2011_11e.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(165745,"BadChannels2011_11f.root")'
# aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(166530,"BadChannels2011_11h.root")'


aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(172439,"BadChannels2012_12a.root")'
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177384,"BadChannels2012_12b1.root")'
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177444,"BadChannels2012_12b2.root")'
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177844,"BadChannels2012_12b3.root")'

