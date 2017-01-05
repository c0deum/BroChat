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
#include "qhitboxsettingsdialog.h"

QHitboxSettingsDialog::QHitboxSettingsDialog(QWidget *parent) : QWidget(parent)
  , hitboxChannelCheckBox( new QCheckBox( this ) )
  , hitboxChannelEdit( new QLineEdit( this ) )
  , hitboxOriginalColorsCheckBox( new QCheckBox( this ) )
  , hitboxAliasesEdit( new QLineEdit( this ) )
  , hitboxSupportersListEdit( new QTextEdit( this ) )
  , hitboxBlackListEdit( new QTextEdit( this ) )
  , hitboxRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QHitboxSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

    QVBoxLayout * hitboxLayout = new QVBoxLayout();

    hitboxChannelCheckBox->setText( CHANNEL_TEXT );
    hitboxChannelCheckBox->setChecked( settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    hitboxChannelEdit->setText( settings.value( HITBOX_CHANNEL_SETTING_PATH, DEFAULT_HITBOX_CHANNEL_NAME ).toString() );
    hitboxChannelEdit->setEnabled( hitboxChannelCheckBox->isChecked() );

    QObject::connect( hitboxChannelCheckBox, SIGNAL( clicked( bool ) ), hitboxChannelEdit, SLOT( setEnabled( bool ) ) );

    hitboxOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    hitboxOriginalColorsCheckBox->setChecked( settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    addWidgets( hitboxLayout, { hitboxChannelCheckBox, hitboxChannelEdit, hitboxOriginalColorsCheckBox  } );

    hitboxAliasesEdit->setText( settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( ALIASES_TEXT, this ), hitboxAliasesEdit } );

    hitboxSupportersListEdit->setText( settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( SUPPORTERS_TEXT, this ), hitboxSupportersListEdit } );

    hitboxBlackListEdit->setText( settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( hitboxLayout, { new QLabel( BLACKLIST_TEXT, this ), hitboxBlackListEdit } );

    hitboxRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    hitboxRemoveBlackListUsersCheckBox->setChecked( settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    hitboxLayout->addWidget( hitboxRemoveBlackListUsersCheckBox );

    hitboxLayout->addStretch( 1 );

    QGroupBox * hitboxGroup = new QGroupBox( this );
    hitboxGroup->setLayout( hitboxLayout );

    return hitboxGroup;
}

void QHitboxSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки hitbox

    bool oldBoolValue = settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( HITBOX_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != hitboxChannelCheckBox->isChecked() || oldStringValue != hitboxChannelEdit->text() )
    {
        settings.setValue( HITBOX_CHANNEL_ENABLE_SETTING_PATH, hitboxChannelCheckBox->isChecked() );
        settings.setValue( HITBOX_CHANNEL_SETTING_PATH, hitboxChannelEdit->text() );

        emit hitboxChannelChanged();
    }

    oldBoolValue = settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != hitboxOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( HITBOX_ORIGINAL_COLORS_SETTING_PATH, hitboxOriginalColorsCheckBox->isChecked() );
        emit hitboxOriginalColorsChanged( hitboxOriginalColorsCheckBox->isChecked() );
    }

    oldStringValue = settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxAliasesEdit->text() )
    {
        settings.setValue( HITBOX_ALIASES_SETTING_PATH, hitboxAliasesEdit->text() );
        emit hitboxAliasesChanged( hitboxAliasesEdit->text() );
    }

    oldStringValue = settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxSupportersListEdit->toPlainText() )
    {
        settings.setValue( HITBOX_SUPPORTERS_LIST_SETTING_PATH, hitboxSupportersListEdit->toPlainText() );
        emit hitboxSupportersListChanged( hitboxSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != hitboxBlackListEdit->toPlainText() )
    {
        settings.setValue( HITBOX_BLACK_LIST_SETTING_PATH, hitboxBlackListEdit->toPlainText() );
        emit hitboxBlackListChanged( hitboxBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != hitboxRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, hitboxRemoveBlackListUsersCheckBox->isChecked() );
        emit hitboxRemoveBlackListUsersChanged( hitboxRemoveBlackListUsersCheckBox->isChecked() );
    }

}

void QHitboxSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QHitboxSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( hitboxChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( hitboxAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( hitboxSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( hitboxBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( hitboxRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( this, SIGNAL( hitboxOriginalColorsChanged( bool ) ), chat, SLOT( changeOriginalColors( bool ) ) );
}

QIcon QHitboxSettingsDialog::getIcon()
{
    return QIcon( ":/resources/hitboxlogo.png" );
}

QString QHitboxSettingsDialog::getName()
{
    return tr( "Hitbox" );
}

QObject *QHitboxSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
