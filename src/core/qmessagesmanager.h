#ifndef QMESSAGESMANAGER
#define QMESSAGESMANAGER

#include "qchatmessage.h"
#include <QList>

#include <QObject>

class QMessagesManager: public QObject
{
    Q_OBJECT
public:
    explicit                        QMessagesManager( QObject * parent = nullptr );
    virtual                         ~QMessagesManager();
public:
    const QList< ChatMessage > &    messages() const;
    bool                            save( const QString & logDir ) const;
public slots:
    void                            add( ChatMessage message );
private:
    QList< ChatMessage >            messages_;
};


#endif // QMESSAGESMANAGER

