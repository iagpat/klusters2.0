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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//include files for the application
#include "prefwaveformview.h"

// include files for QT
#include <QPainter>
#include <qfile.h>
#include <QTextStream>
#include <qmessagebox.h>
#include <QFileDialog>
#include <QSettings>


PrefWaveformView::PrefWaveformView(QWidget *parent,int nbChannels,const char *name):
    PrefWaveformViewLayout(parent),nbChannels(nbChannels){
    if(nbChannels > 0) {
        initializeChannelList();
    } else {
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
        channelList->addItem(QString::number(i + 1));
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
        channelList->addItem(QString::number(i + 1));
    }

    saveButton->setEnabled(true);
    loadButton->setEnabled(true);
    channelList->setEnabled(true);
}

void PrefWaveformView::saveChannelOrder(){
    const QString url = QFileDialog::getSaveFileName(this, tr("Save as..."),QDir::currentPath(), tr("*|All files") );
    if(!url.isEmpty()){
        FILE* channelFile = fopen(url.toLatin1(),"w");
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
    QSettings settings;
    const QString url = QFileDialog::getOpenFileName(this, tr("Load File..."), settings.value("CurrentDirectory").toString(),
                                               tr("*|All files") );

    if(url.isEmpty())
      return;
    QMap<int,int> positions;

    QDir CurrentDir;
    settings.setValue("CurrentDirectory", CurrentDir.absoluteFilePath(url));

    QString tmpChannelFile = url;
    if(!QFile(tmpChannelFile).exists()){
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

    QTextStream positionStream(&channelFile);
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

            return;
        }
        //The channels are counted from 0 to nbChannels - 1.
        positions.insert(channel,position);
        channel++;
    }

    channelFile.close();

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
        channelList->insertItem(positions[i],QString::number(i + 1));
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


