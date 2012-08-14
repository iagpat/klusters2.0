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

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class ChannelList;
class QGroupBox;
class QLabel;
class QSpinBox;
class KPushButton;

class PrefWaveformViewLayout : public QWidget
{
    Q_OBJECT

public:
    PrefWaveformViewLayout( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PrefWaveformViewLayout();

    QGroupBox* groupBox1;
    QLabel* textLabel4_2;
    QSpinBox* gainSpinBox;
    QGroupBox* groupBox2;
    ChannelList* channelList;
    KPushButton* saveButton;
    QLabel* textLabel1;
    KPushButton* loadButton;

protected:
    QGridLayout* PrefWaveformViewLayoutLayout;
    QSpacerItem* spacer1;
    QHBoxLayout* layout14;
    QSpacerItem* spacer16;
    QHBoxLayout* layout40;
    QSpacerItem* spacer7_2;
    QHBoxLayout* layout53;
    QSpacerItem* spacer17;
    QGridLayout* layout52;
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
