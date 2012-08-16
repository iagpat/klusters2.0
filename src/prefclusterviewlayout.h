/****************************************************************************
** Form interface generated from reading ui file './prefclusterviewlayout.ui'
**
** Created:
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PREFCLUSTERVIEWLAYOUT_H
#define PREFCLUSTERVIEWLAYOUT_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class Q3GroupBox;
class QLabel;
class QSpinBox;

class PrefClusterViewLayout : public QWidget
{
    Q_OBJECT

public:
    PrefClusterViewLayout( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PrefClusterViewLayout();

    Q3GroupBox* groupBox3;
    QLabel* textLabel6;
    QSpinBox* intervalSpinBox;

protected:
    Q3GridLayout* PrefClusterViewLayoutLayout;
    QSpacerItem* spacer21;
    Q3GridLayout* layout57;
    QSpacerItem* spacer18;
    Q3HBoxLayout* layout55;
    QSpacerItem* spacer19;

protected slots:
    virtual void languageChange();

};

#endif // PREFCLUSTERVIEWLAYOUT_H
