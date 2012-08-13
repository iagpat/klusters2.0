#include <kdialog.h>
#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './prefwaveformviewlayout.ui'
**
** Created: jeu aoû 25 12:15:20 2011
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "prefwaveformviewlayout.h"

#include <qvariant.h>
#include <channellist.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PrefWaveformViewLayout as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PrefWaveformViewLayout::PrefWaveformViewLayout( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PrefWaveformViewLayout" );
    setMaximumSize( QSize( 32767, 490 ) );
    PrefWaveformViewLayoutLayout = new QGridLayout( this, 1, 1, 11, 6, "PrefWaveformViewLayoutLayout"); 

    layout14 = new QHBoxLayout( 0, 0, 6, "layout14"); 
    spacer16 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout14->addItem( spacer16 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox1->sizePolicy().hasHeightForWidth() ) );
    groupBox1->setMinimumSize( QSize( 280, 0 ) );

    QWidget* privateLayoutWidget = new QWidget( groupBox1, "layout40" );
    privateLayoutWidget->setGeometry( QRect( 21, 29, 202, 24 ) );
    layout40 = new QHBoxLayout( privateLayoutWidget, 0, 6, "layout40"); 

    textLabel4_2 = new QLabel( privateLayoutWidget, "textLabel4_2" );
    layout40->addWidget( textLabel4_2 );

    gainSpinBox = new QSpinBox( privateLayoutWidget, "gainSpinBox" );
    gainSpinBox->setWrapping( TRUE );
    gainSpinBox->setMaxValue( 6000 );
    gainSpinBox->setMinValue( 200 );
    gainSpinBox->setLineStep( 20 );
    gainSpinBox->setValue( 200 );
    layout40->addWidget( gainSpinBox );
    spacer7_2 = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout40->addItem( spacer7_2 );
    layout14->addWidget( groupBox1 );

    PrefWaveformViewLayoutLayout->addLayout( layout14, 0, 0 );
    spacer1 = new QSpacerItem( 21, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    PrefWaveformViewLayoutLayout->addItem( spacer1, 2, 0 );

    layout53 = new QHBoxLayout( 0, 0, 6, "layout53"); 
    spacer17 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout53->addItem( spacer17 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, groupBox2->sizePolicy().hasHeightForWidth() ) );
    groupBox2->setMinimumSize( QSize( 500, 270 ) );

    QWidget* privateLayoutWidget_2 = new QWidget( groupBox2, "layout52" );
    privateLayoutWidget_2->setGeometry( QRect( 20, 30, 473, 226 ) );
    layout52 = new QGridLayout( privateLayoutWidget_2, 1, 1, 11, 6, "layout52"); 
    spacer9_2 = new QSpacerItem( 79, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout52->addItem( spacer9_2, 4, 6 );

    channelList = new ChannelList( privateLayoutWidget_2, "channelList" );
    channelList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, channelList->sizePolicy().hasHeightForWidth() ) );
    channelList->setMinimumSize( QSize( 20, 0 ) );

    layout52->addMultiCellWidget( channelList, 0, 3, 0, 0 );
    spacer14_3 = new QSpacerItem( 30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout52->addItem( spacer14_3, 1, 7 );
    spacer8 = new QSpacerItem( 77, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout52->addItem( spacer8, 4, 4 );
    spacer20_2_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout52->addItem( spacer20_2_2, 5, 3 );
    spacer20_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout52->addItem( spacer20_2, 5, 5 );
    spacer9 = new QSpacerItem( 79, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout52->addItem( spacer9, 4, 2 );
    spacer20 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout52->addItem( spacer20, 2, 5 );

    saveButton = new KPushButton( privateLayoutWidget_2, "saveButton" );

    layout52->addMultiCellWidget( saveButton, 3, 4, 3, 3 );
    spacer14 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout52->addItem( spacer14, 0, 1 );

    textLabel1 = new QLabel( privateLayoutWidget_2, "textLabel1" );

    layout52->addMultiCellWidget( textLabel1, 0, 1, 2, 6 );

    loadButton = new KPushButton( privateLayoutWidget_2, "loadButton" );

    layout52->addMultiCellWidget( loadButton, 3, 4, 5, 5 );
    layout53->addWidget( groupBox2 );

    PrefWaveformViewLayoutLayout->addLayout( layout53, 1, 0 );
    languageChange();
    resize( QSize(559, 426).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PrefWaveformViewLayout::~PrefWaveformViewLayout()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PrefWaveformViewLayout::languageChange()
{
    setCaption( tr2i18n( "Waveformview" ) );
    groupBox1->setTitle( tr2i18n( "Waveform gain" ) );
    textLabel4_2->setText( tr2i18n( "Default gain" ) );
    groupBox2->setTitle( tr2i18n( "Channel positions" ) );
    saveButton->setText( tr2i18n( "Save" ) );
    textLabel1->setText( tr2i18n( "<p>Use the mouse to move the channel IDs up or down the list to fit the physical layout of the channels. Layouts can be saved or loaded using the buttons at the bottom</p>\n"
"\n"
"<p><b>Note: </b> a document must be opened in order to use this feature.</p>" ) );
    loadButton->setText( tr2i18n( "Load" ) );
}

#include "prefwaveformviewlayout.moc"
