/***************************************************************************
                          clusterPalette.h  -  description
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
#ifndef CLUSTERPALETTE_H
#define CLUSTERPALETTE_H

//QT include files
#include <qvariant.h>
#include <QWidget>
#include <QListWidget>
#include <qtooltip.h>

#include <QList>

class QStatusBar;


// forward declaration of the KlustersDoc class
class KlustersDoc;

/**
  * This class represents the Cluster Panel of the application.
  * It receives the user selections and triggers the actions which have to be done.
  *@author Lynn Hazan
  */


class ClusterPaletteWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ClusterPaletteWidget(QWidget *parent);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );

Q_SIGNALS:
    void changeColor(QListWidgetItem* item);
    void onItem(QListWidgetItem* item);
};

class ClusterPalette : public QWidget
{
    Q_OBJECT
    
public:
    /**
    * @param backgroundColor backgroundColor of the cluster palette.
    * @param parent the parent QWidget.
    * @param statusBar a reference to the application status bar.
    * @param name name of the widget (can be used for introspection).
    */
    explicit ClusterPalette(const QColor &backgroundColor, QWidget* parent = 0, QStatusBar * statusBar = 0, const char* name = 0 );
    /*
   *  Destroys the object and frees any allocated resources.
   */
    ~ClusterPalette();

    //Mode of action, in immediate the change of color
    //and the selection of cluster is immediately trigger
    enum Mode {IMMEDIATE = 1, DELAY = 2};
    
    enum DataStored { INDEX = Qt::UserRole+1 };

    void createClusterList(KlustersDoc* doc);
    void updateClusterList();
    void selectItems(const QList<int> &selectedClusters);
    void setImmediateMode(){mode = IMMEDIATE;}
    void setDelayMode(){mode = DELAY;}
    void reset();
    /**Returns the list of selected clusters*/
    QList<int> selectedClusters();

    /** Hides the user cluster information, that is show the normal cluster palette.*/
    void hideUserClusterInformation();

    /** Shows the user cluster information, that is show a modified cluster palette presenting the cluster ids and the user cluster information.
      * @param electrodeGroupId id of the current electrode group.
     */
    void showUserClusterInformation(int electrodeGroupId);

    /**updates the background color of the palette.*/
    void changeBackgroundColor(const QColor& color);

public Q_SLOTS:
    void changeColor(QListWidgetItem *item);
    void moveClustersToNoise();
    void moveClustersToArtefact();
    void groupClusters();
    void updateClusters();

protected Q_SLOTS:
    /** The right click on a cluster icon bring a dialog allowing the user to enter information on the cluster
    * (structure, type, isolation distance, quality and notes).
    */
    void slotClickRedraw();
    void languageChange();
    /**
     * When moving the mouse over an cluster icon, the statusBar is updated with the information the user might could provided
      * (structure, type, isolation distance, quality and notes).
     */
    void slotOnItem(QListWidgetItem *item);
    
    void slotCustomContextMenuRequested(const QPoint&);

Q_SIGNALS:
    void singleChangeColor(int selectedCluster);
    void updateShownClusters(const QList<int>& selectedClusters);
    void groupClusters(const QList<int> &selectedClusters);
    void moveClustersToNoise(const QList<int> &selectedClusters);
    void moveClustersToArtefact(const QList<int> &selectedClusters);
    void clusterInformationModified();

private:
    ClusterPaletteWidget* iconView;
    KlustersDoc* doc;

    Mode mode;//default IMMEDIATE

    /**Prevent from emitting signal while globaly selecting items*/
    bool isInSelectItems;

    /**Prevent from emitting a signal of update if the selection had not changed.*/
    bool isUpToDate;
    
    /**Current palette background Color.*/
    QColor backgroundColor;

    /**Pointer to the status bar of the application.*/
    QStatusBar* statusBar;

    /**Allows to update correctly the cluster text.*/
    bool isInUserClusterInfoMode;

};

#endif // CLUSTERPALETTE_H
