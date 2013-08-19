/***************************************************************************
                          minmaxthread.h  -  description
                             -------------------
    begin                : Wed Oct 22 2003
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

//include files for the application
#include "minmaxthread.h"


void MinMaxThread::run()
{
    data.minMaxDimensionCalculation(modifiedClusters);
}

void MinMaxThread::setModifiedClusters(const QList<int>& clusters)
{
    modifiedClusters = clusters;
}

