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
// application specific includes
#include "clusterinformationdialog.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3VBoxLayout>
#include <QDialogButtonBox>

ClusterInformationDialog::ClusterInformationDialog(QWidget *parent,const QString& caption) :
        QDialog(parent)
{
    setWindowTitle(caption);
    setModal(true);
	page = new QWidget(this);
	//KDAB_PENDING: add spacingHint
	layout = new Q3VBoxLayout(page,0/*,spacingHint()*/);

	//Structure information (label and lineedit)
    QLabel *label1 = new QLabel(tr("Structure"),page,"structure_label");
	layout->addWidget(label1);

	structure = new QLineEdit(page,"structure");
	structure->setMinimumWidth(fontMetrics().maxWidth()*20);
	structure->setFocus();
	layout->addWidget(structure);

	//Type information (label and lineedit)
    QLabel *label2 = new QLabel(tr("Type"),page,"type_label");
	layout->addWidget(label2);

	type = new QLineEdit(page,"type");
	type->setMinimumWidth(fontMetrics().maxWidth()*20);
	layout->addWidget(type);
	
	//Id information (label and lineedit)
    QLabel *label3 = new QLabel(tr("Isolation distance"),page,"id_label");
	layout->addWidget(label3);

	id = new QLineEdit(page,"id");
	id->setMinimumWidth(fontMetrics().maxWidth()*20);
	layout->addWidget(id);
	
	//Quality information (label and lineedit)
    QLabel *label4 = new QLabel(tr("Quality"),page,"quality_label");
	layout->addWidget(label4);

	quality = new QLineEdit(page,"quality");
	quality->setMinimumWidth(fontMetrics().maxWidth()*20);
	layout->addWidget(quality);
	
	//Type information (label and lineedit)
    QLabel *label5 = new QLabel(tr("Notes"),page,"notes_label");
	layout->addWidget(label5);

	notes = new QLineEdit(page,"notes");
	notes->setMinimumWidth(fontMetrics().maxWidth()*20);
	layout->addWidget(notes);

	layout->addStretch(10);

    QDialogButtonBox*buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

ClusterInformationDialog::~ClusterInformationDialog()
{
}


#include "clusterinformationdialog.moc"
