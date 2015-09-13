#include <QApplication>
#include <QWebSettings>
//#include "qbrochatmainwindow.h"

#include "qbrochatview.h"

//#include "qwebenginechatview.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    //настройка QSettings
    QApplication::setOrganizationName( "c0deum" );
    QApplication::setApplicationName( "BroChat" );

    //QWebSettings::AcceleratedCompositingEnabled
    /*
    LocalContentCanAccessRemoteUrls,
    DnsPrefetchEnabled,
    XSSAuditingEnabled,
    AcceleratedCompositingEnabled,
    SpatialNavigationEnabled,
    LocalContentCanAccessFileUrls,
    TiledBackingStoreEnabled,
    FrameFlatteningEnabled,
    SiteSpecificQuirksEnabled,
    JavascriptCanCloseWindows,
    WebGLEnabled,
    CSSRegionsEnabled,
    HyperlinkAuditingEnabled,
    CSSGridLayoutEnabled,
    ScrollAnimatorEnabled,
    CaretBrowsingEnabled,
    NotificationsEnabled,
    WebAudioEnabled,
    Accelerated2dCanvasEnabled
    */

    //test websettings flags
    QWebSettings::globalSettings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
    QWebSettings::globalSettings()->setAttribute( QWebSettings::Accelerated2dCanvasEnabled, true );
    QWebSettings::globalSettings()->setAttribute( QWebSettings::WebGLEnabled, true );

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

    /*
    QBroChatMainWindow window;

    QObject::connect( &window, SIGNAL( closeWindow() ), &app, SLOT( quit() ) );

    window.show();
    */


    QBroChatView chatView;

    QObject::connect( &chatView, SIGNAL( closeWindow() ), &app, SLOT( quit() ) );

    chatView.show();


    /*
    QWebEngineChatView chatView;
    QObject::connect( &chatView, SIGNAL( closeWindow() ), &app, SLOT( quit() ) );
    chatView.show();
    */


    //попытка избавиться от ликов в Webkit
    int retVal = app.exec();

    QWebSettings::clearMemoryCaches();
    chatView.settings()->clearMemoryCaches();

    return retVal;
}
