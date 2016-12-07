#ifndef QBEAMPROCHAT_H
#define QBEAMPROCHAT_H

#include "../core/qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;

class QBeamProChat: public QChatService
{
    Q_OBJECT
public:
    QBeamProChat( QObject * parent = nullptr );
    virtual ~QBeamProChat();
protected:
    virtual void timerEvent( QTimerEvent * event );
private:
    void loadSettings();
    virtual void loadSmiles();

    void loadChannelInfo();

    void loadChatsInfo();

    //void connectToSocketIO();
    void connectToChatWebSocket();



public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:

    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onChatsInfoLoaded();
    void onChatsInfoLoadError();

    void onSmilesLoaded();
    void onSmilesLoadError();

    //void onWebSocketConnected();
    //void onWebSocketConnectError();

    void onChatWebSocketConnected();
    void onChatWebSocketConnectError();

    //void onTextMessageReceived( const QString & message );
    void onChatTextMessageReceived( const QString & message );

private:
    QNetworkAccessManager * nam_ = {nullptr};
    //QWebSocket * socketIO_ = {nullptr};
    QWebSocket * chatSocket_ = {nullptr};
    QString channelName_;
    QString channelId_;
    QStringList chatWebSocketAddrList_;
    int reconnectTimerId_ = {-1};
    int reconnectChatTimerId_= {-1};
    int saveConnectionTimerId_ = {-1};
    int currentChatId_ = {-1};
    int currentRequestId_ = {-1};
    int channelInfoRequestId_ = {-1};
    int chatsInfoRequestId_ = {-1};

    static const QString SERVICE_NAME;
    static const QString SERVICE_USER_NAME;

    static const int RECONNECT_INTERVAL;
    static const int RECONNECT_CHAT_INTERVAL;
    static const int SAVE_CONNECTION_INTERVAL;
};

#endif // QBEAMPROCHAT_H
