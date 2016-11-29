#ifndef QCOREMEDIATOR_H
#define QCOREMEDIATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QList>

#include "qmessagesmodel.h"

class QChatService;

class QCoreMediator: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMessagesModel * messagesModel READ messagesModel NOTIFY messagesModelChanged )
protected:
    explicit QCoreMediator( QObject * parent = nullptr );    
public:
    static QCoreMediator & instance();
    static void release();
    void registerChatService( QSharedPointer< QChatService > service );
    Q_INVOKABLE QMessagesModel * messagesModel();
public slots:
    Q_INVOKABLE void showSettings();
    void onReconnect();
signals:
    void reconnect();
    void messagesModelChanged( QMessagesModel * model );

private:
    static QCoreMediator * self_;
    QList< QSharedPointer< QChatService > > chatServices_;
    QMessagesModel model_;
};

template< typename T >
class ChatServiceRegistrator
{
public:
    ChatServiceRegistrator()
    {
        QCoreMediator::instance().registerChatService( QSharedPointer<T>( new T() ) );
    }
};



#endif // QCOREMEDIATOR_H
