#ifndef QPOLLVIEW
#define QPOLLVIEW

#include <QWebEngineView>

class QPollView: public QWebEngineView
{
    Q_OBJECT
public:
    explicit    QPollView( QWidget * parent = nullptr );
                ~QPollView() override;
protected:
    //virtual void timerEvent( QTimerEvent * event );
private:
};

#endif // QPOLLVIEW

