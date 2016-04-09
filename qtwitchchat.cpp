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

const QString DEFAULT_TWITCH_SMILE_PREFIX = "http://static-cdn.jtvnw.net/emoticons/v1/";

const QString QTwitchChat::SERVICE_NAME = "twitch";
const QString QTwitchChat::SERVICE_USER_NAME = "TWITCH";

const int QTwitchChat::RECONNECT_INTERVAL = 10000;
const int QTwitchChat::STATISTIC_INTERVAL = 10000;
const int QTwitchChat::SAVE_CONNECTION_INTERVAL = 25000;

QTwitchChat::QTwitchChat( QObject *parent )
: QChatService( parent )
, socket_( nullptr )
, nam_( new QNetworkAccessManager( this ) )
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
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

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

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QTwitchChat::reconnect()
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

void QTwitchChat::parseMessage()
{
    if( !socket_ )
        return;



    if( QAbstractSocket::ConnectedState == socket_->state() )
    {
        QString line = socket_->readLine();

        qDebug() << line;

        if( line.contains ( "376 " + DEFAULT_USER_NICK_NAME ) )
        {
            socket_->write( QString( "JOIN #" + channelName_ + "\r\n" ).toStdString().c_str() );

            loadSelf();

            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
            }


            loadStatistic();

            startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );
            startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
        }
        else if( line.contains ( "366 " + DEFAULT_USER_NICK_NAME ) )
        {
            //socket_->write( "CAP REQ :twitch.tv/commands\r\n" );
            socket_->write( "CAP REQ :twitch.tv/tags\r\n" );
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
                //@color=;display-name=c0deum;emotes=28:0-12,14-26;subscriber=0;turbo=0;user-id=64078456;user-type= :c0deum!c0deum@c0deum.tmi.twitch.tv PRIVMSG #c0deum :MrDestructoid MrDestructoid\r\n


                /*
                QString nickName = line.mid( 1, line.indexOf( '!' ) - 1 );
                QString message = line.right( line.size() - line.indexOf( ':', 1 ) - 1 );

                //\r\n
                message = message.left( message.size() - 2 );

                message = ChatMessage::replaceEscapeCharecters( message );
                message = insertSmiles( message );
                */

                const QString COLOR = "@color=";
                QString color;

                int startColorPos = line.indexOf( COLOR ) + COLOR.length();
                int endColorPos = line.indexOf( ";", startColorPos ) - 1;

                if( endColorPos - startColorPos + 1 > 0 )
                {
                    color = line.mid( startColorPos, endColorPos - startColorPos + 1 );
                }

                const QString DISPLAY_NAME = "display-name=";
                QString nickName;

                int startDisplayNamePos = line.indexOf( DISPLAY_NAME ) + DISPLAY_NAME.length();
                int endDisplayNamePos = line.indexOf( ";", startDisplayNamePos ) - 1;

                if( endDisplayNamePos - startDisplayNamePos + 1 > 0 )
                {
                    nickName = line.mid( startDisplayNamePos, endDisplayNamePos - startDisplayNamePos + 1 );
                }
                else
                {
                    const QString USER_TYPE = "user-type=";
                    int startNickNamePos = line.indexOf( USER_TYPE ) + USER_TYPE.length();

                    startNickNamePos = line.indexOf( ":", startNickNamePos ) + 1;

                    int endNickNamePos = line.indexOf( "!", startNickNamePos ) - 1;

                    if( endNickNamePos - startNickNamePos + 1 > 0 )
                    {
                        nickName = line.mid( startNickNamePos, endNickNamePos - startNickNamePos + 1 );
                    }
                }

                if( originalColors_ && !color.isEmpty() )
                {
                    nickName = "<span style=\"color:" + color + "\">" + nickName + "</span>";
                    //nickName = "<span style=\"" + styles_[ role ] + "\">" + nickName + "</span>";
                }

                //test badges


                if( badges_ )
                {
                    QString nickNameIcons;

                    const QString USER_TYPE_PREFIX = "user-type=";

                    int startUserTypePos = line.indexOf( USER_TYPE_PREFIX ) + USER_TYPE_PREFIX.length();
                    int endUserTypePos = line.indexOf( " ", startUserTypePos ) - 1;

                    if( endUserTypePos - startUserTypePos + 1 > 0 )
                    {
                        QString userType = line.mid( startUserTypePos, endUserTypePos - startUserTypePos + 1 );

                        if( badgesMap_.contains( userType ) )
                        {
                            nickNameIcons += "<img class =\"badge\" src=\""+ badgesMap_[ userType ] + "\"></img>";
                        }
                    }



                    const QString TURBO_PREFIX = "turbo=";

                    int startTurboPos = line.indexOf( TURBO_PREFIX ) + TURBO_PREFIX.length();
                    int endTurboPos = line.indexOf( ";", startTurboPos ) - 1;

                    if( badgesMap_.contains( "turbo" ) && endTurboPos - startTurboPos + 1 > 0 && ( "1" == line.mid( startTurboPos, endTurboPos - startTurboPos + 1 ) ) )
                    {
                        nickNameIcons += "<img class =\"badge\" src=\""+ badgesMap_[ "turbo" ] + "\"></img>";
                    }

                    const QString SUBSCRIBER_PREFIX = "subscriber=";

                    int startSubscriberPos = line.indexOf( SUBSCRIBER_PREFIX ) + SUBSCRIBER_PREFIX.length();
                    int endSubscriberPos = line.indexOf( ";", startSubscriberPos ) - 1;

                    if( badgesMap_.contains( "subscriber" ) && endSubscriberPos - startSubscriberPos + 1 > 0 && ( "1" == line.mid( startSubscriberPos, endSubscriberPos - startSubscriberPos + 1 ) ) )
                    {
                        nickNameIcons += "<img class =\"badge\" src=\""+ badgesMap_[ "subscriber" ] + "\"></img>";
                    }

                    nickName = nickNameIcons + nickName;

                }

                QString messagePrefix = "PRIVMSG #" + channelName_ + " :";
                QString message;

                int startMessagePos = line.indexOf( messagePrefix ) + messagePrefix.length();
                int endMessagePos = line.indexOf( "\r\n", startMessagePos ) - 1;

                if( endMessagePos - startMessagePos + 1 > 0 )
                {
                    message = line.mid( startMessagePos, endMessagePos - startMessagePos + 1 );
                }

                message = ChatMessage::replaceEscapeCharecters( message );
                message = insertSmiles( message );

                emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
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
            loadBadges();

        }
    }
    reply->deleteLater();
}

void QTwitchChat::onSelfLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QTwitchChat::loadSmiles()
{
    QChatService::loadSmiles();

    QString smilesCodes[] = { ":)", ":(", ":D", ">(", ":|", "O_o", "B)", ":O", "&lt;3", ":/", ";)", ":P", ";P", "R)" };

    for( int i = 0; i < 14; i++ )
    {
        //qDebug() << DEFAULT_TWITCH_SMILE_PREFIX + QString::number( i + 1 ) + "/1.0";
        addSmile( smilesCodes[ i ], DEFAULT_TWITCH_SMILE_PREFIX + QString::number( i + 1 ) + "/1.0" );
    }

    //QNetworkRequest request( QUrl( emotIconsLink_ + "" ) );

    QNetworkRequest request( QUrl( "https://api.twitch.tv/kraken/chat/emoticon_images" ) );

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

                //addSmile( jsonEmotIcon[ "regex" ].toString(), jsonEmotIcon[ "url" ].toString() );

                addSmile( jsonEmotIcon[ "code" ].toString(), DEFAULT_TWITCH_SMILE_PREFIX + QString::number( jsonEmotIcon[ "id" ].toInt() ) + "/1.0" );

            }
            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
            }
        }
    }

    reply->deleteLater();
}

void QTwitchChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() );
    }

    reply->deleteLater();
}

void QTwitchChat::loadBadges()
{
    badgesMap_.clear();

    QNetworkRequest request( QUrl( badgesLink_ + "" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onBadgesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onBadgesLoadError() ) );
}

void QTwitchChat::onBadgesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );


    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonBadges = jsonDoc.object();

        QStringList keys = jsonBadges.keys();

        foreach( const QString & key, keys )
        {
            if( jsonBadges[ key ].isObject() )
            {
                QJsonObject jsonBadge = jsonBadges[ key ].toObject();

                if( jsonBadge.contains( "image" ) && jsonBadge[ "image" ].isString() )
                {
                    badgesMap_.insert( key, jsonBadge[ "image" ].toString() );
                }
            }
        }

        qDebug() << badgesMap_;

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Badges loaded..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Badges loaded..." ) );
        }
    }

    reply->deleteLater();
}

void QTwitchChat::onBadgesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );    
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load badges..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load badges..." ) + reply->errorString() );
    }
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

            emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
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

    originalColors_ = settings.value( TWITCH_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    badges_ = settings.value( TWITCH_BADGES_SETTING_PATH, false ).toBool();

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
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Socket Error ..." + socket_->errorString(), "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Socket Error ..." + socket_->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QTwitchChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}

void QTwitchChat::changeBadges( bool badges )
{
    badges_ = badges;
}
