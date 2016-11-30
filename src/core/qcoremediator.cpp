#include "qchatservice.h"

#include "../aces/qaceschat.h"
#include "../azubu/qazubuchat.h"
#include "../beampro/qbeamprochat.h"
#include "../cybergame/qcybergamechat.h"
#include "../gamerstv/qgamerstvchat.h"
#include "../gipsyteam/qgipsyteamchat.h"
#include "../goodgame/qgoodgamechat.h"
#include "../hitbox/qhitboxchat.h"
#include "../igdc/qigdcchat.h"
#include "../livecoding/qlivecodingchat.h"
#include "../peka2tv/qpeka2chat.h"
#include "../streamcube/qstreamcubechat.h"
#include "../twitch/qtwitchchat.h"
#include "../vidi/qvidichat.h"
#include "../youtube/qyoutubechatex.h"

#include "chattypeenum.h"

#include "qsettingsdialog.h"

#include "qcoremediator.h"


QCoreMediator * QCoreMediator::self_ = nullptr;

QCoreMediator::QCoreMediator( QObject *parent )
    : QObject( parent )
    , acesChat_( new QAcesChat( this ) )
    , azubuChat_( new QAzubuChat( this ) )
    , beamproChat_( new QBeamProChat( this ) )
    , cybergameChat_( new QCyberGameChat( this ) )
    , gamerstvChat_( new QGamersTvChat( this ) )
    , gipsyteamChat_( new QGipsyTeamChat( this ) )
    , goodgameChat_(new QGoodGameChat( this ) )
    , hitboxChat_( new QHitBoxChat( this ) )
    , igdcChat_( new QIgdcChat( this ) )
    , livecodingChat_( new QLivecodingChat( this ) )
    , peka2Chat_( new QPeka2Chat( this ) )
    , streamcubeChat_( new QStreamCubeChat( this ) )
    , twitchChat_( new QTwitchChat( this ) )
    , vidiChat_( new QVidiChat( this ) )
    , youtubeChat_( new QYoutubeChatEx( this ) )
{
    registerChatService( acesChat_ );
    registerChatService( azubuChat_ );
    registerChatService( beamproChat_ );
    registerChatService( cybergameChat_ );
    registerChatService( gamerstvChat_ );
    registerChatService( gipsyteamChat_ );
    registerChatService( goodgameChat_ );
    registerChatService( hitboxChat_ );
    registerChatService( igdcChat_ );
    registerChatService( livecodingChat_ );
    registerChatService( peka2Chat_ );
    registerChatService( streamcubeChat_ );
    registerChatService( twitchChat_ );
    registerChatService( vidiChat_ );
    registerChatService( youtubeChat_ );

    QObject::connect( this, SIGNAL( reconnectAcesChat() ), acesChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectAzubuChat() ), azubuChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectBeamproChat() ), beamproChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectCybergameChat() ), cybergameChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectGamerstvChat() ), gamerstvChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectGipsyteamChat() ), gipsyteamChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectGoodgameChat() ), goodgameChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectHitboxChat() ), hitboxChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectIgdcChat() ), igdcChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectLivecodingChat() ), livecodingChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectPeka2Chat() ), peka2Chat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectStreamcubeChat() ), streamcubeChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectTwitchChat() ), twitchChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectVidiChat() ), vidiChat_, SLOT( reconnect() ) );
    QObject::connect( this, SIGNAL( reconnectYoutubeChat() ), youtubeChat_, SLOT( reconnect() ) );
}

QCoreMediator & QCoreMediator::instance()
{
    if( !self_ )
        self_ = new QCoreMediator();
    return *self_;
}

void QCoreMediator::release()
{
    delete self_;
    self_ = nullptr;
}

//void QCoreMediator::registerChatService( QSharedPointer<QChatService> service )
void QCoreMediator::registerChatService( QChatService* service )
{
    chatServices_.push_back( service );

    QObject::connect( service, SIGNAL( newMessage( ChatMessage ) ), &model_, SLOT( add( ChatMessage ) ) );
    QObject::connect( this, SIGNAL( reconnect() ), service, SLOT( reconnect() ) );

}

QMessagesModel * QCoreMediator::messagesModel()
{
    return &model_;
}

void QCoreMediator::showSettings()
{
    if( !settingsDialog_ )
    {
        settingsDialog_ = QSharedPointer< QSettingsDialog >( new QSettingsDialog() );

        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, acesChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, azubuChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, beamproChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, cybergameChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, gamerstvChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, gipsyteamChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, goodgameChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, hitboxChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, igdcChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, livecodingChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, peka2Chat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, streamcubeChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, twitchChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, vidiChat_ );
        settingsDialog_->connectDialogToChat( ChatTypeEnum::Aces, youtubeChat_ );

    }
    settingsDialog_.data()->show();
}

void QCoreMediator::onReconnect()
{
    emit reconnect();
}
