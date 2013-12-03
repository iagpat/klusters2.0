/***************************************************************************
                          prefclusterview.h  -  description
                             -------------------
    begin                : Thu Dec 11 2003
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

#ifndef PREFCLUSTERVIEW_H
#define PREFCLUSTERVIEW_H

// include files for QT
#include <QWidget>
#include <QSpinBox>

//include files for the application
#include <prefclusterviewlayout.h>


/**
  * Class representing the Cluster View configuration page of the Klusters preferences dialog.
  *@author Lynn Hazan
  */

class PrefClusterView : public PrefClusterViewLayout  {
    Q_OBJECT
public: 
    explicit PrefClusterView(QWidget *parent=0);
    ~PrefClusterView();

    /**Sets the time interval between 2 lines drawn in the cluster views
  * when the time dimension in selected. The time @p time is in second.*/
    void setTimeInterval(int time);

    /**Returns the time interval between 2 lines drawn in the cluster views
  * when the time dimension in selected. The time is in second.*/
    int getTimeInterval() const;

};

#endif
