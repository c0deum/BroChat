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
#include "qgipsyteamsettingsdialog.h"

QGipsyteamSettingsDialog::QGipsyteamSettingsDialog(QWidget *parent) : QWidget(parent)
  , gipsyteamChannelCheckBox( new QCheckBox( this ) )
  , gipsyteamChannelEdit( new QLineEdit( this ) )
  , gipsyteamAliasesEdit( new QLineEdit( this ) )
  , gipsyteamSupportersListEdit( new QTextEdit( this ) )
  , gipsyteamBlackListEdit( new QTextEdit( this ) )
  , gipsyteamRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QGipsyteamSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

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

    QGroupBox * gipsyteamGroup = new QGroupBox( this );
    gipsyteamGroup->setLayout( gipsyteamLayout );

    return gipsyteamGroup;
}

void QGipsyteamSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки gipsyteam

    bool oldBoolValue = settings.value( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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
}

void QGipsyteamSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QGipsyteamSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( gipsyteamChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( gipsyteamAliasesChanged(QString) ), chat, SLOT( setAliasesList(QString) ) );
    QObject::connect( this, SIGNAL( gipsyteamSupportersListChanged(QString) ), chat, SLOT( setSupportersList(QString) ) );
    QObject::connect( this, SIGNAL( gipsyteamBlackListChanged(QString) ), chat, SLOT( setBlackList(QString) ) );
    QObject::connect( this, SIGNAL( gipsyteamRemoveBlackListUsersChanged(bool) ), chat, SLOT( setRemoveBlackListUsers(bool) ) );

}

QIcon QGipsyteamSettingsDialog::getIcon()
{
    return QIcon( ":/resources/gipsyteamlogo.png" );
}

QString QGipsyteamSettingsDialog::getName()
{
    return  tr( "Gipsyteam" );
}

QObject *QGipsyteamSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
