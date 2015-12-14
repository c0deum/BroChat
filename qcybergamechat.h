#ifndef QCYBERGAMECHAT_H
#define QCYBERGAMECHAT_H

///#include <QAbstractSocket>
#include <QMap>

#include "qchatsmile.h"

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
    QWebSocket *                socket_;
    QString                     channelName_;
    int                         lastUpd_;
    int                         saveConnectionTimerId_;
    int                         reconnectTimerId_;
    int                         saveConnectionInterval_;
    int                         reconnectInterval_;
    int                         statisticTimerId_;
    int                         statisticInterval_;
};

#endif // QCYBERGAMECHAT_H
