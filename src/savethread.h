/***************************************************************************
                          savethread.h  -  description
                             -------------------
    begin                : Fri Oct 17 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SAVETHREAD_H
#define SAVETHREAD_H

//include files for the application
#include "klustersdoc.h"
#include "klusters.h"

//include files for QT
#include <qthread.h>

//include files for KDE
#include <kio/netaccess.h>

/**Thread used to save the cluster file.
 * The thread calls the Data object which will do the work.
 *@author Lynn Hazan
 */

class SaveThread : public QThread{
  
public: 

  inline SaveThread(KlustersApp* parent):doc(0L),parent(parent),isSaveAs(false){};
  inline ~SaveThread(){cout<<"in ~SaveThread"<<endl;};

  
  virtual void run();

  /**
  * Begins the execution of the thread which will save the cluster information.
  * @param url the url where to store the cluster information
  * @param doc document corresponding to the data, contains the algorithm to actually save the cluster information
  * @param isSaveAs true if the current save action is a saveAs action
  * (meaning the file to save is different from the one used to load the data), false otherwise. The default is false.
  */
  void save(const KURL& url,KlustersDoc* doc,bool isSaveAs=false);
  
  class SaveDoneEvent;
  friend class SaveDoneEvent;

  inline SaveDoneEvent* saveDoneEvent(bool status){
    return new SaveDoneEvent(*this,status);
  };

  /**
  * Internal class use to send information to the application object (KlustersApp) concerning
  * the save process.
  *
  */
  class SaveDoneEvent : public QCustomEvent{
   //Only the method saveDoneEvent of SaveThread has access to the private part of SaveDoneEvent,
   //the constructor of saveDoneEvent being private, only this method con create a new saveDoneEvent
   friend SaveDoneEvent* SaveThread::saveDoneEvent(bool status);

  public:
    inline bool isItSaveAs(){return saveThread.isSaveAs;};
    inline void setTemporaryFile(QString tmpFile){tempCluFile = tmpFile;};
    inline QString temporaryFile(){return tempCluFile;};
    inline bool isSaveOk(){return saveOk;};
    inline ~SaveDoneEvent(){};
    
  private:
    SaveDoneEvent(const SaveThread& thread,bool status):QCustomEvent(QEvent::User + 100),saveThread(thread),saveOk(status){};
    
    const SaveThread& saveThread;
    QString tempCluFile;
    bool saveOk;
  };
  
private:
  KlustersDoc* doc;
  KlustersApp* parent;
  KURL url;
  bool isSaveAs;
};

#endif
