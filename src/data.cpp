/***************************************************************************
                          data.cpp  -  description
                             -------------------
    begin                : Wed Sep 17 2003
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
//Application include files
#include "data.h"
#include "minmaxthread.h"
#include "waveformview.h"
#include "autosavethread.h"
#include "klustersxmlreader.h"

//C include files
//#define _LARGEFILE_SOURCE already defined in /usr/include/features.h
#define _FILE_OFFSET_BITS 64
#include <cstring>

//Qt include files
#include <qtextstream.h>
#include <qstringlist.h>
#include <QString>
#include <qregexp.h>

#include <QList>
#include <QDebug>

//kde include files


#include <iomanip> // Required for formated I/O.


#include "timer.h"

extern int nbUndo;

Data::Data()
    :nbSpikes(0),
      traceViewVariablesAvailable(false),
      undoRedoInProcess(false),
      clusterZeroJustModified(false)
{

    minMaxThread = minMaxCalculator();
    spikesByCluster = new SortableTable();
    clusterInfoMap = new ClusterInfoMap();

}

Data::~Data(){
    //If the minMaxThread has not finish, wait until it is done
    while(!minMaxThread->wait()){};
    delete minMaxThread;

    //delete the pointers to the tables and maps
    delete spikesByCluster;
    delete clusterInfoMap;

    qDeleteAll(clusterInfoMapUndoList);
    clusterInfoMapUndoList.clear();
    qDeleteAll(clusterInfoMapRedoList);
    clusterInfoMapRedoList.clear();
    qDeleteAll(spikesByClusterUndoList);
    spikesByClusterUndoList.clear();
    qDeleteAll(spikesByClusterRedoList);
    spikesByClusterRedoList.clear();

    qDeleteAll(waveformDict);
    waveformDict.clear();
    qDeleteAll(correlationDict);
    correlationDict.clear();

}

MinMaxThread* Data::minMaxCalculator(){
    return new MinMaxThread(*this);
}


bool Data::configure(QFile& parFile,int electrodeGroupID,QString& errorInformation){
    KlustersXmlReader reader = KlustersXmlReader();
    if(reader.parseFile(parFile,KlustersXmlReader::PARAMETER)){

        //Load the info
        nbBits = reader.getResolution();
        samplingRate = reader.getSamplingRate();
        QList<int> channels = reader.getNbChannelsByGroup(electrodeGroupID);

        QList<int>::iterator it;
        for(it = channels.begin(); it != channels.end(); ++it) currentChannels.append(*it);
        nbChannels = currentChannels.size();
        nbSamplesInWaveform = reader.getNbSamples(electrodeGroupID);
        peakPositionInWaveform = reader.getPeakSampleIndex(electrodeGroupID);
        nbFeaturesbyChannel = reader.getNbFeatures(electrodeGroupID);
        totalNbChannels = reader.getNbChannels();

        //Variables used by the traceView
        voltageRange = reader.getVoltageRange();
        amplification = reader.getAmplification();
        initialOffset = reader.getOffset();
        if(voltageRange != 0 && amplification != 0 && totalNbChannels != 0)
            traceViewVariablesAvailable = true;

        //cluster user information
        reader.getClusterUserInformation(electrodeGroupID,clusterUserInformationMap);

        reader.closeFile();

        //If  one of the elements was not in the parameter file, its value would have been assigned 0.
        if(nbBits == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the number of bits is missing.");
            return false;
        }
        if(samplingRate == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the sampling rate is missing.");
            return false;
        }
        if(channels.isEmpty()){
            errorInformation = QObject::tr("There is no channels defined for this electrode group.");
            return false;
        }
        if(nbChannels == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the number of channels could not be determined.");
            return false;
        }
        if(nbSamplesInWaveform == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the number of samples per waveform is missing.");
            return false;
        }
        if(peakPositionInWaveform == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the position of the waveform peak is missing.");
            return false;
        }
        if(nbFeaturesbyChannel == 0){
            errorInformation = QObject::tr("In the parameter file (base.xml), the number of features per channel is missings.");
            return false;
        }

        //The sampling rate is given in seconds and the sampling interval used in Klusters is in microseconds.
        samplingInterval = 1000000.0 /samplingRate;

        return true;
    }
    else{
        errorInformation = QObject::tr("The parameter file (base.xml) could not be parsed.");
        return false;
    }
}

bool Data::configure(QFile& parXFile,QFile& parFile,QString& errorInformation){
    QTextStream parX(&parXFile);
    QTextStream par(&parFile);
    QList <QStringList> parXData;
    QList <QStringList> parData;

    int lineCounter = 0;
    QString line;
    for(line = parX.readLine(); !line.isNull();line = parX.readLine()){
        parXData.append(line.split(" ",QString::SkipEmptyParts));
        lineCounter ++;
    }
    //The parX file has to contain at leat 9 lines, otherwise there is a problem
    if(lineCounter != 9){
        errorInformation = QObject::tr("In the general parameter file (base.par), the number of lines should be 9.");
        return false;
    }

    //get the parameters
    totalNbChannels = parXData[0][0].toInt();

    nbChannels = parXData[0][1].toInt();
    for (int i=0; i<nbChannels;++i){
        channelIds.append(parXData[1][i].toInt());
    }
    nbRefactorySample = parXData[2][0].toInt();
    RMSIntWindowLenght = parXData[2][1].toInt();
    firingRate = parXData[3][0].toFloat();
    nbSamplesInWaveform = parXData[4][0].toInt();
    peakPositionInWaveform = parXData[4][1].toInt();
    windowLenghtToRealign = parXData[5][0].toInt();
    peakPositionToRealign = parXData[5][1].toInt();
    nbSampleBeforePeak = parXData[6][0].toInt();
    nbSampleAfterPeak = parXData[6][1].toInt();
    nbFeaturesbyChannel = parXData[7][0].toInt();
    nbSamplesByPCA = parXData[7][1].toInt();
    HighPassFilterFreq = parXData[8][0].toFloat();

    lineCounter = 0;
    for(line = par.readLine(); !line.isNull();line = par.readLine()){
        parData.append(line.split(" ",QString::SkipEmptyParts));
        lineCounter ++;
    }

    //The par file has to contain at leat 3 lines, otherwise there is a problem
    if(lineCounter < 3){
        errorInformation = QObject::tr("In the specific parameter file (base.par.n), there are less than 3 lines.");
        return false;
    }

    nbBits = parData[0][1].toInt();
    samplingInterval = (parData[1][0].toDouble());
    nbTotalElectrodes = parData[2][0].toInt();

    return true;
}

bool Data::loadClusters(QFile &clusterFile, long spkFileLength, QString& errorInformation){
    //Determine the number of spikes using the length of the binary spike file
    int sampleSize;
    switch(nbBits){
    case 12:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 14:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 16:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 32:
        sampleSize = 4;
        isTwoBytesRecording = false;
        break;
    default:   //not implemented
        errorInformation = QObject::tr("The number of bits is not supported.");
        return false;
    }

    nbSpikes =  spkFileLength / static_cast<long>(static_cast<long>(nbChannels) * static_cast<long>(nbSamplesInWaveform) * static_cast<long>(sampleSize));
    dataType upperLimit = nbSpikes + 1;
    //Effectively create the table containing the data
    spikesByCluster->setSize(nbSpikes);
    bool firstLine = true;
    dataType k = 1;
    while (!clusterFile.atEnd()) {
        QByteArray line = clusterFile.readLine();
        if (firstLine) {
            firstLine = false;
        } else {
            QByteArray feature;
            for (int i = 0; i <line.count();++i) {
                if ( (line.at(i) >= '0' && line.at(i) <='9')) {
                    feature.append(line.at(i));
                } else if( !feature.isEmpty()){
                    (*spikesByCluster)(2,k++) = feature.toLongLong();//Warning if the typedef dataType changes, change will have to be make here.
                    feature = "";
                }
            }
            if(k > upperLimit)
                break;
        }
    }

    qDebug()<<" nbSpikes:"<< nbSpikes<< "k:"<<k<<" spkFileLength:"<<spkFileLength<< "nbChannels : "<< nbChannels<<" nbSamplesInWaveform "<<nbSamplesInWaveform<< " sampleSize:"<<sampleSize;

    qDebug()<<" upperLimit"<<upperLimit<< " k "<<k;
    //if the number of clusters read did not correspond to nbSpikes, there is a problem.
    if(k != upperLimit){
        errorInformation = QObject::tr("The number of spikes read in the cluster file does not correspond to number of spikes computed.(computed : %1, upperlimit %2)").arg(k).arg(upperLimit);
        return false;
    } else {
        return true;
    }
}

bool Data::loadFeatures(QFile& featureFile,QString& errorInformation){

    bool firstLine = true;
    dataType k = 0;
    while (!featureFile.atEnd()) {
        QByteArray line = featureFile.readLine();
        //qDebug()<<" line "<<line;
        if (firstLine) {
            firstLine = false;
            int j = 0;
            for (int i = 0; i <line.count();++i) {
                if (line.at(i) >= '0' && line.at(i) <='9') {
                    j++;
                } else if(j) {
                    nbDimensions = line.left(j).toInt();
                    break;
                }
            }
            features.setSize(nbSpikes,nbDimensions);
        } else {
            QByteArray feature;
            for (int i = 0; i <line.count();++i) {
                if (line.at(i) == '-' || (line.at(i) >= '0' && line.at(i) <='9')) {
                    feature.append(line.at(i));
                } else if (!feature.isEmpty()){
                    features[k++] = feature.toLongLong();//Warning if the typedef dataType changes, change will have to be make here.
                    feature = "";
                }
            }
        }
    }

    //qDebug() << "in loadFeatures,  k: "<<k<< " nbSpikes "<<nbSpikes<< " nbDimensions "<<nbDimensions<< endl;

    //if the number of features read did not correspond to nbSpikes*nbDimensions, there is a problem.
    if(k != (nbSpikes * nbDimensions)){
        errorInformation = QObject::tr("The number of features read in the feature file does not correspond to number of spikes times the number of dimensions.");
        return false;
    }
    else
        return true;
}


bool Data::initialize(QFile& featureFile,QFile& clusterFile,long spkFileLength,QString& errorInformation){
    if(!loadClusters(clusterFile,spkFileLength,errorInformation))
        return false;
    if(!loadFeatures(featureFile,errorInformation))
        return false;

    //Fill the first row of spikesByCluster with the row index of the spike,
    //knowing that for the moment the elements of the table are sorted by spike order.
    for(dataType i = 1; i <= nbSpikes; ++ i) (*spikesByCluster)(1,i) = i;

    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);
    QMap<dataType,dataType> clusters;
    dataType max = nbSpikes + 1;
    //Count the number of spikes for each cluster.
    for(dataType i = 1; i < max; ++i) {
        dataType clusterId = (*spikesByCluster)(2,i);
        clusters[clusterId]++;
    }

    //Initialize positions, for each clusterId the value is set to the position of the first spike.
    //Add the cluster user information.
    //The clusters will be sorted by clusterId in spikesByCluster. Initialize clusterInfoMap.
    QMap<dataType,dataType> positions;
    QMap<dataType,dataType>::Iterator iterator;
    int index = 1;
    for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator){
        dataType clusterId = iterator.key();
        positions[clusterId] =  index;
        ClusterUserInformation vClusterUserInformation = clusterUserInformationMap[static_cast<int>(clusterId)];

        clusterInfoMap->insert(clusterId,ClusterInfo(index,iterator.value(),vClusterUserInformation.getStructure(),vClusterUserInformation.getType(),vClusterUserInformation.getId(),vClusterUserInformation.getQuality(),vClusterUserInformation.getNotes()));

        index += iterator.value();
    }

    //Reset the clusterUserInformationMap which only ne used from now on to store the information before writting it to the xml parameter file.
    clusterUserInformationMap.clear();

    //Fill tmp with the data sorted by cluster and by time (<=> position in the fet file)
    for(dataType i = 1; i < max; ++i){
        dataType clusterId = (*spikesByCluster)(2,i);
        dataType position = positions[clusterId];
        dataType positionInFet = (*spikesByCluster)(1,i);
        (*spikesByClusterTemp)(1,position) = positionInFet;
        (*spikesByClusterTemp)(2,position) = clusterId;
        positions[clusterId] ++;
    }

    //Delete spikesByCluster and assign to the pointer the value of spikesByClusterTemp;
    delete spikesByCluster;
    spikesByCluster = 0L;
    spikesByCluster =  spikesByClusterTemp;


    //Calculate the minimum and maximum for each dimension and store them in
    //dimensionMinima and dimensionMaxima respectively
    QList<int> modifiedClusters;
    minMaxDimensionCalculation(modifiedClusters);
    return true;
}



bool Data::initialize(QFile& featureFile,QFile& clusterFile,long spkFileLength,const QString& spkFileName,QFile& parXFile,QFile& parFile,QString& errorInformation){
    this->spkFileName = spkFileName;
    if(!configure(parXFile, parFile,errorInformation))
        return false;

    if(!initialize(featureFile,clusterFile,spkFileLength,errorInformation))
        return false;

    return true;
}

bool Data::initialize(QFile& featureFile,QFile& clusterFile,long spkFileLength,const QString& spkFileName,QFile& parFile,int electrodeGroupID,QString& errorInformation){
    this->spkFileName = spkFileName;

    if(!configure(parFile,electrodeGroupID,errorInformation))
        return false;
    if(!initialize(featureFile,clusterFile,spkFileLength,errorInformation))
        return false;

    return true;
}

bool Data::initialize(QFile& featureFile,long spkFileLength,QString& errorInformation){

    //Determine the number of spikes using the length of the binary spike file
    int sampleSize;
    switch(nbBits){
    case 12:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 14:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 16:
        sampleSize = 2;
        isTwoBytesRecording = true;
        break;
    case 32:
        sampleSize = 4;
        isTwoBytesRecording = false;
        break;
    default:   //not implemented
        errorInformation = QObject::tr("The number of bits is not supported.");
        return false;
    }
    nbSpikes =  spkFileLength / static_cast<long>(static_cast<long>(nbChannels) * static_cast<long>(nbSamplesInWaveform) * static_cast<long>(sampleSize));
    spikesByCluster->setSize(nbSpikes);

    //As the cluster file does not exist assign all the spikes to cluster 1.
    for(dataType i = 1; i <= nbSpikes; ++ i)
        (*spikesByCluster)(2,i) = 1;

    if(!loadFeatures(featureFile,errorInformation))
        return false;

    //Fill the first row of spikesByCluster with the row index of the spike,
    //knowing that for the moment the elements of the table are sorted by spike order.
    for(dataType i = 1; i <= nbSpikes; ++ i)
        (*spikesByCluster)(1,i) = i;

    clusterInfoMap->insert(1, ClusterInfo(1,nbSpikes));

    //Calculate the minimum and maximum for each dimension and store them in
    //dimensionMinima and dimensionMaxima respectively
    QList<int> modifiedClusters;
    minMaxDimensionCalculation(modifiedClusters);
    return true;
}

bool Data::initialize(QFile& featureFile,long spkFileLength,const QString &spkFileName,QFile& parXFile,QFile& parFile,QString& errorInformation){
    this->spkFileName = spkFileName;
    if(!configure(parXFile, parFile,errorInformation))
        return false;
    if(!initialize(featureFile,spkFileLength,errorInformation)){
        return false;
    }

    return true;
}

bool Data::initialize(QFile& featureFile,long spkFileLength,const QString& spkFileName,QFile& parFile,int electrodeGroupID,QString& errorInformation){
    this->spkFileName = spkFileName;

    if(!configure(parFile,electrodeGroupID,errorInformation))
        return false;
    if(!initialize(featureFile,spkFileLength,errorInformation))
        return false;

    return true;
}

void Data::minMaxDimensionCalculation(QList<int> modifiedClusters){
    //If an undo or redo has started or the cluster 0 has been changed again, do not do any calculation, it will be done on the new data.
    if(undoRedoInProcess || clusterZeroJustModified) return;

    //The mutex protects spikesByCluster and clusterInfoMap so that only one thread can
    //access them at the time.
    ClusterInfoMap clusterInfoMapTemp;
    ClusterInfoMap::Iterator iterator;
    mutex.lock();
    SortableTable spikesByClusterTemp(*spikesByCluster);

    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        clusterInfoMapTemp.insert(iterator.key(),iterator.value());
    }
    mutex.unlock();

    Array<dataType> dimensionMaximaTemp(nbDimensions,1);
    Array<dataType> dimensionMinimaTemp(nbDimensions,1);
    dataType max,min;

    bool init = false;
    if(clustersGivingMinimum.isEmpty()){
        init = true;
        for(int i = 0; i<nbDimensions; ++i){
            clustersGivingMinimum.append(0);
            clustersGivingMaximum.append(0);
        }
    }

    //Calculate the minimum and maximum for each dimension and store them in
    //dimensionMinima and dimensionMaxima respectively. The cluster 0 is not taken into account.
    for(int dimension = 1; dimension<nbDimensions; ++dimension){
        //If an undo or redo has started or the cluster 0 has been changed again, stop the calculation.
        if(undoRedoInProcess || clusterZeroJustModified) return;

        max = min = features(1,dimension);

        dataType clusterIdMin = 0;
        dataType clusterIdMax = 0;

        if(!(modifiedClusters.contains(clustersGivingMinimum[dimension - 1]) || modifiedClusters.contains(clustersGivingMaximum[dimension - 1]))
                && !init && !modifiedClusters.isEmpty() && !modifiedClusters.contains(0)){
            dimensionMinimaTemp(dimension,1) = dimensionMinima(dimension,1);
            dimensionMaximaTemp(dimension,1) = dimensionMaxima(dimension,1);
            continue;
        }

        //NB: the iterator iterates on the items sorted by their key
        for(iterator = clusterInfoMapTemp.begin(); iterator != clusterInfoMapTemp.end(); ++iterator){
            dataType clusterId = iterator.key();

            if(clusterId == 0)
                continue;
            dataType firstSpikePosition = iterator.value().firstSpikePosition();
            dataType nbSpikesOfCluster = iterator.value().nbSpikes();
            dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;

            for(dataType i = firstSpikePosition; i < (lastPosition);++i){
                dataType spikePosition = spikesByClusterTemp(1,i);
                dataType currentSpike = features(spikePosition,dimension);

                if(currentSpike < min){
                    min = currentSpike;
                    clusterIdMin = clusterId;
                }
                if(currentSpike > max){
                    clusterIdMax = clusterId;
                    max = currentSpike;
                }

            }

            //If an undo or redo has started or the cluster 0 has been changed again, stop the calculation.
            if(undoRedoInProcess || clusterZeroJustModified) return;
        }
        dimensionMinimaTemp(dimension,1) = min;
        dimensionMaximaTemp(dimension,1) = max;
        clustersGivingMinimum[dimension - 1] = clusterIdMin;
        clustersGivingMaximum[dimension - 1] = clusterIdMax;
    }

    //The time is done seperatly because the minimum is the first spike and the maximun the last spike
    dimensionMinimaTemp(nbDimensions,1) = features(1,nbDimensions);
    dimensionMaximaTemp(nbDimensions,1) = features(nbSpikes,nbDimensions);

    //Update dimensionMinima and dimensionMaxima
    mutex.lock();
    dimensionMaxima.setSize(nbDimensions,1);
    dimensionMinima.setSize(nbDimensions,1);
    for(int i = 1; i<=nbDimensions;++i){
        dimensionMinima(i,1) = dimensionMinimaTemp(i,1);
        dimensionMaxima(i,1) = dimensionMaximaTemp(i,1);
    }
    mutex.unlock();

    qDebug() << "in minMaxDimensionCalculation end" << endl;

}


dataType Data::createNewCluster(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY, QList <int>& fromClusters,QList <int>& emptyClusters){
    //Set the new cluster number to the biggest existing number plus one
    dataType newClusterId = (*spikesByCluster)(2,nbSpikes) + 1;
    dataType nbSpikesInNewCluster = 0;

    //Create the variables to store the number of spikes and the position of the last spike
    //for each cluster contributing to the new cluster. This will be used to sort the new cluster.
    QList<dataType> lastPositions;
    QList<dataType> nbOfspikes;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    //Iteration on the clusters
    ClusterInfoMap::Iterator iterator;
    dataType upperInsertionIndex = 1;
    dataType lowerInsertionIndex = nbSpikes;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType clusterId = iterator.key();

        //if clustersOfOrigin does not contains the current cluster, this cluster is let unchanged
        //and its information is simply copy as is from spikesByCluster to spikesByClusterTemp
        if(!clustersOfOrigin.contains(static_cast<int>(clusterId))){
            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(clusterId,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
            upperInsertionIndex += nbSpikesOfCluster;
        }
        //Now deal with the clusters which may contain spikes to add to the new cluster
        //<=> spike in the region.
        else{
            //Store the last spike position for the current cluster
            lastPositions.append(nbSpikesInNewCluster + 1);

            dataType updatedClusterPosition = 0;
            bool existUntouchSpike = true;
            dataType newNbSpikesOfCluster = nbSpikesOfCluster;
            dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;
            dataType lastPositionLessOne =  lastPosition -1;

            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = static_cast<dataType>((*spikesByCluster)(1,i));
                if(region.contains(
                            QPoint(static_cast<dataType>(features(featuresRowIndex,dimensionX)),
                                   static_cast<dataType>(features(featuresRowIndex,dimensionY))))){
                    //Add the spike to the new cluster <=> add the row index at the end of spikesByCluster at the lowerInsertionIndex
                    (*spikesByClusterTemp)(1,lowerInsertionIndex) = featuresRowIndex;
                    --lowerInsertionIndex;
                    ++nbSpikesInNewCluster;
                    --newNbSpikesOfCluster;
                }
                else{
                    //Update the position of this cluster
                    if(existUntouchSpike){
                        existUntouchSpike = false;
                        updatedClusterPosition = upperInsertionIndex;
                    }
                    //Keep the spike in the current cluster <=> add the row index and the cluster number at the top of spikesByCluster at the upperInsertionIndex
                    (*spikesByClusterTemp)(1,upperInsertionIndex) = featuresRowIndex;
                    (*spikesByClusterTemp)(2,upperInsertionIndex) = (*spikesByCluster)(2,i);
                    ++upperInsertionIndex;
                }

                if(i == (lastPositionLessOne)){
                    if(newNbSpikesOfCluster < nbSpikesOfCluster){
                        //Store the number of spikes coming from the current cluster
                        nbOfspikes.append(nbSpikesOfCluster - newNbSpikesOfCluster);

                        //update fromClusters if at least one spike from that cluster was in the region
                        fromClusters.append(static_cast<int>(clusterId));
                    }
                    //No spike has been move to the new cluster, remove the last entry in lastPositions.
                    else lastPositions.pop_back();

                    //Construct the insertion of the current cluster in the new clusterInfoMap if
                    // the number of spikes is more than zero
                    if(newNbSpikesOfCluster >0)clusterInfoMapTemp->insert(clusterId,ClusterInfo(updatedClusterPosition,newNbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
                    else emptyClusters.append(static_cast<int>(clusterId));
                }
            }
        }
    }

    //If some spikes have been taken from the cluster 0, the max and min
    // dimensions have to be recalculated. If minMaxThread is running, clusterZeroJustModified will
    //inform it that it has to stop (the computation will be done again on the new data).
    if(fromClusters.contains(0)) clusterZeroJustModified = true;

    //For the new cluster, only the row index has been inserted in spikesByClusterTemp,
    //now the cluster number is updated at once for all the spikes of the new cluster
    dataType startInsertion =  lowerInsertionIndex + 1;
    for(dataType i = 0; i<nbSpikesInNewCluster;++i) (*spikesByClusterTemp)(2,startInsertion + i) = newClusterId;

    if(nbSpikesInNewCluster > 0){
        //Construct the insertion of the new cluster in the new clusterInfoMap
        clusterInfoMapTemp->insert(newClusterId,ClusterInfo(lowerInsertionIndex + 1,nbSpikesInNewCluster));

        //Sort the spikes of the newly created cluster.
        sortCluster(clusterInfoMapTemp,spikesByClusterTemp,newClusterId,lastPositions,nbOfspikes,-1);

        //Get the list of clusters before applying the changes, this will be used in the clean
        //of the correlation.
        QList<dataType> currentClusterList = clusterIds();

        //Deal with the undo mechanism
        prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

        //If some spikes have been taken from the cluster 0, the max and min
        // dimensions have to be recalculated. If minMaxThread is running, the call
        //will wait until it finishes before starting the thread again.
        if(fromClusters.contains(0)){
            //If the minMaxThread has not finish, wait until it is done
            while(!minMaxThread->wait()){};
            //Reset the flag to false so the minMaxThread can do the computation
            clusterZeroJustModified = false;
            minMaxThread->setModifiedClusters(fromClusters);
            minMaxThread->start();
            dimensionChangedUndo.prepend(true);
        }
        else dimensionChangedUndo.prepend(false);

        //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster.
        //if there is not a thread working with them,otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
        // and the thread will remove it.
        QList<int>::iterator iterator;
        for(iterator = fromClusters.begin(); iterator != fromClusters.end(); ++iterator){
            mutex.lock();
            if(waveformStatusMap.contains(*iterator)){
                if(!waveformStatusMap[*iterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                    waveformStatusMap.remove(*iterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*iterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*iterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*iterator))) cleanCorrelation(static_cast<dataType>(*iterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*iterator),true);
                mutex.unlock();
            }
        }

        return newClusterId;
    }
    //return 0 if no new cluster have been created
    //safe as cluster 0 (artifact) can never be created that way
    else return 0;
}

QMap<int,int> Data::createNewClusters(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY,QList <int>& emptyClusters){
    QMap<int,int> fromToClusterIds;
    QMap<int,int> fromToNewClusterIds;
    ClusterInfoMap clusterInfoMapTemp; //used in the first part of the function
    int nbNewClusters = 0;
    int nbMaxNewClusters = clustersOfOrigin.size();

    //Set the new cluster number to the biggest existing number plus nbMaxNewClusters.
    //The number will be decremented before being used, and the number will be corrected at the end once the
    //number of really created clusters will be known (the biggest clusterId is store at the bottom of spikesByClusterTemp).
    dataType newClusterId = (*spikesByCluster)(2,nbSpikes) + nbMaxNewClusters;

    //Create the variables to store the number of spikes and the position of the first spike
    //for each cluster contributing to a new cluster. This will be used to sort the new clusters.
    QList< QList<dataType> > firstPositions;
    QList< QList<dataType> > nbOfspikes;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Iteration on the clusters
    ClusterInfoMap::Iterator iterator;
    dataType upperInsertionIndex = 1;
    dataType lowerInsertionIndex = nbSpikes;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType clusterId = iterator.key();

        //if clustersOfOrigin does not contains the current cluster, this cluster is let unchanged
        //and its information is simply copy as is from spikesByCluster to spikesByClusterTemp
        if(!clustersOfOrigin.contains(static_cast<int>(clusterId))){
            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            //Construct the new clusterInfoMap
            clusterInfoMapTemp.insert(clusterId,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
            upperInsertionIndex += nbSpikesOfCluster;
        }
        //Now deal with the clusters which may contain spikes to add to a new cluster <=> spike in the region.
        //If a cluster contain spikes in the region, a new cluster is created
        else{
            dataType nbSpikesInNewCluster = 0;
            dataType updatedClusterPosition = 0;
            bool existUntouchSpike = true;
            dataType newNbSpikesOfCluster = nbSpikesOfCluster;
            dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;
            dataType lastPositionLessOne =  lastPosition -1;

            //Create the variables to store the number of spikes and the position of the last spike
            //for the new cluster contributing to the new cluster. This will be used to sort the new cluster.
            QList<dataType> currentFirstPositions;
            QList<dataType> currentNbOfspikes;

            //Store the last spike position for the current cluster.
            currentFirstPositions.append(1);

            for(dataType i = firstSpikePosition; i < lastPosition;++i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);
                if(region.contains(
                            QPoint(features(featuresRowIndex,dimensionX),
                                   features(featuresRowIndex,dimensionY)))){
                    //Add the spike to the new cluster <=> add the row index at the end of spikesByCluster at the lowerInsertionIndex
                    (*spikesByClusterTemp)(1,lowerInsertionIndex) = featuresRowIndex;

                    --lowerInsertionIndex;
                    ++nbSpikesInNewCluster;
                    newNbSpikesOfCluster --;
                }
                else{
                    //Update the position of this cluster
                    if(existUntouchSpike){
                        existUntouchSpike = false;
                        updatedClusterPosition = upperInsertionIndex;
                    }
                    //Keep the spike in the current cluster <=> add the row index and the cluster number at the top of spikesByCluster at the upperInsertionIndex
                    (*spikesByClusterTemp)(1,upperInsertionIndex) = featuresRowIndex;
                    (*spikesByClusterTemp)(2,upperInsertionIndex) = (*spikesByCluster)(2,i);
                    ++upperInsertionIndex;
                }

                if(i == (lastPositionLessOne)){
                    //Construct the insertion of the current cluster in the new clusterInfoMap if
                    // the number of spikes is more than zero.
                    //Copy the spikes back to spikesByCluster.
                    if(newNbSpikesOfCluster > 0){
                        clusterInfoMapTemp.insert(clusterId,ClusterInfo(updatedClusterPosition,newNbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
                    }
                    else emptyClusters.append(static_cast<int>(clusterId));

                    //If at least one spike from that cluster was in the region, a new cluster will be created
                    if(nbSpikesInNewCluster > 0){
                        //Store the number of spikes of the new cluster.
                        currentNbOfspikes.append(nbSpikesInNewCluster);
                        //Store the information to later sort the new cluster.
                        firstPositions.append(currentFirstPositions);
                        nbOfspikes.append(currentNbOfspikes);


                        ++nbNewClusters;
                        //update fromToClusterIds
                        fromToClusterIds.insert(static_cast<int>(clusterId),static_cast<int>(newClusterId));
                        //Construct the insertion of the newly created cluster with a temporarily clusterId
                        clusterInfoMapTemp.insert(newClusterId,ClusterInfo(lowerInsertionIndex + 1,nbSpikesInNewCluster));
                        //decrement the cluster id for the next cluster to be created
                        --newClusterId;
                    }
                }
            }
        }
    }

    //If some spikes have been taken from the cluster 0, the max and min
    // dimensions have to be recalculated. If minMaxThread is running, clusterZeroJustModified will
    //inform it that it has to stop (the computation will be done again on the new data).
    if(fromToClusterIds.contains(0)) clusterZeroJustModified = true;


    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp2 = new ClusterInfoMap(clusterInfoMapTemp);

    //Update the information on the new clusters,renumber them with the good number now that
    //we know how many clusters have been created.
    if(nbNewClusters > 0){
        int shift =  nbMaxNewClusters - nbNewClusters;

        QList<int> keys = fromToClusterIds.keys();

        //Iteration on the fromToClusterIds
        for(int i = keys.size() - 1; i >= 0; --i){
            int oldCluster = keys[i];
            int clusterToCreate = fromToClusterIds[oldCluster];

            int newClusterId = clusterToCreate - shift;
            fromToNewClusterIds.insert(oldCluster,newClusterId);
            ClusterInfo clusterInfo = clusterInfoMapTemp[clusterToCreate];
            dataType firstSpikePosition = clusterInfo.firstSpikePosition();
            dataType nbSpikesOfCluster = clusterInfo.nbSpikes();

            clusterInfoMapTemp2->remove(clusterToCreate);
            clusterInfoMapTemp2->insert(newClusterId,clusterInfo);

            //For the new cluster, only the row index has been inserted in spikesByClusterTemp,
            //now the cluster number is updated at once for all the spikes of the new cluster
            for(dataType i = 0; i<nbSpikesOfCluster;++i)
                (*spikesByClusterTemp)(2,firstSpikePosition + i) = newClusterId;
        }


        //Sort the spikes of the newly created clusters, the information to do so have been store in the 2 lists
        //firstPositions and nbOfspikes by increasing Id of the cluster of origin.
        //NB: the iterator iterates on the items sorted by their key, here from fromToNewClusterIds
        QMap<int,int>::Iterator iterator;
        int i = 0;
        for(iterator = fromToNewClusterIds.begin(); iterator != fromToNewClusterIds.end(); ++iterator){
            sortCluster(clusterInfoMapTemp2,spikesByClusterTemp,iterator.value(),firstPositions[i],nbOfspikes[i],-1);
            ++i;
        }

        //Get the list of clusters before applying the changes, this will be used in the clean
        //of the correlation.
        QList<dataType> currentClusterList = clusterIds();

        //Deal with the undo mechanism.
        prepareUndo(spikesByClusterTemp,clusterInfoMapTemp2);

        //If some spikes have been taken from the cluster 0, the max and min
        // dimensions have to be recalculated. If minMaxThread is running, the call
        //will wait until it finishes before starting the thread again.
        if(fromToNewClusterIds.contains(0)){
            //If the minMaxThread has not finish, wait until it is done
            while(!minMaxThread->wait()){};
            //Reset the flag to false so the minMaxThread can do the computation
            clusterZeroJustModified = false;
            minMaxThread->start();
            minMaxThread->setModifiedClusters(fromToNewClusterIds.keys());
            dimensionChangedUndo.prepend(true);
        }
        else dimensionChangedUndo.prepend(false);

        //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster.
        //if there is not a thread working with them,otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
        // and the thread will remove it.
        QMap<int,int>::Iterator fromToNewClusterIdsIterator;
        for(fromToNewClusterIdsIterator = fromToNewClusterIds.begin(); fromToNewClusterIdsIterator != fromToNewClusterIds.end(); ++fromToNewClusterIdsIterator){
            int clusterId = fromToNewClusterIdsIterator.key();
            mutex.lock();
            if(waveformStatusMap.contains(clusterId)){
                if(!waveformStatusMap[clusterId].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(clusterId));
                    waveformStatusMap.remove(clusterId);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[clusterId];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(clusterId,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(clusterId))) cleanCorrelation(static_cast<dataType>(clusterId),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(clusterId),true);
                mutex.unlock();
            }
        }
    }
    return fromToNewClusterIds;
}


/*
  The deletion of spikes from a cluster means moving those spikes from a given cluster
  to either the cluster 0 or the cluster one.
  The main algorithm is the following:
  Create a temporarily spikesByClusterTemp and clusterInfoMapTemp where the new configuration will be store
  If possible copy the cluster 0 and the destination cluster into the temporarily variables.
  Loop on all the clusters id in decreasing order, except the cluster 0 and the destination cluster (if it is the cluster 1).
  If the cluster is not concern (not currently in the view) copy it as it is. Otherwise loop on each of its spike
  to test if it is in the region, if so copy it a the top of spikesByClusterTemp (where the destination cluster is beeing build)
  otherwise, copy it at the bottom. Create the entry of the current cluster into clusterInfoMapTemp
  Create the entry of the new cluster destination into clusterInfoMapTemp
  There are special cases which have to be taken into account:
  Cluster 0 or cluster 1 does not exist.
  Cluster one is the destination and cluster 0 can contain spikes to be deleted.
 */
void Data::deleteSpikesFromClusters(QRegion& region, const QList <int>& clustersOfOrigin, int destinationCluster, int dimensionX, int dimensionY, QList <int>& fromClusters,QList <int>& emptyClusters){
    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create the variables to store the number of spikes and the position of the last spike
    //for each cluster contributing to the new cluster and the position of the first spike and the number
    //of spikes for the current cluster destination (cluster 0 or cluster 1).
    //This will be used to sort the new cluster.
    QList<dataType> positions;
    QList<dataType> nbOfspikes;
    dataType firstPosition = 0;
    dataType number = 0;

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    dataType upperInsertionIndex = 1;
    dataType lowerInsertionIndex = nbSpikes + 1;
    dataType nbSpikesInNewCluster = 0;
    dataType nbNewSpikesInNewCluster = 0;
    dataType lastSpikePositionForCurrentClusterPlus1 = 1;
    dataType firstSpikePositionForNewCluster = 1;

    //First process the case of cluster zero and one
    //Zero has to be take care of

    if(destinationCluster == 0){
        //Copy cluster 0 as it is if exists
        if(clusterInfoMap->contains(0)){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[0];
            dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();
            dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();

            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            //Store the first spike position for the cluster 0
            firstPosition = 1;
            //Store the number of spikes coming from the cluster 0
            number = nbSpikesOfCluster;

            upperInsertionIndex += nbSpikesOfCluster;

            //Initialize the number of spikes for the new cluster one with the information coming
            //from the current one
            firstSpikePositionForNewCluster = firstSpikePosition;
            nbSpikesInNewCluster = nbSpikesOfCluster;
            lastSpikePositionForCurrentClusterPlus1 = upperInsertionIndex;
        }
    }
    if(destinationCluster == 1){
        if(clusterInfoMap->contains(0)){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[0];
            dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();
            dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();

            //Copy the cluster 0 as it is
            if(!clustersOfOrigin.contains(0)){
                memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                       &(*spikesByCluster)(1,firstSpikePosition),
                       nbSpikesOfCluster * sizeof(dataType));
                memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                       &(*spikesByCluster)(2,firstSpikePosition),
                       nbSpikesOfCluster * sizeof(dataType));

                //Construct the new clusterInfoMap
                clusterInfoMapTemp->insert(0,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster));
                upperInsertionIndex += nbSpikesOfCluster;
                //assign the position of the first spike of cluster 1
                firstSpikePositionForNewCluster = nbSpikesOfCluster + 1;
            }
            else{
                //copy the points which are not in the region at the top and
                //copy the points which are in the region at a lower index starting at the number of spikes of the cluster 0
                dataType newNbSpikesOfCluster = nbSpikesOfCluster;
                dataType zeroLowerInsertionIndex =  nbSpikesOfCluster;
                dataType lastPosition =  firstSpikePosition + nbSpikesOfCluster;
                dataType lastPositionLessOne =  lastPosition -1;

                for(dataType i = firstSpikePosition; i < lastPosition;++i){
                    dataType featuresRowIndex = (*spikesByCluster)(1,i);
                    if(region.contains(
                                QPoint(features(featuresRowIndex,dimensionX),
                                       features(featuresRowIndex,dimensionY)))){
                        //Add the spike to the new cluster <=> add the row index at the end of spikesByCluster at the lowerInsertionIndex
                        (*spikesByClusterTemp)(1,zeroLowerInsertionIndex) = featuresRowIndex;
                        --zeroLowerInsertionIndex;
                        ++nbSpikesInNewCluster;
                        ++nbNewSpikesInNewCluster;
                        --newNbSpikesOfCluster;
                    }
                    else{
                        //Keep the spike in the current cluster <=> add the row index and the cluster number at the bottom of spikesByCluster at the lowerInsertionIndex
                        (*spikesByClusterTemp)(1,upperInsertionIndex) = featuresRowIndex;
                        (*spikesByClusterTemp)(2,upperInsertionIndex) = (*spikesByCluster)(2,i);
                        ++upperInsertionIndex;
                    }

                    if(i == (lastPositionLessOne)){
                        if(newNbSpikesOfCluster < nbSpikesOfCluster){
                            //Store the last spike position for the cluster 0
                            positions.append(nbSpikesOfCluster - newNbSpikesOfCluster);
                            //Store the number of spikes coming from the cluster 0
                            nbOfspikes.append(nbSpikesOfCluster - newNbSpikesOfCluster);

                            //update fromClusters if at least one spike from cluster 0 was in the region
                            fromClusters.append(0);
                        }
                        //Construct the insertion of the current cluster in the new clusterInfoMap if
                        // the number of spikes is more than zero
                        if(newNbSpikesOfCluster >0)clusterInfoMapTemp->insert(0,ClusterInfo(1,newNbSpikesOfCluster));
                        else emptyClusters.append(0);
                    }
                }
                //For the new cluster, only the row index has been inserted in spikesByClusterTemp,
                //now the cluster number is updated at once for all the spikes of the new cluster coming from cluster 0
                for(dataType i = 0; i<nbNewSpikesInNewCluster;++i) (*spikesByClusterTemp)(2,upperInsertionIndex + i) = destinationCluster;
                firstSpikePositionForNewCluster =  upperInsertionIndex;//when the last spike has been tested upperInsertionIndex =  zeroLowerInsertionIndex+1
                lastSpikePositionForCurrentClusterPlus1 = upperInsertionIndex = firstSpikePositionForNewCluster + nbSpikesInNewCluster;
                //reset nbNewSpikesInNewCluster
                nbNewSpikesInNewCluster = 0;
            }//else
        } //end of exists cluster 0

        if(clusterInfoMap->contains(1)){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[1];
            dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();
            dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();

            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            //Store the first spike position for the cluster 1
            firstPosition = nbSpikesInNewCluster + 1;
            //Store the number of spikes coming from the cluster 1
            number = nbSpikesOfCluster;

            upperInsertionIndex += nbSpikesOfCluster;

            //Initialize the number of spikes and the starting position for the new cluster one with the information coming
            //from the current one.
            nbSpikesInNewCluster += nbSpikesOfCluster;
            lastSpikePositionForCurrentClusterPlus1 = upperInsertionIndex;
        }
    }

    //process all the other clusters

    //Iteration on the clusters in decreasing order
    QList<dataType> clusters = clusterInfoMap->keys();
    qSort(clusters);
    int nbClusters = clusters.size();

    for(int i = nbClusters - 1; i >=0 ; --i){
        dataType clusterId = clusters[i];

        if(clusterId == destinationCluster || clusterId == 0) continue;

        dataType firstSpikePosition = (*clusterInfoMap)[clusterId].firstSpikePosition();
        dataType nbSpikesOfCluster = (*clusterInfoMap)[clusterId].nbSpikes();
        //The user information of the different clusters.
        QString structure = (*clusterInfoMap)[clusterId].getStructure();
        QString	type = (*clusterInfoMap)[clusterId].getType();
        QString	iD = (*clusterInfoMap)[clusterId].getId();
        QString	quality = (*clusterInfoMap)[clusterId].getQuality();
        QString	notes = (*clusterInfoMap)[clusterId].getNotes();


        //if clustersOfOrigin does not contains the current cluster, this cluster is let unchanged
        //and its information is simply copy as is from spikesByCluster to spikesByClusterTemp
        if(!clustersOfOrigin.contains(static_cast<int>(clusterId))){
            lowerInsertionIndex -= nbSpikesOfCluster;
            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,lowerInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,lowerInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(clusterId,ClusterInfo(lowerInsertionIndex,nbSpikesOfCluster,structure,type,iD,quality,notes));
        }
        //Now deal with the clusters which may contain spikes to add to the new cluster
        //<=> spike in the region. Look up the spikes staring from the last one.
        else{
            dataType newNbSpikesOfCluster = nbSpikesOfCluster;
            dataType lastPosition =  firstSpikePosition - 1;

            for(dataType i = firstSpikePosition + nbSpikesOfCluster - 1; i > lastPosition;--i){
                dataType featuresRowIndex = (*spikesByCluster)(1,i);
                if(region.contains(
                            QPoint(features(featuresRowIndex,dimensionX),
                                   features(featuresRowIndex,dimensionY)))){
                    //Add the spike to the new cluster <=> add the row index at the end of spikesByCluster at the lowerInsertionIndex
                    (*spikesByClusterTemp)(1,upperInsertionIndex) = featuresRowIndex;
                    ++upperInsertionIndex;
                    ++nbSpikesInNewCluster;
                    ++nbNewSpikesInNewCluster;
                    --newNbSpikesOfCluster;
                }
                else{
                    //Keep the spike in the current cluster <=> add the row index and the cluster number at the bottom of spikesByCluster at the lowerInsertionIndex
                    --lowerInsertionIndex;
                    (*spikesByClusterTemp)(1,lowerInsertionIndex) = featuresRowIndex;
                    (*spikesByClusterTemp)(2,lowerInsertionIndex) = (*spikesByCluster)(2,i);
                }

                if(i == (firstSpikePosition)){
                    if(newNbSpikesOfCluster < nbSpikesOfCluster){
                        //Store the last spike position for the current cluster
                        positions.append(nbSpikesInNewCluster);
                        //Store the number of spikes coming from the current cluster
                        nbOfspikes.append(nbSpikesOfCluster - newNbSpikesOfCluster);

                        //If the destination cluster is cluster 0,the max and min dimensions have to
                        //be recalculated. If minMaxThread is running, clusterZeroJustModified will
                        //inform it that it has to stop (the computation will be done again on the new data).
                        if(destinationCluster == 0) clusterZeroJustModified = true;

                        //update fromClusters if at least one spike from that cluster was in the region
                        fromClusters.append(static_cast<int>(clusterId));
                    }
                    //Construct the insertion of the current cluster in the new clusterInfoMap if
                    // the number of spikes is more than zero
                    if(newNbSpikesOfCluster >0)clusterInfoMapTemp->insert(clusterId,ClusterInfo(lowerInsertionIndex,newNbSpikesOfCluster,structure,type,iD,quality,notes));
                    else emptyClusters.append(static_cast<int>(clusterId));
                }
            }
        }
    }

    //For the new cluster, only the row index has been inserted in spikesByClusterTemp,
    //now the cluster number is updated at once for all the spikes of the new cluster
    for(dataType i = 0; i<nbNewSpikesInNewCluster;++i) (*spikesByClusterTemp)(2,lastSpikePositionForCurrentClusterPlus1 + i) = destinationCluster;

    if(nbSpikesInNewCluster > 0){
        if(clustersOfOrigin.contains(destinationCluster)){
            //update fromClusters for the cluster destination
            fromClusters.append(destinationCluster);
        }
        //Construct the insertion of the new cluster in the new clusterInfoMap
        clusterInfoMapTemp->insert(destinationCluster,ClusterInfo(firstSpikePositionForNewCluster,nbSpikesInNewCluster));

        //Sort the spikes of the newly created cluster.
        sortCluster(clusterInfoMapTemp,spikesByClusterTemp,destinationCluster,positions,nbOfspikes,firstPosition,number);

        //Get the list of clusters before applying the changes, this will be used in the clean
        //of the correlation.
        QList<dataType> currentClusterList = clusterIds();

        //Deal with the undo mechanism
        prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

        //If the spikes have been sent to the cluster 0, the max and min
        // dimensions have to be recalculated. If minMaxThread is running, the call
        //will wait until it finishes before starting the thread again.
        if(destinationCluster == 0){
            //If the minMaxThread has not finish, wait until it is done
            while(!minMaxThread->wait()){};
            //Reset the flag to false so the minMaxThread can do the computation
            clusterZeroJustModified = false;
            minMaxThread->setModifiedClusters(fromClusters);
            minMaxThread->start();
            dimensionChangedUndo.prepend(true);
        }
        else dimensionChangedUndo.prepend(false);

        //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster.
        //if there is not a thread working with them, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
        // and the thread will remove it.
        QList<int>::iterator iterator;
        for(iterator = fromClusters.begin(); iterator != fromClusters.end(); ++iterator){
            mutex.lock();
            if(waveformStatusMap.contains(*iterator)){
                if(!waveformStatusMap[*iterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                    waveformStatusMap.remove(*iterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*iterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*iterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*iterator))) cleanCorrelation(static_cast<dataType>(*iterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*iterator),true);
                mutex.unlock();
            }
        }
    }
}

void Data::moveClustersToArtefact(QList <int>& clustersToDelete){
    //If clustersToDelete is not empty, the cluster 0 will be modified and the max and min dimensions
    //have to be recalculated. If minMaxThread is running, clusterZeroJustModified will
    //inform it that it has to stop (the computation will be done again on the new data).
    if(clustersToDelete.size() != 0) clusterZeroJustModified = true;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    //Create the variables to store the number of spikes and the position of the first spike
    //for each cluster contributing to the new cluster. This will be used to sort the new cluster.
    QList<dataType> positions;
    QList<dataType> nbOfspikes;

    dataType upperInsertionIndex = 1;

    //If the cluster 0 exits copy the the 2 rows of spikesByCluster
    dataType nbSpikesInNewClusterZero;
    dataType nbSpikesInCurrentClusterZero;
    dataType lastSpikePositionForCurrentClusterZeroPlus1;

    if(clusterInfoMap->contains(0)){
        ClusterInfo currentClusterInfo = (*clusterInfoMap)[0];
        dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();
        dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();

        memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
               &(*spikesByCluster)(1,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));
        memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
               &(*spikesByCluster)(2,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));

        upperInsertionIndex += nbSpikesOfCluster;

        //Initialize the number of spikes for the new cluster 0 with the information coming
        //from the current one if it exists
        nbSpikesInNewClusterZero = nbSpikesInCurrentClusterZero = nbSpikesOfCluster;
        lastSpikePositionForCurrentClusterZeroPlus1 = upperInsertionIndex;

        //Store the first spike position for the original cluster 0
        positions.append(1);
        //Store the number of spikes coming from the original cluster 0v
        nbOfspikes.append(nbSpikesOfCluster);
    }
    //cluster 0 does not exist
    else{
        nbSpikesInNewClusterZero = nbSpikesInCurrentClusterZero = 0;
        lastSpikePositionForCurrentClusterZeroPlus1 = upperInsertionIndex;
    }

    //Move the clusters contain in clustersToDelete to cluster 0 and leave the others as they are
    moveClusters(clustersToDelete,spikesByClusterTemp,clusterInfoMapTemp,upperInsertionIndex,nbSpikesInNewClusterZero,0,positions,nbOfspikes);

    //For the new cluster 0, only the row index has been inserted in spikesByClusterTemp,
    //now the cluster number is updated at once for all the new spikes of the new cluster 0
    dataType nbNewSpikesInClusterZero = nbSpikesInNewClusterZero - nbSpikesInCurrentClusterZero;
    for(dataType i = 0; i<nbNewSpikesInClusterZero;++i) (*spikesByClusterTemp)(2,lastSpikePositionForCurrentClusterZeroPlus1 + i) = 0;

    //Construct the entry for cluster 0 in clusterInfoMap
    clusterInfoMapTemp->insert(0,ClusterInfo(1,nbSpikesInNewClusterZero));

    //Sort the spikes of the newly created cluster.
    sortCluster(clusterInfoMapTemp,spikesByClusterTemp,0,positions,nbOfspikes,1,true);

    //Get the list of clusters before applying the changes, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //Deal with the undo mechanism
    prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

    //The max and min dimensions have to be recalculated.
    //If the minMaxThread has not finish, wait until it is done
    while(!minMaxThread->wait()){qDebug()<<"wait for minMaxThread to finish";};
    //Reset the flag to false so the minMaxThread can do the computation
    clusterZeroJustModified = false;
    minMaxThread->setModifiedClusters(clustersToDelete);
    minMaxThread->start();
    dimensionChangedUndo.prepend(true);

    //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster 0.
    //if there is not a thread working with them, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    QList<int>::iterator iterator;
    for(iterator = clustersToDelete.begin(); iterator != clustersToDelete.end(); ++iterator){
        mutex.lock();
        if(waveformStatusMap.contains(*iterator)){
            if(!waveformStatusMap[*iterator].isInProcess()){
                delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                waveformStatusMap.remove(*iterator);
            }
            else{
                WaveformStatus waveformStatus = waveformStatusMap[*iterator];
                WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                waveformStatusCopy.setClusterModified(true);
                waveformStatusMap.insert(*iterator,waveformStatusCopy);
            }
        }
        mutex.unlock();
        if(!correlationsInProcess.contains(static_cast<dataType>(*iterator))) cleanCorrelation(static_cast<dataType>(*iterator),currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(static_cast<dataType>(*iterator),true);
            mutex.unlock();
        }
    }

    //remove the waveform and correlation data for the cluster 0 if clustersToDelete is not empty <=> cluster 0 will change
    //and if there is not a thread working with it, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    if(!clustersToDelete.empty()){
        mutex.lock();
        if(!waveformStatusMap[0].isInProcess()){
            delete waveformDict.take("0");
            waveformStatusMap.remove(0);
        }
        else{
            WaveformStatus waveformStatus = waveformStatusMap[0];
            WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
            waveformStatusCopy.setClusterModified(true);
            waveformStatusMap.insert(0,waveformStatusCopy);
        }
        mutex.unlock();
        if(!correlationsInProcess.contains(0)) cleanCorrelation(0,currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(0,true);
            mutex.unlock();
        }
    }
}


void Data::moveClustersToNoise(QList<int>& clustersToDelete){
    //If clustersToDelete contains the cluster 0, the max and min dimensions
    //have to be recalculated. If minMaxThread is running, clusterZeroJustModified will
    //inform it that it has to stop (the computation will be done again on the new data).
    if(clustersToDelete.contains(0)) clusterZeroJustModified = true;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    //Create the variables to store the number of spikes and the position of the first spike
    //for each cluster contributing to the new cluster. This will be used to sort the new cluster.
    QList<dataType> positions;
    QList<dataType> nbOfspikes;

    dataType upperInsertionIndex = 1;

    //If the cluster 0 exists and is not a cluster to delete, copy the 2 rows of spikesByCluster.
    // If cluster 1 exists copy the the 2 rows of spikesByCluster
    int i, max;
    if(clusterInfoMap->contains(0) && !clustersToDelete.contains(0)) i = 0;
    else i = 1;
    if(clusterInfoMap->contains(1)) max = 2;
    else max = 1;
    dataType nbSpikesOfCluster = 0;
    dataType firstSpikePosition;
    for(;i<max;++i){
        ClusterInfo currentClusterInfo = (*clusterInfoMap)[i];
        nbSpikesOfCluster = currentClusterInfo.nbSpikes();
        firstSpikePosition = currentClusterInfo.firstSpikePosition();

        memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
               &(*spikesByCluster)(1,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));
        memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
               &(*spikesByCluster)(2,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));

        //Construct the new clusterInfoMap, the entry for cluster 1 will be updated later
        clusterInfoMapTemp->insert(i,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster));

        upperInsertionIndex += nbSpikesOfCluster;

        //Store the information on the spikes of the original cluster 1
        if(i == 1){
            //Store the first spike position for the the original cluster 1
            positions.append(1);
            //Store the number of spikes coming from the original cluster 1
            nbOfspikes.append(nbSpikesOfCluster);
        }
    }

    //Initialize the number of spikes for the new cluster one with the information coming
    //from the current one if it exists
    dataType nbSpikesInNewClusterOne;
    dataType nbSpikesInCurrentClusterOne;
    dataType lastSpikePositionForCurrentClusterOnePlus1;
    if(max == 2){
        nbSpikesInNewClusterOne = nbSpikesInCurrentClusterOne = nbSpikesOfCluster; //nbSpikesOfCluster currently contents the info for cluster 1
        lastSpikePositionForCurrentClusterOnePlus1 = upperInsertionIndex; //upperInsertionIndex currently contents the info for cluster 1
    }
    //max == 1 <=> cluster 1 does not exist
    else{
        nbSpikesInNewClusterOne = nbSpikesInCurrentClusterOne = 0;
        lastSpikePositionForCurrentClusterOnePlus1 = upperInsertionIndex;

        //Construct the new clusterInfoMap, the entry for cluster 1 will be updated later
        clusterInfoMapTemp->insert(1,ClusterInfo(upperInsertionIndex,nbSpikesInNewClusterOne));
    }

    //Move the clusters contain in clustersToDelete to cluster 1 and leave the others as they are
    moveClusters(clustersToDelete,spikesByClusterTemp,clusterInfoMapTemp,upperInsertionIndex,nbSpikesInNewClusterOne,1,positions,nbOfspikes);

    //For the new cluster 1, only the row index has been inserted in spikesByClusterTemp,
    //now the cluster number is updated at once for all the new spikes of the new cluster 1
    dataType nbNewSpikesInClusterOne = nbSpikesInNewClusterOne - nbSpikesInCurrentClusterOne;
    for(dataType i = 0; i<nbNewSpikesInClusterOne;++i) (*spikesByClusterTemp)(2,lastSpikePositionForCurrentClusterOnePlus1 + i) = 1;

    //Update the new clusterInfoMap for the cluster 1
    ClusterInfo clusterOneInfo = (*clusterInfoMapTemp)[1];
    clusterOneInfo.setNbSpikes(nbSpikesInNewClusterOne);
    clusterInfoMapTemp->insert(1,clusterOneInfo);

    //Sort the spikes of the newly created cluster.
    sortCluster(clusterInfoMapTemp,spikesByClusterTemp,1,positions,nbOfspikes,1,true);

    //Get the list of clusters before applying the changes, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //Deal with the undo mechanism
    prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

    //The max and min dimensions have to be recalculated.
    //If the minMaxThread has not finish, wait until it is done
    if(clustersToDelete.contains(0)){
        while(!minMaxThread->wait()){qDebug()<<"wait for minMaxThread to finish"; };
        //Reset the flag to false so the minMaxThread can do the computation
        clusterZeroJustModified = false;
        QList<int> modifiedClusters;
        minMaxThread->setModifiedClusters(modifiedClusters);
        minMaxThread->start();
        dimensionChangedUndo.prepend(true);
    }
    else dimensionChangedUndo.prepend(false);


    //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster 1.
    //if there is not a thread working with them, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    QList<int>::iterator iterator;
    for(iterator = clustersToDelete.begin(); iterator != clustersToDelete.end(); ++iterator){
        mutex.lock();
        if(waveformStatusMap.contains(*iterator)){
            if(!waveformStatusMap[*iterator].isInProcess()){
                delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                waveformStatusMap.remove(*iterator);
            }
            else{
                WaveformStatus waveformStatus = waveformStatusMap[*iterator];
                WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                waveformStatusCopy.setClusterModified(true);
                waveformStatusMap.insert(*iterator,waveformStatusCopy);
            }
        }
        mutex.unlock();
        if(!correlationsInProcess.contains(static_cast<dataType>(*iterator))) cleanCorrelation(static_cast<dataType>(*iterator),currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(static_cast<dataType>(*iterator),true);
            mutex.unlock();
        }
    }

    //remove the waveform and correlation data for the cluster 1 if clustersToDelete is not empty <=> cluster 1 will change
    //and if there is not a thread working with it, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    if(!clustersToDelete.empty()){
        mutex.lock();
        if(!waveformStatusMap[1].isInProcess()){
            delete waveformDict.take("1");
            waveformStatusMap.remove(1);
        }
        else{
            WaveformStatus waveformStatus = waveformStatusMap[1];
            WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
            waveformStatusCopy.setClusterModified(true);
            waveformStatusMap.insert(1,waveformStatusCopy);
        }
        mutex.unlock();
        if(!correlationsInProcess.contains(1)) cleanCorrelation(1,currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(1,true);
            mutex.unlock();
        }
    }
}

dataType Data::groupClusters(QList<int>& clustersToGroup){
    //If the clusters to group contain the cluster 0, the max and min
    // dimensions have to be recalculated. If minMaxThread is running, clusterZeroJustModified will
    //inform it that it has to stop (the computation will be done again on the new data).
    if(clustersToGroup.contains(0)) clusterZeroJustModified = true;

    //Set the new cluster number to the biggest existing number plus one
    dataType newClusterId = (*spikesByCluster)(2,nbSpikes) + 1;
    dataType nbSpikesInNewCluster = 0;

    //Create the variables to store the number of spikes and the position of the first spike
    //for each cluster contributing to the new cluster. This will be used to sort the new cluster.
    QList<dataType> positions;
    QList<dataType> nbOfspikes;

    //The user information of the different clusters to be grouped will be concatenated.
    QString newStructure;
    QString	newType;
    QString	newID;
    QString	newQuality;
    QString	newNotes;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    dataType upperInsertionIndex = 1;
    dataType lowerInsertionIndex = nbSpikes + 1;

    //Iteration on the clusters
    ClusterInfoMap::Iterator iterator;

    //Variable used to determined
    bool first = true;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator) {
        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType clusterId = iterator.key();

        //if clustersToGroup does not contains the current cluster, this cluster is let unchanged
        //and its information is simply copy as is from spikesByCluster to spikesByClusterTemp
        if(!clustersToGroup.contains(static_cast<int>(clusterId))){
            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(clusterId,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
            upperInsertionIndex += nbSpikesOfCluster;
        }
        //Now deal with the clusters which are to be grouped and need to be added to the new cluster
        else{
            memcpy(&(*spikesByClusterTemp)(1,lowerInsertionIndex - nbSpikesOfCluster),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            nbSpikesInNewCluster += nbSpikesOfCluster;
            lowerInsertionIndex -= nbSpikesOfCluster;

            //Store the first spike position for the current cluster
            positions.append(nbSpikesInNewCluster);
            //Store the number of spikes coming from the current cluster
            nbOfspikes.append(nbSpikesOfCluster);

            //Take care of the user information about the current cluster
            if(first){
                newStructure += iterator.value().getStructure();
                newType += iterator.value().getType();
                newID += iterator.value().getId();
                newQuality += iterator.value().getQuality();
                newNotes += iterator.value().getNotes();

                first = false;
            }
            else{
                newStructure += "--" + iterator.value().getStructure();
                newType += "--" + iterator.value().getType();
                newID += "--" + iterator.value().getId();
                newQuality += "--" + iterator.value().getQuality();
                newNotes += "--" + iterator.value().getNotes();
            }
        }
    }

    //For the new cluster, only the row index has been inserted in spikesByClusterTemp,
    //now the cluster number is updated at once for all the spikes of the new cluster
    for(dataType i = 0; i<nbSpikesInNewCluster;++i) (*spikesByClusterTemp)(2,lowerInsertionIndex + i) = newClusterId;

    //Construct the insertion of the new cluster in the new clusterInfoMap
    clusterInfoMapTemp->insert(newClusterId,ClusterInfo(lowerInsertionIndex,nbSpikesInNewCluster,newStructure,newType,newID,newQuality,newNotes));

    //Sort the spikes of the newly created cluster.
    sortCluster(clusterInfoMapTemp,spikesByClusterTemp,newClusterId,positions,nbOfspikes,1);

    //Get the list of clusters before applying the grouping, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //Deal with the undo mechanism
    prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

    //If the clusters to group contain the cluster 0, the max and min
    // dimensions have to be recalculated.
    if(clustersToGroup.contains(0)){
        //If the minMaxThread has not finish, wait until it is done
        while(!minMaxThread->wait()){};
        //Reset the flag to false so the minMaxThread can do the computation
        clusterZeroJustModified = false;
        minMaxThread->setModifiedClusters(clustersToGroup);
        minMaxThread->start();
        dimensionChangedUndo.prepend(true);
    }
    else dimensionChangedUndo.prepend(false);

    //Remove the waveform and correlation data for the clusters which gave the spikes for the new cluster.
    //if there is not a thread working with them, otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    QList<int>::iterator clustersToGroupIterator;
    for(clustersToGroupIterator = clustersToGroup.begin(); clustersToGroupIterator != clustersToGroup.end(); ++clustersToGroupIterator){

        mutex.lock();
        if(waveformStatusMap.contains(*clustersToGroupIterator)){
            if(!waveformStatusMap[*clustersToGroupIterator].isInProcess()){
                delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToGroupIterator));
                waveformStatusMap.remove(*clustersToGroupIterator);
            }
            else{
                WaveformStatus waveformStatus = waveformStatusMap[*clustersToGroupIterator];
                WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                waveformStatusCopy.setClusterModified(true);
                waveformStatusMap.insert(*clustersToGroupIterator,waveformStatusCopy);
            }
        }
        mutex.unlock();

        if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToGroupIterator))) cleanCorrelation(static_cast<dataType>(*clustersToGroupIterator),currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToGroupIterator),true);
            mutex.unlock();
        }
    }

    return newClusterId;
}


void Data::prepareUndo(SortableTable* spikesByClusterTemp,ClusterInfoMap* clusterInfoMapTemp){
    //Store the current spikesByCluster in the undo list and make the temporary becomes the current one.
    spikesByClusterUndoList.prepend(spikesByCluster);
    //Store the current map in the undo list and make the temporary become the current one.
    clusterInfoMapUndoList.prepend(clusterInfoMap);

    mutex.lock();
    clusterInfoMap = clusterInfoMapTemp;
    spikesByCluster = spikesByClusterTemp;
    mutex.unlock();

    //if the number of undo has been reach remove the last element in the undo list (first inserted)
    int currentSpikesByClusterNbUndo = spikesByClusterUndoList.count();
    if(currentSpikesByClusterNbUndo > nbUndo)
        delete spikesByClusterUndoList.takeAt(currentSpikesByClusterNbUndo - 1);

    //if the number of undo has been reach remove the last element in the undo list (first inserted)
    int currentClusterInfoNbUndo = clusterInfoMapUndoList.count();
    if(currentClusterInfoNbUndo > nbUndo)
        delete clusterInfoMapUndoList.takeAt(currentClusterInfoNbUndo - 1);

    //Clear the redoLists
    qDeleteAll(spikesByClusterRedoList);
    spikesByClusterRedoList.clear();
    qDeleteAll(clusterInfoMapRedoList);
    clusterInfoMapRedoList.clear();
}

void Data::nbUndoChangedCleaning(int newNbUndo){
    //if the new number of possible undo is smaller than the current one,
    // clean the undo/redo related variables.
    if(newNbUndo < nbUndo){
        int currentNbUndo = spikesByClusterUndoList.count();
        //if the current number of undo is bigger than the new number of undo,
        // remove the last elements in the undo lists (first ones inserted).
        if(currentNbUndo > newNbUndo){
            while(currentNbUndo > newNbUndo){
                delete spikesByClusterUndoList.takeAt(currentNbUndo - 1);
                delete clusterInfoMapUndoList.takeAt(currentNbUndo - 1);
                currentNbUndo = spikesByClusterUndoList.count();
            }
            //Clear the redoLists
            qDeleteAll(spikesByClusterRedoList);
            spikesByClusterRedoList.clear();
            qDeleteAll(clusterInfoMapRedoList);
            clusterInfoMapRedoList.clear();
        }
        //currentNbUndo < newNbUndo, check the redo list.
        else{
            //number of undo and redo must be <= new number of undo. Remove redo elements if need it.
            int currentNbRedo = spikesByClusterRedoList.count();
            if((currentNbRedo + currentNbUndo) > newNbUndo){
                while((currentNbRedo + currentNbUndo) > newNbUndo){
                    delete clusterInfoMapRedoList.takeAt(currentNbRedo - 1);
                    delete spikesByClusterRedoList.takeAt(currentNbRedo - 1);
                    currentNbRedo = spikesByClusterRedoList.count();
                }
            }
        }
    }
}

void Data::moveClusters(QList<int>& clustersToDelete,SortableTable* spikesByClusterTemp,ClusterInfoMap* clusterInfoMapTemp,long upperInsertionIndex,long& nbSpikesInNewCluster,int destinationId,QList<long>& positions,QList<long>& nbOfspikes){

    //For all the clusters to delete, copy the first row of spikesByCluster into spikesByClusterTemp
    //right after the data coming from the current cluster destination (0 or 1)
    QList<int>::iterator clustersToDeleteIterator;
    for(clustersToDeleteIterator = clustersToDelete.begin(); clustersToDeleteIterator != clustersToDelete.end(); ++clustersToDeleteIterator ){
        dataType clusterId = static_cast<dataType>(*clustersToDeleteIterator);
        if(clusterId == destinationId) continue;
        ClusterInfo currentClusterInfo = (*clusterInfoMap)[clusterId];
        dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();
        dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();

        memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
               &(*spikesByCluster)(1,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));

        //Store the first spike position for the current cluster
        positions.append(nbSpikesInNewCluster + 1);
        //Store the number of spikes coming from the current cluster
        nbOfspikes.append(nbSpikesOfCluster);

        upperInsertionIndex += nbSpikesOfCluster;
        nbSpikesInNewCluster += nbSpikesOfCluster;
    }

    //Copy the 2 rows for all the other clusters
    //Iteration on the clusters starting with the cluster following cluster 1
    ClusterInfoMap::Iterator iterator;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator){
        dataType clusterId = iterator.key();
        if(clusterId <= destinationId) continue;
        if(!clustersToDelete.contains(static_cast<int>(clusterId))){
            dataType firstSpikePosition = iterator.value().firstSpikePosition();
            dataType nbSpikesOfCluster = iterator.value().nbSpikes();

            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(clusterId,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
            upperInsertionIndex += nbSpikesOfCluster;
        }
    }
}

void Data::undo(QList<int>& addedClusters,QList<int>& updatedClusters){
    //Inform that an undo is in process
    undoRedoInProcess = true;

    qDebug()<<"in Data::undo 1";

    //Get the list of clusters before applying the changes, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //If addedClusters or updatedClusters contain any cluster, remove the corresponding entry in waveformDict and correlationDict
    //(the data will have to be uploaded again) if there is not a thread working with it,
    //otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    if(!addedClusters.isEmpty() ){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = addedClusters.begin(); clustersToRemoveIterator != addedClusters.end(); ++clustersToRemoveIterator){

            qDebug()<<"in Data::undo addedClusters.size() > 0, *clustersToRemoveIterator: "<<*clustersToRemoveIterator;
            mutex.lock();
            if(waveformStatusMap.contains(*clustersToRemoveIterator)){
                if(!waveformStatusMap[*clustersToRemoveIterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToRemoveIterator));
                    waveformStatusMap.remove(*clustersToRemoveIterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*clustersToRemoveIterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*clustersToRemoveIterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            qDebug()<<"in Data::undo addedClusters.size() > 0, *clustersToRemoveIterator: "<<*clustersToRemoveIterator<<", correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator): "<<correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator));



            if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator))) cleanCorrelation(static_cast<dataType>(*clustersToRemoveIterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToRemoveIterator),true);
                mutex.unlock();
            }
        }
    }
    if(!updatedClusters.isEmpty()){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = updatedClusters.begin(); clustersToRemoveIterator != updatedClusters.end(); ++clustersToRemoveIterator){

            qDebug()<<"in Data::undo updatedClusters.size() > 0, *clustersToRemoveIterator: "<<*clustersToRemoveIterator;

            mutex.lock();
            if(waveformStatusMap.contains(*clustersToRemoveIterator)){
                if(!waveformStatusMap[*clustersToRemoveIterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToRemoveIterator));
                    waveformStatusMap.remove(*clustersToRemoveIterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*clustersToRemoveIterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*clustersToRemoveIterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator))) cleanCorrelation(static_cast<dataType>(*clustersToRemoveIterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToRemoveIterator),true);
                mutex.unlock();
            }
        }
    }

    //if addedClusters and updatedClusters are both empty, the undo concern the renumbering
    //Can not do much, all the data will have to be reloaded (it shoud not happen very often)
    if(addedClusters.isEmpty() && updatedClusters.isEmpty()){
        //Gets all the clustersId currently available
        QList<dataType> clusters = clusterIds();

        //Loop on all the clusters and delete the linke information if possible (if a thread is not
        //working with it) otherwise, modify the status so the thread will be delete the onformation.
        QList<dataType>::iterator iterator;
        for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator){

            qDebug()<<"in Data::undo addedClusters.isEmpty() && updatedClusters.isEmpty(), *iterator: "<<*iterator;


            mutex.lock();
            if(waveformStatusMap.contains(static_cast<int>(*iterator))){
                if(!waveformStatusMap[static_cast<int>(*iterator)].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                    waveformStatusMap.remove(static_cast<int>(*iterator));
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[static_cast<int>(*iterator)];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(static_cast<int>(*iterator),waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(*iterator)) cleanCorrelation(*iterator,clusters);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(*iterator,true);
                mutex.unlock();
            }
        }
    }

    //If clusterInfoMapUndoList is not empty, make the current clusterInfoMap become the first element
    //of the clusterInfoMapRedoList and the first element of the clusterInfoMapUndoList become the current clusterInfoMap.
    //Do the same with the spikesByCluster
    if(!clusterInfoMapUndoList.isEmpty()){

        clusterInfoMapRedoList.prepend(clusterInfoMap);
        ClusterInfoMap* clusterInfoMapTemp = clusterInfoMapUndoList.takeAt(0);
        spikesByClusterRedoList.prepend(spikesByCluster);
        SortableTable* spikesByClusterTemp = spikesByClusterUndoList.takeAt(0);

        mutex.lock();
        clusterInfoMap =  clusterInfoMapTemp;

        qDebug()<<"in Data::undo 2, clusterInfoMap updated";

        spikesByCluster =  spikesByClusterTemp;

        mutex.unlock();

        qDebug()<<"in Data::undo 3, spikesByCluster updated";

        //If the last action implied a changed of the dimension, change the dimension again
        if(!dimensionChangedUndo.isEmpty() && dimensionChangedUndo.at(0) == true){


            qDebug()<<"in Data::undo dimensionChangedUndo[0] == true";


            //If the minMaxThread has not finish, wait until it is done
            while(!minMaxThread->wait()){};

            //Reset the flag to false so the minMaxThread can do the computation
            undoRedoInProcess = false;
            QList<int> modifiedClusters;
            minMaxThread->setModifiedClusters(modifiedClusters);
            minMaxThread->start();
            dimensionChangedRedo.prepend(true);
        }
    }
    qDebug()<<"in Data::undo end";
}


void Data::redo(QList<int>& addedClusters,QList<int>& updatedClusters,QList<int>& deletedClusters){
    //Inform that a redo is in process
    undoRedoInProcess = true;

    //Get the list of clusters before applying the changes, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //If addedClusters or updatedClusters contain any cluster, remove the corresponding entry in waveformDict and correlationDict
    //(the data will have to be uploaded again).
    if(!addedClusters.isEmpty() ){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = addedClusters.begin(); clustersToRemoveIterator != addedClusters.end(); ++clustersToRemoveIterator){
            mutex.lock();
            if(waveformStatusMap.contains(*clustersToRemoveIterator)){
                if(!waveformStatusMap[*clustersToRemoveIterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToRemoveIterator));
                    waveformStatusMap.remove(*clustersToRemoveIterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*clustersToRemoveIterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*clustersToRemoveIterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator))) cleanCorrelation(static_cast<dataType>(*clustersToRemoveIterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToRemoveIterator),true);
                mutex.unlock();
            }
        }
    }

    if(updatedClusters.size() > 0){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = updatedClusters.begin(); clustersToRemoveIterator != updatedClusters.end(); ++clustersToRemoveIterator){
            mutex.lock();
            if(waveformStatusMap.contains(*clustersToRemoveIterator)){
                if(!waveformStatusMap[*clustersToRemoveIterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToRemoveIterator));
                    waveformStatusMap.remove(*clustersToRemoveIterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*clustersToRemoveIterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*clustersToRemoveIterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator))) cleanCorrelation(static_cast<dataType>(*clustersToRemoveIterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToRemoveIterator),true);
                mutex.unlock();
            }
        }
    }

    if(!deletedClusters.isEmpty()){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = deletedClusters.begin(); clustersToRemoveIterator != deletedClusters.end(); ++clustersToRemoveIterator){
            mutex.lock();
            if(waveformStatusMap.contains(*clustersToRemoveIterator)){
                if(!waveformStatusMap[*clustersToRemoveIterator].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*clustersToRemoveIterator));
                    waveformStatusMap.remove(*clustersToRemoveIterator);
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[*clustersToRemoveIterator];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(*clustersToRemoveIterator,waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(static_cast<dataType>(*clustersToRemoveIterator))) cleanCorrelation(static_cast<dataType>(*clustersToRemoveIterator),currentClusterList);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(static_cast<dataType>(*clustersToRemoveIterator),true);
                mutex.unlock();
            }
        }
    }


    //if addedClusters and updatedClusters are both empty, the undo concern the renumbering
    //Can do much, all the data will have to be reloaded (it shoud not happen very often)
    if(addedClusters.isEmpty() && updatedClusters.isEmpty()){
        //Gets all the clustersId currently available
        QList<dataType> clusters = clusterIds();

        //Loop on all the clusters and delete the linke information if possible (if a thread is not
        //working with it) otherwise, modify the status so the thread will be delete the onformation.
        QList<dataType>::iterator iterator;
        for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator){
            mutex.lock();
            if(waveformStatusMap.contains(static_cast<int>(*iterator))){
                if(!waveformStatusMap[static_cast<int>(*iterator)].isInProcess()){
                    delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                    waveformStatusMap.remove(static_cast<int>(*iterator));
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[static_cast<int>(*iterator)];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(static_cast<int>(*iterator),waveformStatusCopy);
                }
            }
            mutex.unlock();
            if(!correlationsInProcess.contains(*iterator)) cleanCorrelation(*iterator,clusters);
            else{
                mutex.lock();
                correlationsInProcess.setClusterModified(*iterator,true);
                mutex.unlock();
            }
        }
    }

    //If clusterInfoMapRedoList is not empty, make the current clusterInfoMap become the first element
    //of the clusterInfoMapUndoList and the first element of the clusterInfoMapRedoList become the current clusterInfoMap.
    //Do the same with the spikesByCluster
    if(!clusterInfoMapRedoList.isEmpty()){
        clusterInfoMapUndoList.prepend(clusterInfoMap);
        ClusterInfoMap* clusterInfoMapTemp = clusterInfoMapRedoList.takeAt(0);
        spikesByClusterUndoList.prepend(spikesByCluster);
        SortableTable* spikesByClusterTemp = spikesByClusterRedoList.takeAt(0);

        mutex.lock();
        clusterInfoMap =  clusterInfoMapTemp;
        spikesByCluster =  spikesByClusterTemp;
        mutex.unlock();

        //If the last redo implied a changed of the dimension, change the dimension again
        if(!dimensionChangedRedo.isEmpty() && dimensionChangedRedo.at(0) == true){
            //If the minMaxThread has not finish, wait until it is done
            while(!minMaxThread->wait()){};

            //Reset the flag to false so the minMaxThread can do the computation
            undoRedoInProcess = false;
            QList<int> modifiedClusters;
            minMaxThread->setModifiedClusters(modifiedClusters);
            minMaxThread->start();
            dimensionChangedUndo.prepend(true);
        }
    }
}

void Data::renumber(QMap<int,int>& clusterIdsOldNew,QMap<int,int>& clusterIdsNewOld){
    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    //process the clusters 0 and 1 separately as, if they exist, they are never renumber.
    for(int i = 0; i < 2; ++i){
        if(clusterInfoMap->contains(i)){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[i];
            dataType nbSpikesOfCluster = currentClusterInfo.nbSpikes();
            dataType firstSpikePosition = currentClusterInfo.firstSpikePosition();

            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,firstSpikePosition),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,firstSpikePosition),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));

            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(i,ClusterInfo(firstSpikePosition,nbSpikesOfCluster));
            clusterIdsOldNew.insert(i,i);
            clusterIdsNewOld.insert(i,i);
        }
    }

    //Iteration on the clusters
    ClusterInfoMap::Iterator iterator;
    int clusterNumber = 2;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator) {
        dataType firstSpikePosition = iterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = iterator.value().nbSpikes();
        dataType clusterId = iterator.key();

        //The clusters 0 and 1 have been processed separately before
        if(clusterId == 0 || clusterId == 1) continue;

        //Insert into spikesByClusterTemp and clusterInfoMapTemp with the new number

        //copy the first row of spikesByCluster for the given cluster
        memcpy(&(*spikesByClusterTemp)(1,firstSpikePosition),
               &(*spikesByCluster)(1,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));

        if(clusterId == clusterNumber){
            //copy the second row as it is
            memcpy(&(*spikesByClusterTemp)(2,firstSpikePosition),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
        }
        //renumber
        else{
            //Insert the new cluster id in the second row.
            for(long i = 0; i<nbSpikesOfCluster;++i) (*spikesByClusterTemp)(2,firstSpikePosition + i) = clusterNumber;
            //If waveformDict or correlationDict contain that cluster, change the key for it.
            mutex.lock();
            if(waveformStatusMap.contains(static_cast<int>(clusterId))){
                if(!waveformStatusMap[static_cast<int>(clusterId)].isInProcess()){
                    Waveforms* waveforms = waveformDict.take(QString::fromLatin1("%1").arg(clusterId));
                    waveformDict.insert(QString::fromLatin1("%1").arg(clusterNumber),waveforms);
                    WaveformStatus waveformStatus = waveformStatusMap[static_cast<int>(clusterId)];
                    waveformStatusMap.insert(clusterNumber,waveformStatus);
                    waveformStatusMap.remove(static_cast<int>(clusterId));
                }
                else{
                    WaveformStatus waveformStatus = waveformStatusMap[static_cast<int>(clusterId)];
                    WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                    waveformStatusCopy.setClusterModified(true);
                    waveformStatusMap.insert(static_cast<int>(clusterId),waveformStatusCopy);
                }
            }
            mutex.unlock();
        }
        //Construct the new clusterInfoMap
        clusterInfoMapTemp->insert(clusterNumber,ClusterInfo(firstSpikePosition,nbSpikesOfCluster,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes()));
        clusterIdsOldNew.insert(static_cast<int>(clusterId),clusterNumber);
        clusterIdsNewOld.insert(clusterNumber,static_cast<int>(clusterId));

        ++clusterNumber;
    }

    //Renumber the correlations, this is not done in the loop because the complet mapping has
    //to be known in order to do it.
    renumberCorrelation(clusterIdsOldNew);

    //Deal with the undo mechanism
    prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);
}

bool Data::saveClusters(FILE* clusterFile){
    RestartTimer();

    int writeStatus = 0;

    //Sort spikesByCluster by spikes order

    //The mutex protects spikesByCluster and clusterInfoMap so that only one thread can
    //access them at the time.
    ClusterInfoMap clusterInfoMapTemp;
    ClusterInfoMap::Iterator it;
    mutex.lock();
    SortableTable spikesByClusterTemp(*spikesByCluster);

    bool overLimit = false;
    for(it = clusterInfoMap->begin(); it != clusterInfoMap->end(); ++it){
        clusterInfoMapTemp.insert(it.key(),it.value());
        if(it.value().nbSpikes() > 250000) overLimit = true;
    }
    mutex.unlock();

    int nbClusters = clusterInfoMapTemp.count();

    //first line of the file contains the number of clusters
    writeStatus = fprintf(clusterFile, "%i\n",nbClusters);

    //first method
    if(nbClusters > 12 && !overLimit){
        spikesByClusterTemp.sort(1);

        //Store all the clusterIds in spikes order
        for(long i = 1; i <= nbSpikes ; ++i)
            writeStatus = fprintf(clusterFile, "%i\n",static_cast<int>((spikesByClusterTemp)(2,i)));
    }
    //second method
    else if (nbClusters > 1){
        QList<dataType> clusterList = clusterInfoMapTemp.keys();
        QList<dataType>::iterator iterator;

        SortableTable final = SortableTable();
        final.setSize(nbSpikes,false);
        dataType position = 1;
        dataType cluster = spikesByClusterTemp(2,position);
        dataType value = spikesByClusterTemp(1,position);

        while(true){
            for(iterator = clusterList.begin(); iterator != clusterList.end(); ++iterator){
                dataType current = spikesByClusterTemp(1,clusterInfoMapTemp[*iterator].firstSpikePosition());
                if(current < value){
                    value = current;
                    cluster = *iterator;
                }
            }
            final(1,position) = cluster;
            position ++;
            dataType firstSpikePosition = clusterInfoMapTemp[cluster].firstSpikePosition() + 1;
            clusterInfoMapTemp[cluster].setFirstSpikePosition(firstSpikePosition);
            dataType nbSpikesOfCluster = clusterInfoMapTemp[cluster].nbSpikes() - 1;
            clusterInfoMapTemp[cluster].setNbSpikes(nbSpikesOfCluster);
            if(nbSpikesOfCluster == 0){
                nbClusters --;
                clusterList.removeAll(cluster);
                clusterInfoMapTemp.remove(cluster);
            }
            if(nbClusters == 0) break;
            cluster = clusterList[0];
            value = spikesByClusterTemp(1,clusterInfoMapTemp[cluster].firstSpikePosition());
        }

        //Store all the clusterIds in spikes order
        for(long i = 1; i <= nbSpikes ; ++i)
            writeStatus = fprintf(clusterFile, "%i\n",static_cast<int>((final)(1,i)));
    }//nbClusters>1
    //third method
    else{
        //Store all the clusterIds in spikes order
        for(long i = 1; i <= nbSpikes ; ++i)
            writeStatus = fprintf(clusterFile, "%i\n",static_cast<int>((spikesByClusterTemp)(2,i)));
    }
    qDebug() << "save clu file: "<<Timer() << endl;
    if(writeStatus > 0) return 1;
    else return 0;
}

bool Data::spikePositions(int clusterId,SortableTable& subsetTable){

    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId))) return false;

    //Lock the mutex to protect the changes as a whole
    mutex.lock();

    ClusterInfo clusterInfo  = (*clusterInfoMap)[clusterId];
    dataType firstSpikePosition = clusterInfo.firstSpikePosition();
    dataType nbSpikesOfCluster = clusterInfo.nbSpikes();

    spikesByCluster->subset(subsetTable,1,firstSpikePosition,firstSpikePosition + nbSpikesOfCluster - 1);
    mutex.unlock();

    return true;
}



Data::Status Data::getSampleWaveformPoints(int clusterId,dataType nbSpkToDisplay){
    //If the cluster has been suppress after the thread calling this function has been launched
    //return this information that the data are not available.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)))return NOT_AVAILABLE;

    //Take a sample of the spikes (displayNbSpikes) evenly distributed on all the recording.

    QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
    SortableTable positionOfSpikes = SortableTable();
    Waveforms* waveforms;
    dataType nbSpikesOfCluster = 0;

    //Does this cluster has already been processed?
    if(waveformStatusMap.contains(clusterId)){
        Status status = waveformStatusMap[clusterId].sampleStatus();
        if(status == IN_PROCESS)return IN_PROCESS;
        waveforms = waveformDict[clusterIdString];
        //status == READY with the same number of spikes to present
        if((waveforms->nbOfSpikesAsked() == nbSpkToDisplay) && (status == READY))return READY;
        //status == READY with a different number of spikes to present, recollect the data
        mutex.lock();
        waveformStatusMap[clusterId].setSampleStatus(IN_PROCESS);
        mutex.unlock();
        //Check if there not a mean calculation in process, is so wait until it finishes before doing anything
        while(waveformStatusMap[clusterId].sampleMeanStatus() == IN_PROCESS);
        //check if the cluster has not been removed while the mean function was running
        //if so the entry in waveformStatusMap for that cluster will have been removed  in the mean function
        if(!waveformStatusMap.contains(clusterId)) return NOT_AVAILABLE;
        mutex.lock();
        waveformStatusMap[clusterId].setSampleMeanStatus(NOT_AVAILABLE);
        mutex.unlock();

        //Check again that the cluster has not been removed or modified and get the spikes positions in a one row SortableTable.
        if(!spikePositions(clusterId,positionOfSpikes) || waveformStatusMap[clusterId].isClusterModified()){
            mutex.lock();
            waveformStatusMap[clusterId].setClusterModified(false);
            delete waveformDict.take(clusterIdString); //not already done by the function which modified the data as the thread is running.
            waveformStatusMap.remove(clusterId);
            mutex.unlock();
            return NOT_AVAILABLE;
        }
        waveforms->setNbOfSpikesAsked(nbSpkToDisplay);
        //Get the spikes information
        nbSpikesOfCluster = positionOfSpikes.nbOfColumns();
        waveforms->setSize(nbSpikesOfCluster,SAMPLE);
    }
    else{
        mutex.lock();
        waveformStatusMap.insert(clusterId,WaveformStatus(IN_PROCESS));
        mutex.unlock();
        if(isTwoBytesRecording) waveforms = new WaveformData<short>(*this);
        else waveforms = new WaveformData<long>(*this);

        //Check that the cluster has not been removed or modified and get the spikes positions in a one row SortableTable.
        if(!spikePositions(clusterId,positionOfSpikes) || waveformStatusMap[clusterId].isClusterModified()){
            mutex.lock();
            waveformStatusMap[clusterId].setClusterModified(false);
            delete waveformDict.take(clusterIdString); //not already done by the function which modified the data as the thread is running.
            waveformStatusMap.remove(clusterId);
            mutex.unlock();
            return NOT_AVAILABLE;
        }

        waveforms->setNbOfSpikesAsked(nbSpkToDisplay);
        //Get the spikes information
        nbSpikesOfCluster = positionOfSpikes.nbOfColumns();

        waveforms->setSize(nbSpikesOfCluster,SAMPLE);
        waveformDict.insert(clusterIdString,waveforms);
    }

    FILE* spikeFile = fopen(spkFileName.toLatin1(),"r");
    if(spikeFile == NULL){
        // OPEN_ERROR;  ///The openning pb has to be taken into account
    }

    //read and store the data
    waveforms->read(positionOfSpikes,nbSpikesOfCluster,spikeFile,nbSpkToDisplay);
    fclose(spikeFile);

    //If the cluster has been suppress or modified after the thread calling this function has been launched
    //return this information that the data are not available and remove the collected data.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)) || waveformStatusMap[clusterId].isClusterModified()){
        mutex.lock();
        waveformStatusMap[clusterId].setClusterModified(false);
        delete waveformDict.take(clusterIdString);  //not already done by the function which modified the data as the thread is running.
        waveformStatusMap.remove(clusterId);
        mutex.unlock();
        return NOT_AVAILABLE;
    }
    else{
        //Store the information in waveformStatusMap
        mutex.lock();
        waveformStatusMap[clusterId].setSampleStatus(READY);
        mutex.unlock();
        return READY;
    }
}

Data::Status Data::getTimeFrameWaveformPoints(int clusterId,dataType start,dataType end){
    //If the cluster has been suppress after the thread calling this function has been launched
    //return this information that the data are not available.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)))return NOT_AVAILABLE;

    //Take all the spikes in a given time frame
    QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
    SortableTable positionOfSpikes = SortableTable();
    dataType nbSpikesOfCluster = 0;
    dataType startInRecordingUnits = start * static_cast<dataType>(1000000.0 / samplingInterval);
    dataType endInRecordingUnits =  end * static_cast<dataType>(1000000.0 / samplingInterval);
    dataType currentSpikeIndex  = 0;
    Waveforms* waveforms;

    //Does this cluster has already been processed?
    if(waveformStatusMap.contains(clusterId)){
        Status status = waveformStatusMap[clusterId].timeFrameStatus();
        if(status == IN_PROCESS)return IN_PROCESS;
        waveforms = waveformDict[clusterIdString];
        dataType timeEndIndex = waveforms->indexOfTimeEnd();
        dataType timeStart = waveforms->startTime();
        dataType timeEnd = waveforms->endTime();

        //status == READY with the time frame
        if(timeStart == start && timeEnd == end && status == READY) return READY;
        mutex.lock();
        waveformStatusMap[clusterId].setTimeFrameStatus(IN_PROCESS);
        mutex.unlock();
        //Check if there not a mean calculation in process, is so wait until it finishes before doing anything
        while(waveformStatusMap[clusterId].timeFrameMeanStatus() == IN_PROCESS);
        //check if the cluster has not been removed while the mean function was running
        //if so the entry in waveformStatusMap for that cluster will have been removed  in the mean function
        if(!waveformStatusMap.contains(clusterId)) return NOT_AVAILABLE;
        mutex.lock();
        waveformStatusMap[clusterId].setTimeFrameMeanStatus(NOT_AVAILABLE);
        mutex.unlock();

        //Check again that the cluster has not been removed or modifed and get the spikes positions in a one row SortableTable.
        if(!spikePositions(clusterId,positionOfSpikes) || waveformStatusMap[clusterId].isClusterModified()){
            mutex.lock();
            waveformStatusMap[clusterId].setClusterModified(false);
            delete waveformDict.take(clusterIdString); //not already done by the function which modified the data as the thread is running.
            waveformStatusMap.remove(clusterId);
            mutex.unlock();
            return NOT_AVAILABLE;
        }

        //Get the spikes information
        nbSpikesOfCluster = positionOfSpikes.nbOfColumns();
        waveforms->setSize(nbSpikesOfCluster,TIME_FRAME);
        //status == READY with a different time frame, recollect the data
        //Start from the last position where the spikes have been taken
        //(timeEndIndex is one step after the last spike taken).
        if(start == timeEnd) currentSpikeIndex =  timeEndIndex;
    }
    else{
        mutex.lock();
        waveformStatusMap.insert(clusterId,WaveformStatus(NOT_AVAILABLE,IN_PROCESS));
        mutex.unlock();
        if(isTwoBytesRecording) waveforms = new WaveformData<short>(*this);
        else waveforms = new WaveformData<long>(*this);

        //Check that the cluster has not been removed or modified and get the spikes positions in a one row SortableTable.
        if(!spikePositions(clusterId,positionOfSpikes) || waveformStatusMap[clusterId].isClusterModified()){
            mutex.lock();
            waveformStatusMap[clusterId].setClusterModified(false);
            delete waveformDict.take(clusterIdString); //not already done by the function which modified the data as the thread is running.
            waveformStatusMap.remove(clusterId);
            mutex.unlock();
            return NOT_AVAILABLE;
        }
        //Get the spikes information
        nbSpikesOfCluster = positionOfSpikes.nbOfColumns();

        waveforms->setSize(nbSpikesOfCluster,TIME_FRAME);
        waveformDict.insert(clusterIdString,waveforms);
    }

    //Look for the starting position if not already known
    if(currentSpikeIndex == 0){
        dataType max = nbSpikesOfCluster + 1;
        dataType currentPositionInFeatures = 0;
        dataType currentTime = 0;
        for(currentSpikeIndex = 1; currentSpikeIndex < max; ++currentSpikeIndex){
            currentPositionInFeatures = positionOfSpikes(1,currentSpikeIndex);
            currentTime = features(currentPositionInFeatures,nbDimensions);
            if(currentTime >= startInRecordingUnits) break;
        }
    }

    FILE* spikeFile = fopen(spkFileName.toLatin1(),"r");
    if(spikeFile == NULL){
        // OPEN_ERROR;  ///The openning pb has to be taken into account
    }

    //read and store the data
    waveforms->read(positionOfSpikes,nbSpikesOfCluster,spikeFile,currentSpikeIndex,endInRecordingUnits);

    fclose(spikeFile);

    //If the cluster has been suppress or modified after the thread calling this function has been launched
    //return this information that the data are not available and remove the collected data.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)) || waveformStatusMap[clusterId].isClusterModified()){
        mutex.lock();
        waveformStatusMap[clusterId].setClusterModified(false);
        delete waveformDict.take(clusterIdString); //if not already done by the function which modified the data
        waveformStatusMap.remove(clusterId);
        mutex.unlock();
        return NOT_AVAILABLE;
    }
    else{
        //Store the information in waveforms and waveformStatusMap
        waveforms->setStartTime(start);
        waveforms->setEndTime(end);
        waveforms->setIndexOfTimeEnd(currentSpikeIndex);

        //Store the information in waveformStatusMap
        mutex.lock();
        waveformStatusMap[clusterId].setTimeFrameStatus(READY);
        mutex.unlock();
        return READY;
    }
}

template <class T>
void Data::WaveformData<T>::setSize(dataType size,WaveformMode waveformMode){
    mode = waveformMode;
    if(mode == SAMPLE){
        if(sampleSpikesTable) delete []sampleSpikesTable;
        sampleSpikesTable = new T[size * nbPtsBySpike];
        if(sampleMeanTable){
            delete []sampleMeanTable;
            sampleMeanTable = 0L;
            delete []sampleStDeviationTable;
            sampleStDeviationTable = 0L;
        }
        nbSampleSpikes = 0;
    }
    else{
        if(timeFrameSpikesTable) delete[]timeFrameSpikesTable;
        timeFrameSpikesTable = new T[size * nbPtsBySpike];
        if(timeFrameMeanTable){
            delete []timeFrameMeanTable;
            timeFrameMeanTable = 0L;
            delete []timeFrameStDeviationTable;
            timeFrameStDeviationTable = 0L;
        }
        nbTimeFrameSpikes = 0;
    }
}


template <class T>
void Data::WaveformData<T>::read(SortableTable& positionOfSpikes,dataType nbSpikesOfCluster,FILE* spikeFile,dataType nbSpkToDisplay){
    //Show nbSpkToDisplay spikes or all the spikes if nbSpikesOfCluster < nbSpkToDisplay
    if(nbSpikesOfCluster < nbSpkToDisplay){
        dataType max = nbSpikesOfCluster +1;
        dataType position = 0;
        for(dataType i = 1; i < max; ++i){
            //go to the spike position
            dataType currentSpikePosition = (positionOfSpikes(1,i) - 1) * nbPtsBySpike ;
            fseeko64(spikeFile,currentSpikePosition * sizeof(T),SEEK_SET);
            // copy the spikes into spikePoints.
            fread(&(sampleSpikesTable[position]),sizeof(T),nbPtsBySpike,spikeFile);
            position += nbPtsBySpike;
            ++nbSampleSpikes;
        }
    }
    //If there is only one spike to show, take the first one
    else if(nbSpkToDisplay == 1){
        //go to the spike position
        dataType currentSpikePosition = (positionOfSpikes(1,1) - 1) * nbPtsBySpike ;
        fseeko64(spikeFile,currentSpikePosition * sizeof(T),SEEK_SET);
        // copy the spikes into spikePoints.
        fread(&(sampleSpikesTable[0]),sizeof(T),nbPtsBySpike,spikeFile);
        nbSampleSpikes = 1;
    }
    else{
        float factor = static_cast<float>(static_cast<float>(nbSpikesOfCluster - 1) / static_cast<float>(nbSpkToDisplay - 1));
        dataType position = 0;
        dataType max = nbSpkToDisplay +1;
        float floatSpkIndice = 1;
        dataType spkIndice;
        for(float i = 1; i < max; ++i){
            spkIndice = static_cast<dataType>(floatSpkIndice + 0.5);
            //go to the spike position
            dataType currentSpikePosition = (positionOfSpikes(1,spkIndice) - 1) * nbPtsBySpike ;
            fseeko64(spikeFile,currentSpikePosition * sizeof(T),SEEK_SET);
            // copy the spikes into spikePoints.
            fread(&(sampleSpikesTable[position]),sizeof(T),nbPtsBySpike,spikeFile);
            position += nbPtsBySpike;
            ++nbSampleSpikes;
            floatSpkIndice += factor;
        }
    }
}

template <class T>
void Data::WaveformData<T>::read(SortableTable& positionOfSpikes,dataType nbSpikesOfCluster,FILE* spikeFile,dataType& currentSpikeIndex,dataType end){
    dataType max = nbSpikesOfCluster +1;
    dataType position = 0;
    dataType startPositionInSpk;

    for(; currentSpikeIndex < max; ++currentSpikeIndex){
        dataType currentPositionInFeatures = positionOfSpikes(1,currentSpikeIndex);
        dataType currentTime = data.features(currentPositionInFeatures,data.nbDimensions);

        if(currentTime >= end) break;
        //positionOfSpikes and features take indices starting at 1, so currentPositionInFeatures
        //is already correct regarding the presence of an additional first line (nb of features) in the fet file.
        startPositionInSpk = (currentPositionInFeatures - 1) * nbPtsBySpike * sizeof(T);
        //go to the spike position
        fseeko64(spikeFile,startPositionInSpk,SEEK_SET);
        // copy the spikes into timeFrameSpikesTable.
        fread(&(timeFrameSpikesTable[position]),sizeof(T),nbPtsBySpike,spikeFile);
        position += nbPtsBySpike;
        ++nbTimeFrameSpikes;
    }
}

template <class T>
void Data::WaveformData<T>::calculateMean(WaveformMode waveformMode){
    if(waveformMode == SAMPLE){
        sampleMeanTable = new T[data.nbSamplesInWaveform * data.nbChannels];
        sampleStDeviationTable = new T[data.nbSamplesInWaveform * data.nbChannels];
        for(int i = 0; i < data.nbSamplesInWaveform; ++i){
            for(int j = 0; j < data.nbChannels; ++j){
                dataType sum = 0;
                dataType sumOfSquares = 0;
                for(dataType k = 0; k < nbSampleSpikes; ++k){
                    dataType value = sampleSpikesTable[k * (data.nbSamplesInWaveform * data.nbChannels) + (i * data.nbChannels) + j];
                    sum += value;
                    sumOfSquares += (value * value);
                }
                //The data are store as follow:
                //sample after sample and for each of them the value of channel after channel.
                dataType mean = sum / nbSampleSpikes;
                sampleMeanTable[(i * data.nbChannels) + j] = mean;
                //variance(X) = mean(X^2) - mean(X)^2
                dataType variance =  (sumOfSquares / nbSampleSpikes) - (mean * mean);
                //standard deviation = square root of the variance
                sampleStDeviationTable[(i * data.nbChannels) + j] = static_cast<dataType>(sqrt(static_cast<double>(variance)));
            }
        }
    }
    else{
        timeFrameMeanTable = new T[data.nbSamplesInWaveform * data.nbChannels];
        timeFrameStDeviationTable = new T[data.nbSamplesInWaveform * data.nbChannels];
        for(int i = 0; i < data.nbSamplesInWaveform; ++i){
            for(int j = 0; j < data.nbChannels; ++j){
                dataType sum = 0;
                dataType sumOfSquares = 0;
                for(dataType k = 0; k < nbTimeFrameSpikes; ++k){
                    dataType value = timeFrameSpikesTable[k * (data.nbSamplesInWaveform * data.nbChannels) + (i * data.nbChannels) + j];
                    sum += value;
                    sumOfSquares += (value * value);
                }
                //The data are store as follow:
                //sample after sample and for each of them the value of channel after channel.
                dataType mean = sum / nbTimeFrameSpikes;
                timeFrameMeanTable[(i * data.nbChannels) + j] = mean;
                //variance(X) = mean(X^2) - mean(X)^2
                dataType variance =  (sumOfSquares / nbTimeFrameSpikes) - (mean * mean);
                //standard deviation = square root of the variance
                timeFrameStDeviationTable[(i * data.nbChannels) + j] = static_cast<dataType>(sqrt(static_cast<double>(variance)));
            }
        }
    }
}

Data::Status Data::calculateSampleMean(int clusterId,dataType nbSpkToDisplay){
    //Calculate the mean and the standard deviation for
    //a sample of the spikes (displayNbSpikes) evenly distributed on all the recording.
    QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
    Waveforms* waveforms;

    //Does this cluster already processed?
    if(waveformStatusMap.contains(clusterId)){
        mutex.lock();
        Status status = waveformStatusMap[clusterId].sampleMeanStatus();
        waveforms = waveformDict[clusterIdString];
        mutex.unlock();
        if(status == IN_PROCESS)return IN_PROCESS;
        else if(waveforms->nbOfSpikesAsked() != nbSpkToDisplay) return NOT_AVAILABLE;
        //status == READY with the same number of spikes to present
        else if((waveforms->nbOfSpikesAsked() == nbSpkToDisplay) && (status == READY))return READY;
        else{
            if(waveformStatusMap[clusterId].sampleStatus() != READY) return NOT_AVAILABLE;
            if(waveforms->nbOfSpikes(SAMPLE) == 0){
                mutex.lock();
                waveformStatusMap[clusterId].setSampleMeanStatus(NOT_AVAILABLE);
                mutex.unlock();
                return READY;
            }
            mutex.lock();
            waveformStatusMap[clusterId].setSampleMeanStatus(IN_PROCESS);
            mutex.unlock();
        }
    }
    else return NOT_AVAILABLE;

    //calculate the mean and the standard deviation and store the data
    waveforms->calculateMean(SAMPLE);
    //If the cluster has been suppress or modified after the thread calling this function has been launched
    //return this information that the data are not available.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)) || waveformStatusMap[clusterId].isClusterModified()){
        mutex.lock();
        waveformStatusMap[clusterId].setClusterModified(false);
        delete waveformDict.take(clusterIdString);  //if not already done by the function which modified the data
        waveformStatusMap.remove(clusterId);;
        mutex.unlock();
        return NOT_AVAILABLE;
    }
    else{
        //Store the information in waveformStatusMap
        mutex.lock();
        waveformStatusMap[clusterId].setSampleMeanStatus(READY);
        mutex.unlock();
        return READY;
    }
}


Data::Status Data::calculateTimeFrameMean(int clusterId,dataType start,dataType end){
    //Calculate the mean and the standard deviation for
    //a sample of the spikes (displayNbSpikes) evenly distributed on all the recording.

    QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
    Waveforms* waveforms;

    //Does this cluster already processed?
    if(waveformStatusMap.contains(clusterId)){
        mutex.lock();
        Status status = waveformStatusMap[clusterId].timeFrameMeanStatus();
        waveforms = waveformDict[clusterIdString];
        mutex.unlock();
        dataType timeStart = waveforms->startTime();
        dataType timeEnd = waveforms->endTime();

        if(status == IN_PROCESS)return IN_PROCESS;
        else if(timeStart == start && timeEnd == end && status == READY) return READY;
        else{
            if(waveformStatusMap[clusterId].timeFrameStatus() != READY) return NOT_AVAILABLE;
            if(waveforms->nbOfSpikes(TIME_FRAME) == 0){
                mutex.lock();
                waveformStatusMap[clusterId].setTimeFrameMeanStatus(NOT_AVAILABLE);
                mutex.unlock();
                return READY;
            }
            mutex.lock();
            waveformStatusMap[clusterId].setTimeFrameMeanStatus(IN_PROCESS);
            mutex.unlock();
        }
    }
    else return NOT_AVAILABLE;


    //calculate the mean and the standard deviation and store the data
    waveforms->calculateMean(TIME_FRAME);

    //If the cluster has been suppress or modifed after the thread calling this function has been launched
    //return this information that the data are not available.
    if(!clusterInfoMap->contains(static_cast<dataType>(clusterId)) || waveformStatusMap[clusterId].isClusterModified()){
        mutex.lock();
        waveformStatusMap[clusterId].setClusterModified(false);
        delete waveformDict.take(clusterIdString);  //if not already done by the function which modified the data
        waveformStatusMap.remove(clusterId);
        mutex.unlock();
        return NOT_AVAILABLE;
    }
    else{
        //Store the information in waveformStatusMap
        mutex.lock();
        waveformStatusMap[clusterId].setTimeFrameMeanStatus(READY);
        mutex.unlock();
        return READY;
    }
}


void Data::sortCluster(ClusterInfoMap* clusterInfoMapTemp,SortableTable* spikesByClusterTemp, dataType clusterId,QList<dataType> positions,
                       QList<dataType> nbOfspikes,int step,bool fromTop){
    uint nbClusters = static_cast<uint>(positions.size());
    uint indice = 0;

    ClusterInfo clusterInfo  = (*clusterInfoMapTemp)[clusterId];
    dataType nbSpikesOfCluster = clusterInfo.nbSpikes();
    dataType firstSpikePosition = clusterInfo.firstSpikePosition();

    //Update positions to set the last spikes position correctly if need it.
    //The positions were counted from the last spike of the new cluster, now
    //they will be counted from the first spike of the new cluster.
    if(!fromTop){
        for(dataType i = 0; i < static_cast<dataType>(nbClusters); ++i){
            positions[i] = nbSpikesOfCluster - positions[i] + 1;
        }
    }

    SortableTable data = SortableTable();
    //Get the spike positions in a one row SortableTable.
    spikesByClusterTemp->subset(data,1,firstSpikePosition,firstSpikePosition + nbSpikesOfCluster - 1);

    SortableTable final = SortableTable();
    final.setSize(nbSpikesOfCluster,false);
    dataType position = 1;
    dataType value = data(1,positions[0]);

    //If step == -1, the spikes for each cluster are sorted from the first one to the last one
    //by decreasing order. They will be read from last one to the first one.
    //If step == 1, the spikes for each cluster are sorted from the first one to the last one
    //by increasing order. They will be read from first one to the last one.
    while(true){
        for(uint i = 1; i < nbClusters; ++i){
            dataType current = data(1,positions[i]);
            if(current < value){
                value = current;
                indice = i;
            }
        }
        final(1,position) = value;
        position ++;
        positions[indice] += step;
        nbOfspikes[indice] --;
        if(nbOfspikes[indice] == 0){
            nbClusters --;
            positions.removeOne(positions.at(indice));
            nbOfspikes.removeOne(nbOfspikes.at(indice));
        }
        if(nbClusters == 0) break;
        indice = 0;
        value = data(1,positions[0]);
    }

    //Copy the sorted data to spikesByClusterTemp
    memcpy(&(*spikesByClusterTemp)(1,firstSpikePosition),
           &final(1,1),
           nbSpikesOfCluster * sizeof(dataType));
}

void Data::sortCluster(ClusterInfoMap* clusterInfoMapTemp,SortableTable* spikesByClusterTemp,dataType clusterId,QList<dataType> lastPositions,QList<dataType> nbOfspikes,dataType firstPosition,dataType number){
    //Initialize the variables concerning the previous data of clusterId (which is 0 or 1).
    dataType originalFirstPosition = firstPosition;
    dataType originalNb = number;
    uint nbClusters = static_cast<uint>(lastPositions.size());
    uint indice = 0;

    ClusterInfo clusterInfo  = (*clusterInfoMapTemp)[clusterId];
    dataType nbSpikesOfCluster = clusterInfo.nbSpikes();
    dataType firstSpikePosition = clusterInfo.firstSpikePosition();

    SortableTable data = SortableTable();
    //Get the spikes positions in a one row SortableTable.
    spikesByClusterTemp->subset(data,1,firstSpikePosition,firstSpikePosition + nbSpikesOfCluster - 1);

    SortableTable final = SortableTable();
    final.setSize(nbSpikesOfCluster,false);
    dataType position = 1;
    dataType value = data(1,lastPositions[0]);

    //The spikes for each cluster are sorted from the last one to the first one
    //by increasing order.
    if(originalNb > 0){
        while(true){
            for(uint i = 1; i < nbClusters; ++i){
                dataType current = data(1,lastPositions[i]);
                if(current < value){
                    value = current;
                    indice = i;
                }
            }
            //work on the original spikes of the new cluster (which is either cluster 0 or cluster 1).
            dataType current = data(1,originalFirstPosition);
            if(current < value){
                final(1,position) = current;
                position ++;
                originalNb --;
                if(originalNb == 0)break;
                originalFirstPosition ++;
            }
            else{
                final(1,position) = value;
                position ++;
                lastPositions[indice]--;
                nbOfspikes[indice] --;
                if(nbOfspikes[indice] == 0){
                    nbClusters --;
                    lastPositions.removeOne(lastPositions.at(indice));
                    nbOfspikes.removeOne(nbOfspikes.at(indice));
                }
                //Copy all the reminding original spikes of the new cluster.
                if(nbClusters == 0){
                    memcpy(&(final(1,position)),
                           &(data(1,originalFirstPosition)),
                           originalNb * sizeof(dataType));
                    break;
                }
                indice = 0;
                value = data(1,lastPositions[0]);
            }
        }
    }
    //All the original spikes have been sort, now deal with the reminding ones coming
    //from the other clusters.
    if(originalNb == 0 && nbClusters != 0){
        indice = 0;
        value = data(1,lastPositions[0]);
        while(true){
            for(uint i = 1; i < nbClusters; ++i){
                dataType current = data(1,lastPositions[i]);
                if(current < value){
                    value = current;
                    indice = i;
                }
            }
            final(1,position) = value;
            position ++;
            lastPositions[indice]--;
            nbOfspikes[indice] --;
            if(nbOfspikes[indice] == 0){
                nbClusters --;
                lastPositions.removeOne(lastPositions.at(indice));
                nbOfspikes.removeOne(nbOfspikes.at(indice));
            }
            if(nbClusters == 0) break;
            indice = 0;
            value = data(1,lastPositions[0]);
        }
    }

    //Copy the sorted data to spikesByClusterTemp
    memcpy(&(*spikesByClusterTemp)(1,firstSpikePosition),
           &final(1,1),
           nbSpikesOfCluster * sizeof(dataType));
}

Data::Status Data::getCorrelograms(Pair& pair,int binSize,int timeWindow,double binSizeInRU,float timeWindowInRU,int halfBins){
    int cluster1 = pair.getX();
    int cluster2 = pair.getY();
    Pair parameters = Pair(binSize,timeWindow);
    QHash<QString, Correlation*>* dict = 0L;

    //Test first if the clusters still exist
    mutex.lock();
    bool cluster1Removed = !clusterInfoMap->contains(static_cast<dataType>(cluster1));
    bool cluster2Removed = !clusterInfoMap->contains(static_cast<dataType>(cluster2));
    mutex.unlock();

    if(cluster1Removed || cluster2Removed)return NOT_AVAILABLE;

    //Test if the correlogram is in process or already available.
    Status status = NOT_AVAILABLE;
    mutex.lock();
    if(correlationDict[pair.toString()] != 0){
        dict = correlationDict[pair.toString()];
        if((*dict)[parameters.toString()] != 0){
            if(((*dict)[parameters.toString()])->getStatus(binSize,timeWindow) == IN_PROCESS) status = IN_PROCESS;
            else if(((*dict)[parameters.toString()])->getStatus(binSize,timeWindow) == READY) status = READY;
        }
    }
    mutex.unlock();

    if(status != NOT_AVAILABLE) return status;

    //Test if the correlogram, for the given parametres, is not available, if so compute it.
    //If the pair does not exist or the binSize and/or the timeFrame are different, the correlogram will have to be computed.
    bool computeCorrelogram = false;
    mutex.lock();
    dict = correlationDict[pair.toString()];
    if(correlationDict[pair.toString()] == 0 || ((*dict)[parameters.toString()] == 0))
        computeCorrelogram = true;
    mutex.unlock();

    if(computeCorrelogram){
        //Advice that a correlation is in process on the cluster1 and cluster2.
        mutex.lock();
        correlationsInProcess.addProcess(static_cast<dataType>(cluster1));
        correlationsInProcess.addProcess(static_cast<dataType>(cluster2));
        mutex.unlock();

        //Create the correlation object or retrieve it if it already exists.
        //In case several threads, working on the same pair with the same parameters, get to this point, make sure that only one will
        //performs the computation.
        bool correlationAlreadyInProcess = false;
        Correlation* correlation = 0L;
        mutex.lock();
        dict = correlationDict[pair.toString()];
        if(dict == 0){
            dict = new QHash<QString, Correlation*>();
            correlation = new Correlation(*this,binSize,timeWindow);
            correlation->setStatus(IN_PROCESS);
            dict->insert(parameters.toString(),correlation);
            correlationDict.insert(pair.toString(),dict);
        }
        else if((*dict)[parameters.toString()] == 0){
            correlation = new Correlation(*this,binSize,timeWindow);
            correlation->setStatus(IN_PROCESS);
            dict->insert(parameters.toString(),correlation);
        }
        else if(((*dict)[parameters.toString()] != 0) && (*dict)[parameters.toString()]->getStatus() != IN_PROCESS){
            correlation = (*dict)[parameters.toString()];
            correlation->setStatus(IN_PROCESS);
            correlation->reset();
            correlation->setBinSize(binSize);
            correlation->setTimeWindow(timeWindow);
        }
        else correlationAlreadyInProcess = true;
        mutex.unlock();

        if(correlationAlreadyInProcess){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));
            mutex.unlock();
            return IN_PROCESS;
        }

        //If cluster1 or cluster2 have been suppress after the thread calling this function has been launched
        //skip this pair.
        bool clusterNotAvailable = false;
        bool autoCorrelogram = false;
        SortableTable spikesOfCluster1 = SortableTable();
        SortableTable spikesOfCluster2 =  SortableTable();

        if(cluster1 == cluster2) autoCorrelogram = true;

        //Get the spikes positions for the cluster1 in a one row SortableTable.
        if(!spikePositions(cluster1,spikesOfCluster1)){
            cleanCorrelation(static_cast<dataType>(cluster1),clusterIds(),true);
            clusterNotAvailable = true;
        }
        //Get the spikes positions for the cluster2 in a one row SortableTable.
        if(!autoCorrelogram && (!spikePositions(cluster2,spikesOfCluster2))){
            cleanCorrelation(static_cast<dataType>(cluster2),clusterIds(),true);
            clusterNotAvailable = true;
        }
        if(clusterNotAvailable){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));
            delete correlationDict.take(pair.toString()); //if the clusters do not exist anymore they would not have been
            //removed in cleanCorrelation
            mutex.unlock();
            return NOT_AVAILABLE;
        }

        //Check if either the cluster1 or the cluster2 have been modified since the thread has been launched
        if(correlationsInProcess.isClusterModified(static_cast<dataType>(cluster1))){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            delete correlationDict.take(pair.toString());
            mutex.unlock();

            clusterNotAvailable = true;
        }
        if(correlationsInProcess.isClusterModified(static_cast<dataType>(cluster2))){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));
            delete correlationDict.take(pair.toString());
            mutex.unlock();

            clusterNotAvailable = true;
        }
        if(clusterNotAvailable) return NOT_AVAILABLE;


        //Compute the correlogram.
        if(!autoCorrelogram) correlation->calculateCorrelation(spikesOfCluster1,spikesOfCluster2,binSizeInRU,timeWindowInRU,halfBins,autoCorrelogram);
        else correlation->calculateCorrelation(spikesOfCluster1,spikesOfCluster1,binSizeInRU,timeWindowInRU,halfBins,autoCorrelogram);

        //If cluster1 or cluster2 have been suppress or modifed after the thread calling this function has been launched
        //skip this pair.
        if(!clusterInfoMap->contains(static_cast<dataType>(cluster1))){
            cleanCorrelation(static_cast<dataType>(cluster1),clusterIds(),true);
            clusterNotAvailable = true;
        }
        if(!autoCorrelogram && (!clusterInfoMap->contains(static_cast<dataType>(cluster2)))){
            cleanCorrelation(static_cast<dataType>(cluster2),clusterIds(),true);
            clusterNotAvailable = true;
        }
        if(clusterNotAvailable){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));
            delete correlationDict.take(pair.toString()); //if the clusters do not exist anymore they would not have been
            //removed in cleanCorrelation
            mutex.unlock();
            return NOT_AVAILABLE;
        }

        if(correlationsInProcess.isClusterModified(static_cast<dataType>(cluster1))){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            delete correlationDict.take(pair.toString());
            mutex.unlock();
            clusterNotAvailable = true;
        }
        if(correlationsInProcess.isClusterModified(static_cast<dataType>(cluster2))){
            mutex.lock();
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));
            delete correlationDict.take(pair.toString());
            mutex.unlock();
            clusterNotAvailable = true;
        }
        if(clusterNotAvailable) return NOT_AVAILABLE;
        else{
            mutex.lock();

            //Update the status
            correlation->setStatus(READY);

            //Update the correlation status of the cluster1 and cluster2.
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster1));
            correlationsInProcess.removeProcess(static_cast<dataType>(cluster2));

            mutex.unlock();
        }
    }
    return READY;
}

void Data::Correlation::calculateCorrelation(SortableTable& spikesOfCluster1,SortableTable& spikesOfCluster2,double binSizeInRU,double timeWindowInRU,int halfBins,bool autoCorrelogram){
    dataType cluster1NbSpikesPlusOne = spikesOfCluster1.nbOfColumns() + 1;
    dataType cluster2NbSpikes = spikesOfCluster2.nbOfColumns();
    dataType cluster2NbSpikesPlusOne = cluster2NbSpikes + 1;
    dataType spikeOfCluster2 = 1;
    double timeOfCluster1;
    double lowerBound;
    double upperBound;

    int totalNbBins = (2 * halfBins) + 1;
    setNbBins(totalNbBins);
    //Initialize the array which will contain the correlogram data.
    values = new uint[totalNbBins];
    //One additional bin is used for the upper boundary (and his content is later added to the last bin)
    uint* tmpValues = new uint[totalNbBins + 1];
    memset(tmpValues,0,(totalNbBins + 1) * sizeof(uint));

    //Cluster 1 will be the cluster of reference.
    for(dataType spikeOfCluster1 = 1; spikeOfCluster1 < cluster1NbSpikesPlusOne; ++spikeOfCluster1){

        timeOfCluster1 = data.spikeTime(spikesOfCluster1,spikeOfCluster1);

        lowerBound = timeOfCluster1 - timeWindowInRU/2;
        upperBound = timeOfCluster1 + timeWindowInRU/2;

        //If the last spike of cluster2 is before the lower limit the computation is over.
        if(data.spikeTime(spikesOfCluster2,cluster2NbSpikes) < lowerBound) break;

        //Move along cluster2's spikes while the time of the spike is lower than lowerBound.
        for(;spikeOfCluster2 < cluster2NbSpikesPlusOne && data.spikeTime(spikesOfCluster2,spikeOfCluster2) < lowerBound; ++spikeOfCluster2){}

        //Loop backwards along cluster2's spikes and store the bin value as long as we still in the time frame (>= lowerBound)
        if(spikeOfCluster2 > 1){
            double backwardsTime;
            for(dataType backwardsSpike = spikeOfCluster2 - 1;backwardsSpike >= 1 && (backwardsTime = data.spikeTime(spikesOfCluster2,backwardsSpike)) >= lowerBound; --backwardsSpike){

                //calculate the bin.
                int bin = halfBins + static_cast<int>(floor(0.5 + (backwardsTime - timeOfCluster1)/ static_cast<double>(binSizeInRU)));
                if ( bin < 0 ) bin = 0;
                tmpValues[bin]++;
            }
        }

        //Loop forwards along cluster2's spikes and store the bin value as long as we still in the time frame (<= upperBound)
        double forwardsTime;
        //All the spikes at the limit of 2 bins are included in the left one if the time is not round.
        //The same thing is done for the last bin, so a spike of cluster2 having a time difference of timeWindowInRu
        //with a spike of cluster1 will not be computed.
        for(;spikeOfCluster2 < cluster2NbSpikesPlusOne && (forwardsTime = data.spikeTime(spikesOfCluster2,spikeOfCluster2)) < upperBound; ++spikeOfCluster2){
            //calculate the bin.
            int bin = halfBins + static_cast<int>(floor(0.5 + (forwardsTime - timeOfCluster1)/ static_cast<double>(binSizeInRU)));
            if ( bin < 0 ) bin = 0;
            tmpValues[bin]++;
        }

    }

    //If it is an autocorrelogram, remove the center bin.
    if(autoCorrelogram) tmpValues[halfBins] = 0;

    //Update last bin (see comment above)
    tmpValues[2 * halfBins] += tmpValues[totalNbBins];
    //Store values
    memcpy(values,tmpValues,totalNbBins * sizeof(uint));

    delete []tmpValues;

    //Calculate the maximum and the shoulder
    for(int i = 0; i < totalNbBins; ++i)
        if(values[i] > max) max = values[i];

    //Computation of the asymptote: N1*N2*(binSize/Time)
    //with Time, time between the first common spike and the last common spike.
    double time;
    if(autoCorrelogram){
        //Computation also of the firing rate: nbSpikes / Time converted in seconds.
        time = data.spikeTime(spikesOfCluster2,cluster2NbSpikes) - data.spikeTime(spikesOfCluster2,1);
        if(time == 0){
            asymptote = 0;
            firingRate = 0;
        }
        else{
            asymptote = static_cast<float>(
                        static_cast<double>(cluster2NbSpikes) * static_cast<double>(cluster2NbSpikes) *
                        (static_cast<double>(binSizeInRU) / static_cast<double>(time))
                        );

            double timeInS = static_cast<double>(time *data.samplingInterval) / 1000000.0;
            firingRate = static_cast<float>(
                        static_cast<double>(cluster2NbSpikes) / (static_cast<long>(timeInS) + 1 )
                        );
        }
    }
    else{
        double clu1T1 = data.spikeTime(spikesOfCluster1,1);
        double clu2T1 = data.spikeTime(spikesOfCluster2,1);
        double clu1T2 = data.spikeTime(spikesOfCluster1,cluster1NbSpikesPlusOne - 1);
        double clu2T2 = data.spikeTime(spikesOfCluster2,cluster2NbSpikes);
        double T1;
        double T2;
        dataType clu1Spk1;
        dataType clu1Spk2;
        dataType clu2Spk1;
        dataType clu2Spk2;

        if((clu1T2 < clu2T1) || (clu2T2 < clu1T1)) asymptote = 0;
        else{
            if(clu1T1 < clu2T1){
                T1 = clu2T1;
                if(clu1T2 < clu2T2){
                    T2 = clu1T2;
                    //Search the number of spikes of the 2 clusters whithin "time"
                    clu1Spk1 = data.findSpikePosition(T1,spikesOfCluster1);
                    clu1Spk2 = cluster1NbSpikesPlusOne - 1;
                    clu2Spk1 = 1;
                    clu2Spk2 = data.findSpikePosition(T2,spikesOfCluster2);
                }
                else{
                    T2 = clu2T2;
                    //Search the number of spikes of the 2 clusters whithin "time"
                    clu1Spk1 = data.findSpikePosition(T1,spikesOfCluster1);
                    clu1Spk2 = data.findSpikePosition(T2,spikesOfCluster1);
                    clu2Spk1 = 1;
                    clu2Spk2 = cluster2NbSpikes;
                }
            }
            else{
                T1 = clu1T1;
                if(clu1T2 < clu2T2){
                    T2 = clu1T2;
                    //Search the number of spikes of the 2 clusters whithin "time"
                    clu1Spk1 = 1;
                    clu1Spk2 = cluster1NbSpikesPlusOne - 1;
                    clu2Spk1 = data.findSpikePosition(T1,spikesOfCluster2);
                    clu2Spk2 = data.findSpikePosition(T2,spikesOfCluster2);
                }
                else{
                    T2 = clu2T2;
                    //Search the number of spikes of the 2 clusters whithin "time"
                    clu1Spk1 = 1;
                    clu1Spk2 = data.findSpikePosition(T2,spikesOfCluster1);
                    clu2Spk1 = data.findSpikePosition(T1,spikesOfCluster2);
                    clu2Spk2 = cluster2NbSpikes;
                }
            }
            time = T2 - T1;
            if(time == 0) asymptote = 0;
            else asymptote = static_cast<float>(
                        static_cast<double>(clu1Spk2 - clu1Spk1 + 1) * static_cast<double>(clu2Spk2 - clu2Spk1 + 1) *
                        static_cast<double>(static_cast<double>(binSizeInRU) / static_cast<double>(time))
                        );
        }
    }
}

void Data::cleanCorrelation(dataType clusterId,QList<dataType> currentClusterList,bool cleanProcess){
    mutex.lock();
    if(cleanProcess) correlationsInProcess.removeCluster(clusterId);

    //Remove the autocorrelogram separatly as the clusterID has already been removed from
    //the list of clusters.
    delete correlationDict.take(Pair(static_cast<int>(clusterId),static_cast<int>(clusterId)).toString());

    //Gets all the clustersId currently available

    //Remove all the correlations link to clusterId
    QList<dataType>::iterator iterator;
    QList<dataType>::iterator end(currentClusterList.end());
    for(iterator = currentClusterList.begin(); iterator != end; ++iterator){
        //Search pairs as (clusterId,*iterator) where clusterId > *iterator
        //and (*iterator,clusterId) where *iterator > clusterId
        if(*iterator <= clusterId) delete correlationDict.take(Pair(static_cast<int>(*iterator),static_cast<int>(clusterId)).toString());
        else delete correlationDict.take(Pair(static_cast<int>(clusterId),static_cast<int>(*iterator)).toString());
    }
    mutex.unlock();
}

void Data::renumberCorrelation(QMap<int,int>& clusterIdsOldNew){
    //Get all the old cluster ids
    QList<int> oldClusterIds = clusterIdsOldNew.keys();

    QList<int>::iterator iterator;
    mutex.lock();
    int i = 0;
    for(iterator = oldClusterIds.begin(); iterator != oldClusterIds.end(); ++iterator){
        if(correlationsInProcess.contains(*iterator)){
            correlationsInProcess.setClusterModified(*iterator,true);
            continue;
        }
        for(int j = i; j<oldClusterIds.count();j++) {
            int val = oldClusterIds.at(i);
            int val2 = oldClusterIds.at(j);
            if(val2 <= val){
                QHash<QString, Correlation*>* dict = correlationDict.take(Pair(val2,val).toString());
                if(dict != 0)
                    correlationDict.insert(Pair(clusterIdsOldNew[val2],clusterIdsOldNew[val]).toString(),dict);
            }
            else{
                QHash<QString, Correlation*>* dict = correlationDict.take(Pair(val,val2).toString());
                if(dict != 0)
                    correlationDict.insert(Pair(clusterIdsOldNew[val],clusterIdsOldNew[val2]).toString(),dict);
            }

        }
        ++i;
    }
    mutex.unlock();
}

long Data::findSpikePosition(double time,SortableTable& spikesOfCluster){
    dataType clusterNbSpikes = spikesOfCluster.nbOfColumns();
    double currentTime = spikeTime(spikesOfCluster,clusterNbSpikes);
    if(currentTime == time) return clusterNbSpikes;


    int largeStep = 400;
    int smallStep = 20;
    int step = largeStep;
    long i = step + 1;

    for(; i <= clusterNbSpikes; i += step){
        double currentTime = spikeTime(spikesOfCluster,i);
        if(currentTime < time) continue;
        else{
            if(step != 1){
                i -= step;
                if(step == largeStep) step = smallStep;
                else step = 1;
            }
            else{
                return i;
            }
        }
    }

    //Process the last spikes separately
    if(step == largeStep){
        i -= step;
        step = smallStep;
        for(; i <= clusterNbSpikes; i += step){
            double currentTime = spikeTime(spikesOfCluster,i);
            if(currentTime < time) continue;
            else{
                if(step != 1){
                    i -= step;
                    step = 1;
                }
                else{
                    return i;
                }
            }
        }
    }
    //loop with a step of 1
    i -= step;
    for(;i <= clusterNbSpikes; ++i){
        double currentTime = spikeTime(spikesOfCluster,i);
        if(currentTime < time) continue;
        else{
            return i;
        }
    }

    return i;
}

void Data::duplicate(SortableTable* & spikesOfClusterTemp,ClusterInfoMap* & clusterInfoMapTemp){
    //The mutex protect spikesByCluster and clusterInfoMap so that only one thread can
    //access them at the time.
    mutex.lock();
    spikesOfClusterTemp = new SortableTable(*spikesByCluster);
    clusterInfoMapTemp = new ClusterInfoMap(*clusterInfoMap);
    mutex.unlock();
}

void Data::createFeatureFile(QList<int>& clustersToRecluster,QFile& fetFile){
    dataType reclusteringNbSpikes = 0;
    //Loop on the selected clusters to calculate the total number of spikes
    QList<int>::iterator iterator;
    for(iterator = clustersToRecluster.begin(); iterator != clustersToRecluster.end(); ++iterator ){
        ClusterInfo clusterInfo = (*clusterInfoMap)[static_cast<dataType>(*iterator)];
        reclusteringNbSpikes += clusterInfo.nbSpikes();
    }
    reclusteringSpikesByCluster.setSize(reclusteringNbSpikes);//erase any previous data.

    //Loop on the selected clusters
    dataType upperInsertionIndex = 1;
    for(iterator = clustersToRecluster.begin(); iterator != clustersToRecluster.end(); ++iterator ){
        ClusterInfo clusterInfo = (*clusterInfoMap)[static_cast<dataType>(*iterator)];
        dataType firstSpikePosition = clusterInfo.firstSpikePosition();
        dataType nbSpikesOfCluster = clusterInfo.nbSpikes();
        //copy the 2 rows of spikesByCluster for the given cluster
        memcpy(&(reclusteringSpikesByCluster)(1,upperInsertionIndex),
               &(*spikesByCluster)(1,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));
        memcpy(&(reclusteringSpikesByCluster)(2,upperInsertionIndex),
               &(*spikesByCluster)(2,firstSpikePosition),
               nbSpikesOfCluster * sizeof(dataType));
        upperInsertionIndex += nbSpikesOfCluster;
    }

    //Write all the features to file
    QTextStream fetStream(&fetFile);
    fetStream <<nbDimensions<< endl;
    //loop on all the spikes
    for(dataType i = 1; i <= reclusteringNbSpikes;++i){
        dataType featuresRowIndex = reclusteringSpikesByCluster(1,i);
        //loop on the features of the current spike
        for(int j = 1; j < nbDimensions;++j) fetStream << features(featuresRowIndex,j)<<" ";
        fetStream << features(featuresRowIndex,nbDimensions)<<endl;
    }
}

bool Data::integrateReclusteredClusters(QList<int>& clustersToRecluster,QList<int>& reclusteredClusterList, QFile& clusterFile){
    //Replace the cluster ids in reclusteringSpikesByCluster by the new ones.
    if(!loadReclusteredClusters(clusterFile)) return 0;

    //The new information about the cluster will be inserted in the table pointed by spikesByClusterTemp
    SortableTable* spikesByClusterTemp = new SortableTable();
    spikesByClusterTemp->setSize(nbSpikes);

    //Create a new map which will contain the new information about the position of the clusters
    ClusterInfoMap* clusterInfoMapTemp = new ClusterInfoMap();

    //Iteration on the clusters to copy the unchanged clusters.
    ClusterInfoMap::Iterator infoMapIterator;
    dataType upperInsertionIndex = 1;

    //NB: the iterator iterates on the items sorted by their key
    for(infoMapIterator = clusterInfoMap->begin(); infoMapIterator != clusterInfoMap->end(); ++infoMapIterator){
        dataType firstSpikePosition = infoMapIterator.value().firstSpikePosition();
        dataType nbSpikesOfCluster = infoMapIterator.value().nbSpikes();
        dataType clusterId = infoMapIterator.key();

        //if clustersToRecluster does not contains the current cluster, this cluster is let unchanged
        //and its information is simply copy as is from spikesByCluster to spikesByClusterTemp
        if(!clustersToRecluster.contains(static_cast<int>(clusterId))){
            //copy the 2 rows of spikesByCluster for the given cluster
            memcpy(&(*spikesByClusterTemp)(1,upperInsertionIndex),
                   &(*spikesByCluster)(1,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            memcpy(&(*spikesByClusterTemp)(2,upperInsertionIndex),
                   &(*spikesByCluster)(2,firstSpikePosition),
                   nbSpikesOfCluster * sizeof(dataType));
            //Construct the new clusterInfoMap
            clusterInfoMapTemp->insert(clusterId,ClusterInfo(upperInsertionIndex,nbSpikesOfCluster));
            upperInsertionIndex += nbSpikesOfCluster;
        }
    }

    //Sort by time the spikes of the reclustered clusters.
    reclusteringSpikesByCluster.sort(1);

    dataType reclusteringNbSpikes = reclusteringSpikesByCluster.nbOfColumns();

    QMap<dataType,dataType> clusters;
    dataType max = reclusteringNbSpikes + 1;
    //Count the number of spikes for each cluster.
    for(dataType i = 1; i < max; ++i){
        dataType clusterId = reclusteringSpikesByCluster(2,i);
        clusters[clusterId]++;
    }

    //Initialize positions, for each clusterId the value is set to the position of the first spike.
    //The clusters will sorted by clusterId in spikesByClusterTemp. Initialize clusterInfoMapTemp.
    QMap<dataType,dataType> positions;
    QMap<dataType,dataType>::Iterator clusterIterator;
    int index = upperInsertionIndex;//The first new cluster will start after all the clusters which were not reclustered.
    for(clusterIterator = clusters.begin(); clusterIterator != clusters.end(); ++clusterIterator){
        dataType clusterId = clusterIterator.key();
        reclusteredClusterList.append(static_cast<int>(clusterId));
        positions[clusterId] =  index;
        clusterInfoMapTemp->insert(clusterId,ClusterInfo(index,clusterIterator.value()));
        index += clusterIterator.value();
    }

    //Fill spikesByClusterTemp with the data of the reclustered clusters sorted by cluster and by time (<=> position in the fet file)
    for(dataType i = 1; i < max; ++i){
        dataType clusterId = reclusteringSpikesByCluster(2,i);
        dataType position = positions[clusterId];
        dataType positionInFet = reclusteringSpikesByCluster(1,i);
        (*spikesByClusterTemp)(1,position) = positionInFet;
        (*spikesByClusterTemp)(2,position) = clusterId;
        positions[clusterId] ++;
    }

    //clear reclusteringSpikesByCluster
    reclusteringSpikesByCluster.setSize(0,true);

    //Get the list of clusters before applying the changes, this will be used in the clean
    //of the correlation.
    QList<dataType> currentClusterList = clusterIds();

    //Deal with the undo mechanism
    prepareUndo(spikesByClusterTemp,clusterInfoMapTemp);

    //If the cluster 0 have been recluster (very unlikely), the max and min
    // dimensions have to be recalculated. If minMaxThread is running, the call
    //will wait until it finishes before starting the thread again.
    if(clustersToRecluster.contains(0)){
        //If the minMaxThread has not finish, wait until it is done
        while(!minMaxThread->wait()){};
        //Reset the flag to false so the minMaxThread can do the computation
        clusterZeroJustModified = false;
        minMaxThread->setModifiedClusters(clustersToRecluster);
        minMaxThread->start();
        dimensionChangedUndo.prepend(true);
    }
    else dimensionChangedUndo.prepend(false);

    //Remove the waveform and correlation data for the reclustered clusters.
    //If there is not a thread working with them,otherwise advice the thread of the change,by updating waveformStatus and correlationsInProcess
    // and the thread will remove it.
    QList<int>::iterator iterator;
    for(iterator = clustersToRecluster.begin(); iterator != clustersToRecluster.end(); ++iterator){
        mutex.lock();
        if(waveformStatusMap.contains(*iterator)){
            if(!waveformStatusMap[*iterator].isInProcess()){
                delete waveformDict.take(QString::fromLatin1("%1").arg(*iterator));
                waveformStatusMap.remove(*iterator);
            }
            else{
                WaveformStatus waveformStatus = waveformStatusMap[*iterator];
                WaveformStatus waveformStatusCopy = WaveformStatus(waveformStatus);
                waveformStatusCopy.setClusterModified(true);
                waveformStatusMap.insert(*iterator,waveformStatusCopy);
            }
        }
        mutex.unlock();
        if(!correlationsInProcess.contains(static_cast<dataType>(*iterator))) cleanCorrelation(static_cast<dataType>(*iterator),currentClusterList);
        else{
            mutex.lock();
            correlationsInProcess.setClusterModified(static_cast<dataType>(*iterator),true);
            mutex.unlock();
        }
    }

    return 1;
}


bool Data::loadReclusteredClusters(QFile &clusterFile){
    bool firstLine = true;

    dataType highestClusterId = (*spikesByCluster)(2,nbSpikes);
    dataType k = 1;
    while (!clusterFile.atEnd()) {
        QByteArray line = clusterFile.readLine();
        //qDebug()<<" line "<<line;
        if (firstLine) {
            firstLine = false;
        } else {
            QByteArray feature;
            for (int i = 0; i <line.count();++i) {
                if ((line.at(i) >= '0' && line.at(i) <='9')) {
                    feature.append(line.at(i));
                } else if (!feature.isEmpty()){
                    reclusteringSpikesByCluster(2,k++) = feature.toLongLong() + highestClusterId;//Warning if the typedef dataType changes, change will have to be make here.
                    feature = "";
                }
            }
        }
    }

    //if the number of clusters read did not correspond to the number of spikes reclustered, there is a problem.
    if(k != (reclusteringSpikesByCluster.nbOfColumns() + 1))
        return 0;
    else
        return 1;
}

void Data::getClusterUserInformation (int pGroup,QMap<int,ClusterUserInformation>& clusterUserInformationMap)const{
    //Iteration on the clusters
    ClusterInfoMap::Iterator iterator;

    //NB: the iterator iterates on the items sorted by their key
    for(iterator = clusterInfoMap->begin(); iterator != clusterInfoMap->end(); ++iterator) {
        int clusterId = static_cast<int>(iterator.key());

        if(clusterId == 0 || clusterId == 1) continue;

        ClusterUserInformation currentClusterUserInformation = ClusterUserInformation(pGroup,clusterId,iterator.value().getStructure(),iterator.value().getType(),iterator.value().getId(),iterator.value().getQuality(),iterator.value().getNotes());

        clusterUserInformationMap.insert(clusterId,currentClusterUserInformation);
    }
}

