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
#include "qcybergamesettingsdialog.h"


QCybergameSettingsDialog::QCybergameSettingsDialog(QWidget *parent) : QWidget(parent)
  , cyberGameChannelCheckBox( new QCheckBox( this ) )
  , cyberGameChannelEdit( new QLineEdit( this ) )
  , cyberGameAliasesEdit( new QLineEdit( this ) )
  , cyberGameSupportersListEdit( new QTextEdit( this ) )
  , cyberGameBlackListEdit( new QTextEdit( this ) )
  , cyberGameRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QCybergameSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

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

    QGroupBox * cyberGameGroup = new QGroupBox( this );
    cyberGameGroup->setLayout( cyberGameLayout );

    return cyberGameGroup;
}

void QCybergameSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки cybergame

    bool oldBoolValue = settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( CYBERGAME_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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
}

void QCybergameSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QCybergameSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( cyberGameChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( cyberGameAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( cyberGameSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( cyberGameBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( cyberGameRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );

}

QIcon QCybergameSettingsDialog::getIcon()
{
    return QIcon( ":/resources/cybergamelogo.png" );
}

QString QCybergameSettingsDialog::getName()
{
    return tr( "Cybergame" );
}

QObject *QCybergameSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
