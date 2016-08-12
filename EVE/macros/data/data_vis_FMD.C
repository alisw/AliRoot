/**
 * @file   data_vis_FMD.C
 * @author Christian Holm Christensen <cholm@nbi.dk>
 * @date   Thu Aug 11 17:03:06 2016
 * 
 * @brief  Event display visualisation of FMD data 
 * 
 * The code here uses the compiled class AliEveFMDLoader for
 * _everything_.  The class defines a singleton object that makes sure
 * we only initialize internal variables once per process.
 */

#if !defined(__CINT__) || defined(__MAKECINT__)
# include <TStyle.h>
# include <AliEveEventManager.h>
# include <AliEveFMDLoader.h>
#endif

/** 
 * Visualize FMD data in an event display.  
 * 
 * @param type The type of data to visualize.  Currently only raw,
 * simulated hits, or reconstructed data (ESD) is supported.  The
 * class AliEveFMDLoader does however also support (s)digits.
 */
void data_vis_FMD(AliEveEventManager::EDataType type)
{
  AliEveFMDLoader::Instance()->Visualize(type);
}
/** 
 * Visualize raw FMD Data 
 */
void data_vis_FMD_raw()
{
  AliEveFMDLoader::Instance()->Visualize(AliEveEventManager::kRaw);
}
/** 
 * Visualize simulated hits in the FMD
 */
void data_vis_FMD_hits()
{
  AliEveFMDLoader::Instance()->Visualize(AliEveEventManager::kHits);
}
/** 
 * Visualize reconstructed data (ESD) in the FMD
 */
void data_vis_FMD_esd()
{
    AliEveFMDLoader::Instance()->Visualize(AliEveEventManager::kESD);
}

//
// EOF
//
