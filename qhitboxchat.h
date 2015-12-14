#ifndef QHITBOXCHAT
#define QHITBOXCHAT

//#include <QMap>
#include <QList>
#include "qchatsmile.h"
#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;


//class QChatSmile;


class QHitBoxChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QHitBoxChat( QObject * parent = nullptr );
    virtual                 ~QHitBoxChat();

protected:
    virtual void            timerEvent( QTimerEvent *event );
private:
    void                    loadSettings();

    void                    loadServersList();
    void                    loadSocketHash();

    virtual void            loadSmiles();

    void                    loadStatistic();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

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
    QWebSocket *            socket_;
    QString                 channelName_;
    QList< QString >        servers_;

    int                     reconnectTimerId_;
    int                     reconnectInterval_;
    int                     statisticTimerId_;
    int                     statisticInterval_;
    int                     saveConnectionTimerId_;
    int                     saveConnectionInterval_;
    bool                    originalColors_;
};

#endif // QHITBOXCHAT

