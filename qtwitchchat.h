#ifndef QTWITCHCHAT_H
#define QTWITCHCHAT_H

//#include <QList>
#include <QMap>

#include "qchatsmile.h"

#include "qchatservice.h"

class QTcpSocket;
class QNetworkAccessManager;
class QNetworkReply;

class QTwitchChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QTwitchChat( QObject * parent = nullptr );
    virtual                     ~QTwitchChat();

    void                        loadSettings();
    QString                     emotIcon( const QString & smileName );

private:
    void                        loadSelf();
    virtual void                loadSmiles();
    void                        loadBadges();

    void                        loadStatistic();

    void                        safeDeleteSocket();

protected:
    virtual void                timerEvent( QTimerEvent * );

public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();

    void                        changeOriginalColors( bool originalColors );

private slots:
    void                        onConnected();

    void                        parseMessage();

    void                        onSocketError();

    void                        onSelfLoaded();
    void                        onSelfLoadError();

    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onBadgesLoaded();
    void                        onBadgesLoadError();

    void                        onStatisticLoaded();
    void                        onStatisticLoadError();

private:
    QTcpSocket *                socket_;
    QNetworkAccessManager *     nam_;
    QString                     oauthString_;
    QString                     nickName_;
    QString                     channelName_;
    QString                     selfLink_;
    QString                     emotIconsLink_;
    QString                     badgesLink_;

    int                         reconnectTimerId_;
    int                         reconnectInterval_;

    int                         statisticTimerId_;
    int                         statisticInterval_;

    int                         saveConnectionTimerId_;
    int                         saveConnectionInterval_;

    bool                        originalColors_;
};

#endif // QTWITCHCHAT_H
