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

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KColorButton;
class QGroupBox;
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

    QGroupBox* groupBox2;
    QLabel* textLabel4;
    QSpinBox* undoSpinBox;
    QGroupBox* groupBox3;
    QLabel* textLabel1;
    KColorButton* backgroundColorButton;
    QGroupBox* groupBox1;
    QLabel* textLabel3;
    QComboBox* crashRecoveryComboBox;
    QCheckBox* crashRecoveryCheckBox;
    QGroupBox* groupBox2_2;
    QLabel* textLabel4_2;
    QLineEdit* reclusteringExecutableLineEdit;
    QPushButton* reclusteringExecutableButton;
    QLabel* textLabel1_2;
    QLineEdit* reclusteringArgsLineEdit;

protected:
    QGridLayout* PrefGeneralLayoutLayout;
    QSpacerItem* spacer1;
    QHBoxLayout* layout18;
    QSpacerItem* spacer17;
    QHBoxLayout* layout9;
    QSpacerItem* spacer7;
    QHBoxLayout* layout45;
    QSpacerItem* spacer17_2;
    QHBoxLayout* layout16;
    QHBoxLayout* layout17;
    QSpacerItem* spacer16;
    QHBoxLayout* layout6;
    QSpacerItem* spacer5;
    QHBoxLayout* layout8;
    QSpacerItem* spacer6;
    QHBoxLayout* layout33;
    QSpacerItem* spacer17_3;
    QVBoxLayout* layout32;
    QHBoxLayout* layout31;
    QHBoxLayout* layout28;

protected slots:
    virtual void languageChange();

};

#endif // PREFGENERALLAYOUT_H
