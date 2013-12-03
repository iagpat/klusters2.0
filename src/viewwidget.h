/***************************************************************************
                          viewwidget.h  -  description
                             -------------------
    begin                : Thu Sep 25 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                : lynn.hazan@myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

// include files for Qt
#include <QWidget>

#include <QPixmap>

#include <QList>

//Inclused files for KDE
#include <QStatusBar>

//include files for the application
#include "baseframe.h"

class KlustersDoc;
class KlustersView;

/**
  * Base class for the most of the views of the application (ClusterView, WaveformView,
  * CorrelationView and ErrorMatrixView).
  * This class defineds the views common signal and slot communication with KlustersView.
  * Thoses communication enable the views to be update themselves when the clusters have been changed.
  * ViewWidget also contains the zoom feature of the views.
  *@author Lynn Hazan
  */
class ViewWidget : public BaseFrame {

    Q_OBJECT

public:

    /**
  * Constructs the view.
  * @param doc the document instance the view represents.
  * @param view the global view including this view of the data.
  * @param backgroundColor color used as background.
  * @param statusBar a reference to the application status bar.
  * @param parent the parent QWidget.
  * @param name name of the widget (can be used for introspection).
  * @param minSize minumum size of the view.
  * @param maxSize maximum size of the view.
  * @param windowTopLeft the top-left corner of the window (QRect corresponding
  * to the part of the drawing which will actually be drawn onto the widget).
  * @param windowBottomRight bottom-right corner of the window (QRect corresponding
  * to the part of the drawing which will actually be drawn onto the widget).
  * @param border size of the border between the frame and the contents.
  * @param Xborder border on the left and right sides inside the window (QRect corresponding
  * to the part of the drawing which will actually be drawn onto the widget).
  * @param Yborder border on the top and bottom sides inside the window (QRect corresponding
  * to the part of the drawing which will actually be drawn onto the widget).
  */
    explicit ViewWidget(KlustersDoc& doc, KlustersView& view, const QColor &backgroundColor, QStatusBar* statusBar, QWidget* parent=0, const char* name=0,
               int minSize = 500, int maxSize = 4000, int windowTopLeft = -500, int windowBottomRight = 1001, int border = 0, int Xborder = 0, int Yborder = 0);

    ~ViewWidget();

    /**Enum to be use as a Mode.
  * <ul>
  * <li>DELETE_NOISE Enumeration indicating that the user is in a mode enabling him to delete noisy spikes.</li>
  * <li>DELETE_ARTEFACT Enumeration indicating that the user is in a mode enabling him to delete artefact spikes.</li>
  * <li>NEW_CLUSTER Enumeration indicating that the user is in a mode enabling him to create a new cluster.</li>
  * <li>NEW_CLUSTERS Enumeration indicating that the user is in a mode enabling him to create new clusters.</li>
  * <li>SELECT_TIME Enumeration indicating that the user is in a mode enabling him to select a time window which will be display in a TraceView.</li>
  * </ul>
  */
    enum {DELETE_NOISE = ZOOM+1,DELETE_ARTEFACT = ZOOM+2,NEW_CLUSTER = ZOOM+3,NEW_CLUSTERS = ZOOM+4,SELECT_TIME = ZOOM+5};

public Q_SLOTS:

    /**
  * Takes into  account the update of the dimension used to present the clusters.
  * @param dimensionX
  * @param dimensionY
  */
    virtual void updatedDimensions(int dimensionX, int dimensionY){}

    /**Updates the view only for one cluster for which the color has been changed
  * @param clusterId cluster Id for which the color have changed.
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void singleColorUpdate(int clusterId,bool active){}

    /**
  * Adds an additional cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void addClusterToView(int clusterId,bool active){}

    /**
  * Removes a cluster from those already shown.
  * @param clusterId cluster Id to remove.
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void removeClusterFromView(int clusterId,bool active){}

    /**
  * Adds a newly created cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param fromClusters list of clusters from which the spikes of the new cluster are coming.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void addNewClusterToView(QList<int>& fromClusters,int clusterId,bool active){}

    /**
  * Adds a newly created cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void addNewClusterToView(int clusterId,bool active){}

    /**
  * Updates the content of the widget due to the removal of spikes in a cluster.
  * This method aims to reduce the number of clusters to draw.
  * @param fromClusters list of clusters from which the spikes have been taken.
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void spikesRemovedFromClusters(QList<int>& fromClusters,bool active){}

    /**
  * Updates the content of the widget due to the addition of spikes in a cluster.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to which the spikes have been added
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void spikesAddedToCluster(int clusterId,bool active){}

    /**Method called when no spikes have been found in a polygon of selection
  */
    virtual void emptySelection(){}

    /**
  * Updates the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method aims to reduce the number of clusters to draw.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  * @param isModifiedByDeletion true if the clusters of @p modifiedClusters have been modified
  * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact abd cluster of noise respectively).
  */
    virtual void updateClusters(QList<int>& modifiedClusters,bool active,bool isModifiedByDeletion){}

    /**
  * Updates the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method is called only during an undo otherwise the updateClusters is call.
  * There are 2 functions in order to reduce the number of clusters to draw whenever possible.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  */
    virtual void undoUpdateClusters(QList<int>& modifiedClusters,bool active){}
    /**Enables the caller to know if there is any thread running launch by the viewWidget.*/
    virtual bool isThreadsRunning() const{return false;}
    /**Prints the currently display information on a printer via the painter @p printPainter.
  * @param printPainter painter on a printer.
  * @param metrics object providing information about the printer.
  * @param whiteBackground true if the printed background has to be white, false otherwise.
  */
    virtual void print(QPainter& printPainter,int width,int height, bool whiteBackground){}
public: 
    //Members

    /**
  * Buffer to enable smooth updating, obtain flicker-free drawing.
  * Prevent from unnecessary redrawing.
  */
    QPixmap doublebuffer;

    /**Reference on the view containing this widget*/
    KlustersView& view;

    /** The document containing the data the widget is representing*/
    KlustersDoc& doc;

    /**List of clusters to update when the view will become the active one*/
    QList<int> clusterUpdateList;

    /**Pointer to the status bar of the application.*/
    QStatusBar* statusBar;

};


#endif
