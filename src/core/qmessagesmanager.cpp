#include <cassert>

#include <QApplication>
#include <QFile>
#include <QDateTime>

#include <QDebug>

#include "qmessagesmanager.h"

QMessagesManager::QMessagesManager( QObject * parent )
: QObject( parent )
, messages_()
{
}

QMessagesManager::~QMessagesManager()
{
}

void QMessagesManager::add( ChatMessage message )
{
    messages_.append( ChatMessage( message ) );
}

const QList< ChatMessage > & QMessagesManager::messages() const
{
    return messages_;
}

bool QMessagesManager::save( const QString & logDir ) const
{
    qDebug() << logDir;    

    QString nowTime = QDateTime::currentDateTime().toString();

    QFile log( logDir + "log[" + nowTime + "].html" );

    int index = 1;

    while( log.exists() )
    {
        ++index;
        log.setFileName( logDir + "log" + nowTime + QString::number( index ) + ".log" );
    }

    bool result = log.open( QIODevice::WriteOnly );

    if( !result )
        return result;

    QString data;

    data = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset=\"UTF-8\">\r\n"
           "<style>\r\n.service{color:red;}\r\n.nick{color:green;}\r\n.message{color:blue;}\r\n</style>\r\n"
           "</head>\r\n<body>\r\n";
    log.write( data.toUtf8() );

    foreach( const ChatMessage & message, messages_ )
    {
        data = "<span class =\"service\">[" + message.serviceName() + "]</span><span class =\"nick\">" + message.nickName() + "</span>: <span class = \"message\">" + message.message() + "</span><br />\r\n";
        log.write( data.toUtf8() );
    }

    data = "</body>\r\n</html>\r\n";

    log.close();

    return result;
}
