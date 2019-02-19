/***************************************************************************
                          waveform2thread.h  -  description
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

#ifndef WAVEFORM2THREAD_H
#define WAVEFORM2THREAD_H

//include files for the application
#include "waveform2view.h"
#include "data.h"

//include files for QT
#include <qthread.h>


#include <QEvent>
#include <QList>

/**Thread used to retrieve the waveforms2 and compute the means and standard deviations
 * which will be displayed in the Waveform2View.
 * No heavy computation is done is this class, the thread calls the Data object which
 * will do the work.
 *@author Lynn Hazan
 */

class Waveform2Thread : public QThread {

public: 
    //Only the method getWaveforms2 of Waveform2View has access to the private part of Waveform2Thread,
    //the constructor of Waveform2Thread being private, only this method con create a new Waveform2Thread
    friend Waveform2Thread* Waveform2View::getWaveforms2();

    ~Waveform2Thread(){}

    void getWaveform2Information(int clusterId,Waveform2View::PresentationMode mode);
    void getWaveform2Information(const QList<int> &clusterIds, Waveform2View::PresentationMode mode);
    void getMean(const QList<int> &clusterIds, Waveform2View::PresentationMode mode);
    /**Gets the mean and standard deviation for the cluster or the clusters set previously.
  * @param
  */
    void getMean(Waveform2View::PresentationMode mode);

    bool isSingleTriggeringCluster() const {return treatSingleCluster;}
    int triggeringCluster() const {return clusterId;}
    QList<int> triggeringClusters() const {return clusterIds;}
    bool isMeanRequested() const {return  meanRequested;}

    /**Asks the thread to stop his work as soon as possible.*/
    void stopProcessing(){haveToStopProcessing = true;}

    class GetWaveforms2Event;
    friend class GetWaveforms2Event;

    GetWaveforms2Event* getWaveforms2Event(){
        return new GetWaveforms2Event(*this);
    }

    /**
  * Internal class use to send information to the Waveform2View to inform it that
  * the data requested have been collected.
  */
    class GetWaveforms2Event : public QEvent{
        //Only the method getWaveforms2Event of Waveform2Thread has access to the private part of GetWaveforms2Event,
        //the constructor of GetWaveforms2Event being private, only this method con create a new GetWaveforms2Event
        friend GetWaveforms2Event* Waveform2Thread::getWaveforms2Event();

    public:
        Waveform2Thread* parentThread(){return &waveform2Thread;}
        ~GetWaveforms2Event(){}

    private:
        GetWaveforms2Event(Waveform2Thread& thread):QEvent(QEvent::Type(QEvent::User + 200)),waveform2Thread(thread){}

        Waveform2Thread& waveform2Thread;
    };

    class NoWaveform2DataEvent;
    friend class NoWaveform2DataEvent;

    NoWaveform2DataEvent* noWaveform2DataEvent(){
        return new NoWaveform2DataEvent(*this);
    }

    /**
  * Internal class use to send information to the Waveform2View to inform it that
  * there is not data available for the requested cluster. A reason being that the cluster has been suppress
  * after the thread has been launched.
  */
    class NoWaveform2DataEvent : public QEvent{
        //Only the method getWaveforms2Event of Waveform2Thread has access to the private part of GetWaveforms2Event,
        //the constructor of GetWaveforms2Event being private, only this method con create a new GetWaveforms2Event
        friend NoWaveform2DataEvent* Waveform2Thread::noWaveform2DataEvent();

    public:
        Waveform2Thread* parentThread(){return &waveform2Thread;}
        ~NoWaveform2DataEvent(){}

    private:
        NoWaveform2DataEvent(Waveform2Thread& thread):QEvent(QEvent::Type(QEvent::User + 250)),waveform2Thread(thread){}

        Waveform2Thread& waveform2Thread;
    };

protected:
    void run();

private:
    Waveform2Thread(Waveform2View& view,Data& d):waveform2View(view),meanRequested(false),data(d),haveToStopProcessing(false){}

    Waveform2View& waveform2View;
    int clusterId;
    QList<int> clusterIds;
    bool treatSingleCluster;
    bool meanRequested;
    Data& data;
    Waveform2View::PresentationMode mode;
    /**True if the thread has to stop processing, false otherwise.*/
    bool haveToStopProcessing;
};

#endif
