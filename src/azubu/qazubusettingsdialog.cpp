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
#include "qazubusettingsdialog.h"

QAzubuSettingsDialog::QAzubuSettingsDialog(QWidget *parent) : QWidget(parent)
  , azubuChannelCheckBox( new QCheckBox( this ) )
  , azubuChannelEdit( new QLineEdit( this ) )
  , azubuAliasesEdit( new QLineEdit( this ) )
  , azubuSupportersListEdit( new QTextEdit( this ) )
  , azubuBlackListEdit( new QTextEdit( this ) )
  , azubuRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QAzubuSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
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

    QGroupBox * azubuGroup = new QGroupBox( this );
    azubuGroup->setLayout( azubuLayout );

    return azubuGroup;

}

void QAzubuSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки azubu
    bool oldBoolValue = settings.value( AZUBU_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( AZUBU_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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
}

void QAzubuSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}


void QAzubuSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( azubuChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( azubuAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( azubuSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( azubuBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( azubuRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );

}

QIcon QAzubuSettingsDialog::getIcon()
{
    return QIcon( ":/resources/azubulogo.png" );
}

QString QAzubuSettingsDialog::getName()
{
    return tr( "Azubu" );
}

QObject *QAzubuSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
