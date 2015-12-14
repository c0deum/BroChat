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

#include "qchatmessage.h"

#include "qtwitchchat.h"

const QString DEFAULT_TWITCH_CHAT_HOST_NAME = "irc.twitch.tv";
const int DEFAULT_TWITCH_CHAT_PORT_NUMBER = 6667;
const QString DEFAULT_OAUTH_STRING = "oauth:enfcwtwscgdoicn80068eildnqtshu";
const QString DEFAULT_USER_NICK_NAME = "broadcasterchat";
const QString DEFAULT_TWITCH_SELF_PREFIX = "https://api.twitch.tv/kraken/chat/";
const QString DEFAULT_TWITCH_STATISTIC_PREFIX = "https://api.twitch.tv/kraken/streams/";

const int DEFAULT_TWITCH_RECONNECT_INTERVAL = 10000;
const int DEFAULT_TWITCH_STATISTIC_INTERVAL = 10000;
const int DEFAULT_TWITCH_SAVE_CONNECTION_INTERVAL = 25000;

const QString TWITCH_USER = "TWITCH";
const QString TWITCH_SERVICE = "twitch";

QTwitchChat::QTwitchChat( QObject *parent )
: QChatService( parent )
, socket_( nullptr )
, nam_( new QNetworkAccessManager( this ) )
, oauthString_()
, nickName_()
, channelName_()
, selfLink_()
, emotIconsLink_()
, badgesLink_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_TWITCH_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_TWITCH_STATISTIC_INTERVAL  )
, saveConnectionTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_TWITCH_SAVE_CONNECTION_INTERVAL )
{
}

QTwitchChat::~QTwitchChat()
{
    disconnect();
}

void QTwitchChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );

    if( socket_)
    {
        if( socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            return;
        }
        //123
        safeDeleteSocket();
    }    

    socket_ = new QTcpSocket();

    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( socket_, SIGNAL( readyRead() ), this, SLOT( parseMessage() ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onSocketError() ) );

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
    resetTimer( saveConnectionTimerId_ );
    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );

    if( socket_ && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->write( "QUIT\r\n" );
        socket_->flush();
        socket_->disconnect();
    }

    safeDeleteSocket();

    emit newStatistic( new QChatStatistic( TWITCH_SERVICE, QString(), this ) );
}

void QTwitchChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
        emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, "Reconnecting...", "", this ) );
    connect();
}

void QTwitchChat::parseMessage()
{
    if( !socket_ )
        return;

    if( QAbstractSocket::ConnectedState == socket_->state() )
    {
        QString line = socket_->readLine();        

        if( line.contains ( "376 " + DEFAULT_USER_NICK_NAME ) )
        {
            socket_->write( QString( "JOIN #" + channelName_ + "\r\n" ).toStdString().c_str() );

            loadSelf();

            if( isShowSystemMessages() )
                emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );


            loadStatistic();

            startUniqueTimer( statisticTimerId_, statisticInterval_ );
            startUniqueTimer( saveConnectionTimerId_, saveConnectionInterval_ );
        }
        else if( line.contains( "PING" ) )
        {
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

                message = ChatMessage::replaceEscapeCharecters( message );
                message = insertSmiles( message );

                emit newMessage( ChatMessage( TWITCH_SERVICE, nickName, message, QString(), this ) );
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
    if( socket_ )
        socket_->deleteLater();
    socket_ = nullptr;
}

void QTwitchChat::loadSelf()
{
    QNetworkRequest request( QUrl( DEFAULT_TWITCH_SELF_PREFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSelfLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSelfLoadError() ) );
}

void QTwitchChat::onSelfLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject _links = ( jsonDoc.object() )[ "_links" ].toObject() ;

            selfLink_ = _links[ "self" ].toString();
            emotIconsLink_ = _links[ "emoticons" ].toString();
            badgesLink_ = _links[ "badges" ].toString();

            loadSmiles();
        }
    }
    reply->deleteLater();
}

void QTwitchChat::onSelfLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString(), QString(), this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );    

    reply->deleteLater();
}

void QTwitchChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( emotIconsLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QTwitchChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonArray jsonEmotIcons = jsonDoc.object()[ "emoticons" ].toArray();

            foreach( const QJsonValue &value, jsonEmotIcons )
            {
                QJsonObject jsonEmotIcon = value.toObject();

                addSmile( jsonEmotIcon[ "regex" ].toString(), jsonEmotIcon[ "url" ].toString() );

            }
            if( isShowSystemMessages() )
                emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, tr( "Smiles loaded..." ), QString(), this ) );
        }
    }

    reply->deleteLater();
}

void QTwitchChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, tr( "Can not load smiles..." ) + reply->errorString(), QString(), this ) );

    reply->deleteLater();
}

void QTwitchChat::loadBadges()
{
}

void QTwitchChat::onBadgesLoaded()
{
}

void QTwitchChat::onBadgesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QTwitchChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_TWITCH_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QTwitchChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QJsonObject jsonStreamObj = jsonObj[ "stream" ].toObject();

            QString statistic = QString::number( jsonStreamObj[ "viewers" ].toInt() );

            emit newStatistic( new QChatStatistic( TWITCH_SERVICE, statistic, this ) );
        }
    }

    reply->deleteLater();
}

void QTwitchChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QTwitchChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            socket_->write( "PING :tmi.twitch.tv\r\n" );
        }
    }
}

void QTwitchChat::loadSettings()
{
    QSettings settings;
    oauthString_ = settings.value( "/Settings/Twitch/oauth", DEFAULT_OAUTH_STRING ).toString();
    nickName_ = settings.value( "/Settings/Twitch/nickName", DEFAULT_USER_NICK_NAME ).toString();
    channelName_= settings.value( "/Settings/Twitch/Channel", DEFAULT_TWITCH_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( TWITCH_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( TWITCH_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( TWITCH_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( TWITCH_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( TWITCH_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QTwitchChat::onSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( TWITCH_SERVICE, TWITCH_USER, "Socket Error ..." + socket_->errorString(), "", this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}
