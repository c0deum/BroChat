#ifndef QLIVECODINGCHAT
#define QLIVECODINGCHAT

#include <QMap>
#include <QDateTime>
#include <QXmppClient.h>

#include "qchatservice.h"

class QNetworkAccessManager;

class QXmppMucManager;
class QXmppMessage;

class QWebSocket;

class QLivecodingChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QLivecodingChat( QObject * parent = nullptr );
    virtual                     ~QLivecodingChat();
private:
    void                        loadSettings();

    //sid for websocket statistic connection
    void                        loadSid();

    //join afte 3probe received from livecoding
    //void                        joinToChannel();

    virtual void                loadSmiles();

//    void                        loadStatistic();

    void                        connectToWebSocket();
    void                        disconnectFromWebSocket();
    void                        reconnectToWebSocket();

protected:
    virtual void                timerEvent( QTimerEvent * event );
public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();
private slots:
    void                        onConnected();
    void                        onError( QXmppClient::Error );
    void                        onMessageReceived( const QXmppMessage & message );

    void                        onSidLoaded();
    void                        onSidLoadError();

//    void                        onJoinToChannelReplyLoaded();
//    void                        onJoinToChannelReplyLoadError();

    void                        onWebSocketConnected();
    void                        onWebSocketError();
    void                        onWebSocketMessageReceived( const QString & message );

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

//    void                        onStatisticLoaded();
//    void                        onStatisticLoadError();

private:
    QNetworkAccessManager *     nam_;
    QXmppClient *               xmppClient_ = {nullptr};
    QXmppMucManager *           mucManager_ = {nullptr};
    QWebSocket *                socket_ = {nullptr};
    QString                     sid_;
    QString                     channelName_;
    QString                     login_;
    QString                     password_;
    QDateTime                   connectionTime_;
    int                         reconnectTimerId_ = {-1};
    int                         reconnectWebSocketTimerId_ = {-1};
    int                         saveWebSocketConnectionTimerId_ = {-1};
//    int                         statisticTimerId_;
//    int                         statisticInterval_;

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            RECONNECT_INTERVAL;
    static const int            RECONNECT_WEB_SOCKET_INTERVAL;
    static const int            SAVE_WEB_SOCKET_CONNECTION_INTERVAL;
};

#endif // QLIVECODINGCHAT

