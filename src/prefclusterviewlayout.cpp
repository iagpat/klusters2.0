#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './prefclusterviewlayout.ui'
**
** Created: jeu aoû 25 12:15:18 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "prefclusterviewlayout.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PrefClusterViewLayout as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PrefClusterViewLayout::PrefClusterViewLayout( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PrefClusterViewLayout" );
    PrefClusterViewLayoutLayout = new QGridLayout( this, 1, 1, 11, 6, "PrefClusterViewLayoutLayout"); 
    spacer21 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    PrefClusterViewLayoutLayout->addItem( spacer21, 1, 0 );

    layout57 = new QGridLayout( 0, 1, 1, 0, 6, "layout57"); 
    spacer18 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout57->addItem( spacer18, 0, 0 );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    groupBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox3->sizePolicy().hasHeightForWidth() ) );
    groupBox3->setMinimumSize( QSize( 320, 70 ) );

    QWidget* privateLayoutWidget = new QWidget( groupBox3, "layout55" );
    privateLayoutWidget->setGeometry( QRect( 13, 28, 305, 29 ) );
    layout55 = new QHBoxLayout( privateLayoutWidget, 0, 6, "layout55"); 

    textLabel6 = new QLabel( privateLayoutWidget, "textLabel6" );
    textLabel6->setMinimumSize( QSize( 0, 15 ) );
    layout55->addWidget( textLabel6 );

    intervalSpinBox = new QSpinBox( privateLayoutWidget, "intervalSpinBox" );
    intervalSpinBox->setMinimumSize( QSize( 60, 0 ) );
    intervalSpinBox->setWrapping( TRUE );
    intervalSpinBox->setMaxValue( 6000 );
    intervalSpinBox->setMinValue( 60 );
    intervalSpinBox->setLineStep( 60 );
    layout55->addWidget( intervalSpinBox );
    spacer19 = new QSpacerItem( 60, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout55->addItem( spacer19 );

    layout57->addWidget( groupBox3, 0, 1 );

    PrefClusterViewLayoutLayout->addLayout( layout57, 0, 0 );
    languageChange();
    resize( QSize(377, 155).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PrefClusterViewLayout::~PrefClusterViewLayout()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PrefClusterViewLayout::languageChange()
{
    setCaption( tr2i18n( "ClusterView" ) );
    groupBox3->setTitle( tr2i18n( "Time lines" ) );
    textLabel6->setText( tr2i18n( "Time interval (in seconds)" ) );
}

#include "prefclusterviewlayout.moc"
