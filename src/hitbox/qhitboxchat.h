#ifndef QHITBOXCHAT
#define QHITBOXCHAT

//#include <QMap>
#include <QList>

#include "../core/qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QHitBoxChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QHitBoxChat( QObject * parent = nullptr );
                            ~QHitBoxChat() override;

protected:
    void                    timerEvent( QTimerEvent *event ) override;
private:
    void                    loadSettings();

    void                    loadServersList();
    void                    loadSocketHash();

    void                    loadSmiles() override;

    void                    loadStatistic();

public slots:
    void                    connect() override;
    void                    disconnect() override;
    void                    reconnect() override;

    void                    changeOriginalColors( bool originalColors );

private slots:
    void                    onServersListLoaded();
    void                    onServersListLoadError();

    void                    onSocketHashLoaded();
    void                    onSocketHashLoadError();

    void                    onSmilesLoaded();
    void                    onSmilesLoadError();

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

    void                    onTextMessageReceived( const QString & message  );
    void                    onWebSocketError();

private:
    QNetworkAccessManager * nam_;
    QWebSocket *            socket_ = {nullptr};
    QString                 channelName_;
    QList< QString >        servers_;

    int                     reconnectTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    int                     saveConnectionTimerId_ = {-1};
    bool                    originalColors_ = {false};

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        RECONNECT_INTERVAL;
    static const int        STATISTIC_INTERVAL;
    static const int        SAVE_CONNECTION_INTERVAL;
};

#endif // QHITBOXCHAT

