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
#include "qpeka2settingsdialog.h"

QPeka2SettingsDialog::QPeka2SettingsDialog(QWidget *parent):QWidget(parent)
, peka2ChannelCheckBox( new QCheckBox( this ) )
, peka2ChannelEdit( new QLineEdit( this ) )
, peka2OriginalColorsCheckBox( new QCheckBox( this ) )
, peka2BadgesCheckBox( new QCheckBox( this ) )
, peka2AliasesEdit( new QLineEdit( this ) )
, peka2SupportersListEdit( new QTextEdit( this ) )
, peka2BlackListEdit( new QTextEdit( this ) )
, peka2RemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QPeka2SettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

    QVBoxLayout * peka2Layout = new QVBoxLayout;

    peka2ChannelCheckBox->setText( CHANNEL_TEXT );
    peka2ChannelCheckBox->setChecked( settings.value( PEKA2_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    peka2ChannelEdit->setText( settings.value( PEKA2_CHANNEL_SETTING_PATH, DEFAULT_PEKA2_CHANNEL_NAME ).toString() );
    peka2ChannelEdit->setEnabled( peka2ChannelCheckBox->isChecked() );

    QObject::connect( peka2ChannelCheckBox, SIGNAL( clicked( bool ) ), peka2ChannelEdit, SLOT( setEnabled( bool ) ) );

    peka2OriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    peka2OriginalColorsCheckBox->setChecked( settings.value( PEKA2_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    peka2BadgesCheckBox->setText( tr( "Badges" ) );
    peka2BadgesCheckBox->setChecked( settings.value( PEKA2_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( peka2Layout, { peka2ChannelCheckBox, peka2ChannelEdit, peka2OriginalColorsCheckBox, peka2BadgesCheckBox } );

    peka2AliasesEdit->setText( settings.value( PEKA2_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( peka2Layout, { new QLabel( ALIASES_TEXT, this ), peka2AliasesEdit } );

    peka2SupportersListEdit->setText( settings.value( PEKA2_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( peka2Layout, { new QLabel( SUPPORTERS_TEXT, this ), peka2SupportersListEdit } );

    peka2BlackListEdit->setText( settings.value( PEKA2_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( peka2Layout, { new QLabel( BLACKLIST_TEXT, this ), peka2BlackListEdit } );

    peka2RemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    peka2RemoveBlackListUsersCheckBox->setChecked( settings.value( PEKA2_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    peka2Layout->addWidget( peka2RemoveBlackListUsersCheckBox );

    peka2Layout->addStretch( 1 );

    QGroupBox * peka2Group = new QGroupBox( this );
    peka2Group->setLayout( peka2Layout );

    return peka2Group;
}

void QPeka2SettingsDialog::saveSettings(QSettings &settings)
{
    //настройки peka2

    bool oldBoolValue = settings.value( PEKA2_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( PEKA2_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != peka2ChannelCheckBox->isChecked() || oldStringValue != peka2ChannelEdit->text() )
    {
        settings.setValue( PEKA2_CHANNEL_ENABLE_SETTING_PATH, peka2ChannelCheckBox->isChecked() );
        settings.setValue( PEKA2_CHANNEL_SETTING_PATH, peka2ChannelEdit->text() );

        emit peka2ChannelChanged();
    }

    oldBoolValue = settings.value( PEKA2_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != peka2OriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( PEKA2_ORIGINAL_COLORS_SETTING_PATH, peka2OriginalColorsCheckBox->isChecked() );
        emit peka2OriginalColorsChanged( peka2OriginalColorsCheckBox->isChecked() );
    }

    oldBoolValue = settings.value( PEKA2_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != peka2BadgesCheckBox->isChecked() )
    {
        settings.setValue( PEKA2_BADGES_SETTING_PATH, peka2BadgesCheckBox->isChecked() );
        emit peka2BadgesChanged( peka2BadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( PEKA2_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != peka2AliasesEdit->text() )
    {
        settings.setValue( PEKA2_ALIASES_SETTING_PATH, peka2AliasesEdit->text() );
        emit peka2AliasesChanged( peka2AliasesEdit->text() );
    }

    oldStringValue = settings.value( PEKA2_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != peka2SupportersListEdit->toPlainText() )
    {
        settings.setValue( PEKA2_SUPPORTERS_LIST_SETTING_PATH, peka2SupportersListEdit->toPlainText() );
        emit peka2SupportersListChanged( peka2SupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( PEKA2_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != peka2BlackListEdit->toPlainText() )
    {
        settings.setValue( PEKA2_BLACK_LIST_SETTING_PATH, peka2BlackListEdit->toPlainText() );
        emit peka2BlackListChanged( peka2BlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( PEKA2_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != peka2RemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( PEKA2_REMOVE_BLACK_LIST_USERS_SETTING_PATH, peka2RemoveBlackListUsersCheckBox->isChecked() );
        emit peka2RemoveBlackListUsersChanged( peka2RemoveBlackListUsersCheckBox->isChecked() );
    }

}

void QPeka2SettingsDialog::loadSettings(QSettings &settings)
{
    (void)settings;
}

void QPeka2SettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( peka2ChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( peka2AliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( peka2SupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( peka2BlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( peka2RemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );
    QObject::connect( this, SIGNAL( peka2BadgesChanged(bool) ), chat, SLOT( changeBadges(bool) ) );
    QObject::connect( this, SIGNAL( peka2OriginalColorsChanged( bool ) ), chat, SLOT( changeOriginalColors( bool ) ) );

}

QIcon QPeka2SettingsDialog::getIcon()
{
    return QIcon( ":/resources/peka2logo.png" );
}

QString QPeka2SettingsDialog::getName()
{
    return tr( "Peka2" );
}

QObject *QPeka2SettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
