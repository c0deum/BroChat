#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include <QtQml>

//#include <QQmlComponent>


//#include <QtQuickControls2>



//#include "qsettingsdialog.h"

#include "qcoremediator.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    QApplication::setOrganizationName( "c0deum" );
    QApplication::setApplicationName( "BroChat" );

    QQmlApplicationEngine engine;    


    engine.rootContext()->setContextProperty( "coreMediator", &QCoreMediator::instance() );

    QCoreMediator::instance().onReconnect();

    engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );



    //QCoreMediator::release();




    //QObject *rootItem = engine.rootObjects().first();

    //QObject::connect( rootItem, SIGNAL( showSettingsTriggered() ), &settingsDialog, SLOT(show() ) );

    //settingsDialog.show();






    return app.exec();
}
