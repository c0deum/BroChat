#ifndef QCHATUPDATESERVER
#define QCHATUPDATESERVER

#include <QObject>
#include <QList>

class QWebSocketServer;
class QWebSocket;

class QChatUpdateServer: public QObject
{
Q_OBJECT
public:
    explicit                 QChatUpdateServer( unsigned short port, QObject * parent = nullptr );
                            ~QChatUpdateServer() override;

public slots:
    void                    sendMessage( const QString & message );
    void                    close();

private slots:
    void                    onNewConnection();
    void                    onSocketDisconnected();

private:
    QWebSocketServer *      server_;
    QList< QWebSocket * >   clientsList_;
};

#endif // QCHATUPDATESERVER

