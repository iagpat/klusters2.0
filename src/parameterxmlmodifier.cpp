/***************************************************************************
                          parameterxmlmodifier.cpp  -  description
                             -------------------
    begin                : Fri Apr 2 2004
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
#include "parameterxmlmodifier.h"
#include "tags.h"
#include "clusteruserinformation.h"

#include <fstream>

#include <QTextStream>


//include files for QT
#include <qfile.h>
#include <QString>

using namespace klusters;


ParameterXmlModifier::ParameterXmlModifier() {}
ParameterXmlModifier::~ParameterXmlModifier() {}

bool ParameterXmlModifier::parseFile(const QString& url) {

    QFile file(url);
    if (!file.open(QIODevice::ReadWrite)) return false;
    //actually load the file in a tree in  memory
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }

    file.close();

    //Find the root element
    root = doc.firstChild();
    if (root.isNull()) return false;
    //if the first element is an Processing Instruction takes the sibiling child as the root.
    if (root.isProcessingInstruction()) root = root.nextSibling();
    if (root.isNull()) return false;

    //Keep a copy of the original document
    initialXmlDocument = doc.toString();

    return true;
}

bool ParameterXmlModifier::writeTofile(const QString& url) {
    QFile parameterFile(url);
    bool status = parameterFile.open(QIODevice::WriteOnly);
    if (!status) return status;

    //insert a unit node after the spikeDetection node
    spikeDetection = findDirectChild(SPIKE);

    QDomNode newChild;
    newChild = root.insertAfter(units,spikeDetection);

    if (newChild.isNull()) {
        QTextStream stream(&parameterFile);
        stream<< initialXmlDocument;
        parameterFile.close();
        return false;
    }


    QString xmlDocument = doc.toString();

    QTextStream stream(&parameterFile);
    stream<< xmlDocument;
    parameterFile.close();

    return true;
}

QDomNode ParameterXmlModifier::findDirectChild(const QString& childName) {
    QDomNode child = root.firstChild();

    while (!child.isNull()) {
        // the node really is an element and has the right tag.
        if (child.isElement() && child.nodeName() == childName) return child;
        child = child.nextSibling();
    }

    //No node has been found, return an empty node
    return QDomNode();
}

QDomNode ParameterXmlModifier::findDirectChild(const QString& childName,const QDomNode& ancestor) {
    if (ancestor.isNull()) return QDomNode();
    QDomNode child = ancestor.firstChild();
    while (!child.isNull()) {
        // the node really is an element and has the right tag.
        if (child.isElement() && child.nodeName() == childName)  return child;
        child = child.nextSibling();
    }

    //No node has been found, return an empty node
    return QDomNode();
}

QDomNode ParameterXmlModifier::findDirectChild(const QString& childName, const QString& grandChildName,const QString& value,const QDomNode& ancestor) {
    if (ancestor.isNull()) return QDomNode();
    QDomNode child = ancestor.firstChild();
    while (!child.isNull()) {
        // the node really is an element and has the right tag.
        if (child.isElement() && child.nodeName() == childName) {
            QDomNodeList list = child.childNodes();
            for (uint i=0;i<list.count();++i) {
                QDomNode grandChild = list.item(i);
                if (grandChild.isElement() && grandChild.nodeName() == grandChildName) {
                    QDomNode textNode = grandChild.firstChild();
                    if (textNode.isText() && textNode.nodeValue() == value) return child;
                    else break;
                }
            }
        }
        child = child.nextSibling();
    }

    //No node has been found, return an empty node
    return QDomNode();
}

bool ParameterXmlModifier::setClusterUserInformation (int pGroup,QMap<int,ClusterUserInformation>& clusterUserInformationMap) {

    QString vGroupId = QString::number(pGroup);

    //attention il faut garder les informtion des autres groupes.

    QDomNode existingUnits = findDirectChild(UNITS);
    units = doc.createElement(UNITS);

    //If the units element exist, remove the elements concerning the current group
    if (!existingUnits.isNull()) {

        //QDomNode child = existingUnits.firstChild();
        QDomNodeList children = existingUnits.childNodes();

        for(uint i =0; i< children.length();i++){
            QDomNode child = children.item(i);
            if (child.isElement() && child.nodeName() == UNIT){
                QDomNode groupNode = findDirectChild(GROUP,child);
                QDomNode textNode = groupNode.firstChild();
                if (textNode.isText() && textNode.nodeValue() != vGroupId){
                    units.appendChild(child.cloneNode());
                }
            }

        }

        QDomNode oldChild;
        oldChild = root.removeChild(existingUnits);
        if (oldChild.isNull()) {
            return false;
        }
    }

    QMap<int,ClusterUserInformation>::Iterator iterator;
    int clusterId = 0;
    int group = 0;
    QString structure;
    QString type;
    QString ID;
    QString quality;
    QString notes;
    ClusterUserInformation currentClusterUserInformation;

    for (iterator = clusterUserInformationMap.begin(); iterator != clusterUserInformationMap.end(); ++iterator) {
        //Get the cluster user information (structure, type,isolation distance, quality, notes)
        clusterId = iterator.key();
        currentClusterUserInformation = iterator.value();
        group = currentClusterUserInformation.getGroup();
        structure = currentClusterUserInformation.getStructure();
        type = currentClusterUserInformation.getType();
        ID = currentClusterUserInformation.getId();
        quality = currentClusterUserInformation.getQuality();
        notes = currentClusterUserInformation.getNotes();

        QDomElement groupElement = doc.createElement(GROUP);
        QDomText groupValue = doc.createTextNode(QString::fromLatin1("%1").arg(group));
        groupElement.appendChild(groupValue);

        QDomElement clusterElement = doc.createElement(CLUSTER);
        QDomText clusterValue = doc.createTextNode(QString::fromLatin1("%1").arg(clusterId));
        clusterElement.appendChild(clusterValue);

        QDomElement structureElement = doc.createElement(STRUCTURE);
        QDomText structureValue = doc.createTextNode(structure);
        structureElement.appendChild(structureValue);

        QDomElement typeElement = doc.createElement(TYPE);
        QDomText typeValue = doc.createTextNode(type);
        typeElement.appendChild(typeValue);

        QDomElement idElement = doc.createElement(ISOLATION_DISTANCE);
        QDomText idValue = doc.createTextNode(ID);
        idElement.appendChild(idValue);

        QDomElement qualityElement = doc.createElement(QUALITY);
        QDomText qualityValue = doc.createTextNode(quality);
        qualityElement.appendChild(qualityValue);

        QDomElement notesElement = doc.createElement(NOTES);
        QDomText notesValue = doc.createTextNode(notes);
        notesElement.appendChild(notesValue);

        QDomElement unitElement = doc.createElement(UNIT);
        unitElement.appendChild(groupElement);
        unitElement.appendChild(clusterElement);
        unitElement.appendChild(structureElement);
        unitElement.appendChild(typeElement);
        unitElement.appendChild(idElement);
        unitElement.appendChild(qualityElement);
        unitElement.appendChild(notesElement);

        //Add the current unit element to the units element
        units.appendChild(unitElement);
    }

    return true;

}
