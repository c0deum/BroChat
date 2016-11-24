#ifndef QVIDICHAT_H
#define QVIDICHAT_H

#include "../core/qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QVidiChat: public QChatService
{
    Q_OBJECT
public:
    explicit QVidiChat( QObject * parent = nullptr );
    virtual ~QVidiChat();
public:

protected:
    virtual void            timerEvent( QTimerEvent * );

private:    
    virtual void            loadSmiles();

    void                    loadSettings();

    void                    loadChannelInfo();

    void                    connectToSocket();

    void                    loadStatistic();

    //void                    loadSid();

    //void                    connectToJoinSocket();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();
private slots:
    void                    onChannelInfoLoaded();
    void                    onChannelinfoLoadError();

//    void                    onSidLoaded();
//    void                    onSidLoadError();

    void                    onSmilesLoaded();
    void                    onSmilesLoadError();

    void                    onSocketConnected();
    void                    onSocketConnetError();

    void                    onSocketMessageReceived( const QString & message );

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

//    void                    onJoinSocketConnected();
//    void                    onJoinSocketConnectError();

//    void                    onJoinSocketMessageReceived( const QString & message );




private:
    QNetworkAccessManager * nam_;
    //QWebSocket *            joinSocket_ = {nullptr};
    QWebSocket *            socket_ = {nullptr};
    QString                 channelName_;
    QString                 channelId_;    
    //QString                 sid_;
    int                     reconnectTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    //int                     saveJoinSocketConnectionTimerId_ = {-1};
    int                     saveSocketConnectionTimerId_ = {-1};

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    //static const int        SAVE_JOIN_SOCKET_CONNECTION_INTERVAL;
    static const int        SAVE_SOCKET_CONNECTION_INTERVAL;
    static const int        RECONNECT_INTERVAL;
    static const int        STATISTIC_INTERVAL;
};

#endif // QVIDICHAT_H
