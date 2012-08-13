/***************************************************************************
                          waveformthread.h  -  description
                             -------------------
    begin                : Fri Oct 24 2003
    copyright            : (C) 2003 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WAVEFORMTHREAD_H
#define WAVEFORMTHREAD_H

//include files for the application
#include "waveformview.h"
#include "data.h"

//include files for QT
#include <qthread.h>

 /**Thread used to retrieve the waveforms and compute the means and standard deviations
 * which will be displayed in the WaveformView.
 * No heavy computation is done is this class, the thread calls the Data object which
 * will do the work.
 *@author Lynn Hazan
 */

class WaveformThread : public QThread {

public: 
  //Only the method getWaveforms of WaveformView has access to the private part of WaveformThread,
  //the constructor of WaveformThread being private, only this method con create a new WaveformThread
  friend WaveformThread* WaveformView::getWaveforms();

	inline ~WaveformThread(){};

  void getWaveformInformation(int clusterId,WaveformView::PresentationMode mode);
  void getWaveformInformation(QValueList<int> clusterIds,WaveformView::PresentationMode mode);
  void getMean(QValueList<int> clusterIds,WaveformView::PresentationMode mode);
  /**Gets the mean and standard deviation for the cluster or the clusters set previously.
  * @param
  */
  void getMean(WaveformView::PresentationMode mode);

  inline bool isSingleTriggeringCluster(){return treatSingleCluster;};
  inline int triggeringCluster(){return clusterId;};
  inline QValueList<int> triggeringClusters(){return clusterIds;};
  inline bool isMeanRequested(){return  meanRequested;};

  /**Asks the thread to stop his work as soon as possible.*/
  inline void stopProcessing(){haveToStopProcessing = true;};
  
  class GetWaveformsEvent;
  friend class GetWaveformsEvent;

  inline GetWaveformsEvent* getWaveformsEvent(){
    return new GetWaveformsEvent(*this);
  };

  /**
  * Internal class use to send information to the WaveformView to inform it that
  * the data requested have been collected.
  */
  class GetWaveformsEvent : public QCustomEvent{
   //Only the method getWaveformsEvent of WaveformThread has access to the private part of GetWaveformsEvent,
   //the constructor of GetWaveformsEvent being private, only this method con create a new GetWaveformsEvent
   friend GetWaveformsEvent* WaveformThread::getWaveformsEvent();

  public:
    inline WaveformThread* parentThread(){return &waveformThread;};
    inline ~GetWaveformsEvent(){};
    
  private:
    GetWaveformsEvent(WaveformThread& thread):QCustomEvent(QEvent::User + 200),waveformThread(thread){};

    WaveformThread& waveformThread;
  };

  class NoWaveformDataEvent;
  friend class NoWaveformDataEvent;

  inline NoWaveformDataEvent* noWaveformDataEvent(){
    return new NoWaveformDataEvent(*this);
  };

  /**
  * Internal class use to send information to the WaveformView to inform it that
  * there is not data available for the requested cluster. A reason being that the cluster has been suppress
  * after the thread has been launched.
  */
  class NoWaveformDataEvent : public QCustomEvent{
   //Only the method getWaveformsEvent of WaveformThread has access to the private part of GetWaveformsEvent,
   //the constructor of GetWaveformsEvent being private, only this method con create a new GetWaveformsEvent
   friend NoWaveformDataEvent* WaveformThread::noWaveformDataEvent();

  public:
    inline WaveformThread* parentThread(){return &waveformThread;};
    inline ~NoWaveformDataEvent(){};

  private:
    NoWaveformDataEvent(WaveformThread& thread):QCustomEvent(QEvent::User + 250),waveformThread(thread){};

    WaveformThread& waveformThread;
  };

protected:
    void run();
                                                        
private:
	  inline WaveformThread(WaveformView& view,Data& d):waveformView(view),meanRequested(false),data(d),haveToStopProcessing(false){};

    WaveformView& waveformView; 
    int clusterId;
    QValueList<int> clusterIds;
    bool treatSingleCluster;
    bool meanRequested;
    Data& data;
    WaveformView::PresentationMode mode;
    /**True if the thread has to stop processing, false otherwise.*/
    bool haveToStopProcessing;
};

#endif
