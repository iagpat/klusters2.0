/***************************************************************************
                          tags.h  -  description
                             -------------------
    begin                : Wed Mar 31 2004
    copyright            : (C) 2004 by Lynn Hazan
    email                : lynn.hazan.myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TAGS_H
#define TAGS_H

// include files for QT
#include <QString>

/**
  * This class contains the XML tags used in the paaramter files.
  *@author Lynn Hazan
  */

/* @namespace klusters*/
namespace klusters{

/**Tag for the version attribute of the root element.*/
extern const QString VERSION;
/**Tag for the parameter file root element.*/
extern const QString PARAMETERS;
/**Tag for the creator attribut of the parameter file root element.*/
extern const QString CREATOR;

/**Tag for the acquisition element.*/
extern const QString ACQUISITION;
/**Tag for the samplingRate element.*/
extern const QString SAMPLING_RATES;
/**Tag for the spike element.*/
extern const QString SPIKE;
/**Tag for the units element.*/
extern const QString UNITS;

//Tags included in ACQUISITION
/**Tag for the bits element included in the acquisition element.*/
extern const QString BITS;
/**Tag for the nbChannels element included in the acquisition element.*/
extern const QString NB_CHANNELS;
/**Tag for the samplingRate element included in the acquisition element.*/
extern const QString SAMPLING_RATE;
/**Tag for the voltageRange element included in the acquisition element.*/
extern const QString VOLTAGE_RANGE;
/**Tag for the amplification element included in the acquisition element.*/
extern const QString AMPLIFICATION;
/**Tag for the offset element included in the acquisition element.*/
extern const QString OFFSET;


//Tags included in SPIKE
/**Tag for the nbSamples element included in the spike element.*/
extern const QString NB_SAMPLES;
/**Tag for the peakSAmpleIndex element included in the spike element.*/
extern const QString PEAK_SAMPLE_INDEX;
/**Tag for the channelGroups element included in the spike element.*/
extern const QString CHANNEL_GROUPS;
/**Tag for the groups element included in the channelGroups element.*/
extern const QString GROUP;
/**Tag for the channels element included in the GROUP element.*/
extern const QString CHANNELS;
/**Tag for the channel element included in the CHANNELS element.*/
extern const QString CHANNEL;
/**Tag for the nbFatures element included in the GROUP element.*/
extern const QString NB_FEATURES;

//Tags included in UNITS
/**Tag for the unit element included in the units element.*/
extern const QString UNIT;
/**Tag for the cluster element included in the unit element.*/
extern const QString CLUSTER;
/**Tag for the structure element included in the unit element.*/
extern const QString STRUCTURE;
/**Tag for the type element included in the unit element.*/
extern const QString TYPE;
/**Tag for the isolationDistance element included in the unit element.*/
extern const QString ISOLATION_DISTANCE;
/**Tag for the quality element included in the unit element.*/
extern const QString QUALITY;
/**Tag for the notes element included in the unit element.*/
extern const QString NOTES;


}

#endif
