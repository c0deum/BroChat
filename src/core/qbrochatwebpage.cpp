#include <QDesktopServices>

#include "qbrochatwebpage.h"

QBroChatWebPage::QBroChatWebPage( QObject * parent )
: QWebEnginePage( parent )
{
}


bool QBroChatWebPage::acceptNavigationRequest( const QUrl &url, NavigationType type, bool isMainFrame )
{
    Q_UNUSED( isMainFrame );
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        QDesktopServices::openUrl(url);
        return false;
    }
    return true;
}
