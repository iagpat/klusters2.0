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
// application specific includes
#include "clusterinformationdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>



ClusterInformationDialog::ClusterInformationDialog(QWidget *parent,const QString& caption)
    : QDialog(parent)
{
    setWindowTitle(caption);
    setModal(true);
    QWidget *page = new QWidget(this);

    QVBoxLayout*  layout = new QVBoxLayout(page);
    layout->setMargin(2);
    setLayout(layout);
    //Structure information (label and lineedit)
    QLabel *label1 = new QLabel(tr("Structure"),page);
    layout->addWidget(label1);

    structure = new QLineEdit(page);
    structure->setMinimumWidth(fontMetrics().maxWidth()*20);
    structure->setFocus();
    layout->addWidget(structure);

    //Type information (label and lineedit)
    QLabel *label2 = new QLabel(tr("Type"),page);
    layout->addWidget(label2);

    type = new QLineEdit(page);
    type->setMinimumWidth(fontMetrics().maxWidth()*20);
    layout->addWidget(type);

    //Id information (label and lineedit)
    QLabel *label3 = new QLabel(tr("Isolation distance"),page);
    layout->addWidget(label3);

    id = new QLineEdit(page);
    id->setObjectName("id");
    id->setMinimumWidth(fontMetrics().maxWidth()*20);
    layout->addWidget(id);

    //Quality information (label and lineedit)
    QLabel *label4 = new QLabel(tr("Quality"),page);
    layout->addWidget(label4);

    quality = new QLineEdit(page);
    quality->setMinimumWidth(fontMetrics().maxWidth()*20);
    layout->addWidget(quality);

    //Type information (label and lineedit)
    QLabel *label5 = new QLabel(tr("Notes"),page);
    layout->addWidget(label5);

    notes = new QLineEdit(page);
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

