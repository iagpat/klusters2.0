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
 *   the Free Software Foundation; either version 2 of the License, or     *
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
#include <qstring.h>
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
    qDebug()<<" URL:"<<input.fileName();

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


    // Init libxml
    xmlInitParser();

    // Load XML document
    doc = xmlParseFile(input.fileName().toLatin1());
    if(doc == NULL) return false;

    // Create xpath evaluation context
    xpathContex = xmlXPathNewContext(doc);
    if(xpathContex== NULL){
        xmlFreeDoc(doc);
        return false;
    }

    //Read the document version
    xmlNodePtr rootElement = xmlDocGetRootElement(doc);
    xmlChar* versionTag = xmlCharStrdup(VERSION.toLatin1());
    if(rootElement != NULL){
        xmlChar* sVersion = xmlGetProp(rootElement,versionTag);
        if(sVersion != NULL)
            readVersion = QString((char*)sVersion);
        xmlFree(sVersion);
    }
    xmlFree(versionTag);

    return true;
}


void KlustersXmlReader::closeFile(){
    //Cleanup
    xmlXPathFreeContext(xpathContex);
    xmlFreeDoc(doc);

    //Shutdown libxml
    xmlCleanupParser();
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
                                break;
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
    xmlXPathObjectPtr result;
    xmlChar* searchPath = xmlCharStrdup(QString("//" + SPIKE + "/" + CHANNEL_GROUPS + "/" + GROUP).toLatin1());

    //Evaluate xpath expression
    result = xmlXPathEvalExpression(searchPath,xpathContex);
    if(result != NULL){
        xmlNodeSetPtr nodeset = result->nodesetval;
        if(!xmlXPathNodeSetIsEmpty(nodeset)){
            //loop on all the GROUP until reaching the electrodeGroupID one.
            int nbGroups = nodeset->nodeNr;
            for(int i = 0; i < nbGroups; ++i){
                if((i + 1) != electrodeGroupID) continue;
                xmlNodePtr child;
                for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
                    //skip the carriage return (text node named text and containing /n)
                    if(child->type == XML_TEXT_NODE) continue;

                    if(QString((char*)child->name) == CHANNELS){
                        //Should be only one CHANNELS element
                        xmlNodePtr channel;
                        for(channel = child->children;channel != NULL;channel = channel->next){
                            //skip the carriage return (text node named text and containing /n)
                            if(channel->type == XML_TEXT_NODE) continue;
                            if(QString((char*)channel->name) == CHANNEL){
                                xmlChar* sId = xmlNodeListGetString(doc,channel->children, 1);
                                int channelId = QString((char*)sId).toInt();
                                xmlFree(sId);
                                channels.append(channelId);
                            }
                        }
                    }
                }
                if((i + 1) == electrodeGroupID) break;
            }
        }
    }

    xmlFree(searchPath);
    xmlXPathFreeObject(result);
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
    xmlXPathObjectPtr result;
    xmlChar* searchPath = xmlCharStrdup(QString("//" + SPIKE + "/" + CHANNEL_GROUPS + "/" + GROUP).toLatin1());

    //Evaluate xpath expression
    result = xmlXPathEvalExpression(searchPath,xpathContex);
    if(result != NULL){
        xmlNodeSetPtr nodeset = result->nodesetval;
        if(!xmlXPathNodeSetIsEmpty(nodeset)){
            //loop on all the GROUP until reaching the electrodeGroupID one.
            int nbGroups = nodeset->nodeNr;
            for(int i = 0; i < nbGroups; ++i){
                if((i + 1) != electrodeGroupID) continue;
                xmlNodePtr child;
                for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
                    //skip the carriage return (text node named text and containing /n)
                    if(child->type == XML_TEXT_NODE) continue;

                    if(QString((char*)child->name) == NB_SAMPLES){
                        //Should be only one nbSamples element, so take the first one.
                        xmlChar* sNbSamples = xmlNodeListGetString(doc,child->children, 1);
                        nbSamples = QString((char*)sNbSamples).toInt();
                        xmlFree(sNbSamples);
                    }
                }
                if((i + 1) == electrodeGroupID) break;
            }
        }
    }

    xmlFree(searchPath);
    xmlXPathFreeObject(result);
    return nbSamples;
}

int KlustersXmlReader::getPeakSampleIndex(int electrodeGroupID)const{
    int index = 0;
    xmlXPathObjectPtr result;
    xmlChar* searchPath = xmlCharStrdup(QString("//" + SPIKE + "/" + CHANNEL_GROUPS + "/" + GROUP).toLatin1());

    //Evaluate xpath expression
    result = xmlXPathEvalExpression(searchPath,xpathContex);
    if(result != NULL){
        xmlNodeSetPtr nodeset = result->nodesetval;
        if(!xmlXPathNodeSetIsEmpty(nodeset)){
            //loop on all the GROUP until reaching the electrodeGroupID one.
            int nbGroups = nodeset->nodeNr;
            for(int i = 0; i < nbGroups; ++i){
                if((i + 1) != electrodeGroupID) continue;
                xmlNodePtr child;
                for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
                    //skip the carriage return (text node named text and containing /n)
                    if(child->type == XML_TEXT_NODE) continue;
                    if(QString((char*)child->name) == PEAK_SAMPLE_INDEX){
                        //Should be only one index element, so take the first one.
                        xmlChar* sindex = xmlNodeListGetString(doc,child->children, 1);
                        index = QString((char*)sindex).toInt();
                        xmlFree(sindex);
                    }
                }
                if((i + 1) == electrodeGroupID) break;
            }
        }
    }

    xmlFree(searchPath);
    xmlXPathFreeObject(result);
    return index;
}

int KlustersXmlReader::getNbFeatures(int electrodeGroupID)const{
    int nbFeatures = 0;
    xmlXPathObjectPtr result;
    xmlChar* searchPath = xmlCharStrdup(QString("//" + SPIKE + "/" + CHANNEL_GROUPS + "/" + GROUP).toLatin1());

    //Evaluate xpath expression
    result = xmlXPathEvalExpression(searchPath,xpathContex);
    if(result != NULL){
        xmlNodeSetPtr nodeset = result->nodesetval;
        if(!xmlXPathNodeSetIsEmpty(nodeset)){
            //loop on all the GROUP until reaching the electrodeGroupID one.
            int nbGroups = nodeset->nodeNr;
            for(int i = 0; i < nbGroups; ++i){
                if((i + 1) != electrodeGroupID) continue;
                xmlNodePtr child;
                for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
                    //skip the carriage return (text node named text and containing /n)
                    if(child->type == XML_TEXT_NODE) continue;
                    if(QString((char*)child->name) == NB_FEATURES){
                        //Should be only one nbFeatures element, so take the first one.
                        xmlChar* snbFeatures = xmlNodeListGetString(doc,child->children, 1);
                        nbFeatures = QString((char*)snbFeatures).toInt();
                        xmlFree(snbFeatures);
                    }
                }
                if((i + 1) == electrodeGroupID) break;
            }
        }
    }

    xmlFree(searchPath);
    xmlXPathFreeObject(result);
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
                                break;
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
                                break;
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
                                break;
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


