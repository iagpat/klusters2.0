/***************************************************************************
                          prefwaveformview.h  -  description
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

#ifndef PREFWAVEFORMVIEW_H
#define PREFWAVEFORMVIEW_H

// include files for QT
#include <qwidget.h>
#include <qbutton.h>
#include <qiconview.h>
#include <qspinbox.h>
#include <qmap.h>

// include files for KDE
#include <kpushbutton.h>

//include files for the application
#include <prefwaveformviewlayout.h>
#include "channellist.h"

  /**
  * Class representing the Waveform View configuration page of the Klusters preferences dialog.
  *@author Lynn Hazan
  */

class PrefWaveformView : public PrefWaveformViewLayout  {
   Q_OBJECT
public: 
	PrefWaveformView(QWidget* parent=0,int nbChannels = 0,const char *name=0);
	~PrefWaveformView();

  /**Sets the gain used to display the waveforms.*/
  inline void setGain(int gain){gainSpinBox->setValue(gain);};

  /**Returns the gain used to display the waveforms.*/
  inline int getGain() const{return gainSpinBox->value();};

  /**Sets the number of channels contained in the document. This will be used to
  * fill the channelView.*/
  inline void setNbChannels(int nb){nbChannels = nb;};

  /**Returns the number of channels contained in the document.*/
  inline int getNbChannels() const {return nbChannels;};

  /**Initialize the channel view with the number of current channels.*/
  void initializeChannelList();

  /**Reinitialize the channel view with @p nb as the number of channels.
  * @param nb number of channels.
  */
  void resetChannelList(int nb);

  /**Returns the positions of the channels to use in the waveform vies.**/
  inline QValueList<int> getChannelPositions() const {return channelPositions;};

  inline void enableChannelSettings(bool state){
   saveButton->setEnabled(state);
   loadButton->setEnabled(state);
   channelList->setEnabled(state);
  };

  
signals:
  void positionsChanged();
  
private:
  QValueList<int> channelPositions;
  int nbChannels;
  
private slots:
  
  /**Saves the order of the channels to a file.*/
  void saveChannelOrder();

  /**Loads the  order of the channels from a file.*/
  void loadChannelOrder();

  /**Update the channelPositions due to a change of in the channel positions.*/
  void updateChannelPositions();         
};

#endif
