#ifndef QSTREAMBOXCHAT
#define QSTREAMBOXCHAT

#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QStreamBoxChat: public QChatService
{
    Q_OBJECT
public:
    explicit QStreamBoxChat( QObject *parent = 0 );
    virtual ~QStreamBoxChat();
protected:
    virtual void timerEvent( QTimerEvent *event );
private:
    void loadSettings();
    QString insertSmiles( const QString& message );

    void getStatistic();

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:
    void onWebSocketConnected();
    void onTextMessageReceived( const QString &message );
    void onWebSocketError();

    void onStatisticLoaded();
    void onStatisticLoadError();

    void onPong( quint64 elapsedTime, const QByteArray & payload );

private:
    QNetworkAccessManager *nam_;
    QWebSocket *socket_;
    QString channelName_;
    int reconnectTimerId_;
    int reconnectInterval_;
    int statisticTimerId_;
    int statisticInterval_;
    int saveConnectionTimerId_;
    int saveConnectionInterval_;
};

#endif // QSTREAMBOXCHAT


