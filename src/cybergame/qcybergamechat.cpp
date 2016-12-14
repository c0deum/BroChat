#include <QCoreApplication>
#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "../core/qchatmessage.h"

#include "../core/settingsconsts.h"


#include "qcybergamechat.h"

const QString DEFAULT_CYBERGAME_CHANNEL_INFO_LINK = "https://cybergame.tv/";
const QString DEFAULT_CYBERGAME_SMILES_INFO_LINK = "https://cybergame.tv/chats/emotes/emotes.json";
const QString DEFAULT_CYBERGAME_SMILES_PREFIX = "https://cybergame.tv/chats/emotes/";

const QString DEFAULT_CYBERGAME_STATICTIC_LINK_PREFIX = "https://api.cybergame.tv/w/streams2.php?channel=";

//const QString DEFAULT_CYBERGAME_WEBSOCKET_LINK = "ws://newchat.cybergame.tv:9002/";
const QString DEFAULT_CYBERGAME_WEBSOCKET_LINK = "wss://chat.cybergame.tv:9002/";

const QString QCyberGameChat::SERVICE_NAME = "cybergame";
const QString QCyberGameChat::SERVICE_USER_NAME = "CYBERGAME";

const int QCyberGameChat::RECONNECT_INTERVAL = 10000;
const int QCyberGameChat::STATISTIC_INTERVAL = 10000;
const int QCyberGameChat::SAVE_CONNECTION_INTERVAL = 25000;

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

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
}

void QCyberGameChat::disconnect()
{    
    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );
    resetTimer( saveConnectionTimerId_ );

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

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

void QCyberGameChat::connectToWebSocket()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( DEFAULT_CYBERGAME_WEBSOCKET_LINK ) );
    QObject::connect( socket_, SIGNAL( sslErrors( QList<QSslError> ) ), socket_, SLOT( ignoreSslErrors() ) );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
}

void QCyberGameChat::onWebSocketConnected()
{   
    socket_->sendTextMessage( "{\"type\":\"join\",\"data\":{\"cid\":\"" + channelId_ +"\"}}" );

    loadSmiles();
}

void QCyberGameChat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ) + socket_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ) + socket_->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QCyberGameChat::onTextMessageReceived( const QString & message )
{
    QByteArray messageData = message.toUtf8();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( messageData, &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString messageType = jsonObj[ "type" ].toString();

            if( "state" == messageType )
            {
                QJsonObject dataObj = jsonObj[ "data" ].toObject();

                if( 2 == dataObj[ "state" ].toInt() )
                {
                    if( isShowSystemMessages() )
                    {
                        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
                        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
                    }

                    startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
                }
            }
            else if ( "msg" == messageType )
            {
                QJsonObject dataObj = jsonObj[ "data" ].toObject();

                QString nickName = dataObj[ "nickname" ].toString();

                QString messageText;

                QJsonArray messagePartsArr = dataObj[ "message" ].toArray();

                foreach( const QJsonValue & messagePart, messagePartsArr )
                {
                    QJsonObject messagePartObj = messagePart.toObject();

                    QString messagePartType = messagePartObj[ "type" ].toString();

                    if( messagePartType == "text" )
                    {
                        messageText += messagePartObj[ "text" ].toString();
                    }
                    else if ( messagePartType == "emote" )
                    {
                        messageText += messagePartObj[ "image" ].toString();
                    }
                    else if ( messagePartType == "link" )
                    {
                        messageText += messagePartObj[ "text" ].toString();
                    }
                }

                messageText = insertSmiles( messageText );

                emit newMessage( ChatMessage( SERVICE_NAME, nickName, messageText, QString(), this ) );
            }
        }
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

    QString CHANNEL_ID_PREFIX = "index.html#";

    int startIDPos = data.indexOf( CHANNEL_ID_PREFIX ) + CHANNEL_ID_PREFIX.length();
    int endIDPos = data.indexOf( "\"", startIDPos ) - 1;

    if( endIDPos - startIDPos + 1 > 0 )
    {        
        channelId_ = data.mid( startIDPos, endIDPos - startIDPos + 1 );

        connectToWebSocket();

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

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to" ) + channelName_ + " ..." , QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to" ) + channelName_ + " ..."  );    
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL);

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

    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_SMILES_INFO_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QCyberGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );    

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        QJsonObject jsonObj = jsonDoc.object();

        //сика

        QStringList smilesCatList;

        smilesCatList << "global" << "premium";

        foreach( const QString & smilesCat, smilesCatList )
        {
            QJsonArray jsonSmilesArray = jsonObj[ smilesCat ].toArray();

            foreach( const QJsonValue & smileInfoValue, jsonSmilesArray )
            {
                QJsonObject smileInfo = smileInfoValue.toObject();

                addSmile( smileInfo[ "image" ].toString(), DEFAULT_CYBERGAME_SMILES_PREFIX + smileInfo[ "image" ].toString() );
            }
        }

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
        }
    }
    else if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) );
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
    if( event->timerId() == saveConnectionTimerId_ && socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->ping();
    }
    else if( event->timerId() == statisticTimerId_ )
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

