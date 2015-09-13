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
, socket_( 0 )
, channelName_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_STREAMBOX_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_STATISTIC_INTERVAL )
, saveConnectionTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_STREAMBOX_SAVE_CONNECTION_INTERVAL )
{
    //loadSettings();
}

QStreamBoxChat::~QStreamBoxChat()
{
    disconnect();
}

void QStreamBoxChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, "Connecting to " + channelName_ + "...", "", this ) );

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    //socket_-

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageReceived( const QString & ) ) );

    QObject::connect( socket_, SIGNAL( pong( quint64, QByteArray ) ), this, SLOT( onPong( quint64,QByteArray ) ) );

    //QObject::connect( socket_, SIGNAL( sslErrors(QList<QSslError>) ), socket_, SLOT( ignoreSslErrors() ) );
    //disconnect test
    //QObject::connect( socket_, SIGNAL( disconnected() ), this, SLOT( reconnect() ) );


    socket_->open( QUrl( DEFAULT_STREAMBOX_WEBSOCKET_LINK ) );
}


void QStreamBoxChat::disconnect()
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

    emit newStatistic( new QChatStatistic( STREAMBOX_SERVICE, "", this ) );
}

void QStreamBoxChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, "Reconnecting...", "", this  ) );
    connect();
}

void QStreamBoxChat::onWebSocketConnected()
{
    QString message = "{\"event\":\"set_room\",\"data\":{\"chat_id\":\"" + channelName_ +"\",\"token\":false}}";

    socket_->sendTextMessage( message );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, "Connected to " + channelName_ + "...", "", this ) );

    getStatistic();

    if( statisticTimerId_ == -1 )
        statisticTimerId_ = startTimer( statisticInterval_ );

    if( saveConnectionTimerId_ == -1 )
        saveConnectionTimerId_ = startTimer( saveConnectionInterval_ );
}

void QStreamBoxChat::onWebSocketError()
{
    QString errString = "Web Socket Error... " + socket_->errorString();
    //qDebug() << errString;
    errString.replace( "\r\n", "" );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, errString , "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
}

void QStreamBoxChat::onTextMessageReceived( const QString& message )
{
    //qDebug() << message;

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );


    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString event = jsonObj[ "event" ].toString();

            if( event == "message" )
            {
                QString dataString = jsonObj[ "data" ].toString();
                //dataString.replace( "\\\"", "\'" );

                //qDebug() << dataString;

                QJsonDocument jsonDocData = QJsonDocument::fromJson( dataString.toStdString().c_str(), &parseError );

                if( parseError.error == QJsonParseError::NoError )
                {

                    if( jsonDocData.isObject() )
                    {
                        QJsonObject jsonData = jsonDocData.object();

                        QString nickName = jsonData[ "nickname" ].toString();
                        QString message = jsonData[ "text" ].toString();

                        //\"text\":\"<span class=\\\"chat_text_plain\\\">1 <\\\/span><smile><img src=\\\"\\\/img\\\/smiles\\\/dembel.png\\\"><\\\/smile><span class=\\\"chat_text_plain\\\"> 2<\\\/span>\",\"nickname\":\"cppguvdvo\",\"time\":1433797894,\"id\":4751,\"role\":0}"}
                        //qDebug() << message;

                        message.replace( "<smile><img src=\"/img/smiles/", "<img class=\"smile\" src=\"" + DEFAULT_STREAMBOX_SMILE_DIR_PREFIX );
                        message.replace( "</smile>", "</img>" );
                        message.replace( "<span class=\"chat_text_plain\">", "" );
                        message.replace( "</span>", "" );

                        //qDebug() << message;

                        //message.replace( "/img/smiles/", DEFAULT_STREAMBOX_SMILE_DIR_PREFIX )  ;

                        //int role = jsonData[ "role" ].toInt();

                        bool blackListUser = blackList().contains( nickName );
                        bool supportersListUser = supportersList().contains( nickName );

                        bool containsAliases = isContainsAliases( message );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            if( blackListUser )
                            {
                                //TODO: список игнорируемых
                                emit newMessage( new QChatMessage( STREAMBOX_SERVICE, nickName, message, "ignore", this ) );
                            }
                            else
                            {
                                if( supportersListUser )
                                {
                                    //TODO: список саппортеров
                                    emit newMessage( new QChatMessage( STREAMBOX_SERVICE, nickName, message, "supporter", this ) );
                                }
                                else
                                {
                                    if( containsAliases )
                                    {
                                        //TODO: обращение к стримеру
                                        emit newMessage( new QChatMessage( STREAMBOX_SERVICE, nickName, message, "alias", this ) );
                                    }
                                    else
                                    {
                                        emit newMessage( new QChatMessage( STREAMBOX_SERVICE, nickName, message, "", this ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
/*
            <div class="message women" ng-if="message.event == 'women'">
                <span ng-if="message.action == 'slap'">{{message.women}} шлепнула по попке {{message.user}}</span>
                <span ng-if="message.action == 'hit'">{{message.women}} надавала по заднице {{message.user}}</span>
                <span ng-if="message.action == 'deo'">{{message.women}} попшикала антиперспирантом под мышками {{message.user}}</span>
                <span ng-if="message.action == 'pimple'">{{message.women}} выдавила прыщик {{message.user}}</span>
                <span ng-if="message.action == 'head'">{{message.women}} погладила по головке {{message.user}}</span>
                <span ng-if="message.action == 'eyeon'">{{message.women}} подмигнула {{message.user}}</span>
                <span ng-if="message.action == 'limf'">{{message.women}} укусила в лимфоузел {{message.user}}</span>
            </div>
это сообщения с коммандами
*/
            else if( event == "women" )
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
                    emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, womenName + " " + actionsMap[ action ] + " " + userName, "", this ) );
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
            else if( event == "new_role" )
            {
                QString dataString = jsonObj[ "data" ].toString();
                dataString.replace( "\\", "" );

                QJsonDocument jsonDocData = QJsonDocument::fromJson( dataString.toStdString().c_str(), &parseError );

                if( parseError.error == QJsonParseError::NoError )
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
                            emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER,
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

void QStreamBoxChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_STREAMBOX_STATISTIC_LINK + "" ) ) ;

    QByteArray data;

    QString dataString = "url=" + channelName_ + "&type=0";

    data.append( dataString );

    //request.setRawHeader( "Host", "stream-box.ru" );
    //request.setRawHeader( "Accept", "application/json, text/plain, */*" );
    //request.setRawHeader( "Referer", QString( DEFAULT_STREAMBOX_CHAT_ID_PREFIX + channelName_ ).toStdString().c_str() );

    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/x-www-form-urlencoded" ) );
    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QStreamBoxChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( parseError.error == QJsonParseError::NoError )
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
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

void QStreamBoxChat::onPong( quint64 elapsedTime, const QByteArray &payload )
{
    /*
    qDebug() << "Popongui";
    emit newMessage( new QChatMessage( STREAMBOX_SERVICE, STREAMBOX_USER, QString( payload ) + " " + QString::number( elapsedTime ), "", this ) );
    qDebug() << elapsedTime;
    qDebug() << payload;
    */

    qDebug() << "StreamBoxChat pong recieved: " << elapsedTime << " " << payload;
}


QString QStreamBoxChat::insertSmiles( const QString &message )
{
    return QString( message );
}

void QStreamBoxChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
/*

        QStringList smilesList;

        smilesList <<
        "arr" <<
        "azaz" <<
        "blood" <<
        "butthurt" <<
        "buu" <<
        "dembel" <<
        "diablo" <<
        "doom" <<
        "enemy" <<
        "flag" <<
        "frog" <<
        "girl" <<
        "hello" <<
        "inostranez" <<
        "jensen" <<
        "kakashka" <<
        "kek" <<
        "king" <<
        "konchita" <<
        "leps" <<
        "mm" <<
        "nazi" <<
        "nimf" <<
        "potato" <<
        "red" <<
        "rofl" <<
        "sir" <<
        "smoke1" <<
        "smoke2" <<
        "tit" <<
        "uhhh" <<
        "uu" <<
        "viking" <<
        "wtf";

        QString testMessage;

        for( int i = 0; i < 3; i++ )
        {
            testMessage += " :";
            testMessage += smilesList[ qrand() % smilesList.size() ];
            testMessage += ": ";
        }

        for( int i = 0; i < 20; i++ )
        {
            testMessage += 'a' + qrand() % 26;
        }

        for( int i = 0; i < 10; i++ )
        {
            int hexnum = 0x3400 + qrand() % ( 0x4db5 - 0x3400 );

            int utf8Bytes[ 3 ];

            for( int j = 0; j < 3; j++ )
            {
                utf8Bytes[ j ] = hexnum % 64;
                hexnum /= 64;
                utf8Bytes[ j ] += 128;
            }

            utf8Bytes[ 2 ] += 96;


            for( int j = 2; j >= 0; j--  )
            {
                testMessage += "%";

                if( utf8Bytes[ j ] < 16 )
                    testMessage += "0";

                testMessage += QString::number( utf8Bytes[ j ], 16 ).toUpper();
            }


        }

        qDebug() << testMessage;

        QString message = "{\"event\":\"new_message\",\"data\":{\"chatID\":\"" + channelName_ + "\",\"text\":\"" + testMessage + "\"}}";

        socket_->sendTextMessage( message ) ;
        */

    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        //qDebug() << "Popingui";
        if( socket_ && socket_->isValid() )
        {
            //qDebug() << "Popingui: ok";
            //socket_->ping( "BroChat Ping Bleat'!" );
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

    setAliasesList( settings.value( STREAMBOX_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( STREAMBOX_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( STREAMBOX_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( STREAMBOX_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}


