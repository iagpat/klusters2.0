/***************************************************************************
                          waveformview.h  -  description
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

#ifndef WAVEFORMVIEW_H
#define WAVEFORMVIEW_H

// include files for QT
#include <QPainter>
#include <qstyle.h>
#include <QPixmap>
#include <QList>


#include <QResizeEvent>
#include <QMouseEvent>

//include files for the application
#include "zoomwindow.h"
#include "viewwidget.h"



// forward declaration
class KlustersDoc;
class KlustersView;
class WaveformThread;

/**
  * View displaying the waveforms of a subset of the spikes evenly
  * spaced in time over the whole recording.
  * Using the spin boxes in the Parameter Bar, the user can specify which spikes to use.
  * All modification request is sent directly to the KlustersDoc object and the view
  * is automatically updated via KlustersView when the waveforms have been changed.
  * If the view is active, it is also automatically updated when clusers are changed.
  *@author Lynn Hazan
  */

class WaveformView : public ViewWidget  {
    Q_OBJECT

public:

    friend class WaveformThread;

    WaveformView(KlustersDoc& doc, KlustersView& view, const QColor &backgroundColor, int acquisitionGain, const QList<int> &positions, QStatusBar * statusBar, QWidget* parent=0,
                 bool isTimeFrameMode = false, long start = 0, long timeFrameWidth = 0, long nbSpkToDisplay =0, bool overLay = false, bool mean = false,
                 const char* name=0, int minSize = 50, int maxSize = 4000, int windowTopLeft = -500,
                 int windowBottomRight = 1001, int border = 0);
    ~WaveformView();

    /**
  * String indicating in which presentation mode the user is (sample, time frame).
  */
    enum PresentationMode{SAMPLE=1,TIME_FRAME=2};

    /**Signals that the widget is about to be deleted.*/
    void willBeKilled();

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
    inline void addNewClusterToView(int clusterId,bool active){addClusterToView(clusterId,active);}

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
    inline void setMode(BaseFrame::Mode selectedMode){}

    /**
  * Updates the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method aims to reduce the number of clusters to draw.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  * @param isModifiedByDeletion true if the clusters of @p modifiedClusters have been modified
  * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
  */
    inline void updateClusters(QList<int>& modifiedClusters,bool active,bool isModifiedByDeletion){
        spikesRemovedFromClusters(modifiedClusters,active);
    }

    /**
  * Updates the clusters which have been modified by the suppression of spikes
  * (used to create a new cluster or simply move to the cluster of noise or artefact).
  * This method is call only during an undo otherwise the updateClusters is call.
  * There are 2 functions in order to reduce the number of clusters to draw whenever possible.
  * @param modifiedClusters list of clusters from which spikes were taken from.
  * @param active true if the view is the active one, false otherwise.
  */
    inline void undoUpdateClusters(QList<int>& modifiedClusters,bool active){
        spikesRemovedFromClusters(modifiedClusters,active);
    }

    /** Sets the way of presenting the information concerning the waveforms selected to
  * only show the waveforms of the mean and the standard deviation.
  */
    void setMeanPresentation();

    /** Sets the way of presenting the information concerning the waveforms selected to
  * show all the waveforms corresponding to the mode of presentation.
  */
    void setAllWaveformsPresentation();

    /** The waveforms of each cluster are overlaying.
  */
    void setOverLayPresentation();

    /** The waveforms of each cluster are presented side by side.
  */
    void setSideBySidePresentation();

    /**Sets the mode of presentation to sample mode, meaning that, for each shown cluster,
  * only one out of the number of spikes to be displayed will be shown.
  */
    void setSampleMode();

    /**Sets the mode of presentation to time frame mode, meaning that, for each shown cluster,
  * only the spikes within the current time frame will be shown.
  */
    void setTimeFrameMode();

    /**Changes the time and update the view accordingly.
  * @param start start time of the time frame in second.
  * @param width width of the time frame in second.
  */
    void setTimeFrame(long start, long width);

    /**Sets the initial maximum amplitude of the waveforms.
  * @param gain the new value to set the initial maximum amplitude of the waveforms (before increase or decrease).
 */
    inline void setGain(int gain){
        this->acquisitionGain = acquisitionGain;
        gain = 0;
        Yfactor = static_cast<float>(YsizeForMaxAmp)/static_cast<float>(acquisitionGain);

        //Everything has to be redraw
        drawContentsMode = REDRAW ;
    }

    /**Increase of the amplitude of the waveforms.
  */
    void increaseAmplitude();

    /**Decrease of the amplitude of the waveforms.
  */
    void decreaseAmplitude();

    /**Increase of the number of samples display in the sample mode.
  */
    void setDisplayNbSpikes(long nbSpikes);

    /**Enables the caller to know if there is any thread running launch by the view.*/
    bool isThreadsRunning() const;

    /**Update the information presented in the view if need it.*/
    void updateDrawing();

    /**Initialize the position of the channels in the view.
 * @param positions positions of the channels to use in the view set by the user in the settings dialog.
 */
    inline void setChannelPositions(QList<int>& positions){
        delete []channelPositions;
        //nbchannels = positions.size(), this has been check in the calling functions.
        channelPositions = new int[nbchannels];
        for(int i = 0; i < nbchannels; ++i)
            channelPositions[i] = positions[i];

        //Everything has to be redraw
        drawContentsMode = REDRAW;
    }

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
  * Draws the contents of the frame.
  * @param p painter used to draw the contents of the frame.
  */
    void paintEvent ( QPaintEvent *);

    /**Treat the events sent by the WaveformThread instances*/
    void customEvent(QEvent* event);

    /**The view responds to a double click.
  * The waveforms are retrieve in case the data have changed (an other view has changed its parameters)
  * as all the views are sharing the same data.
  * @param event mouse event.
  */
    void mouseDoubleClickEvent (QMouseEvent* event);

    /**The view responds to a mouse click.
  * The waveforms are retrieve in case the data have changed (an other view has changed its parameters)
  * as all the views are sharing the same data.
  * @param event mouse release event.
  */
    void mouseReleaseEvent(QMouseEvent* event);

    /**The view responds to a resize event.
  * The waveforms are retrieve in case the data have changed (an other view has changed its parameters)
  * as all the views are sharing the same data.
  * @param event resize event.
  */
    void resizeEvent(QResizeEvent* event);
    
private:
    //members

    /**The time maximum in second.*/
    long maximumTime;

    /**Mode of presentation used, spike sample or time frame. The default is sample mode.*/
    PresentationMode presentationMode;

    /**
  * Boolean indicating if all the waveforms, corresponding to the mode of presentation,
  * are shown or if only the mean and the standard deviation.
  */
    bool meanPresentation;

    /**
  * Boolean indicating if the waveforms for the presented clusters have to overlay.
  */
    bool overLayPresentation;

    /**Minimal abscissa in window coordinate*/
    long abscissaMin;

    /**Maximal abscissa in window coordinate*/
    long abscissaMax;

    /**Minimal ordinate in window coordinate*/
    long ordinateMin;

    /**Maximal ordinate in window coordinate*/
    long ordinateMax;

    /**Number of points used to describe a waveform.*/
    int nbSamplesInWaveform;

    /**Position of the peak among the points decribing the waveform.*/
    int peakPositionInWaveform;

    /**Number of electrodes used in that experiment.*/
    int nbchannels;

    /**The width border*/
    int widthBorder;

    /**The height border*/
    int heightBorder;

    /**The delta between the starting abscisses of two clusters.*/
    int shift;

    /**Abscissa step between two points of a given spike.*/
    int Xstep;

    /**Abscissa space between two clusters.*/
    int Xspace;

    /**Ordinate space between two electrodes.*/
    int Yspace;

    /**The abscissa of the system coordinate center for the channel
  * which is presented at the top of the view.*/
    long X0;

    /**The ordinate of the system coordinate center for the channel
  * which is presented at the top of the view.*/
    long Y0;

    /**Acquisition system gain.*/
    int acquisitionGain;

    /**The actual gain is 0.75 raised to the power of gain times
  * the acquisition system gain.
  */
    int gain;
    
    /**Size in pixels corresponding to the amplitude maximal.*/
    int YsizeForMaxAmp;

    /**Factor use to calculate the ordinate value to been drawn.
  * The factor equals YsizeForMaxAmp divided by gain.
  */
    float Yfactor;

    /**Index positions of the channels*/
    int* channelPositions;

    /**When the presentation mode is time frame, this variable keeps track of
  * the current start time of the time window.*/
    long startTime;

    /**When the presentation mode is time frame, this variable keeps track of
  * the current end time of the time window.*/
    long endTime;

    /**True is the data where recording using a 12 or 16 bits recording system which
  * gives data coded on 2 bytes, false otherwise (the recording is then assume to be 32 bits
  * and then the data are coded on 4 bytes.*/
    bool isTwoBytesRecording;
    
    /**List of pointers on the threads which have to be suppress when this object is destroy.*/
    QList<WaveformThread*> threadsToBeKill;

    /**True if the waveform information needed to draw the waveforms are available.*/
    bool dataReady;

    /**List of the clusters to be disregared because they have been changed.*/
    QList<int> clustersToDisregard;

    /**The number of spikes to display in sample mode.*/
    long nbSpkToDisplay;

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

    //Functions

    /**
  * Draws the waveforms of the clusters in the list @p clustersList on the given painter
  * @param painter painter on which to draw the waveforms
  * @param clusterList list of clusters to draw
  */
    void drawWaveforms(QPainter& painter,const QList<int>& clusterList);

    /**Updates the dimension of the window.*/
    void updateWindow();

    /**Creates a thread which will get the waveform information.*/
    WaveformThread* getWaveforms();

    /**
  * Asks the waveform information for the cluster @p clusterId by launching a WaveformThread.
  * @param clusterId id of the cluster to ask waveform information for.
  */
    void askForWaveformInformation(int clusterId);

    /**
  * Asks the waveform information for the clusters listed in @p clusterIds by launching a WaveformThread.
  * @param clusterIds ids of the clusters to ask waveform information for.
  */
    void askForWaveformInformation(const QList<int>& clusterIds);


    /**Draws the clusters identifiers.
  * @param painter painter on which to draw the information
  */
    void drawClusterIds(QPainter& painter);

};

#endif
