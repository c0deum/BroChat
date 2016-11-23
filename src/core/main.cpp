#include <QApplication>


#include "qbrochatview.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );

    QApplication::setOrganizationName( "c0deum" );
    QApplication::setApplicationName( "BroChat" );



    QBroChatView chatView;





    QObject::connect( &chatView, SIGNAL( closeWindow() ), &app, SLOT( quit() ) );

    chatView.show();    

    return app.exec();
}
