/********************************************************************************
** Form generated from reading UI file 'prefgenerallayout.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFGENERALLAYOUT_H
#define UI_PREFGENERALLAYOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <qcolorbutton.h>

QT_BEGIN_NAMESPACE

class Ui_PrefGeneralLayout
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacer1;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacer17;
    QGroupBox *groupBox2;
    QGridLayout *gridLayout_3;
    QHBoxLayout *hboxLayout1;
    QLabel *textLabel4;
    QSpinBox *undoSpinBox;
    QSpacerItem *spacer7;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacer17_2;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacer16;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout_4;
    QHBoxLayout *hboxLayout4;
    QCheckBox *crashRecoveryCheckBox;
    QSpacerItem *spacer6;
    QHBoxLayout *hboxLayout5;
    QLabel *textLabel3;
    QComboBox *crashRecoveryComboBox;
    QSpacerItem *spacer5;
    QHBoxLayout *hboxLayout6;
    QSpacerItem *spacer17_3;
    QGroupBox *groupBox2_2;
    QGridLayout *gridLayout_2;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout7;
    QLabel *textLabel4_2;
    QLineEdit *reclusteringExecutableLineEdit;
    QPushButton *reclusteringExecutableButton;
    QHBoxLayout *hboxLayout8;
    QLabel *textLabel1_2;
    QLineEdit *reclusteringArgsLineEdit;
    QGroupBox *groupBox3;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout9;
    QLabel *textLabel1;
    QColorButton *backgroundColorButton;
    QCheckBox *useWhiteColorPrinting;

    void setupUi(QWidget *PrefGeneralLayout)
    {
        if (PrefGeneralLayout->objectName().isEmpty())
            PrefGeneralLayout->setObjectName(QString::fromUtf8("PrefGeneralLayout"));
        PrefGeneralLayout->resize(450, 514);
        gridLayout = new QGridLayout(PrefGeneralLayout);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        spacer1 = new QSpacerItem(21, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacer1, 6, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacer17 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout->addItem(spacer17);

        groupBox2 = new QGroupBox(PrefGeneralLayout);
        groupBox2->setObjectName(QString::fromUtf8("groupBox2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox2->sizePolicy().hasHeightForWidth());
        groupBox2->setSizePolicy(sizePolicy);
        groupBox2->setMaximumSize(QSize(32767, 70));
        gridLayout_3 = new QGridLayout(groupBox2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        textLabel4 = new QLabel(groupBox2);
        textLabel4->setObjectName(QString::fromUtf8("textLabel4"));
        textLabel4->setWordWrap(false);

        hboxLayout1->addWidget(textLabel4);

        undoSpinBox = new QSpinBox(groupBox2);
        undoSpinBox->setObjectName(QString::fromUtf8("undoSpinBox"));
        undoSpinBox->setWrapping(true);
        undoSpinBox->setMaximum(30);
        undoSpinBox->setValue(2);

        hboxLayout1->addWidget(undoSpinBox);

        spacer7 = new QSpacerItem(81, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacer7);


        gridLayout_3->addLayout(hboxLayout1, 0, 0, 1, 1);


        hboxLayout->addWidget(groupBox2);


        gridLayout->addLayout(hboxLayout, 1, 0, 1, 1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacer17_2 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacer17_2);


        gridLayout->addLayout(hboxLayout2, 3, 0, 1, 1);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(0);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        spacer16 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout3->addItem(spacer16);

        groupBox1 = new QGroupBox(PrefGeneralLayout);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        sizePolicy.setHeightForWidth(groupBox1->sizePolicy().hasHeightForWidth());
        groupBox1->setSizePolicy(sizePolicy);
        groupBox1->setMinimumSize(QSize(370, 0));
        groupBox1->setMaximumSize(QSize(32767, 120));
        gridLayout_4 = new QGridLayout(groupBox1);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setSpacing(6);
        hboxLayout4->setContentsMargins(0, 0, 0, 0);
        hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
        crashRecoveryCheckBox = new QCheckBox(groupBox1);
        crashRecoveryCheckBox->setObjectName(QString::fromUtf8("crashRecoveryCheckBox"));

        hboxLayout4->addWidget(crashRecoveryCheckBox);

        spacer6 = new QSpacerItem(16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout4->addItem(spacer6);


        gridLayout_4->addLayout(hboxLayout4, 0, 0, 1, 1);

        hboxLayout5 = new QHBoxLayout();
        hboxLayout5->setSpacing(6);
        hboxLayout5->setContentsMargins(0, 0, 0, 0);
        hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
        textLabel3 = new QLabel(groupBox1);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
        textLabel3->setWordWrap(false);

        hboxLayout5->addWidget(textLabel3);

        crashRecoveryComboBox = new QComboBox(groupBox1);
        crashRecoveryComboBox->setObjectName(QString::fromUtf8("crashRecoveryComboBox"));

        hboxLayout5->addWidget(crashRecoveryComboBox);

        spacer5 = new QSpacerItem(326, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout5->addItem(spacer5);


        gridLayout_4->addLayout(hboxLayout5, 1, 0, 1, 1);


        hboxLayout3->addWidget(groupBox1);


        gridLayout->addLayout(hboxLayout3, 0, 0, 1, 1);

        hboxLayout6 = new QHBoxLayout();
        hboxLayout6->setSpacing(6);
        hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
        spacer17_3 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout6->addItem(spacer17_3);

        groupBox2_2 = new QGroupBox(PrefGeneralLayout);
        groupBox2_2->setObjectName(QString::fromUtf8("groupBox2_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox2_2->sizePolicy().hasHeightForWidth());
        groupBox2_2->setSizePolicy(sizePolicy1);
        groupBox2_2->setMaximumSize(QSize(32767, 140));
        gridLayout_2 = new QGridLayout(groupBox2_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout7 = new QHBoxLayout();
        hboxLayout7->setSpacing(6);
        hboxLayout7->setObjectName(QString::fromUtf8("hboxLayout7"));
        textLabel4_2 = new QLabel(groupBox2_2);
        textLabel4_2->setObjectName(QString::fromUtf8("textLabel4_2"));
        textLabel4_2->setWordWrap(false);

        hboxLayout7->addWidget(textLabel4_2);

        reclusteringExecutableLineEdit = new QLineEdit(groupBox2_2);
        reclusteringExecutableLineEdit->setObjectName(QString::fromUtf8("reclusteringExecutableLineEdit"));

        hboxLayout7->addWidget(reclusteringExecutableLineEdit);

        reclusteringExecutableButton = new QPushButton(groupBox2_2);
        reclusteringExecutableButton->setObjectName(QString::fromUtf8("reclusteringExecutableButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(reclusteringExecutableButton->sizePolicy().hasHeightForWidth());
        reclusteringExecutableButton->setSizePolicy(sizePolicy2);
        reclusteringExecutableButton->setMinimumSize(QSize(60, 29));

        hboxLayout7->addWidget(reclusteringExecutableButton);


        vboxLayout->addLayout(hboxLayout7);

        hboxLayout8 = new QHBoxLayout();
        hboxLayout8->setSpacing(6);
        hboxLayout8->setObjectName(QString::fromUtf8("hboxLayout8"));
        textLabel1_2 = new QLabel(groupBox2_2);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        textLabel1_2->setWordWrap(false);

        hboxLayout8->addWidget(textLabel1_2);

        reclusteringArgsLineEdit = new QLineEdit(groupBox2_2);
        reclusteringArgsLineEdit->setObjectName(QString::fromUtf8("reclusteringArgsLineEdit"));
        reclusteringArgsLineEdit->setMinimumSize(QSize(0, 0));

        hboxLayout8->addWidget(reclusteringArgsLineEdit);


        vboxLayout->addLayout(hboxLayout8);


        gridLayout_2->addLayout(vboxLayout, 0, 0, 1, 1);


        hboxLayout6->addWidget(groupBox2_2);


        gridLayout->addLayout(hboxLayout6, 2, 0, 1, 1);

        groupBox3 = new QGroupBox(PrefGeneralLayout);
        groupBox3->setObjectName(QString::fromUtf8("groupBox3"));
        sizePolicy.setHeightForWidth(groupBox3->sizePolicy().hasHeightForWidth());
        groupBox3->setSizePolicy(sizePolicy);
        gridLayout1 = new QGridLayout(groupBox3);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        hboxLayout9 = new QHBoxLayout();
        hboxLayout9->setSpacing(6);
        hboxLayout9->setContentsMargins(0, 0, 0, 0);
        hboxLayout9->setObjectName(QString::fromUtf8("hboxLayout9"));
        textLabel1 = new QLabel(groupBox3);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        hboxLayout9->addWidget(textLabel1);

        backgroundColorButton = new QColorButton(groupBox3);
        backgroundColorButton->setObjectName(QString::fromUtf8("backgroundColorButton"));
        sizePolicy.setHeightForWidth(backgroundColorButton->sizePolicy().hasHeightForWidth());
        backgroundColorButton->setSizePolicy(sizePolicy);
        backgroundColorButton->setMinimumSize(QSize(200, 30));

        hboxLayout9->addWidget(backgroundColorButton);


        gridLayout1->addLayout(hboxLayout9, 0, 0, 1, 1);

        useWhiteColorPrinting = new QCheckBox(groupBox3);
        useWhiteColorPrinting->setObjectName(QString::fromUtf8("useWhiteColorPrinting"));

        gridLayout1->addWidget(useWhiteColorPrinting, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox3, 4, 0, 1, 1);


        retranslateUi(PrefGeneralLayout);

        QMetaObject::connectSlotsByName(PrefGeneralLayout);
    } // setupUi

    void retranslateUi(QWidget *PrefGeneralLayout)
    {
        PrefGeneralLayout->setWindowTitle(QApplication::translate("PrefGeneralLayout", "General", 0, QApplication::UnicodeUTF8));
        groupBox2->setTitle(QApplication::translate("PrefGeneralLayout", "Undo", 0, QApplication::UnicodeUTF8));
        textLabel4->setText(QApplication::translate("PrefGeneralLayout", "Number of steps", 0, QApplication::UnicodeUTF8));
        groupBox1->setTitle(QApplication::translate("PrefGeneralLayout", "Crash and recovery", 0, QApplication::UnicodeUTF8));
        crashRecoveryCheckBox->setText(QApplication::translate("PrefGeneralLayout", "Periodically save data to a recovery file", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("PrefGeneralLayout", "Time-interval", 0, QApplication::UnicodeUTF8));
        crashRecoveryComboBox->clear();
        crashRecoveryComboBox->insertItems(0, QStringList()
         << QApplication::translate("PrefGeneralLayout", "1 min", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PrefGeneralLayout", "3 min", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PrefGeneralLayout", "5 min", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PrefGeneralLayout", "15 min", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PrefGeneralLayout", "30 min", 0, QApplication::UnicodeUTF8)
        );
        groupBox2_2->setTitle(QApplication::translate("PrefGeneralLayout", "Reclustering", 0, QApplication::UnicodeUTF8));
        textLabel4_2->setText(QApplication::translate("PrefGeneralLayout", "Executable", 0, QApplication::UnicodeUTF8));
        reclusteringExecutableButton->setText(QString());
        textLabel1_2->setText(QApplication::translate("PrefGeneralLayout", "Arguments", 0, QApplication::UnicodeUTF8));
        groupBox3->setTitle(QApplication::translate("PrefGeneralLayout", "Miscellaneous", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("PrefGeneralLayout", "Background color", 0, QApplication::UnicodeUTF8));
        backgroundColorButton->setText(QString());
        useWhiteColorPrinting->setText(QApplication::translate("PrefGeneralLayout", "Use white background when printing", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PrefGeneralLayout: public Ui_PrefGeneralLayout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFGENERALLAYOUT_H
