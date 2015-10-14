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

#include "settingsconsts.h"

#include "qsettingsdialog.h"

QSettingsDialog::QSettingsDialog( QWidget *parent )
: QDialog( parent/*, Qt::WindowStaysOnTopHint*/ )
, nicknamesTextColor()
, messagesTextColor()
, viewersTextColor()
, aliasesTextColor()
, languageCombo( new QComboBox( this ) )
, stayOnTopCheckBox( new QCheckBox( this ) )
, frameLessWindowCheckBox( new QCheckBox( this ) )
//, transparentWindowCheckBox( new QCheckBox( this ) )
, styleCombo( new QComboBox( this ) )
, opacitySlider( new QSlider( this ) )


, showSystemMessagesCheckBox( new QCheckBox( this ) )

, showImagesCheckBox( new QCheckBox( this ) )

, useServerCheckBox( new QCheckBox( this ) )
, serverPortSpinBox( new QSpinBox( this ) )

, saveToFileCheckBox( new QCheckBox( this ) )
, saveToFileIntervalSpinBox( new QSpinBox( this ) )


, fontNameCombo( new QComboBox( this ) )

, nicknamesFontSizeSpinBox( new QSpinBox( this ) )
, nicknamesTextColorButton( new QPushButton( this ) )
, messagesFontSizeSpinBox( new QSpinBox( this ) )
, messagesTextColorButton( new QPushButton( this ) )
, viewersFontSizeSpinBox( new QSpinBox( this ) )
, viewersTextColorButton( new QPushButton( this ) )
, aliasesFontSizeSpinBox( new QSpinBox( this ) )
, aliasesTextColorButton( new QPushButton( this ) )

, smilesSizeSpinBox( new QSpinBox( this ) )
, serviceIconsSizeSpinBox( new QSpinBox( this ) )

, animationTypeCombo( new QComboBox( this ) )


, acesChannelCheckBox( new QCheckBox( this ) )
, acesChannelEdit( new QLineEdit( this ) )
//, acesOriginalColorsCheckBox( new QCheckBox( this ) )
, acesAliasesEdit( new QLineEdit( this ) )
, acesSupportersListEdit( new QTextEdit( this ) )
, acesBlackListEdit( new QTextEdit( this ) )
, acesRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
, cyberGameChannelCheckBox( new QCheckBox( this ) )
, cyberGameChannelEdit( new QLineEdit( this ) )
, cyberGameAliasesEdit( new QLineEdit( this ) )
, cyberGameSupportersListEdit( new QTextEdit( this ) )
, cyberGameBlackListEdit( new QTextEdit( this ) )
, cyberGameRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, funstreamChannelCheckBox( new QCheckBox( this ) )
, funstreamChannelEdit( new QLineEdit( this ) )
, funstreamAliasesEdit( new QLineEdit( this ) )
, funstreamSupportersListEdit( new QTextEdit( this ) )
, funstreamBlackListEdit( new QTextEdit( this ) )
, funstreamRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, gamerstvChannelCheckBox( new QCheckBox( this ) )
, gamerstvChannelEdit( new QLineEdit( this ) )
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


, livecodingChannelCheckBox( new QCheckBox( this ) )
, livecodingChannelEdit( new QLineEdit( this ) )
, livecodingAliasesEdit( new QLineEdit( this ) )
, livecodingSupportersListEdit( new QTextEdit( this ) )
, livecodingBlackListEdit( new QTextEdit( this ) )
, livecodingRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, realltvChannelCheckBox( new QCheckBox( this ) )
, realltvChannelEdit( new QLineEdit( this ) )
, realltvAliasesEdit( new QLineEdit( this ) )
, realltvSupportersListEdit( new QTextEdit( this ) )
, realltvBlackListEdit( new QTextEdit( this ) )
, realltvRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, sc2tvChannelCheckBox( new QCheckBox( this ) )
, sc2tvChannelEdit( new QLineEdit( this ) )
, sc2tvOriginalColorsCheckBox( new QCheckBox( this ) )
, sc2tvAliasesEdit( new QLineEdit( this ) )
, sc2tvSupportersListEdit( new QTextEdit( this ) )
, sc2tvBlackListEdit( new QTextEdit( this ) )
, sc2tvRemoveBlackListUsersCheckBox( new QCheckBox( this ) )


, streamboxChannelCheckBox( new QCheckBox( this ) )
, streamboxChannelEdit( new QLineEdit( this ) )
, streamboxAliasesEdit( new QLineEdit( this ) )
, streamboxSupportersListEdit( new QTextEdit( this ) )
, streamboxBlackListEdit( new QTextEdit( this ) )
, streamboxRemoveBlackListUsersCheckBox( new QCheckBox( this ) )



, twitchChannelCheckBox( new QCheckBox( this ) )
, twitchChannelEdit( new QLineEdit( this ) )
, twitchAliasesEdit( new QLineEdit( this ) )
, twitchSupportersListEdit( new QTextEdit( this ) )
, twitchBlackListEdit( new QTextEdit( this ) )
, twitchRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

, youtubeChannelCheckBox( new QCheckBox( this ) )
, youtubeChannelEdit( new QLineEdit( this ) )
, youtubeAliasesEdit( new QLineEdit( this ) )
, youtubeSupportersListEdit( new QTextEdit( this ) )
, youtubeBlackListEdit( new QTextEdit( this ) )
, youtubeRemoveBlackListUsersCheckBox( new QCheckBox( this ) )

{
    //setMinimumWidth( 960 );
    //setMinimumHeight( 640 );

    setWindowTitle( tr( "BroChat Settings" ) );
    setModal( true );

    const QString CHANNEL_TEXT = tr( "Channel:" );
    const QString ALIASES_TEXT = tr( "Aliases:" );
    const QString SUPPORTERS_TEXT = tr( "Supporters:" );
    const QString BLACKLIST_TEXT = tr( "Black List:" );
    const QString REMOVE_BLACKLIST_USERS_MESSAGES = tr( "Remove Black List Users Messages" );



    QSettings settings;

    QVBoxLayout* mainLayout = new QVBoxLayout( this );
    QTabWidget* tabSettings = new QTabWidget( this );

    //настройки окна
    QVBoxLayout *windowLayout = new QVBoxLayout;

    //QLabel *stayOnTopLabel = new QLabel( tr( "Stay on top" ) );

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

    //QStringList styleFiles = stylesDir.entryList( QStringList( "*.css" ), QDir::Files | QDir::NoSymLinks );
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

    //TODO: NEW CODE WITHOUT IMPLEMENTATION

    showSystemMessagesCheckBox->setText( "Show System Messages" );
    showSystemMessagesCheckBox->setChecked( settings.value( SHOW_SYSTEM_MESSAGES_SETTING_PATH, DEFAULT_SHOW_SYSTEM_MESSAGES ).toBool() );

    windowLayout->addWidget( showSystemMessagesCheckBox );

    showImagesCheckBox->setText( tr( "Show Images" ) );
    showImagesCheckBox->setChecked( settings.value( SHOW_IMAGES_SETTING_PATH, DEFAULT_SHOW_IMAGES ).toBool() );

    windowLayout->addWidget( showImagesCheckBox );

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

    //END NEW CODE

    windowLayout->addStretch( 1 );

    QGroupBox *windowGroup = new QGroupBox( tabSettings );
    windowGroup->setLayout( windowLayout );

    tabSettings->addTab( windowGroup, QIcon( ":/resources/broico.ico" ), tr( "Window" ) );

    //настройки стиля по-умолчанию

    QFontDatabase fontDataBase;

    QVBoxLayout *defaultStyleLayout = new QVBoxLayout;

    QLabel *fontNameLabel = new QLabel( tr( "Font Name:" ) );

    fontNameCombo->addItems( fontDataBase.families() );
    fontNameCombo->setEditable( false );
    int index = fontNameCombo->findText( settings.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() );
    fontNameCombo->setCurrentIndex( index );

    defaultStyleLayout->addWidget( fontNameLabel );
    defaultStyleLayout->addWidget( fontNameCombo );


    QLabel *nicknamesFontSizeLabel = new QLabel( tr( "Nicknames Font Size:" ) );
    nicknamesFontSizeSpinBox->setRange( 1, 100 );
    nicknamesFontSizeSpinBox->setValue( settings.value( GENERATED_STYLE_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ). toInt() );

    defaultStyleLayout->addWidget( nicknamesFontSizeLabel );
    defaultStyleLayout->addWidget( nicknamesFontSizeSpinBox );


    QLabel *nicknamesTextColorTextLabel = new QLabel( tr( "Nicknames Text Color:" ) );
    nicknamesTextColorButton->setFlat( true );
    nicknamesTextColor = settings.value( GENERATED_STYLE_NICKNAMES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_TEXT_COLOR ).toUInt();
    setColorButtonStyle( nicknamesTextColorButton, nicknamesTextColor );
    QObject::connect( nicknamesTextColorButton, SIGNAL( clicked() ), this, SLOT( nicknamesColorSelection() ) );

    defaultStyleLayout->addWidget( nicknamesTextColorTextLabel );
    defaultStyleLayout->addWidget( nicknamesTextColorButton );


    QLabel *messagesFontSizeLabel = new QLabel( tr( "Messages Font Size:" ) );
    messagesFontSizeSpinBox->setRange( 1, 100 );
    messagesFontSizeSpinBox->setValue( settings.value( GENERATED_STYLE_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() );

    defaultStyleLayout->addWidget( messagesFontSizeLabel );
    defaultStyleLayout->addWidget( messagesFontSizeSpinBox );


    QLabel *messagesTextColorTextLabel = new QLabel( tr( "Messages Text Color:" ) );
    messagesTextColorButton->setFlat( true );
    messagesTextColor = settings.value( GENERATED_STYLE_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt();
    setColorButtonStyle( messagesTextColorButton, messagesTextColor );
    QObject::connect( messagesTextColorButton, SIGNAL( clicked() ), this, SLOT( messagesColorSelection() ) );

    defaultStyleLayout->addWidget( messagesTextColorTextLabel );
    defaultStyleLayout->addWidget( messagesTextColorButton );


    QLabel *viewersFontSizeLabel = new QLabel( tr( "Viewers Font Size:" ) );
    viewersFontSizeSpinBox->setRange( 1, 100 );
    viewersFontSizeSpinBox->setValue( settings.value( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE ).toInt() );

    defaultStyleLayout->addWidget( viewersFontSizeLabel );
    defaultStyleLayout->addWidget( viewersFontSizeSpinBox );


    QLabel *viewersTextColorTextLabel = new QLabel( tr( "Viewers Text Color:" ) );
    viewersTextColorButton->setFlat( true );
    viewersTextColor = settings.value( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR ).toUInt();
    setColorButtonStyle( viewersTextColorButton, viewersTextColor );
    QObject::connect( viewersTextColorButton, SIGNAL( clicked() ), this, SLOT( viewersColorSelection() ) );

    defaultStyleLayout->addWidget( viewersTextColorTextLabel );
    defaultStyleLayout->addWidget( viewersTextColorButton );

    QLabel *aliasesFontSizeLabel = new QLabel( tr( "Aliases Font Size:" ) );
    aliasesFontSizeSpinBox->setRange( 1, 100 );
    aliasesFontSizeSpinBox->setValue( settings.value( GENERATED_STYLE_ALIASES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_FONT_SIZE ).toInt() );

    defaultStyleLayout->addWidget( aliasesFontSizeLabel );
    defaultStyleLayout->addWidget( aliasesFontSizeSpinBox );

    QLabel *aliasesTextColorTextLabel = new QLabel( tr( "Aliases Text Color:" ) );
    aliasesTextColorButton->setFlat( true );
    aliasesTextColor = settings.value( GENERATED_STYLE_ALIASES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_TEXT_COLOR ).toUInt();
    setColorButtonStyle( aliasesTextColorButton, aliasesTextColor );
    QObject::connect( aliasesTextColorButton, SIGNAL( clicked() ), this, SLOT( aliasesColorSelection() ) );

    defaultStyleLayout->addWidget( aliasesTextColorTextLabel );
    defaultStyleLayout->addWidget( aliasesTextColorButton );

    QLabel *smilesSizeLabel = new QLabel( tr( "Smiles Size:" ) );
    smilesSizeSpinBox->setRange( 1, 100 );
    smilesSizeSpinBox->setValue( settings.value( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SMILES_SIZE ).toInt() );

    defaultStyleLayout->addWidget( smilesSizeLabel );
    defaultStyleLayout->addWidget( smilesSizeSpinBox );


    QLabel *serviceIconsSizeLabel = new QLabel( tr( "Service Icons Size:" ) );
    serviceIconsSizeSpinBox->setRange( 1, 100 );
    serviceIconsSizeSpinBox->setValue( settings.value( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE ).toInt() );

    defaultStyleLayout->addWidget( serviceIconsSizeLabel );
    defaultStyleLayout->addWidget( serviceIconsSizeSpinBox );


    QLabel *animationTypeLabel = new QLabel( tr( "Animation type:" ) );

    QStringList animationsList;

    animationsList << "None" << "Scale" << "Slide" ;

    animationTypeCombo->addItems( animationsList );
    animationTypeCombo->setEditable( false );
    animationTypeCombo->setCurrentIndex( animationTypeCombo->findText( settings.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE ).toString() ) );

    defaultStyleLayout->addWidget( animationTypeLabel );
    defaultStyleLayout->addWidget( animationTypeCombo );

    defaultStyleLayout->addStretch( 1 );

    QGroupBox *defaultStyleGroup = new QGroupBox;
    defaultStyleGroup->setLayout( defaultStyleLayout );

    tabSettings->addTab( defaultStyleGroup, QIcon( ":/resources/broico.ico" ), tr( "Default Style" ) );

    //настройки aces

    QVBoxLayout *acesLayout = new QVBoxLayout;

    //QLabel *acesChannelLabel = new QLabel( CHANNEL_TEXT );
    acesChannelCheckBox->setText( CHANNEL_TEXT );
    acesChannelCheckBox->setChecked( settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    acesChannelEdit->setText( settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString() );
    acesChannelEdit->setEnabled( acesChannelCheckBox->isChecked() );

    QObject::connect( acesChannelCheckBox, SIGNAL( clicked( bool ) ), acesChannelEdit, SLOT( setEnabled( bool ) ) );

    //acesLayout->addWidget( acesChannelLabel );
    acesLayout->addWidget( acesChannelCheckBox );
    acesLayout->addWidget( acesChannelEdit );

    //acesOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    //acesOriginalColorsCheckBox->setChecked( settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    //acesLayout->addWidget( acesOriginalColorsCheckBox );

    QLabel *acesAliasesLabel  = new QLabel( ALIASES_TEXT );
    acesAliasesEdit->setText( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    acesLayout->addWidget( acesAliasesLabel );
    acesLayout->addWidget( acesAliasesEdit );

    QLabel *acesSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    acesSupportersListEdit->setText( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    acesLayout->addWidget( acesSupportersListLabel );
    acesLayout->addWidget( acesSupportersListEdit );

    QLabel *acesBlackListLabel = new QLabel( BLACKLIST_TEXT );

    acesBlackListEdit->setText( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    acesLayout->addWidget( acesBlackListLabel );
    acesLayout->addWidget( acesBlackListEdit );

    acesRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    acesRemoveBlackListUsersCheckBox->setChecked( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    acesLayout->addWidget( acesRemoveBlackListUsersCheckBox );

    acesLayout->addStretch( 1 );

    QGroupBox *acesGroup = new QGroupBox( tabSettings );
    acesGroup->setLayout( acesLayout );

    tabSettings->addTab( acesGroup, QIcon( ":/resources/aceslogo.png" ), tr( "Aces" ) );

    //настройки cybergame

    QVBoxLayout *cyberGameLayout = new QVBoxLayout;

    //QLabel *cyberGameChannelLabel = new QLabel( CHANNEL_TEXT );

    cyberGameChannelCheckBox->setText( CHANNEL_TEXT );
    cyberGameChannelCheckBox->setChecked( settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    cyberGameChannelEdit->setText( settings.value( CYBERGAME_CHANNEL_SETTING_PATH, DEFAULT_CYBERGAME_CHANNEL_NAME ).toString() );
    cyberGameChannelEdit->setEnabled( cyberGameChannelCheckBox->isChecked() );

    QObject::connect( cyberGameChannelCheckBox, SIGNAL( clicked(bool) ), cyberGameChannelEdit, SLOT( setEnabled( bool ) ) );

    //cyberGameLayout->addWidget( cyberGameChannelLabel );
    cyberGameLayout->addWidget( cyberGameChannelCheckBox );
    cyberGameLayout->addWidget( cyberGameChannelEdit );

    QLabel *cyberGameAliasesLabel = new QLabel( ALIASES_TEXT );
    cyberGameAliasesEdit->setText( settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    cyberGameLayout->addWidget( cyberGameAliasesLabel );
    cyberGameLayout->addWidget( cyberGameAliasesEdit );

    QLabel *cyberGameSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    cyberGameSupportersListEdit->setText( settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    cyberGameLayout->addWidget( cyberGameSupportersListLabel );
    cyberGameLayout->addWidget( cyberGameSupportersListEdit );

    QLabel *cyberGameBlackListLabel = new QLabel( BLACKLIST_TEXT );

    cyberGameBlackListEdit->setText( settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    cyberGameLayout->addWidget( cyberGameBlackListLabel );
    cyberGameLayout->addWidget( cyberGameBlackListEdit );

    cyberGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    cyberGameRemoveBlackListUsersCheckBox->setChecked( settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    cyberGameLayout->addWidget( cyberGameRemoveBlackListUsersCheckBox );

    cyberGameLayout->addStretch( 1 );

    QGroupBox *cyberGameGroup = new QGroupBox( tabSettings );
    cyberGameGroup->setLayout( cyberGameLayout );

    tabSettings->addTab( cyberGameGroup, QIcon( ":/resources/cybergamelogo.png" ), tr( "Cybergame" ) );



    //настройки funstream
    QVBoxLayout *funstreamLayout = new QVBoxLayout;

    //QLabel *funstreamChannelLabel = new QLabel( CHANNEL_TEXT );

    funstreamChannelCheckBox->setText( CHANNEL_TEXT );
    funstreamChannelCheckBox->setChecked( settings.value( FUNSTREAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    funstreamChannelEdit->setText( settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, DEFAULT_FUNSTREAM_CHANNEL_NAME ).toString() );
    funstreamChannelEdit->setEnabled( funstreamChannelCheckBox->isChecked() );

    QObject::connect( funstreamChannelCheckBox, SIGNAL( clicked( bool ) ), funstreamChannelEdit, SLOT( setEnabled( bool ) ) );

    //funstreamLayout->addWidget( funstreamChannelLabel );
    funstreamLayout->addWidget( funstreamChannelCheckBox );
    funstreamLayout->addWidget( funstreamChannelEdit );

    QLabel *funstreamAliasesLabel = new QLabel( ALIASES_TEXT );

    funstreamAliasesEdit->setText( settings.value( FUNSTREAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    funstreamLayout->addWidget( funstreamAliasesLabel );
    funstreamLayout->addWidget( funstreamAliasesEdit );

    QLabel *funstreamSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    funstreamSupportersListEdit->setText( settings.value( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    funstreamLayout->addWidget( funstreamSupportersListLabel );
    funstreamLayout->addWidget( funstreamSupportersListEdit );

    QLabel *funstreamBlackListLabel = new QLabel( BLACKLIST_TEXT );
    funstreamBlackListEdit->setText( settings.value( FUNSTREAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    funstreamLayout->addWidget( funstreamBlackListLabel );
    funstreamLayout->addWidget( funstreamBlackListEdit );

    funstreamRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    funstreamRemoveBlackListUsersCheckBox->setChecked( settings.value( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    funstreamLayout->addWidget( funstreamRemoveBlackListUsersCheckBox );

    funstreamLayout->addStretch( 1 );

    QGroupBox *funstreamGroup = new QGroupBox( tabSettings );
    funstreamGroup->setLayout( funstreamLayout );

    tabSettings->addTab( funstreamGroup, QIcon( ":/resources/funstreamlogo.png" ), tr( "Funstream" ) );



    //настройки gamerstv
    QVBoxLayout *gamerstvLayout = new QVBoxLayout;

    //QLabel *gamerstvChannelLabel = new QLabel( CHANNEL_TEXT );

    gamerstvChannelCheckBox->setText( CHANNEL_TEXT );
    gamerstvChannelCheckBox->setChecked( settings.value( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    gamerstvChannelEdit->setText( settings.value( GAMERSTV_CHANNEL_SETTING_PATH, DEFAULT_GAMERSTV_CHANNEL_NAME ).toString() );
    gamerstvChannelEdit->setEnabled( gamerstvChannelCheckBox->isChecked() );

    QObject::connect( gamerstvChannelCheckBox, SIGNAL( clicked( bool ) ), gamerstvChannelEdit, SLOT( setEnabled( bool ) ) );

    //gamerstvLayout->addWidget( gamerstvChannelLabel );
    gamerstvLayout->addWidget( gamerstvChannelCheckBox );
    gamerstvLayout->addWidget( gamerstvChannelEdit );

    QLabel *gamerstvAliasesLabel = new QLabel( ALIASES_TEXT );

    gamerstvAliasesEdit->setText( settings.value( GAMERSTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    gamerstvLayout->addWidget( gamerstvAliasesLabel );
    gamerstvLayout->addWidget( gamerstvAliasesEdit );

    QLabel *gamerstvSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    gamerstvSupportersListEdit->setText( settings.value( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    gamerstvLayout->addWidget( gamerstvSupportersListLabel );
    gamerstvLayout->addWidget( gamerstvSupportersListEdit );

    QLabel *gamerstvBlackListLabel = new QLabel( BLACKLIST_TEXT );
    gamerstvBlackListEdit->setText( settings.value( GAMERSTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    gamerstvLayout->addWidget( gamerstvBlackListLabel );
    gamerstvLayout->addWidget( gamerstvBlackListEdit );

    gamerstvRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    gamerstvRemoveBlackListUsersCheckBox->setChecked( settings.value( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    gamerstvLayout->addWidget( gamerstvRemoveBlackListUsersCheckBox );

    gamerstvLayout->addStretch( 1 );

    QGroupBox *gamerstvGroup = new QGroupBox( tabSettings );
    gamerstvGroup->setLayout( gamerstvLayout );

    tabSettings->addTab( gamerstvGroup, QIcon( ":/resources/gamerstvlogo.png" ), tr( "Gamerstv" ) );


    //настройки gipsyteam
    QVBoxLayout *gipsyteamLayout = new QVBoxLayout;

    //QLabel *gipsyteamChannelLabel = new QLabel( CHANNEL_TEXT );

    gipsyteamChannelCheckBox->setText( CHANNEL_TEXT );
    gipsyteamChannelCheckBox->setChecked( settings.value( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    gipsyteamChannelEdit->setText( settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, DEFAULT_GIPSYTEAM_CHANNEL_NAME ).toString() );
    gipsyteamChannelEdit->setEnabled( gipsyteamChannelCheckBox->isChecked() );

    QObject::connect( gipsyteamChannelCheckBox, SIGNAL( clicked( bool ) ), gipsyteamChannelEdit, SLOT( setEnabled( bool ) ) );

    //gipsyteamLayout->addWidget( gipsyteamChannelLabel );
    gipsyteamLayout->addWidget( gipsyteamChannelCheckBox );
    gipsyteamLayout->addWidget( gipsyteamChannelEdit );

    QLabel *gipsyteamAliasesLabel = new QLabel( ALIASES_TEXT );

    gipsyteamAliasesEdit->setText( settings.value( GIPSYTEAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    gipsyteamLayout->addWidget( gipsyteamAliasesLabel );
    gipsyteamLayout->addWidget( gipsyteamAliasesEdit );

    QLabel *gipsyteamSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    gipsyteamSupportersListEdit->setText( settings.value( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    gipsyteamLayout->addWidget( gipsyteamSupportersListLabel );
    gipsyteamLayout->addWidget( gipsyteamSupportersListEdit );

    QLabel *gipsyteamBlackListLabel = new QLabel( BLACKLIST_TEXT );
    gipsyteamBlackListEdit->setText( settings.value( GIPSYTEAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    gipsyteamLayout->addWidget( gipsyteamBlackListLabel );
    gipsyteamLayout->addWidget( gipsyteamBlackListEdit );

    gipsyteamRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    gipsyteamRemoveBlackListUsersCheckBox->setChecked( settings.value( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    gipsyteamLayout->addWidget( gipsyteamRemoveBlackListUsersCheckBox );

    gipsyteamLayout->addStretch( 1 );

    QGroupBox *gipsyteamGroup = new QGroupBox( tabSettings );
    gipsyteamGroup->setLayout( gipsyteamLayout );

    tabSettings->addTab( gipsyteamGroup, QIcon( ":/resources/gipsyteamlogo.png" ), tr( "Gipsyteam" ) );



    //настройки goodgame

    QVBoxLayout *goodGameLayout = new QVBoxLayout;

    //QLabel *goodGameChannelLabel = new QLabel( CHANNEL_TEXT );

    goodGameChannelCheckBox->setText( CHANNEL_TEXT );
    goodGameChannelCheckBox->setChecked( settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    goodGameChannelEdit->setText( settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString() );
    goodGameChannelEdit->setEnabled( goodGameChannelCheckBox->isChecked() );

    QObject::connect( goodGameChannelCheckBox, SIGNAL( clicked( bool ) ), goodGameChannelEdit, SLOT( setEnabled( bool ) ) );

    //goodGameLayout->addWidget( goodGameChannelLabel );
    goodGameLayout->addWidget( goodGameChannelCheckBox );
    goodGameLayout->addWidget( goodGameChannelEdit );

    goodGameUseAnimatedSmilesCheckBox->setText( tr( "Use Animated Smiles" ) );
    goodGameUseAnimatedSmilesCheckBox->setChecked( settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameUseAnimatedSmilesCheckBox );

    QLabel *goodGameAliasesLabel = new QLabel( ALIASES_TEXT );
    goodGameAliasesEdit->setText( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameAliasesLabel );
    goodGameLayout->addWidget( goodGameAliasesEdit );

    QLabel *goodGameSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    goodGameSupportersListEdit->setText( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameSupportersListLabel );
    goodGameLayout->addWidget( goodGameSupportersListEdit );

    QLabel *goodGameBlackListLabel = new QLabel( BLACKLIST_TEXT );

    goodGameBlackListEdit->setText( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameBlackListLabel );
    goodGameLayout->addWidget( goodGameBlackListEdit );

    goodGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    goodGameRemoveBlackListUsersCheckBox->setChecked( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameRemoveBlackListUsersCheckBox );

    goodGameLayout->addStretch( 1 );

    QGroupBox *goodGameGroup = new QGroupBox( tabSettings );
    goodGameGroup->setLayout( goodGameLayout );

    tabSettings->addTab( goodGameGroup, QIcon( ":/resources/goodgamelogo.png" ), tr( "Goodgame" ) );


    //настройки hitbox
    QVBoxLayout *hitboxLayout = new QVBoxLayout;

    //QLabel *hitboxChannelLabel = new QLabel( CHANNEL_TEXT );

    hitboxChannelCheckBox->setText( CHANNEL_TEXT );
    hitboxChannelCheckBox->setChecked( settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    hitboxChannelEdit->setText( settings.value( HITBOX_CHANNEL_SETTING_PATH, DEFAULT_HITBOX_CHANNEL_NAME ).toString() );
    hitboxChannelEdit->setEnabled( hitboxChannelCheckBox->isChecked() );

    QObject::connect( hitboxChannelCheckBox, SIGNAL( clicked( bool ) ), hitboxChannelEdit, SLOT( setEnabled( bool ) ) );

    //hitboxLayout->addWidget( hitboxChannelLabel );
    hitboxLayout->addWidget( hitboxChannelCheckBox );
    hitboxLayout->addWidget( hitboxChannelEdit );

    hitboxOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    hitboxOriginalColorsCheckBox->setChecked( settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    hitboxLayout->addWidget( hitboxOriginalColorsCheckBox );

    QLabel *hitboxAliasesLabel = new QLabel( ALIASES_TEXT );
    hitboxAliasesEdit->setText( settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    hitboxLayout->addWidget( hitboxAliasesLabel );
    hitboxLayout->addWidget( hitboxAliasesEdit );

    QLabel *hitboxSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    hitboxSupportersListEdit->setText( settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    hitboxLayout->addWidget( hitboxSupportersListLabel );
    hitboxLayout->addWidget( hitboxSupportersListEdit );

    QLabel *hitboxBlackListLabel = new QLabel( BLACKLIST_TEXT );

    hitboxBlackListEdit->setText( settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    hitboxLayout->addWidget( hitboxBlackListLabel );
    hitboxLayout->addWidget( hitboxBlackListEdit );

    hitboxRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    hitboxRemoveBlackListUsersCheckBox->setChecked( settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    hitboxLayout->addWidget( hitboxRemoveBlackListUsersCheckBox );

    hitboxLayout->addStretch( 1 );

    QGroupBox *hitboxGroup = new QGroupBox( tabSettings );
    hitboxGroup->setLayout( hitboxLayout );

    tabSettings->addTab( hitboxGroup, QIcon( ":/resources/hitboxlogo.png" ), tr( "Hitbox" ) );



    //настройки livecoding
    QVBoxLayout *livecodingLayout = new QVBoxLayout;

    livecodingChannelCheckBox->setText( CHANNEL_TEXT );
    livecodingChannelCheckBox->setChecked( settings.value( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    livecodingChannelEdit->setText( settings.value( LIVECODING_CHANNEL_SETTING_PATH, DEFAULT_LIVECODING_CHANNEL_NAME ).toString() );
    livecodingChannelEdit->setEnabled( livecodingChannelCheckBox->isChecked() );

    QObject::connect( livecodingChannelCheckBox, SIGNAL( clicked( bool ) ), livecodingChannelEdit, SLOT( setEnabled( bool ) ) );

    livecodingLayout->addWidget( livecodingChannelCheckBox );
    livecodingLayout->addWidget( livecodingChannelEdit );

    QLabel *livecodingAliasesLabel = new QLabel( ALIASES_TEXT );

    livecodingAliasesEdit->setText( settings.value( LIVECODING_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    livecodingLayout->addWidget( livecodingAliasesLabel );
    livecodingLayout->addWidget( livecodingAliasesEdit );

    QLabel *livecodingSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    livecodingSupportersListEdit->setText( settings.value( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    livecodingLayout->addWidget( livecodingSupportersListLabel );
    livecodingLayout->addWidget( livecodingSupportersListEdit );

    QLabel *livecodingBlackListLabel = new QLabel( BLACKLIST_TEXT );
    livecodingBlackListEdit->setText( settings.value( LIVECODING_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    livecodingLayout->addWidget( livecodingBlackListLabel );
    livecodingLayout->addWidget( livecodingBlackListEdit );

    livecodingRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    livecodingRemoveBlackListUsersCheckBox->setChecked( settings.value( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    livecodingLayout->addWidget( livecodingRemoveBlackListUsersCheckBox );

    livecodingLayout->addStretch( 1 );

    QGroupBox *livecodingGroup = new QGroupBox( tabSettings );
    livecodingGroup->setLayout( livecodingLayout );

    tabSettings->addTab( livecodingGroup, QIcon( ":/resources/livecodinglogo.png" ), tr( "Livecoding" ) );




    //настройки realltv
    QVBoxLayout *realltvLayout = new QVBoxLayout;

    //QLabel *realltvChannelLabel = new QLabel( CHANNEL_TEXT );

    realltvChannelCheckBox->setText( CHANNEL_TEXT );
    realltvChannelCheckBox->setChecked( settings.value( REALLTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    realltvChannelEdit->setText( settings.value( REALLTV_CHANNEL_SETTING_PATH, DEFAULT_REALLTV_CHANNEL_NAME ).toString() );
    realltvChannelEdit->setEnabled( realltvChannelCheckBox->isChecked() );

    QObject::connect( realltvChannelCheckBox, SIGNAL( clicked( bool ) ), realltvChannelEdit, SLOT( setEnabled( bool ) ) );

    //realltvLayout->addWidget( realltvChannelLabel );
    realltvLayout->addWidget( realltvChannelCheckBox );
    realltvLayout->addWidget( realltvChannelEdit );

    QLabel *realltvAliasesLabel = new QLabel( ALIASES_TEXT );

    realltvAliasesEdit->setText( settings.value( REALLTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    realltvLayout->addWidget( realltvAliasesLabel );
    realltvLayout->addWidget( realltvAliasesEdit );

    QLabel *realltvSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    realltvSupportersListEdit->setText( settings.value( REALLTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    realltvLayout->addWidget( realltvSupportersListLabel );
    realltvLayout->addWidget( realltvSupportersListEdit );

    QLabel *realltvBlackListLabel = new QLabel( BLACKLIST_TEXT );
    realltvBlackListEdit->setText( settings.value( REALLTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    realltvLayout->addWidget( realltvBlackListLabel );
    realltvLayout->addWidget( realltvBlackListEdit );

    realltvRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    realltvRemoveBlackListUsersCheckBox->setChecked( settings.value( REALLTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    realltvLayout->addWidget( realltvRemoveBlackListUsersCheckBox );

    realltvLayout->addStretch( 1 );

    QGroupBox *realltvGroup = new QGroupBox( tabSettings );
    realltvGroup->setLayout( realltvLayout );

    tabSettings->addTab( realltvGroup, QIcon( ":/resources/realltvlogo.png" ), tr( "Realltv" ) );







    //настройки sc2tv
    QVBoxLayout *sc2tvLayout = new QVBoxLayout;

    //QLabel *sc2tvChannelLabel = new QLabel( CHANNEL_TEXT );

    sc2tvChannelCheckBox->setText( CHANNEL_TEXT );
    sc2tvChannelCheckBox->setChecked( settings.value( SC2TV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    sc2tvChannelEdit->setText( settings.value( SC2TV_CHANNEL_SETTING_PATH, DEFAULT_SC2TV_CHANNEL_NAME ).toString() );
    sc2tvChannelEdit->setEnabled( sc2tvChannelCheckBox->isChecked() );

    QObject::connect( sc2tvChannelCheckBox, SIGNAL( clicked( bool ) ), sc2tvChannelEdit, SLOT( setEnabled( bool ) ) );

    //sc2tvLayout->addWidget( sc2tvChannelLabel );
    sc2tvLayout->addWidget( sc2tvChannelCheckBox );
    sc2tvLayout->addWidget( sc2tvChannelEdit );

    sc2tvOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    sc2tvOriginalColorsCheckBox->setChecked( settings.value( SC2TV_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    sc2tvLayout->addWidget( sc2tvOriginalColorsCheckBox );

    QLabel *sc2tvAliasesLabel = new QLabel( ALIASES_TEXT );
    sc2tvAliasesEdit->setText( settings.value( SC2TV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    sc2tvLayout->addWidget( sc2tvAliasesLabel );
    sc2tvLayout->addWidget( sc2tvAliasesEdit );

    QLabel *sc2tvSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    sc2tvSupportersListEdit->setText( settings.value( SC2TV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    sc2tvLayout->addWidget( sc2tvSupportersListLabel );
    sc2tvLayout->addWidget( sc2tvSupportersListEdit );

    QLabel *sc2tvBlackListLabel = new QLabel( BLACKLIST_TEXT );

    sc2tvBlackListEdit->setText( settings.value( SC2TV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    sc2tvLayout->addWidget( sc2tvBlackListLabel );
    sc2tvLayout->addWidget( sc2tvBlackListEdit );

    sc2tvRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    sc2tvRemoveBlackListUsersCheckBox->setChecked( settings.value( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    sc2tvLayout->addWidget( sc2tvRemoveBlackListUsersCheckBox );

    sc2tvLayout->addStretch( 1 );

    QGroupBox *sc2tvGroup = new QGroupBox( tabSettings );
    sc2tvGroup->setLayout( sc2tvLayout );

    tabSettings->addTab( sc2tvGroup, QIcon( ":/resources/sc2tvlogo.png" ), tr( "Sc2tv" ) );

    //настройки streambox
    QVBoxLayout *streamboxLayout = new QVBoxLayout;

    streamboxChannelCheckBox->setText( CHANNEL_TEXT );
    streamboxChannelCheckBox->setChecked( settings.value( STREAMBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    streamboxChannelEdit->setText( settings.value( STREAMBOX_CHANNEL_SETTING_PATH, DEFAULT_STREAMBOX_CHANNEL_NAME ).toString() );
    streamboxChannelEdit->setEnabled( streamboxChannelCheckBox->isChecked() );

    QObject::connect( streamboxChannelCheckBox, SIGNAL( clicked( bool ) ), streamboxChannelEdit, SLOT( setEnabled( bool ) ) );

    streamboxLayout->addWidget( streamboxChannelCheckBox );
    streamboxLayout->addWidget( streamboxChannelEdit );

    QLabel *streamboxAliasesLabel = new QLabel( ALIASES_TEXT );
    streamboxAliasesEdit->setText( settings.value( STREAMBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    streamboxLayout->addWidget( streamboxAliasesLabel );
    streamboxLayout->addWidget( streamboxAliasesEdit );

    QLabel *streamboxSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    streamboxSupportersListEdit->setText( settings.value( STREAMBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    streamboxLayout->addWidget( streamboxSupportersListLabel );
    streamboxLayout->addWidget( streamboxSupportersListEdit );

    QLabel *streamboxBlackListLabel = new QLabel( BLACKLIST_TEXT );

    streamboxBlackListEdit->setText( settings.value( STREAMBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    streamboxLayout->addWidget( streamboxBlackListLabel );
    streamboxLayout->addWidget( streamboxBlackListEdit );

    streamboxRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    streamboxRemoveBlackListUsersCheckBox->setChecked( settings.value( STREAMBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    streamboxLayout->addWidget( streamboxRemoveBlackListUsersCheckBox );

    streamboxLayout->addStretch( 1 );

    QGroupBox *streamboxGroup = new QGroupBox( tabSettings );
    streamboxGroup->setLayout( streamboxLayout );

    tabSettings->addTab( streamboxGroup, QIcon( ":/resources/streamboxlogo.png" ), tr( "Streambox" ) );

    //настройки twitch
    QVBoxLayout *twitchLayout = new QVBoxLayout;

    //QLabel *twitchChannelLabel = new QLabel( CHANNEL_TEXT );

    twitchChannelCheckBox->setText( CHANNEL_TEXT );
    twitchChannelCheckBox->setChecked( settings.value( TWITCH_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    twitchChannelEdit->setText( settings.value( TWITCH_CHANNEL_SETTING_PATH, DEFAULT_TWITCH_CHANNEL_NAME ).toString() );
    twitchChannelEdit->setEnabled( twitchChannelCheckBox->isChecked() );

    QObject::connect( twitchChannelCheckBox, SIGNAL( clicked( bool ) ), twitchChannelEdit, SLOT( setEnabled( bool ) ) );

    //twitchLayout->addWidget( twitchChannelLabel );
    twitchLayout->addWidget( twitchChannelCheckBox );
    twitchLayout->addWidget( twitchChannelEdit );

    QLabel *twitchAliasesLabel = new QLabel( ALIASES_TEXT );
    twitchAliasesEdit->setText( settings.value( TWITCH_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    twitchLayout->addWidget( twitchAliasesLabel );
    twitchLayout->addWidget( twitchAliasesEdit );

    QLabel *twitchSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    twitchSupportersListEdit->setText( settings.value( TWITCH_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    twitchLayout->addWidget( twitchSupportersListLabel );
    twitchLayout->addWidget( twitchSupportersListEdit );

    QLabel *twitchBlackListLabel = new QLabel( BLACKLIST_TEXT );

    twitchBlackListEdit->setText( settings.value( TWITCH_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    twitchLayout->addWidget( twitchBlackListLabel );
    twitchLayout->addWidget( twitchBlackListEdit );

    twitchRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    twitchRemoveBlackListUsersCheckBox->setChecked( settings.value( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    twitchLayout->addWidget( twitchRemoveBlackListUsersCheckBox );

    twitchLayout->addStretch( 1 );

    QGroupBox *twitchGroup = new QGroupBox( tabSettings );
    twitchGroup->setLayout( twitchLayout );

    tabSettings->addTab( twitchGroup, QIcon( ":/resources/twitchlogo.png" ), tr( "Twitch" ) );



    //настройки youtube
    QVBoxLayout *youtubeLayout = new QVBoxLayout;

    youtubeChannelCheckBox->setText( CHANNEL_TEXT );
    youtubeChannelCheckBox->setChecked( settings.value( YOUTUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    youtubeChannelEdit->setText( settings.value( YOUTUBE_CHANNEL_SETTING_PATH, DEFAULT_YOUTUBE_CHANNEL_NAME ).toString() );
    youtubeChannelEdit->setEnabled( youtubeChannelCheckBox->isChecked() );

    QObject::connect( youtubeChannelCheckBox, SIGNAL( clicked( bool ) ), youtubeChannelEdit, SLOT( setEnabled( bool ) ) );

    youtubeLayout->addWidget( youtubeChannelCheckBox );
    youtubeLayout->addWidget( youtubeChannelEdit );

    QLabel *youtubeAliasesLabel = new QLabel( ALIASES_TEXT );

    youtubeAliasesEdit->setText( settings.value( YOUTUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    youtubeLayout->addWidget( youtubeAliasesLabel );
    youtubeLayout->addWidget( youtubeAliasesEdit );

    QLabel *youtubeSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    youtubeSupportersListEdit->setText( settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    youtubeLayout->addWidget( youtubeSupportersListLabel );
    youtubeLayout->addWidget( youtubeSupportersListEdit );

    QLabel *youtubeBlackListLabel = new QLabel( BLACKLIST_TEXT );
    youtubeBlackListEdit->setText( settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    youtubeLayout->addWidget( youtubeBlackListLabel );
    youtubeLayout->addWidget( youtubeBlackListEdit );

    youtubeRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    youtubeRemoveBlackListUsersCheckBox->setChecked( settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    youtubeLayout->addWidget( youtubeRemoveBlackListUsersCheckBox );

    youtubeLayout->addStretch( 1 );

    QGroupBox *youtubeGroup = new QGroupBox( tabSettings );
    youtubeGroup->setLayout( youtubeLayout );

    tabSettings->addTab( youtubeGroup, QIcon( ":/resources/youtubelogo.png" ), tr( "Youtube" ) );


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

    update();
}

QSettingsDialog::~QSettingsDialog()
{
}

void QSettingsDialog::saveSettings()
{
    QSettings settings;
    int oldIntValue;
    bool oldBoolValue;
    QString oldStringValue;

    bool defaultStyleChanged = false;

    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_FONT_NAME_SETTING_PATH, DEFAULT_GENERATED_STYLE_FONT_NAME ).toString() != fontNameCombo->currentText() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_NICKNAMES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_FONT_SIZE ).toInt() != nicknamesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_NICKNAMES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_NICKNAMES_TEXT_COLOR ).toUInt() != nicknamesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_FONT_SIZE ).toInt() != messagesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_MESSAGES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_MESSAGES_TEXT_COLOR ).toUInt() != messagesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_FONT_SIZE ).toInt() != viewersFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_VIEWERS_TEXT_COLOR ).toUInt() != viewersTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_FONT_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_FONT_SIZE ).toInt() != aliasesFontSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ALIASES_TEXT_COLOR_SETTING_PATH, DEFAULT_GENERATED_STYLE_ALIASES_TEXT_COLOR ).toUInt() != aliasesTextColor );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SMILES_SIZE ).toInt() != smilesSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, DEFAULT_GENERATED_STYLE_SERVICE_ICONS_SIZE ).toInt() != serviceIconsSizeSpinBox->value() );
    defaultStyleChanged |= ( settings.value( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, DEFAULT_GENERATED_STYLE_ANIMATION_TYPE ).toString() != animationTypeCombo->currentText() );

    settings.setValue( GENERATED_STYLE_FONT_NAME_SETTING_PATH, fontNameCombo->currentText() );
    settings.setValue( GENERATED_STYLE_NICKNAMES_FONT_SIZE_SETTING_PATH, nicknamesFontSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_NICKNAMES_TEXT_COLOR_SETTING_PATH, nicknamesTextColor );
    settings.setValue( GENERATED_STYLE_MESSAGES_FONT_SIZE_SETTING_PATH, messagesFontSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_MESSAGES_TEXT_COLOR_SETTING_PATH, messagesTextColor );
    settings.setValue( GENERATED_STYLE_VIEWERS_FONT_SIZE_SETTING_PATH, viewersFontSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_VIEWERS_TEXT_COLOR_SETTING_PATH, viewersTextColor );
    settings.setValue( GENERATED_STYLE_ALIASES_FONT_SIZE_SETTING_PATH, aliasesFontSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_ALIASES_TEXT_COLOR_SETTING_PATH, aliasesTextColor );
    settings.setValue( GENERATED_STYLE_SMILES_SIZE_SETTING_PATH, smilesSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_SERVICE_ICONS_SIZE_SETTING_PATH, serviceIconsSizeSpinBox->value() );
    settings.setValue( GENERATED_STYLE_ANIMATION_TYPE_SETTING_PATH, animationTypeCombo->currentText() );

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
    if( ( oldStringValue != styleCombo->currentText() ) || ( defaultStyleChanged && styleCombo->currentIndex() == 0 ) )
    {
        if( styleCombo->currentIndex() == 0 )
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

    oldStringValue = settings.value( ACES_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesChannelEdit->text() )
    {
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

    oldStringValue = settings.value( CYBERGAME_CHANNEL_SETTING_PATH, DEFAULT_CYBERGAME_CHANNEL_NAME ).toString();
    if( oldStringValue != cyberGameChannelEdit->text() )
    {
        settings.setValue( CYBERGAME_CHANNEL_SETTING_PATH, cyberGameChannelEdit->text() );
        emit cyberGameChannelChanged();
    }
    //emit cyberGameChannelChanged();

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
    oldStringValue = settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, DEFAULT_FUNSTREAM_CHANNEL_NAME ).toString();
    if( oldStringValue != funstreamChannelEdit->text() )
    {
        settings.setValue( FUNSTREAM_CHANNEL_SETTING_PATH, funstreamChannelEdit->text() );
        emit funstreamChannelChanged();
    }
    //emit funstreamChannelChanged();

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
    oldStringValue = settings.value( GAMERSTV_CHANNEL_SETTING_PATH, DEFAULT_GAMERSTV_CHANNEL_NAME ).toString();
    if( oldStringValue != gamerstvChannelEdit->text() )
    {
        settings.setValue( GAMERSTV_CHANNEL_SETTING_PATH, gamerstvChannelEdit->text() );
        emit gamerstvChannelChanged();
    }
    //emit gamerstvChannelChanged();

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
    oldStringValue = settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, DEFAULT_GIPSYTEAM_CHANNEL_NAME ).toString();
    if( oldStringValue != gipsyteamChannelEdit->text() )
    {
        settings.setValue( GIPSYTEAM_CHANNEL_SETTING_PATH, gipsyteamChannelEdit->text() );
        emit gipsyteamChannelChanged();
    }
    //emit gamerstvChannelChanged();

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
    oldStringValue = settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString();
    if( oldStringValue != goodGameChannelEdit->text() )
    {
        settings.setValue( GOODGAME_CHANNEL_SETTING_PATH, goodGameChannelEdit->text() );
        emit goodGameChannelChanged();
    }
    //emit goodGameChannelChanged();

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

    oldStringValue = settings.value( HITBOX_CHANNEL_SETTING_PATH, DEFAULT_HITBOX_CHANNEL_NAME ).toString();
    if( oldStringValue != hitboxChannelEdit->text() )
    {
        settings.setValue( HITBOX_CHANNEL_SETTING_PATH, hitboxChannelEdit->text() );
        emit hitboxChannelChanged();
    }
    //emit hitboxChannelChanged();

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



    //настройки livecoding
    oldStringValue = settings.value( LIVECODING_CHANNEL_SETTING_PATH, DEFAULT_LIVECODING_CHANNEL_NAME ).toString();
    if( oldStringValue != livecodingChannelEdit->text() )
    {
        settings.setValue( LIVECODING_CHANNEL_SETTING_PATH, livecodingChannelEdit->text() );
        emit livecodingChannelChanged();
    }

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

    //настройки realltv
    oldStringValue = settings.value( REALLTV_CHANNEL_SETTING_PATH, DEFAULT_REALLTV_CHANNEL_NAME ).toString();
    if( oldStringValue != realltvChannelEdit->text() )
    {
        settings.setValue( REALLTV_CHANNEL_SETTING_PATH, realltvChannelEdit->text() );
        emit realltvChannelChanged();
    }
    //emit realltvChannelChanged();

    oldStringValue = settings.value( REALLTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != realltvAliasesEdit->text() )
    {
        settings.setValue( REALLTV_ALIASES_SETTING_PATH, realltvAliasesEdit->text() );
        emit realltvAliasesChanged( realltvAliasesEdit->text() );
    }

    oldStringValue = settings.value( REALLTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != realltvSupportersListEdit->toPlainText() )
    {
        settings.setValue( REALLTV_SUPPORTERS_LIST_SETTING_PATH, realltvSupportersListEdit->toPlainText() );
        emit realltvSupportersListChanged( realltvSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( REALLTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != realltvBlackListEdit->toPlainText() )
    {
        settings.setValue( REALLTV_BLACK_LIST_SETTING_PATH, realltvBlackListEdit->toPlainText() );
        emit realltvBlackListChanged( realltvBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( REALLTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != realltvRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( REALLTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, realltvRemoveBlackListUsersCheckBox->isChecked() );
        emit realltvRemoveBlackListUsersChanged( realltvRemoveBlackListUsersCheckBox->isChecked() );
    }



    //настройки sc2tv
    oldStringValue = settings.value( SC2TV_CHANNEL_SETTING_PATH, DEFAULT_SC2TV_CHANNEL_NAME ).toString();
    if( oldStringValue != sc2tvChannelEdit->text() )
    {
        settings.setValue( SC2TV_CHANNEL_SETTING_PATH, sc2tvChannelEdit->text() );
        emit sc2tvChannelChanged();
    }
    //emit sc2tvChannelChanged();

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

    //настройки streambox

    oldStringValue = settings.value( STREAMBOX_CHANNEL_SETTING_PATH, DEFAULT_STREAMBOX_CHANNEL_NAME ).toString();
    if( oldStringValue != streamboxChannelEdit->text() )
    {
        settings.setValue( STREAMBOX_CHANNEL_SETTING_PATH, streamboxChannelEdit->text() );
        emit streamboxChannelChanged();
    }
    //emit twitchChannelChanged();

    oldStringValue = settings.value( STREAMBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamboxAliasesEdit->text() )
    {
        settings.setValue( STREAMBOX_ALIASES_SETTING_PATH, streamboxAliasesEdit->text() );
        emit streamboxAliasesChanged( streamboxAliasesEdit->text() );
    }

    oldStringValue = settings.value( STREAMBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamboxSupportersListEdit->toPlainText() )
    {
        settings.setValue( STREAMBOX_SUPPORTERS_LIST_SETTING_PATH, streamboxSupportersListEdit->toPlainText() );
        emit streamboxSupportersListChanged( streamboxSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( STREAMBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != streamboxBlackListEdit->toPlainText() )
    {
        settings.setValue( STREAMBOX_BLACK_LIST_SETTING_PATH, streamboxBlackListEdit->toPlainText() );
        emit streamboxBlackListChanged( streamboxBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( STREAMBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != streamboxRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( STREAMBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, streamboxRemoveBlackListUsersCheckBox->isChecked() );
        emit streamboxRemoveBlackListUsersChanged( streamboxRemoveBlackListUsersCheckBox->isChecked() );
    }

    //настройки twitch
    oldStringValue = settings.value( TWITCH_CHANNEL_SETTING_PATH, DEFAULT_TWITCH_CHANNEL_NAME ).toString();
    if( oldStringValue != twitchChannelEdit->text() )
    {
        settings.setValue( TWITCH_CHANNEL_SETTING_PATH, twitchChannelEdit->text() );
        emit twitchChannelChanged();
    }
    //emit twitchChannelChanged();

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


    //настройки youtube
    oldStringValue = settings.value( YOUTUBE_CHANNEL_SETTING_PATH, DEFAULT_YOUTUBE_CHANNEL_NAME ).toString();
    if( oldStringValue != youtubeChannelEdit->text() )
    {
        settings.setValue( YOUTUBE_CHANNEL_SETTING_PATH, youtubeChannelEdit->text() );
        emit youtubeChannelChanged();
    }

    oldStringValue = settings.value( YOUTUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != youtubeAliasesEdit->text() )
    {
        settings.setValue( YOUTUBE_ALIASES_SETTING_PATH, youtubeAliasesEdit->text() );
        emit youtubeAliasesChanged( youtubeAliasesEdit->text() );
    }

    oldStringValue = settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != youtubeSupportersListEdit->toPlainText() )
    {
        settings.setValue( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, youtubeSupportersListEdit->toPlainText() );
        emit youtubeSupportersListChanged( youtubeSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != youtubeBlackListEdit->toPlainText() )
    {
        settings.setValue( YOUTUBE_BLACK_LIST_SETTING_PATH, youtubeBlackListEdit->toPlainText() );
        emit youtubeBlackListChanged( youtubeBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != youtubeRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, youtubeRemoveBlackListUsersCheckBox->isChecked() );
        emit youtubeRemoveBlackListUsersChanged( youtubeRemoveBlackListUsersCheckBox->isChecked() );
    }


}

void QSettingsDialog::setColorButtonStyle( QPushButton *button, QRgb color, int fontSize )
{
    QColor rgbaColor( color );

    QString newButtonStyle( "QPushButton{ background-color:rgba(" +
                           QString::number( rgbaColor.red() ) + "," +
                           QString::number( rgbaColor.green() ) + "," +
                            QString::number( rgbaColor.blue() ) + "," +
                           QString::number( rgbaColor.alpha() ) + ");" +
                           "border-style: solid; font-size: " + QString::number( fontSize ) + "px;}" );



    //qDebug() << newButtonStyle;
    button->setStyleSheet( newButtonStyle );
}

void QSettingsDialog::nicknamesColorSelection()
{
    bool isOk = false;
    QRgb color = QColorDialog::getRgba( nicknamesTextColor, &isOk, this );
    if( isOk )
    {
        nicknamesTextColor = color;
        setColorButtonStyle( nicknamesTextColorButton, nicknamesTextColor );
    }

}

void QSettingsDialog::messagesColorSelection()
{
    bool isOk = false;
    QRgb color = QColorDialog::getRgba( messagesTextColor, &isOk, this );
    if( isOk )
    {
        messagesTextColor = color;
        setColorButtonStyle( messagesTextColorButton, messagesTextColor );
    }
}

void QSettingsDialog::viewersColorSelection()
{
    bool isOk = false;
    QRgb color = QColorDialog::getRgba( viewersTextColor, &isOk, this );
    if( isOk )
    {
        viewersTextColor = color;
        setColorButtonStyle( viewersTextColorButton, viewersTextColor );
    }
}

void QSettingsDialog::aliasesColorSelection()
{

    bool isOk = false;
    QRgb color = QColorDialog::getRgba( aliasesTextColor, &isOk, this );
    if( isOk )
    {
        aliasesTextColor = color;
        setColorButtonStyle( aliasesTextColorButton, aliasesTextColor );
    }
}

//TODO: retranslate

