//#include <QRegExp>
#include <QApplication>
#include <QDir>

#include <QPair>

#include <QDebug>

#include "qchatservice.h"


const QString QChatService::TYPE_FIELD = "type";

const QString QChatService::SERVICE_NAME_FIELD = "service";

const QString QChatService::NICKNAME_FIELD = "nickname";
const QString QChatService::MESSAGE_TEXT_FIELD = "text";
const QString QChatService::MESSAGE_TYPE_FIELD = "messageType";
const QString QChatService::NICKNAME_COLOR_FIELD = "color";
const QString QChatService::MESSAGE_ICON_SUBSCRIBER_FIELD = "subscriberIcon";
const QString QChatService::MESSAGE_ICON_MODERATOR_FIELD = "moderatorIcon";
const QString QChatService::MESSAGE_ICON_TWITCH_TURBO_FIELD = "twitchTurboIcon";

const QString QChatService::STATISTIC_VALUE_FIELD = "value";

const QString QChatService::MESSAGE_TYPE = "message";
const QString QChatService::STATISTIC_TYPE = "statistic";

const QString QChatService::MESSAGE_TYPE_EMPTY = "";
const QString QChatService::MESSAGE_TYPE_ALIAS = "alias";
const QString QChatService::MESSAGE_TYPE_SUPPORTER = "supporter";
const QString QChatService::MESSAGE_TYPE_IGNORE = "ignore";


QChatService::QChatService( QObject * parent )
: QObject( parent )
, enabled_( true )
, showSystemMessages_( true )
, aliasesSelection_( false )
, removeBlackListUsers_( false )
, aliasesList_()
, supportersList_()
, blackList_()
, smiles_()
{
}

QChatService::~QChatService()
{
}

bool QChatService::isEnabled() const
{
    return enabled_;
}

void QChatService::enable( bool enabled )
{
    enabled_ = enabled;
}

void QChatService::setShowSystemMessages( bool showSystemMessages )
{
    showSystemMessages_ = showSystemMessages;
}

bool QChatService::isShowSystemMessages() const
{
    return showSystemMessages_;
}

void QChatService::setAliasesSelection( bool aliasesSelection )
{
    aliasesSelection_ = aliasesSelection;
}

bool QChatService::isAliasesSelection() const
{
    return aliasesSelection_;
}

void QChatService::setRemoveBlackListUsers( bool removeBlackListUsers )
{
    removeBlackListUsers_ = removeBlackListUsers;
}

bool QChatService::isRemoveBlackListUsers() const
{
    return removeBlackListUsers_;
}

void QChatService::setAliasesList( const QString &aliasesList )
{
    aliasesList_ = aliasesList.split( QRegExp( "\\s" ) );
}

const QStringList& QChatService::aliasesList() const
{
    return aliasesList_;
}

void QChatService::setSupportersList( const QString &supportersList )
{
    supportersList_ = supportersList.split( QRegExp( "\\s" ) );
}

const QStringList& QChatService::supportersList() const
{
    return supportersList_;
}

void QChatService::setBlackList( const QString &blackList  )
{
    blackList_ = blackList.split( QRegExp( "\\s" ) );
}

const QStringList& QChatService::blackList() const
{
    return blackList_;
}

bool QChatService::isContainsAliases( const QString &message ) const
{
    foreach( const QString &alias, aliasesList_ )
    {
        if( !alias.isEmpty() && message.contains( alias ) )
            return true;
    }
    return false;
}

void QChatService::startUniqueTimer( int & id, int interval )
{
    if( -1 == id )
        id = startTimer( interval );
}

void QChatService::resetTimer( int & id )
{
    if( -1 != id )
    {
        killTimer( id );
        id = -1;
    }
}

void QChatService::loadSmiles()
{
    smiles_.clear();

    QString smilesPath = QApplication::applicationDirPath() + "/smiles";

    QStringList extList;
    extList << "*.svg" << "*.png" << "*.gif" << "*.jpg";

    QDir smilesDir( smilesPath );

    QStringList smileFiles = smilesDir.entryList( extList, QDir::Files | QDir::NoSymLinks );

    foreach( const QString& smileName, smileFiles )
    {
        smiles_.insert( ":" + smileName.left( smileName.length() - 4 ) + ":", "file:///" + smilesPath + "/" + smileName );
    }
}

void QChatService::addSmile( const QString & code, const QString & link )
{
    if( !smiles_.contains( code ) )
        smiles_.insert( code, link );
}

QString QChatService::insertSmiles( const QString & message ) const
{
    QString convertedMessage = message;

    QStringList tokens = convertedMessage.split( QRegExp( "\\s" ) );

    tokens = tokens.toSet().toList();

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if( smiles_.contains( tokens.at( i ) ) )
        {                        
            convertedTokens[ i ].replace( tokens.at( i ) , "<img class = \"smile\" alt=\"" + tokens.at( i ) + "\"" + " src=\"" + smiles_[ tokens.at( i ) ] + "\"></img>" );
        }
    }

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    return convertedMessage;
}

void QChatService::emitJsonMessage( std::initializer_list<QPair<QString, QString> > fields )
{
    QJsonObject json;

    for( auto field : fields )
    {
        json.insert( field.first, field.second );
    }

    emit message( json, this );
}

void QChatService::emitSystemMessage( const QString & service, const QString & nickname, const QString & message )
{
    emitJsonMessage( { qMakePair( TYPE_FIELD, MESSAGE_TYPE ),\
                   qMakePair( SERVICE_NAME_FIELD, service ),\
                   qMakePair( NICKNAME_FIELD, nickname ),\
                   qMakePair( MESSAGE_TEXT_FIELD, message ),\
                   qMakePair( MESSAGE_TYPE_FIELD, MESSAGE_TYPE_EMPTY ) } );
}

void QChatService::emitMessage( const QString & service, const QString & nickname, const QString & text, std::initializer_list<QPair<QString, QString> > fields )
{
    bool blackListUser = blackList().contains( nickname );
    bool supportersListUser = supportersList().contains( nickname );
    bool containsAliases = isContainsAliases( text );

    QString type = MESSAGE_TYPE_EMPTY;

    if( !isRemoveBlackListUsers() || !blackListUser )
    {
        if( blackListUser )
        {
            type = MESSAGE_TYPE_IGNORE;
        }
        else if( supportersListUser )
        {
            type = MESSAGE_TYPE_SUPPORTER;
        }
        else if( containsAliases )
        {
            type = MESSAGE_TYPE_ALIAS;
        }
    }

    QJsonObject json;

    json.insert( TYPE_FIELD, MESSAGE_TYPE );
    json.insert( SERVICE_NAME_FIELD, service );
    json.insert( NICKNAME_FIELD, nickname );
    json.insert( MESSAGE_TEXT_FIELD, text );
    json.insert( MESSAGE_TYPE_FIELD, type );

    for( auto field : fields )
    {
        json.insert( field.first, field.second );
    }

    emit message( json, this );

}












