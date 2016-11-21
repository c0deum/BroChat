#include <QNetworkAccessManager>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include <QTimerEvent>

#include <QSettings>

#include "..\core\settingsconsts.h"

#include "qvidichat.h"

const QString DEFAULT_VIDI_CHANNEL_INFO_PREFIX = "http://api.vidi.tv/channels/";

const QString DEFAULT_VIDI_SID_LINK = "http://api.vidi.tv:8080/socket.io/?EIO=3&transport=polling";

const QString DEFAULT_VIDI_JOIN_SOCKET_LINK = "ws://api.vidi.tv:8080/socket.io/?EIO=3&transport=websocket";
const QString DEFAULT_VIDI_SOCKET_LINK = "ws://chat.ottnow.ru:443/chat/websocket";


const QString QVidiChat::SERVICE_NAME = "vidi";
const QString QVidiChat::SERVICE_USER_NAME = "VIDI";

//const int QVidiChat::SAVE_JOIN_SOCKET_CONNECTION_INTERVAL = 10000;
const int QVidiChat::SAVE_SOCKET_CONNECTION_INTERVAL = 10000;
const int QVidiChat::RECONNECT_INTERVAL = 10000;
const int QVidiChat::STATISTIC_INTERVAL = 10000;

QVidiChat::QVidiChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QVidiChat::~QVidiChat()
{
}

void QVidiChat::connect()
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

void QVidiChat::disconnect()
{
    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    /*
    if( joinSocket_ )
    {
        joinSocket_->abort();
        joinSocket_->deleteLater();
    }
    joinSocket_ = nullptr;
    */

    resetTimer( reconnectTimerId_ );
    resetTimer( saveSocketConnectionTimerId_ );

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QVidiChat::reconnect()
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

void QVidiChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_VIDI_CHANNEL_INFO_PREFIX + channelName_ ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelinfoLoadError() ) );
}

void QVidiChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject jsonUser = jsonObj[ "user" ].toObject();

        QJsonObject jsonChannel = jsonUser[ "Channel" ].toObject();

        channelId_ = jsonChannel[ "name" ].toString();


        int delPos = channelId_.indexOf( '_' );

        if( delPos > 0 )
        {
            channelId_ = channelId_.right( channelId_.length() - delPos - 1 );
            connectToSocket();
        }
        else
        {
            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + "...", QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) );
            }

            startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
        }


        //        connectToJoinSocket();
        //loadSid();
    }

    reply->deleteLater();
}

void QVidiChat::onChannelinfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

/*
void QVidiChat::loadSid()
{
    QNetworkRequest request( QUrl( DEFAULT_VIDI_SID_LINK + "" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSidLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSidLoadError() ) );
}

void QVidiChat::onSidLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QByteArray data = reply->readAll();

    int dataSize = data.size();

    const QString SID_PREFIX = "\"sid\":\"";

    int sidStartPos = data.indexOf( SID_PREFIX ) + SID_PREFIX.length();
    if( sidStartPos != -1 )
    {
         int sidEndPos = data.indexOf( "\"", sidStartPos ) - 1;

        if( sidEndPos - sidStartPos + 1 > 0 )
        {
            //sid_ = data.mid( sidStartPos, sidEndPos - sidStartPos + 1 );
            QByteArray test = data.mid( sidStartPos, sidEndPos - sidStartPos + 1 );
            sid_ = test;

            connectToJoinSocket();

            //sidEndPos++;
        }
    }



    reply->deleteLater();
}


void QVidiChat::onSidLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}
*/
void QVidiChat::loadSmiles()
{
}

void QVidiChat::onSmilesLoaded()
{
}

void QVidiChat::onSmilesLoadError()
{
}

/*
void QVidiChat::connectToJoinSocket()
{
    joinSocket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    joinSocket_->open( QUrl( DEFAULT_VIDI_JOIN_SOCKET_LINK + "&sid=" + sid_ ) );

    QObject::connect( joinSocket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onJoinSocketMessageReceived( const QString & ) ) );
    QObject::connect( joinSocket_, SIGNAL( connected() ), this, SLOT( onJoinSocketConnected() ) );
    QObject::connect( joinSocket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onJoinSocketConnectError() ) );
}

void QVidiChat::onJoinSocketConnected()
{
    startUniqueTimer( saveJoinSocketConnectionTimerId_, SAVE_SOCKET_CONNECTION_INTERVAL );
    joinSocket_->sendTextMessage( "2probe" );
}

void QVidiChat::onJoinSocketConnectError()
{
    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QVidiChat::onJoinSocketMessageReceived( const QString & message )
{
    /*
    if( "0" == message.left( 1 ) )
    {
        const QString SID_PREFIX = "\"sid\":\"";

        int sidStartPos = message.indexOf( SID_PREFIX ) + SID_PREFIX.length();
        if( sidStartPos != -1 )
        {
            int sidEndPos = message.indexOf( "\"", sidStartPos );

            if( sidEndPos - sidStartPos + 1 > 0 )
            {
                sid_ = message.mid( sidStartPos, sidEndPos - sidStartPos + 1 );

            }
        }
    }
    */

/*
    if( "3probe" == message )
    {
        joinSocket_->sendTextMessage( "5" );
        joinSocket_->sendTextMessage( "42[\"join_room\",{\"name\":" + channelId_ + "}]" );
        joinSocket_->sendTextMessage( "42[\"authorize\",{\"id\":null}]" );
        joinSocket_->sendTextMessage( "42[\"increment_count\",null]" );

        connectToSocket();
    }

}
*/

void QVidiChat::connectToSocket()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( DEFAULT_VIDI_SOCKET_LINK ) );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onSocketMessageReceived( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onSocketConnetError() ) );
}

void QVidiChat::onSocketConnected()
{
    startUniqueTimer( saveSocketConnectionTimerId_, SAVE_SOCKET_CONNECTION_INTERVAL );
}

void QVidiChat::onSocketConnetError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocket Error... " ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocket Error..." ));
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QVidiChat::onSocketMessageReceived( const QString & message )
{   
    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( message.toUtf8(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QString type = jsonObj[ "type" ].toString();

        if( "message" == type )
        {
            //{\"type\":\"message\",\"data\":{\"channel_id\":1024,\"user_id\":1016,\"user_name\":\"c0deum\",\"user_rights\":20,\"user_groups\":[],\"hideIcon\":0,\"icon\":\"\",\"color\":\"\",\"isStatus\":0,\"mobile\":0,\"paidsmiles\":{\"0\":[],\"1024\":[]},\"message_id\":3,\"timestamp\":1460203043,\"text\":\"123\"}}

            QJsonObject jsonMessageInfo = jsonObj[ "data" ].toObject();

            QString nickName = jsonMessageInfo[ "user_name" ].toString();
            QString messageText = jsonMessageInfo[ "text" ].toString();

            messageText = ChatMessage::deleteTags( messageText );

            emit newMessage( ChatMessage( SERVICE_NAME, nickName, messageText, QString(), this ) );
        }
        else if( "welcome" == type )
        {
            socket_->sendTextMessage( "{\"type\":\"auth\",\"data\":{\"token\":\"undefined\",\"site_id\":2}}" );
        }
        else if( "success_auth" == type )
        {
            socket_->sendTextMessage( "{\"type\":\"unjoin\",\"data\":{\"channel_id\":\"" + channelId_ +"\"}}" );
            socket_->sendTextMessage( "{\"type\":\"join\",\"data\":{\"channel_id\":\"" + channelId_ + "\",\"hidden\":false,\"mobile\":0}}" );
        }
        else if( "success_join" == type )
        {
            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + "...", QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
            }

            loadStatistic();
            startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );

            startUniqueTimer( saveSocketConnectionTimerId_, SAVE_SOCKET_CONNECTION_INTERVAL );
        }
    }

    //{\"type\":\"get_channel_counters\",\"data\":{\"channel_id\":1024}}
    //{\"type\":\"get_channel_history\",\"data\":{\"channel_id\":1024,\"from\":0}}

    //

    //{\"type\":\"channel_counters\",\"data\":{\"channel_id\":\"1024\",\"clients_in_channel\":1,\"users_in_channel\":0}}
    //{\"type\":\"message\",\"data\":{\"channel_id\":1024,\"user_id\":1016,\"user_name\":\"c0deum\",\"user_rights\":20,\"user_groups\":[],\"hideIcon\":0,\"icon\":\"\",\"color\":\"\",\"isStatus\":0,\"mobile\":0,\"paidsmiles\":{\"0\":[],\"1024\":[]},\"message_id\":3,\"timestamp\":1460203043,\"text\":\"123\"}}
}

void QVidiChat::loadStatistic()
{
    QNetworkRequest request( DEFAULT_VIDI_CHANNEL_INFO_PREFIX + channelName_ );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT(onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );
}

void QVidiChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject jsonUser = jsonObj[ "user" ].toObject();

        QJsonObject jsonChannel = jsonUser[ "Channel" ].toObject();

        QString statistic = QString::number( jsonChannel[ "viewers" ].toInt() );

        if( !jsonChannel[ "online" ].toBool() )
            statistic = "0";


        emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
    }

    reply->deleteLater();
}

void QVidiChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}


void QVidiChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( VIDI_CHANNEL_SETTING_PATH, DEFAULT_VIDI_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( VIDI_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( VIDI_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( VIDI_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( VIDI_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( VIDI_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QVidiChat::timerEvent( QTimerEvent * event )
{
    if( reconnectTimerId_ == event->timerId() )
    {
        reconnect();
    }
    else if( statisticTimerId_ == event->timerId() && socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        //socket_->sendTextMessage( "{\"type\":\"get_channel_counters\",\"data\":{\"channel_id\":" + channelId_ +"}}" );
        loadStatistic();
    }
    else if( saveSocketConnectionTimerId_ == event->timerId() && socket_ && socket_->state() == QAbstractSocket::ConnectedState )
    {
        //socket_->sendTextMessage( "[\"{\"type\":\"ping\",\"data\":{}}\"]" );
        socket_->sendTextMessage( "{\"type\":\"get_channel_counters\",\"data\":{\"channel_id\":" + channelId_ +"}}" );
    }
    /*
    else if( saveJoinSocketConnectionTimerId_ == event->timerId() && joinSocket_ && joinSocket_->state() == QAbstractSocket::ConnectedState )
    {
        //joinSocket_->sendTextMessage( "2" );
        joinSocket_->ping( "2" );
    }
    */

}
