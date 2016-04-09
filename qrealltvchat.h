#ifndef QREALLTVCHAT
#define QREALLTVCHAT

#include <QDateTime>
#include <QMap>

#include <QXmppLogger.h>

#include <QXmppClient.h>

#include "qchatservice.h"

class QNetworkAccessManager;

class QXmppMessage;
class QXmppPresence;
class QXmppIq;

class QXmppMucManager;


class QReallTvChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QReallTvChat( QObject * parent = nullptr );
    virtual                     ~QReallTvChat();
private:
    void                        loadSettings();

    void                        loadChannelInfo();
    virtual void                loadSmiles();
    void                        loadStatistic();

protected:
    void                        timerEvent( QTimerEvent * event );
public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();
private slots:
    void                        onConnected();
    void                        onError( QXmppClient::Error error );

    void                        onMessageReceived( const QXmppMessage & message );

    void                        onChannelInfoLoaded();
    void                        onChannelInfoLoadError();

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onStatisticLoaded();
    void                        onStatisticLoadError();

    void                        changeBadges( bool badges );

private:
    QNetworkAccessManager *     nam_;
    QXmppClient *               xmppClient_ = {nullptr};
    QXmppMucManager *           mucManager_ = {nullptr};
    QString                     channelName_;
    QString                     cid_;
    QDateTime                   connectionTime_;
    int                         reconnectTimerId_ = {-1};
    int                         statisticTimerId_ = {-1};
    bool                        badges_ = {false};

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            RECONNECT_INTERVAL;
    static const int            STATISTIC_INTERVAL;
};

#endif // QREALLTVCHAT

