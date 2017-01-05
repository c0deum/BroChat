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
#include "qacessettingsdialog.h"

QAcesSettingsDialog::QAcesSettingsDialog(QWidget *parent) : QWidget(parent)
  , acesChannelCheckBox( new QCheckBox( this ) )
  , acesChannelEdit( new QLineEdit( this ) )
  //, acesOriginalColorsCheckBox( new QCheckBox( this ) )
  , acesAliasesEdit( new QLineEdit( this ) )
  , acesSupportersListEdit( new QTextEdit( this ) )
  , acesBlackListEdit( new QTextEdit( this ) )
  , acesRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QAcesSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{
    Q_UNUSED( parent );

    QVBoxLayout * acesLayout = new QVBoxLayout;

    acesChannelCheckBox->setText( CHANNEL_TEXT );
    acesChannelCheckBox->setChecked( settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    acesChannelEdit->setText( settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString() );
    acesChannelEdit->setEnabled( acesChannelCheckBox->isChecked() );

    QObject::connect( acesChannelCheckBox, SIGNAL( clicked( bool ) ), acesChannelEdit, SLOT( setEnabled( bool ) ) );

    addWidgets( acesLayout, { acesChannelCheckBox, acesChannelEdit } );

    //acesOriginalColorsCheckBox->setText( tr( "Original Colors" ) );
    //acesOriginalColorsCheckBox->setChecked( settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool() );

    //acesLayout->addWidget( acesOriginalColorsCheckBox );

    acesAliasesEdit->setText( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( ALIASES_TEXT, this ), acesAliasesEdit } );

    acesSupportersListEdit->setText( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( SUPPORTERS_TEXT, this ), acesSupportersListEdit } );

    acesBlackListEdit->setText( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( acesLayout, { new QLabel( BLACKLIST_TEXT, this ), acesBlackListEdit } );

    acesRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    acesRemoveBlackListUsersCheckBox->setChecked( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    acesLayout->addWidget( acesRemoveBlackListUsersCheckBox );

    acesLayout->addStretch( 1 );

    QGroupBox * acesGroup = new QGroupBox( this );
    acesGroup->setLayout( acesLayout );

    return acesGroup;
}

void QAcesSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки aces
    bool oldBoolValue = settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( ACES_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != acesChannelCheckBox->isChecked() || oldStringValue != acesChannelEdit->text() )
    {
        settings.setValue( ACES_CHANNEL_ENABLE_SETTING_PATH, acesChannelCheckBox->isChecked() );
        settings.setValue( ACES_CHANNEL_SETTING_PATH, acesChannelEdit->text() );

        emit acesChannelChanged();
    }
    //emit acesChannelChanged();

    /*
     * oldBoolValue = settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != acesOriginalColorsCheckBox->isChecked() )
    {
        settings.setValue( ACES_ORIGINAL_COLORS_SETTING_PATH, acesOriginalColorsCheckBox->isChecked() );
        emit acesOriginalColorsChanged( acesOriginalColorsCheckBox->isChecked() );
    }
    */

    oldStringValue = settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesAliasesEdit->text() )
    {
        settings.setValue( ACES_ALIASES_SETTING_PATH, acesAliasesEdit->text() );
        emit acesAliasesChanged( acesAliasesEdit->text() );
    }

    oldStringValue = settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesSupportersListEdit->toPlainText() )
    {
        settings.setValue( ACES_SUPPORTERS_LIST_SETTING_PATH, acesSupportersListEdit->toPlainText() );
        emit acesSupportersListChanged( acesSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != acesBlackListEdit->toPlainText() )
    {
        settings.setValue( ACES_BLACK_LIST_SETTING_PATH, acesBlackListEdit->toPlainText() );
        emit acesBlackListChanged( acesBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != acesRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, acesRemoveBlackListUsersCheckBox->isChecked() );
        emit acesRemoveBlackListUsersChanged( acesRemoveBlackListUsersCheckBox->isChecked() );
    }




}

void QAcesSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QAcesSettingsDialog::connectDialogToChat(QObject *chat)
{
    QObject::connect( this, SIGNAL( acesChannelChanged() ), chat, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( acesAliasesChanged( QString ) ), chat, SLOT( setAliasesList( QString ) ) );
    QObject::connect( this, SIGNAL( acesSupportersListChanged( QString ) ), chat, SLOT( setSupportersList( QString ) ) );
    QObject::connect( this, SIGNAL( acesBlackListChanged( QString ) ), chat, SLOT( setBlackList( QString ) ) );
    QObject::connect( this, SIGNAL( acesRemoveBlackListUsersChanged( bool ) ), chat, SLOT( setRemoveBlackListUsers(bool ) ) );
}

QIcon QAcesSettingsDialog::getIcon()
{
    return QIcon( ":/resources/aceslogo.png" );
}

QString QAcesSettingsDialog::getName()
{
    return tr( "Aces" );
}

QObject *QAcesSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
