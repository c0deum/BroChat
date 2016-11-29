#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include <QSettings>

#include <QTimerEvent>

#include <QJsonObject>

#include <QPair>

#include "../core/qchatmessage.h"

#include "../core/settingsconsts.h"

#include "../core/qcoremediator.h"

#include "qaceschat.h"

ChatServiceRegistrator< QAcesChat > acesChatRegistrator;

const QString DEFAULT_ACES_CHANNEL_INFO_PREFIX = "http://aces.gg/streams/stream/";

const QString DEFAULT_ACES_WEBSOCKET_LINK = "ws://93.186.192.53:3000/echo/websocket";

const QString QAcesChat::SERVICE_USER_NAME = "ACES";
const QString QAcesChat::SERVICE_NAME = "aces";

const int QAcesChat::RECONNECT_INTERVAL = 10000;
const int QAcesChat::SAVE_CONNECTION_INTERVAL = 10000;

QAcesChat::QAcesChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QAcesChat::~QAcesChat()
{
    disconnect();
}

void QAcesChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
}

void QAcesChat::disconnect()
{
    channelName_.clear();
    channelId_.clear();

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    resetTimer( reconnectTimerId_ );
    resetTimer( saveConnectionId_ );
}

void QAcesChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ) );
    }
    connect();
}

void QAcesChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_ACES_CHANNEL_INFO_PREFIX + channelName_ + "/" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QAcesChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString channelInfo = reply->readAll();

    const QString DATA_CHAT_ID = "data-chat_id=\"";

    int idPosStart = channelInfo.indexOf( DATA_CHAT_ID ) + DATA_CHAT_ID.length();

    int idPosEnd = channelInfo.indexOf( "\"", idPosStart ) - 1;

    if( idPosEnd - idPosStart + 1 > 0 )
    {
        channelId_ = channelInfo.mid( idPosStart, idPosEnd - idPosStart + 1 );

        connectToWebSocket();
    }
    else
    {
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not find channel_id for channel " ) + channelName_ + tr( "..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not find channel_id for channel " ) + channelName_ + tr( "..." ) );
        }
    }

    reply->deleteLater();
}

void QAcesChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QAcesChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == saveConnectionId_ && socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->ping();
    }
    if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QAcesChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
    {
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/", -2 ) - 1 );
        channelName_ = channelName_.left( channelName_.length() - 1 );
    }

    enable( settings.value( ACES_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QAcesChat::connectToWebSocket()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );

    socket_->open( QUrl( DEFAULT_ACES_WEBSOCKET_LINK ) );
}

void QAcesChat::onWebSocketConnected()
{
    socket_->sendTextMessage( "{\"room\":" + channelId_ + ",\"act\":\"joinserver\",\"user_id\":0,\"pass\":\"\",\"reconnected\":1}" );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    startUniqueTimer( saveConnectionId_, SAVE_CONNECTION_INTERVAL );
}

void QAcesChat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocket Error..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocket Error..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QAcesChat::onTextMessageReceived( const QString & message )
{
//a["{\"room\":\"52\",\"act\":\"chat\",\"msTime\":\"2016-04-09T16:23:59.531Z\",\"person\":{\"name\":\"c0deum\",\"user_id\":35859,\"u_lvl\":3},\"msg\":\"123\"}"]

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( message.toUtf8(), & parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        if( jsonObj[ "room" ].toString() == channelId_ && "chat" == jsonObj[ "act" ].toString() )
        {
            QJsonObject jsonPerson = jsonObj[ "person" ].toObject();

            QString nickName = jsonPerson[ "name" ].toString();
            QString textMessage = jsonObj[ "msg" ].toString();

            textMessage.replace( "<img src=\"/uploads/hdgstournament/smiley/", "<img class = \"smile\" src=\"http://aces.gg/uploads/hdgstournament/smiley/" );

            emit newMessage( ChatMessage( SERVICE_NAME, nickName, textMessage, QString(), this ) );
        }
    }
}
