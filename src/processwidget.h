/***************************************************************************
 *          processwidget.h  -  description
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
/**********************************************
*   Modified by Lynn Hazan,lynn.hazan@myrealbox.com, (2004)
*
***********************************************/

#ifndef _PROCESSWIDGET_H_
#define _PROCESSWIDGET_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for KDE
#include <klistbox.h>
#include <kprinter.h>

//include files for Qt
#include <qcolor.h>

//forward declaration 
class KProcess;
class ProcessLineMaker;

class ProcessListBoxItem : public QListBoxText
{
public:
    enum Type { Diagnostic, Normal, Error };
    
    ProcessListBoxItem(const QString &s, Type type);

    inline virtual bool isCustomItem(){return false;};

    inline QColor color(){
      return ((t==Error)? Qt::darkRed : (t==Diagnostic)? Qt::black : Qt::darkBlue);
    };
    
private:
    virtual void paint(QPainter* p);
    Type t;
};


/**
 * This class launchs/stops external processes and displays their outputs.
 * @author Bernd Gehrmann, lynn hazan
 * @since klusters 1.2 
 */
class ProcessWidget : public KListBox
{
    Q_OBJECT

public:
    ProcessWidget(QWidget *parent, const char *name=0);
    ~ProcessWidget();

    /**
     * Returns whether a process is running in this view.
     */
    bool isRunning();

    /**The widget been hidden, updates its internal state.*/
    void hideWidget();

    /** Prints the contents of the view.
    * @param printer printer to print into.
    * @param filePath path of the opened document.
    */
    void print(KPrinter* printer,QString filePath);
    
public slots:
    /**
     * Starts the child process.
     * @return true if the could be started, false otherwise.
     */
    bool startJob(const QString &dir, const QString &command);
    /**
     * Kills the child processss.
     */
    void killJob();
    
    /**
     * Inserts one line from stdin into the listbox. This can
     * be overridden by subclasses to implement
     * syntax highlighting.
     */
    virtual void insertStdoutLine(const QString &line);
    /**
     * Inserts one line from stderr into the listbox. This can
     * be overridden by subclasses to implement
     * syntax highlighting. By default, a ProcessListBoxItem
     * is used.
     */
    virtual void insertStderrLine(const QString &line);

protected:
    /**
     * This is called when the child process exits.
     * The flag 'normal' is true if the process exited
     * normally (i.e. not by a signal or similar), otherwise
     * the exit status can be taken from 'status'.
     */
    virtual void childFinished(bool normal, int status);

signals:
    void processExited(KProcess*);
    void processOutputsFinished();
    void hidden();

protected:
    virtual QSize minimumSizeHint() const;
    
protected slots:
    void slotProcessExited(KProcess*);
    void slotOutputTreatmentOver();
    
private:
  /** Should be called right after an insertItem(),
  * will automatic scroll the listbox if it is already at the bottom
  * to prevent automatic scrolling when the user has scrolled up
  */
   void maybeScrollToBottom();

private:
    KProcess* childproc;
    ProcessLineMaker* procLineMaker;
};


#endif
