#ifndef QPOLLVIEW
#define QPOLLVIEW

#include <QWebEngineView>

class QPollView: public QWebEngineView
{
    Q_OBJECT
public:
    explicit    QPollView( QWidget * parent = nullptr );
    virtual     ~QPollView();
protected:
    //virtual void timerEvent( QTimerEvent * event );
private:
};

#endif // QPOLLVIEW

