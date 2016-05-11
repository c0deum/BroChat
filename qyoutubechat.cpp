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

#include "qchatmessage.h"

#include "settingsconsts.h"
#include "qyoutubechat.h"

const QString DEFAULT_YOUTUBE_CHANNEL_INFO_PREFIX = "https://www.youtube.com/live_chat?v=";
const QString DEFAULT_YOUTUBE_CHANNEL_INFO_POSTFIX = "&dark_theme=1&from_gaming=1&client_version=1.0";

const QString DEFAULT_YOUTUBE_MESSAGES_PREFIX = "https://www.youtube.com/live_comments?action_get_comments=1&video_id=";
const QString DEFAULT_YOUTUBE_MESSAGES_INFIX = "&lt=";
const QString DEFAULT_YOUTUBE_MESSAGES_POSTFIX ="&format=json&comment_version=1";

const QString DEFAULT_YOUTUBE_STATISTIC_PREFIX = "https://www.youtube.com/live_stats?v=";

const QString QYoutubeChat::SERVICE_NAME = "youtube";
const QString QYoutubeChat::SERVICE_USER_NAME = "YOUTUBE";

const int QYoutubeChat::UPDATE_INTERVAL = 3000;
const int QYoutubeChat::STATISTIC_INTERVAL = 10000;
const int QYoutubeChat::RECONNECT_INTERVAL = 30000;

QYoutubeChat::QYoutubeChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QYoutubeChat::~QYoutubeChat()
{
    disconnect();
}

void QYoutubeChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connecting to " + channelName_ + "...", "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connecting to " + channelName_ + "..." );
    }

    loadChannelInfo();        
    //loadSmiles();
}

void QYoutubeChat::disconnect()
{
    lastMessageTime_.clear();
    channelName_.clear();

    resetTimer( updateMessagesTimerId_ );
    resetTimer( updateStatisticTimerId_ );
    resetTimer( reconnectTimerId_ );

    emit newStatistic( new QChatStatistic( SERVICE_NAME, "", this ) );
}

void QYoutubeChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() )
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Reconnecting...", "", this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Reconnecting..." );
        }
    connect();
}

void QYoutubeChat::loadChannelInfo()
{
    //QNetworkRequest request( QUrl( DEFAULT_YOUTUBE_CHANNEL_INFO_PREFIX + channelName_ + DEFAULT_YOUTUBE_CHANNEL_INFO_POSTFIX ) );
    QNetworkRequest request( QUrl( "https://www.googleapis.com/youtube/v3/comments?part=id,snippet&id=" + channelName_ + "&maxResults=5&textFormat=plainText&key=AIzaSyDAp9c9ueG5c0isR-GmTaQU6DulVh_sAmQ" ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QYoutubeChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply *  >( sender() );

    QByteArray test = reply->readAll();

    reply->deleteLater();

    /*
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
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "...", "", this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "..." );
        }

        loadSmiles();

        startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );

        loadStatistic();

        startUniqueTimer( updateStatisticTimerId_, STATISTIC_INTERVAL );
    }

    reply->deleteLater();
    */
}

void QYoutubeChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply *  >( sender() );
    reply->deleteLater();
}

void QYoutubeChat::loadMessages()
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

    //qDebug() << reply->readAll();

    /*
    const QString COMMENTS_START = "<comments><![CDATA[";
    const QString COMMENTS_END = "]]></comments>";

    QByteArray messagesData = reply->readAll();

    qDebug() << "Messages data: " << messagesData;

    int startCommentsPos = messagesData.indexOf( COMMENTS_START ) + COMMENTS_START.length();
    int endCommentsPos = messagesData.indexOf( COMMENTS_END, startCommentsPos ) - 1;

    if( endCommentsPos - startCommentsPos + 1 > 0 )
    {
        QByteArray commentsData = messagesData.mid( startCommentsPos, endCommentsPos - startCommentsPos + 1 );

        qDebug() << "Comments data: " << commentsData;

        unsigned int size = *( reinterpret_cast< unsigned int * >( commentsData.data() + 18 ) );

        qDebug() << "size: " << size;

        QByteArray uncompressedCommentsData = qUncompress( commentsData );

        qDebug() << uncompressedCommentsData;



    }

    */




    /*

    int startMessagesDataPos = messagesData.indexOf( HTML_CONTENT_START ) + HTML_CONTENT_START.length();
    int endMessagesDataPos = messagesData.indexOf( HTML_CONTENT_END, startMessagesDataPos ) - 1;

    messagesData = messagesData.mid( startMessagesDataPos, endMessagesDataPos - startMessagesDataPos + 1 );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( messagesData.toStdString().c_str() ), &parseError );

    if( QJsonParseError::NoError == parseError.error )
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

                message = insertSmiles( message );

                emit newMessage( ChatMessage( YOUTUBE_SERVICE, nickName, message, "", this ) );
            }

            lastMessageTime_ = QString::number( jsonObj[ "latest_time" ].toInt() );
        }
    }
    */


    reply->deleteLater();
}

void QYoutubeChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QYoutubeChat::loadStatistic()
{
    //if( channelName_.isEmpty() )
    //  return;

    QNetworkRequest request( QUrl( DEFAULT_YOUTUBE_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );

}

void QYoutubeChat::onStatisticLoaded()
{

    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString statistic = reply->readAll();

    bool isOk = false;


    statistic.toInt( &isOk );

    if( !isOk )
        statistic = "0";

    emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );

    reply->deleteLater();

}

void QYoutubeChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    reply->deleteLater();
}

void QYoutubeChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == updateMessagesTimerId_ && !channelName_.isEmpty() )
    {
        loadMessages();
    }
    else if( event->timerId() == updateStatisticTimerId_ && !channelName_.isEmpty() )
    {
        loadStatistic();
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

    enable( settings.value( YOUTUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( YOUTUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( YOUTUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( YOUTUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( YOUTUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
