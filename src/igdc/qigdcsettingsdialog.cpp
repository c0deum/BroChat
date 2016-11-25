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
#include "qigdcsettingsdialog.h"

QIgdcSettingsDialog::QIgdcSettingsDialog(QWidget *parent):QWidget(parent)
  , igdcChannelCheckBox( new QCheckBox( this ) )
  , igdcChannelEdit( new QLineEdit( this ) )
  , igdcBadgesCheckBox( new QCheckBox( this ) )
  , igdcAliasesEdit( new QLineEdit( this ) )
  , igdcSupportersListEdit( new QTextEdit( this ) )
  , igdcBlackListEdit( new QTextEdit( this ) )
  , igdcRemoveBlackListUsersCheckBox( new QCheckBox( this ) )
{

}


QWidget *QIgdcSettingsDialog::createLayout(QWidget *parent, QSettings &settings)
{

    QVBoxLayout * igdcLayout = new QVBoxLayout();

    igdcChannelCheckBox->setText( CHANNEL_TEXT );
    igdcChannelCheckBox->setChecked( settings.value( IGDC_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    igdcChannelEdit->setText( settings.value( IGDC_CHANNEL_SETTING_PATH, DEFAULT_IGDC_CHANNEL_NAME ).toString() );
    igdcChannelEdit->setEnabled( igdcChannelCheckBox->isChecked() );

    QObject::connect( igdcChannelCheckBox, SIGNAL( clicked( bool ) ), igdcChannelEdit, SLOT( setEnabled( bool ) ) );

    igdcBadgesCheckBox->setText( tr( "Badges" ) );
    igdcBadgesCheckBox->setChecked( settings.value( IGDC_BADGES_SETTING_PATH, false ).toBool() );

    addWidgets( igdcLayout, { igdcChannelCheckBox, igdcChannelEdit, igdcBadgesCheckBox } );

    igdcAliasesEdit->setText( settings.value( IGDC_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( ALIASES_TEXT, this ), igdcAliasesEdit } );

    igdcSupportersListEdit->setText( settings.value( IGDC_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( SUPPORTERS_TEXT, this ), igdcSupportersListEdit } );

    igdcBlackListEdit->setText( settings.value( IGDC_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    addWidgets( igdcLayout, { new QLabel( BLACKLIST_TEXT, this ), igdcBlackListEdit } );

    igdcRemoveBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    igdcRemoveBlackListUsersCheckBox->setChecked( settings.value( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    igdcLayout->addWidget( igdcRemoveBlackListUsersCheckBox );

    igdcLayout->addStretch( 1 );

    QGroupBox * igdcGroup = new QGroupBox( this );
    igdcGroup->setLayout( igdcLayout );

    return igdcGroup;
}

void QIgdcSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки igdc

    bool oldBoolValue = settings.value( IGDC_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool();
    QString oldStringValue = settings.value( IGDC_CHANNEL_SETTING_PATH, BLANK_STRING ).toString();
    if( oldBoolValue != igdcChannelCheckBox->isChecked() || oldStringValue != igdcChannelEdit->text() )
    {
        settings.setValue( IGDC_CHANNEL_ENABLE_SETTING_PATH, igdcChannelCheckBox->isChecked() );
        settings.setValue( IGDC_CHANNEL_SETTING_PATH, igdcChannelEdit->text() );

        emit igdcChannelChanged();
    }

    oldBoolValue = settings.value( IGDC_BADGES_SETTING_PATH, false ).toBool();
    if( oldBoolValue != igdcBadgesCheckBox->isChecked() )
    {
        settings.setValue( IGDC_BADGES_SETTING_PATH, igdcBadgesCheckBox->isChecked() );
        emit igdcBadgesChanged( igdcBadgesCheckBox->isChecked() );
    }

    oldStringValue = settings.value( IGDC_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcAliasesEdit->text() )
    {
        settings.setValue( IGDC_ALIASES_SETTING_PATH, igdcAliasesEdit->text() );
        emit igdcAliasesChanged( igdcAliasesEdit->text() );
    }

    oldStringValue = settings.value( IGDC_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcSupportersListEdit->toPlainText() )
    {
        settings.setValue( IGDC_SUPPORTERS_LIST_SETTING_PATH, igdcSupportersListEdit->toPlainText() );
        emit igdcSupportersListChanged( igdcSupportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( IGDC_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != igdcBlackListEdit->toPlainText() )
    {
        settings.setValue( IGDC_BLACK_LIST_SETTING_PATH, igdcBlackListEdit->toPlainText() );
        emit igdcBlackListChanged( igdcBlackListEdit->toPlainText() );
    }

    oldBoolValue = settings.value( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != igdcRemoveBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, igdcRemoveBlackListUsersCheckBox->isChecked() );
        emit igdcRemoveBlackListUsersChanged( igdcRemoveBlackListUsersCheckBox->isChecked() );
    }

}

void QIgdcSettingsDialog::loadSettings(QSettings &settings)
{
    (void) settings;
}

void QIgdcSettingsDialog::connectDialogToChat(QObject *chat)
{
     QObject::connect( this, SIGNAL( igdcChannelChanged() ), chat, SLOT( reconnect() ) );
     QObject::connect( this, SIGNAL( igdcAliasesChanged(QString) ), chat, SLOT( setAliasesList(QString) ) );
     QObject::connect( this, SIGNAL( igdcSupportersListChanged(QString) ), chat, SLOT( setSupportersList(QString) ) );
     QObject::connect( this, SIGNAL( igdcBlackListChanged(QString) ), chat, SLOT( setBlackList(QString) ) );
     QObject::connect( this, SIGNAL( igdcRemoveBlackListUsersChanged(bool) ), chat, SLOT( setRemoveBlackListUsers(bool) ) );
     QObject::connect( this, SIGNAL( igdcBadgesChanged(bool) ), chat, SLOT( changeBadges(bool) ) );

}

QIcon QIgdcSettingsDialog::getIcon()
{
    return QIcon( ":/resources/igdclogo.png" );
}

QString QIgdcSettingsDialog::getName()
{
    return tr( "Igdc" ) ;
}

QObject *QIgdcSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
