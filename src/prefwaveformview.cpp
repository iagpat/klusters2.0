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
#include <q3textstream.h>
#include <qmessagebox.h>

// include files for KDE

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
  QString url = KFileDialog::getSaveURL(QDir::currentPath(),
        tr("*|All files"), this, tr("Save as..."));

  if(!url.isEmpty()){
  FILE* channelFile = fopen(url.path().latin1(),"w");
   if(channelFile == NULL){
    QMessageBox::critical (this,tr("Error !"),
         tr("The selected file could not be opened, possibly because of access permissions !")
         );
    return;
   }

   int writeStatus = 0;  
   for(int i = 0; i< nbChannels; ++i) 
    writeStatus = fprintf(channelFile, "%i\n",channelPositions[i]);

   fclose(channelFile);
   
   if(writeStatus < 0){
    QMessageBox::critical (this,tr("IO Error !"),
         tr("The data could not have been saved due to an writing error.")
         );
    return;     
   }
  } 
}

void PrefWaveformView::loadChannelOrder(){
 QString url = KFileDialog::getOpenURL(QString::null,
      tr("*|All files"), this, tr("Load File..."));

 QMap<int,int> positions;
      
 if(!url.isEmpty()){
   QString tmpChannelFile;
   if(!KIO::NetAccess::download(url,tmpChannelFile)){
     QMessageBox::critical (this,tr("Error !"),
         tr("The selected file could not be downloaded !")
         );
     return;
   }

   QFile channelFile(tmpChannelFile);
   if(!channelFile.open(QIODevice::ReadOnly)){
    QMessageBox::critical (this,tr("Error !"),
         tr("The selected file could not be opened !")
         );
    return;
   }

   Q3TextStream positionStream(&channelFile);
   QString line;
   int channel = 0;
   for(line = positionStream.readLine(); !line.isNull();line = positionStream.readLine()){
     bool ok;
     int position = line.toInt(&ok,10);
     if(!ok){
      QMessageBox::critical (this,tr("Error !"),
          tr("The selected file does not have the correct format (list of channels number),\n"
           "it can not be used."));

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
  QMessageBox::critical (this,tr("Error !"),
         tr("The number of channels in the selected file does not correspond to the number of channels of the current file !\n"
              "This file can not be used for the current document.")
         );
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




#include "prefwaveformview.moc"
