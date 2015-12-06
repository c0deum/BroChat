#ifndef QMESSAGESMANAGER
#define QMESSAGESMANAGER

#include "qchatmessage.h"
#include <QList>

#include <QObject>

class QMessagesManager: public QObject
{
    Q_OBJECT
public:
    explicit QMessagesManager( QObject * parent = 0 );
    virtual ~QMessagesManager();
public:
    const QList< QChatMessage > & messages() const;
    bool save( const QString & logDir ) const;
private:
    void add( const QChatMessage & message );
public slots:
    void addMessage( QChatMessage * message );
private:
    QList< QChatMessage > messages_;
};


#endif // QMESSAGESMANAGER

