#ifndef QSC2TVCHAT_H
#define QSC2TVCHAT_H

#include <QList>
#include <QMap>
#include <QTimer>

#include "qchatservice.h"

class QNetworkAccessManager;

class QSc2tvChat: public QChatService
{
    Q_OBJECT
public:
    explicit                    QSc2tvChat( QObject * parent = nullptr );
    virtual                     ~QSc2tvChat();

protected:
    virtual void                timerEvent( QTimerEvent * );

private:
    void                        loadSettings();

    virtual void                loadSmiles();
    void                        loadStyles();

    void                        loadChannelInfo();
    void                        loadMessages();

public slots:
    virtual void                connect();
    virtual void                disconnect();
    virtual void                reconnect();

    //void                        setUpdateMessagesInterval( int interval );
    //void                        setReconnectInterval( int interval );

    void                        changeOriginalColors( bool originalColors );

private slots:
    void                        onSmilesLoaded();
    void                        onSmilesLoadError();

    void                        onStyleLoaded();
    void                        onStylesLoadError();

    void                        onChannelInfoLoaded();
    void                        onChannelInfoLoadError();

    void                        onMessagesLoaded();
    void                        onMessagesLoadError();
private:
    QNetworkAccessManager *     nam_;
    QString                     channelName_;
    QString                     channelLink_;
    QString                     lastMessageId_;
    QMap< QString, QString >    styles_;
    int                         updateMessagesTimerId_ = {-1};
    int                         reconnectTimerId_ = {-1};
    bool                        originalColors_ = {false};

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            UPDATE_INTERVAL;
    static const int            RECONNECT_INTERVAL;
};

#endif // QSC2TVCHAT_H
