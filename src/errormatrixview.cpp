/***************************************************************************
                          errormatrixview.cpp  -  description
                             -------------------
    begin                : Mon Jan 5 2004
    copyright            : (C) 2004 by Lynn Hazan
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
//include files for the application
#include "errormatrixview.h"
#include "errormatrixthread.h"
#include "groupingassistant.h"
#include "array.h"
#include "klustersview.h"
#include "klustersdoc.h"
#include "data.h"

#include "timer.h"

// include files for Qt
#include <qpaintdevicemetrics.h>

//General C++ include files
#include <iostream>
using namespace std;

ErrorMatrixView::ErrorMatrixView(KlustersDoc& doc,KlustersView& view,QColor backgroundColor,KStatusBar* statusBar,QWidget *parent,const char* name,int minSize, int 
maxSize, int windowTopLeft ,int windowBottomRight,int border) :
                          ViewWidget(doc,view,backgroundColor,statusBar,parent,name,minSize,maxSize,windowTopLeft,windowBottomRight,border),
                          dataReady(false),nbColors(100),cutoffProbability(0.1),init(true),hasBeenRenumbered(false),
                          nbActions(0),nbRedo(0),isNotUpToDate(false),nbPreviousUndo(0),nbPreviousRedo(0),goingToDie(false) {
                            
 //The list owns its objects, it will delete the items that are removed.
 threadsToBeKill.setAutoDelete(true);  //The treads will be delete only from threadsToBeKill

 //Set the drawing variables
 abscissaMin = 0;
 ordinateMax = 0;
 cellWidth = 50;
 
 initializeColorMap();
                          
 //Compute the error matrix.
 updateMatrixContents();
}

ErrorMatrixView::~ErrorMatrixView(){
 //Ask the threads to stop as soon as possible.
 willBeKilled();

 //Wait until all the threads have finish before quiting otherwise
 // it may endup in a crash of the application.
 for(ErrorMatrixThread* errorMatrixThread = threadsToBeKill.first(); errorMatrixThread; errorMatrixThread = threadsToBeKill.next())
  while(!errorMatrixThread->wait() && !dataReady){};
    
 delete probabilities;
}

bool ErrorMatrixView::isThreadsRunning(){
 if(threadsToBeKill.count() == 0) return false;
 else return true;
}

void ErrorMatrixView::customEvent(QCustomEvent* event){
 //Event sent by a ErrorMatrixThread to inform that the data are available.
 if(event->type() == QEvent::User + 600){
  
  ErrorMatrixThread::ErrorMatrixEvent* errorMatrixEvent = (ErrorMatrixThread::ErrorMatrixEvent*) event;
  //Get the event information
  ErrorMatrixThread* errorMatrixThread = errorMatrixEvent->parentThread();
  probabilities = errorMatrixThread->getProbabilities();
  clusterList = errorMatrixThread->getClusterList();
  computedClusterList = errorMatrixThread->getComputedClusterList();
  ignoreClusterIndex = errorMatrixThread->getIgnoreClusterIndex();
  
  //Wait to be sure the thread has return from his run method. Even if the send of the event is the last
  //action of the run method it seems that the event loop can be pretty fast and the run has not
  //return when the event is received here.
  while(!errorMatrixThread->wait()){};

  //Delete the errorMatrixThread, this is done by removing it from threadsToBeKill as auto-deletion is enabled.
  threadsToBeKill.remove(errorMatrixThread);

  if(!goingToDie){
   //Each time the matrix is modified, the size of the window is recalculated.
   updateWindow();
   dataReady = true;

   setCursor(Qt::ArrowCursor);

   //Update the widget
   update();    
  }
 }
}

void ErrorMatrixView::updateMatrixContents(){
 if(!goingToDie){  
  setCursor(Qt::WaitCursor);
  ErrorMatrixThread* thread = computeMatrix();
  threadsToBeKill.append(thread);

  //Reset the information used to show that the matrix is not up to date.
  modifiedClusterList.clear();
  selectedPairs.clear();
  hasBeenRenumbered = false;
  renumbering.clear();
  deletedMap.clear();
  nbActions = 0;
  nbRedo = 0;
  isNotUpToDate = false;
  nbPreviousUndo = 0;
  nbPreviousRedo = 0;


  drawContentsMode = REDRAW;
 }
}

ErrorMatrixThread* ErrorMatrixView::computeMatrix(){  
 //The creation of a thread automatically start it.
 return new ErrorMatrixThread(*this,doc.data());  
}

void ErrorMatrixView::updateWindow(){
 int nbOfClusters = clusterList.size();

 widthBorder = (cellWidth * nbOfClusters) / 30;
 heightBorder = (cellWidth * nbOfClusters) / 30;

 abscissaMax = 2 * widthBorder + (cellWidth * nbOfClusters);
 ordinateMin = -(2 * heightBorder + (cellWidth * nbOfClusters));

 //Set the window
 window = ZoomWindow(QRect(QPoint(abscissaMin,ordinateMin),QPoint(abscissaMax,ordinateMax)));
 
 //update the drawing mode if needed (if UPDATE, no change is need it).
 if(drawContentsMode == REFRESH)drawContentsMode = REDRAW ;
}


void ErrorMatrixView::drawContents(QPainter* p){
 if(drawContentsMode == REDRAW){
  QRect contentsRec = contentsRect(); 
  viewport = QRect(contentsRec.left() + 15,contentsRec.top(),contentsRec.width() - 15,contentsRec.height() - 15);

  //Resize the double buffer with the width and the height of the widget(QFrame)
  doublebuffer.resize(contentsRec.width(),contentsRec.height());

  //Create a painter to paint on the double buffer
  QPainter painter;
  painter.begin(&doublebuffer);

  //Set the window (part of the world I want to show)
  QRect r((QRect)window);
  painter.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function

  //Set the viewport (part of the device I want to write on).
  //By default, the viewport is the same as the device's rectangle (contentsRec), taking a smaller
  //one will ensure that the legends (cluster ids) will not ovelap a correlogram.
  painter.setViewport(viewport);

  //Fill the double buffer with the background
  doublebuffer.fill(backgroundColor());

  //Paint the matrix
  if(dataReady)drawMatrix(painter);

  if(dataReady && init){
  //Allowed to detect where the mouse is in order to write the corresponding probability
  //in the status bar.
  setMouseTracking(true);
  init = false;
  }
 
  //reset transformation due to setWindow and setViewport
  painter.resetXForm() ;

  //Draw the cluster Ids along the matrix.
  drawClusterIds(painter);
 
  //Closes the painter on the double buffer
  painter.end();

  //Back to the default
  drawContentsMode = REFRESH;
 }
 //if drawContentsMode == REFRESH, we reuse the double buffer (pixmap)

 //Draw the double buffer (pixmap) by copying it into the paint device.
 p->drawPixmap(0, 0, doublebuffer);
}

void ErrorMatrixView::drawClusterIds(QPainter& painter){
 QFont f("Helvetica",8);
 painter.setFont(f);
 painter.setPen(colorLegend); //set the color for the legends.
 
 //Draw the absciss ids

 //The abscissa of the legend for the current matrix cell.
 uint X = abscissaMin + widthBorder + 4;
 //The ordinate of the legend for the current matrix cell.
 uint Y = 0;
 
 QValueList<int>::iterator iterator;
 for(iterator = clusterList.begin(); iterator != clusterList.end(); ++iterator){
  //the abscissa is increase by the font size to adjust for conversion from world coordinates to viewport coordinates.
  QRect r(worldToViewport(X,-Y).x() + 15,worldToViewport(X,-Y).y() + 2,worldToViewportWidth(cellWidth),12);
  painter.drawText(r,Qt::AlignHCenter,QString("%1").arg(*iterator));
  X += cellWidth;
 }

 //Draw the ordinate ids
 X = 0;
 Y = heightBorder + clusterList.size() * cellWidth - 2;
  
 for(iterator = clusterList.begin(); iterator != clusterList.end(); ++iterator){
  QRect r(worldToViewport(X,-Y).x(),worldToViewport(X,-Y).y(),15,worldToViewportHeight(cellWidth));
  painter.drawText(r,Qt::AlignCenter,QString("%1").arg(*iterator));
  Y -= cellWidth;
 }
}

void ErrorMatrixView::drawMatrix(QPainter& painter){
  if(modifiedClusterList.size() > 0 || hasBeenRenumbered || isNotUpToDate){
   //Draw a red rectangle around the matrix to warn the user that
   //the matrix is not up to date anymore.
   QPen pen(red,4,SolidLine);
   painter.setPen(pen);
   painter.drawRect(abscissaMin + widthBorder - 1,
                  ordinateMin + heightBorder - 2,
                  (clusterList.size() * cellWidth) + 4,
                  (clusterList.size() * cellWidth)  + 5);

   painter.setPen(black);
  }

 int x = abscissaMin + widthBorder;
 int y = ordinateMin + heightBorder;
 
 int nbClusters = clusterList.size(); 
 for(int clusterIndex = 1; clusterIndex <= nbClusters; ++clusterIndex){
  for(int clusterIndex2 = 1; clusterIndex2 <= nbClusters; ++clusterIndex2){    
    if((clusterIndex == clusterIndex2) || ignoreClusterIndex.contains(clusterIndex) || ignoreClusterIndex.contains(clusterIndex2)){
     painter.setBrush(black);
     painter.drawRect(x,y,cellWidth + 1,cellWidth + 1);
    }
    else{
     double probability = (*probabilities)(clusterIndex,clusterIndex2);
     int probColorIndex = static_cast<int>(probability * nbColors / cutoffProbability);
     if(probColorIndex >= nbColors) probColorIndex = nbColors - 1;
     painter.setBrush(colorMap[probColorIndex]);
     painter.drawRect(x,y,cellWidth + 1,cellWidth + 1);
    }
    x += cellWidth;
  }
  x = abscissaMin + widthBorder;
  y += cellWidth;          
 }
}


void ErrorMatrixView::initializeColorMap(){
  for(int i = 0;i<nbColors;i++){
    QColor color;
    int x = static_cast<int>(359 * 0.7 * (1 - static_cast<float>(i) / nbColors));
    color.setHsv(x,255,255);
    colorMap.insert(i,color);
  }
}

void ErrorMatrixView::mouseMoveEvent(QMouseEvent* e){
  //Write the current probability in the statusbar.
  QPoint current = viewportToWorld(e->x() - 15,e->y());

  int x = abscissaMin + widthBorder;
  int y = ordinateMin + heightBorder;

  int cluster1Index = static_cast<int>((current.x() - x) / cellWidth);
  int cluster2Index = static_cast<int>((current.y() - y) / cellWidth);
  
  int indexMax = clusterList.size() - 1;
  if((cluster1Index > -1) && (cluster1Index <= indexMax) &&
     (cluster2Index > -1) && (cluster2Index <= indexMax)){
    statusBar->changeItem("Clusters (" + QString("%1").arg(clusterList[cluster2Index]) + "," +
             QString("%1").arg(clusterList[cluster1Index]) + "): p = " +
             QString("%1").arg((*probabilities)(cluster2Index + 1,cluster1Index + 1)),1);
  }
}

void ErrorMatrixView::mouseReleaseEvent(QMouseEvent* e){
 //Select the clusters corresponding to the current cell of the matrix (if they still exist)
 QPoint current = viewportToWorld(e->x() -15,e->y());

 int x = abscissaMin + widthBorder;
 int y = ordinateMin + heightBorder;

 int cluster1Index = static_cast<int>((current.x() - x) / cellWidth);
 int cluster2Index = static_cast<int>((current.y() - y) / cellWidth);
 int cluster1 = clusterList[cluster1Index];
 int cluster2 = clusterList[cluster2Index];
 Pair pair(cluster1,cluster2);
 QValueList<int> clustersToShow;
 QValueList<int> previousSelectedClusters;
 QValueList<dataType> existingClusters = doc.data().clusterIds();
 
 //Check if the clusters still exist.
 if(existingClusters.contains(static_cast<dataType>(cluster1))){
  clustersToShow.append(cluster1);
  previousSelectedClusters.append(cluster1);
 }
 else pair.setX(-1);  
 if(existingClusters.contains(static_cast<dataType>(cluster2))){
  clustersToShow.append(cluster2);
  previousSelectedClusters.append(cluster2);
 }
 else pair.setY(-1);  

 //If the user control click a second time on a cell of the matrix this will deselect the corresponding pair.
 if((e->state() & Qt::ControlButton) && selectedPairs.contains(pair)){  
  selectedPairs.remove(pair);
  clustersToShow.clear();
  QValueList<Pair>::iterator iterator;
  for(iterator = selectedPairs.begin(); iterator != selectedPairs.end(); ++iterator){
   int firstCluster = static_cast<Pair>(*iterator).getX();
   int secondCluster = static_cast<Pair>(*iterator).getY();
   
   if(firstCluster != -1 && existingClusters.contains(static_cast<dataType>(firstCluster))){
    clustersToShow.append(firstCluster);
    previousSelectedClusters.append(firstCluster);
   }
   if(secondCluster != -1 && existingClusters.contains(static_cast<dataType>(secondCluster))){
    clustersToShow.append(secondCluster);
    previousSelectedClusters.append(secondCluster);
   }
  }
  doc.shownClustersUpdate(clustersToShow,previousSelectedClusters); 
 } 
 else{   
  if(e->state() & Qt::ControlButton){
   //Store the selected pair
   selectedPairs.append(pair);
   doc.addClustersToActiveView(clustersToShow);
  }
  else{
   selectedPairs.clear();
   //Store the selected pair
   selectedPairs.append(pair); 
   doc.shownClustersUpdate(clustersToShow);   
  } 
 }
}


void ErrorMatrixView::clustersGrouped(QValueList<int>& groupedClusters, int newClusterId){
  QValueList<int> deletedList;
  
  //Store all the grouped clusters, used in the error matrix,
  //in the modifiedClusterList and ask to redraw the error matrix
  //in order to signal to the user that the error matrix in no more up to date.
  QValueList<int>::iterator iterator;
  for(iterator = groupedClusters.begin(); iterator != groupedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
     deletedList.append(*iterator);
   }

  deletedMap.insert(newClusterId,deletedList);

  nbActions++;
  
  drawContentsMode = REDRAW;
}

void ErrorMatrixView::clustersDeleted(QValueList<int>& deletedClusters,int destinationCluster){ 
  QValueList<int> deletedList;
  
  //Store all the deletedClusters clusters, used in the error matrix,
  //in the modifiedClusterList and ask to redraw the error matrix
  //in order to signal to the user that the error matrix in no more up to date.
  QValueList<int>::iterator iterator;
  for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator);
    deletedList.append(*iterator);
   }

  deletedMap.insert(destinationCluster,deletedList);

  nbActions++;    
  
  drawContentsMode = REDRAW;
}

void ErrorMatrixView::removeSpikesFromClusters(QValueList<int>& fromClusters, int destinationClusterId,QValueList<int>& emptiedClusters){
  QValueList<int>::iterator iterator;
  for(iterator = fromClusters.begin(); iterator != fromClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
   } 

  QValueList<int> deletedList;
  for(iterator = emptiedClusters.begin(); iterator != emptiedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
     deletedList.append(*iterator);
   }

  deletedMap.insert(destinationClusterId,deletedList);

  nbActions++;
  drawContentsMode = REDRAW;
}

void ErrorMatrixView::newClusterAdded(QValueList<int>& fromClusters,int clusterId,QValueList<int>& emptiedClusters){
  QValueList<int>::iterator iterator;
  for(iterator = fromClusters.begin(); iterator != fromClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator); 
   } 

  QValueList<int> deletedList;
  for(iterator = emptiedClusters.begin(); iterator != emptiedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
     deletedList.append(*iterator);
   }

  deletedMap.insert(clusterId,deletedList);

  nbActions++;
  drawContentsMode = REDRAW;
}

void ErrorMatrixView::newClustersAdded(QMap<int,int>& fromToNewClusterIds,QValueList<int>& emptiedClusters){
  QValueList<int> fromClusters = fromToNewClusterIds.keys();

  //Store all the clusters from where spikes have been taken, used in the error matrix,
  //in the modifiedClusterList and ask to redraw the error matrix
  //in order to signal to the user that the error matrix in no more up to date.
  QValueList<int>::iterator iterator;
  for(iterator = fromClusters.begin(); iterator != fromClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);  
   }

  nbActions++; 
  drawContentsMode = REDRAW;
}


void ErrorMatrixView::newClustersAdded(QValueList<int>& clustersToRecluster){
 //Store all the automatically reclustered clusters, used in the error matrix,
 //in the modifiedClusterList and ask to redraw the error matrix
 //in order to signal to the user that the error matrix in no more up to date.
 QValueList<int>::iterator iterator;
 for(iterator = clustersToRecluster.begin(); iterator != clustersToRecluster.end(); ++iterator)
  if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator);
  }

 nbActions++;
 drawContentsMode = REDRAW; 
}


void ErrorMatrixView::renumber(QMap<int,int>& clusterIdsOldNew){
  hasBeenRenumbered = true;
  nbActions++;
  renumbering.insert(nbActions,true);
  drawContentsMode = REDRAW;
}

void ErrorMatrixView::undoRenumbering(QMap<int,int>& clusterIdsNewOld){
  if(nbActions > 0){
   renumbering.remove(nbActions);
   if(renumbering.isEmpty()) hasBeenRenumbered = false;
  
   nbActions--;
   nbRedo++;
   if(nbActions == 0) isNotUpToDate = false;
   else isNotUpToDate = true;
   drawContentsMode = REDRAW;
  }
  else{
   if(nbPreviousUndo == 0){
    nbPreviousRedo++;
    isNotUpToDate = true;
   }
   else if(nbPreviousUndo == 1){//There was a redo before
    nbPreviousUndo--;
    isNotUpToDate = false;
   }
   else{//nbPreviousUndo >1
    nbPreviousUndo--;
    nbPreviousRedo++;
    isNotUpToDate = true;
   }
   drawContentsMode = REDRAW;
  }
}

void ErrorMatrixView::undoAdditionModification(QValueList<int>& addedClusters,QValueList<int>& updatedClusters){
 if(nbActions > 0){
  nbActions--;
  nbRedo++;
  isNotUpToDate = false;
   
  QValueList<int>::iterator iterator;
  for(iterator = updatedClusters.begin(); iterator != updatedClusters.end(); ++iterator){

   if(modifiedClusterList.contains(*iterator)){
     modifiedClusterList.remove(*iterator);
   }

   if(deletedMap.contains(*iterator)){
     QValueList<int> deletedList = deletedMap[*iterator];
     QValueList<int>::iterator iterator2;
     for(iterator2 = deletedList.begin(); iterator2 != deletedList.end(); ++iterator2)
      modifiedClusterList.remove(*iterator2);
   }
  }
   
  drawContentsMode = REDRAW;
 }
 else{
  if(nbPreviousUndo == 0){
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  else if(nbPreviousUndo == 1){//There was a redo before
   nbPreviousUndo--;
   isNotUpToDate = false;
  }
  else{//nbPreviousUndo >1
   nbPreviousUndo--;
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::undoAddition(QValueList<int>& addedClusters){
 if(nbActions > 0){
  nbActions--;
  nbRedo++;
  isNotUpToDate = false;
  
  QValueList<int>::iterator iterator;
  for(iterator = addedClusters.begin(); iterator != addedClusters.end(); ++iterator){
   if(deletedMap.contains(*iterator)){
     QValueList<int> deletedList = deletedMap[*iterator];
     QValueList<int>::iterator iterator2;
     for(iterator2 = deletedList.begin(); iterator2 != deletedList.end(); ++iterator2)
      modifiedClusterList.remove(*iterator2); 

    drawContentsMode = REDRAW;
   }
  }
 }
 else{
  if(nbPreviousUndo == 0){
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  else if(nbPreviousUndo == 1){//There was a redo before
   nbPreviousUndo--;
   isNotUpToDate = false;
  }
  else{//nbPreviousUndo >1
   nbPreviousUndo--;
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::undoModification(QValueList<int>& updatedClusters){
 if(nbActions > 0){
  nbActions--;
  nbRedo++;
  isNotUpToDate = false;
  
  QValueList<int>::iterator iterator;
  for(iterator = updatedClusters.begin(); iterator != updatedClusters.end(); ++iterator){
   if(modifiedClusterList.contains(*iterator)){
     modifiedClusterList.remove(*iterator); 
   }
   if(deletedMap.contains(*iterator)){
    QValueList<int> deletedList = deletedMap[*iterator];
    QValueList<int>::iterator iterator2;
    for(iterator2 = deletedList.begin(); iterator2 != deletedList.end(); ++iterator2){
     modifiedClusterList.remove(*iterator2);
     }
   }
  }
  drawContentsMode = REDRAW;
 }
 else{ 
  if(nbPreviousUndo == 0){
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  else if(nbPreviousUndo == 1){//There was a redo before
   nbPreviousUndo--;
   isNotUpToDate = false;
  }
  else{//nbPreviousUndo >1
   nbPreviousUndo--;
   nbPreviousRedo++;
   isNotUpToDate = true;
  }
  drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::redoRenumbering(QMap<int,int>& clusterIdsOldNew){
 if(nbPreviousRedo == 1){
  nbPreviousRedo--;
  isNotUpToDate = false;
  drawContentsMode = REDRAW;
 }
 else if (nbPreviousRedo > 1){
  nbPreviousRedo--;
  nbPreviousUndo++;
  isNotUpToDate = true;
  drawContentsMode = REDRAW;
 }
 else if(nbActions > 0 || (nbActions == 0 && nbRedo > 0)){
  nbActions++;
  nbRedo--;
  isNotUpToDate = false;

  renumbering.insert(nbActions,true);
  hasBeenRenumbered = true;
  drawContentsMode = REDRAW;
 }
 else if(nbPreviousRedo == 0){//there was no previous undo
   isNotUpToDate = true;
   nbPreviousUndo++;
   drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::redoAdditionModification(QValueList<int>& addedClusters,QValueList<int>& modifiedClusters,bool isModifiedByDeletion,QValueList<int>& deletedClusters){
 if(nbPreviousRedo == 1){
  nbPreviousRedo--;
  isNotUpToDate = false;
  drawContentsMode = REDRAW;
 }
 else if (nbPreviousRedo > 1){
  nbPreviousRedo--;
  nbPreviousUndo++;
  isNotUpToDate = true;
  drawContentsMode = REDRAW;
 }
 else if(nbActions > 0 || (nbActions == 0 && nbRedo > 0)){
  nbActions++;
  nbRedo--;
  isNotUpToDate = false;

  QValueList<int>::iterator iterator;
  for(iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator);
   }

  for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
   }

  drawContentsMode = REDRAW;
 }
 else if(nbPreviousRedo == 0){//there was no previous undo
   isNotUpToDate = true;
   nbPreviousUndo++;
   drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::redoAddition(QValueList<int>& addedClusters,QValueList<int>& deletedClusters){
 if(nbPreviousRedo == 1){
  nbPreviousRedo--;
  isNotUpToDate = false;
  drawContentsMode = REDRAW;
 }
 else if (nbPreviousRedo > 1){
  nbPreviousRedo--;
  nbPreviousUndo++;
  isNotUpToDate = true;
  drawContentsMode = REDRAW;
 }
 else if(nbActions > 0 || (nbActions == 0 && nbRedo > 0)){
  nbActions++;
  nbRedo--;
  isNotUpToDate = false;

  QValueList<int>::iterator iterator;
  for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator);
   }

  drawContentsMode = REDRAW;
 }
 else if(nbPreviousRedo == 0){//there was no previous undo
   isNotUpToDate = true;
   nbPreviousUndo++;
   drawContentsMode = REDRAW;
 }
}


void ErrorMatrixView::redoModification(QValueList<int>& updatedClusters,bool isModifiedByDeletion,QValueList<int>& deletedClusters){
 if(nbPreviousRedo == 1){
  nbPreviousRedo--;
  isNotUpToDate = false;
  drawContentsMode = REDRAW;
 }
 else if (nbPreviousRedo > 1){
  nbPreviousRedo--;
  nbPreviousUndo++;
  isNotUpToDate = true;
  drawContentsMode = REDRAW;
 }
 else if(nbActions > 0 || (nbActions == 0 && nbRedo > 0)){
  nbActions++;
  nbRedo--;
  isNotUpToDate = false;

  QValueList<int>::iterator iterator;
  for(iterator = updatedClusters.begin(); iterator != updatedClusters.end(); ++iterator)
   if(modifiedClusterList.contains(*iterator)) modifiedClusterList.remove(*iterator);
   else if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
   }

  for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
    modifiedClusterList.append(*iterator);
   }

  drawContentsMode = REDRAW;
 }
 else if(nbPreviousRedo == 0){//there was no previous undo
   isNotUpToDate = true;
   nbPreviousUndo++;
   drawContentsMode = REDRAW;
 } 
}


void ErrorMatrixView::redoDeletion(QValueList<int>& deletedClusters){
 if(nbPreviousRedo == 1){
  nbPreviousRedo--;
  isNotUpToDate = false;
  drawContentsMode = REDRAW;
 }
 else if (nbPreviousRedo > 1){
  nbPreviousRedo--;
  nbPreviousUndo++;
  isNotUpToDate = true;
  drawContentsMode = REDRAW;
 }
 else if(nbActions > 0 || (nbActions == 0 && nbRedo > 0)){
  nbActions++;
  nbRedo--;
  isNotUpToDate = false;

  QValueList<int>::iterator iterator;
  for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   if(clusterList.contains(*iterator)){
     modifiedClusterList.append(*iterator);
   }

  drawContentsMode = REDRAW;
 }
 else if(nbPreviousRedo == 0){//there was no previous undo
   isNotUpToDate = true;
   nbPreviousUndo++;
   drawContentsMode = REDRAW;
 }
}

void ErrorMatrixView::willBeKilled(){
 if(!goingToDie){
  goingToDie = true;
  //inform the running threads to stop processing as soon as possible.
 for(ErrorMatrixThread* errorMatrixThread = threadsToBeKill.first(); errorMatrixThread; errorMatrixThread = threadsToBeKill.next())
   errorMatrixThread->stopProcessing();
 }
}

void ErrorMatrixView::print(QPainter& printPainter,QPaintDeviceMetrics& metrics,bool whiteBackground){
 //Draw the double buffer (pixmap) by copying it into the printer device throught the painter.
  QRect viewportOld = QRect(viewport.left(),viewport.top(),viewport.width(),viewport.height());

 //If the left margin is not visible (the user zoomed without taking it in his selection), the viewport and the printer
  //have the same size.
  QRect r((QRect)window);
  viewport = QRect(printPainter.viewport().left() + 15,printPainter.viewport().top(),printPainter.viewport().width() - 15,printPainter.viewport().height()-15);

  //Set the window (part of the world I want to show)
  printPainter.setWindow(r.left(),r.top(),r.width()-1,r.height()-1);//hack because Qt QRect is used differently in this function

  //Set the viewport (part of the device I want to write on).
  //By default, the viewport is the same as the device's rectangle (contentsRec), taking a smaller
  //one will ensure that the legends (cluster ids) will not ovelap a correlogram.
  printPainter.setViewport(viewport);

  //Fill the background with the background color and ensure we draw the same portion of the world than on the screen
  QRect back = QRect(r.left(),r.top(),r.width(),r.height());
  float heightRatio = (static_cast<float>(back.height())/static_cast<float>(metrics.height()));
  back.setBottom(r.top() + r.height() - 1 + static_cast<long>(15 * heightRatio));
  float widthRatio = (static_cast<float>(back.width())/static_cast<float>(metrics.width()));
  if(r.left() == 0) back.setLeft(r.left() - static_cast<long>(15 * widthRatio));

  QColor colorLegendTmp = colorLegend;
  QColor background= backgroundColor();
  if(whiteBackground){
   colorLegend = black;
   setPaletteBackgroundColor(white);
  }
  
  printPainter.fillRect(back,backgroundColor());
  printPainter.setClipRect(back,QPainter::CoordPainter);

  //Paint the matrix
  drawMatrix(printPainter);

  //reset transformation due to setWindow and setViewport
  printPainter.resetXForm();

  //Draw the cluster Ids along the matrix.
  drawClusterIds(printPainter);

  printPainter.setClipping(false);

  //Restore the colors.
  if(whiteBackground){
   colorLegend = colorLegendTmp;
   setPaletteBackgroundColor(background);
  }
  
  //Restore the previous state
  viewport = QRect(viewportOld.left(),viewportOld.top(),viewportOld.width(),viewportOld.height());
}

