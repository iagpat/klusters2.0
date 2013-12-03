/***************************************************************************
                          klustersview.cpp  -  description
                             -------------------
    begin                : Mon Sep  8 12:06:21 EDT 2003
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


#include <QList>
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QWidgetAction>

// include files for Qt
#include <QPainter>
#include <QDir>
#include <qlabel.h>
#include <QDebug>
#include <QHBoxLayout>

// application specific includes
#include "klusters.h"
#include "klustersview.h"
#include "klustersdoc.h"
#include "clusterview.h"
#include "waveformview.h"
#include "errormatrixview.h"
#include "tracewidget.h"

extern int nbUndo;

const QString KlustersView::DisplayTypeNames[]={QObject::tr("Cluster Display"),
                                                QObject::tr("Waveform Display"),
                                                QObject::tr("Correlation Display"),
                                                QObject::tr("Overview Display"),
                                                QObject::tr("Grouping Assistant Display"),
                                                QObject::tr("Error Matrix Display"),
                                                QObject::tr("Trace Display")};


KlustersView::KlustersView(KlustersApp& mainWindow,KlustersDoc& pDoc,const QColor& backgroundColor,int initialDimensionX,int initialDimensionY,
                           QList<int>* initialClusterList, DisplayType type, QWidget *parent, const char* name,QStatusBar * statusBar,int timeInterval,int maxAmplitude,
                           QList<int> positions,bool isTimeFrameMode,long start,long timeFrameWidth,long nbSpkToDisplay,bool overLay,bool mean,
                           int binSize, int correlationTimeFrame,Data::ScaleMode scale,bool shoulderLine,long startingTime,long duration,bool labelsDisplay,
                           QList< QList<int>* > undoList, QList< QList<int>* > redoList)
    : DockArea(parent),
      doc(pDoc),
      removedClustersUndoList(undoList),
      removedClustersRedoList(redoList),
      dimensionX(initialDimensionX),
      dimensionY(initialDimensionY),
      currentViewWidget(0L),
      numberUndo(undoList.count()),
      inTimeFrameMode(isTimeFrameMode),
      timeWindow(timeFrameWidth),
      startTime(start),
      nbSpkToDisplay(nbSpkToDisplay),
      overLayDisplay(overLay),
      meanDisplay(mean),
      binSize(binSize),
      correlogramTimeFrame(correlationTimeFrame),
      correlationScale(scale),
      shoulderLine(shoulderLine),
      mainWindow(mainWindow),
      traceWidget(0L),
      startingTime(startingTime),
      duration(duration),
      labelsDisplay(labelsDisplay)
{
    setObjectName(name);
    setAutoFillBackground(true);
    shownClusters = initialClusterList;
    removedClusters = new QList<int>();

    setAttribute(Qt::WA_DeleteOnClose, true);
    //Create the mainDock
    mainDock = new QDockWidget(tr(doc.documentName().toLatin1()));
    mainDock->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    mainDock->setAttribute(Qt::WA_DeleteOnClose, true);
    //If the type of view is a not base one, call the function to call the complex views.
    //If the type of view is a base on, construct the appropriate Widget and assign it as the mainDock widget
    //To add a new base type just add a new case with the appropriate widget (do not to add the include line)
    switch(type){
    case CLUSTERS:
    {
        isThereWaveformView = false;
        isThereClusterView = true;
        isThereCorrelationView = false;
        isThereErrorMatrixView = false;
        isThereTraceView = false;
        mainDock->setWidget(new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,mainDock));
        currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->widget());
        viewList.append(currentViewWidget);
        currentViewWidget->installEventFilter(this);//To enable right click popup menu
        mainDock->installEventFilter(this);
        viewCounter.insert("ClusterView",1);
        setConnections(CLUSTERS,currentViewWidget,mainDock);
    }
        break;
    case WAVEFORMS:
    {
        isThereWaveformView = true;
        isThereClusterView = false;
        isThereCorrelationView = false;
        isThereErrorMatrixView = false;
        isThereTraceView = false;
        mainDock->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,mainDock,
                                             inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));

        currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->widget());
        viewList.append(currentViewWidget);
        currentViewWidget->installEventFilter(this);//To enable right click popup menu
        mainDock->installEventFilter(this);
        viewCounter.insert("WaveformView",1);
        setConnections(WAVEFORMS,currentViewWidget,mainDock);
    }
        break;
    case CORRELATIONS:
    {
        isThereWaveformView = false;
        isThereClusterView = false;
        isThereCorrelationView = true;
        isThereErrorMatrixView = false;
        isThereTraceView = false;
        mainDock->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,mainDock,correlationScale,
                                                binSize,correlogramTimeFrame,shoulderLine));

        currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->widget());
        viewList.append(currentViewWidget);
        currentViewWidget->installEventFilter(this);//To enable right click popup menu
        mainDock->installEventFilter(this);
        viewCounter.insert("CorrelationView",1);
        setConnections(CORRELATIONS,currentViewWidget,mainDock);
    }
        break;
    case OVERVIEW:
        isThereWaveformView = true;
        isThereClusterView = true;
        isThereCorrelationView = true;
        isThereErrorMatrixView = false;
        isThereTraceView = false;
        createOverview(backgroundColor,statusBar,timeInterval,maxAmplitude,positions);
        break;
    case GROUPING_ASSISTANT_VIEW:
        isThereWaveformView = true;
        isThereClusterView = true;
        isThereCorrelationView = true;
        isThereErrorMatrixView = true;
        isThereTraceView = false;
        createGroupingAssistantView(backgroundColor,statusBar,timeInterval,maxAmplitude,positions);
        break;
    case ERROR_MATRIX:
        break;
    case TRACES:
    {
        isThereWaveformView = false;
        isThereClusterView = false;
        isThereCorrelationView = false;
        isThereErrorMatrixView = false;
        isThereTraceView = true;
        //Create the providers (data and cluster) if need it
        if(!doc.isTracesProvider()) doc.createProviders();
        QList<int> skippedChannels;
        //the settings are : greyScale, no vertical lines nor rasters and waveforms, no labels displayed, no channel skipped.
        mainDock->setWidget(new TraceWidget(startingTime,duration,true,*doc.getTraceProvider(),false,false,false,
                                            true,labelsDisplay,doc.getCurrentChannels(),doc.getGain(),doc.getAcquisitionGain(),doc.channelColors(),
                                            doc.getDisplayGroupsChannels(),doc.getDisplayChannelsGroups(),offsets,gains,skippedChannels,mainDock,"traces",
                                            backgroundColor,statusBar,5));

        traceWidget = dynamic_cast<TraceWidget*>(mainDock->widget());
        //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
        doc.getClustersProvider()->setClusterIdList(shownClusters);

        //Add the cluster provider to the TraceView
        traceWidget->addClusterProvider(doc.getClustersProvider(),doc.getClustersProvider()->getName(),&static_cast<ItemColors&>(doc.clusterColors()),                           true,*shownClusters,doc.getDisplayGroupsClusterFile(),doc.getChannelsSpikeGroups(),
                                        doc.getNbSamplesBeforePeak(),doc.getNbSamplesAfterPeak(),clustersToSkip);

        traceWidget->installEventFilter(this);//To enable right click popup menu
        mainDock->installEventFilter(this);
        viewCounter.insert("TraceView",1);
        setConnections(TRACES,traceWidget,mainDock);
    }
        break;
    }
    addDockWidget(Qt::TopDockWidgetArea,mainDock);

}


KlustersView::~KlustersView()
{
    qDebug() << "in ~KlustersView(): ";
    qDeleteAll(removedClustersUndoList);
    removedClustersUndoList.clear();
    qDeleteAll(removedClustersRedoList);
    removedClustersRedoList.clear();


    delete shownClusters;
    delete removedClusters;
}

void KlustersView::createOverview(const QColor& backgroundColor,QStatusBar* statusBar,int timeInterval,int maxAmplitude,QList<int> positions){
    /*OVERVIEW type is the combination of 3 base types:
  CLUSTERS on the left side, WAVEFORMS at the right top and CORRELATIONS in the bottom right
 */
    //The main dock will be the cluster view
    ClusterView* view = new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,mainDock);
    mainDock->setWidget(view);

    currentViewWidget = view;
    viewList.append(currentViewWidget);
    currentViewWidget->installEventFilter(this);//To enable right click popup menu
    mainDock->installEventFilter(this);
    viewCounter.insert("ClusterView",1);

    setConnections(CLUSTERS,currentViewWidget,mainDock);


    //Create and add the waveforms view
    QDockWidget* waveforms = new QDockWidget(tr(doc.documentName().toLatin1()));
    waveforms->setAttribute(Qt::WA_DeleteOnClose, true);
    waveforms->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    //createDockWidget( "WaveForm", QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
    waveforms->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,waveforms,
                                          inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));//assign the widget
    ViewWidget* waveformView = dynamic_cast<ViewWidget*>(waveforms->widget());
    viewList.append(waveformView);
    waveformView->installEventFilter(this);//To enable right click popup menu
    waveforms->installEventFilter(this);
    addDockWidget(Qt::BottomDockWidgetArea,waveforms);
    viewCounter.insert("WaveformView",1);

    setConnections(WAVEFORMS,waveformView,waveforms);

    //Create and add the correlations view
    QDockWidget* correlations = new QDockWidget(tr(doc.documentName().toLatin1()));
    correlations->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    correlations->setAttribute(Qt::WA_DeleteOnClose, true);
            //createDockWidget( "Correlation", QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
    correlations->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,correlations,correlationScale,binSize,correlogramTimeFrame,shoulderLine));//assign the widget
    ViewWidget* correlationView = dynamic_cast<ViewWidget*>(correlations->widget());
    viewList.append(correlationView);
    correlationView->installEventFilter(this);//To enable right click popup menu
    correlations->installEventFilter(this);
    addDockWidget(Qt::BottomDockWidgetArea,correlations);
    viewCounter.insert("CorrelationView",1);

    setConnections(CORRELATIONS,correlationView,correlations);
}

void KlustersView::createGroupingAssistantView(const QColor& backgroundColor,QStatusBar* statusBar,int timeInterval,int maxAmplitude,QList<int> positions){
    //First create the overview
    createOverview(backgroundColor,statusBar,timeInterval,maxAmplitude,positions);

    //Create and add the errorMatrixView beneath the clusterView (mainDock)
    QDockWidget* errorMatrix = new QDockWidget(tr(doc.documentName().toLatin1()));
    errorMatrix->setAttribute(Qt::WA_DeleteOnClose, true);
    errorMatrix->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    //createDockWidget("ErrorMatrix", QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
    errorMatrix->setWidget(new ErrorMatrixView(doc,*this,backgroundColor,statusBar,errorMatrix));//assign the widget
    ViewWidget* errorMatrixView = dynamic_cast<ViewWidget*>(errorMatrix->widget());
    viewList.append(errorMatrixView);
    errorMatrixView->installEventFilter(this);//To enable right click popup menu
    errorMatrix->installEventFilter(this);
    addDockWidget(Qt::BottomDockWidgetArea,errorMatrix);
    setConnections(ERROR_MATRIX,errorMatrixView,errorMatrix);
}


void KlustersView::update(KlustersView* pSender){
    if(pSender != this)
        repaint();
}

void KlustersView::print(QPrinter *pPrinter, const QString& filePath, bool whiteBackground)
{
    QPainter printPainter;

    const int width = pPrinter->width();
    const int height = pPrinter->height();
    printPainter.begin(pPrinter);


    QRect textRec = QRect(printPainter.viewport().left() + 5 ,printPainter.viewport().height() - 20,printPainter.viewport().width() - 5,20);
    QFont f("Helvetica",8);

    const int nbViews = viewList.count();
    for(int i = 0; i< nbViews; ++i) {
        if(i > 0)
            pPrinter->newPage();
        ViewWidget* widget = viewList.at(i);
        //Modify the viewport so the view will not draw on the legend
        QRect newViewport = QRect(printPainter.viewport().left(),printPainter.viewport().top(),printPainter.viewport().width(),printPainter.viewport().height());
        newViewport.setBottom(printPainter.viewport().bottom() - 20);
        printPainter.setViewport(newViewport);
        widget->print(printPainter,width,height,whiteBackground);
        printPainter.resetMatrix();

        printPainter.setFont(f);
        printPainter.setPen(Qt::black);
        if(qobject_cast<ClusterView*>(widget)){
            ClusterView* clusterView = static_cast<ClusterView*>(widget);
            printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1      Features: %2,%3").arg(filePath).arg(clusterView->getDimensionX()).arg(clusterView->getDimensionY()));
        } else if(qobject_cast<WaveformView*>(widget)) {
            if(inTimeFrameMode){
                printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1      Start Time: %2 s, Duration: %3 s").arg(filePath).arg(startTime).arg(timeWindow));
            } else {
                printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1      Number of Waveforms: %2").arg(filePath).arg(nbSpkToDisplay));
            }
        } else if(qobject_cast<CorrelationView*>(widget)) {
            QString scaleType;
            switch(correlationScale){
            case Data::RAW :
                scaleType = tr("Uniform Scale");
                break;
            case Data::MAX :
                scaleType =tr("Scale by Maximum");
                break;
            case Data::SHOULDER :
                scaleType = tr("Scale by Asymptote");
                break;
            }
            printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1      %2, Duration: %3 ms, Bin Size: %4 ms").arg(filePath).arg(scaleType).arg(correlogramTimeFrame/2).arg(binSize));
        } else if(qobject_cast<ErrorMatrixView*>(widget)){
            printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1").arg(filePath));
        }

    }

    //Print the trace view if exists
    if(isThereTraceView){
        pPrinter->newPage();
        printPainter.resetMatrix();
        //Print the TraceView
        traceWidget->print(printPainter,width,height,filePath,whiteBackground);
    }

    printPainter.end();
}

void  KlustersView::clusterDockClosed(QObject *clusterView){
    viewList.removeAll(static_cast<ViewWidget*>(clusterView));
    //the clusterView to be removed is the last one
    if(viewCounter["ClusterView"] == 1){
        viewCounter.remove("ClusterView");
        mainWindow.widgetRemovedFromDisplay(CLUSTERS);
        isThereClusterView = false;
        dimensionX = 1;
        dimensionY = 2;
    }
    else viewCounter["ClusterView"]--;

    //Update the spineboxes with the dimensions of the first ClusterView found
    int nbViews = viewList.count();
    for(int i = 0; i< nbViews; i++) {
        ViewWidget* viewWidget = viewList.at(i);

        if(qobject_cast<ClusterView*>(viewWidget)){
            dimensionX = dynamic_cast<ClusterView*>(viewWidget)->getDimensionX();
            dimensionY = dynamic_cast<ClusterView*>(viewWidget)->getDimensionY();
            mainWindow.updateDimensionSpinBoxes(dimensionX,dimensionY);
            QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
            connect(this,SIGNAL(updatedDimensions(int,int)),viewWidget, SLOT(updatedDimensions(int,int)));
            break;
        }
    }
}

void KlustersView::waveformDockClosed(QObject* waveformView){
    QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.
    viewList.removeAll(static_cast<ViewWidget*>(waveformView));

    //For the time being only one WaveformView is allowed in a single View, but in the
    //future who knows ;0). This counter will make it easier to allow multiple WaveformView.
    if(viewCounter["WaveformView"] == 1){
        viewCounter.remove("WaveformView");
        mainWindow.widgetRemovedFromDisplay(WAVEFORMS);
        isThereWaveformView = false;
    }
    else
        viewCounter["WaveformView"]--;
}

void KlustersView::correlogramDockClosed(QObject* correlogramView){

    QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.
    viewList.removeAll(static_cast<ViewWidget*>(correlogramView));

    if(viewCounter["CorrelationView"] == 1){
        viewCounter.remove("CorrelationView");
        mainWindow.widgetRemovedFromDisplay(CORRELATIONS);
        isThereCorrelationView = false;
        binSize = 1;
        correlogramTimeFrame = 61;
        correlationScale = Data::MAX;
        shoulderLine = true;
    }
    else viewCounter["CorrelationView"]--;

    //Update the correlogramView variables with the variables of the first correlogramView found
    int nbViews = viewList.count();
    for(int i = 0; i< nbViews; i++) {
        ViewWidget* viewWidget = viewList.at(i);
        if(qobject_cast<CorrelationView*>(viewWidget)){
            binSize = static_cast<CorrelationView*>(viewWidget)->getBinSize();
            correlogramTimeFrame = dynamic_cast<CorrelationView*>(viewWidget)->getTimeWindow();
            correlationScale = dynamic_cast<CorrelationView*>(viewWidget)->getScaleMode();
            shoulderLine = dynamic_cast<CorrelationView*>(viewWidget)->isShoulderLine();
            mainWindow.updateCorrelogramViewVariables(binSize,correlogramTimeFrame,shoulderLine,correlationScale);
            updateCorrelogramConnections(viewWidget);
            break;
        }
    }
}

void KlustersView::errorMatrixDockClosed(QObject* errorMatrixView){
    QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.

    viewList.removeAll(static_cast<ViewWidget*>(errorMatrixView));
    mainWindow.widgetRemovedFromDisplay(ERROR_MATRIX);
    isThereErrorMatrixView = false;
}

void KlustersView::traceDockClosed(QObject *traceWidget){
    if(viewCounter["TraceView"] == 1){
        viewCounter.remove("TraceView");
        mainWindow.widgetRemovedFromDisplay(TRACES);
        traceWidget = 0L;
        isThereTraceView = false;
    }
    else viewCounter["TraceView"]--;
}

bool KlustersView::eventFilter(QObject* object,QEvent* event){

    if((event->type() == QEvent::MouseButtonPress) && (!qobject_cast<KlustersView*>(object))){
        //Check if the user has selected a dockWidget containing a ClusterView. If so
        //update the dimension spin boxes to reflect the current ClusterView dimensions
        // and make the ClusterView the only view connected to the signal of update of the spin boxes.
        if(qobject_cast<QDockWidget*>(object)){
            QWidget* widget = dynamic_cast<QDockWidget*>(object)->widget();
            if(qobject_cast<ClusterView*>(widget)){
                int nbViews = viewList.count();
                for(int i = 0; i< nbViews; i++) {
                    ViewWidget* viewWidget = viewList.at(i);
                    if(qobject_cast<ClusterView*>(viewWidget) && (widget == viewWidget)){
                        dimensionX = dynamic_cast<ClusterView*>(viewWidget)->getDimensionX();
                        dimensionY = dynamic_cast<ClusterView*>(viewWidget)->getDimensionY();
                        mainWindow.updateDimensionSpinBoxes(dimensionX,dimensionY);
                        QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
                        connect(this,SIGNAL(updatedDimensions(int,int)),viewWidget, SLOT(updatedDimensions(int,int)));
                        return QWidget::eventFilter(object,event);
                    }
                }
            }
            //Check if the user has selected a dockWidget containing a CorrelationView. If so
            //update the bin size and duration boxes to reflect the current CorrelationView values and make the CorrelationView
            // the only view connected to the signal of update of the boxes.
            if(qobject_cast<CorrelationView*>(widget)){
                int nbViews = viewList.count();
                for(int i = 0; i< nbViews; i++) {
                    ViewWidget* viewWidget = viewList.at(i);
                    if(qobject_cast<CorrelationView*>(viewWidget) && (widget == viewWidget)){
                        binSize = dynamic_cast<CorrelationView*>(viewWidget)->getBinSize();
                        correlogramTimeFrame = dynamic_cast<CorrelationView*>(viewWidget)->getTimeWindow();
                        correlationScale = dynamic_cast<CorrelationView*>(viewWidget)->getScaleMode();
                        shoulderLine = dynamic_cast<CorrelationView*>(viewWidget)->isShoulderLine();
                        mainWindow.updateCorrelogramViewVariables(binSize,correlogramTimeFrame,shoulderLine,correlationScale);
                        updateCorrelogramConnections(viewWidget);
                        return QWidget::eventFilter(object,event);
                    }
                }
            }
        }

        //If the view on which the user has clicked is a ClusterView do the following:
        // * update the dimension spin boxes to reflect the current ClusterView dimensions
        //   and make the ClusterView the only view connected to the signal of update of the spin boxes.
        // * if a polygon is been drawn do not interpret the right click as an
        //   inquiery for the add View popupmenu.
        if(qobject_cast<ClusterView*>(object)){
            int nbViews = viewList.count();
            for(int i = 0; i< nbViews; i++) {
                ViewWidget* widget = viewList.at(i);

                if(qobject_cast<ClusterView*>(widget) && (object == widget)){
                    dimensionX = dynamic_cast<ClusterView*>(widget)->getDimensionX();
                    dimensionY = dynamic_cast<ClusterView*>(widget)->getDimensionY();
                    mainWindow.updateDimensionSpinBoxes(dimensionX,dimensionY);
                    QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
                    connect(this,SIGNAL(updatedDimensions(int,int)),widget, SLOT(updatedDimensions(int,int)));
                    bool inProcess = dynamic_cast<ClusterView*>(widget)->isASelectionInProcess();
                    if(inProcess) return QWidget::eventFilter(object,event);
                }
            }
        }

        //Check if the user has selected a dockWidget containing a CorrelationView. If so
        //update the bin size and duration boxes to reflect the current CorrelationView values and make the CorrelationView
        // the only view connected to the signal of update of the boxes.
        if(qobject_cast<CorrelationView*>(object)){
            int nbViews = viewList.count();
            for(int i = 0; i< nbViews; i++) {
                ViewWidget* viewWidget = viewList.at(i);
                if(qobject_cast<CorrelationView*>(viewWidget) && (object == viewWidget)){
                    binSize = dynamic_cast<CorrelationView*>(viewWidget)->getBinSize();
                    correlogramTimeFrame = dynamic_cast<CorrelationView*>(viewWidget)->getTimeWindow();
                    correlationScale = dynamic_cast<CorrelationView*>(viewWidget)->getScaleMode();
                    shoulderLine = dynamic_cast<CorrelationView*>(viewWidget)->isShoulderLine();
                    mainWindow.updateCorrelogramViewVariables(binSize,correlogramTimeFrame,shoulderLine,correlationScale);
                    updateCorrelogramConnections(viewWidget);
                }
            }
        }
        //QWidget* widget;
        if(!qobject_cast<QDockWidget*>(object)) {
            //widget = dynamic_cast<QDockWidget*>(object)->widget();
        } else if(qobject_cast<ClusterView*>(object) ||
                  qobject_cast<WaveformView*>(object) ||
                  qobject_cast<CorrelationView*>(object) ||
                  qobject_cast<ErrorMatrixView*>(object) ||
                  qobject_cast<TraceWidget*>(object)) {
            //widget = dynamic_cast<QWidget*>(object);
        //if the object is a TraceView take its container the TraceWidget
        } else if(qobject_cast<TraceView*>(object)) {
            //widget = traceWidget;
        } else {
            return QWidget::eventFilter(object,event);    // standard event processing
        }

        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::RightButton){
            //Create the popmenu
            QMenu menu(tr("Add a View"),this);
            QWidgetAction *act = new QWidgetAction(&menu);
            QLabel *lab = new QLabel(tr("Add a View"));
            lab->setAlignment(Qt::AlignCenter);
            act->setDefaultWidget(lab);
            menu.addAction(act);
            menu.addSeparator();
            QAction* clusterView = menu.addAction(tr("Add a ClusterView"));
            QAction* waveformView = menu.addAction(tr("Add a WaveformView"));
            QAction* correlationView = menu.addAction(tr("Add a CorrelationView"));
            QAction* errorMatrixView = menu.addAction(tr("Add an ErrorMatrixView"));
            QAction* traceView = menu.addAction(tr("Add a TraceView"));

            //A traceView is possible only if the variables it needs are available (provided in the new parameter file) and
            //the .dat file exists.
            if(!doc.areTraceDataAvailable() || !doc.isTraceViewVariablesAvailable())
                traceView->setEnabled(false);

            //For the moment only one WaveformView and TraceView are allowed per View.
            if(viewCounter.contains("WaveformView"))
                waveformView->setEnabled(false);

            if(viewCounter.contains("CorrelationView"))
                correlationView->setEnabled(true);

            if(viewCounter.contains("TraceView"))
                traceView->setEnabled(false);

            //Only one ErrorMatrixView is allowed for the whole application.
            if(mainWindow.isExistAnErrorMatrix())
                errorMatrixView->setEnabled(false);

            menu.setMouseTracking(true);
            QAction* id = menu.exec(QCursor::pos());

            if(id == clusterView){
                mainWindow.widgetAddToDisplay(CLUSTERS);
                return true;
            }
            else if(id == waveformView){
                mainWindow.widgetAddToDisplay(WAVEFORMS);
                return true;
            }
            else if(id == correlationView){
                mainWindow.widgetAddToDisplay(CORRELATIONS);
                return true;
            }
            else if(id == errorMatrixView){
                mainWindow.widgetAddToDisplay(ERROR_MATRIX);
                return true;
            }
            else if(id == traceView){
                mainWindow.widgetAddToDisplay(TRACES);
                return true;
            }
            else return QWidget::eventFilter(object,event);    // standard event processing
        }
        else return QWidget::eventFilter(object,event);    // standard event processing
    }
    else return QWidget::eventFilter(object,event);    // standard event processing

}

void KlustersView::closeEvent(QCloseEvent* e){

    // DO NOT CALL QWidget::closeEvent(e) here !!
    // This will accept the closing by QCloseEvent::accept() by default.
    // The installed eventFilter() in KlustersApp takes care for closing the widget
    // or ignoring the close event

}

bool KlustersView::addView(DisplayType displayType, const QColor &backgroundColor, QStatusBar* statusBar, int timeInterval, int maxAmplitude, QList<int> positions){

    //Enable docking abilities
    QDockWidget* clusters;
    QDockWidget* waveforms;
    QDockWidget* correlations;
    QDockWidget* errorMatrix;
    ViewWidget* clusterView;
    ViewWidget* waveformView;
    ViewWidget* correlationView;
    ViewWidget* errorMatrixView;
    QDockWidget* traces;
    QList<int> skippedChannels;

    bool newViewType = false;
    QString count;

    switch(displayType){
    case CLUSTERS:
    {
        if(!isThereClusterView){
            newViewType = true;
            viewCounter.insert("ClusterView",1);
        }
        else viewCounter["ClusterView"]++;

        isThereClusterView = true;
        count = QString::number(viewCounter["ClusterView"]);

        clusters = new QDockWidget(tr(doc.documentName().toLatin1()));
        clusters->setAttribute(Qt::WA_DeleteOnClose, true);
        clusters->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
                //createDockWidget(count.prepend("ClusterView"), QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
        clusters->setWidget(new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,clusters));
        clusterView = static_cast<ViewWidget*>(clusters->widget());
        viewList.append(clusterView);
        clusterView->installEventFilter(this);//To enable right click popup menu
        clusters->installEventFilter(this);
        addDockWidget(Qt::BottomDockWidgetArea,clusters);
        //Make the new ClusterView the only view connected to the signal of update of the spin boxes.
        //To do so disconnect all the other clusterViews connected, the actual connection for the current view is done in setConnections.
        QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);

        setConnections(CLUSTERS,clusterView,clusters);

        //Give to the new view the same mode than the other clusterviews
        if(!newViewType){
            int nbViews = viewList.count();
            for(int i = 0; i< nbViews; i++) {
                ViewWidget* viewWidget = viewList.at(i);
                if(qobject_cast<ClusterView*>(viewWidget)){
                    clusterView->setMode(static_cast<ClusterView*>(viewWidget)->getMode());

                    break;
                }
            }
        }
    }
        break;
    case WAVEFORMS:
        if(!isThereWaveformView){
            newViewType = true;
            viewCounter.insert("WaveformView",1);
        } else {
            viewCounter["WaveformView"]++;
        }

        isThereWaveformView = true;
        count = QString::number(viewCounter["WaveformView"]);

        waveforms = new QDockWidget(tr(doc.documentName().toLatin1()));
        waveforms->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
        waveforms->setAttribute(Qt::WA_DeleteOnClose, true);
                //createDockWidget(count.prepend("WaveformView"), QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
        waveforms->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,waveforms,
                                              inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));//assign the widget
        waveformView = dynamic_cast<ViewWidget*>(waveforms->widget());
        viewList.append(waveformView);
        waveformView->installEventFilter(this);//To enable right click popup menu
        waveforms->installEventFilter(this);
        addDockWidget(Qt::BottomDockWidgetArea,waveforms);
        setConnections(WAVEFORMS,waveformView,waveforms);
        break;
    case CORRELATIONS:
        if(!isThereCorrelationView){
            newViewType = true;
            viewCounter.insert("CorrelationView",1);
        }
        else  viewCounter["CorrelationView"]++;

        isThereCorrelationView = true;
        count = QString::number(viewCounter["CorrelationView"]);

        correlations = new QDockWidget(tr(doc.documentName().toLatin1()));
        correlations->setAttribute(Qt::WA_DeleteOnClose, true);
        correlations->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);

                //createDockWidget(count.prepend("CorrelationView"), QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
        correlations->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,correlations,correlationScale,binSize,correlogramTimeFrame,shoulderLine));//assign the widget
        correlationView = dynamic_cast<ViewWidget*>(correlations->widget());
        viewList.append(correlationView);
        correlationView->installEventFilter(this);//To enable right click popup menu
        correlations->installEventFilter(this);
        addDockWidget(Qt::BottomDockWidgetArea,correlations);
        //Make the new CorrelationView the only view connected to the signals.
        //To do so disconnect all the other CorrelationView connected, the actual connection for the current view is done in setConnections.
        QObject::disconnect(this, SIGNAL(updatedBinSizeAndTimeFrame(int,int)),0,0);
        QObject::disconnect(this, SIGNAL(noScale()),0,0);
        QObject::disconnect(this, SIGNAL(maxScale()),0,0);
        QObject::disconnect(this, SIGNAL(shoulderScale()),0,0);
        QObject::disconnect(this, SIGNAL(increaseAmplitudeofCorrelograms()),0,0);
        QObject::disconnect(this, SIGNAL(decreaseAmplitudeofCorrelograms()),0,0);
        QObject::disconnect(this, SIGNAL(setShoulderLine(bool)),0,0);

        setConnections(CORRELATIONS,correlationView,correlations);
        break;
    case ERROR_MATRIX:
        newViewType = true;
        isThereErrorMatrixView = true;

        errorMatrix = new QDockWidget(tr(doc.documentName().toLatin1()));
        errorMatrix->setAttribute(Qt::WA_DeleteOnClose, true);
        errorMatrix->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
        //createDockWidget("ErrorMatrix", QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
        errorMatrix->setWidget(new ErrorMatrixView(doc,*this,backgroundColor,statusBar,errorMatrix));//assign the widget
        errorMatrixView = dynamic_cast<ViewWidget*>(errorMatrix->widget());
        viewList.append(errorMatrixView);
        errorMatrixView->installEventFilter(this);//To enable right click popup menu
        errorMatrix->installEventFilter(this);
        addDockWidget(Qt::BottomDockWidgetArea,errorMatrix);
        setConnections(ERROR_MATRIX,errorMatrixView,errorMatrix);
        break;
    case TRACES:
        if(!isThereTraceView){
            newViewType = true;
            viewCounter.insert("TraceView",1);
            if(!doc.isTracesProvider()) doc.createProviders();
        }
        else viewCounter["TraceView"]++;

        isThereTraceView = true;
        count = QString::number(viewCounter["TraceView"]);

        traces = new QDockWidget(tr(doc.documentName().toLatin1()));
        traces->setAttribute(Qt::WA_DeleteOnClose, true);
        traces->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
                //createDockWidget(count.prepend("TraceView"), QPixmap(), 0L, tr(doc.documentName().toLatin1()), tr(doc.documentName().toLatin1()));
        //the settings are : greyScale, no vertical lines nor rasters and waveforms, no labels displayed, no channel skipped.
        traces->setWidget(new TraceWidget(startingTime,duration,true,*doc.getTraceProvider(),false,false,false,
                                          true,labelsDisplay,doc.getCurrentChannels(),doc.getGain(),doc.getAcquisitionGain(),doc.channelColors(),
                                          doc.getDisplayGroupsChannels(),doc.getDisplayChannelsGroups(),offsets,gains,skippedChannels,traces,"traces",
                                          backgroundColor,statusBar,5));//assign the widget


        traceWidget = dynamic_cast<TraceWidget*>(traces->widget());

        //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
        doc.getClustersProvider()->setClusterIdList(shownClusters);

        //Add the cluster provider to the TraceView
        traceWidget->addClusterProvider(doc.getClustersProvider(),doc.getClustersProvider()->getName(),&static_cast<ItemColors&>(doc.clusterColors()),                           true,*shownClusters,doc.getDisplayGroupsClusterFile(),doc.getChannelsSpikeGroups(),
                                        doc.getNbSamplesBeforePeak(),doc.getNbSamplesAfterPeak(),clustersToSkip);

        traces->installEventFilter(this);//To enable right click popup menu
        traceWidget->installEventFilter(this);
        addDockWidget(Qt::BottomDockWidgetArea,traces);
        setConnections(TRACES,traceWidget,traces);
        break;
    case OVERVIEW:
        break;
    case GROUPING_ASSISTANT_VIEW:
        break;
    }

    return newViewType;
}

void KlustersView::updateDimensions(int dimensionX,int dimensionY){  
    this->dimensionX = dimensionX;
    this->dimensionY = dimensionY;
    //Signal a change to the Widgets
    emit updatedDimensions(dimensionX,dimensionY);
}


void KlustersView::shownClustersUpdate(QList<int>& clustersToShow){
    //Try to minimize the number of clusters to draw
    QVector<int> clustersToRemove;
    
    //If a cluster already shown is not requested, remove it from the view
    QList<int>::iterator shownClustersIterator;
    for(shownClustersIterator = shownClusters->begin(); shownClustersIterator != shownClusters->end(); ++shownClustersIterator ){
        if(clustersToShow.contains(*shownClustersIterator) == 0){
            clustersToRemove.push_back(*shownClustersIterator);
        }
    }

    //Remove the clusters found previously from shownClusters
    removeClustersFromView(clustersToRemove,true);

    //If there is a cluster in clustersToShow which is not in shownClusters, add it to the view
    QList<int>::iterator clustersToShowIterator;
    for(clustersToShowIterator = clustersToShow.begin(); clustersToShowIterator != clustersToShow.end(); ++clustersToShowIterator ){
        if(shownClusters->contains(*clustersToShowIterator) == 0)
            addClusterToView(*clustersToShowIterator,true);
    }

    //Show all the enclosed widgets of the dockWindows.
    showAllWidgets();
}

void KlustersView::updateColors(bool active){
    ItemColors& clusterColors = doc.clusterColors();
    if(clusterColors.isColorChanged()){
        QList<int> colorChangedClusterList = clusterColors.colorChangedItemList();
        QList<int>::iterator iterator;
        for(iterator = colorChangedClusterList.begin(); iterator != colorChangedClusterList.end(); ++iterator ){
            if(shownClusters->contains(*iterator) != 0)
                singleColorUpdate(*iterator,active);
        }
    }
}

void KlustersView::groupedClustersUpdate(QList<int>& groupedClusters, int newClusterId,bool active){  
    bool isGroupedClustersInShownList = false;

    //If a cluster of the groupedClusters is in shownClusters list, remove it
    isGroupedClustersInShownList = clustersDeletionUpdate(groupedClusters,active);
    
    //If at least on cluster of the groupedClusters was in shownClusters list, add the new cluster to the list.
    if(isGroupedClustersInShownList){
        addClusterToView(newClusterId,active);
    }

    //Check if some clusters have had their color changed.
    // If so, update clusterUpdateList if needed
    updateColors(active);
}


bool KlustersView::clustersDeletionUpdate(QList<int>& deletedClusters,int destinationCluster,bool active){
    bool isAClusterRemoved = clustersDeletionUpdate(deletedClusters,active);

    //If the view contains the destinationCluster emit a notice of modification
    if(shownClusters->contains(destinationCluster) != 0){
        QList<int> modifiedcluster;
        modifiedcluster.append(destinationCluster);
        emit modifiedClusters(modifiedcluster,active,true);
    }

    return isAClusterRemoved;
}

bool KlustersView::clustersDeletionUpdate(QList<int>& deletedClusters,bool active){  
    QList<int> inView = clustersInView(deletedClusters);
    bool isAClusterRemoved = false;

    //the removedClustersUndoList have to be updated
    prepareUndo(inView);

    //If deletedClusters in not empty, this view is concerned by the modification
    if(!inView.isEmpty()){
        isAClusterRemoved = true;

        //If one of the clusters in deletedClusters is present in clustersShown list, remove it
        // and call removeClusterFromView on all the widgets
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = inView.begin(); clustersToRemoveIterator != inView.end(); ++clustersToRemoveIterator){
            removeClusterFromView(*clustersToRemoveIterator,active);
        }
    }

    //Check if some clusters have had their color changed.
    // If so, update clusterUpdateList if needed
    updateColors(active);

    return isAClusterRemoved;
}

void KlustersView::removeClusterFromView(int clusterId,bool active){
    shownClusters->removeAll(clusterId);
    emit clusterRemovedFromView(clusterId,active);
}

void KlustersView::removeClustersFromView(const QVector<int>& clusterIds, bool active){
    int size = clusterIds.size();
    for(int i = 0; i<size; ++i){
        shownClusters->removeAll(clusterIds[i]);
        emit clusterRemovedFromView(clusterIds[i],active);
    }
}

void KlustersView::addClusterToView(int clusterId,bool active){
    //Add the clusterId to the list of clusters shown and call addClusterToView on all the widgets
    shownClusters->append(clusterId);
    emit clusterAddedToView(clusterId,active);
}

void KlustersView::addNewClusterToView(QList<int>& fromClusters,int clusterId,QList<int>& emptiedClusters,bool active){  
    //List containing the clusters of this view which contained spikes of the newly created cluster
    QList<int> fromClustersInView = clustersInView(fromClusters);

    //the removedClustersUndoList have to be updated
    QList<int> emptiedClustersInView = clustersInView(emptiedClusters);
    prepareUndo(emptiedClustersInView);


    //If fromClustersInView in not empty, this view is concerned by the modification
    if(!fromClustersInView.isEmpty()){
        if(!emptiedClusters.isEmpty()){
            QList<int>::iterator clustersToRemoveIterator;
            for(clustersToRemoveIterator = emptiedClusters.begin(); clustersToRemoveIterator != emptiedClusters.end(); ++clustersToRemoveIterator ){
                removeClusterFromView(*clustersToRemoveIterator,active);
            }
        }

        shownClusters->append(clusterId);
        emit newClusterAddedToView(fromClustersInView,clusterId,active);
    }

    //Check if some clusters have had their color changed.
    // If so, update clusterUpdateList if needed
    updateColors(active);
}

void KlustersView::addNewClustersToView(QMap<int,int>& fromToNewClusterIds,QList<int>& emptiedClusters,bool active){
    //List containing the clusters of this view which contained spikes of the newly created cluster
    QList<int> fromClusters = fromToNewClusterIds.keys();
    QList<int> fromClustersInView = clustersInView(fromClusters);

    //the removedClustersUndoList have to be updated
    QList<int> emptiedClustersInView = clustersInView(emptiedClusters);
    prepareUndo(emptiedClustersInView);


    //If fromClustersInView in not empty, this view is concerned by the modification
    if(!fromClustersInView.isEmpty()){

        if(!emptiedClusters.isEmpty()){
            QList<int>::iterator clustersToRemoveIterator;
            for(clustersToRemoveIterator = emptiedClusters.begin(); clustersToRemoveIterator != emptiedClusters.end(); ++clustersToRemoveIterator ){
                removeClusterFromView(*clustersToRemoveIterator,active);
            }
        }

        emit modifiedClusters(fromClustersInView,active);

        QList<int>::iterator fromClusterIterator;
        for (fromClusterIterator = fromClustersInView.begin(); fromClusterIterator != fromClustersInView.end(); ++fromClusterIterator){
            int newClusterId = fromToNewClusterIds[*fromClusterIterator];
            shownClusters->append(newClusterId);
            emit newClusterAddedToView(newClusterId,active);
        }

    }

    //Check if some clusters have had their color changed.
    // If so, update clusterUpdateList if needed
    updateColors(active);
}


void KlustersView::addNewClustersToView(QList<int>& clustersToRecluster,QList<int>& reclusteredClusterList,bool active){

    //List containing the clusters of this view which contained recluster clusters
    QList<int> inView = clustersInView(clustersToRecluster);

    //the removedClustersUndoList have to be updated
    prepareUndo(inView);

    //If inView in not empty, this view is concerned by the modification
    if(!inView.isEmpty()){
        //prepareUndo(clustersToRecluster);

        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = clustersToRecluster.begin(); clustersToRemoveIterator != clustersToRecluster.end(); ++clustersToRemoveIterator ){
            removeClusterFromView(*clustersToRemoveIterator,active);
        }

        QList<int>::iterator iterator;
        for (iterator = reclusteredClusterList.begin(); iterator != reclusteredClusterList.end(); ++iterator){
            shownClusters->append(*iterator);
            emit newClusterAddedToView(*iterator,active);
        }
    }
}


void KlustersView::removeSpikesFromClustersInView(QList<int>& fromClusters, int destinationClusterId,QList<int>& emptiedClusters,bool active){    
    //List containing the clusters of this view which contained spikes which were removed
    QList<int> fromClustersInView = clustersInView(fromClusters);

    //the removedClustersUndoList have to be updated
    QList<int> emptiedClustersInView = clustersInView(emptiedClusters);
    prepareUndo(emptiedClustersInView);

    //If fromClustersInView in not empty, this view is concerned by the modification
    if(!fromClustersInView.isEmpty()){

        if(!emptiedClusters.isEmpty()){
            QList<int>::iterator clustersToRemoveIterator;
            for(clustersToRemoveIterator = emptiedClusters.begin(); clustersToRemoveIterator != emptiedClusters.end(); ++clustersToRemoveIterator ){
                removeClusterFromView(*clustersToRemoveIterator,active);
            }
        }

        emit spikesRemovedFromClusters(fromClustersInView,active);
        if(shownClusters->contains(destinationClusterId) != 0)
            emit spikesAddedToCluster(destinationClusterId,active);
    }

    //If the view contains the destinationCluster emit a notice of modification
    else if(shownClusters->contains(destinationClusterId) != 0){
        emit spikesAddedToCluster(destinationClusterId,active);
    }

    //Check if some clusters have had their color changed.
    // If so, update clusterUpdateList if needed
    updateColors(active);
}

QList<int> KlustersView::clustersInView(QList<int>& clusterlist){
    //subset of clusterlist with the clusters of this view
    QList<int> clustersInViewList;

    QList<int>::iterator iterator;
    for (iterator = clusterlist.begin(); iterator != clusterlist.end(); ++iterator){
        if(shownClusters->contains(*iterator) != 0) 
            clustersInViewList.append(*iterator);
    }
    return clustersInViewList;
}

void KlustersView::prepareUndo(QList<int>* removedClustersTemp){
    //Store the current removedClusters in the undo list and make the temporary become the current one.
    removedClustersUndoList.prepend(removedClusters);
    removedClusters = removedClustersTemp;

    ++numberUndo;

    //if the number of undo has been reach remove the last elements in the undo list (first ones inserted)
    if(numberUndo > nbUndo){
        delete removedClustersUndoList.takeAt(numberUndo - 1);
        --numberUndo;
    }

    //Clear the redoList
    qDeleteAll(removedClustersRedoList);
    removedClustersRedoList.clear();
}

void KlustersView::prepareUndo(QList<int>& newlyRemovedClusters){   
    //Create a new shownClusters which will hold the new configuration
    QList<int>* removedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for (iterator = newlyRemovedClusters.begin(); iterator != newlyRemovedClusters.end(); ++iterator)
        removedClustersTemp->append(*iterator);

    prepareUndo(removedClustersTemp);
}

void KlustersView::nbUndoChangedCleaning(int newNbUndo){
    //if the new number of possible undo is smaller than the current one,
    // clean the undo/redo related variables.
    if(newNbUndo < nbUndo){
        //if the current number of undo is bigger than the new number of undo,
        // remove the last elements in the undo lists (first ones inserted).
        if(numberUndo > newNbUndo){
            while(numberUndo > newNbUndo){
                delete removedClustersUndoList.takeAt(numberUndo - 1);
                --numberUndo;
            }
            //Clear the redoLists
            qDeleteAll(removedClustersUndoList);
            removedClustersUndoList.clear();
        }
        //currentNbUndo < newNbUndo, check the redo list.
        else{
            //number of undo and redo must be <= new number of undo. Remove redo elements if need it.
            int currentNbRedo = removedClustersRedoList.count();
            if((currentNbRedo + numberUndo) > newNbUndo){
                while((currentNbRedo + numberUndo) > newNbUndo){
                    delete removedClustersRedoList.takeAt(currentNbRedo - 1);
                    currentNbRedo = removedClustersRedoList.count();
                }
            }
        }
    }
}

void KlustersView::addRemovedClusters(bool active){
    //If removedClustersUndoList is not empty, make the current removedClusters become the first element
    //of the removedClustersRedoList and the first element of the removedClustersUndoList become the current removedClusters.
    if(!removedClustersUndoList.isEmpty()){
        if(!removedClusters->isEmpty()){
            QList<int>::iterator newClusterIterator;
            for(newClusterIterator = removedClusters->begin(); newClusterIterator != removedClusters->end(); ++newClusterIterator){
                shownClusters->append(*newClusterIterator);
                emit newClusterAddedToView(*newClusterIterator,active);
            }
        }
        removedClustersRedoList.prepend(removedClusters);
        QList<int>* removedClustersTemp = removedClustersUndoList.takeAt(0);
        removedClusters =  removedClustersTemp;
    }
}

void KlustersView::undo(bool active){
    //add back the removed clusters
    addRemovedClusters(active);
    --numberUndo;
}

void KlustersView::undoAddedClusters(QList<int>& addedClusters,bool active){
    //If any of the clusters in addedClusters are present, remove them
    //add back the removed clusters
    addRemovedClusters(active);

    //List containing the clusters of this view which have to be removed
    QList<int> inView = clustersInView(addedClusters);

    if(!inView.isEmpty()){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = inView.begin(); clustersToRemoveIterator != inView.end(); ++clustersToRemoveIterator){
            removeClusterFromView(*clustersToRemoveIterator,active);
        }
    }
    --numberUndo;
}

void KlustersView::undoModifiedClusters(QList<int>& updatedClusters,bool active){
    //add back the removed clusters
    addRemovedClusters(active);

    //If any of the clusters in modifiedClusters are present, update them

    //List containing the clusters of this view which have to be updated
    QList<int> inView = clustersInView(updatedClusters);
    if(!inView.isEmpty()){
        emit modifiedClustersUndo(inView,active);
    }
    --numberUndo;
}

void KlustersView::undo(QList<int>& addedClusters,QList<int>& updatedClusters,bool active){  
    //If any of the clusters in addedClusters are present, remove them.

    //List containing the clusters of this view which have to be removed
    QList<int> inView = clustersInView(addedClusters);
    if(!inView.isEmpty()){
        QList<int>::iterator clustersToRemoveIterator;
        for(clustersToRemoveIterator = inView.begin(); clustersToRemoveIterator != inView.end(); ++clustersToRemoveIterator){
            removeClusterFromView(*clustersToRemoveIterator,active);
        }
    }

    //add back the removed clusters
    addRemovedClusters(active);

    //If any of the clusters in modifiedClusters are present, update them
    //List containing the clusters of this view which have to be updated
    inView = clustersInView(updatedClusters);
    if(!inView.isEmpty()) emit modifiedClustersUndo(inView,active);

    --numberUndo;
}


bool KlustersView::removeUndoAddedClusters(bool active){
    bool isClustersRemoved = false;

    //If removedClustersRedoList is not empty, make the current removedClusters become the first element
    //of the removedClustersUndoList and the first element of the removedClustersRedoList become the current removedClusters.
    if(!removedClustersRedoList.isEmpty()){
        removedClustersUndoList.prepend(removedClusters);
        QList<int>* removedClustersTemp = removedClustersRedoList.takeAt(0);
        removedClusters =  removedClustersTemp;

        //List containing the clusters of this view which have to be removed
        QList<int> clustersToRemoveInView = clustersInView(*removedClusters);

        if(!clustersToRemoveInView.isEmpty()){
            isClustersRemoved = true;
            QList<int>::iterator deleteClusterIterator;
            for(deleteClusterIterator = clustersToRemoveInView.begin(); deleteClusterIterator != clustersToRemoveInView.end(); ++deleteClusterIterator){
                removeClusterFromView(*deleteClusterIterator,active);
            }
        }
    }
    return isClustersRemoved;
}


void KlustersView::removeDeletedClusters(bool active,QList<int>& clustersToDelete){
    //List containing the clusters of this view which have to be removed
    QList<int> clustersToRemoveInView = clustersInView(clustersToDelete);

    if(!clustersToRemoveInView.isEmpty()){
        QList<int>::iterator deleteClusterIterator;
        for(deleteClusterIterator = clustersToRemoveInView.begin(); deleteClusterIterator != clustersToRemoveInView.end(); ++deleteClusterIterator){
            removeClusterFromView(*deleteClusterIterator,active);
        }
    }
}

void KlustersView::redo(bool active,QList<int>& deletedClusters){
    removeDeletedClusters(active,deletedClusters);
    removeUndoAddedClusters(active);

    numberUndo++;
}

void KlustersView::redoAddedClusters(QList<int>& addedClusters,bool active,QList<int>& deletedClusters){     
    bool isClustersRemoved = removeUndoAddedClusters(active);
    removeDeletedClusters(active,deletedClusters);

    //Add back all the clusters contained in addedClusters if the view have had initially
    //clusters removed to enable the addition of clusters.
    if(isClustersRemoved){
        QList<int>::iterator newClusterIterator;
        for(newClusterIterator = addedClusters.begin(); newClusterIterator != addedClusters.end(); ++newClusterIterator){
            shownClusters->append(*newClusterIterator);

            emit newClusterAddedToView(*newClusterIterator,active);
        }
    }
    numberUndo++;
}

void KlustersView::redoModifiedClusters(QList<int>& updatedClusters,bool isModifiedByDeletion,bool active,QList<int>& deletedClusters){
    removeDeletedClusters(active,deletedClusters);
    removeUndoAddedClusters(active);

    //If any of the clusters in modifiedClusters are present, update them

    //List containing the clusters of this view which have to be updated
    QList<int> inView = clustersInView(updatedClusters);
    if(!inView.isEmpty())
        emit modifiedClusters(inView,active,isModifiedByDeletion);

    numberUndo++;
}

void KlustersView::redo(QList<int>& addedClusters,QList<int>& updatedClusters,bool isModifiedByDeletion,bool active,QList<int>& deletedClusters){
    removeDeletedClusters(active,deletedClusters);
    removeUndoAddedClusters(active);

    //if there are as many clusters added than updated,
    //the action was the creation of several clusters. In that case add back ony the clusters
    //corresponding to the clusters updated existing in the view.
    //The 2 lists are the keys and values of a previous map fromClusterId-new clusterId,
    //so they are order in correspondance.
    if(addedClusters.size() == updatedClusters.size()){
        QList<int> inView;
        QList<int>::iterator iterator;
        int index = 0;
        for (iterator = updatedClusters.begin(); iterator != updatedClusters.end(); ++iterator){
            if(shownClusters->contains(*iterator) != 0){
                inView.append(*iterator);
                shownClusters->append(addedClusters[index]);
                emit newClusterAddedToView(addedClusters[index],active);
            }
            ++index;
        }
        //If any of the clusters in modifiedClusters are present, update them
        if(!inView.isEmpty())
            emit modifiedClusters(inView,active,isModifiedByDeletion);
    }
    else{
        //If any of the clusters in modifiedClusters are present, update them
        //List containing the clusters of this view which have to be updated
        QList<int> inView = clustersInView(updatedClusters);
        if(!inView.isEmpty()) {
            //Add back all the clusters contained in addedClusters
            QList<int>::iterator newClusterIterator;
            for(newClusterIterator = addedClusters.begin(); newClusterIterator != addedClusters.end(); ++newClusterIterator){
                //If the clusters have been modified by deletion, that means that the clusters to add
                //can only be cluster 0 or cluster 1 which were added because they did not exit already.
                //In that case we do not want to add them back to the view.
                if(isModifiedByDeletion) 
                   continue;
                shownClusters->append(*newClusterIterator);
                emit newClusterAddedToView(*newClusterIterator,active);
            }
            emit modifiedClusters(inView,active,isModifiedByDeletion);
        }
    }
    numberUndo++;
}

void KlustersView::changeClusterIds(QMap<int,int>& clusterIds){
    QList<int>* shownClustersTemp = new QList<int>();

    //Update the clusterIds
    QList<int>::iterator shownClustersIterator;
    for(shownClustersIterator = shownClusters->begin(); shownClustersIterator != shownClusters->end(); ++shownClustersIterator){
        shownClustersTemp->append(clusterIds[*shownClustersIterator]);
    }


    delete shownClusters;
    shownClusters = shownClustersTemp;

}

void KlustersView::renumberClusters(QMap<int,int>& clusterIdsOldNew,bool active){
    //renumber the clusters
    changeClusterIds(clusterIdsOldNew);

    QList<int>* removedClustersTemp = new QList<int>();
    prepareUndo(removedClustersTemp);

    emit clustersRenumbered(active);
}



void KlustersView::undoRenumbering(QMap<int,int>& clusterIdsNewOld,bool active){
    //renumber the clusters
    changeClusterIds(clusterIdsNewOld);

    //add back the removed clusters
    addRemovedClusters(active);


    numberUndo--;

    emit clustersRenumbered(active);
}

void KlustersView::redoRenumbering(QMap<int,int>& clusterIdsOldNew,bool active){
    //renumber the clusters
    changeClusterIds(clusterIdsOldNew);

    removeUndoAddedClusters(active);

    ++numberUndo;

    emit clustersRenumbered(active);
}

bool KlustersView::isThreadsRunning() const{
    bool threadsRunning = false;
    int nbViews = viewList.count();
    for(int i = 0; i< nbViews; i++) {
        ViewWidget* widget = viewList.at(i);
        if(widget->isThreadsRunning()){
            threadsRunning = true;
            widget->willBeKilled();
        }
    }

    if(threadsRunning) return true;
    else return false;
}

QList< QList<int>* > KlustersView::getUndoList(){
    QList< QList<int>* > undoList;
    for(int i = 0; i<removedClustersUndoList.count();++i) {
        QList<int>* undoCopy = new QList<int>();
        const QList<int>* lst = removedClustersUndoList.at(i);
        for(int j= 0; j<lst->count();++j) {
            undoCopy->append(lst->at(j));
        }
        undoList.append(undoCopy);
    }

    return  undoList;
}

QList< QList<int>* >  KlustersView::getRedoList(){
    QList< QList<int>* > redoList;
    for(int i = 0; i<removedClustersRedoList.count();++i) {
        QList<int>* redoCopy = new QList<int>();
        const QList<int>* lst = removedClustersRedoList.at(i);
        for(int j= 0; j<lst->count();++j) {
            redoCopy->append(lst->at(j));
        }
        redoList.append(redoCopy);
    }

    return  redoList;
}

void KlustersView::setConnections(DisplayType displayType, QWidget* view,QDockWidget* dockWidget){
    //Connection(s) common to all widgets.
    connect(this,SIGNAL(updateContents()),view, SLOT(update()));
    
    //Connections common to ClusterView, WaveformView and CorrelationView
    if((displayType == CLUSTERS) || (displayType == WAVEFORMS) || (displayType == CORRELATIONS)){
        connect(this,SIGNAL(singleColorUpdated(int,bool)),view, SLOT(singleColorUpdate(int,bool)));
        connect(this,SIGNAL(clusterRemovedFromView(int,bool)),view, SLOT(removeClusterFromView(int,bool)));
        connect(this,SIGNAL(clusterAddedToView(int,bool)),view, SLOT(addClusterToView(int,bool)));
        connect(this,SIGNAL(newClusterAddedToView(QList<int>&,int,bool)),view, SLOT(addNewClusterToView(QList<int>&,int,bool)));
        connect(this,SIGNAL(newClusterAddedToView(int,bool)),view, SLOT(addNewClusterToView(int,bool)));
        connect(this,SIGNAL(spikesRemovedFromClusters(QList<int>&,bool)),view, SLOT(spikesRemovedFromClusters(QList<int>&,bool)));
        connect(this,SIGNAL(modeToSet(BaseFrame::Mode)),view, SLOT(setMode(BaseFrame::Mode)));
        connect(this,SIGNAL(spikesAddedToCluster(int,bool)),view, SLOT(spikesAddedToCluster(int,bool)));
        connect(this,SIGNAL(modifiedClusters(QList<int>&,bool,bool)),view, SLOT(updateClusters(QList<int>&,bool,bool)));
        connect(this,SIGNAL(modifiedClustersUndo(QList<int>&,bool)),view, SLOT(undoUpdateClusters(QList<int>&,bool)));
        connect(this,SIGNAL(updateDrawing()),view, SLOT(updateDrawing()));
        connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
    }

    if(displayType == CLUSTERS){ //Connections for ClusterViews
        connect(this,SIGNAL(changeTimeInterval(int,bool)),view, SLOT(setTimeStepInSecond(int,bool)));
        connect(this,SIGNAL(updatedDimensions(int,int)),view, SLOT(updatedDimensions(int,int)));
        connect(this,SIGNAL(emptySelection()),view, SLOT(emptySelection()));
        connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(clusterDockClosed(QObject*)));

        //Connect the clusterView to a possible TraceView
        if(isThereTraceView){
            connect(view,SIGNAL(moveToTime(long)),traceWidget, SLOT(moveToTime(long)));
        }
    } else if(displayType == WAVEFORMS) { //Connections for WaveformViews
        connect(this,SIGNAL(updatedTimeFrame(long,long)),view, SLOT(setTimeFrame(long,long)));
        connect(this,SIGNAL(sampleMode()),view, SLOT(setSampleMode()));
        connect(this,SIGNAL(timeFrameMode()),view, SLOT(setTimeFrameMode()));
        connect(this,SIGNAL(meanPresentation()),view, SLOT(setMeanPresentation()));
        connect(this,SIGNAL(allWaveformsPresentation()),view, SLOT(setAllWaveformsPresentation()));
        connect(this,SIGNAL(overLayPresentation()),view, SLOT(setOverLayPresentation()));
        connect(this,SIGNAL(sideBySidePresentation()),view, SLOT(setSideBySidePresentation()));
        connect(this,SIGNAL(increaseAmplitude()),view, SLOT(increaseAmplitude()));
        connect(this,SIGNAL(decreaseAmplitude()),view, SLOT(decreaseAmplitude()));
        connect(this,SIGNAL(updateDisplayNbSpikes(long)),view, SLOT(setDisplayNbSpikes(long)));
        connect(this,SIGNAL(changeGain(int)),view, SLOT(setGain(int)));
        connect(this,SIGNAL(changeChannelPositions(QList<int>&)),view, SLOT(setChannelPositions(QList<int>&)));
        connect(this,SIGNAL(clustersRenumbered(bool)),view, SLOT(clustersRenumbered(bool)));
        connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(waveformDockClosed(QObject*)));
    } else if(displayType == CORRELATIONS){ //Connections for CorrelationViews
        connect(this,SIGNAL(updatedBinSizeAndTimeFrame(int,int)),view, SLOT(setBinSizeAndTimeWindow(int,int)));
        connect(this,SIGNAL(noScale()),view, SLOT(setNoScale()));
        connect(this,SIGNAL(maxScale()),view, SLOT(setMaximumScale()));
        connect(this,SIGNAL(shoulderScale()),view, SLOT(setShoulderScale()));
        connect(this,SIGNAL(increaseAmplitudeofCorrelograms()),view, SLOT(increaseAmplitude()));
        connect(this,SIGNAL(decreaseAmplitudeofCorrelograms()),view, SLOT(decreaseAmplitude()));
        connect(this,SIGNAL(setShoulderLine(bool)),view, SLOT(setShoulderLine(bool)));
        connect(this,SIGNAL(clustersRenumbered(bool)),view, SLOT(clustersRenumbered(bool)));
        connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(correlogramDockClosed(QObject*)));
    } else if(displayType == ERROR_MATRIX){ //Connections for ErrorMatrixViews
        connect(this,SIGNAL(computeProbabilities()),view, SLOT(updateMatrixContents()));
        connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(errorMatrixDockClosed(QObject*)));
        //connection with the document
        connect(&doc,SIGNAL(clustersGrouped(QList<int>&,int)),view, SLOT(clustersGrouped(QList<int>&,int)));
        connect(&doc,SIGNAL(clustersDeleted(QList<int>&,int)),view, SLOT(clustersDeleted(QList<int>&,int)));
        connect(&doc,SIGNAL(removeSpikesFromClusters(QList<int>&,int,QList<int>&)),view, SLOT(removeSpikesFromClusters(QList<int>&,int,QList<int>&)));
        connect(&doc,SIGNAL(newClusterAdded(QList<int>&,int,QList<int>&)),view, SLOT(newClusterAdded(QList<int>&,int,QList<int>&)));
        connect(&doc,SIGNAL(newClustersAdded(QMap<int,int>&,QList<int>&)),view, SLOT(newClustersAdded(QMap<int,int>&,QList<int>&)));
        connect(&doc,SIGNAL(renumber(QMap<int,int>&)),view, SLOT(renumber(QMap<int,int>&)));
        connect(&doc,SIGNAL(undoRenumbering(QMap<int,int>&)),view, SLOT(undoRenumbering(QMap<int,int>&)));
        connect(&doc,SIGNAL(undoAdditionModification(QList<int>&,QList<int>&)),view, SLOT(undoAdditionModification(QList<int>&,QList<int>&)));
        connect(&doc,SIGNAL(undoAddition(QList<int>&)),view, SLOT(undoAddition(QList<int>&)));
        connect(&doc,SIGNAL(undoModification(QList<int>&)),view, SLOT(undoModification(QList<int>&)));
        connect(&doc,SIGNAL(redoRenumbering(QMap<int,int>&)),view, SLOT(redoRenumbering(QMap<int,int>&)));
        connect(&doc,SIGNAL(redoAdditionModification(QList<int>&,QList<int>&,bool,QList<int>&)),view, SLOT(redoAdditionModification(QList<int>&,QList<int>&,bool,QList<int>&)));
        connect(&doc,SIGNAL(redoAddition(QList<int>&,QList<int>&)),view, SLOT(redoAddition(QList<int>&,QList<int>&)));
        connect(&doc,SIGNAL(redoModification(QList<int>&,bool,QList<int>&)),view, SLOT(redoModification(QList<int>&,bool,QList<int>&)));
        connect(&doc,SIGNAL(redoDeletion(QList<int>&)),view, SLOT(redoDeletion(QList<int>&)));
        connect(&doc,SIGNAL(newClustersAdded(QList<int>&)),view, SLOT(newClustersAdded(QList<int>&)));
        connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
    } else if(displayType == TRACES){ //Connections for TraceViews
    
        connect(this,SIGNAL(updateContents()),view, SLOT(updateContents()));
        connect(this,SIGNAL(singleColorUpdated(int,bool)),view, SLOT(updateDrawing()));
        connect(this,SIGNAL(updateClusters(QString,QList<int>&,ItemColors*,bool)),view,SLOT(updateClusters(QString,QList<int>&,ItemColors*,bool)));

        connect(this,SIGNAL(updateDrawing()),view, SLOT(updateDrawing()));
        connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
        connect(view, SIGNAL(destroyed(QObject*)), this, SLOT(traceDockClosed(QObject*)));
        connect(this,SIGNAL(increaseAllAmplitude()),view,SLOT(increaseAllChannelsAmplitude()));
        connect(this,SIGNAL(decreaseAllAmplitude()),view,SLOT(decreaseAllChannelsAmplitude()));
        connect(view,SIGNAL(updateStartAndDuration(long,long)),this, SLOT(setStartAndDuration(long,long)));
        connect(this,SIGNAL(showLabels(bool)),view, SLOT(showLabels(bool)));
        connect(this,SIGNAL(nextCluster()),traceWidget,SLOT(showNextCluster()));
        connect(this,SIGNAL(previousCluster()),traceWidget,SLOT(showPreviousCluster()));

        //Connect the TraceView to possible clusterViews
        if(isThereClusterView){
            int nbViews = viewList.count();
            for(int i = 0; i< nbViews; i++) {
                ViewWidget* viewWidget = viewList.at(i);
                if(qobject_cast<ClusterView*>(viewWidget)){
                    connect(viewWidget,SIGNAL(moveToTime(long)),view, SLOT(moveToTime(long)));
                }
            }
        }
    }
}

void KlustersView::updateTraceView(QString name,ItemColors* clusterColors,bool active){     
    //Set the list of the current clusters as the list of clusters to look up in the ClusterProvider.
    if(doc.getClustersProvider()  )
        doc.getClustersProvider()->setClusterIdList(shownClusters);

    emit updateClusters(name,*shownClusters,clusterColors,active);
}

void KlustersView::updateClustersProvider(){     
    //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
    if(doc.getClustersProvider()  )
        doc.getClustersProvider()->setClusterIdList(shownClusters);
}

void KlustersView::updateCorrelogramConnections(ViewWidget* viewWidget){     
    //First disconnect the view
    QObject::disconnect(this, SIGNAL(updatedBinSizeAndTimeFrame(int,int)),0,0);
    QObject::disconnect(this, SIGNAL(noScale()),0,0);
    QObject::disconnect(this, SIGNAL(maxScale()),0,0);
    QObject::disconnect(this, SIGNAL(shoulderScale()),0,0);
    QObject::disconnect(this, SIGNAL(increaseAmplitudeofCorrelograms()),0,0);
    QObject::disconnect(this, SIGNAL(decreaseAmplitudeofCorrelograms()),0,0);
    QObject::disconnect(this, SIGNAL(setShoulderLine(bool)),0,0);

    //Connect the viewWidget
    connect(this,SIGNAL(updatedBinSizeAndTimeFrame(int,int)),viewWidget, SLOT(setBinSizeAndTimeWindow(int,int)));
    connect(this,SIGNAL(noScale()),viewWidget, SLOT(setNoScale()));
    connect(this,SIGNAL(maxScale()),viewWidget, SLOT(setMaximumScale()));
    connect(this,SIGNAL(shoulderScale()),viewWidget, SLOT(setShoulderScale()));
    connect(this,SIGNAL(increaseAmplitudeofCorrelograms()),viewWidget, SLOT(increaseAmplitude()));
    connect(this,SIGNAL(decreaseAmplitudeofCorrelograms()),viewWidget, SLOT(decreaseAmplitude()));
    connect(this,SIGNAL(setShoulderLine(bool)),viewWidget, SLOT(setShoulderLine(bool)));
}

void KlustersView::updateTimeFrame(long start,long timeFrameWidth)
{
    startTime = start;
    timeWindow = timeFrameWidth;
    qDebug()<<" void KlustersView::updateTimeFrame(long start,long timeFrameWidth)";
    emit updatedTimeFrame(start,timeFrameWidth);
}
