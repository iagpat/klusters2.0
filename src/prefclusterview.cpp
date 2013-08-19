/***************************************************************************
                          prefclusterview.cpp  -  description
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

#include "prefclusterview.h"


PrefClusterView::PrefClusterView(QWidget *parent) : PrefClusterViewLayout(parent) {
}
PrefClusterView::~PrefClusterView(){
}

void PrefClusterView::setTimeInterval(int time){
    intervalSpinBox->setValue(time);
}

int PrefClusterView::getTimeInterval() const{
    return intervalSpinBox->value();
}
