/***************************************************************************
                          prefgeneral.h  -  description
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

#ifndef PREFGENERAL_H
#define PREFGENERAL_H

// include files for QT
#include <qwidget.h>
#include <qcombobox.h>
#include <qcheckbox.h> 
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h> 

//includes files for KDE
#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>

//include files for the application
#include <prefgenerallayout.h>

  /**
  * Class representing the Klusters General Configuration page of the Klusters preferences dialog.
  *@author Lynn Hazan
  */

class PrefGeneral : public PrefGeneralLayout  {
   Q_OBJECT
public: 
	PrefGeneral(QWidget *parent=0, const char *name=0);
	~PrefGeneral();

  /**Sets the use of a crash and recovery autosave.*/
  void setCrashRecovery(bool use);

  /**Sets the index of time interval between 2 crash and recovery autosave.*/
  void setCrashRecoveryIndex(int index);

  /**Sets the number of step in the undo/redo mechanism.*/
  void setNbUndo(int nb);

  /**Sets the background color.*/
  void setBackgroundColor(const QColor& color);

  /**Sets the reclustering executable.*/
  void setReclusteringExecutable(QString executable);

  /**Sets the arguments for the reclustering.*/
  void setReclusteringArguments(QString arguments);
  
  /**Returns true if a crash and recovery autosave is performed, false othewise.*/
  bool isCrashRecovery() const;

  /**Returns the index of the time interval between 2 crash and recovery autosave in minutes.*/
  int crashRecoveryIntervalIndex() const;

  /**Returns the number of step in the undo/redo mechanism.*/
  int getNbUndo() const;

  /**Returns the background color.*/
  QColor getBackgroundColor() const;

  /**Returns the reclustering executable.*/
  QString getReclusteringExecutable() const;

  /**Returns the arguments for the reclustering.*/
  QString getReclusteringArguments() const;
       
private slots:
  void updateCrashRecoveryTimeInterval(int state);

  void updateReclusteringExecutable();
};

#endif
