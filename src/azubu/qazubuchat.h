#ifndef QAZUBUCHAT_H
#define QAZUBUCHAT_H

#include "../core/qchatservice.h"

class QNetworkAccessManager;
class QTcpSocket;

class QAzubuChat: public QChatService
{
    Q_OBJECT
public:
    explicit QAzubuChat( QObject * parent = nullptr );
            ~QAzubuChat() override;
protected:
    void timerEvent( QTimerEvent * event ) override;
private:
    void loadSettings();
    void loadChannelInfo();
    void loadSmiles() override;
    void loadStatistic();
    void safeDeleteSocket();

public slots:
    void connect() override;
    void disconnect() override;
    void reconnect() override;
private slots:
    void onChannelInfoLoaded();
    void onChannelInfoLoadError();

    void onSmilesLoaded();
    void onSmilesLoadError();

    void onStatisticLoaded();
    void onStatisticLoadError();

    void onConnected();

    void parseMessage();

    void onSocketError();

private:
    QNetworkAccessManager * nam_ = {nullptr};
    QTcpSocket * socket_ = {nullptr};
    QString channelName_;
    QString channelIrcName_;
    QString randomName_;
    int reconnectTimerId_ = {-1};
    int statisticTimerId_ = {-1};
    int saveConnectionTimerId_ = {-1};

    static const QString SERVICE_NAME;
    static const QString SERVICE_USER_NAME;

    static const int RECONNECT_INTERVAL;
    static const int STATISTIC_INTERVAL;
    static const int SAVE_CONNECTION_INTERVAL;

};

#endif // QAZUBUCHAT_H
