/***************************************************************************
                          groupingassistant.h  -  description
                             -------------------
    begin                : Mon Dec 22 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUPINGASSISTANT_H
#define GROUPINGASSISTANT_H

//include files for the application
#include "data.h"
#include "array.h"
#include "types.h"

#include <QList>


/**
 * Evaluates the fit of the CEM algorithm by computing the probability
 * of missclassification.
 * All the features are use, all PCs except the time.
 * Classifies points in the features as belonging to a mixture of Gaussians
 * which are specified by the example data features and list of clusters.
 *@author Lynn Hazan
 * @since klusters 1.1
 */

class GroupingAssistant {
public:
    explicit GroupingAssistant();
    ~GroupingAssistant();

    /**
 * Evaluates the fit of the CEM algorithm by computing the probability
 * of missclassification.
 * Computes the mean probabilities that spike of cluster c1 actually belongs to c2.
 * @param clusteringData object containing all the document data.
 * @param clusterList output paramater to return the list of clusters computed sorted
 * like in the return array.
 * @param computedClusterList output paramater to return the list of actually computed clusters sorted
 * like in the return array.
 * @param ignoreClusterIndex list of the indexes of the clusters which where not computed, either
 * because they do not have enough spikes or their determinant could not be calculated (their covariance matrix is not positive definite.)
 * @return nbSpikes x nbClusters array giving the posterior
 * probabilities of belonging to each cluster of Fet2, for each point of Fet1.
 */
    Array<double>* computeMeanProbabilities(Data& clusteringData,QList<int>& clusterList,QList<int>& computedClusterList,
                                            QList<int>& ignoreClusterIndex);

    /**Asks the GroupingAssistant to stop his work as soon as possible.*/
    inline void stopComputing(){haveToStopComputing = true;}

private:
    /**Array containing the covariances of the clusters computed.*/
    Array<double> covariances;

    /**Array containing the means of the clusters computed.*/
    Array<double> means;

    /**
  * Copy of the @ref Data::spikesByCluster, a two line array which contains sorted by cluster numbers:
  * the row index of the spike in features array.
  * the id of the cluster.
  */
    SortableTable* spikesByCluster;

    /**Copy of the @ref Data::clusterInfoMap, contains ClusterInfo(s)
  * key: cluster number
  * value: a ClusterInfo (which gives:
  * the index of the first spike in spikesByCluster and the number of spikes for a given cluster).
  */
    Data::ClusterInfoMap* clusterInfoMap;

    /**True if the cluster 1 is among the clusters to compute, false otherwise.*/
    bool existCluster1;

    /**Index of the first cluster while looping on all the clusters.*/
    int initIndex;

    /**True if has been asked to stop the computation, false otherwise.*/
    bool haveToStopComputing;

    /**
 * Computes an array giving the posterior probabilities of belonging
 * to each cluster, for each spike.
 * @param clusteringData object containing all the document data.
 * @param clusterList output paramater to return the list of clusters computed sorted
 * like in the return array.
 * @param computedClusterList output paramater to return the list of actually computed clusters sorted
 * like in the return array.
 * @param ignoreClusterIndex list of the indexes of the clusters which where not computed, either
 * because they do not have enough spikes or their determinant could not be calculated (their covariance matrix is not positive definite.)
 * @return nbSpikes x nbClusters array giving the posterior
 * probabilities of belonging to each cluster of Fet2, for each point of Fet1.
 */
    Array<double>* computeProbabilities(Data& clusteringData,QList<int>& clusterList,QList<int>& computedClusterList,
                                        QList<int>& ignoreClusterIndex);

    /**
  * Computes a Cholesky Decomposition.
  * The part of the covariances array for the cluster @p clusterIndex,
  * provides the upper triangle of input matrix (in(clusterIndex,i*dimension + j) >0 if j>=i)
  * which is the top half of a symmetric matrix.
  * @param out provides lower triange of output matrix (out(i,j) >0 if j<=i)
  * such that out' * out = in.
  * @param nbDimensions number of dimensions.
  * @param clusterIndex index of the current cluster.
  * @return 0 if OK, 1 if matrix is not positive definite.
  */
    int cholesky(Array<double>& out,int nbDimensions,int clusterIndex);

    /**Computes the means and the covariances.
  * @param nbClusters number of clusters.
  * @param nbDimensions number of dimensions.
  * @param nbSpikes total number of spikes.
  * @param clusteringData object containing all the document data.
  * @param ignoreClusterIndex list of the indexes of the clusters which where not computed, either
  * because they do not have enough spikes or their determinant could not be calculated (their covariance matrix is not positive definite.)
  */
    void meanCovarianceComputation(int nbClusters,int nbDimensions,dataType nbSpikes,Data& clusteringData,QList<int>& ignoreClusterIndex);

};

#endif
