#ifndef QCHATSERVER
#define QCHATSERVER

#include <QTcpServer>
#include <QTcpSocket>

class QChatServer: public QTcpServer
{
Q_OBJECT
public:
    explicit QChatServer( QObject *parent = 0 );
    ~QChatServer();
public:
    void incomingConnection( int handle );
public slots:
    void onReadyRead();
    void onDisconnected();
    void setStyle( const QString& style );
private:
    QString style_;
};

#endif // QCHATSERVER

