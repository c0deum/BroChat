#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSettings>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QColorDialog>
#include <QColor>
#include <QDebug>

#include <QFontDatabase>

#include <QSpinBox>
#include <QDoubleSpinBox>

#include <QScrollArea>

#include <QGridLayout>

#include "settingsconsts.h"

#include "qsettingsdialog.h"

const QString CHANNEL_TEXT = QT_TR_NOOP( "Channel:" );
const QString ALIASES_TEXT = QT_TR_NOOP( "Aliases:" );
const QString SUPPORTERS_TEXT = QT_TR_NOOP( "Supporters:" );
const QString BLACKLIST_TEXT = QT_TR_NOOP( "Black List:" );
const QString REMOVE_BLACKLIST_USERS_MESSAGES = QT_TR_NOOP( "Remove Black List Users Messages" );

QSettingsDialog::QSettingsDialog( QWidget *parent )
: QDialog( parent/*, Qt::WindowStaysOnTopHint*/ )
, bodyBackgroundColor()

, evenMessagesNicknamesColor()
, oddMessagesNicknamesColor()
, aliasesNicknamesColor()
, supportersNicknamesColor()
, blackListNicknamesColor()

, evenMessagesBackgroundColor()
, oddMessagesBackgroundColor()
, aliasesBackgroundColor()
, supportersBackgroundColor()
, blackListBackgroundColor()

, evenMessagesTextColor()
, oddMessagesTextColor()
, aliasesMessagesTextColor()
, supportersMessagesTextColor()
, blackListMessagesTextColor()

, evenMessagesBorderColor()
, oddMessagesBorderColor()
, aliasesMessagesBorderColor()
, supportersMessagesBorderColor()
, blackListMessagesBorderColor()

, viewersTextColor()
, viewersBackgroundColor()
, linksColor()

, tabSettings( new QTabWidget( this ) )

, languageCombo( new QComboBox( this ) )
, stayOnTopCheckBox( new QCheckBox( this ) )
, frameLessWindowCheckBox( new QCheckBox( this ) )
//, transparentWindowCheckBox( new QCheckBox( this ) )
, styleCombo( new QComboBox( this ) )
, opacitySlider( new QSlider( this ) )
, showSystemMessagesCheckBox( new QCheckBox( this ) )
, showImagesCheckBox( new QCheckBox( this ) )
, saveMessagesToLogCheckBox( new QCheckBox( this ) )
, useServerCheckBox( new QCheckBox( this ) )
, serverPortSpinBox( new QSpinBox( this ) )
, saveToFileCheckBox( new QCheckBox( this ) )
, saveToFileIntervalSpinBox( new QSpinBox( this ) )

, fontNameCombo( new QComboBox( this ) )
, bodyBackgroundColorButton( new QPushButton( this ) )


, evenMessagesNicknamesColorButton( new QPushButton( this ) )
, oddMessagesNicknamesColorButton( new QPushButton( this ) )
, aliasesNicknamesColorButton( new QPushButton( this ) )
, supportersNicknamesColorButton( new QPushButton( this ) )
, blackListNicknamesColorButton( new QPushButton( this ) )

, evenMessagesNicknamesFontSizeSpinBox( new QSpinBox( this ) )
, oddMessagesNicknamesFontSizeSpinBox( new QSpinBox( this ) )
, aliasesNicknamesFontSizeSpinBox( new QSpinBox( this ) )
, supportersNicknamesFontSizeSpinBox( new QSpinBox( this ) )
, blackListNicknamesFontSizeSpinBox( new QSpinBox( this ) )

, evenMessagesBackgroundColorButton( new QPushButton( this ) )
, oddMessagesBackgroundColorButton( new QPushButton( this ) )
, aliasesBackgroundColorButton( new QPushButton( this ) )
, supportersBackgroundColorButton( new QPushButton( this ) )
, blackListBackgroundColorButton( new QPushButton( this ) )

, evenMessagesTextColorButton( new QPushButton( this ) )
, oddMessagesTextColorButton( new QPushButton( this ) )
, aliasesMessagesTextColorButton( new QPushButton( this ) )
, supportersMessagesTextColorButton( new QPushButton( this ) )
, blackListMessagesTextColorButton( new QPushButton( this ) )

, evenMessagesFontSizeSpinBox( new QSpinBox( this ) )
, oddMessagesFontSizeSpinBox( new QSpinBox( this ) )
, aliasesMessagesFontSizeSpinBox( new QSpinBox( this ) )
, supportersMessagesFontSizeSpinBox( new QSpinBox( this ) )
, blackListMessagesFontSizeSpinBox( new QSpinBox( this ) )

, evenMessagesBorderColorButton( new QPushButton( this ) )
, oddMessagesBorderColorButton( new QPushButton( this ) )
, aliasesMessagesBorderColorButton( new QPushButton( this ) )
, supportersMessagesBorderColorButton( new QPushButton( this ) )
, blackListMessagesBorderColorButton( new QPushButton( this ) )

, messagesLeftBorderSizeSpinBox( new QSpinBox( this ) )
, messagesRightBorderSizeSpinBox( new QSpinBox( this ) )
, messagesTopBorderSizeSpinBox( new QSpinBox( this ) )
, messagesBottomBorderSizeSpinBox( new QSpinBox( this ) )

, messagesTopLeftBorderRadiusSizeSpinBox( new QSpinBox( this ) )
, messagesTopRightBorderRadiusSizeSpinBox( new QSpinBox( this ) )
, messagesBottomLeftBorderRadiusSizeSpinBox( new QSpinBox( this ) )
, messagesBottomRightBorderRadiusSizeSpinBox( new QSpinBox( this ) )

, messagesMarginBottomSizeSpinBox( new QSpinBox( this ) )

, messagesIndentSizeSpinBox( new QSpinBox( this ) )

, viewersFontSizeSpinBox( new QSpinBox( this ) )
, viewersTextColorButton( new QPushButton( this ) )
, viewersBackgroundColorButton( new QPushButton( this ) )

, linksColorButton( new QPushButton( this ) )
, smilesSizeSpinBox( new QSpinBox( this ) )
, serviceIconsSizeSpinBox( new QSpinBox( this ) )
, maxImagesHeightSpinBox( new QSpinBox( this ) )
, animationTypeCombo( new QComboBox( this ) )
, animationDurationSpinBox( new QDoubleSpinBox( this ) )

, exportDefaultTheme( new QPushButton( tr( "Export..." ), this ) )

, acesChannelCheckBox( new QCheckBox( this ) )
, acesChannelEdit( new QLineEdit( this ) )
//, acesOriginalColorsCheckBox( new QCheckBox( this ) )
, acesAliasesEdit( new QLineEdit( this ) )
, acesSupportersListEdit( new QTextEdit( this ) )
, acesBlackListEdit( new QTextEdit( this ) )
, acesRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, azubuChannelCheckBox( new QCheckBox( this ) )
, azubuChannelEdit( new QLineEdit( this ) )
, azubuAliasesEdit( new QLineEdit( this ) )
, azubuSupportersListEdit( new QTextEdit( this ) )
, azubuBlackListEdit( new QTextEdit( this ) )
, azubuRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, beamproChannelCheckBox( new QCheckBox( this ) )
, beamproChannelEdit( new QLineEdit( this ) )
, beamproAliasesEdit( new QLineEdit( this ) )
, beamproSupportersListEdit( new QTextEdit( this ) )
, beamproBlackListEdit( new QTextEdit( this ) )
, beamproRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, cyberGameChannelCheckBox( new QCheckBox( this ) )
, cyberGameChannelEdit( new QLineEdit( this ) )
, cyberGameAliasesEdit( new QLineEdit( this ) )
, cyberGameSupportersListEdit( new QTextEdit( this ) )
, cyberGameBlackListEdit( new QTextEdit( this ) )
, cyberGameRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, funstreamChannelCheckBox( new QCheckBox( this ) )
, funstreamChannelEdit( new QLineEdit( this ) )
, funstreamOriginalColorsCheckBox( new QCheckBox( this ) )
, funstreamBadgesCheckBox( new QCheckBox( this ) )
, funstreamAliasesEdit( new QLineEdit( this ) )
, funstreamSupportersListEdit( new QTextEdit( this ) )
, funstreamBlackListEdit( new QTextEdit( this ) )
, funstreamRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, gamerstvChannelCheckBox( new QCheckBox( this ) )
, gamerstvChannelEdit( new QLineEdit( this ) )
, gamerstvBadgesCheckBox( new QCheckBox( this ) )
, gamerstvAliasesEdit( new QLineEdit( this ) )
, gamerstvSupportersListEdit( new QTextEdit( this ) )
, gamerstvBlackListEdit( new QTextEdit( this ) )
, gamerstvRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, gipsyteamChannelCheckBox( new QCheckBox( this ) )
, gipsyteamChannelEdit( new QLineEdit( this ) )
, gipsyteamAliasesEdit( new QLineEdit( this ) )
, gipsyteamSupportersListEdit( new QTextEdit( this ) )
, gipsyteamBlackListEdit( new QTextEdit( this ) )
, gipsyteamRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, goodGameChannelCheckBox( new QCheckBox( this ) )
, goodGameChannelEdit( new QLineEdit( this ) )
, goodGameBadgesCheckBox( new QCheckBox( this ) )
, goodGameUseAnimatedSmilesCheckBox( new QCheckBox( this ) )
, goodGameAliasesEdit( new QLineEdit( this ) )
, goodGameSupportersListEdit( new QTextEdit( this ) )
, goodGameBlackListEdit( new QTextEdit( this ) )
, goodGameRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, hitboxChannelCheckBox( new QCheckBox( this ) )
, hitboxChannelEdit( new QLineEdit( this ) )
, hitboxOriginalColorsCheckBox( new QCheckBox( this ) )
, hitboxAliasesEdit( new QLineEdit( this ) )
, hitboxSupportersListEdit( new QTextEdit( this ) )
, hitboxBlackListEdit( new QTextEdit( this ) )
, hitboxRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, igdcChannelCheckBox( new QCheckBox( this ) )
, igdcChannelEdit( new QLineEdit( this ) )
, igdcBadgesCheckBox( new QCheckBox( this ) )
, igdcAliasesEdit( new QLineEdit( this ) )
, igdcSupportersListEdit( new QTextEdit( this ) )
, igdcBlackListEdit( new QTextEdit( this ) )
, igdcRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, livecodingChannelCheckBox( new QCheckBox( this ) )
, livecodingChannelEdit( new QLineEdit( this ) )
, livecodingBadgesCheckBox( new QCheckBox( this ) )
, livecodingLoginEdit( new QLineEdit( this ) )
, livecodingPasswordEdit( new QLineEdit( this ) )
, livecodingAliasesEdit( new QLineEdit( this ) )
, livecodingSupportersListEdit( new QTextEdit( this ) )
, livecodingBlackListEdit( new QTextEdit( this ) )
, livecodingRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, sc2tvChannelCheckBox( new QCheckBox( this ) )
, sc2tvChannelEdit( new QLineEdit( this ) )
, sc2tvOriginalColorsCheckBox( new QCheckBox( this ) )
, sc2tvAliasesEdit( new QLineEdit( this ) )
, sc2tvSupportersListEdit( new QTextEdit( this ) )
, sc2tvBlackListEdit( new QTextEdit( this ) )
, sc2tvRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, streamcubeChannelCheckBox( new QCheckBox( this ) )
, streamcubeChannelEdit( new QLineEdit( this ) )
, streamcubeAliasesEdit( new QLineEdit( this ) )
, streamcubeSupportersListEdit( new QTextEdit( this ) )
, streamcubeBlackListEdit( new QTextEdit( this ) )
, streamcubeRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, twitchChannelCheckBox( new QCheckBox( this ) )
, twitchChannelEdit( new QLineEdit( this ) )
, twitchOriginalColorsCheckBox( new QCheckBox( this ) )
, twitchBadgesCheckBox( new QCheckBox( this ) )
, twitchAliasesEdit( new QLineEdit( this ) )
, twitchSupportersListEdit( new QTextEdit( this ) )
, twitchBlackListEdit( new QTextEdit( this ) )
, twitchRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, vidiChannelCheckBox( new QCheckBox( this ) )
, vidiChannelEdit( new QLineEdit( this ) )
, vidiAliasesEdit( new QLineEdit( this ) )
, vidiSupportersListEdit( new QTextEdit( this ) )
, vidiBlackListEdit( new QTextEdit( this ) )
, vidiRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


{
    setWindowTitle( tr( "BroChat Settings" ) );
    setModal( true );

    setupWidgets();

    QObject::connect( this, SIGNAL( finished( int ) ), this, SLOT( saveDialogSettings() ) );;

    loadDialogSettings();

    update();    
}

QSettingsDialog::~QSettingsDialog()
{
}

void QSettingsDialog::setupWidgets()
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    //QTabWidget* tabSettings = new QTabWidget( this );

    /*
    QVBoxLayout *aboutLayout = new QVBoxLayout;


    //QLabel *licenseLabel = new QLabel();

    aboutLayout->addStretch( 1 );

    QGroupBox *aboutGroup = new QGroupBox( tabSettings );
    aboutGroup->setLayout( aboutLayout );

    tabSettings->addTab( aboutGroup, QIcon( ":/resources/broico.ico" ), tr( "About" ) );
    */

    mainLayout->addWidget( tabSettings );

    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    QPushButton* buttonOk = new QPushButton( tr( "&OK" ) );
    QPushButton* buttonCancel = new QPushButton( tr( "&Cancel" ) );

    QObject::connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( buttonOk, SIGNAL( clicked() ), this, SLOT( saveSettings() ) );
    QObject::connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    buttonsLayout->addStretch( 1 );
    buttonsLayout->addWidget( buttonOk );
    buttonsLayout->addWidget( buttonCancel );

    mainLayout->addLayout( buttonsLayout );

    setLayout( mainLayout );

    setupGeneralTab();
    setupDefaultStyleTab();
    setupAcesTab();
    setupAzubuTab();
    setupBeamproTab();
    setupCybergameTab();
    setupFunstreamTab();
    setupGamerstvTab();
    setupGipsyteamTab();
    setupGoodgameTab();
    setupHitboxTab();
    setupIgdcTab();
    setupLivecodingTab();    
    setupSc2tvTab();    
    setupStreamcubeTab();
    setupTwitchTab();
    setupVidiTab();

}

void QSettingsDialog::setupGeneralTab()
{
    QSettings settings;

    QVBoxLayout * windowLayout = new QVBoxLayout;

    QLabel *languageLabel = new QLabel( tr( "Language:" ) );

    QStringList languges;
    //languges << "English" << "Русский";
    languges << "English" << "English";
    languageCombo->addItems( languges );

    windowLayout->addWidget( languageLabel );
    windowLayout->addWidget( languageCombo );

    stayOnTopCheckBox->setText( tr( "Stay On Top" ) );
    stayOnTopCheckBox->setChecked( settings.value( STAY_ON_TOP_SETTING_PATH, DEFAULT_STAY_ON_TOP ).toBool() );

    frameLessWindowCheckBox->setText( tr( "Frameless Window" ) );
    frameLessWindowCheckBox->setChecked( settings.value( FRAMELESS_WINDOW_SETTING_PATH, DEFAULT_FRAMELESS_WINDOW ).toBool() );

//    transparentWindowCheckBox->setText( tr( "Transparent window" ) );
//    transparentWindowCheckBox->setChecked( settings.value( TRANSPARENT_WINDOW_SETTING_PATH, DEFAULT_TRANSPARENT_WINDOW ).toBool() );

    windowLayout->addWidget( stayOnTopCheckBox );
    windowLayout->addWidget( frameLessWindowCheckBox );
//    windowLayout->addWidget( transparentWindowCheckBox );

    QLabel *styleLabel = new QLabel( tr( "Style:" ) );

    QDir stylesDir = QApplication::applicationDirPath() + "/styles";

    QStringList styleFiles = stylesDir.entryList( QStringList( "*.html" ), QDir::Files | QDir::NoSymLinks );

    styleCombo->addItem( DEFAULT_STYLE_NAME );
    styleCombo->addItems( styleFiles );
    styleCombo->setEditable( false );
    styleCombo->setCurrentIndex( styleCombo->findText( settings.value( STYLE_SETTING_PATH, DEFAULT_STYLE_NAME ).toString() ) );

    windowLayout->addWidget( styleLabel );
    windowLayout->addWidget( styleCombo );

    QLabel *opacityLabel = new QLabel( tr( "Opacity:" )  );

    opacitySlider->setMinimum( 10 );
    opacitySlider->setMaximum( 100 );
    opacitySlider->setOrientation( Qt::Horizontal );
    opacitySlider->setTickPosition(  QSlider::TicksBothSides );
    opacitySlider->setValue( settings.value( OPACITY_SETTING_PATH, DEFAULT_OPACITY_VALUE ).toInt() );

    windowLayout->addWidget( opacityLabel );
    windowLayout->addWidget( opacitySlider );

    showSystemMessagesCheckBox->setText( tr( "Show System Messages" ) );
    showSystemMessagesCheckBox->setChecked( settings.value( SHOW_SYSTEM_MESSAGES_SETTING_PATH, DEFAULT_SHOW_SYSTEM_MESSAGES ).toBool() );

    windowLayout->addWidget( showSystemMessagesCheckBox );

    showImagesCheckBox->setText( tr( "Show Images" ) );
    showImagesCheckBox->setChecked( settings.value( SHOW_IMAGES_SETTING_PATH, DEFAULT_SHOW_IMAGES ).toBool() );

    windowLayout->addWidget( showImagesCheckBox );

    saveMessagesToLogCheckBox->setText( tr( "Save Messages To Log" ) );
    saveMessagesToLogCheckBox->setChecked( settings.value( SAVE_MESSAGES_TO_LOG_FILE_SETTING_PATH, DEFAULT_SAVE_MESSAGES_TO_LOG_FILE ).toBool() );

    windowLayout->addWidget( saveMessagesToLogCheckBox );

    useServerCheckBox->setText( tr( "Use Server" ) );
    useServerCheckBox->setChecked( settings.value( USE_SERVER_SETTING_PAH, DEFAULT_USE_SERVER ).toBool() );

    windowLayout->addWidget( useServerCheckBox );

    QLabel *serverPortLabel = new QLabel( tr( "Port:" ) );

    serverPortSpinBox->setRange( 1, 65535 );
    serverPortSpinBox->setValue( settings.value( SERVER_PORT_SETTING_PATH, DEFAULT_SERVER_PORT ).toInt() );

    windowLayout->addWidget( serverPortLabel );
    windowLayout->addWidget( serverPortSpinBox );

    saveToFileCheckBox->setText( tr( "Save To File" ) );
    saveToFileCheckBox->setChecked( settings.value( SAVE_TO_FILE_SETTING_PATTH, DEFAULT_SAVE_TO_FILE ).toBool() );

    windowLayout->addWidget( saveToFileCheckBox );

    QLabel *saveToFileIntervalLabel = new QLabel( tr( "Save To File Interval(ms):" ) );

    saveToFileIntervalSpinBox->setRange( 100, 5000 );
    saveToFileIntervalSpinBox->setValue( settings.value( SAVE_TO_FILE_INTERVAL_SETTING_PATH, DEFAULT_SAVE_TO_FILE_INTERVAL ).toInt() );

    windowLayout->addWidget( saveToFileIntervalLabel );
    windowLayout->addWidget( saveToFileIntervalSpinBox );

    windowLayout->addStretch( 1 );

    QGroupBox *windowGroup = new QGroupBox( tabSettings );
    windowGroup->setLayout( windowLayout );

    tabSettings->addTab( windowGroup, QIcon( ":/resources/broico.ico" ), tr( "Window" ) );
}

void QSettingsDialog::setupDefaultStyleTab()
{
    QSettings settings;

    QFontDatabase fontDataBase;

    QVBoxLayout * defaultStyleLayout = new QVBoxLayout();


    fontNameCombo->addItems( fontDataBase.families() );
    fontNameCombo->setEditable( false );
    int index = fontNameCombo->findText( settings.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() );
    fontNameCombo->setCurrentIndex( index );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Font Name:" ), this ), fontNameCombo } );

    setupColorButton( bodyBackgroundColorButton, bodyBackgroundColor, DEFAULT_GENERATED_STYLE_BODY_BACKGROUND_COLOR, GENERATED_STYLE_BODY_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( bodyBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( bodyBackgroundColorSelection() ) );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Background Color:" ), this ), bodyBackgroundColorButton } );


    QGridLayout * nicknamesTextColorsValueLayout = new QGridLayout();

    nicknamesTextColorsValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    nicknamesTextColorsValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    nicknamesTextColorsValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    nicknamesTextColorsValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    nicknamesTextColorsValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );   


    nicknamesTextColorsValueLayout->addWidget( evenMessagesNicknamesColorButton, 1, 0 );
    nicknamesTextColorsValueLayout->addWidget( oddMessagesNicknamesColorButton, 1, 1 );
    nicknamesTextColorsValueLayout->addWidget( aliasesNicknamesColorButton, 1, 2 );
    nicknamesTextColorsValueLayout->addWidget( supportersNicknamesColorButton, 1, 3 );
    nicknamesTextColorsValueLayout->addWidget( blackListNicknamesColorButton, 1, 4 );

    setupColorButton( evenMessagesNicknamesColorButton, evenMessagesNicknamesColor, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
    QObject::connect( evenMessagesNicknamesColorButton, SIGNAL( clicked() ), this, SLOT( evenMessagesNicknamesColorSelection() ) );

    setupColorButton( oddMessagesNicknamesColorButton, oddMessagesNicknamesColor, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
    QObject::connect( oddMessagesNicknamesColorButton, SIGNAL( clicked() ), this, SLOT( oddMessagesNicknamesColorSelection() ) );

    setupColorButton( aliasesNicknamesColorButton, aliasesNicknamesColor, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
    QObject::connect( aliasesNicknamesColorButton, SIGNAL( clicked() ), this, SLOT( aliasesMessagesNicknamesColorSelection() ) );

    setupColorButton( supportersNicknamesColorButton, supportersNicknamesColor, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
    QObject::connect( supportersNicknamesColorButton, SIGNAL( clicked() ), this, SLOT( supportersMessagesNicknamesColorSelection() ) );

    setupColorButton( blackListNicknamesColorButton, blackListNicknamesColor, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_COLOR_SETTING_PATH );
    QObject::connect( blackListNicknamesColorButton, SIGNAL( clicked() ), this, SLOT( blackListMessagesNicknamesColorSelection() ) );


    QHBoxLayout * nicknamesColorlayout = new QHBoxLayout();
    nicknamesColorlayout->addWidget( new QLabel( tr( "Nicknames Colors:" ), this ) );
    nicknamesColorlayout->addLayout( nicknamesTextColorsValueLayout );

    defaultStyleLayout->addLayout( nicknamesColorlayout );

    QGridLayout * nicknamesFontSizeValueLayout = new QGridLayout();

    nicknamesFontSizeValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    nicknamesFontSizeValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    nicknamesFontSizeValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    nicknamesFontSizeValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    nicknamesFontSizeValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );

    nicknamesFontSizeValueLayout->addWidget( evenMessagesNicknamesFontSizeSpinBox, 1, 0 );
    nicknamesFontSizeValueLayout->addWidget( oddMessagesNicknamesFontSizeSpinBox, 1, 1 );
    nicknamesFontSizeValueLayout->addWidget( aliasesNicknamesFontSizeSpinBox, 1, 2 );
    nicknamesFontSizeValueLayout->addWidget( supportersNicknamesFontSizeSpinBox, 1, 3 );
    nicknamesFontSizeValueLayout->addWidget( blackListNicknamesFontSizeSpinBox, 1, 4 );   

    setupSpinBox( evenMessagesNicknamesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE, GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( oddMessagesNicknamesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE, GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( aliasesNicknamesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE, GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( supportersNicknamesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE, GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( blackListNicknamesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE, GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH );

    QHBoxLayout * nicknamesFontSizeLayout = new QHBoxLayout();
    nicknamesFontSizeLayout->addWidget( new QLabel( tr( "Messages Font Size:" ), this ) );
    nicknamesFontSizeLayout->addLayout( nicknamesFontSizeValueLayout );

    defaultStyleLayout->addLayout( nicknamesFontSizeLayout );


    QGridLayout * messagesBackgroundColorsValueLayout = new QGridLayout();

    messagesBackgroundColorsValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    messagesBackgroundColorsValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    messagesBackgroundColorsValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    messagesBackgroundColorsValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    messagesBackgroundColorsValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );


    messagesBackgroundColorsValueLayout->addWidget( evenMessagesBackgroundColorButton, 1, 0 );
    messagesBackgroundColorsValueLayout->addWidget( oddMessagesBackgroundColorButton, 1, 1 );
    messagesBackgroundColorsValueLayout->addWidget( aliasesBackgroundColorButton, 1, 2 );
    messagesBackgroundColorsValueLayout->addWidget( supportersBackgroundColorButton, 1, 3 );
    messagesBackgroundColorsValueLayout->addWidget( blackListBackgroundColorButton, 1, 4 );

    setupColorButton( evenMessagesBackgroundColorButton, evenMessagesBackgroundColor, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_EVEN_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( evenMessagesBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( evenMessagesBackgroundColorSelection() ) );

    setupColorButton( oddMessagesBackgroundColorButton, oddMessagesBackgroundColor, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_ODD_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( oddMessagesBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( oddMessagesBackgroundColorSelection() ) );

    setupColorButton( aliasesBackgroundColorButton, aliasesBackgroundColor, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( aliasesBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( aliasesBackgroundColorSelection() ) );

    setupColorButton( supportersBackgroundColorButton, supportersBackgroundColor, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( supportersBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( supportersBackgroundColorSelection() ) );

    setupColorButton( blackListBackgroundColorButton, blackListBackgroundColor, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( blackListBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( blackListBackgroundColorSelection() ) );


    QHBoxLayout * messagesBackgroundColorlayout = new QHBoxLayout();
    messagesBackgroundColorlayout->addWidget( new QLabel( tr( "Messages Background Colors:" ), this ) );
    messagesBackgroundColorlayout->addLayout( messagesBackgroundColorsValueLayout );

    //messagesBackgroundColorlayout->addLayout( messagesBackgroundColorlayout );

    defaultStyleLayout->addLayout( messagesBackgroundColorlayout );


    QGridLayout * messagesTextColorsValueLayout = new QGridLayout();

    messagesTextColorsValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    messagesTextColorsValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    messagesTextColorsValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    messagesTextColorsValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    messagesTextColorsValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );


    messagesTextColorsValueLayout->addWidget( evenMessagesTextColorButton, 1, 0 );
    messagesTextColorsValueLayout->addWidget( oddMessagesTextColorButton, 1, 1 );
    messagesTextColorsValueLayout->addWidget( aliasesMessagesTextColorButton, 1, 2 );
    messagesTextColorsValueLayout->addWidget( supportersMessagesTextColorButton, 1, 3 );
    messagesTextColorsValueLayout->addWidget( blackListMessagesTextColorButton, 1, 4 );

    setupColorButton( evenMessagesTextColorButton, evenMessagesTextColor, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_EVEN_MESSAGES_TEXT_COLOR_SETTING_PATH );
    QObject::connect( evenMessagesTextColorButton, SIGNAL( clicked() ), this, SLOT( evenMessagesTextColorSelection() ) );

    setupColorButton( oddMessagesTextColorButton, oddMessagesTextColor, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_ODD_MESSAGES_TEXT_COLOR_SETTING_PATH );
    QObject::connect( oddMessagesTextColorButton, SIGNAL( clicked() ), this, SLOT( oddMessagesTextColorSelection() ) );

    setupColorButton( aliasesMessagesTextColorButton, aliasesMessagesTextColor, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_TEXT_COLOR_SETTING_PATH );
    QObject::connect( aliasesMessagesTextColorButton, SIGNAL( clicked() ), this, SLOT( aliasesMessagesTextColorSelection() ) );

    setupColorButton( supportersMessagesTextColorButton, supportersMessagesTextColor, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_TEXT_COLOR_SETTING_PATH );
    QObject::connect( supportersMessagesTextColorButton, SIGNAL( clicked() ), this, SLOT( supportersMessagesTextColorSelection() ) );

    setupColorButton( blackListMessagesTextColorButton, blackListMessagesTextColor, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_TEXT_COLOR_SETTING_PATH );
    QObject::connect( blackListMessagesTextColorButton, SIGNAL( clicked() ), this, SLOT( blackListMessagesTextColorSelection() ) );

    QHBoxLayout * messagesTextColorlayout = new QHBoxLayout();
    messagesTextColorlayout->addWidget( new QLabel( tr( "Messages Text Colors:" ), this ) );
    messagesTextColorlayout->addLayout( messagesTextColorsValueLayout );

    defaultStyleLayout->addLayout( messagesTextColorlayout );


    QGridLayout * messagesFontSizeValueLayout = new QGridLayout();

    messagesFontSizeValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    messagesFontSizeValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    messagesFontSizeValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    messagesFontSizeValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    messagesFontSizeValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );

    messagesFontSizeValueLayout->addWidget( evenMessagesFontSizeSpinBox, 1, 0 );
    messagesFontSizeValueLayout->addWidget( oddMessagesFontSizeSpinBox, 1, 1 );
    messagesFontSizeValueLayout->addWidget( aliasesMessagesFontSizeSpinBox, 1, 2 );
    messagesFontSizeValueLayout->addWidget( supportersMessagesFontSizeSpinBox, 1, 3 );
    messagesFontSizeValueLayout->addWidget( blackListMessagesFontSizeSpinBox, 1, 4 );

    setupSpinBox( evenMessagesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE, GENERATED_STYLE_EVEN_MESSAGES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( oddMessagesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE, GENERATED_STYLE_ODD_MESSAGES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( aliasesMessagesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE, GENERATED_STYLE_ALIASES_MESSAGES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( supportersMessagesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE, GENERATED_STYLE_SUPPORTERS_MESSAGES_FONT_SIZE_SETTING_PATH );
    setupSpinBox( blackListMessagesFontSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE, GENERATED_STYLE_BLACK_LIST_MESSAGES_FONT_SIZE_SETTING_PATH );

    QHBoxLayout * messagesFontSizeLayout = new QHBoxLayout();
    messagesFontSizeLayout->addWidget( new QLabel( tr( "Messages Font Size:" ), this ) );
    messagesFontSizeLayout->addLayout( messagesFontSizeValueLayout );

    defaultStyleLayout->addLayout( messagesFontSizeLayout );

    QGridLayout * messagesBorderColorsValueLayout = new QGridLayout();

    messagesBorderColorsValueLayout->addWidget( new QLabel( tr( "Even" ), this ), 0, 0 );
    messagesBorderColorsValueLayout->addWidget( new QLabel( tr( "Odd" ), this ), 0, 1 );
    messagesBorderColorsValueLayout->addWidget( new QLabel( tr( "Aliases" ), this ), 0, 2 );
    messagesBorderColorsValueLayout->addWidget( new QLabel( tr( "Supporters" ), this ), 0, 3 );
    messagesBorderColorsValueLayout->addWidget( new QLabel( tr( "Black List" ), this ), 0, 4 );

    messagesBorderColorsValueLayout->addWidget( evenMessagesBorderColorButton, 1, 0 );
    messagesBorderColorsValueLayout->addWidget( oddMessagesBorderColorButton, 1, 1 );
    messagesBorderColorsValueLayout->addWidget( aliasesMessagesBorderColorButton, 1, 2 );
    messagesBorderColorsValueLayout->addWidget( supportersMessagesBorderColorButton, 1, 3 );
    messagesBorderColorsValueLayout->addWidget( blackListMessagesBorderColorButton, 1, 4 );

    setupColorButton( evenMessagesBorderColorButton, evenMessagesBorderColor, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_EVEN_MESSAGES_BORDER_COLOR_SETTING_PATH );
    QObject::connect( evenMessagesBorderColorButton, SIGNAL( clicked() ), this, SLOT( evenMessagesBorderColorSelection() ) );

    setupColorButton( oddMessagesBorderColorButton, oddMessagesBorderColor, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_ODD_MESSAGES_BORDER_COLOR_SETTING_PATH );
    QObject::connect( oddMessagesBorderColorButton, SIGNAL( clicked() ), this, SLOT( oddMessagesBorderColorSelection() ) );

    setupColorButton( aliasesMessagesBorderColorButton, aliasesMessagesBorderColor, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_ALIASES_MESSAGES_BORDER_COLOR_SETTING_PATH );
    QObject::connect( aliasesMessagesBorderColorButton, SIGNAL( clicked() ), this, SLOT( aliasesMessagesBorderColorSelection() ) );

    setupColorButton( supportersMessagesBorderColorButton, supportersMessagesBorderColor, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_SUPPORTERS_MESSAGES_BORDER_COLOR_SETTING_PATH );
    QObject::connect( supportersMessagesBorderColorButton, SIGNAL( clicked() ), this, SLOT( supportersMessagesBorderColorSelection() ) );

    setupColorButton( blackListMessagesBorderColorButton, blackListMessagesBorderColor, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR, GENERATED_STYLE_BLACK_LIST_MESSAGES_BORDER_COLOR_SETTING_PATH );
    QObject::connect( blackListMessagesBorderColorButton, SIGNAL( clicked() ), this, SLOT( blackListMessagesBorderColorSelection() ) );

    QHBoxLayout * messagesBorderColorlayout = new QHBoxLayout();
    messagesBorderColorlayout->addWidget( new QLabel( tr( "Messages Border Colors:" ), this ) );
    messagesBorderColorlayout->addLayout( messagesBorderColorsValueLayout );

    //messagesBorderColorlayout->addLayout( messagesBorderColorlayout );

    defaultStyleLayout->addLayout( messagesBorderColorlayout );

    QGridLayout * messagesBorderSizeValueLayout = new QGridLayout();

    messagesBorderSizeValueLayout->addWidget( messagesTopBorderSizeSpinBox, 0, 1 );
    messagesBorderSizeValueLayout->addWidget( messagesLeftBorderSizeSpinBox, 1, 0 );
    messagesBorderSizeValueLayout->addWidget( messagesRightBorderSizeSpinBox, 1, 2 );
    messagesBorderSizeValueLayout->addWidget( messagesBottomBorderSizeSpinBox, 2, 1 );

    setupSpinBox( messagesTopBorderSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE, GENERATED_STYLE_MESSAGES_TOP_BORDER_SIZE_SETTING_PATH );
    setupSpinBox( messagesLeftBorderSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE, GENERATED_STYLE_MESSAGES_LEFT_BORDER_SIZE_SETTING_PATH );
    setupSpinBox( messagesRightBorderSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE, GENERATED_STYLE_MESSAGES_RIGHT_BORDER_SIZE_SETTING_PATH );
    setupSpinBox( messagesBottomBorderSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE, GENERATED_STYLE_MESSAGES_BOTTOM_BORDER_SIZE_SETTING_PATH );


    QHBoxLayout * messagesBorderSizeLayout = new QHBoxLayout();
    messagesBorderSizeLayout->addWidget( new QLabel( tr( "Messages Border Size:" ), this ) );
    messagesBorderSizeLayout->addLayout( messagesBorderSizeValueLayout );

    //messagesBorderSizeLayout->addLayout( messagesBorderSizeLayout );

    defaultStyleLayout->addLayout( messagesBorderSizeLayout );


    QGridLayout * messagesBorderRadiusSizeValueLayout = new QGridLayout();

    messagesBorderRadiusSizeValueLayout->addWidget( messagesTopLeftBorderRadiusSizeSpinBox, 0, 0 );
    messagesBorderRadiusSizeValueLayout->addWidget( messagesTopRightBorderRadiusSizeSpinBox, 0, 2 );
    messagesBorderRadiusSizeValueLayout->addWidget( messagesBottomLeftBorderRadiusSizeSpinBox, 2, 0 );
    messagesBorderRadiusSizeValueLayout->addWidget( messagesBottomRightBorderRadiusSizeSpinBox, 2, 2 );

    setupSpinBox( messagesTopLeftBorderRadiusSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE, GENERATED_STYLE_MESSAGES_TOP_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH );
    setupSpinBox( messagesTopRightBorderRadiusSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE, GENERATED_STYLE_MESSAGES_TOP_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH );
    setupSpinBox( messagesBottomLeftBorderRadiusSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE, GENERATED_STYLE_MESSAGES_BOTTOM_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH );
    setupSpinBox( messagesBottomRightBorderRadiusSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE, GENERATED_STYLE_MESSAGES_BOTTOM_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH );

    QHBoxLayout * messagesBorderRadiusSizeLayout = new QHBoxLayout();
    messagesBorderRadiusSizeLayout->addWidget( new QLabel( tr( "Messages Border Radius Size:" ), this ) );
    messagesBorderRadiusSizeLayout->addLayout( messagesBorderRadiusSizeValueLayout );

    //messagesBorderRadiusSizeLayout->addLayout( messagesBorderRadiusSizeLayout );

    defaultStyleLayout->addLayout( messagesBorderRadiusSizeLayout );


    setupSpinBox( messagesMarginBottomSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE, GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE_SETTING_PATH );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Message Spacing:" ), this ), messagesMarginBottomSizeSpinBox } );


    setupSpinBox( messagesIndentSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_MESSAGES_INDENT_SIZE, GENERATED_STYLE_MESSAGES_INDENT_SIZE_SETTING_PATH );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Message Indent Size:" ), this ), messagesIndentSizeSpinBox } );



    setupSpinBox( viewersFontSizeSpinBox, 0, 100, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE, GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Viewers Font Size:" ), this ), viewersFontSizeSpinBox } );

    setupColorButton( viewersTextColorButton, viewersTextColor, DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR, GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH );
    QObject::connect( viewersTextColorButton, SIGNAL( clicked() ), this, SLOT( viewersColorSelection() ) );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Viewers Text Color:" ), this ), viewersTextColorButton } );


    setupColorButton( viewersBackgroundColorButton, viewersBackgroundColor, DEFAULT_GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR, GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR_SETTING_PATH );
    QObject::connect( viewersBackgroundColorButton, SIGNAL( clicked() ), this, SLOT( viewersBackgroundColorSelection() ) );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Viewers Background Color:" ), this ), viewersBackgroundColorButton } );


    setupColorButton( linksColorButton, linksColor, DEFAULT_GENERATED_STYLE_LINKS_COLOR, GENERATED_STYLE_LINKS_COLOR_SETTING_PATH );
    QObject::connect( linksColorButton, SIGNAL( clicked() ), this, SLOT( linksColorSelection() ) );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Links Color:" ), this ), linksColorButton } );

    setupSpinBox( smilesSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_SMILES_SIZE, GENERATED_STYLE_SMILES_SIZE_SETTING_PATH );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Smiles Size:" ), this ), smilesSizeSpinBox } );

    setupSpinBox( serviceIconsSizeSpinBox, 1, 100, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE, GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Service Icons Size:" ), this ), serviceIconsSizeSpinBox } );

    setupSpinBox( maxImagesHeightSpinBox, 0, 2160, DEFAULT_GENERATED_STYLE_MAX_IMAGES_HEIGHT, GENERATED_STYLE_MAX_IMAGES_HEIGHT_SETTING_PATH );


    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Max Images Height( px ):" ), this ), maxImagesHeightSpinBox } );

    QStringList animationsList;

    animationsList << "None";
    animationsList << "Bounce";
    animationsList << "BounceDown";
    animationsList << "BounceUp";
    animationsList << "BounceLeft";
    animationsList << "BounceRight";
    animationsList << "Opacity";
    animationsList << "Rotate";
    animationsList << "Scale";
    animationsList << "Shake";
    animationsList << "SlideFromLeft";
    animationsList << "SlideFromRight";
    animationsList << "SlideAndScaleFromLeft";
    animationsList << "SlideAndScaleFromRight";
    animationsList << "SlideAndSkewToLeft";
    animationsList << "SlideAndSkewToRight";

    animationTypeCombo->addItems( animationsList );
    animationTypeCombo->setEditable( false );
    animationTypeCombo->setCurrentIndex( animationTypeCombo->findText( settings.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE ).toString() ) );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Animation type:" ), this ), animationTypeCombo } );

    animationDurationSpinBox->setRange( 0.0, 10.0 );
    animationDurationSpinBox->setSingleStep( 0.1 );
    animationDurationSpinBox->setValue( settings.value( GENERATED_STYLE_ANIMATION_DURATION_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_DURATION ).toDouble() );

    addLayoutRow( defaultStyleLayout, { new QLabel( tr( "Animation Duration( sec ):" ), this ), animationDurationSpinBox } );


    defaultStyleLayout->addStretch( 1 );


    QObject::connect( exportDefaultTheme, SIGNAL( clicked() ), this, SLOT( onExportDefaultThemeClick() ) );
    defaultStyleLayout->addWidget( exportDefaultTheme );

    QGroupBox * defaultStyleGroup = new QGroupBox;

    defaultStyleGroup->setLayout( defaultStyleLayout );



    QScrollArea * defaultStyleScrollArea = new QScrollArea( this );

    defaultStyleScrollArea->setWidgetResizable( true );
    defaultStyleScrollArea->setWidget( defaultStyleGroup );


    tabSettings->addTab( defaultStyleScrollArea, QIcon( ":/resources/broico.ico" ), tr( "Default Style" ) );

}

void QSettingsDialog::setupAcesTab()
{
    QSettings settings;

    QVBoxLayout * acesLayout = new QVBoxLayout;

    acesChannelCheckBox->setText( CHANNEL_TEXT );
    acesChannelCheckBox->setChecked( settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    acesChannelEdit->setText( settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString() );
    acesChannelEdit->setEnabled( acesChannelCheckBox->isChecked() );

    QObject::connect( acesChannelCheckBox, SIGNAL( clicked( bool ) ), acesChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( acesLayout, { acesChannelCheckBox, acesChannelEdit } );

    //acesOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    //acesOriginalColorsCheckBox->setChecked( settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    //acesLayout->addWidget( acesOriginalColorsCheckBox );

    acesAliasesEdit->setText( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( ALIASES_TEXT, this ), acesAliasesEdit } );

    acesSupportersListEdit->setText( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( SUPPORTERS_TEXT, this ), acesSupportersListEdit } );

    acesBlackListEdit->setText( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( BLACKLIST_TEXT, this ), acesBlackListEdit } );

    acesRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    acesRemoveBlackListUsersCheckBox->setChecked( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    acesLayout->addWidget( acesRemoveBlackListUsersCheckBox );

    acesLayout->addStretch( 1 );

    QGroupBox * acesGroup = new QGroupBox( tabSettings );
    acesGroup->setLayout( acesLayout );

    tabSettings->addTab( acesGroup, QIcon( ":/resources/aceslogo.png" ), tr( "Aces" ) );
}


void QSettingsDialog::setupAzubuTab()
{
    QSettings settings;

    QVBoxLayout * azubuLayout = new QVBoxLayout;

    azubuChannelCheckBox->setText( CHANNEL_TEXT );
    azubuChannelCheckBox->setChecked( settings.value( AZUBU_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    azubuChannelEdit->setText( settings.value( AZUBU_CHANNEL_SETTING_PATH, DEFAULT_AZUBU_CHANNEL_NAME ).toString() );
    azubuChannelEdit->setEnabled( azubuChannelCheckBox->isChecked() );

    QObject::connect( azubuChannelCheckBox, SIGNAL( clicked( bool ) ), azubuChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( azubuLayout, { azubuChannelCheckBox, azubuChannelEdit } );

    azubuAliasesEdit->setText( settings.value( AZUBU_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( azubuLayout, { new QLabel( ALIASES_TEXT, this ), azubuAliasesEdit } );

    azubuSupportersListEdit->setText( settings.value( AZUBU_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( azubuLayout, { new QLabel( SUPPORTERS_TEXT, this ), azubuSupportersListEdit } );

    azubuBlackListEdit->setText( settings.value( AZUBU_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( azubuLayout, { new QLabel( BLACKLIST_TEXT, this ), azubuBlackListEdit } );

    azubuRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    azubuRemoveBlackListUsersCheckBox->setChecked( settings.value( AZUBU_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    azubuLayout->addWidget( azubuRemoveBlackListUsersCheckBox );

    azubuLayout->addStretch( 1 );

    QGroupBox * azubuGroup = new QGroupBox( tabSettings );
    azubuGroup->setLayout( azubuLayout );

    tabSettings->addTab( azubuGroup, QIcon( ":/resources/azubulogo.png" ), tr( "Azubu" ) );
}




void QSettingsDialog::setupBeamproTab()
{
    QSettings settings;

    QVBoxLayout * beamproLayout = new QVBoxLayout;

    beamproChannelCheckBox->setText( CHANNEL_TEXT );
    beamproChannelCheckBox->setChecked( settings.value( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    beamproChannelEdit->setText( settings.value( BEAMPRO_CHANNEL_SETTING_PATH, DEFAULT_BEAMPRO_CHANNEL_NAME ).toString() );
    beamproChannelEdit->setEnabled( beamproChannelCheckBox->isChecked() );

    QObject::connect( beamproChannelCheckBox, SIGNAL( clicked( bool ) ), beamproChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( beamproLayout, { beamproChannelCheckBox, beamproChannelEdit } );

    beamproAliasesEdit->setText( settings.value( BEAMPRO_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( ALIASES_TEXT, this ), beamproAliasesEdit } );

    beamproSupportersListEdit->setText( settings.value( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( SUPPORTERS_TEXT, this ), beamproSupportersListEdit } );

    beamproBlackListEdit->setText( settings.value( BEAMPRO_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( BLACKLIST_TEXT, this ), beamproBlackListEdit } );

    beamproRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    beamproRemoveBlackListUsersCheckBox->setChecked( settings.value( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    beamproLayout->addWidget( beamproRemoveBlackListUsersCheckBox );

    beamproLayout->addStretch( 1 );

    QGroupBox * beamproGroup = new QGroupBox( tabSettings );
    beamproGroup->setLayout( beamproLayout );

    tabSettings->addTab( beamproGroup, QIcon( ":/resources/beamprologo.png" ), tr( "Beampro" ) );
}



void QSettingsDialog::setupCybergameTab()
{
    QSettings settings;

    QVBoxLayout * cyberGameLayout = new QVBoxLayout;

    cyberGameChannelCheckBox->setText( CHANNEL_TEXT );
    cyberGameChannelCheckBox->setChecked( settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    cyberGameChannelEdit->setText( settings.value( CYBERGAME_CHANNEL_SETTING_PATH, DEFAULT_CYBERGAME_CHANNEL_NAME ).toString() );
    cyberGameChannelEdit->setEnabled( cyberGameChannelCheckBox->isChecked() );

    QObject::connect( cyberGameChannelCheckBox, SIGNAL( clicked(bool) ), cyberGameChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( cyberGameLayout, { cyberGameChannelCheckBox, cyberGameChannelEdit } );

    cyberGameAliasesEdit->setText( settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( cyberGameLayout, { new QLabel( ALIASES_TEXT, this ), cyberGameAliasesEdit } );

    cyberGameSupportersListEdit->setText( settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( cyberGameLayout, { new QLabel( SUPPORTERS_TEXT, this ), cyberGameSupportersListEdit } );

    cyberGameBlackListEdit->setText( settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( cyberGameLayout, { new QLabel( BLACKLIST_TEXT, this ), cyberGameBlackListEdit } );

    cyberGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    cyberGameRemoveBlackListUsersCheckBox->setChecked( settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    cyberGameLayout->addWidget( cyberGameRemoveBlackListUsersCheckBox );

    cyberGameLayout->addStretch( 1 );

    QGroupBox * cyberGameGroup = new QGroupBox( tabSettings );
    cyberGameGroup->setLayout( cyberGameLayout );

    tabSettings->addTab( cyberGameGroup, QIcon( ":/resources/cybergamelogo.png" ), tr( "Cybergame" ) );
}

void QSettingsDialog::setupFunstreamTab()
{
    QSettings settings;

    QVBoxLayout * funstreamLayout = new QVBoxLayout;

    funstreamChannelCheckBox->setText( CHANNEL_TEXT );
    funstreamChannelCheckBox->setChecked( settings.value( FUNSTREAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    funstreamChannelEdit->setText( settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, DEFAULT_FUNSTREAM_CHANNEL_NAME ).toString() );
    funstreamChannelEdit->setEnabled( funstreamChannelCheckBox->isChecked() );   

    QObject::connect( funstreamChannelCheckBox, SIGNAL( clicked( bool ) ), funstreamChannelEdit, SLOT( setEnabled( bool ) ) );

    funstreamOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    funstreamOriginalColorsCheckBox->setChecked( settings.value( FUNSTREAM_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    funstreamBadgesCheckBox->setText( tr( "Badges" ) );
    funstreamBadgesCheckBox->setChecked( settings.value( FUNSTREAM_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( funstreamLayout, { funstreamChannelCheckBox, funstreamChannelEdit, funstreamOriginalColorsCheckBox, funstreamBadgesCheckBox } );

    funstreamAliasesEdit->setText( settings.value( FUNSTREAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( funstreamLayout, { new QLabel( ALIASES_TEXT, this ), funstreamAliasesEdit } );

    funstreamSupportersListEdit->setText( settings.value( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( funstreamLayout, { new QLabel( SUPPORTERS_TEXT, this ), funstreamSupportersListEdit } );

    funstreamBlackListEdit->setText( settings.value( FUNSTREAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( funstreamLayout, { new QLabel( BLACKLIST_TEXT, this ), funstreamBlackListEdit } );

    funstreamRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    funstreamRemoveBlackListUsersCheckBox->setChecked( settings.value( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    funstreamLayout->addWidget( funstreamRemoveBlackListUsersCheckBox );

    funstreamLayout->addStretch( 1 );

    QGroupBox * funstreamGroup = new QGroupBox( tabSettings );
    funstreamGroup->setLayout( funstreamLayout );

    tabSettings->addTab( funstreamGroup, QIcon( ":/resources/funstreamlogo.png" ), tr( "Funstream" ) );
}

void QSettingsDialog::setupGamerstvTab()
{
    QSettings settings;

    QVBoxLayout * gamerstvLayout = new QVBoxLayout();

    gamerstvChannelCheckBox->setText( CHANNEL_TEXT );
    gamerstvChannelCheckBox->setChecked( settings.value( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    gamerstvChannelEdit->setText( settings.value( GAMERSTV_CHANNEL_SETTING_PATH, DEFAULT_GAMERSTV_CHANNEL_NAME ).toString() );
    gamerstvChannelEdit->setEnabled( gamerstvChannelCheckBox->isChecked() );       

    QObject::connect( gamerstvChannelCheckBox, SIGNAL( clicked( bool ) ), gamerstvChannelEdit, SLOT( setEnabled( bool ) ) );

    gamerstvBadgesCheckBox->setText( tr( "Badges" ) );
    gamerstvBadgesCheckBox->setChecked( settings.value( GAMERSTV_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( gamerstvLayout, { gamerstvChannelCheckBox, gamerstvChannelEdit, gamerstvBadgesCheckBox } );

    gamerstvAliasesEdit->setText( settings.value( GAMERSTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gamerstvLayout, { new QLabel( ALIASES_TEXT, this ), gamerstvAliasesEdit } );

    gamerstvSupportersListEdit->setText( settings.value( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gamerstvLayout, { new QLabel( SUPPORTERS_TEXT, this ), gamerstvSupportersListEdit } );

    gamerstvBlackListEdit->setText( settings.value( GAMERSTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gamerstvLayout, { new QLabel( BLACKLIST_TEXT, this ), gamerstvBlackListEdit } );

    gamerstvRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    gamerstvRemoveBlackListUsersCheckBox->setChecked( settings.value( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    gamerstvLayout->addWidget( gamerstvRemoveBlackListUsersCheckBox );

    gamerstvLayout->addStretch( 1 );

    QGroupBox * gamerstvGroup = new QGroupBox( tabSettings );
    gamerstvGroup->setLayout( gamerstvLayout );

    tabSettings->addTab( gamerstvGroup, QIcon( ":/resources/gamerstvlogo.png" ), tr( "Gamerstv" ) );
}

void QSettingsDialog::setupGipsyteamTab()
{
    QSettings settings;

    QVBoxLayout * gipsyteamLayout = new QVBoxLayout();

    gipsyteamChannelCheckBox->setText( CHANNEL_TEXT );
    gipsyteamChannelCheckBox->setChecked( settings.value( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    gipsyteamChannelEdit->setText( settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, DEFAULT_GIPSYTEAM_CHANNEL_NAME ).toString() );
    gipsyteamChannelEdit->setEnabled( gipsyteamChannelCheckBox->isChecked() );

    QObject::connect( gipsyteamChannelCheckBox, SIGNAL( clicked( bool ) ), gipsyteamChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( gipsyteamLayout, { gipsyteamChannelCheckBox, gipsyteamChannelEdit } );

    gipsyteamAliasesEdit->setText( settings.value( GIPSYTEAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gipsyteamLayout, { new QLabel( ALIASES_TEXT, this ), gipsyteamAliasesEdit } );

    gipsyteamSupportersListEdit->setText( settings.value( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gipsyteamLayout, { new QLabel( SUPPORTERS_TEXT, this ), gipsyteamSupportersListEdit } );

    gipsyteamBlackListEdit->setText( settings.value( GIPSYTEAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( gipsyteamLayout, { new QLabel( BLACKLIST_TEXT, this ), gipsyteamBlackListEdit } );

    gipsyteamRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    gipsyteamRemoveBlackListUsersCheckBox->setChecked( settings.value( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    gipsyteamLayout->addWidget( gipsyteamRemoveBlackListUsersCheckBox );

    gipsyteamLayout->addStretch( 1 );

    QGroupBox * gipsyteamGroup = new QGroupBox( tabSettings );
    gipsyteamGroup->setLayout( gipsyteamLayout );

    tabSettings->addTab( gipsyteamGroup, QIcon( ":/resources/gipsyteamlogo.png" ), tr( "Gipsyteam" ) );
}

void QSettingsDialog::setupGoodgameTab()
{
    QSettings settings;

    QVBoxLayout * goodGameLayout = new QVBoxLayout();

    goodGameChannelCheckBox->setText( CHANNEL_TEXT );
    goodGameChannelCheckBox->setChecked( settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    goodGameChannelEdit->setText( settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString() );
    goodGameChannelEdit->setEnabled( goodGameChannelCheckBox->isChecked() );

    QObject::connect( goodGameChannelCheckBox, SIGNAL( clicked( bool ) ), goodGameChannelEdit, SLOT( setEnabled( bool ) ) );

    goodGameBadgesCheckBox->setText( tr( "Badges" ) );
    goodGameBadgesCheckBox->setChecked( settings.value( GOODGAME_BADGES_SETTING_PATH, false ).toBool() );

    goodGameUseAnimatedSmilesCheckBox->setText( tr( "Use Animated Smiles" ) );
    goodGameUseAnimatedSmilesCheckBox->setChecked( settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool() );

    addWidgets( goodGameLayout, { goodGameChannelCheckBox, goodGameChannelEdit, goodGameBadgesCheckBox, goodGameUseAnimatedSmilesCheckBox  } );

    goodGameAliasesEdit->setText( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( ALIASES_TEXT, this ), goodGameAliasesEdit } );

    goodGameSupportersListEdit->setText( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( SUPPORTERS_TEXT, this ), goodGameSupportersListEdit } );

    goodGameBlackListEdit->setText( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( BLACKLIST_TEXT, this ), goodGameBlackListEdit } );

    goodGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    goodGameRemoveBlackListUsersCheckBox->setChecked( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameRemoveBlackListUsersCheckBox );

    goodGameLayout->addStretch( 1 );

    QGroupBox * goodGameGroup = new QGroupBox( tabSettings );
    goodGameGroup->setLayout( goodGameLayout );

    tabSettings->addTab( goodGameGroup, QIcon( ":/resources/goodgamelogo.png" ), tr( "Goodgame" ) );
}

void QSettingsDialog::setupHitboxTab()
{
    QSettings settings;

    QVBoxLayout * hitboxLayout = new QVBoxLayout();

    hitboxChannelCheckBox->setText( CHANNEL_TEXT );
    hitboxChannelCheckBox->setChecked( settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    hitboxChannelEdit->setText( settings.value( HITBOX_CHANNEL_SETTING_PATH, DEFAULT_HITBOX_CHANNEL_NAME ).toString() );
    hitboxChannelEdit->setEnabled( hitboxChannelCheckBox->isChecked() );

    QObject::connect( hitboxChannelCheckBox, SIGNAL( clicked( bool ) ), hitboxChannelEdit, SLOT( setEnabled( bool ) ) );

    hitboxOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    hitboxOriginalColorsCheckBox->setChecked( settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    addWidgets( hitboxLayout, { hitboxChannelCheckBox, hitboxChannelEdit, hitboxOriginalColorsCheckBox  } );

    hitboxAliasesEdit->setText( settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( ALIASES_TEXT, this ), hitboxAliasesEdit } );

    hitboxSupportersListEdit->setText( settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( SUPPORTERS_TEXT, this ), hitboxSupportersListEdit } );

    hitboxBlackListEdit->setText( settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( BLACKLIST_TEXT, this ), hitboxBlackListEdit } );

    hitboxRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    hitboxRemoveBlackListUsersCheckBox->setChecked( settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    hitboxLayout->addWidget( hitboxRemoveBlackListUsersCheckBox );

    hitboxLayout->addStretch( 1 );

    QGroupBox * hitboxGroup = new QGroupBox( tabSettings );
    hitboxGroup->setLayout( hitboxLayout );

    tabSettings->addTab( hitboxGroup, QIcon( ":/resources/hitboxlogo.png" ), tr( "Hitbox" ) );
}

void QSettingsDialog::setupIgdcTab()
{
    QSettings settings;

    QVBoxLayout * igdcLayout = new QVBoxLayout();

    igdcChannelCheckBox->setText( CHANNEL_TEXT );
    igdcChannelCheckBox->setChecked( settings.value( IGDC_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    igdcChannelEdit->setText( settings.value( IGDC_CHANNEL_SETTING_PATH, DEFAULT_IGDC_CHANNEL_NAME ).toString() );
    igdcChannelEdit->setEnabled( igdcChannelCheckBox->isChecked() );

    QObject::connect( igdcChannelCheckBox, SIGNAL( clicked( bool ) ), igdcChannelEdit, SLOT( setEnabled( bool ) ) );

    igdcBadgesCheckBox->setText( tr( "Badges" ) );
    igdcBadgesCheckBox->setChecked( settings.value( IGDC_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( igdcLayout, { igdcChannelCheckBox, igdcChannelEdit, igdcBadgesCheckBox } );

    igdcAliasesEdit->setText( settings.value( IGDC_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( ALIASES_TEXT, this ), igdcAliasesEdit } );

    igdcSupportersListEdit->setText( settings.value( IGDC_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( SUPPORTERS_TEXT, this ), igdcSupportersListEdit } );

    igdcBlackListEdit->setText( settings.value( IGDC_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( BLACKLIST_TEXT, this ), igdcBlackListEdit } );

    igdcRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    igdcRemoveBlackListUsersCheckBox->setChecked( settings.value( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    igdcLayout->addWidget( igdcRemoveBlackListUsersCheckBox );

    igdcLayout->addStretch( 1 );

    QGroupBox * igdcGroup = new QGroupBox( tabSettings );
    igdcGroup->setLayout( igdcLayout );

    tabSettings->addTab( igdcGroup, QIcon( ":/resources/igdclogo.png" ), tr( "Igdc" ) );
}

void QSettingsDialog::setupLivecodingTab()
{
    QSettings settings;

    QVBoxLayout * livecodingLayout = new QVBoxLayout();

    livecodingChannelCheckBox->setText( CHANNEL_TEXT );
    livecodingChannelCheckBox->setChecked( settings.value( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    livecodingChannelEdit->setText( settings.value( LIVECODING_CHANNEL_SETTING_PATH, DEFAULT_LIVECODING_CHANNEL_NAME ).toString() );
    livecodingChannelEdit->setEnabled( livecodingChannelCheckBox->isChecked() );

    QObject::connect( livecodingChannelCheckBox, SIGNAL( clicked( bool ) ), livecodingChannelEdit, SLOT( setEnabled( bool ) ) );

    livecodingBadgesCheckBox->setText( tr( "Badges" ) );
    livecodingBadgesCheckBox->setChecked( settings.value( LIVECODING_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( livecodingLayout, { livecodingChannelCheckBox, livecodingChannelEdit, livecodingBadgesCheckBox } );

    livecodingLoginEdit->setText( settings.value( LIVECODING_LOGIN_SETTING_PATH, BLANK_STRING ).toString() );

    livecodingPasswordEdit->setEchoMode( QLineEdit::Password );
    livecodingPasswordEdit->setText( settings.value( LIVECODING_PASSWORD_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( livecodingLayout, { new QLabel( tr( "Login:" ), this ), livecodingLoginEdit, new QLabel( tr( "Password:" ), this ), livecodingPasswordEdit } );

    livecodingAliasesEdit->setText( settings.value( LIVECODING_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( livecodingLayout, { new QLabel( ALIASES_TEXT, this ), livecodingAliasesEdit } );

    livecodingSupportersListEdit->setText( settings.value( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( livecodingLayout, { new QLabel( SUPPORTERS_TEXT, this ), livecodingSupportersListEdit } );

    livecodingBlackListEdit->setText( settings.value( LIVECODING_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( livecodingLayout, { new QLabel( BLACKLIST_TEXT, this ), livecodingBlackListEdit } );

    livecodingRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    livecodingRemoveBlackListUsersCheckBox->setChecked( settings.value( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    livecodingLayout->addWidget( livecodingRemoveBlackListUsersCheckBox );

    livecodingLayout->addStretch( 1 );

    QGroupBox * livecodingGroup = new QGroupBox( tabSettings );
    livecodingGroup->setLayout( livecodingLayout );

    tabSettings->addTab( livecodingGroup, QIcon( ":/resources/livecodinglogo.png" ), tr( "Livecoding" ) );
}

void QSettingsDialog::setupSc2tvTab()
{
    QSettings settings;

    QVBoxLayout * sc2tvLayout = new QVBoxLayout();

    sc2tvChannelCheckBox->setText( CHANNEL_TEXT );
    sc2tvChannelCheckBox->setChecked( settings.value( SC2TV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    sc2tvChannelEdit->setText( settings.value( SC2TV_CHANNEL_SETTING_PATH, DEFAULT_SC2TV_CHANNEL_NAME ).toString() );
    sc2tvChannelEdit->setEnabled( sc2tvChannelCheckBox->isChecked() );

    QObject::connect( sc2tvChannelCheckBox, SIGNAL( clicked( bool ) ), sc2tvChannelEdit, SLOT( setEnabled( bool ) ) );

    sc2tvOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    sc2tvOriginalColorsCheckBox->setChecked( settings.value( SC2TV_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    addWidgets( sc2tvLayout, { sc2tvChannelCheckBox, sc2tvChannelEdit, sc2tvOriginalColorsCheckBox  } );

    sc2tvAliasesEdit->setText( settings.value( SC2TV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( sc2tvLayout, { new QLabel( ALIASES_TEXT, this ), sc2tvAliasesEdit } );

    sc2tvSupportersListEdit->setText( settings.value( SC2TV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( sc2tvLayout, { new QLabel( SUPPORTERS_TEXT, this ), sc2tvSupportersListEdit } );

    sc2tvBlackListEdit->setText( settings.value( SC2TV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( sc2tvLayout, { new QLabel( BLACKLIST_TEXT, this ), sc2tvBlackListEdit } );

    sc2tvRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    sc2tvRemoveBlackListUsersCheckBox->setChecked( settings.value( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    sc2tvLayout->addWidget( sc2tvRemoveBlackListUsersCheckBox );

    sc2tvLayout->addStretch( 1 );

    QGroupBox * sc2tvGroup = new QGroupBox( tabSettings );
    sc2tvGroup->setLayout( sc2tvLayout );

    tabSettings->addTab( sc2tvGroup, QIcon( ":/resources/sc2tvlogo.png" ), tr( "Sc2tv" ) );
}

void QSettingsDialog::setupStreamcubeTab()
{
    QSettings settings;

    QVBoxLayout * streamcubeLayout = new QVBoxLayout();

    streamcubeChannelCheckBox->setText( CHANNEL_TEXT );
    streamcubeChannelCheckBox->setChecked( settings.value( STREAMCUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    streamcubeChannelEdit->setText( settings.value( STREAMCUBE_CHANNEL_SETTING_PATH, DEFAULT_STREAMCUBE_CHANNEL_NAME ).toString() );
    streamcubeChannelEdit->setEnabled( streamcubeChannelCheckBox->isChecked() );

    QObject::connect( streamcubeChannelCheckBox, SIGNAL( clicked( bool ) ), streamcubeChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( streamcubeLayout, { streamcubeChannelCheckBox, streamcubeChannelEdit } );

    streamcubeAliasesEdit->setText( settings.value( STREAMCUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( streamcubeLayout, { new QLabel( ALIASES_TEXT, this ), streamcubeAliasesEdit } );

    streamcubeSupportersListEdit->setText( settings.value( STREAMCUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( streamcubeLayout, { new QLabel( SUPPORTERS_TEXT, this ), streamcubeSupportersListEdit } );

    streamcubeBlackListEdit->setText( settings.value( STREAMCUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( streamcubeLayout, { new QLabel( BLACKLIST_TEXT, this ), streamcubeBlackListEdit } );

    streamcubeRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    streamcubeRemoveBlackListUsersCheckBox->setChecked( settings.value( STREAMCUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    streamcubeLayout->addWidget( streamcubeRemoveBlackListUsersCheckBox );

    streamcubeLayout->addStretch( 1 );

    QGroupBox * streamcubeGroup = new QGroupBox( tabSettings );
    streamcubeGroup->setLayout( streamcubeLayout );

    tabSettings->addTab( streamcubeGroup, QIcon( ":/resources/streamcubelogo.png" ), tr( "Streamcube" ) );
}

void QSettingsDialog::setupTwitchTab()
{
    QSettings settings;

    QVBoxLayout * twitchLayout = new QVBoxLayout();

    twitchChannelCheckBox->setText( CHANNEL_TEXT );
    twitchChannelCheckBox->setChecked( settings.value( TWITCH_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    twitchChannelEdit->setText( settings.value( TWITCH_CHANNEL_SETTING_PATH, DEFAULT_TWITCH_CHANNEL_NAME ).toString() );
    twitchChannelEdit->setEnabled( twitchChannelCheckBox->isChecked() );

    QObject::connect( twitchChannelCheckBox, SIGNAL( clicked( bool ) ), twitchChannelEdit, SLOT( setEnabled( bool ) ) );

    twitchOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    twitchOriginalColorsCheckBox->setChecked( settings.value( TWITCH_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    twitchBadgesCheckBox->setText( tr( "Badges" ) );
    twitchBadgesCheckBox->setChecked( settings.value( TWITCH_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( twitchLayout, { twitchChannelCheckBox, twitchChannelEdit, twitchOriginalColorsCheckBox, twitchBadgesCheckBox } );

    twitchAliasesEdit->setText( settings.value( TWITCH_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( twitchLayout, { new QLabel( ALIASES_TEXT, this ), twitchAliasesEdit } );

    twitchSupportersListEdit->setText( settings.value( TWITCH_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( twitchLayout, { new QLabel( SUPPORTERS_TEXT, this ), twitchSupportersListEdit } );

    twitchBlackListEdit->setText( settings.value( TWITCH_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( twitchLayout, { new QLabel( BLACKLIST_TEXT, this ), twitchBlackListEdit } );

    twitchRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    twitchRemoveBlackListUsersCheckBox->setChecked( settings.value( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    twitchLayout->addWidget( twitchRemoveBlackListUsersCheckBox );

    twitchLayout->addStretch( 1 );

    QGroupBox * twitchGroup = new QGroupBox( tabSettings );
    twitchGroup->setLayout( twitchLayout );

    tabSettings->addTab( twitchGroup, QIcon( ":/resources/twitchlogo.png" ), tr( "Twitch" ) );
}


void QSettingsDialog::setupVidiTab()
{
    QSettings settings;

    QVBoxLayout * vidiLayout = new QVBoxLayout();

    vidiChannelCheckBox->setText( CHANNEL_TEXT );
    vidiChannelCheckBox->setChecked( settings.value( VIDI_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    vidiChannelEdit->setText( settings.value( VIDI_CHANNEL_SETTING_PATH, DEFAULT_VIDI_CHANNEL_NAME ).toString() );
    vidiChannelEdit->setEnabled( vidiChannelCheckBox->isChecked() );

    QObject::connect( vidiChannelCheckBox, SIGNAL( clicked( bool ) ), vidiChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( vidiLayout, { vidiChannelCheckBox, vidiChannelEdit } );

    vidiAliasesEdit->setText( settings.value( VIDI_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( vidiLayout, { new QLabel( ALIASES_TEXT, this ), vidiAliasesEdit } );

    vidiSupportersListEdit->setText( settings.value( VIDI_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( vidiLayout, { new QLabel( SUPPORTERS_TEXT, this ), vidiSupportersListEdit } );

    vidiBlackListEdit->setText( settings.value( VIDI_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( vidiLayout, { new QLabel( BLACKLIST_TEXT, this ), vidiBlackListEdit } );

    vidiRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    vidiRemoveBlackListUsersCheckBox->setChecked( settings.value( VIDI_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    vidiLayout->addWidget( vidiRemoveBlackListUsersCheckBox );

    vidiLayout->addStretch( 1 );

    QGroupBox * vidiGroup = new QGroupBox( tabSettings );
    vidiGroup->setLayout( vidiLayout );

    tabSettings->addTab( vidiGroup, QIcon( ":/resources/vidilogo.png" ), tr( "Vidi" ) );
}


void QSettingsDialog::saveSettings()
{
    QSettings settings;
    int oldIntValue;
    bool oldBoolValue;
    QString oldStringValue;

    bool defaultStyleChanged = false;

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() != fontNameCombo->currentText() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BODY_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_BODY_BACKGROUND_COLOR ).toUInt() != bodyBackgroundColor );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR ).toUInt() != evenMessagesNicknamesColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR ).toUInt() != oddMessagesNicknamesColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR ).toUInt() != aliasesNicknamesColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR ).toUInt() != supportersNicknamesColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_COLOR ).toUInt() != blackListNicknamesColor );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != evenMessagesNicknamesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != oddMessagesNicknamesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != aliasesNicknamesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != supportersNicknamesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != blackListNicknamesFontSizeSpinBox->value() );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR ).toUInt() != evenMessagesBackgroundColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR ).toUInt() != oddMessagesBackgroundColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR ).toUInt() != aliasesBackgroundColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR ).toUInt() != supportersBackgroundColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BACKGROUND_COLOR ).toUInt() != blackListBackgroundColor );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != evenMessagesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != oddMessagesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != aliasesMessagesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != supportersMessagesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != blackListMessagesTextColor );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != evenMessagesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != oddMessagesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != aliasesMessagesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != supportersMessagesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != blackListMessagesFontSizeSpinBox->value() );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_EVEN_MESSAGES_BORDER_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR ).toUInt() != evenMessagesBorderColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ODD_MESSAGES_BORDER_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR ).toUInt() != oddMessagesBorderColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_MESSAGES_BORDER_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR ).toUInt() != aliasesMessagesBorderColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SUPPORTERS_MESSAGES_BORDER_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR ).toUInt() != supportersMessagesBorderColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_BLACK_LIST_MESSAGES_BORDER_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_COLOR ).toUInt() != blackListMessagesBorderColor );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_LEFT_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() != messagesLeftBorderSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_RIGHT_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() != messagesRightBorderSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_TOP_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() != messagesTopBorderSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_BOTTOM_BORDER_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_SIZE ).toInt() != messagesBottomBorderSizeSpinBox->value() );


    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_TOP_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() != messagesTopLeftBorderRadiusSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_TOP_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() != messagesTopRightBorderRadiusSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_BOTTOM_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() != messagesBottomLeftBorderRadiusSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_BOTTOM_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_BORDER_RADIUS_SIZE ).toInt() != messagesBottomRightBorderRadiusSizeSpinBox->value() );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE ).toInt() != messagesMarginBottomSizeSpinBox->value() );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_INDENT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_INDENT_SIZE ).toInt() != messagesIndentSizeSpinBox->value() );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE ).toInt() != viewersFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR ).toUInt() != viewersTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR ).toUInt() != viewersBackgroundColor );

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_LINKS_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_LINKS_COLOR ).toUInt() != linksColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SMILES_SIZE ).toInt() != smilesSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE ).toInt() != serviceIconsSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MAX_IMAGES_HEIGHT_SETTING_PATH, DEFAULT_GENERATED_STYLE_MAX_IMAGES_HEIGHT ).toInt() != maxImagesHeightSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE ).toString() != animationTypeCombo->currentText() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ANIMATION_DURATION_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_DURATION ).toDouble() != animationDurationSpinBox->value() );

    if( defaultStyleChanged )
    {
        settings.setValue( GENERATED_STYLE_FONT_NAME_SETTING_PATH, fontNameCombo->currentText() );
        settings.setValue( GENERATED_STYLE_BODY_BACKGROUND_COLOR_SETTING_PATH, bodyBackgroundColor );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, evenMessagesNicknamesColor );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, oddMessagesNicknamesColor );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, aliasesNicknamesColor );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, supportersNicknamesColor );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_COLOR_SETTING_PATH, blackListNicknamesColor );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, evenMessagesNicknamesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, oddMessagesNicknamesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, aliasesNicknamesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, supportersNicknamesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_NICKNAMES_FONT_SIZE_SETTING_PATH, blackListNicknamesFontSizeSpinBox->value() );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, evenMessagesBackgroundColor );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, oddMessagesBackgroundColor );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, aliasesBackgroundColor );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, supportersBackgroundColor );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_BACKGROUND_COLOR_SETTING_PATH, blackListBackgroundColor );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_TEXT_COLOR_SETTING_PATH, evenMessagesTextColor );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_TEXT_COLOR_SETTING_PATH, oddMessagesTextColor );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_TEXT_COLOR_SETTING_PATH, aliasesMessagesTextColor );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_TEXT_COLOR_SETTING_PATH, supportersMessagesTextColor );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_TEXT_COLOR_SETTING_PATH, blackListMessagesTextColor );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_FONT_SIZE_SETTING_PATH, evenMessagesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_FONT_SIZE_SETTING_PATH, oddMessagesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_FONT_SIZE_SETTING_PATH, aliasesMessagesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_FONT_SIZE_SETTING_PATH, supportersMessagesFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_FONT_SIZE_SETTING_PATH, blackListMessagesFontSizeSpinBox->value() );

        settings.setValue( GENERATED_STYLE_EVEN_MESSAGES_BORDER_COLOR_SETTING_PATH, evenMessagesBorderColor );
        settings.setValue( GENERATED_STYLE_ODD_MESSAGES_BORDER_COLOR_SETTING_PATH, oddMessagesBorderColor );
        settings.setValue( GENERATED_STYLE_ALIASES_MESSAGES_BORDER_COLOR_SETTING_PATH, aliasesMessagesBorderColor );
        settings.setValue( GENERATED_STYLE_SUPPORTERS_MESSAGES_BORDER_COLOR_SETTING_PATH, supportersMessagesBorderColor );
        settings.setValue( GENERATED_STYLE_BLACK_LIST_MESSAGES_BORDER_COLOR_SETTING_PATH, blackListMessagesBorderColor );

        settings.setValue( GENERATED_STYLE_MESSAGES_LEFT_BORDER_SIZE_SETTING_PATH, messagesLeftBorderSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_RIGHT_BORDER_SIZE_SETTING_PATH, messagesRightBorderSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_TOP_BORDER_SIZE_SETTING_PATH, messagesTopBorderSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_BOTTOM_BORDER_SIZE_SETTING_PATH, messagesBottomBorderSizeSpinBox->value() );

        settings.setValue( GENERATED_STYLE_MESSAGES_TOP_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, messagesTopLeftBorderRadiusSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_TOP_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, messagesTopRightBorderRadiusSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_BOTTOM_LEFT_BORDER_RADIUS_SIZE_SETTING_PATH, messagesBottomLeftBorderRadiusSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_BOTTOM_RIGHT_BORDER_RADIUS_SIZE_SETTING_PATH, messagesBottomRightBorderRadiusSizeSpinBox->value() );

        settings.setValue( GENERATED_STYLE_MESSAGES_MARGIN_BOTTOM_SIZE_SETTING_PATH, messagesMarginBottomSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MESSAGES_INDENT_SIZE_SETTING_PATH, messagesIndentSizeSpinBox->value() );

        settings.setValue( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, viewersFontSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, viewersTextColor );
        settings.setValue( GENERATED_STYLE_VIEWERS_BACKGROUND_COLOR_SETTING_PATH, viewersBackgroundColor );

        settings.setValue( GENERATED_STYLE_LINKS_COLOR_SETTING_PATH, linksColor );
        settings.setValue( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, smilesSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, serviceIconsSizeSpinBox->value() );
        settings.setValue( GENERATED_STYLE_MAX_IMAGES_HEIGHT_SETTING_PATH, maxImagesHeightSpinBox->value() );
        settings.setValue( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, animationTypeCombo->currentText() );
        settings.setValue( GENERATED_STYLE_ANIMATION_DURATION_SETTING_PATH, animationDurationSpinBox->value() );
    }

    bool needReloadFlagsAndAtributes = false;

    oldBoolValue = settings.value( STAY_ON_TOP_SETTING_PATH, DEFAULT_STAY_ON_TOP ).toBool();
    if( oldBoolValue != stayOnTopCheckBox->isChecked() )
    {
        settings.setValue( STAY_ON_TOP_SETTING_PATH, stayOnTopCheckBox->isChecked() );
        needReloadFlagsAndAtributes = true;
    }

    oldBoolValue = settings.value( FRAMELESS_WINDOW_SETTING_PATH, DEFAULT_FRAMELESS_WINDOW ).toBool();
    if( oldBoolValue != frameLessWindowCheckBox->isChecked() )
    {
        settings.setValue( FRAMELESS_WINDOW_SETTING_PATH, frameLessWindowCheckBox->isChecked() );
        needReloadFlagsAndAtributes = true;
    }

    /*
    oldBoolValue = settings.value( TRANSPARENT_WINDOW_SETTING_PATH, DEFAULT_TRANSPARENT_WINDOW ).toBool();
    if( oldBoolValue != transparentWindowCheckBox->isChecked() )
    {
        settings.setValue( TRANSPARENT_WINDOW_SETTING_PATH ,transparentWindowCheckBox->isChecked() );
        needReloadFlagsAndAtributes = true;
    }
    */

    if( needReloadFlagsAndAtributes )
    {
        emit flagsOrAttributesChanged();
    }

    oldStringValue = settings.value( STYLE_SETTING_PATH, DEFAULT_STYLE_NAME ).toString();
    if( ( oldStringValue != styleCombo->currentText() ) || ( defaultStyleChanged && 0 == styleCombo->currentIndex() ) )
    {
        if( 0 == styleCombo->currentIndex() )
        {
            settings.setValue( STYLE_SETTING_PATH, styleCombo->currentText() );
            qDebug() << "generated style";
        }
        else
        {
            QString pathToStyle = QApplication::applicationDirPath() + "/styles/" + styleCombo->currentText();
            if( QFileInfo::exists( pathToStyle ) )
            {
                settings.setValue( STYLE_SETTING_PATH, styleCombo->currentText() );
            }
        }

        emit styleChanged();
    }

    oldIntValue = settings.value( OPACITY_SETTING_PATH, DEFAULT_OPACITY_VALUE ).toInt();
    if( oldIntValue != opacitySlider->value() )
    {
        settings.setValue( OPACITY_SETTING_PATH, opacitySlider->value() );
        emit opacityChanged();
    }

    oldBoolValue = settings.value( SHOW_SYSTEM_MESSAGES_SETTING_PATH, DEFAULT_SHOW_SYSTEM_MESSAGES ).toBool();
    if( oldBoolValue != showSystemMessagesCheckBox->isChecked() )
    {
        settings.setValue( SHOW_SYSTEM_MESSAGES_SETTING_PATH, showSystemMessagesCheckBox->isChecked() );
        emit showSystemMessagesChanged();
    }

    oldBoolValue = settings.value( SHOW_IMAGES_SETTING_PATH, DEFAULT_SHOW_IMAGES ).toBool();
    if( oldBoolValue != showImagesCheckBox->isChecked() )
    {
        settings.setValue( SHOW_IMAGES_SETTING_PATH, showImagesCheckBox->isChecked() );
        emit showImagesChanged();
    }

    oldBoolValue = settings.value( SAVE_MESSAGES_TO_LOG_FILE_SETTING_PATH, DEFAULT_SAVE_MESSAGES_TO_LOG_FILE ).toBool();
    if( oldBoolValue != saveMessagesToLogCheckBox->isChecked() )
    {
        settings.setValue( SAVE_MESSAGES_TO_LOG_FILE_SETTING_PATH, saveMessagesToLogCheckBox->isChecked() );
        emit saveMessagesToLogChanged();
    }

    bool serverStateChanged = false;

    oldBoolValue = settings.value( USE_SERVER_SETTING_PAH, DEFAULT_USE_SERVER ).toBool();
    if( oldBoolValue != useServerCheckBox->isChecked() )
    {
        settings.setValue( USE_SERVER_SETTING_PAH, useServerCheckBox->isChecked() );
        serverStateChanged = true;
    }

    oldIntValue = settings.value( SERVER_PORT_SETTING_PATH, DEFAULT_SERVER_PORT ).toInt();
    if( oldIntValue != serverPortSpinBox->value() )
    {
        settings.setValue( SERVER_PORT_SETTING_PATH, serverPortSpinBox->value() );
        serverStateChanged = true;
    }

    if( serverStateChanged )
    {
        emit useServerStateChanged();
    }

    bool isSaveToFileStateChanged = false;

    oldBoolValue = settings.value( SAVE_TO_FILE_SETTING_PATTH, DEFAULT_SAVE_TO_FILE ).toBool();
    if( oldBoolValue != saveToFileCheckBox->isChecked() )
    {
        settings.setValue( SAVE_TO_FILE_SETTING_PATTH, saveToFileCheckBox->isChecked() );
        isSaveToFileStateChanged = true;
    }

    oldIntValue = settings.value( SAVE_TO_FILE_INTERVAL_SETTING_PATH, DEFAULT_SAVE_TO_FILE_INTERVAL ).toInt();
    if( oldIntValue != saveToFileIntervalSpinBox->value() )
    {
        settings.setValue( SAVE_TO_FILE_INTERVAL_SETTING_PATH, saveToFileIntervalSpinBox->value() );
        isSaveToFileStateChanged = true;
    }

    if( isSaveToFileStateChanged )
    {
        emit saveToFileStateChanged();
    }

    //настройки aces
    oldBoolValue = settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( ACES_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != acesChannelCheckBox->isChecked() || oldStringValue != acesChannelEdit->text() )
    {
        settings.setValue( ACES_CHANNEL_ENABLE_SETTING_PATH, acesChannelCheckBox->isChecked() );
        settings.setValue( ACES_CHANNEL_SETTING_PATH, acesChannelEdit->text() );

        emit acesChannelChanged();
    }
    //emit acesChannelChanged();

    /*
     * oldBoolValue = settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != acesOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( ACES_ORIGINAL_COLORS_SETTING_PATH, acesOriginalColorsCheckBox->isChecked() );
        emit acesOriginalColorsChanged( acesOriginalColorsCheckBox->isChecked() );
    }
    */

    oldStringValue = settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesAliasesEdit->text() )
    {
        settings.setValue( ACES_ALIASES_SETTING_PATH, acesAliasesEdit->text() );
        emit acesAliasesChanged( acesAliasesEdit->text() );
    }

    oldStringValue = settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesSupportersListEdit->toPlainText() )
    {
        settings.setValue( ACES_SUPPORTERS_LIST_SETTING_PATH, acesSupportersListEdit->toPlainText() );
        emit acesSupportersListChanged( acesSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesBlackListEdit->toPlainText() )
    {
        settings.setValue( ACES_BLACK_LIST_SETTING_PATH, acesBlackListEdit->toPlainText() );
        emit acesBlackListChanged( acesBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != acesRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, acesRemoveBlackListUsersCheckBox->isChecked() );
        emit acesRemoveBlackListUsersChanged( acesRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки azubu
    oldBoolValue = settings.value( AZUBU_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( AZUBU_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != azubuChannelCheckBox->isChecked() || oldStringValue != azubuChannelEdit->text() )
    {
        settings.setValue( AZUBU_CHANNEL_ENABLE_SETTING_PATH, azubuChannelCheckBox->isChecked() );
        settings.setValue( AZUBU_CHANNEL_SETTING_PATH, azubuChannelEdit->text() );

        emit azubuChannelChanged();
    }

    oldStringValue = settings.value( AZUBU_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != azubuAliasesEdit->text() )
    {
        settings.setValue( AZUBU_ALIASES_SETTING_PATH, azubuAliasesEdit->text() );
        emit azubuAliasesChanged( azubuAliasesEdit->text() );
    }

    oldStringValue = settings.value( AZUBU_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != azubuSupportersListEdit->toPlainText() )
    {
        settings.setValue( AZUBU_SUPPORTERS_LIST_SETTING_PATH, azubuSupportersListEdit->toPlainText() );
        emit azubuSupportersListChanged( azubuSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( AZUBU_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != azubuBlackListEdit->toPlainText() )
    {
        settings.setValue( AZUBU_BLACK_LIST_SETTING_PATH, azubuBlackListEdit->toPlainText() );
        emit azubuBlackListChanged( azubuBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( AZUBU_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != azubuRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( AZUBU_REMOVE_BLACK_LIST_USERS_SETTING_PATH, azubuRemoveBlackListUsersCheckBox->isChecked() );
        emit azubuRemoveBlackListUsersChanged( azubuRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки beampro
    oldBoolValue = settings.value( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( BEAMPRO_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != beamproChannelCheckBox->isChecked() || oldStringValue != beamproChannelEdit->text() )
    {
        settings.setValue( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, beamproChannelCheckBox->isChecked() );
        settings.setValue( BEAMPRO_CHANNEL_SETTING_PATH, beamproChannelEdit->text() );

        emit beamproChannelChanged();
    }

    oldStringValue = settings.value( BEAMPRO_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproAliasesEdit->text() )
    {
        settings.setValue( BEAMPRO_ALIASES_SETTING_PATH, beamproAliasesEdit->text() );
        emit beamproAliasesChanged( beamproAliasesEdit->text() );
    }

    oldStringValue = settings.value( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproSupportersListEdit->toPlainText() )
    {
        settings.setValue( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, beamproSupportersListEdit->toPlainText() );
        emit beamproSupportersListChanged( beamproSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( BEAMPRO_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproBlackListEdit->toPlainText() )
    {
        settings.setValue( BEAMPRO_BLACK_LIST_SETTING_PATH, beamproBlackListEdit->toPlainText() );
        emit beamproBlackListChanged( beamproBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != beamproRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, beamproRemoveBlackListUsersCheckBox->isChecked() );
        emit beamproRemoveBlackListUsersChanged( beamproRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки cybergame

    oldBoolValue = settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( CYBERGAME_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != cyberGameChannelCheckBox->isChecked() || oldStringValue != cyberGameChannelEdit->text() )
    {
        settings.setValue( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, cyberGameChannelCheckBox->isChecked() );
        settings.setValue( CYBERGAME_CHANNEL_SETTING_PATH, cyberGameChannelEdit->text() );

        emit cyberGameChannelChanged();
    }

    oldStringValue = settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != cyberGameAliasesEdit->text() )
    {
        settings.setValue( CYBERGAME_ALIASES_SETTING_PATH, cyberGameAliasesEdit->text() );
        emit cyberGameAliasesChanged( cyberGameAliasesEdit->text() );
    }

    oldStringValue = settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != cyberGameSupportersListEdit->toPlainText() )
    {
        settings.setValue( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, cyberGameSupportersListEdit->toPlainText() );
        emit cyberGameSupportersListChanged( cyberGameSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != cyberGameBlackListEdit->toPlainText() )
    {
        settings.setValue( CYBERGAME_BLACK_LIST_SETTING_PATH, cyberGameBlackListEdit->toPlainText() );
        emit cyberGameBlackListChanged( cyberGameBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != cyberGameRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, cyberGameRemoveBlackListUsersCheckBox->isChecked() );
        emit cyberGameRemoveBlackListUsersChanged( cyberGameRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки funstream

    oldBoolValue = settings.value( FUNSTREAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != funstreamChannelCheckBox->isChecked() || oldStringValue != funstreamChannelEdit->text() )
    {
        settings.setValue( FUNSTREAM_CHANNEL_ENABLE_SETTING_PATH, funstreamChannelCheckBox->isChecked() );
        settings.setValue( FUNSTREAM_CHANNEL_SETTING_PATH, funstreamChannelEdit->text() );

        emit funstreamChannelChanged();
    }

    oldBoolValue = settings.value( FUNSTREAM_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != funstreamOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( FUNSTREAM_ORIGINAL_COLORS_SETTING_PATH, funstreamOriginalColorsCheckBox->isChecked() );
        emit funstreamOriginalColorsChanged( funstreamOriginalColorsCheckBox->isChecked() );
    }

    oldBoolValue = settings.value( FUNSTREAM_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != funstreamBadgesCheckBox->isChecked() )
    {
        settings.setValue( FUNSTREAM_BADGES_SETTING_PATH, funstreamBadgesCheckBox->isChecked() );
        emit funstreamBadgesChanged( funstreamBadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( FUNSTREAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != funstreamAliasesEdit->text() )
    {
        settings.setValue( FUNSTREAM_ALIASES_SETTING_PATH, funstreamAliasesEdit->text() );
        emit funstreamAliasesChanged( funstreamAliasesEdit->text() );
    }

    oldStringValue = settings.value( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != funstreamSupportersListEdit->toPlainText() )
    {
        settings.setValue( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, funstreamSupportersListEdit->toPlainText() );
        emit funstreamSupportersListChanged( funstreamSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( FUNSTREAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != funstreamBlackListEdit->toPlainText() )
    {
        settings.setValue( FUNSTREAM_BLACK_LIST_SETTING_PATH, funstreamBlackListEdit->toPlainText() );
        emit funstreamBlackListChanged( funstreamBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != funstreamRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, funstreamRemoveBlackListUsersCheckBox->isChecked() );
        emit funstreamRemoveBlackListUsersChanged( funstreamRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки gamerstv

    oldBoolValue = settings.value( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( GAMERSTV_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != gamerstvChannelCheckBox->isChecked() || oldStringValue != gamerstvChannelEdit->text() )
    {
        settings.setValue( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, gamerstvChannelCheckBox->isChecked() );
        settings.setValue( GAMERSTV_CHANNEL_SETTING_PATH, gamerstvChannelEdit->text() );

        emit gamerstvChannelChanged();
    }

    oldBoolValue = settings.value( GAMERSTV_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != gamerstvBadgesCheckBox->isChecked() )
    {
        settings.setValue( GAMERSTV_BADGES_SETTING_PATH, gamerstvBadgesCheckBox->isChecked() );
        emit gamerstvBadgesChanged( gamerstvBadgesCheckBox->isChecked() );
    }


    oldStringValue = settings.value( GAMERSTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gamerstvAliasesEdit->text() )
    {
        settings.setValue( GAMERSTV_ALIASES_SETTING_PATH, gamerstvAliasesEdit->text() );
        emit gamerstvAliasesChanged( gamerstvAliasesEdit->text() );
    }

    oldStringValue = settings.value( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gamerstvSupportersListEdit->toPlainText() )
    {
        settings.setValue( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, gamerstvSupportersListEdit->toPlainText() );
        emit gamerstvSupportersListChanged( gamerstvSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( GAMERSTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gamerstvBlackListEdit->toPlainText() )
    {
        settings.setValue( GAMERSTV_BLACK_LIST_SETTING_PATH, gamerstvBlackListEdit->toPlainText() );
        emit gamerstvBlackListChanged( gamerstvBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != gamerstvRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, gamerstvRemoveBlackListUsersCheckBox->isChecked() );
        emit gamerstvRemoveBlackListUsersChanged( gamerstvRemoveBlackListUsersCheckBox->isChecked() );
    }
    //настройки gipsyteam

    oldBoolValue = settings.value( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != gipsyteamChannelCheckBox->isChecked() || oldStringValue != gipsyteamChannelEdit->text() )
    {
        settings.setValue( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, gipsyteamChannelCheckBox->isChecked() );
        settings.setValue( GIPSYTEAM_CHANNEL_SETTING_PATH, gipsyteamChannelEdit->text() );

        emit gipsyteamChannelChanged();
    }

    oldStringValue = settings.value( GIPSYTEAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gipsyteamAliasesEdit->text() )
    {
        settings.setValue( GIPSYTEAM_ALIASES_SETTING_PATH, gipsyteamAliasesEdit->text() );
        emit gipsyteamAliasesChanged( gipsyteamAliasesEdit->text() );
    }

    oldStringValue = settings.value( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gipsyteamSupportersListEdit->toPlainText() )
    {
        settings.setValue( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, gipsyteamSupportersListEdit->toPlainText() );
        emit gipsyteamSupportersListChanged( gipsyteamSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( GIPSYTEAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != gipsyteamBlackListEdit->toPlainText() )
    {
        settings.setValue( GIPSYTEAM_BLACK_LIST_SETTING_PATH, gipsyteamBlackListEdit->toPlainText() );
        emit gipsyteamBlackListChanged( gipsyteamBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != gipsyteamRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, gipsyteamRemoveBlackListUsersCheckBox->isChecked() );
        emit gipsyteamRemoveBlackListUsersChanged( gipsyteamRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки gg

    oldBoolValue = settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( GOODGAME_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != goodGameChannelCheckBox->isChecked() || oldStringValue != goodGameChannelEdit->text() )
    {
        settings.setValue( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, goodGameChannelCheckBox->isChecked() );
        settings.setValue( GOODGAME_CHANNEL_SETTING_PATH, goodGameChannelEdit->text() );

        emit goodGameChannelChanged();
    }

    oldBoolValue = settings.value( GOODGAME_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameBadgesCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_BADGES_SETTING_PATH, goodGameBadgesCheckBox->isChecked() );
        emit goodGameBadgesChanged( goodGameBadgesCheckBox->isChecked() );
    }

    oldBoolValue = settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameUseAnimatedSmilesCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, goodGameUseAnimatedSmilesCheckBox->isChecked() );
        emit goodGameUseAnimatedSmilesChanged( goodGameUseAnimatedSmilesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameAliasesEdit->text() )
    {
        settings.setValue( GOODGAME_ALIASES_SETTING_PATH, goodGameAliasesEdit->text() );
        emit goodGameAliasesChanged( goodGameAliasesEdit->text() );
    }

    oldStringValue = settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameSupportersListEdit->toPlainText() )
    {
        settings.setValue( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, goodGameSupportersListEdit->toPlainText() );
        emit goodGameSupportersListChanged( goodGameSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameBlackListEdit->toPlainText() )
    {
        settings.setValue( GOODGAME_BLACK_LIST_SETTING_PATH, goodGameBlackListEdit->toPlainText() );
        emit goodGameBlackListChanged( goodGameBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, goodGameRemoveBlackListUsersCheckBox->isChecked() );
        emit goodGameRemoveBlackListUsersChanged( goodGameRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки hitbox

    oldBoolValue = settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( HITBOX_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != hitboxChannelCheckBox->isChecked() || oldStringValue != hitboxChannelEdit->text() )
    {
        settings.setValue( HITBOX_CHANNEL_ENABLE_SETTING_PATH, hitboxChannelCheckBox->isChecked() );
        settings.setValue( HITBOX_CHANNEL_SETTING_PATH, hitboxChannelEdit->text() );

        emit hitboxChannelChanged();
    }

    oldBoolValue = settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != hitboxOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( HITBOX_ORIGINAL_COLORS_SETTING_PATH, hitboxOriginalColorsCheckBox->isChecked() );
        emit hitboxOriginalColorsChanged( hitboxOriginalColorsCheckBox->isChecked() );
    }

    oldStringValue = settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxAliasesEdit->text() )
    {
        settings.setValue( HITBOX_ALIASES_SETTING_PATH, hitboxAliasesEdit->text() );
        emit hitboxAliasesChanged( hitboxAliasesEdit->text() );
    }

    oldStringValue = settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxSupportersListEdit->toPlainText() )
    {
        settings.setValue( HITBOX_SUPPORTERS_LIST_SETTING_PATH, hitboxSupportersListEdit->toPlainText() );
        emit hitboxSupportersListChanged( hitboxSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxBlackListEdit->toPlainText() )
    {
        settings.setValue( HITBOX_BLACK_LIST_SETTING_PATH, hitboxBlackListEdit->toPlainText() );
        emit hitboxBlackListChanged( hitboxBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != hitboxRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, hitboxRemoveBlackListUsersCheckBox->isChecked() );
        emit hitboxRemoveBlackListUsersChanged( hitboxRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки igdc

    oldBoolValue = settings.value( IGDC_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( IGDC_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != igdcChannelCheckBox->isChecked() || oldStringValue != igdcChannelEdit->text() )
    {
        settings.setValue( IGDC_CHANNEL_ENABLE_SETTING_PATH, igdcChannelCheckBox->isChecked() );
        settings.setValue( IGDC_CHANNEL_SETTING_PATH, igdcChannelEdit->text() );

        emit igdcChannelChanged();
    }

    oldBoolValue = settings.value( IGDC_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != igdcBadgesCheckBox->isChecked() )
    {
        settings.setValue( IGDC_BADGES_SETTING_PATH, igdcBadgesCheckBox->isChecked() );
        emit igdcBadgesChanged( igdcBadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( IGDC_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcAliasesEdit->text() )
    {
        settings.setValue( IGDC_ALIASES_SETTING_PATH, igdcAliasesEdit->text() );
        emit igdcAliasesChanged( igdcAliasesEdit->text() );
    }

    oldStringValue = settings.value( IGDC_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcSupportersListEdit->toPlainText() )
    {
        settings.setValue( IGDC_SUPPORTERS_LIST_SETTING_PATH, igdcSupportersListEdit->toPlainText() );
        emit igdcSupportersListChanged( igdcSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( IGDC_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcBlackListEdit->toPlainText() )
    {
        settings.setValue( IGDC_BLACK_LIST_SETTING_PATH, igdcBlackListEdit->toPlainText() );
        emit igdcBlackListChanged( igdcBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != igdcRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, igdcRemoveBlackListUsersCheckBox->isChecked() );
        emit igdcRemoveBlackListUsersChanged( igdcRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки livecoding

    bool isLivecodingChannelChanged = false;

    oldBoolValue = settings.value( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( LIVECODING_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != livecodingChannelCheckBox->isChecked() || oldStringValue != livecodingChannelEdit->text() )
    {
        settings.setValue( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, livecodingChannelCheckBox->isChecked() );
        settings.setValue( LIVECODING_CHANNEL_SETTING_PATH, livecodingChannelEdit->text() );

        isLivecodingChannelChanged = true;
    }

    oldBoolValue = settings.value( LIVECODING_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != livecodingBadgesCheckBox->isChecked() )
    {
        settings.setValue( LIVECODING_BADGES_SETTING_PATH, livecodingBadgesCheckBox->isChecked() );
        emit livecodingBadgesChanged( livecodingBadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( LIVECODING_LOGIN_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != livecodingLoginEdit->text() )
    {
        settings.setValue( LIVECODING_LOGIN_SETTING_PATH, livecodingLoginEdit->text() );
        isLivecodingChannelChanged = true;
    }

    oldStringValue = settings.value( LIVECODING_PASSWORD_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != livecodingPasswordEdit->text() )
    {
        settings.setValue( LIVECODING_PASSWORD_SETTING_PATH, livecodingPasswordEdit->text() );
        isLivecodingChannelChanged = true;
    }

    if( isLivecodingChannelChanged )
        emit livecodingChannelChanged();

    oldStringValue = settings.value( LIVECODING_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != livecodingAliasesEdit->text() )
    {
        settings.setValue( LIVECODING_ALIASES_SETTING_PATH, livecodingAliasesEdit->text() );
        emit livecodingAliasesChanged( livecodingAliasesEdit->text() );
    }

    oldStringValue = settings.value( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != livecodingSupportersListEdit->toPlainText() )
    {
        settings.setValue( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, livecodingSupportersListEdit->toPlainText() );
        emit livecodingSupportersListChanged( livecodingSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( LIVECODING_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != livecodingBlackListEdit->toPlainText() )
    {
        settings.setValue( LIVECODING_BLACK_LIST_SETTING_PATH, livecodingBlackListEdit->toPlainText() );
        emit livecodingBlackListChanged( livecodingBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != livecodingRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, livecodingRemoveBlackListUsersCheckBox->isChecked() );
        emit livecodingRemoveBlackListUsersChanged( livecodingRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки sc2tv

    oldBoolValue = settings.value( SC2TV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( SC2TV_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != sc2tvChannelCheckBox->isChecked() || oldStringValue != sc2tvChannelEdit->text() )
    {
        settings.setValue( SC2TV_CHANNEL_ENABLE_SETTING_PATH, sc2tvChannelCheckBox->isChecked() );
        settings.setValue( SC2TV_CHANNEL_SETTING_PATH, sc2tvChannelEdit->text() );

        emit sc2tvChannelChanged();
    }

    oldBoolValue = settings.value( SC2TV_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != sc2tvOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( SC2TV_ORIGINAL_COLORS_SETTING_PATH, sc2tvOriginalColorsCheckBox->isChecked() );
        emit sc2tvOriginalColorsChanged( sc2tvOriginalColorsCheckBox->isChecked() );
    }

    oldStringValue = settings.value( SC2TV_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != sc2tvAliasesEdit->text() )
    {
        settings.setValue( SC2TV_ALIASES_SETTING_PATH, sc2tvAliasesEdit->text() );
        emit sc2tvAliasesChanged( sc2tvAliasesEdit->text() );
    }

    oldStringValue = settings.value( SC2TV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != sc2tvSupportersListEdit->toPlainText() )
    {
        settings.setValue( SC2TV_SUPPORTERS_LIST_SETTING_PATH, sc2tvSupportersListEdit->toPlainText() );
        emit sc2tvSupportersListChanged( sc2tvSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( SC2TV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != sc2tvBlackListEdit->toPlainText() )
    {
        settings.setValue( SC2TV_BLACK_LIST_SETTING_PATH, sc2tvBlackListEdit->toPlainText() );
        emit sc2tvBlackListChanged( sc2tvBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != sc2tvRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, sc2tvRemoveBlackListUsersCheckBox->isChecked() );
        emit sc2tvRemoveBlackListUsersChanged( sc2tvRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки streamcube

    oldBoolValue = settings.value( STREAMCUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( STREAMCUBE_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != streamcubeChannelCheckBox->isChecked() || oldStringValue != streamcubeChannelEdit->text() )
    {
        settings.setValue( STREAMCUBE_CHANNEL_ENABLE_SETTING_PATH, streamcubeChannelCheckBox->isChecked() );
        settings.setValue( STREAMCUBE_CHANNEL_SETTING_PATH, streamcubeChannelEdit->text() );

        emit streamcubeChannelChanged();
    }

    oldStringValue = settings.value( STREAMCUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamcubeAliasesEdit->text() )
    {
        settings.setValue( STREAMCUBE_ALIASES_SETTING_PATH, streamcubeAliasesEdit->text() );
        emit streamcubeAliasesChanged( streamcubeAliasesEdit->text() );
    }

    oldStringValue = settings.value( STREAMCUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamcubeSupportersListEdit->toPlainText() )
    {
        settings.setValue( STREAMCUBE_SUPPORTERS_LIST_SETTING_PATH, streamcubeSupportersListEdit->toPlainText() );
        emit streamcubeSupportersListChanged( streamcubeSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( STREAMCUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamcubeBlackListEdit->toPlainText() )
    {
        settings.setValue( STREAMCUBE_BLACK_LIST_SETTING_PATH, streamcubeBlackListEdit->toPlainText() );
        emit streamcubeBlackListChanged( streamcubeBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( STREAMCUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != streamcubeRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( STREAMCUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, streamcubeRemoveBlackListUsersCheckBox->isChecked() );
        emit streamcubeRemoveBlackListUsersChanged( streamcubeRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки twitch

    oldBoolValue = settings.value( TWITCH_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( TWITCH_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != twitchChannelCheckBox->isChecked() || oldStringValue != twitchChannelEdit->text() )
    {
        settings.setValue( TWITCH_CHANNEL_ENABLE_SETTING_PATH, twitchChannelCheckBox->isChecked() );
        settings.setValue( TWITCH_CHANNEL_SETTING_PATH, twitchChannelEdit->text() );

        emit twitchChannelChanged();
    }


    oldBoolValue = settings.value( TWITCH_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != twitchOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( TWITCH_ORIGINAL_COLORS_SETTING_PATH, twitchOriginalColorsCheckBox->isChecked() );
        emit twitchOriginalColorsChanged( twitchOriginalColorsCheckBox->isChecked() );
    }

    oldBoolValue = settings.value( TWITCH_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != twitchBadgesCheckBox->isChecked() )
    {
        settings.setValue( TWITCH_BADGES_SETTING_PATH, twitchBadgesCheckBox->isChecked() );
        emit twitchBadgesChanged( twitchBadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( TWITCH_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != twitchAliasesEdit->text() )
    {
        settings.setValue( TWITCH_ALIASES_SETTING_PATH, twitchAliasesEdit->text() );
        emit twitchAliasesChanged( twitchAliasesEdit->text() );
    }

    oldStringValue = settings.value( TWITCH_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != twitchSupportersListEdit->toPlainText() )
    {
        settings.setValue( TWITCH_SUPPORTERS_LIST_SETTING_PATH, twitchSupportersListEdit->toPlainText() );
        emit twitchSupportersListChanged( twitchSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( TWITCH_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != twitchBlackListEdit->toPlainText() )
    {
        settings.setValue( TWITCH_BLACK_LIST_SETTING_PATH, twitchBlackListEdit->toPlainText() );
        emit twitchBlackListChanged( twitchBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != twitchRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, twitchRemoveBlackListUsersCheckBox->isChecked() );
        emit twitchRemoveBlackListUsersChanged( twitchRemoveBlackListUsersCheckBox->isChecked() );
    }


    //настройки vidi

    oldBoolValue = settings.value( VIDI_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    oldStringValue = settings.value( VIDI_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != vidiChannelCheckBox->isChecked() || oldStringValue != vidiChannelEdit->text() )
    {
        settings.setValue( VIDI_CHANNEL_ENABLE_SETTING_PATH, vidiChannelCheckBox->isChecked() );
        settings.setValue( VIDI_CHANNEL_SETTING_PATH, vidiChannelEdit->text() );

        emit vidiChannelChanged();
    }

    oldStringValue = settings.value( VIDI_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != vidiAliasesEdit->text() )
    {
        settings.setValue( VIDI_ALIASES_SETTING_PATH, vidiAliasesEdit->text() );
        emit vidiAliasesChanged( vidiAliasesEdit->text() );
    }

    oldStringValue = settings.value( VIDI_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != vidiSupportersListEdit->toPlainText() )
    {
        settings.setValue( VIDI_SUPPORTERS_LIST_SETTING_PATH, vidiSupportersListEdit->toPlainText() );
        emit vidiSupportersListChanged( vidiSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( VIDI_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != vidiBlackListEdit->toPlainText() )
    {
        settings.setValue( VIDI_BLACK_LIST_SETTING_PATH, vidiBlackListEdit->toPlainText() );
        emit vidiBlackListChanged( vidiBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( VIDI_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != vidiRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( VIDI_REMOVE_BLACK_LIST_USERS_SETTING_PATH, vidiRemoveBlackListUsersCheckBox->isChecked() );
        emit vidiRemoveBlackListUsersChanged( vidiRemoveBlackListUsersCheckBox->isChecked() );
    }

}

void QSettingsDialog::setColorButtonStyle( QPushButton * button, QRgb color, int fontSize )
{
    QColor rgbaColor = QColor::fromRgba( color );

    QString newButtonStyle( "QPushButton{ background-color:rgba(" +
                           QString::number( rgbaColor.red() ) + "," +
                           QString::number( rgbaColor.green() ) + "," +
                            QString::number( rgbaColor.blue() ) + "," +
                           QString::number( rgbaColor.alpha() / 255.0, 'f' ) + ");" +
                           "border: 1px solid rgba(128,128,128,0.5); font-size: " + QString::number( fontSize ) + "px;}" );

    button->setStyleSheet( newButtonStyle );
}

void QSettingsDialog::addLayoutRow( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets )
{
    QHBoxLayout * rowLayout = new QHBoxLayout();

    for( auto widget : widgets )
    {
        rowLayout->addWidget( widget );
    }

    layout->addLayout( rowLayout );
}

void QSettingsDialog::addLayoutRow( QVBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts )
{
    QHBoxLayout * rowLayout = new QHBoxLayout();

    for( auto layout : layouts )
    {
        rowLayout->addLayout( layout );
    }

    layout->addLayout( rowLayout );
}


void QSettingsDialog::addWidgets( QVBoxLayout * layout, std::initializer_list< QWidget * > widgets )
{    
    for( auto widget : widgets )
    {
        layout->addWidget( widget );
    }
}

void QSettingsDialog::addLayouts( QHBoxLayout * layout, std::initializer_list< QVBoxLayout * > layouts )
{
    for( auto vboxLayout : layouts )
    {
        layout->addLayout( vboxLayout );
    }
}

void QSettingsDialog::buttonColorSelection( QPushButton * button, QRgb & color )
{
    QColor newColor = QColorDialog::getColor( QColor::fromRgba( color ), this, QString(), QColorDialog::ShowAlphaChannel );

    if( newColor.isValid() && newColor.rgba() != color  )
    {
        color = newColor.rgba();
        setColorButtonStyle( button, color );
    }        
}

void QSettingsDialog::bodyBackgroundColorSelection()
{
    buttonColorSelection( bodyBackgroundColorButton, bodyBackgroundColor );
}


void QSettingsDialog::evenMessagesNicknamesColorSelection()
{
    buttonColorSelection( evenMessagesNicknamesColorButton, evenMessagesNicknamesColor );
}

void QSettingsDialog::oddMessagesNicknamesColorSelection()
{
    buttonColorSelection( oddMessagesNicknamesColorButton, oddMessagesNicknamesColor );
}

void QSettingsDialog::aliasesMessagesNicknamesColorSelection()
{
    buttonColorSelection( aliasesNicknamesColorButton, aliasesNicknamesColor );
}

void QSettingsDialog::supportersMessagesNicknamesColorSelection()
{
    buttonColorSelection( supportersNicknamesColorButton, supportersNicknamesColor );
}

void QSettingsDialog::blackListMessagesNicknamesColorSelection()
{
    buttonColorSelection( blackListNicknamesColorButton, blackListNicknamesColor );
}


void QSettingsDialog::evenMessagesBackgroundColorSelection()
{
    buttonColorSelection( evenMessagesBackgroundColorButton, evenMessagesBackgroundColor );
}

void QSettingsDialog::oddMessagesBackgroundColorSelection()
{
    buttonColorSelection( oddMessagesBackgroundColorButton, oddMessagesBackgroundColor );
}

void QSettingsDialog::aliasesBackgroundColorSelection()
{
    buttonColorSelection( aliasesBackgroundColorButton, aliasesBackgroundColor );
}

void QSettingsDialog::supportersBackgroundColorSelection()
{
    buttonColorSelection( supportersBackgroundColorButton, supportersBackgroundColor );
}

void QSettingsDialog::blackListBackgroundColorSelection()
{
    buttonColorSelection( blackListBackgroundColorButton, blackListBackgroundColor );
}


void QSettingsDialog::evenMessagesTextColorSelection()
{
    buttonColorSelection( evenMessagesTextColorButton, evenMessagesTextColor );
}

void QSettingsDialog::oddMessagesTextColorSelection()
{
    buttonColorSelection( oddMessagesTextColorButton, oddMessagesTextColor );
}

void QSettingsDialog::aliasesMessagesTextColorSelection()
{
    buttonColorSelection( aliasesMessagesTextColorButton, aliasesMessagesTextColor );
}

void QSettingsDialog::supportersMessagesTextColorSelection()
{
    buttonColorSelection( supportersMessagesTextColorButton, supportersMessagesTextColor );
}

void QSettingsDialog::blackListMessagesTextColorSelection()
{
    buttonColorSelection( blackListMessagesTextColorButton, blackListMessagesTextColor );
}

void QSettingsDialog::evenMessagesBorderColorSelection()
{
    buttonColorSelection( evenMessagesBorderColorButton, evenMessagesBorderColor );
}

void QSettingsDialog::oddMessagesBorderColorSelection()
{
    buttonColorSelection( oddMessagesBorderColorButton, oddMessagesBorderColor );
}

void QSettingsDialog::aliasesMessagesBorderColorSelection()
{
    buttonColorSelection( aliasesMessagesBorderColorButton, aliasesMessagesBorderColor );
}

void QSettingsDialog::supportersMessagesBorderColorSelection()
{
    buttonColorSelection( supportersMessagesBorderColorButton, supportersMessagesBorderColor );
}

void QSettingsDialog::blackListMessagesBorderColorSelection()
{
    buttonColorSelection( blackListMessagesBorderColorButton, blackListMessagesBorderColor );
}

void QSettingsDialog::viewersColorSelection()
{
    buttonColorSelection( viewersTextColorButton, viewersTextColor );
}

void QSettingsDialog::viewersBackgroundColorSelection()
{
    buttonColorSelection( viewersBackgroundColorButton, viewersBackgroundColor );
}

void QSettingsDialog::linksColorSelection()
{
    buttonColorSelection( linksColorButton, linksColor );
}

void QSettingsDialog::loadDialogSettings()
{
    QSettings settings;

    const int TEST_VALUE = -1;

    int width = settings.value( "/Settings/SettingsWindow/Width", TEST_VALUE ).toInt();
    int height = settings.value( "/Settings/SettingsWindow/Height", TEST_VALUE ).toInt();

    int xPos = settings.value( "/Settings/SettingsWindow/X", TEST_VALUE ).toInt();
    int yPos = settings.value( "/Settings/SettingsWindow/Y", TEST_VALUE ).toInt();

    if( xPos != TEST_VALUE && yPos != TEST_VALUE )
        move( xPos, yPos );
    if( width != TEST_VALUE && height != TEST_VALUE )
        resize( width, height );
}

void QSettingsDialog::saveDialogSettings()
{
    QSettings settings;

    settings.setValue( "/Settings/SettingsWindow/X", pos().x() );
    settings.setValue( "/Settings/SettingsWindow/Y", pos().y() );
    settings.setValue( "/Settings/SettingsWindow/Width", width() );
    settings.setValue( "/Settings/SettingsWindow/Height", height() );
}

void QSettingsDialog::setupSpinBox( QSpinBox * spinBox, int min, int max, int defaultValue, const QString & settingPath )
{
    QSettings settings;

    spinBox->setRange( min, max );
    spinBox->setValue( settings.value( settingPath, defaultValue ).toInt() );
}

void QSettingsDialog::setupColorButton( QPushButton * button, QRgb & color, unsigned int defaultColor, const QString & settingPath  )
{
    QSettings settings;

    button->setFlat( true );
    color = settings.value( settingPath, defaultColor ).toUInt();
    setColorButtonStyle( button, color );
}

void QSettingsDialog::onExportDefaultThemeClick()
{

}

//TODO: retranslate

