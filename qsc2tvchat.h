#ifndef QSC2TVCHAT_H
#define QSC2TVCHAT_H

#include <QList>
#include <QMap>
#include <QTimer>

#include "qchatsmile.h"

#include "qchatservice.h"

class QNetworkAccessManager;

class QSc2tvChat: public QChatService
{
    Q_OBJECT
public:
    explicit QSc2tvChat( QObject *parent = 0 );
    virtual ~QSc2tvChat();
protected:
    virtual void timerEvent( QTimerEvent * );
private:
    void loadSettings();

    void getSmiles();
    void getStyles();


    //void getLastMessageId();

    void getChannelInfo();
    void getMessages();

    QString insertSmiles( const QString &message ) const;
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();

    void setUpdateMessagesInterval( int interval );
    void setReconnectInterval( int interval );

    void changeOriginalColors( bool originalColors );

private slots:
    void onSmilesLoaded();
    void onSmilesLoadError();

    void onStyleLoaded();
    void onStylesLoadError();

    //void onLastMessageIdLoaded();
    //void onLastMessageIdLoadError();

    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onMessagesLoaded();
    void onMessagesLoadError();
private:
    QNetworkAccessManager *nam_;
    QString channelName_;
    QString channelLink_;
    //QString lastMessageTime_;
    //QString lastMainMessageId_;
    QString lastMessageId_;
    QList< QChatSmile > smiles_;
    QMap< QString, QString > styles_;
    int updateMessagesTimerId_;
    int reconnectTimerId_;
    int updateMessagesInterval_;
    int reconnectInterval_;
    bool originalColors_;
};

#endif // QSC2TVCHAT_H
