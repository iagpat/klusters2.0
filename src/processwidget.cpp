/***************************************************************************
 *          processwidget.cpp  -  description
 *            -------------------
 *
 *   Copyright (C) 1999-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application 
#include "processwidget.h"
#include "processlinemaker.h"

// include files for QT
#include <qdir.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qcursor.h>


#include <QProcess>
#include <q3paintdevicemetrics.h>

//include files for c/c++ libraries
#include <stdlib.h>

ProcessListBoxItem::ProcessListBoxItem(const QString &s, Type type)
    : Q3ListBoxText(s), t(type){}


void ProcessListBoxItem::paint(QPainter* p)
{
    p->setPen((t==Error)? Qt::darkRed :
              (t==Diagnostic)? Qt::black : Qt::darkBlue);
    Q3ListBoxText::paint(p);
}


ProcessWidget::ProcessWidget(QWidget *parent, const char *name)
    : KListBox(parent, name)
{   
    //No selection will be possible
    setSelectionMode(Q3ListBox::NoSelection);
    
    setFocusPolicy(Qt::NoFocus);
    QPalette pal = palette();
    pal.setColor(QColorGroup::HighlightedText,
                 pal.color(QPalette::Normal, QColorGroup::Text));
    pal.setColor(QColorGroup::Highlight,
                 pal.color(QPalette::Normal, QColorGroup::Mid));
    setPalette(pal);

    setCursor(QCursor(Qt::ArrowCursor));
        
    //Create the process
    childproc = new KProcess();
    const char* shellToUse = getenv("SHELL");
    if(shellToUse != NULL) childproc->setUseShell(true,shellToUse);
    else childproc->setUseShell(true);


    procLineMaker = new ProcessLineMaker( childproc );

    connect( procLineMaker, SIGNAL(receivedStdoutLine(const QString&)),
             this, SLOT(insertStdoutLine(const QString&) ));
    connect( procLineMaker, SIGNAL(receivedStderrLine(const QString&)),
             this, SLOT(insertStderrLine(const QString&) ));
    connect( procLineMaker, SIGNAL(outputTreatmentOver()),
             this, SLOT(slotOutputTreatmentOver()));

    connect(this, SIGNAL(hidden()),
             procLineMaker, SLOT(slotWidgetHidden()));

    connect(childproc, SIGNAL(processExited(KProcess*)),
            this, SLOT(slotProcessExited(KProcess*) )) ;
    connect(this, SIGNAL(processExited(KProcess*)),
            procLineMaker, SLOT(slotProcessExited())) ;    
}


ProcessWidget::~ProcessWidget()
{
    delete childproc;
    delete procLineMaker;
}


bool ProcessWidget::startJob(const QString &dir, const QString &command)
{
    procLineMaker->reset();
    clear();
    insertItem(new ProcessListBoxItem(command, ProcessListBoxItem::Diagnostic));
    childproc->clearArguments();
    if(!dir.isNull()) {
     QDir::setCurrent(dir);
    }    
    
    *childproc << command;
    return childproc->start(KProcess::NotifyOnExit, KProcess::AllOutput);
}


void ProcessWidget::killJob()
{
    childproc->kill();
    procLineMaker->processKilled();
}


bool ProcessWidget::isRunning()
{
    return childproc->isRunning();
}


void ProcessWidget::slotProcessExited(KProcess* )
{
    emit processExited(childproc);
}


void ProcessWidget::insertStdoutLine(const QString &line)
{
    insertItem(new ProcessListBoxItem(line.trimmed(),
        ProcessListBoxItem::Normal));
}


void ProcessWidget::insertStderrLine(const QString &line)
{
    insertItem(new ProcessListBoxItem(line.trimmed(),
        ProcessListBoxItem::Error));
}


void ProcessWidget::childFinished(bool normal, int status)
{
    QString s;
    ProcessListBoxItem::Type t;

    if (normal) {
        if (status) {
            s = tr("*** Exited with status: %1 ***").arg(status);
            t = ProcessListBoxItem::Error;
        } else {
            s = tr("*** Exited normally ***");
            t = ProcessListBoxItem::Diagnostic;
        }
    } else {
        s = tr("*** Process aborted ***");
        t = ProcessListBoxItem::Error;
    }

    insertItem(new ProcessListBoxItem(s, t));
}


QSize ProcessWidget::minimumSizeHint() const
{
    // I'm not sure about this, but when I don't use override minimumSizeHint(),
    // the initial size in clearly too small

    return QSize( Q3ListBox::sizeHint().width(),
                  (fontMetrics().lineSpacing()+2)*4 );
}

void ProcessWidget::maybeScrollToBottom()
{
  if ( verticalScrollBar()->value() == verticalScrollBar()->maxValue() ) {
      qApp->processEvents();
      verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
      /// \FIXME dirty hack to _actually_ scroll to the bottom
      qApp->processEvents();
      verticalScrollBar()->setValue( verticalScrollBar()->maxValue() );
  }
}

void ProcessWidget::hideWidget(){
 if(childproc->isRunning()) childproc->suspend();
 emit hidden();
}


void ProcessWidget::slotOutputTreatmentOver(){
 childFinished(childproc->normalExit(), childproc->exitStatus());
 emit processOutputsFinished();
}

void ProcessWidget::print(QPrinter *printer, QString filePath){
  QPainter printPainter;
  Q3PaintDeviceMetrics metrics(printer);// need width/height of printer surface
  const int Margin = 20;
  int yPos = 0;       // y-position for each line

  printPainter.begin(printer);

  QRect textRec = QRect(printPainter.viewport().left() + 5 ,printPainter.viewport().height() - 20,printPainter.viewport().width() - 5,20);
  QFont f("Helvetica",8);
  
  printPainter.setFont(f);
  QFontMetrics fontMetrics = printPainter.fontMetrics();

  for(int i = 0; i< numRows(); i++){
   // no more room on the current page 
   if(Margin + yPos > metrics.height() - Margin) {
    printPainter.setPen(black);
    printPainter.drawText(Margin,Margin + yPos + fontMetrics.lineSpacing(),metrics.width(),fontMetrics.lineSpacing(),
                          Qt::AlignLeft | Qt::AlignVCenter,QString("File: %1").arg(filePath));
     
    printer->newPage();  
    yPos = 0;  // back to top of page
   }
   //Draw text
   ProcessListBoxItem* boxItem = static_cast<ProcessListBoxItem*>(item(i));
   printPainter.setPen(boxItem->color());

   printPainter.drawText(Margin,Margin + yPos,
                        metrics.width(),fontMetrics.lineSpacing(),
                        ExpandTabs | DontClip, boxItem->text());

   yPos = yPos + fontMetrics.lineSpacing();
 }

 //Print the name of the file
 printPainter.resetXForm();
 printPainter.setPen(black);
 printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,QString("File: %1").arg(filePath));
 
 printPainter.end();
}


#include "processwidget.moc"
