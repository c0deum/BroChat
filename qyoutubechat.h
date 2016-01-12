#ifndef QYOUTUBECHAT
#define QYOUTUBECHAT

#include "qchatservice.h"

class QNetworkAccessManager;

class QYoutubeChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QYoutubeChat( QObject * parent = nullptr );
    virtual                 ~QYoutubeChat();

protected:
    virtual void            timerEvent( QTimerEvent * event );

private:
    void                    loadSettings();

    void                    loadChannelInfo();
    void                    loadMessages();
    void                    loadStatistic();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void                    onChannelInfoLoaded();
    void                    onChannelInfoLoadError();

    void                    onMessagesLoaded();
    void                    onMessagesLoadError();

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 lastMessageTime_;
    int                     updateMessagesTimerId_ = {-1};
    int                     updateStatisticTimerId_ = {-1};
    int                     reconnectTimerId_ = {-1};

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        UPDATE_INTERVAL;
    static const int        STATISTIC_INTERVAL;
    static const int        RECONNECT_INTERVAL;
};

#endif // QYOUTUBECHAT

