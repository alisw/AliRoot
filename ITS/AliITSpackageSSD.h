#ifndef ALIITSPACKAGESSD_H
#define ALIITSPACKAGESSD_H
//Class describing set of AliITSoneSideClusterSSDs, which contact each other.
//Piotr Krzysztof Skowronski
//Warsaw University of Technology
//skowron@if.pw.edu.pl
//
#include "TObject.h"
#include "TArrayI.h"

//#include "AliITSclusterSSD.h"

class AliITSclusterSSD;

//-------------------------------------------------------------------

class AliITSpackageSSD : public TObject
{
 public:
  AliITSpackageSSD(); 

  AliITSpackageSSD(Int_t len, TClonesArray *clustersP, TClonesArray *clustersN);  
  AliITSpackageSSD(TClonesArray *clustersP, TClonesArray *clustersN);	

  ~AliITSpackageSSD();
  
  AliITSpackageSSD(const AliITSpackageSSD &package); 
  AliITSpackageSSD&  operator=( const AliITSpackageSSD & package);
  //Add cluster in side N, clindex is an index of this cluster in TClonesArray
  
  void     AddNSideCluster(Int_t clindex) {(*fClusterNIndexes)[fNclustersN++]=clindex;} 
  void     AddPSideCluster(Int_t clindex) {(*fClusterPIndexes)[fNclustersP++]=clindex;} 

  void     AddCluster(Int_t clindex, Bool_t side){
	      (side)?(*fClusterNIndexes)[fNclustersN++]= 
                   clindex:(*fClusterPIndexes)[fNclustersP++]=clindex;}
  	
//Returns index of one side cluster in TClonesArray, NOT AliITSclusterSSD
  Int_t    GetNSideClusterIdx(Int_t index) const; //input index is number of cluster in this package 
  Int_t    GetPSideClusterIdx(Int_t index) const; //returns index in TClonesArray
  Int_t    GetClusterIdx(Int_t index,Bool_t side) const 
	        {return (side)?GetPSideClusterIdx(index):GetNSideClusterIdx(index);}
 
  AliITSclusterSSD*    GetNSideCluster(Int_t index);
  AliITSclusterSSD*    GetPSideCluster(Int_t index); //index is 
  AliITSclusterSSD*    GetCluster(Int_t index, Bool_t side)
	        {return (side)?GetPSideCluster(index):GetNSideCluster(index);}
			
  Int_t    GetNextPIdx(Int_t OI) const; //Returns index of next P cluster in package; OI == Original Inedx (in TClonesArray)
  Int_t    GetPrvPIdx(Int_t OI) const;  //Returns index of previous P cluster in package; OI == Original Inedx (in TClonesArray)
  Int_t    GetNextNIdx(Int_t OI) const; //Returns index of next N cluster in package; OI == Original Inedx (in TClonesArray)
  Int_t    GetPrvNIdx(Int_t OI) const;  //Returns index of previous N cluster in package; OI == Original Inedx (in TClonesArray)
  
  Int_t    GetNumOfClustersN() const {return fNclustersN;}  
  Int_t    GetNumOfClustersP() const {return fNclustersP;}   
  Int_t    GetNumOfClusters() const {return fNclustersP+fNclustersN;}  
  Int_t    GetNumOfClusters(Bool_t side) const {return (side)?fNclustersP:fNclustersN;}
  
  //returns number of clusters belonging to package,
  //that crosses with only one cluster on the other side 
  //There might be only 2 such a clusters:
  //on the begining and on the end of the package
  //
  Bool_t    GetClusterWithOneCross(Int_t & index, Bool_t& side );  
  void      DelCluster(Int_t index, Bool_t side);
  void      DelPCluster(Int_t index);
  void      DelNCluster(Int_t index);
  void      DelClusterOI(Int_t index, Bool_t side); //OI indicates that it is Original Index
  void      DelPClusterOI(Int_t index); //it means value in look up table
  void      DelNClusterOI(Int_t index);  
  
  void      SplitPackage(Int_t pi, Int_t ni, AliITSpackageSSD* pkg);
  void      GetAllCombinations(Int_t**array,Int_t &num,Int_t size);
  
  void      ConsumeClusters();
  void      PrintClusters();
  
protected:
  
  TClonesArray *fClustersN;   //Pointer to array of clusters - only to have direct acces to 
  TClonesArray *fClustersP;   //clusters
  Int_t    fNclustersN;       // number of N clusters
  Int_t    fNclustersP;       // number of P cluster
  TArrayI *fClusterNIndexes;  // array of pointers to the N clusters
  TArrayI *fClusterPIndexes;  // array of pointers to the P clusters

  static const Bool_t fgkSIDEP;  // boolean for P side 
  static const Bool_t fgkSIDEN;  // boolean for N side
  static const Int_t fgkDebug=0;    // debugging flag


/***************/
/*   Methods   */
/***************/

  
  void    MakeCombin(Int_t**arr, Int_t& nu, Int_t np, Int_t *occup,Int_t size);
  Bool_t  IsFree(Int_t idx, Int_t nn, const Int_t *lis) const;
  

		 
private:
		 
//-------------------------------------------------------		 
  ClassDef(AliITSpackageSSD,1)	
};

#endif
