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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//General C++ include files
#include <iostream>
using namespace std;

// include files for Qt
#include <qpainter.h>
#include <qdir.h>
#include <qlabel.h>
#include <qdeepcopy.h>
#include <qpaintdevicemetrics.h>

// include files for KDE
#include <klocale.h>
#include <kpopupmenu.h>

// application specific includes
#include "klusters.h"
#include "klustersview.h"
#include "klustersdoc.h"
#include "clusterview.h"
#include "waveformview.h"
#include "errormatrixview.h"
#include "tracewidget.h"

extern int nbUndo;

const QString KlustersView::DisplayTypeNames[]={"Cluster Display","Waveform Display","Correlation Display","Overview Display","Grouping Assistant Display","Error Matrix Display","Trace Display"};


KlustersView::KlustersView(KlustersApp& mainWindow,KlustersDoc& pDoc,QColor backgroundColor,int initialDimensionX,int initialDimensionY,
 QValueList<int>* initialClusterList, DisplayType type, QWidget *parent, const char* name, int wflags,KStatusBar * statusBar,int timeInterval,int maxAmplitude, 
 QValueList<int> positions,bool isTimeFrameMode,long start,long timeFrameWidth,long nbSpkToDisplay,bool overLay,bool mean,
 int binSize, int correlationTimeFrame,Data::ScaleMode scale,bool shoulderLine,long startingTime,long duration,bool labelsDisplay, QPtrList< QValueList<int> > undoList, QPtrList< QValueList<int> > redoList)
 : KDockArea(parent, name),doc(pDoc), removedClustersUndoList(undoList),removedClustersRedoList(redoList),dimensionX(initialDimensionX),dimensionY(initialDimensionY),
 currentViewWidget(0L),numberUndo(undoList.count()),inTimeFrameMode(isTimeFrameMode),timeWindow(timeFrameWidth),startTime(start),nbSpkToDisplay(nbSpkToDisplay),
 overLayDisplay(overLay),meanDisplay(mean),
 binSize(binSize),correlogramTimeFrame(correlationTimeFrame),correlationScale(scale),shoulderLine(shoulderLine),
 mainWindow(mainWindow),traceWidget(0L),startingTime(startingTime),duration(duration),labelsDisplay(labelsDisplay)
{
  shownClusters = initialClusterList;
  removedClusters = new QValueList<int>();
  removedClustersUndoList.setAutoDelete(true);
  removedClustersRedoList.setAutoDelete(true);

  //Create the mainDock
  mainDock = createDockWidget("Main", QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));

  //If the type of view is a not base one, call the function to call the complex views.
  //If the type of view is a base on, construct the appropriate Widget and assign it as the mainDock widget
  //To add a new base type just add a new case with the appropriate widget (do not to add the include line)
  switch(type){
    case CLUSTERS:
      isThereWaveformView = false;
      isThereClusterView = true;
      isThereCorrelationView = false;
      isThereErrorMatrixView = false;
      isThereTraceView = false;    
      mainDock->setWidget(new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,mainDock));
      setMainDockWidget(mainDock);
      currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->getWidget());
      viewList.append(currentViewWidget);
      currentViewWidget->installEventFilter(this);//To enable right click popup menu
      mainDock->installEventFilter(this);
      viewCounter.insert("ClusterView",1);
      setConnections(CLUSTERS,currentViewWidget,mainDock);
      break;
    case WAVEFORMS:
      isThereWaveformView = true;
      isThereClusterView = false;
      isThereCorrelationView = false;
      isThereErrorMatrixView = false;
      isThereTraceView = false;    
      mainDock->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,mainDock,
                          inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));
      setMainDockWidget(mainDock);
      currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->getWidget());
      viewList.append(currentViewWidget);
      currentViewWidget->installEventFilter(this);//To enable right click popup menu
      mainDock->installEventFilter(this);
      viewCounter.insert("WaveformView",1);  
      setConnections(WAVEFORMS,currentViewWidget,mainDock);      
      break;
    case CORRELATIONS:
      isThereWaveformView = false;
      isThereClusterView = false;
      isThereCorrelationView = true;
      isThereErrorMatrixView = false;
      isThereTraceView = false;    
      mainDock->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,mainDock,correlationScale,
                          binSize,correlogramTimeFrame,shoulderLine));
      setMainDockWidget(mainDock);
      currentViewWidget = dynamic_cast<ViewWidget*>(mainDock->getWidget());
      viewList.append(currentViewWidget);
      currentViewWidget->installEventFilter(this);//To enable right click popup menu
      mainDock->installEventFilter(this);      
      viewCounter.insert("CorrelationView",1);  
      setConnections(CORRELATIONS,currentViewWidget,mainDock);      
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
      isThereWaveformView = false;
      isThereClusterView = false;
      isThereCorrelationView = false;
      isThereErrorMatrixView = false;
      isThereTraceView = true;    
      //Create the providers (data and cluster) if need it 
      if(!doc.isTracesProvider()) doc.createProviders();
      QValueList<int> skippedChannels;
      //the settings are : greyScale, no vertical lines nor rasters and waveforms, no labels displayed, no channel skipped.
      mainDock->setWidget(new TraceWidget(startingTime,duration,true,*doc.getTraceProvider(),false,false,false,
                    true,labelsDisplay,doc.getCurrentChannels(),doc.getGain(),doc.getAcquisitionGain(),doc.channelColors(),
                    doc.getDisplayGroupsChannels(),doc.getDisplayChannelsGroups(),offsets,gains,skippedChannels,mainDock,"traces",
                    backgroundColor,statusBar,5));
      setMainDockWidget(mainDock);
      traceWidget = dynamic_cast<TraceWidget*>(mainDock->getWidget());
      //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
       doc.getClustersProvider()->setClusterIdList(shownClusters);
         
       //Add the cluster provider to the TraceView
       traceWidget->addClusterProvider(doc.getClustersProvider(),doc.getClustersProvider()->getName(),&static_cast<ItemColors&>(doc.clusterColors()),                           true,*shownClusters,doc.getDisplayGroupsClusterFile(),doc.getChannelsSpikeGroups(),
                                  doc.getNbSamplesBeforePeak(),doc.getNbSamplesAfterPeak(),clustersToSkip);

      traceWidget->installEventFilter(this);//To enable right click popup menu
      mainDock->installEventFilter(this);
      viewCounter.insert("TraceView",1);
      setConnections(TRACES,traceWidget,mainDock);    
      break;     
  }

  mainDock->setEnableDocking(KDockWidget::DockCorner); 
}


KlustersView::~KlustersView()
{
  cout << "in ~KlustersView(): "<<endl;
  delete shownClusters;
  delete removedClusters;
}

void KlustersView::createOverview(QColor backgroundColor,KStatusBar* statusBar,int timeInterval,int maxAmplitude,QValueList<int> positions){
 /*OVERVIEW type is the combination of 3 base types:
  CLUSTERS on the left side, WAVEFORMS at the right top and CORRELATIONS in the bottom right
 */
 //The main dock will be the cluster view
 ClusterView* view = new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,mainDock);
 mainDock->setWidget(view);
 setMainDockWidget(mainDock);
 currentViewWidget = view;
 viewList.append(currentViewWidget);
 currentViewWidget->installEventFilter(this);//To enable right click popup menu
 mainDock->installEventFilter(this); 
 viewCounter.insert("ClusterView",1);
 
 setConnections(CLUSTERS,currentViewWidget,mainDock);

 
 //Create and add the waveforms view
 KDockWidget* waveforms = createDockWidget( "WaveForm", QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
 waveforms->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,waveforms,
                      inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));//assign the widget
 ViewWidget* waveformView = dynamic_cast<ViewWidget*>(waveforms->getWidget());
 viewList.append(waveformView);
 waveformView->installEventFilter(this);//To enable right click popup menu
 waveforms->installEventFilter(this); 
 
 waveforms->manualDock(mainDock,KDockWidget::DockRight,50);
 waveforms->setEnableDocking(KDockWidget::DockCorner);
 viewCounter.insert("WaveformView",1);
 
 setConnections(WAVEFORMS,waveformView,waveforms);

 //Create and add the correlations view
 KDockWidget* correlations = createDockWidget( "Correlation", QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
 correlations->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,correlations,correlationScale,binSize,correlogramTimeFrame,shoulderLine));//assign the widget
 ViewWidget* correlationView = dynamic_cast<ViewWidget*>(correlations->getWidget());
 viewList.append(correlationView);
 correlationView->installEventFilter(this);//To enable right click popup menu
 correlations->installEventFilter(this);
  
 correlations->manualDock(waveforms,KDockWidget::DockBottom,50);
 correlations->setEnableDocking(KDockWidget::DockCorner);
 viewCounter.insert("CorrelationView",1);
 
 setConnections(CORRELATIONS,correlationView,correlations);
}

void KlustersView::createGroupingAssistantView(QColor backgroundColor,KStatusBar* statusBar,int timeInterval,int maxAmplitude,QValueList<int> positions){
 //First create the overview
 createOverview(backgroundColor,statusBar,timeInterval,maxAmplitude,positions);
  
 //Create and add the errorMatrixView beneath the clusterView (mainDock)
 KDockWidget* errorMatrix = createDockWidget("ErrorMatrix", QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
 errorMatrix->setWidget(new ErrorMatrixView(doc,*this,backgroundColor,statusBar,errorMatrix));//assign the widget
 ViewWidget* errorMatrixView = dynamic_cast<ViewWidget*>(errorMatrix->getWidget());
 viewList.append(errorMatrixView);
 errorMatrixView->installEventFilter(this);//To enable right click popup menu
 errorMatrix->installEventFilter(this);
 
 errorMatrix->manualDock(mainDock,KDockWidget::DockBottom,50);
 errorMatrix->setEnableDocking(KDockWidget::DockCorner);
 
 setConnections(ERROR_MATRIX,errorMatrixView,errorMatrix);
}

void KlustersView::update(KlustersView* pSender){
	if(pSender != this)
		repaint();
}

void KlustersView::print(KPrinter* pPrinter,QString filePath,bool whiteBackground)
{
  QPainter printPainter;
  QPaintDeviceMetrics metrics(pPrinter);
 
  printPainter.begin(pPrinter);


  QRect textRec = QRect(printPainter.viewport().left() + 5 ,printPainter.viewport().height() - 20,printPainter.viewport().width() - 5,20);
  QFont f("Helvetica",8);
  
  int nbViews = viewList.count(); 
  ViewWidget* widget;
  for(widget = viewList.first(); widget != 0L ; widget = viewList.next()){
   //cout<<" widget->geomerty().width() "<<widget->geometry().width()<<" widget->geomerty().height() "<<widget->geometry().height()<<endl;
   //QWidget* parent = static_cast<QWidget*>(widget->parent());
   // cout<<"parent->geometry().x() "<<parent->geometry().x()<<" parent->geomerty().y() "<<parent->geometry().y()<<endl;

   //Modify the viewport so the view will not draw on the legend
   QRect newViewport = QRect(printPainter.viewport().left(),printPainter.viewport().top(),printPainter.viewport().width(),printPainter.viewport().height());
   newViewport.setBottom(printPainter.viewport().bottom() - 20);
   printPainter.setViewport(newViewport);
   widget->print(printPainter,metrics,whiteBackground);
   printPainter.resetXForm();

   printPainter.setFont(f);
   printPainter.setPen(black);
   if(widget->isA("ClusterView")){
     ClusterView* clusterView = static_cast<ClusterView*>(widget);
     printPainter.drawText(textRec,Qt::AlignAuto | Qt::AlignVCenter,QString("File: %1      Features: %2,%3").arg(filePath).arg(clusterView->getDimensionX()).arg(clusterView->getDimensionY()));
   }
   else if(widget->isA("WaveformView")){
    if(inTimeFrameMode){
     printPainter.drawText(textRec,Qt::AlignAuto | Qt::AlignVCenter,QString("File: %1      Start Time: %2 s, Duration: %3 s").arg(filePath).arg(startTime).arg(timeWindow));      
    }
    else{
     printPainter.drawText(textRec,Qt::AlignAuto | Qt::AlignVCenter,QString("File: %1      Number of Waveforms: %2").arg(filePath).arg(nbSpkToDisplay));
    }
   }
   else if(widget->isA("CorrelationView")){
    QString scaleType;
    switch(correlationScale){
     case Data::RAW :
      scaleType = "Uniform Scale";
      break;
     case Data::MAX :
      scaleType = "Scale by Maximum";
      break;
     case Data::SHOULDER :
       scaleType = "Scale by Asymptote";
       break;
    }
    printPainter.drawText(textRec,Qt::AlignAuto | Qt::AlignVCenter,QString("File: %1      %2, Duration: %3 ms, Bin Size: %4 ms").arg(filePath).arg(scaleType).arg(correlogramTimeFrame/2).arg(binSize));
   }
   if(widget->isA("ErrorMatrixView")){
    printPainter.drawText(textRec,Qt::AlignAuto | Qt::AlignVCenter,QString("File: %1").arg(filePath));
   }
   
   nbViews --;
   if(nbViews > 0) pPrinter->newPage();
  }
  
  //Print the trace view if exists
  if(isThereTraceView){
   pPrinter->newPage();
   printPainter.resetXForm();
   //Print the TraceView
   traceWidget->print(printPainter,metrics,filePath,whiteBackground);
  }
  
  printPainter.end();
}

void  KlustersView::clusterDockClosed(QWidget* clusterView){               
  KDockWidget* dock = dockManager->findWidgetParentDock(clusterView);
  dock->undock();
  viewList.remove(dynamic_cast<ViewWidget*>(clusterView));
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
  ViewWidget* viewWidget;
  for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next()){
   if(viewWidget->isA("ClusterView")){
    dimensionX = dynamic_cast<ClusterView*>(viewWidget)->getDimensionX();
    dimensionY = dynamic_cast<ClusterView*>(viewWidget)->getDimensionY();
    mainWindow.updateDimensionSpinBoxes(dimensionX,dimensionY);
    QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
    connect(this,SIGNAL(updatedDimensions(int,int)),viewWidget, SLOT(updatedDimensions(int,int))); 
    break;
   }
  }

  delete dock;  
}

void KlustersView::waveformDockClosed(QWidget* waveformView){
 //Be sure that all the threads are finished before procceding.
 if(dynamic_cast<ViewWidget*>(waveformView)->isThreadsRunning()){
  QApplication::restoreOverrideCursor();//Clear any previous override comin gfrom this function.
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  dynamic_cast<WaveformView*>(waveformView)->willBeKilled();
  
  QTimer::singleShot(1000,dynamic_cast<WaveformView*>(waveformView), SLOT(dockBeingClosed()));
  return;
 }
 
 QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.

 KDockWidget* dock = dockManager->findWidgetParentDock(waveformView);
 dock->undock();
 viewList.remove(dynamic_cast<ViewWidget*>(waveformView));

 //For the time being only one WaveformView is allowed in a single View, but in the
 //future who knows ;0). This counter will make it easier to allow multiple WaveformView.
 if(viewCounter["WaveformView"] == 1){
  viewCounter.remove("WaveformView");
  mainWindow.widgetRemovedFromDisplay(WAVEFORMS);
  isThereWaveformView = false;
 }
 else viewCounter["WaveformView"]--;

 delete dock; 
}

void KlustersView::correlogramDockClosed(QWidget* correlogramView){
  //Be sure that all the threads are finished before procceding.
  if(dynamic_cast<ViewWidget*>(correlogramView)->isThreadsRunning()){
   QApplication::restoreOverrideCursor();//Clear any previous override comin from this function.
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   dynamic_cast<CorrelationView*>(correlogramView)->willBeKilled();
   QTimer::singleShot(1000,dynamic_cast<CorrelationView*>(correlogramView), SLOT(dockBeingClosed()));
   return;
  }

  QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.

  KDockWidget* dock = dockManager->findWidgetParentDock(correlogramView);
  dock->undock();
  viewList.remove(dynamic_cast<ViewWidget*>(correlogramView));

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
  ViewWidget* viewWidget;
  for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next()){
   if(viewWidget->isA("CorrelationView")){
    binSize = dynamic_cast<CorrelationView*>(viewWidget)->getBinSize();
    correlogramTimeFrame = dynamic_cast<CorrelationView*>(viewWidget)->getTimeWindow();
    correlationScale = dynamic_cast<CorrelationView*>(viewWidget)->getScaleMode();
    shoulderLine = dynamic_cast<CorrelationView*>(viewWidget)->isShoulderLine();
    mainWindow.updateCorrelogramViewVariables(binSize,correlogramTimeFrame,shoulderLine,correlationScale);
    updateCorrelogramConnections(viewWidget);
    break;
   }
  }
  
  delete dock;
}

void KlustersView::errorMatrixDockClosed(QWidget* errorMatrixView){
  //Be sure that all the threads are finished before procceding.
  if(dynamic_cast<ViewWidget*>(errorMatrixView)->isThreadsRunning()){
   QApplication::restoreOverrideCursor();//Clear any previous override comin gfrom this function.
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   dynamic_cast<ErrorMatrixView*>(errorMatrixView)->willBeKilled();
   QTimer::singleShot(1000,dynamic_cast<ErrorMatrixView*>(errorMatrixView), SLOT(dockBeingClosed()));
   return;
  }

  QApplication::restoreOverrideCursor();//Clear any previous overrided coming from this function.

  KDockWidget* dock = dockManager->findWidgetParentDock(errorMatrixView);
  dock->undock();
  viewList.remove(dynamic_cast<ViewWidget*>(errorMatrixView));
  mainWindow.widgetRemovedFromDisplay(ERROR_MATRIX);
  isThereErrorMatrixView = false;

  delete dock;
}

void KlustersView::traceDockClosed(QWidget* traceWidget){
 KDockWidget* dock = dockManager->findWidgetParentDock(traceWidget);
  dock->undock();
  if(viewCounter["TraceView"] == 1){
   viewCounter.remove("TraceView");
   mainWindow.widgetRemovedFromDisplay(TRACES);
   traceWidget = 0L;
   isThereTraceView = false;  
  } 
  else viewCounter["TraceView"]--;
  
  delete dock;  
}

bool KlustersView::eventFilter(QObject* object,QEvent* event){     
 if((event->type() == QEvent::MouseButtonPress) && (!object->isA("KlustersView"))){
 //Check if the user has selected a dockWidget containing a ClusterView. If so
 //update the dimension spin boxes to reflect the current ClusterView dimensions
 // and make the ClusterView the only view connected to the signal of update of the spin boxes.
 if(object->isA("KDockWidget")){
  QWidget* widget = dynamic_cast<KDockWidget*>(object)->getWidget();
  if(widget->isA("ClusterView")){
   ViewWidget* viewWidget;
   for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next()){
    if(viewWidget->isA("ClusterView") && (widget == viewWidget)){
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
  if(widget->isA("CorrelationView")){
   ViewWidget* viewWidget;
   for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next()){
    if(viewWidget->isA("CorrelationView") && (widget == viewWidget)){
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
  if(object->isA("ClusterView")){
   ViewWidget* widget;
   for(widget = viewList.first(); widget != 0L ; widget = viewList.next())
    if(widget->isA("ClusterView") && (object == widget)){
     dimensionX = dynamic_cast<ClusterView*>(widget)->getDimensionX();
     dimensionY = dynamic_cast<ClusterView*>(widget)->getDimensionY();
     mainWindow.updateDimensionSpinBoxes(dimensionX,dimensionY);
     QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
     connect(this,SIGNAL(updatedDimensions(int,int)),widget, SLOT(updatedDimensions(int,int)));      
     bool inProcess = dynamic_cast<ClusterView*>(widget)->isASelectionInProcess();
     if(inProcess) return QWidget::eventFilter(object,event);
   }
  }

  //Check if the user has selected a dockWidget containing a CorrelationView. If so
  //update the bin size and duration boxes to reflect the current CorrelationView values and make the CorrelationView
  // the only view connected to the signal of update of the boxes.
  if(object->isA("CorrelationView")){
   ViewWidget* viewWidget;
   for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next()){
    if(viewWidget->isA("CorrelationView") && (object == viewWidget)){
      binSize = dynamic_cast<CorrelationView*>(viewWidget)->getBinSize();
      correlogramTimeFrame = dynamic_cast<CorrelationView*>(viewWidget)->getTimeWindow();
      correlationScale = dynamic_cast<CorrelationView*>(viewWidget)->getScaleMode();
      shoulderLine = dynamic_cast<CorrelationView*>(viewWidget)->isShoulderLine();      
      mainWindow.updateCorrelogramViewVariables(binSize,correlogramTimeFrame,shoulderLine,correlationScale);
      updateCorrelogramConnections(viewWidget);
    }
   }    
  }
  
  
  QWidget* widget;
  if(object->isA("KDockWidget")) widget = dynamic_cast<KDockWidget*>(object)->getWidget();
  else if(object->isA("ClusterView") || object->isA("WaveformView") || object->isA("CorrelationView") || object->isA("ErrorMatrixView")
         || object->isA("TraceWidget"))
    widget = dynamic_cast<QWidget*>(object);
  //if the object is a TraceView take its container the TraceWidget
  else if(object->isA("TraceView")) widget = traceWidget;
  else return QWidget::eventFilter(object,event);    // standard event processing  
  
  QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
  if(mouseEvent->button() == QMouseEvent::RightButton){
   //Create the popmenu
   KPopupMenu menu(this);
   menu.insertTitle(i18n("Add a View"));

   int clusterView = menu.insertItem(i18n("Add a ClusterView"));
   int waveformView = menu.insertItem(i18n("Add a WaveformView"));
   int correlationView = menu.insertItem(i18n("Add a CorrelationView"));
   int errorMatrixView = menu.insertItem(i18n("Add an ErrorMatrixView"));
   int traceView = menu.insertItem(i18n("Add a TraceView"));

   //A traceView is possible only if the variables it needs are available (provided in the new parameter file) and
   //the .dat file exists.
   if(!doc.areTraceDataAvailable() || !doc.isTraceViewVariablesAvailable()) menu.setItemEnabled(traceView,false); 
   
   //For the moment only one WaveformView and TraceView are allowed per View.
   if(viewCounter.contains("WaveformView")) menu.setItemEnabled(waveformView,false); 
   
   if(viewCounter.contains("CorrelationView")) menu.setItemEnabled(correlationView,true); 
   
   if(viewCounter.contains("TraceView")) menu.setItemEnabled(traceView,false); 

   //Only one ErrorMatrixView is allowed for the whole application.
   if(mainWindow.isExistAnErrorMatrix()) menu.setItemEnabled(errorMatrixView,false);
   
   menu.setMouseTracking(TRUE);
   int id = menu.exec(QCursor::pos());
   
   if(id == clusterView){
    mainWindow.widgetAddToDisplay(CLUSTERS,dockManager->findWidgetParentDock(widget));
    return true;
   }
   else if(id == waveformView){
    mainWindow.widgetAddToDisplay(WAVEFORMS,dockManager->findWidgetParentDock(widget));
    return true;
   }
   else if(id == correlationView){
    mainWindow.widgetAddToDisplay(CORRELATIONS,dockManager->findWidgetParentDock(widget));
    return true;
   }
   else if(id == errorMatrixView){
    mainWindow.widgetAddToDisplay(ERROR_MATRIX,dockManager->findWidgetParentDock(widget));
    return true;
   }
   else if(id == traceView){
    mainWindow.widgetAddToDisplay(TRACES,dockManager->findWidgetParentDock(widget));
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

bool KlustersView::addView(KDockWidget* dockWidget,DisplayType displayType,QColor backgroundColor,KStatusBar* statusBar,int timeInterval,int maxAmplitude,QValueList<int> positions){

 //Enable docking abilities
 dockWidget->setDockSite(KDockWidget::DockCorner);
 KDockWidget* clusters;
 KDockWidget* waveforms;
 KDockWidget* correlations;
 KDockWidget* errorMatrix;
 ViewWidget* clusterView;
 ViewWidget* waveformView;
 ViewWidget* correlationView;
 ViewWidget* errorMatrixView;
 KDockWidget* traces;
 QValueList<int> skippedChannels;
 
 bool newViewType = false;
 QString count;
 
 switch(displayType){
  case CLUSTERS:
   if(!isThereClusterView){
    newViewType = true;
    viewCounter.insert("ClusterView",1);
   }
   else viewCounter["ClusterView"]++;
   
   isThereClusterView = true;
   count = QString("%1").arg(viewCounter["ClusterView"]);
   
   clusters = createDockWidget(count.prepend("ClusterView"), QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
   clusters->setWidget(new ClusterView(doc,*this,backgroundColor,timeInterval,statusBar,clusters));
   clusterView = dynamic_cast<ViewWidget*>(clusters->getWidget());
   viewList.append(clusterView);
   clusterView->installEventFilter(this);//To enable right click popup menu
   clusters->installEventFilter(this);
   
   clusters->manualDock(dockWidget,KDockWidget::DockBottom,50);
   clusters->setEnableDocking(KDockWidget::DockCorner);

   //Make the new ClusterView the only view connected to the signal of update of the spin boxes.
   //To do so disconnect all the other clusterViews connected, the actual connection for the current view is done in setConnections.
   QObject::disconnect(this, SIGNAL(updatedDimensions(int,int)),0,0);
 
   setConnections(CLUSTERS,clusterView,clusters);

   //Give to the new view the same mode than the other clusterviews
   if(!newViewType){
    ViewWidget* viewWidget;
    for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next())
     if(viewWidget->isA("ClusterView")){
      clusterView->setMode(dynamic_cast<ClusterView*>(viewWidget)->getMode());

      break;
     }
   }

   break;
  case WAVEFORMS:
   if(!isThereWaveformView){
    newViewType = true;
    viewCounter.insert("WaveformView",1);
   }
   else  viewCounter["WaveformView"]++;

   isThereWaveformView = true;
   count = QString("%1").arg(viewCounter["WaveformView"]);
   
   waveforms = createDockWidget(count.prepend("WaveformView"), QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
   waveforms->setWidget(new WaveformView(doc,*this,backgroundColor,maxAmplitude,positions,statusBar,waveforms,
                     inTimeFrameMode,startTime,timeWindow,nbSpkToDisplay,overLayDisplay,meanDisplay));//assign the widget
   waveformView = dynamic_cast<ViewWidget*>(waveforms->getWidget());
   viewList.append(waveformView);
   waveformView->installEventFilter(this);//To enable right click popup menu
   waveforms->installEventFilter(this);
   
   waveforms->manualDock(dockWidget,KDockWidget::DockBottom,50);
   waveforms->setEnableDocking(KDockWidget::DockCorner);

   setConnections(WAVEFORMS,waveformView,waveforms);
   break;
  case CORRELATIONS:
   if(!isThereCorrelationView){
    newViewType = true;
    viewCounter.insert("CorrelationView",1);
   }
   else  viewCounter["CorrelationView"]++;

   isThereCorrelationView = true;
   count = QString("%1").arg(viewCounter["CorrelationView"]);
   
   correlations = createDockWidget(count.prepend("CorrelationView"), QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
   correlations->setWidget(new CorrelationView(doc,*this,backgroundColor,statusBar,correlations,correlationScale,binSize,correlogramTimeFrame,shoulderLine));//assign the widget
   correlationView = dynamic_cast<ViewWidget*>(correlations->getWidget());
   viewList.append(correlationView);
   correlationView->installEventFilter(this);//To enable right click popup menu
   correlations->installEventFilter(this);
   
   correlations->manualDock(dockWidget,KDockWidget::DockBottom,50);
   correlations->setEnableDocking(KDockWidget::DockCorner);

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
   
   errorMatrix = createDockWidget("ErrorMatrix", QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
   errorMatrix->setWidget(new ErrorMatrixView(doc,*this,backgroundColor,statusBar,errorMatrix));//assign the widget
   errorMatrixView = dynamic_cast<ViewWidget*>(errorMatrix->getWidget());
   viewList.append(errorMatrixView);
   errorMatrixView->installEventFilter(this);//To enable right click popup menu
   errorMatrix->installEventFilter(this);

   errorMatrix->manualDock(dockWidget,KDockWidget::DockBottom,50);
   errorMatrix->setEnableDocking(KDockWidget::DockCorner);

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
   count = QString("%1").arg(viewCounter["TraceView"]);
   
   traces = createDockWidget(count.prepend("TraceView"), QPixmap(), 0L, i18n(doc.documentName()), i18n(doc.documentName()));
   //the settings are : greyScale, no vertical lines nor rasters and waveforms, no labels displayed, no channel skipped.
   traces->setWidget(new TraceWidget(startingTime,duration,true,*doc.getTraceProvider(),false,false,false,
                    true,labelsDisplay,doc.getCurrentChannels(),doc.getGain(),doc.getAcquisitionGain(),doc.channelColors(),
                    doc.getDisplayGroupsChannels(),doc.getDisplayChannelsGroups(),offsets,gains,skippedChannels,traces,"traces",
                    backgroundColor,statusBar,5));//assign the widget
                         
                    
   traceWidget = dynamic_cast<TraceWidget*>(traces->getWidget());
   
   //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
   doc.getClustersProvider()->setClusterIdList(shownClusters);
     
   //Add the cluster provider to the TraceView
   traceWidget->addClusterProvider(doc.getClustersProvider(),doc.getClustersProvider()->getName(),&static_cast<ItemColors&>(doc.clusterColors()),                           true,*shownClusters,doc.getDisplayGroupsClusterFile(),doc.getChannelsSpikeGroups(),
                              doc.getNbSamplesBeforePeak(),doc.getNbSamplesAfterPeak(),clustersToSkip);
   
   traces->installEventFilter(this);//To enable right click popup menu
   traceWidget->installEventFilter(this);
   
   traces->manualDock(dockWidget,KDockWidget::DockBottom,50);
   traces->setEnableDocking(KDockWidget::DockCorner);

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


void KlustersView::shownClustersUpdate(QValueList<int>& clustersToShow){
  //Try to minimize the number of clusters to draw
  QValueVector<int> clustersToRemove;
    
  //If a cluster already shown is not requested, remove it from the view
  QValueList<int>::iterator shownClustersIterator;
  for(shownClustersIterator = shownClusters->begin(); shownClustersIterator != shownClusters->end(); ++shownClustersIterator ){ 
    if(clustersToShow.contains(*shownClustersIterator) == 0){
      clustersToRemove.push_back(*shownClustersIterator);
    }
  }

  //Remove the clusters found previously from shownClusters
  removeClustersFromView(clustersToRemove,true);
          
  //If there is a cluster in clustersToShow which is not in shownClusters, add it to the view
  QValueList<int>::iterator clustersToShowIterator;
  for(clustersToShowIterator = clustersToShow.begin(); clustersToShowIterator != clustersToShow.end(); ++clustersToShowIterator ){
    if(shownClusters->contains(*clustersToShowIterator) == 0) addClusterToView(*clustersToShowIterator,true);
  }
      
  //Show all the enclosed widgets of the dockWindows.
  showAllWidgets();
}

void KlustersView::updateColors(bool active){
  ItemColors& clusterColors = doc.clusterColors();
  if(clusterColors.isColorChanged()){
    QValueList<int> colorChangedClusterList = clusterColors.colorChangedItemList();
    QValueList<int>::iterator iterator;
    for(iterator = colorChangedClusterList.begin(); iterator != colorChangedClusterList.end(); ++iterator ){
      if(shownClusters->contains(*iterator) != 0) singleColorUpdate(*iterator,active);
    }
  }
}

void KlustersView::groupedClustersUpdate(QValueList<int>& groupedClusters, int newClusterId,bool active){  
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


bool KlustersView::clustersDeletionUpdate(QValueList<int>& deletedClusters,int destinationCluster,bool active){
  bool isAClusterRemoved = false;
  isAClusterRemoved = clustersDeletionUpdate(deletedClusters,active);

  //If the view contains the destinationCluster emit a notice of modification
  if(shownClusters->contains(destinationCluster) != 0){
   QValueList<int> modifiedcluster;
   modifiedcluster.append(destinationCluster);
   emit modifiedClusters(modifiedcluster,active,true);
  }

  return isAClusterRemoved;
}

bool KlustersView::clustersDeletionUpdate(QValueList<int>& deletedClusters,bool active){  
  QValueList<int> inView = clustersInView(deletedClusters);
  bool isAClusterRemoved = false;

  //the removedClustersUndoList have to be updated
  prepareUndo(inView);
   
  //If deletedClusters in not empty, this view is concerned by the modification
  if(inView.size() > 0){
    isAClusterRemoved = true;
  
    //If one of the clusters in deletedClusters is present in clustersShown list, remove it
    // and call removeClusterFromView on all the widgets
    QValueList<int>::iterator clustersToRemoveIterator;
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
  shownClusters->remove(clusterId);
  emit clusterRemovedFromView(clusterId,active);
}

void KlustersView::removeClustersFromView(QValueVector<int> clusterIds,bool active){
  int size = clusterIds.size();
  for(int i = 0; i<size; ++i){
    shownClusters->remove(clusterIds[i]);
    emit clusterRemovedFromView(clusterIds[i],active);
  }
}

void KlustersView::addClusterToView(int clusterId,bool active){
  //Add the clusterId to the list of clusters shown and call addClusterToView on all the widgets
  shownClusters->append(clusterId);
  emit clusterAddedToView(clusterId,active);
}

void KlustersView::addNewClusterToView(QValueList<int>& fromClusters,int clusterId,QValueList<int>& emptiedClusters,bool active){  
  //List containing the clusters of this view which contained spikes of the newly created cluster
  QValueList<int> fromClustersInView = clustersInView(fromClusters);

  //the removedClustersUndoList have to be updated
  QValueList<int> emptiedClustersInView = clustersInView(emptiedClusters);
  prepareUndo(emptiedClustersInView);

  
  //If fromClustersInView in not empty, this view is concerned by the modification
  if(fromClustersInView.size() > 0){     
    if(emptiedClusters.size()>0){
     QValueList<int>::iterator clustersToRemoveIterator;
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

void KlustersView::addNewClustersToView(QMap<int,int>& fromToNewClusterIds,QValueList<int>& emptiedClusters,bool active){
  //List containing the clusters of this view which contained spikes of the newly created cluster
  QValueList<int> fromClusters = fromToNewClusterIds.keys();
  QValueList<int> fromClustersInView = clustersInView(fromClusters);

  //the removedClustersUndoList have to be updated
  QValueList<int> emptiedClustersInView = clustersInView(emptiedClusters);
  prepareUndo(emptiedClustersInView);


  //If fromClustersInView in not empty, this view is concerned by the modification
  if(fromClustersInView.size() > 0){
    
    if(emptiedClusters.size()>0){
      QValueList<int>::iterator clustersToRemoveIterator;
      for(clustersToRemoveIterator = emptiedClusters.begin(); clustersToRemoveIterator != emptiedClusters.end(); ++clustersToRemoveIterator ){
        removeClusterFromView(*clustersToRemoveIterator,active);
      }
    }

    emit modifiedClusters(fromClustersInView,active);
    
    QValueList<int>::iterator fromClusterIterator;
    for (fromClusterIterator = fromClustersInView.begin(); fromClusterIterator != fromClustersInView.end(); ++fromClusterIterator){
      int newClusterId = fromToNewClusterIds[*fromClusterIterator];
cout << "in KlustersView::addNewClustersToView newClusterId "<<newClusterId<<endl;
      shownClusters->append(newClusterId);
      emit newClusterAddedToView(newClusterId,active);      
    }
    
  }

  //Check if some clusters have had their color changed.
  // If so, update clusterUpdateList if needed
  updateColors(active);
}


void KlustersView::addNewClustersToView(QValueList<int>& clustersToRecluster,QValueList<int>& reclusteredClusterList,bool active){
cout << "in KlustersView::addNewClustersToView: "<<endl;

  //List containing the clusters of this view which contained recluster clusters
  QValueList<int> inView = clustersInView(clustersToRecluster);

  //the removedClustersUndoList have to be updated
  prepareUndo(inView);
  
  //If inView in not empty, this view is concerned by the modification
  if(inView.size() > 0){
   //prepareUndo(clustersToRecluster);

   QValueList<int>::iterator clustersToRemoveIterator;
   for(clustersToRemoveIterator = clustersToRecluster.begin(); clustersToRemoveIterator != clustersToRecluster.end(); ++clustersToRemoveIterator ){
    cout << "*clustersToRemoveIterator: "<<*clustersToRemoveIterator<<endl;
    removeClusterFromView(*clustersToRemoveIterator,active);
   }

   QValueList<int>::iterator iterator;
   for (iterator = reclusteredClusterList.begin(); iterator != reclusteredClusterList.end(); ++iterator){
    shownClusters->append(*iterator);
     cout << "*iterator to add: "<<*iterator<<endl;
    emit newClusterAddedToView(*iterator,active);
   }
  }  
}


void KlustersView::removeSpikesFromClustersInView(QValueList<int>& fromClusters, int destinationClusterId,QValueList<int>& emptiedClusters,bool active){    
  //List containing the clusters of this view which contained spikes which were removed
  QValueList<int> fromClustersInView = clustersInView(fromClusters);

  //the removedClustersUndoList have to be updated
  QValueList<int> emptiedClustersInView = clustersInView(emptiedClusters);
  prepareUndo(emptiedClustersInView);

  //If fromClustersInView in not empty, this view is concerned by the modification
  if(fromClustersInView.size() > 0){

    if(emptiedClusters.size()>0){
      QValueList<int>::iterator clustersToRemoveIterator;
      for(clustersToRemoveIterator = emptiedClusters.begin(); clustersToRemoveIterator != emptiedClusters.end(); ++clustersToRemoveIterator ){
        removeClusterFromView(*clustersToRemoveIterator,active);
      }
    }

    emit spikesRemovedFromClusters(fromClustersInView,active);
    if(shownClusters->contains(destinationClusterId) != 0) emit spikesAddedToCluster(destinationClusterId,active);
  }

  //If the view contains the destinationCluster emit a notice of modification
  else if(shownClusters->contains(destinationClusterId) != 0){
   emit spikesAddedToCluster(destinationClusterId,active);
  }
  
  //Check if some clusters have had their color changed.
  // If so, update clusterUpdateList if needed
  updateColors(active);
}

QValueList<int> KlustersView::clustersInView(QValueList<int>& clusterlist){
  //subset of clusterlist with the clusters of this view
  QValueList<int> clustersInViewList;

  QValueList<int>::iterator iterator;
  for (iterator = clusterlist.begin(); iterator != clusterlist.end(); ++iterator){
   if(shownClusters->contains(*iterator) != 0) clustersInViewList.append(*iterator);
  }
  return clustersInViewList;
}

void KlustersView::prepareUndo(QValueList<int>* removedClustersTemp){
  //Store the current removedClusters in the undo list and make the temporary become the current one.
  removedClustersUndoList.prepend(removedClusters);
  removedClusters = removedClustersTemp;
                  
  ++numberUndo;
  
  //if the number of undo has been reach remove the last elements in the undo list (first ones inserted)
  if(numberUndo > nbUndo){
    removedClustersUndoList.remove(numberUndo - 1);
    --numberUndo;
  }

  //Clear the redoList
  removedClustersRedoList.clear();
}
    
void KlustersView::prepareUndo(QValueList<int>& newlyRemovedClusters){   
  //Create a new shownClusters which will hold the new configuration
  QValueList<int>* removedClustersTemp = new QValueList<int>();
  QValueList<int>::iterator iterator;
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
    removedClustersUndoList.remove(numberUndo - 1);
    --numberUndo;
   }
   //Clear the redoLists
   removedClustersUndoList.clear();
  }
  //currentNbUndo < newNbUndo, check the redo list.
  else{
   //number of undo and redo must be <= new number of undo. Remove redo elements if need it.
   int currentNbRedo = removedClustersRedoList.count();
   if((currentNbRedo + numberUndo) > newNbUndo){
    while((currentNbRedo + numberUndo) > newNbUndo){
     removedClustersRedoList.remove(currentNbRedo - 1);
     currentNbRedo = removedClustersRedoList.count();
    }
   }
  }
 }
}

void KlustersView::addRemovedClusters(bool active){
 //If removedClustersUndoList is not empty, make the current removedClusters become the first element
 //of the removedClustersRedoList and the first element of the removedClustersUndoList become the current removedClusters.
 if(removedClustersUndoList.count()>0){
   cout << "in addRemovedClusters removedClustersUndoList.count(): "<<removedClustersUndoList.count()<< endl;
  if(removedClusters->size() > 0){
   QValueList<int>::iterator newClusterIterator;
   for(newClusterIterator = removedClusters->begin(); newClusterIterator != removedClusters->end(); ++newClusterIterator){
     shownClusters->append(*newClusterIterator);
     cout << "in addRemovedClusters *newClusterIterator: "<<*newClusterIterator<< endl;
     emit newClusterAddedToView(*newClusterIterator,active);
   }
  }
  removedClustersRedoList.prepend(removedClusters);
  QValueList<int>* removedClustersTemp = removedClustersUndoList.take(0);
  removedClusters =  removedClustersTemp;
 }
}
 
void KlustersView::undo(bool active){
  //add back the removed clusters
  addRemovedClusters(active);
  --numberUndo;
   cout << "numberUndo in KlustersView::undo added: "<<numberUndo<< endl;
}

void KlustersView::undoAddedClusters(QValueList<int>& addedClusters,bool active){
  //If any of the clusters in addedClusters are present, remove them
  //add back the removed clusters
  addRemovedClusters(active);

  //List containing the clusters of this view which have to be removed
  QValueList<int> inView = clustersInView(addedClusters);

  if(inView.size() > 0){
   QValueList<int>::iterator clustersToRemoveIterator;
   for(clustersToRemoveIterator = inView.begin(); clustersToRemoveIterator != inView.end(); ++clustersToRemoveIterator){
    removeClusterFromView(*clustersToRemoveIterator,active);
   }
  }
  --numberUndo;
   cout << "numberUndo in KlustersView::undo modified: "<<numberUndo<<" removedClustersUndoList.count(): "<<removedClustersUndoList.count()<< endl;
}

void KlustersView::undoModifiedClusters(QValueList<int>& updatedClusters,bool active){
  //add back the removed clusters
  addRemovedClusters(active);

  //If any of the clusters in modifiedClusters are present, update them

  //List containing the clusters of this view which have to be updated
  QValueList<int> inView = clustersInView(updatedClusters);
  if(inView.size() > 0){
    emit modifiedClustersUndo(inView,active);
  }
  --numberUndo;
   cout << "numberUndo in KlustersView::undo updated: "<<numberUndo<< endl;
}

void KlustersView::undo(QValueList<int>& addedClusters,QValueList<int>& updatedClusters,bool active){  
  //If any of the clusters in addedClusters are present, remove them.

  //List containing the clusters of this view which have to be removed
  QValueList<int> inView = clustersInView(addedClusters);
  if(inView.size() > 0){
   QValueList<int>::iterator clustersToRemoveIterator;
   for(clustersToRemoveIterator = inView.begin(); clustersToRemoveIterator != inView.end(); ++clustersToRemoveIterator){
    removeClusterFromView(*clustersToRemoveIterator,active);
   }
  }

  //add back the removed clusters
  addRemovedClusters(active);

  //If any of the clusters in modifiedClusters are present, update them
  //List containing the clusters of this view which have to be updated
  inView = clustersInView(updatedClusters);
  if(inView.size() > 0) emit modifiedClustersUndo(inView,active);

  --numberUndo;
   cout << "numberUndo in KlustersView::undo added-updated: "<<numberUndo<< endl;
}


bool KlustersView::removeUndoAddedClusters(bool active){
 bool isClustersRemoved = false;
 
 //If removedClustersRedoList is not empty, make the current removedClusters become the first element
 //of the removedClustersUndoList and the first element of the removedClustersRedoList become the current removedClusters.
 if(removedClustersRedoList.count()>0){
  removedClustersUndoList.prepend(removedClusters);
  QValueList<int>* removedClustersTemp = removedClustersRedoList.take(0);
  removedClusters =  removedClustersTemp;
 
  //List containing the clusters of this view which have to be removed
  QValueList<int> clustersToRemoveInView = clustersInView(*removedClusters);

  if(clustersToRemoveInView.size() > 0){
   isClustersRemoved = true;
   QValueList<int>::iterator deleteClusterIterator;
   for(deleteClusterIterator = clustersToRemoveInView.begin(); deleteClusterIterator != clustersToRemoveInView.end(); ++deleteClusterIterator){
    removeClusterFromView(*deleteClusterIterator,active);
   }
  }
 } 
 return isClustersRemoved;
}


void KlustersView::removeDeletedClusters(bool active,QValueList<int>& clustersToDelete){
 //List containing the clusters of this view which have to be removed
 QValueList<int> clustersToRemoveInView = clustersInView(clustersToDelete);

 if(clustersToRemoveInView.size() > 0){
  QValueList<int>::iterator deleteClusterIterator;
  for(deleteClusterIterator = clustersToRemoveInView.begin(); deleteClusterIterator != clustersToRemoveInView.end(); ++deleteClusterIterator){
    removeClusterFromView(*deleteClusterIterator,active);
  }
 }
}

void KlustersView::redo(bool active,QValueList<int>& deletedClusters){
  removeDeletedClusters(active,deletedClusters);
  removeUndoAddedClusters(active);

  numberUndo++;
}

void KlustersView::redoAddedClusters(QValueList<int>& addedClusters,bool active,QValueList<int>& deletedClusters){     
  bool isClustersRemoved = removeUndoAddedClusters(active);
  removeDeletedClusters(active,deletedClusters);
  
  //Add back all the clusters contained in addedClusters if the view have had initially
  //clusters removed to enable the addition of clusters.
  if(isClustersRemoved){
   QValueList<int>::iterator newClusterIterator;
    for(newClusterIterator = addedClusters.begin(); newClusterIterator != addedClusters.end(); ++newClusterIterator){
     shownClusters->append(*newClusterIterator);

     emit newClusterAddedToView(*newClusterIterator,active);
    }
  }
  numberUndo++;
}

void KlustersView::redoModifiedClusters(QValueList<int>& updatedClusters,bool isModifiedByDeletion,bool active,QValueList<int>& deletedClusters){
  removeDeletedClusters(active,deletedClusters);
  removeUndoAddedClusters(active);
  
  //If any of the clusters in modifiedClusters are present, update them

  //List containing the clusters of this view which have to be updated
  QValueList<int> inView = clustersInView(updatedClusters);
  if(inView.size() > 0) emit modifiedClusters(inView,active,isModifiedByDeletion);

  numberUndo++;    
}

void KlustersView::redo(QValueList<int>& addedClusters,QValueList<int>& updatedClusters,bool isModifiedByDeletion,bool active,QValueList<int>& deletedClusters){
  removeDeletedClusters(active,deletedClusters);
  removeUndoAddedClusters(active);

  //if there are as many clusters added than updated,
  //the action was the creation of several clusters. In that case add back ony the clusters
  //corresponding to the clusters updated existing in the view.
  //The 2 lists are the keys and values of a previous map fromClusterId-new clusterId,
  //so they are order in correspondance.
  if(addedClusters.size() == updatedClusters.size()){    
    QValueList<int> inView;
    QValueList<int>::iterator iterator;
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
    if(inView.size() > 0) emit modifiedClusters(inView,active,isModifiedByDeletion);
  }
  else{  
   //If any of the clusters in modifiedClusters are present, update them
   //List containing the clusters of this view which have to be updated
   QValueList<int> inView = clustersInView(updatedClusters);
   if(inView.size() > 0) {
    //Add back all the clusters contained in addedClusters
    QValueList<int>::iterator newClusterIterator;
    for(newClusterIterator = addedClusters.begin(); newClusterIterator != addedClusters.end(); ++newClusterIterator){
     //If the clusters have been modified by deletion, that means that the clusters to add
     //can only be cluster 0 or cluster 1 which were added because they did not exit already.
     //In that case we do not want to add them back to the view.
     if(isModifiedByDeletion) continue;
     shownClusters->append(*newClusterIterator);
     emit newClusterAddedToView(*newClusterIterator,active);
    }
    emit modifiedClusters(inView,active,isModifiedByDeletion);
   }
  }
  numberUndo++;  
}

void KlustersView::changeClusterIds(QMap<int,int>& clusterIds){
  QValueList<int>* shownClustersTemp = new QValueList<int>();

  //Update the clusterIds
  QValueList<int>::iterator shownClustersIterator;
  for(shownClustersIterator = shownClusters->begin(); shownClustersIterator != shownClusters->end(); ++shownClustersIterator){
   shownClustersTemp->append(clusterIds[*shownClustersIterator]);
  }
  

  delete shownClusters;
  shownClusters = shownClustersTemp;

}

void KlustersView::renumberClusters(QMap<int,int>& clusterIdsOldNew,bool active){
 //renumber the clusters
 changeClusterIds(clusterIdsOldNew);

 QValueList<int>* removedClustersTemp = new QValueList<int>();
 prepareUndo(removedClustersTemp);
      
 emit clustersRenumbered(active);
}



void KlustersView::undoRenumbering(QMap<int,int>& clusterIdsNewOld,bool active){
 //renumber the clusters
 changeClusterIds(clusterIdsNewOld);

 //add back the removed clusters
 addRemovedClusters(active);

 
 numberUndo--;
 cout << "numberUndo in KlustersView::undoRenumbering: "<<numberUndo<< endl;

 emit clustersRenumbered(active);
}

void KlustersView::redoRenumbering(QMap<int,int>& clusterIdsOldNew,bool active){
 //renumber the clusters
 changeClusterIds(clusterIdsOldNew);

 removeUndoAddedClusters(active);
 
 ++numberUndo;
  cout << "numberUndo in KlustersView::redoRenumbering: "<<numberUndo<< endl;

 emit clustersRenumbered(active);
}

bool KlustersView::isThreadsRunning(){
 bool threadsRunning = false;
 ViewWidget* widget;

 for(widget = viewList.first(); widget != 0L ; widget = viewList.next())
  if(widget->isThreadsRunning()){
   threadsRunning = true;
   widget->willBeKilled();
  }
  
 if(threadsRunning) return true;
 else return false;   
}

QPtrList< QValueList<int> > KlustersView::getUndoList(){
 QPtrList< QValueList<int> > undoList;
 QValueList<int>* undo;
 for(undo = removedClustersUndoList.first(); undo; undo = removedClustersUndoList.next()){
  QValueList<int>* undoCopy = new QValueList<int>();
  QValueList<int>::iterator iterator;
  for(iterator = undo->begin(); iterator != undo->end(); ++iterator)
   undoCopy->append(*iterator);
  undoList.append(undoCopy);
 }

 return  undoList;
}

QPtrList< QValueList<int> >  KlustersView::getRedoList(){
 QPtrList< QValueList<int> > redoList;
 QValueList<int>* redo;
 for(redo = removedClustersRedoList.first(); redo; redo = removedClustersRedoList.next()){
  QValueList<int>* redoCopy = new QValueList<int>();
  QValueList<int>::iterator iterator;
  for(iterator = redo->begin(); iterator != redo->end(); ++iterator)
   redoCopy->append(*iterator);
  redoList.append(redoCopy);
 }

 return  redoList;
}

void KlustersView::setConnections(DisplayType displayType, QWidget* view,KDockWidget* dockWidget){
  //Connection(s) common to all widgets.
  connect(this,SIGNAL(updateContents()),view, SLOT(update()));
  //Enable the View to be inform that the dockWidget is being closed.
  //To do so, connect the dockwidget close button to the dockBeingClosed slot of is contained widget
  //and connect this widget parentDockBeingClosed signal to the view {cluster,waveform,correlogram,errorMatrix}DockClosed slot.
  connect(dockWidget, SIGNAL(headerCloseButtonClicked()), view, SLOT(dockBeingClosed()));
    
  //Connections common to ClusterView, WaveformView and CorrelationView
  if((displayType == CLUSTERS) || (displayType == WAVEFORMS) || (displayType == CORRELATIONS)){    
   connect(this,SIGNAL(singleColorUpdated(int,bool)),view, SLOT(singleColorUpdate(int,bool)));
   connect(this,SIGNAL(clusterRemovedFromView(int,bool)),view, SLOT(removeClusterFromView(int,bool)));
   connect(this,SIGNAL(clusterAddedToView(int,bool)),view, SLOT(addClusterToView(int,bool)));
   connect(this,SIGNAL(newClusterAddedToView(QValueList<int>&,int,bool)),view, SLOT(addNewClusterToView(QValueList<int>&,int,bool)));
   connect(this,SIGNAL(newClusterAddedToView(int,bool)),view, SLOT(addNewClusterToView(int,bool)));
   connect(this,SIGNAL(spikesRemovedFromClusters(QValueList<int>&,bool)),view, SLOT(spikesRemovedFromClusters(QValueList<int>&,bool)));
   connect(this,SIGNAL(modeToSet(BaseFrame::Mode)),view, SLOT(setMode(BaseFrame::Mode)));
   connect(this,SIGNAL(spikesAddedToCluster(int,bool)),view, SLOT(spikesAddedToCluster(int,bool)));
   connect(this,SIGNAL(modifiedClusters(QValueList<int>&,bool,bool)),view, SLOT(updateClusters(QValueList<int>&,bool,bool)));
   connect(this,SIGNAL(modifiedClustersUndo(QValueList<int>&,bool)),view, SLOT(undoUpdateClusters(QValueList<int>&,bool)));
   connect(this,SIGNAL(updateDrawing()),view, SLOT(updateDrawing()));
   connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
  }
  
  //Connections for ClusterViews
  if(displayType == CLUSTERS){
   connect(this,SIGNAL(changeTimeInterval(int,bool)),view, SLOT(setTimeStepInSecond(int,bool))); 
   connect(this,SIGNAL(updatedDimensions(int,int)),view, SLOT(updatedDimensions(int,int)));
   connect(this,SIGNAL(emptySelection()),view, SLOT(emptySelection()));
   connect(view, SIGNAL(parentDockBeingClosed(QWidget*)), this, SLOT(clusterDockClosed(QWidget*)));
   
   //Connect the clusterView to a possible TraceView
   if(isThereTraceView){
    connect(view,SIGNAL(moveToTime(long)),traceWidget, SLOT(moveToTime(long))); 
   }
  }
  
  //Connections for WaveformViews
  if(displayType == WAVEFORMS){
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
   connect(this,SIGNAL(changeChannelPositions(QValueList<int>&)),view, SLOT(setChannelPositions(QValueList<int>&)));
   connect(this,SIGNAL(clustersRenumbered(bool)),view, SLOT(clustersRenumbered(bool)));
   connect(view, SIGNAL(parentDockBeingClosed(QWidget*)), this, SLOT(waveformDockClosed(QWidget*)));
  }
  
  //Connections for CorrelationViews
  if(displayType == CORRELATIONS){
   connect(this,SIGNAL(updatedBinSizeAndTimeFrame(int,int)),view, SLOT(setBinSizeAndTimeWindow(int,int)));
   connect(this,SIGNAL(noScale()),view, SLOT(setNoScale()));
   connect(this,SIGNAL(maxScale()),view, SLOT(setMaximumScale()));
   connect(this,SIGNAL(shoulderScale()),view, SLOT(setShoulderScale()));
   connect(this,SIGNAL(increaseAmplitudeofCorrelograms()),view, SLOT(increaseAmplitude()));
   connect(this,SIGNAL(decreaseAmplitudeofCorrelograms()),view, SLOT(decreaseAmplitude()));
   connect(this,SIGNAL(setShoulderLine(bool)),view, SLOT(setShoulderLine(bool)));
   connect(this,SIGNAL(clustersRenumbered(bool)),view, SLOT(clustersRenumbered(bool)));
   connect(view, SIGNAL(parentDockBeingClosed(QWidget*)), this, SLOT(correlogramDockClosed(QWidget*)));
  }
  
  //Connections for ErrorMatrixViews
  if(displayType == ERROR_MATRIX){
   connect(this,SIGNAL(computeProbabilities()),view, SLOT(updateMatrixContents()));
   connect(view, SIGNAL(parentDockBeingClosed(QWidget*)), this, SLOT(errorMatrixDockClosed(QWidget*)));
   //connection with the document
   connect(&doc,SIGNAL(clustersGrouped(QValueList<int>&,int)),view, SLOT(clustersGrouped(QValueList<int>&,int)));
   connect(&doc,SIGNAL(clustersDeleted(QValueList<int>&,int)),view, SLOT(clustersDeleted(QValueList<int>&,int)));
   connect(&doc,SIGNAL(removeSpikesFromClusters(QValueList<int>&,int,QValueList<int>&)),view, SLOT(removeSpikesFromClusters(QValueList<int>&,int,QValueList<int>&)));
   connect(&doc,SIGNAL(newClusterAdded(QValueList<int>&,int,QValueList<int>&)),view, SLOT(newClusterAdded(QValueList<int>&,int,QValueList<int>&)));
   connect(&doc,SIGNAL(newClustersAdded(QMap<int,int>&,QValueList<int>&)),view, SLOT(newClustersAdded(QMap<int,int>&,QValueList<int>&)));
   connect(&doc,SIGNAL(renumber(QMap<int,int>&)),view, SLOT(renumber(QMap<int,int>&)));
   connect(&doc,SIGNAL(undoRenumbering(QMap<int,int>&)),view, SLOT(undoRenumbering(QMap<int,int>&)));
   connect(&doc,SIGNAL(undoAdditionModification(QValueList<int>&,QValueList<int>&)),view, SLOT(undoAdditionModification(QValueList<int>&,QValueList<int>&)));
   connect(&doc,SIGNAL(undoAddition(QValueList<int>&)),view, SLOT(undoAddition(QValueList<int>&)));
   connect(&doc,SIGNAL(undoModification(QValueList<int>&)),view, SLOT(undoModification(QValueList<int>&)));
   connect(&doc,SIGNAL(redoRenumbering(QMap<int,int>&)),view, SLOT(redoRenumbering(QMap<int,int>&)));
   connect(&doc,SIGNAL(redoAdditionModification(QValueList<int>&,QValueList<int>&,bool,QValueList<int>&)),view, SLOT(redoAdditionModification(QValueList<int>&,QValueList<int>&,bool,QValueList<int>&)));
   connect(&doc,SIGNAL(redoAddition(QValueList<int>&,QValueList<int>&)),view, SLOT(redoAddition(QValueList<int>&,QValueList<int>&)));
   connect(&doc,SIGNAL(redoModification(QValueList<int>&,bool,QValueList<int>&)),view, SLOT(redoModification(QValueList<int>&,bool,QValueList<int>&)));
   connect(&doc,SIGNAL(redoDeletion(QValueList<int>&)),view, SLOT(redoDeletion(QValueList<int>&)));
   connect(&doc,SIGNAL(newClustersAdded(QValueList<int>&)),view, SLOT(newClustersAdded(QValueList<int>&)));
   connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
  }
  
 //Connections for TraceViews
  if(displayType == TRACES){
   connect(this,SIGNAL(updateContents()),view, SLOT(updateContents()));
   connect(this,SIGNAL(singleColorUpdated(int,bool)),view, SLOT(updateDrawing()));   
   connect(this,SIGNAL(updateClusters(QString,QValueList<int>&,ItemColors*,bool)),view,SLOT(updateClusters(QString,QValueList<int>&,ItemColors*,bool)));
   
   connect(this,SIGNAL(updateDrawing()),view, SLOT(updateDrawing()));
   connect(this,SIGNAL(changeBackgroundColor(QColor)),view, SLOT(changeBackgroundColor(QColor)));
   connect(view, SIGNAL(parentDockBeingClosed(QWidget*)), this, SLOT(traceDockClosed(QWidget*)));
   connect(this,SIGNAL(increaseAllAmplitude()),view,SLOT(increaseAllChannelsAmplitude()));
   connect(this,SIGNAL(decreaseAllAmplitude()),view,SLOT(decreaseAllChannelsAmplitude()));
   connect(view,SIGNAL(updateStartAndDuration(long,long)),this, SLOT(setStartAndDuration(long,long)));
   connect(this,SIGNAL(showLabels(bool)),view, SLOT(showLabels(bool)));
   connect(this,SIGNAL(nextCluster()),traceWidget,SLOT(showNextCluster()));
   connect(this,SIGNAL(previousCluster()),traceWidget,SLOT(showPreviousCluster()));   
   
  //Connect the TraceView to possible clusterViews
   if(isThereClusterView){
    ViewWidget* viewWidget;
    for(viewWidget = viewList.first(); viewWidget != 0L ; viewWidget = viewList.next())
     if(viewWidget->isA("ClusterView")){
      connect(viewWidget,SIGNAL(moveToTime(long)),view, SLOT(moveToTime(long))); 
     }
   }   
  }  
}

void KlustersView::updateTraceView(QString name,ItemColors* clusterColors,bool active){     
 //Set the list of the current clusters as the list of clusters to look up in the ClusterProvider.
 if(doc.getClustersProvider() != 0L ) doc.getClustersProvider()->setClusterIdList(shownClusters);
      
 emit updateClusters(name,*shownClusters,clusterColors,active);
}

void KlustersView::updateClustersProvider(){     
 //Set the list of the current view as the list of clusters to look up in the ClusterProvider.
 if(doc.getClustersProvider() != 0L ) doc.getClustersProvider()->setClusterIdList(shownClusters);
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
