#ifndef QCHATMESSAGE_H
#define QCHATMESSAGE_H

#include <QObject>
#include <QString>

class QChatMessage: public QObject
{
    Q_OBJECT
public:
    explicit QChatMessage( const QString &service, const QString &nickName, const QString &message, const QString &type, QObject *parent = 0 );
    virtual ~QChatMessage();
public:
    QString service() const;
    void setService( const QString &service );

    QString nickName() const;
    void setNickName( const QString &nickName );

    QString message() const;
    void setMessage( const QString &message );

    QString type() const;
    void setType( const QString &type );

    static QString deleteTags( const QString& message );

    static QString replaceEscapeCharecters( const QString &message );

    static bool isLink( const QString& link );
    static QString insertLinks( const QString &message, bool convertImageLinks = false );
private:
    QString service_;
    QString nickName_;
    QString message_;
    QString type_;
};

#endif // QCHATMESSAGE_H
