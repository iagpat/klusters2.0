/***************************************************************************
                          prefdialog.h  -  description
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

#ifndef PREFDIALOG_H
#define PREFDIALOG_H

// include files for QT
#include <qwidget.h>

// include files for KDE
#include <kdialogbase.h>

class PrefGeneral;
class PrefWaveformView;
class PrefClusterView;

 /**
  * Class representing the Klusters preferences dialog.
  *@author Lynn Hazan
  */
  
class PrefDialog : public KDialogBase {
    Q_OBJECT
  public:
    /// Constructor
    PrefDialog(QWidget *parent,int nbChannels = 0,const char *name=0, WFlags f=0);

    /** Transfers the settings from the configuration object to the dialog.*/
    void updateDialog();
    /** Transfers the settings from the dialog to the configuration object.*/
    void updateConfiguration();
    /** */
    inline bool isApplyEnable(){return applyEnable;};

    void resetChannelList(int nbChannels);

    void enableChannelSettings(bool state);
    
  public slots:
    /// Will be called when the "Default" button has been clicked.
    void slotDefault();
    /// Will be called when the "Apply" button has been clicked.
    void slotApply();
    /// Will be called whenever a setting was changed.
    void enableApply();

  signals:
    /// Will be emitted when the new settings should be applied.
    void settingsChanged();

  private:
    PrefGeneral* prefGeneral;
    PrefWaveformView* prefWaveformView;
    PrefClusterView* prefclusterView;
    bool applyEnable;
};

#endif  // PREFDIALOG_H
