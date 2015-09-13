#include <QWebSocketServer>
#include <QWebSocket>

#include "qchatupdateserver.h"

QChatUpdateServer::QChatUpdateServer( unsigned short port, QObject *parent )
: QObject( parent )
, server_( new QWebSocketServer( "BroChat Update Server", QWebSocketServer::NonSecureMode, this ) )
, clientsList_()
{
    if( server_->listen( QHostAddress::Any, port ) )
    {
        qDebug() << "BroChat Update Server listening on port" << port;
        connect( server_, SIGNAL( newConnection() ), this, SLOT( onNewConnection() ) );
    }
}

QChatUpdateServer::~QChatUpdateServer()
{
    server_->close();
}

void QChatUpdateServer::onNewConnection()
{
    QWebSocket *socket = server_->nextPendingConnection();

    //connect( socket, SIGNAL( textMessageReceived( const QString& ) ), this, SLOT( onTextMessageRecieved( const QString & ) ) );
    connect( socket, SIGNAL( disconnected() ), this, SLOT( onSocketDisconnected() ) );

    clientsList_ << socket;
}

/*
void QChatUpdateServer::onTextMessageRecieved( const QString& message )
{
}
*/

void QChatUpdateServer::onSocketDisconnected()
{
    QWebSocket *socket = qobject_cast< QWebSocket * >( sender() );
    if( socket )
    {
        clientsList_.removeAll( socket );
        socket->deleteLater();

        qDebug() << "websocket removed";
    }
}

void QChatUpdateServer::sendMessage( const QString& message )
{
    foreach( QWebSocket *socket, clientsList_ )
    {
        if( socket && socket->state() == QAbstractSocket::ConnectedState )
        {
            socket->sendTextMessage( message );
        }
    }
}

void QChatUpdateServer::close()
{
    server_->close();
}
