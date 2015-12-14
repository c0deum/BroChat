//#include <QRegExp>
#include <QApplication>
#include <QDir>

#include <QDebug>

#include "qchatservice.h"

QChatService::QChatService( QObject *parent )
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
        if( alias != "" && message.contains( alias ) )
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

    QStringList convertedTokens = tokens;

    for( int i = 0; i < tokens.size(); ++i )
    {
        if( smiles_.contains( tokens.at( i ) ) )
        {
            convertedTokens[ i ].replace( tokens.at( i ) , "<img class = \"smile\" src=\"" + smiles_[ tokens.at( i ) ] + "\"></img>" );
        }
    }

    for( int i = 0; i < tokens.size(); ++i )
    {
        convertedMessage.replace( tokens.at( i ), convertedTokens.at( i ) );
    }

    return convertedMessage;
}



















