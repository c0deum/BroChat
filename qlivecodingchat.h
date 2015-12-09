#ifndef QLIVECODINGCHAT
#define QLIVECODINGCHAT

#include <QMap>
#include <QDateTime>
#include <QXmppClient.h>

#include "qchatservice.h"
#include "qchatsmile.h"

class QNetworkAccessManager;

class QXmppMucManager;
class QXmppMessage;

class QWebSocket;

class QLivecodingChat: public QChatService
{
    Q_OBJECT
public:
    explicit QLivecodingChat( QObject * parent = 0 );
    virtual ~QLivecodingChat();
private:
    void loadSettings();

    //sid for websocket statistic connection
    void getSid();

    //join afte 3probe received from livecoding
    void joinToChannel();

    void getSmiles();

    void connectToWebSocket();
    void disconnectFromWebSocket();

    void reconnectToWebSocket();

    QString insertSmiles( const QString &message ) const;

protected:
    virtual void timerEvent( QTimerEvent * event );
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:
    void onConnected();
    void onError( QXmppClient::Error );
    void onMessageReceived( const QXmppMessage &message );

    void onSidLoaded();
    void onSidLoadError();

    void onJoinToChannelReplyLoaded();
    void onJoinToChannelReplyLoadError();


    void onWebSocketConnected();
    void onWebSocketError();
    void onWebSocketMessageReceived( const QString & message );
    //void onWebSocketPong();

    void onSmilesLoaded();
    void onSmilesLoadError();

private:
    QNetworkAccessManager * nam_;
    QXmppClient * xmppClient_;
    QXmppMucManager *mucManager_;
    QWebSocket * socket_;
    QString sid_;
    QString channelName_;
    QString login_;
    QString password_;
    QDateTime connectionTime_;
    QMap<QString, QChatSmile> smiles_;
    int reconnectTimerId_;
    int reconnectInterval_;
    int reconnectWebSocketTimerId_;
    int reconnectWebSocketInterval_;
    int saveWebSocketConnectionTimerId_;
    int saveWebSocketConnectionInterval_;
};

#endif // QLIVECODINGCHAT

