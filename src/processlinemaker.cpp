/***************************************************************************
 *          processlinemaker.cpp  -  description
 *            -------------------
 *
 *   Copyright (C) 2002 John Firebaugh <jfirebaugh@kde.org                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application
#include "processlinemaker.h"
#include <QProcess>

ProcessLineMaker::ProcessLineMaker()
    : mProc(0),
      counterOut(0),
      counterErr(0),
      isProcessKilled(false),
      isWidgetHidden(false),
      processExited(false)
{
}

ProcessLineMaker::ProcessLineMaker( QProcess* proc )
    : mProc(proc),
      counterOut(0),
      counterErr(0),
      isProcessKilled(false),
      isWidgetHidden(false),
      processExited(false)
{
    connect(mProc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(slotReceivedStdout()) );
    
    connect(mProc, SIGNAL(readyReadStandardError()),
            this, SLOT(slotReceivedStderr()) );
}

void ProcessLineMaker::slotReceivedStdout()
{
    QString s = QString::fromLocal8Bit(mProc->readAllStandardOutput());

    counterOut++;
    
    // Flush stderr buffer
    if (!stderrbuf.isEmpty()) {
        emit receivedStderrLine(stderrbuf);
        stderrbuf.clear();
    }
    stdoutbuf += s;
    int pos;
    while ( (pos = stdoutbuf.indexOf('\n')) != -1) {
        lineOut = stdoutbuf.left(pos);
        stdoutbuf.remove(0, pos+1);
        emit receivedStdoutLine(lineOut);
    }

    // Do not remove this line! It makes the method thread safe.
    //  Because we can be interrupted whenever more data is available,
    //  this prevents printing the same data twice.
    lineOut.clear();

    counterOut--;

    //The process has benn killed and all the process'outputs sent to be print,
    //warn the processWidget.
    if(counterOut == 0 && (isProcessKilled || isWidgetHidden || processExited))
        emit outputTreatmentOver();
}


void ProcessLineMaker::slotReceivedStderr()
{
    const QString s = QString::fromLocal8Bit(mProc->readAllStandardError());
    counterErr++;

    // Flush stdout buffer
    if (!stdoutbuf.isEmpty()) {
        emit receivedStdoutLine(stdoutbuf);
        stdoutbuf.clear();
    }
    
    stderrbuf += s;
    int pos;
    while ( (pos = stderrbuf.indexOf('\n')) != -1) {
        lineErr = stderrbuf.left(pos);
        stderrbuf.remove(0, pos+1);
        emit receivedStderrLine(lineErr);
    }
    
    // Do not remove this line! It makes the method thread safe.
    //  Because we can be interrupted whenever more data is available,
    //  this prevents printing the same data twice.
    lineErr.clear();

    counterErr--;
    
    //The process has benn killed and all the process'outputs sent to be print,
    //warn the processWidget.
    if(counterErr == 0 && (isProcessKilled || isWidgetHidden))
        emit outputTreatmentOver();
}

