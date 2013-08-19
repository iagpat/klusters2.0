/***************************************************************************
                          clustersprovider.cpp  -  description
                             -------------------
    begin                : Wed Apr 14 2004
    copyright            : (C) 2004 by Lynn Hazan
    email                : lynn.hazan.myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//QT include files
#include <qfile.h>
#include <qstringlist.h>
#include <qfileinfo.h>

#include <QList>

//include files for the application
#include "clustersprovider.h"
#include "timer.h"


ClustersProvider::ClustersProvider(const QString& fileUrl,double samplingRate,double currentSamplingRate,Data& clusteringData,dataType dataFileMaxTime)
    : DataProvider(fileUrl),
      samplingRate(samplingRate),
      clusteringData(clusteringData),
      dataFileMaxTime(dataFileMaxTime)
{

    clusterPosition = 0.25;

    //Find the cluster file number and use it as the name for the provider
    //the file name can be X.clu.n or X.n.clu
    QString fileName = fileUrl;
    int startingIndex = fileName.lastIndexOf("clu");
    if (startingIndex == static_cast<int>(fileName.length()) - 3) {//X.n.clu
        int nBStartingIndex = fileName.lastIndexOf(".",startingIndex - 2);
        name = fileName.mid(nBStartingIndex + 1,(startingIndex - 1) - (nBStartingIndex + 1));
    } else {//X.clu.n
        const int nBStartingIndex = fileName.lastIndexOf(".");
        name = fileName.right(static_cast<int>(fileName.length()) - (nBStartingIndex + 1));
    }

    nbOfDimensions = clusteringData.nbOfDimensions();

    fileMaxTime = static_cast<dataType>(clusteringData.maxDimension(nbOfDimensions));
    previousStartTime = 0;
}

ClustersProvider::~ClustersProvider(){
}


void ClustersProvider::requestData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits){
    retrieveData(startTime,endTime,initiator,startTimeInRecordingUnits);
}

void ClustersProvider::retrieveData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits){
    SortableTable data;

    //Convert the time in miliseconds to time in recording units if need it.
    dataType startInRecordingUnits;
    //startTimeInRecordingUnits has been computed in a previous call to a browsing function. It has to be used insted of computing
    //the value from startTime because of the rounding which has been applied to it.
    if(startTimeInRecordingUnits != 0)
        startInRecordingUnits = startTimeInRecordingUnits;
    else
        startInRecordingUnits = static_cast<dataType>(startTime * samplingRate / 1000.0);
    dataType endInRecordingUnits =  static_cast<dataType>(endTime * samplingRate / 1000.0);

    if(startInRecordingUnits > fileMaxTime){
        //Send the information to the receiver.
        emit dataReady(data,initiator,name);
        return;
    }
    if(endInRecordingUnits > fileMaxTime)
        endInRecordingUnits = fileMaxTime;

    //Store the information for the next request
    previousStartTime = startInRecordingUnits;

    SortableTable* spikesByCluster;
    Data::ClusterInfoMap* clusterInfoMap;
    //Obtain a copy of the internal variables of data storing the information the clusters.
    //A copy is needed because the clusters can changed while the look up of information is in process.
    clusteringData.duplicate(spikesByCluster,clusterInfoMap);

    QList<int>::iterator iterator;
    dataType nbSpikes = 0;
    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        nbSpikes += (*clusterInfoMap)[*iterator].nbSpikes();
    }

    //The exact size (<=> number of spikes is not known yet, so the size of data is set to the maximum possible)
    data.setSize(nbSpikes);//a SortableTable haas by default 2 lines. Here the first line contains the sample index and the secoind the clusterId.
    dataType time = 0;
    long count = 0;

    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        for(dataType i = firstSpikePosition; i < lastPosition;++i){
            dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
            time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
            if(time < startInRecordingUnits) continue;
            if(time > endInRecordingUnits) break;
            data(1,count + 1) = time - startInRecordingUnits;
            data(2,count + 1) = *iterator;
            count++;
        }
    }

    delete spikesByCluster;
    delete clusterInfoMap;


    //Store the data in a array of the good size
    SortableTable finalData;
    finalData.setSize(count);
    finalData.copySubset(data,count);
    //Sort the data by timestampe
    finalData.sort(1);

    //Send the information to the reciever.
    emit dataReady(finalData,initiator,name);
}

void ClustersProvider::requestNextClusterData(long startTime,long timeFrame,QList<int> selectedIds,QObject* initiator,long startTimeInRecordingUnits){
    SortableTable data;

    //the found spike will be placed at clusterPosition*100 % of the timeFrame
    dataType timeFrameInRecordingUnits = static_cast<dataType>(timeFrame * samplingRate / 1000.0);
    float position = static_cast<float>(timeFrameInRecordingUnits) * clusterPosition;

    //Convert the time in miliseconds to time in recording units if need it.
    dataType startInRecordingUnits;
    //startTimeInRecordingUnits has been computed in a previous call to a browsing function. It has to be used insted of computing
    //the value from startTime because of the rounding which has been applied to it.
    if(startTimeInRecordingUnits != 0){
        //the found spike will be placed at clusterPosition*100 % of the timeFrame
        startInRecordingUnits = startTimeInRecordingUnits + static_cast<long>(position);
    } else
        startInRecordingUnits = static_cast<dataType>(startTime * samplingRate / 1000.0);

    if(startInRecordingUnits > fileMaxTime){
        //Send the information to the receiver.
        emit dataReady(data,initiator,name);
        return;
    }

    SortableTable* spikesByCluster;
    Data::ClusterInfoMap* clusterInfoMap;
    //Obtain a copy of the internal variables of data storing the information the clusters.
    //A copy is needed because the clusters can changed while the look up of information is in process.
    clusteringData.duplicate(spikesByCluster,clusterInfoMap);

    QList<int>::iterator iterator;
    dataType nbSpikes = 0;
    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        nbSpikes += (*clusterInfoMap)[*iterator].nbSpikes();
    }

    //The exact size (<=> number of spikes is not known yet, so the size of data is set to the maximum possible)
    data.setSize(nbSpikes);//a SortableTable haas by default 2 lines. Here the first line contains the sample index and the secoind the clusterId.

    //First look up for the the time corresponding to the first spike found after startInRecordingUnits
    QList<int> firstSpikes;
    dataType time = 0;

    for(iterator = selectedIds.begin(); iterator != selectedIds.end(); ++iterator){
        dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        for(dataType i = firstSpikePosition; i < lastPosition;++i){
            dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
            time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
            if(time < startInRecordingUnits) continue;
            if(time > startInRecordingUnits){
                firstSpikes.append(time);
                break;
            }
        }
    }


    //check that a spike has been found, if that is not the case return startTime as the startingTime => no change will be done in the view, and startTimeInRecordingUnits
    if(firstSpikes.isEmpty()){
        emit nextClusterDataReady(data,initiator,name,startTime,startTimeInRecordingUnits);
        return;
    }

    qSort(firstSpikes);
    time = firstSpikes.first();

    //the found spike will be placed at clusterPosition*100 % of the timeFrame
    //compute the final starting time
    dataType startingInRecordingUnits = qMax(time - static_cast<long>(position),0L);

    if(startingInRecordingUnits == previousStartTime){
        if(firstSpikes.size() == 1){
            emit nextClusterDataReady(data,initiator,name,startTime,startTimeInRecordingUnits);
            return;
        }

        //Set startInRecordingUnits to the time which has given this state
        startInRecordingUnits = time;
        firstSpikes.clear();

        for(iterator = selectedIds.begin(); iterator != selectedIds.end(); ++iterator){
            dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
            dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
            dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
                time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
                if(time < startInRecordingUnits) continue;
                if(time > startInRecordingUnits){
                    firstSpikes.append(time);
                    break;
                }
            }
        }

        //check that a spike has been found, if that is not the case return startTime as the startingTime => no change will be done in the view, and startTimeInRecordingUnits
        if(firstSpikes.isEmpty()){
            emit nextClusterDataReady(data,initiator,name,startTime,startTimeInRecordingUnits);
            return;
        }

        qSort(firstSpikes);
        time = firstSpikes[0];

        startingInRecordingUnits = qMax(time - static_cast<long>(position),0L);
    }

    dataType endInRecordingUnits = startingInRecordingUnits + timeFrameInRecordingUnits;

    //Always keep the same timeFrame
    if(endInRecordingUnits > dataFileMaxTime){
        startingInRecordingUnits = dataFileMaxTime - timeFrameInRecordingUnits;
        endInRecordingUnits = dataFileMaxTime;
    }

    long count = 0;
    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        for(dataType i = firstSpikePosition; i < lastPosition;++i){
            dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
            time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
            if(time < startingInRecordingUnits)
                continue;
            if(time > endInRecordingUnits)
                break;
            data(1,count + 1) = time - startingInRecordingUnits;
            data(2,count + 1) = *iterator;
            count++;
        }
    }

    delete spikesByCluster;
    delete clusterInfoMap;

    //Store the data in a array of the good size
    SortableTable finalData;
    finalData.setSize(count);
    finalData.copySubset(data,count);
    //Sort the data by timestampe
    finalData.sort(1);


    //Computes the starting time in miliseconds
    double computeStartingTime = static_cast<double>(static_cast<double>(startingInRecordingUnits) * 1000.0 / samplingRate);
    dataType startingInMiliseconds = static_cast<dataType>(floor(0.5 + computeStartingTime));

    //Store the information for the next request
    previousStartTime = startingInRecordingUnits;

    //Send the information to the receiver.
    emit nextClusterDataReady(finalData,initiator,name,startingInMiliseconds,startingInRecordingUnits);
}

void ClustersProvider::requestPreviousClusterData(long startTime,long timeFrame,QList<int> selectedIds,QObject* initiator,long startTimeInRecordingUnits){
    SortableTable data;

    //the found spike will be placed at clusterPosition*100 % of the timeFrame
    dataType timeFrameInRecordingUnits = static_cast<dataType>(timeFrame * samplingRate / 1000.0);
    float position = static_cast<float>(timeFrameInRecordingUnits) * clusterPosition;

    //Convert the time in miliseconds to time in recording units if need it.
    dataType startInRecordingUnits;
    //startTimeInRecordingUnits has been computed in a previous call to a browsing function. It has to be used insted of computing
    //the value from startTime because of the rounding which has been applied to it.
    if(startTimeInRecordingUnits != 0){
        //the found spike will be placed at clusterPosition*100 % of the timeFrame
        startInRecordingUnits = startTimeInRecordingUnits + static_cast<long>(position);
    }
    else
        startInRecordingUnits = static_cast<dataType>(startTime * samplingRate / 1000.0);

    SortableTable* spikesByCluster;
    Data::ClusterInfoMap* clusterInfoMap;
    //Obtain a copy of the internal variables of data storing the information the clusters.
    //A copy is needed because the clusters can changed while the look up of information is in process.
    clusteringData.duplicate(spikesByCluster,clusterInfoMap);

    QList<int>::iterator iterator;
    dataType nbSpikes = 0;
    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        nbSpikes += (*clusterInfoMap)[*iterator].nbSpikes();
    }

    //The exact size (<=> number of spikes is not known yet, so the size of data is set to the maximum possible)
    data.setSize(nbSpikes);//a SortableTable haas by default 2 lines. Here the first line contains the sample index and the secoind the clusterId.

    //First look up for the the time corresponding to the first spike found before startInRecordingUnits
    QList<int> firstSpikes;
    dataType time = 0;

    for(iterator = selectedIds.begin(); iterator != selectedIds.end(); ++iterator){
        dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
        dataType firstPosition =  firstSpikePosition + nbSpikesOfCluster - 1;
        dataType lastPosition =  firstSpikePosition - 1;

        for(dataType i = firstPosition; i > lastPosition;--i){
            dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
            time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
            if(time > startInRecordingUnits)
                continue;
            if(time < startInRecordingUnits) {
                firstSpikes.append(time);
                break;
            }
        }
    }




    //check that a spike has been found, if that is not the case return startTime as the startingTime => no change will be done in the view, and startTimeInRecordingUnits
    if(firstSpikes.isEmpty()){
        emit previousClusterDataReady(data,initiator,name,startTime,startTimeInRecordingUnits);
        return;
    }

    qSort(firstSpikes);
    time = firstSpikes.at(firstSpikes.size() - 1);

    //the found spike will be placed at clusterPosition*100 % of the timeFrame
    //compute the final starting time
    dataType startingInRecordingUnits = qMax(time - static_cast<long>(position),0L);

    dataType endInRecordingUnits = startingInRecordingUnits + timeFrameInRecordingUnits;

    long count = 0;
    for(iterator = clusterIds->begin(); iterator != clusterIds->end(); ++iterator){
        dataType firstSpikePosition = (*clusterInfoMap)[*iterator].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[*iterator].nbSpikes();
        dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

        for(dataType i = firstSpikePosition; i < lastPosition;++i){
            dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
            time = static_cast<dataType>(clusteringData.features(featuresRowIndex,nbOfDimensions));
            if(time < startingInRecordingUnits) continue;
            if(time > endInRecordingUnits) break;
            data(1,count + 1) = time - startingInRecordingUnits;
            data(2,count + 1) = *iterator;
            count++;
        }
    }

    delete spikesByCluster;
    delete clusterInfoMap;


    //Store the data in a array of the good size
    SortableTable finalData;
    finalData.setSize(count);
    finalData.copySubset(data,count);
    //Sort the data by timestampe
    finalData.sort(1);


    //Computes the starting time in miliseconds
    double computeStartingTime = static_cast<double>(static_cast<double>(startingInRecordingUnits) * 1000.0 / samplingRate);
    dataType startingInMiliseconds = static_cast<dataType>(floor(0.5 + computeStartingTime));

    //Store the information for the next request
    previousStartTime = startingInRecordingUnits;

    //Send the information to the receiver.
    emit previousClusterDataReady(finalData,initiator,name,startingInMiliseconds,startingInRecordingUnits);
}
