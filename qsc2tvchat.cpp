#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include <QTimerEvent>

#include <QTimeZone>

#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include "settingsconsts.h"

#include "qchatmessage.h"

#include "qsc2tvchat.h"

const QString DEFAULT_SC2TV_STREAMS_LINK = "http://sc2tv.ru/streams_list.json";
const QString DEFAULT_SC2TV_SMILES_LINK = "http://chat.sc2tv.ru/js/smiles.js";
const QString DEFAULT_SC2TV_SMILES_DIR_LINK = "http://chat.sc2tv.ru/img/";

const QString DEFAULT_SC2TV_CHAT_CSS_LINK = "http://chat.sc2tv.ru/css/chat.css";

const QString DEFAULT_SC2TV_CHANNEL_PREFIX = "http://sc2tv.ru/channel/";
const QString DEFAULT_SC2TV_CHANNEL_PREFIX_LINK = "http://chat.sc2tv.ru/memfs/channel-";

const QString SERVICE_USER_NAME = "SC2TV";
const QString SERVICE_NAME = "sc2tv";

const int DEFAULT_SC2TV_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_SC2TV_RECONNECT_INTERVAL = DEFAULT_SC2TV_UPDATE_MESSAGES_INTERVAL * 10;

const QString QSc2tvChat::SERVICE_NAME = "sc2tv";
const QString QSc2tvChat::SERVICE_USER_NAME = "SC2TV";

const int QSc2tvChat::UPDATE_INTERVAL = 3000;
const int QSc2tvChat::RECONNECT_INTERVAL = 3000;

QSc2tvChat::QSc2tvChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QSc2tvChat::~QSc2tvChat()
{
    disconnect();
}

void QSc2tvChat::connect()
{   
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    styles_.clear();

    loadStyles();

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
}

void QSc2tvChat::disconnect()
{
    channelLink_.clear();

    resetTimer( updateMessagesTimerId_ );
    resetTimer( reconnectTimerId_ );
}

void QSc2tvChat::reconnect()
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

void QSc2tvChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_SC2TV_SMILES_LINK + "" ) );
    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QSc2tvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( QNetworkReply::NoError == reply->error()  )
    {
        QByteArray jsonFile = reply->readAll();

        int startPos = jsonFile.indexOf( '[' );
        jsonFile = jsonFile.mid( startPos, jsonFile.indexOf( ';' ) - startPos );

        QJsonParseError parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson( jsonFile, &parseError  );

        if( QJsonParseError::NoError == parseError.error  )
        {
            if( jsonDoc.isArray() )
            {
                QJsonArray smilesArray = jsonDoc.array();

                foreach( const QJsonValue &value, smilesArray )
                {
                    QJsonObject jsonSmile = value.toObject();

                    QString img = jsonSmile[ "img" ].toString();
                    img = DEFAULT_SC2TV_SMILES_DIR_LINK + img.left( img.indexOf( '.' ) + 4 );

                    addSmile( ":s" + jsonSmile[ "code" ].toString(), img );
                    addSmile( jsonSmile[ "code" ].toString().insert( 1, "free-" ), img );

                }
            }
        }
    }

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Smiles loaded...", "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Smiles loaded..." );
    }

    reply->deleteLater();
}

void QSc2tvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }
    reply->deleteLater();
}

void QSc2tvChat::loadStyles()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_CHAT_CSS_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStyleLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStylesLoadError() ) );
}


void QSc2tvChat::onStyleLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString chatStyles = reply->readAll();

    styles_.clear();

    int startRolePos = chatStyles.indexOf( "role-" );

    while( startRolePos >= 0 )
    {
        startRolePos = chatStyles.indexOf( '-', startRolePos ) + 1;
        int endRolePos = chatStyles.indexOf( '{', startRolePos ) - 1;

        QString role = chatStyles.mid( startRolePos, endRolePos - startRolePos + 1 );
        role.replace( ' ', "" );//moderator-role fix

        int startStylePos = chatStyles.indexOf( '{', endRolePos ) + 1;
        int endStylePos = chatStyles.indexOf( '}', startStylePos ) - 1;

        QString style = chatStyles.mid( startStylePos, endStylePos - startStylePos + 1 );

        style.replace( '\t', "" );
        style.replace( '\r', "" );
        style.replace( '\n', "" );
        style.replace( " !important;", "" );

        styles_.insert( role, style );

        startRolePos = chatStyles.indexOf( "role-", endStylePos );

    }

    reply->deleteLater();
}

void QSc2tvChat::onStylesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    reply->deleteLater();
}

bool cmpJsonObject( const QJsonObject &lho, const QJsonObject& rho )
{
    return lho[ "id" ].toString() < rho[ "id" ].toString();
}

void QSc2tvChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_CHANNEL_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QSc2tvChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    QString channelInfo = reply->readAll();

    int channelIdStart = channelInfo.indexOf( "current_channel_id" );
    if( channelIdStart > 0 )
    {
        channelIdStart = channelInfo.indexOf( "=", channelIdStart );
        int channelIdEnd = channelInfo.indexOf( ";", channelIdStart );

        channelInfo = channelInfo.mid( channelIdStart + 1, channelIdEnd - channelIdStart - 1 );

        channelInfo.replace( ' ', "" );

        if( "0" != channelInfo )
        {            
            channelLink_ = DEFAULT_SC2TV_CHANNEL_PREFIX_LINK + channelInfo + ".json";

            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
            }

            loadSmiles();
            loadStyles();

            startUniqueTimer( updateMessagesTimerId_, UPDATE_INTERVAL );
        }
        else
        {
            channelLink_.clear();
        }
    }
    else
    {
        channelLink_.clear();
    }

    if( channelLink_.isEmpty() )
    {        
        startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
    }

    reply->deleteLater();
}

void QSc2tvChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() + "..." + QDateTime::currentDateTime().toString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QSc2tvChat::loadMessages()
{
    QNetworkRequest request( QUrl( channelLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), SLOT( onMessagesLoadError() ) );
}

void QSc2tvChat::onMessagesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonArray jsonMessages = jsonDoc.object()[ "messages" ].toArray();

            QList< QJsonObject > jsonMessagesList;

            foreach( const QJsonValue &value, jsonMessages )
            {
                jsonMessagesList.append( value.toObject() );
            }

            qSort( jsonMessagesList.begin(), jsonMessagesList.end(), cmpJsonObject );

            if( !lastMessageId_.isEmpty() )
            {
                int messageIndex = 0;

                while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "id" ].toString() <= lastMessageId_ )
                    ++messageIndex;
                while( messageIndex < jsonMessagesList.size() )
                {
                    QJsonObject jsonMessage = jsonMessagesList[ messageIndex ];

                    QString message = jsonMessage[ "message" ].toString();

                    message.replace( "[b]", "" );
                    message.replace( "[/b]", "" );
                    message.replace( "[url]", "" );
                    message.replace( "[/url]", "" );

                    message.replace( QRegExp( "\\[url=(.*)\\]" ), "" );

                    message = insertSmiles( message );

                    QString nickName = jsonMessage[ "name" ].toString();

                    if( originalColors_ )
                    {
                        QString role = jsonMessage[ "role" ].toString();
                        if( styles_.contains( role ) )
                        {                            
                            nickName = "<span style=\"" + styles_[ role ] + "\">" + nickName + "</span>";
                        }
                        qDebug() << nickName;
                    }

                    emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );

                    ++messageIndex;
                }
            }
            else
            {
            }
            if( jsonMessagesList.size() > 0 )
            {                
                lastMessageId_ = jsonMessagesList.last()[ "id" ].toString();
            }
        }
    }

    reply->deleteLater();
}

void QSc2tvChat::onMessagesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load messages..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load messages..." ) + reply->errorString() + tr( "..." ) + QDateTime::currentDateTime().toString() );
    }

    reply->deleteLater();
}

void QSc2tvChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == updateMessagesTimerId_ && !channelLink_.isEmpty() )
    {
        loadMessages();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QSc2tvChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( SC2TV_CHANNEL_SETTING_PATH, DEFAULT_SC2TV_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( SC2TV_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    originalColors_ = settings.value( SC2TV_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( SC2TV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( SC2TV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( SC2TV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

/*
void QSc2tvChat::setUpdateMessagesInterval( int interval )
{
    updateMessagesInterval_ = interval;
}

void QSc2tvChat::setReconnectInterval( int interval )
{
    reconnectInterval_ = interval;
}
*/

void QSc2tvChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}
