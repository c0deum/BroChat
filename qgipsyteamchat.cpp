#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>

#include <QSettings>

#include <QTimerEvent>

#include "settingsconsts.h"
#include "qgipsyteamchat.h"

const QString DEFAULT_GIPSYTEAM_CHANNEL_INFO_PREFIX = "http://www.gipsyteam.ru/live/stream?streamer=";

const QString DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX = "http://www.gipsyteam.ru/chat/streams/messages/";
const QString DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX = ".js";

const int DEFAULT_GIPSYTEAM_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_GIPSYTEAM_RECONNECT_INTERVAL = 10000;

const QString GIPSYTEAM_SERVICE = "gipsyteam";
const QString GIPSYTEAM_USER = "GIPSYTEAM";

QGipsyTeamChat::QGipsyTeamChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, channelLink_()
, lastMessageId_()
, updateMessagesTimerId_( -1 )
, updateMessagesInterval_( DEFAULT_GIPSYTEAM_UPDATE_MESSAGES_INTERVAL )
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_GIPSYTEAM_RECONNECT_INTERVAL )
{
}

QGipsyTeamChat::~QGipsyTeamChat()
{
    disconnect();
}

void QGipsyTeamChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, GIPSYTEAM_USER, "Connecting to " + channelName_ + "...", "", this ) );

    channelLink_ = DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX + channelName_ + DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, GIPSYTEAM_USER, "Connected to " + channelName_ + "...", "", this ) );

    if( updateMessagesTimerId_ == -1 )
        updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );

    //getChannelInfo();
}

void QGipsyTeamChat::disconnect()
{
    //channelName_ = "";
    channelLink_ = "";
    lastMessageId_ = "";

    if( updateMessagesTimerId_ >= 0  )
    {
        killTimer( updateMessagesTimerId_ );
        updateMessagesTimerId_ = -1;
    }

    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }
}

void QGipsyTeamChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, GIPSYTEAM_USER, "Reconnecting to " + channelName_ + "...", "", this ) );
    connect();
}

void QGipsyTeamChat::getChannelInfo()
{
    QNetworkRequest request( DEFAULT_GIPSYTEAM_CHANNEL_INFO_PREFIX + channelName_ );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QGipsyTeamChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString channelInfo = reply->readAll();


    const QString CHANNEL_ID_PREFIX = "?id=";

    int startPos =  channelInfo.indexOf( CHANNEL_ID_PREFIX ) + CHANNEL_ID_PREFIX.length();
    int endPos = channelInfo.indexOf( ".", startPos );


    if( endPos - startPos > 0 )
    {
        QString channelId = channelInfo.mid( startPos, endPos - startPos );

        //qDebug() << channelId;

        channelLink_ = DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_PREFIX + channelId + DEFAULT_GIPSYTEAM_CHANNEL_MESSAGES_POSTFIX;


        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, GIPSYTEAM_USER, "Connected to " + channelName_ + "...", "", this ) );

        if( updateMessagesTimerId_ == -1 )
            updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );
    }
    else
    {

        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, GIPSYTEAM_USER, "Can not connect to " + channelName_ + "...", "", this ) );

        if( reconnectTimerId_ == -1 )
            reconnectTimerId_ = startTimer( reconnectInterval_ );
    }

    reply->deleteLater();
}

void QGipsyTeamChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QGipsyTeamChat::loadMessages()
{
    QNetworkRequest request( QUrl( channelLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onMessagesLoadError() ) );
}

void QGipsyTeamChat::onMessagesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QStringList messagesId = jsonObj.keys();

            if( lastMessageId_ != "" )
            {

                foreach( const QString& id, messagesId )
                {
                    if( id > lastMessageId_ )
                    {
                        QString messageData = jsonObj[ id ].toString();

    //                    /qDebug() << messageData;

                        const QString NICKNAME_PREFIX = "<strong>";
                        const QString NICKNAME_POSTFIX = "</strong>";

                        int startPos = messageData.indexOf( NICKNAME_PREFIX ) + NICKNAME_PREFIX.length();
                        int endPos = messageData.indexOf( NICKNAME_POSTFIX, startPos );

                        QString nickName = messageData.mid( startPos, endPos - startPos );

                        //qDebug() << nickName;

                        const QString MESSAGE_PREFIX = "<span>";
                        const QString MESSAGE_POSTFIX = "</span>";

                        startPos = messageData.indexOf( MESSAGE_PREFIX, endPos ) + MESSAGE_PREFIX.length();
                        endPos = messageData.indexOf( MESSAGE_POSTFIX, startPos );

                        QString message = messageData.mid( startPos, endPos - startPos );

                        //qDebug() << message;

                        emit newMessage( new QChatMessage( GIPSYTEAM_SERVICE, nickName, message, "", this ) );
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
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QGipsyTeamChat::timerEvent( QTimerEvent *event )
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
    setAliasesList( settings.value( GIPSYTEAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GIPSYTEAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GIPSYTEAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( GIPSYTEAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
