/***************************************************************************
                          waveform2thread.cpp  -  description
                             -------------------
    begin                : Fri Oct 24 2003
    copyright            : (C) 2003 by
    email                :
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
#include "waveform2thread.h"
#include "waveform2view.h"

//QT include files
#include <qapplication.h>

#include <QList>
#include <QDebug>

//Unix include file
#include <unistd.h>

void Waveform2Thread::getWaveform2Information(int clusterId,Waveform2View::PresentationMode mode){
    qDebug() << "Waveform2Thread::getWaveform2Information";
    this->clusterId = clusterId;
    treatSingleCluster = true;
    this->mode = mode;
    start();
}

void Waveform2Thread::getWaveform2Information(const QList<int>& clusterIds,Waveform2View::PresentationMode mode){
    qDebug() << "Waveform2Thread::getWaveform2Information";
    this->clusterIds = clusterIds;
    treatSingleCluster = false;
    this->mode = mode;
    start();
}


void Waveform2Thread::run(){
    qDebug() << "Waveform2Thread::run()";
    //Need to modify this function so that it only provides the spikes that match our min spike diff criteria.

    int sleepingAmount = 1;
    //If the triggering action is not the calculation of the mean and standard variation,
    //get the data and store them in waveform2View.waveform2InfoMap.
    //wait until the data are available. The status can be READY or IN_PROCESS.
    //In the later case, an other thread in working on the same cluster.
    if(!meanRequested  && !haveToStopProcessing){
        if(waveform2View.presentationMode == Waveform2View::SAMPLE){
            if(treatSingleCluster){
                if(!haveToStopProcessing){
                    Data::Status status = data.getSampleWaveform2Points(clusterId,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                    if(status == Data::NOT_AVAILABLE){
                        //Send an event to the waveform2View to let it know that the data requested are not available.
                        NoWaveform2DataEvent* event = noWaveform2DataEvent();
                        QApplication::postEvent(&waveform2View,event);
                        return;
                    }
                    else if(status == Data::IN_PROCESS){
                        while(true){
                            if(haveToStopProcessing) break;
                            sleep(sleepingAmount);
                            status = data.getSampleWaveform2Points(clusterId,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                        }
                    }
                }
            }
            //iterate on all the clusters contained in clusterIds before returning
            else{
                if(!haveToStopProcessing){
                    QList<int>::iterator iterator;
                    QList<int>::iterator end(clusterIds.end());
                    for(iterator = clusterIds.begin(); iterator != end; ++iterator){
                        if(!haveToStopProcessing){
                            Data::Status status = data.getSampleWaveform2Points(*iterator,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                            //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                            if(status == Data::NOT_AVAILABLE)
                                continue;
                            else if(status == Data::IN_PROCESS)
                                while(!haveToStopProcessing && (data.getSampleWaveform2Points(*iterator,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff) == Data::IN_PROCESS))
                                {
                                    sleep(sleepingAmount);
                                }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
        else if(waveform2View.presentationMode == Waveform2View::TIME_FRAME){
            if(treatSingleCluster){
                if(!haveToStopProcessing){
                    Data::Status status = data.getTimeFrameWaveformPoints(clusterId,waveform2View.startTime,waveform2View.endTime);
                    if(status == Data::NOT_AVAILABLE){
                        //Send an event to the wavefor2mView to let it know that the data requested are not available.
                        NoWaveform2DataEvent* event = noWaveform2DataEvent();
                        QApplication::postEvent(&waveform2View,event);
                        return;
                    }
                    else if(status == Data::IN_PROCESS){
                        while(true){
                            if(haveToStopProcessing) break;
                            sleep(sleepingAmount);
                            status = data.getTimeFrameWaveformPoints(clusterId,waveform2View.startTime,waveform2View.endTime);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                        }
                    }
                }
            }
            //iterate on all the clusters contained in clusterIds before returning
            else{
                if(!haveToStopProcessing){
                    QList<int>::iterator iterator;
                    QList<int>::iterator end(clusterIds.end());
                    for(iterator = clusterIds.begin(); iterator != end; ++iterator){
                        if(!haveToStopProcessing){
                            Data::Status status = data.getTimeFrameWaveformPoints(*iterator,waveform2View.startTime,waveform2View.endTime);
                            //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                            if(status == Data::NOT_AVAILABLE) continue;
                            else if(status == Data::IN_PROCESS)
                                while(!haveToStopProcessing && (data.getTimeFrameWaveformPoints(*iterator,waveform2View.startTime,waveform2View.endTime) == Data::IN_PROCESS))
                                {
                                    sleep(sleepingAmount);
                                }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }
    //Calculate the means and standard deviation if needed
    //wait until the data are available. The status can be READY, IN_PROCESS or NOT_AVAILABLE.
    //In the IN_PROCESS case, an other thread in working on the same cluster,
    //In the NOT_AVAILABLE case, the spikes have not been collected, get the data and
    //ask to calculate the data again.
    if((meanRequested || waveform2View.meanPresentation)  && !haveToStopProcessing){
        if(waveform2View.presentationMode == Waveform2View::SAMPLE){
            if(treatSingleCluster){
                if(!haveToStopProcessing){
                    Data::Status status = data.calculateSampleMean(clusterId,waveform2View.nbSpkToDisplay);
                    if(status == Data::NOT_AVAILABLE && !haveToStopProcessing){
                        Data::Status dataStatus = data.getSampleWaveform2Points(clusterId,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                        if(dataStatus == Data::NOT_AVAILABLE){
                            //Send an event to the waveform2View to let it know that the data requested are not available.
                            NoWaveform2DataEvent* event = noWaveform2DataEvent();
                            QApplication::postEvent(&waveform2View,event);
                            return;
                        }
                        else if(dataStatus == Data::IN_PROCESS){
                            while(true){
                                if(haveToStopProcessing) break;
                                sleep(sleepingAmount);
                                dataStatus = data.getSampleWaveform2Points(clusterId,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                                if(dataStatus == Data::READY) break;
                                else if(dataStatus == Data::NOT_AVAILABLE){
                                    //Send an event to the waveform2View to let it know that the data requested are not available.
                                    NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                    QApplication::postEvent(&waveform2View,event);
                                    return;
                                }
                            }
                        }
                        //Now that the data are available, compute the mean and standard deviation
                        while(true){
                            if(haveToStopProcessing) break;
                            status = data.calculateSampleMean(clusterId,waveform2View.nbSpkToDisplay);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                            sleep(sleepingAmount);
                        }
                    }
                    else if(status == Data::IN_PROCESS){
                        while(true){
                            if(haveToStopProcessing) break;
                            sleep(sleepingAmount);
                            status = data.calculateSampleMean(clusterId,waveform2View.nbSpkToDisplay);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                        }
                    }
                }
            } //one cluster
            //iterate on all the clusters contained in clusterIds before returning
            else{
                if(!haveToStopProcessing){
                    QList<int>::iterator iterator;
                    QList<int>::iterator end(clusterIds.end());
                    for(iterator = clusterIds.begin(); iterator != end; ++iterator){
                        if(!haveToStopProcessing){
                            Data::Status status = data.calculateSampleMean(*iterator,waveform2View.nbSpkToDisplay);
                            if(status == Data::NOT_AVAILABLE && !haveToStopProcessing){
                                Data::Status dataStatus = data.getSampleWaveform2Points(*iterator,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);

                                //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                                if(dataStatus == Data::NOT_AVAILABLE) continue;
                                if(dataStatus == Data::IN_PROCESS || (dataStatus == Data::READY)){
                                    while(true){
                                        if(haveToStopProcessing) break;
                                        if(dataStatus == Data::READY){
                                            //Now that the data are available, compute the mean and standard deviation
                                            while(true){
                                                if(haveToStopProcessing)
                                                    break;
                                                status = data.calculateSampleMean(*iterator,waveform2View.nbSpkToDisplay);
                                                if(status == Data::READY || status == Data::NOT_AVAILABLE)
                                                    break;
                                                sleep(sleepingAmount);
                                            }
                                            break;
                                        }
                                        else{
                                            sleep(sleepingAmount);
                                            dataStatus = data.getSampleWaveform2Points(*iterator,waveform2View.nbSpkToDisplay, waveform2View.minSpikeDiff);
                                            if(dataStatus == Data::NOT_AVAILABLE)
                                                break;
                                        }
                                    }
                                }
                            }
                            else if(status == Data::IN_PROCESS){
                                //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                                while(true){
                                    if(haveToStopProcessing)
                                        break;
                                    sleep(sleepingAmount);
                                    status = data.calculateSampleMean(*iterator,waveform2View.nbSpkToDisplay);
                                    if(status == Data::READY || status == Data::NOT_AVAILABLE)
                                        break;
                                }
                            }
                        }//Stop processing
                        else break;
                    }//iteration on the clusters
                }//Stop processing
            }//several clusters
        }//Sample
        else if(waveform2View.presentationMode == Waveform2View::TIME_FRAME){
            if(treatSingleCluster){
                if(!haveToStopProcessing){
                    Data::Status status = data.calculateTimeFrameMean(clusterId,waveform2View.startTime,waveform2View.endTime);
                    if(status == Data::NOT_AVAILABLE && !haveToStopProcessing){
                        Data::Status dataStatus = data.getTimeFrameWaveformPoints(clusterId,waveform2View.startTime,waveform2View.endTime);
                        if(dataStatus == Data::NOT_AVAILABLE){
                            //Send an event to the waveform2View to let it know that the data requested are not available.
                            NoWaveform2DataEvent* event = noWaveform2DataEvent();
                            QApplication::postEvent(&waveform2View,event);
                            return;
                        }
                        else if(dataStatus == Data::IN_PROCESS){
                            while(true){
                                if(haveToStopProcessing)
                                    break;
                                sleep(sleepingAmount);
                                dataStatus = data.getTimeFrameWaveformPoints(clusterId,waveform2View.startTime,waveform2View.endTime);
                                if(dataStatus == Data::READY)  {
                                    break;
                                } else if(dataStatus == Data::NOT_AVAILABLE) {
                                    //Send an event to the waveform2View to let it know that the data requested are not available.
                                    NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                    QApplication::postEvent(&waveform2View,event);
                                    return;
                                }
                            }
                        }
                        //Now that the data are available, compute the mean and standard deviation
                        while(true){
                            if(haveToStopProcessing) break;
                            status = data.calculateTimeFrameMean(clusterId,waveform2View.startTime,waveform2View.endTime);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                            sleep(sleepingAmount);
                        }
                    }
                    else if(status == Data::IN_PROCESS){
                        while(true){
                            if(haveToStopProcessing) break;
                            sleep(sleepingAmount);
                            status = data.calculateTimeFrameMean(clusterId,waveform2View.startTime,waveform2View.endTime);
                            if(status == Data::READY) break;
                            else if(status == Data::NOT_AVAILABLE){
                                //Send an event to the waveform2View to let it know that the data requested are not available.
                                NoWaveform2DataEvent* event = noWaveform2DataEvent();
                                QApplication::postEvent(&waveform2View,event);
                                return;
                            }
                        }
                    }
                }//Stop processing
            }//one cluster
            //iterate on all the clusters contained in clusterIds before returning
            else{
                if(!haveToStopProcessing){
                    QList<int>::iterator iterator;
                    QList<int>::iterator end(clusterIds.end());
                    for(iterator = clusterIds.begin(); iterator != end; ++iterator){
                        if(!haveToStopProcessing){
                            Data::Status status = data.calculateTimeFrameMean(*iterator,waveform2View.startTime,waveform2View.endTime);
                            if(status == Data::NOT_AVAILABLE && !haveToStopProcessing){
                                Data::Status dataStatus = data.getTimeFrameWaveformPoints(*iterator,waveform2View.startTime,waveform2View.endTime);
                                //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                                if(dataStatus == Data::NOT_AVAILABLE)
                                    continue;
                                if(dataStatus == Data::IN_PROCESS  || (dataStatus == Data::READY)){
                                    while(true){
                                        if(haveToStopProcessing) break;
                                        if(dataStatus == Data::READY){
                                            //Now that the data are available, compute the mean and standard deviation
                                            while(true){
                                                if(haveToStopProcessing) break;
                                                status = data.calculateTimeFrameMean(*iterator,waveform2View.startTime,waveform2View.endTime);
                                                if(status == Data::READY || status == Data::NOT_AVAILABLE) break;
                                                sleep(sleepingAmount);
                                            }
                                            break;
                                        }
                                        else{
                                            sleep(sleepingAmount);
                                            dataStatus = data.getTimeFrameWaveformPoints(*iterator,waveform2View.startTime,waveform2View.endTime);
                                            if(dataStatus == Data::NOT_AVAILABLE)
                                                break;
                                        }
                                    }
                                }
                            }
                            else if(status == Data::IN_PROCESS){
                                //If the data for one cluster is not available, skip it (do not send an event to the waveform2View)
                                while(true){
                                    if(haveToStopProcessing) break;
                                    sleep(sleepingAmount);
                                    status = data.calculateTimeFrameMean(*iterator,waveform2View.startTime,waveform2View.endTime);
                                    if(status == Data::READY || status == Data::NOT_AVAILABLE) break;
                                }
                            }
                        }//Stop processing
                        else break;
                    }//iteration on the clusters
                }//Stop processing
            }//several clusters
        }//Time frame
    }//mean

    //Send an event to the waveform2View to let it know that the waveform2 information have been retrieved.
    GetWaveforms2Event* event = getWaveforms2Event();
    QApplication::postEvent(&waveform2View,event);
}

void Waveform2Thread::getMean(Waveform2View::PresentationMode mode){
    meanRequested = true;
    this->mode = mode;
    start();
}

void Waveform2Thread::getMean(const QList<int>& clusterIds,Waveform2View::PresentationMode mode){

    qDebug()<<"in  Waveform2Thread::getMean";

    meanRequested = true;
    this->clusterIds = clusterIds;
    treatSingleCluster = false;
    this->mode = mode;
    start();
}

