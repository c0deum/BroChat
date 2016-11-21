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

#include "..\core\qchatmessage.h"

#include "..\core\settingsconsts.h"

#include "qlivecodingchat.h"


const QString DEFAULT_LIVECODING_LOGIN = "broadcasterchat";
const QString DEFAULT_LIVECODING_JID_POSTFIX = "@livecoding.tv";
const QString DEFAULT_LIVECODING_JID = DEFAULT_LIVECODING_LOGIN + DEFAULT_LIVECODING_JID_POSTFIX;
const QString DEFAULT_LIVECODING_PASSWORD = "8e4820297b36ec893f1242bc36ffc1e38";
const QString DEFAULT_LIVECODING_CONFERENCE_JID_POSTFIX = "@chat.livecoding.tv";
const QString DEFAULT_LIVECODING_CANDY_JS_LINK = "https://www.livecoding.tv/static/candy-chat/candy.bundle.js";
//const QString DEFAULT_LIVECODING_SMILES_LINK_PREFIX = "https://www.livecoding.tv/static/candy-chat/img/emoticons_hd/";
const QString DEFAULT_LIVECODING_SMILES_LINK_PREFIX = "https://www.livecoding.tv/static/candy-chat/img/emoticons-dark/";
const QString DEFAULT_LIVECODING_SID_INFO_REQUEST_PREFIX = "https://ws.www.livecoding.tv/live.eio/?EIO=3&transport=polling";
const QString DEFAULT_LIVECODING_WEBSOCKET_LINK = "wss://ws.www.livecoding.tv/live.eio/?EIO=3&transport=websocket";

const QString DEFAULT_LIVECODING_STATISTIC_PREFIX = "https://www.livecoding.tv:443/api/livestreams/";

const QString DEFAULT_LIVECODING_USER_INFO_PREFIX = "https://www.livecoding.tv:443/api/users/";

const int DEFAULT_LIVECODING_STATISTIC_INTERVAL = 10000;

const QString QLivecodingChat::SERVICE_NAME = "livecoding";
const QString QLivecodingChat::SERVICE_USER_NAME = "LIVECODING";

const int QLivecodingChat::RECONNECT_INTERVAL = 10000;
const int QLivecodingChat::RECONNECT_WEB_SOCKET_INTERVAL = 10000;
const int QLivecodingChat::SAVE_WEB_SOCKET_CONNECTION_INTERVAL = 25000;

QLivecodingChat::QLivecodingChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
//, statisticTimerId_( -1 )
//, statisticInterval_( DEFAULT_LIVECODING_STATISTIC_INTERVAL )
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
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

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
    //resetTimer( statisticTimerId_ );

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

    //badgesMap_.clear();

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QLivecodingChat::reconnect()
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
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    //startUniqueTimer( statisticTimerId_, statisticInterval_ );

    loadSmiles();

    loadSid();
}

void QLivecodingChat::onError( QXmppClient::Error )
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QLivecodingChat::onMessageReceived( const QXmppMessage & message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {
        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        if( badges_ )
        {
            LoadUserInfo( nickName );

            if( badgesMap_.contains( nickName ) )
            {
                nickName = "<img class =\"badge\" src=\"" + badgesMap_[ nickName ] + "\"></img>" + nickName;
            }

        }

        QString messageBody = message.body();

        messageBody = ChatMessage::replaceEscapeCharecters( messageBody );

        messageBody = insertSmiles( messageBody );

        emit newMessage( ChatMessage( SERVICE_NAME, nickName, messageBody, QString(), this ) );
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

    qDebug() << webSocketInfoData;

    webSocketInfoData = "{" + webSocketInfoData.right( webSocketInfoData.length() - webSocketInfoData.indexOf( "\"sid\":" ) );

    qDebug() << webSocketInfoData;

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

/*
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
*/

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
    startUniqueTimer( saveWebSocketConnectionTimerId_, SAVE_WEB_SOCKET_CONNECTION_INTERVAL );

    if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->sendTextMessage( "2probe" );
    }
}

void QLivecodingChat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web socket error..." ) + socket_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web socket error..." ) + socket_->errorString() );
    }

    startUniqueTimer( reconnectWebSocketTimerId_, RECONNECT_WEB_SOCKET_INTERVAL );
}

void QLivecodingChat::onWebSocketMessageReceived( const QString & message )
{
    qDebug() << message;
    const QString CHANNEL_MESSAGE_PREFIX = "4[\"channel_msg\"";

    if( "3probe" == message )
    {
        //joinToChannel();

        socket_->sendTextMessage( "5" );

        socket_->sendTextMessage( "4[\"join\",\"stream." + channelName_ + "\"]" );
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

                QString statistic = jsonObj.contains( VIEWS_LIVE )? QString::number( jsonObj[ VIEWS_LIVE ].toInt() ) : "0";

                emit newStatistic ( new QChatStatistic( SERVICE_NAME, statistic, this ) );
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

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
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
    /*
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
    */

    const QString EMOTICONS = "emoticons: {";

    int smilesInfoStart = info.indexOf( EMOTICONS );

    QStringList smilesLists;
    smilesLists << "free: [" << "pro: [" << "kappaFree: [" << "kappaPro: [";

    QStringList smilesPrefixList;

    smilesPrefixList    << "https://www.livecoding.tv/static/candy-chat/img/emoticons-dark/" \
                        << "https://www.livecoding.tv/static/candy-chat/img/emoticons-dark/" \
                        << "https://www.livecoding.tv/static/candy-chat/img/emoticons-kappa/" \
                        << "https://www.livecoding.tv/static/candy-chat/img/emoticons-kappa/" ;

    if( smilesInfoStart != -1 )
    {

        for( int i = 0; i < smilesLists.size(); ++i )
        {
            smilesInfoStart = info.indexOf( smilesLists[ i ], smilesInfoStart ) + smilesLists[ i ].length() - 1;
            int smilesInfoEnd = info.indexOf( ']', smilesInfoStart );



            QString smilesInfo = info.mid( smilesInfoStart, smilesInfoEnd - smilesInfoStart + 1 );

            smilesInfo.replace( '\n', "" );
            smilesInfo.replace( '\t', "" );
            smilesInfo.replace( ' ', ""  );

            smilesInfo.replace( "plain", "\"plain\"" );
            smilesInfo.replace( "regex:", "\"regex\":\"" );
            smilesInfo.replace( "/gim", "/gim\"" );
            smilesInfo.replace( "/gm", "/gm\"" );
            smilesInfo.replace( "image", "\"image\"" );
            smilesInfo.replace( "sprite", "\"sprite\"" );

            QJsonParseError parseError;

            QJsonDocument jsonDoc = QJsonDocument::fromJson( smilesInfo.toStdString().c_str(), &parseError );

            if( QJsonParseError::NoError == parseError.error )
            {
                if( jsonDoc.isArray() )
                {
                    QJsonArray smilesArr = jsonDoc.array();

                    foreach( const QJsonValue &value, smilesArr )
                    {
                        QJsonObject smileInfo = value.toObject();

                        addSmile( smileInfo[ "plain" ].toString(), smilesPrefixList[ i ] + smileInfo[ "image" ].toString() );
                    }
                }
            }


        }
    }







    /*

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

    */

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
    }

    reply->deleteLater();

}

void QLivecodingChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QLivecodingChat::timerEvent( QTimerEvent * event )
{    
    /*if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else*/
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

    badges_ = settings.value( LIVECODING_BADGES_SETTING_PATH, false ).toBool();


    login_ = settings.value( LIVECODING_LOGIN_SETTING_PATH, BLANK_STRING ).toString();
    password_ = settings.value( LIVECODING_PASSWORD_SETTING_PATH, BLANK_STRING ).toString();
}

void QLivecodingChat::LoadUserInfo( const QString & userName )
{
    //TODO: OAuth2

    return;

    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_USER_INFO_PREFIX + userName + "/" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onUserInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onUserInfoLoadError() ) );
}

void QLivecodingChat::onUserInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QByteArray data =  reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( data, &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QString userName = jsonObj[ "username" ].toString();
        QString avatar = jsonObj[ "avatar" ].toString();

        badgesMap_.insert( jsonObj[ "username" ].toString(), jsonObj[ "avatar" ].toString() );

        qDebug() << badgesMap_;
    }

    reply->deleteLater();
}

void QLivecodingChat::onUserInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QLivecodingChat::changeBadges( bool badges )
{
    badges_ = badges;
}

/*
void QLivecodingChat::loadStatistic()
{
    qDebug() << DEFAULT_LIVECODING_STATISTIC_PREFIX + channelName_ + "/?format=json";
    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_STATISTIC_PREFIX + channelName_ + "/?format=json" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );
}

void QLivecodingChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    qDebug() << reply->readAll();

    reply->deleteLater();
}

void QLivecodingChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}
*/
