/***************************************************************************
                          klustersdoc.cpp  -  description
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

// include files for Qt
#include <qdir.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qapplication.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QCustomEvent>
#include <Q3PtrList>
#include <QEvent>

// include files for KDE


#include <kio/job.h>


// application specific includes
#include "klusters.h"
#include "klustersdoc.h"
#include "klustersview.h"
#include "klustersdoc.h"
#include "types.h"
#include "autosavethread.h"
#include "parameterxmlmodifier.h"

//C, C++ include files
//#define _LARGEFILE_SOURCE already defined in /usr/include/features.h
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <math.h>

#include "timer.h"

extern int nbUndo;

KlustersDoc::KlustersDoc(QWidget* parent,ClusterPalette& clusterPalette,bool autoSave,int savingInterval): clusterColorListUndoList(),clusterColorListRedoList(),modified(false),docUrl(),baseName(""), xmlParameterFile(""),tmpCluFile(""),tmpSpikeFile(""),parent(parent),clusterPalette(clusterPalette),
addedClustersUndoList(),addedClustersRedoList(),modifiedClustersUndoList(),modifiedClustersRedoList(),autoSave(autoSave),savingInterval(savingInterval),tracesProvider(0L),clustersProvider(0L),channelColorList(0L){
  viewList = new Q3PtrList<KlustersView>;
  viewList->setAutoDelete(false);
  clusterColorListUndoList.setAutoDelete(true);
  clusterColorListRedoList.setAutoDelete(true);
  addedClustersUndoList.setAutoDelete(true);
  addedClustersRedoList.setAutoDelete(true);
  modifiedClustersUndoList.setAutoDelete(true);
  modifiedClustersRedoList.setAutoDelete(true);
  deletedClustersUndoList.setAutoDelete(true);
  deletedClustersRedoList.setAutoDelete(true);
  clusterColorList = 0L;
  addedClusters = 0L;
  modifiedClusters = 0L;
  deletedClusters = 0L;
  endAutoSaving = false;
  autoSaveThread = 0L;
}

KlustersDoc::~KlustersDoc(){
    cout << "~KlustersDoc()"<<endl;

  delete viewList;

  if(clusterColorList != 0L){
   delete clusteringData;
   delete clusterColorList;
   delete addedClusters;
   delete modifiedClusters;
   delete deletedClusters;
  }

  //If an autoSaveThread exists and has not finish, wait until it is done
  if(autoSave && autoSaveThread != 0L){
   if(!autoSaveThread->running()){
     autoSaveThread->removeTmpFile();
     delete autoSaveThread;
     autoSaveThread = 0L;
    }
    else{
     endAutoSaving = true;
     while(!autoSaveThread->wait()){};
     //Wait that the customEvent has process the AutoSaveEvent and deleted the autoSaveThread
     while(autoSaveThread != 0L){};
    }
   }
}

void KlustersDoc::addView(KlustersView *view)
{
  viewList->append(view);
}

void KlustersDoc::removeView(KlustersView *view){
	  viewList->remove(view);
}


bool KlustersDoc::isLastView() {
  return ((int) viewList->count() == 1);
}


void KlustersDoc::updateAllViews(KlustersView *sender){
  KlustersView *view;
  for(view = viewList->first(); view != 0; view = viewList->next())
  {
     view->update(sender);
  }

}

bool KlustersDoc::canCloseDocument(KlustersApp* mainWindow,QString callingMethod){
  //Before closing, make sure that there is no thread running.
  //Loop on all the views, moving to the next one when the current one has no more thread running.
  KlustersView* view;
  bool threadRunning = false;

  for(view = viewList->first(); view!=0; view = viewList->next())
  {
    threadRunning = view->isThreadsRunning();
    if(threadRunning) break;
  }

  if(threadRunning || !stopAutoSaving(true)){
   //Send an event to the klusters (main window) to let it know that the document can not
   //be close because some thread are still running.
   CloseDocumentEvent* event = getCloseDocumentEvent(callingMethod);
   QApplication::postEvent(mainWindow,event);
   return false;
  }
  else return true;
}

void KlustersDoc::closeDocument(){
  //If a document has been open reset the members
  viewList->clear();
  docUrl = QString();
  baseName = "";
  xmlParameterFile = "";
  clusterColorListUndoList.clear();
  clusterColorListRedoList.clear();
  addedClustersUndoList.clear();
  addedClustersRedoList.clear();
  modifiedClustersUndoList.clear();
  modifiedClustersRedoList.clear();
  clusterIdsNewOldMap.clear();
  clusterIdsOldNewMap.clear();


  if(clusterColorList != 0L){
    delete clusteringData;
    clusteringData = 0L;
    delete clusterColorList;
    clusterColorList = 0L;
    delete addedClusters;
    addedClusters = 0L;
    delete modifiedClusters;
    modifiedClusters = 0L;
  }
  //Remove the temp files if any
  if(tmpCluFile != "")KIO::NetAccess::removeTempFile(tmpCluFile);
  tmpCluFile = "";
  if(tmpSpikeFile != "")KIO::NetAccess::removeTempFile(tmpSpikeFile);
  tmpSpikeFile = "";

  //Variables link to TraceView
  if(channelColorList != 0L){
   delete channelColorList;
   channelColorList = 0L;
   delete tracesProvider;
   tracesProvider = 0L;
   delete clustersProvider;
   clustersProvider = 0L;
  }

  displayChannelsGroups.clear();
  channelsSpikeGroups.clear();
  displayGroupsChannels.clear();
  spikeGroupsChannels.clear();
  displayGroupsClusterFile.clear();
  gain = 0;
  acquisitionGain = 0;
}


bool KlustersDoc::importDocument(const QString &url, const char *format /*=0*/){
  bool returnValue = true;

  //1 - Get the base name of the file
  //2 - load the config information: Parse the XML config file, initialize clusteringData (loadConfigFromNewFormat())
  //3 - load the spikes, clusters, time and PCA information (loadDataFromNewFormat())
  return  returnValue;
}

int KlustersDoc::openDocument(const QString &url,QString& errorInformation, const char *format /*=0*/){
  //1 - Get the base name of the file
  //2 - load the config information: read the different files, initialize clusteringData (loadConfigFromNewFormat())
  //3 - load the spikes, clusters, time and PCA information (loadDataFromNewFormat())

  //Initialize the members specific to a document
  clusteringData = new Data();
  clusterColorList = new ItemColors();
  addedClusters = new Q3ValueList<int>();
  modifiedClusters = new Q3ValueList<int>();
  modified = false;

  //Store the baseName for future use
  QString fileName = url.fileName();
  QStringList fileParts = QStringList::split(".", fileName);
  if(fileParts.count() < 3) return INCORRECT_FILE;
  baseName = fileParts[0];
  for(uint i = 1;i < fileParts.count()-2; ++i) baseName += "." + fileParts[i];
  electrodeGroupID = fileParts[fileParts.count()-1];

  //Create the files url to open (baseName.spk.x,baseName.clu.x,baseName.fet.x,baseName.par.x,baseName.par and baseName.xml)
  //and store the url (corresponding to the cluster file). If the xml format parameter file does not exist, the parameter files
  // baseName.par.x,baseName.par will be used otherwise the baseName.xml will be used.
  QString spkFileUrl(url);
  spkFileUrl.setFileName(baseName +".spk."+ electrodeGroupID);
  QString cluFileUrl(url);
  cluFileUrl.setFileName(baseName +".clu."+ electrodeGroupID);
  docUrl = cluFileUrl;
  cluFileSaveUrl = QString(cluFileUrl);
  cluFileSaveUrl.setFileName("." + cluFileUrl.fileName() +".autoSave");
  QString fetFileUrl(url);
  fetFileUrl.setFileName(baseName +".fet."+ electrodeGroupID);

  //Parameter files
  QString xmlParFileUrl(url);
  xmlParFileUrl.setFileName(baseName +".xml");
  xmlParameterFile = xmlParFileUrl.path();
  QString parXFileUrl(url);
  parXFileUrl.setFileName(baseName +".par."+ electrodeGroupID);
  QString parFileUrl(url);
  parFileUrl.setFileName(baseName +".par");

  //Download the spike and fet files in temp files if necessary
  if(!KIO::NetAccess::download(spkFileUrl, tmpSpikeFile)) return SPK_DOWNLOAD_ERROR;
  QString tmpFetFile;
  if(!KIO::NetAccess::download(fetFileUrl, tmpFetFile)) return FET_DOWNLOAD_ERROR;

  //Open the the spike and fet files. Only the fet file will be loaded the spike file
  // will be used on the fly when waveforms will need to be drawn.
  //The biggest files are open in a C FILE to enable a quick access, the others (parameter files) are open in a QFile
  FILE* fetFile = fopen(tmpFetFile.toLatin1(),"r");
  if(fetFile == NULL){
   //Remove the temp files if any
   KIO::NetAccess::removeTempFile(tmpSpikeFile);
   KIO::NetAccess::removeTempFile(tmpFetFile);
   return OPEN_ERROR;
  }

  //The length of the spike file is used to determine the number of spikes.
  FILE* spikeFile = fopen(tmpSpikeFile.toLatin1(),"r");
  if(spikeFile == NULL){
   fclose(fetFile);
   //Remove the temp files if any
   KIO::NetAccess::removeTempFile(tmpSpikeFile);
   KIO::NetAccess::removeTempFile(tmpFetFile);
   return OPEN_ERROR;
  }
  fseeko(spikeFile,0,SEEK_END);
  long spkFileLength = ftell(spikeFile);
  fclose(spikeFile);
  //Remove the temp file if any
  KIO::NetAccess::removeTempFile(tmpSpikeFile);

  bool isXmlParExist = false;
  QString tmpXmlParFile;
  QString tmpParXFile;
  QString tmpParFile;
  QFileInfo xmlParFileInfo(xmlParFileUrl.path());
  QFile xmlParFile;
  QFile parXFile;
  QFile parFile;
  if(xmlParFileInfo.exists()){
   if(!KIO::NetAccess::download(xmlParFileUrl,tmpXmlParFile)) return PARXML_DOWNLOAD_ERROR;
   isXmlParExist = true;
   //Check if the generic parameter file also exist, if so, warn the user that the xml format parameter file will be used.
   QFileInfo parFileInfo(parFileUrl.path());
   if(parFileInfo.exists()){
    QApplication::restoreOverrideCursor();
    KMessageBox::information(0,tr("Two parameter files were found, %1 and %2. The parameter file %3 will be used.").arg(xmlParFileUrl.fileName()).arg(parFileUrl.fileName()).arg(xmlParFileUrl.fileName()), tr("Warning!"),"TwoParameterFiles");
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   }
   xmlParFile.setName(tmpXmlParFile);
   if(!xmlParFile.open(QIODevice::ReadOnly)){
    fclose(fetFile);
    //Remove the temp files if any
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
    KIO::NetAccess::removeTempFile(tmpXmlParFile);
    return OPEN_ERROR;
   }
  }
  else{
   if(!KIO::NetAccess::download(parXFileUrl, tmpParXFile)) return PARX_DOWNLOAD_ERROR;
   parXFile.setName(tmpParXFile);
   if(!parXFile.open(QIODevice::ReadOnly)){
    fclose(fetFile);
    //Remove the temp files if any
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
    KIO::NetAccess::removeTempFile(tmpParXFile);
    return OPEN_ERROR;
   }
   if(!KIO::NetAccess::download(parFileUrl, tmpParFile)) return PAR_DOWNLOAD_ERROR;
   parFile.setName(tmpParFile);
   if(!parFile.open(QIODevice::ReadOnly)){
    fclose(fetFile);
    parXFile.close();
    //Remove the temp files if any
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
    KIO::NetAccess::removeTempFile(tmpParXFile);
    KIO::NetAccess::removeTempFile(tmpParFile);
    return OPEN_ERROR;
   }
  }

  //If a crashRecoveryFile exits, check if it is newer than the clu file, if so
  //ask the user if he wants to use that one to replace the clu file.
   QFileInfo crashFileInfo(cluFileSaveUrl.path());
   if(crashFileInfo.exists()){
     QFileInfo cluFileInfo(cluFileUrl.path());
     if((cluFileInfo.exists() && crashFileInfo.lastModified() > cluFileInfo.lastModified()) ||
      !cluFileInfo.exists()){
        QApplication::restoreOverrideCursor();
        switch(KMessageBox::questionYesNo(0,tr("A more recent copy of the cluster file (a rescue file) was found on the disk. This indicates that Klusters crashed while editing these data during a previous session.\n"
               "Do you wish to use the newer copy (The old copy will be saved under another name)?"),
                tr("More recent cluster file found"), tr("Use newer copy"), tr("Discard newer copy") ))
        {
					case KMessageBox::Yes:
					 QDir dir(crashFileInfo.dir());
           QString cluName = cluFileInfo.fileName();
           bool renameStatus;
           if(cluFileInfo.exists()){
            renameStatus = dir.rename(cluName,cluFileInfo.fileName()+ "." + cluFileInfo.lastModified().toString("MM.dd.yyyy.hh.mm"));
           }
           renameStatus = dir.rename(crashFileInfo.fileName(),cluName);
           if(!renameStatus)
            KMessageBox::error(0,tr(
              "It appears that the rescue file cannot be renamed (possibly because of insufficient file access permissions).\n"
              "The rescue file will thus not be used."), tr("I/O Error !"));

           break;
      }
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
     }
   }


  //Treat the cluster file separately as it can be empty
  if(KIO::NetAccess::exists(cluFileUrl)){
   if(!KIO::NetAccess::download(cluFileUrl, tmpCluFile)){
    if(isXmlParExist){
     xmlParFile.close();
     KIO::NetAccess::removeTempFile(tmpXmlParFile);
    }
    else{
     parXFile.close();
     parFile.close();
     //Remove the temp files if any
     KIO::NetAccess::removeTempFile(tmpParXFile);
     KIO::NetAccess::removeTempFile(tmpParFile);
    }
    fclose(fetFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
    return DOWNLOAD_ERROR;
   }

   FILE* cluFile = fopen(tmpCluFile.toLatin1(),"r");
   if(cluFile == NULL){
    if(isXmlParExist){
     xmlParFile.close();
     KIO::NetAccess::removeTempFile(tmpXmlParFile);
    }
    else{
     parXFile.close();
     parFile.close();
     //Remove the temp files if any
     KIO::NetAccess::removeTempFile(tmpParXFile);
     KIO::NetAccess::removeTempFile(tmpParFile);
    }
    fclose(fetFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
    return OPEN_ERROR;
   }

   //Initialize the data
   if(isXmlParExist){
    if(!clusteringData->initialize(fetFile,cluFile,spkFileLength,tmpSpikeFile,xmlParFile,electrodeGroupID.toInt(),errorInformation)){
     //close the files
     xmlParFile.close();
     fclose(fetFile);
     fclose(cluFile);
     KIO::NetAccess::removeTempFile(tmpXmlParFile);
     KIO::NetAccess::removeTempFile(tmpSpikeFile);
     return INCORRECT_CONTENT;
    }
    xmlParFile.close();
    fclose(fetFile);
    fclose(cluFile);
    KIO::NetAccess::removeTempFile(tmpXmlParFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
   }
   else{
    if(!clusteringData->initialize(fetFile,cluFile,spkFileLength,tmpSpikeFile,parXFile,parFile,errorInformation)){
     //close the files
     parXFile.close();
     parFile.close();
     fclose(fetFile);
     fclose(cluFile);
     KIO::NetAccess::removeTempFile(tmpParXFile);
     KIO::NetAccess::removeTempFile(tmpParFile);
     KIO::NetAccess::removeTempFile(tmpSpikeFile);
     return INCORRECT_CONTENT;
    }
    parXFile.close();
    parFile.close();
    fclose(fetFile);
    fclose(cluFile);
    KIO::NetAccess::removeTempFile(tmpParXFile);
    KIO::NetAccess::removeTempFile(tmpParFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
   }
  }//end //the cluster file exists
  //the cluster file does not exist
  else{
   tmpCluFile =  cluFileUrl.path();

   //Initialize the data
   if(isXmlParExist){
    if(!clusteringData->initialize(fetFile,spkFileLength,tmpSpikeFile,xmlParFile,electrodeGroupID.toInt(),errorInformation)){
     //close the files
     xmlParFile.close();
     fclose(fetFile);
     KIO::NetAccess::removeTempFile(tmpXmlParFile);
     KIO::NetAccess::removeTempFile(tmpSpikeFile);
     return INCORRECT_CONTENT;
    }
    xmlParFile.close();
    fclose(fetFile);
    KIO::NetAccess::removeTempFile(tmpXmlParFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
   }
   else{
    if(!clusteringData->initialize(fetFile,spkFileLength,tmpSpikeFile,parXFile,parFile,errorInformation)){
     //close the files
     parXFile.close();
     parFile.close();
     fclose(fetFile);
     KIO::NetAccess::removeTempFile(tmpParXFile);
     KIO::NetAccess::removeTempFile(tmpParFile);
     KIO::NetAccess::removeTempFile(tmpSpikeFile);

     return INCORRECT_CONTENT;
    }
    //close the files
    parXFile.close();
    parFile.close();
    fclose(fetFile);
    KIO::NetAccess::removeTempFile(tmpParXFile);
    KIO::NetAccess::removeTempFile(tmpParFile);
    KIO::NetAccess::removeTempFile(tmpSpikeFile);
   }
  }//end the cluster file does not exist


  //Constructs the clusterColorList
  Q3ValueList<dataType> clusterList = clusteringData->clusterIds();
  Q3ValueList<dataType>::iterator it;
  for(it = clusterList.begin(); it != clusterList.end(); ++it){
    QColor color;
    if(*it == 1) color.setHsv(0,0,220);//Cluster 1 is always gray
    else color.setHsv(static_cast<int>(fmod(static_cast<double>(*it)*7,36))*10,255,255);
    clusterColorList->append(static_cast<int>(*it),color);
  }


  //If ask create a thread for the auto saving of the document.
  if(autoSave){
    cout<<"autoSave = true in openDoc"<<endl;
   endAutoSaving = false;
   autoSaveThread = new AutoSaveThread(*clusteringData,this,cluFileSaveUrl);
   autoSaveThread->start();
  }

  return OK;
}

void KlustersDoc::updateAutoSavingInterval(int interval){
  savingInterval = interval;
  endAutoSaving = false;
  if(!autoSave){
   autoSave = true;
   autoSaveThread = new AutoSaveThread(*clusteringData,this,cluFileSaveUrl);
   autoSaveThread->start();
  }
}

bool KlustersDoc::stopAutoSaving(bool currentDocument){
 if(autoSave && autoSaveThread != 0L){
  if(!autoSaveThread->running()){
     autoSaveThread->removeTmpFile();
     delete autoSaveThread;
     autoSaveThread = 0L;
     if(!currentDocument) autoSave = false;
     endAutoSaving = true;
     return true;
  }
  else{
   endAutoSaving = true;
   return false;
  }
 }
 else{
  endAutoSaving = true;
  if(!currentDocument) autoSave = false;
  return true;
 }
}

void KlustersDoc::launchAutoSave(){
  if(!endAutoSaving)autoSaveThread->start();
}

void KlustersDoc::customEvent(QCustomEvent* event){
 //The autoSaveThread has finish, it can be delete.
 if(event->type() == QEvent::User + 500){
  if(endAutoSaving){
   if(autoSaveThread != 0L){
    autoSaveThread->removeTmpFile();
    delete autoSaveThread;
    autoSaveThread = 0L;
   }
  }
  else{
    if(((AutoSaveThread::AutoSaveEvent*)event)->isIOerror())
     QMessageBox::critical(0,tr("I/O Error !"),tr(
      "In order to protect your work in case of a crash, Klusters periodically saves a hidden copy of the cluster file"
      " in the directory where your files are located (this temporary rescue file is removed when you quit the application).\n"
      "However, it now appears that this rescue file cannot be created (possibly because of insufficient file access permissions).\n"
      "This feature will thus be disabled for the current session.") );
    else
    //upload the temp file, this can not be done asynchronously.
   //wait savingInterval before starting the autoSaveThread again.
    QTimer::singleShot(savingInterval*60000,this, SLOT(launchAutoSave()));
  }
 }
}

int KlustersDoc::saveDocument(const QString& saveUrl, const char *format /*=0*/){

  QString tmpCluFileSave = tmpCluFile;
  if(docUrl != saveUrl){
	  tmpCluFile =  saveUrl.path();
  }

  //Open the temp file in write mode
  FILE* cluFile = fopen(tmpCluFile.toLatin1(),"w");
 	if(cluFile == NULL){
    return OPEN_ERROR;
    tmpCluFile = tmpCluFileSave;
  }

  if(!clusteringData->saveClusters(cluFile)){
    tmpCluFile = tmpCluFileSave;
    return SAVE_ERROR;
  }

  //close the file
  fclose(cluFile);

  //if it was a saveAs, the url has changed, update it
  if(docUrl != saveUrl){
	  docUrl = saveUrl;
	  QString fileName = docUrl.fileName();
	  QStringList fileParts = QStringList::split(".", fileName);
	  baseName = fileParts[0];
	  if(fileParts.count() > 2) for(uint i = 1;i < fileParts.count()-2; ++i)baseName += "." + fileParts[i];
	  if(fileParts.count() < 3) electrodeGroupID = "";
	  else electrodeGroupID = fileParts[fileParts.count()-1];

	  QString xmlParFileUrl(saveUrl);
	  xmlParFileUrl.setFileName(baseName +".xml");
	  xmlParameterFile = xmlParFileUrl.path();
  }

  //Save the cluster user information if the xmlParameterFile exists
  //NB : for the moment, the specific errors are not return to the user, only a generic message (document could not be saved).
  if(clusteringData->isTraceViewVariablesAvailable()){
	//Save the document information
	QFileInfo parFileInfo = QFileInfo(xmlParameterFile);

  	 //Check that the file is writable
	if(!parFileInfo.isWritable()) return NOT_WRITABLE;
	bool status;

	QMap<int,ClusterUserInformation> clusterUserInformationMap = QMap<int,ClusterUserInformation>();
	clusteringData->getClusterUserInformation(electrodeGroupID.toInt(),clusterUserInformationMap);

	ParameterXmlModifier parameterModifier = ParameterXmlModifier();
	status = parameterModifier.parseFile(xmlParameterFile);
	if(!status) return PARSE_ERROR;

	status = parameterModifier.setClusterUserInformation(electrodeGroupID.toInt(),clusterUserInformationMap);
	if(!status) return CREATION_ERROR;

	status = parameterModifier.writeTofile(xmlParameterFile);
	if(!status) return CREATION_ERROR;
  }

  modified=false;

  return OK;
}


bool KlustersDoc::canCloseView(){
	bool returnValue = false;
  if(isModified()){
		QString saveURL;
    switch(KMessageBox::warningYesNoCancel(0, tr("The current file has been modified.\n"
                          "Do you want to save it?"),url().fileName()))
    {
			case KMessageBox::Yes:
				saveURL=url();
        int saveStatus;
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        saveStatus = saveDocument(saveURL);
        QApplication::restoreOverrideCursor();
        if(saveStatus != OK){
                    switch(KMessageBox::warningYesNo(0,tr("Could not save the current document !\n"
                                                                                                                "Close anyway ?"), tr("I/O Error !")))
 					{
 						case KMessageBox::Yes:
 							returnValue = true;
              modified = false;
              break;
 						case KMessageBox::No:
 							returnValue = false;
              break;
 					}
				}
				else{
					returnValue = true;
          modified = false;
        }
				break;
			case KMessageBox::No:
				returnValue = true;
        modified = false;
				break;
			case KMessageBox::Cancel:
        returnValue = false;
				break;
			default:
				returnValue = false;
				break;
		}
	}
	else
		returnValue = true;

	return returnValue;
}

QString KlustersDoc::documentName(){
  return docUrl.directory() + "/" + baseName + "-" + electrodeGroupID;
}

QString KlustersDoc::documentBaseName(){
  return baseName;
}

QString KlustersDoc::documentDirectory(){
  return docUrl.directory();
}

void KlustersDoc::setGain(int acquisitionGain){
 //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
   view->setGain(acquisitionGain);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //Ask the active view to take the modification into account immediately
  activeView->showAllWidgets();
}

void KlustersDoc::setBackgroundColor(QColor backgroundColor){
 //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
   view->updateBackgroundColor(backgroundColor);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //Ask the active view to take the modification into account immediately
  activeView->showAllWidgets();
}

void KlustersDoc::setTimeStepInSecond(int step){
  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

 //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next()){
    if(view != activeView) view->setTimeStepInSecond(step,false);
    else view->setTimeStepInSecond(step,true);
  }

  //Ask the active view to take the modification into account immediately
  activeView->showAllWidgets();
}

void KlustersDoc::setChannelPositions(Q3ValueList<int>& positions){
 //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
   view->setChannelPositions(positions);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //Ask the active view to take the modification into account immediately
  activeView->showAllWidgets();
}

void KlustersDoc::singleColorUpdate(int clusterId,KlustersView& activeView){
  //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
  {
     if(view != &activeView) view->singleColorUpdate(clusterId,false);
     else view->singleColorUpdate(clusterId,true);
  }

  //Ask the active view to take the modification into account immediately
  activeView.showAllWidgets();
}


void KlustersDoc::shownClustersUpdate(Q3ValueList<int> clustersToShow,KlustersView& activeView){
  if(clusterColorList->isColorChanged()){
    //Notify all the views of the modification
    KlustersView* view;
    for(view = viewList->first(); view!=0; view = viewList->next())
    {
      if(view != &activeView) view->updateColors(false);
      else view->updateColors(true);
    }

    //Reset the color status in clusterColors
    clusterColorList->resetAllColorStatus();

    //Update the palette of clusters
    clusterPalette.updateClusterList();
    clusterPalette.selectItems(clustersToShow);
  }

  //The new selection of clusters only means for the active view
  activeView.shownClustersUpdate(clustersToShow);

  //update the TraceView if any
  activeView.updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::shownClustersUpdate(Q3ValueList<int> clustersToShow){
  //Update the palette of cluster
  clusterPalette.selectItems(clustersToShow);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //The new selection of clusters only means for the active view
  activeView->shownClustersUpdate(clustersToShow);

  //update the TraceView if any
  activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::shownClustersUpdate(Q3ValueList<int> clustersToShow,Q3ValueList<int> previousSelectedClusterPairs){
  //Get the clusters currently selected
  Q3ValueList<int> currentShownClusters = clusterPalette.selectedClusters();

  //Add the clusters which were shown and not part of the previous selected cluster pairs
  Q3ValueList<int>::iterator clustersToAdd;
  for(clustersToAdd = currentShownClusters.begin(); clustersToAdd != currentShownClusters.end(); ++clustersToAdd )
   if(!previousSelectedClusterPairs.contains(*clustersToAdd)) clustersToShow.append(*clustersToAdd);

  //Update the palette of cluster
  clusterPalette.selectItems(clustersToShow);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //The new selection of clusters only means for the active view
  activeView->shownClustersUpdate(clustersToShow);

 //update the TraceView if any
  activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::showAllClustersExcept(Q3ValueList<int> clustersToHide){

  Q3ValueList<dataType> clusterList = clusteringData->clusterIds();
  Q3ValueList<int> clustersToShow;

  Q3ValueList<dataType>::iterator clustersToAdd;
  for(clustersToAdd = clusterList.begin(); clustersToAdd != clusterList.end(); ++clustersToAdd ){
   if(!clustersToHide.contains(static_cast<int>(*clustersToAdd))) clustersToShow.append(static_cast<int>(*clustersToAdd));
  }

  //Update the palette of cluster
  clusterPalette.selectItems(clustersToShow);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //The new selection of clusters only means for the active view
  activeView->shownClustersUpdate(clustersToShow);

 //update the TraceView if any
 activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::addClustersToActiveView(Q3ValueList<int> clustersToShow){
  //Get the clusters currently selected
  Q3ValueList<int> currentShownClusters = clusterPalette.selectedClusters();

  Q3ValueList<int>::iterator clustersToAdd;
  for(clustersToAdd = currentShownClusters.begin(); clustersToAdd != currentShownClusters.end(); ++clustersToAdd )
   clustersToShow.append(*clustersToAdd);

  //Update the palette of cluster
  clusterPalette.selectItems(clustersToShow);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //The new selection of clusters only means for the active view
  activeView->shownClustersUpdate(clustersToShow);

 //update the TraceView if any
  activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::groupClusters(Q3ValueList<int> clustersToGroup,KlustersView& activeView){
  //Call data to group the clusters
  float newClusterId = clusteringData->groupClusters(clustersToGroup);
  int newClusterIdint = static_cast<int>(newClusterId);

  //Prepare the undo
  prepareUndo(newClusterIdint,clustersToGroup);

  //Add the cluster in clusterColors.
  QColor color;
  color.setHsv(static_cast<int>(fmod(newClusterId*7,36))*10,255,255);
  clusterColorList->append(newClusterIdint,color);

  //Remove the clusters which were grouped
  Q3ValueList<int>::iterator clustersToRemove;
  for (clustersToRemove = clustersToGroup.begin(); clustersToRemove != clustersToGroup.end(); ++clustersToRemove ){
    clusterColorList->remove(*clustersToRemove);
  }

  //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next()){
   if(view != &activeView){
    view->groupedClustersUpdate(clustersToGroup,newClusterIdint,false);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,false);
   }
   else{
    view->groupedClustersUpdate(clustersToGroup,newClusterIdint,true);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,true);
   }
  }

  //Notify the errorMatrixView of the modification
  emit clustersGrouped(clustersToGroup,newClusterIdint);

  //Reset the color status in clusterColors if need it
  if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

  //Ask the active view to take the modification into account immediately
  activeView.showAllWidgets();

  //Update the palette of cluster
  clusterPalette.updateClusterList();
  Q3ValueList<int> clustersToShow;
  clustersToShow.append(newClusterIdint);
  clusterPalette.selectItems(clustersToShow);
}


void KlustersDoc::deleteClusters(Q3ValueList<int> clustersToDelete,KlustersView& activeView,int clusterId){
   Q3ValueList<int> modifiedcluster;
   modifiedcluster.append(clusterId);

  //If only one cluster has been deleted, the following cluster on the list, if any, will be selected.
  //Find that following cluster.
  int clusterToSelect;
  bool existNextCluster = false;
  if(clustersToDelete.size() == 1){
   int clusterToDelete =  clustersToDelete[0];
   bool previous = false;
   Q3ValueList<dataType> clusters = clusteringData->clusterIds();
   Q3ValueList<dataType>::iterator clustersIterator;
   for(clustersIterator = clusters.begin(); clustersIterator != clusters.end(); ++clustersIterator){
    if(previous){
     clusterToSelect = static_cast<int>(*clustersIterator);
     existNextCluster = true;
     break;
    }
    if(*clustersIterator == clusterToDelete) previous = true;
   }
  }

   //case where the clusters are moved to the cluster 0 (artefact)
   if(clusterId == 0){
   //Call data to move the clusters
   clusteringData->moveClustersToArtefact(clustersToDelete);
   //Update clusterColors, add cluster 0 if need it
   if(!clusterColorList->contains(0)){
     //Prepare the undo
     prepareUndo(0,modifiedcluster,clustersToDelete);
     QColor color(red); //Cluster 01 is always red
     clusterColorList->insert(static_cast<int>(0),color,0);
   }
   else
     //Prepare the undo
     prepareUndo(modifiedcluster,clustersToDelete);
  }
  //case where the clusters are moved to the cluster 1 (noise)
  if(clusterId == 1){
    //Call data to move the clusters
    clusteringData->moveClustersToNoise(clustersToDelete);
    //Update clusterColors, add cluster 1 if need it
    if(!clusterColorList->contains(1)){
     //Prepare the undo
     prepareUndo(1,modifiedcluster,clustersToDelete);
     QColor color;
     color.setHsv(0,0,220);//Cluster 1 is always gray
     if(clusterColorList->contains(0)) clusterColorList->insert(static_cast<int>(1),color,1);
       else clusterColorList->insert(static_cast<int>(1),color,0);
    }
    else
      //Prepare the undo
      prepareUndo(modifiedcluster,clustersToDelete);
  }

  //Update clusterColors,remove the clusters which were deleted
  Q3ValueList<int>::iterator clustersToRemove;
  for (clustersToRemove = clustersToDelete.begin(); clustersToRemove != clustersToDelete.end(); ++clustersToRemove ){
    if(*clustersToRemove == clusterId) continue;
    clusterColorList->remove(*clustersToRemove);
  }

  //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next()){
   if(view != &activeView){
    view->clustersDeletionUpdate(clustersToDelete,clusterId,false);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,false);
   }
   else{
    view->clustersDeletionUpdate(clustersToDelete,clusterId,true);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,true);
   }
  }

  //Notify the errorMatrixView of the modification
  emit clustersDeleted(clustersToDelete,clusterId);

  //Reset the color status in clusterColors if need it
  if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

  //Ask the active view to take the modification into account immediately
  activeView.showAllWidgets();

 //Update the palette of cluster
 clusterPalette.updateClusterList();

 //If only one cluster has been deleted, select the following cluster on the list if any.
 if(existNextCluster){
  Q3ValueList<int> clusters;
  clusters.append(clusterToSelect);

  //Update the cluster palette
  clusterPalette.selectItems(clusters);
  activeView.shownClustersUpdate(clusters);

  //update the TraceView if any
  activeView.updateTraceView(electrodeGroupID,clusterColorList,true);
 }
}

void KlustersDoc::deleteArtifact(QRegion& region,const Q3ValueList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
 deleteSpikesFromClusters(0,region,clustersOfOrigin,dimensionX,dimensionY);
}


void KlustersDoc::deleteNoise(QRegion& region,const Q3ValueList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
 deleteSpikesFromClusters(1,region,clustersOfOrigin,dimensionX,dimensionY);
}

void KlustersDoc::deleteSpikesFromClusters(int destination, QRegion& region,const Q3ValueList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
  //list which will contain the clusters really having spikes in the region of selection.
  Q3ValueList <int> fromClusters;
  //list which will contain the clusters which became empty because all their spikes were in the region of selection.
  Q3ValueList <int> emptyClusters;
  Q3ValueList<int> clustersToShow(clustersOfOrigin);

  clusteringData->deleteSpikesFromClusters(region,clustersOfOrigin,destination,dimensionX,dimensionY,fromClusters,emptyClusters);

  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //check if any spikes have been selected
  if(fromClusters.size() == 0){
    activeView->selectionIsEmpty();
    activeView->showAllWidgets();
  }
  else{
   Q3ValueList<int> updatedClusters = Q3ValueList<int>(fromClusters);
   updatedClusters.append(destination);

   //Update clusterColors, add cluster 1 if need it
   if(destination == 1 && !clusterColorList->contains(1)){
     //Prepare the undo
     prepareUndo(1,updatedClusters,emptyClusters,true);
     QColor color;
     color.setHsv(0,0,220);//Cluster 1 is always gray
     if(clusterColorList->contains(0)) clusterColorList->insert(static_cast<int>(1),color,1);
      else clusterColorList->insert(static_cast<int>(1),color,0);
   }
   //Update clusterColors, add cluster 0 if need it
   else if(destination == 0 && !clusterColorList->contains(0)){
     //Prepare the undo
     prepareUndo(0,updatedClusters,emptyClusters,true);
     QColor color(red); //Cluster 01 is always red
     clusterColorList->insert(static_cast<int>(0),color,0);
   }
   else
    //Prepare the undo
    prepareUndo(updatedClusters,emptyClusters,true);

  //Remove all the empty clusters from clusterColors and clustersToShow
  if(emptyClusters.size()>0){
    Q3ValueList<int>::iterator clustersToRemove;
    for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
      clusterColorList->remove(*clustersToRemove);
      clustersToShow.remove(*clustersToRemove);
    }
  }

  //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next()){
   if(view != activeView){
    view->removeSpikesFromClustersInView(fromClusters,destination,emptyClusters,false);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,false);
   }
   else{
    view->removeSpikesFromClustersInView(fromClusters,destination,emptyClusters,true);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,true);
   }
  }

  //Notify the errorMatrixView of the modification
  emit removeSpikesFromClusters(fromClusters,destination,emptyClusters);

  //Reset the color status in clusterColors if need it
  if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

  activeView->showAllWidgets();

  //Update the palette of cluster
  clusterPalette.updateClusterList();
  clusterPalette.selectItems(clustersToShow);

  //Notify the application that spikes have been deleted.
  emit spikesDeleted();
  }
}


void KlustersDoc::createNewCluster(QRegion& region, const Q3ValueList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
  //list which will contain the clusters really having spikes in the region of selection.
  Q3ValueList <int> fromClusters;
  //list which will contain the clusters which became empty because all their spikes were in the region of selection.
  Q3ValueList <int> emptyClusters;
  Q3ValueList<int> clustersToShow(clustersOfOrigin);
  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  float newClusterId = clusteringData->createNewCluster(region,clustersOfOrigin,dimensionX,dimensionY,fromClusters,emptyClusters);

  //Check if a new cluster has been created
  if(newClusterId == 0){
    activeView->selectionIsEmpty();
    activeView->showAllWidgets();
  }
  else{
    int newClusterIdint = static_cast<int>(newClusterId);

    //Prepare the undo
    prepareUndo(newClusterIdint,fromClusters,emptyClusters);

    //Add the cluster in clusterColors and clustersToShow.
    QColor color;
    color.setHsv(static_cast<int>(fmod(newClusterId*7,36))*10,255,255);
    clusterColorList->append(newClusterIdint,color);
    clustersToShow.append(newClusterIdint);
    //Remove all the empty clusters from clusterColors and clustersToShow
    if(emptyClusters.size()>0){
      Q3ValueList<int>::iterator clustersToRemove;
      for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
        clusterColorList->remove(*clustersToRemove);
        clustersToShow.remove(*clustersToRemove);
      }
    }

    //Notify all the views of the modification
    KlustersView* view;
    for(view = viewList->first(); view!=0; view = viewList->next()){
     if(view != activeView){
      view->addNewClusterToView(fromClusters,newClusterIdint,emptyClusters,false);
      //update the TraceView if any
      view->updateTraceView(electrodeGroupID,clusterColorList,false);
     }
     else{
      view->addNewClusterToView(fromClusters,newClusterIdint,emptyClusters,true);
      //update the TraceView if any
      view->updateTraceView(electrodeGroupID,clusterColorList,true);
     }
    }

    //Notify the errorMatrixView of the modification
    emit newClusterAdded(fromClusters,newClusterIdint,emptyClusters);

    //Reset the color status in clusterColors if need it
    if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

    activeView->showAllWidgets();

    //Update the palette of cluster
    clusterPalette.updateClusterList();
    clusterPalette.selectItems(clustersToShow);
  }
}

void KlustersDoc::createNewClusters(QRegion& region, const Q3ValueList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
  //list which will contain the clusters really having spikes in the region of selection.
  Q3ValueList <int> fromClusters;
  //list which will contain the clusters which became empty because all their spikes were in the region of selection.
  Q3ValueList <int> emptyClusters;
  Q3ValueList<int> clustersToShow(clustersOfOrigin);
  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  Q3ValueList <int> newClusters;
  QMap<int,int> fromToNewClusterIds = clusteringData->createNewClusters(region,clustersOfOrigin,dimensionX,dimensionY,emptyClusters);
  newClusters = fromToNewClusterIds.values();
  fromClusters = fromToNewClusterIds.keys();

  //Check if at least one new cluster has been created
  if(newClusters.size() == 0){
    activeView->selectionIsEmpty();
    activeView->showAllWidgets();
  }
  else{
    //Prepare the undo
    prepareUndo(newClusters,fromClusters,emptyClusters);

    //Add the clusters in clusterColors and clustersToShow.
    QColor color;
    Q3ValueList<int>::iterator clustersToCreate;
    qSort(newClusters);
    for (clustersToCreate = newClusters.begin(); clustersToCreate != newClusters.end(); ++clustersToCreate ){
        color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
        clusterColorList->append(*clustersToCreate,color);
        clustersToShow.append(*clustersToCreate);
    }
    //Remove all the empty clusters
    if(emptyClusters.size()>0){
      Q3ValueList<int>::iterator clustersToRemove;
      for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
        clusterColorList->remove(*clustersToRemove);
        clustersToShow.remove(*clustersToRemove);
      }
    }

    //Notify all the views of the modification
    KlustersView* view;
    for(view = viewList->first(); view!=0; view = viewList->next()){
     if(view != activeView){
      view->addNewClustersToView(fromToNewClusterIds,emptyClusters,false);
      //update the TraceView if any
      view->updateTraceView(electrodeGroupID,clusterColorList,false);
     }
     else{
      view->addNewClustersToView(fromToNewClusterIds,emptyClusters,true);
      //update the TraceView if any
      view->updateTraceView(electrodeGroupID,clusterColorList,true);
     }
    }

    //Notify the errorMatrixView of the modification
    emit newClustersAdded(fromToNewClusterIds,emptyClusters);


    //Reset the color status in clusterColors if need it
    if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

    activeView->showAllWidgets();

    //Update the palette of cluster
    clusterPalette.updateClusterList();
    clusterPalette.selectItems(clustersToShow);
  }
}

void KlustersDoc::prepareClusterColorUndo(){
  //Update the boolean modified here as every action implies a call to the function
  modified = true;

  //Create a new clusterColors which will hold the new configuration
  ItemColors* clusterColorListTemp = new ItemColors(*clusterColorList);

  //Store the current clusterColors in the undo list and make the temporary become the current one.
  clusterColorListUndoList.prepend(clusterColorList);
  clusterColorList = clusterColorListTemp;

  //if the number of undo has been reach remove the last element in the undo list (first inserted)
  int currentClusterColorsNbUndo = clusterColorListUndoList.count();
  if(currentClusterColorsNbUndo > nbUndo)
   clusterColorListUndoList.remove(currentClusterColorsNbUndo - 1);

  //Clear the redoList
  clusterColorListRedoList.clear();

  //Signal to klusters the new number of undo and redo
  emit updateUndoNb(clusterColorListUndoList.count());
  emit updateRedoNb(0);
}

void KlustersDoc::prepareUndo(Q3ValueList<int>* addedClustersTemp,Q3ValueList<int>* modifiedClustersTemp,Q3ValueList<int>* deletedClustersTemp,bool isModifiedByDeletion){
  //Prepare the undo for the cluster palette
  prepareClusterColorUndo();

  //Store the current addedClusters in the undo list and make the temporary become the current one.
  addedClustersUndoList.prepend(addedClusters);
  addedClusters = addedClustersTemp;

  //Store the current modifiedClusters in the undo list and make the temporary become the current one.
  modifiedClustersUndoList.prepend(modifiedClusters);
  modifiedClusters = modifiedClustersTemp;

  //Store the current deletedClusters in the undo list and make the temporary become the current one.
  deletedClustersUndoList.prepend(deletedClusters);
  deletedClusters = deletedClustersTemp;

  //The renumbering actions which were redo are now lost
  Q3ValueList<int>::iterator iterator;
  for(iterator = renumberingRedoList.begin(); iterator != renumberingRedoList.end(); ++iterator){
    clusterIdsOldNewMap.remove(*iterator);
    clusterIdsNewOldMap.remove(*iterator);
  }
  renumberingRedoList.clear();

  //if the number of undo has been reach remove the last element in the undo lists (first inserted)
  int currentNbUndo = addedClustersUndoList.count();
  if(currentNbUndo > nbUndo){
   addedClustersUndoList.remove(currentNbUndo - 1);
   modifiedClustersUndoList.remove(currentNbUndo - 1);
   deletedClustersUndoList.remove(currentNbUndo - 1);
   modifiedClustersByDeleteUndo.remove(currentNbUndo);
   if(isModifiedByDeletion) modifiedClustersByDeleteUndo.append(currentNbUndo - 1);

   //The clusterIdsOldNew and clusterIdsNewOld maps are associated with
   //undo numbers. As the meaning of the numbers change (first undo will not be accessible anymore,
   //and the following ones are shift by one down (2->1, 3->2 etc..)), the maps have to be updated accordingly.
   if(clusterIdsOldNewMap.count() == 1 && clusterIdsOldNewMap.contains(1)){
     clusterIdsOldNewMap.remove(1);
     clusterIdsNewOldMap.remove(1);
   }
   else{
     for(int i = 2; i <= nbUndo; ++i){
      if(!clusterIdsOldNewMap.contains(i)) continue;
      QMap<int,int> clusterIdsOldNew = clusterIdsOldNewMap[i];
      clusterIdsOldNewMap.insert(i-1,clusterIdsOldNew);
      QMap<int,int> clusterIdsNewOld = clusterIdsNewOldMap[i];
      clusterIdsNewOldMap.insert(i-1,clusterIdsNewOld);
     }
     //remove the map entries with the bigger key (has not be taken into account by the previous loop)
     Q3ValueList<int> undoNbs = clusterIdsOldNewMap.keys();
     qSort(undoNbs);
     int biggerUndo = undoNbs.last();
     clusterIdsOldNewMap.remove(biggerUndo);
     clusterIdsNewOldMap.remove(biggerUndo);
   }
  }
  else if(isModifiedByDeletion) modifiedClustersByDeleteUndo.append(currentNbUndo);

  //Clear the redoLists
  addedClustersRedoList.clear();
  modifiedClustersRedoList.clear();
  deletedClustersRedoList.clear();
}



void KlustersDoc::nbUndoChangedCleaning(int newNbUndo){
 //if the new number of possible undo is smaller than the current one,
 // clean the undo/redo related variables.
 if(newNbUndo < nbUndo){
  //Make data clean its internal variables
  clusteringData->nbUndoChangedCleaning(newNbUndo);

  //Process the renumbering variables. All the undo indices in renumberingRedoList which
  //are bigger than newNbUndo will not be accesible any more, delete them.
  Q3ValueList<int>::iterator iterator;
  Q3ValueList<int> suppressIndices;
  for(iterator = renumberingRedoList.begin(); iterator != renumberingRedoList.end(); ++iterator){
    if(*iterator > newNbUndo){
     clusterIdsOldNewMap.remove(*iterator);
     clusterIdsNewOldMap.remove(*iterator);
     suppressIndices.append(*iterator);
    }
  }
  for(iterator = suppressIndices.begin(); iterator != suppressIndices.end(); ++iterator)
    renumberingRedoList.remove(*iterator);

  int currentNbUndo = clusterColorListUndoList.count();

  //if the current number of undo is bigger than the new number of undo,
  // remove the last elements in the undo lists (first ones inserted).
  if(currentNbUndo > newNbUndo){
   while(currentNbUndo > newNbUndo){
    addedClustersUndoList.remove(currentNbUndo - 1);
    modifiedClustersUndoList.remove(currentNbUndo - 1);
    deletedClustersUndoList.remove(currentNbUndo - 1);
    clusterColorListUndoList.remove(currentNbUndo - 1);
    modifiedClustersByDeleteUndo.remove(currentNbUndo);

    //The clusterIdsOldNew and clusterIdsNewOld maps are associated with
    //undo numbers. As the meaning of the numbers change (first undo will not be accessible anymore,
    //and the following ones are shift by one down (2->1, 3->2 etc..)), the maps have to be updated accordingly.
    if(clusterIdsOldNewMap.count() == 1 && clusterIdsOldNewMap.contains(1)){
      clusterIdsOldNewMap.remove(1);
      clusterIdsNewOldMap.remove(1);
    }
    else{
      for(int i = 2; i <= currentNbUndo; ++i){
       if(!clusterIdsOldNewMap.contains(i)) continue;
       QMap<int,int> clusterIdsOldNew = clusterIdsOldNewMap[i];
       clusterIdsOldNewMap.insert(i-1,clusterIdsOldNew);
       QMap<int,int> clusterIdsNewOld = clusterIdsNewOldMap[i];
       clusterIdsNewOldMap.insert(i-1,clusterIdsNewOld);
     }
     //remove the map entries with the bigger key (has not be taken into account by the previous loop)
     Q3ValueList<int> undoNbs = clusterIdsOldNewMap.keys();
     qSort(undoNbs);
     int biggerUndo = undoNbs.last();
     clusterIdsOldNewMap.remove(biggerUndo);
     clusterIdsNewOldMap.remove(biggerUndo);
    }

    currentNbUndo = clusterColorListUndoList.count();
   }
   //clear the redo lists
   addedClustersRedoList.clear();
   modifiedClustersRedoList.clear();
   deletedClustersRedoList.clear();
   clusterColorListRedoList.clear();
  }
  //currentNbUndo < newNbUndo, check the redo list.
  else{
   //number of undo and redo must be <= new number of undo. Remove redo elements if need it.
   int currentNbRedo = clusterColorListRedoList.count();
   if((currentNbRedo + currentNbUndo) > newNbUndo){
    while((currentNbRedo + currentNbUndo) > newNbUndo){
     addedClustersRedoList.remove(currentNbRedo - 1);
     modifiedClustersRedoList.remove(currentNbRedo - 1);
     deletedClustersRedoList.remove(currentNbRedo - 1);
     clusterColorListRedoList.remove(currentNbRedo - 1);
     modifiedClustersByDeleteRedo.remove(currentNbRedo);

     currentNbRedo = clusterColorListRedoList.count();
    }
   }
  }

  //Make the views clean its internal variables
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
   view->nbUndoChangedCleaning(newNbUndo);

  //Signal to klusters the new number of undo and redo
  emit updateUndoNb(clusterColorListUndoList.count());
  emit updateRedoNb(clusterColorListRedoList.count());
 }
}


void KlustersDoc::prepareUndo(){
  //Create a new empty list of created clusters
  Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();

  //Create a new empty list of modified clusters
  Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();

  //Create a new empty list of deleted clusters
  Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();

  prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::prepareUndo(int newCluster,Q3ValueList<int>& deletedClusters){
  //Create a new list of created clusters
  Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();
  addedClustersTemp->append(newCluster);

  //Create a new list of modified clusters
  Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();

  //Create a new list of deleted clusters
  Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();
  Q3ValueList<int>::iterator iterator;
  for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   deletedClustersTemp->append(*iterator);

  prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::prepareUndo(Q3ValueList<int>& modifiedClusters,Q3ValueList<int>& deletedClusters,bool isModifiedByDeletion){
 //Create a new empty list of created clusters
  Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();

  //Create a new list of modified clusters
  Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();
  Q3ValueList<int>::iterator iterator;
  for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
   modifiedClustersTemp->append(*iterator);

  //Create a new list of deleted clusters
  Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();
  for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   deletedClustersTemp->append(*iterator);

  prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp,isModifiedByDeletion);
}

void KlustersDoc::prepareUndo(int newCluster, Q3ValueList<int>& modifiedClusters,Q3ValueList<int>& deletedClusters,bool isModifiedByDeletion){
 //Create a new empty list of created clusters
  Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();
  addedClustersTemp->append(newCluster);

  //Create a new list of modified clusters
  Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();
  Q3ValueList<int>::iterator iterator;
  for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
   modifiedClustersTemp->append(*iterator);

  //Create a new list of deleted clusters
  Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();
  for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   deletedClustersTemp->append(*iterator);

  prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp,isModifiedByDeletion);
}

void KlustersDoc::prepareUndo(Q3ValueList<int>& newClusters, Q3ValueList<int>& modifiedClusters,Q3ValueList<int>& deletedClusters){
 //Create a new list of created clusters
  Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();
  Q3ValueList<int>::iterator iterator;
  for (iterator = newClusters.begin(); iterator != newClusters.end(); ++iterator)
   addedClustersTemp->append(*iterator);

  //Create a new list of modified clusters
  Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();
  for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
   modifiedClustersTemp->append(*iterator);

  //Create a new list of deleted clusters
  Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();
  for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
   deletedClustersTemp->append(*iterator);

  prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}


void KlustersDoc::prepareUndo(QMap<int,int> clusterIdsOldNew,QMap<int,int> clusterIdsNewOld){
  prepareUndo();

  //Update the renumbering lists
  int currentNbUndo = clusterColorListUndoList.count();
  cout<<"currentNbUndo in KlustersDoc::prepareUndo: "<<currentNbUndo<<endl;
  clusterIdsOldNewMap.insert(currentNbUndo,clusterIdsOldNew);
  clusterIdsNewOldMap.insert(currentNbUndo,clusterIdsNewOld);
}


void KlustersDoc::prepareReclusteringUndo(Q3ValueList<int>& newClusters,Q3ValueList<int>& deletedClusters){
 //Create a new list of created clusters
 Q3ValueList<int>* addedClustersTemp = new Q3ValueList<int>();
 Q3ValueList<int>::iterator iterator;
 for(iterator = newClusters.begin(); iterator != newClusters.end(); ++iterator)
  addedClustersTemp->append(*iterator);

 //Create a new list of modified clusters
 Q3ValueList<int>* modifiedClustersTemp = new Q3ValueList<int>();

 //Create a new list of deleted clusters
 Q3ValueList<int>* deletedClustersTemp = new Q3ValueList<int>();
 for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
  deletedClustersTemp->append(*iterator);

 prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::undo(){

cout<<"in KlustersDoc::undo 1"<<endl;

 //Update the boolean modified here as every undo action implies a call to the function.
 //The user can save and make an undo just behind, in that case the document is modified.
 modified = true;

 //Get the active view.
 KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

 clusteringData->undo(*addedClusters,*modifiedClusters);

 //If clusterColorListUndoList is not empty, make the current clusterColorList become the first element
 //of the clusterColorListRedoList and the first element of the clusterColorListUndoList become the current clusterColorList
 //do the same for the addedClusters and modifiedClusters Lists.
 if(clusterColorListUndoList.count()>0){
  clusterColorListRedoList.prepend(clusterColorList);
  ItemColors* clusterColorListTemp = clusterColorListUndoList.take(0);
  clusterColorList =  clusterColorListTemp;

  int nbUndo = clusterColorListUndoList.count();

  cout << "nbUndo in KlustersDoc::undo: "<<nbUndo<< endl;

  //If this undo does concern renumbering
  if(clusterIdsNewOldMap.contains(nbUndo + 1)){
    cout << "renumber in KlustersDoc::undo, nbUndo + 1 : "<<nbUndo + 1<< endl;
    //Add the current undo indice to the renumberingRedoList
    renumberingRedoList.append(nbUndo + 1);

   //Notify all the views of the undo
   KlustersView* view;
   for(view = viewList->first(); view!=0; view = viewList->next())
    if(view != activeView){
     view->undoRenumbering(clusterIdsNewOldMap[nbUndo + 1],false);
     //update the TraceView if any
     view->updateTraceView(electrodeGroupID,clusterColorList,false);
    }
    else{
     view->undoRenumbering(clusterIdsNewOldMap[nbUndo + 1],true);
     //update the TraceView if any
     view->updateTraceView(electrodeGroupID,clusterColorList,true);
    }

    //Notify the errorMatrixView of the modification
    emit undoRenumbering(clusterIdsNewOldMap[nbUndo + 1]);
  }
  else{
    if(modifiedClustersByDeleteUndo.contains(nbUndo + 1) != 0){
     modifiedClustersByDeleteUndo.remove(nbUndo + 1);
     int nbRedo = clusterColorListRedoList.count();
     modifiedClustersByDeleteRedo.append(nbRedo);
    }

    //Notify all the views of the undo
    KlustersView* view;
    if(addedClusters->size() > 0 && modifiedClusters->size() > 0){
       cout << "addedClusters->size() > 0 && modifiedClusters->size() > 0"<< endl;
      for(view = viewList->first(); view!=0; view = viewList->next())
       if(view != activeView){
        view->undo(*addedClusters,*modifiedClusters,false);
        //update the TraceView if any
        view->updateTraceView(electrodeGroupID,clusterColorList,false);
       }
       else{
        view->undo(*addedClusters,*modifiedClusters,true);
        //update the TraceView if any
        view->updateTraceView(electrodeGroupID,clusterColorList,true);
       }

     //Notify the errorMatrixView of the modification
     emit undoAdditionModification(*addedClusters,*modifiedClusters);
    }
    else if(addedClusters->size() > 0 && modifiedClusters->size() == 0){
      cout << "addedClusters->size() > 0 && modifiedClusters->size() == 0"<< endl;
     for(view = viewList->first(); view!=0; view = viewList->next())
      if(view != activeView){
       view->undoAddedClusters(*addedClusters,false);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,false);
      }
      else{
       view->undoAddedClusters(*addedClusters,true);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,true);
      }

     //Notify the errorMatrixView of the modification
     emit undoAddition(*addedClusters);
    }
    else if(addedClusters->size() == 0 && modifiedClusters->size() > 0){
      cout << "addedClusters->size() == 0 && modifiedClusters->size() > 0"<< endl;
     for(view = viewList->first(); view!=0; view = viewList->next())
      if(view != activeView){
       view->undoModifiedClusters(*modifiedClusters,false);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,false);
      }
      else{
       view->undoModifiedClusters(*modifiedClusters,true);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,true);
      }

     //Notify the errorMatrixView of the modification
     emit undoModification(*modifiedClusters);
    }
    //////!!!!This last condition should not be reach anymore, to test and remove.!!!!!////
    else if(addedClusters->size() == 0 && modifiedClusters->size() == 0){
      cout << "addedClusters->size() == 0 && modifiedClusters->size() == 0"<< endl;
     for(view = viewList->first(); view!=0; view = viewList->next())
      if(view != activeView){
       view->undo(false);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,false);
      }
      else{
       view->undo(true);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,true);
      }
    }
  }
  addedClustersRedoList.prepend(addedClusters);
  Q3ValueList<int>* addedClustersTemp = addedClustersUndoList.take(0);
  addedClusters =  addedClustersTemp;

  modifiedClustersRedoList.prepend(modifiedClusters);
  Q3ValueList<int>* modifiedClustersTemp = modifiedClustersUndoList.take(0);
  modifiedClusters =  modifiedClustersTemp;

  deletedClustersRedoList.prepend(deletedClusters);
  Q3ValueList<int>* deletedClustersTemp = deletedClustersUndoList.take(0);
  deletedClusters =  deletedClustersTemp;

  Q3ValueList<int> clustersToShow = activeView->clusters();

  //Call redraw on the active view
  activeView->showAllWidgets();

  //Update the clusterPalette
  clusterPalette.updateClusterList();
  clusterPalette.selectItems(clustersToShow);

  //Signal to klusters the new number of undo and redo
  emit updateUndoNb(clusterColorListUndoList.count());
  emit updateRedoNb(clusterColorListRedoList.count());
 }

 cout<<"in KlustersDoc::undo 2"<<endl;
}


void KlustersDoc::redo(){
  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  //Update the boolean modified here as every redo action implies a call to the function.
  //The user can save and make an redo just behind, in that case the document is modified.
  modified = true;

  //If clusterColorListRedoList is not empty, make the current clusterColorList become the first element
  //of the clusterColorListUndoList and the first element of the clusterColorListRedoList become the current clusterColorList
  //do the same for the addedClusters and modifiedClusters Lists.
  if(clusterColorListRedoList.count()>0){
    clusterColorListUndoList.prepend(clusterColorList);
    ItemColors* clusterColorListTemp = clusterColorListRedoList.take(0);
    clusterColorList =  clusterColorListTemp;

    addedClustersUndoList.prepend(addedClusters);
    Q3ValueList<int>* addedClustersTemp = addedClustersRedoList.take(0);
    addedClusters =  addedClustersTemp;

    modifiedClustersUndoList.prepend(modifiedClusters);
    Q3ValueList<int>* modifiedClustersTemp = modifiedClustersRedoList.take(0);
    modifiedClusters =  modifiedClustersTemp;

    deletedClustersUndoList.prepend(deletedClusters);
    Q3ValueList<int>* deletedClustersTemp = deletedClustersRedoList.take(0);
    deletedClusters =  deletedClustersTemp;

    clusteringData->redo(*addedClusters,*modifiedClusters,*deletedClusters);

    //If this redo does concern renumbering
    int nbUndo = clusterColorListUndoList.count();

     cout << "in KlustersDoc::redo, nbUndo  : "<<nbUndo<< endl;

    if(clusterIdsOldNewMap.contains(nbUndo)){
       cout << "renumber in KlustersDoc::redo, nbUndo  : "<<nbUndo<< endl;
     //remove the current undo indice from the renumberingRedoList
     renumberingRedoList.remove(nbUndo);

     //Notify all the views of the undo
     KlustersView* view;
     for(view = viewList->first(); view!=0; view = viewList->next())
      if(view != activeView){
       view->redoRenumbering(clusterIdsOldNewMap[nbUndo],false);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,false);
      }
      else{
       view->redoRenumbering(clusterIdsOldNewMap[nbUndo],true);
       //update the TraceView if any
       view->updateTraceView(electrodeGroupID,clusterColorList,true);
      }

      //Notify the errorMatrixView of the modification
      emit redoRenumbering(clusterIdsOldNewMap[nbUndo]);
    }
    else{
      int nbRedo = clusterColorListRedoList.count();
      bool isModifiedByDeletion = false;
      if(modifiedClustersByDeleteRedo.contains(nbRedo + 1) != 0){
       isModifiedByDeletion = true;
       modifiedClustersByDeleteRedo.remove(nbRedo + 1);
       int nbUndo = clusterColorListUndoList.count();
       modifiedClustersByDeleteUndo.append(nbUndo);
      }

      //Notify all the views of the undo
      KlustersView* view;
      if(addedClusters->size() > 0 && modifiedClusters->size() > 0){
         cout << "in KlustersDoc::redo, nbUndo  addedClusters->size() > 0 && modifiedClusters->size()>0"<< endl;
       for(view = viewList->first(); view!=0; view = viewList->next())
        if(view != activeView){
         view->redo(*addedClusters,*modifiedClusters,isModifiedByDeletion,false,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,false);
        }
        else{
         view->redo(*addedClusters,*modifiedClusters,isModifiedByDeletion,true,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,true);
        }

       //Notify the errorMatrixView of the modification
       emit redoAdditionModification(*addedClusters,*modifiedClusters,isModifiedByDeletion,*deletedClusters);
      }
      else if(addedClusters->size() > 0 && modifiedClusters->size() == 0){
         cout << "in KlustersDoc::redo, nbUndo  addedClusters->size() > 0 && modifiedClusters->size()==0"<< endl;
       for(view = viewList->first(); view!=0; view = viewList->next())
        if(view != activeView){
         view->redoAddedClusters(*addedClusters,false,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,false);
        }
        else{
         view->redoAddedClusters(*addedClusters,true,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,true);
        }

       //Notify the errorMatrixView of the modification
       emit redoAddition(*addedClusters,*deletedClusters);
      }
      else if(addedClusters->size() == 0 && modifiedClusters->size() > 0){
         cout << "in KlustersDoc::redo, nbUndo  addedClusters->size() == 0 && modifiedClusters->size()>0"<< endl;
        for(view = viewList->first(); view!=0; view = viewList->next())
        if(view != activeView){
         view->redoModifiedClusters(*modifiedClusters,isModifiedByDeletion,false,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,false);
        }
        else{
         view->redoModifiedClusters(*modifiedClusters,isModifiedByDeletion,true,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,true);
        }

       //Notify the errorMatrixView of the modification
       emit redoModification(*modifiedClusters,isModifiedByDeletion,*deletedClusters);
      }
      else if(addedClusters->size() == 0 && modifiedClusters->size() == 0){
         cout << "in KlustersDoc::redo, nbUndo  addedClusters->size() == 0 && modifiedClusters->size() ==0"<< endl;
       for(view = viewList->first(); view!=0; view = viewList->next())
        if(view != activeView){
         view->redo(false,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,false);
        }
        else{
         view->redo(true,*deletedClusters);
         //update the TraceView if any
         view->updateTraceView(electrodeGroupID,clusterColorList,true);
        }

       //Notify the errorMatrixView of the modification
       emit redoDeletion(*deletedClusters);
      }
    }

 cout << "in KlustersDoc::redo, 2  : "<< endl;

    Q3ValueList<int> clustersToShow = activeView->clusters();

    //Call redraw on the active view
    activeView->showAllWidgets();
    //Update the clusterPalette
    clusterPalette.updateClusterList();

 cout << "in KlustersDoc::redo, 3 b : "<< endl;

    clusterPalette.selectItems(clustersToShow);

      cout << "in KlustersDoc::redo, 4  : "<< endl;

    //Signal to klusters the new number of undo and redo
    emit updateUndoNb(clusterColorListUndoList.count());
    emit updateRedoNb(clusterColorListRedoList.count());

      cout << "in KlustersDoc::redo, end  : "<< endl;
 }
}

void KlustersDoc::renumberClusters(){
  //Get the active view.
  KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

  QMap<int,int> clusterIdsOldNew;
  QMap<int,int> clusterIdsNewOld;

  clusteringData->renumber(clusterIdsOldNew,clusterIdsNewOld);

  prepareUndo(clusterIdsOldNew,clusterIdsNewOld);

  //Update the clusterColorList, keep the same colors, only update the clusterIds
  Q3ValueList<dataType> clusterList = clusteringData->clusterIds();
  int nbClusters = clusterList.size();

  for(int i = 0; i < nbClusters; ++i){
    int clusterId = static_cast<int>(clusterList[i]);
    clusterColorList->changeItemId(i,clusterId);
  }

  //Notify all the views of the modification
  KlustersView* view;
  for(view = viewList->first(); view!=0; view = viewList->next())
  {
   if(view != activeView){
    view->renumberClusters(clusterIdsOldNew,false);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,false);
   }
   else{
    view->renumberClusters(clusterIdsOldNew,true);
    //update the TraceView if any
    view->updateTraceView(electrodeGroupID,clusterColorList,true);
   }
  }

  //Notify the errorMatrixView of the modification
  emit renumber(clusterIdsOldNew);

  //Reset the color status in clusterColors if need it
  if(clusterColorList->isColorChanged()) clusterColorList->resetAllColorStatus();

  activeView->showAllWidgets();

  //Update the palette of cluster
  clusterPalette.updateClusterList();
  clusterPalette.selectItems(activeView->clusters());
}

int KlustersDoc::createFeatureFile(Q3ValueList<int>& clustersToRecluster,QString reclusteringFetFileName){
  QFile fetFile(reclusteringFetFileName);
  if(!fetFile.open(QIODevice::WriteOnly)) return OPEN_ERROR;

  //Create the file
  clusteringData->createFeatureFile(clustersToRecluster,fetFile);
  fetFile.close();
  int status = fetFile.status();
  if(status == IO_Ok) return OK;
  else return CREATION_ERROR;
}

int KlustersDoc::integrateReclusteredClusters(Q3ValueList<int>& clustersToRecluster,Q3ValueList<int>& reclusteredClusterList,QString reclusteringFetFileName){

 QString cluFileName(reclusteringFetFileName);
 cluFileName.replace(".fet.",".clu.");

  QString cluFileUrl(cluFileName);
  QString tmpCluFile;
  if(KIO::NetAccess::exists(cluFileUrl))
    if(!KIO::NetAccess::download(cluFileUrl,tmpCluFile)){
     if(!QFile::remove(reclusteringFetFileName))
      KMessageBox::error(0,tr("Could not delete the temporary feature file used by the reclustering program."), tr("Warning !"));
     if(!QFile::remove(cluFileName))
      KMessageBox::error(0,tr("Could not delete the temporary cluster file used by the reclustering program."), tr("Warning !"));
     return DOWNLOAD_ERROR;
    }

  FILE* cluFile = fopen(tmpCluFile.toLatin1(),"r");
  if(cluFile == NULL){
   KIO::NetAccess::removeTempFile(tmpCluFile);
   if(!QFile::remove(reclusteringFetFileName))
    KMessageBox::error(0,tr("Could not delete the temporary feature file used by the reclustering program."), tr("Warning !"));
    if(!QFile::remove(cluFileName))
     KMessageBox::error(0,tr("Could not delete the temporary cluster file used by the reclustering program."), tr("Warning !"));
   return OPEN_ERROR;
  }

  //Actually integrate the new clusters.
  if(!clusteringData->integrateReclusteredClusters(clustersToRecluster,reclusteredClusterList,cluFile)){
   fclose(cluFile);
   KIO::NetAccess::removeTempFile(tmpCluFile);
   if(!QFile::remove(reclusteringFetFileName))
    KMessageBox::error(0,tr("Could not delete the temporary feature file used by the reclustering program."), tr("Warning !"));
   if(!QFile::remove(cluFileName))
    KMessageBox::error(0,tr("Could not delete the temporary cluster file used by the reclustering program."), tr("Warning !"));
   return INCORRECT_CONTENT;
  }

  fclose(cluFile);
  KIO::NetAccess::removeTempFile(tmpCluFile);

  //Suppress the fet and clu files.
  if(!QFile::remove(reclusteringFetFileName))
    KMessageBox::error(0,tr("Could not delete the temporary feature file used by the reclustering program."), tr("Warning !"));
  if(!QFile::remove(cluFileName))
   KMessageBox::error(0,tr("Could not delete the temporary cluster file used by the reclustering program."), tr("Warning !"));

 return OK;
}

void KlustersDoc::reclusteringUpdate(Q3ValueList<int>& clustersToRecluster,Q3ValueList<int>& reclusteredClusterList){
  //Prepare the undo
  prepareReclusteringUndo(reclusteredClusterList,clustersToRecluster);

  //Check if the active view is a ProcessWidget
  bool isProcessWidget = dynamic_cast<KlustersApp*>(parent)->doesActiveDisplayContainProcessWidget();

  if(!isProcessWidget){
   //Get the active view.
   KlustersView* activeView = dynamic_cast<KlustersApp*>(parent)->activeView();

   Q3ValueList<int> clustersToShow;
   Q3ValueList<int>::const_iterator iterator;
   Q3ValueList<int> const clusters = activeView->clusters();
   for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator)
    clustersToShow.append(*iterator);

   //Add the new clusters in clusterColors and clustersToShow.
   QColor color;
   Q3ValueList<int>::iterator clustersToCreate;
   for(clustersToCreate = reclusteredClusterList.begin(); clustersToCreate != reclusteredClusterList.end(); ++clustersToCreate ){
    color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
    clusterColorList->append(*clustersToCreate,color);
    clustersToShow.append(*clustersToCreate);
   }

   //Remove all the reclustered clusters from clusterColors and clustersToShow.
   Q3ValueList<int>::iterator clustersToRemove;
   for (clustersToRemove = clustersToRecluster.begin(); clustersToRemove != clustersToRecluster.end(); ++clustersToRemove ){
    clusterColorList->remove(*clustersToRemove);
    clustersToShow.remove(*clustersToRemove);
   }

   //Notify all the views of the modification
   KlustersView* view;
   for(view = viewList->first(); view!=0; view = viewList->next()){
    if(view != activeView){
     view->addNewClustersToView(clustersToRecluster,reclusteredClusterList,false);
     //update the TraceView if any
     view->updateTraceView(electrodeGroupID,clusterColorList,false);
    }
    else{
     view->addNewClustersToView(clustersToRecluster,reclusteredClusterList,true);
     //update the TraceView if any
     view->updateTraceView(electrodeGroupID,clusterColorList,true);
    }
   }

   //Notify the errorMatrixView of the modification
   emit newClustersAdded(clustersToRecluster);

   activeView->showAllWidgets();

   //Update the palette of cluster
   clusterPalette.updateClusterList();
   clusterPalette.selectItems(clustersToShow);
  }
  else{//processWidget
   //Add the new clusters in clusterColors.
   QColor color;
   Q3ValueList<int>::iterator clustersToCreate;
   for(clustersToCreate = reclusteredClusterList.begin(); clustersToCreate != reclusteredClusterList.end(); ++clustersToCreate ){
    color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
    clusterColorList->append(*clustersToCreate,color);
   }

   //Remove all the reclustered clusters from clusterColors and clustersToShow.
   Q3ValueList<int>::iterator clustersToRemove;
   for (clustersToRemove = clustersToRecluster.begin(); clustersToRemove != clustersToRecluster.end(); ++clustersToRemove ){
    clusterColorList->remove(*clustersToRemove);
   }

   //Notify all the views of the modification
   KlustersView* view;
   for(view = viewList->first(); view!=0; view = viewList->next())
    if(!view->isA("ProcessWidget")){
     view->addNewClustersToView(clustersToRecluster,reclusteredClusterList,false);
     //update the TraceView if any
     view->updateTraceView(electrodeGroupID,clusterColorList,false);
    }

   //Notify the errorMatrixView of the modification
   emit newClustersAdded(clustersToRecluster);

   //Update the palette of cluster
   clusterPalette.updateClusterList();
   Q3ValueList<int> emptyList;
   clusterPalette.selectItems(emptyList);
  }
}

void KlustersDoc::createProviders(){
 QString datUrl(docUrl);
 datUrl.setFileName(baseName +".dat");

 int resolution = clusteringData->getResolution();
 int voltageRange = clusteringData->getVoltageRange();
 double samplingRate = clusteringData->getSamplingRate();
 int channelNb = clusteringData->getTotalNbChannels();

 //Create the tracesProviders
 tracesProvider = new TracesProvider(datUrl.path(),channelNb,
                      resolution,samplingRate,clusteringData->getOffset());


 acquisitionGain = static_cast<int>(0.5 +
                       static_cast<float>(pow(static_cast<double>(2),static_cast<double>(resolution))
                       / static_cast<float>(voltageRange * 1000))
                       * clusteringData->getAmplification());

 //the screen grain is fixed to 0.2
 float screenGain = 0.2;
 gain = static_cast<int>(0.5 + screenGain * static_cast<float>(acquisitionGain));

 //Create the colorlist
 //Constructs the channelColorList, assign to all the channels the same blue color.
 //Put all the channels of the spike group corresponding to the open file in the same group(the electrodeGroupID)
 channelColorList = new ChannelColors();
 QColor color;
 Q3ValueList<int> group;
 color.setHsv(210,255,255);

 Q3ValueList<int>& currentChannels =  clusteringData->getCurrentChannels();
 Q3ValueList<int>::const_iterator iterator;
 for(iterator = currentChannels.begin(); iterator != currentChannels.end(); ++iterator){
  channelColorList->append(*iterator,color);
  displayChannelsGroups.insert(*iterator,electrodeGroupID.toInt());
  channelsSpikeGroups.insert(*iterator,electrodeGroupID.toInt());
  group.append(*iterator);
 }

 displayGroupsChannels.insert(electrodeGroupID.toInt(),group);
 spikeGroupsChannels.insert(electrodeGroupID.toInt(),group);

 ////Put all the other channels in the trash group (group 0).
 Q3ValueList<int> trashGroup;
 for(int i = 0; i < channelNb; ++i){
  if(!currentChannels.contains(i)){
   channelColorList->append(i,color);
   displayChannelsGroups.insert(i,0);
   channelsSpikeGroups.insert(i,0);
   trashGroup.append(i);
  }
 }

 displayGroupsChannels.insert(0,trashGroup);
 spikeGroupsChannels.insert(0,trashGroup);

 clustersProvider = new ClustersProvider(docUrl,samplingRate,samplingRate,*clusteringData,tracesProvider->getTotalNbSamples());

 //The current cluster file contains the data for the unique display group.
 Q3ValueList<int> list;
 list.append(electrodeGroupID.toInt());
 displayGroupsClusterFile.insert(electrodeGroupID.toInt(),list);
}


void KlustersDoc::showUserClusterInformation(){
	clusterPalette.showUserClusterInformation(electrodeGroupID.toInt());
}
#include "klustersdoc.moc"
