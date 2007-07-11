// $Id$

/*! 

\page README_mapping README mapping
 

See detailed description in ALICE-INT-2003-025.


\section mapping_s1  Graphical User Interface
  
To use the GUI to plot DE segmentation run:

<pre>
new AliMpDEVisu();
</pre>

or

<pre>
new AliMpDEVisu(w, h);
</pre>

if you want to change the size of the GUI window.
Typical value are:
<pre>
  w = 1200, h = 600 for PC
  w = 1000, h = 550 for laptop
</pre>

The GUI allows:
- drawing motif of a slat/quadrant
- search of a given manu (motif) number
- draw the channel number for a given manu number by clicking of the motif in canvas
- write down in log message informations about the given detection element
  * DE Id, DE name, 
  * number of buspatches, manus, manu serials
- option to save log message onto disc

\section mapping_s2 Test macros

<pre>
   cd ../mapping/macro
   root
   root [0] .x testMacroName.C    
                      see available macros below   
</pre>
                   
A set of test macros be run at once by test_suite.pl scripts:
-  test_suite.pl      - run all test macros and compare results with
                        the reference output
-  test_suite_ref.pl  - generates reference output
                       !! this script will overwrite the refence output
                          provided with the source;
                          it should be used only by developers 

Macros included in the test suite:
-  testReadSector.C
-  testReadMotifType.C
-  testGraphics.C        
-  testSectorFind.C
-  testPlaneFind.C
-  testPrintLimits.C         
-  testExistingPads.C    
-  testPadDimensions.C    
-  testSectorPadIterators.C
-  testMotifTypeIterators.C     
-  testNeighboursPadIterator.C  
-  testAnyPadIterators.C  
-  testPadsUp.C
-  testPlaneAreaIterator.C
  
Other macros (not included in the test suite):
-  testAllIndices.C
-  testUpdateGlobalIndices.C
   

\section mapping_s3  Data files format


\subsection mapping_s3_sub1  zones.dat:

Describes layout of zones, rows, row segments, subzones, motifs

<pre>
  SECTOR_DATA
    number of zones  
    number of rows  
    direction of constant pad size (X or Y)
    offset in X direction
    offset in Y direction
  
  ZONE     
    number of zone  
    half legth of pad size in x  
    half legth of pad size in y

  SUBZONE  
    motif id  
    motif type_id       

  ROW_SEGMENT  
    x offset (in number of pads) 
    y offset (in number of pads) 
    row number 
    nof motifs 
    first motif position Id
    step to the next motif position Id (+1 or -1)
</pre>
  

\subsection mapping_s3_sub2  zones_special.dat:

Describes layout of special row segments (with irregular motifs)

<pre>
  SECTOR_SPECIAL_DATA

  MOTIF
    zone id
    motif id  
    motif type_id       

  ROW
    row number
  
  PAD_ROWS
    number of these pad rows in row   
  
  PAD_ROW_SEGMENT
    mumber of pads in the rows segment  
    motif id  
    motif position id
  
  motifX.dat
  ----------
  Describes characteristics of the motif type X

  In lines:
    Berg number
    Kapton number
    Pad number
    Gassi number
</pre>


\subsection mapping_s3_sub3  motifSpecialX.dat

Describes characteristics of the special motif with motif Id X;
the special motif caontains pads of different size

<pre>
  In lines:
    pad index i (in x)
    pad index j (in y)
    half legth of pad size in x  
    half legth of pad size in y
</pre>
  
\subsection mapping_s3_sub4  padPosX.dat

Maps pad numbers used in the motifX.dat files to
the local pad indices (i,j)

<pre>
  In lines:
    Pad number
    pad index i (in x)
    pad index j (in y)
</pre>
  

\subsection mapping_s3_sub4  *.pcb files

Lines starting with # are comments.

<pre>
  SIZES PadSizeX PadSizeY SizeX SizeY (cm)

  MOTIF motifType ix iy
  MOTIF motifType ix iy
  ...
</pre>

where ix, iy are the local coordinates (in pad unit) of the
lower-left corner of the motif (0,0 is the lower-left corner
of the PCB).

PCB *MUST* be described in a rotating way, starting lower-left and 
then counter-clockwise, otherwise the manu-to-motif association 
(fixed in the slat definition files) will be wrong.

Note that for "full" PCBs, the SizeX and SizeY are redundant as they could be 
computed from the motif alone (but that serves as a cross-check that the motif 
pattern given is ok). That's not the case for short or rounded PCB though.


\subsection mapping_s3_sub5  *.slat files

A slat is defined by the list of its PCB, described starting 
from the beam and going outward.

One PCB per line, preceded by the keyword PCB
Other lines not matching this syntax are ignored.
After the PCB is the list of manu ids for this PCB.

Example :

<pre>
  PCB X 1-3;24-20;42;44;53
  PCB X 1-14
  PCB Y 100-90
  PCB Z 1;2;3;4;5;6;7;12;120
</pre>

defines a slat with 4 PCBs : XXYZ

The manu to motif relationship is attached to the fact that we're counting 
counter-clockwise, starting on the lower-left of the PCB. (and the pcb files 
have to follow this convention to defined their motifs, otherwise all 
this won't work).

Note that the definition of the PCBs have to be in files with extension
.pcb (X.pcb, Y.pcb, Z.pcb)

  
\subsection mapping_s3_sub6  DetElemIdToBusPatch.dat

Lines starting with # are comments.

Contains the detection element identifier with the associated buspatch numbers 
and the corresponding DDL identifier.
The link between buspatches and DE's is needed on the rawdata level to identify 
the type of quadrant/slat to get the corresponding mapping.
The DDL id is needed for the rawdata generation only.

To generate this file, the macro MUONGenerateBusPatch.C could be used.


\subsection mapping_s3_sub7  crate.dat
  
Muon trigger electronics configuration file (decoded in class 
AliMUONTriggerCrateStore) directly copy/paste from the ALICE PRR 
ALICE-EN-2003-010. Gives local board number, name, 
crate name it belongs to, slot number, and internal switches 
(used in the algorithm).


\section mapping_s4  Units used
 
Lengths are in centimeters.
 
*/
  
