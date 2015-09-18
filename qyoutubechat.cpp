#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include <QSettings>

#include <QList>

#include <QDebug>

#include "settingsconsts.h"
#include "qyoutubechat.h"

const QString DEFAULT_YOUTUBE_CHANNEL_INFO_PREFIX = "https://www.youtube.com/live_chat?v=";
const QString DEFAULT_YOUTUBE_CHANNEL_INFO_POSTFIX = "&dark_theme=1&from_gaming=1&client_version=1.0";

const QString DEFAULT_YOUTUBE_MESSAGES_PREFIX = "https://www.youtube.com/live_comments?action_get_comments=1&video_id=";
const QString DEFAULT_YOUTUBE_MESSAGES_INFIX = "&lt=";
const QString DEFAULT_YOUTUBE_MESSAGES_POSTFIX ="&format=json&comment_version=1";

const int DEFAULT_YOUTUBE_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_YOUTUBE_UPDATE_STATISTIC_INTERVAL = 10000;
const int DEFAULT_YOUTUBE_RECONNECT_INTERVAL = 30000;

const QString YOUTUBE_USER = "YOUTUBE";
const QString YOUTUBE_SERVICE = "youtube";


QYoutubeChat::QYoutubeChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, lastMessageTime_()
, updateMessagesTimerId_( -1 )
, updateMessagesInterval_( DEFAULT_YOUTUBE_UPDATE_MESSAGES_INTERVAL )
, updateStatisticTimerId_( -1 )
, updateStatisticInterval_( DEFAULT_YOUTUBE_UPDATE_STATISTIC_INTERVAL )
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_YOUTUBE_RECONNECT_INTERVAL )
{
}

QYoutubeChat::~QYoutubeChat()
{
    disconnect();
}

void QYoutubeChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( YOUTUBE_SERVICE, YOUTUBE_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getChannelInfo();
}

void QYoutubeChat::disconnect()
{
    lastMessageTime_ = "";
    channelName_ = "";

    if( updateMessagesTimerId_ >= 0 )
    {
        killTimer( updateMessagesTimerId_ );
        updateMessagesTimerId_ = -1;
    }

    if( updateStatisticTimerId_ >= 0 )
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

void QYoutubeChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( YOUTUBE_SERVICE, YOUTUBE_USER, "Reconnecting...", "", this ) );
    connect();
}

void QYoutubeChat::getChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_YOUTUBE_CHANNEL_INFO_PREFIX + channelName_ + DEFAULT_YOUTUBE_CHANNEL_INFO_POSTFIX ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QYoutubeChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply *  >( sender() );

    QString channelInfo = reply->readAll();

    int startLastTimePos = channelInfo.indexOf( "\'COMMENTS_LATEST_TIMESTAMP\'" );
    startLastTimePos = channelInfo.indexOf( ':', startLastTimePos ) + 1;
    int endLastTimePos = channelInfo.indexOf( ',', startLastTimePos ) - 1;

    lastMessageTime_ = channelInfo.mid( startLastTimePos,endLastTimePos - startLastTimePos + 1 );

    lastMessageTime_.replace( ' ', "" );

    if( !lastMessageTime_.isEmpty() )
    {
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( YOUTUBE_SERVICE, YOUTUBE_USER, "Connected to " + channelName_ + "...", "", this ) );

        if( updateMessagesTimerId_ == -1 )
            updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );
    }

    reply->deleteLater();
}

void QYoutubeChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply *  >( sender() );
    reply->deleteLater();
}

void QYoutubeChat::getMessages()
{
    QNetworkRequest request( QUrl( DEFAULT_YOUTUBE_MESSAGES_PREFIX + channelName_ + DEFAULT_YOUTUBE_MESSAGES_INFIX + lastMessageTime_ + DEFAULT_YOUTUBE_MESSAGES_POSTFIX ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onMessagesLoadError() ) );

}

bool cmpJsonObjectByTimeCreated( const QJsonObject &lho, const QJsonObject& rho )
{
    return lho[ "time_created" ].toInt() < rho[ "time_created" ].toInt();
}

void QYoutubeChat::onMessagesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    const QString HTML_CONTENT_START = "<html_content><![CDATA[";
    const QString HTML_CONTENT_END = "]]></html_content>";

    QString messagesData = reply->readAll();

    int startMessagesDataPos = messagesData.indexOf( HTML_CONTENT_START ) + HTML_CONTENT_START.length();
    int endMessagesDataPos = messagesData.indexOf( HTML_CONTENT_END, startMessagesDataPos ) - 1;

    messagesData = messagesData.mid( startMessagesDataPos, endMessagesDataPos - startMessagesDataPos + 1 );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( messagesData.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QJsonArray jsonMessagesArr = jsonObj[ "comments" ].toArray();

            QList< QJsonObject > messagesList;

            foreach( const QJsonValue & value, jsonMessagesArr )
            {
                messagesList.append( value.toObject() );
            }

            qSort( messagesList.begin(), messagesList.end(), cmpJsonObjectByTimeCreated );

            for( int i = 0; i < messagesList.size(); ++i )
            {
                QJsonObject jsonMessageInfo = messagesList[ i ];

                QString message = jsonMessageInfo[ "comment" ].toString();
                QString nickName = jsonMessageInfo[ "author_name" ].toString();

                bool blackListUser = blackList().contains( nickName );
                bool supportersListUser = supportersList().contains( nickName );

                if( !isRemoveBlackListUsers() || !blackListUser )
                {
                    if( blackListUser )
                    {
                        //TODO: игнорируемые
                        emit newMessage( new QChatMessage( YOUTUBE_SERVICE, nickName, message, "ignore", this ) );
                    }
                    else
                    {
                        if( supportersListUser )
                        {
                            //TODO: саппортеры
                            emit newMessage( new QChatMessage( YOUTUBE_SERVICE, nickName, message, "supporter", this ) );
                        }
                        else
                        {
                            if( isContainsAliases( message ) )
                            {
                                //TODO: сообщение к стримеру
                                emit newMessage( new QChatMessage( YOUTUBE_SERVICE, nickName, message, "alias", this ) );
                            }
                            else
                            {
                                //TODO: простое сообщение
                                emit newMessage( new QChatMessage( YOUTUBE_SERVICE, nickName, message, "", this ) );
                            }
                        }
                    }
                }
            }

            lastMessageTime_ = QString::number( jsonObj[ "latest_time" ].toInt() );
        }
    }

    reply->deleteLater();
}

void QYoutubeChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QYoutubeChat::getStatistic()
{
}

void QYoutubeChat::onStatisticLoaded()
{
}

void QYoutubeChat::onStatisticLoadError()
{
}

void QYoutubeChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == updateMessagesTimerId_ && channelName_ != "" )
    {
        getMessages();
    }
    else if( event->timerId() == updateStatisticTimerId_ && channelName_ != "" )
    {
        getStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QYoutubeChat::loadSettings()
{    
    QSettings settings;
    channelName_ = settings.value( YOUTUBE_CHANNEL_SETTING_PATH, DEFAULT_YOUTUBE_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( YOUTUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
