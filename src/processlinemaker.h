/***************************************************************************
 *          processlinemaker.h  -  description
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

/**********************************************
* Modified by Lynn Hazan,lynn.hazan@myrealbox.com, (2004)
*
***********************************************/

#ifndef _PROCESSLINEMAKER_H_
#define _PROCESSLINEMAKER_H_

// include files for Qt
#include <QString>
#include <qobject.h>



// forward declaration 
class QProcess;

/**
 * This class receives the outputs of external processes launched by ProcessWidget.
 * It converts them in QString and forwards them to ProcessWidget to be displayed.
 *@author John Firebaugh, Lynn Hazan
 * @since klusters 1.2
 */

class ProcessLineMaker : public QObject
{
    Q_OBJECT

public:
    ProcessLineMaker();
    ProcessLineMaker(QProcess*);
    void processKilled(){isProcessKilled = true;}
    void reset(){
        counterOut = 0;
        counterErr = 0;
        isProcessKilled = false;
        isWidgetHidden = false;
        processExited = false;
    }

public Q_SLOTS:
    void slotReceivedStdout();
    void slotReceivedStderr();
    void slotWidgetHidden(){isWidgetHidden = true;}

    void slotProcessExited(){
        processExited = true;
        //In case the outputs from the process are already finished.
        if(counterOut == 0 && counterErr == 0)
            emit outputTreatmentOver();
    }


Q_SIGNALS:
    void receivedStdoutLine( const QString& line );
    void receivedStderrLine( const QString& line );
    void outputTreatmentOver();

private:
    QProcess *mProc;
    QString stdoutbuf;
    QString stderrbuf;
    QString lineOut;
    QString lineErr;
    int counterOut;
    int counterErr;
    bool isProcessKilled;
    bool isWidgetHidden;
    bool processExited;
};

#endif
