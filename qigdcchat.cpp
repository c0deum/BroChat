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

#include "settingsconsts.h"

#include "qigdcchat.h"

const QString DEFAULT_IGDC_CHANNEl_INFO_LINK_PREFIX = "http://igdc.ru/streams/?channel=";
const QString DEFAULT_IGDC_MESSAGES_INFO_LINK_PREFIX = "http://igdc.ru/streams/chat.php?";

const QString IGDC_SERVICE = "igdc";
const QString IGDC_USER = "IGDC";

const int DEFAULT_IGDC_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_IGDC_RECONNECT_INTERVAL = 10000;


QIgdcChat::QIgdcChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, channelId_()
, lastMessageId_( "0" )
, updateMessagesTimerId_( -1 )
, updateMessagesInterval_( DEFAULT_IGDC_UPDATE_MESSAGES_INTERVAL )
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_IGDC_RECONNECT_INTERVAL )
{
}

QIgdcChat::~QIgdcChat()
{
    disconnect();
}

void QIgdcChat::connect()
{
    if( !isEnabled() || channelName_ == "" )
        return;


    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( IGDC_SERVICE, IGDC_USER, "Connecting to " + channelName_ + "...", "", this ) );

    loadChannelInfo();
}

void QIgdcChat::disconnect()
{
    channelId_ = "";
    lastMessageId_ = "0";
    if( updateMessagesTimerId_ >= 0 )
    {
        killTimer( updateMessagesTimerId_ );
        updateMessagesTimerId_ = -1;
    }
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }

    emit newStatistic( new QChatStatistic( IGDC_SERVICE, "", this ) );
}

void QIgdcChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( IGDC_SERVICE, IGDC_USER, "Reconnecting...", "", this ) );
    connect();
}


void QIgdcChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == updateMessagesTimerId_ && channelId_ != "" )
    {
        loadMessages();
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

    if( QChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "=" ) - 1 );

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
            emit newMessage( new QChatMessage( IGDC_SERVICE, IGDC_USER, "Connected to " + channelName_ + "...", "", this ) );

        if( updateMessagesTimerId_ == -1 )
            updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );

    }
    else
    {
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( IGDC_SERVICE, IGDC_USER, "Can not connect to " + channelName_ + "...Can not find channel id...", "", this ) );
    }


    reply->deleteLater();
}

void QIgdcChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( IGDC_SERVICE, IGDC_USER, "Can not connect to " + channelName_ + "..." + reply->errorString() + "...", "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

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

    //qDebug() << reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
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


            if( lastMessageId_ != "0" )
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

                    bool blackListUser = blackList().contains( nickName );
                    bool supportersListUser = supportersList().contains( nickName );

                    if( !isRemoveBlackListUsers() || !blackListUser )
                    {
                        if( blackListUser )
                        {
                            //TODO: игнорируемые
                            emit newMessage( new QChatMessage( IGDC_SERVICE, nickName, message, "ignore", this ) );
                        }
                        else
                        {
                            if( supportersListUser )
                            {
                                //TODO: саппортеры
                                emit newMessage( new QChatMessage( IGDC_SERVICE, nickName, message, "supporter", this ) );
                            }
                            else
                            {
                                if( isContainsAliases( message ) )
                                {
                                    //TODO: сообщение к стримеру
                                    emit newMessage( new QChatMessage( IGDC_SERVICE, nickName, message, "alias", this ) );
                                }
                                else
                                {
                                    //TODO: простое сообщение
                                    emit newMessage( new QChatMessage( IGDC_SERVICE, nickName, message, "", this ) );
                                }
                            }
                        }
                    }
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
