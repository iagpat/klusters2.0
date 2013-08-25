/***************************************************************************
                          correlationthread.h  -  description
                             -------------------
    begin                : Fri Nov 14 2003
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

#ifndef CORRELATIONTHREAD_H
#define CORRELATIONTHREAD_H

//include files for the application
#include "correlationview.h"
#include "data.h"
#include "pair.h"

//include files for QT
#include <qthread.h>


#include <QEvent>
#include <QList>


/** Thread used to compute the correlograms displayed in the CorrelationView.
 * No heavy computation is done is this class, the thread calls the Data object which
 * will do the work.
 *@author Lynn Hazan
 */

class CorrelationThread : public QThread {


public:
    //Only the method getCorrelations of CorrelationView has access to the private part of CorrelationThread,
    //the constructor of CorrelationThread being private, only this method con create a new CorrelationThread
    friend CorrelationThread* CorrelationView::getCorrelations(QList<Pair>* pairsToCompute,const QList<int>& clusterIds);

    ~CorrelationThread(){}
    QList<Pair>* triggeringPairs(){return clusterPairs;}
    QList<int> triggeringClusters() const {return clusterIds;}

    /**Asks the thread to stop his work as soon as possible.*/
    void stopProcessing(){haveToStopProcessing = true;}

    class CorrelationsEvent;
    friend class CorrelationsEvent;

    CorrelationsEvent* getCorrelationsEvent(){
        return new CorrelationsEvent(*this);
    }

    /**
  * Internal class use to send information to the CorrelationView to inform it that
  * the data requested have been collected.
  * @author Lynn Hazan
  */
    class CorrelationsEvent : public QEvent{
        //Only the method getCorrelationsEvent of CorrelationThread has access to the private part of CorrelationsEvent,
        //the constructor of CorrelationsEvent being private, only this method con create a new CorrelationsEvent
        friend CorrelationsEvent* CorrelationThread::getCorrelationsEvent();

    public:
        CorrelationThread* parentThread(){return &correlationThread;}
        ~CorrelationsEvent(){}

    private:
        CorrelationsEvent(CorrelationThread& thread)
            :QEvent(QEvent::Type(QEvent::User + 300)),correlationThread(thread){}

        CorrelationThread& correlationThread;
    };

protected:
    void run();

private:
    CorrelationThread(CorrelationView& view,Data& d,QList<Pair>* pairs,const QList<int>& clusterIds)
        :correlationView(view),data(d),haveToStopProcessing(false){
        clusterPairs = pairs;
        this->clusterIds = clusterIds;
        start();
    }

    CorrelationView& correlationView;
    Data& data;
    QList<Pair>* clusterPairs;
    QList<int> clusterIds;
    /**True if the thread has to stop processing, false otherwise.*/
    bool haveToStopProcessing;

};


#endif
