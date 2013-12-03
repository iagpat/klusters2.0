/***************************************************************************
                          tracesprovider.cpp  -  description
                             -------------------
    begin                : Mon Mar 1 2004
    copyright            : (C) 2004 by Lynn Hazan
    email                : lynn.hazan.myrealbox.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//C include files
//#define _LARGEFILE_SOURCE already defined in /usr/include/features.h
#define _FILE_OFFSET_BITS 64

#include <fstream>
#include <QFileInfo>
#include <QDebug>

//include files for the application
#include "tracesprovider.h"

#include <qfile.h>

TracesProvider::TracesProvider(const QString& fileUrl,int nbChannels,int resolution,double samplingRate,int offset):
    DataProvider(fileUrl),
    nbChannels(nbChannels),
    resolution(resolution),
    samplingRate(samplingRate),
    offset(offset)
{
    computeRecordingLength();
}

TracesProvider::~TracesProvider(){
    qDebug()<<"in ~TracesProvider";
}



dataType TracesProvider::getNbSamples(long startTime,long endTime,long startTimeInRecordingUnits){
    //Search what is the number of samples in the given time frame.
    //Convert the time in miliseconds to time in recording units.

    //Convert the time in miliseconds to time in recording units if need it.
    dataType startInRecordingUnits;
    //startTimeInRecordingUnits has been computed in a previous call to a clustersProvider browsing function. It has to be used insted of computing
    //the value from startTime because of the rounding which has been applied to it.
    if(startTimeInRecordingUnits != 0) startInRecordingUnits = startTimeInRecordingUnits;
    else startInRecordingUnits = static_cast<dataType>(startTime * static_cast<double>(samplingRate / static_cast<double>(1000)));
    dataType endInRecordingUnits =  static_cast<dataType>(endTime * static_cast<double>(samplingRate / static_cast<double>(1000)));


    //The caller should have check that we do not go over the end of the file.
    //The recording starts at time equals 0 and ends at length of the file minus one.
    //Therefore if the end time requested equals the length of the file, endInRecordingUnits
    // should be diminish by one sample.
    if(endTime == length) endInRecordingUnits--;

    dataType nbSamples = static_cast<dataType>(endInRecordingUnits - startInRecordingUnits) + 1;

    return nbSamples;
}

void TracesProvider::requestData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits){
    retrieveData(startTime,endTime,initiator,startTimeInRecordingUnits);
}

void TracesProvider::retrieveData(long startTime,long endTime,QObject* initiator,long startTimeInRecordingUnits){
    Array<dataType> data;

    FILE* dataFile = fopen(fileName.toLatin1(),"rb");
    if(dataFile == NULL){
        //emit the signal with an empty array, the reciever will take care of it, given a message to the user.
        data.setSize(0,0);
        emit dataReady(data,initiator);
        return;
    }

    //When the bug in gcc will be corrected for the 64 bits, the c++ code will be use
    //[alex@slut]/home/alex/src/sizetest > ./sizetest-2.95.3
    //  sizeof(std::streamoff) = 8 bytes (64 bits)
    //  sizeof(std::streampos) = 8 bytes (64 bits)

    // [alex@slut]/home/alex/src/sizetest > ./sizetest-3.2.2
    //  sizeof(std::streamoff) = 4 bytes (32 bits)
    //  sizeof(std::streampos) = 12 bytes (96 bits)

    /*  ifstream dataFile;

 //Open the file containing the data
 dataFile.open(fileName,ifstream::in|ifstream::binary);
 if(dataFile.fail()){
  //emit the signal with an empty array, the reciever will take care of it, given a message to the user.
  data.setSize(0,0);
  emit dataReady(data,initiator);
  return;
 }*/

    //Search what is the number of samples in the given time frame.

    //Convert the time in miliseconds to time in recording units if need it.
    dataType startInRecordingUnits;
    //startTimeInRecordingUnits has been computed in a previous call to a clustersProvider browsing function. It has to be used insted of computing
    //the value from startTime because of the rounding which has been applied to it.
    if(startTimeInRecordingUnits != 0) startInRecordingUnits = startTimeInRecordingUnits;
    else startInRecordingUnits = static_cast<dataType>(startTime * static_cast<double>(samplingRate / static_cast<double>(1000)));
    dataType endInRecordingUnits =  static_cast<dataType>(endTime * static_cast<double>(samplingRate / static_cast<double>(1000)));

    //The caller should have check that we do not go over the end of the file.
    //The recording starts at time equals 0 and ends at length of the file minus one.
    //Therefore if the end time requested equals the length of the file, endInRecordingUnits
    // should be diminish by one sample.
    if(endTime == length) endInRecordingUnits--;

    dataType nbSamples = static_cast<dataType>(endInRecordingUnits - startInRecordingUnits) + 1;

    //data will contain the final values.
    data.setSize(nbSamples,nbChannels);

    //Depending on the acquisition system resolution, the data are store as short or long
    if((resolution == 12) | (resolution == 14) | (resolution == 16)){
        Array<short> retrieveData(nbSamples,nbChannels);

        QFile dataFile(fileName);
        if (!dataFile.open(QIODevice::ReadOnly)) {
            data.setSize(0,0);
            emit dataReady(data,initiator);
            return;
        }
        qint64 nbValues = nbSamples * nbChannels;
        qint64 position = static_cast<qint64>(static_cast<qint64>(startInRecordingUnits)* static_cast<qint64>(nbChannels));

        dataFile.seek(position * sizeof(short));
        qint64 nbRead = dataFile.read(reinterpret_cast<char*>(&retrieveData[0]), sizeof(short) * nbValues);

        // copy the data into retrieveData.
        if(nbRead != qint64(nbValues*sizeof(short))){
            //emit the signal with an empty array, the reciever will take care of it, given a message to the user.
            data.setSize(0,0);
            dataFile.close();
            emit dataReady(data,initiator);
            return;
        }
        dataFile.close();
        //Apply the offset if need it,convert to dataType and store the values in data.
        if(offset != 0){
            for(qint64 i = 0; i < nbValues; ++i){
                data[i] = static_cast<dataType>(retrieveData[i]) - static_cast<dataType>(offset);
            }
        }
        else{
            for(qint64 i = 0; i < nbValues; ++i){
                data[i] = static_cast<dataType>(retrieveData[i]);
            }
        }
    }
    else if(resolution == 32){
        QFile dataFile(fileName);
        if (!dataFile.open(QIODevice::ReadOnly)) {
            data.setSize(0,0);
            emit dataReady(data,initiator);
            return;
        }
        Array<dataType> retrieveData(nbSamples,nbChannels);
        qint64 nbValues = nbSamples * nbChannels;
        qint64 position = static_cast<qint64>(static_cast<qint64>(startInRecordingUnits)* static_cast<qint64>(nbChannels));

        dataFile.seek(position * sizeof(short));
        qint64 nbRead = dataFile.read(reinterpret_cast<char*>(&retrieveData[0]), sizeof(short) * nbValues);

        // copy the data into retrieveData.
        if(nbRead != qint64(nbValues*sizeof(short))){
            //emit the signal with an empty array, the reciever will take care of it, given a message to the user.
            data.setSize(0,0);
            dataFile.close();
            emit dataReady(data,initiator);
            return;
        }

        //Apply the offset if need it and store the values in data.
        if(offset != 0){
            for(qint64 i = 0; i < nbValues; ++i)
                data[i] = retrieveData[i] - static_cast<dataType>(offset);
        }
        else{
            for(qint64 i = 0; i < nbValues; ++i){
                data[i] = static_cast<dataType>(retrieveData[i]);
            }
        }
    }

    //The data have been retrieve, close the file.
    fclose(dataFile);//dataFile.close();

    //Send the information to the receiver.
    emit dataReady(data,initiator);
}

void TracesProvider::computeRecordingLength(){
    //When the bug in gcc will be corrected for the 64 bits the c++ code will be use
    //[alex@slut]/home/alex/src/sizetest > ./sizetest-2.95.3
    //  sizeof(std::streamoff) = 8 bytes (64 bits)
    //  sizeof(std::streampos) = 8 bytes (64 bits)

    // [alex@slut]/home/alex/src/sizetest > ./sizetest-3.2.2
    //  sizeof(std::streamoff) = 4 bytes (32 bits)
    //  sizeof(std::streampos) = 12 bytes (96 bits)

    /*ifstream dataFile;

 //Open the file containing the data
 dataFile.open(fileName,ifstream::in|ifstream::binary);
 if(dataFile.fail()) return 0;

 // get the length of the file:
 dataFile.seekg (0, ios::end);
 qint64 fileLength = dataFile.tellg();
 dataFile.close();*/

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        length = 0;
        return;
    }
    f.close();
    QFileInfo fInfo(fileName);
    qint64 fileLength = fInfo.size();

    int dataSize = 0;
    if((resolution == 12) | (resolution == 14) | (resolution == 16)) dataSize = 2;
    else if(resolution == 32) dataSize = 4;

    length = static_cast<long long>(
                static_cast<double>(
                    fileLength / static_cast<double>(nbChannels) * samplingRate * static_cast<double>(dataSize)
                    ) * 1000
                );
}

long TracesProvider::getTotalNbSamples(){
    return static_cast<long>((length * samplingRate) / 1000);
}

