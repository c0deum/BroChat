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
#include "qvidisettingsdialog.h"

QVidiSettingsDialog::QVidiSettingsDialog(QWidget *parent) : QWidget(parent)
  , vidiChannelCheckBox( new QCheckBox( this ) )
  , vidiChannelEdit( new QLineEdit( this ) )
  , vidiAliasesEdit( new QLineEdit( this ) )
  , vidiSupportersListEdit( new QTextEdit( this ) )
  , vidiBlackListEdit( new QTextEdit( this ) )
  , vidiRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QVidiSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{

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

    QGroupBox * vidiGroup = new QGroupBox( this );
    vidiGroup->setLayout( vidiLayout );

    return vidiGroup;
}

void QVidiSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки vidi

    bool oldBoolValue = settings.value( VIDI_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( VIDI_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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

void QVidiSettingsDialog::loadSettings(QSettings &settings)
{
     (void)settings;
}

void QVidiSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( vidiChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( vidiAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( vidiSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( vidiBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( vidiRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );;

}

QIcon QVidiSettingsDialog::getIcon()
{
    return QIcon( ":/resources/vidilogo.png" );
}

QString QVidiSettingsDialog::getName()
{
    return tr( "Vidi" );
}

QObject *QVidiSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
