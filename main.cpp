#include <QApplication>
#include <QWebSettings>

#include "qbrochatview.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    QApplication::setOrganizationName( "c0deum" );
    QApplication::setApplicationName( "BroChat" );

    QWebSettings::globalSettings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
    QWebSettings::globalSettings()->setAttribute( QWebSettings::Accelerated2dCanvasEnabled, true );
    QWebSettings::globalSettings()->setAttribute( QWebSettings::WebGLEnabled, true );
    QWebSettings::globalSettings()->setAttribute( QWebSettings::PluginsEnabled, true );

    QBroChatView chatView;

    QObject::connect( &chatView, SIGNAL( closeWindow() ), &app, SLOT( quit() ) );

    chatView.show();

    int retVal = app.exec();

    QWebSettings::clearMemoryCaches();
    chatView.settings()->clearMemoryCaches();

    return retVal;
}
