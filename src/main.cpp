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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


// include files for QT
#include <QDir>
#include <QString>
#include <QApplication>
#include <QAction>
#include <QDebug>
//Application include files
#include "klusters.h"

#include "timer.h"
#include "config-klusters.h"

int nbUndo;

int main(int argc, char* argv[])
{ 
    QApplication::setGraphicsSystem("raster");
    QApplication::setOrganizationName("sourceforge");
    QApplication::setOrganizationDomain("sourceforge.net");
    QApplication::setApplicationName("klusters");

    QApplication app(argc, argv); 

    QStringList args = QApplication::arguments();
    QStringList argsList;
    for (int i = 1, n = args.size(); i < n; ++i) {
        const QString arg = args.at(i);
        if (arg == "-h" || arg == "--help" || arg == "-help") {
            qWarning() << "Usage: " << qPrintable(args.at(0))
                       << " [file]"
                       << "\n\n"
                       << "Arguments:\n"
                       << "  -h, --help              print this help\n";
            return 1;
        }
        qDebug()<<" QString::fromLocal8Bit(argv[i]) :"<<QString::fromLocal8Bit(argv[i]);
        argsList.push_back(QString::fromLocal8Bit(argv[i]));
    }


    KlustersApp* Klusters = new KlustersApp();
    Klusters->show();
    if(argsList.count()){
        const QString file = argsList.at(0);
        QFileInfo fInfo(file);
        if (file.startsWith(QLatin1String("-")) ) {
            qWarning() << "it's not a filename :"<<file;
        }
        else if(fInfo.isRelative()) {
            QString url;
            url = QDir::currentPath()+ QDir::separator() + file;
            Klusters->openDocumentFile(url);
        }
        else {
            Klusters->openDocumentFile(file);
        }
    }
    int ret = app.exec();
    delete Klusters;
    return ret;
}
