#ifndef QFUNSTREAMCHAT_H
#define QFUNSTREAMCHAT_H

#include <QMap>
#include "qchatsmile.h"

#include "qchatservice.h"


class QNetworkAccessManager;
class QWebSocket;

class QFunStreamChat: public QChatService
{
    Q_OBJECT
public:
    explicit QFunStreamChat( QObject *parent = 0 );
    virtual ~QFunStreamChat();
protected:
    virtual void timerEvent( QTimerEvent * );
private:
    void loadSettings();
    void getChannelInfo();
    void connectToWebClient();
    //void getSmiles();
    QString insertSmiles( const QString &message ) const;

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:

    void onWebSocketConnected();
    void onWebSocketError();
    void onTextMessageRecieved( const QString &message );

    //void onSmilesLoaded();
    //void onSmileLoadError();

    void onChannelInfoLoaded();
    void onChannelInfoLoadError();


    void onPong( quint64 elapsedTime, const QByteArray & payload );
private:
    QNetworkAccessManager *nam_;
    QWebSocket *socket_;
    QString channelName_;
    QString channelId_;
    //QMap< QString, QChatSmile > smiles_;
    QList< QChatSmile > smiles_;
    int saveConnectionTimerId_;
    int reconnectTimerId_;
    int saveConnectionInterval_;
    int reconnectInterval_;
    int lastMessageId_;
};

#endif // QFUNSTREAMCHAT_H

