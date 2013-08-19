/***************************************************************************
                          parameterxmlmodifier.h  -  description
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

#ifndef PARAMETERXMLMODIFIER_H
#define PARAMETERXMLMODIFIER_H




//include files for QT
#include <QList>
#include <qmap.h>
#include <QDomNode>
class ClusterUserInformation;

/**
  * Class modifying the parameter xml file.
For the moment this class only adds the cluster user information (in the units section).
  *@author Lynn Hazan
  */

class ParameterXmlModifier {
public:

    /**Constructor.
  */
    ParameterXmlModifier();

    ~ParameterXmlModifier();

    /**Opens and parses the file with the @p url.
  * @param url url of the file to open.
  * @return true if the file was correctly parse, false othewise.
  */
    bool parseFile(const QString& url);

    /**Writes the modified xml tree to a parameter file given by @p url.
  * @param url url of the file to write to.
  * @return true if the parameter file could be write to disk, false otherwise.
  */
    bool writeTofile(const QString& url);

    /**
  * Finds the first child node with the tag name @p childName direct child of the root node.
  * @param childName name of the child node tag to look up.
  * @return the first node starting from root having @p tagName as tag name.
  */
    QDomNode findDirectChild(const QString &childName);

    /**
  * Finds the first child node with the tag name @p childName direct child of the @p ancestor node.
  * @param childName name of thechild node tag to look up.
  * @param ancestor starting node from which to look up for the node.
  * @return the first node starting from @p ancestor having @p tagName as tag name.
  */
    QDomNode findDirectChild(const QString &childName, const QDomNode &ancestor);

    /**
  * Finds the first child node with the tag name @p childName direct child of the @p ancestor node.
  * This node has to contain a direct child node with the tag name @p grandChildName with a text
  * value of @p value.
  * @param childName name of the child node tag to look up.
  * @param grandChildName name of the grandchild node tag to look up.
  * @param value value of the textNode contained in @p grandChildName.
  * @param ancestor starting node from which to look up for the child node.
  * @return the first child node corresponding to the criteria.
  */
    QDomNode findDirectChild(const QString &childName, const QString &grandChildName, const QString &value, const QDomNode &ancestor);

    /** Sets the map of cluster user information.
    * @param pGroup the electrod group id for which the cluster user information have to be retrieved (corresponds to the 	currently opened cluster file).
    * @param clusterUserInformationMap map given the cluster user information, the key is the cluster id and the value an instance of ClusterUserInformation.
    * @return true if the modification succeded, false otherwise.
  */
    bool setClusterUserInformation (int pGroup,QMap<int,ClusterUserInformation>& clusterUserInformationMap);


private:

    /**The session document.*/
    QDomDocument doc;

    /**The root element.*/
    QDomNode root;
    
    /**The acquisition system element.*/
    QDomNode acquisitionSystem;

    /**The spike description element.*/
    QDomNode spikeDetection;
    
    /**The units element.*/
    QDomNode units;

    /**The parameter file as it has been loaded.*/
    QString initialXmlDocument;

    static const QString parameterVersion;

};

#endif
