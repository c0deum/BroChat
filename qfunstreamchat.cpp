#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>
#include <QString>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include <QSettings>

#include "settingsconsts.h"

#include "qfunstreamchat.h"

const QString DEFAULT_FUNSTREAM_WEBSOCKET_LINK = "ws://funstream.tv:3811/socket.io/?EIO=3&transport=websocket";
const QString DEFAULT_FUNSTREAM_SMILES_LINK = "http://funstream.tv/build/images/smiles/";

const QString DEFAULT_FUNSTREAM_CHANNEL_PREFIX = "http://funstream.tv/stream/";

const int DEFAULT_FUNSTREAM_SAVE_CONNECTION_INTTERVAL = 25000;
const int DEFAULT_FUNSTREAM_RECONNECT_INTERVAL = 10000;

const QString FUNSTREAM_USER = "FUNSTREAM";
const QString FUNSTREAM_SERVICE = "funstream";

QFunStreamChat::QFunStreamChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( 0 )
, channelName_()
, channelId_()
, smiles_()
, saveConnectionTimerId_( -1 )
, reconnectTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_FUNSTREAM_SAVE_CONNECTION_INTTERVAL )
, reconnectInterval_( DEFAULT_FUNSTREAM_RECONNECT_INTERVAL )
, lastMessageId_( 0 )
{

}

QFunStreamChat::~QFunStreamChat()
{
    disconnect();
}

void QFunStreamChat::connect()
{
    if( channelName_ == "" )
        return;

    lastMessageId_ = 0;

    smiles_.clear();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getChannelInfo();
}

void QFunStreamChat::disconnect()
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

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = 0;
}

void QFunStreamChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Reconnecting...", "", this ) );
    connect();
}

void QFunStreamChat::getChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_FUNSTREAM_CHANNEL_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );

}

void QFunStreamChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString info = reply->readAll();

    int startInfoPos = info.indexOf( "[[" );
    int endInfoPos = info.indexOf( ";</script>" );

    info = info.mid( startInfoPos, endInfoPos - startInfoPos );

    //qDebug() << "Script:\n" << info;

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( info.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {

            QJsonArray jsonArr = jsonDoc.array();

            /*
            QJsonArray jsonStreamInfoArr = jsonArr[ 0 ].toArray();

            QJsonObject streamInfo = jsonStreamInfoArr[ 1 ].toObject();

            channelId_ = QString::number( streamInfo[ "id" ].toInt() );
            */


            QJsonArray jsonStreamInfoArr = jsonArr[ 1 ].toArray();
            //parse stream info

            QJsonObject jsonStreamInfoObj = jsonStreamInfoArr[ 2 ].toObject();
            QJsonObject jsonStreamerInfoObj = jsonStreamInfoObj[ "streamer" ].toObject();

            channelId_ = QString::number( jsonStreamerInfoObj[ "id" ].toInt() );

            //parse smile info
            QJsonArray jsonSmilesInfoArr = jsonArr[ 3 ].toArray();
            QJsonArray jsonSmilesTabs = jsonSmilesInfoArr[ 2 ].toArray();

            //0-6 + 1(streamers smiles)
            const int SMILES_TABS = 8;

            for( int i = 0; i < SMILES_TABS; i++ )
            {
                foreach( const QJsonValue &smileInfo, jsonSmilesTabs[ i ].toArray() )
                {
                    QJsonObject smileInfoObj = smileInfo.toObject();
                    smiles_.append( QChatSmile( ":" + smileInfoObj[ "code" ].toString() + ":", DEFAULT_FUNSTREAM_SMILES_LINK + smileInfoObj[ "image" ].toString(), 0 , 0 ) );

                    //Free smiles for funstream
                    smiles_.append( QChatSmile( ":free-" + smileInfoObj[ "code" ].toString() + ":", DEFAULT_FUNSTREAM_SMILES_LINK + smileInfoObj[ "image" ].toString(), 0 , 0 ) );
                }
            }

            if( isShowSystemMessages() )
                emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Smiles ready...", "", this ) );

            connectToWebClient();
        }
    }

    reply->deleteLater();
}

void QFunStreamChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Can not connect to " + channelName_ + "..." + reply->errorString(), "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

    reply->deleteLater();
}

void QFunStreamChat::connectToWebClient()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );

    //test ping-pong support
    QObject::connect( socket_, SIGNAL( pong( quint64,QByteArray ) ), this, SLOT( onPong( quint64,QByteArray ) ) );

    socket_->open( QUrl( DEFAULT_FUNSTREAM_WEBSOCKET_LINK ) );
}

QString QFunStreamChat::insertSmiles( const QString &message ) const
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




void QFunStreamChat::onWebSocketConnected()
{
    if( saveConnectionTimerId_ == -1 )
        saveConnectionTimerId_ = startTimer( saveConnectionInterval_ );
}

void QFunStreamChat::onWebSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Web socket error..." + socket_->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
}


void QFunStreamChat::onTextMessageRecieved( const QString &message )
{
    //qDebug() << QString( message );
    //qDebug() << message;
    //42["/chat/message",{"id":37969,"channel":"stream/109344","from":{"id":152083,"name":"c0deum"},"to":{"id":0,"name":""},"text":"буду фанстримс тут тестить:grumpy:","time":"2015-05-28 21:35:47"}]
    if( message.left( 18 ) == "42[\"/chat/message\"" )
    {
        QString messageContent = message.mid( 19 );
        messageContent = messageContent.left( messageContent.length() - 1 );

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( messageContent.toStdString().c_str() ), &parseError );
        if( parseError.error == QJsonParseError::NoError )
        {
            if( jsonDoc.isObject() )
            {
                //{"id":37969,"channel":"stream/109344","from":{"id":152083,"name":"c0deum"},"to":{"id":0,"name":""},"text":"буду фанстримс тут тестить:grumpy:","time":"2015-05-28 21:35:47"}
                QJsonObject jsonObj = jsonDoc.object();

                if( "stream/" + channelId_ == jsonObj[ "channel" ].toString() && jsonObj[ "id" ].toInt() > lastMessageId_ )
                {
                    lastMessageId_ = jsonObj[ "id" ].toInt();                                       

                    QString type = jsonObj[ "type" ].toString();

                    if( type == "announce" )
                        return;

                    QString nickName = jsonObj[ "from" ].toObject()[ "name" ].toString();
                    QString message = jsonObj[ "text" ].toString();

                    message = QChatMessage::replaceEscapeCharecters( message );

                    if( type == "fastdonate" )
                        message += "<img class = \"smile\" src=\"http://funstream.tv/build/images/icon_donate.png\"></img>";

                    if( jsonObj[ "to" ].toObject()[ "name" ].toString() != "" )
                        message = jsonObj[ "to" ].toObject()[ "name" ].toString() + ", " + message;

                    message = insertSmiles( message );

                    bool blackListUser = blackList().contains( nickName );
                    bool supportersListUser = supportersList().contains( nickName );

                    if( !isRemoveBlackListUsers() || !blackListUser )
                    {
                        if( blackListUser )
                        {
                            //TODO: игнорируемые
                            emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, nickName, message, "ignore", this ) );
                        }
                        else
                        {
                            if( supportersListUser )
                            {
                                //TODO: саппортеры
                                emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, nickName, message, "supporter", this ) );
                            }
                            else
                            {
                                if( isContainsAliases( message ) )
                                {
                                    //TODO: сообщение к стримеру
                                    emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, nickName, message, "alias", this ) );
                                }
                                else
                                {
                                    //TODO: простое сообщение
                                    emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, nickName, message, "", this ) );
                                }
                            }
                        }
                    }


                    //emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, jsonObj[ "from" ].toObject()[ "name" ].toString(), jsonObj[ "to" ].toObject()[ "name" ].toString() + jsonObj[ "text" ].toString(), "", this ) );
                }

            }
        }
    }
    else if( message.left( 1 ) == "0" )
    {

    }
    else if( message.left( 2 ) == "40" )
    {
        //420["/chat/login",{"token":null}]
        QString message = "420[\"/chat/login\",{\"token\":null}]";
        socket_->sendTextMessage( message );
        //421["/chat/join",{"channel":"stream/109344"}]
        message = "420[\"/chat/join\",{\"channel\":\"stream/" + channelId_ + "\"}]";
        socket_->sendTextMessage( message );
    }
    else if( message.left(3) == "430" || message.left( 3 ) == "431" )
    {
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Connected to " + channelName_ + "...", "", this ) );
    }
}

void QFunStreamChat::onPong( quint64 elapsedTime, const QByteArray &payload )
{
    qDebug() << "FunStreamChat pong recieved: " << elapsedTime << " " << payload;
}


void QFunStreamChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && socket_->state() == QAbstractSocket::ConnectedState )
        {
            socket_->sendTextMessage( "2" );
            socket_->ping();
        }
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QFunStreamChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, DEFAULT_FUNSTREAM_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( FUNSTREAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( FUNSTREAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
