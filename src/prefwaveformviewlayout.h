#ifndef PREFWAVEFORMVIEWLAYOUT_H
#define PREFWAVEFORMVIEWLAYOUT_H

#include "ui_prefwaveformviewlayout.h"


class PrefWaveformViewLayout : public QWidget, public Ui_PrefWaveformViewLayout
{
    Q_OBJECT

public:
    explicit PrefWaveformViewLayout( QWidget* parent = 0 );

};

#endif
