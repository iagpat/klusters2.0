/***************************************************************************
                          klusters.cpp  -  description
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

// include files for QT
#include <qdir.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qimage.h>
#include <qiconset.h>  
#include <qcursor.h>
#include <qfileinfo.h> 

// include files for KDE
#include <kprinter.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kstatusbar.h>
#include <kpopupmenu.h>
#include <kglobal.h>
#include <ktoolbar.h>
#include <kkeydialog.h>
#include <kio/netaccess.h>
#include <kprocess.h>
#include <kinputdialog.h>

//General C++ include files
#include <iostream>
using namespace std;

// application specific includes
#include "klusters.h"
#include "klustersdoc.h"
#include "clusterPalette.h"
#include "savethread.h"
#include "prefdialog.h"
#include "configuration.h"  // class Configuration
#include "processwidget.h"

extern int nbUndo;

const QString KlustersApp::INITIAL_WAVEFORM_TIME_WINDOW = "30";
const long KlustersApp::DEFAULT_NB_SPIKES_DISPLAYED = 100;
const QString KlustersApp::INITIAL_CORRELOGRAMS_HALF_TIME_FRAME = "30";
const QString KlustersApp::DEFAULT_BIN_SIZE = "1";


KlustersApp::KlustersApp():KDockMainWindow(0,"Klusters"), displayCount(0),mainDock(0),
clusterPanel(0),clusterPalette(0),tabsParent(0),dimensionX(0L),dimensionY(0L),isInit(true),
currentNbUndo(0),currentNbRedo(0),timeWindow(INITIAL_WAVEFORM_TIME_WINDOW.toLong()),validator(this),spikesTodisplayStep(20),
correlogramTimeFrame(INITIAL_CORRELOGRAMS_HALF_TIME_FRAME.toInt() * 2 + 1),binSize(DEFAULT_BIN_SIZE.toInt()),binSizeValidator(this),
correlogramsHalfTimeFrameValidator(this),prefDialog(0L),processWidget(0L),processFinished(true),processOutputDock(0L),
processOutputsFinished(true),processKilled(false),errorMatrixExists(false),filePath("")
{
	
  //Gets the configuration object of the application throught the static reference to the application kapp
  config = KGlobal::config();
  printer = new KPrinter;
  
  //Apply the user settings.
  initializePreferences();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initClusterPanel();

  //Create a KlustersDoc which will hold the document manipulated by the application.
  doc = new KlustersDoc(this,*clusterPalette,configuration().isCrashRecovery(),configuration().crashRecoveryInterval());
  //create the thread which will be used to save the cluster file
  saveThread = new SaveThread(this);
  
  //Prepare the actions
  initActions();
  
  //Prepare the spineboxes and line edit
  initSelectionBoxes();

  setMinimumSize(QSize(600,400));
  
  // Apply the saved mainwindow settings, if any, and ask the mainwindow
  // to automatically save settings if changed: window size, toolbar
  // position, icon size, etc.
  setAutoSaveSettings();

  slotUpdateParameterBar();
  
  // initialize the recent file list
  fileOpenRecent->loadEntries(config);

  //Disable some actions at startup (see the klustersui.rc file)
  slotStateChanged("initState");
}

KlustersApp::~KlustersApp()
{
  //Clear the memory by deleting all the pointers
  delete printer;
  delete doc;
  delete saveThread;
  if(processWidget != 0L){
   delete processWidget;
   processWidget = 0L; 
  }
  if(processOutputDock != 0L){
   delete processOutputDock;
   processOutputDock = 0L;
  }
}

void KlustersApp::initActions()
{

//File menu
  KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection());
  KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  new KAction(i18n("&Close"), "fileclose",0,this, SLOT(slotFileClose()),actionCollection(), "file_close");

  KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  KStdAction::quit(this, SLOT(close()), actionCollection());
  viewMainToolBar = KStdAction::showToolbar(this, SLOT(slotViewMainToolBar()), actionCollection());
  viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());
  new KAction(i18n("Re&number and Save"),QIconSet(QPixmap("filesave.png")), CTRL + SHIFT + Key_S,this, SLOT(slotFileRenumberAndSave()),actionCollection(), "file_renumber_save");
  new KAction(i18n("&Import File"), CTRL + Key_I,this, SLOT(slotFileImport()),actionCollection(), "fileImport");
              
  //Edit menu
  new KAction(i18n("Select &All"), CTRL + Key_A, this, SLOT(slotSelectAll()),actionCollection(),"edit_select_all");
  new KAction(i18n("Select All except 0 and 1"), CTRL + SHIFT + Key_A, this, 
    SLOT(slotSelectAllWO01()),actionCollection(),"edit_select_all_except01");
  KStdAction::undo(this, SLOT(slotUndo()), actionCollection());
  KStdAction::redo(this, SLOT(slotRedo()), actionCollection());
    
  //Displays menu
  viewMenu = new KActionMenu(i18n("&Window"), actionCollection(), "window_menu");
  newClusterDisplay = new KAction(i18n("New C&luster Display"), 0, this, SLOT(slotWindowNewClusterDisplay()), actionCollection(),"new_clusterDisplay");
  newWaveformDisplay = new KAction(i18n("New &Waveform Display"), 0, this, SLOT(slotWindowNewWaveformDisplay()), actionCollection(),"new_waveformDisplay"); 
  newCrosscorrelationDisplay = new KAction(i18n("New C&orrelation Display"),0,this,SLOT(slotWindowNewCrosscorrelationDisplay()), actionCollection(),"new_crosscorrelationDisplay");
  newOverViewDisplay = new KAction(i18n("New &Overview Display"), 0, this,SLOT(slotWindowNewOverViewDisplay()),actionCollection(),"new_overViewDisplay");
  newGroupingAssistantDisplay =
   new KAction(i18n("New &Grouping Assistant Display"),0,this,SLOT(slotWindowNewGroupingAssistantDisplay()),actionCollection(),"new_goupingAssistantDisplay");   
  new KAction(i18n("&Rename Active Display"), CTRL + Key_R, this, SLOT(renameActiveDisplay()),actionCollection(),"rename_display");
  new KAction(i18n("&Close Active Display"), CTRL + Key_W, this, SLOT(slotDisplayClose()),actionCollection(),"close_display");
  new KAction(i18n("New &Trace Display"),0, this, SLOT(slotNewTraceDisplay()),actionCollection(),"trace_display");

  //Not used for the moment.
  connect(viewMenu->popupMenu(),SIGNAL(aboutToShow()), this, SLOT(viewMenuAboutToShow()));

  KIconLoader* loader = KGlobal::iconLoader();

  //Actions menu
  new KAction(i18n("Delete &Artifact Cluster(s)"),QIconSet(loader->loadIcon("delete_artefact", KIcon::User)),SHIFT + Key_Delete,clusterPalette, SLOT(moveClustersToArtefact()),actionCollection(), "move_clusters_to_artifact");
  new KAction(i18n("Delete &Noisy Cluster(s)"),QIconSet(loader->loadIcon("delete_noise", KIcon::User)),Key_Delete,clusterPalette, SLOT(moveClustersToNoise()),actionCollection(), "move_clusters_to_noise");
  new KAction(i18n("&Group Clusters"),QIconSet(loader->loadIcon("group", KIcon::User)), Key_G,clusterPalette, SLOT(groupClusters()),actionCollection(), "group_clusters");
  new KAction(i18n("&Update Display"),QIconSet(loader->loadIcon("update", KIcon::User)), 0,clusterPalette, SLOT(updateClusters()),actionCollection(), "update_display");
  new KAction(i18n("&Renumber Clusters"),0, Key_R,doc, SLOT(renumberClusters()),actionCollection(), "renumber");
  new KAction(i18n("&Update Error Matrix"),QIconSet(loader->loadIcon("grouping_assistant_update", KIcon::User)),Key_U,this, SLOT(slotUpdateErrorMatrix()),actionCollection(),
            "update_errorMatrix");
  new KAction(i18n("Re&cluster"),0, SHIFT  + Key_R,this, SLOT(slotRecluster()),actionCollection(), "recluster");
  new KAction(i18n("&Abort Reclustering"),0, 0,this, SLOT(slotStopRecluster()),actionCollection(), "stop_recluster");
             
  //Tools menu
  new KAction(i18n("Zoom"),QIconSet(loader->loadIcon("zoom_tool", KIcon::User)), Key_Z,this, SLOT(slotZoom()),actionCollection(), "zoom");
  new KAction(i18n("New Cluster"),QIconSet(loader->loadIcon("new_cluster", KIcon::User)), Key_C,this, SLOT(slotSingleNew()),actionCollection(), "single_new");
  new KAction(i18n("&Split Clusters"),QIconSet(loader->loadIcon("new_clusters", KIcon::User)), Key_S,this, SLOT(slotMultipleNew()),actionCollection(), "multiple_new");
  new KAction(i18n("Delete &Artifact Spikes"),QIconSet(loader->loadIcon("delete_artefact_tool", KIcon::User)),Key_A,this, SLOT(slotDeleteArtefact()),actionCollection(), "delete_artifact");
  new KAction(i18n("Delete &Noisy Spikes"),QIconSet(loader->loadIcon("delete_noise_tool", KIcon::User)),Key_N,this, SLOT(slotDeleteNoise()),actionCollection(), "delete_noise");
  new KAction(i18n("Select Time"),QIconSet(loader->loadIcon("time_tool", KIcon::User)), Key_W,this, SLOT(slotSelectTime()),actionCollection(), "time");
  //Waveforms menu
  timeFrameMode = new KToggleAction(i18n("&Time Frame"), Key_T,this, SLOT(slotTimeFrameMode()),actionCollection(), "time_frame");
  overlayPresentation = new KToggleAction(i18n("&Overlay"), Key_O,this, SLOT(setOverLayPresentation()),actionCollection(), "overlay");
  meanPresentation = new KToggleAction(i18n("&Mean and Standard Deviation"), Key_M,this, SLOT(slotMeanPresentation()),actionCollection(), "mean");
  new KAction(i18n("&Increase Amplitude"), Key_I,this, SLOT(slotIncreaseAmplitude()),actionCollection(), "increase");
  new KAction(i18n("&Decrease Amplitude"), Key_D,this, SLOT(slotDecreaseAmplitude()),actionCollection(), "decrease");
  timeFrameMode->setChecked(false);
  overlayPresentation->setChecked(false);                      
  meanPresentation->setChecked(false);                                         

  //Correlations menu
  scaleByMax = new KRadioAction(i18n("Scale by &Maximum"),SHIFT + Key_M,this,SLOT(slotScaleByMax()),actionCollection(), "scale_by_max");
  scaleByMax->setExclusiveGroup("scale");
  scaleByShouler = new KRadioAction(i18n("Scale by &Asymptote"),SHIFT + Key_A,this,SLOT(slotScaleByShouler()),actionCollection(), "scale_by_shouler");
  scaleByShouler->setExclusiveGroup("scale");
  noScale = new KRadioAction(i18n("&Uniform Scale"),SHIFT + Key_U,this,SLOT(slotNoScale()),actionCollection(), "raw_data");
  noScale->setExclusiveGroup("scale");
  //Initialize the presentation mode to scale by maximum.
  scaleByMax->setChecked(true);
  new KAction(i18n("&Increase Amplitude"),SHIFT + Key_I,this, SLOT(slotIncreaseCorrelogramsAmplitude()),actionCollection(), "increase_correlograms");
  new KAction(i18n("&Decrease Amplitude"),SHIFT +  Key_D,this, SLOT(slotDecreaseCorrelogramsAmplitude()),actionCollection(), "decrease_correlograms");
  shoulderLine = new KToggleAction(i18n("Asymptote &Line"), Key_L,this, SLOT(slotShoulderLine()),actionCollection(), "shoulder_line");

  //Traces menu
   new KAction(i18n("&Increase Channel Amplitudes"),CTRL + SHIFT + Key_I,this, SLOT(slotIncreaseAllChannelsAmplitude()),actionCollection(), "increase_all_channels");
   new KAction(i18n("&Decrease Channel Amplitudes"),CTRL + SHIFT + Key_D,this, SLOT(slotDecreaseAllChannelsAmplitude()),actionCollection(), "decrease_all_channels");
   showHideLabels = new KToggleAction(i18n("Show &Labels"),0,CTRL + Key_L, this, SLOT(slotShowLabels()), actionCollection(),"show_labels");
   showHideLabels->setChecked(false);
   new KAction(i18n("&Next Spike"),QIconSet(loader->loadIcon("forwardCluster", KIcon::User)),CTRL + SHIFT + Key_F, this, SLOT(slotShowNextCluster()), actionCollection(),"show_next_cluster");
   new KAction(i18n("&Previous Spike"),QIconSet(loader->loadIcon("backCluster", KIcon::User)),CTRL + SHIFT + Key_B, this, SLOT(slotShowPreviousCluster()), actionCollection(),"show_previous_cluster");
  
  //Settings menu
  viewActionBar = new KToggleAction(i18n("Show Actions"),0,this, SLOT(slotViewActionBar()),actionCollection(), "show_actionBar");
  viewActionBar->setChecked(true);
  viewToolBar = new KToggleAction(i18n("Show Tools"),0,this, SLOT(slotViewToolBar()),actionCollection(), "show_toolBar");
  viewToolBar->setChecked(true);
  viewParameterBar = new KToggleAction(i18n("Show Parameters"),0,this, SLOT(slotViewParameterBar()),actionCollection(), "show_parameterBar");
  viewParameterBar->setChecked(true);
  KRadioAction* immediateSelection = new KRadioAction(i18n("Immediate Update"),0,this,SLOT(slotImmediateSelection()),actionCollection(), "immediate_selection");
  immediateSelection->setExclusiveGroup("selection");
  KRadioAction* delaySelection = new KRadioAction(i18n("Delayed Update"),0,this,SLOT(slotDelaySelection()),actionCollection(), "delay_selection");
  delaySelection->setExclusiveGroup("selection");
  viewClusterInfo = new KToggleAction(i18n("Show Cluster Info"),0,this, SLOT(slotViewClusterInfo()),actionCollection(), "show_clusterInfo");
  viewClusterInfo->setChecked(false);
  
  //Initialize the update mode
  immediateSelection->setChecked(true);
  
  KStdAction::keyBindings(this, SLOT(slotKeyBindings()), actionCollection());
  KStdAction::preferences(this,SLOT(executePreferencesDlg()), actionCollection());

  fileOpenRecent->setStatusText(i18n("Opens a recently used file"));
  actionCollection()->action(KStdAction::stdName(KStdAction::Save))->setStatusText(i18n("Saves the actual document"));
  actionCollection()->action(KStdAction::stdName(KStdAction::SaveAs))->setStatusText(i18n("Saves the actual document as..."));
  actionCollection()->action(KStdAction::stdName(KStdAction::Close))->setStatusText(i18n("Closes the actual document"));
  actionCollection()->action(KStdAction::stdName(KStdAction::Print))->setStatusText(i18n("Prints out the actual document"));
  actionCollection()->action(KStdAction::stdName(KStdAction::Quit))->setStatusText(i18n("Quits the application"));
  viewMainToolBar->setStatusText(i18n("Enables/disables the main tool bar"));
  viewStatusBar->setStatusText(i18n("Enables/disables the status bar"));
 
  //Custom connections
  connect(clusterPalette, SIGNAL(singleChangeColor(int)), this, SLOT(slotSingleColorUpdate(int)));
  connect(clusterPalette, SIGNAL(updateShownClusters(QValueList<int>)), this, SLOT(slotUpdateShownClusters(QValueList<int>)));
  connect(clusterPalette, SIGNAL(groupClusters(QValueList<int>)), this, SLOT(slotGroupClusters(QValueList<int>)));
  connect(clusterPalette, SIGNAL(moveClustersToNoise(QValueList<int>)), this, SLOT(slotMoveClustersToNoise(QValueList<int>)));
  connect(clusterPalette, SIGNAL(moveClustersToArtefact(QValueList<int>)), this, SLOT(slotMoveClustersToArtefact(QValueList<int>)));
  connect(clusterPalette, SIGNAL(clusterInformationModified()), this, SLOT(slotClusterInformationModified()));
  connect(doc, SIGNAL(updateUndoNb(int)), this, SLOT(slotUpdateUndoNb(int)));
  connect(doc, SIGNAL(updateRedoNb(int)), this, SLOT(slotUpdateRedoNb(int)));
  connect(doc, SIGNAL(spikesDeleted()), this, SLOT(slotSpikesDeleted()));
  
  //Actually create the menus and toolbars
  createGUI();
}

void KlustersApp::initSelectionBoxes(){
  QFont font("Helvetica",9);

  paramBar = toolBar("parameterBar");
 
  //Create and initialize the spin boxes for the dimensions
  dimensionX = new QSpinBox(1,1,1,paramBar,"dimensionX");
  dimensionY = new QSpinBox(1,1,1,paramBar,"dimensionY");
  //Enable to step the value from the highest value to the lowest value and vice versa
  dimensionX->setWrapping(true);
  dimensionY->setWrapping(true);
  featureXLabel = new QLabel("Features (x,y) ",paramBar);
  featureXLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  featureXLabel->setFont(font);
  //Insert the spine boxes in the main tool bar and make the connections
  paramBar->insertWidget(-1,featureXLabel->sizeHint().width(),featureXLabel);
  paramBar->insertWidget(-1,dimensionX->sizeHint().width(),dimensionX);
  paramBar->insertWidget(-1,dimensionY->sizeHint().width(),dimensionY);
  connect(dimensionX, SIGNAL(valueChanged(int)),this, SLOT(slotUpdateDimensionX(int)));
  connect(dimensionY, SIGNAL(valueChanged(int)),this, SLOT(slotUpdateDimensionY(int)));

  //Create and initialize the spin boxe and lineEdit for the waveforms time frame mode.
  start = new QSpinBox(1,1,timeWindow,paramBar,"start");
  //Enable to step the value from the highest value to the lowest value and vice versa
  start->setWrapping(true);
  duration = new QLineEdit(paramBar,INITIAL_WAVEFORM_TIME_WINDOW);
  duration->setMaxLength(5);
  //duration will only accept integers between 0 and a max equal
  //to maximum of time for the current document (set when the document will be opened)
  duration->setValidator(&validator);
  durationLabel = new QLabel("  Duration (s)",paramBar);
  durationLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  durationLabel->setFont(font);
  startLabel = new QLabel("  Start time (s)",paramBar);
  startLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  startLabel->setFont(font);
  paramBar->insertWidget(-1,startLabel->sizeHint().width(),startLabel);
  start->setMinimumSize(70,start->minimumHeight());
  start->setMaximumSize(70,start->maximumHeight());
  paramBar->insertWidget(-1,start->sizeHint().width(),start);
  paramBar->insertWidget(-1,durationLabel->sizeHint().width(),durationLabel);
  duration->setMinimumSize(70,duration->minimumHeight());
  duration->setMaximumSize(70,duration->maximumHeight());
  paramBar->insertWidget(-1,duration->sizeHint().width(),duration);
  connect(start, SIGNAL(valueChanged(int)),this, SLOT(slotUpdateStartTime(int)));
  connect(duration, SIGNAL(returnPressed()),this, SLOT(slotUpdateDuration()));

  //Create and initialize the spin boxe for the waveforms sample mode.
  spikesTodisplay = new QSpinBox(1,1,spikesTodisplayStep,paramBar,"spikesTodisplay");
  //Enable to step the value from the highest value to the lowest value and vice versa
  spikesTodisplay->setWrapping(true);
  spikesTodisplayLabel = new QLabel("  Waveforms",paramBar);
  spikesTodisplayLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  spikesTodisplayLabel->setFont(font);
  paramBar->insertWidget(-1,spikesTodisplayLabel->sizeHint().width(),spikesTodisplayLabel);
  spikesTodisplay->setMinimumSize(70,spikesTodisplay->minimumHeight());
  spikesTodisplay->setMaximumSize(70,spikesTodisplay->maximumHeight());
  paramBar->insertWidget(-1,spikesTodisplay->sizeHint().width(),spikesTodisplay);
  connect(spikesTodisplay, SIGNAL(valueChanged(int)),this, SLOT(slotSpikesTodisplay(int)));

  //Create and initialize the lineEdit for the correlations.
  binSizeBox = new QLineEdit(paramBar,DEFAULT_BIN_SIZE);
  binSizeBox->setMaxLength(10);
  //binSizeBox will only accept integers between 1 and a max equal
  //to maximum of time for the current document in miliseconds (set when the document will be opened)
  binSizeBox->setValidator(&binSizeValidator);
  binSizeLabel = new QLabel("  Bin size (ms)",paramBar);
  binSizeLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  binSizeLabel->setFont(font);
  paramBar->insertWidget(-1,binSizeLabel->sizeHint().width(),binSizeLabel);
  binSizeBox->setMinimumSize(30,binSizeBox->minimumHeight());
  binSizeBox->setMaximumSize(30,binSizeBox->maximumHeight());
  paramBar->insertWidget(-1,binSizeBox->sizeHint().width(),binSizeBox);
  connect(binSizeBox, SIGNAL(returnPressed()),this, SLOT(slotUpdateBinSize()));

  correlogramsHalfDuration = new QLineEdit(paramBar,INITIAL_CORRELOGRAMS_HALF_TIME_FRAME);
  correlogramsHalfDuration->setMaxLength(12);
  //correlogramsHalfDuration will only accept integers between 1 and a max equal
  //to half the maximum of time for the current document in miliseconds (set when the document will be opened)
  correlogramsHalfDuration->setValidator(&correlogramsHalfTimeFrameValidator);
  correlogramsHalfDurationLabel = new QLabel("  Duration (ms)",paramBar);
  correlogramsHalfDurationLabel->setFrameStyle(QFrame::ToolBarPanel|QFrame::Plain);
  correlogramsHalfDurationLabel->setFont(font);
  paramBar->insertWidget(-1,correlogramsHalfDurationLabel->sizeHint().width(),correlogramsHalfDurationLabel);
  correlogramsHalfDuration->setMinimumSize(70,correlogramsHalfDuration->minimumHeight());
  correlogramsHalfDuration->setMaximumSize(70,correlogramsHalfDuration->maximumHeight());
  paramBar->insertWidget(-1,correlogramsHalfDuration->sizeHint().width(),correlogramsHalfDuration);
  connect(correlogramsHalfDuration, SIGNAL(returnPressed()),this, SLOT(slotUpdateCorrelogramsHalfDuration()));      

  //Connect the move function of the parameterBar to slotUpdateParameterBar to always correctly show its contents. 
  connect(paramBar, SIGNAL(placeChanged(QDockWindow::Place)), this, SLOT(slotUpdateParameterBar()));
  connect(paramBar, SIGNAL(moved(BarPosition)), this, SLOT(slotUpdateParameterBar()));
}

void KlustersApp::executePreferencesDlg(){
  if(prefDialog == 0L){
    if(mainDock) prefDialog = new PrefDialog(this,doc->nbOfchannels());  // create dialog on demand
    else prefDialog = new PrefDialog(this);  
    // connect to the "settingsChanged" signal    
    connect(prefDialog,SIGNAL(settingsChanged()),this,SLOT(applyPreferences()));
  }
  else{
   //If the dialog has been open the first time before any document has been open
   //the number of channel is zero. If now a document is open, update the number of channels. 
   if(configuration().getNbChannels() == 0 && mainDock){
    prefDialog->resetChannelList(doc->nbOfchannels());
   } 
  }
  
  // update the dialog widgets.
  prefDialog->updateDialog();
  
  if(prefDialog->exec() == QDialog::Accepted){  // execute the dialog
   //if the user did not click the applyButton, save the new settings.
   if(prefDialog->isApplyEnable()){
    prefDialog->updateConfiguration();        // store settings in config object
    applyPreferences();                      // let settings take effect
   }
  }
}

void KlustersApp::applyPreferences() {  
  configuration().write();
  int newNbUndo = configuration().getNbUndo();  
  if(nbUndo != newNbUndo){
   if(mainDock)doc->nbUndoChangedCleaning(newNbUndo); 
   nbUndo = newNbUndo; 
  }

  if(backgroundColor != configuration().getBackgroundColor()){
   backgroundColor = configuration().getBackgroundColor();
   if(mainDock)doc->setBackgroundColor(backgroundColor);
   clusterPalette->changeBackgroundColor(backgroundColor);
  }
  
  if(waveformsGain != configuration().getGain()){
   waveformsGain = configuration().getGain();
   if(mainDock)doc->setGain(waveformsGain); 
  } 

  if(displayTimeInterval != configuration().getTimeInterval()){
   displayTimeInterval = configuration().getTimeInterval();
   if(mainDock)doc->setTimeStepInSecond(displayTimeInterval); 
  }
  
  if(configuration().isCrashRecovery()){
   if(mainDock)doc->updateAutoSavingInterval(configuration().crashRecoveryInterval());
   else doc->setAutoSaving(configuration().crashRecoveryInterval());
  }
  else doc->stopAutoSaving();

  if(configuration().getNbChannels() != 0 && channelPositions != (*configuration().getChannelPositions())){
   QValueList<int>* positions = configuration().getChannelPositions();
   channelPositions.clear();
   for(int i = 0; i < static_cast<int>(positions->size()); ++i)
     channelPositions.append((*positions)[i]);   
   if(mainDock)doc->setChannelPositions(channelPositions);  
  }

  if(reclusteringExecutable != configuration().getReclusteringExecutable())
   reclusteringExecutable = configuration().getReclusteringExecutable();

  if(reclusteringArgs != configuration().getReclusteringArguments())
   reclusteringArgs = configuration().getReclusteringArguments();    
}

void KlustersApp::initializePreferences(){
  nbUndo = configuration().getNbUndo();
  waveformsGain = configuration().getGain();
  displayTimeInterval = configuration().getTimeInterval();
  backgroundColor =  configuration().getBackgroundColor();
  reclusteringExecutable =  configuration().getReclusteringExecutable();
  reclusteringArgs = configuration().getReclusteringArguments(); 
}
  
void KlustersApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  statusBar()->insertItem(i18n("Ready."),1);
}


bool KlustersApp::eventFilter(QObject* object,QEvent* event){
 if(object == paramBar && event->type() == 71){//filter the removal of items from the paramBar
  return true;
 }
 return QWidget::eventFilter(object,event);    // standard event processing
}


void KlustersApp::initClusterPanel()
{
  //Creation of the left panel containing the clusters
  clusterPanel = createDockWidget( "clusterPanel",QImage("classnew") , 0L, i18n("The cluster list"), 0L);
  //Initialisation of the cluster palette containing the cluster list
  clusterPalette = new ClusterPalette(backgroundColor,clusterPanel,statusBar(),"ClusterPalette");
  //Place the clusterPalette frame in the clusterPanel (the view)
  clusterPanel->setWidget(clusterPalette);
}

void KlustersApp::initDisplay(){
   isInit = true; //prevent the spine boxes or the lineedit and the editline to trigger during initialisation
   timeFrameMode->setChecked(false);
   overlayPresentation->setChecked(false);
   meanPresentation->setChecked(false);
   scaleByMax->setChecked(true);
   dimensionX->show();
   dimensionY->show();
   featureXLabel->show();
   spikesTodisplay->show();
   spikesTodisplayLabel->show();
   correlogramsHalfDuration->setText(INITIAL_CORRELOGRAMS_HALF_TIME_FRAME);
   correlogramsHalfDuration->show();
   correlogramsHalfDurationLabel->show();
   binSizeBox->setText(DEFAULT_BIN_SIZE);
   binSizeBox->show();
   binSizeLabel->show();
   shoulderLine->setChecked(true);
   
   //Set the range value of the spine boxes
   dimensionX->setRange(1,doc->nbDimensions());
   dimensionY->setRange(1,doc->nbDimensions());
   maximumTime = doc->maxTime();
   start->setRange(0,maximumTime);
   validator.setRange(0,maximumTime);
   long totalNbSpikes = doc->totalNbOfSpikes();
   spikesTodisplay->setRange(1,totalNbSpikes);
   maximumTime *= 1000;
   correlogramsHalfTimeFrameValidator.setRange(0,static_cast<int>((maximumTime - 1) / 2));
   binSizeValidator.setRange(0,maximumTime);
                     
  //Create the mainDock (first view)
   mainDock = createDockWidget( "1", QPixmap(), 0L, i18n(doc->documentName()), "Overview Display");
   mainDock->setDockWindowTransient(this,true);

   //If the setting dialog exists (has already be open once), enable the settings for the channels.
   if(prefDialog != 0L) prefDialog->enableChannelSettings(true);
   
   //No clusters are shown by default.
   QValueList<int>* clusterList = new QValueList<int>();
    
   //Update the dimension and start spine boxes
   dimensionX->setValue(1);
   dimensionY->setValue(2);

   isInit = false; //now a change in a spine box or the lineedit will trigger an update of the display

   //If 2 documents, opened one after the other, do not have the same number of channels
   //discard any settings concerning the positions of the channels.
   if(configuration().getNbChannels() != 0 && configuration().getNbChannels() != doc->nbOfchannels()) channelPositions.clear();

   KlustersView* view = new KlustersView(*this,*doc,backgroundColor,1,2,clusterList,KlustersView::OVERVIEW,mainDock,0,WDestructiveClose,statusBar(),
                        displayTimeInterval,waveformsGain,channelPositions,false,0,timeWindow,DEFAULT_NB_SPIKES_DISPLAYED,
                        false,false,DEFAULT_BIN_SIZE.toInt(),INITIAL_CORRELOGRAMS_HALF_TIME_FRAME.toInt() * 2 + 1,Data::MAX);

   view->installEventFilter(this);

   //Keep track of the number of displays
   displayCount ++;
   
   //Update the document's list of view
   doc->addView(view);
   
   mainDock->setWidget(view);
   //allow dock on the left side only
   mainDock->setDockSite(KDockWidget::DockLeft);
   setView(mainDock); // central widget in a KDE mainwindow <=> setMainWidget
   setMainDockWidget(mainDock);
   //disable docking abilities of mainDock itself
   mainDock->setEnableDocking(KDockWidget::DockNone);

   //Initialize and dock the clusterpanel
   //Create the cluster list and select the clusters which will be drawn
   clusterPalette->createClusterList(doc);
   clusterPalette->selectItems(*clusterList);

   //allow dock on the right side only
   clusterPanel->setDockSite(KDockWidget::DockRight);
     
   //Dock the clusterPanel on the left
   clusterPanel->setEnableDocking(KDockWidget::DockFullSite);
   clusterPanel->manualDock(mainDock,KDockWidget::DockLeft,0);  // relation target/this (in percent)
   
   //forbit docking abilities of clusterPanel itself
   clusterPanel->setEnableDocking(KDockWidget::DockNone);

   //Update the Time frame and sample related widgets
   spikesTodisplay->setValue(DEFAULT_NB_SPIKES_DISPLAYED);  
   start->setValue(0);
   start->setLineStep(timeWindow);
   duration->setText(INITIAL_WAVEFORM_TIME_WINDOW);
   if(timeFrameMode->isChecked()){
    duration->show();
    durationLabel->show();
    start->show();
    startLabel->show();
    spikesTodisplay->hide();
    spikesTodisplayLabel->hide();
   }
   else{
    duration->hide();
    durationLabel->hide();
    start->hide();
    startLabel->hide();
    spikesTodisplay->show();
    spikesTodisplayLabel->show();
   }
   
   //show all the encapsulated widgets of all controlled dockwidgets
   dockManager->activate();
   
   //Enable some actions now that a document is open (see the klustersui.rc file)
   slotStateChanged("documentState");
}

void KlustersApp::createDisplay(KlustersView::DisplayType type)
{
  if(mainDock){
    KDockWidget* display;
    QString displayName = (doc->documentName()).append(type);
    QString displayType = KlustersView::DisplayTypeNames[type];

    display = createDockWidget(QString(QChar(displayCount)),QImage("classnew") , 0L, i18n(displayName), displayType);

    //Check if the active display contains a ProcessWidget
    bool isProcessWidget = doesActiveDisplayContainProcessWidget();
    
    //Present the clusters of the current display in the new display (if it was not a processing display).
    QValueList<int>* clusterList = new QValueList<int>();
    if(!isProcessWidget){
     const QValueList<int>& currentClusters = activeView()->clusters();

     QValueList<int>::const_iterator shownClustersIterator;
     for(shownClustersIterator = currentClusters.begin(); shownClustersIterator != currentClusters.end(); ++shownClustersIterator )
      clusterList->append(*shownClustersIterator);
    }
    
    //Use the current dimensions for the new display
    int XDimension = dimensionX->value();
    int YDimension = dimensionY->value();

    //Use the same scale, bin size, time frame and shoulder line for the new correlation view
    //as the existing one if it exists.
    Data::ScaleMode scaleMode = Data::MAX;
    int sizeOfBin = DEFAULT_BIN_SIZE.toInt();
    int correlogramTimeWindow = INITIAL_CORRELOGRAMS_HALF_TIME_FRAME.toInt() * 2 + 1;
    bool line = true;
    if(!isProcessWidget && activeView()->containsCorrelationView()){
     scaleMode = activeView()->scaleMode();
     sizeOfBin = binSize;
     correlogramTimeWindow = correlogramTimeFrame;
     line = activeView()->isShoulderLine();
    }

    //Use the same waveform presentation mode for the new display (will be apply only if
    //the new display contains a waveform view). The values of the associated widgets
    //(start and duration or spikesTodisplay) are the same as the activeDisplay ones.
    //The default for a new display is sample mode without mean and overlay.
    bool overLay = false;
    bool mean = false;
    bool inTimeFrameMode = false;
    long startingTime = 0;
    long timeFrameWidth = INITIAL_WAVEFORM_TIME_WINDOW.toLong();
    long nbSpkToDisplay = DEFAULT_NB_SPIKES_DISPLAYED;
    if(!isProcessWidget && activeView()->containsWaveformView()){
     overLay = activeView()->isOverLayPresentation();
     mean = activeView()->isMeanPresentation();
     if(activeView()->isInTimeFrameMode()){
      inTimeFrameMode = true;
      startingTime = startTime;
      timeFrameWidth = timeWindow;
     }
     else nbSpkToDisplay = spikesTodisplay->value();
    }

    KlustersView* view;    
    
    if(!isProcessWidget) view = new KlustersView(*this,*doc,backgroundColor,XDimension,YDimension,clusterList,type,display,0,WDestructiveClose,statusBar(),
                                        displayTimeInterval,waveformsGain,channelPositions,inTimeFrameMode,startingTime,timeFrameWidth,
                                        nbSpkToDisplay,overLay,mean,sizeOfBin,correlogramTimeWindow,scaleMode,line,activeView()->getStartingTime(),activeView()->getDuration(),showHideLabels->isChecked(),activeView()->getUndoList(),activeView()->getRedoList());

    else view = new KlustersView(*this,*doc,backgroundColor,XDimension,YDimension,clusterList,type,display,0,WDestructiveClose,statusBar(),
                                        displayTimeInterval,waveformsGain,channelPositions,inTimeFrameMode,startingTime,timeFrameWidth,
                                        nbSpkToDisplay,overLay,mean,sizeOfBin,correlogramTimeWindow,scaleMode,line,activeView()->getStartingTime(),activeView()->getDuration(),showHideLabels->isChecked());

                                                        
    view->installEventFilter(this);
            
    //Update the document's list of view
    doc->addView(view);
    //install the new view in the display so it can be see in the future tab.
    display->setWidget(view);

    //Temporarily allow addition of a new dockWidget in the center
    mainDock->setDockSite(KDockWidget::DockCenter);
    //Add the new display as a tab and get a new DockWidget, grandParent of the target (mainDock)
    //and the new display.
    KDockWidget* grandParent = display->manualDock(mainDock,KDockWidget::DockCenter);

    //Disconnect the previous connection
    if(tabsParent != NULL) disconnect(tabsParent,0,0,0);

    //The grandParent's widget is the KDockTabGroup regrouping all the tabs
    tabsParent = static_cast<KDockTabGroup*>(grandParent->getWidget());

    //Connect the change tab signal to slotTabChange(QWidget* widget) to trigger updates when
    //the active display change.
    connect(tabsParent, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotTabChange(QWidget*)));

    slotStateChanged("tabState");
    
    //Back to enable dock to the left side only
    mainDock->setDockSite(KDockWidget::DockLeft);

    // forbit docking abilities of display itself
    display->setEnableDocking(KDockWidget::DockNone);
    // allow others to dock to the left side only
    display->setDockSite(KDockWidget::DockLeft);

    //Keep track of the number of displays
    displayCount ++;

    //show all the encapsulated widgets of all controlled dockwidgets
    dockManager->activate();
   }
}

void KlustersApp::openDocumentFile(const KURL& url)
{    
  slotStatusMsg(i18n("Opening file..."));

  filePath = url.path();
  QFileInfo file(filePath);
  if(url.protocol() == "file"){
   if((fileOpenRecent->items().contains(url.prettyURL())) && !file.exists()){
    QString title = "File not found: ";
    title.append(filePath);
    int answer = KMessageBox::questionYesNo(this,i18n("The selected file no longer exists, do you want to remove it from the list?"), i18n(title));
    if(answer == KMessageBox::Yes) fileOpenRecent->removeURL(url);
    else  fileOpenRecent->addURL(url); //hack, unselect the item
    filePath = "";    
    slotStatusMsg(i18n("Ready."));
    return;
   }
  } 
  //Do not handle remote files
  else{
   KMessageBox::sorry(this,i18n("Sorry, Klusters does not handle remote files."),i18n("Remote file handling"));
   fileOpenRecent->removeURL(url);
   filePath = "";
   slotStatusMsg(i18n("Ready."));
   return; 
  }  
  
  //Check if the file exists
  if(!file.exists()){
   KMessageBox::error (this,i18n("The selected file does not exist."), i18n("Error!"));
   return;
   slotStatusMsg(i18n("Ready."));
  } 

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
  //If no document is open already open the document asked.
  if(!mainDock){
    displayCount = 0;
    currentNbUndo = 0;
    currentNbRedo = 0;

    fileOpenRecent->addURL(url);
                                             
    // Open the file (that will also initialize the doc)
    QString errorInformation = "";
    int returnStatus = doc->openDocument(url,errorInformation);        
    if(returnStatus == KlustersDoc::INCORRECT_FILE)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("The selected file is invalid, it has to be of the form baseName.clu.n or baseName.fet.n or baseName.par.n"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();

      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    if(returnStatus == KlustersDoc::DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the cluster file (base.clu.n)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }

    if(returnStatus == KlustersDoc::SPK_DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the spike file (base.spk.n)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    if(returnStatus == KlustersDoc::FET_DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the feature file (base.fet.n)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    if(returnStatus == KlustersDoc::PAR_DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the general parameter file (base.par)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    if(returnStatus == KlustersDoc::PARX_DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the specific parameter file (base.par.n)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    
    if(returnStatus == KlustersDoc::PARXML_DOWNLOAD_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not get the parameter file (base.xml)"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    
    if(returnStatus == KlustersDoc::OPEN_ERROR)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,i18n("Could not open the files"), i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();      
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }
    if(returnStatus == KlustersDoc::INCORRECT_CONTENT)
    {
      QApplication::restoreOverrideCursor();
      KMessageBox::error (this,errorInformation, i18n("Error!"));
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      //close the document
      doc->closeDocument();
      resetState();      
      QApplication::restoreOverrideCursor();
      slotStatusMsg(i18n("Ready."));
      return;
    }   

    //Save the recent file list
    fileOpenRecent->saveEntries(config);
    
    setCaption(doc->documentName());
    initDisplay();
    
    //A traceView is possible only if the variables it needs are available (provided in the new parameter file) and
    //the .dat file exists.
    if(doc->areTraceDataAvailable() && doc->isTraceViewVariablesAvailable()) slotStateChanged("traceDisplayState"); 
    
    QApplication::restoreOverrideCursor(); 
  }
  // check, if this document is already open. If yes, do not do anything
  else{
   QString docName = doc->documentName();
   QStringList fileParts = QStringList::split(".", url.fileName());
   QString electrodNb;
   if(fileParts.count() < 3) electrodNb = "";
   else electrodNb = fileParts[fileParts.count()-1];;
   QString baseName = fileParts[0];
   for(uint i = 1;i < fileParts.count()-2; ++i) baseName += "." + fileParts[i];
   QString name = url.directory() + "/" + baseName + "-" + electrodNb;
   
   if(docName == name){
    fileOpenRecent->addURL(url); //hack, unselect the item
    QApplication::restoreOverrideCursor();
    slotStatusMsg(i18n("Ready."));
    return; 
   }
   //If the document asked is not the already open. Open a new instance of the application with it.
   else{
    fileOpenRecent->addURL(url);
    filePath = doc->url().path();
     
    KProcess* childproc = new KProcess();
    childproc->setUseShell(true);

    QString command = "`which klusters` ";
    command.append(url.path());
    *childproc << command;
    childproc->start(KProcess::DontCare);
    
    childproc->detach();
    delete childproc;

    QApplication::restoreOverrideCursor();  
   } 
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::importDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Importing file..."));

  //If no document is open already open the document ask.
  if(!mainDock){
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // Open the file (that will also initialize the doc)
    if(!doc->importDocument(url))
    {
      KMessageBox::error (this,i18n("Could not import document !"), i18n("Error !"));
      //close the document
      doc->closeDocument();
      QApplication::restoreOverrideCursor();
      return;
    }
	  fileOpenRecent->addURL(url);
    initDisplay();
    QApplication::restoreOverrideCursor();
  }
  // check, if this document is already open. If yes, do not do anything
  else if(doc->url()==url){
    QApplication::restoreOverrideCursor();
    return;
  }

  //If the document asked is not the already open. Open a new instance of the application with it.
  //Only one document at the time is allowed.
   else{
     //TODO
     QApplication::restoreOverrideCursor();
  }

  slotStatusMsg(i18n("Ready."));
}

bool KlustersApp::doesActiveDisplayContainProcessWidget(){
 KDockWidget* current;

 //Get the active tab
 if(tabsParent) current = static_cast<KDockWidget*>(tabsParent->currentPage());
 //or the active window if there is only one display (which can only be the mainDock)
 else current = mainDock;

 return (current->getWidget())->isA("ProcessWidget"); 
}

KlustersView* KlustersApp::activeView(){
  KDockWidget* current;
  
  //Get the active tab
  if(tabsParent) current = static_cast<KDockWidget*>(tabsParent->currentPage());
  //or the active window if there is only one display (which can only be the mainDock)
  else current = mainDock;
 
  return static_cast<KlustersView*>(current->getWidget());
}

void KlustersApp::saveProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored

  //Save the recent file list
  fileOpenRecent->saveEntries(config);
  config->writePathEntry("openFile",filePath);
}

void KlustersApp::readProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'

  // initialize the recent file list
  fileOpenRecent->loadEntries(config);
  filePath = config->readPathEntry("openFile");
  KURL url;
  url.setPath(filePath);
  openDocumentFile(url);    
}

//TO implement , see documentation
bool KlustersApp::queryClose()
{  
  //Save the recent file list
  fileOpenRecent->saveEntries(config);
    
  //call when the kDockMainWindow will be close
  //implement to ask the user to save if necessary before closing
  if(doc == 0) return true;
  else{
    if(doc->canCloseView()){
     //Set a waiting cursor in case there is some delay to the ending of the running threads.
     QApplication::restoreOverrideCursor();
     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     if(doc->canCloseDocument(this,"queryClose")){     
      if(processWidget != 0L && processWidget->isRunning()){
       processWidget->killJob();
       processKilled = true;
      } 
      if(!(processFinished && processOutputsFinished)){        
       QTimer::singleShot(2000,this, SLOT(close()));
       return false;
      }
      else if(processWidget != 0L){
       delete processWidget;
       processWidget = 0L ;
      }
      doc->closeDocument();
      QApplication::restoreOverrideCursor();
      return true;
     }
     else return false;
    }
    else return false;
  }
}

//TO implement , see documentation
bool KlustersApp::queryExit()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  //If the saveThread has not finish, wait until id done
  while(!saveThread->wait()){cout<<"in queryExit"<<endl;};
  QApplication::restoreOverrideCursor();
  
  return true;
}

void KlustersApp::customEvent (QCustomEvent* event){
  //Event sent by the SaveThread
  if(event->type() == QEvent::User + 100){
    slotStatusMsg(i18n("Save file done."));
    SaveThread::SaveDoneEvent* saveEvent = (SaveThread::SaveDoneEvent*) event;
    if(saveEvent->isSaveOk()){
     //The cluster data have been saved to a local temporary file if the original file was a remote one
     //(otherwise the so call temporary file is the orignal one).
     //If the requested save location is remote, the temporary file needs now
     //to be uploaded. The save process is made in a thread and it seams that
     //the KDE upload can not be call asynchronously, so the upload is call after the end of the thread.
     if(!KIO::NetAccess::upload(saveEvent->temporaryFile(),doc->url())){
       KMessageBox::error (0,i18n("Could not save the current document !"), i18n("I/O Error !"));
     }
     if(saveEvent->isItSaveAs()){
   	   fileOpenRecent->addURL(doc->url());
       setCaption(doc->documentName());
     }
    }
    else
       KMessageBox::error (0,i18n("Could not save the current document !"), i18n("I/O Error !"));

    slotStatusMsg(i18n("Ready."));
    slotStateChanged("SavingDoneState");
  }
  //Event sent by klusterDoc to advice that there is some threads still running.
  if(event->type() == QEvent::User + 400){
   KlustersDoc::CloseDocumentEvent* closeEvent = (KlustersDoc::CloseDocumentEvent*) event;
   QString origin = closeEvent->methodOfOrigin();

   //Try to close the document again
   if(doc->canCloseDocument(this,origin)){
    doc->closeDocument();

    //Execute what is need it after the close depending on the callingMethod
    if(origin == "queryClose"){
       QApplication::restoreOverrideCursor();
       close();
    }
    else if(origin == "fileClose" || origin == "displayClose"){
     slotFileClose();
     QApplication::restoreOverrideCursor();
     slotStatusMsg(i18n("Ready."));
    }
   }
  }

  //Event sent by the parameterBar to advice that it has been resized.
  if(event->type() == QEvent::User + 1000){
    slotUpdateParameterBar();
  }
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void KlustersApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));

  KURL url=KFileDialog::getOpenURL(QString::null,
      i18n("*.fet.*|Feature File (*.fet.n)\n*.clu.*|Cluster File (*.clu.n)\n*.spk.*|Spike File (*.spk.n)\n*.par.*|Specific Parameter File (*.par.n)\n*|All files"), this, i18n("Open File..."));
  if(!url.isEmpty())
  {
    openDocumentFile(url);
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotFileClose(){
  if(doc != 0){
    while(!saveThread->wait()){};
    if(doc->canCloseView()){
     QApplication::restoreOverrideCursor();
     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     //try to close the document
     if(doc->canCloseDocument(this,"fileClose")){
      //remove all the displays
      if(tabsParent){
        //Remove the display from the group of tabs
        while(true){
          int nbOfTabs = tabsParent->count();
          KDockWidget* current = static_cast<KDockWidget*>(tabsParent->page(0));
          if(current == mainDock){
            current = static_cast<KDockWidget*>(tabsParent->page(1));
          }
        
          if((current->getWidget())->isA("KlustersView")){
           tabsParent->removePage(current);
           delete current;
          } 
          else{//give the time to the process to finish.          
           if(processWidget->isRunning()){
            processWidget->killJob();
            processKilled = true;
           } 
           if(processFinished && processOutputsFinished){
            tabsParent->removePage(current); 
            delete current;
            processWidget = 0L;
           }
           else{
            QTimer::singleShot(2000,this, SLOT(slotFileClose()));
            return;
           }
          }

          if(nbOfTabs == 2) break; //the reminding one is the mainDock
        }

        tabsParent = 0L;
       }
       //reset the cluster palette and hide the cluster panel
       clusterPalette->reset();
       clusterPanel->hide();
       clusterPanel->undock();
       //The last display is the mainDock
       if((mainDock->getWidget())->isA("KlustersView")) delete mainDock;
       else{
        if(processWidget->isRunning()){
         processWidget->killJob();
         processKilled = true;
        } 
        if(processFinished && processOutputsFinished){
         delete mainDock;
         processWidget = 0L;
        }
        else{
         mainDock->hide();
         QTimer::singleShot(2000,this, SLOT(slotFileClose()));
         return;
        }
       }
       mainDock = 0L;
       doc->closeDocument();
       resetState();
       QApplication::restoreOverrideCursor();
      }
    }
  }
}

void KlustersApp::slotFileImport(){
    slotStatusMsg(i18n("Importing file..."));

  KURL url=KFileDialog::getOpenURL(QString::null,
      i18n("*|All files"), this, i18n("Import File..."));
  if(!url.isEmpty())
  {
    importDocumentFile(url);
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotFileOpenRecent(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  openDocumentFile(url);

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));

  slotStateChanged("SavingState");
  saveThread->save(doc->url(),doc,false);
}

void KlustersApp::slotFileRenumberAndSave(){
  slotStatusMsg(i18n("Renumbering and saving..."));
  slotStateChanged("SavingState");
  doc->renumberClusters();
  slotFileSave();
}

void KlustersApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."));

  KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*|All files"), this, i18n("Save as..."));
  if(!url.isEmpty()){
    slotStateChanged("SavingState");
    saveThread->save(url,doc,true);
  }
}

void KlustersApp::slotDisplayClose()
{
  KDockWidget* current;

  slotStatusMsg(i18n("Closing display..."));

  //Get the active tab
  if(tabsParent){
     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     int nbOfTabs = tabsParent->count();
     current = static_cast<KDockWidget*>(tabsParent->currentPage());
     //If the active display is the mainDock, assign the mainDock status to an other display (take the first one available)
     if(current == mainDock){
       if(tabsParent->currentPageIndex() == 0){
         mainDock = static_cast<KDockWidget*>(tabsParent->page(1));
         setMainDockWidget(mainDock);
       }
       else setMainDockWidget(static_cast<KDockWidget*>(tabsParent->page(0)));
     }
     //Remove the display from the group of tabs
     tabsParent->removePage(current);
     displayCount --;
     //If there is only one display left, the group of tabs will be deleted so we set tabsParent to null
     if(nbOfTabs == 2){
      slotStateChanged("noTabState");
      tabsParent = 0L; 
     } 

     //Remove the view from the document list if need it
     if((current->getWidget())->isA("KlustersView")){
      KlustersView* view = dynamic_cast<KlustersView*>(current->getWidget());
      doc->removeView(view);

      //Update the Displays menu if the current display is a grouping assistant.
      if(view->containsErrorMatrixView()){
       slotStateChanged("groupingAssistantDisplayNotExists");
       errorMatrixExists = false;
      }

      //Delete the view
      delete current;
     }
     else{
      if(processFinished && processOutputsFinished){
       delete current;
       processWidget = 0L; 
      }
      else{
       processOutputDock = current;
       processWidget->hideWidget();  
      }  
     }
     
    QApplication::restoreOverrideCursor();
  }
  //or the active window if there is only one display (which can only be the mainDock)
  else{
   //If a save is already in process, wait until it is done
   if(saveThread->running()){
    QApplication::restoreOverrideCursor(); 
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    while(!saveThread->wait()){};
    //reset the cluster palette and hide the cluster panel
    clusterPalette->reset();
    clusterPanel->hide();
    clusterPanel->undock();
    //try to close the document
    if(doc->canCloseDocument(this,"displayClose")){
     doc->closeDocument();
     //Delete the view
     if((mainDock->getWidget())->isA("KlustersView")){
      if(processWidget != 0L){
       delete processWidget;
       processWidget = 0L; 
      }
      delete mainDock; 
     } 
     else{
      if(processWidget->isRunning()){
       processWidget->killJob();
       processKilled = true;
      } 
      if(processFinished && processOutputsFinished){
       delete mainDock;
       processWidget = 0L;
      }
      else{
       mainDock->hide();
       processWidget->hideWidget();
       QTimer::singleShot(2000,this, SLOT(slotDisplayClose()));
       return;
      }
     }
     mainDock = 0L;
     QApplication::restoreOverrideCursor();
    }
   }
   //Ask the user if wants to save the document if need it before closing and confirm the closing
   else if(doc->canCloseView()){
    QApplication::restoreOverrideCursor(); 
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    //reset the cluster palette and hide the cluster panel
    clusterPalette->reset();
    clusterPanel->hide();
    clusterPanel->undock();
    //try to close the document
    if(doc->canCloseDocument(this,"displayClose")){
     doc->closeDocument();
     //Delete the view
     if((mainDock->getWidget())->isA("KlustersView")){
      if(processWidget != 0L){
       delete processWidget;
       processWidget = 0L;
      }
      delete mainDock;
     } 
     else{
      if(processWidget->isRunning()){
       processWidget->killJob();
       processKilled = true;
      } 
      if(processFinished && processOutputsFinished){
       delete mainDock;
       processWidget = 0L;
      }
      else{
       mainDock->hide();
       processWidget->hideWidget();
       QTimer::singleShot(2000,this, SLOT(slotDisplayClose()));
       return; 
      }        
     }
     mainDock = 0L;
     QApplication::restoreOverrideCursor();
    }
    resetState();
   }
  }
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  printer->setOrientation(KPrinter::Landscape);
  printer->setColorMode(KPrinter::Color);
  printer->addDialogPage(new printDialogPage(this));
  
  if (printer->setup(this))
  {
   //retrieve the backgroundColor setting from KPrinter object, //1 <=> white background
   int backgroundColor = printer->option("kde-klusters-backgroundColor").toInt();
   if(!doesActiveDisplayContainProcessWidget()){
    KlustersView* view = activeView();
    if(backgroundColor == 1) view->print(printer,filePath,true);
    else view->print(printer,filePath,false);     
   }
   else{
    KDockWidget* dock = static_cast<KDockWidget*>(tabsParent->currentPage());
    ProcessWidget* view = static_cast<ProcessWidget*>(dock->getWidget());
    view->print(printer,filePath);
   }
  }

  slotStatusMsg(i18n("Ready.")); 
}

void KlustersApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
/*
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
  KMainWindow* w;
  if(memberList) //List of members of KMainWindow class
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
        break;
    }
  }*/

  close();

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotUndo()
{  
  slotStatusMsg(i18n("Reverting last action..."));

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  doc->undo();
  QApplication::restoreOverrideCursor();
  
  //Update the browsing possibility of the traceView
  KlustersView* view = activeView();
  if(view->containsTraceView() && view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
  else  slotStateChanged("noTraceViewBrowsingState");  

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotRedo()
{
  slotStatusMsg(i18n("Reverting last undo action..."));
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  doc->redo();
  QApplication::restoreOverrideCursor();

  //Update the browsing possibility of the traceView
  KlustersView* view = activeView();
  if(view->containsTraceView() && view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
  else  slotStateChanged("noTraceViewBrowsingState");
  
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotUpdateUndoNb(int undoNb){
  currentNbUndo = undoNb;
  if(currentNbUndo > 0) slotStateChanged("undoState");
  else slotStateChanged("emptyUndoState");
}

void KlustersApp::slotUpdateRedoNb(int redoNb){
  currentNbRedo = redoNb;
  if(currentNbRedo == 0) slotStateChanged("emptyRedoState");
}

void KlustersApp::slotViewMainToolBar()
{
  slotStatusMsg(i18n("Toggle the main toolbar..."));

  // turn Toolbar on or off
  if(!viewMainToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotViewActionBar(){
  slotStatusMsg(i18n("Toggle the action..."));
  
  // turn Toolbar on or off
  if(!viewActionBar->isChecked())
  {
    toolBar("actionBar")->hide();
  }
  else
  {
    toolBar("actionBar")->show();
  }
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotViewParameterBar(){
  slotStatusMsg(i18n("Toggle the parameters..."));

  // turn Toolbar on or off
  if(!viewParameterBar->isChecked())
  {
    paramBar->hide();
  }
  else
  {
    paramBar->show();
    slotUpdateParameterBar();
  }
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggle the tools..."));

  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
  {
    toolBar("toolBar")->hide();
  }
  else
  {
    toolBar("toolBar")->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if(!viewStatusBar->isChecked())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotViewClusterInfo(){
	slotStatusMsg(i18n("Toggle the presentation of the cluster information in the cluster palette..."));

  // turn the user cluster information on or off
	if(!viewClusterInfo->isChecked())
	{
		clusterPalette->hideUserClusterInformation();
	}
	else
	{
		doc->showUserClusterInformation();		
	}

	slotStatusMsg(i18n("Ready."));

}


void KlustersApp::slotWindowNewClusterDisplay()
{
  slotStatusMsg(i18n("Opening a new cluster view..."));

  createDisplay(KlustersView::CLUSTERS);
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotWindowNewWaveformDisplay()
{
  slotStatusMsg(i18n("Opening a new waveform view..."));

  createDisplay(KlustersView::WAVEFORMS);
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotWindowNewCrosscorrelationDisplay()
{
  slotStatusMsg(i18n("Opening a new correlation view..."));

  createDisplay(KlustersView::CORRELATIONS);
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotWindowNewOverViewDisplay()
{
  slotStatusMsg(i18n("Opening a new over view..."));

  createDisplay(KlustersView::OVERVIEW);
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotWindowNewGroupingAssistantDisplay()
{
  slotStatusMsg(i18n("Opening a new grouping assistant view..."));

  createDisplay(KlustersView::GROUPING_ASSISTANT_VIEW);
  slotStateChanged("groupingAssistantDisplayExists");
  errorMatrixExists = true;
     
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotNewTraceDisplay(){
  slotStatusMsg(i18n("Opening a new grouping assistant view..."));

  createDisplay(KlustersView::TRACES);
  
  slotStatusMsg(i18n("Ready."));
}


void KlustersApp::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text,1);
}

void KlustersApp::slotKeyBindings(){
  slotStatusMsg(i18n("Changing the key bindings..."));
  KKeyDialog::configure(actionCollection());
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::viewMenuAboutToShow()
{
/*  viewMenu->popupMenu()->clear();
  viewMenu->insert(newClusterView);
  viewMenu->insert(newWaveformView);
  viewMenu->insert(newCrosscorrelationView);
  viewMenu->insert(newOverView);  */

  /*if ( pWorkspace->windowList().isEmpty() ){
    windowNewWindow->setEnabled(false);
  }
  else{
    windowNewWindow->setEnabled(true);
  }
  viewMenu->popupMenu()->insertSeparator();

  QWidgetList windows = pWorkspace->windowList();
  for ( int i = 0; i < int(windows.count()); ++i )
  {
    int id = viewMenu->popupMenu()->insertItem(QString("&%1 ").arg(i+1)+windows.at(i)->caption(), this, SLOT( viewMenuActivated( int ) ) );
    viewMenu->popupMenu()->setItemParameter( id, i );
    viewMenu->popupMenu()->setItemChecked( id, pWorkspace->activeWindow() == windows.at(i) );
  }*/
}

void KlustersApp::viewMenuActivated( int id )
{
 /* QWidget* w = pWorkspace->windowList().at( id );
  if ( w )
    w->setFocus();*/
}


/*Slots for the actions menu*/
/**Creates a single cluster by selecting an area*/
void KlustersApp::slotSingleNew(){
   slotStatusMsg(i18n("Create new cluster..."));

   //If we are in delay mode, update the display, if need it, before triggering the tool change
   if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
    clusterPalette->updateClusters();
   }

   KlustersView* view = activeView();
   view->setMode(ViewWidget::NEW_CLUSTER);
   slotStatusMsg(i18n("Ready."));
}
/**Creates a multiple clusters by selecting an area*/
void KlustersApp::slotMultipleNew(){
   slotStatusMsg(i18n("Split clusters..."));

   //If we are in delay mode, update the display, if need it, before triggering the tool change
   if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
    clusterPalette->updateClusters();
   }

   KlustersView* view = activeView();
   view->setMode(ViewWidget::NEW_CLUSTERS);
   slotStatusMsg(i18n("Ready."));
}
/**Deletes spikes from a cluster and move them to the cluster (number 1) containing the poorly isolated cells*/
void KlustersApp::slotDeleteNoise(){
   slotStatusMsg(i18n("Delete noise..."));

   //If we are in delay mode, update the display, if need it, before triggering the tool change
   if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
    clusterPalette->updateClusters();
   }

   KlustersView* view = activeView();
   view->setMode(ViewWidget::DELETE_NOISE);
   slotStatusMsg(i18n("Ready."));
}
/**Deletes spikes from a cluster and move them to the cluster (number 0) containing the artifacts*/
void KlustersApp::slotDeleteArtefact(){
   slotStatusMsg(i18n("Delete artifact..."));

   //If we are in delay mode, update the display, if need it, before triggering the tool change
   if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
    clusterPalette->updateClusters();
   }

   KlustersView* view = activeView();
   view->setMode(ViewWidget::DELETE_ARTEFACT);
   slotStatusMsg(i18n("Ready."));
}
/**Zooms*/
void KlustersApp::slotZoom(){
   slotStatusMsg(i18n("Zooming..."));

   //If we are in delay mode, update the display, if need it, before triggering the tool change
   if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
    clusterPalette->updateClusters();
   }

   KlustersView* view = activeView();
   view->setMode(BaseFrame::ZOOM);
   slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotSelectTime(){
 slotStatusMsg(i18n("Selecting time..."));

 //If we are in delay mode, update the display, if need it, before triggering the tool change
 if(dynamic_cast<KRadioAction*>(actionCollection()->action("delay_selection"))->isChecked()){
  clusterPalette->updateClusters();
 }
 
 KlustersView* view = activeView();
 view->setMode(ViewWidget::SELECT_TIME);

 slotStatusMsg(i18n("Ready."));  
}


void KlustersApp::slotSingleColorUpdate(int clusterId){
 //Trigger the action only if the active display does not contain a ProcessWidget
  if(!doesActiveDisplayContainProcessWidget()){
    KlustersView* view = activeView();
    doc->singleColorUpdate(clusterId,*view);
  }
}

void KlustersApp::slotUpdateShownClusters(QValueList<int> selectedClusters){  
 //Trigger ths action only if the active display does not contain a ProcessWidget
 if(!doesActiveDisplayContainProcessWidget()){
   
  //Update the browsing possibility of the traceView
  if(activeView()->containsTraceView() && selectedClusters.size() != 0) slotStateChanged("traceViewBrowsingState");
  else  slotStateChanged("noTraceViewBrowsingState");
  
  KlustersView* view = activeView();
  doc->shownClustersUpdate(selectedClusters,*view);
 } 
}

void KlustersApp::slotGroupClusters(QValueList<int> selectedClusters){
  slotStatusMsg(i18n("Grouping clusters..."));
  KlustersView* view = activeView();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  doc->groupClusters(selectedClusters,*view);
  QApplication::restoreOverrideCursor();
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotMoveClustersToNoise(QValueList<int> selectedClusters){
  slotStatusMsg(i18n("Delete &noisy cluster(s)..."));
  KlustersView* view = activeView();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  doc->deleteClusters(selectedClusters,*view,1);
  
  //Update the browsing possibility of the traceView
  if(view->containsTraceView() && view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
  else  slotStateChanged("noTraceViewBrowsingState");
  
  QApplication::restoreOverrideCursor();
  slotStatusMsg(i18n("Ready."));
}

void KlustersApp::slotMoveClustersToArtefact(QValueList<int> selectedClusters){
  slotStatusMsg(i18n("Delete &artifact cluster(s)..."));
  KlustersView* view = activeView();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  doc->deleteClusters(selectedClusters,*view,0);
  
  //Update the browsing possibility of the traceView
  if(view->containsTraceView() && view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
  else  slotStateChanged("noTraceViewBrowsingState");
  
  QApplication::restoreOverrideCursor();
  slotStatusMsg(i18n("Ready."));
}


void KlustersApp::slotImmediateSelection(){
  //Disable the update action (see the klustersui.rc file)
  slotStateChanged("immediateSelectionState");
  clusterPalette->setImmediateMode();
  clusterPalette->updateClusters();
}
void KlustersApp::slotDelaySelection(){
  //Enable the update action (see the klustersui.rc file)
  slotStateChanged("delaySelectionState");
  clusterPalette->setDelayMode();
}

void KlustersApp::slotTabChange(QWidget* widget){
 KDockWidget* display = dynamic_cast<KDockWidget*>(widget);
 if((display->getWidget())->isA("KlustersView")){
   KlustersView* activeView = dynamic_cast<KlustersView*>(display->getWidget());

   //Update the content of the view contains in active display.
   activeView->updateViewContents();

   isInit = true; //prevent the spine boxes or the lineedit and the editline to trigger during initialisation
  
   //The select time tool is useful only if both a clusterView and a traceView are present
   if(activeView->containsClusterView() && activeView->containsTraceView()) slotStateChanged("traceViewClusterViewState");
   
   if(activeView->containsClusterView()){
    //Update the dimension spine boxes
    int x =  activeView->abscissaDimension();
    int y =  activeView->ordinateDimension();
    dimensionX->setValue(x);
    dimensionY->setValue(y);
    slotStateChanged("clusterViewState");
    dimensionX->show();
    dimensionY->show();
    featureXLabel->show();
   }
   else{
    slotStateChanged("noClusterViewState");
    dimensionX->hide();
    dimensionY->hide();
    featureXLabel->hide();
   }

   if(activeView->containsWaveformView()){
     slotStateChanged("waveformsViewState");
     if(activeView->isOverLayPresentation()) overlayPresentation->setChecked(true);
     else overlayPresentation->setChecked(false);
     if(activeView->isMeanPresentation()) meanPresentation->setChecked(true);
     else meanPresentation->setChecked(false);

     if(activeView->isInTimeFrameMode()){
      timeFrameMode->setChecked(true);
      timeWindow = activeView->timeFrameWidth();
      startTime =  activeView->timeFrameStart();
      start->setValue(startTime);
      start->setLineStep(timeWindow);
      duration->setText(QString("%1").arg(timeWindow));
      duration->show();
      durationLabel->show();
      start->show();
      startLabel->show();
      spikesTodisplay->hide();
      spikesTodisplayLabel->hide();
     }
     else{
      timeFrameMode->setChecked(false);
      duration->hide();
      durationLabel->hide();
      start->hide();
      startLabel->hide();
      spikesTodisplay->setValue(activeView->displayedNbSpikes());
      spikesTodisplay->show();
      spikesTodisplayLabel->show();
     }
   }
   else{
     timeFrameMode->setChecked(false);
     overlayPresentation->setChecked(false);
     meanPresentation->setChecked(false);
     slotStateChanged("noWaveformsViewState");
     duration->hide();
     durationLabel->hide();
     start->hide();
     startLabel->hide();
     spikesTodisplay->hide();
     spikesTodisplayLabel->hide();
   }

   if(activeView->containsCorrelationView()){
    slotStateChanged("correlationViewState");
    Data::ScaleMode correlationScale = activeView->scaleMode();
    switch(correlationScale){
     case Data::RAW :
      noScale->setChecked(true);
      break;
     case Data::MAX :
      scaleByMax->setChecked(true);
      break;
     case Data::SHOULDER :
       scaleByShouler->setChecked(true);
       break;
    }

    //Update the lineEdit
    correlogramTimeFrame = activeView->correlationTimeFrameWidth();
    binSize = activeView->sizeOfBin();
    correlogramsHalfDuration->setText(QString("%1").arg(correlogramTimeFrame / 2));
    binSizeBox->setText(QString("%1").arg(binSize));
    correlogramsHalfDuration->show();
    correlogramsHalfDurationLabel->show();
    binSizeBox->show();
    binSizeLabel->show();
    //Update the shoulder line menu entry
    shoulderLine->setChecked(activeView->isShoulderLine());
   }
   else{
    slotStateChanged("noCorrelationViewState");
    correlogramsHalfDuration->hide();
    correlogramsHalfDurationLabel->hide();
    binSizeBox->hide();
    binSizeLabel->hide();
   }

   if(activeView->containsErrorMatrixView())
     slotStateChanged("errorMatrixViewState");
   else slotStateChanged("noErrorMatrixViewState");

   if(activeView->containsTraceView()){
    showHideLabels->setChecked(activeView->getLabelStatus());
    activeView->updateClustersProvider();
    slotStateChanged("traceViewState");
    
    //Update the browsing possibility of the traceView
    if(activeView->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
    else  slotStateChanged("noTraceViewBrowsingState");        
   }
   else slotStateChanged("noTraceViewState");
   
   isInit = false; //now a change in a spine box  or the lineedit
   //will trigger an update of the display
   
   //Update the cluster palette
   clusterPalette->selectItems(activeView->clusters());


   //Check if a reclustering process is working in order to correctly set up the menus
   if(processFinished){
    slotStateChanged("noReclusterState");
    updateUndoRedoDisplay(); 
   } 
   else slotStateChanged("reclusterState");
  }
  else{// a ProcessWidget
   dimensionX->hide();
   dimensionY->hide();
   featureXLabel->hide();
   timeFrameMode->setChecked(false);
   overlayPresentation->setChecked(false);
   meanPresentation->setChecked(false);
   duration->hide();
   durationLabel->hide();
   start->hide();
   startLabel->hide();
   spikesTodisplay->hide();
   spikesTodisplayLabel->hide();
   correlogramsHalfDuration->hide();
   correlogramsHalfDurationLabel->hide();
   binSizeBox->hide();
   binSizeLabel->hide();

   //Update the palette of clusters
   if(!processFinished) clusterPalette->selectItems(clustersToRecluster);
   else{
    QValueList<int> emptyList;
    clusterPalette->selectItems(emptyList);
   } 

   slotStateChanged("reclusterViewState");
  }
}

void KlustersApp::slotUpdateDimensionX(int dimensionXValue){
  if(!isInit)updateDimensions(dimensionXValue,dimensionY->value());
}

void KlustersApp::slotUpdateDimensionY(int dimensionYValue){
  if(!isInit)updateDimensions(dimensionX->value(),dimensionYValue);
}

void KlustersApp::updateDimensions(int dimensionXValue, int dimensionYValue){
  activeView()->updateDimensions(dimensionXValue,dimensionYValue);
  activeView()->showAllWidgets();
}

void KlustersApp::slotUpdateDuration(){
  if(!isInit){ 
   timeWindow =  (duration->displayText()).toLong();
   start->setLineStep(timeWindow);
   activeView()->updateTimeFrame(startTime,timeWindow);
  }
}

void KlustersApp::slotTimeFrameMode(){
 if(!isInit){
  if(timeFrameMode->isChecked()){
   timeWindow = activeView()->timeFrameWidth();
   startTime =  activeView()->timeFrameStart();
   start->setValue(startTime);
   start->setLineStep(timeWindow);
   duration->setText(QString("%1").arg(timeWindow));
   duration->show();
   durationLabel->show();
   start->show();
   startLabel->show();
   spikesTodisplay->hide();
   spikesTodisplayLabel->hide();
   activeView()->setTimeFrameMode();
  }
  else{
   spikesTodisplay->setValue(activeView()->displayedNbSpikes()); 
   spikesTodisplay->show();
   spikesTodisplayLabel->show();
   duration->hide();
   durationLabel->hide();
   start->hide();
   startLabel->hide();
   activeView()->setSampleMode();
  }
 }
}

void KlustersApp::slotClusterInformationModified(){
	doc->clusterInformationModified();
}

void KlustersApp::resetState(){
  isInit = true; //prevent the spine boxes or the lineedit and the editline to trigger during initialisation
  timeFrameMode->setChecked(false);
  duration->hide();
  durationLabel->hide();
  start->hide();
  startLabel->hide();
  dimensionX->hide();
  dimensionY->hide();
  featureXLabel->hide();
  spikesTodisplay->hide();
  spikesTodisplayLabel->hide();
  correlogramsHalfDuration->hide();
  correlogramsHalfDurationLabel->hide();
  binSizeBox->hide();
  binSizeLabel->hide();
  shoulderLine->setChecked(true);
  binSize = DEFAULT_BIN_SIZE.toInt();
  correlogramTimeFrame = INITIAL_CORRELOGRAMS_HALF_TIME_FRAME.toInt() * 2 + 1;
  startTime = 0;
  timeWindow = INITIAL_WAVEFORM_TIME_WINDOW.toLong();
  showHideLabels->setChecked(false);
  
  isInit = false; //now a change in a spine box  or the lineedit
  //will trigger an update of the view contain in the display.

  processOutputDock = 0L;
  displayCount = 0;
  errorMatrixExists = false;
  filePath = "";
  
  //Disable some actions when no document is open (see the klustersui.rc file)
  slotStateChanged("initState");
  //A traceView is possible only if the variables it needs are available (provided in the new parameter file) and
  //the .dat file exists. Therefore disable the menu entry by default.
  slotStateChanged("noTraceDisplayState"); 
  
  setCaption("");

  //If the a setting dialog exists (has already be open once), disable the settings for the channels.
  if(prefDialog != 0L) prefDialog->enableChannelSettings(false);
}

void KlustersApp::slotUpdateCorrelogramsHalfDuration(){
  if(!isInit){
   int halfTimeFrame = (correlogramsHalfDuration->displayText()).toInt();
   if(halfTimeFrame > (maximumTime - binSize) / 2){
    correlogramsHalfDuration->setText(QString("%1").arg((correlogramTimeFrame - binSize) / 2));
    return;
   }

   float x = (2*static_cast<float>(halfTimeFrame)
     /static_cast<float>(binSize)-1) * 0.5;
   int k = static_cast<int>(x + 0.5);

   correlogramTimeFrame = (2*k+1)*binSize;
   if(k != x){
     correlogramsHalfDuration->setText(QString("%1").arg(static_cast<int>(correlogramTimeFrame / 2)));
   } 
   activeView()->updateBinSizeAndTimeFrame(binSize,correlogramTimeFrame);
  }
}

void KlustersApp::slotUpdateBinSize(){
  if(!isInit){    
   binSize = (binSizeBox->displayText()).toInt();
   activeView()->updateBinSizeAndTimeFrame(binSize,correlogramTimeFrame);
  }
}

void KlustersApp::slotUpdateParameterBar(){  
  duration->hide();
  durationLabel->hide();
  start->hide();
  startLabel->hide();
  dimensionX->hide();
  dimensionY->hide();
  featureXLabel->hide();
  spikesTodisplay->hide();
  spikesTodisplayLabel->hide();
  correlogramsHalfDuration->hide();
  correlogramsHalfDurationLabel->hide();
  binSizeBox->hide();
  binSizeLabel->hide();

  if(mainDock != 0L){
   KlustersView* currentView = activeView();

   if(currentView->containsClusterView()){
    dimensionX->show();
    dimensionY->show();
    featureXLabel->show();
   }

   if(currentView->containsWaveformView()){
     if(currentView->isInTimeFrameMode()){
      duration->show();
      durationLabel->show();
      start->show();
      startLabel->show();
     }
     else{
      spikesTodisplay->show();
      spikesTodisplayLabel->show();
     }
   }

   if(currentView->containsCorrelationView()){
    correlogramsHalfDuration->show();
    correlogramsHalfDurationLabel->show();
    binSizeBox->show();
    binSizeLabel->show();
   }
  }
}

void KlustersApp::slotUpdateErrorMatrix(){
  KlustersView* view = activeView();
  view->updateErrorMatrix();
}

void KlustersApp::slotSelectAll(){
 //Trigger the action only if the active display does not contain a ProcessWidget
 if(!doesActiveDisplayContainProcessWidget()){
  QValueList<int> clustersToHide;
  doc->showAllClustersExcept(clustersToHide);
 }
}

void KlustersApp::slotSelectAllWO01(){
 //Trigger the action only if the active display does not contain a ProcessWidget
 if(!doesActiveDisplayContainProcessWidget()){
  QValueList<int> clustersToHide;
  clustersToHide.append(0);
  clustersToHide.append(1);
  doc->showAllClustersExcept(clustersToHide);
 }
}

void KlustersApp::slotRecluster(){
 if(processOutputDock != 0L){
  if(processFinished && processOutputsFinished){
   delete processOutputDock;
   processOutputDock = 0L;
   processWidget = 0L;
   processKilled = false;
  }
  else{
   QTimer::singleShot(2000,this, SLOT(slotRecluster()()));
   return;
  }
 }

 //Get the clusters to recluster (those selected in the active display)
 const QValueList<int>& currentClusters = activeView()->clusters();
 if(currentClusters.size() == 0){
   KMessageBox::error (this,i18n("No clusters have been selected to be reclustered."), i18n("Error !"));
  return;
 }

 clustersToRecluster.clear();
 QValueList<int>::const_iterator shownClustersIterator;
 for(shownClustersIterator = currentClusters.begin(); shownClustersIterator != currentClusters.end(); ++shownClustersIterator)
  clustersToRecluster.append(*shownClustersIterator);
 qHeapSort(clustersToRecluster);

 //Build the command line to launch the reclustering

 //Create the fet file name: baseName + -- + -clusterIDs -- + timestamp + .fet + .electrodeGroupID
 QString fileBaseName = doc->documentBaseName();
 fileBaseName.append("--");

 int max = clustersToRecluster.size() - 1;
 int i = 0;
 for(; i < max; ++i){
  fileBaseName.append(QString::number(clustersToRecluster[i]));
  fileBaseName.append("-");   
 }
 fileBaseName.append(QString::number(clustersToRecluster[i]));
 
 QString date = QDate::currentDate().toString("--MM.dd.yyyy-");
 QString time = QTime::currentTime().toString("hh.mm");
 fileBaseName.append(date);
 fileBaseName.append(time);
  
 QString command(reclusteringExecutable);
 command.append(" ");
 command.append(reclusteringArgs);

 QString electrodeGroupID = doc->currentElectrodeGroupID();
 if(electrodeGroupID == "") electrodeGroupID = "1";

 //The default arguments are: "%fileBaseName %electrodeGroupID -MinClusters 2 -MaxClusters 12"
 //The fet file name will be docPath/%fileBaseName.fet.%electrodeNb
 if(command.contains("%electrodeGroupID")) command.replace("%electrodeGroupID",electrodeGroupID);
 command.replace("%fileBaseName",fileBaseName);
 reclusteringFetFileName = (doc->documentDirectory() + "/");
 reclusteringFetFileName.append(fileBaseName);
 reclusteringFetFileName.append(".fet.");
 reclusteringFetFileName.append(electrodeGroupID);

 //Subset of the input features to use. The default is to use all the PCA and the time feature.
 //If other features exist there are not used that is zeros are added for them.
 //NB: the time is always the last feature.
 if(command.contains("%features")){
  int totalNbOfPCAs = doc->totalNbOfPCAs();
  int nbDimensions = doc->nbDimensions();
  int nbExtraFeatures = nbDimensions - totalNbOfPCAs - 1;
  QString features;
  for(int i = 0; i < totalNbOfPCAs; ++i) features.append("1");
  for(int i = 0; i < nbExtraFeatures; ++i) features.append("0");
  features.append("1"); // the time feature.
  command.replace("%features",features);
 }
 
 //Create the feature file for the selected clusters and get its name.
 int returnStatus = doc->createFeatureFile(clustersToRecluster,reclusteringFetFileName);
 if(returnStatus == KlustersDoc::OPEN_ERROR){
  KMessageBox::error (this,i18n("The reclustering feature file cannot be created (possibly because of insufficient file access permissions).\n Reclustering can not be done."), i18n("Error !"));
  return;
 }
 if(returnStatus == KlustersDoc::CREATION_ERROR){
  KMessageBox::error (this,i18n("An error happened while creating the reclustering feature file.\n Reclustering can not be done."), i18n("IO Error !"));
  return;
 }

     
 if(processWidget == 0L){
  KDockWidget* display;
  display = createDockWidget(QString(QChar(displayCount)),0, 0L, i18n("Recluster output"), i18n("Recluster output"));

  processWidget = new ProcessWidget(display);
  connect(processWidget,SIGNAL(processExited(KProcess*)), this, SLOT(slotProcessExited(KProcess*)));
  connect(processWidget,SIGNAL(processOutputsFinished()), this, SLOT(slotOutputTreatmentOver()));

  //install the new view in the display so it can be see in the future tab.
  display->setWidget(processWidget);

  //Temporarily allow addition of a new dockWidget in the center
  mainDock->setDockSite(KDockWidget::DockCenter);
  //Add the new display as a tab and get a new DockWidget, grandParent of the target (mainDock)
  //and the new display.
  KDockWidget* grandParent = display->manualDock(mainDock,KDockWidget::DockCenter);

  //The grandParent's widget is the KDockTabGroup regrouping all the tabs
  tabsParent = static_cast<KDockTabGroup*>(grandParent->getWidget());

  //Connect the change tab signal to slotTabChange(QWidget* widget) to trigger updates when
  //the active display changes.
  connect(tabsParent, SIGNAL(currentChanged(QWidget*)), this, SLOT(slotTabChange(QWidget*)));

  //Back to enable dock to the left side only
  mainDock->setDockSite(KDockWidget::DockLeft);

  // forbit docking abilities of display itself
  display->setEnableDocking(KDockWidget::DockNone);

  // allow others to dock to the left side only
  display->setDockSite(KDockWidget::DockLeft);

  //Keep track of the number of displays
  displayCount ++;

  //show all the encapsulated widgets of all controlled dockwidgets
  dockManager->activate();
 } 

 //Rest the different variables.
 clustersFromReclustering.clear();
 processFinished = false;
 processOutputsFinished = false;
 processKilled = false;
 slotStateChanged("reclusterState");
 
 //Start the process
 bool status;
 status = processWidget->startJob(doc->documentDirectory(),command);
 
 if(!status){
  KMessageBox::error (this,i18n("The reclustering program could not be started.\n"
                      "One possible reason is that the automatic reclustering program could not be found."), i18n("Error !"));
  processFinished = true;
  processKilled = false;
  slotStateChanged("noReclusterState");
  updateUndoRedoDisplay();
 }
}

void KlustersApp::slotProcessExited(KProcess* process){
 //Check if the process has exited "voluntarily" and if so if it was successful
 if(!process->normalExit() || (process->normalExit() && process->exitStatus())){
  if(process->normalExit() || (!process->normalExit() && !processKilled))
    KMessageBox::error (this,i18n("The reclustering program did not finished normaly.\n"
                      "Check the output log for more information."), i18n("Error !"));
   
  if(!QFile::remove(reclusteringFetFileName))
   KMessageBox::error(0,i18n("Could not delete the temporary feature file used by the reclustering program."), i18n("Warning !"));
  processFinished = true;
  processOutputsFinished = true;
  processKilled = false;
  slotStateChanged("noReclusterState");
  if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay();
  else slotStateChanged("reclusterViewState");
  return;
 } 
 
 QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     
 int returnStatus = doc->integrateReclusteredClusters(clustersToRecluster,clustersFromReclustering,reclusteringFetFileName);

 switch(returnStatus){
  case KlustersDoc::DOWNLOAD_ERROR:
   QApplication::restoreOverrideCursor();
   KMessageBox::error(this,i18n("Could not download the temporary file containing the new clusters."), i18n("Error !"));
   processFinished = true;
   processOutputsFinished = true;
   processKilled = false;
   slotStateChanged("noReclusterState");
   if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay();
   else slotStateChanged("reclusterViewState");
   return;
  case KlustersDoc::OPEN_ERROR:
   QApplication::restoreOverrideCursor();
   KMessageBox::error (this,i18n("Could not open the temporary file containing the new clusters."), i18n("Error !"));
   processFinished = true;
   processOutputsFinished = true;
   processKilled = false; 
   slotStateChanged("noReclusterState");
   if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay();
   else slotStateChanged("reclusterViewState");
   return;
  case KlustersDoc::INCORRECT_CONTENT:
   QApplication::restoreOverrideCursor();
   KMessageBox::error (this,i18n("The temporary file containing the new clusters contains incorrect data."), i18n("Error !"));
   processFinished = true;
   processOutputsFinished = true;
   processKilled = false;
   slotStateChanged("noReclusterState");
   if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay();
   else slotStateChanged("reclusterViewState");
   return;
  case KlustersDoc::OK:
   break;
 }

 QString info = "The automatic reclustering of ";
 if(clustersToRecluster.size() > 1) info.append("clusters ");
 else info.append("cluster ");
 QValueList<int>::iterator iterator;
 for(iterator = clustersToRecluster.begin(); iterator != clustersToRecluster.end(); ++iterator ){
  info.append(QString::number(*iterator));
  info.append(" ");
 }
 
 if(clustersFromReclustering.size() > 1)
  info.append("is finished.\nThe following new clusters have been created: "); 
 else info.append("is finished.\nThe following new cluster has been created: "); 
 for(iterator = clustersFromReclustering.begin(); iterator != clustersFromReclustering.end(); ++iterator ){
  info.append(QString::number(*iterator));
  info.append(" ");
 }
 info.append(".\nThe cluster list will now be updated.");

 QApplication::restoreOverrideCursor();
 KMessageBox::information(this,i18n(info), i18n("Automatic Reclustering !"));
 QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
 doc->reclusteringUpdate(clustersToRecluster,clustersFromReclustering);

 processFinished = true;
 processKilled = false;
 slotStateChanged("noReclusterState");
 if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay(); 
 else slotStateChanged("reclusterViewState");
 QApplication::restoreOverrideCursor();      
}

void KlustersApp::slotStopRecluster(){
  processWidget->killJob();
  processKilled = true; 
  slotStateChanged("stoppedReclusterState");
}

void KlustersApp::slotOutputTreatmentOver(){
  processOutputsFinished = true;
  slotStateChanged("noRclusterState");
  if(!doesActiveDisplayContainProcessWidget()) updateUndoRedoDisplay();
  else slotStateChanged("reclusterViewState");
}

void KlustersApp::updateUndoRedoDisplay(){
  if(currentNbUndo > 0) slotStateChanged("undoState");
  else slotStateChanged("emptyUndoState");
  if(currentNbRedo == 0) slotStateChanged("emptyRedoState");
}

void KlustersApp::widgetAddToDisplay(KlustersView::DisplayType displayType,KDockWidget* docWidget){
 KlustersView* view = activeView(); 
 bool newWidgetType = view->addView(docWidget,displayType,backgroundColor,statusBar(),displayTimeInterval,waveformsGain,channelPositions);
 
 isInit = true; //prevent the spine boxes or the lineedit and the editline to trigger during initialisation.
 
 if(newWidgetType)
  switch(displayType){
   case KlustersView::CLUSTERS:
    //Update the dimension spine boxes with the initial values.
    dimensionX->setValue(1);
    dimensionY->setValue(2);
    slotStateChanged("clusterViewState");
    dimensionX->show();
    dimensionY->show();
    featureXLabel->show();
    break;
   case KlustersView::WAVEFORMS:
    slotStateChanged("waveformsViewState");
    overlayPresentation->setChecked(false);
    meanPresentation->setChecked(false);
    timeFrameMode->setChecked(false);
    duration->hide();
    durationLabel->hide();
    start->hide();
    startLabel->hide();
    spikesTodisplay->setValue(DEFAULT_NB_SPIKES_DISPLAYED);
    spikesTodisplay->show();
    spikesTodisplayLabel->show();   
    break;    
   case KlustersView::CORRELATIONS:
    slotStateChanged("correlationViewState");
    //Update the lineEdit
    scaleByMax->setChecked(true);
    correlogramTimeFrame = INITIAL_CORRELOGRAMS_HALF_TIME_FRAME.toInt() * 2 + 1; 
    binSize = DEFAULT_BIN_SIZE.toInt();
    correlogramsHalfDuration->setText(QString("%1").arg(correlogramTimeFrame / 2));    
    binSizeBox->setText(QString("%1").arg(binSize));
    correlogramsHalfDuration->show();
    correlogramsHalfDurationLabel->show();
    binSizeBox->show();
    binSizeLabel->show();
    //Update the shoulder line menu entry
    shoulderLine->setChecked(true);
     break;
   case KlustersView::ERROR_MATRIX :
     slotStateChanged("errorMatrixViewState");
     slotStateChanged("groupingAssistantDisplayExists");
     errorMatrixExists = true;   
     break;
   case KlustersView::OVERVIEW:
     break;
   case KlustersView::GROUPING_ASSISTANT_VIEW:
     break;
   case KlustersView::TRACES:
     slotStateChanged("traceViewState");
     showHideLabels->setChecked(view->getLabelStatus());  
     //Update the browsing possibility of the traceView
     if(view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");        
     break;   
 }

 isInit = false; //now a change in a spine box  or the lineedit
 //will trigger an update of the view contains in the acative display.
 
 //The select time tool is useful only if both a clusterView and a traceView are present
 if(view->containsClusterView() && view->containsTraceView()) slotStateChanged("traceViewClusterViewState"); 
}

void KlustersApp::widgetRemovedFromDisplay(KlustersView::DisplayType displayType){
 switch(displayType){
  case KlustersView::CLUSTERS:
   slotStateChanged("noClusterViewState");
   dimensionX->hide();
   dimensionY->hide();
   featureXLabel->hide();
   break;
  case KlustersView::WAVEFORMS:
   timeFrameMode->setChecked(false);
   overlayPresentation->setChecked(false);
   meanPresentation->setChecked(false);
   slotStateChanged("noWaveformsViewState");
   duration->hide();
   durationLabel->hide();
   start->hide();
   startLabel->hide();
   spikesTodisplay->hide();
   spikesTodisplayLabel->hide();
   break;
  case KlustersView::CORRELATIONS:
   slotStateChanged("noCorrelationViewState");
   correlogramsHalfDuration->hide();
   correlogramsHalfDurationLabel->hide();
   binSizeBox->hide();
   binSizeLabel->hide();
   break;
  case KlustersView::ERROR_MATRIX :
   slotStateChanged("noErrorMatrixViewState");
   slotStateChanged("groupingAssistantDisplayNotExists");
   errorMatrixExists = false;
   break;   
  case KlustersView::OVERVIEW:
   break;
  case KlustersView::GROUPING_ASSISTANT_VIEW:
   break;
  case KlustersView::TRACES:
   slotStateChanged("noTraceViewState");
   break;   
 }  
}

void KlustersApp::updateDimensionSpinBoxes(int dimensionX, int dimensionY){
 isInit = true; //prevent the spine boxes or the lineedit and the editline to trigger during initialisation

 //Update the dimension spine boxes
 this->dimensionX->setValue(dimensionX);
 this->dimensionY->setValue(dimensionY);
 this->dimensionX->show();
 this->dimensionY->show();

 isInit = false; //now a change in a spine box  or the lineedit
 //will trigger an update of the view contains in the active display.  
}

void KlustersApp::renameActiveDisplay(){
 KDockWidget* current;

 //Get the active tab
 current = static_cast<KDockWidget*>(tabsParent->currentPage());

 QString newLabel = KInputDialog::getText(i18n("New Display label"),i18n("Type in the new display label"),current->tabPageLabel());
 if(!newLabel.isEmpty()){
  tabsParent->setTabLabel(current,newLabel);
  current->setTabPageLabel(newLabel);
 }
}

void KlustersApp::slotShowLabels(){
 KlustersView* currentView = activeView();
 if(currentView->containsTraceView()) currentView->showLabelsUpdate(showHideLabels->isChecked());
}

void KlustersApp::updateCorrelogramViewVariables(int binSize,int timeWindow,bool isShoulderLine, Data::ScaleMode correlationScale){
 isInit = true; //prevent the boxes or menu entry to trigger during initialisation

 //Update the boxes
 this->binSize = binSize;
 binSizeBox->setText(QString("%1").arg(binSize));
 //timeWindow is the time frame use to compute the correlograms, the lineedit correlogramsHalfDuration contains half of it.
 correlogramTimeFrame = timeWindow;
 //int k = ((static_cast<float>(correlogramTimeFrame) / static_cast<float>(binSize)) - 1) / 2;
 //correlogramsHalfDuration->setText(QString("%1").arg(k * binSize)); 
 correlogramsHalfDuration->setText(QString("%1").arg(static_cast<int>(correlogramTimeFrame / 2))); 
 shoulderLine->setChecked(isShoulderLine);
 
 switch(correlationScale){
  case Data::RAW :
   noScale->setChecked(true);
   break;
  case Data::MAX :
   scaleByMax->setChecked(true);
   break;
  case Data::SHOULDER :
    scaleByShouler->setChecked(true);
    break;
 } 

 isInit = false; //now a change in the boxes or menu entry
 //will trigger an update of the view contains in the active display.  
}

void KlustersApp::slotShowNextCluster(){
//Trigger ths action only if the active display does not contain a ProcessWidget
 if(!doesActiveDisplayContainProcessWidget()){
  KlustersView* view = activeView();
  view->showNextCluster();
 } 
}

void KlustersApp::slotShowPreviousCluster(){
//Trigger ths action only if the active display does not contain a ProcessWidget
 if(!doesActiveDisplayContainProcessWidget()){
  KlustersView* view = activeView();
  view->showPreviousCluster();
 } 
}

void KlustersApp::slotSpikesDeleted(){
 //Update the browsing possibility of the traceView
 KlustersView* view = activeView();
 if(view->containsTraceView() && view->clusters().size() != 0) slotStateChanged("traceViewBrowsingState");
 else  slotStateChanged("noTraceViewBrowsingState");  
}
