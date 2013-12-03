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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <QDir>
#include <QWidget>
#include <qstringlist.h>
#include <QString>
#include <qtimer.h>
#include <qdatetime.h>
#include <qapplication.h>

#include <QList>

#include <QEvent>
#include <QMessageBox>
#include <QDebug>
#include <QAction>
#include <QUrl>

// application specific includes
#include "processwidget.h"
#include "klusters.h"
#include "klustersdoc.h"
#include "klustersview.h"
#include "klustersdoc.h"
#include "clusterPalette.h"
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

KlustersDoc::KlustersDoc(QWidget* parent,ClusterPalette& clusterPalette,bool autoSave,int savingInterval)
    : clusterColorListUndoList(),clusterColorListRedoList(),modified(false),docUrl(),parent(parent),clusterPalette(clusterPalette),
    addedClustersUndoList(),addedClustersRedoList(),modifiedClustersUndoList(),modifiedClustersRedoList()
  ,autoSave(autoSave),savingInterval(savingInterval),tracesProvider(0L),clustersProvider(0L),channelColorList(0L)
{
    viewList = new QList<KlustersView*>();
    clusterColorList = 0L;
    addedClusters = 0L;
    modifiedClusters = 0L;
    deletedClusters = 0L;
    endAutoSaving = false;
    autoSaveThread = 0L;
}

KlustersDoc::~KlustersDoc(){
    qDebug() << "~KlustersDoc()";

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
        if(!autoSaveThread->isRunning()){
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
    viewList->removeAll(view);
}


bool KlustersDoc::isLastView() {
    return ((int) viewList->count() == 1);
}


void KlustersDoc::updateAllViews(KlustersView *sender){
    for(int i =0; i<viewList->count();++i)
    {
        KlustersView *view = viewList->at(i);
        view->update(sender);
    }

}

bool KlustersDoc::canCloseDocument(KlustersApp* mainWindow,const QString& callingMethod){
    //Before closing, make sure that there is no thread running.
    //Loop on all the views, moving to the next one when the current one has no more thread running.
    bool threadRunning = false;

    for(int i =0; i<viewList->count();++i)
    {
        KlustersView *view = viewList->at(i);
        threadRunning = view->isThreadsRunning();
        if(threadRunning)
            break;
    }

    if(threadRunning || !stopAutoSaving(true)){
        //Send an event to the klusters (main window) to let it know that the document can not
        //be close because some thread are still running.
        CloseDocumentEvent* event = getCloseDocumentEvent(callingMethod);
        QApplication::postEvent(mainWindow,event);
        return false;
    }
    else
        return true;
}

void KlustersDoc::closeDocument(){
    //If a document has been open reset the members
    viewList->clear();
    docUrl = QString();
    baseName.clear();
    xmlParameterFile.clear();
    qDeleteAll(clusterColorListUndoList);
    clusterColorListUndoList.clear();
    qDeleteAll(clusterColorListRedoList);
    clusterColorListRedoList.clear();
    qDeleteAll(addedClustersUndoList);
    addedClustersUndoList.clear();
    qDeleteAll(addedClustersRedoList);
    addedClustersRedoList.clear();
    qDeleteAll(modifiedClustersUndoList);
    modifiedClustersUndoList.clear();
    qDeleteAll(modifiedClustersUndoList);
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
    tmpCluFile.clear();
    tmpSpikeFile.clear();

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


bool KlustersDoc::importDocument(const QString &url, const char *format ){
    bool returnValue = true;

    //1 - Get the base name of the file
    //2 - load the config information: Parse the XML config file, initialize clusteringData (loadConfigFromNewFormat())
    //3 - load the spikes, clusters, time and PCA information (loadDataFromNewFormat())
    return  returnValue;
}

int KlustersDoc::openDocument(const QString &url,QString& errorInformation, const char *format ){
    //1 - Get the base name of the file
    //2 - load the config information: read the different files, initialize clusteringData (loadConfigFromNewFormat())
    //3 - load the spikes, clusters, time and PCA information (loadDataFromNewFormat())

    //Initialize the members specific to a document
    clusteringData = new Data();
    clusterColorList = new ItemColors();
    addedClusters = new QList<int>();
    modifiedClusters = new QList<int>();
    modified = false;

    //Store the baseName for future use
    QFileInfo urlFileInfo(url);

    QString fileName = urlFileInfo.fileName();
    const QStringList fileParts = fileName.split(".", QString::SkipEmptyParts);
    if(fileParts.count() < 3)
        return INCORRECT_FILE;
    baseName = fileParts[0];

    for(uint i = 1;i < fileParts.count()-2; ++i)
        baseName += "." + fileParts[i];

    electrodeGroupID = fileParts[fileParts.count()-1];

    //Create the files url to open (baseName.spk.x,baseName.clu.x,baseName.fet.x,baseName.par.x,baseName.par and baseName.xml)
    //and store the url (corresponding to the cluster file). If the xml format parameter file does not exist, the parameter files
    // baseName.par.x,baseName.par will be used otherwise the baseName.xml will be used.

    QString spkFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".spk."+ electrodeGroupID;

    QString cluFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".clu."+ electrodeGroupID;
    docUrl = cluFileUrl;

    cluFileSaveUrl = urlFileInfo.absolutePath() + QDir::separator() + "." + urlFileInfo.fileName() + ".autosave";


    QString fetFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".fet."+ electrodeGroupID;
    //Parameter files
    QString xmlParFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".xml";
    xmlParameterFile = xmlParFileUrl;



    QString parXFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".par."+ electrodeGroupID;


    QString parFileUrl = urlFileInfo.absolutePath() + QDir::separator() + baseName +".par";


    //Download the spike and fet files in temp files if necessary
    if(!QFile(spkFileUrl).exists())
        return SPK_DOWNLOAD_ERROR;
    QString tmpSpikeFile = spkFileUrl;


    QFile fetFile(fetFileUrl);
    if(!fetFile.exists())
        return FET_DOWNLOAD_ERROR;
    //Open the the spike and fet files. Only the fet file will be loaded the spike file
    // will be used on the fly when waveforms will need to be drawn.
    //The biggest files are open in a C FILE to enable a quick access, the others (parameter files) are open in a QFile
    if(!fetFile.open(QIODevice::ReadOnly)){
        return OPEN_ERROR;
    }

    //The length of the spike file is used to determine the number of spikes.
    QFile spikeFile(tmpSpikeFile);

    if(!spikeFile.open(QIODevice::ReadOnly)){
        fetFile.close();
        return OPEN_ERROR;
    }
    long spkFileLength = spikeFile.size();
    spikeFile.close();

    bool isXmlParExist = false;
    QString tmpXmlParFile;
    QString tmpParXFile;
    QString tmpParFile;
    QFileInfo xmlParFileInfo(xmlParFileUrl);
    QFile xmlParFile;
    QFile parXFile;
    QFile parFile;
    if(xmlParFileInfo.exists()){
        tmpXmlParFile = xmlParFileUrl;
        isXmlParExist = true;
        //Check if the generic parameter file also exist, if so, warn the user that the xml format parameter file will be used.
        QFileInfo parFileInfo(parFileUrl);
        if(parFileInfo.exists()){
            QApplication::restoreOverrideCursor();
            QMessageBox::information(0, tr("Warning!"), tr("Two parameter files were found, %1 and %2. The parameter file %3 will be used.").arg(xmlParFileUrl).arg(parFileUrl).arg(xmlParFileUrl));
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        }
        xmlParFile.setFileName(tmpXmlParFile);
        if(!xmlParFile.open(QIODevice::ReadOnly)){
            fetFile.close();
            return OPEN_ERROR;
        }
    }
    else{
        if(!QFile(parXFileUrl).exists()) {
            fetFile.close();
            return PARX_DOWNLOAD_ERROR;
        }
        tmpParXFile = parXFileUrl;
        parXFile.setFileName(tmpParXFile);
        if(!parXFile.open(QIODevice::ReadOnly)){
            fetFile.close();
            return OPEN_ERROR;
        }
        if(!QFile(parFileUrl).exists()) {
            fetFile.close();
            parXFile.close();
            return PAR_DOWNLOAD_ERROR;
        }
        tmpParFile = parFileUrl;
        parFile.setFileName(tmpParFile);
        if(!parFile.open(QIODevice::ReadOnly)){
            fetFile.close();
            parXFile.close();
            return OPEN_ERROR;
        }
    }

    //If a crashRecoveryFile exits, check if it is newer than the clu file, if so
    //ask the user if he wants to use that one to replace the clu file.
    QFileInfo crashFileInfo(cluFileSaveUrl);
    if(crashFileInfo.exists()){
        QFileInfo cluFileInfo(cluFileUrl);
        if((cluFileInfo.exists() && crashFileInfo.lastModified() > cluFileInfo.lastModified()) ||
                !cluFileInfo.exists()){
            QApplication::restoreOverrideCursor();
            switch(QMessageBox::question(0, tr("More recent cluster file found"), tr("A more recent copy of the cluster file (a rescue file) was found on the disk. This indicates that Klusters crashed while editing these data during a previous session.\n"
                                                   "Do you wish to use the newer copy (The old copy will be saved under another name)?"),QMessageBox::Yes|QMessageBox::No))
            {
            case QMessageBox::Yes:
                QDir dir(crashFileInfo.dir());
                QUrl cluName = cluFileInfo.fileName();
                bool renameStatus;
                if(cluFileInfo.exists()){
                    const QString newName = cluFileInfo.fileName()+ QLatin1String(".") + cluFileInfo.lastModified().toString("MM.dd.yyyy.hh.mm");
                    renameStatus = dir.rename(cluName.toString(),newName);
                }
                renameStatus = dir.rename(crashFileInfo.fileName(),cluName.toString());
                if(!renameStatus)
                    QMessageBox::critical(0, tr("I/O Error !"),tr(
                                              "It appears that the rescue file cannot be renamed (possibly because of insufficient file access permissions).\n"
                                              "The rescue file will thus not be used."));

                break;
            }
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        }
    }

    //Treat the cluster file separately as it can be empty
    QFile cluFile(cluFileUrl);
    if(cluFile.exists()){
        tmpCluFile = cluFileUrl;
        if(!cluFile.open(QIODevice::ReadOnly)) {
            if(isXmlParExist){
                xmlParFile.close();
            }
            else{
                parXFile.close();
                parFile.close();
            }
            fetFile.close();
            return OPEN_ERROR;
        }

        //Initialize the data
        if(isXmlParExist){
            if(!clusteringData->initialize(fetFile,cluFile,spkFileLength,tmpSpikeFile,xmlParFile,electrodeGroupID.toInt(),errorInformation)){
                //close the files
                xmlParFile.close();
                fetFile.close();
                cluFile.close();
                return INCORRECT_CONTENT;
            }
            xmlParFile.close();
            fetFile.close();
            cluFile.close();
        }
        else{
            if(!clusteringData->initialize(fetFile,cluFile,spkFileLength,tmpSpikeFile,parXFile,parFile,errorInformation)){
                //close the files
                parXFile.close();
                parFile.close();
                fetFile.close();
                cluFile.close();
                return INCORRECT_CONTENT;
            }
            parXFile.close();
            parFile.close();
            fetFile.close();
            cluFile.close();
        }
    }//end //the cluster file exists
    //the cluster file does not exist
    else{
        tmpCluFile =  cluFileUrl;

        //Initialize the data
        if(isXmlParExist){
            if(!clusteringData->initialize(fetFile,spkFileLength,tmpSpikeFile,xmlParFile,electrodeGroupID.toInt(),errorInformation)){
                //close the files
                xmlParFile.close();
                fetFile.close();
                return INCORRECT_CONTENT;
            }
            xmlParFile.close();
            fetFile.close();
        }
        else{
            if(!clusteringData->initialize(fetFile,spkFileLength,tmpSpikeFile,parXFile,parFile,errorInformation)){
                //close the files
                parXFile.close();
                parFile.close();
                fetFile.close();

                return INCORRECT_CONTENT;
            }
            //close the files
            parXFile.close();
            parFile.close();
            fetFile.close();
        }
    }//end the cluster file does not exist

    //Constructs the clusterColorList
    QList<dataType> clusterList = clusteringData->clusterIds();
    QList<dataType>::iterator it;
    for(it = clusterList.begin(); it != clusterList.end(); ++it){
        QColor color;
        if(*it == 1)
            color.setHsv(0,0,220);//Cluster 1 is always gray
        else
            color.setHsv(static_cast<int>(fmod(static_cast<double>(*it)*7,36))*10,255,255);
        clusterColorList->append(static_cast<int>(*it),color);
    }


    //If ask create a thread for the auto saving of the document.
    if(autoSave){
        qDebug()<<"autoSave = true in openDoc";
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
        if(!autoSaveThread->isRunning()){
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

void KlustersDoc::customEvent(QEvent *event){
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
        tmpCluFile =  saveUrl;
    }
    //Open the temp file in write mode
    FILE* cluFile = fopen(tmpCluFile.toLatin1(),"w");
    if(cluFile == NULL){
        tmpCluFile = tmpCluFileSave;
        return OPEN_ERROR;
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
        QFileInfo docUrlFileInfo(docUrl);
        QString fileName = docUrlFileInfo.fileName();
        const QStringList fileParts = fileName.split(".", QString::SkipEmptyParts);
        baseName = fileParts.first();
        if(fileParts.count() > 2)  {
            for(uint i = 1;i < fileParts.count()-2; ++i){
                baseName += "." + fileParts.at(i);
            }
        }
        if(fileParts.count() < 3)
            electrodeGroupID.clear();
        else
            electrodeGroupID = fileParts.at(fileParts.count()-1);

        QString xmlParFileUrl = docUrlFileInfo.absoluteFilePath() + QDir::separator()+ baseName +".xml";
        xmlParameterFile = xmlParFileUrl;
    }

    //Save the cluster user information if the xmlParameterFile exists
    //NB : for the moment, the specific errors are not return to the user, only a generic message (document could not be saved).
    if(clusteringData->isTraceViewVariablesAvailable()){
        //Save the document information
        qDebug()<<" xmlParameterFile"<<xmlParameterFile;
        QFileInfo parFileInfo = QFileInfo(xmlParameterFile);

        //Check that the file is writable
        if(!parFileInfo.isWritable()) {
            return NOT_WRITABLE;
        }

        QMap<int,ClusterUserInformation> clusterUserInformationMap = QMap<int,ClusterUserInformation>();
        clusteringData->getClusterUserInformation(electrodeGroupID.toInt(),clusterUserInformationMap);

        ParameterXmlModifier parameterModifier = ParameterXmlModifier();
        bool status = parameterModifier.parseFile(xmlParameterFile);
        if(!status)
            return PARSE_ERROR;

        status = parameterModifier.setClusterUserInformation(electrodeGroupID.toInt(),clusterUserInformationMap);
        if(!status)
            return CREATION_ERROR;

        status = parameterModifier.writeTofile(xmlParameterFile);
        if(!status)
            return CREATION_ERROR;
    }

    modified=false;
    return OK;
}


bool KlustersDoc::canCloseView(){
    bool returnValue = false;
    if(isModified()){
        QString saveURL;
        switch(QMessageBox::question(0, url(),tr("The current file has been modified.\n"
                                                 "Do you want to save it?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
        {
        case QMessageBox::Yes:
            saveURL=url();
            int saveStatus;
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            saveStatus = saveDocument(saveURL);
            QApplication::restoreOverrideCursor();
            if(saveStatus != OK){
                switch(QMessageBox::question(0, tr("I/O Error !"),tr("Could not save the current document !\n"
                                                                     "Close anyway ?"),QMessageBox::No|QMessageBox::Yes))
                {
                case QMessageBox::Yes:
                    returnValue = true;
                    modified = false;
                    break;
                case QMessageBox::No:
                    returnValue = false;
                    break;
                }
            }
            else{
                returnValue = true;
                modified = false;
            }
            break;
        case QMessageBox::No:
            returnValue = true;
            modified = false;
            break;
        case QMessageBox::Cancel:
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

QString KlustersDoc::documentName() const{
    QFileInfo docUrlFileInfo(docUrl);
    return docUrlFileInfo.absolutePath() + QDir::separator() + baseName + "-" + electrodeGroupID;
}

QString KlustersDoc::documentBaseName() const{
    return baseName;
}

QString KlustersDoc::documentDirectory() const {
    QFileInfo docUrlFileInfo(docUrl);
    return docUrlFileInfo.absolutePath();
}

void KlustersDoc::setGain(int acquisitionGain){
    //Notify all the views of the modification
    for(int i =0; i<viewList->count();++i) {
        KlustersView *view = viewList->at(i);
        view->setGain(acquisitionGain);
    }

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //Ask the active view to take the modification into account immediately
    activeView->showAllWidgets();
}

void KlustersDoc::setBackgroundColor(const QColor &backgroundColor){
    //Notify all the views of the modification
    for(int i =0; i<viewList->count();++i) {
        KlustersView *view = viewList->at(i);
        view->updateBackgroundColor(backgroundColor);
    }

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //Ask the active view to take the modification into account immediately
    activeView->showAllWidgets();
}

void KlustersDoc::setTimeStepInSecond(int step){
    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //Notify all the views of the modification
    for(int i =0; i<viewList->count();++i){
        KlustersView *view = viewList->at(i);
        if(view != activeView) view->setTimeStepInSecond(step,false);
        else view->setTimeStepInSecond(step,true);
    }

    //Ask the active view to take the modification into account immediately
    activeView->showAllWidgets();
}

void KlustersDoc::setChannelPositions(QList<int>& positions){
    //Notify all the views of the modification

    for(int i =0; i<viewList->count();++i) {
        KlustersView *view = viewList->at(i);
        view->setChannelPositions(positions);
    }

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //Ask the active view to take the modification into account immediately
    activeView->showAllWidgets();
}

void KlustersDoc::singleColorUpdate(int clusterId,KlustersView& activeView){
    //Notify all the views of the modification

    for(int i =0; i<viewList->count();++i)
    {
        KlustersView *view = viewList->at(i);
        if(view != &activeView) view->singleColorUpdate(clusterId,false);
        else view->singleColorUpdate(clusterId,true);
    }

    //Ask the active view to take the modification into account immediately
    activeView.showAllWidgets();
}


void KlustersDoc::shownClustersUpdate(QList<int> clustersToShow,KlustersView& activeView){
    if(clusterColorList->isColorChanged()){
        //Notify all the views of the modification

        for(int i =0; i<viewList->count();++i)
        {
            KlustersView *view = viewList->at(i);
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

void KlustersDoc::shownClustersUpdate(QList<int> clustersToShow){
    //Update the palette of cluster
    clusterPalette.selectItems(clustersToShow);

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //The new selection of clusters only means for the active view
    activeView->shownClustersUpdate(clustersToShow);

    //update the TraceView if any
    activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::shownClustersUpdate(QList<int> clustersToShow,QList<int> previousSelectedClusterPairs){
    //Get the clusters currently selected
    QList<int> currentShownClusters = clusterPalette.selectedClusters();

    //Add the clusters which were shown and not part of the previous selected cluster pairs
    QList<int>::iterator clustersToAdd;
    for(clustersToAdd = currentShownClusters.begin(); clustersToAdd != currentShownClusters.end(); ++clustersToAdd )
        if(!previousSelectedClusterPairs.contains(*clustersToAdd)) clustersToShow.append(*clustersToAdd);

    //Update the palette of cluster
    clusterPalette.selectItems(clustersToShow);

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //The new selection of clusters only means for the active view
    activeView->shownClustersUpdate(clustersToShow);

    //update the TraceView if any
    activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::showAllClustersExcept(QList<int> clustersToHide){

    QList<dataType> clusterList = clusteringData->clusterIds();
    QList<int> clustersToShow;

    QList<dataType>::iterator clustersToAdd;
    for(clustersToAdd = clusterList.begin(); clustersToAdd != clusterList.end(); ++clustersToAdd ){
        if(!clustersToHide.contains(static_cast<int>(*clustersToAdd))) clustersToShow.append(static_cast<int>(*clustersToAdd));
    }

    //Update the palette of cluster
    clusterPalette.selectItems(clustersToShow);

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //The new selection of clusters only means for the active view
    activeView->shownClustersUpdate(clustersToShow);

    //update the TraceView if any
    activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::addClustersToActiveView(QList<int> clustersToShow){
    //Get the clusters currently selected
    QList<int> currentShownClusters = clusterPalette.selectedClusters();

    QList<int>::iterator clustersToAdd;
    for(clustersToAdd = currentShownClusters.begin(); clustersToAdd != currentShownClusters.end(); ++clustersToAdd )
        clustersToShow.append(*clustersToAdd);

    //Update the palette of cluster
    clusterPalette.selectItems(clustersToShow);

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //The new selection of clusters only means for the active view
    activeView->shownClustersUpdate(clustersToShow);

    //update the TraceView if any
    activeView->updateTraceView(electrodeGroupID,clusterColorList,true);
}

void KlustersDoc::groupClusters(QList<int> clustersToGroup,KlustersView& activeView){
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
    QList<int>::iterator clustersToRemove;
    QList<int>::iterator clustersToRemoveEnd(clustersToGroup.end());
    for (clustersToRemove = clustersToGroup.begin(); clustersToRemove != clustersToRemoveEnd; ++clustersToRemove ){
        clusterColorList->remove(*clustersToRemove);
    }

    //Notify all the views of the modification

    for(int i =0; i<viewList->count();++i){
        KlustersView *view = viewList->at(i);
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
    if(clusterColorList->isColorChanged())
        clusterColorList->resetAllColorStatus();

    //Ask the active view to take the modification into account immediately
    activeView.showAllWidgets();

    //Update the palette of cluster
    clusterPalette.updateClusterList();
    QList<int> clustersToShow;
    clustersToShow.append(newClusterIdint);
    clusterPalette.selectItems(clustersToShow);
}


void KlustersDoc::deleteClusters(QList<int> clustersToDelete,KlustersView& activeView,int clusterId){
    QList<int> modifiedcluster;
    modifiedcluster.append(clusterId);

    //If only one cluster has been deleted, the following cluster on the list, if any, will be selected.
    //Find that following cluster.
    int clusterToSelect;
    bool existNextCluster = false;
    if(clustersToDelete.size() == 1){
        int clusterToDelete =  clustersToDelete[0];
        bool previous = false;
        QList<dataType> clusters = clusteringData->clusterIds();
        QList<dataType>::iterator clustersIterator;
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
            QColor color(Qt::red); //Cluster 01 is always red
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
    QList<int>::iterator clustersToRemove;
    for (clustersToRemove = clustersToDelete.begin(); clustersToRemove != clustersToDelete.end(); ++clustersToRemove ){
        if(*clustersToRemove == clusterId) continue;
        clusterColorList->remove(*clustersToRemove);
    }

    //Notify all the views of the modification

    for(int i =0; i<viewList->count();++i){
        KlustersView *view = viewList->at(i);
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
        QList<int> clusters;
        clusters.append(clusterToSelect);

        //Update the cluster palette
        clusterPalette.selectItems(clusters);
        activeView.shownClustersUpdate(clusters);

        //update the TraceView if any
        activeView.updateTraceView(electrodeGroupID,clusterColorList,true);
    }
}

void KlustersDoc::deleteArtifact(QRegion& region,const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
    deleteSpikesFromClusters(0,region,clustersOfOrigin,dimensionX,dimensionY);
}


void KlustersDoc::deleteNoise(QRegion& region,const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
    deleteSpikesFromClusters(1,region,clustersOfOrigin,dimensionX,dimensionY);
}

void KlustersDoc::deleteSpikesFromClusters(int destination, QRegion& region,const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
    //list which will contain the clusters really having spikes in the region of selection.
    QList <int> fromClusters;
    //list which will contain the clusters which became empty because all their spikes were in the region of selection.
    QList <int> emptyClusters;
    QList<int> clustersToShow(clustersOfOrigin);

    clusteringData->deleteSpikesFromClusters(region,clustersOfOrigin,destination,dimensionX,dimensionY,fromClusters,emptyClusters);

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //check if any spikes have been selected
    if(fromClusters.isEmpty()){
        activeView->selectionIsEmpty();
        activeView->showAllWidgets();
    }
    else{
        QList<int> updatedClusters = QList<int>(fromClusters);
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
            QColor color(Qt::red); //Cluster 01 is always red
            clusterColorList->insert(static_cast<int>(0),color,0);
        }
        else
            //Prepare the undo
            prepareUndo(updatedClusters,emptyClusters,true);

        //Remove all the empty clusters from clusterColors and clustersToShow
        if(!emptyClusters.isEmpty()){
            QList<int>::iterator clustersToRemove;
            for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
                clusterColorList->remove(*clustersToRemove);
                clustersToShow.removeAll(*clustersToRemove);
            }
        }

        //Notify all the views of the modification

        for(int i =0; i<viewList->count();++i){
            KlustersView *view = viewList->at(i);
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


void KlustersDoc::createNewCluster(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
    //list which will contain the clusters really having spikes in the region of selection.
    QList <int> fromClusters;
    //list which will contain the clusters which became empty because all their spikes were in the region of selection.
    QList <int> emptyClusters;
    QList<int> clustersToShow(clustersOfOrigin);
    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

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
        if(!emptyClusters.isEmpty()){
            QList<int>::iterator clustersToRemove;
            for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
                clusterColorList->remove(*clustersToRemove);
                clustersToShow.removeAll(*clustersToRemove);
            }
        }

        //Notify all the views of the modification

        for(int i =0; i<viewList->count();++i){
            KlustersView *view = viewList->at(i);
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

void KlustersDoc::createNewClusters(QRegion& region, const QList <int>& clustersOfOrigin, int dimensionX, int dimensionY){
    //list which will contain the clusters really having spikes in the region of selection.
    QList <int> fromClusters;
    //list which will contain the clusters which became empty because all their spikes were in the region of selection.
    QList <int> emptyClusters;
    QList<int> clustersToShow(clustersOfOrigin);
    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    QList <int> newClusters;
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
        QList<int>::iterator clustersToCreate;
        qSort(newClusters);
        for (clustersToCreate = newClusters.begin(); clustersToCreate != newClusters.end(); ++clustersToCreate ){
            color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
            clusterColorList->append(*clustersToCreate,color);
            clustersToShow.append(*clustersToCreate);
        }
        //Remove all the empty clusters
        if(!emptyClusters.isEmpty()){
            QList<int>::iterator clustersToRemove;
            for (clustersToRemove = emptyClusters.begin(); clustersToRemove != emptyClusters.end(); ++clustersToRemove ){
                clusterColorList->remove(*clustersToRemove);
                clustersToShow.removeAll(*clustersToRemove);
            }
        }

        //Notify all the views of the modification

        for(int i =0; i<viewList->count();++i){
            KlustersView *view = viewList->at(i);
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
        delete clusterColorListUndoList.takeAt(currentClusterColorsNbUndo - 1);

    //Clear the redoList
    qDeleteAll(clusterColorListRedoList);
    clusterColorListRedoList.clear();

    //Signal to klusters the new number of undo and redo
    emit updateUndoNb(clusterColorListUndoList.count());
    emit updateRedoNb(0);
}

void KlustersDoc::prepareUndo(QList<int>* addedClustersTemp,QList<int>* modifiedClustersTemp,QList<int>* deletedClustersTemp,bool isModifiedByDeletion){
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
    QList<int>::iterator iterator;
    for(iterator = renumberingRedoList.begin(); iterator != renumberingRedoList.end(); ++iterator){
        clusterIdsOldNewMap.remove(*iterator);
        clusterIdsNewOldMap.remove(*iterator);
    }
    renumberingRedoList.clear();

    //if the number of undo has been reach remove the last element in the undo lists (first inserted)
    int currentNbUndo = addedClustersUndoList.count();
    if(currentNbUndo > nbUndo){
        delete addedClustersUndoList.takeAt(currentNbUndo - 1);
        delete modifiedClustersUndoList.takeAt(currentNbUndo - 1);
        delete deletedClustersUndoList.takeAt(currentNbUndo - 1);
        modifiedClustersByDeleteUndo.removeAt(currentNbUndo);
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
            if(!clusterIdsOldNewMap.isEmpty()) {
                QList<int> undoNbs = clusterIdsOldNewMap.keys();
                qSort(undoNbs);
                int biggerUndo = undoNbs.last();
                clusterIdsOldNewMap.remove(biggerUndo);
                clusterIdsNewOldMap.remove(biggerUndo);
            }
        }
    }
    else if(isModifiedByDeletion) modifiedClustersByDeleteUndo.append(currentNbUndo);

    //Clear the redoLists
    qDeleteAll(addedClustersRedoList);
    addedClustersRedoList.clear();
    qDeleteAll(modifiedClustersRedoList);
    modifiedClustersRedoList.clear();
    qDeleteAll(deletedClustersRedoList);
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
        QList<int>::iterator iterator;
        QList<int> suppressIndices;
        for(iterator = renumberingRedoList.begin(); iterator != renumberingRedoList.end(); ++iterator){
            if(*iterator > newNbUndo){
                clusterIdsOldNewMap.remove(*iterator);
                clusterIdsNewOldMap.remove(*iterator);
                suppressIndices.append(*iterator);
            }
        }
        for(iterator = suppressIndices.begin(); iterator != suppressIndices.end(); ++iterator)
            renumberingRedoList.removeAll(*iterator);

        int currentNbUndo = clusterColorListUndoList.count();

        //if the current number of undo is bigger than the new number of undo,
        // remove the last elements in the undo lists (first ones inserted).
        if(currentNbUndo > newNbUndo){
            while(currentNbUndo > newNbUndo){
                delete addedClustersUndoList.takeAt(currentNbUndo - 1);
                delete modifiedClustersUndoList.takeAt(currentNbUndo - 1);
                delete deletedClustersUndoList.takeAt(currentNbUndo - 1);
                delete clusterColorListUndoList.takeAt(currentNbUndo - 1);
                modifiedClustersByDeleteUndo.removeAt(currentNbUndo);

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
                    QList<int> undoNbs = clusterIdsOldNewMap.keys();
                    qSort(undoNbs);
                    int biggerUndo = undoNbs.last();
                    clusterIdsOldNewMap.remove(biggerUndo);
                    clusterIdsNewOldMap.remove(biggerUndo);
                }

                currentNbUndo = clusterColorListUndoList.count();
            }
            //clear the redo lists
            qDeleteAll(addedClustersRedoList);
            addedClustersRedoList.clear();
            qDeleteAll(modifiedClustersRedoList);
            modifiedClustersRedoList.clear();
            qDeleteAll(deletedClustersRedoList);
            deletedClustersRedoList.clear();
            qDeleteAll(clusterColorListRedoList);
            clusterColorListRedoList.clear();
        }
        //currentNbUndo < newNbUndo, check the redo list.
        else{
            //number of undo and redo must be <= new number of undo. Remove redo elements if need it.
            int currentNbRedo = clusterColorListRedoList.count();
            if((currentNbRedo + currentNbUndo) > newNbUndo){
                while((currentNbRedo + currentNbUndo) > newNbUndo){
                    delete addedClustersRedoList.takeAt(currentNbRedo - 1);
                    delete modifiedClustersRedoList.takeAt(currentNbRedo - 1);
                    delete deletedClustersRedoList.takeAt(currentNbRedo - 1);
                    delete clusterColorListRedoList.takeAt(currentNbRedo - 1);
                    modifiedClustersByDeleteRedo.removeAt(currentNbRedo);

                    currentNbRedo = clusterColorListRedoList.count();
                }
            }
        }

        //Make the views clean its internal variables

        for(int i =0; i<viewList->count();++i) {
            KlustersView *view = viewList->at(i);
            view->nbUndoChangedCleaning(newNbUndo);
        }

        //Signal to klusters the new number of undo and redo
        emit updateUndoNb(clusterColorListUndoList.count());
        emit updateRedoNb(clusterColorListRedoList.count());
    }
}


void KlustersDoc::prepareUndo(){
    //Create a new empty list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();

    //Create a new empty list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();

    //Create a new empty list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::prepareUndo(int newCluster,QList<int>& deletedClusters){
    //Create a new list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();
    addedClustersTemp->append(newCluster);

    //Create a new list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();

    //Create a new list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
        deletedClustersTemp->append(*iterator);

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::prepareUndo(QList<int>& modifiedClusters,QList<int>& deletedClusters,bool isModifiedByDeletion){
    //Create a new empty list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();

    //Create a new list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
        modifiedClustersTemp->append(*iterator);

    //Create a new list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();
    for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
        deletedClustersTemp->append(*iterator);

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp,isModifiedByDeletion);
}

void KlustersDoc::prepareUndo(int newCluster, QList<int>& modifiedClusters,QList<int>& deletedClusters,bool isModifiedByDeletion){
    //Create a new empty list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();
    addedClustersTemp->append(newCluster);

    //Create a new list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
        modifiedClustersTemp->append(*iterator);

    //Create a new list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();
    for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
        deletedClustersTemp->append(*iterator);

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp,isModifiedByDeletion);
}

void KlustersDoc::prepareUndo(QList<int>& newClusters, QList<int>& modifiedClusters,QList<int>& deletedClusters){
    //Create a new list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for (iterator = newClusters.begin(); iterator != newClusters.end(); ++iterator)
        addedClustersTemp->append(*iterator);

    //Create a new list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();
    for (iterator = modifiedClusters.begin(); iterator != modifiedClusters.end(); ++iterator)
        modifiedClustersTemp->append(*iterator);

    //Create a new list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();
    for (iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
        deletedClustersTemp->append(*iterator);

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}


void KlustersDoc::prepareUndo(QMap<int,int> clusterIdsOldNew,QMap<int,int> clusterIdsNewOld){
    prepareUndo();

    //Update the renumbering lists
    int currentNbUndo = clusterColorListUndoList.count();
    qDebug()<<"currentNbUndo in KlustersDoc::prepareUndo: "<<currentNbUndo;
    clusterIdsOldNewMap.insert(currentNbUndo,clusterIdsOldNew);
    clusterIdsNewOldMap.insert(currentNbUndo,clusterIdsNewOld);
}


void KlustersDoc::prepareReclusteringUndo(QList<int>& newClusters,QList<int>& deletedClusters){
    //Create a new list of created clusters
    QList<int>* addedClustersTemp = new QList<int>();
    QList<int>::iterator iterator;
    for(iterator = newClusters.begin(); iterator != newClusters.end(); ++iterator)
        addedClustersTemp->append(*iterator);

    //Create a new list of modified clusters
    QList<int>* modifiedClustersTemp = new QList<int>();

    //Create a new list of deleted clusters
    QList<int>* deletedClustersTemp = new QList<int>();
    for(iterator = deletedClusters.begin(); iterator != deletedClusters.end(); ++iterator)
        deletedClustersTemp->append(*iterator);

    prepareUndo(addedClustersTemp, modifiedClustersTemp,deletedClustersTemp);
}

void KlustersDoc::undo(){

    qDebug()<<"in KlustersDoc::undo 1";

    //Update the boolean modified here as every undo action implies a call to the function.
    //The user can save and make an undo just behind, in that case the document is modified.
    modified = true;

    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    if(!activeView)
        return;
    clusteringData->undo(*addedClusters,*modifiedClusters);

    //If clusterColorListUndoList is not empty, make the current clusterColorList become the first element
    //of the clusterColorListRedoList and the first element of the clusterColorListUndoList become the current clusterColorList
    //do the same for the addedClusters and modifiedClusters Lists.
    if(clusterColorListUndoList.count()>0){
        clusterColorListRedoList.prepend(clusterColorList);
        ItemColors* clusterColorListTemp = clusterColorListUndoList.takeAt(0);
        clusterColorList =  clusterColorListTemp;

        int nbUndo = clusterColorListUndoList.count();

        qDebug() << "nbUndo in KlustersDoc::undo: "<<nbUndo<< endl;

        //If this undo does concern renumbering
        if(clusterIdsNewOldMap.contains(nbUndo + 1)){
            qDebug() << "renumber in KlustersDoc::undo, nbUndo + 1 : "<<nbUndo + 1<< endl;
            //Add the current undo indice to the renumberingRedoList
            renumberingRedoList.append(nbUndo + 1);

            //Notify all the views of the undo

            for(int i =0; i<viewList->count();++i) {
                KlustersView *view = viewList->at(i);
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
            }

            //Notify the errorMatrixView of the modification
            emit undoRenumbering(clusterIdsNewOldMap[nbUndo + 1]);
        }
        else{
            if(modifiedClustersByDeleteUndo.contains(nbUndo + 1) != 0){
                modifiedClustersByDeleteUndo.removeAll(nbUndo + 1);
                int nbRedo = clusterColorListRedoList.count();
                modifiedClustersByDeleteRedo.append(nbRedo);
            }

            //Notify all the views of the undo
            if(addedClusters->size() > 0 && modifiedClusters->size() > 0){
                qDebug() << "addedClusters->size() > 0 && modifiedClusters->size() > 0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit undoAdditionModification(*addedClusters,*modifiedClusters);
            }
            else if(!addedClusters->isEmpty() && modifiedClusters->isEmpty()){
                qDebug() << "addedClusters->size() > 0 && modifiedClusters->size() == 0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit undoAddition(*addedClusters);
            }
            else if(addedClusters->isEmpty() && !modifiedClusters->isEmpty()){
                qDebug() << "addedClusters->size() == 0 && modifiedClusters->size() > 0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit undoModification(*modifiedClusters);
            }
            //////!!!!This last condition should not be reach anymore, to test and remove.!!!!!////
            else if(addedClusters->size() == 0 && modifiedClusters->size() == 0){
                qDebug() << "addedClusters->size() == 0 && modifiedClusters->size() == 0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
        }
        addedClustersRedoList.prepend(addedClusters);
        QList<int>* addedClustersTemp = addedClustersUndoList.takeAt(0);
        addedClusters =  addedClustersTemp;

        modifiedClustersRedoList.prepend(modifiedClusters);
        QList<int>* modifiedClustersTemp = modifiedClustersUndoList.takeAt(0);
        modifiedClusters =  modifiedClustersTemp;

        deletedClustersRedoList.prepend(deletedClusters);
        QList<int>* deletedClustersTemp = deletedClustersUndoList.takeAt(0);
        deletedClusters =  deletedClustersTemp;

        QList<int> clustersToShow = activeView->clusters();

        //Call redraw on the active view
        activeView->showAllWidgets();

        //Update the clusterPalette
        clusterPalette.updateClusterList();
        clusterPalette.selectItems(clustersToShow);

        //Signal to klusters the new number of undo and redo
        emit updateUndoNb(clusterColorListUndoList.count());
        emit updateRedoNb(clusterColorListRedoList.count());
    }

    qDebug()<<"in KlustersDoc::undo 2";
}


void KlustersDoc::redo(){
    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    //Update the boolean modified here as every redo action implies a call to the function.
    //The user can save and make an redo just behind, in that case the document is modified.
    modified = true;

    //If clusterColorListRedoList is not empty, make the current clusterColorList become the first element
    //of the clusterColorListUndoList and the first element of the clusterColorListRedoList become the current clusterColorList
    //do the same for the addedClusters and modifiedClusters Lists.
    if(clusterColorListRedoList.count()>0){
        clusterColorListUndoList.prepend(clusterColorList);
        ItemColors* clusterColorListTemp = clusterColorListRedoList.takeAt(0);
        clusterColorList =  clusterColorListTemp;

        addedClustersUndoList.prepend(addedClusters);
        QList<int>* addedClustersTemp = addedClustersRedoList.takeAt(0);
        addedClusters =  addedClustersTemp;

        modifiedClustersUndoList.prepend(modifiedClusters);
        QList<int>* modifiedClustersTemp = modifiedClustersRedoList.takeAt(0);
        modifiedClusters =  modifiedClustersTemp;

        deletedClustersUndoList.prepend(deletedClusters);
        QList<int>* deletedClustersTemp = deletedClustersRedoList.takeAt(0);
        deletedClusters =  deletedClustersTemp;

        clusteringData->redo(*addedClusters,*modifiedClusters,*deletedClusters);

        //If this redo does concern renumbering
        int nbUndo = clusterColorListUndoList.count();

        qDebug() << "in KlustersDoc::redo, nbUndo  : "<<nbUndo<< endl;

        if(clusterIdsOldNewMap.contains(nbUndo)){
            qDebug() << "renumber in KlustersDoc::redo, nbUndo  : "<<nbUndo<< endl;
            //remove the current undo indice from the renumberingRedoList
            renumberingRedoList.removeAll(nbUndo);

            //Notify all the views of the undo
            for(int i =0; i<viewList->count();++i) {
                KlustersView *view = viewList->at(i);
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
            }

            //Notify the errorMatrixView of the modification
            emit redoRenumbering(clusterIdsOldNewMap[nbUndo]);
        }
        else{
            int nbRedo = clusterColorListRedoList.count();
            bool isModifiedByDeletion = false;
            if(modifiedClustersByDeleteRedo.contains(nbRedo + 1) != 0){
                isModifiedByDeletion = true;
                modifiedClustersByDeleteRedo.removeAt(nbRedo + 1);
                int nbUndo = clusterColorListUndoList.count();
                modifiedClustersByDeleteUndo.append(nbUndo);
            }

            //Notify all the views of the undo
            if(addedClusters->size() > 0 && modifiedClusters->size() > 0){
                qDebug() << "in KlustersDoc::redo, nbUndo  addedClusters->size() > 0 && modifiedClusters->size()>0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit redoAdditionModification(*addedClusters,*modifiedClusters,isModifiedByDeletion,*deletedClusters);
            }
            else if(addedClusters->size() > 0 && modifiedClusters->size() == 0){
                qDebug() << "in KlustersDoc::redo, nbUndo  addedClusters->size() > 0 && modifiedClusters->size()==0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit redoAddition(*addedClusters,*deletedClusters);
            }
            else if(addedClusters->size() == 0 && modifiedClusters->size() > 0){
                qDebug() << "in KlustersDoc::redo, nbUndo  addedClusters->size() == 0 && modifiedClusters->size()>0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit redoModification(*modifiedClusters,isModifiedByDeletion,*deletedClusters);
            }
            else if(addedClusters->size() == 0 && modifiedClusters->size() == 0){
                qDebug() << "in KlustersDoc::redo, nbUndo  addedClusters->size() == 0 && modifiedClusters->size() ==0"<< endl;
                for(int i =0; i<viewList->count();++i) {
                    KlustersView *view = viewList->at(i);
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
                }

                //Notify the errorMatrixView of the modification
                emit redoDeletion(*deletedClusters);
            }
        }

        qDebug() << "in KlustersDoc::redo, 2  : "<< endl;

        QList<int> clustersToShow = activeView->clusters();

        //Call redraw on the active view
        activeView->showAllWidgets();
        //Update the clusterPalette
        clusterPalette.updateClusterList();

        qDebug() << "in KlustersDoc::redo, 3 b : "<< endl;

        clusterPalette.selectItems(clustersToShow);

        qDebug() << "in KlustersDoc::redo, 4  : "<< endl;

        //Signal to klusters the new number of undo and redo
        emit updateUndoNb(clusterColorListUndoList.count());
        emit updateRedoNb(clusterColorListRedoList.count());

        qDebug() << "in KlustersDoc::redo, end  : "<< endl;
    }
}

void KlustersDoc::renumberClusters(){
    //Get the active view.
    KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

    QMap<int,int> clusterIdsOldNew;
    QMap<int,int> clusterIdsNewOld;

    clusteringData->renumber(clusterIdsOldNew,clusterIdsNewOld);

    prepareUndo(clusterIdsOldNew,clusterIdsNewOld);

    //Update the clusterColorList, keep the same colors, only update the clusterIds
    QList<dataType> clusterList = clusteringData->clusterIds();
    int nbClusters = clusterList.size();

    for (int i = 0; i < nbClusters; ++i){
        int clusterId = static_cast<int>(clusterList[i]);
        clusterColorList->changeItemId(i,clusterId);
    }

    //Notify all the views of the modification
    const int numberOfView(viewList->count());
    for(int i =0; i<numberOfView;++i)
    {
        KlustersView* view = viewList->at(i);
        if (view != activeView){
            view->renumberClusters(clusterIdsOldNew,false);
            //update the TraceView if any
            view->updateTraceView(electrodeGroupID,clusterColorList,false);
        } else {
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
    QList<int> activeClusters = activeView->clusters();
    clusterPalette.updateClusterList();
    clusterPalette.selectItems(activeClusters);
    shownClustersUpdate(activeClusters,*activeView);
}

int KlustersDoc::createFeatureFile(QList<int>& clustersToRecluster,const QString& reclusteringFetFileName){
    QFile fetFile(reclusteringFetFileName);
    if(!fetFile.open(QIODevice::WriteOnly))
        return OPEN_ERROR;

    //Create the file
    clusteringData->createFeatureFile(clustersToRecluster,fetFile);
    fetFile.close();
    if(fetFile.error() == QFile::NoError)
        return OK;
    else
        return CREATION_ERROR;
}

int KlustersDoc::integrateReclusteredClusters(QList<int>& clustersToRecluster,QList<int>& reclusteredClusterList,QString reclusteringFetFileName){

    QString cluFileName(reclusteringFetFileName);
    qDebug()<<"reclusteringFetFileName "<<reclusteringFetFileName;
    cluFileName.replace(".fet.",".clu.");

    QString cluFileUrl(cluFileName);
    QString tmpCluFile = cluFileUrl;
    if(!QFile::exists(cluFileUrl)) {
        QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary cluster file used by the reclustering program.") );
        return DOWNLOAD_ERROR;
    }

    qDebug()<<" tmpCluFile"<<tmpCluFile;
    QFile cluFile(tmpCluFile);

    if(!cluFile.open(QIODevice::ReadOnly)){
        if(!QFile::remove(reclusteringFetFileName))
            QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary feature file used by the reclustering program.") );
        if(!QFile::remove(cluFileName))
            QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary cluster file used by the reclustering program.") );
        return OPEN_ERROR;
    }

    //Actually integrate the new clusters.
    if(!clusteringData->integrateReclusteredClusters(clustersToRecluster,reclusteredClusterList,cluFile)){
        cluFile.close();
        if(!QFile::remove(reclusteringFetFileName))
            QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary feature file used by the reclustering program.") );
        if(!QFile::remove(cluFileName))
            QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary cluster file used by the reclustering program.") );
        return INCORRECT_CONTENT;
    }
    cluFile.close();

    //Suppress the fet and clu files.
    if(!QFile::remove(reclusteringFetFileName))
        QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary feature file used by the reclustering program.") );
    if(!QFile::remove(cluFileName))
        QMessageBox::critical(0,tr("Warning !"),tr("Could not delete the temporary cluster file used by the reclustering program.") );

    return OK;
}

void KlustersDoc::reclusteringUpdate(QList<int>& clustersToRecluster,QList<int>& reclusteredClusterList){
    //Prepare the undo
    prepareReclusteringUndo(reclusteredClusterList,clustersToRecluster);

    //Check if the active view is a ProcessWidget
    bool isProcessWidget = dynamic_cast<KlustersApp*>(parent)->doesActiveDisplayContainProcessWidget();

    if(!isProcessWidget){
        //Get the active view.
        KlustersView* activeView = static_cast<KlustersApp*>(parent)->activeView();

        QList<int> clustersToShow;
        QList<int>::const_iterator iterator;
        QList<int> const clusters = activeView->clusters();
        for(iterator = clusters.begin(); iterator != clusters.end(); ++iterator)
            clustersToShow.append(*iterator);

        //Add the new clusters in clusterColors and clustersToShow.
        QColor color;
        QList<int>::iterator clustersToCreate;
        for(clustersToCreate = reclusteredClusterList.begin(); clustersToCreate != reclusteredClusterList.end(); ++clustersToCreate ){
            color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
            clusterColorList->append(*clustersToCreate,color);
            clustersToShow.append(*clustersToCreate);
        }

        //Remove all the reclustered clusters from clusterColors and clustersToShow.
        QList<int>::iterator clustersToRemove;
        for (clustersToRemove = clustersToRecluster.begin(); clustersToRemove != clustersToRecluster.end(); ++clustersToRemove ){
            clusterColorList->remove(*clustersToRemove);
            clustersToShow.removeAll(*clustersToRemove);
        }

        //Notify all the views of the modification
        for(int i =0; i<viewList->count();++i){
            KlustersView* view = viewList->at(i);
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
        QList<int>::iterator clustersToCreate;
        for(clustersToCreate = reclusteredClusterList.begin(); clustersToCreate != reclusteredClusterList.end(); ++clustersToCreate ){
            color.setHsv(static_cast<int>(fmod(static_cast<float>(*clustersToCreate)*7,36))*10,255,255);
            clusterColorList->append(*clustersToCreate,color);
        }

        //Remove all the reclustered clusters from clusterColors and clustersToShow.
        QList<int>::iterator clustersToRemove;
        for (clustersToRemove = clustersToRecluster.begin(); clustersToRemove != clustersToRecluster.end(); ++clustersToRemove ){
            clusterColorList->remove(*clustersToRemove);
        }

        //Notify all the views of the modification
        for(int i =0; i<viewList->count();++i) {
	    KlustersView* view = viewList->at(i);
            if(!qobject_cast<ProcessWidget*>(view)){
                view->addNewClustersToView(clustersToRecluster,reclusteredClusterList,false);
                //update the TraceView if any
                view->updateTraceView(electrodeGroupID,clusterColorList,false);
            }
	}

        //Notify the errorMatrixView of the modification
        emit newClustersAdded(clustersToRecluster);

        //Update the palette of cluster
        clusterPalette.updateClusterList();
        QList<int> emptyList;
        clusterPalette.selectItems(emptyList);
    }
}

void KlustersDoc::createProviders(){
    QFileInfo docInfo(docUrl);
    const QString datUrl = docInfo.absolutePath() + "/" + docInfo.baseName() +".dat";

    int resolution = clusteringData->getResolution();
    int voltageRange = clusteringData->getVoltageRange();
    double samplingRate = clusteringData->getSamplingRate();
    int channelNb = clusteringData->getTotalNbChannels();

    //Create the tracesProviders
    tracesProvider = new TracesProvider(datUrl,channelNb,
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
    QList<int> group;
    color.setHsv(210,255,255);

    QList<int>& currentChannels =  clusteringData->getCurrentChannels();
    QList<int>::const_iterator iterator;
    for(iterator = currentChannels.begin(); iterator != currentChannels.end(); ++iterator){
        channelColorList->append(*iterator,color);
        displayChannelsGroups.insert(*iterator,electrodeGroupID.toInt());
        channelsSpikeGroups.insert(*iterator,electrodeGroupID.toInt());
        group.append(*iterator);
    }

    displayGroupsChannels.insert(electrodeGroupID.toInt(),group);
    spikeGroupsChannels.insert(electrodeGroupID.toInt(),group);

    ////Put all the other channels in the trash group (group 0).
    QList<int> trashGroup;
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
    QList<int> list;
    list.append(electrodeGroupID.toInt());
    displayGroupsClusterFile.insert(electrodeGroupID.toInt(),list);
}


void KlustersDoc::showUserClusterInformation(){
    clusterPalette.showUserClusterInformation(electrodeGroupID.toInt());
}


