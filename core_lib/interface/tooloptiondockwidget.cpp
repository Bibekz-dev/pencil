#include <QLabel>
#include <QToolButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QSettings>
#include <QDebug>
#include "spinslider.h"
#include "toolmanager.h"
#include "tooloptiondockwidget.h"
#include "editor.h"

ToolOptionWidget::ToolOptionWidget( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle( tr( "Options", "Window title of tool options like pen width, feather etc.." ) );
}

ToolOptionWidget::~ToolOptionWidget()
{
}

void ToolOptionWidget::initUI()
{
    createUI();
}

void ToolOptionWidget::updateUI()
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    Q_ASSERT( currentTool );

    disableAllOptions();

    sizeSlider->setVisible( currentTool->isPropertyEnabled( WIDTH ) );
    featherSlider->setVisible( currentTool->isPropertyEnabled( FEATHER ) );
    usePressureBox->setVisible( currentTool->isPropertyEnabled( PRESSURE ) );
    makeInvisibleBox->setVisible( currentTool->isPropertyEnabled( INVISIBILITY ) );
    preserveAlphaBox->setVisible( currentTool->isPropertyEnabled( PRESERVEALPHA ) );

    const Properties& p = currentTool->properties;

    setPenWidth( p.width );
    setPenFeather( p.feather );
    setPressure( p.pressure );
    setPenInvisibility( p.invisibility );
    setPreserveAlpha( p.preserveAlpha );
}

void ToolOptionWidget::createUI()
{
    setMinimumWidth( 110 );

    QFrame* optionGroup = new QFrame();
    QGridLayout* pLayout = new QGridLayout();
    pLayout->setMargin( 8 );
    pLayout->setSpacing( 8 );

    QSettings settings( "Pencil", "Pencil" );

    sizeSlider = new SpinSlider( tr( "Size" ), "log", "real", 0.1, 200.0, this );
    sizeSlider->setValue( settings.value( "pencilWidth" ).toDouble() );
    sizeSlider->setToolTip( tr( "Set Pen Width <br><b>[SHIFT]+drag</b><br>for quick adjustment" ) );

    featherSlider = new SpinSlider( tr( "Feather" ), "log", "real", 0.0, 100.0, this );
    featherSlider->setValue( settings.value( "pencilFeather" ).toDouble() );
    featherSlider->setToolTip( tr( "Set Pen Feather <br><b>[CTRL]+drag</b><br>for quick adjustment" ) );

    usePressureBox = new QCheckBox( tr( "Pressure" ) );
    usePressureBox->setToolTip( tr( "Size with pressure" ) );
    usePressureBox->setFont( QFont( "Helvetica", 10 ) );
    usePressureBox->setChecked( true );

    makeInvisibleBox = new QCheckBox( tr( "Invisible" ) );
    makeInvisibleBox->setToolTip( tr( "Make invisible" ) );
    makeInvisibleBox->setFont( QFont( "Helvetica", 10 ) );
    makeInvisibleBox->setChecked( false );

    preserveAlphaBox = new QCheckBox( tr( "Alpha" ) );
    preserveAlphaBox->setToolTip( tr( "Preserve Alpha" ) );
    preserveAlphaBox->setFont( QFont( "Helvetica", 10 ) );
    preserveAlphaBox->setChecked( false );

    pLayout->addWidget( sizeSlider, 8, 0, 1, 2 );
    pLayout->addWidget( featherSlider, 9, 0, 1, 2 );
    pLayout->addWidget( usePressureBox, 11, 0, 1, 2 );
    pLayout->addWidget( preserveAlphaBox, 12, 0, 1, 2 );
    pLayout->addWidget( makeInvisibleBox, 14, 0, 1, 2 );

    pLayout->setRowStretch( 15, 1 );

    optionGroup->setLayout( pLayout );

    setWidget( optionGroup );
}

void ToolOptionWidget::makeConnectionToEditor( Editor* editor )
{
    auto toolManager = editor->tools();

    connect( usePressureBox, &QCheckBox::clicked, toolManager, &ToolManager::setPressure );
    connect( makeInvisibleBox, &QCheckBox::clicked, toolManager, &ToolManager::setInvisibility );
    connect( preserveAlphaBox, &QCheckBox::clicked, toolManager, &ToolManager::setPreserveAlpha );

    connect( sizeSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setWidth );
    connect( featherSlider, &SpinSlider::valueChanged, toolManager, &ToolManager::setFeather );

    connect( toolManager, &ToolManager::toolChanged, this, &ToolOptionWidget::onToolChanged );
    connect( toolManager, &ToolManager::toolPropertyChanged, this, &ToolOptionWidget::onToolPropertyChanged );
}

void ToolOptionWidget::onToolPropertyChanged( ToolType, ToolPropertyType ePropertyType )
{
    const Properties& p = editor()->tools()->currentTool()->properties;

    switch ( ePropertyType )
    {
        case WIDTH: 
            setPenWidth( p.width );
            break;
        case FEATHER:
            setPenFeather( p.feather );
            break;
        case PRESSURE:
            setPressure( p.pressure );
            break;
        case INVISIBILITY:
            setPenInvisibility( p.invisibility );
            break;
        case PRESERVEALPHA:
            setPreserveAlpha( p.preserveAlpha );
            break;
    }
}

void ToolOptionWidget::onToolChanged( ToolType )
{
    updateUI();
}

void ToolOptionWidget::setPenWidth( qreal width )
{
    QSignalBlocker b( sizeSlider );
    sizeSlider->setEnabled( true );
    sizeSlider->setValue( width );
}

void ToolOptionWidget::setPenFeather( qreal featherValue )
{
    QSignalBlocker b( featherSlider );
    featherSlider->setEnabled( true );
    featherSlider->setValue( featherValue );
}

void ToolOptionWidget::setPenInvisibility( int x )
{
    QSignalBlocker b( makeInvisibleBox );
    makeInvisibleBox->setEnabled( true );
    makeInvisibleBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPressure( int x )
{
    QSignalBlocker b( usePressureBox );
    usePressureBox->setEnabled( true );
    usePressureBox->setChecked( x > 0 );
}

void ToolOptionWidget::setPreserveAlpha( int x )
{
    qDebug() << "Setting - Preserve Alpha=" << x;

    QSignalBlocker b( preserveAlphaBox );
    preserveAlphaBox->setEnabled( true );
    preserveAlphaBox->setChecked( x > 0 );
}

void ToolOptionWidget::disableAllOptions()
{
    sizeSlider->hide();
    featherSlider->hide();
    usePressureBox->hide();
    makeInvisibleBox->hide();
    preserveAlphaBox->hide();
}
