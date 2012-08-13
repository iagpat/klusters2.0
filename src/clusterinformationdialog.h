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
#ifndef CLUSTERINFORMATIONDIALOG_H
#define CLUSTERINFORMATIONDIALOG_H

//QT includes
#include "qlayout.h"
#include "qlabel.h"
#include "qlineedit.h"

//KDE includes
#include <kdialogbase.h>

/**
	@author Lynn Hazan <lynn.hazan@myrealbox.com>
	This class is the dialog used to gather cluster information.
*/
class ClusterInformationDialog : public KDialogBase
{
Q_OBJECT
public:
	ClusterInformationDialog(QWidget *parent = 0,const QString& caption = "Cluster information");
	virtual ~ClusterInformationDialog();
	
	inline const QString getStructure() { return structure->text(); };
	inline const QString getType() { return type->text(); };
	inline const QString getId() { return id->text(); };
	inline const QString getQuality() { return quality->text(); };
	inline const QString getNotes() { return notes->text(); };

	inline void setStructure(QString pStructure) { structure->setText(pStructure); };
	inline void setType(QString pType) { type->setText(pType); };
	inline void setId(QString pID) { id->setText(pID); };
	inline void setQuality(QString pQuality) { quality->setText(pQuality); };
	inline void setNotes(QString pNotes) { notes->setText(pNotes); };
			
	private:
		QWidget			*page;
		QLineEdit		*structure;
		QLineEdit		*type;
		/**Isolation Distance*/
		QLineEdit		*id;
		QLineEdit		*quality;
		QLineEdit		*notes;						
		QVBoxLayout		*layout;

};

#endif
