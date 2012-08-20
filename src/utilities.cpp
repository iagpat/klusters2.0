/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn@myrealbox.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "utilities.h"

// include files for KDE
#include <QProcess>
#include <QTemporaryFile>
//Added by qt3to4:
#include <Q3TextStream>

//include files for c/c++ libraries
#include <stdlib.h>

int Utilities::getNbLines(QString path){
 int nbLines = -1;
 
 KProcess childproc;
 const char* shellToUse = getenv("SHELL");
 if(shellToUse != NULL) childproc.setUseShell(true,shellToUse);
 else childproc.setUseShell(true);

 QTemporaryFile counterFile;//make a unique file
 childproc << "wc -l "<<path<<" > "<<counterFile.fileName();
 childproc.start(KProcess::DontCare);
 sleep(1);
 QFileInfo fi(counterFile.name());
 while(!fi.exists()){
  sleep(1);
 } 
 QFile tmpFile(counterFile.name());
 bool status = tmpFile.open(QIODevice::ReadOnly);
 
 //If the number of lines could not be determined, stop here
 if(!status) return nbLines;
 
 //Create a reader on the temp file
 Q3TextStream fileStream(&tmpFile);
 QString infoLine = fileStream.readLine();
 QString info;
 if(!infoLine.isEmpty()){
  info = infoLine.trimmed();
  QStringList parts = QStringList::split(" ", info);
  nbLines = parts[0].toLong();
  tmpFile.close();
  cout<<"nbLines "<<nbLines<<endl;
 }

  
 //Remove the temporary file
 KProcess childproc2;
 childproc2.setUseShell(true);
 childproc2 <<"rm -f "<<counterFile.name();
 bool res = childproc2.start(KProcess::DontCare);
 

 //If the number of lines could not be determined, try again
 if(infoLine.isEmpty() || info == ""){
   cout<<"infoLine == NULL || info == ''"<<endl;
  return getNbLines(path);   
 }
 
 return nbLines;
}



void Utilities::createBackup(QString path){
 QFile original(path);
 QFile backup(path+"~");
 original.open(QIODevice::ReadOnly);
 backup.open(QIODevice::WriteOnly);
 backup.write(original.readAll());
 original.close();
 backup.close();
}

