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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MINMAXTHREAD_H
#define MINMAXTHREAD_H

//include files for the application
#include "data.h"

//include files for QT
#include <qthread.h>

/**Thread used to compute the minimum and maximum values for each dimension (feature).
 * The thread calls the data object which will do the work.
 *@author Lynn Hazan
 */

class MinMaxThread : public QThread  {
public:
  //Only the method minmaxCalculator of Data has access to the private part of MinMaxThread,
  //the constructor of MinMaxThread being private, only this method con create a new MinMaxThread
  friend MinMaxThread* Data::minMaxCalculator();

	inline ~MinMaxThread(){cout<<"in ~MinMaxThread"<<endl;};

  inline void run(){data.minMaxDimensionCalculation(modifiedClusters);};
  inline void setModifiedClusters(QValueList<int> clusters){modifiedClusters = clusters;};
  
private:
    Data& data;
	  inline MinMaxThread(Data& d):data(d){};
    QValueList<int> modifiedClusters;  
};


#endif
