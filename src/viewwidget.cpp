/***************************************************************************
                          viewwidget.cpp  -  description
                             -------------------
    begin                : Thu Sep 25 2003
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

#include "viewwidget.h"

//include files for the application
#include "klustersview.h"

ViewWidget::ViewWidget(KlustersDoc& doc,KlustersView& view,QColor backgroundColor,KStatusBar* statusBar,QWidget* parent, const char* name,
int minSize,int maxSize ,int windowTopLeft ,int windowBottomRight,int border,int Xborder,int Yborder):
BaseFrame(Xborder,Yborder,parent,name,backgroundColor,minSize,maxSize,windowTopLeft,windowBottomRight,border),
doublebuffer(),view(view),doc(doc),statusBar(statusBar){
}

ViewWidget::~ViewWidget(){
  cout << "in ~ViewWidget(): "<<endl;
}





