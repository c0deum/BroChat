//#include <QRegExp>
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
