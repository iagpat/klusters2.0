/***************************************************************************
                          klustersview.h  -  description
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

#ifndef KCLUSTERSVIEW_H
#define KCLUSTERSVIEW_H


#include <QDockWidget>
#include <QStatusBar>

// include files for Qt
#include <QWidget>
#include <qvector.h>
#include <qprinter.h>

#include <QList>
#include <QEvent>
#include <QCloseEvent>

//include files for the application
#include "itemcolors.h"
#include "data.h"
#include "viewwidget.h"
#include "correlationview.h"
#include <dockarea.h>


// forward declarations
class CorrelationView;
class KlustersDoc;
class KlustersApp;
class TraceWidget;

/** The KlustersView class provides the view widgets for the document instance (KlustersDoc instance) and is displayed
 * as a MDI child window in the main view area of the KlustersApp class instance.
 * The document instance keeps a list of all views that represent the document contents as there
 * can be more than one view. Views get created in KlustersApp::createDisplay() and automatically added to the list of views.
 * The KlustersView class inherits QWidget as a KDockArea to give it the ability to contains dockWidgets.
 * @author Lynn Hazan
 */
class KlustersView : public DockArea
{
    Q_OBJECT

    friend class KlustersDoc;

public:

    /**
    * Enum indicating wich type of view the user wants.
    */
    enum DisplayType {CLUSTERS=0,WAVEFORMS=1,CORRELATIONS=2,OVERVIEW=3,GROUPING_ASSISTANT_VIEW=4,ERROR_MATRIX=5,TRACES=6};

    /** Constructs a view.
      * @param mainWindow a reference on the main window of the application.
      * @param doc the document instance the view represents.
      * @param backgroundColor color used as background.
      * @param initialDimensionX initial dimension to be presented on the abscissa axis.
      * @param initialDdimensionY initial dimension to be presented on the ordinate axis.
      * @param initialClusterList list of cluster to be shown at the opening of the view.
      * @param type a DisplayType giving the way the document has to be presented.
      * @param parent the parent QWidget.
      * @param name name of the widget (can be used for introspection).
      * @param wflags common QWidget flags.
      * @param statusBar a reference to the application status bar.
      * @param timeInterval time interval between 2 time lines drawn in the ClusterView.
      * @param maxAmplitude initial gain use to draw the waveforms in the WaveformView.
      * @param positions positions of the channels to use in the view set by the user in the settings dialog.
      * @param isTimeFrameMode whether the Waveform View (if any) is in timeFrame mode.
      * @param start initial time of the time frame use by the WaveformView (if any).
      * @param timeFrameWidth initial amount of time for the time frame use by the WaveformView (if any).
      * @param nbSpkToDisplay initial number of spikes to display for each cluster when the waveform presentation
      *  mode is sample.
      * @param overLay boolean indicating if the waveforms are overlaping each other.
      * @param mean boolean indicating if the means and standard deviation are displayed.
      * @param binSize initial bin size used by the CorrelationView.
      * @param correlationTimeFrame initial time amount, in second, of the time frame used by the CorrelationView.
      * @param scale initial type of scale used to present the correlation data.
      * @param shoulderLine boolean indicating if a doted line is drawn at the shoulder level
      * of the correlograms.
      * @param startingTime starting time in miliseconds used in the TraceView.
      * @param duration time window in miliseconds used in the TraceView.
      * @param labelsDisplay true if labels are drawn next to the traces, false otherwise.
      * @param undoList list of list of removed clusters used to enable undo action.
      * @param redoList list of list of removed clusters used to enable redo action.
     */
    KlustersView(KlustersApp& mainWindow, KlustersDoc& doc, const QColor& backgroundColor, int initialDimensionX, int initialDdimensionY,
                 QList<int>* initialClusterList, DisplayType type, QWidget* parent, const char *name, QStatusBar * statusBar, int timeInterval, int maxAmplitude,
                 QList<int> positions, bool isTimeFrameMode = false, long start = 0, long timeFrameWidth = 0, long nbSpkToDisplay = 0, bool overLay = false,
                 bool mean = false, int binSize = 0, int correlationTimeFrame = 0, Data::ScaleMode scale = Data::MAX, bool shoulderLine = true,
                 long startingTime = 0, long duration = 100, bool labelsDisplay = false, QList< QList<int>* > undoList = QList< QList<int>* >(), QList< QList<int>* > redoList = QList< QList<int>* >());

    /** Destructor for the main view */
    ~KlustersView();

    /** Returns a pointer to the document presented in the view.
    */
    KlustersDoc& getDocument() const{return doc;}
    
    /**Updates the view.*/
    void update(KlustersView* pSender);

    /** Prints the views in the display, for multiple-view displays, each view is printed on a separate page.
    * The file name and view-specific parameters are printed below the figure.
    * @param printer printer to print into.
    * @param filePath path of the opened document.
    * @param whiteBackground true if the printed background has to be white, false otherwise.
    */
    void print(QPrinter* printer, const QString &filePath, bool whiteBackground);
    
    /**Adds a new view (ClusterView,WaveformView or CorrelationView).
    * @param docWidget QDockWidget to which the new view will be docked.
    * @param displayType type of view to add.
    * @param backgroundColor color used as background.
    * @param statusBar a reference to the application status bar.
    * @param timeInterval time interval between 2 time lines drawn in the cluster view.
    * @param maxAmplitude initial gain use to draw the waveforms in the Waveform View.
    * @param positions positions of the channels to use in the view set by the user in the settings dialog.
    * @return false if there was already a view of @displayType in the view, true otherwise.
    */
    bool addView(DisplayType displayType,const QColor& backgroundColor,QStatusBar* statusBar,int timeInterval,int maxAmplitude,QList<int> positions);
    
    /**List of the names for the different type of view available.*/
    static const QString DisplayTypeNames[];

    /*Updates the dimension used to present the clusters in the Cluster View.
    * @param dimensionX abscissa dimension.
    * @param dimensionY ordinate dimension.
    **/
    void updateDimensions(int dimensionX,int dimensionY);

    /**Returns the dimension used for the abscissa axis in the Cluster View.
    * @return abscissa dimension.
    */
    int abscissaDimension() const {return dimensionX;}

    /**Returns the dimension used for the ordinate axis in the Cluster View.
    * @return ordinate dimension.
    */
    int ordinateDimension() const {return dimensionY;}

    /**Returns the list of the clusters presented in the view.
    * @return list of cluster ids.
    */
    const QList<int>& clusters() const {return *shownClusters;}

    /**Checks if some clusters have had their color changed.
    * If so, update clusterUpdateList if needed.
    * @param active true if the view is the active one, false otherwise.
    */
    void updateColors(bool active);

    /**Show all the encapsulated widgets contain in the view. The widgets know how to draw themselves.*/
    void showAllWidgets(){
        emit updateContents();
    }

    /**Update all the encapsulated widgets contain in the view. The widgets know how to draw themselves.*/
    void updateViewContents(){
        emit updateDrawing();
    }

    /**Updates the view only for one cluster for which the color has been changed.
    * @param clusterId the cluster id for which the color has been changed.
    * @param active true if the view is the active one, false otherwise.
    */
    void singleColorUpdate(int clusterId,bool active){
        emit singleColorUpdated(clusterId,active);
    }

    /**
    * Updates the list of clusters shown with @p clustersToShow.
    * Makes the different views update their drawing. This method is called only on the active view.
    * @param clustersToShow new list of clusters to be shown.
    */
    void shownClustersUpdate(QList<int>& clustersToShow);

    /**Updates the view due to a rearrangement of clusters.
    * @param groupedClusters list of clusters having been grouped.
    * @param newClusterId the id of the cluster created by the grouping of those specified in @p groupedClusters.
    * @param active true if the view is the active one, false otherwise.
    */
    void groupedClustersUpdate(QList<int>& groupedClusters, int newClusterId,bool active);

    /**Updates the view due deletion of clusters.
    * @param deletedClusters list of clusters having been deleted.
    * @param destinationCluster cluster with which the clusters in @p deletedClusters are merge
    * (cluster 0 or 1 cluster of artefact and cluster of noise respectively).
    * @param active true if the view is the active one, false otherwise.
    * @return true if at least one of the clusters in deletedClusters was in the list of clusters shown.
    */
    bool clustersDeletionUpdate(QList<int>& deletedClusters,int destinationCluster,bool active);

    /**Updates the view due deletion of clusters.
    * @param deletedClusters list of clusters having been deleted.
    * @param active true if the view is the active one, false otherwise.
    * @param active true if the view is the active one, false otherwise.
    * @return true if at least one of the clusters in deletedClusters was in the list of clusters shown.
    */
    bool clustersDeletionUpdate(QList<int>& deletedClusters,bool active);

    /**
    * Adds a newly created cluster to those already shown.
    * This method aims to reduce the number of clusters to draw.
    * @param fromClusters list of clusters from which the spikes of the new cluster are coming.
    * @param clusterId cluster to add to the clusters already drawn.
    * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
    * @param active true if the view is the active one, false otherwise.
    */
    void addNewClusterToView(QList<int>& fromClusters,int clusterId,QList<int>& emptiedClusters,bool active);

    /**
    * Adds newly created clusters to those already shown.
    * This method aims to reduce the number of clusters to draw.
    * @param fromToNewClusterIds map where the keys are ids of the clusters which really contained spikes in the region
    * and the values are the ids of the newly created clusters.
    * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
    * @param active true if the view is the active one, false otherwise.
    */
    void addNewClustersToView(QMap<int,int>& fromToNewClusterIds,QList<int>& emptiedClusters,bool active);

    /**
   * Updates the content of the view due to the removal of spikes in a cluster.
   * This method aims to reduce the number of clusters to draw.
   * @param fromClusters list of clusters from which the spikes have been taken.
   * @param destinationClusterId cluster id to which the spikes have been added
   * @param emptiedClusters list clusters numbers which became empty because all their spikes were put in the new one.
    * @param active true if the view is the active one, false otherwise.
   */
    void removeSpikesFromClustersInView(QList<int>& fromClusters, int destinationClusterId,QList<int>& emptiedClusters,bool active);

    /**Updates the mode, methode call from the application on the active view after the user selected a tool.
   * @param selectedMode the new mode.
   */
    void setMode(BaseFrame::Mode selectedMode){
        emit modeToSet(selectedMode);
    }

    /**Sets the currentViewWidget (not currently used).*/
    void setCurrentViewWidget(ViewWidget* current){currentViewWidget = current;}

    /**Method called when no spikes have been found in a polygon of selection.
   */
    void selectionIsEmpty(){emit emptySelection();}

    /** Reverts the last user action. If any clusters have been removed they will be added back
    * @param active true if the view is the active one, false otherwise.*/
    void undo(bool active);

    /** Reverts the last user action. If any clusters have been removed they will be added back
   * The clusters in @p addedClusters, if present in shownClusters, will be removed.
   * @param addedClusters list of clusters which were added
   * @param active true if the view is the active one, false otherwise.
   */
    void undoAddedClusters(QList<int>& addedClusters,bool active);

    /** Reverts the last user action. If any clusters have been removed they will be added back
   * The clusters in @p modifiedClusters,if present in shownClusters, will be updated.
   * @param updatedClusters list of clusters which were modified
   * @param active true if the view is the active one, false otherwise.
   */
    void undoModifiedClusters(QList<int>& updatedClusters,bool active);


    /** Reverts the last user action. If any clusters have been removed they will be added back
   * The clusters in @p addedClusters, if present in shownClusters,
   * will be removed and the clusters in @p modifiedClusters,if present in
   * shownClusters, will be updated.
   * @param addedClusters list of clusters which were added.
   * @param updatedClusters list of clusters which were modified .
   * @param active true if the view is the active one, false otherwise.
   */
    void undo(QList<int>& addedClusters,QList<int>& updatedClusters,bool active);

    /** Revers the last undo action. If any clusters have been added back in the undo
   * they will be removed again if they are shown.
   * @param active true if the view is the active one, false otherwise.
   * @param deletedClusters list of clusters which were deleted.
   */
    void redo(bool active,QList<int>& deletedClusters);

    /** Reverts the last undo action. If any clusters have been added back in the undo
   * they will be removed again if they are shown.
   * The clusters in @p addedClusters will be added back.
   * @param addedClusters list of clusters which were added
   * @param active true if the view is the active one, false otherwise.
   * @param deletedClusters list of clusters which were deleted.
   */
    void redoAddedClusters(QList<int>& addedClusters,bool active,QList<int>& deletedClusters);

    /** Reverts the last undo action. If any clusters have been added back in the undo
   * they will be removed again if they are shown. The boolean @p isModifiedByDeletion is used
   * to reduce the number of cluster to redraw whenever possible.
   * The clusters in @p modifiedClusters, if present in shownClusters, will be updated.
   * @param updatedClusters list of clusters which were modified.
   * @param isModifiedByDeletion true if the clusters of @p updatedClusters have been modified
   * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
   * @param active true if the view is the active one, false otherwise.
   * @param deletedClusters list of clusters which were deleted.
   */
    void redoModifiedClusters(QList<int>& updatedClusters,bool isModifiedByDeletion,bool active,QList<int>& deletedClusters);


    /** Reverts the last undo action. If any clusters have been added back in the undo
   * they will be removed again if they are shown. The boolean @p isModifiedByDeletion is used
   * to reduce the number of cluster to redraw whenever possible.
   * The clusters in @p addedClusters will be added back and the clusters in @p modifiedClusters,if present in
   * shownClusters, will be updated.
   * @param addedClusters list of clusters which were added.
   * @param modifiedClusters list of clusters which were modified.
   * @param isModifiedByDeletion true if the clusters of @p updatedClusters have been modified
   * by the deletion of spikes (moved to cluster 0 or 1, cluster of artefact and cluster of noise respectively).
   * @param active true if the view is the active one, false otherwise.
   * @param deletedClusters list of clusters which were deleted.
   */
    void redo(QList<int>& addedClusters,QList<int>& modifiedClusters,bool isModifiedByDeletion,bool active,QList<int>& deletedClusters);

    /**
   * Updates the list of clusterIds shown using the new ids.
   * @param clusterIdsOldNew map given for each old clusterId the new clusterId.
   * @param active true if the view is the active one, false otherwise.
   */
    void renumberClusters(QMap<int,int>& clusterIdsOldNew,bool active);

    /**Reverts the last renumbering action.
   * @param clusterIdsNewOld map given for each new clusterId the old clusterId.
   * @param active true if the view is the active one, false otherwise.
   */
    void undoRenumbering(QMap<int,int>& clusterIdsNewOld,bool active) ;

    /**Reverts the last undo action on a renumbering.
   * @param clusterIdsOldNew map given for each old clusterId the new clusterId.
   * @param active true if the view is the active one, false otherwise.
   */
    void redoRenumbering(QMap<int,int>& clusterIdsOldNew,bool active);

    /**Informs the WaveformView to present the waveforms for an updated time frame.
   * @param start the start time of the time frame.
   * @param timeFrameWidth the time amount, in second, to use to show the waveforms.
   */
    void updateTimeFrame(long start,long timeFrameWidth);

    /**Sets the presentation mode of the waveform view to sample mode, meaning that, for each shown cluster,
  * only one out of the number of spikes to be displayed will be shown.
  */
    void setSampleMode(){
        inTimeFrameMode = false;
        emit sampleMode();
    }

    /**Set the presentation mode of the waveform view to time frame mode, meaning that, for each shown cluster,
  * only the spikes within the current time frame will be shown.
  */
    void setTimeFrameMode(){
        inTimeFrameMode = true;
        emit timeFrameMode();
    }

    /**Sets the way of presenting the information concerning the waveforms selected to
  * only show the waveforms of the mean and the standard deviation.
  */
    void setMeanPresentation(){
        meanDisplay = true;
        emit meanPresentation();
    }

    /**Sets the way of presenting the information concerning the waveforms selected to
  * show all the waveforms corresponding to the mode of presentation.
  */
    void setAllWaveformsPresentation(){
        meanDisplay = false;
        emit allWaveformsPresentation();
    }

    /**Sets the waveforms of each cluster to overlap.
  */
    void setOverLayPresentation(){
        overLayDisplay = true;
        emit overLayPresentation();
    }

    /** Sets the waveforms of each cluster to be presented side by side.
  */
    void setSideBySidePresentation(){
        overLayDisplay = false;
        emit sideBySidePresentation();
    }

    /**Triggers the increase of the amplitude of the waveforms in the Waveform view.
  */
    void increaseWaveformsAmplitude(){emit increaseAmplitude();}

    /**Triggers the decrease of the amplitude of the waveforms in the Waveform view.
  */
    void decreaseWaveformsAmplitude(){emit decreaseAmplitude();}

    /**Returns a boolean indicating if the view contains a WaveformView.
  * @return true if the view contains a WaveformView, false otherwise.*/
    bool containsWaveformView() const {return isThereWaveformView;}

    /**Returns true if the view contains a WavefromView in time frame mode,
  * false otherwise.*/
    bool isInTimeFrameMode() const {return inTimeFrameMode;}

    /**Returns the time amount, in second, of the time frame used by the WaveformView.
  */
    long timeFrameWidth() const {
        return timeWindow;
    }

    /**Returns the start time of the time frame used by the WaveformView.
  */
    long timeFrameStart() const {
        return startTime;
    }

    /**Returns a boolean indicating if the view contains a ClusterView.
  * @return true if the view contains a ClusterView, false otherwise.*/
    bool containsClusterView() const {return isThereClusterView;}

    /**Updates the number of spikes to display for each cluster when the waveform presentation
  * mode is sample.
  * @param nbSpikes number of spikes to display.
  */
    void setDisplayNbSpikes(int nbSpikes){
        nbSpkToDisplay = nbSpikes;
        emit updateDisplayNbSpikes(nbSpikes);
    }

    /**Returns the number of spikes displayed in the Waveform View, if any, when the presentation mode is sample.
  * @return number of spikes displayed.
  */
    long displayedNbSpikes() const {return nbSpkToDisplay;}

    /**Returns a boolean indicating if the means and standard deviation are displayed.
  * @return true if the Waveform View, if any, is presenting only the means and standard deviation,
  * false otherwise.
  */
    bool isMeanPresentation() const {return meanDisplay;}

    /**Returns a boolean indicating if the waveforms are overlaping each other.
  * @return true if the Waveform View, if any, is presenting the waveforms overlaping each other,
  * false otherwise.
  */
    bool isOverLayPresentation() const {return overLayDisplay;}

    /**Returns the time amount, in second, of the time frame used by the Correlation View.
  * @return time amount in second.
  */
    long correlationTimeFrameWidth() const {
        return correlogramTimeFrame;
    }

    /**Returns the bin size used by the Correlation View.
  * @return bin size.
  */
    long sizeOfBin() const {
        return binSize;
    }

    /**Returns a boolean indicating if the view contains a CorrelationView.
  * @return true if the view contains a CorrelationView, false otherwise.*/
    bool containsCorrelationView() const {return isThereCorrelationView;}

    /**Returns the type of scale used to present the correlation data.
  * @return type of scale.
  */
    Data::ScaleMode scaleMode() const {return correlationScale;}

    /**Sets the acquisition system gain.
  * @param acquisitionGain acquisition system gain.
  */
    void setGain(int acquisitionGain){emit changeGain(acquisitionGain);}

    /**Triggers the increase of the amplitude of the correlograms in the Correlation View.
  */
    void increaseCorrelogramsAmplitude(){emit increaseAmplitudeofCorrelograms();}

    /**Triggers the decrease of the amplitude of the correlograms in the Correlation View.
  */
    void decreaseCorrelogramsAmplitude(){emit decreaseAmplitudeofCorrelograms();}

    /**Removes any scale applied to the correlation data.*/
    void setNoScale(){
        correlationScale = Data::RAW;
        emit noScale();
    }

    /**Sets the scale of the correlation data to the maximum value.*/
    void setScaleByMax(){
        correlationScale = Data::MAX;
        emit maxScale();
    }

    /**Sets the scale of the correlation data to the shoulder value.*/
    void setScaleByShouler(){
        correlationScale = Data::SHOULDER;
        emit shoulderScale();
    }

    /**Informs the Correlation View to present the correlograms for an updated bin size and time frame.
  * @param size the size of the bin, in second, to use to compute the correlograms.
  * @param timeFrame time frame used to compute the correlograms.
  */
    void updateBinSizeAndTimeFrame(int size,int timeFrame){
        binSize = size;
        correlogramTimeFrame = timeFrame;
        emit updatedBinSizeAndTimeFrame(size,timeFrame);
    }

    /**Enables the caller to know if there is any thread running link to this view,
  * meaning launch by any of the view's viewWidget.
  * If at least one is running the method returns true, if none are running the method return false.
  */
    bool isThreadsRunning() const;

    /**
  * Update the presentation of a doted line at the shoulder level on the correlograms of the Correlation View if any.
  * If @p b is true a line will be drawn, none will be drawn otherwise.
  * @param b boolean indicating if a shoulder line has to be drawn.
  */
    void updateShoulderLine(bool b){
        shoulderLine = b;
        emit setShoulderLine(b);
    }

    /**Returns a boolean indicating if a doted line is drawn at the shoulder level
  * of the correlograms of the Correlation View if any.
  * @return true if a line is drawn, false otherwise.
  */
    bool isShoulderLine() const {return shoulderLine;}

    /**Returns the list of list of removed clusters used to enable undo action.
  */
    QList< QList<int>* > getUndoList();

    /**Returns the list of list of removed clusters used to enable redo action.*/
    QList< QList<int>* > getRedoList();

    /**Updates the time interval between time lines drawn in the cluster view for the time dimension.
  * @param step the interval to use in second.
  * @param active true if the view is the active one, false otherwise.
  */
    void setTimeStepInSecond(int step,bool active){emit changeTimeInterval(step,active);}

    /**Initialize the position of the channels in the waveform view.
  * @param positions positions of the channels to use in the view set by the user in the settings dialog.
  */
    void setChannelPositions(QList<int>& positions){emit changeChannelPositions(positions);}

    /**Makes all the internal changes due to a modification of the number of undo.
  * @param newNbUndo the futur new number of undo.
  */
    void nbUndoChangedCleaning(int newNbUndo);

    /**Updates the probabilitites in the errorMatrix view.*/
    void updateErrorMatrix(){emit computeProbabilities();}

    /**Returns a boolean indicating if the view contains a Grouping Assistant View.
  * @return true if the view contains a Grouping Assistant View, false otherwise.*/
    bool containsErrorMatrixView() const {return isThereErrorMatrixView;}

    /***Update the background color of the views.*/
    void updateBackgroundColor(const QColor& color){emit changeBackgroundColor(color);}

    /**
  * Adds the clusters created by the automatic reclustering program to those already shown and remove
  * those reclustered.
  * This method aims to reduce the number of clusters to draw.
  * @param clustersToRecluster list of clusters reclustered.
  * @param reclusteredClusterList list of the newly created clusters.
  * @param active c
  */
    void addNewClustersToView(QList<int>& clustersToRecluster,QList<int>& reclusteredClusterList,bool active);

    /**
  * Updates the TraceView after a modification on the clusters.
  * @param name identifier of the cluster file.
  * @param clusterColors list of colors for the clusters.
  * @param active true if the view is the active one, false otherwise.
  */
    void updateTraceView(QString name,ItemColors* clusterColors,bool active);


    /**
  * Updates the ClustersProvider to enable it to work with the current display.
  */
    void updateClustersProvider();

    /**Returns a boolean indicating if the view contains a TraceView.
  * @return true if the view contains a TraceView, false otherwise.*/
    bool containsTraceView() const {return isThereTraceView;}

    /**Triggers the increase of the amplitude of all the channels.
 */
    void increaseAllChannelsAmplitude(){emit increaseAllAmplitude();}

    /**Triggers the decrease of the amplitude of all the channels.
 */
    void decreaseAllChannelsAmplitude(){emit decreaseAllAmplitude();}

    /**Gets the starting time in miliseconds used in the TraceView.
  * @return starting time.
  */
    long getStartingTime(){return startingTime;}

    /**Gets the time window in miliseconds used in the TraceView.
  * @return duration time window.
  */
    long getDuration(){return duration;}

    /**Displays or hides the labels next to the traces in the TraceView.
  * @param status true if the labels have to be drawn, false otherwise.
  */
    void showLabelsUpdate(bool status){
        labelsDisplay = status;
        emit showLabels(status);
    }

    /**Returns true if labels are drawn next to the traces in the TraceView, false otherwise.
  */
    bool getLabelStatus() const{return labelsDisplay;}

    /**Retrieves the next cluster.*/
    void showNextCluster(){emit nextCluster();}

    /**Retrieves the previous cluster.*/
    void showPreviousCluster(){emit previousCluster();}

public Q_SLOTS:

    /**Takes care of the closing of a ClusterView.
  * @param clusterView the ClusterView to be closed.
  */
    void clusterDockClosed(QObject *obj);

    /**Takes care of the closing of a WaveformView.
  * @param waveformView the WaveformView to be closed.
  */
    void waveformDockClosed(QObject *waveformView);

    /**Takes care of the closing of a CorrelogramView.
  * @param correlogramView the CorrelogramView to be closed.
  */
    void correlogramDockClosed(QObject *correlogramView);

    /**Takes care of the closing of an ErrorMatrixView.
  * @param errorMatrixView the ErrorMatrixView to be closed.
  */
    void errorMatrixDockClosed(QObject* errorMatrixView);

    /**Takes care of the closing of a TraceView.
  * @param traceWidget the traceView to be closed.*/
    void traceDockClosed(QObject* traceWidget);

    /**Sets the starting time and the time window in miliseconds used in the TraceView.
  * @param start starting time.
  * @param duration time window.
  */
    void setStartAndDuration(long start,long duration){
        startingTime = start;
        this->duration = duration;
    }

Q_SIGNALS:
    void updatedDimensions(int dimensionX,int dimensionY);
    void singleColorUpdated(int clusterId,bool active);
    void clusterRemovedFromView(int ClusterId,bool active);
    void clusterAddedToView(int clusterId,bool active);
    void newClusterAddedToView(QList<int>& fromClusters,int clusterId,bool active);
    void newClusterAddedToView(int clusterId,bool active);
    void spikesRemovedFromClusters(QList<int>& fromClusters,bool active);
    void modeToSet(BaseFrame::Mode selectedMode);
    void spikesAddedToCluster(int destinationClusterId,bool active);
    void updateContents();
    void emptySelection();
    void modifiedClusters(QList<int>& modifiedClusters,bool active, bool isModifiedByDeletion=false);
    void modifiedClustersUndo(QList<int>& modifiedClusters,bool active);
    void updatedTimeFrame(long start,long timeFrameWidth);
    void sampleMode();
    void timeFrameMode();
    void meanPresentation();
    void allWaveformsPresentation();
    void overLayPresentation();
    void sideBySidePresentation();
    void increaseAmplitude();
    void decreaseAmplitude();
    void updateDisplayNbSpikes(long nbSpikes);
    void increaseAmplitudeofCorrelograms();
    void decreaseAmplitudeofCorrelograms();
    void noScale();
    void maxScale();
    void shoulderScale();
    void updatedBinSizeAndTimeFrame(int size,int timeFrame);
    void setShoulderLine(bool b);
    void updateDrawing();
    void changeGain(int acquisitionGain);
    void changeTimeInterval(int step,bool active);
    void changeChannelPositions(QList<int>& positions);
    void computeProbabilities();
    void changeBackgroundColor(QColor color);
    void clustersRenumbered(bool active);
    void updateClusters(QString name,QList<int>& clustersToShow,ItemColors* clustersColors,bool active);
    void increaseAllAmplitude();
    void decreaseAllAmplitude();
    void showLabels(bool show);
    void nextCluster();
    void previousCluster();

protected:
    /** overwritten QWidget::closeEvent() to catch closing views. Does nothing, as the closeEvents for
    * KlustersView's are processed by KlustersApp::eventFilter(), so this overwitten closeEvent is necessary
    * and has to be empty. Don't overwrite this method !
    */
    void closeEvent(QCloseEvent* e);

    /** The document connected to the view, specified in the constructor */
    KlustersDoc& doc;

    /** Event filter to catch right click for contextual menu.
    * @param object target object for the event.
    * @param event event sent.
    */
    bool eventFilter(QObject* object,QEvent* event);

private:
    //members
    
    /** mainDock is the main DockWidget to which all other dockWidget will be dock. Inititalized in
     * the constructor.
     */
    QDockWidget* mainDock;

    /**List of the presented clusters*/
    QList<int>* shownClusters;

    /**List of the removed clusters*/
    QList<int>* removedClusters;

    /**Represents a list of list of removed clusters use to enable undo action.
    */
    QList< QList<int>* > removedClustersUndoList;

    /**Represents a list of list of removed clusters use to enable redo action.
    */
    QList< QList<int>* > removedClustersRedoList;
    
    /**Dimension shown on the abscissa axis*/
    int dimensionX;

    /**Dimension shown on the ordinate axis*/
    int dimensionY;

    /**An internal pointer to the last activated widget.*/
    ViewWidget* currentViewWidget;

    int numberUndo;

    /**True if the view contains a Waveform view, false otherwise.*/
    bool isThereWaveformView;

    /**True if the view contains a Waveform view and this one is in
    * time frame mode, false otherwise. The default is false.*/
    bool inTimeFrameMode;

    /**True if the view contains a cluster view, false otherwise.*/
    bool isThereClusterView;

    /**True if the view contains an ErrorMatrix view, false otherwise.*/
    bool isThereErrorMatrixView;
    
    /**True if the view contains a Trace view, false otherwise.*/
    bool isThereTraceView;
    
    /**Amount of time used in a WaveformView when looking for the spikes
    * when the presentation mode is time frame.
    * This amount is in second.
    */
    long timeWindow;

    /**Starting time in a WaveformView when looking for the spikes
    * when the presentation mode is time frame. This amount is in second.
    */
    long startTime;

    /**The number of spikes to display in the WaveformView, if any, when that view
    * is in sample mode.*/
    long nbSpkToDisplay;

    /**
    * Boolean indicating, for the WaveformView if any, if the waveforms for the presented clusters have to overlay.
    */
    bool overLayDisplay;

    /**
    * Boolean indicating, for the WaveformView if any, if all the waveforms, corresponding to the mode of presentation,
    * are shown or if only the mean and the standard deviation.
    */
    bool meanDisplay;

    /**Size of the bins to use to compute the correlograms.*/
    int binSize;

    /**Width of the time frame to use to compute the correlograms.*/
    int correlogramTimeFrame;

    /**True if the view contains a Correlation View, false otherwise.*/
    bool isThereCorrelationView;

    /**Store the type of presentation used in the Correlation View if any.
    * It can be raw data, data scale by the maximum valeu or data scale by the shoulder value.
    */
    Data::ScaleMode correlationScale;

    /**List of the ViewWidgets contained in this view.*/
    QList<ViewWidget*> viewList;

    /**Boolean indicating if a shoulder line has to be drawn on the correlograms
   * of the Correlation View if any.*/
    bool shoulderLine;

    /**Reference on the main window.*/
    KlustersApp& mainWindow;

    /**Counter for each type of view (ClusterView, WaveformView, CorrelationView and
   * ErrorMatrixView) in the view.*/
    QMap<QString,int> viewCounter;

    /**List containing the offset for each channel in a TraceView.*/
    QList<int> offsets;

    /**List of the exponents used to compute the drawing gain for each channel in a TraceView.*/
    QList<int> gains;

    QList<int> clustersToSkip;

    /**Pointer to the TraceView if any.*/
    TraceWidget* traceWidget;

    /*Starting time in miliseconds used in the TraceView.*/
    long startingTime;

    /*Time window in miliseconds used in the TraceView.*/
    long duration;

    /**True if labels are drawn next to the traces, false otherwise.*/
    bool labelsDisplay;

    //methods
    
    /** Creates the OverView View (composition of the 3 basic views).
    * @param backgroundColor color used as background.
    * @param statusBar QStatusBar which will be use to construct the viewWidgets.
    * @param timeInterval time interval between 2 time lines drawn in the cluster view.
    * when the time dimension is selected.
    * @param maxAmplitude initial gain use to draw the waveforms in the waveform view.
    * @param positions initial position of the channels in the waveform view.
    */
    void createOverview(const QColor& backgroundColor,QStatusBar * statusBar,int timeInterval,int maxAmplitude,QList<int> positions);

    /** Creates the Grouping Assistant view:
    * an Overview (composition of the 3 basic views) to which have been added
    * a ErrorMatrix View under the clusterview.
    * @param backgroundColor color used as background.
    * @param statusBar QStatusBar which will be use to construct the viewWidgets.
    * @param timeInterval time interval between 2 time lines drawn in the cluster view
    * when the time dimension is selected.
    * @param maxAmplitude initial gain use to draw the waveforms in the waveform view.
    * @param positions initial position of the channels in the waveform view.
    */
    void createGroupingAssistantView(const QColor& backgroundColor,QStatusBar * statusBar,int timeInterval,int maxAmplitude,QList<int> positions);

    /**
    * Adds a cluster to the view. The @p clusterId is added to both the shownClusters list
    * and the clusterUpdateList.
    * @param clusterId the id of the cluster to add
    * @param active true if the view is the active one, false otherwise.
    */
    void addClusterToView(int clusterId,bool active);

    /**
    * Removes the cluster from the the list of clusters shown and call removeClusterFromView
    * on all the widgets.
    * @param clusterId id of cluster to be removed
    * @param active true if the view is the active one, false otherwise.
     */
    void removeClusterFromView(int clusterId,bool active);

    /**
    * Removes the clusters from the the list of clusters.
    * @param clusterIds idc of the clusters to be removed
    * @param active true if the view is the active one, false otherwise.
    */
    void removeClustersFromView(const QVector<int> &clusterIds, bool active);
    
    /**
    * Returns the subset of the clusters contained in @p clusterlist which are in presented in this view.
    * @param clusterlist list to extract from.
    */
    QList<int> clustersInView(QList<int>& clusterlist);

    /**
    * Fills the undo list (removedClustersUndoList) and clear the redo list
    * (removedClustersRedoList) to prepare for a futur undo.
    * @param removedClustersTemp a pointer to the list of currently removed clusters which will be
    * added to removedClustersUndoList
    */
    void prepareUndo(QList<int>* removedClustersTemp);

    /**
    * Fills the undo list (removedClustersUndoList) and clear the redo list
    * (removedClustersRedoList) to prepare for a futur undo.
    * @param newlyRemovedClusters the list of currently removed clusters which will be added to removedClustersUndoList
    */
    void prepareUndo(QList<int>& newlyRemovedClusters);

    /**
    * In the process of the undo mechanism, the clusters which were removed are added back to the view.
    * @param active true if the view is the active one, false otherwise.
    */
    void addRemovedClusters(bool active);

    /**
    * In the process of the redo mechanism, the clusters which were added back to the view in the undo
    * are removed from the view again.
    * @param active true if the view is the active one, false otherwise.
    * @return true if any clusters have been removed, false otherwise.
    */
    bool removeUndoAddedClusters(bool active);

    /**
   * Updates the list of clusterIds shown using the new ids.
   * @param clusterIds map given for each current clusterId the new clusterId.
   */
    void changeClusterIds(QMap<int,int>& clusterIds);

    /**
   * In the process of the redo mechanism, the clusters which were added back to the list of existing clusters
   * in the undo are removed from the view if present.
   * @param active true if the view is the active one, false otherwise.
   * @param clustersToDelete list of clusters to delete.
   */
    void removeDeletedClusters(bool active,QList<int>& clustersToDelete);

    /**
   * Makes signal and slot connection between the current view and one of is contained view (@p view)
   * @param displayType type of view for which connections will be made.
   * @param view view for which connections will be made.
   * @param dockWidget QDockWidget containing the view for which connections will be made.
   */
    void setConnections(DisplayType displayType,QWidget* view,QDockWidget* dockWidget);

    /**
   * Makes signal and slot connection between the current view and the active correlogramView.
   * @param viewWidget view for which connections will be made.
   */
    void updateCorrelogramConnections(ViewWidget* viewWidget);
};

#endif // KCLUSTERSVIEW_H
