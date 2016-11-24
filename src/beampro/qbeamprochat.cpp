#include <QNetworkAccessManager>

#include <QWebSocket>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include <QSettings>

#include "../core/settingsconsts.h"

#include "qbeamprochat.h"

const QString QBeamProChat::SERVICE_NAME = "beampro";
const QString QBeamProChat::SERVICE_USER_NAME = "BEAMPRO";

const int QBeamProChat::RECONNECT_INTERVAL = 10000;
const int QBeamProChat::RECONNECT_CHAT_INTERVAL = 5000;
const int QBeamProChat::SAVE_CONNECTION_INTERVAL = 25000;


const QString DEFAULT_BEAMPRO_SOCKETIO_LINK = "wss://realtime.beam.pro/socket.io/?EIO=3&transport=websocket";



QBeamProChat::QBeamProChat( QObject * parent )
: QChatService( parent )
{
}

QBeamProChat::~QBeamProChat()
{
    disconnect();
}

void QBeamProChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    connectToSocketIO();
}

void QBeamProChat::disconnect()
{
    resetTimer( reconnectTimerId_ );
    resetTimer( reconnectChatTimerId_ );
    resetTimer( saveConnectionTimerId_ );

    if( socketIO_ )
    {
        socketIO_->abort();
        socketIO_->deleteLater();
    }
    socketIO_ = nullptr;

    if( chatSocket_ )
    {
        chatSocket_->abort();
        chatSocket_->deleteLater();
    }
    chatSocket_ = nullptr;

    currentChatId_ = -1;

    currentRequestId_ = -1;
    channelInfoRequestId_ = -1;
    chatsInfoRequestId_ = -1;

    channelId_.clear();

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}


void QBeamProChat::reconnect()
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

void QBeamProChat::timerEvent( QTimerEvent * event )
{
    if( saveConnectionTimerId_ == event->timerId() )
    {
        if( socketIO_ && QAbstractSocket::ConnectedState == socketIO_->state() )
        {
            socketIO_->ping( "2" );
        }
    }
    else if( reconnectChatTimerId_ == event->timerId() )
    {
        connectToChatWebSocket();
    }
    else if( reconnectTimerId_ == event->timerId() )
    {
        reconnect();
    }
}

void QBeamProChat::loadSmiles()
{
    QChatService::loadSmiles();
}

void QBeamProChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( BEAMPRO_CHANNEL_SETTING_PATH, DEFAULT_BEAMPRO_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( BEAMPRO_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( BEAMPRO_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( BEAMPRO_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( BEAMPRO_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( BEAMPRO_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QBeamProChat::connectToSocketIO()
{
    socketIO_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socketIO_->open( QUrl( DEFAULT_BEAMPRO_SOCKETIO_LINK + "" ) );
    QObject::connect( socketIO_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );
    QObject::connect( socketIO_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketConnectError() ) );
}


void QBeamProChat::onWebSocketConnected()
{
}

void QBeamProChat::onWebSocketConnectError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocket Error... " ) + socketIO_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "WebSocketError..." ) + socketIO_->errorString() );
    }
    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QBeamProChat::onTextMessageReceived( const QString & message )
{   
    if( "42[\"channel:" + channelId_ + ":update" == message.left( 19 + channelId_.length() ) )
    {
        int startInfoObjPos = message.indexOf( "{" );
        int endInfoObjPos = message.lastIndexOf( "}" );

        if( endInfoObjPos - startInfoObjPos > -1 )
        {
            QByteArray json = message.mid( startInfoObjPos, endInfoObjPos - startInfoObjPos + 1 ).toUtf8();

            QJsonParseError parseError;

            QJsonDocument jsonDoc = QJsonDocument::fromJson( json, &parseError );

            if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
            {
                QJsonObject jsonObj = jsonDoc.object();

                if( jsonObj.contains( "viewersCurrent" ) )
                    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString::number( jsonObj[ "viewersCurrent" ].toInt() ), this ) );

            }
        }
    }
    else if( "43" + QString::number( channelInfoRequestId_ ) == message.left( QString::number( channelInfoRequestId_ ).length() + 2 ) )
    {
        int startInfoObjPos = message.indexOf( "[" ) + 1;
        int endInfoObjPos = message.lastIndexOf( "]" ) - 1;

        if( endInfoObjPos - startInfoObjPos > -1 )
        {
            QByteArray json = message.mid( startInfoObjPos, endInfoObjPos - startInfoObjPos + 1 ).toUtf8();

            QJsonParseError parseError;

            QJsonDocument jsonDoc = QJsonDocument::fromJson( json, &parseError );

            if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
            {
                QJsonObject jsonObj = jsonDoc.object();

                QJsonObject jsonBody = jsonObj[ "body" ].toObject();

                channelId_ = QString::number( jsonBody[ "id" ].toInt() );

                QString request = "42" + QString::number( chatsInfoRequestId_ = ++currentRequestId_ ) + \

                        "[\"get\",{\"method\":\"get\",\"headers\":{},\"data\":{},\"url\":\"/api/v1/chats/" + channelId_ + "\"}]";


                socketIO_->sendTextMessage( request );

                startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
            }
        }
    }
    else if( "43" + QString::number( chatsInfoRequestId_ ) == message.left( QString::number( chatsInfoRequestId_ ).length() + 2 ) )
    {
        int startInfoObjPos = message.indexOf( "[" ) + 1;
        int endInfoObjPos = message.lastIndexOf( "]" ) - 1;

        if( endInfoObjPos - startInfoObjPos > -1 )
        {
            QByteArray json = message.mid( startInfoObjPos, endInfoObjPos - startInfoObjPos + 1 ).toUtf8();

            QJsonParseError parseError;

            QJsonDocument jsonDoc = QJsonDocument::fromJson( json, &parseError );

            if( QJsonParseError::NoError == parseError.error )
            {
                QJsonObject jsonObj = jsonDoc.object();

                QJsonObject jsonBody = jsonObj[ "body" ].toObject();

                QJsonArray chatsEndPointsArr = jsonBody[ "endpoints" ].toArray();

                chatWebSocketAddrList_.clear();

                for( const QJsonValue chatEndPoint : chatsEndPointsArr )
                {
                    chatWebSocketAddrList_.append( chatEndPoint.toString() );
                }

                QString request = "42" + QString::number( ++currentRequestId_ ) + \

                        "[\"put\",{\"method\":\"put\",\"headers\":{},\"data\":{\"slug\":[\"channel:" + channelId_ + ":update\"]},\"url\":\"/api/v1/live\"}]";

                socketIO_->sendTextMessage( request );

                request = "42" + QString::number( ++currentRequestId_ ) + \

                        "[\"put\",{\"method\":\"put\",\"headers\":{},\"data\":{\"slug\":[\"channel:" + channelId_ + ":subscribed\"]},\"url\":\"/api/v1/live\"}]";

                socketIO_->sendTextMessage( request );



                connectToChatWebSocket();

            }
        }
    }
    else if( "40" == message )
    {
        //422["get",{"method":"get","headers":{},"data":{},"url":"/api/v1/channels/c0deum?"}]

        QString request = "42" + QString::number( channelInfoRequestId_ = ++currentRequestId_ ) + \
                "[\"get\",{\"method\":\"get\",\"headers\":{},\"data\":{},\"url\":\"/api/v1/channels/" + channelName_ + "?\"}]";

        socketIO_->sendTextMessage( request );
    }
}

void QBeamProChat::connectToChatWebSocket()
{
    if( chatSocket_ )
    {
        chatSocket_->abort();
        chatSocket_->deleteLater();
    }
    chatSocket_ = nullptr;

    ++currentChatId_;

    if( currentChatId_ > chatWebSocketAddrList_.length() - 1 )
        currentChatId_ = 0;

    chatSocket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    chatSocket_->open( QUrl( chatWebSocketAddrList_[ currentChatId_ ]  + "" ) );
    QObject::connect( chatSocket_, SIGNAL( connected() ), this, SLOT( onChatWebSocketConnected() ) );
    QObject::connect( chatSocket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onChatTextMessageReceived( const QString & ) ) );
    QObject::connect( chatSocket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onChatWebSocketConnectError() ) );
}

void QBeamProChat::onChatWebSocketConnected()
{    
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }
    chatSocket_->sendTextMessage( "{\"type\":\"method\",\"method\":\"auth\",\"arguments\":[" + channelId_ +",null,null],\"id\":0}" );
}

void QBeamProChat::onChatWebSocketConnectError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Chat WebSoket Error... " ) + chatSocket_->errorString() + "... Reconnecting...", QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Chat WebSoket Error... " ) + chatSocket_->errorString() + "... Reconnecting..." );
    }
    startUniqueTimer( reconnectChatTimerId_, RECONNECT_CHAT_INTERVAL );
}

void QBeamProChat::onChatTextMessageReceived( const QString & message )
{
    //{"type":"event","event":"ChatMessage","data":{"channel":113379,"id":"2734fa80-226a-11e6-945b-e50eceb61ade","user_name":"c0deum","user_id":155908,"user_roles":["Owner"],"message":{"message":[{"type":"text","data":"321"}],"meta":{}}}}

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( message.toUtf8(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QString type = jsonObj[ "type" ].toString();

        if( "event" == type )
        {
            QString event = jsonObj[ "event" ].toString();

            if( "ChatMessage" == event )
            {
                QJsonObject jsonData = jsonObj[ "data" ].toObject();

                QString nickName = jsonData[ "user_name" ].toString();

                QJsonObject jsonMessageObj = jsonData[ "message" ].toObject();
                QJsonArray jsonMessageArr = jsonMessageObj[ "message" ].toArray();

                QString message;

                for( const QJsonValue & jsonMessageVal : jsonMessageArr )
                {
                    QJsonObject jsonMessageInfoObj = jsonMessageVal.toObject();

                    QString type = jsonMessageInfoObj[ "type" ].toString();

                    if( "text" == type )
                    {
                        message += jsonMessageInfoObj[ "data" ].toString();
                    }
                    else if( "link" == type )
                    {
                        message += jsonMessageInfoObj[ "text" ].toString();
                    }
                    else if( "emoticon" == type )
                    {
                        //css sprites must die
                        message += "<img class = \"smile\" src=\"https://funstream.tv/build/images/smiles/anifk.png\"></img>" ;
                    }
                }
                emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
            }
        }
    }
}


