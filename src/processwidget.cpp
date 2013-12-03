/***************************************************************************
 *          processwidget.cpp  -  description
 *            -------------------
 *
 *   Copyright (C) 1999-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//include files for the application 
#include "processwidget.h"
#include "processlinemaker.h"

// include files for QT
#include <QDir>
#include <QPainter>
#include <qapplication.h>
#include <qcursor.h>
#include <QPrinter>
#include <QScrollBar>
#include <QDebug>

#include <QProcess>


//include files for c/c++ libraries
#include <stdlib.h>

ProcessListBoxItem::ProcessListBoxItem(const QString &s, Type type)
    : QListWidgetItem(s),
      t(type)
{
}

QVariant ProcessListBoxItem::data ( int role ) const
{
    if(role == Qt::TextColorRole ){
        return ((t==Error)? QColor(Qt::darkRed) : (t==Diagnostic)? QColor(Qt::black) : QColor(Qt::darkBlue));
    }
    return QListWidgetItem::data(role);
}


ProcessWidget::ProcessWidget(QWidget *parent, const char *name)
    : QListWidget(parent)
{   
    //No selection will be possible
    setSelectionMode(QAbstractItemView::NoSelection);
    
    setFocusPolicy(Qt::NoFocus);
    QPalette pal = palette();
    pal.setColor(QPalette::HighlightedText,
                 pal.color(QPalette::Normal, QPalette::Text));
    pal.setColor(QPalette::Highlight,
                 pal.color(QPalette::Normal, QPalette::Mid));
    setPalette(pal);

    setCursor(QCursor(Qt::ArrowCursor));

    //Create the process
    childproc = new QProcess();

    procLineMaker = new ProcessLineMaker( childproc );

    connect( procLineMaker, SIGNAL(receivedStdoutLine(QString)),
             this, SLOT(insertStdoutLine(QString)));
    connect( procLineMaker, SIGNAL(receivedStderrLine(QString)),
             this, SLOT(insertStderrLine(QString)));
    connect( procLineMaker, SIGNAL(outputTreatmentOver()),
             this, SLOT(slotOutputTreatmentOver()));

    connect(this, SIGNAL(hidden()),
            procLineMaker, SLOT(slotWidgetHidden()));

    connect(childproc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(slotProcessExited(int,QProcess::ExitStatus))) ;
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)),
            procLineMaker, SLOT(slotProcessExited()));
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
    addItem(new ProcessListBoxItem(command, ProcessListBoxItem::Diagnostic));
    if(!dir.isEmpty()) {
        childproc->setWorkingDirectory(dir);
    }
    childproc->start(command);
    bool childProcStarted = childproc->waitForStarted();
    if (!childProcStarted) {
        insertStderrLine(childproc->errorString());
        childFinished((childproc->exitStatus() == QProcess::NormalExit), childproc->exitStatus());
        emit processNotStarted();
    }
    return childProcStarted;
}


void ProcessWidget::killJob()
{
    childproc->kill();
    procLineMaker->processKilled();
}


bool ProcessWidget::isRunning()
{
    return childproc->state() == QProcess::Running;
}


void ProcessWidget::slotProcessExited(int value , QProcess::ExitStatus status)
{
    emit finished(value, status);
}


void ProcessWidget::insertStdoutLine(const QString &line)
{
    addItem(new ProcessListBoxItem(line.trimmed(),
                                   ProcessListBoxItem::Normal));
}


void ProcessWidget::insertStderrLine(const QString &line)
{
    addItem(new ProcessListBoxItem(line.trimmed(),
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

    addItem(new ProcessListBoxItem(s, t));
}


QSize ProcessWidget::minimumSizeHint() const
{
    // I'm not sure about this, but when I don't use override minimumSizeHint(),
    // the initial size in clearly too small

    return QSize( QListView::sizeHint().width(),
                  (fontMetrics().lineSpacing()+2)*4 );
}

void ProcessWidget::maybeScrollToBottom()
{
    if ( verticalScrollBar()->value() == verticalScrollBar()->maximum() ) {
        qApp->processEvents();
        verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
        /// \FIXME dirty hack to _actually_ scroll to the bottom
        qApp->processEvents();
        verticalScrollBar()->setValue( verticalScrollBar()->maximum() );
    }
}

void ProcessWidget::hideWidget(){
    if(childproc->state() == QProcess::Running)
        childproc->terminate();
    emit hidden();
}


void ProcessWidget::slotOutputTreatmentOver(){
    childFinished((childproc->exitStatus() == QProcess::NormalExit), childproc->exitStatus());
    emit processOutputsFinished();
}

void ProcessWidget::print(QPrinter *printer, const QString &filePath){
    QPainter printPainter;
    const int height = printer->height();
    const int width = printer->width();
    const int Margin = 20;
    int yPos = 0;       // y-position for each line

    printPainter.begin(printer);

    QRect textRec = QRect(printPainter.viewport().left() + 5 ,printPainter.viewport().height() - 20,printPainter.viewport().width() - 5,20);
    QFont f("Helvetica",8);

    printPainter.setFont(f);
    QFontMetrics fontMetrics = printPainter.fontMetrics();

    for(int i = 0; i< count(); i++){
        // no more room on the current page
        if(Margin + yPos > height - Margin) {
            printPainter.setPen(Qt::black);
            printPainter.drawText(Margin,Margin + yPos + fontMetrics.lineSpacing(),width,fontMetrics.lineSpacing(),
                                  Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1").arg(filePath));

            printer->newPage();
            yPos = 0;  // back to top of page
        }
        //Draw text
        ProcessListBoxItem* boxItem = static_cast<ProcessListBoxItem*>(item(i));
        printPainter.setPen(boxItem->color());

        printPainter.drawText(Margin,Margin + yPos,
                              width,fontMetrics.lineSpacing(),
                              Qt::TextExpandTabs | Qt::TextDontClip, boxItem->text());

        yPos = yPos + fontMetrics.lineSpacing();
    }

    //Print the name of the file
    printPainter.resetMatrix();
    printPainter.setPen(Qt::black);
    printPainter.drawText(textRec,Qt::AlignLeft | Qt::AlignVCenter,tr("File: %1").arg(filePath));

    printPainter.end();
}
