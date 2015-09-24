#include <QCoreApplication>
#include <QSettings>
#include <QUuid>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include <QXmppClient.h>
#include <QXmppMessage.h>
#include <QXmppPresence.h>
#include <QXmppIq.h>

#include <QXmppMucManager.h>

#include "settingsconsts.h"


#include "qrealltvchat.h"

const QString DEFAULT_REALLTV_JID = "903@reall.tv";
const QString DEFAULT_REALLTV_PASSWORD = "8e4820297b36ec893f1242bc36ffc1e3";

const QString DEFAULT_REALLTV_CANDY_JS_LINK = "http://reall.tv/js/candy/candy.min.js";
const QString DEFAULT_REALLTV_SMILES_PREFIX = "http://reall.tv/img/emoticons/";
const QString DEFAULT_REALLTV_CONFERENCE_JID_POSTFIX = "@conference.reall.tv";

const QString DEFAULT_REALLTV_CHANNEL_INFO_LINK = "http://reall.tv/channel/get_cid";
const QString DEFAULT_REALLTV_STATICTIC_LINK_PREFIX = "http://reall.tv/api/client/status/c/";



const int DEFAULT_REALLTV_RECONNECT_INTERVAL = 10000;
const int DEFAULT_REALLTV_STATISTIC_INTERVAL = 10000;


const QString REALLTV_SERVICE = "realltv";
const QString REALLTV_USER = "REALLTV";

QReallTvChat::QReallTvChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, xmppClient_( 0 )
, mucManager_( 0 )
, channelName_()
, cid_()
, connectionTime_()
, smiles_()
, reconnectTimerId_( -1 )
, reconnectInterval_( DEFAULT_REALLTV_RECONNECT_INTERVAL )
, statisticTimerId_( -1 )
, statisticInterval_( DEFAULT_REALLTV_STATISTIC_INTERVAL )
{
    /*
    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError() ) );

    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );
    //QObject::connect( xmppClient_, SIGNAL( presenceReceived(QXmppPresence) ), this, SLOT( onPresenseReceived( QXmppPresence ) ) );
    //QObject::connect( xmppClient_, SIGNAL( iqReceived( QXmppIq ) ), this, SLOT( onIqReceived( QXmppIq ) ) );

    //QObject::connect( xmppClient_, SIGNAL( logMessage( QXmppLogger::MessageType,QString ) ), this, SLOT( onLogMessage( QXmppLogger::MessageType,QString ) ) );

    //xmppClient_->logger()->setLoggingType( QXmppLogger::StdoutLogging );
    xmppClient_->addExtension( mucManager_ );
    */

}

QReallTvChat::~QReallTvChat()
{
    disconnect();
}

void QReallTvChat::connect()
{
    //qDebug() << "Connect";

    if( channelName_ == "" )
        return;

    qsrand( QDateTime::currentDateTime().toTime_t() );

    xmppClient_ = new QXmppClient( this );

    QObject::connect( xmppClient_, SIGNAL( connected() ), this, SLOT( onConnected() ) );
    QObject::connect( xmppClient_, SIGNAL( error( QXmppClient::Error ) ), this, SLOT( onError( QXmppClient::Error ) ) );
    QObject::connect( xmppClient_, SIGNAL( messageReceived( QXmppMessage ) ), this, SLOT( onMessageReceived( QXmppMessage ) ) );

    mucManager_ = new QXmppMucManager();

    xmppClient_->addExtension( mucManager_ );

    //xmppClient_->logger()->setLoggingType( QXmppLogger::StdoutLogging );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Connecting to " + channelName_ + "...", "", this ) );



    //xmppClient_->connectToServer( DEFAULT_REALLTV_JID , DEFAULT_REALLTV_PASSWORD );

    QXmppConfiguration conf;

    conf.setDomain( "reall.tv" );
    conf.setSaslAuthMechanism( "ANONYMOUS" );

    xmppClient_->connectToServer( conf );
}

void QReallTvChat::disconnect()
{
    //qDebug() << "Disconnect";

    cid_ = "";

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
        //mucManager_->rooms().first()->leave();
        //mucManager_->rooms().first()->deleteLater();

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

    emit newStatistic( new QChatStatistic( REALLTV_SERVICE, "", this ) );
}

void QReallTvChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Reconnecting...", "", this ) );
    connect();
}

void QReallTvChat::onConnected()
{
    //qDebug() << "onConnected";

    connectionTime_ = QDateTime::currentDateTimeUtc();

    QXmppMucRoom *room = mucManager_->addRoom( channelName_ + DEFAULT_REALLTV_CONFERENCE_JID_POSTFIX );
    room->setNickName( "broadcasterchat" + QUuid::createUuid().toString() );
    room->join();

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Connected to " + channelName_ + "...", "", this ) );

    getChannelInfo();

    emit newStatistic( new QChatStatistic( REALLTV_SERVICE, "", this ) );

    smiles_.clear();
    getSmiles();
}

void QReallTvChat::onError( QXmppClient::Error error )
{
    qDebug() << error;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Unknown Error ...", "", this ) );

    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );

}

void QReallTvChat::onMessageReceived( const QXmppMessage &message )
{
    if( message.stamp().toTime_t() > connectionTime_.toTime_t() )
    {
        QString nickName = message.from();
        nickName = nickName.right( nickName.length() - nickName.indexOf( '/' ) - 1 );

        QString messageBody = message.body();

        int dotPos = messageBody.indexOf( "·" );
        if( dotPos != -1 )
        {
            messageBody = messageBody.right( messageBody.length() - dotPos - 1 );
        }

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
                emit newMessage( new QChatMessage( REALLTV_SERVICE, nickName, messageBody, "ignore", this ) );
            }
            else
            {
                if( supportersListUser )
                {
                    //TODO: список саппортеров
                    emit newMessage( new QChatMessage( REALLTV_SERVICE, nickName, messageBody, "supporter", this ) );
                }
                else
                {
                    if( containsAliases )
                    {
                        //TODO: обращение к стримеру
                        emit newMessage( new QChatMessage( REALLTV_SERVICE, nickName, messageBody, "alias", this ) );
                    }
                    else
                    {
                        emit newMessage( new QChatMessage( REALLTV_SERVICE, nickName, messageBody, "", this ) );
                    }
                }
            }
        }
    }
}

void QReallTvChat::getChannelInfo()
{
    QNetworkRequest request( QUrl(  DEFAULT_REALLTV_CHANNEL_INFO_LINK + "" ) );

    QByteArray data;

    QString dataString = "{\"data\":{\"nick\":\"" + channelName_ + "\"}}";

    data.append( dataString );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant( "application/json;charset=utf-8" ) );
    request.setRawHeader( "Content-Length", QString::number( data.size() ).toStdString().c_str() );

    QNetworkReply *reply = nam_->post( request, data );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QReallTvChat::onChannelInfoLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();
            QJsonObject jsonInfo = jsonObj[ "info" ].toObject();

            cid_ = QString::number( jsonInfo[ "cid" ].toInt() );

            //qDebug() << "Reall Cid: " << cid_;

            //{"status":"ok","info":{"cid":855,"uid":892,"poster":"jpg"}}
        }
    }

    getStatistic();
    if( statisticTimerId_ == -1 )
        statisticTimerId_ = startTimer( statisticInterval_ );

    reply->deleteLater();
}

void QReallTvChat::onChannelInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

void QReallTvChat::getStatistic()
{
    QNetworkRequest request( QUrl( DEFAULT_REALLTV_STATICTIC_LINK_PREFIX + cid_ ) );

    QNetworkReply *reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );

}

void QReallTvChat::onStatisticLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    //{"cid":855,"nick":"popcorn","online":0,"viewers":1,"viewers_total":17,"viewers_all_time":92,"qualities":[{"id":-1,"original":true}],"followers":0,"followers_list":null,"related_channels":[]}

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if(  parseError.error == QJsonParseError::NoError )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            /*
            bool isOnline = jsonObj[ "online" ].toInt() != 0;


            if( isOnline )
            {
                QString statistic = QString::number( jsonObj[ "viewers" ].toInt() );
                emit newStatistic( new QChatStatistic( REALLTV_SERVICE, statistic, this ) );
            }
            else
            {
                emit newStatistic( new QChatStatistic( REALLTV_SERVICE, "0", this ) );
            }
            */
            QString statistic = QString::number( jsonObj[ "viewers" ].toInt() );
            emit newStatistic( new QChatStatistic( REALLTV_SERVICE, statistic, this ) );
        }
    }



    reply->deleteLater();
}

void QReallTvChat::onStatisticLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}



void QReallTvChat::getSmiles()
{
    QNetworkRequest request( QUrl( DEFAULT_REALLTV_CANDY_JS_LINK + "" ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}

void QReallTvChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString response = reply->readAll();

    const QString EMOTICONS = "emoticons:";

    int emotIconsPos = response.indexOf( EMOTICONS );

    if( emotIconsPos )
    {
        int emotIconsArrayStart = emotIconsPos + EMOTICONS.length();

        int emotIconsArrayEnd = response.indexOf( ']', emotIconsArrayStart );

        response = response.mid( emotIconsArrayStart, emotIconsArrayEnd - emotIconsArrayStart + 1 );

        response.replace( "plain", "\"plain\"" );
        response.replace( "regex:", "\"regex\":\"" );
        response.replace( "/gim", "/gim\"" );
        response.replace( "/gm", "/gm\"" );
        response.replace( "image", "\"image\"" );


        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( response.toStdString().c_str() ), &parseError );
        if( parseError.error == QJsonParseError::NoError )
        {
            if( jsonDoc.isArray() )
            {
                QJsonArray smilesInfoArr = jsonDoc.array();

                foreach( const QJsonValue &value, smilesInfoArr )
                {
                    QJsonObject smileInfo = value.toObject();

                    QChatSmile smile(  smileInfo[ "plain" ].toString(), DEFAULT_REALLTV_SMILES_PREFIX + smileInfo[ "image" ].toString(), 0 ,0 );

                    smiles_.insert( smileInfo[ "plain" ].toString(), smile );

                }
            }
        }

        //qDebug() << response;

    }


    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Smiles ready...", "", this ) );

    reply->deleteLater();
}


void QReallTvChat::onSmilesLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( REALLTV_SERVICE, REALLTV_USER, "Can not load smiles..." + reply->errorString() + "..." + QDateTime::currentDateTime().toString(), "", this ) );

    reply->deleteLater();
}

QString QReallTvChat::insertSmiles( const QString &message ) const
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

/*
void QReallTvChat::onPresenseReceived( const QXmppPresence &presence )
{
    qDebug() << "onPresenseReceived";
}

void QReallTvChat::onIqReceived( const QXmppIq &iq )
{
    qDebug() << "onIqReceived";
}

void QReallTvChat::onLogMessage( QXmppLogger::MessageType messageType, const QString &logMessage )
{
    qDebug() << "Client state = " << xmppClient_->state();
    qDebug() << "onLogMessage " << messageType << " " << logMessage;
    qDebug() << "Client state = " << xmppClient_->state();
}
*/
void QReallTvChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == statisticTimerId_ )
    {
        getStatistic();
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QReallTvChat::loadSettings()
{
    QSettings settings;

    channelName_ = settings.value( REALLTV_CHANNEL_SETTING_PATH, DEFAULT_REALLTV_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( REALLTV_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( REALLTV_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( REALLTV_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setRemoveBlackListUsers( settings.value( REALLTV_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

