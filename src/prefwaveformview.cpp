/***************************************************************************
                          prefwaveformview.cpp  -  description
                             -------------------
    begin                : Thu Dec 11 2003
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
//include files for the application
#include "prefwaveformview.h"

// include files for QT
#include <qpainter.h>
#include <qfile.h>
#include <qtextstream.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kio/job.h>
#include <kio/netaccess.h>

PrefWaveformView::PrefWaveformView(QWidget *parent,int nbChannels,const char *name):
PrefWaveformViewLayout(parent,name),nbChannels(nbChannels){
 if(nbChannels > 0) initializeChannelList();
 else{
   saveButton->setEnabled(false);
   loadButton->setEnabled(false);
   channelList->setEnabled(false);
 }

 connect(saveButton,SIGNAL(clicked()),this,SLOT(saveChannelOrder()));
 connect(loadButton,SIGNAL(clicked()),this,SLOT(loadChannelOrder()));
 connect(channelList,SIGNAL(positionsChanged()),this,SLOT(updateChannelPositions()));
 
}
PrefWaveformView::~PrefWaveformView(){
}

void PrefWaveformView::initializeChannelList(){
 ///Initialize the position of the channels.
 //The first one in the file will be the first one (at the top), second one will be beneath and so on.
 for(int i = 0; i < nbChannels; ++i){
  channelPositions.append(i);
  channelList->insertItem(QString("%1").arg(i + 1));
 } 
}

void PrefWaveformView::resetChannelList(int nb){
 nbChannels = nb;
 channelPositions.clear();
 channelList->clear();

 ///Initialize the position of the channels.
 //The first one in the file will be the first one (at the top), second one will be beneath and so on.
 for(int i = 0; i < nbChannels; ++i){
  channelPositions.append(i);
  channelList->insertItem(QString("%1").arg(i + 1));
 }
 
 saveButton->setEnabled(true);
 loadButton->setEnabled(true);
 channelList->setEnabled(true);
}

void PrefWaveformView::saveChannelOrder(){
  KURL url = KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*|All files"), this, i18n("Save as..."));

  if(!url.isEmpty()){
  FILE* channelFile = fopen(url.path(),"w");
   if(channelFile == NULL){
    KMessageBox::error (this,
         i18n("The selected file could not be opened, possibly because of access permissions !"),
         i18n("Error !"));
    return;
   }

   int writeStatus = 0;  
   for(int i = 0; i< nbChannels; ++i) 
    writeStatus = fprintf(channelFile, "%i\n",channelPositions[i]);

   fclose(channelFile);
   
   if(writeStatus < 0){
    KMessageBox::error (this,
         i18n("The data could not have been saved due to an writing error."),
         i18n("IO Error !"));
    return;     
   }
  } 
}

void PrefWaveformView::loadChannelOrder(){
 KURL url = KFileDialog::getOpenURL(QString::null,
      i18n("*|All files"), this, i18n("Load File..."));

 QMap<int,int> positions;
      
 if(!url.isEmpty()){
   QString tmpChannelFile;
   if(!KIO::NetAccess::download(url,tmpChannelFile)){
     KMessageBox::error (this,
         i18n("The selected file could not be downloaded !"),
         i18n("Error !"));
     return;
   }

   QFile channelFile(tmpChannelFile);
   if(!channelFile.open(IO_ReadOnly)){
    KMessageBox::error (this,
         i18n("The selected file could not be opened !"),
         i18n("Error !"));
    return;
   }

   QTextStream positionStream(&channelFile);
   QString line;
   int channel = 0;
   for(line = positionStream.readLine(); !line.isNull();line = positionStream.readLine()){
     bool ok;
     int position = line.toInt(&ok,10);
     if(!ok){
      KMessageBox::error (this,
          i18n("The selected file does not have the correct format (list of channels number),\n"
           "it can not be used."),i18n("Error !"));

      channelFile.close();
      //Remove the temp file
      KIO::NetAccess::removeTempFile(tmpChannelFile);

      return;
     }
     //The channels are counted from 0 to nbChannels - 1.
     positions.insert(channel,position);
     channel++;
   }
      
   channelFile.close();

   //Remove the temp file
   KIO::NetAccess::removeTempFile(tmpChannelFile);
 }

 if(nbChannels != static_cast<int>(positions.count())){
  KMessageBox::error (this,
         i18n("The number of channels in the selected file does not correspond to the number of channels of the current file !\n"
              "This file can not be used for the current document."),
         i18n("Error !"));
  return;
 }

 //Update the list with the positions get from the file.
 channelList->clear();
 for(int i = 0; i< nbChannels; ++i){
  channelList->insertItem(QString("%1").arg(i + 1),positions[i]);
  channelPositions[i] = positions[i];
 }

 //signal to the dialog that channel positions have changed so it can update the Apply button.
 emit positionsChanged();   
}

void PrefWaveformView::updateChannelPositions(){
 for(int i = 0; i< nbChannels; ++i){
  QString currentChannel = (channelList->item(i))->text();
  int currentChannelInt = currentChannel.toInt() - 1;
  channelPositions[currentChannelInt] = i;
 }

 //signal to the dialog that channel positions have changed so it can update the Apply button.
 emit positionsChanged();
}



