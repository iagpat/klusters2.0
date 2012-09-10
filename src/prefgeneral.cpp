/***************************************************************************
                          prefgeneral.cpp  -  description
                             -------------------
    begin                : Thu Dec 11 2003
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
//Application specific includes.
#include "prefgeneral.h"

//QT includes
#include <qicon.h>
#include <QFileDialog>
// include files for KDE


PrefGeneral::PrefGeneral(QWidget *parent, const char *name ) : PrefGeneralLayout(parent) {
    connect(crashRecoveryCheckBox,SIGNAL(stateChanged(int)),this,SLOT(updateCrashRecoveryTimeInterval(int)));
    connect(reclusteringExecutableButton,SIGNAL(clicked()),this,SLOT(updateReclusteringExecutable()));

    //Set an icon on the reclusteringExecutableButton button

    reclusteringExecutableButton->setIconSet(QIcon(":/icons/fileopen"));
}
PrefGeneral::~PrefGeneral(){
}

void PrefGeneral::setCrashRecovery(bool use){
    crashRecoveryCheckBox->setChecked(use);
    if(use)
        updateCrashRecoveryTimeInterval(QCheckBox::On);
    else
        updateCrashRecoveryTimeInterval(QCheckBox::Off);
}

void PrefGeneral::setCrashRecoveryIndex(int index){crashRecoveryComboBox->setCurrentItem(index);}

void PrefGeneral::setNbUndo(int nb){undoSpinBox->setValue(nb);}

void PrefGeneral::setBackgroundColor(const QColor& color) {
    backgroundColorButton->setColor(color);
}

void PrefGeneral::setReclusteringExecutable(const QString& executable) {reclusteringExecutableLineEdit->setText(executable);}

void PrefGeneral::setReclusteringArguments(const QString& arguments) {reclusteringArgsLineEdit->setText(arguments);}

bool PrefGeneral::isCrashRecovery() const{return crashRecoveryCheckBox->isChecked();}

int PrefGeneral::crashRecoveryIntervalIndex() const{return crashRecoveryComboBox->currentItem();}

int PrefGeneral::getNbUndo() const{return undoSpinBox->value();}

QColor PrefGeneral::getBackgroundColor() const
{
    return backgroundColorButton->color();
}

QString PrefGeneral::getReclusteringExecutable() const{return reclusteringExecutableLineEdit->text();}

QString PrefGeneral::getReclusteringArguments() const{return reclusteringArgsLineEdit->text();}

void PrefGeneral::updateCrashRecoveryTimeInterval(int state){
    if(state == QCheckBox::On)
        crashRecoveryComboBox->setEnabled(true);
    else if(state == QCheckBox::Off)
        crashRecoveryComboBox->setEnabled(false);
}

void PrefGeneral::updateReclusteringExecutable(){
    QString executable = QFileDialog::getOpenFileName(QString(),
                                                       QString(), this, tr("Select the Reclustering executable..."));

    setReclusteringExecutable(executable);
}

#include "prefgeneral.moc"
