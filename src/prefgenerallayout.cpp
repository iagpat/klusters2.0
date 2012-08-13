#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './prefgenerallayout.ui'
**
** Created: jeu aoû 25 12:15:19 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "prefgenerallayout.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "kcolorbutton.h"

/*
 *  Constructs a PrefGeneralLayout as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PrefGeneralLayout::PrefGeneralLayout( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PrefGeneralLayout" );
    PrefGeneralLayoutLayout = new QGridLayout( this, 1, 1, 11, 6, "PrefGeneralLayoutLayout"); 

    layout18 = new QHBoxLayout( 0, 0, 6, "layout18"); 
    spacer17 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout18->addItem( spacer17 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox2->sizePolicy().hasHeightForWidth() ) );
    groupBox2->setMaximumSize( QSize( 32767, 70 ) );

    QWidget* privateLayoutWidget = new QWidget( groupBox2, "layout9" );
    privateLayoutWidget->setGeometry( QRect( 20, 30, 200, 24 ) );
    layout9 = new QHBoxLayout( privateLayoutWidget, 0, 6, "layout9"); 

    textLabel4 = new QLabel( privateLayoutWidget, "textLabel4" );
    layout9->addWidget( textLabel4 );

    undoSpinBox = new QSpinBox( privateLayoutWidget, "undoSpinBox" );
    undoSpinBox->setWrapping( TRUE );
    undoSpinBox->setMaxValue( 30 );
    undoSpinBox->setValue( 2 );
    layout9->addWidget( undoSpinBox );
    spacer7 = new QSpacerItem( 81, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( spacer7 );
    layout18->addWidget( groupBox2 );

    PrefGeneralLayoutLayout->addLayout( layout18, 1, 0 );
    spacer1 = new QSpacerItem( 21, 30, QSizePolicy::Minimum, QSizePolicy::Expanding );
    PrefGeneralLayoutLayout->addItem( spacer1, 4, 0 );

    layout45 = new QHBoxLayout( 0, 0, 6, "layout45"); 
    spacer17_2 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout45->addItem( spacer17_2 );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    groupBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox3->sizePolicy().hasHeightForWidth() ) );

    QWidget* privateLayoutWidget_2 = new QWidget( groupBox3, "layout16" );
    privateLayoutWidget_2->setGeometry( QRect( 20, 30, 325, 40 ) );
    layout16 = new QHBoxLayout( privateLayoutWidget_2, 0, 6, "layout16"); 

    textLabel1 = new QLabel( privateLayoutWidget_2, "textLabel1" );
    layout16->addWidget( textLabel1 );

    backgroundColorButton = new KColorButton( privateLayoutWidget_2, "backgroundColorButton" );
    backgroundColorButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, backgroundColorButton->sizePolicy().hasHeightForWidth() ) );
    backgroundColorButton->setMinimumSize( QSize( 200, 30 ) );
    layout16->addWidget( backgroundColorButton );
    layout45->addWidget( groupBox3 );

    PrefGeneralLayoutLayout->addLayout( layout45, 3, 0 );

    layout17 = new QHBoxLayout( 0, 0, 0, "layout17"); 
    spacer16 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout17->addItem( spacer16 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox1->sizePolicy().hasHeightForWidth() ) );
    groupBox1->setMinimumSize( QSize( 370, 0 ) );
    groupBox1->setMaximumSize( QSize( 32767, 120 ) );

    QWidget* privateLayoutWidget_3 = new QWidget( groupBox1, "layout6" );
    privateLayoutWidget_3->setGeometry( QRect( 20, 70, 245, 32 ) );
    layout6 = new QHBoxLayout( privateLayoutWidget_3, 0, 6, "layout6"); 

    textLabel3 = new QLabel( privateLayoutWidget_3, "textLabel3" );
    layout6->addWidget( textLabel3 );

    crashRecoveryComboBox = new QComboBox( FALSE, privateLayoutWidget_3, "crashRecoveryComboBox" );
    layout6->addWidget( crashRecoveryComboBox );
    spacer5 = new QSpacerItem( 326, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout6->addItem( spacer5 );

    QWidget* privateLayoutWidget_4 = new QWidget( groupBox1, "layout8" );
    privateLayoutWidget_4->setGeometry( QRect( 20, 30, 287, 30 ) );
    layout8 = new QHBoxLayout( privateLayoutWidget_4, 0, 6, "layout8"); 

    crashRecoveryCheckBox = new QCheckBox( privateLayoutWidget_4, "crashRecoveryCheckBox" );
    layout8->addWidget( crashRecoveryCheckBox );
    spacer6 = new QSpacerItem( 16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout8->addItem( spacer6 );
    layout17->addWidget( groupBox1 );

    PrefGeneralLayoutLayout->addLayout( layout17, 0, 0 );

    layout33 = new QHBoxLayout( 0, 0, 6, "layout33"); 
    spacer17_3 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout33->addItem( spacer17_3 );

    groupBox2_2 = new QGroupBox( this, "groupBox2_2" );
    groupBox2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, groupBox2_2->sizePolicy().hasHeightForWidth() ) );
    groupBox2_2->setMaximumSize( QSize( 32767, 140 ) );

    QWidget* privateLayoutWidget_5 = new QWidget( groupBox2_2, "layout32" );
    privateLayoutWidget_5->setGeometry( QRect( 17, 44, 370, 70 ) );
    layout32 = new QVBoxLayout( privateLayoutWidget_5, 11, 0, "layout32"); 

    layout31 = new QHBoxLayout( 0, 0, 6, "layout31"); 

    textLabel4_2 = new QLabel( privateLayoutWidget_5, "textLabel4_2" );
    layout31->addWidget( textLabel4_2 );

    reclusteringExecutableLineEdit = new QLineEdit( privateLayoutWidget_5, "reclusteringExecutableLineEdit" );
    layout31->addWidget( reclusteringExecutableLineEdit );

    reclusteringExecutableButton = new QPushButton( privateLayoutWidget_5, "reclusteringExecutableButton" );
    reclusteringExecutableButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, reclusteringExecutableButton->sizePolicy().hasHeightForWidth() ) );
    reclusteringExecutableButton->setMinimumSize( QSize( 60, 29 ) );
    layout31->addWidget( reclusteringExecutableButton );
    layout32->addLayout( layout31 );

    layout28 = new QHBoxLayout( 0, 0, 6, "layout28"); 

    textLabel1_2 = new QLabel( privateLayoutWidget_5, "textLabel1_2" );
    layout28->addWidget( textLabel1_2 );

    reclusteringArgsLineEdit = new QLineEdit( privateLayoutWidget_5, "reclusteringArgsLineEdit" );
    reclusteringArgsLineEdit->setMinimumSize( QSize( 0, 0 ) );
    layout28->addWidget( reclusteringArgsLineEdit );
    layout32->addLayout( layout28 );
    layout33->addWidget( groupBox2_2 );

    PrefGeneralLayoutLayout->addLayout( layout33, 2, 0 );
    languageChange();
    resize( QSize(450, 514).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PrefGeneralLayout::~PrefGeneralLayout()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PrefGeneralLayout::languageChange()
{
    setCaption( tr2i18n( "General" ) );
    groupBox2->setTitle( tr2i18n( "Undo" ) );
    textLabel4->setText( tr2i18n( "Number of steps" ) );
    groupBox3->setTitle( tr2i18n( "Miscellaneous" ) );
    textLabel1->setText( tr2i18n( "Background color" ) );
    backgroundColorButton->setText( QString::null );
    groupBox1->setTitle( tr2i18n( "Crash and recovery" ) );
    textLabel3->setText( tr2i18n( "Time-interval" ) );
    crashRecoveryComboBox->clear();
    crashRecoveryComboBox->insertItem( tr2i18n( "1 min" ) );
    crashRecoveryComboBox->insertItem( tr2i18n( "3 min" ) );
    crashRecoveryComboBox->insertItem( tr2i18n( "5 min" ) );
    crashRecoveryComboBox->insertItem( tr2i18n( "15 min" ) );
    crashRecoveryComboBox->insertItem( tr2i18n( "30 min" ) );
    crashRecoveryCheckBox->setText( tr2i18n( "Periodically save data to a recovery file" ) );
    groupBox2_2->setTitle( tr2i18n( "Reclustering" ) );
    textLabel4_2->setText( tr2i18n( "Executable" ) );
    reclusteringExecutableButton->setText( QString::null );
    textLabel1_2->setText( tr2i18n( "Arguments" ) );
}

#include "prefgenerallayout.moc"
