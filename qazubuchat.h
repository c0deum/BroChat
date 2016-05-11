#ifndef QAZUBUCHAT_H
#define QAZUBUCHAT_H

#include "qchatservice.h"

class QAzubuChat: public QChatService
{
    Q_OBJECT
public:
    explicit QAzubuChat( QObject * parent = nullptr );
    virtual ~QAzubuChat();
public slots:
    virtual void connect();
    virtual void disconnect();
    virtual void reconnect();
};

#endif // QAZUBUCHAT_H
