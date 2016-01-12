#ifndef QGIPSYTEAMCHAT
#define QGIPSYTEAMCHAT

#include "qchatservice.h"

class QNetworkAccessManager;

class QGipsyTeamChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QGipsyTeamChat( QObject * parent = nullptr );
    virtual                 ~QGipsyTeamChat();

protected:
    virtual void            timerEvent( QTimerEvent * event );

private:
    void                    loadSettings();    

    void                    loadChannelInfo();

    void                    loadMessages();

public slots:
    virtual void            connect();
    virtual void            disconnect();
    virtual void            reconnect();

private slots:
    void                    onChannelInfoLoaded();
    void                    onChannelInfoLoadError();

    void                    onMessagesLoaded();
    void                    onMessagesLoadError();

private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 channelLink_;
    QString                 lastMessageId_;

    int                     updateMessagesTimerId_ = {-1};
    int                     reconnectTimerId_ = {-1};


    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        UPDATE_INTERVAL;
    static const int        RECONNECT_INTERVAL;

};

#endif // QGIPSYTEAMCHAT

