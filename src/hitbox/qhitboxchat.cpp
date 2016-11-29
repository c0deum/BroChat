#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QSettings>

#include <QTimerEvent>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "../core/qchatmessage.h"

#include "../core/settingsconsts.h"

#include "../core/qcoremediator.h"

#include "qhitboxchat.h"

ChatServiceRegistrator< QHitBoxChat > hitboxChatRegistrator;

const QString DEFAULT_HITBOX_SERVERS_LIST_LINK = "https://api.hitbox.tv/chat/servers";
const QString DEFAULT_HITBOX_SMILES_INFO_PEFIX = "https://api.hitbox.tv/chat/emotes/";
const QString DEFAULT_HITBOX_STATISTIC_PREFIX = "https://api.hitbox.tv/media/status/";
const QString DEFAULT_HITBOX_SMILES_PREFIX = "http://edge.sf.hitbox.tv";

const QString QHitBoxChat::SERVICE_NAME = "hitbox";
const QString QHitBoxChat::SERVICE_USER_NAME = "HITBOX";

const int QHitBoxChat::RECONNECT_INTERVAL = 10000;
const int QHitBoxChat::STATISTIC_INTERVAL = 10000;
const int QHitBoxChat::SAVE_CONNECTION_INTERVAL = 25000;

QHitBoxChat::QHitBoxChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QHitBoxChat::~QHitBoxChat()
{
    disconnect();
}

void QHitBoxChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    servers_.clear();
    loadServersList();
}

void QHitBoxChat::disconnect()
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

void QHitBoxChat::reconnect()
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

void QHitBoxChat::loadServersList()
{
    QNetworkRequest request( QUrl( DEFAULT_HITBOX_SERVERS_LIST_LINK + "" ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onServersListLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onServersListLoadError() ) );
}

void QHitBoxChat::onServersListLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError ) ;

    if( QJsonParseError::NoError == parseError.error  )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray jsonArray = jsonDoc.array();

            if( jsonArray.isEmpty() )
            {                
                startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
                return;
            }

            int serverNumber = 0;

            foreach( const QJsonValue &value, jsonArray )
            {
                if( value.isObject() )
                {
                    serverNumber++;
                    QJsonObject jsonServer = value.toObject();

                    servers_.append( jsonServer[ "server_ip" ].toString() );
                }
            }

        }
    }

    loadSocketHash();

    reply->deleteLater();
}

void QHitBoxChat::onServersListLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply* >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load servers list..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load servers list..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QHitBoxChat::loadSocketHash()
{
    if( !servers_.isEmpty() )
    {
        QNetworkRequest request( QUrl( "http://" + servers_.first() + "/socket.io/1" ) );
        QNetworkReply * reply = nam_->get( request );
        QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSocketHashLoaded() ) );
        QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSocketHashLoadError() ) );
    }
}

void QHitBoxChat::onSocketHashLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply* >( sender() );

    QString hash = reply->readAll();

    hash = hash.left( hash.indexOf( ':' ) );

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( "ws://" + servers_.first() + "/socket.io/1/websocket/" + hash ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );

    startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );

    reply->deleteLater();
}


void QHitBoxChat::onSocketHashLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load websocket hash..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load websocket hash..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QHitBoxChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_HITBOX_SMILES_INFO_PEFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSmilesLoadError() ) );
}

void QHitBoxChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray jsonSmilesArray = jsonDoc.array();

            foreach( const QJsonValue& smileInfoValue, jsonSmilesArray )
            {
                QJsonObject smileInfo = smileInfoValue.toObject();

                addSmile( smileInfo[ "icon_short" ].toString(), DEFAULT_HITBOX_SMILES_PREFIX + smileInfo[ "icon_path" ].toString() );
                addSmile( smileInfo[ "icon_short_alt" ].toString(), DEFAULT_HITBOX_SMILES_PREFIX + smileInfo[ "icon_path" ].toString() );
            }
        }
    }

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Smiles loaded...", "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Smiles loaded..." );
    }

    reply->deleteLater();

}

void QHitBoxChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    reply->deleteLater();
}

void QHitBoxChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_HITBOX_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );
}

void QHitBoxChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObject = jsonDoc.object();

            QString statistic = jsonObject[ "media_views" ].toString();

            if( "0" == jsonObject[ "media_is_live" ].toString() )
                statistic = "0";

            emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic ) );
        }
    }

    reply->deleteLater();
}

void QHitBoxChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

void QHitBoxChat::onTextMessageReceived( const QString &message )
{
    if( "1::" == message )
    {
        QString response = "5:::{\"name\":\"message\",\"args\":[{\"method\":\"joinChannel\",\"params\":{\"channel\":\"" + channelName_ + "\",\"name\":\"UnknownSoldier\",\"token\":null,\"isAdmin\":false}}]}";
        socket_->sendTextMessage( response );
    }
    else if( "2::" == message )
    {
        socket_->sendTextMessage( "2::" );
    }
    else if( "5:::" == message.left( 4 ) )
    {
        QString jsonData = message.right( message.size() - 4 );

        QJsonParseError parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( jsonData.toStdString().c_str() ), &parseError );

        if( QJsonParseError::NoError == parseError.error )
        {
            if( jsonDoc.isObject() )
            {
                QJsonObject jsonObject = jsonDoc.object();

                if( "message" == jsonObject[ "name" ].toString() )
                {
                    QJsonArray jsonArgs = jsonObject[ "args" ].toArray();

                    foreach( const QJsonValue &value, jsonArgs )
                    {

                        if( value.isString() )
                        {
                            QString argsString = value.toString();

                            QJsonDocument argsDoc = QJsonDocument::fromJson( QByteArray( argsString.toStdString().c_str() ) , &parseError );

                            if( QJsonParseError::NoError == parseError.error )
                            {
                                if( argsDoc.isObject() )
                                {
                                    QJsonObject argsObj = argsDoc.object();

                                    if( "chatMsg" == argsObj[ "method" ].toString()  )
                                    {
                                        QJsonObject jsonParamsObj = argsObj[ "params" ].toObject();

                                        if( jsonParamsObj[ "buffer" ].isBool() )
                                            return;

                                        QString nickName = jsonParamsObj[ "name" ].toString();
                                        QString message = jsonParamsObj[ "text" ].toString();

                                        message = insertSmiles( message );

                                        if( originalColors_ )
                                        {
                                            QString color = "#" + jsonParamsObj[ "nameColor" ].toString();

                                            nickName = "<span style=\"color:" + color + "\">" + nickName + "</span>";
                                        }

                                        emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );

                                    }
                                    else if( "loginMsg" == argsObj[ "method" ].toString() )
                                    {
                                        if( isShowSystemMessages() )
                                        {
                                            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "...", "", this ) );
                                            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "..." );
                                        }

                                        loadSmiles();
                                        loadStatistic();

                                        startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void QHitBoxChat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web Socket Error ..." ) + socket_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web Socket Error ..." ) + socket_->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QHitBoxChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}

void QHitBoxChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        //if( socket_ && socket_->isValid() && socket_->state() == QAbstractSocket::ConnectedState )
        //    socket_->ping();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QHitBoxChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( HITBOX_CHANNEL_SETTING_PATH, DEFAULT_HITBOX_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( HITBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    originalColors_ = settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}


