#ifndef QSTREAMCUBECHAT_H
#define QSTREAMCUBECHAT_H

#include "..\core\qchatservice.h"

class QNetworkAccessManager;

class QStreamCubeChat: public QChatService
{
    Q_OBJECT
public:
    QStreamCubeChat( QObject * parent = nullptr );
    virtual ~QStreamCubeChat();
protected:
    virtual void timerEvent( QTimerEvent * event );
private:
    void loadSettings();

    void loadChannelInfo();

    virtual void loadSmiles();

    void loadMessages();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onSmilesLoaded();
    void onSmilesLoadError();

    void onMessagesLoaded();
    void onMessagesLoadError();

private:
    QNetworkAccessManager * nam_ = {nullptr};
    QString channelName_;
    QString channelId_;
    QString lastMessageId_;
    int updateTimerId_ = {-1};
    int reconnectTimerId_ = {-1};

    static const QString    SERVICE_USER_NAME;
    static const QString    SERVICE_NAME;

    static const int        UPDATE_INTERVAL;
    static const int        RECONNECT_INTERVAL;
};

#endif // QSTREAMCUBECHAT_H
