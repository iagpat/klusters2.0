/***************************************************************************
 *   Copyright (C) 2007 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include <QDialog>

#include <QLineEdit>

/**
    @author Lynn Hazan <lynn.hazan@myrealbox.com>
    This class is the dialog used to gather cluster information.
*/
class ClusterInformationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClusterInformationDialog(QWidget *parent = 0,const QString& caption = tr("Cluster information"));
    ~ClusterInformationDialog();

     const QString getStructure() const { return structure->text(); }
     const QString getType()  const { return type->text(); }
     const QString getId() const { return id->text(); }
     const QString getQuality()  const { return quality->text(); }
     const QString getNotes() const { return notes->text(); }

     void setStructure(const QString& pStructure) { structure->setText(pStructure); }
     void setType(const QString& pType) { type->setText(pType); }
     void setId(const QString& pID) { id->setText(pID); }
     void setQuality(const QString& pQuality) { quality->setText(pQuality); }
     void setNotes(const QString& pNotes) { notes->setText(pNotes); }

private:
    QLineEdit		*structure;
    QLineEdit		*type;
    /**Isolation Distance*/
    QLineEdit		*id;
    QLineEdit		*quality;
    QLineEdit		*notes;

};

#endif
