#ifndef QACESCHAT_H
#define QACESCHAT_H

#include "qchatservice.h"

class QNetworkAccessManager;

class QAcesChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QAcesChat( QObject * parent = nullptr );
    virtual                 ~QAcesChat();

protected:
    virtual void            timerEvent( QTimerEvent * );

private:
    void                    loadSettings();
    void                    getChannelInfo();
    void                    getLastMessage();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void                    onChannelInfoLoaded();
    void                    onChannelInfoLoadError();

    void                    onLastMessageLoaded();
    void                    onLastMessageLoadError();

    void                    onChatInfoLoaded();
    void                    onChatInfoLoadError();
private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 channelId_;
    int                     lastMessageId_;
    int                     updateChatInfoTimerId_;
    int                     reconnectTimerId_;
    int                     updateChatInfoInterval_;
    int                     reconnectInterval_;
};

#endif // QACESCHAT_H
