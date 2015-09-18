#ifndef QYOUTUBECHAT
#define QYOUTUBECHAT

#include "qchatservice.h"

class QNetworkAccessManager;

class QYoutubeChat: public QChatService
{
    Q_OBJECT
public:
    explicit QYoutubeChat( QObject * parent = 0 );
    virtual ~QYoutubeChat();
protected:
    virtual void timerEvent( QTimerEvent * event );
private:
    void loadSettings();

    void getChannelInfo();
    void getMessages();
    void getStatistic();

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:
    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onMessagesLoaded();
    void onMessagesLoadError();

    void onStatisticLoaded();
    void onStatisticLoadError();

private:
    QNetworkAccessManager * nam_;
    QString channelName_;

    QString lastMessageTime_;

    int updateMessagesTimerId_;
    int updateMessagesInterval_;

    int updateStatisticTimerId_;
    int updateStatisticInterval_;

    int reconnectTimerId_;
    int reconnectInterval_;
};

#endif // QYOUTUBECHAT

