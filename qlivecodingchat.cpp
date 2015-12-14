#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


#include <QSettings>
#include <QTimerEvent>

#include <QXmppMucManager.h>
#include <QXmppMessage.h>

#include <QXmppStanza.h>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include <QUuid>

#include "qchatmessage.h"

#include "settingsconsts.h"

#include "qlivecodingchat.h"


const QString DEFAULT_LIVECODING_LOGIN = "broadcasterchat";
const QString DEFAULT_LIVECODING_JID_POSTFIX = "@livecoding.tv";
const QString DEFAULT_LIVECODING_JID = DEFAULT_LIVECODING_LOGIN + DEFAULT_LIVECODING_JID_POSTFIX;
const QString DEFAULT_LIVECODING_PASSWORD = "8e4820297b36ec893f1242bc36ffc1e38";
const QString DEFAULT_LIVECODING_CONFERENCE_JID_POSTFIX = "@chat.livecoding.tv";
const QString DEFAULT_LIVECODING_CANDY_JS_LINK = "https://www.livecoding.tv/static/candy-chat/candy.bundle.js";
const QString DEFAULT_LIVECODING_SMILES_LINK_PREFIX = "https://www.livecoding.tv/static/candy-chat/img/emoticons_hd/";
const QString DEFAULT_LIVECODING_SID_INFO_REQUEST_PREFIX = "https://www.livecoding.tv/live.eio/?EIO=3&transport=polling";
const QString DEFAULT_LIVECODING_WEBSOCKET_LINK = "wss://www.livecoding.tv/live.eio/?EIO=3&transport=websocket";

const int DEFAULT_LIVECODING_RECONNECT_INTERVAL = 10000;
const int DEFAULT_LIVECODING_RECONNECT_WEBSOCKET_INTERVAL = 10000;
const int DEFAULT_LIVECODING_SAVE_WEBSOCKET_CONNECTION_INTERVAL = 25000;

const QString LIVECODING_USER ="LIVECODING";
const QString LIVECODING_SERVICE = "livecoding";

QLivecodingChat::QLivecodingChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, xmppClient_( nullptr )
, mucManager_( nullptr )
, socket_( nullptr )
, sid_()
, channelName_()
, login_()
, password_()
, connectionTime_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_LIVECODING_RECONNECT_INTERVAL )
, reconnectWebSocketTimerId_( -1 )
, reconnectWebSocketInterval_( DEFAULT_LIVECODING_RECONNECT_WEBSOCKET_INTERVAL )
, saveWebSocketConnectionTimerId_( -1 )
, saveWebSocketConnectionInterval_( DEFAULT_LIVECODING_SAVE_WEBSOCKET_CONNECTION_INTERVAL )
{
}

QLivecodingChat::~QLivecodingChat()
{
    disconnect();
}

void QLivecodingChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    login_.clear();
    password_.clear();

    qsrand( QDateTime::currentDateTime().toTime_t() );

    xmppClient_ = new QXmppClient( this );

    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError( QXmppClient::Error ) ) );
    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );

    mucManager_ = new QXmppMucManager();

    xmppClient_->addExtension( mucManager_ );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );

    QXmppConfiguration conf;

    if( !login_.isEmpty() )
    {
        conf.setJid( login_ + DEFAULT_LIVECODING_JID_POSTFIX );
        conf.setPassword( password_ );
        conf.setResource( login_ + QUuid::createUuid().toString() );
    }
    else
    {
        conf.setJid( DEFAULT_LIVECODING_JID );
        conf.setPassword( DEFAULT_LIVECODING_PASSWORD );
        conf.setResource( DEFAULT_LIVECODING_LOGIN + QUuid::createUuid().toString() );
    }
    xmppClient_->connectToServer( conf );
}

void QLivecodingChat::disconnect()
{    
    resetTimer( reconnectTimerId_ );
    resetTimer( reconnectWebSocketTimerId_ );
    resetTimer( saveWebSocketConnectionTimerId_ );

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

    disconnectFromWebSocket();
}

void QLivecodingChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Reconnecting..." ), QString(), this ) );
    connect();
}

void QLivecodingChat::onConnected()
{
    connectionTime_ = QDateTime::currentDateTimeUtc();

    QXmppMucRoom * room = mucManager_->addRoom( channelName_ + DEFAULT_LIVECODING_CONFERENCE_JID_POSTFIX );

    if( !login_.isEmpty() )
        room->setNickName( login_ );
    else
        room->setNickName( DEFAULT_LIVECODING_LOGIN );
    room->join();

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );

    loadSmiles();

    loadSid();
}

void QLivecodingChat::onError( QXmppClient::Error )
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Unknown Error ..." ), QString(), this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}

void QLivecodingChat::onMessageReceived( const QXmppMessage & message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {
        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        QString messageBody = message.body();

        messageBody = ChatMessage::replaceEscapeCharecters( messageBody );

        messageBody = insertSmiles( messageBody );

        emit newMessage( ChatMessage( LIVECODING_SERVICE, nickName, messageBody, QString(), this ) );
    }
}

void QLivecodingChat::loadSid()
{
    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_SID_INFO_REQUEST_PREFIX + "" )  );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSidLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSidLoadError() ) );
}

void QLivecodingChat::onSidLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QByteArray webSocketInfoData = reply->readAll();

    webSocketInfoData = "{" + webSocketInfoData.right( webSocketInfoData.length() - webSocketInfoData.indexOf( "\"sid\":" ) );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( webSocketInfoData, &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();
        sid_ = jsonObj[ "sid" ].toString();

        connectToWebSocket();
    }
    reply->deleteLater();
}

void QLivecodingChat::onSidLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QLivecodingChat::joinToChannel()
{
    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_SID_INFO_REQUEST_PREFIX + "&sid=" + sid_ ) );

    QByteArray data = QString(  "30:4[\"join\",\"stream." + channelName_ + "\"]" ).toUtf8();

    request.setHeader( QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8" );

    QNetworkReply * reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onJoinToChannelReplyLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onJoinToChannelReplyLoadError() ) );
}


void QLivecodingChat::onJoinToChannelReplyLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        socket_->sendTextMessage( "5" );

    reply->deleteLater();
}

void QLivecodingChat::onJoinToChannelReplyLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}


void QLivecodingChat::connectToWebSocket()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onWebSocketMessageReceived( const QString & ) ) );

    socket_->open( QUrl( DEFAULT_LIVECODING_WEBSOCKET_LINK + "&sid=" + sid_ ) );
}

void QLivecodingChat::onWebSocketConnected()
{       
    startUniqueTimer( saveWebSocketConnectionTimerId_, saveWebSocketConnectionInterval_ );

    if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->sendTextMessage( "2probe" );
    }
}

void QLivecodingChat::onWebSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Web socket error..." ) + socket_->errorString(), QString(), this ) );

    startUniqueTimer( reconnectWebSocketTimerId_, reconnectWebSocketInterval_ );
}

void QLivecodingChat::onWebSocketMessageReceived( const QString & message )
{
    const QString CHANNEL_MESSAGE_PREFIX = "4[\"channel_msg\"";

    if( "3probe" == message )
    {
        joinToChannel();

        //socket_->sendTextMessage( "5" );
    }
    else if( CHANNEL_MESSAGE_PREFIX == message.left( CHANNEL_MESSAGE_PREFIX.length() )  )
    {
        int infoStartPos = message.indexOf( "{" );
        int infoEndPos = message.indexOf( "}" );

        if( infoEndPos - infoStartPos + 1 > 0 )
        {
            QByteArray infoData = message.mid( infoStartPos, infoEndPos - infoStartPos + 1 ).toUtf8();

            QJsonParseError parseError;

            QJsonDocument jsonDoc = QJsonDocument::fromJson( infoData, &parseError );

            if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
            {
                QJsonObject jsonObj = jsonDoc.object();

                const QString VIEWS_LIVE = "views_live";

                if( jsonObj.contains( VIEWS_LIVE ) )
                    emit newStatistic ( new QChatStatistic( LIVECODING_SERVICE, QString::number( jsonObj[ VIEWS_LIVE ].toInt() ), this ) );
            }

        }
    }
    else
    {        
    }
}

void QLivecodingChat::disconnectFromWebSocket()
{
    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    emit newStatistic( new QChatStatistic( LIVECODING_SERVICE, QString(), this ) );
}

void QLivecodingChat::reconnectToWebSocket()
{
    disconnectFromWebSocket();
    loadSid();
}

void QLivecodingChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_CANDY_JS_LINK + "" ) );

    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QLivecodingChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString info = reply->readAll();

    //говно говно код
    const QString EMOTICONS = "emoticons: [";

    int smilesInfoStart = info.indexOf( EMOTICONS ) + EMOTICONS.length() - 1;
    int smilesInfoEnd = info.indexOf( ']', smilesInfoStart );

    info = info.mid( smilesInfoStart, smilesInfoEnd - smilesInfoStart + 1 );

    info.replace( '\n', "" );
    info.replace( '\t', "" );
    info.replace( ' ', ""  );

    info.replace( "plain", "\"plain\"" );
    info.replace( "regex:", "\"regex\":\"" );
    info.replace( "/gim", "/gim\"" );
    info.replace( "/gm", "/gm\"" );
    info.replace( "image", "\"image\"" );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( info.toStdString().c_str(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray smilesArr = jsonDoc.array();

            foreach( const QJsonValue &value, smilesArr )
            {
                QJsonObject smileInfo = value.toObject();

                addSmile( smileInfo[ "plain" ].toString(), DEFAULT_LIVECODING_SMILES_LINK_PREFIX + smileInfo[ "image" ].toString() );
            }
        }
    }

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( LIVECODING_SERVICE, LIVECODING_USER, tr( "Smiles loaded..." ), QString(), this ) );

    reply->deleteLater();

}

void QLivecodingChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QLivecodingChat::timerEvent( QTimerEvent * event )
{    
    if( event->timerId() == saveWebSocketConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            socket_->sendTextMessage( "2" );
        }
    }
    else if( event->timerId() == reconnectWebSocketTimerId_ )
    {
        reconnectToWebSocket();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QLivecodingChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( LIVECODING_CHANNEL_SETTING_PATH, DEFAULT_LIVECODING_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
    {
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/", -2 ) - 1 );
        channelName_ = channelName_.left( channelName_.length() - 1 );
    }

    enable( settings.value( LIVECODING_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( LIVECODING_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( LIVECODING_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );

    login_ = settings.value( LIVECODING_LOGIN_SETTING_PATH, BLANK_STRING ).toString();
    password_ = settings.value( LIVECODING_PASSWORD_SETTING_PATH, BLANK_STRING ).toString();
}
