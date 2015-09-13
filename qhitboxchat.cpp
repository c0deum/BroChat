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

#include "settingsconsts.h"

#include "qhitboxchat.h"

const QString DEFAULT_HITBOX_SERVERS_LIST_LINK = "https://api.hitbox.tv/chat/servers";
const QString DEFAULT_HITBOX_SMILES_INFO_PEFIX = "https://api.hitbox.tv/chat/emotes/";
const QString DEFAULT_HITBOX_STATISTIC_PREFIX = "https://api.hitbox.tv/media/status/";
const QString DEFAULT_HITBOX_SMILES_PREFIX = "http://edge.sf.hitbox.tv";

const int DEFAULT_HITBOX_RECONNECT_INTERVAL = 10000;
const int DEFAULT_HITBOX_STATISTIC_INTERVAL = 10000;
const int DEFAULT_HITBOX_SAVE_CONNECTION_INTERVAL = 25000;

const QString HITBOX_SERVICE = "hitbox";
const QString HITBOX_USER = "HITBOX";

QHitBoxChat::QHitBoxChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( 0 )
, channelName_( DEFAULT_HITBOX_CHANNEL_NAME )
, servers_()
, smiles_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_HITBOX_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_HITBOX_STATISTIC_INTERVAL )
, saveConnectionTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_HITBOX_SAVE_CONNECTION_INTERVAL )
, originalColors_( false )
{
}

QHitBoxChat::~QHitBoxChat()
{
    disconnect();
}

void QHitBoxChat::connect()
{
    if( channelName_ == "" )
        return;

    smiles_.clear();
    getSmiles();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Connecting to " + channelName_ + "...", "", this ) );

    servers_.clear();
    getServersList();

}

void QHitBoxChat::disconnect()
{
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }
    if( statisticTimerId_ >= 0 )
    {
        killTimer( statisticTimerId_ );
        statisticTimerId_ = -1;
    }

    if( saveConnectionTimerId_ >= 0 )
    {
        killTimer( saveConnectionTimerId_ );
        saveConnectionTimerId_ = -1;
    }

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = 0;

    emit newStatistic( new QChatStatistic( HITBOX_SERVICE, "", this ) );
}

void QHitBoxChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Reconnecting...", "", this ) );
    connect();
}

void QHitBoxChat::getServersList()
{
    QNetworkRequest request( QUrl( DEFAULT_HITBOX_SERVERS_LIST_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onServersListLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onServersListLoadError() ) );
}

void QHitBoxChat::onServersListLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(),&parseError ) ;

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray jsonArray = jsonDoc.array();

            if( jsonArray.size() == 0 )
            {
                if( reconnectTimerId_ == -1 )
                    reconnectTimerId_ = startTimer( reconnectInterval_ );
                return;
            }

            int serverNumber = 0;

            foreach( const QJsonValue &value, jsonArray )
            {
                if( value.isObject() )
                {
                    serverNumber++;
                    QJsonObject jsonServer = value.toObject();

                    //servers_.append( "ws://" + jsonServer[ "server_ip" ].toString() + "/socket.io/1/websocket" );
                    //servers_.append( "ws://" + jsonServer[ "server_ip" ].toString() );

                    servers_.append( jsonServer[ "server_ip" ].toString() );

                    //qDebug() << jsonServer[ "server_ip" ].toString();
                }
            }

        }
    }

    getSocketHash();

    reply->deleteLater();
}

void QHitBoxChat::onServersListLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Can not load servers list..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

    reply->deleteLater();
}

void QHitBoxChat::getSocketHash()
{
    if( !servers_.isEmpty() )
    {
        QNetworkRequest request( QUrl( "http://" + servers_.first() + "/socket.io/1" ) );
        QNetworkReply *reply = nam_->get( request );
        QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSocketHashLoaded() ) );
        QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSocketHashLoadError() ) );
    }
}

void QHitBoxChat::onSocketHashLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    QString hash = reply->readAll();

    //qDebug() << hash;

    hash = hash.left( hash.indexOf( ':' ) );

    //qDebug() << hash;

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( "ws://" + servers_.first() + "/socket.io/1/websocket/" + hash ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );

    //test ping-pong support
    QObject::connect( socket_, SIGNAL( pong( quint64,QByteArray ) ), this, SLOT( onPong( quint64,QByteArray ) ) );

    if( saveConnectionTimerId_ == -1 )
        saveConnectionTimerId_ = startTimer( saveConnectionInterval_ );

    reply->deleteLater();
}


void QHitBoxChat::onSocketHashLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Can not load websocket hash..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

    reply->deleteLater();
}

void QHitBoxChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_HITBOX_SMILES_INFO_PEFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSmilesLoadError() ) );
}

void QHitBoxChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray jsonSmilesArray = jsonDoc.array();

            foreach( const QJsonValue& smileInfoValue, jsonSmilesArray )
            {
                QJsonObject smileInfo = smileInfoValue.toObject();

                smiles_.append( QChatSmile( smileInfo[ "icon_short" ].toString(), DEFAULT_HITBOX_SMILES_PREFIX + smileInfo[ "icon_path" ].toString(), 0, 0 ) );
                smiles_.append( QChatSmile( smileInfo[ "icon_short_alt" ].toString(), DEFAULT_HITBOX_SMILES_PREFIX + smileInfo[ "icon_path" ].toString(), 0, 0 ) );
            }

            //qDebug() << smiles_;
        }
    }

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Smiles ready...", "", this ) );

    reply->deleteLater();

}

void QHitBoxChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Can not load styles..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );

    reply->deleteLater();
}

void QHitBoxChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_HITBOX_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );
}

void QHitBoxChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObject = jsonDoc.object();

            QString statistic = jsonObject[ "media_views" ].toString();

            if( jsonObject[ "media_is_live" ].toString() == "0" )
                statistic = "0";

            emit newStatistic( new QChatStatistic( HITBOX_SERVICE, statistic ) );
        }
    }

    reply->deleteLater();
}

void QHitBoxChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    reply->deleteLater();
}

void QHitBoxChat::onTextMessageReceived( const QString &message )
{
    //qDebug() << message;

    if( message == "1::" )
    {
        //5:::{"name":"message","args":[{"method":"joinChannel","params":{"channel":"perfectbalance","name":"UnknownSoldier","token":null,"isAdmin":false}}]}
        QString response = "5:::{\"name\":\"message\",\"args\":[{\"method\":\"joinChannel\",\"params\":{\"channel\":\"" + channelName_ + "\",\"name\":\"UnknownSoldier\",\"token\":null,\"isAdmin\":false}}]}";
        socket_->sendTextMessage( response );
    }
    else if( message == "2::" )
    {
        socket_->sendTextMessage( "2::" );
    }
    else if( message.left( 4 ) == "5:::" )
    {
        QString jsonData = message.right( message.size() - 4 );
        //qDebug() << jsonData;

        QJsonParseError parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( jsonData.toStdString().c_str() ), &parseError );

        if( parseError.error == QJsonParseError::NoError )
        {
            if( jsonDoc.isObject() )
            {
                QJsonObject jsonObject = jsonDoc.object();

                if( jsonObject[ "name" ].toString() == "message" )
                {
                    QJsonArray jsonArgs = jsonObject[ "args" ].toArray();

                    foreach( const QJsonValue &value, jsonArgs )
                    {

                        if( value.isString() )
                        {
                            QString argsString = value.toString();


                            //qDebug() << argsString;

                            QJsonDocument argsDoc = QJsonDocument::fromJson( QByteArray( argsString.toStdString().c_str() ) , &parseError );


                            if( parseError.error == QJsonParseError::NoError  )
                            {
                                if( argsDoc.isObject() )
                                {
                                    QJsonObject argsObj = argsDoc.object();

                                    if( argsObj[ "method" ].toString() == "chatMsg" )
                                    {
                                        QJsonObject jsonParamsObj = argsObj[ "params" ].toObject();

                                        if( jsonParamsObj[ "buffer" ].isBool() )
                                            return;



                                        QString nickName = jsonParamsObj[ "name" ].toString();
                                        QString message = jsonParamsObj[ "text" ].toString();

                                        message = insertSmiles( message );

                                        bool blackListUser = blackList().contains( nickName );
                                        bool supportersListUser = supportersList().contains( nickName );

                                        if( originalColors_ )
                                        {
                                            QString color = "#" + jsonParamsObj[ "nameColor" ].toString();

                                            nickName = "<span style=\"color:" + color + "\">" + nickName + "</span>";
                                        }


                                        if( !isRemoveBlackListUsers() || !blackListUser )
                                        {
                                            if( blackListUser )
                                            {
                                                //TODO: игнорируемые
                                                emit newMessage( new QChatMessage( HITBOX_SERVICE, nickName, message, "ignore", this ) );
                                            }
                                            else
                                            {
                                                if( supportersListUser )
                                                {
                                                    //TODO: саппортеры
                                                    emit newMessage( new QChatMessage( HITBOX_SERVICE, nickName, message, "supporter", this ) );
                                                }
                                                else
                                                {
                                                    if( isContainsAliases( message ) )
                                                    {
                                                        //TODO: сообщение к стримеру
                                                        emit newMessage( new QChatMessage( HITBOX_SERVICE, nickName, message, "alias", this ) );
                                                    }
                                                    else
                                                    {
                                                        //TODO: простое сообщение
                                                        emit newMessage( new QChatMessage( HITBOX_SERVICE, nickName, message, "", this ) );
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if( argsObj[ "method" ].toString() == "loginMsg" )
                                    {
                                        if( isShowSystemMessages() )
                                            emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Connected to " + channelName_ + "...", "", this ) );
                                        getStatistic();
                                        if( statisticTimerId_ )
                                            statisticTimerId_ = startTimer( statisticInterval_ );
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
        emit newMessage( new QChatMessage( HITBOX_SERVICE, HITBOX_USER, "Web Socket Error ..." + socket_->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
    //qDebug() << socket_->errorString();
}

void QHitBoxChat::onPong( quint64 elapsedTime, const QByteArray &payload )
{
    qDebug() << "HitBoxChat pong recieved: " << elapsedTime << " " << payload;
}

QString QHitBoxChat::insertSmiles( const QString &message )
{

    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( !QChatMessage::isLink( tokens.at( i ) ) )//не ссылка
        {   
            //qDebug() << tokens[ i ];
            foreach( const QChatSmile &smile, smiles_ )
            {
                if( convertedTokens[ i ] == smile.name() )
                {
                    //convertedTokens[ i ].replace( smile, "<img class = \"smile\" src=\"" + smile + "\"></img>" );
                    convertedTokens[ i ] = "<img class = \"smile\" src=\"" + smile.link() + "\"></img>";
                    break;
                }
            }
        }
    }

    /*
    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }
    */

    int startPos = 0;

    for( int i = 0; i < tokens.size(); ++i )
    {

        //convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );

        int pos = convertedMessage.indexOf( tokens.at( i ), startPos );
        convertedMessage.remove( pos, tokens.at( i ).length() );
        convertedMessage.insert( pos, convertedTokens.at( i ) );
        startPos = pos + convertedTokens.at( i ).length();
    }

    return convertedMessage;

}

void QHitBoxChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}

void QHitBoxChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
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
    originalColors_ = settings.value( HITBOX_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( HITBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( HITBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( HITBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( HITBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}


