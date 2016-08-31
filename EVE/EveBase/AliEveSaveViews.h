//
//  AliEveSaveViews.h
//
//
//  Created by Jeremi Niedziela on 16/03/15.
//
//

#ifndef __AliRoot__AliEveSaveViews__
#define __AliRoot__AliEveSaveViews__

#include <AliESDEvent.h>

#include <TString.h>
#include <TASImage.h>

#include <vector>


class AliEveSaveViews
{
public:
    AliEveSaveViews(int width=3840,int height=2160);
    ~AliEveSaveViews();
    // 3,840 × 2,160 (4K)
    // 7,680 × 4,320 (8K)
    // 15,360 × 8,640 (16K)
    
    void SaveForAmore();
    void Save(bool withDialog=true,char* filename="");
    int SendToAmore();
private:
    TASImage* GetPicture(int width, int heigth, int height3DView, bool projections=true);
    void ChangeRun();
    
    void BuildEventInfoString();
    void BuildTriggerClassesStrings();
    void BuildClustersInfoString();
    
    int fRunNumber;
    int fCurrentFileNumber;
    int fMaxFiles;
    int fNumberOfClusters;
    TString fCompositeImgFileName;
    
    AliESDEvent *fESDEvent;
    
    // images and dimensions
    int fHeightInfoBar;     // height of the Information bar
    int fWidth;             // width of resulting image
    int fHeight;            // height of resulting image
        
    // strings with additional info
    TString fEventInfo;
    TString fTriggerClasses[3];
    TString fClustersInfo;
    
    // info from logbook
    std::vector<int> fCluster;
    std::vector<ULong64_t> fInputDetectorMask;
    
    AliEveSaveViews(const AliEveSaveViews&);
    AliEveSaveViews& operator=(const AliEveSaveViews&);
};

#endif
