#include <QWebSocket>

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
#include <QFileInfo>

#include "settingsconsts.h"

#include "qgoodgamechat.h"

const QString DEFAULT_GOODGAME_WEBSOCKET_LINK = "ws://chat.goodgame.ru:8081/chat/websocket";

const QString DEFAULT_GOODGAME_COMMON_SMILES_INFO_LINK = "http://goodgame.ru/css/compiled/common_smiles.css";
const QString DEFAULT_GOODGAME_CHANNELS_SMILES_INFO_LINK = "http://goodgame.ru/css/compiled/channels_smiles.css";
const QString DEFAULT_GOODGAME_ANIMATED_SMILES_PATH = "http://goodgame.ru/images/anismiles/";

const QString DEFAULT_GOODGAME_CHANNEL_STATUS_PREFIX = "http://goodgame.ru/api/getchannelstatus?id=";
const QString DEFAULT_GOODGAME_CHANNEL_STATUS_POSTFIX = "&fmt=json";

const QString DEFAULT_GOOD_GAME_STATISTIC_PREFIX = "http://goodgame.ru/api/getggchannelstatus?id=";
const QString DEFAULT_GOOD_GAME_STATISTIC_POSTFIX = "&fmt=json";

const int DEFAULT_GOODGAME_SAVE_CONNECTION_INTERVAL = 25000;
const int DEFAULT_GOODGAME_RECONNECT_INTERVAL = 10000;
const int DEFAULT_GOODGAME_STATISTIC_INTERVAL = 10000;

const QString GOODGAME_USER = "GOODGAME";
const QString GOODGAME_SERVICE = "goodgame";

QGoodGameChat::QGoodGameChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( 0 )
, channelName_()
, lastTimeStamp_( 0 )
, smiles_()
, animatedSmiles_()
, saveConnectionTimerId_( -1 )
, reconnectTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_GOODGAME_SAVE_CONNECTION_INTERVAL )
, reconnectInterval_( DEFAULT_GOODGAME_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticTimerInterval_( DEFAULT_GOODGAME_STATISTIC_INTERVAL )
, useAnimatedSmiles_( false )
{
}

QGoodGameChat::~QGoodGameChat()
{
    disconnect();
}

void QGoodGameChat::connect()
{
    if( channelName_ == "" )
        return;

    smiles_.clear();
    getSmiles();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getChannelInfo();
}

void QGoodGameChat::disconnect()
{
    if( saveConnectionTimerId_ >= 0 )
    {
        killTimer( saveConnectionTimerId_ );
        saveConnectionTimerId_ = -1;
    }
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }
    if( statisticTimerId_ >= 0 )
    {
        killTimer( statisticTimerId_ );
        statisticTimerId_ = -1;
    }

    lastTimeStamp_ = 0;

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = 0;

    emit newStatistic( new QChatStatistic( GOODGAME_SERVICE, "", this ) );

}

void QGoodGameChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Reconnecting...", "", this ) );
    connect();
}

void QGoodGameChat::getSmiles()
{
    QNetworkRequest commonSmilesRequest( QUrl( DEFAULT_GOODGAME_COMMON_SMILES_INFO_LINK + "" ) );
    QNetworkReply *reply = nam_->get( commonSmilesRequest );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );

    QNetworkRequest channelsSmilesRequest( QUrl( DEFAULT_GOODGAME_CHANNELS_SMILES_INFO_LINK + "" ) );
    reply = nam_->get( channelsSmilesRequest );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QGoodGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString smilesInfo = reply->readAll();

    smilesInfo.replace( "\n", "" );

    const QString SMILE_INFO_PREFIX = ".big-smiles .smiles";
    const int SMILE_INFO_PREFIX_LENGTH = SMILE_INFO_PREFIX.length();

    int smileInfoStartPos = smilesInfo.indexOf( SMILE_INFO_PREFIX );

    while( smileInfoStartPos >= 0 )
    {
        int smileNameStartPos = smilesInfo.indexOf( ".", smileInfoStartPos + SMILE_INFO_PREFIX_LENGTH ) + 1;
        int smileNameEndPos = smilesInfo.indexOf( ",", smileNameStartPos ) - 1;

        if( smileNameEndPos - smileNameStartPos + 1 > 0  )
        {
            QString smileName = ':' + smilesInfo.mid( smileNameStartPos, smileNameEndPos - smileNameStartPos + 1 ) + ':';

            smileName.replace( " ", "" );

            int imageLinkStartPos = smilesInfo.indexOf( "\'", smileNameEndPos ) + 1;
            int imageLinkEndPos = smilesInfo.indexOf( "?", imageLinkStartPos ) - 1;

            QString imageLink = smilesInfo.mid( imageLinkStartPos, imageLinkEndPos - imageLinkStartPos + 1 );

            smiles_.insert( smileName, imageLink );
        }

        smileInfoStartPos = smilesInfo.indexOf( SMILE_INFO_PREFIX, smileNameEndPos );
    }

    const QString ANIMATED_SMILE_INFO_PREFIX = ".animated-smiles .smiles";
    const int ANIMATED_SMILE_INFO_PREFIX_LENGTH = ANIMATED_SMILE_INFO_PREFIX.length();

    smileInfoStartPos = smilesInfo.indexOf( ANIMATED_SMILE_INFO_PREFIX );

    while( smileInfoStartPos >= 0 )
    {
        int smileNameStartPos = smilesInfo.indexOf( ".", smileInfoStartPos + ANIMATED_SMILE_INFO_PREFIX_LENGTH ) + 1;
        int smileNameEndPos = smilesInfo.indexOf( ".", smileNameStartPos ) - 1;

        if( smileNameEndPos - smileNameStartPos + 1 > 0  )
        {
            QString smileName = ":" + smilesInfo.mid( smileNameStartPos, smileNameEndPos - smileNameStartPos + 1 ) + ":";

            smileName.replace( " ", "" );

            int imageLinkStartPos = smilesInfo.indexOf( "\'", smileNameEndPos ) + 1;
            int imageLinkEndPos = smilesInfo.indexOf( "?", imageLinkStartPos ) - 1;

            QString imageLink = smilesInfo.mid( imageLinkStartPos, imageLinkEndPos - imageLinkStartPos + 1 );

            animatedSmiles_.insert( smileName, imageLink );
        }

        smileInfoStartPos = smilesInfo.indexOf( ANIMATED_SMILE_INFO_PREFIX, smileNameEndPos );
    }

    //own smiles code
    QString smilesPath = QApplication::applicationDirPath() + "/smiles";

    QStringList extList;
    extList << "*.svg" << "*.png" << "*.gif" << "*.jpg";

    QDir smilesDir( smilesPath );

    QStringList smileFiles = smilesDir.entryList( extList, QDir::Files | QDir::NoSymLinks );

    foreach( const QString& smileName, smileFiles )
    {
        QString formattedSmileName = ":" + smileName.left( smileName.length() - 4 ) + ":";
        if( !smiles_.contains( formattedSmileName ) )
            smiles_.insert( formattedSmileName, "file:///" + smilesPath + "/" + smileName );
    }
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Smiles ready...", "", this ) );
    reply->deleteLater();
}


void QGoodGameChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Can not load smiles..." + reply->errorString(), "", this ) );
    reply->deleteLater();
}

void QGoodGameChat::getChannelInfo()
{   
    QNetworkRequest request( QUrl( DEFAULT_GOODGAME_CHANNEL_STATUS_PREFIX + channelName_ + DEFAULT_GOODGAME_CHANNEL_STATUS_POSTFIX ) );

    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QGoodGameChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();
            channelId_ = jsonObj.keys().at( 0 );

            connectToWebClient();
        }
    }

    reply->deleteLater();
}

void QGoodGameChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Can not connect to " + channelName_ + "..." + reply->errorString(), "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

    reply->deleteLater();
}

void QGoodGameChat::connectToWebClient()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( DEFAULT_GOODGAME_WEBSOCKET_LINK ) );
    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );

    //test ping-pong support
    QObject::connect( socket_, SIGNAL( pong( quint64,QByteArray ) ), this, SLOT( onPong( quint64,QByteArray ) ) );
}

void QGoodGameChat::onWebSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Web Socket Error ..." + socket_->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
}

void QGoodGameChat::onTextMessageRecieved( const QString& message )
{
    QJsonParseError parseError;

    //qDebug() << message;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonMessage = jsonDoc.object();

            QString messageType = jsonMessage[ "type" ].toString();

            QString answer;

            if( messageType == "message" )
            {
                //TODO: перенаправлять в парс
                QJsonObject jsonData = jsonMessage[ "data" ].toObject();

                QString message = jsonData[ "text" ].toString();

                message = QChatMessage::deleteTags( message );

                message = insertSmiles( message );
                //message = QChatMessage::insertLinks( message );

                lastTimeStamp_ = jsonData[ "timestamp" ].toInt();

                QString nickName = jsonData[ "user_name" ].toString();

                bool blackListUser = blackList().contains( nickName );
                bool supportersListUser = supportersList().contains( nickName );

                if( !isRemoveBlackListUsers() || !blackListUser )
                {
                    if( blackListUser )
                    {
                        //TODO: игнорируемые
                        emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "ignore", this ) );
                    }
                    else
                    {
                        if( supportersListUser )
                        {
                            //TODO: саппортеры
                            emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "supporter", this ) );
                        }
                        else
                        {
                            if( isContainsAliases( message ) )
                            {
                                //TODO: обращение к стримеру
                                emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "alias", this ) );
                            }
                            else
                            {
                                //TODO: простое сообщение
                                emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "", this ) );
                            }
                        }
                    }
                }
            }
            else if( message == "private_message" )
            {
                //TODO: перенаправлять в парс
                QJsonObject jsonData = jsonMessage[ "data" ].toObject();

                QString message = jsonData[ "text" ].toString();

                //message.replace( QRegExp( "~<a\\b[^>]*+>|</a\\b[^>]*+>~" ), "" );

                message = QChatMessage::deleteTags( message );

                message = insertSmiles( message );
                //message = QChatMessage::insertLinks( message );

                lastTimeStamp_ = jsonData[ "timestamp" ].toInt();

                QString nickName = jsonData[ "user_name" ].toString();

                bool blackListUser = blackList().contains( nickName );
                bool supportersListUser = supportersList().contains( nickName );

                if( !isRemoveBlackListUsers() || !blackListUser )
                {
                    if( blackListUser )
                    {
                        //TODO: игнорируемые
                        emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "ignore", this ) );
                    }
                    else
                    {
                        if( supportersListUser )
                        {
                            //TODO: саппортеры
                            emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "supporter", this ) );
                        }
                        else
                        {
                            if( isContainsAliases( message ) )
                            {
                                //TODO: обращение к стримеру
                                emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "alias", this ) );
                            }
                            else
                            {
                                emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "", this ) );
                            }
                        }
                    }
                }
            }
            else if( messageType == "welcome" )
            {
                answer = "{\"type\":\"join\",\"data\":{\"channel_id\":" + channelId_ + "}}";
                socket_->sendTextMessage( answer );
            }
            else if( messageType == "success_join" )
            {
                answer = "{\"type\":\"get_channel_history\",\"data\":{\"channel_id\":" + channelId_ + "}}";
                socket_->sendTextMessage( answer );

                if( isShowSystemMessages() )
                    emit newMessage( new QChatMessage( GOODGAME_SERVICE, GOODGAME_USER, "Connected to " + channelName_ + "...", "", this ) );

                getStatistic();
                if( statisticTimerId_ == -1 )
                    statisticTimerId_ = startTimer( statisticTimerInterval_ );

                if( saveConnectionTimerId_ )
                    saveConnectionTimerId_ = startTimer( saveConnectionInterval_ );                               
            }
            else if( messageType == "channel_history" && lastTimeStamp_ )
            {
                QJsonObject jsonData = jsonMessage[ "data" ].toObject();
                QJsonArray jsonMessages = jsonData[ "messages" ].toArray();

                for( int i = 0; i < jsonMessages.size(); ++i )
                {

                    QJsonObject val = jsonMessages[ i ].toObject();
                    int timeStamp = val[ "timestamp" ].toInt();
                    if( timeStamp > lastTimeStamp_ )
                    {
                        QString message = val[ "text" ].toString();
                        //message.replace( QRegExp( "~<a\\b[^>]*+>|</a\\b[^>]*+>~" ), "" );

                        message = QChatMessage::deleteTags( message );

                        message = insertSmiles( message );
                        //message = QChatMessage::insertLinks( message );

                        QString nickName = val[ "user_name" ].toString();

                        bool blackListUser = blackList().contains( nickName );
                        bool supportersListUser = supportersList().contains( nickName );

                        if( !isRemoveBlackListUsers() || !blackListUser )
                        {
                            if( blackListUser )
                            {
                                //TODO: игнорируемые
                                emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "ignore", this ) );
                            }
                            else
                            {
                                if( supportersListUser )
                                {
                                    //TODO: саппортеры
                                    emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "supporter", this ) );
                                }
                                else
                                {
                                    if( isContainsAliases( message ) )
                                    {
                                        //TODO: обращение к стримеру
                                        emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "alias", this ) );
                                    }
                                    else
                                    {
                                        //TODO: простое сообщение
                                        emit newMessage( new QChatMessage( GOODGAME_SERVICE, nickName, message, "", this ) );
                                    }
                                }
                            }
                        }
                        lastTimeStamp_ = timeStamp;
                    }
                }
            }
            else
            {
                //qDebug() << "Unsupported message: " << messageType;
            }

        }
    }
}

void QGoodGameChat::getStatistic()
{
    QNetworkRequest request( DEFAULT_GOOD_GAME_STATISTIC_PREFIX + channelName_ + DEFAULT_GOOD_GAME_STATISTIC_POSTFIX );
    QNetworkReply *reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QGoodGameChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            if( jsonObj[ channelId_ ].isObject() )
            {
                QJsonObject jsonStatistic = jsonObj[ channelId_ ].toObject();

                QString statistic = jsonStatistic[ "viewers" ].toString();

                if( jsonStatistic[ "status" ].toString() != "Live" )
                    statistic = "0";

                emit newStatistic( new QChatStatistic( GOODGAME_SERVICE, statistic, this ) );               
            }
        }
    }

    reply->deleteLater();
}

void QGoodGameChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

void QGoodGameChat::onPong( quint64 elapsedTime, const QByteArray &payload )
{
    qDebug() << "GoodGameChat pong recieved: " << elapsedTime << " " << payload;
}

QString QGoodGameChat::insertSmiles( const QString &message ) const
{
    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if ( !QChatMessage::isLink( tokens.at( i ) ) )//не ссылка
        {
            bool isReplaced = false;

            if( useAnimatedSmiles_ )
            {
                //заменено, если присутствует в мапе
                isReplaced = animatedSmiles_.contains( tokens.at( i ) );
                if( isReplaced )
                {
                    convertedTokens[ i ] = "<img class = \"smile\" src=\"" + animatedSmiles_[ tokens.at( i ) ] + "\"></img>";
                }
            }
            if( !isReplaced )
            {
                if( smiles_.contains( tokens.at( i ) ) )
                {
                    convertedTokens[ i ] = "<img class = \"smile\" src=\"" + smiles_[ tokens.at( i ) ] + "\"></img>";
                }
            }
        }
    }

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    return convertedMessage;
}

void QGoodGameChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && socket_->state() == QAbstractSocket::ConnectedState )
        {
            socket_->ping();
        }
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QGoodGameChat::changeUseAnimatedSmiles( bool useAnimatedSmiles )
{
    useAnimatedSmiles_ = useAnimatedSmiles;
}

void QGoodGameChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString();

    useAnimatedSmiles_ = settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool();

    //load lists
    setAliasesList( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
