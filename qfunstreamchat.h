#ifndef QFUNSTREAMCHAT_H
#define QFUNSTREAMCHAT_H

#include <QMap>

#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QFunStreamChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QFunStreamChat( QObject * parent = nullptr );
    virtual                 ~QFunStreamChat();
protected:
    virtual void            timerEvent( QTimerEvent * );
private:
    void                    loadSettings();
    void                    loadChannelInfo();
    void                    connectToWebClient();
    void                    loadStatistic();
    void                    loadHistory();    
    virtual void            loadSmiles();

    void                    loadTwitchSmiles();

    void                    loadStyles();

    void                    loadBadges();

    void                    parseMessage( const QJsonObject & jsonObj );

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

    void                    changeOriginalColors( bool originalColors );
    void                    changeBadges( bool badges );

private slots:
    void                    onWebSocketConnected();
    void                    onWebSocketError();
    void                    onTextMessageRecieved( const QString & message );

    void                    onSmilesLoaded();
    void                    onSmileLoadError();

    void                    onStylesLoaded();
    void                    onStylesLoadError();

    void                    onChannelInfoLoaded();
    void                    onChannelInfoLoadError();

    void                    onBadgesLoaded();
    void                    onBadgesLoadError();

    void                    onTwitchSmilesLoaded();
    void                    onTwitchSmilesLoadError();

private:
    QNetworkAccessManager * nam_;
    QWebSocket *            socket_ = {nullptr};
    QString                 channelName_;
    QString                 channelId_;
    int                     reconnectTimerId_ = {-1};
    int                     saveConnectionTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    int                     lastMessageId_ = {0};
    int                     requestId_ = {-1};
    int                     joinRequestId_ = {-1};
    int                     statisticRequestId_ = {-1};
    int                     historyRequestId_ = {-1};
    int                     historyLastMessageId_ = {0};
    QMap<int, QString>      styles_;
    bool                    originalColors_ = {false};
    bool                    badges_ = {false};
    QMap<int, QString>      badgesMap_;

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        RECONNECT_INTERVAL;
    static const int        SAVE_CONNECTION_INTERVAL;
    static const int        STATISTIC_INTERVAL;
};

#endif // QFUNSTREAMCHAT_H

