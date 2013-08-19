/***************************************************************************
                          autosavethread.cpp  -  description
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

#include "autosavethread.h"

//QT include files
#include <qapplication.h>

//C, C++ include files
//#define _LARGEFILE_SOURCE already defined in /usr/include/features.h
#define _FILE_OFFSET_BITS 64
#include <stdio.h>

void AutoSaveThread::run(){
    //Open the temp file in write mode
    FILE* cluFile = fopen(autoSaveUrl.toLatin1(),"w");

    //If can be open, save the data.
    if(cluFile != NULL){
        data.saveClusters(cluFile);
        //close the file
        fclose(cluFile);

        //Send an event to the document to warn it that the thread has finish.
        AutoSaveEvent* event = autoSaveEvent();
        QApplication::postEvent(doc,event);
    } else {
        //Send an event to the document to warn it that the thread could not save the file.
        AutoSaveEvent* event = autoSaveEvent();
        event->setIOerror(true);
        QApplication::postEvent(doc,event);
    }
}

