/***************************************************************************
                          configuration.h  -  description
                             -------------------
    begin                : Thu Dec 12 2003
    copyright            : (C) 2003 by Lynn Hazan
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// include files for QT
#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>

//General C++ include files
#include <iostream>
using namespace std;

/**
  * This is the one and only configuration object.
  * The member functions read() and write() can be used to load and save
  * the properties to the application configuration file.
  *@author Lynn Hazan
*/

class Configuration {
  public:
    /** Reads the configuration data from the application config file.
    * If a property does not already exist in the config file it will be
    * set to a default value.*/
    void read();
    /** Writes the configuration data to the application config file.*/
    void write() const;

    /**Sets the use of a crash and recovery autosave.*/
    inline void setCrashRecovery(bool use){crashRecovery = use;};

    /**Sets the time interval between 2 crash and recovery autosave.*/
    inline void setCrashRecoveryIndex(int index){crashRecoveryIndex = index;};

    /**Sets the gain used to display the waveforms.*/
    inline void setGain(int gain){this->gain = gain;};

    /**Sets the time interval between 2 lines drawn in the cluster views
    * when the time dimension in selected. The time @p time is in second.*/
    inline void setTimeInterval(int time){timeInterval = time;};

    /**Sets the number of step in the undo/redo mechanism.*/
    inline void setNbUndo(int nb){nbUndo = nb;};

    /**Sets the positions of the channels.*/
    inline void setChannelPositions(QValueList<int> positions){
     channelPositions.clear();
     QValueList<int>::iterator iterator;
     for(iterator = positions.begin(); iterator != positions.end(); ++iterator)
      channelPositions.append(*iterator);
    };

    /**Sets the number of channels.*/
    inline void setNbChannels(int nb){nbChannels = nb;};

    /**Sets the background color.*/
    inline void setBackgroundColor(QColor color) {backgroundColor = color;};

    /**Sets the reclustering executable.*/
    inline void setReclusteringExecutable(QString executable) {reclusteringExecutable = executable;};

    /**Sets the arguments for the reclustering.*/
    inline void setReclusteringArguments(QString arguments) {reclusteringArgs = arguments;};
    
    /**Returns true if a crash and recovery autosave is performed, false othewise.*/
    inline bool isCrashRecovery() const{return crashRecovery;};

    /**Returns the time interval between 2 crash and recovery autosave in minutes.*/
    inline int crashRecoveryInterval() const{
     switch(crashRecoveryIndex){
      case 0:
        return 1;
      case 1:
        return 3;
      case 2:
        return 5;
      case 3:
        return 15;
      case 4:
        return 30;
      default:
        return 1;
     }           
    };

    /**Returns the index corresponding to the time interval between
    * 2 crash and recovery autosave in minutes.*/
    inline int crashRecoveryIntervalIndex() const{return crashRecoveryIndex;};

    /**Returns the gain used to display the waveforms.*/
    inline int getGain() const{return gain;};

    /**Returns the time interval between 2 lines drawn in the cluster views
    * when the time dimension in selected. The time is in second.*/
    inline int getTimeInterval() const{return timeInterval;};

    /**Returns the number of step in the undo/redo mechanism.*/
    inline int getNbUndo() const{return nbUndo;};

    /**Returns the positions of the channels.*/
    inline QValueList<int>* getChannelPositions() {return &channelPositions;};

    /**Returns the number of channels.*/
    inline int getNbChannels() const{return nbChannels;};

    /**Returns the background color.*/
    inline QColor getBackgroundColor() const{return backgroundColor;};
    
    /**Returns the reclustering executable.*/
    inline QString getReclusteringExecutable() const{return reclusteringExecutable;};

    /**Returns the arguments for the reclustering.*/
    inline QString getReclusteringArguments() const{return reclusteringArgs;};

    /**Returns the default value for the crash and recovery mechanism.
    * True if a crash and recovery autosave is performed, false othewise.*/
    inline bool isCrashRecoveryDefault() const{return crashRecoveryDefault;};

    /**Returns the index corresponding to the default time interval between
    * 2 crash and recovery autosave in minutes.*/
    inline int crashRecoveryIntervalIndexDefault() const{return crashRecoveryIndexDefault;};

    /**Returns the default gain used to display the waveforms.*/
    inline int getGainDefault() const{return gainDefault;};

    /**Returns the default time interval between 2 lines drawn in the cluster views
    * when the time dimension in selected. The time is in second.*/
    inline int getTimeIntervalDefault() const{return timeIntervalDefault;};

    /**Returns the default number of step in the undo/redo mechanism.*/
    inline int getNbUndoDefault() const{return nbUndoDefault;};

    /**Returns the the default background color.*/
    inline QColor getBackgroundColorDefault() const{return backgroundColorDefault;};

    /**Returns the default reclustering executable.*/
    inline QString getReclusteringExecutableDefault() const{return reclusteringExecutableDefault;};

    /**Returns the default arguments for the reclustering.*/
    inline QString getReclusteringArgumentsDefault() const{return reclusteringArgsDefault;};

  private:
    /**Boolean indicating if a crash and recovery is ask.*/
    bool crashRecovery; 
    /**Index of a dropdown list giving the time-interval between 2 autosave for a crashRecovery.*/
    int  crashRecoveryIndex; 
    /**Initial gain used to display the waveforms.*/
    int  gain;
    /**Time interval between 2 lines drawn in the cluster views when the time dimension in selected.*/
    int  timeInterval; 
    /**Number of step in the undo/redo mechanism.*/
    int  nbUndo; 
    /**Positions of the channels in the waveform view.*/
    QValueList<int> channelPositions;
    /**Number of channels.*/
    int nbChannels;
    /**Background color of the views.*/
    QColor backgroundColor;
    /**Path to the reclustering executable.*/
    QString reclusteringExecutable;
    /**Arguments for the reclustering executable.*/
    QString reclusteringArgs;
 
    static const bool crashRecoveryDefault; 
    static const int  crashRecoveryIndexDefault;
    static const int  gainDefault;
    static const int  timeIntervalDefault;
    static const int  nbUndoDefault;
    static const QColor backgroundColorDefault;
    static const QString reclusteringExecutableDefault;
    static const QString reclusteringArgsDefault;

    Configuration();
    Configuration(const Configuration&);

    friend Configuration& configuration();
};

/// Returns a reference to the application configuration object.
Configuration& configuration();

#endif  // CONFIGURATION_H
