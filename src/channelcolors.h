/***************************************************************************
                          channelcolors.h  -  description
                             -------------------
    begin                : Tue Sep 16 2004
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

#ifndef CHANNELCOLORS_H
#define CHANNELCOLORS_H

// include files for Qt
#include <qlist.h>
#include <qcolor.h>

/**
  * This class represents the list of the channels with their associated id, colors
  * and colors status (i.e the color has been changed or no).
  * The list index is zero based.
  *@author Lynn Hazan
  */

class ChannelColors  {

public:

  ChannelColors();
  virtual ~ChannelColors();
  ChannelColors(const ChannelColors& channelcolors);


public:

  /**
  * Suppress all the elements of
  */
  inline void removeAll(){qDeleteAll(channelList); channelList.clear();}
  
  /**
  * Returns the color for a channel with a given id (@p identifier).
  * @param identifier channel id.
  * @return the QColor for the given channel.
  */
  virtual QColor color(int identifier);

  /**
  * Returns the group color for a channel with a given id (@p identifier).
  * @param identifier channel id.
  * @return the QColor for the group of the given channel.
  */
  virtual QColor groupColor(int identifier);

  /**
  * Returns the spike group color for a channel with a given id (@p identifier).
  * @param identifier channel id.
  * @return the QColor for the spike group of the given channel
  */
  virtual QColor spikeGroupColor(int identifier);
  

 /**
  * Sets the color for a channel with a given id to color.
  * @param identifier channel id.
  * @param color color to attribute to the channel.
  */
  virtual void setColor(int identifier,const QColor& color);

 /**
  * Sets the group color for a channel with a given id to color.
  * @param identifier channel id.
  * @param color group color to attribute to the channel.
  */
  virtual void setGroupColor(int identifier,const QColor& color);

 /**
  * Sets the spike group color for a channel with a given id to color.
  * @param identifier channel id.
  * @param color spike group color to attribute to the channel.
  */
  virtual void setSpikeGroupColor(int identifier,QColor color);

  /**
  * Returns the channel id with a given position in the list (@p index).
  * @param index the index in the list of channels
  * @return the channel id
  */
  virtual int channelId(int index);

  /**
  * Returns true if the channel Id exists false otherwise.
  * @param channelId the id of the channel to check for existence
  * @return the boolean value for the existance of the channel
  */
  virtual bool contains(int channelId);

  /**
  * Returns the number of channels.
  * @return the number of channels in the list
  */
  virtual inline uint numberOfChannels() const {return channelList.count();}

  /**
  * Appends a channel to the list of channels.
  * @param channelId the channel id.
  * @param color the color of the channel.
  * @param groupColor the group color of the channel.
  * @param spikeGroupColor the decteion group color of the channel.   
  * @return the index in the list.
  */
  virtual uint append(int channelId, const QColor &color, const QColor &groupColor, const QColor &spikeGroupColor);

  /**
  * This is an overloaded member function, provided for convenience. It behaves essentially like the above function.
  * Appends a channel to the list of channels with its groupColor and spikeGroupColor equal to its color.
  * @param channelId the channel id.
  * @param color the color of the channel.
  * @return the index in the list.
  */
  virtual uint append(int channelId, const QColor &color);

  /**
  * Inserts a channel at position @p index in the list of channels.
  * @param channelId the channel id.
  * @param index index position where to insert the channel.
  * @param color the color of the channel.
  * @param groupColor the group color of the channel.
  * @param spikeGroupColor the decteion group color of the channel.    
  */
  virtual void insert(int channelId, int index, const QColor &color, const QColor &groupColor, const QColor &spikeGroupColor);


  /**
  * Removes a channel, with a given id (@p identifier),from the list of channels.
  * @param identifier the channel id.
  * @return true if successful,i.e. if identifier is in range, otherwise returns false.
  */
  virtual bool remove(int identifier);

private:

  /**Structure representing a color channel.*/
  struct ChannelColor{
    int channelId;
    QColor color;
    QColor groupColor;
    QColor spikeGroupColor;

    ChannelColor(int id,const QColor& c,const QColor& gc,const QColor& gdc){
      channelId = id;
      color = c;
      groupColor = gc;
      spikeGroupColor = gdc;
    }

    ChannelColor(const ChannelColor& origin):channelId(origin.channelId),
                      color(origin.color),groupColor(origin.groupColor),spikeGroupColor(origin.spikeGroupColor){}
    ~ChannelColor(){}
  };

  /**list of the ChannelColor contained in the class.*/
  QList<ChannelColor*> channelList;

  /**
  * Looks up for the channel id @p channelId.
  * @param channelId the channel id
  * @return the channelColor corresponding to the channel id or null if not found
  */
  ChannelColor* channelColor(int channelId) const;

  /**
  * Looks up for the channel index of the channel id @p channelId.
  * @param channelId the channel id.
  * @return the index in the list corresponding to the channel id or -1 if not found.
  */
  int channelColorIndex(int channelId) const;

};

#endif
