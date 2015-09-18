#ifndef QCHATSERVICE_H
#define QCHATSERVICE_H

#include <QObject>
#include <QStringList>

#include "qchatmessage.h"
#include "qchatstatistic.h"

class QChatService: public QObject
{
    Q_OBJECT
public:
    explicit QChatService( QObject *parent = 0 );
    virtual ~QChatService();

    bool isShowSystemMessages() const;
    bool isAliasesSelection() const;
    bool isRemoveBlackListUsers() const;

    bool isContainsAliases( const QString &message ) const;

    const QStringList& aliasesList() const;
    const QStringList& supportersList() const;
    const QStringList& blackList() const;

public slots:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void reconnect() = 0;

    void setShowSystemMessages( bool showSystemMessages );
    void setAliasesSelection( bool aliasesSelection );
    void setRemoveBlackListUsers( bool removeBlackListUsers );

    void setAliasesList( const QString &aliasesList );
    void setSupportersList( const QString &supportersList );
    void setBlackList( const QString &blackList );


signals:
    void newMessage( QChatMessage *message );
    void newStatistic( QChatStatistic *statistic );
private:
    bool showSystemMessages_;
    bool aliasesSelection_;
    bool removeBlackListUsers_;    
    QStringList aliasesList_;
    QStringList supportersList_;
    QStringList blackList_;
};

#endif // QCHATSERVICE_H
