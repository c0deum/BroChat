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
#include "../youtube/qyoutubesettingsdialog.h"
#include "../goodgame/qgoodgamesettingsdialog.h"
#include "../twitch/qtwitchsettingsdialog.h"
#include "../vidi/qvidisettingsdialog.h"
#include "../peka2tv/qpeka2settingsdialog.h"
#include "../streamcube/qstreamcubesettingsdialog.h"
#include "../livecoding/qlivecodingsettingsdialog.h"
#include "../hitbox/qhitboxsettingsdialog.h"
#include "../gipsyteam/qgipsyteamsettingsdialog.h"
#include "../gamerstv/qgamerstvsettingsdialog.h"
#include "../cybergame/qcybergamesettingsdialog.h"
#include "../igdc/qigdcsettingsdialog.h"
#include "../beampro/qbeamprosettingsdialog.h"
#include "../azubu/qazubusettingsdialog.h"


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

    QSettings settings;
    populateTabs(tabSettings,settings);

}

//generate tabs and add them into the TabWidget
void QSettingsDialog::populateTabs(QTabWidget* tabHost,QSettings& settings)
{
    tabs_[ChatTypeEnum::Goodgame] = new QGoodGameSettingsDialog(this);
    tabs_[ChatTypeEnum::Youtube] = new QYoutubeSettingsDialog(this);
    tabs_[ChatTypeEnum::Twitch] = new QTwitchSettingsDialog(this);
    tabs_[ChatTypeEnum::Vidi] =  new QVidiSettingsDialog(this);
    tabs_[ChatTypeEnum::Peka2tv] = new QPeka2SettingsDialog(this);
    tabs_[ChatTypeEnum::Streamcube] = new QStreamCubeSettingsDialog(this);
    tabs_[ChatTypeEnum::Livecoding] = new QLivecodingSettingsDialog(this);
    tabs_[ChatTypeEnum::Hitbox] = new QHitboxSettingsDialog(this);
    tabs_[ChatTypeEnum::Igdc] = new QIgdcSettingsDialog(this);
    tabs_[ChatTypeEnum::Gipsyteam] = new QGipsyteamSettingsDialog(this);
    tabs_[ChatTypeEnum::Gamerstv] = new QGamerstvSettingsDialog(this);
    tabs_[ChatTypeEnum::Cybergame] = new QCybergameSettingsDialog(this);
    tabs_[ChatTypeEnum::Beampro] = new QBeamproSettingsDialog(this);
    tabs_[ChatTypeEnum::Azubu] = new QAzubuSettingsDialog(this);


    for (const auto& tab:tabs_.values())
    {
         tabHost->addTab( tab->createLayout(tabHost,settings),tab->getIcon(),
                     tab->getName() );
    }

}

//save setting from every tab
void QSettingsDialog::saveTabsSettings(QSettings &settings)
{
    for(auto& tab:tabs_) {
        tab->saveSettings(settings);
    }
}

void QSettingsDialog::connectDialogToChat(ChatTypeEnum chatType, QObject *chat)
{
    if (!tabs_.contains(chatType))
    {
        //todo: create helper enum 2 str
        //throw std::invalid_argument(tr("No chat with type "+ chatType + "registered"));
        throw std::invalid_argument("No chat with selected type registered");
    }
    auto& chatDialogTab = tabs_[chatType];
    chatDialogTab->connectDialogToChat(chat);;
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




    saveTabsSettings(settings);

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

