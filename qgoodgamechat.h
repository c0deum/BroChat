#ifndef QGOODGAMECHAT_H
#define QGOODGAMECHAT_H

#include <QMap>

#include "qchatservice.h"

class QNetworkAccessManager;
class QWebSocket;


class QGoodGameChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QGoodGameChat( QObject * parent = nullptr );
    virtual                     ~QGoodGameChat();

protected:
    virtual void                timerEvent( QTimerEvent * );

private:
    void                        loadSettings();

    void                        loadSmiles();
    void                        loadChannelInfo();
    void                        connectToWebClient();

    void                        loadStatistic();

    QString                     insertSmiles( const QString & message ) const;

public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();

    void                        changeUseAnimatedSmiles( bool use );

private slots:
    void                        onChannelInfoLoaded();
    void                        onChannelInfoLoadError();

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onTextMessageRecieved( const QString & message );
    void                        onWebSocketError();

    void                        onStatisticLoaded();
    void                        onStatisticLoadError();

private:
    QNetworkAccessManager *     nam_;
    QWebSocket *                socket_;
    QString                     channelName_;
    QString                     channelId_;
    int                         lastTimeStamp_;
    QMap< QString, QString >    smiles_;
    QMap< QString, QString >    animatedSmiles_;
    int                         saveConnectionTimerId_;
    int                         reconnectTimerId_;
    int                         saveConnectionInterval_;
    int                         reconnectInterval_;
    int                         statisticTimerId_;
    int                         statisticTimerInterval_;
    bool                        useAnimatedSmiles_;
};

#endif // QGOODGAMECHAT_H
