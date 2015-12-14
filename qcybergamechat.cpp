#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>

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

#include "qchatmessage.h"

#include "settingsconsts.h"

#include "qcybergamechat.h"

const QString DEFAULT_CYBERGAME_WEBSOCKET_LINK = "ws://cybergame.tv:9090/websocket";
const QString DEFAULT_CYBERGAME_CHAT_LINK = "http://cybergame.tv/cgchat.htm";
const QString DEFAULT_CYBERGAME_SMILE_PREFIX = "http://cybergame.tv/";
const QString DEFAULT_CYBERGAME_STATISTIC_PREFIX = "http://api.cybergame.tv/w/streams2.php?channel=";

const int DEFAULT_CYBERGAME_SAVE_CONNECTION_INTERVAL = 25000;
const int DEFAULT_CYBERGAME_RECONNECT_INTERVAL = 10000;
const int DEFAULT_CYBERGAME_STATISTIC_INTERVAL = 10000;

const QString CYBERGAME_USER = "CYBERGAME";
const QString CYBERGAME_SERVICE = "cybergame";

QCyberGameChat::QCyberGameChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( nullptr )
, channelName_()
, lastUpd_( 0 )
, saveConnectionTimerId_( -1 )
, reconnectTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_CYBERGAME_SAVE_CONNECTION_INTERVAL )
, reconnectInterval_( DEFAULT_CYBERGAME_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_CYBERGAME_STATISTIC_INTERVAL )
{
}

QCyberGameChat::~QCyberGameChat()
{
    disconnect();
}

void QCyberGameChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );

    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );

    socket_->open( QUrl( DEFAULT_CYBERGAME_WEBSOCKET_LINK ) );
}

void QCyberGameChat::disconnect()
{
    resetTimer( saveConnectionTimerId_ );
    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );

    lastUpd_ = 0;

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    emit newStatistic( new QChatStatistic( CYBERGAME_SERVICE, QString(), this ) );
}

void QCyberGameChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() && isShowSystemMessages() )
        emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Reconnecting..." ), QString(), this ) );
    connect();
}

void QCyberGameChat::onWebSocketConnected()
{
    QString message = "{\"command\":\"login\",\"message\":\"{\\\"login\\\":\\\"\\\",\\\"password\\\":\\\"\\\",\\\"channel\\\":\\\"#" + channelName_ + "\\\"}\"}";
    socket_->sendTextMessage( message );
}

void QCyberGameChat::onWebSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Web socket error..." ) + socket_->errorString(), QString(), this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}

void QCyberGameChat::onTextMessageRecieved( const QString &message )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );

    if( QJsonParseError::NoError  == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString command = jsonObj[ "command" ].toString();
            QString answer;

            if( "changeWindow" == command )
            {
                answer = "{\"command\":\"getHistory\",\"message\":\"#" + channelName_ +"\"}";
                socket_->sendTextMessage( answer );

                if( isShowSystemMessages() )
                    emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );

                loadSmiles();

                loadStatistic();
                startUniqueTimer( statisticTimerId_, statisticInterval_ );
            }
            else if( "getHistory" == command  )
            {
                if( lastUpd_ )
                {
                }               
                startUniqueTimer( saveConnectionTimerId_, saveConnectionInterval_ );
            }
            else if( "chatMessage" == command )
            {
                QString chatMessage = jsonObj[ "message" ].toString();

                jsonDoc = QJsonDocument::fromJson( QByteArray( chatMessage.toStdString().c_str() ), &parseError );
                if( QJsonParseError::NoError == parseError.error  )
                {

                    if( jsonDoc.isObject() )
                    {
                        QString message = jsonDoc.object()[ "text" ].toString();

                        message = insertSmiles( message );

                        QString nickName = jsonDoc.object()[ "from" ].toString();

                        emit newMessage( ChatMessage( CYBERGAME_SERVICE, nickName, message, "", this ) );

                    }
                }
            }
            else
            {
            }
        }
    }
}

void QCyberGameChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_CHAT_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QCyberGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString smilesString = reply->readAll();

    int startSmilesPos = smilesString.indexOf( "smiles =" );
    int leftSmilesBracketPos = smilesString.indexOf( "{", startSmilesPos );
    int rightSmilesBracketPos = smilesString.indexOf( "}", startSmilesPos );

    smilesString = smilesString.mid( leftSmilesBracketPos, rightSmilesBracketPos - leftSmilesBracketPos + 1 );

    smilesString.replace( "{", "[" );
    smilesString.replace( "}", "]" );
    smilesString.replace( " ", "" );
    smilesString.replace( "\r", "" );
    smilesString.replace( "\n", "" );
    smilesString.replace( "\":\"", "\",\"" );

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( smilesString.toStdString().c_str() ), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray smilesInfo = jsonDoc.array();

            for( int i = 0; i < smilesInfo.size() / 2; ++i )
            {
                addSmile( smilesInfo[ 2 * i ].toString(), DEFAULT_CYBERGAME_SMILE_PREFIX + smilesInfo[ 2 * i + 1 ].toString() );
            }
            if( isShowSystemMessages() )
                emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Smiles loaded..." ), QString(), this ) );
        }
    }

    reply->deleteLater();
}

void QCyberGameChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( CYBERGAME_SERVICE, CYBERGAME_USER, tr( "Can not load smiles..." ), QString(), this ) );

    reply->deleteLater();
}

void QCyberGameChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_CYBERGAME_STATISTIC_PREFIX + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QCyberGameChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString statistic = jsonObj[ "viewers" ].toString();

            emit newStatistic( new QChatStatistic( CYBERGAME_SERVICE, statistic, this ) );
        }
    }

    reply->deleteLater();
}

void QCyberGameChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply* >( sender() );
    reply->deleteLater();
}

void QCyberGameChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            socket_->ping();
        }
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QCyberGameChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( CYBERGAME_CHANNEL_SETTING_PATH, DEFAULT_CYBERGAME_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
    {
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/", -2 ) - 1 );
        channelName_ = channelName_.left( channelName_.length() - 1 );
    }

    enable( settings.value( CYBERGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( CYBERGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( CYBERGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( CYBERGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( CYBERGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
