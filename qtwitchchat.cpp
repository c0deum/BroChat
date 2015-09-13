#include <QTcpSocket>

#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonArray>

#include <QTimerEvent>

#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include "settingsconsts.h"

#include "qtwitchchat.h"

const QString DEFAULT_TWITCH_CHAT_HOST_NAME = "irc.twitch.tv";
const int DEFAULT_TWITCH_CHAT_PORT_NUMBER = 6667;
const QString DEFAULT_OAUTH_STRING = "oauth:enfcwtwscgdoicn80068eildnqtshu";
const QString DEFAULT_USER_NICK_NAME = "broadcasterchat";
const QString DEFAULT_TWITCH_SELF_PREFIX = "https://api.twitch.tv/kraken/chat/";
const QString DEFAULT_TWITCH_STATISTIC_PREFIX = "https://api.twitch.tv/kraken/streams/";

const int DEFAULT_TWITCH_RECONNECT_INTERVAL = 10000;
const int DEFAULT_TWITCH_STATISTIC_INTERVAL = 10000;

const QString TWITCH_USER = "TWITCH";
const QString TWITCH_SERVICE = "twitch";

QTwitchChat::QTwitchChat( QObject *parent )
: QChatService( parent )
, socket_( 0 )
, nam_( new QNetworkAccessManager( this ) )
, oauthString_()
, nickName_()
, channelName_()
, selfLink_()
, emotIconsLink_()
, badgesLink_()
, emotIcons_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_TWITCH_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_TWITCH_STATISTIC_INTERVAL  )
{
    //loadSettings();
}

QTwitchChat::~QTwitchChat()
{
    disconnect();
}

void QTwitchChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Connecting to " + channelName_ + "...", "", this ) );

    if( socket_)
    {
        if( socket_->state() == QAbstractSocket::ConnectedState )
            return;
        /*else
            reconnect();*/
    }

    socket_ = new QTcpSocket();

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( socket_, SIGNAL( readyRead() ), this, SLOT( parseMessage() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onSocketError() ) );

    //QObject::connect( socket_, SIGNAL( disconnected() ), this, SLOT( reconnect() ) );

    socket_->connectToHost( DEFAULT_TWITCH_CHAT_HOST_NAME, DEFAULT_TWITCH_CHAT_PORT_NUMBER );
}

void QTwitchChat::onConnected()
{
    if( !socket_ )
        return;

    loadSettings();

    socket_->write( QString( "PASS " + oauthString_ + "\r\n" ).toStdString().c_str() );
    socket_->write( QString( "NICK " + nickName_ + "\r\n" ).toStdString().c_str() );
}

void QTwitchChat::disconnect()
{
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
        //qDebug() << "Twitch::disconnect(): reconnectTimerId_ = " << reconnectTimerId_;
    }

    if( statisticTimerId_ >= 0 )
    {
        killTimer( statisticTimerId_ );
        statisticTimerId_ = -1;
    }


    //if( !socket_ )
    //    return;

    if( socket_ && socket_->state() == QAbstractSocket::ConnectedState )
    {
        socket_->write( "QUIT\r\n" );
        socket_->flush();
        socket_->disconnect();
    }

    safeDeleteSocket();


    emit newStatistic( new QChatStatistic( TWITCH_SERVICE, "", this ) );
}

void QTwitchChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Reconnecting...", "", this ) );
    connect();
}

void QTwitchChat::parseMessage()
{
    if( !socket_ )
        return;

    if( socket_->state() == QAbstractSocket::ConnectedState )
    {
        QString line = socket_->readLine();

        //qDebug() << line;

        //поадекватнее надо быть

        if( line.contains ( "376 " + DEFAULT_USER_NICK_NAME ) )
        {
            socket_->write( QString( "JOIN #" + channelName_ + "\r\n" ).toStdString().c_str() );
            getSelf();

            //emit newMessage( new QChatMessage( "qrc:/resources/twitchlogo.png", TWITCH_USER, "Connecting to " + channelName_ + "...", this ) );
            if( isShowSystemMessages() )
                emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Connected to " + channelName_ + "...", "", this ) );

            getStatistic();
            if( statisticTimerId_ )
                statisticTimerId_ = startTimer( statisticInterval_ );
        }
        else if( line.contains( "PING" ) )
        {
            qDebug() << line;
            //socket_->write( "PONG tmi.twitch.tv\r\n" );
            socket_->write( "PONG :tmi.twitch.tv\r\n" );
        }
        else
        {
            //TODO: parse message
            if( line.contains( "PRIVMSG" ) && !line.contains( "HISTORYEND" ) && !line.contains( "USERCOLOR") )
            {
                QString nickName = line.mid( 1, line.indexOf( '!' ) - 1 );
                QString message = line.right( line.size() - line.indexOf( ':', 1 ) - 1 );

                //\r\n
                message = message.left( message.size() - 2 );

                //qDebug() << message;

                message = QChatMessage::replaceEscapeCharecters( message );
                message = insertEmotIcons( message );
                //message = QChatMessage::insertLinks( message );

                bool blackListUser = blackList().contains( nickName );
                bool supportersListUser = supportersList().contains( nickName );

                if( !isRemoveBlackListUsers() || !blackListUser )
                {
                    if( blackListUser )
                    {
                        emit newMessage( new QChatMessage( TWITCH_SERVICE, nickName, message, "ignore", this ) );
                    }
                    else
                    {
                        if( supportersListUser )
                        {
                            emit newMessage( new QChatMessage( TWITCH_SERVICE, nickName, message, "supporter", this ) );
                        }
                        else
                        {
                            if( isContainsAliases( message ) )
                            {
                                emit newMessage( new QChatMessage( TWITCH_SERVICE, nickName, message, "alias", this ) );
                            }
                            else
                            {
                                emit newMessage( new QChatMessage( TWITCH_SERVICE, nickName, message, "", this ) );
                            }
                        }
                    }
                }
            }
        }

        if( socket_->canReadLine() )
        {
            parseMessage();
        }
    }
}

void QTwitchChat::safeDeleteSocket()
{
    /*
    delete socket_;
    socket_ = 0;
    */
    if( socket_ )
        socket_->deleteLater();
    socket_ = 0;
}

void QTwitchChat::getSelf()
{
    QNetworkRequest request( QUrl( DEFAULT_TWITCH_SELF_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSelfLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSelfLoadError() ) );
}

void QTwitchChat::onSelfLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject _links = ( jsonDoc.object() )[ "_links" ].toObject() ;

            selfLink_ = _links[ "self" ].toString();
            emotIconsLink_ = _links[ "emoticons" ].toString();
            badgesLink_ = _links[ "badges" ].toString();

            getEmotIcons();
        }
    }
    reply->deleteLater();
}

void QTwitchChat::onSelfLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    //reconnect();
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Can not connect to " + channelName_ + "..." + reply->errorString(), "", this ) );
    //reconnect();
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
    //qDebug() << "Twitch::onSelfLoadError(): reconnectTimerId_" << reconnectTimerId_;

    reply->deleteLater();
}

void QTwitchChat::getEmotIcons()
{
    QNetworkRequest request( QUrl( emotIconsLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onEmotIconsLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onEmotIconsLoadError() ) );
}

void QTwitchChat::onEmotIconsLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            emotIcons_.clear();

            QJsonArray jsonEmotIcons = jsonDoc.object()[ "emoticons" ].toArray();

            foreach( const QJsonValue &value, jsonEmotIcons )
            {
                QJsonObject jsonEmotIcon = value.toObject();

                QChatSmile smile(   jsonEmotIcon[ "regex" ].toString(),
                                    jsonEmotIcon[ "url" ].toString(),
                                    jsonEmotIcon[ "width" ].toInt(),
                                    jsonEmotIcon[ "height" ].toInt() );
                //emotIcons_.append( smile );
                emotIcons_.insert( smile.name(), smile );
            }
            if( isShowSystemMessages() )
                emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Smiles ready...", "", this ) );
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
        emotIcons_.insert( smile.name(), smile );

        //qDebug() << smile.name() << smile.link();
    }

    reply->deleteLater();
}

void QTwitchChat::onEmotIconsLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Can not load smiles..." + reply->errorString(), "", this ) );

    //TODO: timer for smiles loading
    //getEmotIcons();
    //reconnect();
    reply->deleteLater();
}

void QTwitchChat::getBadges()
{
}

void QTwitchChat::onBadgesLoaded()
{
}

void QTwitchChat::onBadgesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    //reconnect();

    //getBadges();

    reply->deleteLater();
}

void QTwitchChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_TWITCH_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QTwitchChat::onStatisticLoaded()
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

            QJsonObject jsonStreamObj = jsonObj[ "stream" ].toObject();

            QString statistic = QString::number( jsonStreamObj[ "viewers" ].toInt() );

            emit newStatistic( new QChatStatistic( TWITCH_SERVICE, statistic, this ) );
            //emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, QString( "Viewers: " + statistic ), "", this ) );

        }
    }

    reply->deleteLater();
}

void QTwitchChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

QString QTwitchChat::insertEmotIcons( const QString& message ) const
{
    //TODO: думать

    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( !QChatMessage::isLink( tokens.at( i ) ) )//не ссылка
        {
            foreach( const QChatSmile &smile, emotIcons_ )
            {
                if( tokens.at( i ) == smile.name() )
                    convertedTokens[ i ].replace( smile.name(), "<img class = \"smile\" src=\"" + smile.link() + "\"></img>" );
            }
        }
    }

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    return convertedMessage;
}

void QTwitchChat::timerEvent( QTimerEvent *event )
{
    //qDebug() << "Twitch::timerEvent()";
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        //qDebug() << "Twitch::timerEvent(): reconnectTimerId_" << reconnectTimerId_;
        reconnect();
    }
}

void QTwitchChat::loadSettings()
{
    QSettings settings;
    oauthString_ = settings.value( "/Settings/Twitch/oauth", DEFAULT_OAUTH_STRING ).toString();
    nickName_ = settings.value( "/Settings/Twitch/nickName", DEFAULT_USER_NICK_NAME ).toString();
    channelName_= settings.value( "/Settings/Twitch/Channel", DEFAULT_TWITCH_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( TWITCH_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( TWITCH_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( TWITCH_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QTwitchChat::onSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( TWITCH_SERVICE, TWITCH_USER, "Socket Error ..." + socket_->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
    //qDebug() << "Twitch::onSocketError: reconnectTimerId_ = " << reconnectTimerId_;
}
