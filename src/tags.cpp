/***************************************************************************
                          tags.cpp  -  description
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

#include "tags.h"

namespace klusters{
extern const QString VERSION = "version";
extern const QString PARAMETERS = "parameters";
extern const QString CREATOR = "creator";

extern const QString ACQUISITION = "acquisitionSystem";
extern const QString SAMPLING_RATES = "samplingRates";
extern const QString SPIKE = "spikeDetection";
extern const QString UNITS = "units";

//Tags included in ACQUISITION
extern const QString BITS = "nBits";
extern const QString NB_CHANNELS = "nChannels";
extern const QString SAMPLING_RATE = "samplingRate";
extern const QString VOLTAGE_RANGE = "voltageRange";
extern const QString AMPLIFICATION = "amplification";
extern const QString OFFSET = "offset";

//Tags included in FILED_POTENTIALS
extern const QString LFP_SAMPLING_RATE = "lfpSamplingRate";

//Tags included in SPIKE
extern const QString NB_SAMPLES = "nSamples";
extern const QString PEAK_SAMPLE_INDEX = "peakSampleIndex";
extern const QString CHANNEL_GROUPS = "channelGroups";
extern const QString GROUP = "group";
extern const QString CHANNELS = "channels";
extern const QString CHANNEL = "channel";
extern const QString NB_FEATURES = "nFeatures";

//Tags included in UNITS
extern const QString UNIT = "unit";
extern const QString CLUSTER = "cluster";
extern const QString STRUCTURE = "structure";
extern const QString TYPE = "type";
extern const QString ISOLATION_DISTANCE = "isolationDistance";
extern const QString QUALITY = "quality";
extern const QString NOTES = "notes";

}

