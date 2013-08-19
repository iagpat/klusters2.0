/***************************************************************************
                          autosavethread.h  -  description
                             -------------------
    begin                : Thu Dec 4 2003
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

#ifndef AUTOSAVETHREAD_H
#define AUTOSAVETHREAD_H

//include files for the application
#include "data.h"
#include "klustersdoc.h"

//include files for QT
#include <qthread.h>

#include <QEvent>
#include <QDebug>



/**Thread used to save automatically the document on a defined schedule.
 * The thread calls the Data object which will do the work.
 *@author Lynn Hazan
 */

class AutoSaveThread : public QThread {
public:

    ~AutoSaveThread(){qDebug()<<"in ~AutoSaveThread";}
    //The default saving interval is 5 minutes.
    AutoSaveThread(Data& d, KlustersDoc* doc,const QString &saveTmpUrl):data(d),doc(doc),autoSaveUrl(saveTmpUrl){
        qDebug()<<"in constructor AutoSaveThread";
    }

    void removeTmpFile(){
        //delete the temporary files
        QFile::remove(autoSaveUrl);
    }
    void run();

    class AutoSaveEvent;
    friend class AutoSaveEvent;

    AutoSaveEvent* autoSaveEvent(){
        return new AutoSaveEvent();
    }

    /**
  * Internal class use to inform the document object (KlustersDoc) that the thread has finished.
  *
  */
    class AutoSaveEvent : public QEvent{
        //Only the method autoSaveEvent of AutoSaveThread has access to the private part of AutoSaveEvent,
        //the constructor of AutoSaveEvent being private, only this method con create a new AutoSaveEvent
        friend AutoSaveEvent* AutoSaveThread::autoSaveEvent();

    public:
        ~AutoSaveEvent(){}
        bool isIOerror(){return IOerror;}
        void setIOerror(bool status){IOerror = status;}

    private:
        AutoSaveEvent():QEvent(QEvent::Type(QEvent::User + 500)){IOerror = false;}
        bool IOerror;
    };

private:
    Data& data;
    KlustersDoc* doc;
    QString autoSaveUrl;
};

#endif
