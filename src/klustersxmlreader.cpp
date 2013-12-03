/***************************************************************************
                          klustersxmlreader.cpp  -  description
                             -------------------
    begin                : Tue Mar 2 2004
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
//application specific include files.
#include "config-klusters.h"
#include "klustersxmlreader.h"
#include "tags.h"
#include "clusteruserinformation.h"

#include <QList>
#include <QDomDocument>

//include files for QT
#include <qfileinfo.h>
#include <QString>
#include <QDebug>

using namespace klusters;

KlustersXmlReader::KlustersXmlReader()
    :readVersion("")
{
}

KlustersXmlReader::~KlustersXmlReader()
{
}

bool KlustersXmlReader::parseFile(const QFile& file,fileType type){
    this->type = type;

    QFile input(file.fileName());
    qDebug()<<" file.fileName"<<file.fileName();

    QDomDocument docElement;
    QString errorMsg;
    int errorRow;
    int errorCol;
    if ( !docElement.setContent( &input, &errorMsg, &errorRow, &errorCol ) ) {
        qWarning() << "Unable to load document.Parse error in " <<  input.fileName() << ", line " << errorRow << ", col " << errorCol << ": " << errorMsg << endl;
        return false;
    }

    QDomElement element = docElement.documentElement();

    if (element.tagName() == QLatin1String("parameters")) {
        if( element.hasAttribute(VERSION)) {
            readVersion = element.attribute(VERSION);
            qDebug()<<" readVersion "<<readVersion;
        }
    }
    documentNode = element;

    return true;
}


void KlustersXmlReader::closeFile(){
}


int KlustersXmlReader::getResolution()const{
    int resolution = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == BITS) {
                                resolution = u.text().toInt();
                                return resolution;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }

    return resolution;
}

int KlustersXmlReader::getNbChannels()const{
    int nbChannels = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == NB_CHANNELS) {
                                nbChannels = u.text().toInt();
                                return nbChannels;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return nbChannels;
}


QList<int> KlustersXmlReader::getNbChannelsByGroup(int electrodeGroupID)const{
    QList<int> channels;

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == SPIKE) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                int i = 0;
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            if((i + 1) != electrodeGroupID) {
                                                ++i;
                                            } else {
                                                QDomNode group = val.firstChild(); // try to convert the node to an element.

                                                while(!group.isNull()) {
                                                    QDomElement valGroup = group.toElement();

                                                    if (!valGroup.isNull()) {
                                                        tag = valGroup.tagName();
                                                        if( tag == CHANNELS) {

                                                            if((i + 1) != electrodeGroupID) {
                                                                ++i;
                                                                continue;
                                                            }
                                                            QDomNode channelsNode = valGroup.firstChild(); // try to convert the node to an element.
                                                            while(!channelsNode.isNull()) {
                                                                QDomElement channelsElement = channelsNode.toElement();
                                                                if (!channelsElement.isNull()) {
                                                                    tag = channelsElement.tagName();
                                                                    if (tag == CHANNEL) {
                                                                        int channelId = channelsElement.text().toInt();
                                                                        channels.append(channelId);
                                                                    }
                                                                }
                                                                channelsNode = channelsNode.nextSibling();
                                                            }
                                                        }
                                                    }
                                                    group = group.nextSibling();
                                                }
                                                if((i + 1) == electrodeGroupID)
                                                    break;
                                                ++i;

                                            }
                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return channels;
}



double KlustersXmlReader::getSamplingRate()const{
    double samplingRate = 0;

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == SAMPLING_RATE) {
                                samplingRate = u.text().toDouble();
                                return samplingRate;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return samplingRate;
}

int KlustersXmlReader::getNbSamples(int electrodeGroupID)const{
    int nbSamples = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == SPIKE) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                int i = 0;
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            QDomNode group = val.firstChild(); // try to convert the node to an element.

                                            while(!group.isNull()) {
                                                QDomElement valGroup = group.toElement();

                                                if (!valGroup.isNull()) {
                                                    tag = valGroup.tagName();
                                                    if( tag == NB_SAMPLES) {
                                                        if((i + 1) != electrodeGroupID) {
                                                            ++i;
                                                            continue;
                                                        }
                                                        nbSamples = valGroup.text().toInt();
                                                    }
                                                }
                                                group = group.nextSibling();
                                            }
                                            if((i + 1) == electrodeGroupID)
                                                break;
                                            ++i;

                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return nbSamples;
}

int KlustersXmlReader::getPeakSampleIndex(int electrodeGroupID)const{
    int index = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == SPIKE) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                int i = 0;
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            QDomNode group = val.firstChild(); // try to convert the node to an element.

                                            while(!group.isNull()) {
                                                QDomElement valGroup = group.toElement();

                                                if (!valGroup.isNull()) {
                                                    tag = valGroup.tagName();
                                                    if( tag == PEAK_SAMPLE_INDEX) {
                                                        if((i + 1) != electrodeGroupID) {
                                                            ++i;
                                                            continue;
                                                        }
                                                        index = valGroup.text().toInt();
                                                    }
                                                }
                                                group = group.nextSibling();
                                            }
                                            if((i + 1) == electrodeGroupID)
                                                break;
                                            ++i;

                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return index;
}

int KlustersXmlReader::getNbFeatures(int electrodeGroupID)const{
    int nbFeatures = 0;

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == SPIKE) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                int i = 0;
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            QDomNode group = val.firstChild(); // try to convert the node to an element.

                                            while(!group.isNull()) {
                                                QDomElement valGroup = group.toElement();

                                                if (!valGroup.isNull()) {
                                                    tag = valGroup.tagName();
                                                    if( tag == NB_FEATURES) {
                                                        if((i + 1) != electrodeGroupID) {
                                                            ++i;
                                                            continue;
                                                        }
                                                        nbFeatures = valGroup.text().toInt();
                                                    }
                                                }
                                                group = group.nextSibling();
                                            }
                                            if((i + 1) == electrodeGroupID)
                                                break;
                                            ++i;

                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }

    return nbFeatures;
}


int KlustersXmlReader::getVoltageRange() const{
    int range = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == VOLTAGE_RANGE) {
                                range = u.text().toInt();
                                return range;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return range;
}


int KlustersXmlReader::getAmplification() const{
    int amplification = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == AMPLIFICATION) {
                                amplification = u.text().toInt();
                                return amplification;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return amplification;
}


int KlustersXmlReader::getOffset()const{
    int offset = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == OFFSET) {
                                offset = u.text().toInt();
                                return offset;
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return offset;
}

void KlustersXmlReader::getClusterUserInformation (int pGroup, QMap<int,ClusterUserInformation>& clusterUserInformationMap)const{

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == UNITS) {
                    QDomNode unit = e.firstChild(); // try to convert the node to an element.
                    int groupId = 0;
                    int clusterId = 0;
                    QString structure;
                    QString type;
                    QString ID;
                    QString quality;
                    QString notes;
                    ClusterUserInformation currentClusterUserInformation = ClusterUserInformation();

                    while(!unit.isNull()) {
                        QDomElement u = unit.toElement();
                        if (!u.isNull()) {

                            tag = u.tagName();
                            QDomNode val = u.firstChild();
                            while(!val.isNull()) {
                                QDomElement valElement = val.toElement();
                                if (!valElement.isNull()) {
                                    tag = valElement.tagName();
                                    if (tag == GROUP) {
                                        groupId = valElement.text().toInt();
                                        if(groupId != pGroup){
                                            break;
                                        }
                                        else{
                                            currentClusterUserInformation.setGroup(groupId);
                                        }
                                    } else if (tag == CLUSTER) {
                                        clusterId = valElement.text().toInt();
                                        currentClusterUserInformation.setCluster(clusterId);
                                    } else if (tag ==STRUCTURE ) {
                                        structure = valElement.text();
                                        currentClusterUserInformation.setStructure(structure);
                                    } else if (tag ==TYPE ) {
                                        type = valElement.text();
                                        currentClusterUserInformation.setType(type);
                                    } else if (tag == ISOLATION_DISTANCE) {
                                        ID = valElement.text();
                                        currentClusterUserInformation.setId(ID);
                                    } else if (tag == QUALITY) {
                                        quality = valElement.text();
                                        currentClusterUserInformation.setQuality(quality);
                                    } else if (tag == NOTES) {
                                        notes = valElement.text();
                                        currentClusterUserInformation.setNotes(notes);
                                    }
                                }
                                val = val.nextSibling();
                            }
                        }
                        if(groupId == pGroup){
                            clusterUserInformationMap.insert(clusterId,currentClusterUserInformation);
                        }
                        unit = unit.nextSibling();
                    }
                }
            }
            n = n.nextSibling();
        }
    }

}


