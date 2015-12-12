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

#include "qchatmessage.h"

#include "settingsconsts.h"

#include "qfunstreamchat.h"

const QString DEFAULT_FUNSTREAM_WEBSOCKET_LINK = "ws://funstream.tv:3811/socket.io/?EIO=3&transport=websocket";
const QString DEFAULT_FUNSTREAM_SMILES_LINK = "http://funstream.tv/build/images/smiles/";
const QString DEFAULT_FUNSTREAM_STATISTIC_LINK = "https://funstream.tv/api/user/list";


const QString DEFAULT_FUNSTREAM_SMILES_REQUEST = "https://funstream.tv/api/smile";

const QString DEFAULT_FUNSTREAM_CHANNEL_PREFIX = "http://funstream.tv/stream/";



const int DEFAULT_FUNSTREAM_RECONNECT_INTERVAL = 10000;
const int DEFAULT_FUNSTREAM_SAVE_CONNECTION_INTTERVAL = 25000;
const int DEFAULT_FUNSTREAM_STATISTIC_INTERVAL = 10000;


const QString FUNSTREAM_USER = "FUNSTREAM";
const QString FUNSTREAM_SERVICE = "funstream";

QFunStreamChat::QFunStreamChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, socket_( 0 )
, channelName_()
, channelId_()
, smiles_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_FUNSTREAM_RECONNECT_INTERVAL )
, saveConnectionTimerId_( -1 )
, saveConnectionInterval_( DEFAULT_FUNSTREAM_SAVE_CONNECTION_INTTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_FUNSTREAM_STATISTIC_INTERVAL )
, lastMessageId_( 0 )
, requestId_( -1 )
, joinRequestId_( -1 )
, statisticRequestId_( -1 )
, historyRequestId_( -1 )
, historyLastMessageId_( 0 )
{

}

QFunStreamChat::~QFunStreamChat()
{
    disconnect();
}

void QFunStreamChat::connect()
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

    smiles_.clear();

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getSmiles();
    getChannelInfo();
}

void QFunStreamChat::disconnect()
{
    resetTimer( reconnectTimerId_ );
    resetTimer( saveConnectionTimerId_ );
    resetTimer( statisticTimerId_ );

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = 0;

    emit newStatistic( new QChatStatistic( FUNSTREAM_SERVICE, "", this ) );
}

void QFunStreamChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( isEnabled() && !channelName_.isEmpty() && !oldChannelName.isEmpty() )
    {
        //новый канал
        if( channelName_ != oldChannelName )
        {
            //флаг, что историю грузить не надо
            historyLastMessageId_ = 0;
        }
        if( isShowSystemMessages() )
            emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Reconnecting...", "", this ) );
    }
    connect();
}

void QFunStreamChat::getChannelInfo()
{
    //QString requestLink = ChatMessage::isLink( channelName_ )? channelName_ : DEFAULT_FUNSTREAM_CHANNEL_PREFIX + channelName_;

    QNetworkRequest request( QUrl( DEFAULT_FUNSTREAM_CHANNEL_PREFIX + channelName_ ) );

    //QNetworkRequest request( QUrl( requestLink + "" ) );


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

    if( QJsonParseError::NoError == parseError.error )
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

            /*
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
                emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Smiles ready...", "", this ) );
            */

            connectToWebClient();
        }
    }

    reply->deleteLater();
}

void QFunStreamChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Can not connect to " + channelName_ + "..." + reply->errorString(), "", this ) );

    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );

    reply->deleteLater();
}

void QFunStreamChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_FUNSTREAM_SMILES_REQUEST + "" ) );

    request.setRawHeader( "Accept", "application/json" );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );

    QByteArray data;
    data.append( "{}" );

    QNetworkReply * reply = nam_->post( request, data );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmileLoadError() ) );
}

void QFunStreamChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    //qDebug() << reply->readAll();

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
                smiles_.append( QChatSmile( ":" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString() ) );

                //Free smiles for funstream
                smiles_.append( QChatSmile( ":free-" + smileInfoObj[ "code" ].toString() + ":", smileInfoObj[ "url" ].toString() ) );

            }                        

            if( isShowSystemMessages() )
                emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Smiles ready...", "", this ) );
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
        QChatSmile smile( ":" + smileName.left( smileName.length() - 4 ) + ":",
                          "file:///" + smilesPath + "/" + smileName );
        smiles_.append( smile );
    }

    reply->deleteLater();
}

void QFunStreamChat::onSmileLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Can not load smiles...", "", this ) );

    reply->deleteLater();
}

void QFunStreamChat::getStatistic()
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

void QFunStreamChat::getHistory()
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

        //qDebug() << message;

        socket_->sendTextMessage( message );
    }
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
        if ( !ChatMessage::isLink( tokens.at( i ) ) )//не ссылка
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
    startUniqueTimer( saveConnectionTimerId_, saveConnectionInterval_ );
}

void QFunStreamChat::onWebSocketError()
{
    if( isShowSystemMessages() )
        emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Web socket error..." + socket_->errorString(), "", this ) );    
    startUniqueTimer( reconnectTimerId_, reconnectInterval_ );
}

//TODO: переделатьнах
void QFunStreamChat::parseMessage( const QJsonObject & jsonObj )
{
    if( "stream/" + channelId_ == jsonObj[ "channel" ].toString() && jsonObj[ "id" ].toInt() > historyLastMessageId_ )
    {
        historyLastMessageId_ = jsonObj[ "id" ].toInt();

        QString type = jsonObj[ "type" ].toString();

        if( "announce" == type )
            return;

        QString nickName = jsonObj[ "from" ].toObject()[ "name" ].toString();
        QString message = jsonObj[ "text" ].toString();

        message = ChatMessage::replaceEscapeCharecters( message );

        if( "fastdonate" == type )
            message += "<img class = \"smile\" src=\"http://funstream.tv/build/images/icon_donate.png\"></img>";

        if( !jsonObj[ "to" ].toObject()[ "name" ].toString().isEmpty() )
            message = jsonObj[ "to" ].toObject()[ "name" ].toString() + ", " + message;

        message = insertSmiles( message );

        emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "", this ) );
    }
}



void QFunStreamChat::onTextMessageRecieved( const QString &message )
{

    //qDebug() << message;

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
                QJsonObject jsonObj = jsonDoc.object();

                if( "stream/" + channelId_ == jsonObj[ "channel" ].toString() && jsonObj[ "id" ].toInt() > lastMessageId_ )
                {
                    lastMessageId_ = jsonObj[ "id" ].toInt();                                       

                    QString type = jsonObj[ "type" ].toString();

                    if( "announce" == type )
                        return;

                    QString nickName = jsonObj[ "from" ].toObject()[ "name" ].toString();
                    QString message = jsonObj[ "text" ].toString();

                    message = ChatMessage::replaceEscapeCharecters( message );

                    if( "fastdonate" == type )
                        message += "<img class = \"smile\" src=\"http://funstream.tv/build/images/icon_donate.png\"></img>";

                    if( !jsonObj[ "to" ].toObject()[ "name" ].toString().isEmpty() )
                        message = jsonObj[ "to" ].toObject()[ "name" ].toString() + ", " + message;

                    message = insertSmiles( message );

                    emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "", this ) );

                    /*
                    bool blackListUser = blackList().contains( nickName );
                    bool supportersListUser = supportersList().contains( nickName );

                    if( !isRemoveBlackListUsers() || !blackListUser )
                    {
                        if( blackListUser )
                        {
                            //TODO: игнорируемые
                            emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "ignore", this ) );
                        }
                        else
                        {
                            if( supportersListUser )
                            {
                                //TODO: саппортеры
                                emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "supporter", this ) );
                            }
                            else
                            {
                                if( isContainsAliases( message ) )
                                {
                                    //TODO: сообщение к стримеру
                                    emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "alias", this ) );
                                }
                                else
                                {
                                    //TODO: простое сообщение
                                    emit newMessage( ChatMessage( FUNSTREAM_SERVICE, nickName, message, "", this ) );
                                }
                            }
                        }
                    }
                    */


                    //emit newMessage( ChatMessage( FUNSTREAM_SERVICE, jsonObj[ "from" ].toObject()[ "name" ].toString(), jsonObj[ "to" ].toObject()[ "name" ].toString() + jsonObj[ "text" ].toString(), "", this ) );
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
            emit newMessage( ChatMessage( FUNSTREAM_SERVICE, FUNSTREAM_USER, "Connected to " + channelName_ + "...", "", this ) );

        getHistory();
        getStatistic();

        startUniqueTimer( statisticTimerId_, statisticInterval_ );
    }
    else
    {
        QString requestId = message.left( message.indexOf( "[" ) );

        if( requestId == QString( "43" + QString::number( statisticRequestId_ ) ) )
        {
            /*
            const QString STATISTIC_PREFIX = "\"amount\":";
            const QString STATISTIC_POSTFIX = ",";
            int startStatisticPos = message.indexOf( STATISTIC_PREFIX ) + STATISTIC_PREFIX.length();
            int endStatisticPos = message.indexOf( STATISTIC_POSTFIX, startStatisticPos ) - 1;

            if( endStatisticPos - startStatisticPos + 1 > 0 )
            {
                QString statistic = message.mid( startStatisticPos, endStatisticPos - startStatisticPos + 1 ).replace( " ", "" );
                emit newStatistic( new QChatStatistic( FUNSTREAM_SERVICE, statistic, this ) );
            }
            */

            const QString RESULT_PREFIX = "\"result\":";
            const QString RESULT_POSTFIX = "}";
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

                    emit newStatistic( new QChatStatistic( FUNSTREAM_SERVICE, statistic, this ) );

                }

            }

        }
        else if( requestId == QString( "43" + QString::number( historyRequestId_ ) ) )
        {
            /*
            if( lastMessageId_ == 0 )
                return;
                */

            const QString MESSAGES_INFO_PREFIX = "\"result\":";

            int messagesInfoStart = message.indexOf( MESSAGES_INFO_PREFIX ) + MESSAGES_INFO_PREFIX.length();
            int messagesInfoEnd = message.indexOf( ']', messagesInfoStart );



            if( messagesInfoEnd - messagesInfoStart + 1 > 0 )
            {

                QString messagesInfo = message.mid( messagesInfoStart, messagesInfoEnd - messagesInfoStart + 1 );

                //qDebug() << messagesInfo;

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


void QFunStreamChat::onPong( quint64 elapsedTime, const QByteArray &payload )
{
    qDebug() << "FunStreamChat pong recieved: " << elapsedTime << " " << payload;
}



void QFunStreamChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
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

void QFunStreamChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( FUNSTREAM_CHANNEL_SETTING_PATH, DEFAULT_FUNSTREAM_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/" ) - 1 );

    enable( settings.value( FUNSTREAM_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    setAliasesList( settings.value( FUNSTREAM_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( FUNSTREAM_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( FUNSTREAM_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( FUNSTREAM_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
