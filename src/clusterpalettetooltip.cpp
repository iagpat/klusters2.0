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
#include "clusterpalettetooltip.h"
#include "clusterpaletteiconviewitem.h"

ClusterPaletteToolTip::ClusterPaletteToolTip(QIconView* parentIconView, QToolTipGroup* toolTipGroup):QToolTip(parentIconView->viewport(), toolTipGroup), parent(parentIconView)
{
}

ClusterPaletteToolTip::~ClusterPaletteToolTip(void)
{
}

void ClusterPaletteToolTip::maybeTip(const QPoint& point){
	{
		ClusterPaletteIconViewItem* item;
		QPoint absoluteCoordinates;

        // get the absolute item coordinates
		absoluteCoordinates.setX(point.x()+parent->contentsX());
		absoluteCoordinates.setY(point.y()+parent->contentsY());
		item=dynamic_cast<ClusterPaletteIconViewItem*>(parent->findItem(absoluteCoordinates));
		if (item)
		{
			QRect coordonneesCoordinates;

                // use portview-relative coords
			coordonneesCoordinates.setX(point.x());
			coordonneesCoordinates.setY(point.y());
			coordonneesCoordinates.setWidth(item->rect().width());
			coordonneesCoordinates.setHeight(item->rect().height());
			tip(coordonneesCoordinates, item->getToolTipText());
		}
	}
}



