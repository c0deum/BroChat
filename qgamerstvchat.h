#ifndef QGAMERSTVCHAT
#define QGAMERSTVCHAT

#include <QList>

#include "qchatsmile.h"
#include "qchatservice.h"


class QNetworkAccessManager;

class QGamersTvChat: public QChatService
{
    Q_OBJECT
public:
    explicit QGamersTvChat( QObject *parent = 0 );
    virtual ~QGamersTvChat();
protected:
    void timerEvent( QTimerEvent * );
private:
    void loadSettings();
    void loadSmiles();

    void loadMessages();

    void getStatistic();

    QString insertSmiles( const QString &message );

public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
private slots:

    void onSmilesLoaded();
    void onSmilesLoadError();

    void onMessagesLoaded();
    void onMessagesLoadError();

    void onStatisticLoaded();
    void onStatisticLoadError();

private:
    QNetworkAccessManager *nam_;
    QString channelName_;
    QString channelLink_;
    QString lastMessageId_;
    QList< QChatSmile > smiles_;
    int updateMessagesTimerId_;
    int reconnectTimerId_;
    int updateMessagesInterval_;
    int reconnectInterval_;
    int statisticTimerId_;
    int statisticInterval_;
};

#endif // QGAMERSTVCHAT

