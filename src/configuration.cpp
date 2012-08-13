/***************************************************************************
                          configuration.cpp  -  description
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
// include files for KDE
#include <kapplication.h>       // for 'kapp'
#include <kconfig.h>            // for KConfig
#include <klocale.h>            // for i18n()

//include files for the application
#include "configuration.h"

/**
  *@author Lynn Hazan
*/


const bool Configuration::crashRecoveryDefault = true;
const int  Configuration::crashRecoveryIndexDefault = 0;
const int  Configuration::gainDefault = 200;
const int  Configuration::timeIntervalDefault = 60;
const int  Configuration::nbUndoDefault = 2;
const QColor Configuration::backgroundColorDefault = QColor(Qt::black);
const QString Configuration::reclusteringExecutableDefault = "KlustaKwik";
const QString Configuration::reclusteringArgsDefault =
      "%fileBaseName %electrodeGroupID -MinClusters 2 -MaxClusters 12 -UseFeatures %features";

Configuration::Configuration():nbChannels(0) {
    read(); // read the settings or set them to the default values
}

void Configuration::read() {
    KConfig* conf = kapp->config();

    //read general options
    conf->setGroup("General");
    crashRecovery = conf->readBoolEntry("crashRecovery",crashRecoveryDefault);
    crashRecoveryIndex = conf->readNumEntry("crashRecoveryIndex",crashRecoveryIndexDefault);
    nbUndo = conf->readNumEntry("nbUndo",nbUndoDefault);    
    backgroundColor = conf->readColorEntry("backgroundColor",&backgroundColorDefault);
    reclusteringExecutable = conf->readEntry("reclusteringExecutable",reclusteringExecutableDefault);
    reclusteringArgs = conf->readEntry("reclusteringArgs",reclusteringArgsDefault);
    
    //read cluster view options
    conf->setGroup("clusterView");
    timeInterval = conf->readNumEntry("timeInterval",timeIntervalDefault);

    //read waveform view options
    conf->setGroup("waveformView");
    gain = conf->readNumEntry("gain",gainDefault);
}

void Configuration::write() const {  
    KConfig* conf = kapp->config();
    //write general options
    conf->setGroup("General");
    conf->writeEntry("crashRecovery",crashRecovery);
    conf->writeEntry("crashRecoveryIndex",crashRecoveryIndex);
    conf->writeEntry("nbUndo",nbUndo);
    conf->writeEntry("backgroundColor",backgroundColor);
    conf->writeEntry("reclusteringExecutable",reclusteringExecutable);
    conf->writeEntry("reclusteringArgs",reclusteringArgs); 
    
    //write cluster view options
    conf->setGroup("clusterView");
    conf->writeEntry("timeInterval",timeInterval);

    //write waveform view options
    conf->setGroup("waveformView");
    conf->writeEntry("gain",gain);
}

Configuration& configuration() {
  //The C++ standard requires that static variables in functions
  //have to be created upon first call of the function.
  static Configuration conf;
  return conf;
}
