#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QSettings>

#include <QTimerEvent>


#include "settingsconsts.h"

#include "qaceschat.h"

const QString DEFAULT_ACES_CHAT_REQUEST_LINK_PREFIX = "http://aces.gg/engine/ajax/tournament.php/?act=chat_messages&chat_message_fix_limit=";
const QString DEFAULT_ACES_SMILES_SHORT = "/uploads/hdgstournament/smiley/";
const QString DEFAULT_ACES_SMILES_LINK_PREFIX = "http://aces.gg/uploads/hdgstournament/smiley/";

const QString ACES_USER = "ACES";
const QString ACES_SERVICE = "aces";

int DEFAULT_ACES_UPDATE_CHAT_INFO_INTERVAL = 3000;
int DEFAULT_ACES_RECONNECT_INTERVAL = 10000;

struct MessageData
{
    QString nickName;
    QString message;
    int messageId;
};

QAcesChat::QAcesChat( QObject *parent )
: QChatService( parent )
, nam_( new QNetworkAccessManager( this ) )
, channelName_()
, lastMessageId_( -1 )
, updateChatInfoTimerId_( -1 )
, reconnectTimerId_( -1 )
, updateChatInfoInterval_( DEFAULT_ACES_UPDATE_CHAT_INFO_INTERVAL )
, reconnectInterval_( DEFAULT_ACES_RECONNECT_INTERVAL )
{
}

QAcesChat::~QAcesChat()
{
    disconnect();
}

void QAcesChat::connect()
{
    if( channelName_ == "" )
        return;

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Connecting to " + channelName_ + "...", "", this ) );

    getLastMessage();
}

void QAcesChat::disconnect()
{
    channelName_ = "";
    lastMessageId_ = -1;

    if( updateChatInfoTimerId_ >= 0 )
    {
        killTimer( updateChatInfoTimerId_ );
        updateChatInfoTimerId_ = -1;
    }
    if( reconnectTimerId_ >= 0 )
    {
        killTimer( reconnectTimerId_ );
        reconnectTimerId_ = -1;
    }
}

void QAcesChat::reconnect()
{
    QString oldChannelName = channelName_;
    disconnect();
    loadSettings();
    if( channelName_ != "" && oldChannelName != "" )
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Reconnecting...", "", this ) );
    connect();
}

void QAcesChat::getLastMessage()
{
    QNetworkRequest request( QUrl( DEFAULT_ACES_CHAT_REQUEST_LINK_PREFIX + "1&chat_message_f_chat=" + channelName_ ) );
    QNetworkReply *reply = nam_->get( request );
    QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onLastMessageLoaded() ) );
    QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onLastMessageLoadError() ) );
}

void QAcesChat::onLastMessageLoaded()
{
    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );


    QString response = reply->readAll();

    const QString ID_PREFIX = "data-id=\"";
    int startIdPos = response.indexOf( ID_PREFIX );

    if( startIdPos == -1 )
    {
        if( isShowSystemMessages() )
            emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not read last message from " + channelName_ + "..." + reply->errorString(), "", this ) );
        if( reconnectTimerId_ == -1 )
            reconnectTimerId_ = startTimer( reconnectInterval_ );
        reply->deleteLater();
        return;
    }

    startIdPos += ID_PREFIX.length();
    int endIdPos = response.indexOf( "\"", startIdPos  );

    lastMessageId_ = ( response.mid( startIdPos, endIdPos - startIdPos ) ).toInt();

    if( updateChatInfoTimerId_ == -1 )
        updateChatInfoTimerId_ = startTimer( updateChatInfoInterval_ );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Connected to " + channelName_ + "...", "", this ) );

    reply->deleteLater();
}

void QAcesChat::onLastMessageLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );
    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not connect to " + channelName_ + "..." + reply->errorString(), "", this ) );
    if( reconnectTimerId_ == -1 )
        reconnectTimerId_ = startTimer( reconnectInterval_ );
    reply->deleteLater();
}

bool cmpMessageDataStruct( const MessageData & lhs, const MessageData & rhs )
{
    return lhs.messageId < rhs.messageId;
}

void QAcesChat::onChatInfoLoaded()
{

    QNetworkReply * reply = qobject_cast< QNetworkReply * >( sender() );

    QString response = reply->readAll();

    if( "no" == response )
    {
        reply->deleteLater();
        return;
    }

    const QString ID_PREFIX = "data-id=\"";
    int startIdPos = response.indexOf( ID_PREFIX );
    int endIdPos = response.indexOf( "\"", startIdPos + ID_PREFIX.length() );

    const QString NICKNAME_PREPREFIX = "<span class=\"chat_message_user";
    const QString NICKNAME_PREFIX = "\">";

    const QString NICKNAME_POSTFIX = "</span>:\n";

    const QString MESSAGE_PREPREFIX = "<span class=\"chat_message_msg\">";
    const QString MESSAGE_PREFIX = "     \n                            ";
    const QString MESSAGE_POSTFIX = "                        </span>       \n";


    QList< MessageData > messagesList;

    while( ( startIdPos >= 0 ) && ( endIdPos - startIdPos > 0 ) )
    {
        startIdPos += ID_PREFIX.length();

        int newLastMessageId = ( response.mid( startIdPos, endIdPos - startIdPos ) ).toInt();

        int startNickNamePos = response.indexOf( NICKNAME_PREPREFIX, endIdPos );

        if( startNickNamePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        startNickNamePos += NICKNAME_PREPREFIX.length();

        startNickNamePos = response.indexOf( NICKNAME_PREFIX, startNickNamePos );

        if( startNickNamePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        startNickNamePos += NICKNAME_PREFIX.length();


        int endNickNamePos = response.indexOf( NICKNAME_POSTFIX, startNickNamePos );

        if( endNickNamePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        QString nickName = response.mid( startNickNamePos, endNickNamePos - startNickNamePos );

        int startMessagePos = response.indexOf( MESSAGE_PREPREFIX, endNickNamePos );

        if( startMessagePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        startMessagePos += MESSAGE_PREPREFIX.length();

        startMessagePos = response.indexOf( MESSAGE_PREFIX, startMessagePos );

        if( startMessagePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        startMessagePos += MESSAGE_PREFIX.length();

        int endMessagePos = response.indexOf( MESSAGE_POSTFIX, startMessagePos );

        if( endMessagePos == -1 )
        {
            reply->deleteLater();
            return;
        }

        if( newLastMessageId > lastMessageId_ )
        {

            QString message = response.mid( startMessagePos, endMessagePos - startMessagePos );

            const QString SPAN = "<span style=";

            if( message.left( SPAN.length() ) == SPAN )
            {
                message = message.right( message.length() - message.indexOf( "\">" ) - 2 );
                message = message.replace( "</span>", "" );
            }

            message = message.replace( DEFAULT_ACES_SMILES_SHORT, DEFAULT_ACES_SMILES_LINK_PREFIX );
            message = message.replace( "img src=", "img class = \"smile\" src=" );
            message = message.replace( '\'', "\"" );

            MessageData messageData;
            messageData.nickName = nickName;
            messageData.message = message;
            messageData.messageId = newLastMessageId;

            messagesList.append( messageData );
        }

        startIdPos = response.indexOf( ID_PREFIX, endMessagePos );
        endIdPos = response.indexOf( "\"", startIdPos + ID_PREFIX.length() );
    }

    qSort( messagesList.begin(), messagesList.end(), cmpMessageDataStruct );

    for( int i = 0; i < messagesList.size(); i++ )
    {
        QString nickName = messagesList[ i ].nickName;
        QString message = messagesList[ i ].message;

        bool blackListUser = blackList().contains( nickName );
        bool supportersListUser = supportersList().contains( nickName );

        bool containsAliases = isContainsAliases( message );

        if( !isRemoveBlackListUsers() || !blackListUser )
        {
            if( blackListUser )
            {
                //TODO: список игнорируемых
                emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "ignore", this ) );
            }
            else
            {
                if( supportersListUser )
                {
                    //TODO: список саппортеров
                    emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "supporter", this ) );
                }
                else
                {
                    if( containsAliases )
                    {
                        //TODO: обращение к стримеру
                        emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "alias", this ) );
                    }
                    else
                    {
                        emit newMessage( new QChatMessage( ACES_SERVICE, nickName, message, "", this ) );
                    }
                }
            }
        }
    }

    if( messagesList.size() > 0 )
        lastMessageId_ = messagesList.last().messageId;


    reply->deleteLater();
}

void QAcesChat::onChatInfoLoadError()
{
    QNetworkReply *reply = qobject_cast< QNetworkReply * >( sender() );

    if( isShowSystemMessages() )
        emit newMessage( new QChatMessage( ACES_SERVICE, ACES_USER, "Can not read channel messages..." + reply->errorString(), "", this ) );

    reply->deleteLater();
}

void QAcesChat::timerEvent( QTimerEvent *event )
{
    if( event->timerId() == updateChatInfoTimerId_ )
    {
        QString reqStr = DEFAULT_ACES_CHAT_REQUEST_LINK_PREFIX + "50&chat_message_f_chat=" + channelName_ + "&chat_message_f_msg_max_id=" + QString::number( lastMessageId_ );

        QNetworkRequest request( QUrl(  reqStr + "" ) );
        QNetworkReply *reply = nam_->get( request );

        QObject::connect( reply, SIGNAL( finished() ), this, SLOT( onChatInfoLoaded() ) );
        QObject::connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ), this, SLOT( onChatInfoLoadError() ) );
    }
    else if( event->timerId() == reconnectTimerId_ )
    {
        reconnect();
    }
}

void QAcesChat::loadSettings()
{
    QSettings settings;
    channelName_ = settings.value( ACES_CHANNEL_SETTING_PATH, DEFAULT_ACES_CHANNEL_NAME ).toString();

    setAliasesList( settings.value( ACES_ALIASES_SETTING_PATH, BLANK_STRING ).toString() );
    setSupportersList( settings.value( ACES_SUPPORTERS_LIST_SETTING_PATH, BLANK_STRING ).toString() );
    setBlackList( settings.value( ACES_BLACK_LIST_SETTING_PATH, BLANK_STRING ).toString() );

    setRemoveBlackListUsers( settings.value( ACES_REMOVE_BLACK_LIST_USERS_SETTING_PATH, false ).toBool() );
}
