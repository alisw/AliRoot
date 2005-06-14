////////////////////////////////////////////////////////////////////////////////
//
// Author: Artur Szostak
// Email:  artur@alice.phy.uct.ac.za | artursz@iafrica.com
//
////////////////////////////////////////////////////////////////////////////////

#ifndef dHLT_CLUSTERING_CLUSTER_FINDER_HPP
#define dHLT_CLUSTERING_CLUSTER_FINDER_HPP

#include "BasicTypes.hpp"
#include "Cluster.hpp"
#include "ADCStream.hpp"
#include "Utils.hpp"


namespace dHLT {
    namespace Clustering {


	class ClusterFinder;


	class ClusterFinderCallback {
	  public:

	    /* Called when the cluster finder has found all clusters in the ADC stream.
	       At this point the ADC stream is no longer is use by the cluster finder and
	       the stream can be released.
	       The numberfound parameter indicated how many clusters were actually found.
	     */
	    virtual void FoundClusters(ClusterFinder * clusterfinder,
				       const UInt numberfound) = 0;

	    /* Called when the cluster finder has finished its job however no clusters were
	       found in the ADC stream. At this point the ADC stream is no longer is use by
	       the cluster finder and the stream can be released.
	     */
	    virtual void NoClustersFound(ClusterFinder * clusterfinder) =
		0;

	};


	class ClusterFinder {
	  public:

	    ClusterFinder() {
		callback = NULL;
	    };

	    /* This is the starting point of the cluster finding algorithm.
	       Deriving cluster finders should implement all processing in this method
	       to find clusters in the specified ADC stream. When all clusters are found
	       the FoundClusters method should be called to indicate that processing is
	       complete. If no clusters could be found then call NoClustersFound instead.
	     */
	    virtual void FindClusters(const ADCStream * stream) = 0;

	    /* After a call to FoundClusters this method will be called to retreive the
	       cluster points. The clusters array should be filled consecutively with 
	       the points that were found. However no more than 'arraysize' number of 
	       points should be written to the clusters array.
	       This method should also return the actual number of cluster points written
	       to the array.
	       If the number of clusters written is less that the number specified in the
	       'numberfound' parameter of the FoundClusters method, then this method will
	       be called again by the framework. Thus on successive calls to this method,
	       the cluster finder must resume writing clusters at the point it stopped at
	       in the previous call to FillClusterData.
	     */
	    virtual UInt FillClusterData(ClusterPoint * clusters,
					 const UInt arraysize) = 0;

	    /* This is called when the cluster finder should be reset to an initial state.
	       All extra internal memory allocated during processing should be released.
	     */
	    virtual void Reset() = 0;

	    /* Sets the ClusterFinderCallback callback interface.
	     */
	    inline void SetCallback(ClusterFinderCallback * callback) {
		this->callback = callback;
	    };

	  protected:

	    /* Called by the cluster finder algorithm when all clusters in the ADC stream
	       were found. At this point the ADC stream should be considered released and
	       the memory block MUST NOT be accessed.
	       The numberfound parameter should indicate how many clusters were found.
	     */
	     inline void FoundClusters(const UInt numberfound) {
		Assert(callback != NULL);
		callback->FoundClusters(this, numberfound);
	    };

	    /* When the cluster finding algorithm is finished processing but no clusters
	       could be found then this method should be called. At this point the ADC stream
	       should be considered released and the memory block MUST NOT be accessed.
	     */
	    inline void NoClustersFound() {
		Assert(callback != NULL);
		callback->NoClustersFound(this);
	    };

	  private:

	    ClusterFinderCallback * callback;
	};


    }				// Clustering
}				// dHLT

#endif				// dHLT_CLUSTERING_CLUSTER_FINDER_HPP
