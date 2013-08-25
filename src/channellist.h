/***************************************************************************
                          channellist.h  -  description
                             -------------------
    begin                : Mon Dec 15 2003
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

#ifndef CHANNELLIST_H
#define CHANNELLIST_H


// include files for QT
#include <QListWidget>

/**
  * The ChannelList widget provides a list of selectable, read-only items to be used
  * to configure the order in which individual electrodes are displayed in the WaveformView.
  *@author Lynn Hazan
  */

class ChannelList : public QListWidget  {
    Q_OBJECT
public: 
    explicit ChannelList(QWidget *parent=0);
    ~ChannelList();

Q_SIGNALS:
    void positionsChanged();

};

#endif
