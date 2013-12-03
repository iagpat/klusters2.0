/***************************************************************************
                          errormatrixview.h  -  description
                             -------------------
    begin                : Mon Jan 5 2004
    copyright            : (C) 2004 by Lynn Hazan
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

#ifndef ERRORMATRIXVIEW_H
#define ERRORMATRIXVIEW_H

// include files for Qt
#include <QWidget>
#include <qmap.h>
#include <QColor>
#include <QList>

#include <QResizeEvent>

#include <QMouseEvent>



// application specific includes
#include <viewwidget.h>
#include "array.h"
#include "pair.h"

// forward declaration
class KlustersDoc;
class KlustersView;
class ErrorMatrixThread;

/**
  * View displaying the Error Matrix. Each element in the matrix
  * indicates how likely it is that the two clusters corresponding to the row and column
  * of the element contain spikes from the same neuron. This view is part of the Grouping
  * Assistant View which contains also a ClusterView, a WaveformView and a CorrelationView.
  * A click on one of the elements of the matrix display the corresponding clusters in the others views.
  * This view is not automatically updated when the clusters are changed.
  *@author Lynn Hazan
  * @since klusters 1.1
  */

class ErrorMatrixView : public ViewWidget  {
    Q_OBJECT

public:

    friend class ErrorMatrixThread;

    explicit ErrorMatrixView(KlustersDoc& doc, KlustersView& view, const QColor &backgroundColor, QStatusBar* statusBar, QWidget *parent=0, const char* name=0, int minSize = 50, int maxSize = 4000, int windowTopLeft = -500,
                    int windowBottomRight = 1001, int border = 0);
    ~ErrorMatrixView();

    /**Signals that the widget is about to be deleted.*/
    void willBeKilled();

public Q_SLOTS:

    /**Enables the caller to know if there is any thread running launch by the Widget.*/
    bool isThreadsRunning() const;

    /**Update the error matrix.*/
    void updateMatrixContents();

    /**Updates the error matrix drawing by adding a red border
  * if the rearrangement of clusters have modified clusters presented in the matrix.
  * @param groupedClusters list of clusters having been grouped.
  * @param newClusterId the id of the cluster created by the grouping of those specified in @p groupedClusters
  */
    void clustersGrouped(QList<int>& groupedClusters, int newClusterId);

    /**Updates the error matrix drawing by adding a red border
  * if the deletion of clusters have impacted clusters presented in the matrix.
  * @param deletedClusters list of clusters having been deleted
  * @param destinationCluster cluster with which the clusters in @p deletedClusters are merge
  * (cluster 0 or 1 cluster of artefact and cluster of noise respectively).
  */
    void clustersDeleted(QList<int>& deletedClusters,int destinationCluster);

    /**
  * Updates the error matrix drawing by adding a red border if spikes have been
  * removed from clusters presented in the matrix.
  * @param fromClusters list of clusters from which the spikes have been taken.
  * @param destinationClusterId cluster id to which the spikes have been added
  * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
  */
    void removeSpikesFromClusters(QList<int>& fromClusters, int destinationClusterId,QList<int>& emptiedClusters);

    /**
  * Updates the error matrix drawing by adding a red border if the clusters presented in the matrix
  * have been modified to create the new cluster.
  * @param fromClusters list of clusters from which the spikes of the new cluster are coming.
  * @param clusterId cluster to add to the clusters already drawn
  * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
  */
    void newClusterAdded(QList<int>& fromClusters,int clusterId,QList<int>& emptiedClusters);

    /**
  * Updates the error matrix drawing by adding a red border if the clusters presented in the matrix
  * have been modified to create the new clusters.
  * @param fromToNewClusterIds map where the keys are ids of the clusters which really contained spikes in the region
  * and the values are the ids of the newly created clusters.
  * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
  */
    void newClustersAdded(QMap<int,int>& fromToNewClusterIds,QList<int>& emptiedClusters);

    /**
  * Updates the error matrix drawing by adding a red border if the clusters presented in the matrix
  * have been modified to create the new clusters.
  * @param clustersToRecluster list of clusters automatically reclustered.
  */
    void newClustersAdded(QList<int>& clustersToRecluster);

    /**
  * Updates the error matrix drawing by adding a red border due to an renumbering of the cluster ids.
  * @param clusterIdsOldNew map given for each old clusterId the new clusterId.
  */
    void renumber(QMap<int,int>& clusterIdsOldNew);

    /**Updates the error matrix drawing due to the reversion of the last renumbering action.
  * @param clusterIdsNewOld map given for each new clusterId the old clusterId.
  */
    void undoRenumbering(QMap<int,int>& clusterIdsNewOld);

    /**Updates the error matrix drawing due to reversion of the last user action.
  * @param addedClusters list of clusters which were added.
  * @param updatedClusters list of clusters which were modified.
  */
    void undoAdditionModification(QList<int>& addedClusters,QList<int>& updatedClusters);

    /**Updates the error matrix drawing due to reversion of the last user action.
  * @param addedClusters list of clusters which were added.
  */
    void undoAddition(QList<int>& addedClusters);

    /**Updates the error matrix drawing due to reversion of the last user action.
  * @param updatedClusters list of clusters which were modified.
  */
    void undoModification(QList<int>& updatedClusters);

    /** Updates the error matrix drawing by adding a red border.
  * @param clusterIdsOldNew map given for each old clusterId the new clusterId.
  */
    void redoRenumbering(QMap<int,int>& clusterIdsOldNew);

    /**Updates the error matrix drawing by adding a red border if the reversion of
  * the last undo action has modified any of the clusters presented in the error matrix.
  * @param addedClusters list of clusters which were added
  * @param modifiedClusters list of clusters which were modified
  * @param isModifiedByDeletion true if the clusters of @p updatedClusters have been modified
  * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
  * @param deletedClusters list of clusters which were deleted.
  */
    void redoAdditionModification(QList<int>& addedClusters,QList<int>& modifiedClusters,bool isModifiedByDeletion,QList<int>& deletedClusters);

    /**Updates the error matrix drawing by adding a red border if the reversion of
  * the last undo action has modified any of the clusters presented in the error matrix.
  * @param addedClusters list of clusters which were added
  * @param deletedClusters list of clusters which were deleted.
  */
    void redoAddition(QList<int>& addedClusters,QList<int>& deletedClusters);

    /**Updates the error matrix drawing by adding a red border if the reversion of
  * the last undo action has modified any of the clusters presented in the error matrix.
  * @param updatedClusters list of clusters which were modified
  * @param isModifiedByDeletion true if the clusters of @p updatedClusters have been modified
  * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
  * @param deletedClusters list of clusters which were deleted.
  */
    void redoModification(QList<int>& updatedClusters,bool isModifiedByDeletion,QList<int>& deletedClusters);

    /**Updates the error matrix drawing by adding a red border if the reversion of
  * the last undo action has modified any of the clusters presented in the error matrix.
  * @param deletedClusters list of clusters which were deleted.
  */
    void redoDeletion(QList<int>& deletedClusters);

    /**Prints the currently display information on a printer via the painter @p printPainter.
  * @param printPainter painter on a printer.
  * @param metrics object providing information about the printer.
  * @param whiteBackground true if the printed background has to be white, false otherwise.
  */
    void print(QPainter& printPainter,int width,int height, bool whiteBackground);

protected:
    /**
  * Draws the contents of the frame
  * @param p painter used to draw the contents
  */
    void paintEvent ( QPaintEvent*);

    /**Treat the events sent by the groupAssistantThread instances*/
    void customEvent(QEvent* event);

    inline void resizeEvent(QResizeEvent* event){
        //Trigger parent event
        ViewWidget::resizeEvent(event);
    }

    inline void mousePressEvent(QMouseEvent* event){}
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    inline void mouseDoubleClickEvent(QMouseEvent* event){}

private:
    /**Color map use to represent the probabilities of the error matrix.*/
    QMap<int,QColor> colorMap;
    /**List of the clusters present in the error matrix.*/
    QList<int> clusterList;
    /**List of the clusters present in the error matrix which have a probability calculated.*/
    QList<int> computedClusterList;
    /**List of the clusters present in the error matrix which do not have a probability calculated.*/
    QList<int> ignoreClusterIndex;
    /**Error matrix.*/
    Array<double>* probabilities;
    /**List of the clusters which have been modified since the last computation of the errror matrix.*/
    QList<int> modifiedClusterList;

    /**Map of the clusters deleted.*/
    QMap<int, QList<int> > deletedMap;

    /**Minimal abscissa in window coordinate*/
    long abscissaMin;

    /**Maximal abscissa in window coordinate*/
    long abscissaMax;

    /**Minimal ordinate in window coordinate*/
    long ordinateMin;

    /**Maximal ordinate in window coordinate*/
    long ordinateMax;

    /**The width border*/
    uint widthBorder;

    /**The height border*/
    uint heightBorder;

    /**The width of a cell of the error matrix.*/
    int cellWidth;

    /**List of pointers on the threads which have to be suppress when this object is destroy.*/
    QList<ErrorMatrixThread*> threadsToBeKill;

    /**True if the probabilities are available.*/
    bool dataReady;

    /**Number of colors used for the errror matrix.*/
    int nbColors;

    /**Highest probability for which there will be a distinct color.*/
    float cutoffProbability;

    /**True if it is the first computation.*/
    bool init;

    /**True if the clusters have been renumbered, false otherwise.*/
    bool hasBeenRenumbered;

    /**Number of action made between 2 update of the error matrix.*/
    int nbActions;

    /**Number of redo action possible (equals to the number of undo action made
  * after the update of the matrix).*/
    int nbRedo;

    /**True if the matrix is not up to date due to an action made before the update of the matrix.*/
    bool isNotUpToDate;

    /**Number of undo action possible (equals to the number of action made
  * before the update of the matrix).*/
    int nbPreviousUndo;

    /**Number of redo action possible (equals to the number of undo action made
  * before the update of the matrix).*/
    int nbPreviousRedo;

    /**True if the widget is about to be deleted, false otherwise.*/
    bool goingToDie;

    /**List of the selected pairs.*/
    QList<Pair> selectedPairs;

    /**Map keepipng track of the renumbering done on the data.*/
    QMap<int,bool> renumbering;

    //Methods

    /**Launches a ErrorMatrixThread to comput the error matrix.*/
    ErrorMatrixThread* computeMatrix();

    /**Updates the dimensions of the window.*/
    void updateWindow();

    /**
  * Draws the error matrix on the given painter.
  * @param painter painter on which to draw the error matrix
  */
    void drawMatrix(QPainter& painter);

    /**Draws the clusters identifiers.
  * @param painter painter on which to draw the information
  */
    void drawClusterIds(QPainter& painter);

    /**Initialize the internal colorMap use to represents the probabilities of the error matrix.*/
    void initializeColorMap();
};

#endif
