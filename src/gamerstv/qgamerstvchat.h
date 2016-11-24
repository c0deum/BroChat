#ifndef QGAMERSTVCHAT
#define QGAMERSTVCHAT

#include <QList>

#include "../core/qchatservice.h"

class QNetworkAccessManager;

class QGamersTvChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QGamersTvChat( QObject * parent = nullptr );
    virtual                 ~QGamersTvChat();
protected:
    void                    timerEvent( QTimerEvent * );

private:
    void                    loadSettings();
    virtual void            loadSmiles();
    void                    loadMessages();

    void                    loadStatistic();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:

    void                    onSmilesLoaded();
    void                    onSmilesLoadError();

    void                    onMessagesLoaded();
    void                    onMessagesLoadError();

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

    void                    changeBadges( bool badges );

private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 channelLink_;
    QString                 lastMessageId_;
    int                     updateMessagesTimerId_ = {-1};
    int                     reconnectTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    bool                    badges_ = {false};

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        UPDATE_INTERVAL;
    static const int        RECONNECT_INTERVAL;
    static const int        STATISTIC_INTERVAL;
};

#endif // QGAMERSTVCHAT

