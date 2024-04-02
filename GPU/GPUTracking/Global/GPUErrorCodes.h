//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUErrorCodes.h
/// \author David Rohr

// Error Codes for GPU Tracker
// Denotes: ERROR_NUMBER, ERROR_STRING, [Parameter 1, Parameter 2, Parameter 3] (SectorRow = Sector * 1000 + Row, same for SectorCRU and SectorEndpoint)
GPUCA_ERROR_CODE(0, ERROR_NONE)

// All overflow errors will not stop the processing, but the part of the data that would be written beyond the max buffer size is discarded.
GPUCA_ERROR_CODE(1, ERROR_ROWSTARTHIT_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(2, ERROR_STARTHIT_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(3, ERROR_TRACKLET_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(4, ERROR_TRACKLET_HIT_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(5, ERROR_TRACK_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(6, ERROR_TRACK_HIT_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(7, ERROR_GLOBAL_TRACKING_TRACK_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(8, ERROR_GLOBAL_TRACKING_TRACK_HIT_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(9, ERROR_LOOPER_OVERFLOW)
GPUCA_ERROR_CODE(10, ERROR_MERGER_CE_HIT_OVERFLOW, Value, Max)
GPUCA_ERROR_CODE(11, ERROR_MERGER_LOOPER_OVERFLOW, Value, Max)
GPUCA_ERROR_CODE(12, ERROR_SLICEDATA_FIRSTHITINBIN_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(13, ERROR_SLICEDATA_HITINROW_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(14, ERROR_SLICEDATA_BIN_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(15, ERROR_SLICEDATA_Z_OVERFLOW, Sector, Value)
GPUCA_ERROR_CODE(16, ERROR_MERGER_HIT_OVERFLOW, Value, Max)
GPUCA_ERROR_CODE(17, ERROR_MERGER_TRACK_OVERFLOW, Value, Max)
GPUCA_ERROR_CODE(18, ERROR_COMPRESSION_ROW_HIT_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(19, ERROR_LOOPER_MATCH_OVERFLOW, Value, Max)
GPUCA_ERROR_CODE(20, ERROR_CF_PEAK_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(21, ERROR_CF_CLUSTER_OVERFLOW, Sector, Value, Max)
GPUCA_ERROR_CODE(22, ERROR_CF_ROW_CLUSTER_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(23, ERROR_CF_GLOBAL_CLUSTER_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(24, ERROR_DECOMPRESSION_ATTACHED_CLUSTER_OVERFLOW, SectorRow, Value, Max)
GPUCA_ERROR_CODE(25, MAX_OVERFLOW_ERROR_NUMBER) // Overflow errors are detected as errno <= MAX_OVERFLOW_ERROR_NUMBER

GPUCA_ERROR_CODE(26, ERROR_TPCZS_INVALID_ROW, SectorRow)                                  // Data from invalid row is skipped
GPUCA_ERROR_CODE(27, ERROR_TPCZS_INVALID_NADC, SectorCRU, SamplesInPage, SamplesWritten)  // Invalid number of ADC samples in header, existing samples were decoded
GPUCA_ERROR_CODE(28, ERROR_TPCZS_INCOMPLETE_HBF, SectorCRU, PacketCount, NextPacketCount) // Part of HBF is missing, decoding incomplete
GPUCA_ERROR_CODE(29, ERROR_TPCZS_INVALID_OFFSET, SectorEndpoint, Value, Expected)         // Raw page is skipped since it contains invalid payload offset

// #define GPUCA_CHECK_TPCZS_CORRUPTION
