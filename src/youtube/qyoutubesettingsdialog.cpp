#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QSettings>
#include <QLabel>
#include "qyoutubesettingsdialog.h"
#include "YoutubeController.h"
#include "../core/settingsconsts.h"


QYoutubeSettingsDialog::QYoutubeSettingsDialog(QObject *parent):QObject(parent)
{

}


QWidget *QYoutubeSettingsDialog::createLayout(QWidget *parent,QSettings& settings)
{
    QGroupBox *youtubeGroup = new QGroupBox( parent );

    //настройки youtube
    QVBoxLayout *youtubeLayout = new QVBoxLayout;
    youtubeGroup->setLayout( youtubeLayout );

    channelCheckBox = new QCheckBox(parent);
    youtubeLoginWebsitePushButton = new QPushButton(parent);
    youtubeDeloginWebsitePushButton = new QPushButton(parent);
    supportersListEdit = new QTextEdit( parent );
    blackListEdit = new QTextEdit( parent ) ;
    removeBlackListUsersCheckBox = new QCheckBox( parent);


    channelCheckBox->setText( CHANNEL_TEXT );
    channelCheckBox->setChecked( settings.value( YOUTUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );


    QObject::connect( channelCheckBox, SIGNAL( clicked( bool ) ), youtubeLoginWebsitePushButton, SLOT( setEnabled( bool ) ) );
    QObject::connect( channelCheckBox, SIGNAL( clicked( bool ) ), this, SLOT( changeChannelAvailability(bool) ) );
    QObject::connect( youtubeLoginWebsitePushButton, SIGNAL( clicked( bool ) ), this, SLOT( loginWebsiteButtonClicked() ) );
    QObject::connect( youtubeDeloginWebsitePushButton, SIGNAL( clicked( bool ) ), this, SLOT( deloginWebsiteButtonClicked() ) );

    youtubeLayout->addWidget( channelCheckBox );

    youtubeLoginWebsitePushButton->setText(tr("Login to yor google account"));
    youtubeLoginWebsitePushButton->setMaximumWidth(200);
    youtubeLayout->addWidget( youtubeLoginWebsitePushButton );

    youtubeDeloginWebsitePushButton->setText(tr("Delogin"));
    youtubeDeloginWebsitePushButton->setMaximumWidth(200);
    youtubeLayout->addWidget( youtubeDeloginWebsitePushButton );


    QLabel *youtubeSupportersListLabel = new QLabel( SUPPORTERS_TEXT );

    supportersListEdit->setText( settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    youtubeLayout->addWidget( youtubeSupportersListLabel );
    youtubeLayout->addWidget( supportersListEdit );

    QLabel *youtubeBlackListLabel = new QLabel( BLACKLIST_TEXT );
    blackListEdit->setText( settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    youtubeLayout->addWidget( youtubeBlackListLabel );
    youtubeLayout->addWidget( blackListEdit );

    removeBlackListUsersCheckBox->setText( REMOVE_BLACKLIST_USERS_MESSAGES );
    removeBlackListUsersCheckBox->setChecked( settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    youtubeLayout->addWidget( removeBlackListUsersCheckBox );

    youtubeLayout->addStretch( 1 );

    return youtubeGroup;
}



void QYoutubeSettingsDialog::saveSettings(QSettings &settings)
{
    //настройки youtube
    /*
    QString oldStringValue = settings.value( YOUTUBE_CHANNEL_SETTING_PATH, DEFAULT_YOUTUBE_CHANNEL_NAME ).toString();
    if( oldStringValue != youtubeChannelEdit->text() )
    {
        settings.setValue( YOUTUBE_CHANNEL_SETTING_PATH, youtubeChannelEdit->text() );
        emit youtubeChannelChanged();
    }

    oldStringValue = settings.value( YOUTUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != youtubeAliasesEdit->text() )
    {
        settings.setValue( YOUTUBE_ALIASES_SETTING_PATH, youtubeAliasesEdit->text() );
        emit youtubeAliasesChanged( youtubeAliasesEdit->text() );
    }
    */


    QString oldStringValue = settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != supportersListEdit->toPlainText() )
    {
        settings.setValue( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, supportersListEdit->toPlainText() );
        emit supportersListChanged( supportersListEdit->toPlainText() );
    }

    oldStringValue = settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString();
    if( oldStringValue != blackListEdit->toPlainText() )
    {
        settings.setValue( YOUTUBE_BLACK_LIST_SETTING_PATH, blackListEdit->toPlainText() );
        emit blackListChanged( blackListEdit->toPlainText() );
    }

    bool oldBoolValue = settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool();
    if( oldBoolValue != removeBlackListUsersCheckBox->isChecked() )
    {
        settings.setValue( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, removeBlackListUsersCheckBox->isChecked() );
        emit removeBlackListUsersChanged( removeBlackListUsersCheckBox->isChecked() );
    }
}

void QYoutubeSettingsDialog::retrieveO2Token()
{

    //youtubeController_->doAuth();

    //youtubeAuth_->getComments();
}

void QYoutubeSettingsDialog::loginWebsiteButtonClicked()
{
   //retrieveO2Token();
    emit loginClicked();
}

void QYoutubeSettingsDialog::deloginWebsiteButtonClicked()
{
    //youtubeController_->delogin();
    emit deloginClicked();
}

void QYoutubeSettingsDialog::changeChannelAvailability(bool )
{
    QSettings settings;
    settings.setValue(YOUTUBE_CHANNEL_ENABLE_SETTING_PATH,channelCheckBox->isChecked() );
}


void QYoutubeSettingsDialog::loadSettings(QSettings &settings)
{
    (void)settings;
}

QIcon QYoutubeSettingsDialog::getIcon()
{
    return QIcon( ":/resources/youtubelogo.png" );
}


QString QYoutubeSettingsDialog::getName()
{
    return tr( "Youtube" );
}

QObject *QYoutubeSettingsDialog::getAdditionalInfo()
{
    return nullptr;
}
