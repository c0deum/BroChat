#ifndef QGAMERSTVCHAT
#define QGAMERSTVCHAT

#include <QList>

#include "../core/qchatservice.h"

class QNetworkAccessManager;

class QGamersTvChat: public QChatService
{
    Q_OBJECT
public:
    explicit                QGamersTvChat( QObject * parent = nullptr );
                            ~QGamersTvChat() override;
protected:
    void                    timerEvent( QTimerEvent * );

private:
    void                    loadSettings();
    void                    loadSmiles() override;
    void                    loadMessages();

    void                    loadStatistic();

public slots:
    void                    connect() override;
    void                    disconnect() override;
    void                    reconnect() override;

private slots:

    void                    onSmilesLoaded();
    void                    onSmilesLoadError();

    void                    onMessagesLoaded();
    void                    onMessagesLoadError();

    void                    onStatisticLoaded();
    void                    onStatisticLoadError();

    void                    changeBadges( bool badges );

private:
    QNetworkAccessManager * nam_;
    QString                 channelName_;
    QString                 channelLink_;
    QString                 lastMessageId_;
    int                     updateMessagesTimerId_ = {-1};
    int                     reconnectTimerId_ = {-1};
    int                     statisticTimerId_ = {-1};
    bool                    badges_ = {false};

    static const QString    SERVICE_NAME;
    static const QString    SERVICE_USER_NAME;

    static const int        UPDATE_INTERVAL;
    static const int        RECONNECT_INTERVAL;
    static const int        STATISTIC_INTERVAL;
};

#endif // QGAMERSTVCHAT

