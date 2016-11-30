#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include <QtQml>

#include "qcoremediator.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    QApplication::setOrganizationName( "c0deum" );
    QApplication::setApplicationName( "BroChat" );

    QQmlApplicationEngine engine;    

    engine.rootContext()->setContextProperty( "coreMediator", &QCoreMediator::instance() );

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );

    QObject *rootItem = engine.rootObjects().first();

    QObject::connect( rootItem, SIGNAL( showSettingsTriggered() ), &QCoreMediator::instance(), SLOT( showSettings() ) );
    QObject::connect( rootItem, SIGNAL( reconnectAllChatsTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnect() ) );
    QObject::connect( rootItem, SIGNAL( reconnectAcesChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectAcesChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectAzubuChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectAzubuChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectBeamproChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectBeamproChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectCybergameChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectCybergameChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectGamerstvChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectGamerstvChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectGipsyteamChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectGipsyteamChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectGoodgameChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectGoodgameChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectHitboxChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectHitboxChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectIgdcChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectIgdcChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectLivecodingChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectLivecodingChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectPeka2ChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectPeka2Chat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectStreamcubeChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectStreamcubeChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectTwitchChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectTwitchChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectVidiChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectVidiChat() ) );
    QObject::connect( rootItem, SIGNAL( reconnectYoutubeChatTriggered() ), &QCoreMediator::instance(), SIGNAL( reconnectYoutubeChat() ) );

    QObject::connect( rootItem, SIGNAL( exitTriggered() ), &app, SLOT( quit() ) );

    QCoreMediator::instance().onReconnect();

    return app.exec();
}
