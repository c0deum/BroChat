#include <QCoreApplication>
#include <QSettings>
#include <QUuid>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include <QXmppClient.h>
#include <QXmppMessage.h>
#include <QXmppPresence.h>
#include <QXmppIq.h>

#include <QXmppMucManager.h>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "qchatmessage.h"

#include "settingsconsts.h"


#include "qcybergamechat.h"

const QString DEFAULT_CYBERGAME_CANDY_JS_LINK = "http://cybergame.tv/js/chatj/candycg2.min.js";
const QString DEFAULT_CYBERGAME_SMILES_PREFIX = "http://cybergame.tv/styles/chatj/img/emoticons/";
const QString DEFAULT_CYBERGAME_CONFERENCE_JID_POSTFIX = "@conference.cybergame.tv";
const QString DEFAULT_CYBERGAME_CHANNEL_INFO_LINK = "http://cybergame.tv/";
const QString DEFAULT_CYBERGAME_STATICTIC_LINK_PREFIX = "http://api.cybergame.tv/w/streams2.php?channel=";

const QString QCyberGameChat::SERVICE_NAME = "cybergame";
const QString QCyberGameChat::SERVICE_USER_NAME = "CYBERGAME";

const int QCyberGameChat::RECONNECT_INTERVAL = 10000;
const int QCyberGameChat::STATISTIC_INTERVAL = 10000;

QCyberGameChat::QCyberGameChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QCyberGameChat::~QCyberGameChat()
{
    disconnect();
}

void QCyberGameChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    loadChannelInfo();

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }
}

void QCyberGameChat::disconnect()
{
    roomId_.clear();

    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );

    if( mucManager_ )
    {
        if( !mucManager_->rooms().empty() )
        {
            mucManager_->rooms().first()->leave();
            mucManager_->rooms().first()->deleteLater();
        }

        mucManager_->deleteLater();
        mucManager_ = nullptr;
    }

    if( xmppClient_ )
    {
        xmppClient_->disconnectFromServer();
        xmppClient_->deleteLater();

        xmppClient_ = nullptr;
    }

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QCyberGameChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Reconnecting...", "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Reconnecting..." );
    }
    connect();
}

void QCyberGameChat::connectToXmpp()
{

    xmppClient_ = new QXmppClient( this );

    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError() ) );
    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );

    mucManager_ = new QXmppMucManager();

    xmppClient_->addExtension( mucManager_ );

    qsrand( QDateTime::currentDateTime().toTime_t() );

    QXmppConfiguration conf;

    conf.setDomain( "cybergame.tv" );
    conf.setIgnoreSslErrors( true );
    conf.setSaslAuthMechanism( "ANONYMOUS" );

    xmppClient_->connectToServer( conf );
}

void QCyberGameChat::onConnected()
{
    connectionTime_ = QDateTime::currentDateTimeUtc();    

    QXmppMucRoom *room = mucManager_->addRoom( roomId_ + DEFAULT_CYBERGAME_CONFERENCE_JID_POSTFIX );

    room->setNickName( "broadcasterchat" + QUuid::createUuid().toString() );
    room->join();

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    loadSmiles();
}

void QCyberGameChat::onError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QCyberGameChat::onMessageReceived( const QXmppMessage &message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {
        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        QString messageBody = message.body();

        int dotPos = messageBody.indexOf( "·" );
        if( -1 != dotPos )
        {
            messageBody = messageBody.right( messageBody.length() - dotPos - 1 );
        }

        messageBody = ChatMessage::replaceEscapeCharecters( messageBody );

        messageBody = insertSmiles( messageBody );

        emit newMessage( ChatMessage( SERVICE_NAME, nickName, messageBody, QString(), this ) );
    }
}

void QCyberGameChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl(  DEFAULT_CYBERGAME_CHANNEL_INFO_LINK + channelName_ + "/" ) );

    QNetworkReply *reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QCyberGameChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QByteArray data = reply->readAll();

    QString ROOM_ID_PREFIX = "http://cybergame.tv/cgchatjjj.html?v=h#";

    int startIDPos = data.indexOf( ROOM_ID_PREFIX ) + ROOM_ID_PREFIX.length();
    int endIDPos = data.indexOf( "\"", startIDPos ) - 1;

    if( endIDPos - startIDPos + 1 > 0 )
    {
        roomId_ = data.mid( startIDPos, endIDPos - startIDPos + 1 );
        connectToXmpp();

        loadStatistic();

        startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );
    }
    else if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to" ) + channelName_ + " ..." , QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to" ) + channelName_ + " ..."  );
    }

    reply->deleteLater();
}

void QCyberGameChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QCyberGameChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_STATICTIC_LINK_PREFIX + channelName_ ) );

    QNetworkReply *reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QCyberGameChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString statistic = jsonObj[ "viewers" ].toString();

            emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
        }
    }

    reply->deleteLater();
}

void QCyberGameChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QCyberGameChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_CANDY_JS_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QCyberGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString response = reply->readAll();

    const QString EMOTICONS = "emoticons:";

    int emotIconsPos = response.indexOf( EMOTICONS );

    if( emotIconsPos )
    {
        int emotIconsArrayStart = emotIconsPos + EMOTICONS.length();

        int emotIconsArrayEnd = response.indexOf( "//", emotIconsArrayStart );

        response = response.mid( emotIconsArrayStart, emotIconsArrayEnd - emotIconsArrayStart );

        response.append( ']' );

        response.replace( "plain", "\"plain\"" );
        response.replace( "regex:", "\"regex\":\"" );
        response.replace( "/gim", "/gim\"" );
        response.replace( "/gm", "/gm\"" );
        response.replace( "image", "\"image\"" );        


        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( response.toStdString().c_str() ), &parseError );
        if( QJsonParseError::NoError == parseError.error )
        {
            if( jsonDoc.isArray() )
            {
                QJsonArray smilesInfoArr = jsonDoc.array();

                foreach( const QJsonValue &value, smilesInfoArr )
                {
                    QJsonObject smileInfo = value.toObject();

                    addSmile( smileInfo[ "plain" ].toString(), DEFAULT_CYBERGAME_SMILES_PREFIX + smileInfo[ "image" ].toString() );

                }
            }
        }
    }

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
    }

    reply->deleteLater();
}


void QCyberGameChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    reply->deleteLater();
}

void QCyberGameChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QCyberGameChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( CYBERGAME_CHANNEL_SETTING_PATH, DEFAULT_CYBERGAME_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    //badges_ = settings.value( CYBERGAME_BADGES_SETTING_PATH, false ).toBool();

    enable( settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QCyberGameChat::changeBadges( bool badges )
{
    badges_ = badges;
}

