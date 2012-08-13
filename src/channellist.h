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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for QT
#include <qwidget.h>
#include <qlistbox.h>

class QDragEnterEvent;
class QDragDropEvent;

/**
  * The ChannelList widget provides a list of selectable, read-only items to be used
  * to configure the order in which individual electrodes are displayed in the WaveformView.
  *@author Lynn Hazan
  */

class ChannelList : public QListBox  {
   Q_OBJECT
public: 
	ChannelList(QWidget *parent=0, const char *name=0, WFlags f = 0);
	~ChannelList();

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void contentsDragMoveEvent(QDragMoveEvent* event);

signals:
  void positionsChanged();

private:
    int dragging;
};

#endif
