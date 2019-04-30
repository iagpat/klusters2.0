/***************************************************************************
                          waveformview.cpp  -  description
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

//include files for the application
#include "klustersview.h"
#include "klustersdoc.h"
#include "data.h"
#include "itemcolors.h"
#include "waveformview.h"
#include "waveformthread.h"
#include "types.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// include files for Qt
#include <qpaintdevice.h>
#include <QPolygon>
#include <qcursor.h>


#include <QList>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>

const int WaveformView::XMARGIN = 0;
const int WaveformView::YMARGIN = 0;

WaveformView::WaveformView(KlustersDoc& doc,KlustersView& view,const QColor& backgroundColor,int acquisitionGain,const QList<int>& positions,QStatusBar * statusBar,QWidget* parent,
                           bool isTimeFrameMode,long start,long timeFrameWidth,long nbSpkToDisplay, long batchIteration,
                           bool overLay,bool mean, const char* name,int minSize, int maxSize, int windowTopLeft ,int windowBottomRight,
                           int border) :
    ViewWidget(doc,view,backgroundColor,statusBar,parent,name,minSize,maxSize,windowTopLeft,windowBottomRight,border,XMARGIN,YMARGIN)
  ,meanPresentation(mean),overLayPresentation(overLay),acquisitionGain(acquisitionGain),dataReady(true),
    nbSpkToDisplay(nbSpkToDisplay), batchIteration(batchIteration),isZoomed(false),goingToDie(false){

    //Set the default modes
    mode = ZOOM;
    if(isTimeFrameMode)
        presentationMode = TIME_FRAME;
    else
        presentationMode = SAMPLE;

    //Set the drawing variables
    Data& clusteringData = doc.data();
    nbSamplesInWaveform = clusteringData.nbOfSampleInWaveform();
    peakPositionInWaveform = clusteringData.positionOfPeakInWaveform();
    nbchannels = clusteringData.nbOfchannels();
    widthBorder = 0;
    heightBorder = 20;
    Xstep = 10;
    Xspace = 30;
    shift = (nbSamplesInWaveform - 1) * Xstep + Xspace;
    Yspace = 40;
    YsizeForMaxAmp = 100;
    Yfactor = static_cast<float>(YsizeForMaxAmp)/static_cast<float>(acquisitionGain);
    gain = 0;
    voltageSelectionCursor = QCursor(QPixmap(":/cursors/delete_noise_cursor.png"),0,0);
    channelPositions = new int[nbchannels];

    ///Initialize the position of the channels (the positions will be configurable in the settings).
    //The first one in the file will be the first one (at the top), second one will be beneath and so on.
    //The channels are counted from 0 to nbchannels - 1.
    if(positions.isEmpty()) {
        for(int i = 0; i < nbchannels; ++i) {
            channelPositions[i] = i;
        }
    } else {
        for(int i = 0; i < nbchannels; ++i) {
            channelPositions[i] = positions.at(i);
        }
    }

    ordinateMin = -(2 * heightBorder + nbchannels * YsizeForMaxAmp + (nbchannels - 1) * Yspace);
    ordinateMax = 2 * Yspace;
    abscissaMin = 0;

    X0 = widthBorder;
    Y0 = nbchannels * YsizeForMaxAmp + (nbchannels - 1) * Yspace + heightBorder - (YsizeForMaxAmp/2);

    isTwoBytesRecording = clusteringData.isRecordingTwoBytes();
    startTime = start;
    endTime = start + timeFrameWidth;

    maximumTime = clusteringData.maxTime();

    updateWindow();

    //Set the cursor shap to a magnifier as the only action allowed on the widget is to zoom.
    setCursor(zoomCursor);
    setMouseTracking(true);
}

WaveformView::~WaveformView(){
    delete []channelPositions;

    //Ask the threads to stop as soon as possible.
    willBeKilled();

    //Wait until all the threads have finish before quiting otherwise
    // it may endup in a crash of the application.
    for(int i = 0; i<threadsToBeKill.count();i++) {
        WaveformThread* waveformThread = threadsToBeKill.at(i);
        while(!waveformThread->wait()){};
    }
    qDeleteAll(threadsToBeKill);
    threadsToBeKill.clear();
}

WaveformThread* WaveformView::getWaveforms(){
    return new WaveformThread(*this,doc.data());
}

bool WaveformView::isThreadsRunning() const{
    if(threadsToBeKill.count() == 0)
        return false;
    else
        return true;
}

void WaveformView::singleColorUpdate(int clusterId,bool active){
    if(active){
        //Add the the cluster id to the clusterUpdateList,
        // so it will be updated during the next update
        if(drawContentsMode == REFRESH){
            clusterUpdateList.append(clusterId);
            drawContentsMode = UPDATE;
        }
        else if(drawContentsMode == UPDATE)
            clusterUpdateList.append(clusterId);
    }
    else{
        //Update drawContentsMode if need it.
        if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)
            drawContentsMode = REDRAW;
    }
}

void WaveformView::askForWaveformInformation(int clusterId){
    //If the widget is not about to be deleted, request the data.
    //if (batchIteration == 0) view.mainWindow.previousBatchSwitch(true);
    qDebug()<<"askForWaveformInformation - clusterID = " << clusterId;
    if(!goingToDie){
        dataReady = false;
        //Create a thread to get the waveform data for that cluster.
        WaveformThread* waveformThread = getWaveforms();
        threadsToBeKill.append(waveformThread);
        waveformThread->getWaveformInformation(clusterId,presentationMode);
    }
}

void WaveformView::askForWaveformInformation(const QList<int> &clusterIds){
    qDebug()<<"askForWaveformInformation - clusterIDs size = " << clusterIds.size();
    //If the widget is not about to be deleted, request the data.
    if(!goingToDie){
        dataReady = false;
        //Create a thread to get the waveform data for that clusters.
        WaveformThread* waveformThread = getWaveforms();
        threadsToBeKill.append(waveformThread);
        waveformThread->getWaveformInformation(clusterIds,presentationMode);
    }
}


void WaveformView::addClusterToView(int clusterId,bool active){
    isZoomed = false;//Hack because all the tabs share the same data.

    if(active && overLayPresentation){
        if(drawContentsMode == REFRESH){
            clusterUpdateList.append(clusterId);
            drawContentsMode = UPDATE;
        }
        else if(drawContentsMode == UPDATE)clusterUpdateList.append(clusterId);

        setCursor(Qt::WaitCursor);
        //Create a thread to get the waveform data for that cluster specifying the
        //triggering action.
        askForWaveformInformation(clusterId);
    }
    else if(active){
        //Update drawContentsMode if need it.
        if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;

        //The data have to be collected, if need it, for all the clusters.
        if(active && !view.clusters().isEmpty()){
            setCursor(Qt::WaitCursor);
            askForWaveformInformation(view.clusters());
        }
    }
    else{
        //Update drawContentsMode if need it.
        if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;
    }
}

void WaveformView::removeClusterFromView(int clusterId,bool active){ 
    isZoomed = false;//Hack because all the tabs share the same data.

    //Update drawContentsMode if need it.
    if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;

    //The data have to be collected, if need it, for all the clusters.
    if(active && !view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::addNewClusterToView(QList<int>& fromClusters,int clusterId,bool active){

    isZoomed = false;//Hack because all the tabs share the same data.

    //Update drawContentsMode if need it.
    if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;

    //The data have to be collected, if need it, for all the clusters.
    if(active && !view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}


void WaveformView::spikesRemovedFromClusters(QList<int>& fromClusters,bool active){
    isZoomed = false;//Hack because all the tabs share the same data.

    //Update drawContentsMode if need it.
    if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;

    //The data have to be collected, if need it, for all the clusters.
    if(active && !view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::spikesAddedToCluster(int clusterId,bool active){  
    isZoomed = false;//Hack because all the tabs share the same data.

    //Update drawContentsMode if need it.
    if(drawContentsMode == REFRESH || drawContentsMode == UPDATE)drawContentsMode = REDRAW;

    //The data have to be collected, if need it, for all the clusters.
    if(active && !view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::customEvent(QEvent *event){
    //Event sent by a Waveform2Thread to inform that the data are available.
    if(event->type() == QEvent::User + 200){
        WaveformThread::GetWaveformsEvent* waveformsEvent = (WaveformThread::GetWaveformsEvent*) event;
        //Get the event information
        WaveformThread* waveformThread = waveformsEvent->parentThread();
        bool meanRequested = waveformThread->isMeanRequested();

        //Wait to be sure the thread has return from his run method. Even if the send of the event is the last
        //action of the run method it seems that the event loop can be pretty fast and the run has not
        //return when the event is received here.
        while(!waveformThread->wait()){};

        //Only one cluster was concern by the thread
        if(waveformThread->isSingleTriggeringCluster()){
            //the data have be retrieved and the mean and standard deviation calculated
            if(meanPresentation || (!meanPresentation && meanRequested)){
                //Delete the waveform2Thread, this is done by removing it from threadsToBeKill as auto-deletion is enabled.
                threadsToBeKill.removeAll(waveformThread);
            }
            if(meanPresentation && !goingToDie){
                //Each time a cluster is added to the view or modified, the size of the window is recalculated.
                if(!isZoomed) updateWindow();
                else drawContentsMode = REDRAW;

                dataReady = true;
                //setCursor(zoomCursor);
                //Update the widget
                update();
            }
            //the data have be retrieved but the mean and standard deviation have not be calculated.
            if(!meanPresentation && !meanRequested){
                //If the widget is not about to be deleted, launch as thread to do calculate the mean and standard deviation.
                //And draw the waveforms2.
                //Calculating it now will speed up the next call to a mean presentation.
                if(!goingToDie){
                    waveformThread->getMean(presentationMode);
                    //Each time a cluster is added to the view or modified, the size of the window is recalculated.
                    if(!isZoomed) updateWindow();
                    else drawContentsMode = REDRAW;

                    dataReady = true;
                    //setCursor(zoomCursor);
                    //Update the widget
                    update();
                }
                else{
                    //Delete the waveform2Thread, this is done by removing it from threadsToBeKill as auto-deletion is enabled.
                    threadsToBeKill.removeAll(waveformThread);
                }
            }
        }
        //Several clusters were concern by the thread
        else{
            //the data have be retrieved and the mean and standard deviation calculated
            if(meanPresentation || (!meanPresentation && meanRequested)){
                //Delete the waveform2Thread, this is done by removing it from threadsToBeKill as auto-deletion is enabled.
                threadsToBeKill.removeAll(waveformThread);
                // setCursor(zoomCursor);
            }
            if(meanPresentation && !goingToDie){
                //Each time a cluster is added to the view or modified, the size of the window is recalculated.
                if(!isZoomed) updateWindow();
                else drawContentsMode = REDRAW;

                dataReady = true;
                //setCursor(zoomCursor);
                //Update the widget
                update();
            }
            //the data have be retrieved but the mean and standard deviation have not be calculated.
            if(!meanPresentation && !meanRequested){
                if(!goingToDie){
                    waveformThread->getMean(presentationMode);
                    //Each time a cluster is added to the view or modified, the size of the window is recalculated.
                    if(!isZoomed) updateWindow();
                    else drawContentsMode = REDRAW;

                    dataReady = true;
                    //setCursor(zoomCursor);
                    //Update the widget
                    update();
                }
                else threadsToBeKill.removeAll(waveformThread);
            }
        }
    }
    //Event sent by a Waveform2Thread to inform that the data are not available for the cluster requested.
    //Disregard the cluster.
    if(event->type() == QEvent::User + 250){
        WaveformThread::NoWaveformDataEvent* waveformsEvent = (WaveformThread::NoWaveformDataEvent*) event;
        //Get the parent thread
        WaveformThread* waveformThread = waveformsEvent->parentThread();

        //Wait to be sure the thread has return from his run method. Even if the send of the event is the last
        //action of the run method it seems that the event loop can be pretty fast and the run has not
        //return when the event is received here.
        while(!waveformThread->wait()){};

        //Delete the waveform2Thread, this is done by removing it from threadsToBeKill as auto-deletion is enabled.
        threadsToBeKill.removeAll(waveformThread);
        //setCursor(zoomCursor);
    }
}

void WaveformView::paintEvent ( QPaintEvent *){
    QPainter p(this);
    QRect r((QRect)window);
    if((drawContentsMode == UPDATE || drawContentsMode == REDRAW) && dataReady){
        QRect contentsRec = contentsRect();
        viewport = QRect(contentsRec.left(),contentsRec.top(),contentsRec.width(),contentsRec.height() /*- 10*/);

        //Resize the double buffer with the width and the height of the widget(QFrame)
        if (viewport.size() != doublebuffer.size()) {
            if(!doublebuffer.isNull()) {
                QPixmap tmp = QPixmap( viewport.width() +10 ,viewport.height() +10 );
                tmp.fill( Qt::white );
                QPainter painter2( &tmp );
                painter2.drawPixmap( 0,0, doublebuffer );
                painter2.end();
                doublebuffer = tmp;
            } else {
                doublebuffer = QPixmap(viewport.width() + 10 ,viewport.height() +10);
            }
        }


        //Create a painter to paint on the double buffer
        QPainter painter;
        painter.begin(&doublebuffer);

        //set the window (part of the world I want to show)
        QRect r((QRect)window);
        painter.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function

        //Set the viewport (part of the device I want to write on).
        //By default, the viewport is the same as the device's rectangle (contentsRec), taking a smaller
        //one will ensure that the legends (cluster ids) will not ovelap to much a waveform.
        painter.setViewport(viewport);

        if(drawContentsMode == REDRAW){
            //Fill the double buffer with the background
            doublebuffer.fill(palette().color(backgroundRole()));

            //Paint all the waveforms in the shownclusters list (in the double buffer)
            drawWaveforms(painter,view.clusters());
        }

        //The update mode applies only when the color of a cluster has changed.
        if(drawContentsMode == UPDATE){
            //Paint the waveforms for the clusters contained in clusterUpdateList
            drawWaveforms(painter,clusterUpdateList);

            //Reset the clusterUpdateList list for the next call
            clusterUpdateList.clear();
        }

        //reset transformation due to setWindow and setViewport
        painter.resetMatrix() ;

        //Draw the cluster Ids below the waveforms if they are not in overlay presentation.
        if(!overLayPresentation)
            drawClusterIds(painter);

        //Closes the painter on the double buffer
        painter.end();

        //Back to the default
        drawContentsMode = REFRESH;
    }
    p.drawPixmap(0, 0, doublebuffer);

    if(!selectionPolygon.isEmpty()) {
        p.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function
        p.setPen(Qt::green);
        p.drawPolyline(selectionPolygon);
    }

    if (mode == REMOVE_SPIKES){
        setCursor(voltageSelectionCursor);
    }
    else{
        setCursor(zoomCursor);
    }
}


void WaveformView::drawWaveforms(QPainter& painter,const QList<int>& clusterList){
    //the clusters will be presented by increasing ids, to do so the lists have to be sorted.
    QList<int> shownClusters;
    QList<int>::const_iterator iterator;
    QList<int> const clusters = view.clusters();
    listCorresponingSpikes.clear(); //Spike number of each polygon in listCOmparePolygons
    listCorresponingClusters.clear();
    listComparePolygons.clear(); //Each spike displayed in the view

    for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator)
        shownClusters.append(*iterator);

    qSort(shownClusters);

    QList<int> clusterListSorted;
    for(iterator = clusterList.begin(); iterator != clusterList.end(); ++iterator)
        clusterListSorted.append(*iterator);

    qSort(clusterListSorted);

    //Loop on the clusters to be drawn
    QList<int>::const_iterator clusterIterator;

    ItemColors& clusterColors = doc.clusterColors();
    Data& clusteringData = doc.data();

    //The abscissa of the system coordinate center for the current cluster
    int X = X0;
    //If it is an update that means that there some clusters to redraw.
    //If the presentation is side by side and the position of those clusters in the drawing
    //has to been find in order to redraw them at the correct position.
    bool specificPosition = false;
    if(drawContentsMode == UPDATE && !overLayPresentation)specificPosition = true;

    //The ordinate of the system coordinate center for the current channel of the current cluster
    int Y;

    int clusterShift;
    if(overLayPresentation) clusterShift = 0;
    else clusterShift = shift;

    for(clusterIterator = clusterListSorted.begin(); clusterIterator != clusterListSorted.end(); ++clusterIterator){
        if(specificPosition){
            int index = shownClusters.indexOf(*clusterIterator);
            X = X0 + index * shift;
        }
        //Get the color associated with the cluster and set the color to use to this color
        QColor color = clusterColors.color(*clusterIterator);
        QPen pen(color);
        painter.setPen(pen);
        //Get the iterator on the spikes of the current cluster
        Data::WaveformIterator* waveformIterator;

        if(presentationMode == SAMPLE) waveformIterator = clusteringData.sampleWaveformIterator(static_cast<dataType>(*clusterIterator),nbSpkToDisplay);
        else waveformIterator = clusteringData.timeFrameWaveformIterator(static_cast<dataType>(*clusterIterator),startTime,endTime);

        //Iterate over the waveforms of the cluster and draw them
        if(meanPresentation){
            if(!waveformIterator->isMeanAvailable()) continue;
            int x = 0;
            QPolygon mean;
            QPolygon max;
            QPolygon min;
            for(int i = 0; i < nbSamplesInWaveform; ++i){
                for(int j = 0; j < nbchannels; ++j){
                    Y = Y0 - channelPositions[j] * (YsizeForMaxAmp + Yspace);
                    long meanValue = waveformIterator->nextMeanValue();
                    //The point is drawn in the QT coordinate system where the Y axis in oriented downwards
                    //The value receive from the iterator is already inverted.
                    mean.putPoints((j*nbSamplesInWaveform) + i, 1, X + x,-Y + static_cast<long>(meanValue * Yfactor));
                    long stDeviation = waveformIterator->nextStDeviationValue();
                    min.putPoints((j*nbSamplesInWaveform) + i, 1, X + x,-Y + static_cast<long>((meanValue - stDeviation) * Yfactor));
                    max.putPoints((j*nbSamplesInWaveform) + i, 1, X + x,-Y + static_cast<long>((meanValue + stDeviation) * Yfactor));
                }
                x += Xstep;
            }
            for(int k = 0;k < nbchannels;++k){
                //Draw the mean with a solid line
                pen.setStyle(Qt::SolidLine);
                painter.setPen(pen);

                int pointCount = (nbSamplesInWaveform == -1) ?  mean.size() - k * nbSamplesInWaveform : nbSamplesInWaveform;
                painter.drawPolyline(mean.constData() + k * nbSamplesInWaveform, pointCount);

                //Draw the 2 standard deviations with a dash line
                pen.setStyle(Qt::DotLine);
                painter.setPen(pen);
                int pointCountMin = (nbSamplesInWaveform == -1) ?  min.size() - k * nbSamplesInWaveform : nbSamplesInWaveform;
                painter.drawPolyline(min.constData() + k * nbSamplesInWaveform, pointCountMin);

                int pointCountMax = (nbSamplesInWaveform == -1) ?  max.size() - k * nbSamplesInWaveform : nbSamplesInWaveform;
                painter.drawPolyline(max.constData() + k * nbSamplesInWaveform, pointCountMax);
            }
        }
        //Draw all the selected waveforms
        //The data are store as follow:
        //spike after spike and for each spike sample after sample and for each sample
        //channel after channel.
        else{
            if(!waveformIterator->areSpikesAvailable())continue;
            long nbOfSpikes = waveformIterator->nbOfSpikes();

            QList <QPolygon> redSpikes;

            long spikesIndex = 0;
            for(long i = 0; i < nbOfSpikes; ++i){
                int x = 0;
                QPolygon spike(nbchannels * nbSamplesInWaveform);
                for(int i = 0; i < nbSamplesInWaveform; ++i){
                    for(int j = 0; j < nbchannels; ++j){
                        Y = Y0 - channelPositions[j] * (YsizeForMaxAmp + Yspace);
                        //The point is drawn in the QT coordinate system where the Y axis in oriented downwards
                        //The value receive from the iterator is already inverted.
                        long temp = static_cast<long>(waveformIterator->nextSpike(static_cast<dataType>(*clusterIterator),clusteringData,spikesIndex) * Yfactor);
                        if (i==0 && j ==0){//On the first timepoint and channels of a viable spike
                            listComparePolygons.append(QVector<QPolygon>());//Add a list of polygons for this spike
                            listCorresponingSpikes.append(spikesIndex);//Save the spike index for the spike
                            listCorresponingClusters.append(static_cast<dataType>(*clusterIterator));
                        }
                        if (i==0){//On the first timepoint of each channel in each viable spike, add a new polygon
                            listComparePolygons[listComparePolygons.size()-1].append(QPolygon(nbSamplesInWaveform));
                        }
                        listComparePolygons[listComparePolygons.size()-1][j].setPoint(i, X + x,-Y + temp);//Store the point for this spike and channel
                        spike.setPoint((j*nbSamplesInWaveform) + i, X + x,-Y + temp);
                    }
                    x += Xstep;

                }

                if (listSelectedSpikes.contains(listCorresponingSpikes.at(listCorresponingSpikes.size()-1))){
                    redSpikes.append(spike);
                    continue; //do not draw if it is a red spike
                }
                /*if (listSelectedMSDSpikes.contains(listCorresponingSpikes.at(listCorresponingSpikes.size()-1))){
                    painter.setPen(Qt::red);
                } else {
                    painter.setPen(color);
                }*/
                for(int k = 0;k < nbchannels;++k){
                    int pointCount = (nbSamplesInWaveform == -1) ?  spike.size() - k * nbSamplesInWaveform : nbSamplesInWaveform;
                    painter.drawPolyline(spike.constData() + k * nbSamplesInWaveform, pointCount);
                }
            }
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.setPen(Qt::red);
            for(int k1 = 0; k1 < redSpikes.size(); k1++){
                for(int k = 0;k < nbchannels;++k){
                    int pointCount = (nbSamplesInWaveform == -1) ?  redSpikes.at(k1).size() - k * nbSamplesInWaveform : nbSamplesInWaveform;
                    painter.drawPolyline(redSpikes.at(k1).constData() + k * nbSamplesInWaveform, pointCount);
                }
            }

        }
        //Delete the waveform iterator received for the current cluster
        delete waveformIterator;
        //Reinitialize the Y,the starting ordinate
        Y = Y0 - channelPositions[0] * (YsizeForMaxAmp + Yspace);
        //Shift X to the starting abscissa of the next cluster if there is not
        //to find the specific position of the next cluster (case treated at the beging of the loop)
        if(!specificPosition)X += clusterShift;
    }
}

void WaveformView::updateWindow(){  
    int nbOfClusters = view.clusters().size();

    //Update the window if the clusters are side by side or any case if there is only one cluster.
    if(!overLayPresentation){
        abscissaMax = 2 * widthBorder + nbOfClusters * (nbSamplesInWaveform -1) * Xstep + (nbOfClusters - 1) * Xspace;

        //Set the window
        window = ZoomWindow(QRect(QPoint(abscissaMin,ordinateMin),QPoint(abscissaMax,ordinateMax)));

        //Everything has to be redraw
        drawContentsMode = REDRAW ;
    }
    else{
        abscissaMax = 2 * widthBorder + (nbSamplesInWaveform -1) * Xstep;

        //Set the window
        window = ZoomWindow(QRect(QPoint(abscissaMin,ordinateMin),QPoint(abscissaMax,ordinateMax)));

        //update the drawing mode if needed (if UPDATE, no change is need it).
        if(drawContentsMode == REFRESH)drawContentsMode = REDRAW ;
    }
}

void WaveformView::setMeanPresentation(){
    meanPresentation = true;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    dataReady = false;
    if(!view.clusters().isEmpty()){
        //Create a thread to get the waveform data for the clusters.
        WaveformThread* waveformThread = getWaveforms();
        threadsToBeKill.append(waveformThread);
        setCursor(Qt::WaitCursor);
        waveformThread->getMean(view.clusters(),presentationMode);
    }
}

void WaveformView::setAllWaveformsPresentation(){
    meanPresentation = false;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}


void WaveformView::setSampleMode(){
    presentationMode = SAMPLE;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setTimeFrameMode(){
    presentationMode = TIME_FRAME;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setTimeFrame(long start, long width){
    startTime = start;
    endTime = start + width;
    if(endTime > maximumTime) endTime = maximumTime;

    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setDisplayNbSpikes(long nbSpikes){
    if (nbSpkToDisplay != nbSpikes) batchIteration = 0;
    //add stuff about fading

    nbSpkToDisplay =  nbSpikes;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setWaveformBatch(long bchIteration){
    batchIteration=  bchIteration;
    isZoomed = false;//Hack because all the tabs share the same data.
    drawContentsMode = REDRAW;

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}


void WaveformView::mousePressEvent(QMouseEvent* e){
    //There are two things that can possibly happen when one presses the mouse, either zoom in or select spikes to remove
    if(mode == ZOOM || isRubberBandToBeDrawn){
        //Test if a selected rectangle exist, if so draw it and delete it.
        if(e->button() == Qt::LeftButton){
            //Assign firstClick
            QRect r((QRect)window);
            firstClick = e->pos();
            if (!mRubberBand)
                mRubberBand = new KlusterRubberBand(QRubberBand::Rectangle, this);
            //Construct the rubber starting on the selected point (width = 1 and not 0 because bottomRight = left+width-1, same trick for height ;0))
            //or using only the abscissa and the ordinate if the top of the window if the rubber band has to
            //drawn on whole the height of the window.
            if(isRubberBandToBeDrawn && wholeHeightRectangle)
                mRubberBand->setGeometry(QRect(firstClick.x(),r.top(),1,1));
            else
                mRubberBand->setGeometry(QRect(firstClick.x(),firstClick.y(),1,1));
            mRubberBand->show();
        }
    }
    if(mode == REMOVE_SPIKES){
        if(e->button() == Qt::LeftButton){ //Left button pressed
            QPoint selectedPoint = viewportToWorld(e->x(),e->y());
            if (selectionPolygon.empty()){ //If this is the first point of the line for voltage selection
                selectionPolygon.putPoints(0, 1, selectedPoint.x(),selectedPoint.y());
                listSelectedClusters.clear();
                listSelectedSpikes.clear();
                drawContentsMode = REDRAW;
                update();
            }else { //If this is the second point of the line
                selectionPolygon.putPoints(1, 1, selectedPoint.x(),selectedPoint.y());
                QLineF VoltageSelectorLine = QLineF(selectionPolygon.at(0),selectionPolygon.at(1));
                QPointF A;
                QPointF B;
                QPointF intersectionPoint;
                for(int i = 0; i<listComparePolygons.size();i++){ //For every displayed spike
                    for(int j = 0; j<nbchannels;j++){ //For every channel
                        for( int k = 0; k<nbSamplesInWaveform-1;k++){ //For every line forming the spike
                            A = listComparePolygons[i][j].at(k); //First point
                            B = listComparePolygons[i][j].at(k+1); //Second point
                            int compare = VoltageSelectorLine.intersect(QLineF(A,B), &intersectionPoint);
                            if(1==compare && !listSelectedSpikes.contains(listCorresponingSpikes[i])){ //If it intersects and we dont already have that spike
                                listSelectedSpikes.append(listCorresponingSpikes[i]); //Add the spike to the list of selected spikes
                                listSelectedClusters.append(listCorresponingClusters[i]);
                                QPainter painter;
                                painter.begin(&doublebuffer);
                                QRect r((QRect)window);
                                painter.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function
                                painter.setViewport(viewport);
                                painter.setPen(Qt::red);
                                for(int j2 = 0; j2<nbchannels;j2++) painter.drawPolyline(listComparePolygons[i][j2]); //Paint every channel
                                painter.end();
                            }
                        }

                    }
                }
                selectionPolygon.clear();
            }
            drawContentsMode = REDRAW;
            update();
        }
        if(e->button() == Qt::RightButton){ //Right button pressed
            listSelectedSpikes.clear();
            listSelectedClusters.clear();
            selectionPolygon.clear();
            drawContentsMode = REDRAW;
            update();
        }
        if(e->button() == Qt::MiddleButton){ //MIddle button pressed
            //By pressing the middle button one comfirms moving the spikes to the noise cluster
            Data& clusteringData = doc.data();
            int xdimensionValue; //For the current spike, value of the x dimensional feature
            int ydimensionValue; //For the current spike, value of the y dimensional feature
            QRegion region;
            for(int i = 0; i<listSelectedSpikes.size();i++){
                xdimensionValue = clusteringData.spikeFeature(listSelectedSpikes[i],view.abscissaDimension());
                ydimensionValue = clusteringData.spikeFeature(listSelectedSpikes[i],view.ordinateDimension());
                region = QRegion(xdimensionValue,ydimensionValue,1,1,QRegion::Rectangle); //This region is too big
                QList<int> clustersOfOrigin;
                clustersOfOrigin.append(listSelectedClusters[i]);
                doc.deleteNoise(region,clustersOfOrigin,view.abscissaDimension(),view.ordinateDimension());
            }
            selectionPolygon.clear();
            listSelectedSpikes.clear();
            listSelectedClusters.clear();
            drawContentsMode = REDRAW;
            update();
        }
    }
}

void WaveformView::mouseMoveEvent(QMouseEvent* e){
    QPoint coordinates = viewportToWorld(e->x(),e->y());
    //qDebug()<<"coordinates (x,y) = (" << coordinates.x()<<"," << coordinates.y()<<")";
    if (mode == REMOVE_SPIKES){
        if(selectionPolygon.isEmpty()) return;
        selectionPolygon.putPoints(1, 1, coordinates.x(),coordinates.y());
        drawContentsMode = REFRESH;
        update();
    } else if (mode == ZOOM){
        if(e->buttons() == Qt::LeftButton){
            //Test if a selected rectangle exist, if so draw to erase the previous one,
            //update it and draw again.
            if(mRubberBand) {
                mRubberBand->setGeometry(QRect(firstClick, e->pos()).normalized());
            }
        }
    }

}

void WaveformView::mouseDoubleClickEvent (QMouseEvent *e){
    if (mode == ZOOM) {
        ViewWidget::mouseDoubleClickEvent(e);
        if((!view.clusters().isEmpty())){
            Data& clusteringData = doc.data();
            bool waveformsNotAvailable = false;
            QList<int>::const_iterator clusterIterator;
            QList<int> const clusters = view.clusters();
            for(clusterIterator = clusters.begin(); clusterIterator != clusters.end(); ++clusterIterator){
                Data::WaveformIterator* waveformIterator;
                if(presentationMode == SAMPLE) waveformIterator = clusteringData.sampleWaveformIterator(static_cast<dataType>(*clusterIterator),nbSpkToDisplay);
                else waveformIterator = clusteringData.timeFrameWaveformIterator(static_cast<dataType>(*clusterIterator),startTime,endTime);
                if(meanPresentation && (!waveformIterator->isMeanAvailable())) waveformsNotAvailable = true;
                else if(!waveformIterator->areSpikesAvailable()) waveformsNotAvailable = true;
                delete waveformIterator;
            }
            if(waveformsNotAvailable){
                setCursor(Qt::WaitCursor);
                askForWaveformInformation(clusters);
            }
        }
        isZoomed = true;
    }else if (mode == REMOVE_SPIKES){
        selectionPolygon.clear();
        listSelectedSpikes.clear();
        listSelectedClusters.clear();
        drawContentsMode = REDRAW;
        update();
    }
}

void WaveformView::mouseReleaseEvent(QMouseEvent* e){
    if (mode == ZOOM){
        ViewWidget::mouseReleaseEvent(e);
        if((e->button() & Qt::LeftButton) && (!view.clusters().isEmpty())){
            Data& clusteringData = doc.data();
            bool waveformsNotAvailable = false;
            QList<int>::const_iterator clusterIterator;
            QList<int> const clusters = view.clusters();
            for(clusterIterator = clusters.begin(); clusterIterator != clusters.end(); ++clusterIterator){
                Data::WaveformIterator* waveformIterator;
                if(presentationMode == SAMPLE)
                    waveformIterator = clusteringData.sampleWaveformIterator(static_cast<dataType>(*clusterIterator),nbSpkToDisplay);
                else
                    waveformIterator = clusteringData.timeFrameWaveformIterator(static_cast<dataType>(*clusterIterator),startTime,endTime);
                if(meanPresentation && (!waveformIterator->isMeanAvailable()))
                    waveformsNotAvailable = true;
                else if(!waveformIterator->areSpikesAvailable())
                    waveformsNotAvailable = true;
                delete waveformIterator;
            }
            if(waveformsNotAvailable){
                setCursor(Qt::WaitCursor);
                askForWaveformInformation(clusters);
            }
        }
        isZoomed = true;
    }
}



void WaveformView::drawClusterIds(QPainter& painter){
    QList<int> shownClusters;
    QList<int>::const_iterator iterator;
    QList<int> const clusters = view.clusters();
    for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator)
        shownClusters.append(*iterator);
    qSort(shownClusters);

    QFont f("Helvetica",8);
    painter.setFont(f);
    painter.setPen(colorLegend); //set the color for the legends.

    //Draw the absciss ids

    //The abscissa of the legend for the current waveform.
    uint X = widthBorder;
    //The ordinate of the legend for the current waveform.
    uint Y = 0;

    for(iterator = shownClusters.begin(); iterator != shownClusters.end(); ++iterator){
        //the abscissa is increase by the font size to adjust for conversion from world coordinates to viewport coordinates.
        painter.drawText(worldToViewport(X,-Y).x() + 8,worldToViewport(X,-Y).y() /*+8*/,QString::fromLatin1("%1").arg(*iterator));
        X += shift;
    }
}

void WaveformView::updateDrawing(){
    //The data have to be collected if need it and everything has to be redrawn
    if(!view.clusters().isEmpty() && drawContentsMode == REDRAW){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::clustersRenumbered(bool active){
    //The data have to be collected and everything has to be redrawn.
    //If the widget is in the active view, it is done immediately otherswise it will be done
    //when the view willbecome active (updateDrawing will be called).
    drawContentsMode = REDRAW;

    if(!view.clusters().isEmpty() && active){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}


void WaveformView::resizeEvent(QResizeEvent* e){
    drawContentsMode = REDRAW;

    if(!view.clusters().isEmpty()){
        Data& clusteringData = doc.data();
        bool waveformsNotAvailable = false;
        QList<int>::const_iterator clusterIterator;
        QList<int> const clusters = view.clusters();
        for(clusterIterator = clusters.begin(); clusterIterator != clusters.end(); ++clusterIterator){
            Data::WaveformIterator* waveformIterator;
            if(presentationMode == SAMPLE) waveformIterator = clusteringData.sampleWaveformIterator(static_cast<dataType>(*clusterIterator),nbSpkToDisplay);
            else waveformIterator = clusteringData.timeFrameWaveformIterator(static_cast<dataType>(*clusterIterator),startTime,endTime);
            if(meanPresentation && (!waveformIterator->isMeanAvailable())) waveformsNotAvailable = true;
            else if(!waveformIterator->areSpikesAvailable()) waveformsNotAvailable = true;
            delete waveformIterator;
        }
        if(waveformsNotAvailable){
            setCursor(Qt::WaitCursor);
            askForWaveformInformation(clusters);
        }
    }
}

void WaveformView::willBeKilled(){  
    if(!goingToDie){
        goingToDie = true;
        //inform the running threads to stop processing as soon as possible.
        for(int i = 0; i<threadsToBeKill.count();i++) {
            WaveformThread* waveformThread = threadsToBeKill.at(i);
            waveformThread->stopProcessing();
        }
    }
}

void WaveformView::print(QPainter& printPainter,int width,int height, bool whiteBackground){
    //Draw the double buffer (pixmap) by copying it into the printer device throught the painter.
    QRect viewportOld = QRect(viewport.left(),viewport.top(),viewport.width(),viewport.height());

    viewport = QRect(printPainter.viewport().left(),printPainter.viewport().top(),printPainter.viewport().width(),printPainter.viewport().height()-10);

    //Set the window (part of the world I want to show)
    QRect r((QRect)window);
    printPainter.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function

    //Set the viewport (part of the device I want to write on).
    //By default, the viewport is the same as the device's rectangle (contentsRec), taking a smaller
    //one will ensure that the legends (cluster ids) will not ovelap to much a waveform.
    printPainter.setViewport(viewport);

    //Fill the background with the background color and ensure we draw the same portion of the world than on the screen
    QRect back = QRect(r.left(),r.top(),r.width(),r.height()+10);

    QColor colorLegendTmp = colorLegend;
    QColor background= palette().color(backgroundRole());
    if(whiteBackground){
        colorLegend = Qt::black;
        QPalette palette;
        palette.setColor(backgroundRole(), Qt::white);
        setPalette(palette);
    }

    printPainter.fillRect(back,palette().color(backgroundRole()));
    printPainter.setClipRect(back);

    //Paint all the waveforms in the shownclusters list (in the double buffer)
    drawWaveforms(printPainter,view.clusters());

    //reset transformation due to setWindow and setViewport
    printPainter.resetMatrix();

    //Draw the cluster Ids below the waveforms if they are not in overlay presentation.
    if(!overLayPresentation) drawClusterIds(printPainter);

    printPainter.setClipping(false);

    //Restore the colors.
    if(whiteBackground){
        colorLegend = colorLegendTmp;
        QPalette palette;
        palette.setColor(backgroundRole(), background);
        setPalette(palette);

    }

    //Restore the previous state
    viewport = QRect(viewportOld.left(),viewportOld.top(),viewportOld.width(),viewportOld.height());
}

void WaveformView::increaseAmplitude(){
    //Decreases the ordinate scale resulting in
    //an enlargement of the waveforms in the ordinate direction.
    //factor = traceVspace / ((4/3)^gain * acquisitionGain)
    gain++;
    Yfactor = static_cast<float>(YsizeForMaxAmp)/static_cast<float>(pow(0.75,gain) * acquisitionGain);
    
    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::decreaseAmplitude(){
    //Increases the ordinate scale resulting in
    //an reduction of the waveforms in the ordinate direction.
    //factor = traceVspace / ((4/3)^gain * acquisitionGain)
    gain--;
    Yfactor = static_cast<float>(YsizeForMaxAmp)/static_cast<float>(pow(0.75,gain) * acquisitionGain);
    
    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setMode(BaseFrame::Mode selectedMode){
    statusBar->clearMessage();
    selectionPolygon.clear();
    mode = selectedMode;
    if (mode == REMOVE_SPIKES){
        //qDebug()<<"setCursor(voltageSelectionCursor);";
        setCursor(voltageSelectionCursor);
    } else {
        setCursor(zoomCursor);
    }
    drawContentsMode = REFRESH;
    update();
}

void WaveformView::setOverLayPresentation(){
    overLayPresentation = true;
    isZoomed = false;//Hack because all the tabs share the same data.

    //Everything has to be redraw
    //     updateWindow();
    //     update();

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }
}

void WaveformView::setSideBySidePresentation(){
    overLayPresentation = false;
    isZoomed = false;//Hack because all the tabs share the same data.
    
    //Everything has to be redraw
    //     updateWindow();
    //     update();

    //The data have to be collected if need it and everything has to be redraw
    if(!view.clusters().isEmpty()){
        setCursor(Qt::WaitCursor);
        askForWaveformInformation(view.clusters());
    }

}
