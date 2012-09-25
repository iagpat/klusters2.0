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


ClusterPaletteIconViewItem::ClusterPaletteIconViewItem(Q3IconView* parent, QString label)
    :Q3IconViewItem(parent, label)
{
}

ClusterPaletteIconViewItem::ClusterPaletteIconViewItem(Q3IconView* parent, QString label, const QPixmap & icon ):Q3IconViewItem(parent, label,icon)
{
}

ClusterPaletteIconViewItem::~ClusterPaletteIconViewItem()
{
}

void ClusterPaletteIconViewItem::setToolTipText(const QString &toolTipText)
{
#if KDAB_PENDING	
    this->toolTipText=toolTipText;
    if (!toolTip){
        toolTip=new ClusterPaletteToolTip(static_cast<Q3IconView*>(iconView()));
    }
#endif	
}



