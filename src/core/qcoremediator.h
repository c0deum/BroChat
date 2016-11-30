#ifndef QCOREMEDIATOR_H
#define QCOREMEDIATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QList>

#include "qmessagesmodel.h"

class QChatService;

class QAcesChat;
class QAzubuChat;
class QBeamProChat;
class QCyberGameChat;
class QGamersTvChat;
class QGipsyTeamChat;
class QGoodGameChat;
class QHitBoxChat;
class QIgdcChat;
class QLivecodingChat;
class QPeka2Chat;
class QStreamCubeChat;
class QTwitchChat;
class QVidiChat;
class QYoutubeChatEx;

class QSettingsDialog;

class QCoreMediator: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMessagesModel * messagesModel READ messagesModel NOTIFY messagesModelChanged )
protected:
    explicit QCoreMediator( QObject * parent = nullptr );    
public:
    static QCoreMediator & instance();
    static void release();
    //void registerChatService( QSharedPointer< QChatService > service );
    void registerChatService( QChatService* service );
    Q_INVOKABLE QMessagesModel * messagesModel();
public slots:
    Q_INVOKABLE void showSettings();
    void onReconnect();
signals:
    void reconnect();
    void messagesModelChanged( QMessagesModel * model );

    void reconnectAllChats();
    void reconnectAcesChat();
    void reconnectAzubuChat();
    void reconnectBeamproChat();
    void reconnectCybergameChat();
    void reconnectGamerstvChat();
    void reconnectGipsyteamChat();
    void reconnectGoodgameChat();
    void reconnectHitboxChat();
    void reconnectIgdcChat();
    void reconnectLivecodingChat();
    void reconnectPeka2Chat();
    void reconnectStreamcubeChat();
    void reconnectTwitchChat();
    void reconnectVidiChat();
    void reconnectYoutubeChat();


private:
    static QCoreMediator * self_;
    //QList< QSharedPointer< QChatService > > chatServices_;
    QList< QChatService * > chatServices_;
    QMessagesModel model_;

    QAcesChat * acesChat_;
    QAzubuChat * azubuChat_;
    QBeamProChat * beamproChat_;
    QCyberGameChat * cybergameChat_;
    QGamersTvChat * gamerstvChat_;
    QGipsyTeamChat * gipsyteamChat_;
    QGoodGameChat * goodgameChat_;
    QHitBoxChat * hitboxChat_;
    QIgdcChat * igdcChat_;
    QLivecodingChat * livecodingChat_;
    QPeka2Chat * peka2Chat_;
    QStreamCubeChat * streamcubeChat_;
    QTwitchChat * twitchChat_;
    QVidiChat * vidiChat_;
    QYoutubeChatEx * youtubeChat_;

    QSharedPointer< QSettingsDialog > settingsDialog_;
};

template< typename T >
class ChatServiceRegistrator
{
public:
    ChatServiceRegistrator()
    {
        QCoreMediator::instance().registerChatService( QSharedPointer<T>( new T() ) );
    }
};



#endif // QCOREMEDIATOR_H
