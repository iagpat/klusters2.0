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

//General C++ include files
#include <iostream>
//Added by qt3to4:
#include <Q3ValueList>
using namespace std;

//include files for QT
#include <qfileinfo.h>
#include <qstring.h>

using namespace klusters;

KlustersXmlReader::KlustersXmlReader():readVersion(""){
}
KlustersXmlReader::~KlustersXmlReader(){
}

bool KlustersXmlReader::parseFile(const QFile& file,fileType type){
 this->type = type;

 // Init libxml
 xmlInitParser();

 // Load XML document
 doc = xmlParseFile(file.fileName().toLatin1());
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
  if(sVersion != NULL) readVersion = QString((char*)sVersion);
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
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + BITS).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one resolution element, so take the first one.
   xmlChar* sResolution = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   resolution = QString((char*)sResolution).toInt();
   xmlFree(sResolution);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);
 return resolution;
}

int KlustersXmlReader::getNbChannels()const{
 int nbChannels = 0;
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + NB_CHANNELS).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one nbChannels element, so take the first one.
   xmlChar* sNbChannels = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   nbChannels = QString((char*)sNbChannels).toInt();
   xmlFree(sNbChannels);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);
 return nbChannels;
}


Q3ValueList<int> KlustersXmlReader::getNbChannelsByGroup(int electrodeGroupID)const{
 Q3ValueList<int> channels;
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
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + SAMPLING_RATE).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one sampling rate element at that level, so take the first one.
   xmlChar* sSamplingRate = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   samplingRate = QString((char*)sSamplingRate).toDouble();
   xmlFree(sSamplingRate);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);
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
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + VOLTAGE_RANGE).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one range element, so take the first one.
   xmlChar* sRange = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   range = QString((char*)sRange).toInt();
   xmlFree(sRange);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);
 return range;
}


int KlustersXmlReader::getAmplification() const{
 int amplification = 0;
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + AMPLIFICATION).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one amplification element, so take the first one.
   xmlChar* sAmplification = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   amplification = QString((char*)sAmplification).toInt();
   xmlFree(sAmplification);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);
 return amplification;
}


int KlustersXmlReader::getOffset()const{
 int offset = 0;
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup(QString("//" + ACQUISITION + "/" + OFFSET).toLatin1());

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //Should be only one offset element, so take the first one.
   xmlChar* sOffset = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
   offset = QString((char*)sOffset).toInt();
   xmlFree(sOffset);
  }
 }

 xmlFree(searchPath);
 xmlXPathFreeObject(result);

 return offset;
}

void KlustersXmlReader::getClusterUserInformation (int pGroup, QMap<int,ClusterUserInformation>& clusterUserInformationMap)const{
	xmlXPathObjectPtr result;
	xmlChar* searchPath = xmlCharStrdup(QString("//" + UNITS + "/" + UNIT).toLatin1());

 //Evaluate xpath expression
	result = xmlXPathEvalExpression(searchPath,xpathContex);
	if(result != NULL){
		xmlNodeSetPtr nodeset = result->nodesetval;
		if(!xmlXPathNodeSetIsEmpty(nodeset)){
   	//loop on all the UNIT.
			int nbUnits = nodeset->nodeNr;
			for(int i = 0; i < nbUnits; ++i){
				int groupId = 0;
				int clusterId = 0;
				QString structure = "";
				QString type = "";
				QString ID = "";
				QString quality = "";
				QString notes = "";
				ClusterUserInformation currentClusterUserInformation = ClusterUserInformation();
				xmlNodePtr child;

				for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     				//skip the carriage return (text node named text and containing /n)
					if(child->type == XML_TEXT_NODE) continue;

					if(QString((char*)child->name) == GROUP){
						xmlChar* sId = xmlNodeListGetString(doc,child->children, 1);
						groupId = QString((char*)sId).toInt();
						xmlFree(sId);

						//Group is the first tag and we are looking only for the units having a groupId equals to pGroup.
						if(groupId != pGroup){
							break;
						}
						else{
							currentClusterUserInformation.setGroup(groupId);
						}
					}
					if(QString((char*)child->name) == CLUSTER){
						xmlChar* sClusterId = xmlNodeListGetString(doc,child->children, 1);
						clusterId =  QString((char*)sClusterId).toInt();
						xmlFree(sClusterId);

						currentClusterUserInformation.setCluster(clusterId);
					}
					if(QString((char*)child->name) == STRUCTURE){
						xmlChar* sStructure = xmlNodeListGetString(doc,child->children, 1);
						QString structure = QString((char*)sStructure);
						xmlFree(sStructure);

						currentClusterUserInformation.setStructure(structure);
					}
					if(QString((char*)child->name) == TYPE){
						xmlChar* sType = xmlNodeListGetString(doc,child->children, 1);
						QString type = QString((char*)sType);
						xmlFree(sType);

						currentClusterUserInformation.setType(type);
					}
					if(QString((char*)child->name) == ID){
						xmlChar* sID = xmlNodeListGetString(doc,child->children, 1);
						QString ID = QString((char*)sID);
						xmlFree(sID);

						currentClusterUserInformation.setId(ID);
					}
					if(QString((char*)child->name) == QUALITY){
						xmlChar* sQuality = xmlNodeListGetString(doc,child->children, 1);
						QString quality = QString((char*)sQuality);
						xmlFree(sQuality);

						currentClusterUserInformation.setQuality(quality);
					}
					if(QString((char*)child->name) == NOTES){
						xmlChar* sNotes = xmlNodeListGetString(doc,child->children, 1);
						QString notes = QString((char*)sNotes);
						xmlFree(sNotes);

						currentClusterUserInformation.setNotes(notes);
					}
				}
				if(groupId == pGroup){
					clusterUserInformationMap.insert(clusterId,currentClusterUserInformation);
				}
			}
		}
	}
}


