#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QAbstractSocket>

#include <QSettings>

#include <QTimerEvent>

#include "qchatmessage.h"

#include "settingsconsts.h"

#include "qstreamboxchat.h"

const QString DEFAULT_STREAMBOX_WEBSOCKET_LINK = "ws://stream-box.ru:8080";
const QString DEFAULT_STREAMBOX_SMILE_DIR_PREFIX = "http://stream-box.ru/img/smiles/";
const QString DEFAULT_STREAMBOX_CHAT_ID_PREFIX = "http://stream-box.ru/chat_";
const QString DEFAULT_STREAMBOX_STATISTIC_LINK = "http://stream-box.ru/api/get_online";


const int DEFAULT_STREAMBOX_RECONNECT_INTERVAL = 10000;
const int DEFAULT_STATISTIC_INTERVAL = 10000;
const int DEFAULT_STREAMBOX_SAVE_CONNECTION_INTERVAL = 25000;

const QString STREAMBOX_SERVICE = "streambox";
const QString STREAMBOX_USER = "STREAMBOX";

QStreamBoxChat::QStreamBoxChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( nullptr )
, channelName_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_STREAMBOX_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_STATISTIC_INTERVAL )
, saveConnectionTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_STREAMBOX_SAVE_CONNECTION_INTERVAL )
{
}

QStreamBoxChat::~QStreamBoxChat()
{
    disconnect();
}

void QStreamBoxChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );

    socket_->open( QUrl( DEFAULT_STREAMBOX_WEBSOCKET_LINK ) );
}


void QStreamBoxChat::disconnect()
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

    emit newStatistic( new QChatStatistic( STREAMBOX_SERVICE, QString(), this ) );
}

void QStreamBoxChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
        emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, tr( "Reconnecting..." ), QString(), this  ) );
    connect();
}

void QStreamBoxChat::onWebSocketConnected()
{
    QString message = "{\"event\":\"set_room\",\"data\":{\"chat_id\":\"" + channelName_ +"\",\"token\":false}}";

    socket_->sendTextMessage( message );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );

    loadSmiles();

    loadStatistic();

    startUniqueTimer( statisticTimerId_, statisticInterval_ );
    startUniqueTimer( saveConnectionTimerId_, saveConnectionInterval_ );
}

void QStreamBoxChat::onWebSocketError()
{
    QString errString = tr( "Web Socket Error... " ) + socket_->errorString();

    errString.replace( "\r\n", "" );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, errString , QString(), this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}

void QStreamBoxChat::onTextMessageReceived( const QString& message )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );


    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString event = jsonObj[ "event" ].toString();

            if( "message" == event )
            {
                QString dataString = jsonObj[ "data" ].toString();

                QJsonDocument jsonDocData = QJsonDocument::fromJson( dataString.toStdString().c_str(), &parseError );

                if( QJsonParseError::NoError == parseError.error )
                {

                    if( jsonDocData.isObject() )
                    {
                        QJsonObject jsonData = jsonDocData.object();

                        QString nickName = jsonData[ "nickname" ].toString();
                        QString message = jsonData[ "text" ].toString();

                        //\"text\":\"<span class=\\\"chat_text_plain\\\">1 <\\\/span><smile><img src=\\\"\\\/img\\\/smiles\\\/dembel.png\\\"><\\\/smile><span class=\\\"chat_text_plain\\\"> 2<\\\/span>\",\"nickname\":\"cppguvdvo\",\"time\":1433797894,\"id\":4751,\"role\":0}"}                        

                        message.replace( "<smile><img src=\"/img/smiles/", "<img class=\"smile\" src=\"" + DEFAULT_STREAMBOX_SMILE_DIR_PREFIX );
                        message.replace( "</smile>", "</img>" );
                        message.replace( "<span class=\"chat_text_plain\">", "" );
                        message.replace( "</span>", "" );

                        message.replace( "<a href=\"", "" );

                        message.replace( QRegExp( "\\\" target(.*)\\>" ), "" );

                        message = insertSmiles( message );

                        emit newMessage( ChatMessage( STREAMBOX_SERVICE, nickName, message, QString(), this ) );

                        //message.replace( "/img/smiles/", DEFAULT_STREAMBOX_SMILE_DIR_PREFIX )  ;

                        //int role = jsonData[ "role" ].toInt();
                    }
                }
            }
            else if( "women" == event )
            {

                static QMap< QString, QString > actionsMap;

                actionsMap.insert( "slap", "шлепнула по попке" );
                actionsMap.insert( "hit", "надавала по заднице" );
                actionsMap.insert( "deo", "попшикала антиперспирантом под мышками" );
                actionsMap.insert( "pimple", "выдавила прыщик" );
                actionsMap.insert( "head", "погладила по головке" );
                actionsMap.insert( "eyeon", "подмигнула" );
                actionsMap.insert( "limf", "укусила в лимфоузел" );

                //{"event":"women","data":{"chatID":"6","women":"OMEGA","user":"YasTpe6","action":"hit"}}

                QJsonObject jsonDataObj = jsonObj[ "data" ].toObject();

                QString womenName = jsonDataObj[ "women" ].toString();
                QString userName = jsonDataObj[ "user" ].toString();

                QString action = jsonDataObj[ "action" ].toString();

                if( actionsMap.contains( action ) )
                {
                    //if( isShowSystemMessages() )
                    emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, womenName + " " + actionsMap[ action ] + " " + userName, QString(), this ) );
                }

            }
            //{"event":"new_role","data":"{\"moderator\":\"Admin\",\"user\":\"stepanator\",\"level_up\":false,\"role\":0}"}
/*
            <div class="message new_role" ng-if="message.event == 'new_role'">
                <span ng-if="message.role == 0" >{{message.moderator}} разжаловал {{message.user}}</span>
                <span ng-if="message.role == 1" >{{message.moderator}} назначил {{message.user}} <b>Администратором</b></span>
                <span ng-if="message.role == 2" >{{message.moderator}} назначил {{message.user}} <b>Модератором</b></span>
                <span ng-if="message.role == 3" >{{message.moderator}} дал {{message.user}} права <b>Cтримера</b></span>
                <span ng-if="message.role == 4" >{{message.moderator}} назначил {{message.user}} помощником <b>Стримера</b></span>
                <span ng-if="message.role == 5" >{{message.moderator}} сделал {{message.user}} <b>Девушкой</b></span>
            </div>
*/
            else if( "new_role" == event )
            {
                QString dataString = jsonObj[ "data" ].toString();
                dataString.replace( "\\", "" );

                QJsonDocument jsonDocData = QJsonDocument::fromJson( dataString.toStdString().c_str(), &parseError );

                if( QJsonParseError::NoError == parseError.error )
                {
                    if( jsonDocData.isObject() )
                    {
                        QJsonObject jsonData = jsonDocData.object();

                        QString moderatorName = jsonData[ "moderator" ].toString();
                        QString userName = jsonData[ "user" ].toString();

                        int role = jsonData[ "role" ].toInt();

                        static QMap< int, QString > roleActionsMap;

                        roleActionsMap.insert( 0, "разжаловал" );
                        roleActionsMap.insert( 1, "назначил" );
                        roleActionsMap.insert( 2, "назначил" );
                        roleActionsMap.insert( 3, "дал" );
                        roleActionsMap.insert( 4, "назначил" );
                        roleActionsMap.insert( 5, "сделал" );

                        static QMap< int, QString > newRoleMap;

                        newRoleMap.insert( 0, "" );
                        newRoleMap.insert( 1, " Администратором" );
                        newRoleMap.insert( 2, " Модератором" );
                        newRoleMap.insert( 3, " права Cтримера" );
                        newRoleMap.insert( 4, " помощником Стримера" );
                        newRoleMap.insert( 5, " Девушкой" );

                        if( role >= 0 && role < roleActionsMap.size() - 1 )
                        {
                            //if( isShowSystemMessages() )
                            emit newMessage( ChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER,
                                                               moderatorName + " " +
                                                               roleActionsMap[ role ] + " " +
                                                               userName + newRoleMap[ role ], "", this ) );
                        }
                    }
                }
            }
        }
    }
}

void QStreamBoxChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_STREAMBOX_STATISTIC_LINK + "" ) ) ;

    QByteArray data;

    QString dataString = "url=" + channelName_ + "&type=0";

    data.append( dataString );

    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/x-www-form-urlencoded" ) );
    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QStreamBoxChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            int online = jsonObj[ "online" ].toInt();
            int usersCount = jsonObj[ "usersCount" ].toInt();

            QString statistic = QString::number( usersCount ) + "+(" + QString::number( online - usersCount ) + ")";

            emit newStatistic( new QChatStatistic( STREAMBOX_SERVICE, statistic, this ) );            
        }
    }

    reply->deleteLater();
}

void QStreamBoxChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QStreamBoxChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() )
        {
            socket_->ping();
        }
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QStreamBoxChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( STREAMBOX_CHANNEL_SETTING_PATH, DEFAULT_STREAMBOX_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( STREAMBOX_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( STREAMBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( STREAMBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( STREAMBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( STREAMBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}


