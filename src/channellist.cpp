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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application
#include "channellist.h"



ChannelList::ChannelList(QWidget * parent)
    :QListWidget( parent )
{
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::SingleSelection);
    connect( model(),
             SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
             SIGNAL(positionsChanged()) );

}

ChannelList::~ChannelList(){
}


