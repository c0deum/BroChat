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
#include "qlivecodingsettingsdialog.h"

QLivecodingSettingsDialog::QLivecodingSettingsDialog(QWidget *parent) : QWidget(parent)
  , livecodingChannelCheckBox( new QCheckBox( this ) )
  , livecodingChannelEdit( new QLineEdit( this ) )
  , livecodingBadgesCheckBox( new QCheckBox( this ) )
  , livecodingLoginEdit( new QLineEdit( this ) )
  , livecodingPasswordEdit( new QLineEdit( this ) )
  , livecodingAliasesEdit( new QLineEdit( this ) )
  , livecodingSupportersListEdit( new QTextEdit( this ) )
  , livecodingBlackListEdit( new QTextEdit( this ) )
  , livecodingRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QLivecodingSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{

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

    QGroupBox * livecodingGroup = new QGroupBox( this );
    livecodingGroup->setLayout( livecodingLayout );

    return livecodingGroup;
}

void QLivecodingSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки livecoding

    bool isLivecodingChannelChanged = false;

    bool oldBoolValue = settings.value( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( LIVECODING_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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
}

void QLivecodingSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QLivecodingSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( livecodingChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( livecodingAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( livecodingSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( livecodingBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( livecodingRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( this, SIGNAL( livecodingBadgesChanged(bool) ), chat, SLOT( changeBadges(bool) ) );


}

QIcon QLivecodingSettingsDialog::getIcon()
{
    return QIcon( ":/resources/livecodinglogo.png" );
}

QString QLivecodingSettingsDialog::getName()
{
    return tr( "Livecoding" );
}

QObject *QLivecodingSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
