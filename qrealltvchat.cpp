#include <QCoreApplication>
#include <QSettings>
#include <QUuid>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include <QXmppClient.h>
#include <QXmppMessage.h>
#include <QXmppPresence.h>
#include <QXmppIq.h>

#include <QXmppMucManager.h>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "qchatmessage.h"

#include "settingsconsts.h"


#include "qrealltvchat.h"

const QString DEFAULT_REALLTV_JID = "903@reall.tv";
const QString DEFAULT_REALLTV_PASSWORD = "8e4820297b36ec893f1242bc36ffc1e3";

const QString DEFAULT_REALLTV_CANDY_JS_LINK = "http://reall.tv/js/candy/candy.min.js";
const QString DEFAULT_REALLTV_SMILES_PREFIX = "http://reall.tv/img/emoticons/";
const QString DEFAULT_REALLTV_CONFERENCE_JID_POSTFIX = "@conference.reall.tv";
const QString DEFAULT_REALLTV_CHANNEL_INFO_LINK = "http://reall.tv/channel/get_cid";
const QString DEFAULT_REALLTV_STATICTIC_LINK_PREFIX = "http://reall.tv/api/client/status/c/";

const QString QReallTvChat::SERVICE_NAME = "realltv";
const QString QReallTvChat::SERVICE_USER_NAME = "REALLTV";

const int QReallTvChat::RECONNECT_INTERVAL = 10000;
const int QReallTvChat::STATISTIC_INTERVAL = 10000;

QReallTvChat::QReallTvChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QReallTvChat::~QReallTvChat()
{
    disconnect();
}

void QReallTvChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    qsrand( QDateTime::currentDateTime().toTime_t() );

    xmppClient_ = new QXmppClient( this );

    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError( QXmppClient::Error ) ) );
    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );

    mucManager_ = new QXmppMucManager();

    xmppClient_->addExtension( mucManager_ );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    QXmppConfiguration conf;

    conf.setDomain( "reall.tv" );
    conf.setSaslAuthMechanism( "ANONYMOUS" );

    xmppClient_->connectToServer( conf );
}

void QReallTvChat::disconnect()
{
    cid_.clear();

    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );

    if( mucManager_ )
    {
        if( !mucManager_->rooms().empty() )
        {
            mucManager_->rooms().first()->leave();
            mucManager_->rooms().first()->deleteLater();
        }

        mucManager_->deleteLater();
        mucManager_ = nullptr;
    }

    if( xmppClient_ )
    {
        xmppClient_->disconnectFromServer();
        xmppClient_->deleteLater();

        xmppClient_ = nullptr;
    }

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QReallTvChat::reconnect()
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

void QReallTvChat::onConnected()
{
    connectionTime_ = QDateTime::currentDateTimeUtc();

    QXmppMucRoom *room = mucManager_->addRoom( channelName_ + DEFAULT_REALLTV_CONFERENCE_JID_POSTFIX );
    room->setNickName( "broadcasterchat" + QUuid::createUuid().toString() );
    room->join();

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    loadSmiles();

    loadChannelInfo();
}

void QReallTvChat::onError( QXmppClient::Error )
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Unknown Error ..." ) );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QReallTvChat::onMessageReceived( const QXmppMessage &message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {
        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        QString messageBody = message.body();

        int dotPos = messageBody.indexOf( "·" );
        if( -1 != dotPos )
        {
            messageBody = messageBody.right( messageBody.length() - dotPos - 1 );

            //test badges
            if( badges_ )
            {
                nickName = "<img class =\"badge\" src=\"http://reall.tv/images/avatars/" + messageBody.left( dotPos - 1 ) + "\"></img>" + nickName;
            }
        }                        


        messageBody = ChatMessage::replaceEscapeCharecters( messageBody );

        messageBody = insertSmiles( messageBody );

        emit newMessage( ChatMessage( SERVICE_NAME, nickName, messageBody, QString(), this ) );
    }
}

void QReallTvChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl(  DEFAULT_REALLTV_CHANNEL_INFO_LINK + "" ) );

    QByteArray data;

    QString dataString = "{\"data\":{\"nick\":\"" + channelName_ + "\"}}";

    data.append( dataString );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/json;charset=utf-8" ) );
    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QReallTvChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();
            QJsonObject jsonInfo = jsonObj[ "info" ].toObject();

            cid_ = QString::number( jsonInfo[ "cid" ].toInt() );
        }
    }

    loadStatistic();

    startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );

    reply->deleteLater();
}

void QReallTvChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QReallTvChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_REALLTV_STATICTIC_LINK_PREFIX + cid_ ) );

    QNetworkReply *reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QReallTvChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString statistic = QString::number( jsonObj[ "viewers" ].toInt() );

            emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
        }
    }

    reply->deleteLater();
}

void QReallTvChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QReallTvChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_REALLTV_CANDY_JS_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QReallTvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString response = reply->readAll();

    const QString EMOTICONS = "emoticons:";

    int emotIconsPos = response.indexOf( EMOTICONS );

    if( emotIconsPos )
    {
        int emotIconsArrayStart = emotIconsPos + EMOTICONS.length();

        int emotIconsArrayEnd = response.indexOf( ']', emotIconsArrayStart );

        response = response.mid( emotIconsArrayStart, emotIconsArrayEnd - emotIconsArrayStart + 1 );

        response.replace( "plain", "\"plain\"" );
        response.replace( "regex:", "\"regex\":\"" );
        response.replace( "/gim", "/gim\"" );
        response.replace( "/gm", "/gm\"" );
        response.replace( "image", "\"image\"" );

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( response.toStdString().c_str() ), &parseError );
        if( QJsonParseError::NoError == parseError.error )
        {
            if( jsonDoc.isArray() )
            {
                QJsonArray smilesInfoArr = jsonDoc.array();

                foreach( const QJsonValue &value, smilesInfoArr )
                {
                    QJsonObject smileInfo = value.toObject();

                    addSmile( smileInfo[ "plain" ].toString(), DEFAULT_REALLTV_SMILES_PREFIX + smileInfo[ "image" ].toString() );

                }
            }
        }
    }

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
    }

    reply->deleteLater();
}


void QReallTvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    reply->deleteLater();
}

void QReallTvChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QReallTvChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( REALLTV_CHANNEL_SETTING_PATH, DEFAULT_REALLTV_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    badges_ = settings.value( REALLTV_BADGES_SETTING_PATH, false ).toBool();

    enable( settings.value( REALLTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( REALLTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( REALLTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( REALLTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( REALLTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QReallTvChat::changeBadges( bool badges )
{
    badges_ = badges;
}
