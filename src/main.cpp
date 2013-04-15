/***************************************************************************
                          main.cpp  -  description
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


// include files for QT
#include <qdir.h>
#include <qstring.h>
#include <QApplication>
#include <QAction>

//Application include files
#include "klusters.h"

#include "timer.h"
#include "config-klusters.h"

int nbUndo;

int main(int argc, char* argv[])
{ 
    QApplication::setOrganizationName("sourceforge");
    QApplication::setOrganizationDomain("sourceforge.net");
    QApplication::setApplicationName("klusters");

    QStringList args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(QString::fromLocal8Bit(argv[i]));
    }
    QApplication app(argc, argv);

    KlustersApp* Klusters = new KlustersApp();
    Klusters->show();
    if(args.count()){
        const QString file = args.at(0);
        if(file.left(1) != QLatin1String("/")){
            QString url;
            url = QDir::currentPath()+ QDir::separator() + file;
            Klusters->openDocumentFile(url);
        } else {
            Klusters->openDocumentFile(file);
        }
    }
    int ret = app.exec();
    delete Klusters;
    return ret;
}
