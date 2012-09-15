/***************************************************************************
                          channelcolors.cpp  -  description
                             -------------------
    begin                : Tue Sep 16 2004
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
// application specific includes
#include "channelcolors.h"

//Added by qt3to4:
#include <QList>
#include <QDebug>



ChannelColors::ChannelColors():channelList(){
}

ChannelColors::~ChannelColors(){
    qDebug() << "~ChannelColors()";
    qDeleteAll(channelList);
    channelList.clear();
}


ChannelColors::ChannelColors(const ChannelColors& origin){
    //Insert into channelList a deep copy of all the elements of origin.channelList
    QList<ChannelColor*> originChannelList =  origin.channelList;
    for (int i = 0; i < originChannelList.size(); ++i) {
        channelList.append(new ChannelColor(*originChannelList.at(0)));
    }
}

QColor ChannelColors::color(int identifier){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);

    //In case no ChannelColor have been find (should not happen), return black.
    if(theChannelColor == NULL)
        return QColor(Qt::black);
    else
        return theChannelColor->color;
}


QColor ChannelColors::groupColor(int identifier){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);

    //In case no ChannelColor have been find (should not happen), return black.
    if(theChannelColor == NULL) return QColor(Qt::black);
    else return theChannelColor->groupColor;
}

QColor ChannelColors::spikeGroupColor(int identifier){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);

    //In case no ChannelColor have been find (should not happen), return black.
    if(theChannelColor == NULL) return QColor(Qt::black);
    else return theChannelColor->spikeGroupColor;
}

void ChannelColors::setColor(int identifier, const QColor& color){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);
    theChannelColor->color = color;
}

void ChannelColors::setGroupColor(int identifier, const QColor &color){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);
    theChannelColor->groupColor = color;
}

void ChannelColors::setSpikeGroupColor(int identifier,QColor color){
    ChannelColors::ChannelColor* theChannelColor = 0L;
    theChannelColor = channelColor(identifier);
    theChannelColor->spikeGroupColor = color;
}

int ChannelColors::channelId(int index){
    return (channelList.at(static_cast<uint>(index)))->channelId;
}

bool ChannelColors::contains(int channelId){
    if(channelColorIndex(channelId) == -1) return false;
    else return true;
}


uint ChannelColors::append(int channelId, const QColor& color,const QColor& groupColor,const QColor& spikeGroupColor){
    channelList.append(new ChannelColor(channelId,color,groupColor,spikeGroupColor));
    return channelList.count();
}

uint ChannelColors::append(int channelId, const QColor& color){
    channelList.append(new ChannelColor(channelId,color,color,color));
    return channelList.count();
}

void ChannelColors::insert(int channelId,int index,const QColor& color,const QColor& groupColor,const QColor& spikeGroupColor){
    channelList.insert(index, new ChannelColor(channelId,color,groupColor,spikeGroupColor));
}

bool ChannelColors::remove(int identifier){
    const int index = channelColorIndex(identifier);
    if(index == -1)
        return false;
    channelList.removeAt(index);
    return true;
}

ChannelColors::ChannelColor* ChannelColors::channelColor(int channelId) const{

    for (int i = 0; i < channelList.size(); ++i) {
        if (channelList.at(i)->channelId == channelId)
            return channelList.at(i);
    }
    return 0;
}

int ChannelColors::channelColorIndex(int channelId) const{
    for (int i = 0; i < channelList.size(); ++i) {
        if (channelList.at(i)->channelId == channelId)
            return i;
    }
    return -1;//Normally never reach
}



