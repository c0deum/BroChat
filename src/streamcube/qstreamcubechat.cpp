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


#include "../core/settingsconsts.h"

#include "../core/qcoremediator.h"

#include "qstreamcubechat.h"

//ChatServiceRegistrator< QStreamCubeChat > streamcubeChatRegistrator;

const QString QStreamCubeChat::SERVICE_NAME = "streamcube";
const QString QStreamCubeChat::SERVICE_USER_NAME = "STREAMCUBE";

const int QStreamCubeChat::UPDATE_INTERVAL = 3000;
const int QStreamCubeChat::RECONNECT_INTERVAL = 10000;


const QString DEFAULT_STREAM_CUBE_REQUEST_LINK = "http://build.strimim.ru/chat/upd";
const QString DEFAULT_STREAM_CUBE_SMILES_INFO_LINK = "http://build.strimim.ru/chat/api/smile";
const QString DEFAULT_STREAM_CUBE_CHANNEL_INFO_LINK_PREFIX = "http://build.strimim.ru/chat/api/?url=";
const QString DEFAULT_STREAM_CUBE_SMILE_PREFIX = "http://build.strimim.ru";

QStreamCubeChat::QStreamCubeChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QStreamCubeChat::~QStreamCubeChat()
{
    disconnect();
}

void QStreamCubeChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
    loadSmiles();
}

void QStreamCubeChat::disconnect()
{
    channelId_.clear();
    //lastMessageId_.clear();
    lastMessageId_ = "0";

    resetTimer( updateTimerId_ );
    resetTimer( reconnectTimerId_ );
}

void QStreamCubeChat::reconnect()
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

void QStreamCubeChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_STREAM_CUBE_CHANNEL_INFO_LINK_PREFIX + channelName_ ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QStreamCubeChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isArray() )
    {
        QJsonArray jsonArr = jsonDoc.array();

        channelId_ = jsonArr[ 0 ].toString();

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
        }

        loadMessages();
        startUniqueTimer( updateTimerId_, UPDATE_INTERVAL );
    }

    reply->deleteLater();
}

void QStreamCubeChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
    reply->deleteLater();
}

void QStreamCubeChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_STREAM_CUBE_SMILES_INFO_LINK + "") );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSmilesLoadError() ) );
}

void QStreamCubeChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isArray() )
    {
        QJsonArray jsonArr =jsonDoc.array();

        for( const QJsonValue & smileVal : jsonArr )
        {
            QJsonArray smileInfo = smileVal.toArray();

            addSmile( smileInfo[ 0 ].toString(), DEFAULT_STREAM_CUBE_SMILE_PREFIX + smileInfo[ 1 ].toString() );
        }

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
        }
    }
    else
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not laod smiles..." ) );
    }

    reply->deleteLater();
}

void QStreamCubeChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not laod smiles..." ) );
    }

    reply->deleteLater();
}

void QStreamCubeChat::loadMessages()
{
    QNetworkRequest request( DEFAULT_STREAM_CUBE_REQUEST_LINK );

    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );

    QByteArray data;

    QString dataString = "last=" + lastMessageId_ + "&tv=" + channelId_ + "&to=0";

    data.append( dataString );

    QNetworkReply * reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onMessagesLoadError() ) );
}

void QStreamCubeChat::onMessagesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QByteArray data = reply->readAll();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( data, &parseError );

    if( QJsonParseError::NoError == parseError.NoError && jsonDoc.isArray() )
    {
        QJsonArray jsonMessagesArr = jsonDoc.array();

        if( "0" == lastMessageId_ )
        {
            for( const QJsonValue & messageValue : jsonMessagesArr )
            {
                QJsonArray messageInfoArr = messageValue.toArray();

                if( messageInfoArr[ 1 ].toString().toInt() > lastMessageId_.toInt() )
                    lastMessageId_ = messageInfoArr[ 1 ].toString();
            }
        }
        else
        {
            for( const QJsonValue & messageValue : jsonMessagesArr )
            {
                QJsonArray messageInfoArr = messageValue.toArray();

                if( messageInfoArr[ 1 ].toString().toInt() > lastMessageId_.toInt() )
                {
                    QString nickName = messageInfoArr[ 3 ].toString();
                    QString message = messageInfoArr[ 5 ].toString();

                    if( "1" == messageInfoArr[ 0 ].toString() )
                    {
                        message = messageInfoArr[ 7 ].toString() + ", " + message;
                    }

                    message = insertSmiles( message );

                    lastMessageId_ = messageInfoArr[ 1 ].toString();

                    emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
                }
            }
        }
    }

    reply->deleteLater();
}

void QStreamCubeChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load messages..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not laod messages..." ) );
    }

    reply->deleteLater();
}

void QStreamCubeChat::timerEvent( QTimerEvent * event )
{
    if( updateTimerId_ == event->timerId() )
    {
        loadMessages();
    }
    else if( reconnectTimerId_ == event->timerId() )
    {
        reconnect();
    }
}

void QStreamCubeChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( STREAMCUBE_CHANNEL_SETTING_PATH, DEFAULT_STREAMCUBE_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( STREAMCUBE_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( STREAMCUBE_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( STREAMCUBE_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( STREAMCUBE_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( STREAMCUBE_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

