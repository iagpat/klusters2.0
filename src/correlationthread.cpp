/***************************************************************************
                          correlationthread.cpp  -  description
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
//include files for the application
#include "correlationthread.h"

//QT include files
#include <qapplication.h>

#include <QList>

#include <math.h>
#include <stdlib.h>

void CorrelationThread::run(){
    if(!haveToStopProcessing){
        //Convert the miliseconds in recording units.
        double binSizeInRU = static_cast<double>((static_cast<double>(correlationView.binSize) * 1000.0) / data.samplingInterval);
        double timeWindowInRU = static_cast<double>((static_cast<double>(correlationView.timeWindow) * 1000.0) / data.samplingInterval);

        //Calculate the number of bins to compute - so there are a total of nBins = 1+2*halfBins bins
        //(halfBins + 1/2 for each half time window)
        int halfBins = ((correlationView.timeWindow / correlationView.binSize) - 1) / 2;

        QList<Pair>::iterator pairIterator;
        for(pairIterator = clusterPairs->begin(); pairIterator != clusterPairs->end(); ++pairIterator){
            if(!haveToStopProcessing){
                Data::Status status = data.getCorrelograms(*pairIterator,correlationView.binSize,correlationView.timeWindow,binSizeInRU,timeWindowInRU,halfBins);
                if(status == Data::NOT_AVAILABLE)
                    continue;
                else if(status == Data::IN_PROCESS) {
                    while(!haveToStopProcessing && (data.getCorrelograms(*pairIterator,correlationView.binSize,correlationView.timeWindow,binSizeInRU,timeWindowInRU,halfBins) == Data::IN_PROCESS))
                    {
                        sleep(1);
                    }
                }
            }
        }
    }

    //Send an event to the CorrelationView to let it know that the data requested are available.
    CorrelationsEvent* event = getCorrelationsEvent();
    QApplication::postEvent(&correlationView,event);

    delete clusterPairs;
}
