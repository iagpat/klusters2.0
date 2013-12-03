/***************************************************************************
                          klusters.h  -  description
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

#ifndef KLUSTERS_H
#define KLUSTERS_H

//include files application specific
#include "spinbox.h"
#include "klustersview.h"


// include files for Qt
#include <QList>
#include <QSpinBox> 
#include <qvalidator.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qapplication.h>


#include <QEvent>
#include <QShowEvent>

#include <QDockWidget>
#include <QMainWindow>
#include <QAction>
#include <QTableWidget>
#include <QProcess>



// forward declaration of the Klusters classes
class KlustersDoc;
class ClusterPalette;
class SaveThread;
class PrefDialog;
class ProcessWidget;
class QRecentFileAction;
class QExtendTabWidget;

/**
  * The Klusters main window and central class. It sets up the main
  * window and reads the config file as well as providing a menubar, toolbar
  * and statusbar. There is only one document open by application.
  * In initClusterPanel(), the palette for the clusters is created.
  * View windows are created in createView().The MDI child is an instance of KlustersView,
  * the document an instance of KlustersDoc.
  * KlustersApp reimplements the methods that DockMainWindow provides for main window handling and supports
  * full session management as well as keyboard accelerator configuration.
  * @author Lynn Hazan
  */
class KlustersApp : public QMainWindow
{
    Q_OBJECT

public:
    /** Construtor of KlustersApp, calls all init functions to create the application.
     * @see initMenuBar initToolBar
     */
    explicit KlustersApp();
    ~KlustersApp();

    /**Opens a file, only one document at the time is allowed.
    * Asking for a new one will open a new instance of the application with it.
    */
    virtual void openDocumentFile(const QString& url=QString());

    /** Imports a file using the old format, only one document at the time is allowed.
    * Asking for a new one will open a new instance of the application with it.
    */
    virtual void importDocumentFile(const QString& url=QString());

    /**Returns the view contains in the active display.
    * @return active view.
    */
    KlustersView* activeView();

    /**Returns true if the active display contains the output of a process, false otherwise.*/
    bool doesActiveDisplayContainProcessWidget();

    /**Adds a new view (ClusterView, WaveformView or CorrelationView) to the active display.
    * @param displayType type of view to add (ClusterView,WaveformView or CorrelationView).
    * @param docWidget QDockWidget to which the new view will be docked.
    */
    void widgetAddToDisplay(KlustersView::DisplayType displayType);

    /**Updates the menu due to the removal of a view (ClusterView, WaveformView or CorrelationView) in the active display.
    * @param displayType type of view to add (ClusterView,WaveformView or CorrelationView).
    */
    void widgetRemovedFromDisplay(KlustersView::DisplayType displayType);

    /**Informs of the existance of an Error Matrix View in the application.
    * @return true if an Error Matrix View exists in the application, false otherwise.
    */
    bool isExistAnErrorMatrix() const {return errorMatrixExists;}

    /**Updates the dimension spin boxes.
    * @param dimensionX absciss dimension.
    * @param dimensionY ordinate dimension.
    */
    void updateDimensionSpinBoxes(int dimensionX, int dimensionY);

    /**Updates the correlogeramView parameters.
    * @param binSize size of the bins to use to compute the correlograms.
    * @param timeWindow time frame to use to compute the correlograms.
    * @param isShoulderLine boolean indicating if a doted line is drawn at the shoulder level of the correlograms.
    * @param correlationScale  type of scale used to present the correlation data.
    */
    void updateCorrelogramViewVariables(int binSize,int timeWindow,bool isShoulderLine, Data::ScaleMode correlationScale);
    

protected:
    /** queryExit is called by KDocMainWindow when the last window of the application is going to be closed during the closeEvent().
     * Its purpose is purely to prepare the application (with possible user interaction)
     * so it can safely be closed later (without user interaction).
     */
    bool queryExit();
    
    void customEvent (QEvent *event);
    
    void showEvent(QShowEvent* event){slotUpdateParameterBar();}

    /** Event filter to catch right click for contextual menu.
    * @param object target object for the event.
    * @param event event sent.
    */
    bool eventFilter(QObject* object,QEvent* event);

    void closeEvent(QCloseEvent *event);

public Q_SLOTS:
    /** queryClose is called by KDocMainWindow call just before being closed.
     */
    bool queryClose();

    /**Shows or hide the parameters boxes base on the user settings.*/
    void slotUpdateParameterBar();

    /** Executes the preferences dialog.*/
    void executePreferencesDlg();

    /** Updates the widgets so that new user settings take effect.*/
    void applyPreferences();

    /**Initializes some of the variables defined in the settings (preferences).*/
    void initializePreferences();

    void slotStateChanged(const QString& state);

private Q_SLOTS:
    /** Open a file and load it into the document.*/
    void slotFileOpen();
    /** Opens a file in the old format and load it into the document.*/
    void slotFileImport();
    /** Opens a file from the recent files menu */
    void slotFileOpenRecent(const QString& url);
    /** Save the document */
    void slotFileSave();
    /** Renumbers the cluster and save the document.*/
    void slotFileRenumberAndSave();
    /** Save the document with a new filename.*/
    void slotFileSaveAs();
    /** Asks for saving if the file is modified, then closes the actual file and window*/
    void slotFileClose();
    /** Prints the views in the current display, for multiple-view displays, each view is printed on a separate page. */
    void slotFilePrint();
    /** Closes the document and quits the application.*/
    void slotFileQuit();
    /** Reverts the last user action.*/
    void slotUndo();
    /** Reverts the last undo action.*/
    void slotRedo();
    
    /** Toggles the main tool bar.*/
    void slotViewMainToolBar();
    /** Toggles the status bar.*/
    void slotViewStatusBar();
    /** Toggles the bar for the actions.*/
    void slotViewActionBar();
    /** Toggles the bar for the tools.*/
    void slotViewToolBar();
    /** Toggles the bar for the parameters.*/
    void slotViewParameterBar();
    /** Toggles the user cluster information in the cluster palette.*/
    void slotViewClusterInfo();
    
    /** Creates a new cluster display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotWindowNewClusterDisplay();
    /** Creates a new waveform display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotWindowNewWaveformDisplay();
    /** Creates a new crosscorrelation display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotWindowNewCrosscorrelationDisplay();
    /** Creates a new overview display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotWindowNewOverViewDisplay();
    
    /** Creates a new grouping assistant display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotWindowNewGroupingAssistantDisplay();

    /** Creates a new trace display for the document and adds the new display to the
     * list of displays the document maintains.
     */
    void slotNewTraceDisplay();
    
    /** Changes the statusbar contents for the standard label permanently, used to indicate current actions.
     * @param text the text that is displayed in the statusbar
     */
    void slotStatusMsg(const QString &text);
    /** Activates the MDI child widget when it gets selected from the window menu. */
    void viewMenuActivated( int id );
    /*Slots for the tools menu.*/
    /**Changes to a mode enabling the creation of a single cluster by selecting an area.*/
    void slotSingleNew();
    /**Changes to a mode enabling the creation of a multiple clusters by selecting an area.*/
    void slotMultipleNew();
    /**Changes to a mode enabling the deletion of spikes from a cluster and move them to the cluster (number 1) containing the poorly isolated cells.*/
    void slotDeleteNoise();
    /**Changes to a mode enabling the deletion of spikes from a cluster and move them to the cluster (number 0) containing the artefacts.*/
    void slotDeleteArtefact();
    /**Changes to a mode enabling the user to zoom.*/
    void slotZoom();
    
    /**Chooses the selection time tool, enabling the user to select a time frame
    * for which the traces are going to be displayed in the TraceView. This slot is accessible only if a TraceView and a ClusterView are present
    * and if one of the dimensions in the ClusterView is the time.*/
    void slotSelectTime();
    
    
    /*Slots for the actions menu.*/

    /**Redraws a cluster because his color has been changed.
    * @param clusterId id of the cluster to redraw.
    */
    void slotSingleColorUpdate(int clusterId);
    /**Draws the clusters contain in @p selectedClusters list.
    * @param selectedClusters list of clusters which have been selected to be shown.
    */
    void slotUpdateShownClusters(const QList<int> &selectedClusters);
    /**Groups the clusters contain in @p selectedClusters list and trigger the update of the displays.
    * @param selectedClusters list of clusters which have been selected to be grouped.
    */
    void slotGroupClusters(QList<int> selectedClusters);
    /**Calls the document to move the clusters contain in @p selectedClusters list
    * to the cluster of noise (cluster 1) and trigger the update of the displays.
    * @param selectedClusters list of clusters which have been selected to be moved
    */
    void slotMoveClustersToNoise(QList<int> selectedClusters);
    /**Calls the document to move the clusters contain in @p selectedClusters list
    * to the cluster of artefact (cluster 0) and trigger the update of the displays.
    * @param selectedClusters list of clusters which have been selected to be moved
    */
    void slotMoveClustersToArtefact(QList<int> selectedClusters);
    /**Sets the selection mode to immediate, disenabling the update action.*/
    void slotImmediateSelection();
    /**Set the selection mode to delay, enabling the update action.*/
    void slotDelaySelection();
    /**Updates the palette and the spine boxes when the active display changes.*/
    void slotTabChange(int index);
    /**Triggers an update of the dimensions due to a change of the absciss dimension.*/
    void slotUpdateDimensionX(int dimensionX);
    /**Triggers an update of the dimensions due to a change of the ordinate dimension.*/
    void slotUpdateDimensionY(int dimensionYs);
    /** Closes the display and if it is the last one asks for saving, then closes the actual file and window.*/
    void slotDisplayClose();
    /**Updates the number of undo*/
    void slotUpdateUndoNb(int undoNb);
    /**Updates the number of redo*/
    void slotUpdateRedoNb(int redoNb);

    /**Informs the active display to present the waveforms for an updated time frame.*/
    void slotUpdateStartTime(int start);

    /**Informs the active display to present the waveforms for an updated time frame.*/
    void slotUpdateDuration();

    /**Sets the presentation mode of the waveform view of the active display. It can be either the sample mode
   * or the time frame mode. In the sample mode, for each shown cluster, only one out of the number of spikes
   * to be displayed will be shown, the time frame mode, for each shown cluster, only the spikes
   * within the current time frame will be shown.
   */
    void slotTimeFrameMode();

    /**Sets the waveforms of each cluster in the active display to overlay or be side by side.
   */
    void setOverLayPresentation(){
        if(overlayPresentation->isChecked())activeView()->setOverLayPresentation();
        else activeView()->setSideBySidePresentation();
    }

    /**Sets the way of presenting the information concerning the waveforms selected in the active display.
   * It can be either the mean presentation or the normal presentation. In the mean presentation,
   * there is only the waveforms of the mean and the standard deviation. In the normal presentation all the
   * waveforms for the selected (depending on the sample/time frame mode) spikes are shown.
   */
    void slotMeanPresentation(){
        if(meanPresentation->isChecked())activeView()->setMeanPresentation();
        else activeView()->setAllWaveformsPresentation();
    }

    /**Triggers the increase of the amplitude of the waveforms in the waveform view.
   */
    void slotIncreaseAmplitude(){activeView()->increaseWaveformsAmplitude();}

    /**Triggers the decrease of the amplitude of the waveforms in the waveform view.
   */
    void slotDecreaseAmplitude(){activeView()->decreaseWaveformsAmplitude();}


    /**Informs the active display to present an updated number of waveforms when the
   * waveformView is in sample mode.
   */
    void slotSpikesTodisplay(int nbSpikes){
        if(!isInit){
            activeView()->setDisplayNbSpikes(static_cast<long>(nbSpikes));
        }
    }

    /**Informs the active display to present the correlations with a new time frame.*/
    void slotUpdateCorrelogramsHalfDuration();

    /**Informs the active display to present the correlations with a new size for the bins.*/
    void slotUpdateBinSize();

    /**Triggers the increase of the amplitude of the correlograms in the correlation view.
   */
    void slotIncreaseCorrelogramsAmplitude(){activeView()->increaseCorrelogramsAmplitude();}

    /**Triggers the decrease of the amplitude of the correlograms in the correlation view.
   */
    void slotDecreaseCorrelogramsAmplitude(){activeView()->decreaseCorrelogramsAmplitude();}

    /**Present the correlograms with the raw data without appling any scale.
   */
    void slotNoScale(){activeView()->setNoScale();}

    /**Presents the correlograms scaling the data by the maximum value.
   */
    void slotScaleByMax(){activeView()->setScaleByMax();}

    /**Present the correlograms scaling the data by the shoulder value.
   */
    void slotScaleByShouler(){activeView()->setScaleByShouler();}

    /**Informs the active display to update the drawing of a doted line
   * at the shoulder level on the correlograms.*/
    void slotShoulderLine(){
        activeView()->updateShoulderLine(shoulderLine->isChecked());
    }

    /**Triggers the update of the errorMatrix view in the grouping assistant view.
   */
    void slotUpdateErrorMatrix();

    /**Select all the clusters.*/
    void slotSelectAll();

    /**Selects all the clusters except the clusters of artefact and noise
   * (clusters 0 and 1 respectively).*/
    void slotSelectAllWO01();

    /**Launchs a separate process to recluster the selected clusters.*/
    void slotRecluster();

    /**Stops the separate process which is reclustering some clusters.*/
    void slotStopRecluster();

    /**Triggers the update of data incorporating the new data from the reclustering.
   * @param process process which has just finished.
   */
    void slotProcessExited(int, QProcess::ExitStatus);

    /**Updates internal state indicating that the outputs of the separate process, which
   * has been killed, is finished.*/
    void slotOutputTreatmentOver();

    /**Launches a dialog to enable the user to change the tab label of the active display.*/
    void renameActiveDisplay();

    /**Triggers the increase of the amplitude of all the channels.
   */
    void slotIncreaseAllChannelsAmplitude(){activeView()->increaseAllChannelsAmplitude();}

    /**Triggers the decrease of the amplitude of all the channels.
   */
    void slotDecreaseAllChannelsAmplitude(){activeView()->decreaseAllChannelsAmplitude();}

    /**Enables or disables the display of labels next to the traces.*/
    void slotShowLabels();

    /**Retrieves the next cluster.*/
    void slotShowNextCluster();

    /**Retrieves the previous cluster.*/
    void slotShowPreviousCluster();

    /**Updates, if needed, the browsing possibility of the traceView.*/
    void slotSpikesDeleted();

    /**Updates the status modified of the current opend document.*/
    void slotClusterInformationModified();

    void slotAbout();

    void slotHanbook();

    void slotSaveRecentFiles();

private:
    void readSettings();
    void initView();
    void createMenus();

    void createToolBar();

    /**Initializes the different parameter widgets.*/
    void initSelectionBoxes();
    
    /** Sets up the statusbar for the main window by initialzing a statuslabel.*/
    void initStatusBar();

    /** Creates the palette of clusters (left tool view).*/
    void initClusterPanel();

    /** Initialize the first display (create the mainDockWidget).*/
    void initDisplay();
    
    /** Creates a new display.
     * @param type enum representing the type of view to be created.
     */
    void createDisplay(KlustersView::DisplayType type);

    /**Updates the active display due to a change of one of the dimensions.
    * @param dimensionX absciss dimension.
    * @param dimensionX ordinate dimension.
    */
    void updateDimensions(int dimensionX, int dimensionY);

    /**Resets the state of the application to a none document open state.*/
    void resetState();

    /**Updates the acess to the undo/redo mechanism*/
    void updateUndoRedoDisplay();

    //Members
    
    /** A counter that gets increased each time the user creates a new display of the document with "Displays"->"New ...".*/
    int displayCount;

    /** mainDock is the main DockWidget to which all other dockWidget will be dock. Inititalized in
     * initDisplay()
     */
    DockArea* mainDock;

    /** clustersPanel is the DockWidget containing the ClusterPalette. Inititalized in initClusterPanel()
     */
    QDockWidget* clusterPanel;

    /** ClusterPalette is the Widget containing the cluster list. Inititalized in initClusterPanel()
    */
    ClusterPalette* clusterPalette;

    /**
    * Represents the document on which the application works
    */
    KlustersDoc* doc;

    /** tabsParent groups all the tabs, it is updated eache time a display is added.
    * It is null when there is only one display open. It enables to get the active tab.
    */
    QExtendTabWidget* tabsParent;

    QToolBar* paramBar;
    QToolBar* mActionBar;
    QToolBar* mToolBar;
    QToolBar* mClusterBar;
    QToolBar* mMainToolBar;

    QRecentFileAction* mFileOpenRecent;
    
    //Action and toolbar pointers
    QAction* viewMainToolBar;
    //QActionMenu* viewMenu;
    QAction* newClusterDisplay;
    QAction* newWaveformDisplay;
    QAction* newCrosscorrelationDisplay;
    QAction* newOverViewDisplay;
    QAction* newGroupingAssistantDisplay;
    QAction* viewActionBar;
    QAction* viewToolBar;
    QAction* viewParameterBar;
    QAction* viewClusterInfo;
    QAction* timeFrameMode;
    QAction* overlayPresentation;
    QAction* meanPresentation;
    QAction* noScale;
    QAction* scaleByMax;
    QAction* scaleByShouler;
    QAction* shoulderLine;
    QAction* showHideLabels;

    QAction *mRenameActiveDisplay;
    QAction *mCloseActiveDisplay;
    QAction *mNewTraceDisplay;
    QAction *mDeleteArtifact;
    QAction *mDeleteNoisy;
    QAction *mGroupeClusters;
    QAction *mUpdateDisplay;
    QAction *mRenumberClusters;
    QAction *mReCluster;
    QAction *mAbortReclustering;
    QAction *mZoomAction;
    QAction *mNewCluster;
    QAction *mSplitClusters;
    QAction *mDeleteArtifactSpikes;
    QAction *mDeleteNoisySpikes;
    QAction *mSelectTime;
    QAction *mSelectAllAction;
    QAction *mSelectAllExceptAction;
    QAction *mSaveAction;
    QAction *mSaveAsAction;
    QAction *mCloseAction;
    QAction *mImportFile;
    QAction *mIncreaseAmplitude;
    QAction *mDecreaseAmplitude;
    QAction *mDecreaseAmplitudeCorrelation;
    QAction *mIncreaseChannelAmplitudes;
    QAction *mDecreaseChannelAmplitudes;
    QAction *mNextSpike;
    QAction *mPreviousSpike;
    QAction *mPrintAction;
    QAction *mImmediateSelection;
    QAction *mDelaySelection;
    QAction *mUndo;
    QAction *mIncreaseAmplitudeCorrelation;
    QAction *mQuitAction;
    QAction *mOpenAction;
    QAction *mRedo;
    QAction *mRenumberAndSave;
    QAction *mUpdateErrorMatrix;
    QAction *mPreferenceAction;

    QAction *mViewStatusBar;
    /**Spine box enabling to choose the absciss dimension*/
    SpinBox* dimensionX;

    /**Spine box enabling to choose the ordinate dimension*/
    SpinBox* dimensionY;

    /**Boolean used to prevent the trigger of the spin box update during initialization.*/
    bool isInit;

    /**The current number of undo used to enable/disable the the undo action.*/
    int currentNbUndo;

    /**The current number of redo used to enable/disable the the redo action.*/
    int currentNbRedo;

    /**Thread used to save a document as it can take 5 seconds. The thread is used only
    * when using the save menu, on quit if a save is need it the thread is not call.
    */
    SaveThread* saveThread;

    /**Amount of time used when looking for the spikes when the presentation mode is time frame.
    * This amount is in second and the default is 30.
    */
    long timeWindow;

    /**Starting time when looking for the spikes when the presentation mode is time frame.
    * This amount is in second and the default is 0.
    */
    long startTime;

    /**Spine box enabling to choose the start time used to display the waveforms while
    * in time frame mode.
    */
    SpinBox* start;

    /**Small box where the user can enter the width of the time frame to use
    * to display the waveforms while in time frame mode.*/
    QLineEdit* duration;

    class Validator;
    friend class Validator;

    /**
    * Represents a validator for the duration lineEdit which fix any bad entry
    * by setting the last correct value.
    */
    class Validator: public QIntValidator{

    public:
        Validator(QObject* parent):QIntValidator(parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        Validator(int minimum,int maximum,QObject* parent):
            QIntValidator(minimum,maximum,parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        ~Validator(){}
        void fixup (QString& input) const{
            input = QString::fromLatin1("%1").arg(klusters->timeWindow);
        }
    private:
        KlustersApp* klusters;
    };
    
    /**A validator for the time frame to use to display the waveforms while
    * in time frame mode (@see duration). The limits are zero and the maximun of time for the current document.
    */
    Validator validator;

    QLabel* featureXLabel;
    QLabel* durationLabel;
    QLabel* startLabel;

    /**Spine box enabling to choose the number of spikes to display in
    * the waveform view, if any, while that view is in sample mode.
    */
    SpinBox* spikesTodisplay;

    /**The step used to increase or decrease the number of spikes to display
    * in the waveform view, if any, while that view is in sample mode. The default
    * is 20;
    */
    long spikesTodisplayStep;
    
    QLabel* spikesTodisplayLabel;
    
    static const QString INITIAL_WAVEFORM_TIME_WINDOW;
    static const long DEFAULT_NB_SPIKES_DISPLAYED;
    /**The size of half the time frame for the correlations has to be k*bineSize,
    * as the total number of bins = 2*halfbins + 1 (halfBins.5 for each half time frame).
    */
    static const QString INITIAL_CORRELOGRAMS_HALF_TIME_FRAME;
    static const QString DEFAULT_BIN_SIZE;

    /**Length of the recording in miliseconds.*/
    long maximumTime;
    
    /**Small box where the user can enter the width of half the time frame to use
    * to compute the correlograms.*/
    QLineEdit* correlogramsHalfDuration;

    /**Small box where the user can enter the size of the bins to use
    * to compute the correlograms.*/
    QLineEdit* binSizeBox;

    /**Time frame to use to compute the correlograms.*/
    int correlogramTimeFrame;
    
    /**Size of the bins to use to compute the correlograms.*/
    int binSize;

    QLabel* correlogramsHalfDurationLabel;
    QLabel* binSizeLabel;

    class BinSizeValidator;
    friend class BinSizeValidator;

    /**
    * Represents a validator for the binSize lineEdit which fix any bad entry
    * by setting the last correct value and update the correlogramsHalfDuration
    * to ensure the relation correlogramsTimeFrame = (2k + 1) * binSize.
    */
    class BinSizeValidator: public QIntValidator{

    public:
        BinSizeValidator(QObject* parent):QIntValidator(parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        BinSizeValidator(int minimum,int maximum,QObject* parent):
            QIntValidator(minimum,maximum,parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        ~BinSizeValidator(){}
        void fixup (QString& input) const{
            //If the state determine in validate was invalid, fix by setting back the last correct value.
            input = QString::fromLatin1("%1").arg(klusters->binSize);
        }
        QValidator::State validate(QString &input,int& pos) const{
            QValidator::State state = QIntValidator::validate(input,pos);
            //Let the QIntValidator validates the value as to know if it is a correct integer (within the range).
            if(state != QValidator::Acceptable) return state;
            //If the value is a correct integer, update the correlogramsHalfDuration if need it.
            // correlogramTimeFrame (2 * correlogramsHalfDuration) has to be (2k + 1) * binSize.
            else{
                int sizeOfBin = input.toInt();
                int halfTimeFrame = (klusters->correlogramsHalfDuration->displayText()).toInt();

                float x = (2*static_cast<float>(halfTimeFrame)/static_cast<float>(sizeOfBin)-1)*0.5;
                int k = static_cast<int>(x + 0.5);

                klusters->correlogramTimeFrame = (2*k+1) * sizeOfBin;
                if(k != x){
                    // klusters->correlogramsHalfDuration->setText(QString::fromLatin1("%1").arg(k * sizeOfBin));
                    klusters->correlogramsHalfDuration->setText(QString::fromLatin1("%1").arg(static_cast<int>(klusters->correlogramTimeFrame / 2)));
                }
                return state;
            }
        }
    private:
        KlustersApp* klusters;
    };

    /**A validator for the bin size of the correlations.
    * The range is between 0-1 and the maximun of time for the current document in miliseconds.
    */
    BinSizeValidator binSizeValidator;

    class CorrelogramsHalfTimeFrameValidator;
    friend class CorrelogramsHalfTimeFrameValidator;

    /**
    * Represents a validator for the correlogramTimeFrame lineEdit which fix any bad entry
    * by setting the last correct value and update it
    * to ensure the relation correlogramsTimeFrame = k * binSize.
    */
    class CorrelogramsHalfTimeFrameValidator: public QIntValidator{

    public:
        CorrelogramsHalfTimeFrameValidator(QObject* parent):QIntValidator(parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        CorrelogramsHalfTimeFrameValidator(int minimum,int maximum,QObject* parent):
            QIntValidator(minimum,maximum,parent){
            klusters = dynamic_cast<KlustersApp*>(parent);
        }
        ~CorrelogramsHalfTimeFrameValidator(){}
        void fixup (QString& input) const{
            int halfTimeFrame = input.toInt();

            //The value entered was not an integer
            if(halfTimeFrame == 0){
                input = QString::fromLatin1("%1").arg((klusters->correlogramTimeFrame - klusters->binSize) / 2);
                return;
            }
        }

    private:
        KlustersApp* klusters;
    };

    /**A validator for the time frame used to compute the correlations.
    */
    CorrelogramsHalfTimeFrameValidator correlogramsHalfTimeFrameValidator;

    /**Settings dialog.*/
    PrefDialog* prefDialog;

    /**Time interval between 2 lines drawn in the cluster views when the time dimension is selected.*/
    int displayTimeInterval;
    
    /**Initial gain used to display the waveforms in the waveform views.*/
    int waveformsGain;

    /**Position of the channels in the waveform views.*/
    QList<int> channelPositions;

    /**Background color for the views.*/
    QColor backgroundColor;

    /**Widget launching external process and displaying its output.*/
    ProcessWidget* processWidget;

    /**True if the external process has exited, false otherwise.*/
    bool processFinished;

    /**True if all the outputs of the external process have been printed, false otherwise.*/
    bool processOutputsFinished;

    /**List of the clusters to recluster.*/
    QList<int> clustersToRecluster;

    /**List of the clusters created by the reclustering.*/
    QList<int> clustersFromReclustering;

    /**Path to the reclustering executable.*/
    QString reclusteringExecutable;

    /**Arguments for the reclustering executable.*/
    QString reclusteringArgs;

    /**Name of the reclustering fet file.*/
    QString reclusteringFetFileName;

    /**True if the process has been killed through Klusters.*/
    bool processKilled;

    /**True if a Error Martix exists, false otherwise.*/
    bool errorMatrixExists;

    /**The path of the currently open document.*/
    QString filePath;

    bool useWhiteColorDuringPrinting;


    QAction *featureXLabelAction;
    QAction *dimensionXAction;
    QAction *dimensionYAction;
    QAction *startLabelAction;
    QAction *startAction;
    QAction *durationLabelAction;
    QAction *durationAction;
    QAction *spikesTodisplayLabelAction;
    QAction *spikesTodisplayAction;
    QAction *binSizeLabelAction;
    QAction *binSizeBoxAction;
    QAction *correlogramsHalfDurationLabelAction;
    QAction *correlogramsHalfDurationAction;


};

#endif // KCLUSTERS_H
