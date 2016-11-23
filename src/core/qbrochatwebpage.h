#ifndef QBROCHATWEBPAGE_H
#define QBROCHATWEBPAGE_H

#include <QWebEnginePage>

class QBroChatWebPage: public QWebEnginePage
{
    Q_OBJECT
public:
    QBroChatWebPage( QObject * parent = nullptr );
protected:
    virtual bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // QBROCHATWEBPAGE_H
