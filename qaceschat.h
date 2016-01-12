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
    void                    loadChannelInfo();
    void                    loadLastMessage();

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
    int                     lastMessageId_ = {-1};
    int                     updateChatInfoTimerId_ = {-1};
    //int                     updateChatInfoInterval_;
    int                     reconnectTimerId_ = {-1};
    //int                     reconnectInterval_;

    static const QString    SERVICE_USER_NAME;
    static const QString    SERVICE_NAME;

    static const int        UPDATE_INTTERVAL;
    static const int        RECONNECT_INTERVAL;
};

#endif // QACESCHAT_H
