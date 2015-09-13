#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include <QSettings>

#include <QVariantMap>
#include <QTimerEvent>


#include "settingsconsts.h"

#include "qaceschat.h"

const QString DEFAULT_ACES_MAIN_PAGE_LINK = "http://aces.gg";
const QString DEFAULT_ACES_CHAT_PREFIX = "http://aces.gg/chat/";
const QString DEFAULT_ACES_CHAT_LINK = "http://aces.gg/stream/chat/get";

const QString ACES_USER = "ACES";
const QString ACES_SERVICE = "aces";

int DEFAULT_ACES_UPDATE_CHAT_INFO_INTERVAL = 1000;
int DEFAULT_ACES_RECONNECT_INTERVAL = DEFAULT_ACES_UPDATE_CHAT_INFO_INTERVAL * 10;

QAcesChat::QAcesChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, channelId_()
, lastMessageDateTime_()
, updateChatInfoTimerId_( -1 )
, reconnectTimerId_( -1 )
, updateChatInfoInterval_( DEFAULT_ACES_UPDATE_CHAT_INFO_INTERVAL )
, reconnectInterval_( DEFAULT_ACES_RECONNECT_INTERVAL )
, originalColors_( false )
{
    //loadSettings();
}

QAcesChat::~QAcesChat()
{
    disconnect();
}

void QAcesChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getStreamesList();
}

void QAcesChat::disconnect()
{
    channelId_ = "";
    lastMessageDateTime_ = "";

    if( updateChatInfoTimerId_ >= 0 )
    {
        killTimer( updateChatInfoTimerId_ );
        updateChatInfoTimerId_ = -1;
    }
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }
}

void QAcesChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Reconnecting...", "", this ) );
    connect();
}

void QAcesChat::getStreamesList()
{
    QNetworkRequest request( QUrl( DEFAULT_ACES_MAIN_PAGE_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStreamesListLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStreamesListLoadError() ) );
}

void QAcesChat::onStreamesListLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString streamesInfo = reply->readAll();

    int startStreamesPos = streamesInfo.indexOf( "aces.stream.setStreamData" );
    startStreamesPos = streamesInfo.indexOf( "(", startStreamesPos );
    int endStreamesPos = streamesInfo.indexOf( ");", startStreamesPos );

    streamesInfo = streamesInfo.mid( startStreamesPos + 1, endStreamesPos - startStreamesPos -1 );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( streamesInfo.toStdString().c_str(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonStreamesInfo = jsonDoc.object();

            channelId_ = "";

            for( QJsonObject::Iterator iter = jsonStreamesInfo.begin(); channelId_ == "" && iter != jsonStreamesInfo.end(); ++iter )
            {
                //qDebug() << iter.value().toObject()[ "channel" ].toString();

                if( iter.value().toObject()[ "channel" ].toString() == channelName_ )
                {
                    channelId_ = iter.value().toObject()[ "id" ].toString();

                    if( updateChatInfoTimerId_ == -1 )
                        updateChatInfoTimerId_ = startTimer( updateChatInfoInterval_ );
                    if( isShowSystemMessages() )
                        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Connected to " + channelName_ + "...", "", this ) );
                }
            }
            if( channelId_ == "" )
            {
                if( isShowSystemMessages() )
                    emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not find " + channelName_ + " channel...", "", this ) );
                //reconnect();
                if( reconnectTimerId_ == -1 )
                    reconnectTimerId_ = startTimer( reconnectInterval_ );
            }
        }
    }

    reply->deleteLater();
}

void QAcesChat::onStreamesListLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not load streames list..." + reply->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
    reply->deleteLater();
}

void QAcesChat::onChatInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError ) ;

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonChatInfo = jsonDoc.object();

            QJsonObject jsonChat = jsonChatInfo[ "chat" ].toObject();


            if( lastMessageDateTime_ == "" )
            {
                //TODO: говнокод
                for( QJsonObject::Iterator iter = jsonChat.begin(); iter != jsonChat.end(); ++iter )
                {
                    lastMessageDateTime_ = iter.value().toObject()[ "dt_message" ].toString();
                }
            }
            else
            {
                QString messageDateTime;
                QString maxMessageDateTime = lastMessageDateTime_;

                for( QJsonObject::Iterator iter = jsonChat.begin(); iter != jsonChat.end(); ++iter )
                {
                    messageDateTime = iter.value().toObject()[ "dt_message" ].toString();
                    if( messageDateTime > lastMessageDateTime_ )
                    {
                        QJsonObject jsonUser = iter.value().toObject()[ "user" ].toObject();

                        QString nickName = jsonUser[ "name" ].toString();

                        bool blackListUser = blackList().contains( nickName );
                        bool supportersListUser = supportersList().contains( nickName );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            //модератор
                            if( jsonUser[ "is_moderator" ].toBool() )
                            {
                                nickName = "♠" + nickName;
                            }

                            if( originalColors_ )
                            {
                                QJsonObject jsonChatSettings = iter.value().toObject()[ "user" ].toObject()[ "chat_settings" ].toObject();

                                QString chatColor = jsonChatSettings[ "chat_user_color" ].toString();

                                nickName = "<span style=\"color: " + chatColor + "\">" + nickName + "</span>";
                            }

                            QString message =  iter.value().toObject()[ "message" ].toString();

                            int smileLinkStart = message.indexOf( "background-image:url(" );

                            while( smileLinkStart >= 0 )
                            {
                                smileLinkStart = message.indexOf( "\"", smileLinkStart ) + 1;

                                int smileLinkEnd = message.indexOf( "\"", smileLinkStart ) - 1;

                                if( smileLinkEnd - smileLinkStart > 0 )
                                {

                                    QString smileLink = message.mid( smileLinkStart, smileLinkEnd - smileLinkStart +1 );

                                    int spanEnd = message.indexOf( ">", smileLinkEnd );

                                    message.insert( spanEnd + 1, "\a\a\timg class = \"smile\" src=\"" + smileLink + "\"\a\a\r\a\a\t/img\a\a\r" );

                                    smileLinkStart = message.indexOf( "background-image:url(", smileLinkEnd );
                                }
                                else
                                {
                                    smileLinkStart = -1;
                                }
                            }
                            //подсветка ников

                            if( originalColors_ )
                            {
                                int nickNameIdStart = message.indexOf( "\"user_in_text\"" );

                                while( nickNameIdStart >= 0 )
                                {
                                    nickNameIdStart = message.indexOf( '=', nickNameIdStart ) + 2;
                                    int nickNameIdEnd = message.indexOf( "\"", nickNameIdStart ) - 1;

                                    QString nickNameId = message.mid( nickNameIdStart, nickNameIdEnd - nickNameIdStart + 1 );

                                    QJsonObject jsonChatUserInfos = jsonChatInfo[ "chat_user_infos" ].toObject();

                                    int nickNameStart = message.indexOf( ">", nickNameIdEnd ) + 1;
                                    int nickNameEnd = message.indexOf( "<", nickNameStart ) - 1;

                                    QString nickNameColor = jsonChatUserInfos[ nickNameId ].toObject()[ "chat_user_color" ].toString();

                                    message.insert( nickNameEnd + 1, "\a\a\t/span\a\a\r" );
                                    message.insert( nickNameStart, "\a\a\tspan style=\"color: " + nickNameColor + "\"\a\a\r" );

                                    nickNameIdStart = message.indexOf( "\"user_in_text\"", nickNameEnd );
                                }
                            }
                            message = QChatMessage::deleteTags( message );

                            message.replace( "\a\a\t", "<" );
                            message.replace( "\a\a\r", ">" );

                            //message = QChatMessage::insertLinks( message );

                            if( blackListUser )
                            {
                                //TODO: выделение игнорируемых
                                emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "ignore", this ) );
                            }
                            else
                            {
                                if( supportersListUser )
                                {
                                    //TODO: выделение саппортов
                                    emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "supporter", this ) );
                                }
                                else
                                {
                                    if( isContainsAliases( message ) )
                                    {
                                        //TODO: выделение обращения к стримеру
                                        emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "alias", this ) );
                                    }
                                    else
                                    {
                                        //TODO: простое сообщение
                                        emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "", this ) );
                                    }
                                }
                            }
                        }
                        if( messageDateTime > maxMessageDateTime )
                            maxMessageDateTime = messageDateTime;
                    }
                }
                lastMessageDateTime_ = maxMessageDateTime;
            }
        }
    }
    reply->deleteLater();
}

void QAcesChat::onChatInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not read channel messages..." + reply->errorString(), "", this ) );

    reply->deleteLater();
}

void QAcesChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == updateChatInfoTimerId_ )
    {
        QByteArray data;

        data.append( "id=" + channelId_ );
        //data.append( "form=0" );

        QNetworkRequest request( QUrl( DEFAULT_ACES_CHAT_LINK + "" ) );
        request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/x-www-form-urlencoded; charset=UTF-8" ) );
        request.setRawHeader( "Host", "aces.gg" );
        request.setRawHeader( "Connection", "keep-alive" );
        request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

        request.setRawHeader( "Accept", "application/json, text/javascript, */*; q=0.01" );
        request.setRawHeader( "Origin", "http://aces.gg" );
        request.setRawHeader( "X-Requested-With", "XMLHttpRequest" );
        request.setRawHeader( "Referer", QString( DEFAULT_ACES_CHAT_PREFIX + channelId_ ).toStdString().c_str() );
        request.setRawHeader( "Accept-Encoding", "gzip, deflate" );
        request.setRawHeader( "Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4" );

        QNetworkReply *reply = nam_->post( request, data );

        QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChatInfoLoaded() ) );
        QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChatInfoLoadError() ) );
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QAcesChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString();
    originalColors_ = settings.value( ACES_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QAcesChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}
