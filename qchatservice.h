#ifndef QCHATSERVICE_H
#define QCHATSERVICE_H

#include <QObject>
#include <QStringList>

#include <QJsonObject>

#include <QMap>

#include "qchatmessage.h"
#include "qchatstatistic.h"

class QChatService: public QObject
{
    Q_OBJECT
public:
    explicit                    QChatService( QObject * parent = nullptr );
    virtual                     ~QChatService();

    bool                        isEnabled() const;

    bool                        isShowSystemMessages() const;
    bool                        isAliasesSelection() const;
    bool                        isRemoveBlackListUsers() const;

    bool                        isContainsAliases( const QString & message ) const;

    const QStringList &         aliasesList() const;
    const QStringList &         supportersList() const;
    const QStringList &         blackList() const;
protected:
    void                        startUniqueTimer( int & id, int interval );
    void                        resetTimer( int & id );

    virtual void                loadSmiles();

    void                        addSmile( const QString & code, const QString & link );

    virtual QString             insertSmiles( const QString & message ) const;

    void                        emitJsonMessage( std::initializer_list< QPair< QString, QString> > fields );
    void                        emitSystemMessage( const QString & service, const QString & nickname, const QString & message );

    void                        emitMessage( const QString & service, const QString & nickname, const QString & text, std::initializer_list< QPair< QString, QString> > fields );




public slots:
    virtual void                connect() = 0;
    virtual void                disconnect() = 0;
    virtual void                reconnect() = 0;

    void                        enable( bool enabled );

    void                        setShowSystemMessages( bool showSystemMessages );
    void                        setAliasesSelection( bool aliasesSelection );
    void                        setRemoveBlackListUsers( bool removeBlackListUsers );

    void                        setAliasesList( const QString & aliasesList );
    void                        setSupportersList( const QString & supportersList );
    void                        setBlackList( const QString & blackList );

signals:
    void                        newMessage( ChatMessage );
    void                        newStatistic( QChatStatistic * statistic );
    void                        message( QJsonObject json, const QChatService * service );
public:
    static const QString        TYPE_FIELD;

    static const QString        SERVICE_NAME_FIELD;

    static const QString        NICKNAME_FIELD;
    static const QString        MESSAGE_TEXT_FIELD;
    static const QString        MESSAGE_TYPE_FIELD;
    static const QString        NICKNAME_COLOR_FIELD;
    static const QString        MESSAGE_ICON_SUBSCRIBER_FIELD;
    static const QString        MESSAGE_ICON_MODERATOR_FIELD;
    static const QString        MESSAGE_ICON_TWITCH_TURBO_FIELD;

    static const QString        STATISTIC_VALUE_FIELD;

    static const QString        MESSAGE_TYPE;
    static const QString        STATISTIC_TYPE;

    static const QString        MESSAGE_TYPE_EMPTY;
    static const QString        MESSAGE_TYPE_ALIAS;
    static const QString        MESSAGE_TYPE_SUPPORTER;
    static const QString        MESSAGE_TYPE_IGNORE;

private:
    bool                        enabled_;
    bool                        showSystemMessages_;
    bool                        aliasesSelection_;
    bool                        removeBlackListUsers_;
    QStringList                 aliasesList_;
    QStringList                 supportersList_;
    QStringList                 blackList_;

    QMap< QString, QString >    smiles_;
};

#endif // QCHATSERVICE_H
