#ifndef QGAMERSTVCHAT
#define QGAMERSTVCHAT

#include <QList>

#include "qchatsmile.h"
#include "qchatservice.h"


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

private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 channelLink_;
    QString                 lastMessageId_;
    int                     updateMessagesTimerId_;
    int                     reconnectTimerId_;
    int                     updateMessagesInterval_;
    int                     reconnectInterval_;
    int                     statisticTimerId_;
    int                     statisticInterval_;
};

#endif // QGAMERSTVCHAT

