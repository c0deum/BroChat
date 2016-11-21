#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QSettings>
#include <QLabel>
#include <QApplication>
#include "qgoodgamesettingsdialog.h"
#include "../core/settingsconsts.h"

QGoodGameSettingsDialog::QGoodGameSettingsDialog(QWidget *parent):QWidget(parent)
  , goodGameChannelCheckBox( new QCheckBox( this ) )
  , goodGameChannelEdit( new QLineEdit( this ) )
  , goodGameBadgesCheckBox( new QCheckBox( this ) )
  , goodGameUseAnimatedSmilesCheckBox( new QCheckBox( this ) )
  , goodGameAliasesEdit( new QLineEdit( this ) )
  , goodGameSupportersListEdit( new QTextEdit( this ) )
  , goodGameBlackListEdit( new QTextEdit( this ) )
  , goodGameRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QGoodGameSettingsDialog::createLayout(QWidget *parent,QSettings& settings)
{

    QVBoxLayout * goodGameLayout = new QVBoxLayout();

    goodGameChannelCheckBox->setText( CHANNEL_TEXT );
    goodGameChannelCheckBox->setChecked( settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    goodGameChannelEdit->setText( settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString() );
    goodGameChannelEdit->setEnabled( goodGameChannelCheckBox->isChecked() );

    QObject::connect( goodGameChannelCheckBox, SIGNAL( clicked( bool ) ), goodGameChannelEdit, SLOT( setEnabled( bool ) ) );

    goodGameBadgesCheckBox->setText( tr( "Badges" ) );
    goodGameBadgesCheckBox->setChecked( settings.value( GOODGAME_BADGES_SETTING_PATH, false ).toBool() );

    goodGameUseAnimatedSmilesCheckBox->setText( tr( "Use Animated Smiles" ) );
    goodGameUseAnimatedSmilesCheckBox->setChecked( settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool() );

    addWidgets( goodGameLayout, { goodGameChannelCheckBox, goodGameChannelEdit, goodGameBadgesCheckBox, goodGameUseAnimatedSmilesCheckBox  } );

    goodGameAliasesEdit->setText( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( ALIASES_TEXT, this ), goodGameAliasesEdit } );

    goodGameSupportersListEdit->setText( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( SUPPORTERS_TEXT, this ), goodGameSupportersListEdit } );

    goodGameBlackListEdit->setText( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( goodGameLayout, { new QLabel( BLACKLIST_TEXT, this ), goodGameBlackListEdit } );

    goodGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    goodGameRemoveBlackListUsersCheckBox->setChecked( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameRemoveBlackListUsersCheckBox );

    goodGameLayout->addStretch( 1 );

    QGroupBox * goodGameGroup = new QGroupBox( parent );
    goodGameGroup->setLayout( goodGameLayout );

    return goodGameGroup;
}

void QGoodGameSettingsDialog::saveSettings(QSettings &settings)
{
    bool oldBoolValue = settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( GOODGAME_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != goodGameChannelCheckBox->isChecked() || oldStringValue != goodGameChannelEdit->text() )
    {
        settings.setValue( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, goodGameChannelCheckBox->isChecked() );
        settings.setValue( GOODGAME_CHANNEL_SETTING_PATH, goodGameChannelEdit->text() );

        emit goodGameChannelChanged();
    }

    oldBoolValue = settings.value( GOODGAME_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameBadgesCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_BADGES_SETTING_PATH, goodGameBadgesCheckBox->isChecked() );
        emit goodGameBadgesChanged( goodGameBadgesCheckBox->isChecked() );
    }

    oldBoolValue = settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameUseAnimatedSmilesCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, goodGameUseAnimatedSmilesCheckBox->isChecked() );
        emit goodGameUseAnimatedSmilesChanged( goodGameUseAnimatedSmilesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameAliasesEdit->text() )
    {
        settings.setValue( GOODGAME_ALIASES_SETTING_PATH, goodGameAliasesEdit->text() );
        emit goodGameAliasesChanged( goodGameAliasesEdit->text() );
    }

    oldStringValue = settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameSupportersListEdit->toPlainText() )
    {
        settings.setValue( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, goodGameSupportersListEdit->toPlainText() );
        emit goodGameSupportersListChanged( goodGameSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != goodGameBlackListEdit->toPlainText() )
    {
        settings.setValue( GOODGAME_BLACK_LIST_SETTING_PATH, goodGameBlackListEdit->toPlainText() );
        emit goodGameBlackListChanged( goodGameBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != goodGameRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, goodGameRemoveBlackListUsersCheckBox->isChecked() );
        emit goodGameRemoveBlackListUsersChanged( goodGameRemoveBlackListUsersCheckBox->isChecked() );
    }



}

void QGoodGameSettingsDialog::loadSettings(QSettings &settings)
{
    (void)settings;
}

QIcon QGoodGameSettingsDialog::getIcon()
{
    return QIcon( ":/resources/goodgamelogo.png" );
}


QString QGoodGameSettingsDialog::getName()
{
    return tr( "Goodgame" );
}

QObject *QGoodGameSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
