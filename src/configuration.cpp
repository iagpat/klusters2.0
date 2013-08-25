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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application
#include "configuration.h"
#include <QSettings>

/**
  *@author Lynn Hazan
*/


const bool Configuration::crashRecoveryDefault = true;
const int  Configuration::crashRecoveryIndexDefault = 0;
const int  Configuration::gainDefault = 200;
const int  Configuration::timeIntervalDefault = 60;
const int  Configuration::nbUndoDefault = 2;
const QColor Configuration::backgroundColorDefault = QColor(Qt::black);
const QString Configuration::reclusteringExecutableDefault = QLatin1String("KlustaKwik");
const QString Configuration::reclusteringArgsDefault =
        "%fileBaseName %electrodeGroupID -MinClusters 2 -MaxClusters 12 -UseFeatures %features";

Configuration::Configuration():nbChannels(0) {
    read(); // read the settings or set them to the default values
}

void Configuration::read() {
    QSettings settings;

    settings.beginGroup("General");
    crashRecovery = settings.value("crashRecovery",crashRecoveryDefault).toBool();
    crashRecoveryIndex = settings.value("crashRecoveryIndex",crashRecoveryIndexDefault).toInt();
    nbUndo = settings.value("nbUndo",nbUndoDefault).toInt();
    backgroundColor = settings.value("backgroundColor",&backgroundColorDefault).value<QColor>();
    reclusteringExecutable = settings.value("reclusteringExecutable",reclusteringExecutableDefault).toString();
    reclusteringArgs = settings.value("reclusteringArgs",reclusteringArgsDefault).toString();
    useWhiteColorDuringPrinting = settings.value("useWhiteColorDuringPrinting",true).toBool();
    settings.endGroup();

    //read cluster view options
    settings.beginGroup("clusterView");
    timeInterval = settings.value("timeInterval",timeIntervalDefault).toInt();
    settings.endGroup();

    //read waveform view options
    settings.beginGroup("waveformView");
    gain = settings.value("gain",gainDefault).toInt();
    settings.endGroup();
}

void Configuration::write() const {  
    QSettings settings;

    settings.beginGroup("General");
    settings.setValue("crashRecovery",crashRecovery);
    settings.setValue("crashRecoveryIndex",crashRecoveryIndex);
    settings.setValue("nbUndo",nbUndo);
    settings.setValue("backgroundColor",backgroundColor);
    settings.setValue("reclusteringExecutable",reclusteringExecutable);
    settings.setValue("reclusteringArgs",reclusteringArgs);
    settings.setValue("useWhiteColorDuringPrinting",useWhiteColorDuringPrinting);
    settings.endGroup();
    
    //write cluster view options
    settings.beginGroup("clusterView");
    settings.setValue("timeInterval",timeInterval);
    settings.endGroup();

    //write waveform view options
    settings.beginGroup("waveformView");
    settings.setValue("gain",gain);
    settings.beginGroup("General");
}

Configuration& configuration() {
    //The C++ standard requires that static variables in functions
    //have to be created upon first call of the function.
    static Configuration conf;
    return conf;
}
