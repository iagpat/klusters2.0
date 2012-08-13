/***************************************************************************
 *          processlinemaker.cpp  -  description
 *            -------------------
 *
 *   Copyright (C) 2002 John Firebaugh <jfirebaugh@kde.org                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application
#include "processlinemaker.h"
#include <kprocess.h>

ProcessLineMaker::ProcessLineMaker():counterOut(0),counterErr(0),isProcessKilled(false),isWidgetHidden(false),processExited(false){}

ProcessLineMaker::ProcessLineMaker( const KProcess* proc ):counterOut(0),counterErr(0),isProcessKilled(false),
isWidgetHidden(false),processExited(false)
{
    connect(proc, SIGNAL(receivedStdout(KProcess*,char*,int)),
            this, SLOT(slotReceivedStdout(KProcess*,char*,int)) );
    
    connect(proc, SIGNAL(receivedStderr(KProcess*,char*,int)),
            this, SLOT(slotReceivedStderr(KProcess*,char*,int)) );
}

void ProcessLineMaker::slotReceivedStdout( const QString& s )
{
    counterOut++;
    
    // Flush stderr buffer
    if (!stderrbuf.isEmpty()) {
        emit receivedStderrLine(stderrbuf);
        stderrbuf = "";
    }
    stdoutbuf += s;
    int pos;
    while ( (pos = stdoutbuf.find('\n')) != -1) {
        lineOut = stdoutbuf.left(pos);
        stdoutbuf.remove(0, pos+1);
        emit receivedStdoutLine(lineOut); 
    }

    // Do not remove this line! It makes the method thread safe.
    //  Because we can be interrupted whenever more data is available,
    //  this prevents printing the same data twice.
    lineOut = "";

    counterOut--;

    //The process has benn killed and all the process'outputs sent to be print,
    //warn the processWidget.
    if(counterOut == 0 && (isProcessKilled || isWidgetHidden || processExited)) emit outputTreatmentOver(); 
}


void ProcessLineMaker::slotReceivedStderr( const QString& s )
{
    counterErr++;
  
    // Flush stdout buffer
    if (!stdoutbuf.isEmpty()) {
        emit receivedStdoutLine(stdoutbuf);
        stdoutbuf = "";
    }
    
    stderrbuf += s;
    int pos;
    while ( (pos = stderrbuf.find('\n')) != -1) {
        lineErr = stderrbuf.left(pos);
        stderrbuf.remove(0, pos+1);        
        emit receivedStderrLine(lineErr);
    }
    
    // Do not remove this line! It makes the method thread safe.
    //  Because we can be interrupted whenever more data is available,
    //  this prevents printing the same data twice.
    lineErr = "";

    counterErr--;
    
    //The process has benn killed and all the process'outputs sent to be print,
    //warn the processWidget.
    if(counterErr == 0 && (isProcessKilled || isWidgetHidden)) emit outputTreatmentOver();     
}

