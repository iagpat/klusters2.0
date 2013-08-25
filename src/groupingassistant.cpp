/***************************************************************************
                          groupingassistant.cpp  -  description
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
//include files for the application
#include "groupingassistant.h"

//Include files for QT
#include <qmap.h>
#include <QList>

//include files for c/c++ libraires.
#include <math.h>
#include <stdio.h>


GroupingAssistant::GroupingAssistant()
    :existCluster1(false),initIndex(1),haveToStopComputing(false)
{
}

GroupingAssistant::~GroupingAssistant()
{
}

Array<double>* GroupingAssistant::computeMeanProbabilities(Data& clusteringData,QList<int>& clusterList,QList<int>& computedClusterList,QList<int>& ignoreClusterIndex){
    Array<double>* probabilities;

    if(haveToStopComputing)
        return new Array<double>(0,0); //We do not care about what is return as it will not be used.

    //Compute the probabilities
    probabilities = computeProbabilities(clusteringData,clusterList,computedClusterList,ignoreClusterIndex);

    Array<double>* errorMatrix = new Array<double>(static_cast<long>(clusterList.size()),static_cast<long>(clusterList.size()));
    errorMatrix->fillWithZeros();

    //Compute "Error matrix" = mean probabilies that spike of cluster c1 actually belongs to c2.
    int nbClusters = clusterList.size();

    Data::ClusterInfoMap::Iterator iterator;
    int clusterIndex = initIndex;
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){

        if(haveToStopComputing)
            break; //We do not care about what is return as it will not be used.

        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        //Check if the current cluster has been ignored
        if(ignoreClusterIndex.contains(clusterIndex) != 0){
            ++clusterIndex;
            continue;
        }

        for(int clusterIndex2 = initIndex; clusterIndex2 <= nbClusters; ++clusterIndex2){
            //Check if the current cluster has been ignore
            if(ignoreClusterIndex.contains(clusterIndex2) != 0)
                continue;
            if(haveToStopComputing)
                break;

            //Accumulate sums for mean calculation
            double sum = 0;
            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);

                sum += (*probabilities)(featuresRowIndex,clusterIndex2);
            }
            (*errorMatrix)(clusterIndex,clusterIndex2) = sum / nbSpikesOfCluster;
        }
        ++clusterIndex;
    }

    //Put zeros on the diagonal of errorMatrix
    for(int clusterIndex = 1; clusterIndex <= nbClusters; ++clusterIndex){
        (*errorMatrix)(clusterIndex,clusterIndex) = 0;
    }

    delete spikesByCluster;
    delete clusterInfoMap;
    delete probabilities;

    return errorMatrix;
}


Array<double>* GroupingAssistant::computeProbabilities(Data& clusteringData,QList<int>& clusterList,QList<int>& computedClusterList,QList<int>& ignoreClusterIndex){
    dataType nbSpikes = clusteringData.totalNbOfSpikes();
    //The number of dimensions includes the number of PCA by channels and the any additionnal features
    //like the Valley to peak amplitude,the peak to valley amplitude,the max of the to previous data
    //the width of the spike,the time of the spike.
    //Only the PCs dimensions are taken into account.
    int nbDimensions = clusteringData.totalNbOfPCAs();

    spikesByCluster;
    clusterInfoMap;
    //Obtain a copy of the internal variables of data storing the information the clusters.
    //A copy is needed because the clusters can changed while the calculation is in process.
    clusteringData.duplicate(spikesByCluster,clusterInfoMap);

    //Cluster 0 is not compute.
    if(clusterInfoMap->contains(0))
        clusterInfoMap->remove(0);

    int nbClusters = clusterInfoMap->count();

    if(haveToStopComputing) return new Array<double>(0,0);//We do not care about what is return as it will not be used.

    //Calculate the means and the covariances.
    meanCovarianceComputation(nbClusters,nbDimensions,nbSpikes,clusteringData,ignoreClusterIndex);

    //logP(spike,clusterIndex) = minus log likelihood for spike spike in cluster clusterIndex
    //exp(logP)
    Array<double>* probabilities = new Array<double>(nbSpikes,nbClusters);
    probabilities->fillWithZeros();

    double piTerm = static_cast<double>(log(2 * M_PI)) * nbDimensions / 2;

    Data::ClusterInfoMap::Iterator iterator;
    int clusterIndex = 1;
    int cluster1Index = 1;

    //NB: the iterator iterates on the items sorted by their key (clusterId)
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        if(haveToStopComputing) return probabilities;//We do not care about what is return as it will not be used.

        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType clusterId = iterator.key();

        //Check if the cluster1 exists
        if(clusterId == 1){
            existCluster1 = true;
            cluster1Index = clusterIndex;
        }

        //Add the current cluster to the list of analysed clusters.
        clusterList.append(static_cast<int>(clusterId)); //cluster 0 is ignore.

        //ignore the clusters which do not have enough spikes or the cluster 0.
        if(ignoreClusterIndex.contains(clusterIndex) != 0){
            clusterIndex++;
            continue;
        }

        if(haveToStopComputing) return probabilities;//We do not care about what is return as it will not be used.

        //Calculate the cholesky decomposition for the current cluster.
        Array<double> choleskyDecomposition;
        choleskyDecomposition.setSize(nbDimensions,nbDimensions);
        if(cholesky(choleskyDecomposition,nbDimensions,clusterIndex)){
            // If cholesky returns 1, it means the matrix is not positive definite.
            // So the cluster is not used.
            ignoreClusterIndex.append(clusterIndex);
            clusterIndex++;
            continue;
        }

        //Add the current cluster to the list of computed clusters.
        computedClusterList.append(static_cast<int>(clusterId));

        double logRootDet = 0; // log of square root of covariance determinant.
        //logRootDet is given by log of product of diagonal elements.
        for(int i = 1; i<= nbDimensions;++i)
            logRootDet += static_cast<double>(log(static_cast<double>(choleskyDecomposition(i,i))));

        double weight = static_cast<double>(log(static_cast<double>(static_cast<double>(nbSpikesOfCluster)/static_cast<double>(nbSpikes))));
        double logTerm = logRootDet - weight + piTerm;

        Array<double> dataMinusMean;
        dataMinusMean.setSize(1,nbDimensions);
        Array<double> root;
        root.setSize(1,nbDimensions);
        root.fillWithZeros();

        Data::ClusterInfoMap::Iterator iterator2;
        int clusterIndex2 = 1;
        for(iterator2 = clusterInfoMap->begin(); iterator2 != clusterInfoMap->end(); ++iterator2){
            if(haveToStopComputing) return probabilities;//We do not care about what is return as it will not be used.

            dataType firstSpikePosition = iterator2.value().firstSpikePosition();
            dataType lastPosition =  firstSpikePosition + iterator2.value().nbSpikes();

            //Check if the current cluster has been ignore
            if(ignoreClusterIndex.contains(clusterIndex2) != 0){
                ++clusterIndex2;
                continue;
            }

            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);

                double sum = 0;
                //Calculate data minus cluster mean.
                for(int i = 1; i <= nbDimensions;++i){
                    sum = clusteringData.features(featuresRowIndex,i)
                            - means(clusterIndex,i);

                    //Calculate root vector - by choleskyDecomposition*root = dataMinusMean.
                    for(int j= i-1; j >= 1;--j) sum -= choleskyDecomposition(i,j) * root(1,j); // j<i
                    root(1,i) = sum / choleskyDecomposition(i,i);
                }


                //Compute Mahalanobis distance of point from cluster center.
                double mahal = 0;
                for(int i = 1; i <= nbDimensions;++i) mahal += root(1,i) * root(1,i);

                (*probabilities)(featuresRowIndex,clusterIndex) =
                        static_cast<double>(exp(- static_cast<double>(mahal/2 + logTerm)));

            }
            ++clusterIndex2;
        }

        ++clusterIndex;
    }

    //If the cluster 1 does not exist, add it to the matrix of probabilities.
    if(!existCluster1){
        Array<double>* probabilitiesTmp = new Array<double>(nbSpikes,nbClusters + 1);
        probabilitiesTmp->fillWithZeros();
        //probabilitiesTmp->copy(*probabilities,1,1,nbClusters * nbSpikes,1,2);
        probabilitiesTmp->copyAndPrependColumn(*probabilities);

        delete probabilities;
        probabilities = probabilitiesTmp;

        clusterList.prepend(1);
        computedClusterList.prepend(1);
        for(int i = 0; i < static_cast<int>(ignoreClusterIndex.size());++i) ignoreClusterIndex[i] += 1;
        nbClusters ++; //The number of cluster is now incremented by the cluster 1.
        initIndex = 2;//skip cluster 1
    }

    clusterIndex = initIndex;
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        if(haveToStopComputing) return probabilities;//We do not care about what is return as it will not be used.

        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType lastPosition =  firstSpikePosition + iterator.value().nbSpikes();

        //Check if the current cluster has been ignore
        if(ignoreClusterIndex.contains(clusterIndex) != 0){
            ++clusterIndex;
            continue;
        }

        for(dataType i = firstSpikePosition; i < lastPosition;++i){
            dataType featuresRowIndex = (*spikesByCluster)(1,i);

            double sum = 0;
            for(int clusterIndex2 = initIndex; clusterIndex2 <= nbClusters; ++clusterIndex2)
                sum += (*probabilities)(featuresRowIndex,clusterIndex2);

            //If any spikes have all probabilities equal to zero, set them to cluster 1.
            if(sum == 0){
                sum = 1;
                (*probabilities)(featuresRowIndex,cluster1Index) = 1;
            }

            //Normalize the probabilities
            for(int clusterIndex2 = initIndex; clusterIndex2 <= nbClusters; ++clusterIndex2)
                (*probabilities)(featuresRowIndex,clusterIndex2) /= sum;
        }
        ++clusterIndex;
    }

    return probabilities;
}


int GroupingAssistant::cholesky(Array<double>& out,int nbDimensions,int clusterIndex){
    double sum = 0;

    for(int i = 1;i <= nbDimensions;++i){
        for(int j = i;j <= nbDimensions;++j){	// j>=i
            sum = covariances(clusterIndex,(i - 1) * nbDimensions + j);

            for(int k = i - 1;k >= 1;--k) sum -= out(i,k) * out(j,k); // i,j >= k
            if(i==j){
                if(sum <= 0) return(1); // Cholesky decomposition has failed
                out(i,i) = static_cast<double>(sqrt(static_cast<double>(sum)));
            }
            else{
                out(j,i) = sum/out(i,i);
            }
        }
    }

    return 0; // for sucess
}


void GroupingAssistant::meanCovarianceComputation(int nbClusters,int nbDimensions,dataType nbSpikes,Data& clusteringData,QList<int>& ignoreClusterIndex){
    //initialize dataMinusMean,means and covariances.
    Array<double> dataMinusMean;
    dataMinusMean.setSize(1,nbDimensions);
    means.setSize(nbClusters,nbDimensions);
    means.fillWithZeros();
    covariances.setSize(nbClusters,nbDimensions * nbDimensions);
    covariances.fillWithZeros();

    Data::ClusterInfoMap::Iterator iterator;

    int clusterIndex = 1;
    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        if(haveToStopComputing) return;//We do not care about the result as it will not be used.

        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        //Check if a cluster as to be ignore <=> not enough spikes.
        bool ignore = false;
        if(nbSpikesOfCluster <= nbDimensions){
            ignoreClusterIndex.append(clusterIndex);
            ignore = true;
        }

        if(!ignore && !haveToStopComputing){
            //Calculate the means.

            //Accumulate sums for mean caculation
            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);
                for(int j = 1;j <= nbDimensions;++j){
                    means(clusterIndex,j) += static_cast<double>(clusteringData.features(featuresRowIndex,j));
                }
            }

            //Normalize
            for(int j = 1;j <= nbDimensions;++j){
                means(clusterIndex,j) /= nbSpikesOfCluster;
            }

            //Calculate the covariances.
            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);

                //Calculate distance from mean
                for(int j = 1;j <= nbDimensions;++j){
                    dataMinusMean(1,j) = (static_cast<double>(clusteringData.features(featuresRowIndex,j))
                                          - means(clusterIndex,j));
                }

                for(int i = 1;i <= nbDimensions;++i){
                    for(int j = 1;j <= nbDimensions;++j)
                        covariances(clusterIndex,(i - 1) * nbDimensions + j) += dataMinusMean(1,i) * dataMinusMean(1,j);
                }

            }

            //Normalize
            for(int i = 1;i <= nbDimensions;++i){
                for(int j = 1;j <= nbDimensions;++j){
                    covariances(clusterIndex,(i - 1) * nbDimensions + j) /= (nbSpikesOfCluster - 1);
                }
            }

        }//end ignore

        ++clusterIndex;
    }
}

