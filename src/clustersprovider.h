/***************************************************************************
                          clustersprovider.h  -  description
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

#ifndef CLUSTERSPROVIDER_H
#define CLUSTERSPROVIDER_H

//include files for the application
#include <dataprovider.h>
#include <array.h>
#include <types.h>
#include "data.h"

// include files for QT
#include <qobject.h>

#include <QList>


/**
  *@author Lynn Hazan
  */

class ClustersProvider : public DataProvider  {
    Q_OBJECT
public:

    /**Information retun after a call to openFile/saveDocument/createFeatureFile*/
    enum loadReturnMessage {OK=0,OPEN_ERROR=1,MISSING_FILE=3,COUNT_ERROR=4,INCORRECT_CONTENT=5};

    /**Constructor.
  * @param fileUrl the url of the cluster file containing the cluster ids.
  * @param samplingRate sampling rate used to record the data (of the acquisition system).
  * @param currentSamplingRate sampling rate of the current file.
  * @param clusteringData object containing all the document data.
  * @param dataFileMaxTime maximum time of the data file in recording units.
  */
    explicit ClustersProvider(const QString &fileUrl, double samplingRate, double currentSamplingRate, Data& clusteringData, dataType dataFileMaxTime);
    ~ClustersProvider();

    /**Triggers the retrieve of the cluster information included in the time interval given by @p startTime and @p endTime.
  * @param startTime begining of the time interval from which to retrieve the data in miliseconds.
  * @param endTime end of the time interval from which to retrieve  the data.
  * @param initiator instance requesting the data.
  * @param startTimeInRecordingUnits begining of the time interval from which to retrieve the data in recording units.
  */
    void requestData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits);


    /**Looks up for the first of the clusters included in the list @p selectedIds existing after the time @p startTime.
  * All the clusters included in the time interval given by @p timeFrame are retrieved. The time interval start time is
  * computed in order to have the first cluster found located at @p clusterPosition percentage of the time interval.
  * @param startTime starting time, in miliseconds, for the look up.
  * @param timeFrame time interval for which to retrieve  the data.
  * @param selectedIds list of cluster ids to look up for.
  * @param initiator instance requesting the data.
  * @param startTimeInRecordingUnits starting time, in recording units, for the look up.
  */
    void requestNextClusterData(long startTime,long timeFrame,QList<int> selectedIds,QObject* initiator,long startTimeInRecordingUnits);


    /**Looks up for the first of the clusters included in the list @p selectedIds existing before the time @p endTime.
  * All the clusters included in the time interval given by @p timeFrame are retrieved. The time interval start time is
  * computed in order to have the first cluster found located at @p clusterPosition percentage of the time interval.
  * @param startTime starting time, in miliseconds, for the look up.
  * @param timeFrame time interval for which to retrieve  the data.
  * @param selectedIds list of cluster ids to look up for.
  * @param initiator instance requesting the data.
  * @param startTimeInRecordingUnits starting time, in recording units, for the look up.
  */
    void requestPreviousClusterData(long startTime,long timeFrame,QList<int> selectedIds,QObject* initiator,long startTimeInRecordingUnits);


    /**Sets the list of cluster Ids to be used when providing data.*/
    void setClusterIdList(QList<int>* list){clusterIds = list;}


    /**Returns the name of the provider which is the cluster file number.
  * @return provider'name.
  */
    QString getName() const {return name;}


Q_SIGNALS:
    /**Signals that the data have been retrieved.
  * @param data 2 line array containing the sample index of the peak index of each spike existing in the requested time frame with the
  * corresponding cluster id. The first line contains the sample index and the second line the cluster id.
  * @param initiator instance requesting the data.
  * @param providerName name of the instance providing the data.
  */
    void dataReady(Array<dataType>& data,QObject* initiator,QString providerName);

    /**Signals that the data for the next cluster have been retrieved.
  * @param data 2 line array containing the sample index of the peak index of each spike existing in the requested time frame with the
  * corresponding cluster id. The first line contains the sample index and the second line the cluster id.
  * @param initiator instance requesting the data.
  * @param providerName name of the instance providing the data.
  * @param startingTime time from which the data have been retrieved in miliseconds.
  * @param startingTimeInRecordingUnits time from which the data have been retrieved in recording units.
  */
    void nextClusterDataReady(Array<dataType>& data,QObject* initiator,QString providerName,long startingTime,long startingTimeInRecordingUnits);

    /**Signals that the data for the previous cluster have been retrieved.
  * @param data 2 line array containing the sample index of the peak index of each spike existing in the requested time frame with the
  * corresponding cluster id. The first line contains the sample index and the second line the cluster id.
  * @param initiator instance requesting the data.
  * @param providerName name of the instance providing the data.
  * @param startingTime time from which the data have been retrieved.
  * @param startingTimeInRecordingUnits time from which the data have been retrieved in recording units.
  */
    void previousClusterDataReady(Array<dataType>& data,QObject* initiator,QString providerName,long startingTime,long startingTimeInRecordingUnits);


private:

    /**Provider's name.*/
    QString name;

    /**Sampling rate used to record the data.*/
    double samplingRate;

    /**List of the cluster ids to be used when looking up for the data.*/
    QList<int>* clusterIds;

    /**The maximum time of the file in recording units.*/
    dataType fileMaxTime;

    /**Represents the percentage from the begining of the window where the clusters are display when browsing.*/
    float clusterPosition;

    /**Reference on the object containing all the document information.*/
    Data& clusteringData;

    /**Total number of dimensions of the data.*/
    int nbOfDimensions;

    /**The start time in recording units for the previously requested data.*/
    long previousStartTime;

    /**The maximum time of the data file in recording units.*/
    dataType dataFileMaxTime;

    //Functions

    /**Retrieves the peak index of each spike included in the time frame given by @p startTime and @p endTime.
  * @param startTime begining of the time frame from which to retrieve the data, given in milisecond.
  * @param endTime end of the time frame from which to retrieve the data, given in milisecond.
  * @param initiator instance requesting the data.
  * @param startTimeInRecordingUnits begining of the time interval from which to retrieve  the data in recording units.
  */
    void retrieveData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits);

};


#endif
