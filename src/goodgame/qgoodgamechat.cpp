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

#include "../core/settingsconsts.h"

#include "../core/qchatmessage.h"

#include "../core/qcoremediator.h"

#include "qgoodgamechat.h"

//ChatServiceRegistrator< QGoodGameChat > goodgameChatRegistrator;

const QString DEFAULT_GOODGAME_LINK = "http://goodgame.ru";
const QString DEFAULT_GOODGAME_WEBSOCKET_LINK = "ws://chat.goodgame.ru:8081/chat/websocket";
const QString DEFAULT_GOODGAME_COMMON_SMILES_INFO_LINK = "http://goodgame.ru/css/compiled/common_smiles.css";
const QString DEFAULT_GOODGAME_CHANNELS_SMILES_INFO_LINK = "http://goodgame.ru/css/compiled/channels_smiles.css";
const QString DEFAULT_GOODGAME_ANIMATED_SMILES_PATH = "http://goodgame.ru/images/anismiles/";
const QString DEFAULT_GOODGAME_ALL_SMILES_PATH = "https://goodgame.ru/js/minified/global.js";
const QString DEFAULT_GOODGAME_CHANNEL_STATUS_PREFIX = "http://goodgame.ru/api/getchannelstatus?id=";
const QString DEFAULT_GOODGAME_CHANNEL_STATUS_POSTFIX = "&fmt=json";
const QString DEFAULT_GOOD_GAME_STATISTIC_PREFIX = "http://goodgame.ru/api/getggchannelstatus?id=";
const QString DEFAULT_GOOD_GAME_STATISTIC_POSTFIX = "&fmt=json";


const QString QGoodGameChat::SERVICE_USER_NAME = "GOODGAME";
const QString QGoodGameChat::SERVICE_NAME = "goodgame";

const int QGoodGameChat::SAVE_CONNECTION_INTERVAL = 25000;
const int QGoodGameChat::RECONNECT_INTERVAL = 10000;
const int QGoodGameChat::STATISTIC_INTERVAL = 10000;

QGoodGameChat::QGoodGameChat( QObject * parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
{
}

QGoodGameChat::~QGoodGameChat()
{
    disconnect();
}

void QGoodGameChat::connect()
{
    if( !isEnabled() || channelName_.isEmpty() )
        return;

    smiles_.clear();

    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connecting to " ) + channelName_ + tr( "..." ) );
    }

    loadChannelInfo();
}

void QGoodGameChat::disconnect()
{
    resetTimer( saveConnectionTimerId_ );
    resetTimer( reconnectTimerId_ );
    resetTimer( statisticTimerId_ );

    lastTimeStamp_ = 0;

    if( socket_ )
    {
        socket_->abort();
        socket_->deleteLater();
    }
    socket_ = nullptr;

    emit newStatistic( new QChatStatistic( SERVICE_NAME, QString(), this ) );

}

void QGoodGameChat::reconnect()
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


//#region new smiles

void QGoodGameChat::loadSmiles()
{
    QNetworkRequest smilesRequest( QUrl(DEFAULT_GOODGAME_ALL_SMILES_PATH +"") );
    QNetworkReply *reply = nam_->get( smilesRequest );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onSmilesLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onSmilesLoadError() ) );
}


void QGoodGameChat::onSmilesLoaded()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    QString smilesData = reply->readAll();
    smilesData.remove(0,smilesData.indexOf("{"));

    //validator cant eat keys without quotes so lets add them
    smilesData.replace("Smiles : [","\"Smiles\" : [");
    smilesData.replace("Channel_Smiles : {","\"Channel_Smiles\" : {");
    smilesData.replace("timezone_offset : ","\"timezone_offset\" : ");
    smilesData.replace("icons : [","\"icons\" : [");
    smilesData.replace("Content_Width:","\"Content_Width\":");
    smilesData.replace("};","}");

    QJsonDocument jsonResponse = QJsonDocument::fromJson(smilesData.toUtf8());
    QVariantMap variantData = jsonResponse.toVariant().toMap();
    QVariantList smiles = variantData["Smiles"].toList();
    parseSmiles(smiles);

    QVariantMap channelItems = variantData["Channel_Smiles"].toMap();
    for(const auto& channelName:channelItems.keys())
    {
        parseSmiles(channelItems[channelName].toList());
    }


    qWarning() << smiles_["brownuke3"];
    qWarning() << "Smiles count " << smiles_.count();
    if( isShowSystemMessages() )
    {  emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ), QString(), this ) );
       emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Smiles loaded..." ) );
    }

    reply->deleteLater();

}


/*
 * Typical smile info format
 * "id": "1720",
        "bind": "0",
        "name": "ghost",
        "donat": 0,
        "premium": 0,
        "paid": 0,
        "animated": true,
        "tag": "?s4c7rc",
        "img": "https:\/\/goodgame.ru\/images\/smiles\/ghost.png",
        "img_big": "https:\/\/goodgame.ru\/images\/smiles\/ghost-big.png",
        "img_gif": "https:\/\/goodgame.ru\/images\/anismiles\/ghost-gif.gif",
        "channel": "",
        "channel_id": 0
*/
void QGoodGameChat::parseSmiles(const QVariantList& smiles)
{
    for(auto& smileVariant:smiles)
    {
        auto smile = smileVariant.toMap();
        QString name = smile["name"].toString();
        QString image_url = smile["img"].toString();
        QString big_image_url = smile["img_big"].toString();
        if (big_image_url.size()>0)
            smiles_.insert( name, big_image_url );
        else
            smiles_.insert( name, image_url );

        QString ani_image_url;
        bool animated = smile["animated"].toBool();
        if (animated)
        {
            ani_image_url = smile["img_gif"].toString();
            animatedSmiles_.insert(name,ani_image_url);
        }
    }

}

//this function parses input string to QStrilList that contain every
//char from input so destination string can be concatenated.
//Separator char is whitespace
QStringList QGoodGameChat::parseTokensKeepWhitespaces(QString string) const
{
  //makes our finite automat' states
  enum class ParseState
  {
      Begin,Whitespace,NonWhitespace
  };

  QStringList tokens;

  QString currentToken;
  ParseState state = ParseState::Begin;
  currentToken.reserve(string.size());

  for(int k = 0; k< string.size(); ++k)
  {
    QChar currentSymbol = string.at(k);
    if (currentSymbol == ' ')
    {
        switch(state)
        {
        case ParseState::Begin:
            currentToken+=currentSymbol;
            state = ParseState::Whitespace;
            break;
        case ParseState::Whitespace:
            currentToken+=currentSymbol;
            break;
        case ParseState::NonWhitespace:
            tokens.append(currentToken);
            currentToken = currentSymbol;
            state = ParseState::Whitespace;
            break;
        }
    }
    //non-whitespace char
    else
    {
        switch(state)
        {
        case ParseState::Begin:
            currentToken+=currentSymbol;
            state = ParseState::NonWhitespace;
            break;
        case ParseState::Whitespace:
            tokens.append(currentToken);
            currentToken = currentSymbol;
            state = ParseState::NonWhitespace;
            break;
        case ParseState::NonWhitespace:
            currentToken+=currentSymbol;
            break;
        }
    }
  }

  //after  cycle we should add token if it isnt empty
  if (!currentToken.isEmpty())
      tokens.append(currentToken);
return tokens;
}

QString QGoodGameChat::replaceSmiles(const QString& token) const
{

    //make finite automat
    enum class SmileState
    {
        OutsideSmile,Colon,InsideSmile
    };


    SmileState state = SmileState::OutsideSmile;
    QStringList tokens;
    QString currentToken="";
    QString currentTokenWithoutColons = "";
    QChar currentSymbol;
    for(int k = 0; k< token.size();++k)
    {
        currentSymbol = token.at(k);
        if (currentSymbol == ':')
        {
            switch(state)
            {
            case SmileState::OutsideSmile:
                tokens.append(currentToken);
                currentToken=currentSymbol;
                currentTokenWithoutColons = "";
                state = SmileState::Colon;
                break;
            case SmileState::Colon:
                //colon after colon? ignore previous
                tokens.append(currentToken);
                currentToken=currentSymbol;
                currentTokenWithoutColons = "";
                break;
            case SmileState::InsideSmile:
                //we found possible smile's suffix colon
                auto replacedToken = replaceSmile(currentTokenWithoutColons);
                qWarning()<< "possible smile token: " <<currentTokenWithoutColons;
                //no smile found
                if (replacedToken == currentTokenWithoutColons)
                {
                    tokens.append(currentToken);

                    currentTokenWithoutColons = "";
                    currentToken+=currentSymbol;
                    state = SmileState::Colon;
                }
                else
                //smile found
                {
                    tokens.append(replacedToken);
                    currentTokenWithoutColons = "";
                    currentToken="";
                    state=SmileState::OutsideSmile;
                }
                break;
            }
        }
        else
        {
            switch(state)
            {
            case SmileState::OutsideSmile:
                currentToken+=currentSymbol;
                currentTokenWithoutColons+=currentSymbol;
                break;
            case SmileState::Colon:
                state=SmileState::InsideSmile;
                currentToken+=currentSymbol;
                currentTokenWithoutColons+=currentSymbol;
                break;
            case SmileState::InsideSmile:
                currentToken+=currentSymbol;
                currentTokenWithoutColons+=currentSymbol;
                break;
            }
        }

    }

    tokens.append(currentToken);

    QString result = "";
    for(auto& token:tokens)
    {
        result+=token;
    }
    return result;

}

QString QGoodGameChat::replaceSmile(const QString &token) const
{
    bool isReplaced = false;
    if( useAnimatedSmiles_ )
    {
        //заменено, если присутствует в мапе
        isReplaced = animatedSmiles_.contains( token );
        if( isReplaced )
        {
            qWarning()<< "found token as animated smile" <<token;
            return "<img class = \"smile\" src=\"" + animatedSmiles_[ token ] + "\"></img>";
        }
    }
    if( !isReplaced )
    {
        if( smiles_.contains( token ) )
        {
             qWarning()<< "found token as smile" <<token;
            return  "<img class = \"smile\" src=\"" + smiles_[ token ] + "\"></img>";
        }
    }
    //no changes, return original token
    return token;
}



//#endregion new smiles


void QGoodGameChat::onSmilesLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not load smiles..." ) + reply->errorString() );
    }
    reply->deleteLater();
}

void QGoodGameChat::loadChannelInfo()
{   
    QNetworkRequest request( QUrl( DEFAULT_GOODGAME_CHANNEL_STATUS_PREFIX + channelName_ + DEFAULT_GOODGAME_CHANNEL_STATUS_POSTFIX ) );
    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChannelInfoLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChannelInfoLoadError() ) );
}

void QGoodGameChat::onChannelInfoLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error )
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
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Can not connect to " ) + channelName_ + tr( "..." ) + reply->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );

    reply->deleteLater();
}

void QGoodGameChat::connectToWebClient()
{
    socket_ = new QWebSocket( QString(), QWebSocketProtocol::VersionLatest, this );
    socket_->open( QUrl( DEFAULT_GOODGAME_WEBSOCKET_LINK ) );

    QObject::connect( socket_, SIGNAL( textMessageReceived( const QString & ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    QObject::connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( onWebSocketError() ) );
}

void QGoodGameChat::onWebSocketError()
{
    if( isShowSystemMessages() )
    {
        emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web Socket Error ..." ) + socket_->errorString(), QString(), this ) );
        emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Web Socket Error ..." ) + socket_->errorString() );
    }

    startUniqueTimer( reconnectTimerId_, RECONNECT_INTERVAL );
}

void QGoodGameChat::onTextMessageRecieved( const QString & message )
{
    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( QByteArray( message.toStdString().c_str() ), &parseError );

    if( QJsonParseError::NoError == parseError.error )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonMessage = jsonDoc.object();

            QString messageType = jsonMessage[ "type" ].toString();

            QString answer;

            if( "message" == messageType )
            {
                QJsonObject jsonData = jsonMessage[ "data" ].toObject();

                QString message = jsonData[ "text" ].toString();

                message = ChatMessage::deleteTags( message );

                message = insertSmiles( message );

                lastTimeStamp_ = jsonData[ "timestamp" ].toInt();

                QString nickName = jsonData[ "user_name" ].toString();

                emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
            }
            else if( "private_message" == message )
            {
                //TODO: перенаправлять в парс
                QJsonObject jsonData = jsonMessage[ "data" ].toObject();

                QString message = jsonData[ "text" ].toString();

                message = ChatMessage::deleteTags( message );

                message = insertSmiles( message );

                lastTimeStamp_ = jsonData[ "timestamp" ].toInt();

                QString nickName = jsonData[ "user_name" ].toString();

                emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );
            }
            else if( "welcome" == messageType )
            {
                answer = "{\"type\":\"join\",\"data\":{\"channel_id\":" + channelId_ + "}}";
                socket_->sendTextMessage( answer );
            }
            else if( "success_join" == messageType )
            {
                answer = "{\"type\":\"get_channel_history\",\"data\":{\"channel_id\":" + channelId_ + "}}";
                socket_->sendTextMessage( answer );

                if( isShowSystemMessages() )
                {
                    emit newMessage( ChatMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ), QString(), this ) );
                    emitSystemMessage( SERVICE_NAME, SERVICE_USER_NAME, tr( "Connected to " ) + channelName_ + tr( "..." ) );
                }

                loadSmiles();
                loadStatistic();

                startUniqueTimer( statisticTimerId_, STATISTIC_INTERVAL );
                startUniqueTimer( saveConnectionTimerId_, SAVE_CONNECTION_INTERVAL );
            }
            else if( "channel_history" == messageType  && lastTimeStamp_ )
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

                        message = ChatMessage::deleteTags( message );

                        message = insertSmiles( message );

                        QString nickName = val[ "user_name" ].toString();

                        emit newMessage( ChatMessage( SERVICE_NAME, nickName, message, QString(), this ) );

                        lastTimeStamp_ = timeStamp;
                    }
                }
            }
            else
            {
            }

        }
    }
}

void QGoodGameChat::loadStatistic()
{
    QNetworkRequest request( DEFAULT_GOOD_GAME_STATISTIC_PREFIX + channelName_ + DEFAULT_GOOD_GAME_STATISTIC_POSTFIX );
    QNetworkReply * reply = nam_->get( request );

    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onStatisticLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onStatisticLoadError() ) );
}

void QGoodGameChat::onStatisticLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QJsonParseError parseError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson( reply->readAll(), &parseError );

    if( QJsonParseError::NoError == parseError.error  )
    {
        if( jsonDoc.isObject() )
        {
            QJsonObject jsonObj = jsonDoc.object();

            if( jsonObj[ channelId_ ].isObject() )
            {
                QJsonObject jsonStatistic = jsonObj[ channelId_ ].toObject();

                QString statistic = jsonStatistic[ "viewers" ].toString();

                if( "Live" != jsonStatistic[ "status" ].toString() )
                    statistic = "0";

                emit newStatistic( new QChatStatistic( SERVICE_NAME, statistic, this ) );
            }
        }
    }

    reply->deleteLater();
}

void QGoodGameChat::onStatisticLoadError()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );
    reply->deleteLater();
}

QString QGoodGameChat::insertSmiles( const QString & message ) const
{
    QString convertedMessage;

    QStringList whiteSpacetokens = parseTokensKeepWhitespaces(message);

    for( const auto& whiteSpaceToken: whiteSpacetokens)
    {
        if ( !ChatMessage::isLink( whiteSpaceToken ) )//не ссылка
        {
            convertedMessage+=replaceSmiles(whiteSpaceToken);
        }
        else
        {
            convertedMessage+=whiteSpaceToken;
        }
    }


    return convertedMessage;

}

void QGoodGameChat::timerEvent( QTimerEvent * event )
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

void QGoodGameChat::changeUseAnimatedSmiles( bool useAnimatedSmiles )
{
    useAnimatedSmiles_ = useAnimatedSmiles;
}

void QGoodGameChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( GOODGAME_CHANNEL_SETTING_PATH, DEFAULT_GOODGAME_CHANNEL_NAME ).toString();

    if( ChatMessage::isLink( channelName_ ) )
    {
        channelName_ = channelName_.right( channelName_.length() - channelName_.lastIndexOf( "/", -2 ) - 1 );
        channelName_ = channelName_.left( channelName_.length() - 1 );
    }

    enable( settings.value( GOODGAME_CHANNEL_ENABLE_SETTING_PATH, DEFAULT_CHANNEL_ENABLE ).toBool() );

    badges_ = settings.value( GOODGAME_BADGES_SETTING_PATH, false ).toBool();

    useAnimatedSmiles_ = settings.value( GOODGAME_USE_ANIMATED_SMILES_SETTING_PATH, false ).toBool();

    setAliasesList( settings.value( GOODGAME_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( GOODGAME_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( GOODGAME_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( GOODGAME_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}

void QGoodGameChat::changeBadges( bool badges )
{
    badges_ = badges;
}
