#ifndef QYOUTUBECHATEX_H
#define QYOUTUBECHATEX_H

#include "../core/qchatservice.h"
#include "youtubecontroller.h"

class QNetworkAccessManager;
class QYoutubeChatEx : public QChatService
{
    Q_OBJECT
public:
    explicit QYoutubeChatEx( QObject * parent = nullptr );
    ~QYoutubeChatEx() override;

    // QChatService interface
public slots:
    void connect() override;
    void disconnect() override;
    void reconnect() override;
private:
    //check user enabled youtube chat provider or not
    bool youtubeChatProviderEnabled();
    QNetworkAccessManager * nam_;   
    youtube::YoutubeController* youtubeController_;
protected slots:
    //yotubeapi
    void error(const QString errorMessage);
    void message(const QString message,const QString authorName,const QString authorImageLogo,
                 bool isChatModerator = false, bool isChatOwner = false,bool isChatSponsor = false);
    void fundraising(const QString userComment,const QString authorName,const QString authorImageLogo
                     ,ulong amountMicros,QString currency,QString amountDisplayString);
    void newSponsor(const QString authorName);

    void doLogin();
    void doDelogin();
    void doFreshLogin();


};

#endif // QYOUTUBECHATEX_H
