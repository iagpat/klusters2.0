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
#include <qdragobject.h> 

//include files for the application
#include "channellist.h"

//General C++ include files
#include <iostream>
using namespace std;


ChannelList::ChannelList(QWidget * parent, const char * name, WFlags f):QListBox( parent, name, f ){
  setVScrollBarMode(QScrollView::Auto);
  setDragAutoScroll(true);
  setSelectionMode(QListBox::Single);
  setAcceptDrops(TRUE);
  dragging = FALSE;
}

ChannelList::~ChannelList(){
}

void ChannelList::dragEnterEvent(QDragEnterEvent* event){
  if(QTextDrag::canDecode(event))
    event->accept();
}


void ChannelList::dropEvent(QDropEvent* event){
  QString text;

  if(QTextDrag::decode(event,text))
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
  QListBox::mousePressEvent(event);
  dragging = TRUE;
}


void ChannelList::mouseMoveEvent(QMouseEvent* event){  
 if(dragging && event->state() == Qt::LeftButton){
   QDragObject* drag = new QTextDrag(currentText(),this);
   drag->dragMove();
   dragging = FALSE;
 }
}

void ChannelList::contentsDragMoveEvent(QDragMoveEvent* event){
}
