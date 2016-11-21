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

QGoodGameSettingsDialog::QGoodGameSettingsDialog(QObject *parent):QObject(parent)
{

}


QWidget *QGoodGameSettingsDialog::createLayout(QWidget *parent,QSettings& settings)
{
    QGroupBox *goodgameGroup = new QGroupBox( parent );

    //настройки youtube
    QVBoxLayout *goodGameLayout = new QVBoxLayout;
    goodgameGroup->setLayout( goodGameLayout );

    goodGameChannelCheckBox = new QCheckBox( parent );
    goodGameChannelEdit = new QLineEdit( parent );
    goodGameUseAnimatedSmilesCheckBox = new QCheckBox( parent );
    goodGameAliasesEdit = new QLineEdit( parent );
    goodGameSupportersListEdit = new QTextEdit( parent );
    goodGameBlackListEdit = new QTextEdit( parent );
    goodGameRemoveBlackListUsersCheckBox = new QCheckBox( parent );

    goodGameChannelCheckBox->setText( CHANNEL_TEXT );
    goodGameChannelCheckBox->setChecked( settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    goodGameChannelEdit->setText( settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString() );
    goodGameChannelEdit->setEnabled( goodGameChannelCheckBox->isChecked() );

    QObject::connect( goodGameChannelCheckBox, SIGNAL( clicked( bool ) ), goodGameChannelEdit, SLOT( setEnabled( bool ) ) );

    //goodGameLayout->addWidget( goodGameChannelLabel );
    goodGameLayout->addWidget( goodGameChannelCheckBox );
    goodGameLayout->addWidget( goodGameChannelEdit );

    goodGameUseAnimatedSmilesCheckBox->setText( tr( "Use Animated Smiles" ) );
    goodGameUseAnimatedSmilesCheckBox->setChecked( settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameUseAnimatedSmilesCheckBox );

    QLabel *goodGameAliasesLabel = new QLabel( ALIASES_TEXT );
    goodGameAliasesEdit->setText( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameAliasesLabel );
    goodGameLayout->addWidget( goodGameAliasesEdit );

    QLabel *goodGameSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    goodGameSupportersListEdit->setText( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameSupportersListLabel );
    goodGameLayout->addWidget( goodGameSupportersListEdit );

    QLabel *goodGameBlackListLabel = new QLabel( BLACKLIST_TEXT );

    goodGameBlackListEdit->setText( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    goodGameLayout->addWidget( goodGameBlackListLabel );
    goodGameLayout->addWidget( goodGameBlackListEdit );

    goodGameRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    goodGameRemoveBlackListUsersCheckBox->setChecked( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    goodGameLayout->addWidget( goodGameRemoveBlackListUsersCheckBox );

    goodGameLayout->addStretch( 1 );

    return goodgameGroup;
}

void QGoodGameSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки gg
    QString oldStringValue = settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString();
    if( oldStringValue != goodGameChannelEdit->text() )
    {
        settings.setValue( GOODGAME_CHANNEL_SETTING_PATH, goodGameChannelEdit->text() );
        emit goodGameChannelChanged();
    }
    //emit goodGameChannelChanged();

    bool oldBoolValue = settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool();
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
