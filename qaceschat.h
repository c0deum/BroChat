#ifndef QACESCHAT_H
#define QACESCHAT_H

#include "qchatservice.h"

class QNetworkAccessManager;

class QAcesChat: public QChatService
{
    Q_OBJECT
public:
    explicit QAcesChat( QObject *parent = 0 );
    virtual ~QAcesChat();
protected:
    virtual void timerEvent( QTimerEvent * );
private:
    void loadSettings();
    void getStreamesList();
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:
    void onStreamesListLoaded();
    void onStreamesListLoadError();

    void onChatInfoLoaded();
    void onChatInfoLoadError();

    void changeOriginalColors( bool originalColors );
private:
    QNetworkAccessManager *nam_;
    QString channelName_;
    QString channelId_;
    QString lastMessageDateTime_;
    int updateChatInfoTimerId_;
    int reconnectTimerId_;
    int updateChatInfoInterval_;
    int reconnectInterval_;
    bool originalColors_;
};

#endif // QACESCHAT_H
