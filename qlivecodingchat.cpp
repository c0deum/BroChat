#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


#include <QSettings>
#include <QTimerEvent>

#include <QXmppMucManager.h>
#include <QXmppMessage.h>

#include "settingsconsts.h"

#include "qlivecodingchat.h"


const QString DEFAULT_LIVECODING_LOGIN = "broadcasterchat";
const QString DEFAULT_LIVECODING_JID = DEFAULT_LIVECODING_LOGIN + "@livecoding.tv";
const QString DEFAULT_LIVECODING_PASSWORD = "8e4820297b36ec893f1242bc36ffc1e38";
const QString DEFAULT_LIVECODING_CONFERENCE_JID_POSTFIX = "@chat.livecoding.tv";

const QString DEFAULT_LIVECODING_CANDY_JS_LINK = "https://www.livecoding.tv/static/candy-chat/candy.bundle.js";
const QString DEFAULT_LIVECODING_SMILES_LINK_PREFIX = "https://www.livecoding.tv/static/candy-chat/img/emoticons_hd/";


const int DEFAULT_LIVECODING_RECONNECT_INTERVAL = 10000;
const int DEFAULT_LIVECODING_STATISTIC_INTERVAL = 10000;

const QString LIVECODING_USER ="LIVECODING";
const QString LIVECODING_SERVICE = "livecoding";

QLivecodingChat::QLivecodingChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, xmppClient_( 0 )
, mucManager_( 0 )
, channelName_()
, connectionTime_()
, smiles_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_LIVECODING_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_LIVECODING_STATISTIC_INTERVAL )
{
}

QLivecodingChat::~QLivecodingChat()
{
}

void QLivecodingChat::connect()
{
    if( channelName_ == "" )
        return;

    qsrand( QDateTime::currentDateTime().toTime_t() );

    xmppClient_ = new QXmppClient( this );

    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError( QXmppClient::Error ) ) );
    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );

    mucManager_ = new QXmppMucManager();

    xmppClient_->addExtension( mucManager_ );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( LIVECODING_SERVICE, LIVECODING_USER, "Connecting to " + channelName_ + "...", "", this ) );

    xmppClient_->connectToServer( DEFAULT_LIVECODING_JID , DEFAULT_LIVECODING_PASSWORD );
}

void QLivecodingChat::disconnect()
{
    if( reconnectTimerId_ != -1 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }

    if( statisticTimerId_ != -1 )
    {
        killTimer( statisticTimerId_ );
        statisticTimerId_ = -1;
    }

    if( mucManager_ )
    {
        if( !mucManager_->rooms().empty() )
        {
            mucManager_->rooms().first()->leave();
            mucManager_->rooms().first()->deleteLater();
        }

        mucManager_->deleteLater();
        mucManager_ = 0;
    }

    if( xmppClient_ )
    {
        xmppClient_->disconnectFromServer();
        xmppClient_->deleteLater();

        xmppClient_ = 0;
    }

    //TODO: emit newStatistic( new QChatStatistic( LIVECODING_SERVICE, "", this ) );
}

void QLivecodingChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( LIVECODING_SERVICE, LIVECODING_USER, "Reconnecting...", "", this ) );
    connect();
}

void QLivecodingChat::onConnected()
{
    connectionTime_ = QDateTime::currentDateTimeUtc();

    QXmppMucRoom *room = mucManager_->addRoom( channelName_ + DEFAULT_LIVECODING_CONFERENCE_JID_POSTFIX );
    room->setNickName( DEFAULT_LIVECODING_LOGIN );
    room->join();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( LIVECODING_SERVICE, LIVECODING_USER, "Connected to " + channelName_ + "...", "", this ) );

    smiles_.clear();
    getSmiles();
}

void QLivecodingChat::onError( QXmppClient::Error )
{
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( LIVECODING_SERVICE, LIVECODING_USER, "Unknown Error ...", "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
}

void QLivecodingChat::onMessageReceived( const QXmppMessage & message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {

        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        QString messageBody = message.body();

        messageBody = QChatMessage::replaceEscapeCharecters( messageBody );

        messageBody = insertSmiles( messageBody );

        bool blackListUser = blackList().contains( nickName );
        bool supportersListUser = supportersList().contains( nickName );

        bool containsAliases = isContainsAliases( messageBody );

        if( !isRemoveBlackListUsers() || !blackListUser )
        {
            if( blackListUser )
            {
                //TODO: список игнорируемых
                emit newMessage( new QChatMessage( LIVECODING_SERVICE, nickName, messageBody, "ignore", this ) );
            }
            else
            {
                if( supportersListUser )
                {
                    //TODO: список саппортеров
                    emit newMessage( new QChatMessage( LIVECODING_SERVICE, nickName, messageBody, "supporter", this ) );
                }
                else
                {
                    if( containsAliases )
                    {
                        //TODO: обращение к стримеру
                        emit newMessage( new QChatMessage( LIVECODING_SERVICE, nickName, messageBody, "alias", this ) );
                    }
                    else
                    {
                        emit newMessage( new QChatMessage( LIVECODING_SERVICE, nickName, messageBody, "", this ) );
                    }
                }
            }
        }
    }
}

void QLivecodingChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_LIVECODING_CANDY_JS_LINK + "" ) );

    QNetworkReply * reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QLivecodingChat::onSmilesLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString info = reply->readAll();

    //говно говно код
    const QString EMOTICONS = "emoticons: [";

    int smilesInfoStart = info.indexOf( EMOTICONS ) + EMOTICONS.length() - 1;
    int smilesInfoEnd = info.indexOf( ']', smilesInfoStart );

    info = info.mid( smilesInfoStart, smilesInfoEnd - smilesInfoStart + 1 );

    info.replace( '\n', "" );
    info.replace( '\t', "" );
    info.replace( ' ', ""  );

    info.replace( "plain", "\"plain\"" );
    info.replace( "regex:", "\"regex\":\"" );
    info.replace( "/gim", "/gim\"" );
    info.replace( "/gm", "/gm\"" );
    info.replace( "image", "\"image\"" );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( info.toStdString().c_str(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isArray() )
        {
            QJsonArray smilesArr = jsonDoc.array();

            foreach( const QJsonValue &value, smilesArr )
            {
                QJsonObject smileInfo = value.toObject();

                QChatSmile smile(  smileInfo[ "plain" ].toString(), DEFAULT_LIVECODING_SMILES_LINK_PREFIX + smileInfo[ "image" ].toString(), 0 ,0 );

                smiles_.insert( smileInfo[ "plain" ].toString(), smile );
            }
        }
    }

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( LIVECODING_SERVICE, LIVECODING_USER, "Smiles ready...", "", this ) );

    reply->deleteLater();

}

void QLivecodingChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

QString QLivecodingChat::insertSmiles( const QString & message ) const
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
                if( tokens.at( i ) == smile.name() )
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

void QLivecodingChat::timerEvent( QTimerEvent * event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        //getStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QLivecodingChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( LIVECODING_CHANNEL_SETTING_PATH, DEFAULT_LIVECODING_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( LIVECODING_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( LIVECODING_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( LIVECODING_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( LIVECODING_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
