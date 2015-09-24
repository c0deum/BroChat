#ifndef QREALLTVCHAT
#define QREALLTVCHAT

#include <QDateTime>
#include <QMap>

#include <QXmppLogger.h>

#include <QXmppClient.h>

#include "qchatservice.h"
#include "qchatsmile.h"

class QNetworkAccessManager;

class QXmppMessage;
class QXmppPresence;
class QXmppIq;

class QXmppMucManager;


class QReallTvChat: public QChatService
{
    Q_OBJECT
public:
    explicit QReallTvChat( QObject *parent = 0 );
    virtual ~QReallTvChat();
private:
    void loadSettings();

    void getChannelInfo();

    void getSmiles();
    void getStatistic();

    QString insertSmiles( const QString &message ) const;

protected:
    void timerEvent( QTimerEvent *event );
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:
    void onConnected();
    void onError( QXmppClient::Error );

    void onMessageReceived( const QXmppMessage &message );

    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onSmilesLoaded();
    void onSmilesLoadError();

    void onStatisticLoaded();
    void onStatisticLoadError();

private:
    QNetworkAccessManager *nam_;
    QXmppClient *xmppClient_;
    QXmppMucManager *mucManager_;
    QString channelName_;
    QString cid_;
    QDateTime connectionTime_;
    QMap<QString, QChatSmile> smiles_;
    int reconnectTimerId_;
    int reconnectInterval_;
    int statisticTimerId_;
    int statisticInterval_;
};

#endif // QREALLTVCHAT

