/***************************************************************************
                          channellist.cpp  -  description
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

// include files for QT
#include <q3dragobject.h> 
//Added by qt3to4:
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragEnterEvent>

//include files for the application
#include "channellist.h"

//General C++ include files
#include <iostream>
using namespace std;


ChannelList::ChannelList(QWidget * parent, const char * name, WFlags f):Q3ListBox( parent, name, f ){
  setVScrollBarMode(Q3ScrollView::Auto);
  setDragAutoScroll(true);
  setSelectionMode(Q3ListBox::Single);
  setAcceptDrops(TRUE);
  dragging = FALSE;
}

ChannelList::~ChannelList(){
}

void ChannelList::dragEnterEvent(QDragEnterEvent* event){
  if(Q3TextDrag::canDecode(event))
    event->accept();
}


void ChannelList::dropEvent(QDropEvent* event){
  QString text;

  if(Q3TextDrag::decode(event,text))
   if(event->source() == this && event->action() == QDropEvent::Copy){
    // Careful not to tread on my own feet
    event->acceptAction();
    //QString text = currentText();
    removeItem(currentItem());
    insertItem(text,index(itemAt(event->pos())));
    //signal to the parent widget that channel positions have changed so it can update its internal variables.
    emit positionsChanged();
   }    
}


void ChannelList::mousePressEvent(QMouseEvent* event){
  Q3ListBox::mousePressEvent(event);
  dragging = TRUE;
}


void ChannelList::mouseMoveEvent(QMouseEvent* event){  
 if(dragging && event->state() == Qt::LeftButton){
   Q3DragObject* drag = new Q3TextDrag(currentText(),this);
   drag->dragMove();
   dragging = FALSE;
 }
}

void ChannelList::contentsDragMoveEvent(QDragMoveEvent* event){
}

#include "channellist.moc"
