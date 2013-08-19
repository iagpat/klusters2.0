/***************************************************************************
                          data.h  -  description
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

#ifndef DATA_H
#define DATA_H

//Include files of the application
#include "array.h"
#include "sortabletable.h"
#include "pair.h"
#include "types.h"
#include "clusteruserinformation.h"

//Include files for QT
#include <QList>
#include <QHash>
#include <qregion.h>
#include <qmap.h>
#include <qfile.h>
#include <qmutex.h>
#include <qthread.h>


#include <stdexcept>
#include <math.h>
#include <vector>
using namespace std;

// forward declaration
class MinMaxThread;
class WaveformThread;
class CorrelationThread;


class Data;

/**
  * This class contains and manages the data.
  *@author Lynn Hazan
  */

class Data {


public:
    friend class MinMaxThread;
    friend class WaveformThread;
    friend class CorrelationThread;
    friend class AutoSaveThread;
    friend class GroupingAssistant;
    friend class ClustersProvider;

    Data();
    ~Data();

    /**
  * Loads the features in data.
  * @param featureFile the .fet file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the loading succed false otherwise
  */
    bool loadFeatures(QFile &featureFile, QString& errorInformation);

    /**
  * Loads the clusters in spikesByCluster.
  * @param clusterFile the .clu.i file.
  * @param spkFileLength the length of the .spk.i file.
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the loading succed false otherwise.
  */
    bool loadClusters(QFile &clusterFile, long spkFileLength, QString& errorInformation);

    /**
  * Loads the configuration parameters.
  * @param parXFile the .par.i file
  * @param parFile the .par file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the loading succed false otherwise.
  */
    bool configure(QFile& parXFile,QFile& parFile,QString& errorInformation);

    /**
  * Loads the configuration parameters from the xml format parameter file.
  * @param parFile the .xml parameter file.
  * @param electrodeGroupID the id of the electrode group currently opened.
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the loading succed false otherwise.
  */
    bool configure(QFile& parFile,int electrodeGroupID,QString& errorInformation);

    /**
  * Load the features data, cluster data and configuration data when the cluster file does not exist.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param spkFileLength the length of the .spk.i file
  * @param spkFileName the name of the .spk.i file
  * @param parXFile the .par.i file
  * @param parFile the .par file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile& featureFile, long spkFileLength, const QString &spkFileName, QFile& parXFile, QFile& parFile, QString& errorInformation);

    /**
  * Load the features data, cluster data and configuration data.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param clusterFile the .clu.i file
  * @param spkFileLength the length of the .spk.i file
  * @param spkFileName the name of the .spk.i file
  * @param parXFile the .par.i file
  * @param parFile the .par file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile& featureFile, QFile& clusterFile, long spkFileLength, const QString &spkFileName, QFile& parXFile, QFile& parFile, QString& errorInformation);

    /**
  * Load the features data, cluster data and configuration data when the cluster file does not exist.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param spkFileLength the length of the .spk.i file
  * @param spkFileName the name of the .spk.i file
  * @param parFile the .xml parameter file
  * @param electrodeGroupID the id of the electrode group currently opened.
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile& featureFile, long spkFileLength, const QString &spkFileName, QFile& parFile, int electrodeGroupID, QString& errorInformation);

    /**
  * Load the features data, cluster data and configuration data.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param clusterFile the .clu.i file
  * @param spkFileLength the length of the .spk.i file
  * @param spkFileName the name of the .spk.i file
  * @param parFile the .xml parameter file
  * @param electrodeGroupID the id of the electrode group currently opened.
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile& featureFile,QFile& clusterFile, long spkFileLength, const QString &spkFileName, QFile& parFile, int electrodeGroupID, QString& errorInformation);

    /**Calculate the minimum and maximum for each dimension and store them in
  *dimensionMinima and dimensionMaxima respectively.
  * @param modifiedClusters list of the clusters which have been modified implying
  * the modification of the cluster 0 this the recalculation of the minima and maxima.
  */
    void minMaxDimensionCalculation(QList<int> modifiedClusters);

    /**
  * Creates a new cluster out of existing ones.
  * @param region the polygon defining the area containing the spikes for the new cluster.
  * @param clustersOfOrigin a list of the cluster numbers identifying the clusters which
  * may contain spikes in the region
  * @param dimensionX the dimension used as absciss to display the clusters
  * @param dimensionY the dimension used as ordinate to display the clusters
  * @param fromClusters an empty list used as a return value, which will be filled
  * with the cluster numbers which really contained spikes in the region
  * @param emptyClusters an empty list used as a return value, which will be filled
  * with the cluster numbers which became empty because all their spikes were put in the new one.
  * @return the number of the newly created cluster or 0 if no cluster have been created (no spikes selected).
  * This is safe as cluster 0 (artifact) can never be created that way.
  */
    dataType createNewCluster(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY, QList <int>& fromClusters,QList <int>& emptyClusters);

    /**
  * Creates a new clusters out of existing ones. If the polygon of selection contains x clusters
  * x new clusters will be created.
  * @param region the polygon defining the area containing the spikes for the new cluster.
  * @param clustersOfOrigin a list of the cluster numbers identifying the clusters which
  * may contain spikes in the region
  * @param dimensionX the dimension used as absciss to display the clusters
  * @param dimensionY the dimension used as ordinate to display the clusters
  * @param emptyClusters an empty list used as a return value, which will be filled
  * with the cluster numbers which became empty because all their spikes were put in the new one.
  * @return a map where the keys are ids of the clusters which really contained spikes in the region
  * and the values are the ids of the newly created clusters.
  */
    QMap<int,int> createNewClusters(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY,QList <int>& emptyClusters);

    /**
  * Removes spikes from some clusters and assign them to the cluster @p destinationCluster
  * which is either the cluster 0, corresponding to the artifact, or the cluster 1, corresponding to the noise.
  * @param region the polygon defining the area containing the spikes corresponding to the noise.
  * @param clustersOfOrigin a list of the cluster numbers (in ascending order) identifying the clusters which
  * may contain spikes in the region
  * @param destinationCluster the cluster number to assign the spikes contained in the region
  * @param fromClusters an empty list used as a return value, which will be filled
  * with the cluster numbers which really contained spikes in the region
  * @param dimensionX the dimension used as absciss to display the clusters
  * @param dimensionY the dimension used as ordinate to display the clusters
  * @param emptyClusters an empty list used as a return value, which will be filled
  * with the cluster numbers which became empty because all their spikes were put in the new one.
  */
    void deleteSpikesFromClusters(QRegion& region, const QList <int>& clustersOfOrigin, int destinationCluster, int dimensionX, int dimensionY, QList <int>& fromClusters,QList <int>& emptyClusters);

    /**
  * Deletes the clusters contained in @p clustersToDelete. The correponding spikes are assign to cluster 1 (the noise)
  * @param clustersToDelete a list of the cluster numbers (in ascending order) identifying the clusters to delete.
  */
    void moveClustersToNoise(QList <int>& clustersToDelete);

    /**
  * Deletes the clusters contained in @p clustersToDelete. The correponding spikes are assign to cluster 0 (the artifact)
  * @param clustersToDelete a list of the cluster numbers (in ascending order) identifying the clusters to delete.
  */
    void moveClustersToArtefact(QList <int>& clustersToDelete);

    /**
  * Groups the clusters contained in @p clustersToGroup. The correponding spikes are assign to a new cluster.
  * The user information of the different clusters to be grouped will be concatenated.
  * @param clustersToGroup a list of the cluster numbers (in ascending order) identifying the clusters to group
  * @return the number of the newly created cluster.
  */
    dataType groupClusters(QList <int>& clustersToGroup);

    /**Returns the number of dimensions of the data.*/
    int nbOfDimensions(){return nbDimensions;}

    /** Reverts the last user action.
  * @param addedClusters list of clusters which were added (can be empty).
  * @param updatedClusters list of clusters which were modified (can be empty).
  */
    void undo(QList<int>& addedClusters,QList<int>& updatedClusters);

    /** Reverts the last undo action
  * @param addedClusters list of clusters which were added (can be empty).
  * @param updatedClusters list of clusters which were modified (can be empty).
  * @param deletedClusters list of clusters which were deleted (can be empty).
  */
    void redo(QList<int>& addedClusters,QList<int>& updatedClusters,QList<int>& deletedClusters);

    /**Renumbers the clusters, so the the clusterIds will be consecutive.
  * @param clusterIdsOldNew map between old and new cluster ids.
  * @param clusterIdsNewOld map between new and old cluster ids.
  */
    void renumber(QMap<int,int>& clusterIdsOldNew,QMap<int,int>& clusterIdsNewOld);

    /**Makes all the internal changes due to a modification of the number of undo.
  * @param newNbUndo the futur new number of undo.
  */
    void nbUndoChangedCleaning(int newNbUndo);

    class Iterator;
    friend class Iterator;

    /**
  * Creates and returns an iterator on the spikes of the given cluster.
  * @param clusterId the number of the cluster on which spikes the iterator will iterates.
  * @return the iterator on the spikes of the given cluster.
  */
    Iterator iterator(dataType clusterId){
        return Iterator(clusterId, *this);
    }

    /**
  * Specialized iterator which iterate on the features contained in features for
  * the cluster specified in the constructor.
  */

    class Iterator{
        //Only the method iterator of data has access to the private part of Iterator,
        //the constructor of Iterator being private, only this method con create a new Iterator
        friend Iterator Data::iterator(dataType clusterId);

    public:
        ~Iterator(){}
        /**
    * Returns a QPoint for the given dimensions for the current spike for the cluster on which this class iterates
    * Caution: in Qt graphical coordinate system, the Y axis is inverted (increasing downwards),
    * thus a point (x,y) as to be drawn and tested as (x,-y).
    * @param dimensionX the feature used as the absciss
    * @param dimensionY the feature used as the ordinate
    * @return a QPoint for the couple (dimensionX,dimensionY) taking the Qt graphical
    * coordinate system into consideration, the ordinate coordinate is the opposite of the raw data.
    */
        QPoint operator()(dataType dimensionX, dataType dimensionY) const{
            dataType featuresRowIndex = (*data.spikesByCluster)(1,index);
            return QPoint(data.features(featuresRowIndex,dimensionX),
                          - data.features(featuresRowIndex,dimensionY));
        }
        /**
    * Returns the specified feature for the current spike for the cluster on which this class iterates.
    * @param dimension the feature requested
    * @return the value of the feature.
    */
        dataType operator()(dataType dimension) const{
            return data.features((*data.spikesByCluster)(1,index),dimension);
        }
        /**Increments the iterator.*/
        void next(){index++;}
        /**Check if there is more spikes*/
        bool hasNext(){return (lastIndex >= index);}

    private:
        Iterator(dataType clusterId, const Data& d):data(d),clusterId(clusterId){
            ClusterInfo clusterInfo = (*data.clusterInfoMap)[clusterId];
            index = clusterInfo.firstSpikePosition();
            lastIndex = index + clusterInfo.nbSpikes() - 1;
        }
        /**Returns true if the iterator has reach the last spike for the cluster on which it iterates,
      * false otherwise.
      */
        const Data& data;
        dataType clusterId;
        dataType index;
        dataType lastIndex;
    };

    /** Returns the list of cluster Ids.*/
    QList<dataType> clusterIds(){
        return clusterInfoMap->keys();
    }

    /**Returns the maximum for the dimension
  * @param dimension for which the maximum is requested. Numbering starts at 1
  * @return maximum of the dimension
  */
    dataType maxDimension(int dimension){return dimensionMaxima(dimension,1);}

    /**Returns the minimum for the dimension
  * @param dimension for which the minimum is requested. Numbering starts at 1
  * @return minimum of the dimension
  */
    dataType minDimension(int dimension){return dimensionMinima(dimension,1);}

    /**Saves the clusters information to file
  * @param clusterFile the .clu.i file
  * @return true if the data have been successfully saved to file, false otherwise.
  */
    bool saveClusters(FILE* clusterFile);

    /**Returns the number of points used to describe a waveform. Each point
  correspond to a diffrent instant in time.*/
    int nbOfSampleInWaveform()const{return nbSamplesInWaveform;}

    /**Returns the position of the peak among the points decribing the waveform.*/
    int positionOfPeakInWaveform()const{return peakPositionInWaveform;}

    /**Returns the number of channels used.*/
    int nbOfchannels()const{return nbChannels;}

    /**Returns the total number of PCAs used
  * (number of channels times number of PCA by channel).*/
    int totalNbOfPCAs()const{return (nbChannels*nbFeaturesbyChannel);}

    /**Returns the sampling interval (time between two samples) in second.*/
    double intervalOfSampling()const{return samplingInterval;}

    /**Returns the dimension for the time.*/
    int timeDimension()const{return nbDimensions;}
    /**Returns the maximum value for the time dimension in seconds.*/
    long maxTime(){
        double maximumTimeInRecordingUnits = static_cast<double>(maxDimension(nbDimensions));
        //the cast takes the non floating part, to include the last record we add 1.
        double maxTimeInS = static_cast<double>(maximumTimeInRecordingUnits * samplingInterval) / static_cast<double>(1000000);

        return static_cast<long>(floor(0.5 + maxTimeInS));
    }


    /**Gives information on how the data were recorded. True if the data where recording using a 12 or 16 bits recording system which
  * gives data coded on 2 bytes, false otherwise, (the recording is then assume to be 32 bits
  * and then the data are coded on 4 bytes).*/
    bool isRecordingTwoBytes(){return isTwoBytesRecording;}

    /**Gets a onr row SortableTable with the spike positions for the cluster @p clusterId.
  * @param clusterId d of the cluster for which the spike position are search.
  * @param subsetTable the array where the result of the search will be store.
  * @return true if the cluster exist and the data have been retreive, false otherwise.
  */
    bool spikePositions(int clusterId,SortableTable& subsetTable);

    /**Returns the number of points corresponding to a spike. This equals to:
  * nbChannels * nbSamplesInWaveform
  */
    int nbPtsBySpike(){return nbChannels * nbSamplesInWaveform;}

    /**Returns the number of spikes of the cluster
  * @param clusterId id of the cluster for which the number of spikes is requested.
  * @return the number of spikes of the cluster @p clusterId.
  */
    dataType nbOfSpikes(dataType clusterId){
        ClusterInfo currentClusterInfo = (*clusterInfoMap)[clusterId];
        return currentClusterInfo.nbSpikes();
    }

    /**Returns the total number of spikes.
  * @return the total number of spikes.
  */
    dataType totalNbOfSpikes() const{return nbSpikes;}

    /**
  * String indicating in scale mode the user is using (raw, scale by the maximum,
  * scale by the shoulder) in the correlationView.
  */
    enum ScaleMode{RAW=1,MAX=2,SHOULDER=3};

    /**Returns the current number of clusters.
  * @return the number of clusters.
  */
    int nbOfClusters(){
        mutex.lock();
        int nbClusters = clusterInfoMap->count();
        mutex.unlock();

        return nbClusters;
    }

    /**Assignes to the cluster the information given by the user.
  * @param clusterId id of the cluster for which the information are given.
  * @param structure structure in which the cluster is supposed to be.
  * @param type type of the cluster.
  * @param ID Isolation distance of the cluster.
  * @param quality quality of the cluster.
  * @param notes notes of any type on the cluster.
  */
    void setUserClusterInformation(int clusterId, QString structure,
                                          QString	type,QString ID, QString	quality, QString notes){
        if((*clusterInfoMap).contains(static_cast<dataType>(clusterId))){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[static_cast<dataType>(clusterId)];

            currentClusterInfo.setStructure(structure);
            currentClusterInfo.setType(type);
            currentClusterInfo.setId(ID);
            currentClusterInfo.setQuality(quality);
            currentClusterInfo.setNotes(notes);

            clusterInfoMap->insert(static_cast<dataType>(clusterId),currentClusterInfo);
        }
    }

    /**Gest the cluster the information given by the user.
    * @param clusterId id of the cluster for which the information are given.
    * @return array of QString containing the following information (in the following order) :
    * structure in which the cluster is supposed to be.
    * type of the cluster.
    * Isolation distance of the cluster.
    * quality of the cluster.
    * notes of any type on the cluster.
    */
    void getUserClusterInformation(int clusterId,QList<QString>& clusterInformation){

        if((*clusterInfoMap).contains(static_cast<dataType>(clusterId))){
            ClusterInfo currentClusterInfo = (*clusterInfoMap)[static_cast<dataType>(clusterId)];

            clusterInformation.append(currentClusterInfo.getStructure());
            clusterInformation.append(currentClusterInfo.getType());
            clusterInformation.append(currentClusterInfo.getId());
            clusterInformation.append(currentClusterInfo.getQuality());
            clusterInformation.append(currentClusterInfo.getNotes());
        }
    }

    /** Gets the map of cluster user information.
     * @param pGroup the current electrod group id.
     * @param clusterUserInformationMap map given the cluster user information, the key is the cluster id and the value an instance of ClusterUserInformation.
  */
    void getClusterUserInformation (int pGroup,QMap<int,ClusterUserInformation>& clusterUserInformationMap)const;

    /**Creates the feature file to automatically recluster the clusters contained in @p clustersToRecluster.
  * @param clustersToRecluster list of clusters to recluster.
  * @param fetFile file to which the data will be saved.
  */
    void createFeatureFile(QList<int>& clustersToRecluster,QFile& fetFile);

    /**Integrates the clusters obtained by automatic reclustering.
  * Suppress the reclustered ones and add the newly created ones.
  * @param clustersToRecluster list of clusters reclustered.
  * @param reclusteredClusterList output parameter, the list of the newly created clusters.
  * The list will be empty if the integration is not successful.
  * @param clusterFile cluster file created by the automatic reclustering program.
  * @return true if the integration is successful, false otherwise.
  */
    bool integrateReclusteredClusters(QList<int>& clustersToRecluster, QList<int>& reclusteredClusterList, QFile &clusterFile);

    /**
  * Informs if the the variables need it by the traceView are available. Those variables are retrieve only from
  * the parameter file in xml format (the new for
  * @return true if the variables are available, false otherwise.*/
    bool isTraceViewVariablesAvailable()const {return traceViewVariablesAvailable;}

    /**
  *Returns the acquisition system resolution.
  */
    int getResolution()const{return nbBits;}
    /**Returns the total number of channels used during the recording.*/
    int getTotalNbChannels()const{return totalNbChannels;}
    /**Returns the sampling rate in microseconds.*/
    double getSamplingRate()const{return samplingRate;}
    /**Returns the acquisition system voltage range.*/
    int getVoltageRange()const{return voltageRange;}
    /**Returns the acquisition system offset.*/
    int getOffset()const{return initialOffset;}
    /**Returns the acquisition system amplification.*/
    int getAmplification()const{return amplification;}
    /**Returns the number of samples used to describe a waveform.*/
    int getNbSamplesInWaveform()const{return nbSamplesInWaveform;}
    /**Returns the sample index of the peak.*/
    int getPeakPositionInWaveform()const{return peakPositionInWaveform;}
    /**Returns the list of channels of the current electrode.*/
    QList<int>& getCurrentChannels(){return currentChannels;}

private:

    /**
  * String indicating what is the status of the processing of the waveform information.
  */
    enum Status{NOT_AVAILABLE=1,IN_PROCESS=2,READY=3};

    MinMaxThread* minMaxThread;

    int nbChannels;
    int nbSamplesInWaveform;
    int peakPositionInWaveform;
    QList<int> channelIds;
    int nbRefactorySample;
    int RMSIntWindowLenght;
    float firingRate;
    int nbSampleBeforePeak;
    int nbSampleAfterPeak;
    int windowLenghtToRealign;
    int peakPositionToRealign;
    int nbFeaturesbyChannel;
    int nbSamplesByPCA;
    float HighPassFilterFreq;
    int nbDimensions;
    long nbSpikes;
    int lowPassFilterFreq;
    /**Sampling rate (time between two samples) in micro seconds.*/
    double samplingInterval;
    double samplingRate;
    int nbTotalElectrodes;
    int nbBits;
    QString spkFileName;
    int voltageRange;
    int amplification;
    int initialOffset;
    int totalNbChannels;
    bool traceViewVariablesAvailable;
    QList<int> currentChannels;

    /**
  * A array which contains the coefficients  to apply the spike samples in
  * order to upsample the waveform.
  * the principal componants
  * the Valley to peak amplitude
  * the peak to valley amplitude
  * the max of the to previous data
  * the width of the spike
  * the time of the spike
  */
    // Array<dataType> coeff;

    /**
  * A array which contains the features for each spike:
  * the principal componants
  * the Valley to peak amplitude
  * the peak to valley amplitude
  * the max of the to previous data
  * the width of the spike
  * the time of the spike
  */

    Array<dataType> features;
    /**
  * A two line array which contains sorted by cluster numbers and then by time:
  * the row index of the spike in features
  * the id of the cluster
  */
    SortableTable* spikesByCluster;

    /**Represents a list of clusterInfoMap
  * use to enable undo action.
  */
    QList<SortableTable*> spikesByClusterUndoList;

    /**Represents a list of clusterInfoMap
  * use to enable redo action.
  */
    QList<SortableTable*> spikesByClusterRedoList;

    /**
  * Represents information on a cluster:
  * the index of the first spike of a given cluster number in spikesByCluster
  * the number of spikes of the cluster
  * and optionnally information added by the user:
  * the structure where the cluster is located
  * the type of the unit
  * the isolation distance
  * the quality
  * notes
  */
    class ClusterInfo {

    public:
        ClusterInfo(const QString& pStructure = QString(), const QString& pType = QString(),const QString& pID = QString(),const QString& pQuality = QString(),const QString& pNotes = QString())
            :structure(pStructure),type(pType),ID(pID),quality(pQuality),notes(pNotes){}
        ClusterInfo(dataType position, dataType nb,QString pStructure = QString(),QString pType = QString(),QString pID = QString(),QString pQuality = QString(),QString pNotes =QString())
            :position(position),spikeNb(nb),structure(pStructure),type(pType),ID(pID),quality(pQuality),notes(pNotes){}
        ~ClusterInfo(){}
         dataType firstSpikePosition() const {return position;}
         dataType nbSpikes() const {return spikeNb;}
         void setNbSpikes(dataType nbSpikes){spikeNb = nbSpikes;}
         void setFirstSpikePosition(dataType position){this->position = position;}

         QString getStructure() const { return structure; }
         QString getType() const { return type; }
         QString getId() const { return ID; }
         QString getQuality() const { return quality; }
         QString getNotes() const { return notes; }

         void setStructure(const QString& pStructure) { structure = pStructure; }
         void setType(const QString& pType) { type = pType; }
         void setId(const QString& pId) { ID = pId; }
         void setQuality(const QString& pQuality) { quality = pQuality; }
         void setNotes(const QString& pNotes) { notes = pNotes; }

    private:
        dataType position;
        dataType spikeNb;

        QString		structure;
        QString		type;
        /**Isolation Distance*/
        QString		ID;
        QString		quality;
        QString		notes;
    } ;

    typedef QMap<dataType,ClusterInfo> ClusterInfoMap;

    /**Contains ClusterInfo
  * key: cluster number
  * value: a ClusterInfo (which gives:
  * the index of the first spike in spikesByCluster and the number of spikes for a given cluster)
  */
    ClusterInfoMap* clusterInfoMap;

    /**Represents a list of clusterInfoMap
  * use to enable undo action.
  */
    QList<ClusterInfoMap*> clusterInfoMapUndoList;

    /**Represents a list of clusterInfoMap
  * use to enable redo action.
  */
    QList<ClusterInfoMap*> clusterInfoMapRedoList;

    /**List of the maximum of each dimension*/
    Array<dataType> dimensionMaxima;
    /**List of the minimum of each dimension*/
    Array<dataType> dimensionMinima;

    /**List of the clusters giving the maximum of each dimension (sorted by dimension)*/
    QList<int> clustersGivingMaximum;

    /**List of the clusters giving the minimum of each dimension (sorted by dimension)*/
    QList<int> clustersGivingMinimum;

    /**QT object providing access serialization between threads*/
    QMutex mutex;

    /**List containing the the dimension change status of each action of the undo list*/
    QList<bool> dimensionChangedUndo;

    /**List containing the the dimension change status of each action of the redo list*/
    QList<bool> dimensionChangedRedo;

    /**True is the data where recording using a 12 or 16 bits recording system which
  * gives data coded on 2 bytes, false otherwise (the recording is then assume to be 32 bits
  * and then the data are coded on 4 bytes.*/
    bool isTwoBytesRecording;

    /**
  * String indicating in which presentation mode the user is (sample, time frame).
  */
    enum WaveformMode{SAMPLE=1,TIME_FRAME=2};

    class WaveformStatus{
    public:
        WaveformStatus(Status sample = NOT_AVAILABLE,Status timeFrame = NOT_AVAILABLE,Status sampleMean = NOT_AVAILABLE,Status timeFrameMean = NOT_AVAILABLE )
            :sample(sample),timeFrame(timeFrame),sampleMean(sampleMean),timeFrameMean(timeFrameMean){
            clusterModified = false;
        }
        WaveformStatus(const WaveformStatus& s):sample(s.sample),timeFrame(s.timeFrame),sampleMean(s.sampleMean),timeFrameMean(s.timeFrameMean), clusterModified(s.clusterModified){}
        ~WaveformStatus(){}
        void setSampleStatus(Status status){sample = status;}
        Status sampleStatus() const {return sample;}
        void setTimeFrameStatus(Status status){timeFrame = status;}
        Status timeFrameStatus() const {return timeFrame;}
        void setSampleMeanStatus(Status status){sampleMean = status;}
        Status sampleMeanStatus() const {return sampleMean;}
        void setTimeFrameMeanStatus(Status status){timeFrameMean = status;}
        Status timeFrameMeanStatus() const {return timeFrameMean;}
        bool isInProcess() const {
            if(sample == IN_PROCESS || timeFrame == IN_PROCESS || sampleMean == IN_PROCESS || timeFrameMean == IN_PROCESS) return true;
            else return false;
        }
        void setClusterModified(bool modified){clusterModified = modified;}
        bool isClusterModified()  const {return clusterModified;}
    private:
        Status sample;
        Status timeFrame;
        Status sampleMean;
        Status timeFrameMean;
        bool clusterModified;
    };

    class Waveforms;
    friend class Waveforms;

    /**
  * Represents waveform data for a cluster:
  * This class is the purely virtual parent.
  */
    class Waveforms{

    public:
        virtual ~Waveforms(){}
        dataType indexOfTimeEnd() const {return timeEndIndex;}
        void setIndexOfTimeEnd(dataType index){timeEndIndex = index;}
        dataType startTime() const {return timeStart;}
        void setStartTime(dataType time){timeStart = time;}
        dataType endTime() const {return timeEnd;}
        void setEndTime(dataType time){timeEnd = time;}
        dataType nbOfSpikes(WaveformMode waveformMode){
            mode = waveformMode;
            if(waveformMode == SAMPLE) return nbSampleSpikes;
            else return nbTimeFrameSpikes;
        }
        dataType nbOfSpikesAsked() const {return nbSpikesAsked;}
        void setNbOfSpikesAsked(dataType nb) {nbSpikesAsked = nb;}
        void setMode(WaveformMode waveformMode){mode = waveformMode;}

        virtual void setSize(dataType size,WaveformMode waveformMode) = 0;
        virtual dataType getSample(dataType index) const = 0;
        virtual dataType getTimeFrame(dataType index) const  = 0;
        virtual dataType getSampleMean(dataType index) const  = 0;
        virtual dataType getTimeFrameMean(dataType index) const  = 0;
        virtual dataType getSampleStDeviation(dataType index) const  = 0;
        virtual dataType getTimeFrameStDeviation(dataType index) const  = 0;
        virtual void read(SortableTable& positionOfSpikes,dataType nbSpikesOfCluster,FILE* spikeFile,dataType nbSpkToDisplay) = 0;
        virtual void read(SortableTable& positionOfSpikes,dataType nbSpikesOfCluster,FILE* spikeFile,dataType& currentSpikeIndex,dataType end) = 0;
        virtual void calculateMean(WaveformMode waveformMode) = 0;

    protected:
        Waveforms(Data& d,dataType nbSampleSpikes = 0,dataType nbTimeFrameSpikes = 0,dataType index = 0,dataType startTime = 0,dataType endTime = 0):data(d){
            timeEndIndex = index;
            timeStart = startTime;
            timeEnd = endTime;
            nbPtsBySpike = data.nbChannels * data.nbSamplesInWaveform;
            nbSpikesAsked = 0;
        }

    protected:
        Data& data;
        dataType timeEndIndex;
        dataType timeStart;
        dataType timeEnd;
        dataType nbSampleSpikes;
        dataType nbTimeFrameSpikes;
        WaveformMode mode;
        int nbPtsBySpike;
        dataType nbSpikesAsked;
    } ;

    template <class T>
    class WaveformData;

    friend class WaveformData<class T>;

    /**
  * Represents waveform data for a cluster. Has the spike information format
  * relyes on the recording system (either a 12 or 16 bits recording system),
  * this class is a template to allow the internal use type to be a short or a long int.
  */
    template <class T>
    class WaveformData : public Waveforms {

    public:
        WaveformData(Data& d,dataType nbSampleSpikes = 0,dataType nbTimeFrameSpikes = 0,dataType index = 0,dataType startTime = 0,dataType endTime = 0):
            Waveforms(d,nbSampleSpikes,nbTimeFrameSpikes,index,startTime,endTime){
            sampleSpikesTable = 0L;
            timeFrameSpikesTable = 0L;
            sampleMeanTable = 0L;
            timeFrameMeanTable = 0L;
            sampleStDeviationTable = 0L;
            timeFrameStDeviationTable = 0L;
        }
        ~WaveformData(){
            if(sampleSpikesTable != 0L) delete []sampleSpikesTable;
            if(timeFrameSpikesTable != 0L) delete []timeFrameSpikesTable;
            if(sampleMeanTable != 0L) delete []sampleMeanTable;
            if(timeFrameMeanTable != 0L) delete []timeFrameMeanTable;
            if(sampleStDeviationTable != 0L) delete []sampleStDeviationTable;
            if(timeFrameStDeviationTable != 0L) delete []timeFrameStDeviationTable;
        }
        /**Specifies the number of spikes which can be store.*/
        void setSize(dataType size,WaveformMode waveformMode = SAMPLE);
        dataType getSample(dataType index) const {
            return static_cast<dataType>(sampleSpikesTable[index]);
        }
        dataType getTimeFrame(dataType index) const {
            return static_cast<dataType>(timeFrameSpikesTable[index]);
        }
        dataType getSampleMean(dataType index) const {
            return static_cast<dataType>(sampleMeanTable[index]);
        }
        dataType getTimeFrameMean(dataType index) const {
            return static_cast<dataType>(timeFrameMeanTable[index]);
        }
        dataType getSampleStDeviation(dataType index) const {
            return static_cast<dataType>(sampleStDeviationTable[index]);
        }
        dataType getTimeFrameStDeviation(dataType index) const {
            return static_cast<dataType>(timeFrameStDeviationTable[index]);
        }
        void read(SortableTable& positionOfSpikes,dataType currentSpikeIndex,FILE* spikeFile,dataType nbSpkToDisplay);
        void read(SortableTable& positionOfSpikes,dataType nbSpikesOfCluster,FILE* spikeFile,dataType& currentSpikeIndex,dataType end);
        void calculateMean(WaveformMode waveformMode = SAMPLE);
    private:
        T* sampleSpikesTable;
        T* timeFrameSpikesTable;
        T* sampleMeanTable;
        T* timeFrameMeanTable;
        T* sampleStDeviationTable;
        T* timeFrameStDeviationTable;
    } ;


    /**
  * Map containing the waveform status by cluster. Only the clusters
  * for which information have been asked are present in this map.
  */
    QMap<int,WaveformStatus> waveformStatusMap;

    /**
  * Dictionary containing the waveform data by cluster. Only the clusters
  * for which data have been asked are present in this dictionary.
  */
    QHash<QString, Waveforms*> waveformDict;

    /**Boolean use to inform the MinMaxThread that an undo or a redo is in process and that it has to stop.*/
    bool undoRedoInProcess;

    /**Boolean use to inform the MinMaxThread that the cluster 0 has changed and that it has to stop.*/
    bool clusterZeroJustModified;

    /**
  * This class stores the information to know which cluster has
  * correlations in process.
  */
    class CorrelationsInProcess{

    public:
        CorrelationsInProcess(){}
        ~CorrelationsInProcess(){}
        void addProcess(dataType clusterId){
            if(clusters.contains(clusterId)) clusters[clusterId]++;
            else {
                clusters.insert(clusterId,1);
                clustersModified.insert(clusterId,false);
            }
        }
        void removeProcess(dataType clusterId){
            if(clusters.contains(clusterId) && clusters[clusterId] > 1)clusters[clusterId]--;
            else if(clusters.contains(clusterId) && clusters[clusterId] == 1){
                clusters.remove(clusterId);
                clustersModified.remove(clusterId);
            }
        }
        void removeCluster(dataType clusterId){
            clusters.remove(clusterId);
            clustersModified.remove(clusterId);
        }
        bool contains(dataType clusterId) const {return clusters.contains(clusterId);}

        void setClusterModified(dataType clusterId,bool modified){clustersModified[clusterId] = modified;}
        bool isClusterModified(dataType clusterId)  const {return clustersModified[clusterId];}

    private:
        QMap<dataType,int> clusters;
        QMap<dataType,bool> clustersModified;
    } ;

    /**Stores the information to know which cluster has
 * correlations in process.*/
    CorrelationsInProcess correlationsInProcess;

    class Correlation;
    friend class Correlation;

    /**
  * Represents correlation data for a pair of clusters.
  */
    class Correlation{

    public:
        Correlation(Data& d):data(d){
            reset();
        }
        Correlation(Data& d,int size,int timeWindow):data(d),binSize(size),timeFrame(timeWindow){
            values = 0L;
            max = 0;
            asymptote = 0;
            nbBins = 0;
            firingRate = 0;
        }
        ~Correlation(){
            if(values != 0L) delete []values;
        }
        void reset(){
            if(values != 0L) delete []values;
            values = 0L;
            max = 0;
            asymptote = 0;
            binSize = 0;
            timeFrame = 0;
            nbBins = 0;
            firingRate = 0;
        }
        void setStatus(Status s){status = s;}
        Status getStatus() const {return status;}
        Status getStatus(int size,int timeWindow) const {
            if(binSize != size || timeFrame != timeWindow) return NOT_AVAILABLE;
            else return status;
        }
        /**Returns the size of a bin in miliseconds*/
        int getBinSize() const {return binSize;}
        void setBinSize(int size){binSize = size;}
        /**Returns the size of the time window in miliseconds*/
        int getTimeWindow() const {return timeFrame;}
        void setTimeWindow(int timeWindow){timeFrame = timeWindow;}
        uint getMaximum() const {return max;}
        void setMaximum(uint m){max = m;}
        float getShoulder() const {return asymptote;}
        void setShoulder(float s){asymptote = s;}
        void calculateCorrelation(SortableTable& spikesOfCluster1,SortableTable& spikesOfCluster2,double binSizeInRU,double timeWindowInRU,int halfBins,bool autoCorrelogram);
        int getNbBins(){return nbBins;}
        void setNbBins(int nb){nbBins = nb;}
        uint getValue(int index){return values[index];}
        float getFiringRate() const {return firingRate;}

    private:
        Data& data;
        uint* values;
        Status status;
        int binSize;
        int timeFrame;
        uint max;
        float asymptote;
        int nbBins;
        float firingRate;
    } ;

    /**Dict containing the correlations.
  * Key: a string representing the pair of clusters (id1-id2). The first value of the pair is always the bigger (the correlograms
  * are calculated stored only for (A,B) with A > B and not for (B,A).
  * value: a qdict containing a pair as a key and a Correlation object as a value. The pair represent the the bin size and the time window of the Correlation object.
  */
    QHash< QString, QHash<QString, Correlation*>* > correlationDict;

    /**Excerpt of spikesByCluster for the clusters selected to be recluster.*/
    SortableTable reclusteringSpikesByCluster;

    /** Map given the cluster user information, the key is the cluster id and the value an instance of ClusterUserInformation.
  This map is only used to store the data read from or write to the parameter file (xml).
  */
    QMap<int,ClusterUserInformation> clusterUserInformationMap;

    //Methods
    /**
  * Fills the undo lists (spikesByClusterUndoList,clusterInfoMapUndoList) to prepare for a futur undo.
  * @param spikesByClusterTemp the newly created spikesByCluster array
  * @param clusterInfoMapTemp the newly created ClusterInfoMap map
  */
    void prepareUndo(SortableTable* spikesByClusterTemp,ClusterInfoMap* clusterInfoMapTemp);

    /**
  * Moves the clusters contained in @p clustersToDelete to a the cluster @p destinationId. The correponding spikes are assign to cluster @p destinationId
  * @param clustersToDelete a list of the cluster numbers (in ascending order) identifying the clusters to delete.
  * @param spikesByClusterTemp the new spikesByCluster which will contain the new distribution of the spikes among the clusters
  * @param clusterInfoMapTemp the new clusterInfoMap which will contain the information on the new clusters
  * @param upperInsertionIndex the starting position in spikesByCluster where to insert the spikes which are moved
  * @param nbSpikesInNewCluster the current number of spikes in the new cluster
  * @param destinationId the cluster id of destination (0 for artifact or 1for noise)
  * @param positions list containing, for each cluster to delete,
  * the position of its first spike in the new cluster's spikes (starting from the first spike of the new cluster).
  * @param nbOfspikes list containing, for each cluster to delete, its number of spikes.
  */
    void moveClusters(QList<int>& clustersToDelete,SortableTable* spikesByClusterTemp,ClusterInfoMap* clusterInfoMapTemp,long upperInsertionIndex,long& nbSpikesInNewCluster,int destinationId,QList<long>& positions,QList<long>& nbOfspikes);

    /**Creates a new thread to calculate the min and max of the dimensions when the cluster 0 is modified.*/
    MinMaxThread* minMaxCalculator();

    /**
  * Gets the waveform points for cluster @p clusterId in the sample mode.
  * Take a sample of the spikes evenly distributed on all the recording.
  * @param clusterId id of the cluster to get waveform information for.
  * @param nbSpkToDisplay number of spikes to display.
  * @return the status, READY if the data have already been collected or the current collection is finish,
  * and IN_PROCESS if an other thread is already treating @p clusterId.
  */
    Status getSampleWaveformPoints(int clusterId,dataType nbSpkToDisplay);

    /**
  * Gets the waveform points for cluster @p clusterId in time frame mode.
  * Take all the spikes in a given time frame.
  * @param clusterId id of the cluster to get waveform information for.
  * @param start starting time in second
  * @param end ending time in second.
  * @return the status, READY if the data have already been collected or the current collection is finish,
  * and IN_PROCESS if an other thread is already treating that cluster.
  */
    Status getTimeFrameWaveformPoints(int clusterId,dataType start,dataType end);

    /**
  * Calculates the mean and the standard deviation for cluster @p clusterId in the sample mode.
  * In that mode, the spikes used are a sample of the spikes evenly distributed on all the recording.
  * @param clusterId id of the cluster to calculate the data for.
  * @param nbSpkToDisplay number of spikes diplayed.
  * @return the status, READY if the data have already been calculated or the current calculation is finish,
  * IN_PROCESS if an other thread is already treating that cluster and NOT_AVAILABLE
  * if the spikes have not been collected yet.
  */
    Status calculateSampleMean(int clusterId,dataType nbSpkToDisplay);

    /**
  * Calculates the mean and the standard deviation for cluster @p clusterId in the time frame mode.
  * In that mode, all the spikes in a given time frame are selected.
  * @param clusterId id of the cluster to calculate the data for.
  * @param start starting time in second
  * @param end ending time in second.
  * @return the status, READY if the data have already been calculated or the current calculation is finish,
  * IN_PROCESS if an other thread is already treating that cluster and NOT_AVAILABLE
  * if the spikes have not been collected yet.
  */
    Status calculateTimeFrameMean(int clusterId,dataType start,dataType end);

    /**
  * Remove all the correlations link to the cluster @p clusterId. This mean remove the
  * corresponding entries from correlationMap.
  * @param clusterId id of the cluster for which the cleaning has been asked.
  * @param currentClusterList list of the clusters to look for cleaning.
  * @param cleanProcess true if the cluster has to be remove from correlationInProcess false otherwise.
  * The default is false.
  */
    void cleanCorrelation(dataType clusterId,QList<dataType> currentClusterList,bool cleanProcess = false);

    /**
  * Renumber all the correlations.
  * @param clusterIdsOldNew map between old and new cluster ids.
  */
    void renumberCorrelation(QMap<int,int>& clusterIdsOldNew);

    /**Returns the time corresponding to a spike.
  * @param spikesOfCluster one row SortableTable corresponding to the position of the cluser's spikes in
  * features sorted by position.
  * @param spike position.
  */
    double spikeTime(SortableTable& spikesOfCluster,dataType spike){
        dataType currentPositionInFeatures = spikesOfCluster(1,spike);
        return static_cast<double>(features(currentPositionInFeatures,nbDimensions));
    }

    /**Sorts by time the spikes of a newly created cluster created from other clusters, knowing
  * that the spikes from the other clusters are already sorted.
  * @param clusterInfoMapTemp the new clusterInfoMap which will contain the information on the new clusters.
  * @param spikesByClusterTemp the new spikesByCluster which will contain the new distribution of the spikes among the clusters.
  * @param clusterId the id of the cluster to sort.
  * @param positions list containing, for each contributing cluster,
  * the position of either its last spike (if @p fromTop is false) or its first spike (if @p fromTop is true)
  * in the new cluster's spikes.
  * @param nbOfspikes list containing, for each contributing cluster, its number of spikes.
  * @param step equals -1 if the contributing clusters are sorted from the bigger to the smaller
  * and equals 1 if the contributing clusters are sorted from the smaller to the bigger.
  * @param fromTop true if the position is given starting from the first spike of the new cluster, false if
  * it is given starting from the last spike of the new cluster. The default is false.
  */
    void sortCluster(ClusterInfoMap* clusterInfoMapTemp,SortableTable* spikesByClusterTemp,dataType clusterId,QList<dataType> positions,QList<dataType> nbOfspikes,int step,bool fromTop = false);

    /**
  * Sorts by time the spikes of a newly created cluster created from other clusters.
  * The function is an overload of the previous one for the cases where the cluster to sort is
  * either the cluster 0 or the cluster 1. In that case the spikes coming from those clusters
  * are sorted from the smaller time to the bigger whereas the other clusters still
  * sorted from the bigger to the smaller.
  * @param clusterInfoMapTemp the new clusterInfoMap which will contain the information on the new clusters.
  * @param spikesByClusterTemp the new spikesByCluster which will contain the new distribution of the spikes among the clusters.
  * @param clusterId the id of the cluster to sort.
  * @param lastPositions list containing, for each contributing cluster,
  * the position of its last spike in the new cluster's spikes (starting from the last spike
  * of the new cluster).
  * @param nbOfspikes list containing, for each contributing cluster, tits number of spikes.e
  * @param firstPosition position of the first spike of the cluster 0 or 1.
  * @param number number of spikes of either the cluster 0 or 1.
  */
    void sortCluster(ClusterInfoMap* clusterInfoMapTemp,SortableTable* spikesByClusterTemp,dataType clusterId,QList<dataType> lastPositions,QList<dataType> nbOfspikes,dataType firstPosition,dataType number);


    /**
  * Finds the closest spike to a given time @p time among the pikes contain in @p spikesOfCluster.
  * @param time the time look up.
  * @param spikesOfCluster array of sorted spikes in which to look up.
  */
    long findSpikePosition(double time,SortableTable& spikesOfCluster);

    /**
  * Makes a copy of the internal variables, spikesOfCluster and clusterInfoMap, used to store
  * insformation about the clusters.
  * @param spikesOfClusterTemp pointer on a SortableTable which will point on a copy of the internal variable spikesOfCluster.
  * @param clusterInfoMapTemp pointer on a ClusterInfoMap which will point on a copy of the internal variable clusterInfoMap.
  */
    void duplicate(SortableTable* & spikesOfClusterTemp,ClusterInfoMap* & clusterInfoMapTemp);

public:

    /**This class a wrapper to waveform information (spikes, mean value and standard deviation).
  */
    class WaveformIterator{
    public:
        virtual ~WaveformIterator(){}
        void setSpikesAvailable(bool available){spikesAvailable = available;}
        bool areSpikesAvailable(){return spikesAvailable;}
        void setMeanAvailable(bool available){meanAvailable = available;}
        bool isMeanAvailable(){return meanAvailable;}
        virtual dataType nextSpike(){return 0;}
        virtual dataType nextMeanValue(){return 0;}
        virtual dataType nextStDeviationValue(){return 0;}
        virtual dataType nbOfSpikes() const{return 0;}

        WaveformIterator(){init();}

    protected:
        WaveformIterator(Waveforms* waveformsData){
            init();
            waveforms = waveformsData;
        }
        void init(){
            waveforms = 0L;
            spikesIndex = -1;
            meanIndex = -1;
            stDeviationIndex = -1;
            spikesAvailable = false;
            meanAvailable = false;
        };

        Waveforms* waveforms;
        dataType spikesIndex;
        dataType meanIndex;
        dataType stDeviationIndex;
        bool spikesAvailable;
        bool meanAvailable;
    };



    class SampleWaveformIterator;
    friend class SampleWaveformIterator;

    /**
  * Creates and returns an sampleWaveformIterator on the spikes of the given cluster with the specified @p nbSampleSpikes number of spikes.
  * This iterator iterates on the spikes selected for the sample mode presentation of the waveforms.
  * In that mode, only a sample of the spikes evenly distributed on all the recording are available.
  * @param clusterId the number of the cluster on which spikes the iterator will iterates.
  * @param nbSampleSpikes number of spikes selected for the sample mode presentation.
  * @return the sampleWaveformIterator on the spikes of the given cluster.
  */
    SampleWaveformIterator* sampleWaveformIterator(dataType clusterId,dataType nbSampleSpikes){
        QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
        int clusterIdInt = static_cast<int>(clusterId);
        SampleWaveformIterator* waveformIterator;

        if(waveformStatusMap.contains(clusterIdInt)){
            Waveforms* waveforms = waveformDict[clusterIdString];
            waveformIterator = new SampleWaveformIterator(waveforms);
            if(waveformStatusMap[clusterIdInt].sampleMeanStatus() == READY)
                waveformIterator->setMeanAvailable(true);
            if(waveformStatusMap[clusterIdInt].sampleStatus() == READY){
                waveformIterator->setSpikesAvailable(true);
                waveformIterator->updateStatus(nbSampleSpikes);
            }
        }
        else{
            //No data are available, create any of the iterator (they will have their
            //data availability booleans to false).
            //The case is for security reason but should never be reach.
            waveformIterator = new SampleWaveformIterator();
        }
        return waveformIterator;
    }

    /** Returns an iterator on the latest waveform data stored by a request of
   * a waveformTread for a given cluster. The data correspond to the sample display mode
   * (only a sample of the spikes evenly distributed on all the recording were collected).
   * Caution: in Qt graphical coordinate system, the Y axis is inverted (increasing downwards),
   * thus a point (x,y) as to be drawn and tested as (x,-y).
   * The value returns by the iterator is the spike sample value taking the Qt graphical
    * coordinate system into consideration, the ordinate coordinate is the opposite of the raw data.
  */
    class SampleWaveformIterator: public WaveformIterator{
        //Only the method iterator of data has access to the protected part of Iterator,
        //the constructor of Iterator being private, only this method con create a new Iterator
        friend SampleWaveformIterator* Data::sampleWaveformIterator(dataType clusterId,dataType nbSampleSpikes);

    public:
        ~SampleWaveformIterator(){}
        dataType nextSpike(){
            ++spikesIndex;
            return - static_cast<dataType>(waveforms->getSample(spikesIndex));
        }
        dataType nextMeanValue(){
            ++meanIndex;
            return - static_cast<dataType>(waveforms->getSampleMean(meanIndex));
        }
        dataType nextStDeviationValue(){
            ++stDeviationIndex;
            return - static_cast<dataType>(waveforms->getSampleStDeviation(stDeviationIndex));
        }
        dataType nbOfSpikes() const{
            return waveforms->nbOfSpikes(SAMPLE);
        }
    private:
        SampleWaveformIterator(): WaveformIterator(){}
        SampleWaveformIterator(Waveforms* waveformsData): WaveformIterator(waveformsData){}
        void updateStatus(dataType nbSampleSpikes){
            if(waveforms->nbOfSpikesAsked() != nbSampleSpikes){
                setSpikesAvailable(false);
                setMeanAvailable(false);
            }
        }

    };

    class TimeFrameWaveformIterator;
    friend class TimeFrameWaveformIterator;

    /**
  * Creates and returns an TimeFrameWaveformIterator on the spikes of the given cluster.
  * This iterator iterates on the spikes selected for the time frame mode presentation of the waveforms.
  * In that mode, all the spikes in a given time frame are available.
  * @param clusterId the number of the cluster on which spikes the iterator will iterates.
  * @param startTime starting time selected for the time frame mode.
  * @param endTime ending time selected for the time frame mode.
  * @return the TimeFrameWaveformIterator on the spikes of the given cluster.
  */
    TimeFrameWaveformIterator* timeFrameWaveformIterator(dataType clusterId,dataType startTime,dataType endTime){
        QString clusterIdString = QString::fromLatin1("%1").arg(clusterId);
        int clusterIdInt = static_cast<int>(clusterId);
        TimeFrameWaveformIterator* waveformIterator;

        if(waveformStatusMap.contains(clusterIdInt)){
            Waveforms* waveforms = waveformDict[clusterIdString];
            waveformIterator = new TimeFrameWaveformIterator(waveforms);
            if(waveformStatusMap[clusterIdInt].timeFrameMeanStatus() == READY) waveformIterator->setMeanAvailable(true);
            if(waveformStatusMap[clusterIdInt].timeFrameStatus() == READY){
                waveformIterator->setSpikesAvailable(true);
                waveformIterator->updateStatus(startTime,endTime);
            }
        }
        else{
            //No data are available, create any of the iterator (they will have their
            //data availability booleans to false).
            //The case is for security reason but should never be reach.
            waveformIterator = new TimeFrameWaveformIterator();
        }
        return waveformIterator;
    }

    /** Returns an iterator on the latest waveform data stored by a request of
   * a waveformTread for a given cluster. The data correspond to the time frame display mode
   * (ll the spikes in a given time frame are available).
   * Caution: in Qt graphical coordinate system, the Y axis is inverted (increasing downwards),
   * thus a point (x,y) as to be drawn and tested as (x,-y).
   * The value returns by the iterator is the spike sample value taking the Qt graphical
    * coordinate system into consideration, the ordinate coordinate is the opposite of the raw data.
  */
    class TimeFrameWaveformIterator: public WaveformIterator{
        //Only the method iterator of data has access to the protected part of Iterator,
        //the constructor of Iterator being private, only this method con create a new Iterator
        friend TimeFrameWaveformIterator* Data::timeFrameWaveformIterator(dataType clusterId,dataType startTime,dataType endTime);

    public:
        ~TimeFrameWaveformIterator(){}
        dataType nextSpike(){
            ++spikesIndex;
            return - static_cast<dataType>(waveforms->getTimeFrame(spikesIndex));
        }
        dataType nextMeanValue(){
            ++meanIndex;
            return - static_cast<dataType>(waveforms->getTimeFrameMean(meanIndex));
        }
        dataType nextStDeviationValue(){
            ++stDeviationIndex;
            return - static_cast<dataType>(waveforms->getTimeFrameStDeviation(stDeviationIndex));
        }
        dataType nbOfSpikes() const{
            return waveforms->nbOfSpikes(TIME_FRAME);
        }

    private:
        TimeFrameWaveformIterator(): WaveformIterator(){}
        TimeFrameWaveformIterator(Waveforms* waveformsData): WaveformIterator(waveformsData){}
        void updateStatus(dataType start,dataType end){
            if(waveforms->startTime() != start || waveforms->endTime() != end){
                setSpikesAvailable(false);
                setMeanAvailable(false);
            }
        }

    };
    /**
  * Calculates the correlograms for each pair of clusters given in pairs.
  * @param pair pair of clusters for which a correlogram has to be compute.
  * @param binSize size of the bins to compute given in miliseconds.
  * @param timeWindow time frame use to compute the correlograms, given in miliseconds.
  * @param binSizeInRU size of the bins to compute given in recording units.
  * @param timeWindowInRU half of the time frame use to compute the correlograms, given in recording units.
  * @param halfBins  the number of bins to compute are so there are a total of nBins = 1+2*halfBins bins
  * (halfBins.5 for each halfTimeWindow).
  * @return the status, READY if the data have already been calculated or the asked computation is finish,
  * and IN_PROCESS if an other thread is already treating the pairs the thread has to do.
  */
    Status getCorrelograms(Pair& pair,int binSize,int timeWindow,double binSizeInRU,float timeWindowInRU,int halfBins);

    class CorrelogramIterator;
    friend class CorrelogramIterator;

    /**
  * Creates and returns an iterator on the correlogram data of the given pair of clusters (@p pair).
  * @param pair the pair of clusters on which correlogram data the iterator will iterates.
  * @param scale the scale to applied to the correlogram's data (maximum value, shoulder value or no scale).
  * @param binSize size of the bin used to compute the correlogram.
  * @param timeframe time frame used to compute the correlogram.
  * @return the iterator on the correlogram data of the given pair.
  */
    CorrelogramIterator correlogramIterator(Pair pair,ScaleMode scale,int binSize,int timeframe){
        return CorrelogramIterator(*this,pair,scale,binSize,timeframe);
    }

    /** Specialized iterator on the latest correlation data stored by a request of
   * a correlationTread for a given pair of clusters. The data correspond to the binSize and
   * timeFrame specified by the correlationTread.
   * Caution: in Qt graphical coordinate system, the Y axis is inverted (increasing downwards),
   * thus a point (x,y) as to be drawn and tested as (x,-y).
   * The value returns by the iterator is either the raw correlogram value or the values scaled
   * either the maximum of the correlogram or the shoulder. The values return take the Qt graphical
   * coordinate system into consideration, the ordinate coordinate is the opposite of the raw data.
  */
    class CorrelogramIterator{
        //Only the method correlogramIterator of data has access to the private part of CorrelogramIterator,
        //the constructor of CorrelogramIterator being private, only this method con create a new CorrelogramIterator
        friend CorrelogramIterator Data::correlogramIterator(Pair pair,ScaleMode scale,int binSize,int timeframe);

    public:
        ~CorrelogramIterator(){}
        /**Returns the current value and increments the iterator.*/
        float next(){
            float value =  - (static_cast<float>(correlation->getValue(index)) / static_cast<float>(scale));
            index++;
            return value;
        }
        /**Check if there is more values.*/
        bool hasNext(){return (lastIndex >= index);}
        /**Returns true if there is data corresponding to the parameters given
    * in the iterator constructor.
    */
        bool isDataAvailable(){return dataAvailable;}

        float getShoulder() const {return - correlation->getShoulder();}

        float getScaledShoulder() const {return - correlation->getShoulder() / static_cast<float>(scale);}

        float getFiringRate() const {return correlation->getFiringRate(); }
    private:
        CorrelogramIterator(const Data& d,Pair pair,ScaleMode scaleMode,int binSize,int timeframe):data(d){
            index = 0;
            lastIndex = -1;
            QHash<QString, Correlation*>* dict = data.correlationDict[pair.toString()];
            if(dict == 0) dataAvailable = false;
            else{
                Pair parameters = Pair(binSize,timeframe);
                correlation = (*dict)[parameters.toString()];
                if(correlation == 0) dataAvailable = false;
                else{
                    if(correlation->getStatus(binSize,timeframe) == READY){
                        dataAvailable = true;
                        lastIndex = correlation->getNbBins() - 1;
                        switch(scaleMode){
                        case RAW:
                            scale = 1;
                            break;
                        case MAX:
                            scale = static_cast<float>(correlation->getMaximum());
                            break;
                        case SHOULDER:
                            scale = correlation->getShoulder();
                            break;
                        }
                    }
                    else dataAvailable = false;
                }
            }
        };
        /**Returns true if the iterator has reach the last spike for the cluster on which it iterates,
      * false otherwise.
      */
        const Data& data;
        long index;
        long lastIndex;
        bool dataAvailable;
        Data::Correlation* correlation;
        float scale;
    };

    /**
  * Loads the clusters created by the automatic reclustering program.
  * @param clusterFile the cluster file created by the automatic reclustering program.
  * @return true if the loading succed false otherwise
  */
    bool loadReclusteredClusters(QFile &clusterFile);

    /**
  * Load the features data and configuration data when the cluster file does not exist.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param spkFileLength the length of the .spk.i file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile &featureFile, long spkFileLength, QString& errorInformation);

    /**
  * Load the features data, cluster data and configuration data.
  * Initialize the internal representation of the data.
  * @param featureFile the .fet file
  * @param clusterFile the .clu.i file
  * @param spkFileLength the length of the .spk.i file
  * @param errorInformation string which, in case of an error, will contain detail about it.
  * @return true if the initialization succeded false otherwise
  */
    bool initialize(QFile &featureFile, QFile &clusterFile, long spkFileLength, QString& errorInformation);

};

#endif
