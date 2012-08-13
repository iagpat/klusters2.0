/***************************************************************************
                          clusterPalette.h  -  description
                             -------------------
    begin                : Mon Sep  8 12:06:21 EDT 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CLUSTERPALETTE_H
#define CLUSTERPALETTE_H

//QT include files
#include <qvariant.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qiconview.h>
#include <qtooltip.h>

class KStatusBar;

//General C++ include files
#include <iostream>
using namespace std;

class QIconView;
class QIconViewItem;

// forward declaration of the KlustersDoc class
class KlustersDoc;

 /**
  * This class represents the Cluster Panel of the application.
  * It receives the user selections and triggers the actions which have to be done.
  *@author Lynn Hazan
  */


class ClusterPalette : public QVBox
{
    Q_OBJECT
    
public:
	/**
	* @param backgroundColor backgroundColor of the cluster palette.
	* @param parent the parent QWidget.
	* @param statusBar a reference to the application status bar.
	* @param name name of the widget (can be used for introspection).
	* @param wflags common QWidget flags.
	*/
	ClusterPalette(QColor backgroundColor,QWidget* parent = 0,KStatusBar * statusBar = 0, const char* name = 0, WFlags fl = 0 );
   /*
   *  Destroys the object and frees any allocated resources.
   */
   ~ClusterPalette();

    //Mode of action, in immediate the change of color
    //and the selection of cluster is immediately trigger
    enum Mode {IMMEDIATE = 1, DELAY = 2};
    
    virtual void createClusterList(KlustersDoc* doc);
    virtual void updateClusterList();
    void selectItems(QValueList<int> selectedClusters);
    virtual inline void setImmediateMode(){mode = IMMEDIATE;};
    virtual inline void setDelayMode(){mode = DELAY;};
    virtual void reset();
    /**Returns the list of selected clusters*/
    QValueList<int> selectedClusters();

	 /** Hides the user cluster information, that is show the normal cluster palette.*/
	 void hideUserClusterInformation();
	 
	 /** Shows the user cluster information, that is show a modified cluster palette presenting the cluster ids and the user cluster information.
	  * @param electrodeGroupId id of the current electrode group.
	 */
	 void showUserClusterInformation(int electrodeGroupId);

    /**updates the background color of the palette.*/
    void changeBackgroundColor(QColor color){
     backgroundColor = color;
     int h;
     int s;
     int v;
     color.hsv(&h,&s,&v);
     if(s <= 80 && v >= 240 || (s <= 40 && v >= 220)) iconView->setPaletteForegroundColor(black);
     else iconView->setPaletteForegroundColor(white);      
     iconView->setPaletteBackgroundColor(color);
     
     //get the list of selected clusters
     QValueList<int> selected = selectedClusters();
     
     //Set isInSelectItems to true to prevent the emission of signals due to selectionChange
     isInSelectItems = true;

     //Redraw the icons
     updateClusterList();
     
     //reselect the clusters
     selectItems(selected);
     
     //reset isInSelectItems to false to enable again the the emission of signals due to selectionChange
     isInSelectItems = false;

     update(); 
    }

public slots:
    virtual void changeColor(QIconViewItem* item);
    virtual void moveClustersToNoise();
    virtual void moveClustersToArtefact();
    virtual void groupClusters();
    virtual void updateClusters();

protected slots:
	/** The right click on a cluster icon bring a dialog allowing the user to enter information on the cluster
	* (structure, type, isolation distance, quality and notes).
	*/
    virtual void slotRightPressed(QIconViewItem* item);
    virtual void slotMousePressed(int button,QIconViewItem* item);
    virtual void slotClickRedraw();
    virtual void languageChange();
	 /**
	 * When moving the mouse over an cluster icon, the statusBar is updated with the information the user might could provided
	  * (structure, type, isolation distance, quality and notes).
	 */
	 virtual void slotOnItem(QIconViewItem* item);
    
signals:
    void singleChangeColor(int selectedCluster);
    void updateShownClusters(QValueList<int> selectedClusters);
    void groupClusters(QValueList<int> selectedClusters);
    void moveClustersToNoise(QValueList<int> selectedClusters);
    void moveClustersToArtefact(QValueList<int> selectedClusters);
	 void clusterInformationModified();    
	
private:
    QIconView* iconView;
    KlustersDoc* doc;

    Mode mode;//default IMMEDIATE

    /**Prevent from emitting signal while globaly selecting items*/
    bool isInSelectItems;

    /**Prevent from emitting a signal of update if the selection had not changed.*/
    bool isUpToDate;
    
    /**Current palette background Color.*/
    QColor backgroundColor;

	 /**Pointer to the status bar of the application.*/
	 KStatusBar* statusBar;

	 /**Allows to update correctly the cluster text.*/
	 bool isInUserClusterInfoMode;

};

#endif // CLUSTERPALETTE_H
