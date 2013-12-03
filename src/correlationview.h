/***************************************************************************
                          correlationview.h  -  description
                             -------------------
    begin                : Fri Sep 26 2003
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

#ifndef CORRELATIONVIEW_H
#define CORRELATIONVIEW_H

//include files for the application
#include "zoomwindow.h"
#include "viewwidget.h"
#include "pair.h"
#include "data.h"

// include files for QT
#include <QPainter>
#include <qstyle.h>
#include <QPixmap>
#include <qtimer.h>
#include <QList>


#include <QResizeEvent>
#include <QMouseEvent>


class KlustersDoc;
class KlustersView;
class CorrelationThread;

/**
  * View displaying auto- and cross-correlations of all selected clusters.
  * Using the text boxes in the Parameter Bar, the user can modified the bin size and duration
  * used to compute the correlograms.
  * All modification request is sent directly to the KlustersDoc object and the view
  * is automatically updated via KlustersView when the correlograms have been changed.
  * If the view is active, it is also automatically updated when clusers are changed.
  *@author Lynn Hazan
  */

class CorrelationView : public ViewWidget  {
    Q_OBJECT

public:

    friend class CorrelationThread;
    
    CorrelationView(KlustersDoc& doc, KlustersView& view, const QColor &backgroundColor, QStatusBar * statusBar, QWidget* parent=0, Data::ScaleMode scale = Data::MAX,
                    int binSize = 0, int correlationTimeFrame = 0, bool shoulderLine = false, const char* name=0,
                    int minSize = 50, int maxSize = 4000, int windowTopLeft = -500,
                    int windowBottomRight = 1001, int border = 0);
    ~CorrelationView();

    /**Signals that the widget is about to be deleted.*/
    void willBeKilled();

    /** Returns the size of the bins to use in the correlograms, given in miliseconds.
  *@return size of the bins.
  */
    int getBinSize()const{return binSize;}
    
    /** Returns the time frame use to compute the correlograms, given in miliseconds.
  *@return  time frame.
  */
    int getTimeWindow()const{return timeWindow;}

    /**Returns the type of scale used to present the correlation data.
  * @return type of scale.
  */
    Data::ScaleMode getScaleMode() const {return scaleMode;}

    /**Returns a boolean indicating if a doted line is drawn at the shoulder level
  * of the correlograms.
  * @return true if a line is drawn, false otherwise.
  */
    bool isShoulderLine() const {return shoulderLine;}

public Q_SLOTS:

    /**Updates the view only for one cluster for which the color has been changed
  * @param clusterId cluster Id for which the color have changed.
  * @param active true if the view is the active one, false otherwise.
  */
    void singleColorUpdate(int clusterId,bool active);

    /**
  * Draws an additional cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
  */
    void addClusterToView(int clusterId,bool active);

    /**
  * Removes a cluster from those already shown. Which impose to redraw everything
  * @param clusterId cluster Id to remove.
  * @param active true if the view is the active one, false otherwise.
  */
    void removeClusterFromView(int clusterId,bool active);

    /**
  * Adds a newly created cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param fromClusters list of clusters from which the spikes of the new cluster are coming.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
 */
    void addNewClusterToView(QList<int>& fromClusters,int clusterId,bool active);

    /**
  * Adds a newly created cluster to those already shown.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to add to the clusters already drawn
  * @param active true if the view is the active one, false otherwise.
  */
    void addNewClusterToView(int clusterId,bool active){addClusterToView(clusterId,active);}

    /**
  * Updates the content of the widget due to the removal of spikes in a cluster.
  * This method aims to reduce the number of clusters to draw.
  * @param fromClusters list of clusters from which the spikes have been taken.
  * @param active true if the view is the active one, false otherwise.
  */
    void spikesRemovedFromClusters(QList<int>& fromClusters,bool active);

    /**
  * Updates the content of the widget due to the addition of spikes in a cluster.
  * This method aims to reduce the number of clusters to draw.
  * @param clusterId cluster Id to which the spikes have been added
  * @param active true if the view is the active one, false otherwise.
  */
    void spikesAddedToCluster(int clusterId,bool active);

    /**Changes the current mode, call by a selection of a tool
  * @param selectedMode new mode of drawing (selection or zoom)
  */
    void setMode(BaseFrame::Mode selectedMode){}

    /**Sets the mode of presentation to raw mode, meaning that the value of each bin in a
  * correlogram will be the computed value.
  */
    void setNoScale();

    /**Sets the mode of presentation to raw mode, meaning that the value of each bin in a
  * correlogram will be the computed value scale by the maximum value.
  */
    void setMaximumScale();

    /**Sets the mode of presentation to raw mode, meaning that the value of each bin in a
  * correlogram will be the computed value scale by the shoulder value.
  */
    void setShoulderScale();

    /**Changes the size of the bins and the size of the time frame used to compute the correlograms.*/
    void setBinSizeAndTimeWindow(int size,int width);

    /**Increase of the amplitude of the correlograms.
  */
    void increaseAmplitude();

    /**Decrease of the amplitude of the correlograms.
  */
    void decreaseAmplitude();

    /**Enables the caller to know if there is any thread running launch by the view.*/
    bool isThreadsRunning() const;

    /**
  * Update the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method is call only during an undo otherwise the updateClusters is call.
  * There are 2 functions in order to reduce the number of clusters to draw whenever possible.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  */
    void undoUpdateClusters(QList<int>& modifiedClusters,bool active){
        spikesRemovedFromClusters(modifiedClusters,active);
    }

    /**
  * Update the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method aims to reduce the number of clusters to draw.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  * @param isModifiedByDeletion true if the clusters of @p modifiedClusters have been modified
  * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
  */
    void updateClusters(QList<int>& modifiedClusters,bool active,bool isModifiedByDeletion){
        spikesRemovedFromClusters(modifiedClusters,active);
    }

    /**
  * Update the presentation of a doted line at the shoulder level.
  * If @p b is true a line will be drawn, none will be drawn otherwise.
  * @param b boolean indicating if a shoulder line has to be drawn.
  */
    void setShoulderLine(bool b);

    /**Update the information presented in the view if need it.*/
    void updateDrawing();

    /**
 * Update the information presented in the view after a renumbering if need it.
 * @param active true if the view is the active one, false otherwise.
 */
    void clustersRenumbered(bool active);

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
    void paintEvent ( QPaintEvent *);

    /**Treat the events sent by the CorrelationThread instances.
  * @param event custom event.
  */
    void customEvent (QEvent* event);

    /**The view responds to a double click.
  * The correlograms are retrieve in case the data have changed (an other view has changed its parameters)
  * as all the views are sharing the same data.
  * @param event mouse event.
  */
    void mouseDoubleClickEvent (QMouseEvent* event);

    /**The view responds to a mouse click.
  * The correlograms are retrieve in case the data have changed (an other view has chaneg its parameters.
  * @param event mouse release event.
  */
    void mouseReleaseEvent(QMouseEvent* event);
    
    /**The view responds to a resize event.
  * The correlograms are retrieve in case the data have changed (an other view has changed its parameters)
  * as all the views are sharing the same data.
  * @param event resize event.
  */
    void resizeEvent(QResizeEvent* event);

    /**The view responds to a mouse move event.
  * The time is display in the status bar.
  * @param event mouse move event.
  */
    void mouseMoveEvent(QMouseEvent* event);

private:

    /**Type of scale used, raw, scale by the maximum, or scale by the shoulder.
  * The default is raw mode.*/
    Data::ScaleMode scaleMode;

    /**Creates a thread which will get the correlations information for
  * the pairs of clusters contained in @p pairsToCompute due to the clusters in @p clusterIds.
  * @param pairsToCompute couple of clusters for which a correlogram has to be obtained.
  * @param clusterIds clusters for which the correlograms will be computed.
  */
    CorrelationThread* getCorrelations(QList<Pair>* pairsToCompute, const QList<int> &clusterIds);

    /**
 * Draws the correlograms of the pair of clusters in the list @p pairList on the given painter.
 * @param painter painter on which to draw the correlograms.
 * @param pairList list of pair of clusters for which a correlogram has to be drawn.
 */
    void drawCorrelograms(QPainter& painter,QList<Pair>& pairList);

    /**Updates the dimension of the window.*/
    void updateWindow();

    /**
 * Asks the correlograms for all the clusters currently shown by launching a CorrelationThread.
 */
    void askForCorrelograms();

    /**Draws the clusters identifiers.
 * @param painter painter on which to draw the information.
 */
    void drawClusterIds(QPainter& painter);

    //Members

    /**List of pointers on the threads which have to be suppress when this object is destroy.*/
    QList<CorrelationThread*> threadsToBeKill;

    /**True if the correlation information needed to draw the correlograms are available.*/
    bool dataReady;

    /**List of pairs of clusters for which a correlogram has to be drawn.*/
    QList<Pair> pairs;

    /**size of the bins to use in the correlograms, given in miliseconds */
    int binSize;

    /**Time frame use to compute the correlograms, given in miliseconds.*/
    int timeWindow;

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

    /**The width of a bins.*/
    int binWidth;

    /**Abscissa space between two correlograms.*/
    uint Xspace;

    /**Ordinate space between two correlograms*/
    uint Yspace;

    /**Size in pixels corresponding to the maximale value of a correlogram.*/
    uint YsizeForMaxAmp;

    /**Factor use to calculate the ordinate value to been drawn.
 * The factor equals YsizeForMaxAmp multipled by a zoom factor.
 */
    float Yfactor;

    /**The delta between the starting abscisses of two correlograms.*/
    int shift;

    /**Number of bins per correlogram (2k + 1).*/
    int nbBins;

    /**List of pairs corresponding to clusters to update.*/
    QList<Pair> pairUpdateList;

    /**Boolean indicating if a shoulder line has to be drawn on the correlograms.*/
    bool shoulderLine;

    /**Step in pixels between two tick marks.*/
    float tickMarkStep;

    /**Number of tick marks in a time window*/
    int nbTickMarks;

    /**Abscissa of the tick mark for the center bin (time = 0)*/
    int tickMarkZero;

    /**Map of firing rate for the autocorrelograms.*/
    QMap<int,QString> firingRates;

    /**True if the view has been zoomed, false ohterwise.*/
    bool isZoomed;

    /**True if the widget is about to be deleted, false otherwise.*/
    bool goingToDie;

    /**Border on the left and right sides inside the window (QRect corresponding
 * to the part of the drawing which will actually be drawn onto the widget).*/
    static const int XMARGIN;

    /**Border on the top and bottom sides inside the window (QRect corresponding
 * to the part of the drawing which will actually be drawn onto the widget).*/
    static const int YMARGIN;

    /*True if the view is currently been printed, false otherwise.**/
    bool printState;

    /**The region representing the area to print.*/
    QRegion printRegion;

};

#endif
