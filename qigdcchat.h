#ifndef QIGDCCHAT
#define QIGDCCHAT

#include "qchatservice.h"

class QNetworkAccessManager;

class QIgdcChat: public QChatService
{
    Q_OBJECT
public:
    explicit QIgdcChat( QObject * parent = 0 );
    virtual ~QIgdcChat();
protected:
    virtual void timerEvent( QTimerEvent * event );
private:
    void loadSettings();

    void loadChannelInfo();

    void loadMessages();

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:

    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onMessagesLoaded();
    void onMessagesLoadError();

private:
    QNetworkAccessManager *nam_;
    QString channelName_;
    QString channelId_;
    QString lastMessageId_;
    int updateMessagesTimerId_;
    int updateMessagesInterval_;
    int reconnectTimerId_;
    int reconnectInterval_;
};

#endif // QIGDCCHAT

