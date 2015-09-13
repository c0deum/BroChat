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
#include <QFileInfo>

#include "settingsconsts.h"

#include "qcybergamechat.h"

const QString DEFAULT_CYBERGAME_WEBSOCKET_LINK = "ws://cybergame.tv:9090/websocket";
const QString DEFAULT_CYBERGAME_CHAT_LINK = "http://cybergame.tv/cgchat.htm";
const QString DEFAULT_CYBERGAME_SMILE_PREFIX = "http://cybergame.tv/";

const QString DEFAULT_CYBERGAME_STATISTIC_PREFIX = "http://api.cybergame.tv/w/streams2.php?channel=";

const int DEFAULT_CYBERGAME_SAVE_CONNECTION_INTERVAL = 25000;
const int DEFAULT_CYBERGAME_RECONNECT_INTERVAL = 10000;
const int DEFAULT_CYBERGAME_STATISTIC_INTERVAL = 10000;

const QString CYBERGAME_USER = "CYBERGAME";
const QString CYBERGAME_SERVICE = "cybergame";

QCyberGameChat::QCyberGameChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( 0 )
, channelName_()
, lastUpd_( 0 )
, smiles_()
, saveConnectionTimerId_( -1 )
, reconnectTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_CYBERGAME_SAVE_CONNECTION_INTERVAL )
, reconnectInterval_( DEFAULT_CYBERGAME_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_CYBERGAME_STATISTIC_INTERVAL )
{
    //loadSettings();
    //getSmiles();
}

QCyberGameChat::~QCyberGameChat()
{
    disconnect();
}

void QCyberGameChat::connect()
{
    if( channelName_ == "" )
        return;

    smiles_.clear();
    getSmiles();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Connecting to " + channelName_ + "...", "", this ) );

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );

    //test ping-pong support
    QObject::connect( socket_, SIGNAL( pong( quint64,QByteArray ) ), this, SLOT( onPong( quint64,QByteArray ) ) );

    socket_->open( QUrl( DEFAULT_CYBERGAME_WEBSOCKET_LINK ) );
}

void QCyberGameChat::disconnect()
{
    if( saveConnectionTimerId_ >= 0 )
    {
        killTimer( saveConnectionTimerId_ );
        saveConnectionTimerId_ = -1;
    }
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

    lastUpd_ = 0;

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = 0;

    emit newStatistic( new QChatStatistic( CYBERGAME_SERVICE, "", this ) );
}

void QCyberGameChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Reconnecting...", "", this ) );
    connect();
}

void QCyberGameChat::onWebSocketConnected()
{
    QString message = "{\"command\":\"login\",\"message\":\"{\\\"login\\\":\\\"\\\",\\\"password\\\":\\\"\\\",\\\"channel\\\":\\\"#" + channelName_ + "\\\"}\"}";
    socket_->sendTextMessage( message );
}

void QCyberGameChat::onWebSocketError()
{
    //qDebug() << socket_->error() << socket_->errorString() << socket_->state() << error;
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Web socket error..." + socket_->errorString(), "", this ) );
    //reconnect();
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
}

void QCyberGameChat::onTextMessageRecieved( const QString &message )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString command = jsonObj[ "command" ].toString();
            QString answer;
            //a["{\"command\":\"changeWindow\",\"message\":{\"window\":\"chatPage\"}}"]
            if( command == "changeWindow" )
            {
                answer = "{\"command\":\"getHistory\",\"message\":\"#" + channelName_ +"\"}";
                socket_->sendTextMessage( answer );

                if( isShowSystemMessages() )
                    emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Connected to " + channelName_ + "...", "", this ) );

                getStatistic();
                if( statisticTimerId_ == -1 )
                    statisticTimerId_ = startTimer( statisticInterval_ );
            }
            else if( command == "getHistory" )
            {
                if( lastUpd_ )
                {
                }
                if( saveConnectionTimerId_ == -1 )
                    saveConnectionTimerId_ = startTimer( saveConnectionInterval_ );
            }
            else if( command == "chatMessage" )
            {
             //"{"command":"chatMessage","message":"{\"when\":1416566861280,\"from\":\"c0deum\",\"text\":\"test\"}"}"
                QString chatMessage = jsonObj[ "message" ].toString();
                //chatMessage.replace( "\\\"", "\"" );
                //qDebug() << chatMessage;

                jsonDoc = QJsonDocument::fromJson( QByteArray( chatMessage.toStdString().c_str() ), &parseError );
                if( parseError.error == QJsonParseError::NoError )
                {

                    if( jsonDoc.isObject() )
                    {
                        QString message = jsonDoc.object()[ "text" ].toString();

                        message = insertSmiles( message );
                        //message = QChatMessage::insertLinks( message );

                        QString nickName = jsonDoc.object()[ "from" ].toString();

                        bool blackListUser = blackList().contains( nickName );
                        bool supportersListUser = supportersList().contains( nickName );

                        bool containsAliases = isContainsAliases( message );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            if( blackListUser )
                            {
                                //TODO: список игнорируемых
                                emit newMessage( new QChatMessage( CYBERGAME_SERVICE, nickName, message, "ignore", this ) );
                            }
                            else
                            {
                                if( supportersListUser )
                                {
                                    //TODO: список саппортеров
                                    emit newMessage( new QChatMessage( CYBERGAME_SERVICE, nickName, message, "supporter", this ) );
                                }
                                else
                                {
                                    if( containsAliases )
                                    {
                                        //TODO: обращение к стримеру
                                        emit newMessage( new QChatMessage( CYBERGAME_SERVICE, nickName, message, "alias", this ) );
                                    }
                                    else
                                    {
                                        emit newMessage( new QChatMessage( CYBERGAME_SERVICE, nickName, message, "", this ) );
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                //qDebug() << "Unsupported message" << command;
                 //emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, command, this ) );
            }
        }
    }
}

void QCyberGameChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_CHAT_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QCyberGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString smilesString = reply->readAll();

    int startSmilesPos = smilesString.indexOf( "smiles =" );
    int leftSmilesBracketPos = smilesString.indexOf( "{", startSmilesPos );
    int rightSmilesBracketPos = smilesString.indexOf( "}", startSmilesPos );

    smilesString = smilesString.mid( leftSmilesBracketPos, rightSmilesBracketPos - leftSmilesBracketPos + 1 );

    smilesString.replace( "{", "[" );
    smilesString.replace( "}", "]" );
    smilesString.replace( " ", "" );
    smilesString.replace( "\r", "" );
    smilesString.replace( "\n", "" );
    smilesString.replace( "\":\"", "\",\"" );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( smilesString.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {
            smiles_.clear();
            QJsonArray smilesInfo = jsonDoc.array();

            for( int i = 0; i < smilesInfo.size() / 2; ++i )
            {
                QChatSmile smile(
                                    smilesInfo[ 2 * i ].toString(),
                                    DEFAULT_CYBERGAME_SMILE_PREFIX + smilesInfo[ 2 * i + 1 ].toString(),
                                    0, 0 );

                //qDebug() << smile.name() << smile.link();

                smiles_.insert( smile.name(), smile );
            }
            if( isShowSystemMessages() )
                emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Smiles ready...", "", this ) );
        }
    }

    //own smiles code
    QString smilesPath = QApplication::applicationDirPath() + "/smiles";

    QStringList extList;
    extList << "*.svg" << "*.png" << "*.gif" << "*.jpg";

    QDir smilesDir( smilesPath );

    QStringList smileFiles = smilesDir.entryList( extList, QDir::Files | QDir::NoSymLinks );

    foreach( const QString& smileName, smileFiles )
    {
        QChatSmile smile(   ":" + smileName.left( smileName.length() - 4 ) + ":",
                                "file:///" + smilesPath + "/" + smileName, 0, 0 );
        smiles_.insert( smile.name(), smile );
    }

    //qDebug() << smilesString;
    reply->deleteLater();

}

void QCyberGameChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, "Can not load smiles...", "", this ) );

    //TODO: timer for smiles loading
    //getSmiles();

    reply->deleteLater();
}

void QCyberGameChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QCyberGameChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    //qDebug() << reply->readAll();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString statistic = jsonObj[ "viewers" ].toString();
            //"online":"1"
            emit newStatistic( new QChatStatistic( CYBERGAME_SERVICE, statistic, this ) );
            //emit newMessage( new QChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, QString( "Viewers: " + statistic ), "", this ) );
        }
    }

    reply->deleteLater();
}

void QCyberGameChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

void QCyberGameChat::onPong(quint64 elapsedTime, const QByteArray &payload)
{
    qDebug() << "CyberGame pong recieved: " << elapsedTime << " " << payload;
}

QString QCyberGameChat::insertSmiles( const QString &message )
{
    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    //qDebug() << tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( !QChatMessage::isLink( tokens.at( i ) ) )//не ссылка
        {
            foreach( const QChatSmile &smile, smiles_ )
            {
               //qDebug() << smile.name();
               convertedTokens[ i ].replace( smile.name(), "<img class = \"smile\" src=\"" + smile.link() + "\"></img>" );
               if( tokens[ i ] != convertedTokens[ i ] )
                   break;
            }
        }
    }

    //qDebug() << convertedTokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    //qDebug() << convertedMessage;

    return convertedMessage;
}


void QCyberGameChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && socket_->state() == QAbstractSocket::ConnectedState )
        {

            //QString message = "{\"command\":\"getUsers\",\"message\":\"{\\\"channel\\\":\\\"#" + channelName_ + "\\\"}\"}";
            //socket_->sendTextMessage( message );

            socket_->ping();
        }
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

    setAliasesList( settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
