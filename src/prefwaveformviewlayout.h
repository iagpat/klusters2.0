/****************************************************************************
** Form interface generated from reading ui file './prefwaveformviewlayout.ui'
**
** Created:
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PREFWAVEFORMVIEWLAYOUT_H
#define PREFWAVEFORMVIEWLAYOUT_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class ChannelList;
class Q3GroupBox;
class QLabel;
class QSpinBox;
class KPushButton;

class PrefWaveformViewLayout : public QWidget
{
    Q_OBJECT

public:
    PrefWaveformViewLayout( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PrefWaveformViewLayout();

    Q3GroupBox* groupBox1;
    QLabel* textLabel4_2;
    QSpinBox* gainSpinBox;
    Q3GroupBox* groupBox2;
    ChannelList* channelList;
    KPushButton* saveButton;
    QLabel* textLabel1;
    KPushButton* loadButton;

protected:
    Q3GridLayout* PrefWaveformViewLayoutLayout;
    QSpacerItem* spacer1;
    Q3HBoxLayout* layout14;
    QSpacerItem* spacer16;
    Q3HBoxLayout* layout40;
    QSpacerItem* spacer7_2;
    Q3HBoxLayout* layout53;
    QSpacerItem* spacer17;
    Q3GridLayout* layout52;
    QSpacerItem* spacer9_2;
    QSpacerItem* spacer14_3;
    QSpacerItem* spacer8;
    QSpacerItem* spacer20_2_2;
    QSpacerItem* spacer20_2;
    QSpacerItem* spacer9;
    QSpacerItem* spacer20;
    QSpacerItem* spacer14;

protected slots:
    virtual void languageChange();

};

#endif // PREFWAVEFORMVIEWLAYOUT_H
