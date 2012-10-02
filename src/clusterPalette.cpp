/***************************************************************************
                          clusterPalette.cpp  -  description
                             -------------------
    begin                : Mon Sep  8 12:06:21 EDT 2003
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



// application specific includes
#include "klustersdoc.h"
#include "clusterPalette.h"
#include "itemcolors.h"
#include "clusterinformationdialog.h"
#include "clusterpaletteiconviewitem.h"

// include files for Qt
#include <qvariant.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qtooltip.h>

#include <qpixmap.h>
#include <qbitmap.h>
#include <qcolordialog.h>
#include <qcolor.h>
//Added by qt3to4:
#include <QFrame>
#include <QList>
//KDE includes

#include <QStatusBar>

//General C++ include files
#include <vector>

ClusterPalette::ClusterPalette(const QColor& backgroundColor,QWidget* parent,QStatusBar * statusBar, const char* name )
    : QWidget( parent ),doc(0L),mode(IMMEDIATE),isInSelectItems(false),isUpToDate(true),backgroundColor(backgroundColor),statusBar(statusBar),isInUserClusterInfoMode(false)
{
    setObjectName(name);
    QVBoxLayout *layout = new QVBoxLayout;
    //Set the palette color

    setAutoFillBackground(true);
    QPalette palette; palette.setColor(backgroundRole(), backgroundColor); setPalette(palette);
    setPaletteForegroundColor(Qt::white);

    iconView = new Q3IconView(this, "ClusterPalette");
    layout->addWidget(iconView);
    QFont font( "Helvetica",10);
    iconView->setFont(font);
    iconView->setFrameStyle(QFrame::NoFrame);
    iconView->setArrangement(Q3IconView::LeftToRight);
    iconView->setResizeMode(Q3IconView::Adjust);

    palette = iconView->palette();
    palette.setColor(iconView->backgroundRole(), backgroundColor);
    iconView->setPalette(palette);

    iconView->setPaletteBackgroundColor(backgroundColor);
    iconView->setAutoFillBackground(true);
    iconView->viewport()->setAutoFillBackground(true);
    iconView->viewport()->setPalette(palette);
    iconView->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    iconView->setVScrollBarMode(Q3ScrollView::AlwaysOff);

    int h;
    int s;
    int v;
    backgroundColor.getHsv(&h,&s,&v);
    if(s <= 80 && v >= 240 || (s <= 40 && v >= 220))
        iconView->setPaletteForegroundColor(Qt::black);
    else
        iconView->setPaletteForegroundColor(Qt::white);

    iconView->setSelectionMode(Q3IconView::Extended);
    iconView->setItemsMovable(false);
    iconView->setSpacing(4);
    QFontInfo fontInfo = QFontInfo(QFont());
    iconView->setGridX(fontInfo.pixelSize() * 2);
    iconView->setGridY(15);
    iconView->arrangeItemsInGrid();


    //Deal with the sizes
    setSizePolicy(QSizePolicy((QSizePolicy::Policy)5,(QSizePolicy::Policy)5));

    //Set the legend in the good language
    languageChange();

    //Signal and slot connection
    connect(iconView,SIGNAL(contextMenuRequested(Q3IconViewItem*,QPoint)),this, SLOT(slotRightPressed(Q3IconViewItem*)));
    connect(iconView,SIGNAL(selectionChanged()),this, SLOT(slotClickRedraw()));
    connect(iconView,SIGNAL(mouseButtonPressed(int,Q3IconViewItem*,QPoint)),this, SLOT(slotMousePressed(int,Q3IconViewItem*)));

    connect(iconView,SIGNAL(onItem(Q3IconViewItem*)),this, SLOT(slotOnItem(Q3IconViewItem*)));
    setLayout(layout);
}


ClusterPalette::~ClusterPalette()
{
    // no need to delete child widgets, Qt does it all for us
}

void ClusterPalette::createClusterList(KlustersDoc* document){
    //Assign the document to the doc member for future use
    doc = document;

    updateClusterList();
}

void ClusterPalette::updateClusterList(){
    if(!doc)
        return;
    iconView->clear();


    //Get the list of clusters with their color
    ItemColors& clusterColors = doc->clusterColors();
    int nbClusters = clusterColors.numberOfItems();

    //Construct one icon for each cluster
    QPainter painter;

    for(int i = 0; i<nbClusters; ++i){
        QPixmap pix(14,14);
        pix.fill(backgroundColor);
        painter.begin(&pix);
        painter.fillRect(0,0,12,12,clusterColors.color(i,ItemColors::BY_INDEX));
        painter.end();
        QString clusterText = QString::fromLatin1("%1").arg(clusterColors.itemId(i));

        if(isInUserClusterInfoMode){
            if(clusterColors.itemId(i) == 0){
                clusterText.append(" - ").append("artefact");
            }
            else if(clusterColors.itemId(i) == 1){
                clusterText.append(" - ").append("noise");
            }
            else{
                vector<QString> clusterInformation = vector<QString>();
                doc->data().getUserClusterInformation(clusterColors.itemId(i),clusterInformation);

                if(clusterInformation[0] != ""){
                    clusterText.append(" - ").append(clusterInformation[0]);
                }
                if(clusterInformation[1] != ""){
                    clusterText.append(", ").append(clusterInformation[1]);
                }
                if(clusterInformation[2] != ""){
                    clusterText.append(", ").append(clusterInformation[2]);
                }
                if(clusterInformation[3] != ""){
                    clusterText.append(", ").append(clusterInformation[3]);
                }
                if(clusterInformation[4] != ""){
                    clusterText.append(", ").append(clusterInformation[4]);
                }
            }
            (void)new ClusterPaletteIconViewItem(iconView,clusterText,pix);
        }
        else{
            (void)new ClusterPaletteIconViewItem(iconView,clusterText,pix);
        }
    }
}

void ClusterPalette::slotRightPressed(Q3IconViewItem* item){

    if ( !item ) return; // right pressed on viewport,pix
    else{
        ////If several options are available a poppupmenu can be added////
        /*  //Create a popmenu with an changeColor option
    QPopupMenu menu(iconView);

      int CHANGE_COLOR = menu.insertItem(clusterInformationtr("Change color"));
      clusterNumber
    menu.setMouseTracking( TRUE );
    int id = menu.exec( QCursor::pos());

    if(id == CHANGE_COLOR){
      changeColor(item);
    }*/

        int clusterNumber = doc->clusterColors().itemId(item->index());

        //The dialog is not shown for the Noise and arterfact clusters (1 and 0).
        if(clusterNumber != 0 && clusterNumber != 1){
            ClusterInformationDialog *clusterInformationDialog = new ClusterInformationDialog();
            //initizialize the dialog with the previous information
            vector<QString> clusterInformation = vector<QString>();
            doc->data().getUserClusterInformation(clusterNumber,clusterInformation);
            clusterInformationDialog->setStructure(clusterInformation[0]);
            clusterInformationDialog->setType(clusterInformation[1]);
            clusterInformationDialog->setId(clusterInformation[2]);
            clusterInformationDialog->setQuality(clusterInformation[3]);
            clusterInformationDialog->setNotes(clusterInformation[4]);

            if(clusterInformationDialog->exec() == QDialog::Accepted)
            {
                //Update the cluster user information.
                doc->data().setUserClusterInformation(doc->clusterColors().itemId(item->index()),clusterInformationDialog->getStructure(),clusterInformationDialog->getType(),clusterInformationDialog->getId(),clusterInformationDialog->getQuality(),clusterInformationDialog->getNotes());

                //update the text of the item
                if(isInUserClusterInfoMode){
                    item->setText(QString::fromLatin1("%1").arg(clusterNumber) + ": " + clusterInformationDialog->getStructure()+ ",  " + clusterInformationDialog->getType() + ", " + clusterInformationDialog->getId() + ", " + clusterInformationDialog->getQuality() + ", " + clusterInformationDialog->getNotes());

                    QString clusterText = QString::fromLatin1("%1").arg(clusterNumber);
                    bool first = true;

                    if( clusterInformationDialog->getStructure() != ""){
                        clusterText.append(" - ").append( clusterInformationDialog->getStructure());
                        first = false;
                    }
                    if(clusterInformationDialog->getType() != ""){
                        if(!first){
                            clusterText.append(", ").append(clusterInformationDialog->getType());
                        }
                        else{
                            clusterText.append(" - ").append(clusterInformationDialog->getType());
                            first = false;
                        }
                    }
                    if(clusterInformationDialog->getId() != ""){
                        if(!first){
                            clusterText.append(", ").append(clusterInformationDialog->getId());
                        }
                        else{
                            clusterText.append(" - ").append(clusterInformationDialog->getId());
                        }
                    }
                    if(clusterInformationDialog->getQuality() != ""){
                        if(!first){
                            clusterText.append(", ").append(clusterInformationDialog->getQuality());
                        }
                        else{
                            clusterText.append(" - ").append(clusterInformationDialog->getQuality());
                            first = false;
                        }
                    }
                    if(clusterInformationDialog->getNotes() != ""){
                        if(!first){
                            clusterText.append(", ").append(clusterInformationDialog->getNotes());
                        }
                        else{
                            clusterText.append(" - ").append(clusterInformationDialog->getNotes());
                            first = false;
                        }
                    }

                    item->setText(clusterText);
                }
                emit clusterInformationModified();
            }
        }
    }
}

void ClusterPalette::slotOnItem(Q3IconViewItem* item){

    if ( !item ) return; // right pressed on viewport
    else{
        ClusterPaletteIconViewItem* clusterItem = static_cast<ClusterPaletteIconViewItem*>(item);

        int clusterNumber = doc->clusterColors().itemId(item->index());

        //The information are not shown in the statusBar for the Noise and arterfact clusters (1 and 0).
        if(clusterNumber != 0 && clusterNumber != 1){
            //Update the statusbar with the cluster information
            vector<QString> clusterInformation = vector<QString>();
            doc->data().getUserClusterInformation(clusterNumber,clusterInformation);

            QString clusterText;
            bool first = true;

            if(clusterInformation[0] != ""){
                first = false;
                clusterText.append("Structure: ").append(clusterInformation[0]);
            }
            if(clusterInformation[1] != ""){
                if(first){
                    clusterText.append("Type: ").append(clusterInformation[1]);
                }
                else{
                    first = false;
                    clusterText.append(", Type: ").append(clusterInformation[1]);
                }

            }
            if(clusterInformation[2] != ""){
                if(first){
                    clusterText.append("ID: ").append(clusterInformation[2]);
                }
                else{
                    first = false;
                    clusterText.append(", ID: ").append(clusterInformation[2]);
                }

            }
            if(clusterInformation[3] != ""){
                if(first){
                    clusterText.append("Quality: ").append(clusterInformation[3]);
                }
                else{
                    first = false;
                    clusterText.append(", Quality: ").append(clusterInformation[3]);
                }
            }
            if(clusterInformation[4] != ""){
                if(first){
                    clusterText.append("Notes: ").append(clusterInformation[4]);
                }
                else{
                    first = false;
                    clusterText.append(", Notes: ").append(clusterInformation[4]);
                }
            }

            statusBar->showMessage(clusterText);

            //Ajout de l'information en tooltip
            //clusterItem->setToolTipText("Structure: " + clusterInformation[0] + ", Type: " + clusterInformation[1] + ", ID: " + clusterInformation[2] + ", Quality: " + clusterInformation[3] + ", notes: " + clusterInformation[4]);
        }
        else{
            statusBar->clearMessage();
        }
    }
}

void ClusterPalette::slotMousePressed(int button,Q3IconViewItem* item){
    if ( !item ) return; //pressed on viewport
    else{
        // middle pressed on item
        if(button == Qt::MidButton) changeColor(item);
    }
}

QList<int> ClusterPalette::selectedClusters(){
    //Get the list of clusters with their color
    if(!doc) {
        return QList<int>();
    }
    ItemColors& clusterColors = doc->clusterColors();

    QList<int> selectedClusters;

    ClusterPaletteIconViewItem* clusterPaletteItem;

    for(Q3IconViewItem* item = iconView->firstItem(); item; item = item->nextItem()){
        clusterPaletteItem  = static_cast<ClusterPaletteIconViewItem*>(item);
        if(item->isSelected()){
            selectedClusters.append(clusterColors.itemId(item->index()));
        }
    }

    //Selection has just changed
    isUpToDate = false;

    return selectedClusters;
}

void ClusterPalette::slotClickRedraw (){
    //If mode == DELAY nothing is done before a call to update
    isUpToDate = false;

    if(mode == IMMEDIATE && !isInSelectItems){
        QList<int> selection = selectedClusters();
        emit updateShownClusters(selection);
        isUpToDate = true;
    }
}


void ClusterPalette::groupClusters(){
    QList<int> selected = selectedClusters();
    //To group clusters, there must be more then one cluster !!!
    if(selected.size()>1){
        emit groupClusters(selected);
        isUpToDate = true;
    }
}

void ClusterPalette::moveClustersToNoise(){
    QList<int> selected = selectedClusters();

    if(!selected.isEmpty()){
        emit moveClustersToNoise(selected);
        isUpToDate = true;
    }
}

void ClusterPalette::moveClustersToArtefact(){
    QList<int> selected = selectedClusters();
    if(!selected.isEmpty()){
        emit moveClustersToArtefact(selected);
        isUpToDate = true;
    }
}

void ClusterPalette::updateClusters(){
    if(!isUpToDate){
        emit updateShownClusters(selectedClusters());
        isUpToDate = true;
    }
}


void ClusterPalette::changeColor(Q3IconViewItem* item){
    //Get the list of clusters with their color
    ItemColors& clusterColors = doc->clusterColors();

    int index = item->index();

    //Get the clusterColor associated with the item
    QColor color = clusterColors.color(index,ItemColors::BY_INDEX);

    QColor result = QColorDialog::getColor(color, 0);
    if (result.isValid()) {
        //Update the clusterColor
        clusterColors.setColor(index,result,ItemColors::BY_INDEX);

        if(mode == IMMEDIATE){
            //Update the icon
            QPixmap* pixmap = item->pixmap();
            QPainter painter;
            painter.begin(pixmap);
            painter.fillRect(0,0,12,12,result);
            painter.end();

            //As soon a color changes a signal is emitted
            emit singleChangeColor(clusterColors.itemId(index));
        }
        else{
            //If mode several selection before update (DELAY) => update the change status
            //The color change will be effective on the next call to either Update, Group or Delete
            clusterColors.setColorChanged(index,true,ItemColors::BY_INDEX);
            clusterColors.setColorChanged(true);

            //The view is no more up to date
            isUpToDate = false;
        }
    }
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ClusterPalette::languageChange()
{
    setWindowTitle( tr( "Cluster palette" ) );
}

void ClusterPalette::selectItems(QList<int> selectedClusters){
    //Set isInSelectItems to true to prevent the emission of signals due to selectionChange
    isInSelectItems = true;

    //unselect all the items first
    iconView->selectAll(false);

    //Loop on the clusters to be selected
    QList<int>::iterator clusterIterator;
    ClusterPaletteIconViewItem* currentIcon = 0L;
    for(clusterIterator = selectedClusters.begin(); clusterIterator != selectedClusters.end(); ++clusterIterator){
        currentIcon =  static_cast<ClusterPaletteIconViewItem*>(iconView->findItem(QString::fromLatin1("%1").arg(*clusterIterator),Q3IconView::BeginsWith));
        currentIcon->setSelected(true,true);
    }
    //Last item in selection gets focus if it exists
    if(selectedClusters.size() != 0) iconView->setCurrentItem(currentIcon);

    //reset isInSelectItems to false to enable again the the emission of signals due to selectionChange
    isInSelectItems = false;

    isUpToDate = true;
}

void ClusterPalette::reset(){
    iconView->clear();
    doc = 0L;
    mode = IMMEDIATE;
    isInSelectItems = false;
    isUpToDate = true;
}

void ClusterPalette::showUserClusterInformation(int electrodeGroupId){
    //update the flag
    isInUserClusterInfoMode = true;

    iconView->setItemTextPos(Q3IconView::Right);
    iconView->setGridX(2500);
    iconView->arrangeItemsInGrid();

    QMap<int,ClusterUserInformation> clusterUserInformationMap = QMap<int,ClusterUserInformation>();
    doc->data().getClusterUserInformation(electrodeGroupId,clusterUserInformationMap);

    ItemColors& clusterColors = doc->clusterColors();
    int clusterId;
    ClusterUserInformation currentClusterInformation;

    for(Q3IconViewItem* item = iconView->firstItem(); item; item = item->nextItem()){
        clusterId = clusterColors.itemId(item->index());

        QString clusterText = item->text();

        if(clusterId == 0){
            clusterText.append(" - ").append("artefact");
        }
        else if(clusterId == 1){
            clusterText.append(" - ").append("noise");
        }
        else{
            currentClusterInformation = clusterUserInformationMap[clusterId];
            bool first = true;

            if(currentClusterInformation.getStructure() != ""){
                first = false;
                clusterText.append(" - ").append(currentClusterInformation.getStructure());
            }
            if(currentClusterInformation.getType() != ""){
                if(!first){
                    clusterText.append(", ").append(currentClusterInformation.getType());
                }
                else{
                    clusterText.append(" - ").append(currentClusterInformation.getType());
                    first = false;
                }
            }
            if(currentClusterInformation.getId() != ""){
                if(!first){
                    clusterText.append(", ").append(currentClusterInformation.getId());
                }
                else{
                    clusterText.append(" - ").append(currentClusterInformation.getId());
                    first = false;
                }
            }
            if(currentClusterInformation.getQuality() != ""){
                if(!first){
                    clusterText.append(", ").append(currentClusterInformation.getQuality());
                }
                else{
                    clusterText.append(" - ").append(currentClusterInformation.getQuality());
                    first = false;
                }
            }
            if(currentClusterInformation.getNotes() != ""){
                if(!first){
                    clusterText.append(", ").append(currentClusterInformation.getNotes());
                }
                else{
                    clusterText.append(" - ").append(currentClusterInformation.getNotes());
                    first = false;
                }
            }
        }
        item->setText(clusterText);
    }
    iconView->setWordWrapIconText(false);
    iconView->adjustSize();
}

void ClusterPalette::hideUserClusterInformation(){
    //update the flag
    isInUserClusterInfoMode = false;

    iconView->setItemTextPos(Q3IconView::Bottom);
    //Let's go back to normal
    QFontInfo fontInfo = QFontInfo(QFont());
    iconView->setGridX(fontInfo.pixelSize() * 2);
    iconView->setGridY(15);
    iconView->arrangeItemsInGrid();

    ItemColors& clusterColors = doc->clusterColors();
    int clusterId;

    for(Q3IconViewItem* item = iconView->firstItem(); item; item = item->nextItem()){
        clusterId = clusterColors.itemId(item->index());
        item->setText(QString::fromLatin1("%1").arg(clusterId));
    }

    iconView->setWordWrapIconText(true);
    iconView->resize(this->width(),this->height());
}



#include "clusterPalette.moc"
