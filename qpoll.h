#ifndef QPOLL
#define QPOLL

#include <QList>
#include <QMap>
#include <QPair>
#include <QSet>

#include<QObject>

class ChatMessage;

class QPoll: public QObject
{
    Q_OBJECT
public:
    struct PollEntry
    {
        QString answer;
        QSet< QString > users;
        QSet< QPair<QString,QString> > servicesUsers;
        int votes;
    };
public:
    explicit                            QPoll( QObject * parent = nullptr );
    virtual                             ~QPoll();
public:
    const QString &                     question() const;
    void                                question( const QString & newQuestion );

    const QMap< QString, PollEntry > &  entries() const;
    void                                entries( const QMap< QString, PollEntry > & newEntries );

    int                                 time() const;
    void                                time( int newTime );

    bool                                isMultiChoice() const;
    void                                multiChoice( bool flag );

protected:
    virtual void                        timerEvent( QTimerEvent * event );

private:
    void                                init();
    void                                loadSettings();
    void                                saveSettings();

public slots:
    void                                start();
    void                                stop();
    void                                finish();
    void                                onNewMessage( const ChatMessage * message );
signals:
    void                                started();
    void                                finished();
private:
    QString                             question_;
    QMap< QString, PollEntry >          entries_;
    int                                 time_;
    int                                 elapsedTime_;
    qint64                              startTime_;
    int                                 updateTimerId_;
    int                                 updateTimerInterval_;
    bool                                multiChoice_;
};

#endif // QPOLL

