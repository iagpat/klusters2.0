/***************************************************************************
 *   Copyright (C) 2007 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//include files for the application
#include "clusterpaletteiconviewitem.h"

//includes pour QT
#include <qpixmap.h>

//General C++ include files
#include <iostream>
using namespace std;

ClusterPaletteIconViewItem::ClusterPaletteIconViewItem(QIconView* parent, QString label)
	:QIconViewItem(parent, label)
{
	toolTipText="";
	toolTip=NULL;
}

ClusterPaletteIconViewItem::ClusterPaletteIconViewItem(QIconView* parent, QString label, const QPixmap & icon ):QIconViewItem(parent, label,icon)
{
	toolTipText="";
	toolTip=NULL;
}

ClusterPaletteIconViewItem::~ClusterPaletteIconViewItem()
{
}

void ClusterPaletteIconViewItem::setToolTipText(const QString toolTipText)
{
	this->toolTipText=toolTipText;
	if (!toolTip){
		toolTip=new ClusterPaletteToolTip(static_cast<QIconView*>(iconView()));
	}
}



