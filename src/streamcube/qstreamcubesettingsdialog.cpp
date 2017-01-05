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
#include "qstreamcubesettingsdialog.h"

QStreamCubeSettingsDialog::QStreamCubeSettingsDialog(QWidget *parent) : QWidget(parent)
  , streamcubeChannelCheckBox( new QCheckBox( this ) )
  , streamcubeChannelEdit( new QLineEdit( this ) )
  , streamcubeAliasesEdit( new QLineEdit( this ) )
  , streamcubeSupportersListEdit( new QTextEdit( this ) )
  , streamcubeBlackListEdit( new QTextEdit( this ) )
  , streamcubeRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QStreamCubeSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

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

    QGroupBox * streamcubeGroup = new QGroupBox( this);
    streamcubeGroup->setLayout( streamcubeLayout );

    return streamcubeGroup;
}

void QStreamCubeSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки streamcube

    bool oldBoolValue = settings.value( STREAMCUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( STREAMCUBE_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
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

}

void QStreamCubeSettingsDialog::loadSettings(QSettings &settings)
{
    (void)(settings);
}

void QStreamCubeSettingsDialog::connectDialogToChat(QObject *chat)
{
     QObject::connect( this, SIGNAL( streamcubeChannelChanged() ), chat, SLOT( reconnect() ) );
     QObject::connect( this, SIGNAL( streamcubeAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
     QObject::connect( this, SIGNAL( streamcubeSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
     QObject::connect( this, SIGNAL( streamcubeBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
     QObject::connect( this, SIGNAL( streamcubeRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );

}

QIcon QStreamCubeSettingsDialog::getIcon()
{
    return QIcon( ":/resources/streamcubelogo.png" );
}

QString QStreamCubeSettingsDialog::getName()
{
    return tr( "Streamcube" );
}

QObject *QStreamCubeSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
