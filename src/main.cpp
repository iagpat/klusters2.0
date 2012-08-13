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
// include files for KDE
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kurl.h>

// include files for QT
#include <qdir.h>
#include <qstring.h>

//Application include files
#include "klusters.h"

#include "timer.h"

static const char *description =
	I18N_NOOP("Klusters - Manual clustering of neuronal action potential");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE

static KCmdLineOptions options[] =
{
  { "+file", I18N_NOOP("Document to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int nbUndo;
QString version = VERSION;

int main(int argc, char* argv[])
{ 
	KAboutData aboutData( "klusters", I18N_NOOP("Klusters"),
		VERSION, description, KAboutData::License_GPL,
		"(c) 2003-2004-2005-2007, Lynn Hazan", 0, 0, "lynn.hazan@myrealbox.com");
	aboutData.addAuthor("Lynn Hazan",I18N_NOOP("Designer and developer"), "lynn.hazan@myrealbox.com");
  aboutData.addAuthor("Michael Zugaro",I18N_NOOP("Co-designed the basic data model and algorithms,\nand the interface. Tested the application."), "michael.zugaro@college-de-france.fr");
  aboutData.addAuthor("Ken Harris",I18N_NOOP("Provided the algorithms for the grouping Assistant."), "kdharris@andromeda.rutgers.edu");
	aboutData.addCredit("CMBN Members",I18N_NOOP("Helped define the set of features and tested the application."),0, "http://osiris.rutgers.edu");  
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions(options); // Add our own options.

  KApplication app;

  // see if we are starting with session management
  if (app.isRestored())
  {
    RESTORE(KlustersApp);
  }
  else
  {
    // no session.. just start up normally
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    KlustersApp* Klusters = new KlustersApp();
    Klusters->show();

    //Only one document can be open at the time
    if(args->count()){
      QString file = args->arg(0);
      if(file.left(1) != "/"){
       KURL url = KURL();
       url.setPath((QDir::currentDirPath()).append("/"));
       url.setFileName(file);
       Klusters->openDocumentFile(url);
      }
      else  Klusters->openDocumentFile(file);
    } 

		args->clear();
  }

  return app.exec();
}
