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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// include files for QT
#include <qlayout.h>        // for QVBoxLayout
#include <qlabel.h>         // for QLabel

#include <QMessageBox>

//include files for the application
#include "prefdialog.h"     // class PrefDialog

#include "configuration.h"          // class Configuration and Config()
#include "prefgeneral.h"            // class PrefGeneral
#include "prefwaveformview.h" // class PrefWaveformView
#include "prefclusterview.h"        // class PrefClusterView
#include "channellist.h"
#include "config-klusters.h"
#include <qhelpviewer.h>

/**
  *@author Lynn Hazan
*/

PrefDialog::PrefDialog(QWidget *parent,int nbChannels)
 : QPageDialog(parent)
{

    setButtons(Help | Default | Ok | Apply | Cancel);
    setDefaultButton(Ok);
    setFaceType(List);
    setWindowTitle(tr("Preferences"));

    setHelp("settings","klusters");
    
    QWidget * w = new QWidget(this);
    prefGeneral = new PrefGeneral(w);
    QPageWidgetItem *item = new QPageWidgetItem(prefGeneral,tr("General"));
    item->setHeader(tr("Klusters General Configuration"));
    item->setIcon(QIcon(":/shared-icons/folder-open"));


    addPage(item);



    //adding page "Cluster view configuration"
    w = new QWidget(this);
    prefclusterView = new PrefClusterView(w);

    item = new QPageWidgetItem(prefclusterView,tr("Cluster view"));
    item->setHeader(tr("Cluster View configuration"));
    item->setIcon(QIcon(":/icons/clusterview"));
    addPage(item);


    //adding page "Waveform view configuration"
    w = new QWidget(this);
    prefWaveformView = new PrefWaveformView(w,nbChannels);

    item = new QPageWidgetItem(prefWaveformView,tr("Waveform view"));
    item->setHeader(tr("Waveform View configuration"));
    item->setIcon(QIcon(":/icons/waveformview"));
    addPage(item);



    // connect interactive widgets and selfmade signals to the enableApply slotDefault
    connect(prefGeneral->crashRecoveryCheckBox,SIGNAL(clicked()),this,SLOT(enableApply()));
    connect(prefGeneral->crashRecoveryComboBox,SIGNAL(activated(int)),this,SLOT(enableApply()));
    connect(prefGeneral->undoSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefGeneral->backgroundColorButton,SIGNAL(colorChanged(QColor)),this,SLOT(enableApply()));
    connect(prefGeneral->reclusteringExecutableLineEdit,SIGNAL(textChanged(QString)),this,SLOT(enableApply()));
    //connect(prefGeneral,SIGNAL(reclusteringArgsUpdate()),this,SLOT(enableApply()));
    connect(prefGeneral->reclusteringArgsLineEdit,SIGNAL(textChanged(QString)),this,SLOT(enableApply()));
    connect(prefGeneral->useWhiteColorPrinting,SIGNAL(clicked()),this,SLOT(enableApply()));
    
    connect(prefclusterView->intervalSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefWaveformView->gainSpinBox,SIGNAL(valueChanged(int)),this,SLOT(enableApply()));
    connect(prefWaveformView,SIGNAL(positionsChanged()),this,SLOT(enableApply()));


    connect(this, SIGNAL(applyClicked()), SLOT(slotApply()));
    connect(this, SIGNAL(defaultClicked()), SLOT(slotDefault()));
    connect(this,SIGNAL(helpClicked()),SLOT(slotHelp()));

    applyEnable = false;
}

void PrefDialog::slotHelp()
{
    QHelpViewer *helpDialog = new QHelpViewer(this);
    helpDialog->setHtml(KLUSTER_DOC_PATH + QLatin1String("index.html"));
    helpDialog->setAttribute( Qt::WA_DeleteOnClose );
    helpDialog->show();
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
  prefGeneral->setUseWhiteColorDuringPrinting(configuration().getUseWhiteColorDuringPrinting());
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
  configuration().setUseWhiteColorDuringPrinting(prefGeneral->useWhiteColorDuringPrinting());
  enableButtonApply(false);   // disable apply button
  applyEnable = false;
}


void PrefDialog::slotDefault() {
  if (QMessageBox::question(this, tr("Set default options?"), tr("This will set the default options "
      "in ALL pages of the preferences dialog! Do you wish to continue?"), QMessageBox::RestoreDefaults|QMessageBox::Cancel
      )==QMessageBox::RestoreDefaults){
        
   prefGeneral->setCrashRecovery(configuration().isCrashRecoveryDefault());
   prefGeneral->setCrashRecoveryIndex(configuration().crashRecoveryIntervalIndexDefault());
   prefGeneral->setNbUndo(configuration().getNbUndoDefault());
   prefGeneral->setBackgroundColor(configuration().getBackgroundColorDefault());
   prefGeneral->setReclusteringExecutable(configuration().getReclusteringExecutableDefault());
   prefGeneral->setReclusteringArguments(configuration().getReclusteringArgumentsDefault()); 
   prefGeneral->setUseWhiteColorDuringPrinting(configuration().getUseWhiteColorDuringPrinting());

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
    

