#ifndef QFUNSTREAMCHAT_H
#define QFUNSTREAMCHAT_H

#include <QMap>
#include "qchatsmile.h"

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

    void                    parseMessage( const QJsonObject & jsonObj );

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void                    onWebSocketConnected();
    void                    onWebSocketError();
    void                    onTextMessageRecieved( const QString & message );

    void                    onSmilesLoaded();
    void                    onSmileLoadError();

    void                    onChannelInfoLoaded();
    void                    onChannelInfoLoadError();

private:
    QNetworkAccessManager * nam_;
    QWebSocket *            socket_;
    QString                 channelName_;
    QString                 channelId_;
    int                     reconnectTimerId_;
    int                     reconnectInterval_;
    int                     saveConnectionTimerId_;
    int                     saveConnectionInterval_;
    int                     statisticTimerId_;
    int                     statisticInterval_;
    int                     lastMessageId_;
    int                     requestId_;
    int                     joinRequestId_;
    int                     statisticRequestId_;
    int                     historyRequestId_;
    int                     historyLastMessageId_;
};

#endif // QFUNSTREAMCHAT_H

