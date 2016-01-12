#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>

#include <QSettings>

#include <QTimerEvent>

#include "qchatmessage.h"

#include "settingsconsts.h"
#include "qgipsyteamchat.h"

const QString DEFAULT_GIPSYTEAM_CHANNEL_INFO_PREFIX = "http://www.gipsyteam.ru/live/stream?streamer=";
const QString DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX = "http://www.gipsyteam.ru/chat/streams/messages/";
const QString DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX = ".js";

const int DEFAULT_GIPSYTEAM_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_GIPSYTEAM_RECONNECT_INTERVAL = 10000;

const QString SERVICE_NAME = "gipsyteam";
const QString SERVICE_USER_NAME = "GIPSYTEAM";

const QString QGipsyTeamChat::SERVICE_NAME = "gipsyteam";
const QString QGipsyTeamChat::SERVICE_USER_NAME = "GIPSYTEAM";

const int QGipsyTeamChat::UPDATE_INTERVAL = 3000;
const int QGipsyTeamChat::RECONNECT_INTERVAL = 10000;

QGipsyTeamChat::QGipsyTeamChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QGipsyTeamChat::~QGipsyTeamChat()
{
    disconnect();
}

void QGipsyTeamChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    channelLink_ = DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX + channelName_ + DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    loadSmiles();

    startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );

}

void QGipsyTeamChat::disconnect()
{
    channelLink_.clear();
    lastMessageId_.clear();

    resetTimer( updateMessagesTimerId_ );
    resetTimer( reconnectTimerId_ );
}

void QGipsyTeamChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting to " ) + channelName_ + tr( "..." ) );
    }
    connect();
}

void QGipsyTeamChat::loadChannelInfo()
{
    QNetworkRequest request( DEFAULT_GIPSYTEAM_CHANNEL_INFO_PREFIX + channelName_ );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QGipsyTeamChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString channelInfo = reply->readAll();


    const QString CHANNEL_ID_PREFIX = "?id=";

    int startPos =  channelInfo.indexOf( CHANNEL_ID_PREFIX ) + CHANNEL_ID_PREFIX.length();
    int endPos = channelInfo.indexOf( ".", startPos );


    if( endPos - startPos > 0 )
    {
        QString channelId = channelInfo.mid( startPos, endPos - startPos );

        channelLink_ = DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX + channelId + DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX;

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
        }

        startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );
    }
    else
    {
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) );
        }

        startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
    }

    reply->deleteLater();
}

void QGipsyTeamChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QGipsyTeamChat::loadMessages()
{
    QNetworkRequest request( QUrl( channelLink_ + "" ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onMessagesLoadError() ) );
}

void QGipsyTeamChat::onMessagesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QStringList messagesId = jsonObj.keys();

            if( !lastMessageId_.isEmpty() )
            {
                foreach( const QString & id, messagesId )
                {
                    if( id > lastMessageId_ )
                    {
                        QString messageData = jsonObj[ id ].toString();

                        const QString NICKNAME_PREFIX = "<strong>";
                        const QString NICKNAME_POSTFIX = "</strong>";

                        int startPos = messageData.indexOf( NICKNAME_PREFIX ) + NICKNAME_PREFIX.length();
                        int endPos = messageData.indexOf( NICKNAME_POSTFIX, startPos );

                        QString nickName = messageData.mid( startPos, endPos - startPos );

                        const QString MESSAGE_PREFIX = "<span>";
                        const QString MESSAGE_POSTFIX = "</span>";

                        startPos = messageData.indexOf( MESSAGE_PREFIX, endPos ) + MESSAGE_PREFIX.length();
                        endPos = messageData.indexOf( MESSAGE_POSTFIX, startPos );

                        QString message = messageData.mid( startPos, endPos - startPos );

                        message = insertSmiles( message );

                        emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
                    }
                }
            }
            lastMessageId_ = messagesId.last();
        }
    }

    reply->deleteLater();
}

void QGipsyTeamChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QGipsyTeamChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == updateMessagesTimerId_ )
    {
        loadMessages();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QGipsyTeamChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( GIPSYTEAM_CHANNEL_SETTING_PATH, DEFAULT_GIPSYTEAM_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "=" ) - 1 );

    enable( settings.value( GIPSYTEAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( GIPSYTEAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GIPSYTEAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
