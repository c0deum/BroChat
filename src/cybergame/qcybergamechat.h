#ifndef QCYBERGAMECHAT_H
#define QCYBERGAMECHAT_H

#include "../core/qchatservice.h"

class QNetworkAccessManager;

class QWebSocket;

class QCyberGameChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QCyberGameChat( QObject * parent = nullptr );
    virtual                     ~QCyberGameChat();
private:
    void                        loadSettings();


    void                        loadChannelInfo();
    virtual void                loadSmiles();
    void                        loadStatistic();

    void                        connectToWebSocket();



protected:
    void                        timerEvent( QTimerEvent * event );
public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();
private slots:
    void                        onWebSocketConnected();
    void                        onWebSocketError();

    void                        onTextMessageReceived( const QString & message );

    void                        onChannelInfoLoaded();
    void                        onChannelInfoLoadError();

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onStatisticLoaded();
    void                        onStatisticLoadError();

    void                        changeBadges( bool badges );

private:
    QNetworkAccessManager *     nam_ = {nullptr};
    QWebSocket *                socket_ = {nullptr};

    QString                     channelName_;
    QString                     channelId_;

    int                         reconnectTimerId_ = {-1};
    int                         statisticTimerId_ = {-1};
    int                         saveConnectionTimerId_ = {-1};

    bool                        badges_ = {false};

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            RECONNECT_INTERVAL;
    static const int            STATISTIC_INTERVAL;
    static const int            SAVE_CONNECTION_INTERVAL;
};

#endif // QCYBERGAMECHAT_H
