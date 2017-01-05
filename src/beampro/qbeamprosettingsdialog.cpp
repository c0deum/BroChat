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
#include "qbeamprosettingsdialog.h"

QBeamproSettingsDialog::QBeamproSettingsDialog(QWidget *parent) : QWidget(parent)
  , beamproChannelCheckBox( new QCheckBox( this ) )
  , beamproChannelEdit( new QLineEdit( this ) )
  , beamproAliasesEdit( new QLineEdit( this ) )
  , beamproSupportersListEdit( new QTextEdit( this ) )
  , beamproBlackListEdit( new QTextEdit( this ) )
  , beamproRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QBeamproSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

    QVBoxLayout * beamproLayout = new QVBoxLayout;

    beamproChannelCheckBox->setText( CHANNEL_TEXT );
    beamproChannelCheckBox->setChecked( settings.value( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    beamproChannelEdit->setText( settings.value( BEAMPRO_CHANNEL_SETTING_PATH, DEFAULT_BEAMPRO_CHANNEL_NAME ).toString() );
    beamproChannelEdit->setEnabled( beamproChannelCheckBox->isChecked() );

    QObject::connect( beamproChannelCheckBox, SIGNAL( clicked( bool ) ), beamproChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( beamproLayout, { beamproChannelCheckBox, beamproChannelEdit } );

    beamproAliasesEdit->setText( settings.value( BEAMPRO_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( ALIASES_TEXT, this ), beamproAliasesEdit } );

    beamproSupportersListEdit->setText( settings.value( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( SUPPORTERS_TEXT, this ), beamproSupportersListEdit } );

    beamproBlackListEdit->setText( settings.value( BEAMPRO_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( beamproLayout, { new QLabel( BLACKLIST_TEXT, this ), beamproBlackListEdit } );

    beamproRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    beamproRemoveBlackListUsersCheckBox->setChecked( settings.value( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    beamproLayout->addWidget( beamproRemoveBlackListUsersCheckBox );

    beamproLayout->addStretch( 1 );

    QGroupBox * beamproGroup = new QGroupBox( this );
    beamproGroup->setLayout( beamproLayout );

    return  beamproGroup;
}

void QBeamproSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки beampro
    bool oldBoolValue = settings.value( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( BEAMPRO_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != beamproChannelCheckBox->isChecked() || oldStringValue != beamproChannelEdit->text() )
    {
        settings.setValue( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, beamproChannelCheckBox->isChecked() );
        settings.setValue( BEAMPRO_CHANNEL_SETTING_PATH, beamproChannelEdit->text() );

        emit beamproChannelChanged();
    }

    oldStringValue = settings.value( BEAMPRO_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproAliasesEdit->text() )
    {
        settings.setValue( BEAMPRO_ALIASES_SETTING_PATH, beamproAliasesEdit->text() );
        emit beamproAliasesChanged( beamproAliasesEdit->text() );
    }

    oldStringValue = settings.value( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproSupportersListEdit->toPlainText() )
    {
        settings.setValue( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, beamproSupportersListEdit->toPlainText() );
        emit beamproSupportersListChanged( beamproSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( BEAMPRO_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != beamproBlackListEdit->toPlainText() )
    {
        settings.setValue( BEAMPRO_BLACK_LIST_SETTING_PATH, beamproBlackListEdit->toPlainText() );
        emit beamproBlackListChanged( beamproBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != beamproRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, beamproRemoveBlackListUsersCheckBox->isChecked() );
        emit beamproRemoveBlackListUsersChanged( beamproRemoveBlackListUsersCheckBox->isChecked() );
    }

}

void QBeamproSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QBeamproSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( beamproChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( beamproAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( beamproAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( beamproBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( beamproRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );

}

QIcon QBeamproSettingsDialog::getIcon()
{
    return QIcon( ":/resources/beamprologo.png" );
}

QString QBeamproSettingsDialog::getName()
{
    return tr( "Beampro" );
}

QObject *QBeamproSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
