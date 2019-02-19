#ifndef PREFWAVEFORM2VIEWLAYOUT_H
#define PREFWAVEFORM2VIEWLAYOUT_H

#include "ui_prefwaveformviewlayout.h"


class PrefWaveform2ViewLayout : public QWidget, public Ui_PrefWaveformViewLayout
{
    Q_OBJECT

public:
    explicit PrefWaveform2ViewLayout( QWidget* parent = 0 );

};

#endif


