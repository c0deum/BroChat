#ifndef QCYBERGAMECHAT_H
#define QCYBERGAMECHAT_H

///#include <QAbstractSocket>
#include <QMap>

#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QCyberGameChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QCyberGameChat( QObject * parent = nullptr );
    virtual                     ~QCyberGameChat();

protected:
    virtual void                timerEvent( QTimerEvent * );

private:
    void                        loadSettings();
    virtual void                loadSmiles();
    void                        loadStatistic();

public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();

private slots:
    void                        onWebSocketConnected();
    void                        onWebSocketError();
    void                        onTextMessageRecieved( const QString & message );

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onStatisticLoaded();
    void                        onStatisticLoadError();

private:
    QNetworkAccessManager *     nam_;
    QWebSocket *                socket_ = {nullptr};
    QString                     channelName_;
    int                         lastUpd_ = {0};
    int                         saveConnectionTimerId_= {-1};
    int                         reconnectTimerId_ = {-1};
    int                         statisticTimerId_ = {-1};

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            SAVE_CONNECTION_INTERVAL;
    static const int            RECONNECT_INTERVAL;
    static const int            STATISTIC_INTERVAL;

};

#endif // QCYBERGAMECHAT_H
