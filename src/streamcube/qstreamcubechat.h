#ifndef QSTREAMCUBECHAT_H
#define QSTREAMCUBECHAT_H

#include "../core/qchatservice.h"

class QNetworkAccessManager;

class QStreamCubeChat: public QChatService
{
    Q_OBJECT
public:
    QStreamCubeChat( QObject * parent = nullptr );
    ~QStreamCubeChat() override;
protected:
    void timerEvent( QTimerEvent * event ) override;
private:
    void loadSettings();

    void loadChannelInfo();

    void loadSmiles() override;

    void loadMessages();

public slots:
    void            connect() override;
    void            disconnect() override;
    void            reconnect() override;

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
