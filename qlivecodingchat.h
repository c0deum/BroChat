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

class QLivecodingChat: public QChatService
{
    Q_OBJECT
public:
    explicit QLivecodingChat( QObject * parent = 0 );
    virtual ~QLivecodingChat();
private:
    void loadSettings();

    void getSmiles();

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


    void onSmilesLoaded();
    void onSmilesLoadError();

private:
    QNetworkAccessManager * nam_;
    QXmppClient * xmppClient_;
    QXmppMucManager *mucManager_;
    QString channelName_;
    QDateTime connectionTime_;
    QMap<QString, QChatSmile> smiles_;
    int reconnectTimerId_;
    int reconnectInterval_;
    int statisticTimerId_;
    int statisticInterval_;
};

#endif // QLIVECODINGCHAT

