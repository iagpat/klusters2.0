/****************************************************************************
** Form interface generated from reading ui file './prefgenerallayout.ui'
**
** Created:
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PREFGENERALLAYOUT_H
#define PREFGENERALLAYOUT_H

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
class KColorButton;
class Q3GroupBox;
class QLabel;
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QPushButton;

class PrefGeneralLayout : public QWidget
{
    Q_OBJECT

public:
    PrefGeneralLayout( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PrefGeneralLayout();

    Q3GroupBox* groupBox2;
    QLabel* textLabel4;
    QSpinBox* undoSpinBox;
    Q3GroupBox* groupBox3;
    QLabel* textLabel1;
    KColorButton* backgroundColorButton;
    Q3GroupBox* groupBox1;
    QLabel* textLabel3;
    QComboBox* crashRecoveryComboBox;
    QCheckBox* crashRecoveryCheckBox;
    Q3GroupBox* groupBox2_2;
    QLabel* textLabel4_2;
    QLineEdit* reclusteringExecutableLineEdit;
    QPushButton* reclusteringExecutableButton;
    QLabel* textLabel1_2;
    QLineEdit* reclusteringArgsLineEdit;

protected:
    Q3GridLayout* PrefGeneralLayoutLayout;
    QSpacerItem* spacer1;
    Q3HBoxLayout* layout18;
    QSpacerItem* spacer17;
    Q3HBoxLayout* layout9;
    QSpacerItem* spacer7;
    Q3HBoxLayout* layout45;
    QSpacerItem* spacer17_2;
    Q3HBoxLayout* layout16;
    Q3HBoxLayout* layout17;
    QSpacerItem* spacer16;
    Q3HBoxLayout* layout6;
    QSpacerItem* spacer5;
    Q3HBoxLayout* layout8;
    QSpacerItem* spacer6;
    Q3HBoxLayout* layout33;
    QSpacerItem* spacer17_3;
    Q3VBoxLayout* layout32;
    Q3HBoxLayout* layout31;
    Q3HBoxLayout* layout28;

protected slots:
    virtual void languageChange();

};

#endif // PREFGENERALLAYOUT_H
