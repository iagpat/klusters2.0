/***************************************************************************
                          errormatrixthread.cpp  -  description
                             -------------------
    begin                : Mon Jan 12 2004
    copyright            : (C) 2004 by Lynn Hazan
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
//include files for the application
#include "errormatrixthread.h"

//QT include files
#include <qapplication.h>

void ErrorMatrixThread::run(){
    if(!haveToStopProcessing) probabilities =
            assistant.computeMeanProbabilities(data,clusterList,computedClusterList,ignoreClusterIndex);

    //Send an event to the ErrorMatrixView to let it know that the computation is finish.
    ErrorMatrixEvent* event = getErrorMatrixEvent();
    QApplication::postEvent(&errorMatrixView,event);
}

