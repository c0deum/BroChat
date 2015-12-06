#include <cassert>

#include <QApplication>
#include <QFile>
#include <QDateTime>

#include "qmessagesmanager.h"

QMessagesManager::QMessagesManager( QObject * parent )
: QObject( parent )
, messages_()
{
}

QMessagesManager::~QMessagesManager()
{
}

void QMessagesManager::add( const QChatMessage & message )
{
    messages_.append( QChatMessage( message ) );
}

const QList< QChatMessage > & QMessagesManager::messages() const
{
    return messages_;
}

bool QMessagesManager::save( const QString & logDir ) const
{
    bool result = true;

    QString nowTime = QDateTime::currentDateTime().toString();

    QFile log( logDir + "log[" + nowTime + "].html" );

    int index = 1;

    while( log.exists() )
    {
        ++index;
        log.setFileName( logDir + "log" + nowTime + QString::number( index ) + ".log" );
    }

    result = log.open( QIODevice::WriteOnly );

    if( !result )
        return result;

    QString data;

    data = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset=\"UTF-8\">\r\n"
           "<style>\r\n.service{color:red;}\r\n.nick{color:green;}\r\n.message{color:blue;}\r\n</style>\r\n"
           "</head>\r\n<body>\r\n";
    log.write( data.toUtf8() );

    foreach( const QChatMessage & message, messages_ )
    {
        data = "<span class =\"service\">[" + message.service() + "]</span><span class =\"nick\">" + message.nickName() + "</span>: <span class = \"message\">" + message.message() + "</span><br />\r\n";
        log.write( data.toUtf8() );
    }

    data = "</body>\r\n</html>\r\n";

    log.close();

    return result;
}

void QMessagesManager::addMessage( QChatMessage * message )
{
    assert( message );
    if( message )
        add( *message );
}

