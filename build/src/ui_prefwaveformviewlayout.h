/********************************************************************************
** Form generated from reading UI file 'prefwaveformviewlayout.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFWAVEFORMVIEWLAYOUT_H
#define UI_PREFWAVEFORMVIEWLAYOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <channellist.h>
#include "channellist.h"

QT_BEGIN_NAMESPACE

class Ui_PrefWaveformViewLayout
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacer16;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout1;
    QHBoxLayout *hboxLayout1;
    QLabel *textLabel4_2;
    QSpinBox *gainSpinBox;
    QSpacerItem *spacer7_2;
    QSpacerItem *spacer1;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacer17;
    QGroupBox *groupBox2;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout2;
    QSpacerItem *spacer9_2;
    ChannelList *channelList;
    QSpacerItem *spacer14_3;
    QSpacerItem *spacer8;
    QSpacerItem *spacer20_2_2;
    QSpacerItem *spacer20_2;
    QSpacerItem *spacer9;
    QSpacerItem *spacer20;
    QPushButton *saveButton;
    QSpacerItem *spacer14;
    QLabel *textLabel1;
    QPushButton *loadButton;

    void setupUi(QWidget *PrefWaveformViewLayout)
    {
        if (PrefWaveformViewLayout->objectName().isEmpty())
            PrefWaveformViewLayout->setObjectName(QString::fromUtf8("PrefWaveformViewLayout"));
        PrefWaveformViewLayout->resize(559, 426);
        PrefWaveformViewLayout->setMaximumSize(QSize(32767, 490));
        gridLayout = new QGridLayout(PrefWaveformViewLayout);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacer16 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout->addItem(spacer16);

        groupBox1 = new QGroupBox(PrefWaveformViewLayout);
        groupBox1->setObjectName(QString::fromUtf8("groupBox1"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox1->sizePolicy().hasHeightForWidth());
        groupBox1->setSizePolicy(sizePolicy);
        groupBox1->setMinimumSize(QSize(280, 0));
        gridLayout1 = new QGridLayout(groupBox1);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        textLabel4_2 = new QLabel(groupBox1);
        textLabel4_2->setObjectName(QString::fromUtf8("textLabel4_2"));
        textLabel4_2->setWordWrap(false);

        hboxLayout1->addWidget(textLabel4_2);

        gainSpinBox = new QSpinBox(groupBox1);
        gainSpinBox->setObjectName(QString::fromUtf8("gainSpinBox"));
        gainSpinBox->setWrapping(true);
        gainSpinBox->setMinimum(200);
        gainSpinBox->setMaximum(6000);
        gainSpinBox->setSingleStep(20);
        gainSpinBox->setValue(200);

        hboxLayout1->addWidget(gainSpinBox);

        spacer7_2 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacer7_2);


        gridLayout1->addLayout(hboxLayout1, 0, 0, 1, 1);


        hboxLayout->addWidget(groupBox1);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 1);

        spacer1 = new QSpacerItem(21, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacer1, 2, 0, 1, 1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        spacer17 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacer17);

        groupBox2 = new QGroupBox(PrefWaveformViewLayout);
        groupBox2->setObjectName(QString::fromUtf8("groupBox2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox2->sizePolicy().hasHeightForWidth());
        groupBox2->setSizePolicy(sizePolicy1);
        groupBox2->setMinimumSize(QSize(500, 270));
        gridLayout_2 = new QGridLayout(groupBox2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout2 = new QGridLayout();
        gridLayout2->setSpacing(6);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        spacer9_2 = new QSpacerItem(79, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacer9_2, 4, 6, 1, 1);

        channelList = new ChannelList(groupBox2);
        channelList->setObjectName(QString::fromUtf8("channelList"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(channelList->sizePolicy().hasHeightForWidth());
        channelList->setSizePolicy(sizePolicy2);
        channelList->setMinimumSize(QSize(20, 0));

        gridLayout2->addWidget(channelList, 0, 0, 4, 1);

        spacer14_3 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout2->addItem(spacer14_3, 1, 7, 1, 1);

        spacer8 = new QSpacerItem(77, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacer8, 4, 4, 1, 1);

        spacer20_2_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacer20_2_2, 5, 3, 1, 1);

        spacer20_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout2->addItem(spacer20_2, 5, 5, 1, 1);

        spacer9 = new QSpacerItem(79, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacer9, 4, 2, 1, 1);

        spacer20 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout2->addItem(spacer20, 2, 5, 1, 1);

        saveButton = new QPushButton(groupBox2);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        gridLayout2->addWidget(saveButton, 3, 3, 2, 1);

        spacer14 = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout2->addItem(spacer14, 0, 1, 1, 1);

        textLabel1 = new QLabel(groupBox2);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(true);

        gridLayout2->addWidget(textLabel1, 0, 2, 2, 5);

        loadButton = new QPushButton(groupBox2);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));

        gridLayout2->addWidget(loadButton, 3, 5, 2, 1);


        gridLayout_2->addLayout(gridLayout2, 0, 0, 1, 1);


        hboxLayout2->addWidget(groupBox2);


        gridLayout->addLayout(hboxLayout2, 1, 0, 1, 1);


        retranslateUi(PrefWaveformViewLayout);

        QMetaObject::connectSlotsByName(PrefWaveformViewLayout);
    } // setupUi

    void retranslateUi(QWidget *PrefWaveformViewLayout)
    {
        PrefWaveformViewLayout->setWindowTitle(QApplication::translate("PrefWaveformViewLayout", "Waveformview", 0, QApplication::UnicodeUTF8));
        groupBox1->setTitle(QApplication::translate("PrefWaveformViewLayout", "Waveform gain", 0, QApplication::UnicodeUTF8));
        textLabel4_2->setText(QApplication::translate("PrefWaveformViewLayout", "Default gain", 0, QApplication::UnicodeUTF8));
        groupBox2->setTitle(QApplication::translate("PrefWaveformViewLayout", "Channel positions", 0, QApplication::UnicodeUTF8));
        saveButton->setText(QApplication::translate("PrefWaveformViewLayout", "Save", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("PrefWaveformViewLayout", "<p>Use the mouse to move the channel IDs up or down the list to fit the physical layout of the channels. Layouts can be saved or loaded using the buttons at the bottom</p>\n"
"\n"
"<p><b>Note: </b> a document must be opened in order to use this feature.</p>", 0, QApplication::UnicodeUTF8));
        loadButton->setText(QApplication::translate("PrefWaveformViewLayout", "Load", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PrefWaveformViewLayout: public Ui_PrefWaveformViewLayout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFWAVEFORMVIEWLAYOUT_H
