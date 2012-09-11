#! bash
# How to create the badchannels root files with histogramas SM by SM.
# First argument of the macro = run number(normally, one can use any number within the specific period)
# Second argument=Filename.root (this will be used in the CreateEMCAL_OADB_BadChannels.C)

aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(146860,"BadChannels2011_11a.root")'  # Run144871_146860_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(150629,"BadChannels2011_11b.root")'  # Run148531_150629_v5_s0.root 
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(152700,"BadChannels2011_11c1.root")' # Run151636_155384_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(153570,"BadChannels2011_11c2.root")' # Run153570_154733_v5_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(155384,"BadChannels2011_11c3.root")' # Run151636_155384_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(159635,"BadChannels2011_11d.root")'  # Run156477_159635_v7_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(162740,"BadChannels2011_11e.root")'  # Run160676_162740_v6_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(165745,"BadChannels2011_11f.root")'  # Run162933_165746_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(166530,"BadChannels2011_11h.root")'  # Run166529_170673_v5_s0.root

aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(172439,"BadChannels2012_12a1.root")' # Run172439_177296_v2_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(176327,"BadChannels2012_12a2.root")' # Run176326_177295_v3_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177381,"BadChannels2012_12b1.root")' # Run177320_999999999_v2_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177384,"BadChannels2012_12b2.root")' # Run177384_178220_v3_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177444,"BadChannels2012_12b3.root")' # Run177444_177682_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(177845,"BadChannels2012_12b4.root")' # Run177844_177849_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(179570,"BadChannels2012_12c1.root")' # Run179569_182744_v3_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(180128,"BadChannels2012_12c2.root")' # Run180127_180194_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(180196,"BadChannels2012_12c3.root")' # Run180195_180200_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(180290,"BadChannels2012_12c4.root")' # Run180289_182740_v4_s0.root

aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(183914,"BadChannels2012_12d1.root")' # Run183913_184389_v4_s0.root 
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(184390,"BadChannels2012_12d2.root")' # Run183913_186320_v3_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(185848,"BadChannels2012_12d3.root")' # Run185847_185908_v4_s0.root
aliroot -b -q 'AliEMCALOCDBTenderConverter.cxx(185910,"BadChannels2012_12d4.root")' # Run185909_186035_v4_s0.root

# 12a --> 177295 172440
# 12b --> 178220 177381
# 12c --> 182744 179569
# 12d --> 186320 183913	
