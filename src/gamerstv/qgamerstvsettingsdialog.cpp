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
#include "qgamerstvsettingsdialog.h"

QGamerstvSettingsDialog::QGamerstvSettingsDialog(QWidget *parent) : QWidget(parent)
  , gamerstvChannelCheckBox( new QCheckBox( this ) )
  , gamerstvChannelEdit( new QLineEdit( this ) )
  , gamerstvBadgesCheckBox( new QCheckBox( this ) )
  , gamerstvAliasesEdit( new QLineEdit( this ) )
  , gamerstvSupportersListEdit( new QTextEdit( this ) )
  , gamerstvBlackListEdit( new QTextEdit( this ) )
  , gamerstvRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QGamerstvSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

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

    QGroupBox * gamerstvGroup = new QGroupBox( this );
    gamerstvGroup->setLayout( gamerstvLayout );

    return gamerstvGroup;
}

void QGamerstvSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки gamerstv

    bool oldBoolValue = settings.value( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( GAMERSTV_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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

}

void QGamerstvSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QGamerstvSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( gamerstvChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( gamerstvAliasesChanged(QString) ), chat, SLOT( setAliasesList(QString) ) );
    QObject::connect( this, SIGNAL( gamerstvSupportersListChanged(QString) ), chat, SLOT( setSupportersList(QString) ) );
    QObject::connect( this, SIGNAL( gamerstvBlackListChanged(QString) ), chat, SLOT( setBlackList(QString) ) );
    QObject::connect( this, SIGNAL( gamerstvRemoveBlackListUsersChanged(bool) ), chat, SLOT( setRemoveBlackListUsers(bool) ) );
    QObject::connect( this, SIGNAL( gamerstvBadgesChanged(bool) ), chat, SLOT( changeBadges(bool) ) );

}

QIcon QGamerstvSettingsDialog::getIcon()
{
    return QIcon( ":/resources/gamerstvlogo.png" );
}

QString QGamerstvSettingsDialog::getName()
{
    return tr( "Gamerstv" );
}

QObject *QGamerstvSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
