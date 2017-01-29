#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QWebSocket>
#include <QString>
#include <QStack>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QTimerEvent>

#include <QSettings>

#include <QApplication>
#include <QDir>
#include <QStringList>

#include "../core/qchatmessage.h"

#include "../core/settingsconsts.h"

#include "qpeka2chat.h"

const QString DEFAULT_PEKA2_WEBSOCKET_LINK = "ws://chat.funstream.tv/?EIO=3&transport=websocket";

const QString DEFAULT_PEKA2_SMILES_LINK = "http://funstream.tv/build/images/smiles/";
const QString DEFAULT_PEKA2_STATISTIC_LINK = "http://funstream.tv/api/user/list";
const QString DEFAULT_PEKA2_SMILES_REQUEST = "http://funstream.tv/api/smile";
const QString DEFAULT_PEKA2_CHANNEL_PREFIX = "http://funstream.tv/stream/";

const QString DEFAULT_PEKA2_CHANNEL_INFO_PREFIX = "http://funstream.tv/api/user";
const QString DEFAULT_PEKA2_STREAM_INFO_REQUEST = "http://funstream.tv/api/stream";


const QString DEFAULT_PEKA2_USERS_COLORS_LINK = "http://funstream.tv/build/bundle.css";

const QString DEFAULT_PEKA2_BADGES_LINK = "http://funstream.tv/api/masterstreamer/icon/list";

const QString QPeka2Chat::SERVICE_NAME = "peka2";
const QString QPeka2Chat::SERVICE_USER_NAME = "PEKA2";

const int QPeka2Chat::SAVE_CONNECTION_INTERVAL = 25000;
const int QPeka2Chat::RECONNECT_INTERVAL = 10000;
const int QPeka2Chat::STATISTIC_INTERVAL = 10000;


QPeka2Chat::QPeka2Chat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QPeka2Chat::~QPeka2Chat()
{
    disconnect();
}

void QPeka2Chat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    historyLastMessageId_ = lastMessageId_;
    lastMessageId_ = 0;
    channelId_.clear();

    requestId_ = -1;
    joinRequestId_ = -1;
    statisticRequestId_ = -1;
    historyRequestId_ = -1;

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();

}

void QPeka2Chat::disconnect()
{
    resetTimer( reconnectTimerId_ );
    resetTimer( saveConnectionTimerId_ );
    resetTimer( statisticTimerId_ );

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );
}

void QPeka2Chat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() )
    {
        if( channelName_ != oldChannelName )
        {
            historyLastMessageId_ = 0;
        }
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ), QString(), this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Reconnecting..." ) );
        }
    }
    connect();
}


void QPeka2Chat::loadChannelInfo()
{
    QNetworkRequest request( QUrl( DEFAULT_PEKA2_STREAM_INFO_REQUEST + "" ) );

    request.setRawHeader( "Accept", "application/json" );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );

    QByteArray data;

    //data.append( "{id:null,options:null,owner:\"" + channelName_ + "\"}" );
    data.append( "{slug:\"" + channelName_ + "\"}" );

    QNetworkReply * reply = nam_->post( request, data );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onChannelInfoLoadError() ) );

}

void QPeka2Chat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
    {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject jsonOwner = jsonObj[ "owner" ].toObject();

        channelId_ = QString::number( jsonOwner[ "id" ].toInt() );

        connectToWebClient();
    }

    reply->deleteLater();
}

void QPeka2Chat::onChannelInfoLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QPeka2Chat::loadSmiles()
{
    QChatService::loadSmiles();

    QNetworkRequest request( QUrl( DEFAULT_PEKA2_SMILES_REQUEST + "" ) );

    request.setRawHeader( "Accept", "application/json" );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );

    QByteArray data;
    data.append( "{}" );

    QNetworkReply * reply = nam_->post( request, data );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmileLoadError() ) );

    loadTwitchSmiles();
}

void QPeka2Chat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );
    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray jsonSmilesInfoArr = jsonDoc.array();

            foreach( const QJsonValue &smileInfo, jsonSmilesInfoArr )
            {

                QJsonObject smileInfoObj = smileInfo.toObject();                

                //addSmile( ":" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString()/*.replace( "https://", "http://" )*/ );
                //addSmile( ":free-" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString() );

                addSmile( ":" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString().replace( "https://", "http://" ) );
                addSmile( ":free-" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString().replace( "https://", "http://" ) );
            }            

            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
            }
        }
    }

    reply->deleteLater();
}

void QPeka2Chat::onSmileLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) );
    }

    reply->deleteLater();
}

void QPeka2Chat::loadStatistic()
{
    if( channelId_.isEmpty() )
        return;
    if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        requestId_++;
        statisticRequestId_ = requestId_;
        QString message = "42" + QString::number( statisticRequestId_ ) + "[\"/chat/channel/list\",{\"channel\":\"stream/" + channelId_ + "\"}]";

        socket_->sendTextMessage( message );
    }
}

void QPeka2Chat::loadHistory()
{
    if( channelId_.isEmpty() )
        return;

    if( 0 == historyLastMessageId_ )
        return;

    if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
    {
        requestId_++;
        historyRequestId_ = requestId_;
        QString message = "42" + QString::number( historyRequestId_ ) + \
                "[\"/chat/history\",{\"channel\":\"stream/" + channelId_ + "\",\"amount\":100,\"query\":null}]";

        socket_->sendTextMessage( message );
    }
}

void QPeka2Chat::connectToWebClient()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
    QObject::connect( socket_, SIGNAL( connected() ), this, SLOT( onWebSocketConnected() ) );

    socket_->open( QUrl( DEFAULT_PEKA2_WEBSOCKET_LINK ) );
}

void QPeka2Chat::onWebSocketConnected()
{       
    startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
}

void QPeka2Chat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web socket error..." ) + socket_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web socket error..." ) + socket_->errorString() );
    }
    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QPeka2Chat::parseMessage( const QJsonObject & jsonObj )
{
    if( "stream/" + channelId_ == jsonObj[ "channel" ].toString() && jsonObj[ "id" ].toInt() > historyLastMessageId_ )
    {
        historyLastMessageId_ = jsonObj[ "id" ].toInt();

        QString type = jsonObj[ "type" ].toString();

        if( "announce" == type )
            return;

        QString nickName = jsonObj[ "from" ].toObject()[ "name" ].toString();

        if( originalColors_ )
        {
            int colorIndex = jsonObj[ "from" ].toObject()[ "color" ].toInt();

            if( styles_.contains( colorIndex ) )
            {
                nickName = "<span style=\"" + styles_[ colorIndex ] + "\">" + nickName + "</span>";
            }
            qDebug() << nickName;
        }

        //test badges
        if( badges_ )
        {
            int userId = jsonObj[ "from" ].toObject()[ "id" ].toInt();
            if( badgesMap_.contains( userId ) )
            {
                nickName = "<img class =\"badge\" src=\"" + badgesMap_[ userId ] + "\"></img>" + nickName;
            }
        }

        QString message = jsonObj[ "text" ].toString();

        message = ChatMessage::replaceEscapeCharecters( message );

        if( "fastdonate" == type )
            message += "<img class = \"smile\" src=\"http://funstream.tv/build/images/icon_donate.png\"></img>";

        if( !jsonObj[ "to" ].toObject()[ "name" ].toString().isEmpty() )
            message = jsonObj[ "to" ].toObject()[ "name" ].toString() + ", " + message;

        message = insertSmiles( message );

        emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, "", this ) );
    }
}



void QPeka2Chat::onTextMessageRecieved( const QString &message )
{
    qDebug() << message;
    if( "42[\"/chat/message\"" == message.left( 18 ) )
    {
        QString messageContent = message.mid( 19 );
        messageContent = messageContent.left( messageContent.length() - 1 );

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( messageContent.toStdString().c_str() ), &parseError );
        if( QJsonParseError::NoError == parseError.error )
        {
            if( jsonDoc.isObject() )
            {
                //{"id":37969,"channel":"stream/109344","from":{"id":152083,"name":"c0deum"},"to":{"id":0,"name":""},"text":"буду фанстримс тут тестить:grumpy:","time":"2015-05-28 21:35:47"}

                //42["/chat/message",{"id":208802523,"channel":"stream/125","from":{"id":191649,"name":"Red Old Fox","color":14},"to":null,"text":":uuu:","type":"message","time":1462272985}]
                QJsonObject jsonObj = jsonDoc.object();

                if( "stream/" + channelId_ == jsonObj[ "channel" ].toString() && jsonObj[ "id" ].toInt() > lastMessageId_ )
                {

                    qDebug() << jsonObj;

                    lastMessageId_ = jsonObj[ "id" ].toInt();                                       

                    QString type = jsonObj[ "type" ].toString();

                    if( "announce" == type )
                        return;

                    QString nickName = jsonObj[ "from" ].toObject()[ "name" ].toString();

                    if( originalColors_ )
                    {
                        int colorIndex = jsonObj[ "from" ].toObject()[ "color" ].toInt();

                        if( styles_.contains( colorIndex ) )
                        {
                            nickName = "<span style=\"" + styles_[ colorIndex ] + "!important;\">" + nickName + "</span>";
                        }
                        qDebug() << nickName;
                    }


                    //test badges
                    if( badges_ )
                    {
                        int userId = jsonObj[ "from" ].toObject()[ "id" ].toInt();
                        if( badgesMap_.contains( userId ) )
                        {
                            nickName = "<img class =\"badge\" src=\"" + badgesMap_[ userId ] + "\"></img>" + nickName;
                        }
                    }


                    QString message = jsonObj[ "text" ].toString();

                    message = ChatMessage::replaceEscapeCharecters( message );

                    if( "fastdonate" == type )
                        message += "<img class = \"smile\" src=\"http://funstream.tv/build/images/icon_donate.png\"></img>";

                    if( !jsonObj[ "to" ].toObject()[ "name" ].toString().isEmpty() )
                        message = jsonObj[ "to" ].toObject()[ "name" ].toString() + ", " + message;

                    message = insertSmiles( message );

                    emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
                }

            }
        }
    }
    else if( "0" == message.left( 1 ) )
    {

    }
    else if( "40" == message.left( 2 ) )
    {        
        requestId_ = 0;
        //QString message = "420[\"/chat/login\",{\"token\":null}]";
        QString message = "420[\"/chat/join\",{\"channel\":\"stream/" + channelId_ + "\"}]";
        socket_->sendTextMessage( message );
    }
    else if( "430" == message.left(3) )
    {
        if( isShowSystemMessages() )
        {
            emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "...", "", this ) );
            emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, "Connected to " + channelName_ + "..." );
        }

        loadSmiles();
        loadStyles();
        loadHistory();
        loadStatistic();

        loadBadges();

        startUniqueTimer( statisticTimerId_, SAVE_CONNECTION_INTERVAL );
    }
    else
    {
        QString requestId = message.left( message.indexOf( "[" ) );

        if( requestId == QString( "43" + QString::number( statisticRequestId_ ) ) )
        {
            const QString RESULT_PREFIX = "\"result\":";
            const QString RESULT_POSTFIX = "}}";
            int startResultPos = message.indexOf( RESULT_PREFIX ) + RESULT_PREFIX.length();
            int endResultPos = message.indexOf( RESULT_POSTFIX, startResultPos );

            if( endResultPos - startResultPos + 1 > 0 )
            {
                QString jsonText = message.mid( startResultPos, endResultPos - startResultPos + 1 ).replace( " ", "" );

                QJsonParseError parseError;

                QJsonDocument jsonDoc = QJsonDocument::fromJson( jsonText.toUtf8(), &parseError );

                if( QJsonParseError::NoError == parseError.error && jsonDoc.isObject() )
                {
                    QJsonObject jsonObj = jsonDoc.object();

                    int amount = jsonObj[ "amount" ].toInt();

                    QJsonArray users = jsonObj[ "users" ].toArray();

                    QString statistic = QString::number( users.size() ) + "+(" + QString::number( amount - users.size() )  + ")";

                    emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );

                }

            }

        }
        else if( requestId == QString( "43" + QString::number( historyRequestId_ ) ) )
        {
            const QString MESSAGES_INFO_PREFIX = "\"result\":";

            int messagesInfoStart = message.indexOf( MESSAGES_INFO_PREFIX ) + MESSAGES_INFO_PREFIX.length();
            int messagesInfoEnd = message.indexOf( ']', messagesInfoStart );

            if( messagesInfoEnd - messagesInfoStart + 1 > 0 )
            {

                QString messagesInfo = message.mid( messagesInfoStart, messagesInfoEnd - messagesInfoStart + 1 );

                QJsonParseError parseError;
                QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( messagesInfo.toStdString().c_str() ), &parseError );
                if( QJsonParseError::NoError == parseError.error )
                {
                    if( jsonDoc.isArray() )
                    {
                        QJsonArray jsonMessagesInfoArr = jsonDoc.array();

                        QStack< QJsonObject > messagesStack;

                        foreach( const QJsonValue & jsonMesasgeInfo, jsonMessagesInfoArr  )
                        {

                            QJsonObject jsonMesasgeInfoObj = jsonMesasgeInfo.toObject();
                            if( jsonMesasgeInfoObj[ "id" ].toInt() > historyLastMessageId_ )
                            {
                                messagesStack.push( jsonMesasgeInfoObj );
                            }
                        }

                        while( !messagesStack.isEmpty() )
                        {
                            parseMessage( messagesStack.pop() );
                        }

                    }
                }
            }
        }
    }
}

void QPeka2Chat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        loadStatistic();
    }
    else if( event->timerId() == saveConnectionTimerId_ )
    {
        if( socket_ && socket_->isValid() && QAbstractSocket::ConnectedState == socket_->state() )
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

void QPeka2Chat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( PEKA2_CHANNEL_SETTING_PATH, DEFAULT_PEKA2_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( PEKA2_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    originalColors_ = settings.value( PEKA2_ORIGINAL_COLORS_SETTING_PATH, false ).toBool();
    badges_ = settings.value( PEKA2_BADGES_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( PEKA2_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( PEKA2_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( PEKA2_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( PEKA2_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QPeka2Chat::loadStyles()
{
    styles_.clear();

    QNetworkRequest request( QUrl( DEFAULT_PEKA2_USERS_COLORS_LINK + "" ) );

    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStylesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onStylesLoadError() ) );

}

void QPeka2Chat::onStylesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString stylesData = reply->readAll();

    const QString USER_STYLE_PREFIX = ".chat_msg .user_name.user-name-color";

    int startUserStylePrefixPos = stylesData.indexOf( USER_STYLE_PREFIX );

    while( -1 != startUserStylePrefixPos  )
    {
        int startUserStyleIndexPos = startUserStylePrefixPos + USER_STYLE_PREFIX.length();

        int endUserStyleIndexPos = stylesData.indexOf( "{", startUserStyleIndexPos ) - 1;

        if( endUserStyleIndexPos - startUserStyleIndexPos + 1 > 0 )
        {
            int index = stylesData.mid( startUserStyleIndexPos, endUserStyleIndexPos - startUserStyleIndexPos + 1 ).toInt();

            if( !styles_.contains( index ) )
            {
                int startUserStylePos = stylesData.indexOf( "{", endUserStyleIndexPos ) + 1;
                int endUserStylePos = stylesData.indexOf( "}", startUserStylePos ) - 1;

                if( endUserStylePos - startUserStylePos + 1 )
                {
                    styles_.insert( index, stylesData.mid( startUserStylePos, endUserStylePos - startUserStylePos + 1 ) );
                }

            }

        }

        startUserStylePrefixPos = stylesData.indexOf( USER_STYLE_PREFIX, startUserStyleIndexPos );
    }

    qDebug() << styles_;



    reply->deleteLater();
}

void QPeka2Chat::onStylesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load styles..." ) );
    }

    reply->deleteLater();
}

void QPeka2Chat::changeOriginalColors( bool originalColors )
{
    originalColors_ = originalColors;
}

void QPeka2Chat::loadBadges()
{
    badgesMap_.clear();

    QNetworkRequest request( QUrl( DEFAULT_PEKA2_BADGES_LINK + "" ) );

    QByteArray data;

    data.append( "{}" );

    QNetworkReply * reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onBadgesLoaded() ) );
    QObject::connect( reply, SIGNAL( error(QNetworkReply::NetworkError) ), this, SLOT( onBadgesLoadError() ) );
}

void QPeka2Chat::onBadgesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    //qDebug() << reply->readAll();

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error && jsonDoc.isArray() )
    {


        QJsonArray jsonArr = jsonDoc.array();

        foreach( const QJsonValue & jsonVal, jsonArr )
        {
            QJsonObject jsonObj = jsonVal.toObject();

            badgesMap_.insert( jsonObj[ "userId" ].toInt(), jsonObj[ "icon" ].toString().replace( "https://", "http://" ) );
        }

        /*
        QJsonObject jsonObj = jsonDoc.object();

        QString nickName = jsonObj[ "name" ].toString();
        QString badgeIcon;

        if( jsonObj[ "icon" ].isString() )
        {
            badgeIcon = jsonObj[ "icon" ].isString();
        }

        if( !badgeIcon.isEmpty() )
        {
            badges_[ nickName ] = badgeIcon;
        }
        */




        qDebug() << badgesMap_;
    }

    reply->deleteLater();
}

void QPeka2Chat::onBadgesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QPeka2Chat::changeBadges( bool badges )
{
    badges_ = badges;
}

void QPeka2Chat::loadTwitchSmiles()
{
    QString smilesCodes[] = { ":)", ":(", ":D", ">(", ":|", "O_o", "B)", ":O", "&lt;3", ":/", ";)", ":P", ";P", "R)" };

    for( int i = 0; i < 14; i++ )
    {
        //qDebug() << DEFAULT_TWITCH_SMILE_PREFIX + QString::number( i + 1 ) + "/1.0";
        addSmile( ":tw-" + smilesCodes[ i ] + ":", "http://static-cdn.jtvnw.net/emoticons/v1/" + QString::number( i + 1 ) + "/1.0" );
    }

    QNetworkRequest request( QUrl( "https://api.twitch.tv/kraken/chat/emoticon_images" ) );

    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onTwitchSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onTwitchSmilesLoadError() ) );
}

void QPeka2Chat::onTwitchSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;
    QJsonDocument jsonDoc;

    jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonArray jsonEmotIcons = jsonDoc.object()[ "emoticons" ].toArray();

            foreach( const QJsonValue &value, jsonEmotIcons )
            {
                QJsonObject jsonEmotIcon = value.toObject();

                //addSmile( jsonEmotIcon[ "regex" ].toString(), jsonEmotIcon[ "url" ].toString() );

                addSmile( ":tw-" + jsonEmotIcon[ "code" ].toString() + ":", "http://static-cdn.jtvnw.net/emoticons/v1/" + QString::number( jsonEmotIcon[ "id" ].toInt() ) + "/1.0" );

            }
            if( isShowSystemMessages() )
            {
                emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
                emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
            }
        }
    }

    reply->deleteLater();
}

void QPeka2Chat::onTwitchSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}


























