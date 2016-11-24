#include <QNetworkAccessManager>

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QSettings>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include "../core/qchatmessage.h"

#include "../core/settingsconsts.h"

#include "qigdcchat.h"

const QString DEFAULT_IGDC_CHANNEl_INFO_LINK_PREFIX = "http://igdc.ru/streams/?channel=";
const QString DEFAULT_IGDC_MESSAGES_INFO_LINK_PREFIX = "http://igdc.ru/streams/chat.php?";
const QString DEFAULT_IGDC_STATISTIC_INFO_LINK_PREFIX = "http://igdc.ru/streams/chat.php?";

const QString QIgdcChat::SERVICE_NAME = "igdc";
const QString QIgdcChat::SERVICE_USER_NAME = "IGDC";

const int QIgdcChat::UPDATE_INTERVAL = 3000;
const int QIgdcChat::RECONNECT_INTERVAL = 10000;
const int QIgdcChat::STATISTIC_INTERVAL = 10000;

QIgdcChat::QIgdcChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QIgdcChat::~QIgdcChat()
{
    disconnect();
}

void QIgdcChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
}

void QIgdcChat::disconnect()
{
    channelId_.clear();
    lastMessageId_ = "0";

    resetTimer( updateMessagesTimerId_ );
    resetTimer( reconnectTimerId_ );

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QIgdcChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ) );
    }
    connect();
}


void QIgdcChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == updateMessagesTimerId_ && !channelId_.isEmpty() )
    {
        loadMessages();
    }
    else if( event->timerId() == statisticTimerId_ && !channelId_.isEmpty() )
    {
        loadStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QIgdcChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( IGDC_CHANNEL_SETTING_PATH, DEFAULT_IGDC_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "=" ) - 1 );

    badges_ = settings.value( IGDC_BADGES_SETTING_PATH, false ).toBool();

    enable( settings.value( IGDC_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( IGDC_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( IGDC_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( IGDC_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( IGDC_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QIgdcChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_IGDC_CHANNEl_INFO_LINK_PREFIX + channelName_ ) );
    QNetworkReply * reply = nam_->get( request  );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QIgdcChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString channelInfo = reply->readAll();

    const QString ID_PREFIX = "var channel_id =";
    const QString ID_POSTFIX = ";";

    int startChannelIdPos = channelInfo.indexOf( ID_PREFIX ) + ID_PREFIX.length();
    int endChannelPos = channelInfo.indexOf( ID_POSTFIX, startChannelIdPos ) - 1;

    if( endChannelPos - startChannelIdPos > 0  )
    {
        channelId_ = channelInfo.mid( startChannelIdPos, endChannelPos - startChannelIdPos + 1 );
        channelId_.replace( " ", "" );

        qDebug() << "channelId_ = " << channelId_;

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
        }

        loadSmiles();

        startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );

        loadStatistic();

        startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );

    }
    else
    {
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "...Can not find channel id..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "...Can not find channel id..." ) );
        }
    }

    reply->deleteLater();
}

void QIgdcChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() + tr( "..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}


void QIgdcChat::loadMessages()
{
    QNetworkRequest request( QUrl( DEFAULT_IGDC_MESSAGES_INFO_LINK_PREFIX + "channel=" + channelId_ + "&max_id=" + lastMessageId_ ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onMessagesLoadError() ) );
}

bool igdcCmpJsonObject( const QJsonObject &lho, const QJsonObject& rho )
{
    return lho[ "id" ].toString() < rho[ "id" ].toString();
}

void QIgdcChat::onMessagesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QJsonArray jsonMessages = jsonObj[ "messages" ].toArray();

            QList< QJsonObject > jsonMessagesList;

            foreach( const QJsonValue &value, jsonMessages )
            {
                jsonMessagesList.append( value.toObject() );
            }

            qSort( jsonMessagesList.begin(), jsonMessagesList.end(), igdcCmpJsonObject );


            if( "0" != lastMessageId_ )
            {
                int messageIndex = 0;
                while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "id" ].toString() <= lastMessageId_ )
                    ++messageIndex;
                while( messageIndex < jsonMessagesList.size() )
                {
                    QJsonObject jsonMessage = jsonMessagesList[ messageIndex ];

                    QString message = jsonMessage[ "text" ].toString();

                    QJsonObject jsonUserInfo = jsonMessage[ "user" ].toObject();

                    QString nickName = jsonUserInfo[ "name" ].toString();

                    //test badges
                    if( badges_ && jsonUserInfo[ "avatar" ].isString() )
                    {
                        nickName = "<img class =\"badge\" src=\"http://igdc.ru/images/avatars/" + jsonUserInfo[ "avatar" ].toString() + "\"></img>" + nickName;
                    }

                    message = insertSmiles( message );

                    emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );

                    ++messageIndex;
                }
            }

            if( jsonMessagesList.size() > 0 )
            {
                lastMessageId_ = jsonMessagesList.last()[ "id" ].toString();
            }
        }
    }

    reply->deleteLater();
}

void QIgdcChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QIgdcChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_IGDC_STATISTIC_INFO_LINK_PREFIX + "channel=" + channelId_ + "&online" ) );
    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStatisticLoadError() ) );
}

void QIgdcChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        emit newStatistic( new QChatStatistic( SERVICE_NAME, QString::number( jsonObj[ "count" ].toInt() ), this ) );
    }

    reply->deleteLater();
}

void QIgdcChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QIgdcChat::changeBadges( bool badges )
{
    badges_ = badges;
}


