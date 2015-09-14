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

#include "qsc2tvchat.h"

const QString DEFAULT_SC2TV_STREAMS_LINK = "http://sc2tv.ru/streams_list.json";
const QString DEFAULT_SC2TV_SMILES_LINK = "http://chat.sc2tv.ru/js/smiles.js";
const QString DEFAULT_SC2TV_SMILES_DIR_LINK = "http://chat.sc2tv.ru/img/";

//TODO: preload styles, as smiles
const QString DEFAULT_SC2TV_CHAT_CSS_LINK = "http://chat.sc2tv.ru/css/chat.css";

const QString DEFAULT_SC2TV_CHANNEL_PREFIX = "http://sc2tv.ru/channel/";
const QString DEFAULT_SC2TV_CHANNEL_PREFIX_LINK = "http://chat.sc2tv.ru/memfs/channel-";

const QString SC2TV_USER = "SC2TV";
const QString SC2TV_SERVICE = "sc2tv";

const int DEFAULT_SC2TV_UPDATE_MESSAGES_INTERVAL = 3000;
const int DEFAULT_SC2TV_RECONNECT_INTERVAL = DEFAULT_SC2TV_UPDATE_MESSAGES_INTERVAL * 10;

QSc2tvChat::QSc2tvChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, channelLink_()
//, lastMessageTime_()
//, lastMainMessageId_()
, lastMessageId_()
, smiles_()
, styles_()
, updateMessagesTimerId_( -1 )
, reconnectTimerId_( -1 )
, updateMessagesInterval_( DEFAULT_SC2TV_UPDATE_MESSAGES_INTERVAL )
, reconnectInterval_( DEFAULT_SC2TV_RECONNECT_INTERVAL )
, originalColors_( false )
{
    //loadSettings();
    //getLastMessageId();
    //getSmiles();
    //getStyles();
}

QSc2tvChat::~QSc2tvChat()
{
    disconnect();
}

void QSc2tvChat::connect()
{   
    if( channelName_ == "" )
        return;

    smiles_.clear();
    styles_.clear();
    //getLastMessageId();
    getSmiles();
    getStyles();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getChannelInfo();
}

void QSc2tvChat::disconnect()
{
    channelLink_ = "";
    //lastMessageTime_ = "";

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
}

void QSc2tvChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Reconnecting...", "", this ) );
    connect();
}

void QSc2tvChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_SMILES_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QSc2tvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( reply->error() == QNetworkReply::NoError )
    {
        QByteArray jsonFile = reply->readAll();

        int startPos = jsonFile.indexOf( '[' );
        jsonFile = jsonFile.mid( startPos, jsonFile.indexOf( ';' ) - startPos );

        QJsonParseError parseError;

        QJsonDocument jsonDoc = QJsonDocument::fromJson( jsonFile, &parseError  );

        if( parseError.error == QJsonParseError::NoError )
        {
            if( jsonDoc.isArray() )
            {
                smiles_.clear();
                QJsonArray smilesArray = jsonDoc.array();

                foreach( const QJsonValue &value, smilesArray )
                {
                    QJsonObject jsonSmile = value.toObject();

                    QString img = jsonSmile[ "img" ].toString();
                    img = DEFAULT_SC2TV_SMILES_DIR_LINK + img.left( img.indexOf( '.' ) + 4 );

                    //qDebug() << img;

                    QChatSmile smile(   ":s" + jsonSmile[ "code" ].toString(),
                                        img,
                                        jsonSmile[ "width" ].toInt(),
                                        jsonSmile[ "height" ].toInt()
                                    );
                    smiles_.append( smile );

                    QChatSmile freeSmile(   jsonSmile[ "code" ].toString().insert( 1, "free-" ),
                                            img,
                                            jsonSmile[ "width" ].toInt(),
                                            jsonSmile[ "height" ].toInt()
                                    );
                    smiles_.append( freeSmile );

                }
            }
        }
    }

    //own smiles code
    QString smilesPath = QApplication::applicationDirPath() + "/smiles";

    QStringList extList;
    extList << "*.svg" << "*.png" << "*.gif" << "*.jpg";

    QDir smilesDir( smilesPath );

    QStringList smileFiles = smilesDir.entryList( extList, QDir::Files | QDir::NoSymLinks );

    foreach( const QString& smileName, smileFiles )
    {
        QChatSmile smile(   ":" + smileName.left( smileName.length() - 4 ) + ":",
                                "file:///" + smilesPath + "/" + smileName, 0, 0 );
        smiles_.append( smile );
    }

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Smiles ready...", "", this ) );

    reply->deleteLater();
}

void QSc2tvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Can not load smiles..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );
    //TODO: timer for smiles loading
    //getSmiles();// timer may be
    reply->deleteLater();
}

void QSc2tvChat::getStyles()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_CHAT_CSS_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStyleLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStylesLoadError() ) );
}


void QSc2tvChat::onStyleLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    //TODO: parse styles

    QString chatStyles = reply->readAll();

    styles_.clear();

    int startRolePos = chatStyles.indexOf( "role-" );

    while( startRolePos >= 0 )
    {
        startRolePos = chatStyles.indexOf( '-', startRolePos ) + 1;
        int endRolePos = chatStyles.indexOf( '{', startRolePos ) - 1;

        QString role = chatStyles.mid( startRolePos, endRolePos - startRolePos + 1 );
        role.replace( ' ', "" );//moderator-role fix

        //qDebug() << role;

        int startStylePos = chatStyles.indexOf( '{', endRolePos ) + 1;
        int endStylePos = chatStyles.indexOf( '}', startStylePos ) - 1;

        QString style = chatStyles.mid( startStylePos, endStylePos - startStylePos + 1 );

        style.replace( '\t', "" );
        style.replace( '\r', "" );
        style.replace( '\n', "" );
        style.replace( " !important;", "" );

        //qDebug() << style;

        styles_.insert( role, style );

        startRolePos = chatStyles.indexOf( "role-", endStylePos );

    }

    reply->deleteLater();
}

void QSc2tvChat::onStylesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Can not load styles..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );
    //TODO: timer for loading smiles
    //getStyles();// timer may be

    reply->deleteLater();
}

/*
void QSc2tvChat::getLastMessageId()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_CHANNEL_PREFIX_LINK + "0.json" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onLastMessageIdLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError )  ), this, SLOT( onLastMessageIdLoadError() ) );
}
*/


bool cmpJsonObject( const QJsonObject &lho, const QJsonObject& rho )
{
    return lho[ "id" ].toString() < rho[ "id" ].toString();
}

/*
void QSc2tvChat::onLastMessageIdLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonArray jsonMessages = jsonDoc.object()[ "messages" ].toArray();
            lastMainMessageId_ = jsonMessages.first().toObject()[ "id" ].toString();



            QList< QJsonObject > jsonMessagesList;

            foreach( const QJsonValue &value, jsonMessages )
            {
                jsonMessagesList.append( value.toObject() );
                qDebug() << value.toObject()[ "id" ].toString();
            }

            qSort( jsonMessagesList.begin(), jsonMessagesList.end(), cmpJsonObject );

            if( jsonMessagesList.size() > 0 )
            {
                lastMessageId_ = jsonMessagesList.last()[ "id" ].toString();
                qDebug() << "lastId = " << lastMessageId_;
            }

        }
    }

    reply->deleteLater();
}

void QSc2tvChat::onLastMessageIdLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    qDebug() << reply->errorString();

    if( lastMessageId_ == "" )
        getLastMessageId();

    reply->deleteLater();
}
*/

void QSc2tvChat::getChannelInfo()
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

    //qDebug() << channelInfo;

    int channelIdStart = channelInfo.indexOf( "current_channel_id" );
    if( channelIdStart > 0 )
    {
        channelIdStart = channelInfo.indexOf( "=", channelIdStart );
        int channelIdEnd = channelInfo.indexOf( ";", channelIdStart );

        channelInfo = channelInfo.mid( channelIdStart + 1, channelIdEnd - channelIdStart - 1 );

        channelInfo.replace( ' ', "" );

        if( channelInfo != "0" )
        {            
            channelLink_ = DEFAULT_SC2TV_CHANNEL_PREFIX_LINK + channelInfo + ".json";

            if( isShowSystemMessages() )
                emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Connected to " + channelName_ + "...", "", this ) );

            if( updateMessagesTimerId_ == -1 )
                updateMessagesTimerId_ = startTimer( updateMessagesInterval_ );
        }
        else
        {
            channelLink_ = "";
        }
    }
    else
    {
        channelLink_ = "";
    }

    if( channelLink_ == "" )
    {
        if( reconnectTimerId_ == -1 )
            reconnectTimerId_ = startTimer( reconnectInterval_ );
        //reconnect();
    }

    reply->deleteLater();
}

void QSc2tvChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Can not connect to " + channelName_ + "..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

    reply->deleteLater();
}

void QSc2tvChat::getMessages()
{
    QNetworkRequest request( QUrl( channelLink_ + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onMessagesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), SLOT( onMessagesLoadError() ) );
}

void QSc2tvChat::onMessagesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    //qDebug() << reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( parseError.error == QJsonParseError::NoError )
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

            //if( lastMessageTime_ != "" )
            if( lastMessageId_ != "" )
            {
                int messageIndex = 0;
                //while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "date" ].toString() <= lastMessageTime_ )
                while( messageIndex < jsonMessagesList.size() && jsonMessagesList[ messageIndex ][ "id" ].toString() <= lastMessageId_ )
                    ++messageIndex;
                while( messageIndex < jsonMessagesList.size() )
                {
                    QJsonObject jsonMessage = jsonMessagesList[ messageIndex ];

                    QString message = jsonMessage[ "message" ].toString();

                    //TODO: править

                    message = message.replace( "[b]", "" );
                    message = message.replace( "[/b]", "" );
                    message = message.replace( "[url]", "" );
                    message = message.replace( "[/url]", "" );


                    message = insertSmiles( message );
                    //message = QChatMessage::insertLinks( message );

                    QString nickName = jsonMessage[ "name" ].toString();

                    bool blackListUser = blackList().contains( nickName );
                    bool supportersListUser = supportersList().contains( nickName );

                    if( originalColors_ )
                    {
                        QString role = jsonMessage[ "role" ].toString();
                        if( styles_.contains( role ) )
                        {                            
                            nickName = "<span style=\"" + styles_[ role ] + "\">" + nickName + "</span>";
                        }
                    }

                    if( !isRemoveBlackListUsers() || !blackListUser )
                    {
                        if( blackListUser )
                        {
                            //TODO: игнорируемые
                            emit newMessage( new QChatMessage( SC2TV_SERVICE, nickName, message, "ignore", this ) );
                        }
                        else
                        {
                            if( supportersListUser )
                            {
                                //TODO: саппортеры
                                emit newMessage( new QChatMessage( SC2TV_SERVICE, nickName, message, "supporter", this ) );
                            }
                            else
                            {
                                if( isContainsAliases( message ) )
                                {
                                    //TODO: сообщение к стримеру
                                    emit newMessage( new QChatMessage( SC2TV_SERVICE, nickName, message, "alias", this ) );
                                }
                                else
                                {
                                    //TODO: простое сообщение
                                    emit newMessage( new QChatMessage( SC2TV_SERVICE, nickName, message, "", this ) );
                                }
                            }
                        }
                    }
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
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    //emit newMessage( new QChatMessage( SC2TV_SERVICE, SC2TV_USER, "Can not load messages..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), this ) );
    //reconnect(); channelId есть

    //lastMessageId_ = lastMainMessageId_;

    reply->deleteLater();
}

QString QSc2tvChat::insertSmiles( const QString &message ) const
{
    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( !QChatMessage::isLink( tokens.at( i ) ) )//не ссылка
        {
            foreach( const QChatSmile &smile, smiles_ )
            {
               convertedTokens[ i ].replace( smile.name(), "<img class = \"smile\" src=\"" + smile.link() + "\"></img>" );
            }
        }
    }

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    return convertedMessage;
}


void QSc2tvChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == updateMessagesTimerId_ && channelLink_ != "" )
    {
        getMessages();
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
    originalColors_ = settings.value( SC2TV_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( SC2TV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( SC2TV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( SC2TV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( SC2TV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QSc2tvChat::setUpdateMessagesInterval( int interval )
{
    updateMessagesInterval_ = interval;
}

void QSc2tvChat::setReconnectInterval( int interval )
{
    reconnectInterval_ = interval;
}

void QSc2tvChat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}

/*
void QSc2tvChat::getStreamesList()
{
    QNetworkRequest request( QUrl( DEFAULT_SC2TV_STREAMS_LINK + ""  ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStreamesLoaded() ) );
}
*/
/*
void QSc2tvChat::onStreamesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( reply->error() == QNetworkReply::NoError )
    {
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
        if( parseError.error == QJsonParseError::NoError )
        {
            if( jsonDoc.isObject() )
            {
                QJsonArray streams = jsonDoc.object()[ "streams" ].toArray();

                foreach( const QJsonValue& value, streams )
                {
                    QJsonObject stream = value.toObject();

                    QString streamName = stream[ "path" ].toString();

                    streamName = streamName.right( streamName.size() - streamName.indexOf( '/' ) - 1 );

                    if( streamName == channelName_ )
                    {
                        channelLink_ = DEFAULT_SC2TV_CHANNEL_PREFIX_LINK + QString::number( stream[ "id" ].toInt() ) + ".json";
                        //getMessages();
                        emit newMessage( new QChatMessage( "sc2tv", SC2TV_USER, "Connecting to " + channelName_ + "...", this ) );
                        startTimer( updateInterval_ );
//                        break;
                    }
                }
                //if( channelLink_ == "" )
                //    emit newMessage( new QChatMessage( "sc2tv", SC2TV_USER, "Can not connect to " + channelName_ + "... Channel is offline...", this ) );
            }
        }
    }

    reply->deleteLater();
}
*/
