#ifndef QBROCHATWEBPAGE_H
#define QBROCHATWEBPAGE_H

#include <QWebEnginePage>

class QBroChatWebPage: public QWebEnginePage
{
    Q_OBJECT
public:
    QBroChatWebPage( QObject * parent = nullptr );
protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
};

#endif // QBROCHATWEBPAGE_H
