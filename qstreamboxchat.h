#ifndef QSTREAMBOXCHAT
#define QSTREAMBOXCHAT

#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QStreamBoxChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QStreamBoxChat( QObject * parent = nullptr );
    virtual                 ~QStreamBoxChat();
protected:
    virtual void            timerEvent( QTimerEvent * event );
private:
    void                    loadSettings();

    void                    loadStatistic();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void                    onWebSocketConnected();
    void                    onTextMessageReceived( const QString & message );
    void                    onWebSocketError();

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

    void                    changeBadges( bool badges );

private:
    QNetworkAccessManager * nam_;
    QWebSocket *            socket_ = {nullptr};
    QString                 channelName_;
    int                     reconnectTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    int                     saveConnectionTimerId_ = {-1};
    bool                    badges_ = {false};
    QMap< int ,QString>     badgesMap_;

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        RECONNECT_INTERVAL;
    static const int        STATISTIC_INTERVAL;
    static const int        SAVE_CONNECTION_INTERVAL;
};

#endif // QSTREAMBOXCHAT


