#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QSettings>

#include <QTimerEvent>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "..\core\qchatmessage.h"

#include "..\core\settingsconsts.h"

#include "qgamerstvchat.h"

const QString DEFAULT_GAMERSTV_SMILES_DIR = "http://gamerstv.ru/smiles/";
const QString DEFAULT_GAMERSTV_MESSAGES_PREFIX = "http://gamerstv.ru/modules/ajax/chat_cache/room_";
const QString DEFAULT_GAMERSTV_SMILES_INFO_LINK = "http://gamerstv.ru/smiles/smiles.js";
const QString DEFAULT_GAMERSTV_STATISTIC_LINK = "http://gamerstv.ru/modules/ajax/chat_online.php";

const QString SERVICE_NAME = "gamerstv";
const QString SERVICE_USER_NAME = "GAMERSTV";

const int DEFAULT_GAMERSTV_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_GAMERSTV_RECONNECT_INTERVAL = 10000;
const int DEFAULT_GAMERSTV_STATISTIC_INTERVAL = 10000;

const QString QGamersTvChat::SERVICE_NAME = "gamerstv";
const QString QGamersTvChat::SERVICE_USER_NAME = "GAMERSTV";

const int QGamersTvChat::UPDATE_INTERVAL = 3000;
const int QGamersTvChat::RECONNECT_INTERVAL = 10000;
const int QGamersTvChat::STATISTIC_INTERVAL = 10000;

QGamersTvChat::QGamersTvChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QGamersTvChat::~QGamersTvChat()
{
    disconnect();
}

void QGamersTvChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty()  )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    channelLink_ = DEFAULT_GAMERSTV_MESSAGES_PREFIX + channelName_ + ".js";    

    startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
    }

    loadSmiles();
    loadStatistic();    

    startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );

}

void QGamersTvChat::disconnect()
{
    channelLink_.clear();
    lastMessageId_.clear();

    resetTimer( updateMessagesTimerId_  );
    resetTimer( reconnectTimerId_  );
    resetTimer( statisticTimerId_  );

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QGamersTvChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_ .isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting to " ) + channelName_ + tr( "..." ) );
    }
    connect();
}

void QGamersTvChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_GAMERSTV_SMILES_INFO_LINK + "" ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSmilesLoadError() ) );
}

void QGamersTvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            if( jsonDoc.object()[ "img" ].isArray() )
            {
                QJsonArray smiles = jsonDoc.object()[ "img" ].toArray();

                foreach( const QJsonValue& smile, smiles )
                {
                    addSmile( smile.toObject()[ "code" ].toString(), DEFAULT_GAMERSTV_SMILES_DIR + smile.toObject()[ "src" ].toString() );
                }
                if( isShowSystemMessages() )
                {
                    emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
                    emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
                }
            }
        }
    }

    reply->deleteLater();
}

void QGamersTvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() );
    }

    reply->deleteLater();
}

void QGamersTvChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_GAMERSTV_STATISTIC_LINK + "" ) );

    QByteArray data;
    QString dataString = "room_id=" + channelName_ + "&room_type=streams";    

    data.append( dataString );

    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/x-www-form-urlencoded; charset=UTF-8" ) );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QGamersTvChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            emit newStatistic( new QChatStatistic( SERVICE_NAME, QString::number( jsonObj[ "users" ].toInt() ) + "+(" + QString::number( jsonObj[ "guests" ].toInt() ) + ")", this  ) );
        }
    }

    reply->deleteLater();
}


void QGamersTvChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QGamersTvChat::loadMessages()
{
    QNetworkRequest request( QUrl( channelLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onMessagesLoadError() ) );
}

bool cmpJsonObjectByChatId( const QJsonObject &lho, const QJsonObject& rho )
{
    return lho[ "chat_id" ].toString() < rho[ "chat_id" ].toString();
}

void QGamersTvChat::onMessagesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonMessagesInfo = jsonDoc.object();

            if( jsonMessagesInfo[ "text" ].isArray() )
            {
                QJsonArray jsonMessagesArray = jsonMessagesInfo[ "text" ].toArray();

                QList< QJsonObject > jsonMessagesList;

                foreach( const QJsonValue &value, jsonMessagesArray )
                {
                    jsonMessagesList.append( value.toObject() );
                }

                qSort( jsonMessagesList.begin(), jsonMessagesList.end(), cmpJsonObjectByChatId );

                if( !lastMessageId_.isEmpty() )
                {
                    int messageIndex = 0;
                    while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "chat_id" ].toString() <= lastMessageId_ )
                        ++messageIndex;
                    while( messageIndex < jsonMessagesList.size() )
                    {
                        //TODO: parse message
                        const QJsonObject &jsonMessage = jsonMessagesList[ messageIndex ];

                        QString nickName = jsonMessage[ "name" ].toString();


                        //test badges
                        if( badges_ && jsonMessage[ "img" ].isString() )
                        {
                            nickName = "<img class =\"badge\" src=\"http://gamerstv.ru/gallery/users/" + jsonMessage[ "img" ].toString() + "_s.jpg\"></img>" + nickName;
                        }

                        //TODO: to do )))

                        bool blackListUser = blackList().contains( nickName );
                        //bool supportersListUser = supportersList().contains( nickName );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            QString message = jsonMessage[ "text" ].toString();

                            message.replace ( "<\\/u>", "" );
                            message.replace ( "<u>", "" );
                            message = insertSmiles( message );
                            //message = ChatMessage::insertLinks( message );

                            emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, "", this ) );

                        }
                        ++messageIndex;
                    }
                }
                if( jsonMessagesList.size() > 0 )
                    lastMessageId_ = jsonMessagesList.last()[ "chat_id" ].toString();
            }
        }
    }

    reply->deleteLater();
}

void QGamersTvChat::onMessagesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load messages for channel " ) + channelName_ + tr( "..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load messages for channel " ) + channelName_ + tr( "..." ) + reply->errorString() );
    }

    reply->deleteLater();
}

void QGamersTvChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( GAMERSTV_CHANNEL_SETTING_PATH, DEFAULT_GAMERSTV_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
    {
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

        int startChannelNamePos = channelName_.indexOf( "i" ) + 1;
        int endChannelNamePos = channelName_.indexOf( ".html" ) - 1;

        channelName_ = channelName_.mid( startChannelNamePos, endChannelNamePos - startChannelNamePos + 1 );
    }

    badges_ = settings.value( GAMERSTV_BADGES_SETTING_PATH, false ).toBool();

    enable( settings.value( GAMERSTV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( GAMERSTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GAMERSTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QGamersTvChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    if( event->timerId() == updateMessagesTimerId_ && !channelLink_.isEmpty() )
    {
        loadMessages();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QGamersTvChat::changeBadges( bool badges )
{
    badges_ = badges;
}
