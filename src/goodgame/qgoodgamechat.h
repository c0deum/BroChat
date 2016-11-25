#ifndef QGOODGAMECHAT_H
#define QGOODGAMECHAT_H

#include <QMap>

#include "../core/qchatservice.h"

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
void                            loadSmiles();

    void                        loadChannelInfo();
    void                        connectToWebClient();

    void                        loadStatistic();


    //new parsing functions

    void                        parseSmiles(const QVariantList& smiles);

    QStringList                 parseTokensKeepWhitespaces(QString string) const;
    QString                     replaceSmiles(const QString& token) const;
    QString                     replaceSmile(const QString& token) const;


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

    void                        changeBadges( bool badges );

private:
    QNetworkAccessManager *     nam_;
    QWebSocket *                socket_ = {nullptr};
    QString                     channelName_;
    QString                     channelId_;
    int                         lastTimeStamp_ = {0};
    QMap< QString, QString >    smiles_;
    QMap< QString, QString >    animatedSmiles_;
    int                         saveConnectionTimerId_ = {-1};
    int                         reconnectTimerId_ = {-1};
    int                         statisticTimerId_ = {-1};
    bool                        useAnimatedSmiles_ = {false};
    bool                        badges_ = {false};

    static const QString        SERVICE_NAME;
    static const QString        SERVICE_USER_NAME;

    static const int            SAVE_CONNECTION_INTERVAL;
    static const int            RECONNECT_INTERVAL;
    static const int            STATISTIC_INTERVAL;
};

#endif // QGOODGAMECHAT_H
