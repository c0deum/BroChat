#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QSettings>
#include <QLabel>
#include <QApplication>
#include "../core/settingsconsts.h"
#include "qtwitchsettingsdialog.h"

QTwitchSettingsDialog::QTwitchSettingsDialog(QWidget *parent) : QWidget(parent)
  , twitchChannelCheckBox( new QCheckBox( this ) )
  , twitchChannelEdit( new QLineEdit( this ) )
  , twitchOriginalColorsCheckBox( new QCheckBox( this ) )
  , twitchBadgesCheckBox( new QCheckBox( this ) )
  , twitchAliasesEdit( new QLineEdit( this ) )
  , twitchSupportersListEdit( new QTextEdit( this ) )
  , twitchBlackListEdit( new QTextEdit( this ) )
  , twitchRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QTwitchSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{

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

    QGroupBox * twitchGroup = new QGroupBox( parent );
    twitchGroup->setLayout( twitchLayout );

    return twitchGroup;
}

void QTwitchSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки twitch

    bool oldBoolValue = settings.value( TWITCH_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( TWITCH_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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
}

void QTwitchSettingsDialog::loadSettings(QSettings &settings)
{
     (void)settings;
}

void QTwitchSettingsDialog::connectDialogToChat(QObject *chat)
{
     QObject::connect( this, SIGNAL( twitchChannelChanged() ), chat, SLOT( reconnect() ) );
     QObject::connect( this, SIGNAL( twitchAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
     QObject::connect( this, SIGNAL( twitchSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
     QObject::connect( this, SIGNAL( twitchBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
     QObject::connect( this, SIGNAL( twitchRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );
     QObject::connect( this, SIGNAL( twitchOriginalColorsChanged( bool ) ), chat, SLOT( changeOriginalColors( bool ) ) );
     QObject::connect( this, SIGNAL( twitchBadgesChanged(bool) ), chat, SLOT( changeBadges(bool) ) );

}

QIcon QTwitchSettingsDialog::getIcon()
{
    return QIcon( ":/resources/twitchlogo.png");
}

QString QTwitchSettingsDialog::getName()
{
    return tr( "Twitch" );
}

QObject *QTwitchSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
