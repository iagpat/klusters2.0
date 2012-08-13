/***************************************************************************
 *          processlinemaker.h  -  description
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

/**********************************************
* Modified by Lynn Hazan,lynn.hazan@myrealbox.com, (2004)
*
***********************************************/
 
#ifndef _PROCESSLINEMAKER_H_
#define _PROCESSLINEMAKER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <qstring.h>
#include <qobject.h>


//General C++ include files
#include <iostream>
using namespace std;

// forward declaration 
class KProcess;

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
	ProcessLineMaker(const KProcess*);
  inline void processKilled(){isProcessKilled = true;};
  inline void reset(){
    counterOut = 0;
    counterErr = 0;
    isProcessKilled = false;
    isWidgetHidden = false;
    processExited = false;   
  };

public slots:
	void slotReceivedStdout(const QString& s);
	void slotReceivedStderr(const QString& s);
  inline void slotWidgetHidden(){isWidgetHidden = true;};

  inline void slotProcessExited(){
   processExited = true;
   //In case the outputs from the process are already finished.
   if(counterOut == 0 && counterErr == 0) emit outputTreatmentOver();
  };
  
protected slots:
	inline void slotReceivedStdout(KProcess* process, char* buffer,int buflen){
    slotReceivedStdout(QString::fromLocal8Bit(buffer,buflen));
  };
	inline void slotReceivedStderr(KProcess* process,char* buffer, int buflen){
   slotReceivedStderr(QString::fromLocal8Bit(buffer,buflen));
  };
        
signals:
	void receivedStdoutLine( const QString& line );
	void receivedStderrLine( const QString& line );
  void outputTreatmentOver();
        
private:
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
