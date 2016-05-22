#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include <QSettings>

#include <QUuid>


#include "settingsconsts.h"

#include "qazubuchat.h"

const QString DEFAULT_AZUBU_SMILES_REQUEST_LINK = "http://www.azubu.tv/api/emoticon/list";
const QString DEFAULT_AZUBU_CHANNEL_INFO_LINK_PREFIX = "http://www.azubu.tv/api/user/chat/channels/";
const QString DEFAULT_AZUBU_STATISTIC_LINK_PREFIX = "http://api.azubu.tv/public/channel/";

const QString DEFAULT_AZUBU_CHAT_HOST_NAME = "irc.quakenet.org";
const int DEFAULT_AZUBU_CHAT_PORT_NUMBER = 6667;

const QString DEFAULT_AZUBU_USER_NAME = "broadcasterchat";

const QString QAzubuChat::SERVICE_NAME = "azubu";
const QString QAzubuChat::SERVICE_USER_NAME = "AZUBU";

const int QAzubuChat::RECONNECT_INTERVAL = 20000;
const int QAzubuChat::STATISTIC_INTERVAL = 10000;
const int QAzubuChat::SAVE_CONNECTION_INTERVAL = 25000;

QAzubuChat::QAzubuChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QAzubuChat::~QAzubuChat()
{
    disconnect();
}

void QAzubuChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    if( socket_)
    {
        if( socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            return;
        }

        safeDeleteSocket();
    }

    loadChannelInfo();
    loadSmiles();
}

void QAzubuChat::disconnect()
{
    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );
    resetTimer( saveConnectionTimerId_ );

    if( socket_ && QAbstractSocket::ConnectedState == socket_->state() )
    {
        socket_->write( "PART\r\n" );
        socket_->write( "QUIT\r\n" );
        socket_->flush();
        socket_->disconnect();
    }

    safeDeleteSocket();

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QAzubuChat::reconnect()
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

void QAzubuChat::timerEvent( QTimerEvent * event )
{
    if( saveConnectionTimerId_ ==  event->timerId() )
    {
        if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
        {
            socket_->write( "PING :irc.quakenet.org\r\n" );
        }
    }
    else if( statisticTimerId_ == event->timerId() )
    {
        loadStatistic();
    }
    else if( reconnectTimerId_ == event->timerId() )
    {
        reconnect();
    }
}

void QAzubuChat::loadSettings()
{    
    QSettings settings;

    channelName_= settings.value( "/Settings/Azubu/Channel", DEFAULT_AZUBU_CHANNEL_NAME ).toString();

    //originalColors_ = settings.value( TWITCH_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();

    //badges_ = settings.value( TWITCH_BADGES_SETTING_PATH, false ).toBool();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( AZUBU_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( AZUBU_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( AZUBU_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( AZUBU_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( AZUBU_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QAzubuChat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_AZUBU_CHANNEL_INFO_LINK_PREFIX + channelName_ ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QAzubuChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender () );

    QByteArray data = reply->readAll();

    qDebug() << data;

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( data, &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonArray jsonArr = jsonObj[ "data" ].toArray();

        for( const QJsonValue & channelInfoObj : jsonArr )
        {
            //QJsonObject channelInfoObj = jsonArr[ 0 ].toObject();

            if( channelInfoObj.toObject()[ "default" ].toBool() )
                channelIrcName_ = channelInfoObj.toObject()[ "verbose_name" ].toString().toLower();
        }

        /////////////////////////////

        socket_ = new QTcpSocket();

        QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
        QObject::connect( socket_, SIGNAL( readyRead() ), this, SLOT( parseMessage() ) );
        QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onSocketError() ) );

        socket_->connectToHost( DEFAULT_AZUBU_CHAT_HOST_NAME, DEFAULT_AZUBU_CHAT_PORT_NUMBER );
    }

    reply->deleteLater();
}

void QAzubuChat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender () );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Can not connect to..." + channelName_, "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Can not connect to..." + channelName_ );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QAzubuChat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_AZUBU_SMILES_REQUEST_LINK + "" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT(onSmilesLoadError() ) );
}

void QAzubuChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender () );

    QByteArray data = reply->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( data, &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonArray smilesInfoArr = jsonObj[ "data" ].toArray();

        for( const QJsonValue & jsonVal : smilesInfoArr )
        {
            QJsonObject smileInfo = jsonVal.toObject();
            addSmile( smileInfo[ "code" ].toString(), smileInfo[ "source_url" ].toString() );
        }

        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
        }
    }

    reply->deleteLater();
}

void QAzubuChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender () );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) );
    }

    reply->deleteLater();
}

void QAzubuChat::onConnected()
{
    if( !socket_ )
        return;

    //loadSettings();

    randomName_ = "brochat" + QUuid::createUuid().toString().mid( 1, 8 );

    socket_->write( QString( "USER " + randomName_ + " * * :" + randomName_ + "\r\n" ).toStdString().c_str() );
    socket_->write( QString( "NICK " + randomName_ + "\r\n" ).toStdString().c_str() );

}


void QAzubuChat::parseMessage()
{
    if( !socket_ )
        return;

    if( QAbstractSocket::ConnectedState == socket_->state() )
    {
        QString line = socket_->readLine();

        line.replace( "\r\n", "" );

        //emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, line, QString(), this ) );
        //qDebug() << line;

        if( line.startsWith( "PING :" ) )
        {
            line.replace( "PING", "PONG" );
            line += "\r\n";

            socket_->write( line.toStdString().c_str() );
        }
        else if( line.contains( "376 " + randomName_ ) )
        {
            socket_->write( QString( "JOIN " + channelIrcName_ + "\r\n" ).toStdString().c_str() );

            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
            }

            loadStatistic();

            startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );
            startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
        }
        else if( line.contains( "PRIVMSG " + channelIrcName_ ) )
        {
            int startMessagePos = line.indexOf( channelIrcName_ ) + channelIrcName_.length() + 2;
            int endMessagePos = line.length() - 4;

            QString nickName = line.mid( 1, line.indexOf( "!" ) - 1 ) ;
            QString message = line.mid( startMessagePos, endMessagePos - startMessagePos + 1 );

            message = ChatMessage::replaceEscapeCharecters( message );

            message = insertSmiles( message );

            //:c0deum!azubu@26a83f5a.flash.quakenet.org PRIVMSG #azubu.c0deum.en :3218
            emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
        }
        if( socket_->canReadLine() )
        {
            parseMessage();
        }
    }
}

void QAzubuChat::onSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Socket Error ..." + socket_->errorString(), "", this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Socket Error ..." + socket_->errorString() );
    }
    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QAzubuChat::loadStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_AZUBU_STATISTIC_LINK_PREFIX + channelName_ + "/info" ) );

    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),this, SLOT( onStatisticLoadError() ) );
}

void QAzubuChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject dataObj = jsonObj[ "data" ].toObject();

        QString statistic = dataObj[ "is_live" ].toBool()? QString::number( dataObj[ "view_count" ].toInt() ) : "";

        emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
    }

    reply->deleteLater();
}

void QAzubuChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    reply->deleteLater();
}

void QAzubuChat::safeDeleteSocket()
{
    if( socket_ )
    {
        socket_->deleteLater();
    }
    socket_ = nullptr;
}
