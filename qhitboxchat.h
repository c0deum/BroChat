#ifndef QHITBOXCHAT
#define QHITBOXCHAT

//#include <QMap>
#include <QList>
#include "qchatsmile.h"
#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;


//class QChatSmile;


class QHitBoxChat: public QChatService
{
    Q_OBJECT
public:
    explicit QHitBoxChat( QObject *parent = 0 );
    virtual ~QHitBoxChat();
protected:
    virtual void timerEvent( QTimerEvent *event );
private:
    void loadSettings();

//    void getChannelInfo();
    void getServersList();
    void getSocketHash();

    void getSmiles();

    void getStatistic();

    QString insertSmiles( const QString &message );
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();

    void changeOriginalColors( bool originalColors );

private slots:
//    void onChannelInfoLoaded();
//    void onChannelInfoLoadError();

    void onServersListLoaded();
    void onServersListLoadError();

    void onSocketHashLoaded();
    void onSocketHashLoadError();

    void onSmilesLoaded();
    void onSmilesLoadError();

    void onStatisticLoaded();
    void onStatisticLoadError();

    void onTextMessageReceived( const QString &message  );
    void onWebSocketError();

    void onPong( quint64 elapsedTime, const QByteArray & payload );

private:
    QNetworkAccessManager *nam_;
    QWebSocket *socket_;

    QString channelName_;

    QList< QString > servers_;

    //QMap< QString, QString > smiles_;
    QList< QChatSmile > smiles_;

    int reconnectTimerId_;
    int reconnectInterval_;

    int statisticTimerId_;
    int statisticInterval_;

    int saveConnectionTimerId_;
    int saveConnectionInterval_;

    bool originalColors_;
};

#endif // QHITBOXCHAT

