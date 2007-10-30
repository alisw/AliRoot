#include "TKDPDF.h"
#include "TKDNodeInfo.h"

#include "TClonesArray.h"
#include "TTree.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TBox.h"
#include "TGraph.h"
#include "TMarker.h"

ClassImp(TKDPDF)



//_________________________________________________________________
TKDPDF::TKDPDF() :
	TKDTreeIF()
	,TKDInterpolatorBase()
{
// Default constructor. To be used with care since in this case building
// of data structure is completly left to the user responsability.
}

//_________________________________________________________________
TKDPDF::TKDPDF(Int_t npoints, Int_t ndim, UInt_t bsize, Float_t **data) :
	TKDTreeIF(npoints, ndim, bsize, data)
	,TKDInterpolatorBase(ndim)
{
// Wrapper constructor for the TKDTree.

	Build();
}


//_________________________________________________________________
TKDPDF::TKDPDF(TTree *t, const Char_t *var, const Char_t *cut, UInt_t bsize, Long64_t nentries, Long64_t firstentry) :
	TKDTreeIF()
	,TKDInterpolatorBase()
{
// Alocate data from a tree. The variables which have to be analysed are
// defined in the "var" parameter as a colon separated list. The format should
// be identical to that used by TTree::Draw().
//
// 

	TObjArray *vars = TString(var).Tokenize(":");
	fNDim = vars->GetEntriesFast(); fNDimm = 2*fNDim;
	fNSize = fNDim;
	if(fNDim > 6/*kDimMax*/) Warning("TKDPDF(TTree*, const Char_t, const Char_t, UInt_t)", Form("Variable number exceed maximum dimension %d. Results are unpredictable.", 6/*kDimMax*/));
	fBucketSize = bsize;

	Int_t np;
	Double_t *v;
	for(int idim=0; idim<fNDim; idim++){
		if(!(np = t->Draw(((TObjString*)(*vars)[idim])->GetName(), cut, "goff", nentries, firstentry))){
			Warning("TKDPDF(TTree*, const Char_t, const Char_t, UInt_t)", Form("Can not access data for keys %s. Key defined on tree :", ((TObjString*)(*vars)[idim])->GetName() ));
			TIterator *it = (t->GetListOfLeaves())->MakeIterator();
			TObject *o;
			while((o = (*it)())) printf("\t%s\n", o->GetName());
			continue;
		}
		if(!fNpoints){
			fNpoints = np;
			//Info("TKDPDF(TTree*, const Char_t, const Char_t, UInt_t)", Form("Allocating %d data points in %d dimensions.", fNpoints, fNDim));
			fData = new Float_t*[fNDim];
			for(int idim=0; idim<fNDim; idim++) fData[idim] = new Float_t[fNpoints];
			kDataOwner = kTRUE;
		}
		v = t->GetV1();
		for(int ip=0; ip<fNpoints; ip++) fData[idim][ip] = (Float_t)v[ip];
	}
	TKDTreeIF::Build();
	Build();
}

//_________________________________________________________________
TKDPDF::~TKDPDF()
{
}

//_________________________________________________________________
void TKDPDF::Build(Int_t)
{
// Fill interpolator's data array i.e.
//  - estimation points 
//  - corresponding PDF values

	fNTNodes = fNpoints/fBucketSize + ((fNpoints%fBucketSize)?1:0);/*TKDTreeIF::GetNTNodes();*/
	if(!fBoundaries) MakeBoundaries();
	fLambda = 1 + fNDim + (fNDim*(fNDim+1)>>1);
	//printf("after MakeBoundaries() %d\n", memory());
	
	// allocate interpolation nodes
	TKDInterpolatorBase::Build(fNTNodes);

	TKDNodeInfo *node = 0x0;
	Float_t *bounds = 0x0;
	Int_t *indexPoints;
	for(int inode=0, tnode = fNnodes; inode<fNTNodes-1; inode++, tnode++){
		node = (TKDNodeInfo*)(*fTNodes)[inode];
		node->Val()[0] =  Float_t(fBucketSize)/fNpoints;
		bounds = GetBoundary(tnode);
		for(int idim=0; idim<fNDim; idim++) node->Val()[0] /= (bounds[2*idim+1] - bounds[2*idim]);
		node->Val()[1] =  node->Val()[0]/TMath::Sqrt(float(fBucketSize));
		
		indexPoints = GetPointsIndexes(tnode);
		// loop points in this terminal node
		for(int idim=0; idim<fNDim; idim++){
			node->Data()[idim] = 0.;
			for(int ip = 0; ip<fBucketSize; ip++) node->Data()[idim] += fData[idim][indexPoints[ip]];
			node->Data()[idim] /= fBucketSize;
		}
		memcpy(&(node->Data()[fNDim]), bounds, fNDimm*sizeof(Float_t));
	}

	// analyze last (incomplete) terminal node
	Int_t counts = fNpoints%fBucketSize;
	counts = counts ? counts : fBucketSize;
	Int_t inode = fNTNodes - 1, tnode = inode + fNnodes;
	node = (TKDNodeInfo*)(*fTNodes)[inode];
	node->Val()[0] =  Float_t(counts)/fNpoints;
	bounds = GetBoundary(tnode);
	for(int idim=0; idim<fNDim; idim++) node->Val()[0] /= (bounds[2*idim+1] - bounds[2*idim]);
	node->Val()[1] =  node->Val()[0]/TMath::Sqrt(float(counts));

	// loop points in this terminal node
	indexPoints = GetPointsIndexes(tnode);
	for(int idim=0; idim<fNDim; idim++){
		node->Data()[idim] = 0.;
		for(int ip = 0; ip<counts; ip++) node->Data()[idim] += fData[idim][indexPoints[ip]];
		node->Data()[idim] /= counts;
	}
	memcpy(&(node->Data()[fNDim]), bounds, fNDimm*sizeof(Float_t));

	delete [] fBoundaries;
	fBoundaries = 0x0;
}


//_________________________________________________________________
void TKDPDF::DrawNode(Int_t tnode, UInt_t ax1, UInt_t ax2)
{
// Draw node "node" and the data points within.
//
// Observation:
// This function creates some graphical objects
// but don't delete it. Abusing this function may cause memory leaks !

	if(tnode < 0 || tnode >= fNTNodes){
		Warning("DrawNode()", Form("Terminal node %d outside defined range.", tnode));
		return;
	}

	Int_t inode = tnode;
	tnode += fNnodes;
	// select zone of interest in the indexes array
	Int_t *index = GetPointsIndexes(tnode);
	Int_t nPoints = (tnode == 2*fNnodes) ? fNpoints%fBucketSize : fBucketSize;

	// draw data points
	TGraph *g = new TGraph(nPoints);
	g->SetMarkerStyle(7);
	for(int ip = 0; ip<nPoints; ip++) g->SetPoint(ip, fData[ax1][index[ip]], fData[ax2][index[ip]]);

	// draw estimation point
	TKDNodeInfo *node = (TKDNodeInfo*)(*fTNodes)[inode];
	TMarker *m=new TMarker(node->Data()[ax1], node->Data()[ax2], 20);
	m->SetMarkerColor(2);
	m->SetMarkerSize(1.7);
	
	// draw node contour
	Float_t *bounds = GetBoundary(tnode);
	TBox *n = new TBox(bounds[2*ax1], bounds[2*ax2], bounds[2*ax1+1], bounds[2*ax2+1]);
	n->SetFillStyle(0);

	g->Draw("ap");
	m->Draw();
	n->Draw();
	
	return;
}

