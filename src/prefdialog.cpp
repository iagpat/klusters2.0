/***************************************************************************
                          prefdialog.cpp  -  description
                             -------------------
    begin                : Thu Dec 12 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// include files for QT
#include <qlayout.h>        // for QVBoxLayout
#include <qlabel.h>         // for QLabel
#include <qframe.h>         // for QFrame

// include files for KDE
#include <kcolorbutton.h>   // for KColorButton
#include <kpushbutton.h>    // for KPushButton
#include <klocale.h>        // for i18n()
#include <kiconloader.h>    // for KIconLoader
#include <kglobal.h>        // for KGlobal
#include <klineedit.h>      // for KLineEdit
#include <kmessagebox.h>    // for KMessageBox

//include files for the application
#include "prefdialog.h"     // class PrefDialog

#include "configuration.h"          // class Configuration and Config()
#include "prefgeneral.h"            // class PrefGeneral
#include "prefwaveformview.h" // class PrefWaveformView
#include "prefclusterview.h"        // class PrefClusterView
#include "channellist.h"

//General C++ include files
#include <iostream>
using namespace std;

/**
  *@author Lynn Hazan
*/

PrefDialog::PrefDialog(QWidget *parent,int nbChannels, const char *name, WFlags f)
 : KDialogBase(IconList, i18n("Preferences"), Help|Default|Ok|Apply|Cancel, Ok, parent, name, f)
{
    setHelp("settings","klusters");
    
    //adding page "General options"
    QFrame *frame = addPage(i18n("General"), i18n("Klusters General Configuration"),
        KGlobal::iconLoader()->loadIcon("kfm",KIcon::Panel,0,false) );
    QVBoxLayout *frameLayout = new QVBoxLayout( frame, 0, 0 );
    prefGeneral = new PrefGeneral(frame);
    frameLayout->addWidget(prefGeneral);

    //adding page "Cluster view configuration"
    frame = addPage(i18n("Cluster view"), i18n("Cluster View configuration"),
        KGlobal::iconLoader()->loadIcon("clusterview",KIcon::User));
    frameLayout = new QVBoxLayout( frame, 0, 0 );
    prefclusterView = new PrefClusterView(frame);
    frameLayout->addWidget(prefclusterView);

    //adding page "Waveform view configuration"
    frame = addPage(i18n("Waveform view"), i18n("Waveform View configuration"),
        KGlobal::iconLoader()->loadIcon("waveformview",KIcon::User));
    frameLayout = new QVBoxLayout( frame, 0, 0 );
    prefWaveformView = new PrefWaveformView(frame,nbChannels);
    frameLayout->addWidget(prefWaveformView);

    // connect interactive widgets and selfmade signals to the enableApply slotDefault
    connect(prefGeneral->crashRecoveryCheckBox,SIGNAL(clicked()),this,SLOT(enableApply()));
    connect(prefGeneral->crashRecoveryComboBox,SIGNAL(activated(int)),this,SLOT(enableApply()));
    connect(prefGeneral->undoSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefGeneral->backgroundColorButton,SIGNAL(changed(const QColor&)),this,SLOT(enableApply()));
    connect(prefGeneral->reclusteringExecutableLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(enableApply()));
    //connect(prefGeneral,SIGNAL(reclusteringArgsUpdate()),this,SLOT(enableApply()));
    connect(prefGeneral->reclusteringArgsLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(enableApply()));
    
    connect(prefclusterView->intervalSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefWaveformView->gainSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefWaveformView,SIGNAL(positionsChanged()),this,SLOT(enableApply()));

    applyEnable = false;
}

void PrefDialog::updateDialog() {  
  prefGeneral->setCrashRecovery(configuration().isCrashRecovery());
  prefGeneral->setCrashRecoveryIndex(configuration().crashRecoveryIntervalIndex());
  prefGeneral->setNbUndo(configuration().getNbUndo());
  prefGeneral->setBackgroundColor(configuration().getBackgroundColor());
  prefGeneral->setReclusteringExecutable(configuration().getReclusteringExecutable());
  prefGeneral->setReclusteringArguments(configuration().getReclusteringArguments()); 
  prefclusterView->setTimeInterval(configuration().getTimeInterval());
  prefWaveformView->setGain(configuration().getGain());
  
  enableButtonApply(false);   // disable apply button
  applyEnable = false;
}
 

void PrefDialog::updateConfiguration(){
  configuration().setCrashRecovery(prefGeneral->isCrashRecovery());
  configuration().setCrashRecoveryIndex(prefGeneral->crashRecoveryIntervalIndex());
  configuration().setNbUndo(prefGeneral->getNbUndo());
  configuration().setBackgroundColor(prefGeneral->getBackgroundColor()); 
  configuration().setReclusteringExecutable(prefGeneral->getReclusteringExecutable());
  configuration().setReclusteringArguments(prefGeneral->getReclusteringArguments());
  configuration().setTimeInterval(prefclusterView->getTimeInterval());
  configuration().setGain(prefWaveformView->getGain());
  configuration().setNbChannels(prefWaveformView->getNbChannels());
  configuration().setChannelPositions(prefWaveformView->getChannelPositions()); 
      
  enableButtonApply(false);   // disable apply button
  applyEnable = false;
}


void PrefDialog::slotDefault() {
  if (KMessageBox::warningContinueCancel(this, i18n("This will set the default options "
      "in ALL pages of the preferences dialog! Do you wish to continue?"), i18n("Set default options?"),
      i18n("Set defaults"))==KMessageBox::Continue){
        
   prefGeneral->setCrashRecovery(configuration().isCrashRecoveryDefault());
   prefGeneral->setCrashRecoveryIndex(configuration().crashRecoveryIntervalIndexDefault());
   prefGeneral->setNbUndo(configuration().getNbUndoDefault());
   prefGeneral->setBackgroundColor(configuration().getBackgroundColorDefault());
   prefGeneral->setReclusteringExecutable(configuration().getReclusteringExecutableDefault());
   prefGeneral->setReclusteringArguments(configuration().getReclusteringArgumentsDefault()); 
   prefclusterView->setTimeInterval(configuration().getTimeIntervalDefault());
   prefWaveformView->setGain(configuration().getGainDefault());
   prefWaveformView->resetChannelList(configuration().getNbChannels());
   
   enableApply();   // enable apply button
  }
}


void PrefDialog::slotApply() {
  updateConfiguration();      // transfer settings to configuration object
  emit settingsChanged();     // apply the preferences    
  enableButtonApply(false);   // disable apply button again
}


void PrefDialog::enableApply() {
    enableButtonApply(true);   // enable apply button
    applyEnable = true;
}

void PrefDialog::resetChannelList(int nbChannels){
  prefWaveformView->resetChannelList(nbChannels);
}

void PrefDialog::enableChannelSettings(bool state){
  prefWaveformView->enableChannelSettings(state);
}
    

