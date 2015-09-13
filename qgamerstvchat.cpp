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

#include "settingsconsts.h"

#include "qgamerstvchat.h"

const QString DEFAULT_GAMERSTV_SMILES_DIR = "http://gamerstv.ru/smiles/";
const QString DEFAULT_GAMERSTV_MESSAGES_PREFIX = "http://gamerstv.ru/modules/ajax/chat_cache/room_";
const QString DEFAULT_GAMERSTV_SMILES_INFO_LINK = "http://gamerstv.ru/smiles/smiles.js";

const QString DEFAULT_GAMERSTV_STATISTIC_LINK = "http://gamerstv.ru/modules/ajax/chat_online.php";

const QString GAMERSTV_SERVICE = "gamerstv";
const QString GAMERSTV_USER = "GAMERSTV";

const int DEFAULT_GAMERSTV_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_GAMERSTV_RECONNECT_INTERVAL = 10000;
const int DEFAULT_GAMERSTV_STATISTIC_INTERVAL = 10000;

QGamersTvChat::QGamersTvChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_( "" )
, channelLink_( "" )
, lastMessageId_( "" )
, updateMessagesTimerId_( -1 )
, reconnectTimerId_( -1 )
, updateMessagesInterval_( DEFAULT_GAMERSTV_UPDATE_MESSAGES_INTERVAL )
, reconnectInterval_( DEFAULT_GAMERSTV_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_GAMERSTV_STATISTIC_INTERVAL )
{
    //loadSettings();
    //loadSmiles();
}

QGamersTvChat::~QGamersTvChat()
{
    disconnect();
}

void QGamersTvChat::connect()
{
    if( channelName_ == "" )
        return;

    smiles_.clear();
    loadSmiles();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Connecting to " + channelName_ + "...", "", this ) );

    channelLink_ = DEFAULT_GAMERSTV_MESSAGES_PREFIX + channelName_ + ".js";

    if( updateMessagesTimerId_ == -1 )
        updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Connected to " + channelName_ + "...", "", this ) );

    getStatistic();
    if( statisticTimerId_ == -1 )
        statisticTimerId_ = startTimer( statisticInterval_ );

}

void QGamersTvChat::disconnect()
{
    channelLink_ = "";
    lastMessageId_ = "";
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
    if( statisticTimerId_ != -1 )
    {
        killTimer( statisticTimerId_ );
        statisticTimerId_ = -1;
    }
    emit newStatistic( new QChatStatistic( GAMERSTV_SERVICE, "", this ) );
}

void QGamersTvChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Reconnecting to " + channelName_ + "...", "", this ) );
    connect();
}

void QGamersTvChat::loadSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_GAMERSTV_SMILES_INFO_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onSmilesLoadError() ) );
}

void QGamersTvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            if( jsonDoc.object()[ "img" ].isArray() )
            {
                QJsonArray smiles = jsonDoc.object()[ "img" ].toArray();

                smiles_.clear();

                foreach( const QJsonValue& smile, smiles )
                {
                    smiles_.append( QChatSmile(
                                                smile.toObject()[ "code" ].toString(),
                                                DEFAULT_GAMERSTV_SMILES_DIR + smile.toObject()[ "src" ].toString(),
                                                smile.toObject()[ "width" ].toInt(),
                                                smile.toObject()[ "height" ].toInt() ) );
                }
                if( isShowSystemMessages() )
                    emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Smiles ready...", "", this ) );
            }
        }
    }

    reply->deleteLater();
}

void QGamersTvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Can not load smiles..." + reply->errorString(), "", this ) );

    //TODO: timer for smiles loading
    //loadSmiles();

    reply->deleteLater();
}

void QGamersTvChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_GAMERSTV_STATISTIC_LINK + "" ) );
/*
    QByteArray data;

    QString dataString = "{\"data\":{\"nick\":\"" + channelName_ + "\"}}";

    data.append( dataString );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/json;charset=utf-8" ) );
    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
*/

    QByteArray data;
    QString dataString = "room_id=" + channelName_ + "&room_type=streams";

    //qDebug() << dataString;

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

    //{"users":"1","guests":"1"}

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            //qDebug() << jsonObj[ "users" ].toString() + "+(" + jsonObj[ "guests" ].toString() + ")";

            emit newStatistic( new QChatStatistic( GAMERSTV_SERVICE, jsonObj[ "users" ].toString() + "+(" + jsonObj[ "guests" ].toString() + ")", this  ) );

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
    if( parseError.error == QJsonParseError::NoError )
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

                if( lastMessageId_ != "" )
                {
                    int messageIndex = 0;
                    while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "chat_id" ].toString() <= lastMessageId_ )
                        ++messageIndex;
                    while( messageIndex < jsonMessagesList.size() )
                    {
                        //TODO: parse message
                        const QJsonObject &jsonMessage = jsonMessagesList[ messageIndex ];

                        QString nickName = jsonMessage[ "name" ].toString();

                        bool blackListUser = blackList().contains( nickName );
                        bool supportersListUser = supportersList().contains( nickName );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            QString message = jsonMessage[ "text" ].toString();

                            message.replace ( "<\\/u>", "" );
                            message.replace ( "<u>", "" );
                            message = insertSmiles( message );
                            //message = QChatMessage::insertLinks( message );

                            if( blackListUser )
                            {
                                emit newMessage( new QChatMessage( GAMERSTV_SERVICE, nickName, message, "ignore", this ) );
                            }
                            else
                            {
                                if( supportersListUser )
                                {
                                    emit newMessage( new QChatMessage( GAMERSTV_SERVICE, nickName, message, "supporter", this ) );
                                }
                                else
                                {
                                    if( isContainsAliases( message ) )
                                    {
                                        emit newMessage( new QChatMessage( GAMERSTV_SERVICE, nickName, message, "alias", this ) );
                                    }
                                    else
                                    {
                                        emit newMessage( new QChatMessage( GAMERSTV_SERVICE, nickName, message, "", this ) );
                                    }
                                }
                            }

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
    //emit newMessage( new QChatMessage( GAMERSTV_SERVICE, GAMERSTV_USER, "Can not load messages for channel №" + channelName_ + "..." + reply->errorString(), "", this ) );

    //reconnectTimerId_ =  startTimer( reconnectInterval_ );

    reply->deleteLater();
}

QString QGamersTvChat::insertSmiles( const QString &message )
{
    QString convertedMessage = message;

    foreach( const QChatSmile &smile, smiles_ )
    {
       convertedMessage.replace( smile.name(), "<img class = \"smile\" src=\"" + smile.link() + "\"></img>" );
    }
    return convertedMessage;
}

void QGamersTvChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( GAMERSTV_CHANNEL_SETTING_PATH, DEFAULT_GAMERSTV_CHANNEL_NAME ).toString();
    setAliasesList( settings.value( GAMERSTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GAMERSTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GAMERSTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( GAMERSTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QGamersTvChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
    }
    if( event->timerId() == updateMessagesTimerId_ && channelLink_ != "" )
    {
        loadMessages();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        //qDebug() << "Timer reconnect";
        reconnect();
    }
}
